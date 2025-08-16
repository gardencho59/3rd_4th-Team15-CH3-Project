#include "Inventory/UI/AttachmentSlot.h"
#include "Inventory/UI/ItemSlotPreview.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UAttachmentSlot::UAttachmentSlot(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}


void UAttachmentSlot::NativeConstruct()
{
	Super::NativeConstruct();
	ImageIcon->SetVisibility(ESlateVisibility::Collapsed);
}

FReply UAttachmentSlot::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Display, TEXT("NativeOnPreviewMouseButtonDown"));
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (!IsEquipped)
		{
			return FReply::Unhandled();
		}
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

	// 인베토리 안일 때
	if (InventoryUI->IsOverInventory(DropPos))
	{
		InventoryComp->AddToInventory(ItemID);
		UnEquipAttachment();
		return;
	}

	// 인벤 UI 밖이면
	if (true)
	{
		InventoryComp->DropFromAttachment(ItemID);
		UnEquipAttachment();
	}
	
}


void UAttachmentSlot::SetWidgetData(FName NewItemID, EItemType NewItemType, UTexture2D* NewImageIcon,
	UInventoryUI* NewInventoryUI, UInventoryComponent* NewInventoryComponent, EArmorType NewArmorType, EAttachmentType NewAttachmentType, EWeaponType NewWeaponType)
{
	ItemID = NewItemID;
	ItemType = NewItemType;
	ItemTexture = NewImageIcon;
	InventoryUI = NewInventoryUI;
	InventoryComp = NewInventoryComponent;

	ArmorType = NewArmorType;
	AttachmentType = NewAttachmentType;
	WeaponType = NewWeaponType;
}

void UAttachmentSlot::EquipAttachment()
{
	ImageIcon->SetBrushFromTexture(ItemTexture);
	ImageIcon->SetVisibility(ESlateVisibility::Visible);
	IsEquipped = true;
}

void UAttachmentSlot::UnEquipAttachment()
{
	UE_LOG(LogTemp, Display, TEXT("UnEquipped"));
	ImageIcon->SetVisibility(ESlateVisibility::Collapsed);
	IsEquipped = false;
	
	switch (ItemType)
	{
	case EItemType::Armor:
		InventoryComp->UnEquipArmor(ArmorType);
		break;
	case EItemType::Attachment:
		InventoryComp->UnEquipAttachment(AttachmentType, WeaponType);
		break;
	}
}