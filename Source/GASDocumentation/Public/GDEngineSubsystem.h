// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "GDEngineSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class GASDOCUMENTATION_API UGDEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
