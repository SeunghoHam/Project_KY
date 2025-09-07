// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TanjiroAnimMontageMap.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class KY_API UTanjiroAnimMontageMap : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TMap<FName, TSoftObjectPtr<UAnimMontage>> MontageMap;
};
