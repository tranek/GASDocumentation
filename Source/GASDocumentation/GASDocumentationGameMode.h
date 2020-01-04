// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GASDocumentationGameMode.generated.h"

UCLASS(minimalapi)
class AGASDocumentationGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGASDocumentationGameMode();

	void HeroDied(AController* Controller);

protected:
	float RespawnDelay;

	TSubclassOf<class AGDHeroCharacter> HeroClass;

	AActor* EnemySpawnPoint;

	virtual void BeginPlay() override;

	void RespawnHero(AController* Controller);
};
