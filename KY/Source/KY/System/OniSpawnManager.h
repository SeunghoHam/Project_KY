// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OniSpawnManager.generated.h"

class AOni;
UCLASS()
class KY_API AOniSpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AOniSpawnManager();
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=Setting)
	TSubclassOf<AOni> DefaultOni;
	
	void SpawnOni();

private:
	FTimerHandle SpawnTimerHanlde;
};
