// Fill out your copyright notice in the Description page of Project Settings.


#include "Tanjiro/Components/InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//Items.SetNum(MaxSlots); // 빈 슬롯
	// ...
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Items.Num() == 0 && NumSlots > 0)
	{
		Items.SetNum(NumSlots);
		// BeginPlay 직후 UI가 이미 바인딩되어 있을 수 있으니 한 번 브로드캐스트
		BroadcastChanged();
	}
}

void UInventoryComponent::InitSlots(int32 InNumSlots)
{
	NumSlots = FMath::Max(0, InNumSlots);
	Items.SetNum(NumSlots);
	// 레이아웃이 바뀌었으니 UI가 전체를 다시 그리도록 알림
	BroadcastChanged();
}

int32 UInventoryComponent::AddById(FName Id, int32 Count)
{
	if (Id.IsNone() || Count <= 0) return Count;
	int32 Left = Count;
	const int32 MaxCount = MaxStackFor(Id);
	
	// 보유중인 아이템
	while (Left > 0)
	{
		auto Index = FindStackIndex(Id,true);
		if (!Index.IsSet()) break;
		int32 i = Index.GetValue();
		int32 Space = MaxCount - Items[i].Count;
		int32 Put = FMath::Min(Space, Left);
		Items[i].Count += Put;
		Left -= Put;
	}
	// 빈슬롯 채우기
	while (Left > 0)
	{
		int32 i = FirstFreeSlot();
		if (i == INDEX_NONE) break;
		int32 Put = FMath::Min(MaxCount, Left);
		Items[i] = { Id, Put };
		Left -= Put;
	}
	BroadcastChanged();
	return Left;
}

int32 UInventoryComponent::RemoveById(FName Id, int32 Count)
{
	if (Id.IsNone() || Count <= 0) return 0;
	int32 Removed = 0;

	for (int32 i=0; i<Items.Num() && Removed < Count; ++i)
	{
		auto& S = Items[i];
		if (S.Id != Id) continue;
		const int32 Take = FMath::Min(S.Count, Count - Removed);
		S.Count -= Take;
		Removed += Take;
		if (S.Count <= 0) S = {};
	}

	if (Removed > 0)
	{
		BroadcastChanged();
	}
	return Removed;
}

bool UInventoryComponent::MoveOrSwap(int32 From, int32 To)
{
	if (!Items.IsValidIndex(From) || !Items.IsValidIndex(To) || From == To) return false;

	auto& A = Items[From];
	auto& B = Items[To];

	// 같은 아이템이면 합치기
	if (!A.IsEmpty() && !B.IsEmpty() && A.Id == B.Id)
	{
		const int32 MaxStack = MaxStackFor(A.Id);
		const int32 Space = MaxStack - B.Count;
		const int32 Move = FMath::Min(Space, A.Count);
		B.Count += Move;
		A.Count -= Move;
		if (A.Count <= 0) A = {};
	}
	else
	{
		Swap(A, B);
	}

	if (GetOwnerRole() == ROLE_Authority) BroadcastChanged();
	return true;
}


int32 UInventoryComponent::FirstFreeSlot() const
{
	for (int32 i = 0; i < Items.Num(); ++i) if (Items[i].IsEmpty()) return i;
	return INDEX_NONE;
}

TOptional<int32> UInventoryComponent::FindStackIndex(FName Id, bool bFindNonFull) const
{
	const int32 MaxStack = MaxStackFor(Id);
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		const auto& S = Items[i];
		if (S.Id == Id)
		{
			if (!bFindNonFull || S.Count < MaxStack)
				return i;
		}
	}
	return {};
}

void UInventoryComponent::BroadcastChanged()
{
	OnInventoryChanged.Broadcast();
}
