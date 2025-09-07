// Fill out your copyright notice in the Description page of Project Settings.


#include "Tanjiro/Widgets/ItemSlotWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "DataAsset/ItemDB.h"

void UItemSlotWidget::SetData(FName InId, int32 InCount, UItemDB* DB)
{
	ItemId = InId; ItemCount = InCount;
	if (DB && !InId.IsNone())
	{
		FItemMeta Meta;
		if (DB->TryGet(InId, Meta))
		{
			if (IconImage) IconImage->SetBrushFromTexture(Meta.Icon, true);
			if (QtyText)   QtyText->SetText(FText::AsNumber(InCount));
			// TooltipText 방식
			SetToolTipText(Meta.DisplayName);
			// 오버레이 텍스트 방식(선택)
			if (NameHoverText)
			{
				NameHoverText->SetText(Meta.DisplayName);
				NameHoverText->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	else
	{
		if (IconImage) IconImage->SetBrushFromTexture(nullptr);
		if (QtyText)   QtyText->SetText(FText());
		SetToolTipText(FText());
		if (NameHoverText)
		{
			NameHoverText->SetText(FText());
			NameHoverText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

/*
void UItemSlotWidget::Setup(int32 InIndex, const FInventorySlot& Stack, UItemDB* _DB, UInventoryComponent* _Comp)
{
	Index = InIndex; Inv = _Comp;

	if ( _DB && !Stack.IsEmpty() && Icon)
		Icon->SetBrushFromTexture(_DB->GetIcon(Stack.Id), true);

	if (Qty) Qty->SetText(Stack.IsEmpty() ? FText::GetEmpty() : FText::AsNumber(Stack.Count));
}

FReply UItemSlotWidget::NativeOnMouseButtonDown(const FGeometry& G, const FPointerEvent& E)
{
	if (E.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		// FEventReply → FReply 변환
		//return UWidgetBlueprintLibrary::DetectDragIfPressed(E, this, EKeys::LeftMouseButton).NativeReply;
		return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	return Super::NativeOnMouseButtonDown(G, E);
}

void UItemSlotWidget::NativeOnDragDetected(const FGeometry& G, const FPointerEvent& E, UDragDropOperation*& OutOp)
{
	UDragDropOperation* Op = NewObject<UDragDropOperation>();
	Op->Payload = this;
	Op->DefaultDragVisual = this; // 간단히 자기 자신 표시(원하면 프록시 위젯)
	OutOp = Op;
}

bool UItemSlotWidget::NativeOnDrop(const FGeometry& G, const FDragDropEvent& E, UDragDropOperation* Op)
{
	if (auto* From = Cast<UItemSlotWidget>(Op->Payload))
	{
		if (Inv && Inv->GetOwnerRole()==ROLE_Authority)
		{
			Inv->MoveOrSwap(From->Index, Index); // 서버에서 처리 권장
		}
		else if (Inv)
		{
			// 클라→서버 RPC로 요청하는 래퍼를 만들어 호출
		}
		return true;
	}
	return false;
}
*/
void UItemSlotWidget::NativeOnMouseEnter(const FGeometry& InGeo, const FPointerEvent& InEvt)
{
	Super::NativeOnMouseEnter(InGeo, InEvt);
	if (NameHoverText) NameHoverText->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UItemSlotWidget::NativeOnMouseLeave(const FPointerEvent& InEvt)
{
	Super::NativeOnMouseLeave(InEvt);
	if (NameHoverText) NameHoverText->SetVisibility(ESlateVisibility::Hidden);
}
