// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GDAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class GASDOCUMENTATION_API UGDAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	bool CharacterAbilitiesGiven = false;
	bool GameplayEffectsGivenOnFirstSpawnGiven = false;
	bool GameplayEffectsActiveWhileAliveGiven = false;
};
