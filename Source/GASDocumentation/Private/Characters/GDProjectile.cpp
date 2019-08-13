// Fill out your copyright notice in the Description page of Project Settings.


#include "GDProjectile.h"

// Sets default values
AGDProjectile::AGDProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

// Called when the game starts or when spawned
void AGDProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

