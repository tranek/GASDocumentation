// Copyright 2020 Dan Kestranek.


#include "Characters/Abilities/AbilityTasks/GDAT_WaitReceiveDamage.h"
#include "Characters/Abilities/GDAbilitySystemComponent.h"

UGDAT_WaitReceiveDamage::UGDAT_WaitReceiveDamage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TriggerOnce = false;
}

UGDAT_WaitReceiveDamage* UGDAT_WaitReceiveDamage::WaitReceiveDamage(UGameplayAbility* OwningAbility, bool InTriggerOnce)
{
	UGDAT_WaitReceiveDamage* MyObj = NewAbilityTask<UGDAT_WaitReceiveDamage>(OwningAbility);
	MyObj->TriggerOnce = InTriggerOnce;
	return MyObj;
}

void UGDAT_WaitReceiveDamage::Activate()
{
	UGDAbilitySystemComponent* GDASC = Cast<UGDAbilitySystemComponent>(AbilitySystemComponent);

	if (GDASC)
	{
		GDASC->ReceivedDamage.AddDynamic(this, &UGDAT_WaitReceiveDamage::OnDamageReceived);
	}
}

void UGDAT_WaitReceiveDamage::OnDestroy(bool AbilityIsEnding)
{
	UGDAbilitySystemComponent* GDASC = Cast<UGDAbilitySystemComponent>(AbilitySystemComponent);

	if (GDASC)
	{
		GDASC->ReceivedDamage.RemoveDynamic(this, &UGDAT_WaitReceiveDamage::OnDamageReceived);
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UGDAT_WaitReceiveDamage::OnDamageReceived(UGDAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
	}

	if (TriggerOnce)
	{
		EndTask();
	}
}
