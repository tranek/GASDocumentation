// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GASDocumentation.h"
#include "GDGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GASDOCUMENTATION_API UGDGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGDGameplayAbility();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EGDAbilityInputID AbilityInputID = EGDAbilityInputID::None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EGDAbilityInputID AbilityIDForLevel = EGDAbilityInputID::None;
};
