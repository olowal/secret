#include "SecCharacterMovementComponent.h"
#include "Character/SecCharacter.h"
#include "Attribute/SecAttributeComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Attribute/SecCharacterMovementSpeedMultiplierAttribute.h"
#include "Attribute/SecCharacterRotationSpeedMultiplierAttribute.h"

const FName USecCharacterMovementComponent::CharacterMovementSpeedMultiplierName(TEXT("CharacterMovementSpeedMultiplier"));

void USecCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	SecCharacterOwner = CastChecked<ASecCharacter>(GetOwner());
	Attribute = SecCharacterOwner->GetAttributeComponent();

	// Kind of a hack from the old system, let's re-think this and use attributes in a different way here.
	MovementSpeedMultipler = Attribute->AddAttribute<USecCharacterMovementSpeedMultiplierAttribute>();
	RotationSpeedMultiplier = Attribute->AddAttribute<USecCharacterRotationSpeedMultiplierAttribute>();

	MovementSpeedMultipler->Setup();
	RotationSpeedMultiplier->Setup();
}
void USecCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentFacingDirection = FQuat::Slerp(CurrentFacingDirection, TargetFacingDirection, GetMaxRotationSpeed() * DeltaTime);
}

void USecCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (GetCharacterOwner() && GetCharacterOwner()->IsLocallyControlled())
	{
		MoveUpdatedComponent(FVector::ZeroVector, CurrentFacingDirection, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

float USecCharacterMovementComponent::GetMaxSpeed() const
{
#if !UE_BUILD_SHIPPING
	if (!ensure(MovementSpeedMultipler != nullptr))
		return Super::GetMaxSpeed();
#endif // !UE_BUILD_SHIPPING
	return Super::GetMaxSpeed() * MovementSpeedMultipler->GetAttributeValue();
}

float USecCharacterMovementComponent::GetMaxRotationSpeed() const
{
#if !UE_BUILD_SHIPPING
	if (!ensure(RotationSpeedMultiplier != nullptr))
		return 1.0f;
#endif // !UE_BUILD_SHIPPING
	return RotationSpeed * RotationSpeedMultiplier->GetAttributeValue();
}

bool USecCharacterMovementComponent::IsMovingOnGround() const
{
	if (bKnockback && CurrentFloor.IsWalkableFloor())
	{
		return true;
	}
	
	return Super::IsMovingOnGround();
}

void USecCharacterMovementComponent::SetTargetFacingRotation(const FQuat& NewTargetFacingRotation)
{
	TargetFacingDirection = NewTargetFacingRotation;
}

void USecCharacterMovementComponent::SetTargetFacingDirection(const FVector& NewTargetFacingDirection)
{
	TargetFacingDirection = NewTargetFacingDirection.ToOrientationQuat();
}

void USecCharacterMovementComponent::ApplyKnockback(const FVector& InKnockbackImpulse)
{	
	if (bKnockback)
		return;

	Internal_ApplyKnockback(InKnockbackImpulse);
	Server_ApplyKnockback(InKnockbackImpulse);
}

void USecCharacterMovementComponent::Multicast_ApplyKnockback_Implementation(const FVector& NewKnockbackImpulse)
{
	if (!bKnockback)
	{
		Internal_ApplyKnockback(NewKnockbackImpulse);
	}
}

void USecCharacterMovementComponent::Server_ApplyKnockback_Implementation(const FVector& NewKnockbackImpulse)
{
	Multicast_ApplyKnockback(NewKnockbackImpulse);
}

void USecCharacterMovementComponent::Internal_ApplyKnockback(const FVector& InKnockbackImpulse)
{
	Velocity = InKnockbackImpulse;
	bKnockback = true;
	SetMovementMode(MOVE_Custom);
}

void USecCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	Super::PhysCustom(DeltaTime, Iterations);
	Velocity *= FMath::Pow(KnockbackFriction, DeltaTime);
	const float KnockbackSizeSq = Velocity.SizeSquared();
	if (KnockbackSizeSq < FMath::Square(KnockbackStopSize))
	{
		bKnockback = false;
		SetMovementMode(MOVE_Walking);
		return;
	}
	
	FHitResult Hit;
	const FVector KnockbackDelta = Velocity * DeltaTime;
	SafeMoveUpdatedComponent(KnockbackDelta, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (Hit.bBlockingHit)
	{
		SlideAlongSurface(KnockbackDelta, 1.0f - Hit.Time, Hit.ImpactNormal, Hit, true);
	}
}

void USecCharacterMovementComponent::ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration)
{
	if (!bKnockback)
	{
		Super::ApplyVelocityBraking(DeltaTime, Friction, BrakingDeceleration);
	}
}

void USecCharacterMovementComponent::ReplicateMoveToServer(float DeltaTime, const FVector& NewAcceleration)
{
	check(CharacterOwner != NULL);

	// Can only start sending moves if our controllers are synced up over the network, otherwise we flood the reliable buffer.
	APlayerController * PC = Cast<APlayerController>(CharacterOwner->GetController());
	if (PC && PC->AcknowledgedPawn != CharacterOwner)
	{
		return;
	}

	// Bail out if our character's controller doesn't have a Player. This may be the case when the local player
	// has switched to another controller, such as a debug camera controller.
	if (PC && PC->Player == nullptr)
	{
		return;
	}

	FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
	if (!ClientData)
	{
		return;
	}

	// Update our delta time for physics simulation.
	DeltaTime = ClientData->UpdateTimeStampAndDeltaTime(DeltaTime, *CharacterOwner, *this);

	// Find the oldest (unacknowledged) important move (OldMove).
	// Don't include the last move because it may be combined with the next new move.
	// A saved move is interesting if it differs significantly from the last acknowledged move
	FSavedMovePtr OldMove = NULL;
	if (ClientData->LastAckedMove.IsValid())
	{
		const int32 NumSavedMoves = ClientData->SavedMoves.Num();
		for (int32 i = 0; i < NumSavedMoves - 1; i++)
		{
			const FSavedMovePtr& CurrentMove = ClientData->SavedMoves[i];
			if (CurrentMove->IsImportantMove(ClientData->LastAckedMove))
			{
				OldMove = CurrentMove;
				break;
			}
		}
	}

	// Get a SavedMove object to store the movement in.
	FSavedMovePtr NewMovePtr = ClientData->CreateSavedMove();
	FSavedMove_Character* const NewMove = NewMovePtr.Get();
	if (NewMove == nullptr)
	{
		return;
	}
	
	NewMove->SetMoveFor(CharacterOwner, DeltaTime, NewAcceleration, *ClientData);
	const UWorld* MyWorld = GetWorld();

	// see if the two moves could be combined
	// do not combine moves which have different TimeStamps (before and after reset).
	if (const FSavedMove_Character * PendingMove = ClientData->PendingMove.Get())
	{
		if (!PendingMove->bOldTimeStampBeforeReset && PendingMove->CanCombineWith(NewMovePtr, CharacterOwner, ClientData->MaxMoveDeltaTime * CharacterOwner->GetActorTimeDilation(*MyWorld)))
		{

			// Only combine and move back to the start location if we don't move back in to a spot that would make us collide with something new.
			const FVector OldStartLocation = PendingMove->GetRevertedLocation();
			if (!OverlapTest(OldStartLocation, PendingMove->StartRotation.Quaternion(), UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CharacterOwner))
			{
				// Avoid updating Mesh bones to physics during the teleport back, since PerformMovement() will update it right away anyway below.
				// Note: this must be before the FScopedMovementUpdate below, since that scope is what actually moves the character and mesh.

				// Accumulate multiple transform updates until scope ends.
				FScopedMovementUpdate ScopedMovementUpdate(UpdatedComponent, EScopedUpdate::DeferredUpdates);

				NewMove->CombineWith(PendingMove, CharacterOwner, PC, OldStartLocation);

				if (PC)
				{
					// We reverted position to that at the start of the pending move (above), however some code paths expect rotation to be set correctly
					// before character movement occurs (via FaceRotation), so try that now. The bOrientRotationToMovement path happens later as part of PerformMovement() and PhysicsRotation().
					CharacterOwner->FaceRotation(CurrentFacingDirection.Rotator(), NewMove->DeltaTime);
				}

				SaveBaseLocation();
				NewMove->SetInitialPosition(CharacterOwner);

				// Remove pending move from move list. It would have to be the last move on the list.
				if (ClientData->SavedMoves.Num() > 0 && ClientData->SavedMoves.Last() == ClientData->PendingMove)
				{
					const bool bAllowShrinking = false;
					ClientData->SavedMoves.Pop(bAllowShrinking);
				}
				ClientData->FreeMove(ClientData->PendingMove);
				ClientData->PendingMove = nullptr;
				PendingMove = nullptr; // Avoid dangling reference, it's deleted above.
			}
			else
			{
			
			}
		}
		else
		{
			
		}
	}

	// Acceleration should match what we send to the server, plus any other restrictions the server also enforces (see MoveAutonomous).
	Acceleration = NewMove->Acceleration.GetClampedToMaxSize(GetMaxAcceleration());
	AnalogInputModifier = ComputeAnalogInputModifier(); // recompute since acceleration may have changed.

	// Perform the move locally
	CharacterOwner->ClientRootMotionParams.Clear();
	CharacterOwner->SavedRootMotion.Clear();
	PerformMovement(NewMove->DeltaTime);

	NewMove->PostUpdate(CharacterOwner, FSavedMove_Character::PostUpdate_Record);
	NewMove->SavedControlRotation = TargetFacingDirection.Rotator(); //CurrentFacingDirection.Rotator();
	NewMove->StartBaseRotation = CurrentFacingDirection;
	// Add NewMove to the list
	if (CharacterOwner->IsReplicatingMovement())
	{
		check(NewMove == NewMovePtr.Get());
		ClientData->SavedMoves.Push(NewMovePtr);

		const bool bCanDelayMove = false;

		if (bCanDelayMove && ClientData->PendingMove.IsValid() == false)
		{
			// Decide whether to hold off on move
			const float NetMoveDelta = FMath::Clamp(GetClientNetSendDeltaTime(PC, ClientData, NewMovePtr), 1.f / 120.f, 1.f / 5.f);

			if ((MyWorld->TimeSeconds - ClientData->ClientUpdateTime) * MyWorld->GetWorldSettings()->GetEffectiveTimeDilation() < NetMoveDelta)
			{
				// Delay sending this move.
				ClientData->PendingMove = NewMovePtr;
				return;
			}
		}

		ClientData->ClientUpdateTime = MyWorld->TimeSeconds;


		bool bSendServerMove = true;

		// Send move to server if this character is replicating movement
		if (bSendServerMove)
		{
			CallServerMove(NewMove, OldMove.Get());
		}
	}

	ClientData->PendingMove = NULL;
}
