// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Tanjiro/Components/InventoryComponent.h"
#include "ItemSlotWidget.generated.h"

/**
 * 
 */
class UItemDB;
class UTextBlock;
class UImage;
UCLASS()
class KY_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))  UImage* IconImage = nullptr;
	UPROPERTY(meta=(BindWidget))  UTextBlock* QtyText = nullptr;
	UPROPERTY(meta=(BindWidgetOptional))  UTextBlock* NameHoverText = nullptr; // 오버레이형 선택
	// FItemStack = In InventoryComponent
	UFUNCTION(BlueprintCallable)
	void SetData(FName InId, int32 InCount, UItemDB* DB);
	//void Setup(int32 InIndex, const FInventorySlot& Stack, UItemDB* _DB, UInventoryComponent* _Comp);

	UPROPERTY(BlueprintReadOnly) int32 SlotIndex = -1;
	UPROPERTY(BlueprintReadOnly) FName ItemId = NAME_None;
	UPROPERTY(BlueprintReadOnly) int32 ItemCount = 0;
	
protected:
	//int32 Index = -1;
	//UPROPERTY() TObjectPtr<UInventoryComponent> Inv = nullptr;
	
	//virtual FReply NativeOnMouseButtonDown(const FGeometry& G, const FPointerEvent& E) override;
	//virtual void NativeOnDragDetected(const FGeometry& G, const FPointerEvent& E, UDragDropOperation*& OutOp) override;
	//virtual bool NativeOnDrop(const FGeometry& G, const FDragDropEvent& E, UDragDropOperation* Op) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeo, const FPointerEvent& InEvt) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InEvt) override;
	//UItemDB* DB;
};
