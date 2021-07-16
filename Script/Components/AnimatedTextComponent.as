
class UAnimatedTextData : UDataAsset
{
    UPROPERTY()
    TSubclassOf<UAnimatedTextComponent> TextComponentClass;
    UPROPERTY()
    float MovementSpeed = 100.0f;
    UPROPERTY()
    float Friction = 0.0f;
    UPROPERTY()
    float Gravity = 0.0f;
    UPROPERTY()
    float Lifetime = 3.0f;
    UPROPERTY()
    FVector WorldDirection = FVector::UpVector;
    UPROPERTY()
    bool bRandomizeDirection = false;
    UPROPERTY(meta = (EditCondition = "bRandomizeDirection"))
    float ConeHalfAngleInDegrees = 180.0f;
    UPROPERTY()
    bool bRotateTowardsDirection = false;
    UPROPERTY(meta = (EditCondition = "bRotateTowardsDirection"))
    FVector UpVector = FVector::UpVector;
    UPROPERTY()
    FLinearColor Color;
    UPROPERTY()
    bool bRandomizeColor = false;
}

namespace Gameplay
{
    void CreateAnimatedText(const FVector& Location, const FString& Text, TSubclassOf<UAnimatedTextComponent> TextClass, AActor Target = nullptr)
    {
        if(Target == nullptr)
        {
           Target = Gameplay::GetGameState();
        }

        if(Target == nullptr)
        {
            return;
        }

        UAnimatedTextComponent AnimatedTextComp =  Cast<UAnimatedTextComponent>(Target.CreateComponent(TextClass));
        AnimatedTextComp.SetWorldLocation(Location + FVector(0.0f, 0.0f, 100.0f));
        AnimatedTextComp.SetText(FText::FromString(Text));
    }

    void CreateAnimatedText(const FVector& Location, const FString& Text, UAnimatedTextData AnimatedTextData, AActor Target = nullptr)
    {
        if(Target == nullptr)
        {
           Target = Gameplay::GetGameState();
        }

        if(Target == nullptr)
        {
            return;
        }

        UAnimatedTextComponent AnimatedTextComp = Cast<UAnimatedTextComponent>(Target.CreateComponent(AnimatedTextData.TextComponentClass));
        AnimatedTextComp.SetWorldLocation(Location + FVector(0.0f, 0.0f, 100.0f));
        AnimatedTextComp.SetNewAnimatedTextData(AnimatedTextData);
        AnimatedTextComp.SetText(FText::FromString(Text));
    }
}

UCLASS(abstract)
class UAnimatedTextComponent : UTextRenderComponent
{
    default SetMaterial(0, Asset("/Game/Assets/Materials/Font/MI_TextRender_World.MI_TextRender_World"));
    default HorizontalAlignment = EHorizTextAligment::EHTA_Center;
    default PrimaryComponentTick.bStartWithTickEnabled = true;
    default bAbsoluteLocation = true;
    default bAbsoluteRotation = true;

    UPROPERTY()
    private UAnimatedTextData AnimatedTextData;

    UPROPERTY()
    float MovementSpeed = 100.0f;

    UPROPERTY()
    float LifeTime = 1.0f;

    UPROPERTY()
    FVector Direction = FVector::UpVector;

    void SetNewAnimatedTextData(UAnimatedTextData NewAnimatedTextData)
    {
        AnimatedTextData = NewAnimatedTextData;
        MovementSpeed = AnimatedTextData.MovementSpeed;
        LifeTime = AnimatedTextData.Lifetime;

        if(NewAnimatedTextData.bRandomizeDirection)
        {
            Direction = FMath::VRandCone(AnimatedTextData.WorldDirection, FMath::DegreesToRadians(NewAnimatedTextData.ConeHalfAngleInDegrees));
        }
        else
        {
            Direction = AnimatedTextData.WorldDirection;
        }
        
        Direction.Normalize();
    }

    UFUNCTION(BlueprintOverride)
    void Tick(float DeltaTime)
    {
        if(AnimatedTextData != nullptr)
        {

            MovementSpeed -= MovementSpeed * (AnimatedTextData.Friction * DeltaTime);
            Direction -= (FVector::UpVector * (AnimatedTextData.Gravity * DeltaTime));

            if(AnimatedTextData.bRotateTowardsDirection)
            {
                const float Angle = FMath::RadiansToDegrees((FMath::Atan2(Direction.Y, Direction.Z)));
                SetWorldRotation(FRotator(0.0f, Angle, 0.0f));
            }
        }

        FHitResult Hit;
        AddWorldOffset(Direction * (MovementSpeed * DeltaTime), false, Hit, true);
        //AddWorldRotation(FRotator(0.0f, 1.5f, 0.0f));

        LifeTime -= DeltaTime;
        if(LifeTime <= 0.0f)
        {
            DestroyComponent(this);
        }
    }
}
