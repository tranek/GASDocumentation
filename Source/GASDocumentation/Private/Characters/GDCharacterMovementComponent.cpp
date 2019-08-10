// Fill out your copyright notice in the Description page of Project Settings.


#include "GDCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GDCharacterBase.h"

float UGDCharacterMovementComponent::GetMaxSpeed() const
{
	AGDCharacterBase* Owner = Cast<AGDCharacterBase>(GetOwner());
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() No Owner"), TEXT(__FUNCTION__));
		return Super::GetMaxSpeed();
	}

	if (!Owner->IsAlive())
	{
		return 0.0f;
	}

	if (Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.AimDownSights"))) &&
		!Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.AimDownSights.Removal"))))
	{
		float ADSSpeed = Owner->GetMoveSpeedBaseValue() * 0.7f;
		return ADSSpeed < Owner->GetMoveSpeed() ? ADSSpeed : Owner->GetMoveSpeed();
	}

	return Owner->GetMoveSpeed();
}
