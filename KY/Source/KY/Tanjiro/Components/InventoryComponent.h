// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
	// 각 슬롯은 ID와 Count를 갖고, 표시는 DB에서 끌고오기
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Id = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;

	bool IsEmpty() const { return Id.IsNone() || Count <= 0; }
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UInventoryComponent();
protected:
	virtual void BeginPlay() override;
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:	
	// 슬롯 수 & 기본 스택 최대치
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 NumSlots  = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxStackDefault  = 30;

	// UI가 바인딩할 데이터(복제됨)
	UPROPERTY(BlueprintReadOnly)
	TArray<FInventorySlot> Items;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnInventoryChanged;

	UFUNCTION(BlueprintCallable) void InitSlots(int32 InNumSlots);
	UFUNCTION(BlueprintCallable) int32 AddById(FName Id, int32 Count);   // 남은 개수 반환
	UFUNCTION(BlueprintCallable) int32 RemoveById(FName Id, int32 Count); // 제거된 개수 반환
	UFUNCTION(BlueprintCallable) bool  MoveOrSwap(int32 FromIdx, int32 ToIdx);
	/*
	UFUNCTION(BlueprintCallable)
	int32 AddItem(FName Id, int32 Count); // 반환: 남은 개수(못 넣은 수)

	UFUNCTION(BlueprintCallable)
	int32 RemoveItem(FName Id, int32 Count); // 반환: 실제 제거된 수

	UFUNCTION(BlueprintCallable) bool MoveOrSwap(int32 From, int32 To);

	UFUNCTION(BlueprintPure) const TArray<FInventorySlot>& GetAll() const { return Items; }
*/
protected:
	//int32 MaxStackFor(FName Id) const; // 필요시 DB에서 per-item로 확장
	//int32 FirstFreeSlot() const;
	//TOptional<int32> FindStackIndex(FName Id, bool bFindNonFull) const;
	void BroadcastChanged();
	// 내부 유틸
	int32 MaxStackFor(FName Id) const { return MaxStackDefault; }
	int32 FirstFreeSlot() const;
	TOptional<int32> FindStackIndex(FName Id, bool bFindNonFull) const;

};
