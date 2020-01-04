// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GDDamageExecCalculation.generated.h"

/**
 * 
 */
UCLASS()
class GASDOCUMENTATION_API UGDDamageExecCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UGDDamageExecCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
