#include "Inventory/UI/AttachmentSlot.h"
#include "Inventory/UI/ItemSlotPreview.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UAttachmentSlot::UAttachmentSlot(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

FReply UAttachmentSlot::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Display, TEXT("NativeOnPreviewMouseButtonDown"));
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		FEventReply ReplyResult = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
		return ReplyResult.NativeReply;
	}

	return FReply::Unhandled();
}

bool UAttachmentSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return false;
}

void UAttachmentSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UE_LOG(LogTemp, Display, TEXT("NativeOnDragDetected"));
	
	if (!ItemSlotPreviewClass)
	{
		return;
	}
	
	UItemSlotPreview* ItemSlotPreview = CreateWidget<UItemSlotPreview>(GetWorld(), ItemSlotPreviewClass);
	if (!ItemSlotPreview)
	{
		UE_LOG(LogTemp, Display, TEXT("Null ItemSlotPreview"));
		return;
	}
	ItemSlotPreview->ItemID = ItemID;
	
	UDragDropOperation* DragOp = NewObject<UDragDropOperation>();
	if (!DragOp)
	{
		UE_LOG(LogTemp, Display, TEXT("Null DragOp"));
		return;
	}
	DragOp->Pivot = EDragPivot::CenterCenter;
	DragOp->DefaultDragVisual = ItemSlotPreview;
	DragOp->Payload = this;

	OutOperation = DragOp;
}

void UAttachmentSlot::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Display, TEXT("NativeOnDragCancelled"));

	if (!InventoryComp)
	{
		UE_LOG(LogTemp, Display, TEXT("!InventoryComp"));
		return;
	}

	FVector2D DropPos = InDragDropEvent.GetScreenSpacePosition();
	if (!InventoryUI)
	{
		return;
	}
	if (InventoryUI->IsOverInventory(DropPos))
	{
		InventoryComp->AddToInventory(ItemID);
		UnEquipped();
		return;
	}

	// 인벤 UI 밖이면
	if (true)
	{
		InventoryComp->DropFromAttachment(ItemID);
		UnEquipped();
	}
	
}

void UAttachmentSlot::UnEquipped()
{
	UE_LOG(LogTemp, Display, TEXT("UnEquipped"));
}