// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameWidget.generated.h"

/**
 * 
 */
class UWidgetSwitcher;
class UCanvasPanel;
class UItemSlotWidget;
class UInventoryComponent;
class UItemDB;
class UImage;
class UTextBlock;
UCLASS()
class KY_API UInGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite ,meta=(BindWidget))
	UWidgetSwitcher* Switcher;
	void ToggleSwitcher();

	UPROPERTY(BlueprintReadWrite ,meta=(BindWidget))
	UImage* AvoidImage;

	UPROPERTY(BlueprintReadWrite ,meta=(BindWidget)) UTextBlock* CanAttack;
	UPROPERTY(BlueprintReadWrite ,meta=(BindWidget)) UTextBlock* TargetName;
	

	UPROPERTY(meta=(BindWidget)) class UUniformGridPanel* Grid = nullptr;
	UPROPERTY(EditAnywhere) TSubclassOf<UItemSlotWidget> SlotClass;
	UPROPERTY(EditAnywhere) int32 NumColumns = 5;
	UPROPERTY(EditAnywhere, Category="InventoryUI") FVector2D SlotSize = FVector2D(96,96);

	UFUNCTION(BlueprintCallable)
	void Init(UInventoryComponent* InInv, UItemDB* InDB, int32 InNumSlots, int32 InNumColumns, FVector2D InSlotSize);

	UFUNCTION(BlueprintCallable)
	void RebuildIfLayoutChanged(int32 InNumSlots, int32 InNumColumns, FVector2D InSlotSize);


	void ChangeOniState(FText targetName, bool enable);

protected:
	UPROPERTY() TObjectPtr<UInventoryComponent> InventoryComponent = nullptr;
	UPROPERTY() TObjectPtr<UItemDB> DB = nullptr;
	UPROPERTY() TArray<TObjectPtr<UItemSlotWidget>> SlotPool;
	
	//UPROPERTY() TObjectPtr<UInventoryComponent> Comp;
	//UPROPERTY() TObjectPtr<UItemDB> DB;
	//UFUNCTION() void Rebuild();

	
	
	UFUNCTION() void OnInventoryChanged();
	void BuildLayout();   // 슬롯 위젯 생성 & Grid 배치(최초/레이아웃 변경 시)
	void RefreshData();   // 슬롯별 Icon/Qty/Name 갱신(자주 호출)

	
};
