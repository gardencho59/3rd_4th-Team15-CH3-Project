#include "Inventory/UI/InventoryUI.h"
#include "Inventory/UI/ItemSlotUI.h"
#include "Inventory/Data/Armor/ArmorData.h"
#include "Inventory/Data/Armor/ArmorType.h"
#include "Inventory/Data/Item/ItemData.h"
#include "Inventory/Data/Item/ItemType.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Inventory/Component/InventoryComponent.h"

UInventoryUI::UInventoryUI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UInventoryUI::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}
	
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(TakeWidget());
	
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
}
 
void UInventoryUI::NativeDestruct()
{
	Super::NativeDestruct();
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}
	
	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->bShowMouseCursor = false;
}

bool UInventoryUI::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (!InOperation)
	{
		return false;
	}
	
	UItemSlotUI* DraggedSlot = Cast<UItemSlotUI>(InOperation->Payload);
	if (!DraggedSlot)
	{
		UE_LOG(LogTemp, Log, TEXT("!Dragged Slot"));
		return false;
	}
	
	FName ItemID = DraggedSlot->ItemID;
	FArmorData* ArmorData = ArmorDataTable->FindRow<FArmorData>(ItemID, TEXT("Get Armor Row in InventoryUI"));
	FAttachmentData* AttachmentData = AttachmentDataTable->FindRow<FAttachmentData>(ItemID, TEXT("Get Attachment Row in InventoryUI"));
	if (!AttachmentData && !ArmorData)
	{
		return false;
	}

	// 아이콘 가져오기
	FItemData* ItemData = ItemDataTable->FindRow<FItemData>(ItemID, TEXT("Get Item Data in InventoryUI"));
	if (!ItemData)
	{
		return false;
	}

	FVector2D DropPosition = InDragDropEvent.GetScreenSpacePosition();
	// 장비 부착 -> 이미 부착된 상태면 원래 아이템 인벤에 추가되어야함
	if (ArmorData)
	{
		if (BorderHelmet->GetCachedGeometry().IsUnderLocation(DropPosition) && ArmorData->ArmorType == EArmorType::Helmet && ItemData->ItemType == EItemType::Armor)
		{
			EquipArmor(WBP_Helmet, *ItemData, *ArmorData, EArmorType::Helmet);
			return false;
		}
		if (BorderVest->GetCachedGeometry().IsUnderLocation(DropPosition) && ArmorData->ArmorType == EArmorType::Vest && ItemData->ItemType == EItemType::Armor)
		{
			EquipArmor(WBP_Vest, *ItemData, *ArmorData, EArmorType::Vest);
			return false;
		}	
	}

	if (AttachmentData)
	{
		// 라이플 부착물 부착
		if (RifleSilencer->GetCachedGeometry().IsUnderLocation(DropPosition) && AttachmentData->AttachmentType == EAttachmentType::Silencer && ItemData->ItemType == EItemType::Attachment)
		{
			EquipAttachment(WBP_RifleSilencer, *ItemData, *AttachmentData, EAttachmentType::Silencer, EWeaponType::Rifle);
			return false;
		}
		if (RifleExtendedMag->GetCachedGeometry().IsUnderLocation(DropPosition) && AttachmentData->AttachmentType == EAttachmentType::ExtendedMag && ItemData->ItemType == EItemType::Attachment)
		{
			EquipAttachment(WBP_RifleExtendedMag, *ItemData, *AttachmentData, EAttachmentType::ExtendedMag, EWeaponType::Rifle);
			return false;
		}
	
		// 피스톨 부착물 부착
		if (PistolSilencer->GetCachedGeometry().IsUnderLocation(DropPosition) && AttachmentData->AttachmentType == EAttachmentType::Silencer && ItemData->ItemType == EItemType::Attachment)
		{
			EquipAttachment(WBP_PistolSilencer, *ItemData, *AttachmentData, EAttachmentType::Silencer, EWeaponType::Pistol);
		}
		if (PistolExtendedMag->GetCachedGeometry().IsUnderLocation(DropPosition) && AttachmentData->AttachmentType == EAttachmentType::ExtendedMag && ItemData->ItemType == EItemType::Attachment)
		{
			EquipAttachment(WBP_PistolExtendedMag, *ItemData, *AttachmentData, EAttachmentType::ExtendedMag, EWeaponType::Pistol);
			return false;
		}
	}
	return true;
}

void UInventoryUI::InitializeInventoryUI(UInventoryComponent* InInventoryComp)
{
	if (!InInventoryComp)
	{
		return;
	}
	InventoryComp = InInventoryComp;

	InventoryComp->OnInventoryUpdated.RemoveAll(this);
	InventoryComp->OnInventoryUpdated.AddUObject(this, &UInventoryUI::CreateItemSlots);

	CreateItemSlots();
}

void UInventoryUI::CreateItemSlots()
{
	if (!InventoryComp || !ItemSlotWidgetClass || !ItemWrapBox)
	{
		return;
	}
	ItemWrapBox->ClearChildren();

	const TArray<FItemSlot>& ItemSlots = InventoryComp->GetItemSlots();
	for (int32 Index = 0; Index < ItemSlots.Num(); ++Index)
	{
		const FItemSlot& Item = ItemSlots[Index];
		if (Item.ItemID == NAME_None)
		{
			return;
		}
		UItemSlotUI* NewSlot = CreateWidget<UItemSlotUI>(GetWorld(), ItemSlotWidgetClass);
		if (!NewSlot)
		{
			return;
		}
		NewSlot->SetWidgetData(Item.ItemID, Item.ItemType, Item.ItemQuantity, Index, InventoryComp);
		NewSlot->SetItemData();

		ItemWrapBox->AddChild(NewSlot);
	}
}

bool UInventoryUI::IsOverInventory(FVector2D ScreenPos)
{
	if (ItemScrollBorder->GetCachedGeometry().IsUnderLocation(ScreenPos))
	{
		return true;
	}
	return false;
}

void UInventoryUI::EquipArmor(UAttachmentSlot* Widget, FItemData& ItemData, FArmorData& ArmorData, EArmorType ArmorType)
{
	if (!Widget->InventoryComp)
	{
		Widget->InventoryComp = InventoryComp;
	}

	Widget->SetWidgetData(ItemData.ItemName, ItemData.ItemType, ItemData.ItemIcon, this, InventoryComp, ArmorType, EAttachmentType::None, EWeaponType::None); // 위젯에 데이터 전달
	Widget->EquipAttachment(); // 위젯에 장착
	InventoryComp->EquipArmor(ArmorData, ArmorType); // 인벤토리 컴포넌트 방어구 장착 호출
}

void UInventoryUI::EquipAttachment(UAttachmentSlot* Widget, FItemData& ItemData, FAttachmentData& AttachmentData, EAttachmentType AttachmentType, EWeaponType WeaponType)
{
	if (!Widget->InventoryComp)
	{
		Widget->InventoryComp = InventoryComp;
	}

	Widget->SetWidgetData(ItemData.ItemName, ItemData.ItemType, ItemData.ItemIcon, this, InventoryComp, EArmorType::None, AttachmentType, WeaponType); // 위젯에 데이터 전달
	Widget->EquipAttachment(); // 위젯에 장착
	InventoryComp->EquipAttachment(AttachmentData, AttachmentType, WeaponType); // 인벤토리 컴포넌트 부착물 장비 장착 호출
}
