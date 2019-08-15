// Fill out your copyright notice in the Description page of Project Settings.

#include "GDAT_WaitReceiveDamage.h"
#include "GDAbilitySystemComponent.h"

UGDAT_WaitReceiveDamage::UGDAT_WaitReceiveDamage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UGDAT_WaitReceiveDamage* UGDAT_WaitReceiveDamage::WaitReceiveDamage(UGameplayAbility* OwningAbility)
{
	UGDAT_WaitReceiveDamage* MyObj = NewAbilityTask<UGDAT_WaitReceiveDamage>(OwningAbility);

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
	EndTask();
}
