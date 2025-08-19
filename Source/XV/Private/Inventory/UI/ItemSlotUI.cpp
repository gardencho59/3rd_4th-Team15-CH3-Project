#include "Inventory/UI/ItemSlotUI.h"
#include "Components/Image.h"
#include "Inventory/Data/Item/ItemData.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Inventory/Component/InventoryComponent.h"

UItemSlotUI::UItemSlotUI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UItemSlotUI::NativeConstruct()
{
	Super::NativeConstruct();

	SetItemData();
}

FReply UItemSlotUI::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		FEventReply ReplyResult = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
		return ReplyResult.NativeReply;
	}

	return FReply::Unhandled();
}

bool UItemSlotUI::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return false;
}

void UItemSlotUI::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	
	if (!ItemSlotPreviewClass)
	{
		return;
	}
	
	UItemSlotPreview* ItemSlotPreview = CreateWidget<UItemSlotPreview>(GetWorld(), ItemSlotPreviewClass);
	if (!ItemSlotPreview)
	{
		return;
	}
	ItemSlotPreview->ItemID = ItemID;
	ItemSlotPreview->ItemQuantity = ItemQuantity;
	
	UDragDropOperation* DragOp = NewObject<UDragDropOperation>();
	if (!DragOp)
	{
		return;
	}
	DragOp->Pivot = EDragPivot::CenterCenter;
	DragOp->DefaultDragVisual = ItemSlotPreview;
	DragOp->Payload = this;

	OutOperation = DragOp;
}

void UItemSlotUI::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{

	if (!InventoryComp)
	{
		return;
	}

	InventoryComp->DropFromInventory(ItemID, ItemType, ItemQuantity, Index);
	
}

void UItemSlotUI::SetWidgetData(FName NewItemID, EItemType NewItemType, int32 NewItemQuantity, int32 NewIndex, UInventoryComponent* NewInventoryComp)
{
	ItemID = NewItemID;
	ItemType = NewItemType;
	ItemQuantity = NewItemQuantity;
	Index = NewIndex;
	InventoryComp = NewInventoryComp;
}

void UItemSlotUI::SetItemData()
{

	if (!ItemData || !ItemIcon)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	FItemData* ItemRow = ItemData->FindRow<FItemData>(ItemID, TEXT("Get Item Row in SlotUI"));
	if (ItemRow)
	{
		
		ItemIcon->SetBrushFromTexture(ItemRow->ItemIcon);
		ItemText->SetText(FText::FromName(ItemRow->ItemName));
		QuantityText->SetText(FText::AsNumber(ItemQuantity));
		
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}

}
