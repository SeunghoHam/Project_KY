// Fill out your copyright notice in the Description page of Project Settings.


#include "Tanjiro/Widgets/InGameWidget.h"

#include "LandscapeUtils.h"
#include "DataAsset/ItemDB.h"
#include "Tanjiro/Components/InventoryComponent.h"
#include "Tanjiro/Widgets/ItemSlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/WidgetSwitcher.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Tanjiro/Tanjiro.h"

void UInGameWidget::ToggleSwitcher()
{
	if (!Switcher) return;
	int32 cur = Switcher->GetActiveWidgetIndex();
	if (cur == 0) Switcher->SetActiveWidgetIndex(1);
	else Switcher->SetActiveWidgetIndex(0);
}

void UInGameWidget::Init(UInventoryComponent* InInv, UItemDB* InDB, int32 InNumSlots, int32 InNumColumns,
	FVector2D InSlotSize)
{
	InventoryComponent = InInv; DB = InDB;
	NumColumns = InNumColumns; SlotSize = InSlotSize;
	if (InventoryComponent)
	{
		InventoryComponent->InitSlots(InNumSlots); // 원하면 여기서 슬롯 수 지정
		InventoryComponent->OnInventoryChanged.AddDynamic(this, &UInGameWidget::OnInventoryChanged);
	}
	BuildLayout();
	RefreshData();

	//R->OnEnemyRegistered.AddUObject(this, &AEnemyAttackState::HandleEnemyRegistered);
	if (ATanjiro* tanjiro = Cast<ATanjiro>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0)))
	{
		tanjiro->OnOniStateChange.AddUObject(this, &UInGameWidget::ChangeOniState);
	}
}
void UInGameWidget::BuildLayout()
{
	if (!Grid || !SlotClass || !InventoryComponent) return;
	Grid->ClearChildren();
	SlotPool.Reset();

	const int32 N = InventoryComponent->Items.Num();
	for (int32 i=0;i<N;++i)
	{
		UItemSlotWidget* ItemSlot = CreateWidget<UItemSlotWidget>(this, SlotClass);
		ItemSlot->SlotIndex = i;

		// SizeBox로 슬롯 크기 고정(디자이너에서 래핑해도 됨)
		USizeBox* Sizer = NewObject<USizeBox>(this);
		Sizer->SetWidthOverride(SlotSize.X);
		Sizer->SetHeightOverride(SlotSize.Y);
		Sizer->AddChild(ItemSlot);

		int32 Row = i / NumColumns;
		int32 Col = i % NumColumns;
		Grid->AddChildToUniformGrid(Sizer, Row, Col);

		SlotPool.Add(ItemSlot);
	}
}

void UInGameWidget::RefreshData()
{
	if (!InventoryComponent || !DB) return;
	const auto& Arr = InventoryComponent->Items;
	const int32 N = FMath::Min(Arr.Num(), SlotPool.Num());
	for (int32 i=0;i<N;++i)
	{
		SlotPool[i]->SetData(Arr[i].Id, Arr[i].Count, DB);
	}
}
void UInGameWidget::RebuildIfLayoutChanged(int32 InNumSlots, int32 InNumColumns, FVector2D InSlotSize)
{
	bool bNeedRebuild = (InNumColumns != NumColumns) || (InSlotSize != SlotSize) || (InventoryComponent && InventoryComponent->Items.Num() != InNumSlots);
	NumColumns = InNumColumns; SlotSize = InSlotSize;
	if (InventoryComponent && InventoryComponent->Items.Num() != InNumSlots) InventoryComponent->InitSlots(InNumSlots);

	if (bNeedRebuild) { BuildLayout(); }
	RefreshData();
}

void UInGameWidget::ChangeOniState(FText targetName, bool enable)
{
	TargetName->SetText(targetName);
	
	FText boolText = FText::FromString(enable ? TEXT("true") : TEXT("false"));
	CanAttack->SetText(boolText);
}

void UInGameWidget::OnInventoryChanged()
{
	RefreshData();
}



/*
void UInGameWidget::Init(UInventoryComponent* _comp, UItemDB* _DB)
{
	Comp = _comp; DB = _DB;
	if (Comp) Comp->OnInventoryChanged.AddDynamic(this, &UInGameWidget::Rebuild);
	Rebuild();
}



void UInGameWidget::Rebuild()
{
	if (!Grid || !Comp) return;
	Grid->ClearChildren();

	const auto& Arr = Comp->GetAll();
	for (int32 i=0;i<Arr.Num();++i)
	{
		// OwningObject this 에서 바꿔야되는지 확인
		UItemSlotWidget* ItemSlot = CreateWidget<UItemSlotWidget>(GetWorld(), SlotClass);
		ItemSlot->Setup(i, Arr[i], DB, Comp); // 아이콘/수량 세팅
		int32 Row = i / NumColumns;
		int32 Col = i % NumColumns;
		Grid->AddChildToUniformGrid(ItemSlot, Row, Col);
	}
}*/
