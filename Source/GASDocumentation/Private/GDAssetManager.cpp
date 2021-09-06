// Copyright 2020 Dan Kestranek.


#include "GDAssetManager.h"
#include "AbilitySystemGlobals.h"


UGDAssetManager& UGDAssetManager::Get() 
{
	UGDAssetManager* Singleton = Cast<UGDAssetManager>(GEngine->AssetManager);

	if (Singleton)
	{
		return *Singleton;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be GDAssetManager!"));
		return *NewObject<UGDAssetManager>();	 // never calls this
	}
}


void UGDAssetManager::StartInitialLoading() 
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();
}
