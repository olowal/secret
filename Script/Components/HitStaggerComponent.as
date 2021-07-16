
class UHitStaggerComponent : UActorComponent
{
	UPROPERTY()
	UAnimMontage HitStaggerAnim;

	UPROPERTY()
	float HitStaggerMultiplier = 1.0f;

	bool bHitStagger = false;

	float GetHitStaggerTime() const property
	{
		if(!ensure(HitStaggerAnim != nullptr))
			return 1.0f;

		if(!ensure(!FMath::IsNearlyZero(HitStaggerMultiplier)))
			return 1.0f;

		const float StaggerTime = HitStaggerAnim.SequenceLength / HitStaggerMultiplier;
		return StaggerTime;
	}
}
