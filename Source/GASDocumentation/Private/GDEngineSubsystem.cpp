// Copyright 2019 Dan Kestranek.


#include "GDEngineSubsystem.h"
#include "AbilitySystemGlobals.h"

void UGDEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UAbilitySystemGlobals::Get().InitGlobalData();
}
