// Fill out your copyright notice in the Description page of Project Settings.


#include "GDAbilitySystemComponent.h"

void UGDAbilitySystemComponent::ReceiveDamage(UGDAbilitySystemComponent * SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}
