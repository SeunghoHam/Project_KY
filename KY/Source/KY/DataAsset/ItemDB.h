// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "ItemDB.generated.h"

/**
 * 
 */
class USphereComponent;
class UPrimitiveComponent;

USTRUCT(BlueprintType)
struct FItemMeta
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	FText DisplayName;

	// 하드 레퍼런스(간단/자동 쿠킹). 대규모면 TSoftObjectPtr로 교체 가능
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	TObjectPtr<UTexture2D> Icon = nullptr;
};

UCLASS(BlueprintType)
class KY_API UItemDB : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// Key: 아이템 ID (ex. Potion_S), Value: 메타(이름/아이콘)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	TMap<FName, FItemMeta> Items;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Item)
	//TMap<FName, TObjectPtr<UTexture2D>> Items;
	
	UFUNCTION(BlueprintPure)
	bool TryGet(FName Id, FItemMeta& Out) const
	{
		if (const FItemMeta* Found = Items.Find(Id)) { Out = *Found; return true; }
		return false;
	}
/*
	UFUNCTION(BlueprintPure)
	UTexture2D* GetIcon(FName Id) const
	{
		if (const FItemMeta* Found = Items.Find(Id)) return Found->Icon;
		return nullptr;
	}

	UFUNCTION(BlueprintPure)
	FText GetDisplayName(FName Id) const
	{
		if (const FItemMeta* Found = Items.Find(Id)) return Found->DisplayName;
		return FText::GetEmpty();
	}
	*/
};
