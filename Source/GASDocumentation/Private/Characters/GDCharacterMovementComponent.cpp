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

	if (Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"))))
	{
		return 0.0f;
	}

	if (Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.AimDownSights"))) &&
		!Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.AimDownSights.Removal"))))
	{
		float ADSSpeed = Owner->GetMoveSpeedBaseValue() * 0.6f;
		return ADSSpeed < Owner->GetMoveSpeed() ? ADSSpeed : Owner->GetMoveSpeed();
	}

	if (Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Sprint"))) &&
		!Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Sprint.Removal"))))
	{
		float SprintSpeed = Owner->GetMoveSpeedBaseValue() * 1.4f;
		return SprintSpeed > Owner->GetMoveSpeed() ? SprintSpeed : Owner->GetMoveSpeed();
	}

	return Owner->GetMoveSpeed();
}
