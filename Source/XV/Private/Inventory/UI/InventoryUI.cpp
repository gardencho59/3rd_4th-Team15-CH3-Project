#include "Inventory/UI/InventoryUI.h"
#include "Inventory/UI/ItemSlotUI.h"
#include "Inventory/Data/Armor/ArmorData.h"
#include "Inventory/Data/Armor/ArmorType.h"
#include "Inventory/Data/Item/ItemData.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/Border.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Weapon/WeaponTypes.h"

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
	
	FVector2D DropPosition = InDragDropEvent.GetScreenSpacePosition();
	UItemSlotUI* DraggedItemSlot = Cast<UItemSlotUI>(InOperation->Payload);
	if (!DraggedItemSlot)
	{
		return false;
	}
	if (DraggedItemSlot)
	{
		UE_LOG(LogTemp, Log, TEXT("DraggedItemSlot"))
		if (IsOverInventory(DropPosition))
		{
			return true;
		}
	}

	FName ItemID = DraggedItemSlot->ItemID;
	FArmorData* ArmorData = ArmorDataTable->FindRow<FArmorData>(ItemID, TEXT("Get Armor Row in InventoryUI"));
	FAttachmentData* AttachmentData = AttachmentDataTable->FindRow<FAttachmentData>(ItemID, TEXT("Get Attachment Row in InventoryUI"));
	if (!AttachmentData && !ArmorData)
	{
		UE_LOG(LogTemp, Log, TEXT("Is Over Any Border"))
		if (IsOverAnyBorder(DropPosition))
		{
			return true;
		}
		return false;
	}

	// 아이콘 가져오기
	FItemData* ItemData = ItemDataTable->FindRow<FItemData>(ItemID, TEXT("Get Item Data in InventoryUI"));
	if (!ItemData)
	{
		return false;
	}

	// 장비 부착
	if (ArmorData)
	{
		if (IsOverBorder(BorderHelmet, DropPosition) && ArmorData->ArmorType == EArmorType::Helmet)
		{
			EquipArmor(WBP_Helmet, *ItemData, *ArmorData, EArmorType::Helmet);
		}
		if (IsOverBorder(BorderVest, DropPosition) && ArmorData->ArmorType == EArmorType::Vest)
		{
			EquipArmor(WBP_Vest, *ItemData, *ArmorData, EArmorType::Vest);
		}	
	}

	if (AttachmentData)
	{
		// 라이플 부착물 부착
		if (IsOverBorder(RifleSilencer, DropPosition) && AttachmentData->AttachmentType == EAttachmentType::Silencer)
		{
			EquipAttachment(WBP_RifleSilencer, *ItemData, *AttachmentData, EAttachmentType::Silencer, EWeaponType::Rifle);
		}
		if (IsOverBorder(RifleExtendedMag, DropPosition) && AttachmentData->AttachmentType == EAttachmentType::ExtendedMag)
		{
			EquipAttachment(WBP_RifleExtendedMag, *ItemData, *AttachmentData, EAttachmentType::ExtendedMag, EWeaponType::Rifle);
		}
	
		// 피스톨 부착물 부착
		if (IsOverBorder(PistolSilencer, DropPosition) && AttachmentData->AttachmentType == EAttachmentType::Silencer)
		{
			EquipAttachment(WBP_PistolSilencer, *ItemData, *AttachmentData, EAttachmentType::Silencer, EWeaponType::Pistol);
		}
		if (IsOverBorder(PistolExtendedMag, DropPosition) && AttachmentData->AttachmentType == EAttachmentType::ExtendedMag)
		{
			EquipAttachment(WBP_PistolExtendedMag, *ItemData, *AttachmentData, EAttachmentType::ExtendedMag, EWeaponType::Pistol);
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

	CreateEquipmentSlots();
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

void UInventoryUI::CreateEquipmentSlots()
{
	FName HelmetName = InventoryComp->EquippedArmor.Helmet.ArmorName;
	if (HelmetName != NAME_None)
	{
		SetArmorSlots(WBP_Helmet, HelmetName, EArmorType::Helmet);
	}
	FName VestName = InventoryComp->EquippedArmor.Vest.ArmorName;
	if (VestName != NAME_None)
	{
		SetArmorSlots(WBP_Vest, VestName, EArmorType::Vest);
	}
	FName RifleSilencerName = InventoryComp->RifleAttachment.Silencer.AttachmentName;
	if (RifleSilencerName != NAME_None)
	{
		SetAttachmentSlots(WBP_RifleSilencer, RifleSilencerName, EAttachmentType::Silencer, EWeaponType::Rifle);
	}
	FName RifleExtendedMagName = InventoryComp->RifleAttachment.ExtendedMag.AttachmentName;
	if (RifleExtendedMagName != NAME_None)
	{
		SetAttachmentSlots(WBP_RifleExtendedMag, RifleExtendedMagName, EAttachmentType::ExtendedMag, EWeaponType::Rifle);
	}
	FName PistolSilencerName = InventoryComp->PistolAttachment.Silencer.AttachmentName;
	if (PistolSilencerName != NAME_None)
	{
		SetAttachmentSlots(WBP_PistolSilencer, PistolSilencerName, EAttachmentType::Silencer, EWeaponType::Pistol);
	}
	FName PistolExtendedMagName = InventoryComp->PistolAttachment.ExtendedMag.AttachmentName;
	if (PistolExtendedMagName != NAME_None)
	{
		SetAttachmentSlots(WBP_PistolExtendedMag, PistolExtendedMagName, EAttachmentType::ExtendedMag, EWeaponType::Pistol);
	}
}

void UInventoryUI::SetArmorSlots(UAttachmentSlot* Widget, FName ItemName, EArmorType ArmorType)
{
	FItemData* ItemData = ItemDataTable->FindRow<FItemData>(ItemName, TEXT("Get Item Row in Inventory Component"));
	FArmorData* ArmorData = ArmorDataTable->FindRow<FArmorData>(ItemName, TEXT("Get Item Row in Inventory Component"));
	EquipArmor(Widget, *ItemData, *ArmorData, ArmorType);
}

void UInventoryUI::SetAttachmentSlots(UAttachmentSlot* Widget, FName ItemName, EAttachmentType AttachmentType, EWeaponType WeaponType)
{
	FItemData* ItemData = ItemDataTable->FindRow<FItemData>(ItemName, TEXT("Get Item Row in Inventory Component"));
	FAttachmentData* AttachmentData = AttachmentDataTable->FindRow<FAttachmentData>(ItemName, TEXT("Get Item Row in Inventory Component"));
	EquipAttachment(Widget, *ItemData, *AttachmentData, AttachmentType, WeaponType);
}

bool UInventoryUI::IsOverInventory(FVector2D ScreenPos)
{
	if (BorderInventory->GetCachedGeometry().IsUnderLocation(ScreenPos))
	{
		return true;
	}
	return false;
}

bool UInventoryUI::IsOverBorder(UBorder* Border, FVector2D ScreenPos)
{
	if (Border->GetCachedGeometry().IsUnderLocation(ScreenPos))
	{
		return true;
	}
	return false;
}

bool UInventoryUI::IsOverAnyBorder(FVector2D ScreenPos)
{
	if (BorderInventory->GetCachedGeometry().IsUnderLocation(ScreenPos) ||
		BorderEquipment->GetCachedGeometry().IsUnderLocation(ScreenPos) ||
		BorderRifle->GetCachedGeometry().IsUnderLocation(ScreenPos) ||
		BorderPistol->GetCachedGeometry().IsUnderLocation(ScreenPos))
	{
		return true;
	}
	return false;
}

void UInventoryUI::EquipArmor(UAttachmentSlot* Widget, FItemData& ItemData, FArmorData& ArmorData, EArmorType ArmorType)
{
	if (Widget->IsEquipped)
	{
		// 현재 장착 중인 무기 해제
		InventoryComp->AddToInventory(Widget->ItemID);
	}
	Widget->SetWidgetData(ItemData.ItemName, ItemData.ItemType, ItemData.ItemIcon, this, InventoryComp, ArmorType, EAttachmentType::None, EWeaponType::None); // 위젯에 데이터 전달
	Widget->EquipAttachment(); // 위젯에 장착
	InventoryComp->EquipArmor(ArmorData, ArmorType); // 인벤토리 컴포넌트 방어구 장착 호출
}

void UInventoryUI::EquipAttachment(UAttachmentSlot* Widget, FItemData& ItemData, FAttachmentData& AttachmentData, EAttachmentType AttachmentType, EWeaponType WeaponType)
{
	if (Widget->IsEquipped)
	{
		// 현재 장착 중인 무기 해제
		InventoryComp->AddToInventory(Widget->ItemID);
	}
	Widget->SetWidgetData(ItemData.ItemName, ItemData.ItemType, ItemData.ItemIcon, this, InventoryComp, EArmorType::None, AttachmentType, WeaponType); // 위젯에 데이터 전달
	Widget->EquipAttachment(); // 위젯에 장착
	InventoryComp->EquipAttachment(AttachmentData, AttachmentType, WeaponType); // 인벤토리 컴포넌트 부착물 장비 장착 호출
}
