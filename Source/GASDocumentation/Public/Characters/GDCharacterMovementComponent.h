// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GDCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class GASDOCUMENTATION_API UGDCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	virtual float GetMaxSpeed() const override;
};
