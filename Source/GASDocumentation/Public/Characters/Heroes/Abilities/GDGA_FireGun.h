// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Abilities/GDGameplayAbility.h"
#include "GDGA_FireGun.generated.h"

/**
 * 
 */
UCLASS()
class GASDOCUMENTATION_API UGDGA_FireGun : public UGDGameplayAbility
{
	GENERATED_BODY()
	
public:
public:
	UGDGA_FireGun();

	/** Actually activate ability, do not call this directly. We'll call it from APAHeroCharacter::ActivateAbilitiesWithTags(). */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
