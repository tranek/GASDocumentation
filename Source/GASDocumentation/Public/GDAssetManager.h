// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "GDAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class GASDOCUMENTATION_API UGDAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:

	static UGDAssetManager& Get();

	/** Starts initial load, gets called from InitializeObjectReferences */
	virtual void StartInitialLoading() override;
};
