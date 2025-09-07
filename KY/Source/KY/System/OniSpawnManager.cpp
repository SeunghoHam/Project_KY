// Fill out your copyright notice in the Description page of Project Settings.


#include "System/OniSpawnManager.h"
#include "Enemy/Oni.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AOniSpawnManager::AOniSpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AOniSpawnManager::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHanlde, this, &AOniSpawnManager::SpawnOni, 2.0f, false);
}

void AOniSpawnManager::SpawnOni()
{
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AOni* oni = GetWorld()->SpawnActor<AOni>(DefaultOni,GetActorLocation(), GetActorRotation(),param );
	oni->SpawnDefaultController();
}

