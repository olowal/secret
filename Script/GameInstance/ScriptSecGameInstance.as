import AI.Perception.SecSensingComponent;

void RegisterSensingComponent(USecSensingComponent Comp)
{
	UScriptGameInstance GameInstance = Cast<UScriptGameInstance>(Gameplay::GameInstance);

	if(GameInstance == nullptr)
		return;

	GameInstance.SensingCollection.AddUnique(Comp);
}

void UnregisterSensingComponent(USecSensingComponent Comp)
{
	UScriptGameInstance GameInstance = Cast<UScriptGameInstance>(Gameplay::GameInstance);

	if(GameInstance == nullptr)
		return;

	GameInstance.SensingCollection.Remove(Comp);
}

void GetSensingComponents(TArray<USecSensingComponent>& OutSensingComponents)
{
	UScriptGameInstance GameInstance = Cast<UScriptGameInstance>(Gameplay::GameInstance);

	if(GameInstance == nullptr)
		return;

	OutSensingComponents = GameInstance.SensingCollection;
}

class UScriptGameInstance : USecGameInstance
{
	TArray<USecSensingComponent> SensingCollection;
}
