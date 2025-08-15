#include "Inventory/UI/InventoryUI.h"
#include "Inventory/UI/ItemSlotUI.h"
#include "Inventory/Data/Armor/ArmorData.h"
#include "Inventory/Data/Armor/ArmorType.h"
#include "Inventory/Data/Item/ItemData.h"
#include "Inventory/Data/Item/ItemType.h"
#include "Blueprint/DragDropOperation.h"
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
		UE_LOG(LogTemp, Log, TEXT("In Helmet Border"));
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
	// 장비 부착
	if (BorderHelmet->GetCachedGeometry().IsUnderLocation(DropPosition) && ArmorData->ArmorType == EArmorType::Helmet && ItemData->ItemType == EItemType::Armor)
	{
		UE_LOG(LogTemp, Log, TEXT("In Helmet Border"));

		HelmetIcon->SetBrushFromTexture(ItemData->ItemIcon); // 아이콘 설정
		InventoryComp->EquipArmor(*ArmorData, EArmorType::Helmet); // 인벤토리 컴포넌트 장비 장착 호출
	}
	else if (BorderVest->GetCachedGeometry().IsUnderLocation(DropPosition) && ArmorData->ArmorType == EArmorType::Vest && ItemData->ItemType == EItemType::Armor)
	{
		UE_LOG(LogTemp, Log, TEXT("In Vest Border"));
		
		VestIcon->SetBrushFromTexture(ItemData->ItemIcon);
		InventoryComp->EquipArmor(*ArmorData, EArmorType::Vest);
	}
	// 라이플 부착물 부착
	if (RifleSilencer->GetCachedGeometry().IsUnderLocation(DropPosition) && AttachmentData->AttachmentType == EAttachmentType::Silencer && ItemData->ItemType == EItemType::Attachment)
	{
		UE_LOG(LogTemp, Log, TEXT("In Rifle Silencer Border"));

		RifleSilencerIcon->SetBrushFromTexture(ItemData->ItemIcon); // 아이콘 설정
		InventoryComp->EquipAttachment(*AttachmentData, EAttachmentType::Silencer, EWeaponType::Rifle); // 인벤토리 부착물 장비 장착 호출
	}
	else if (RifleExtendedMag->GetCachedGeometry().IsUnderLocation(DropPosition) && AttachmentData->AttachmentType == EAttachmentType::ExtendedMag && ItemData->ItemType == EItemType::Attachment)
	{
		UE_LOG(LogTemp, Log, TEXT("In Rifle ExtendedMag Border"));
		
		RifleExtendedMagIcon->SetBrushFromTexture(ItemData->ItemIcon); // 아이콘 설정
		InventoryComp->EquipAttachment(*AttachmentData, EAttachmentType::ExtendedMag, EWeaponType::Rifle); // 인벤토리 부착물 장비 장착 호출
	}
	
	// 피스톨 부착물 부착
	if (PistolSilencer->GetCachedGeometry().IsUnderLocation(DropPosition) && AttachmentData->AttachmentType == EAttachmentType::Silencer && ItemData->ItemType == EItemType::Attachment)
	{
		UE_LOG(LogTemp, Log, TEXT("In Pistol Silencer Border"));

		PistolSilencerIcon->SetBrushFromTexture(ItemData->ItemIcon); // 아이콘 설정
		InventoryComp->EquipAttachment(*AttachmentData, EAttachmentType::Silencer, EWeaponType::Pistol); // 인벤토리 부착물 장비 장착 호출
	}
	else if (PistolExtendedMag->GetCachedGeometry().IsUnderLocation(DropPosition) && AttachmentData->AttachmentType == EAttachmentType::ExtendedMag && ItemData->ItemType == EItemType::Attachment)
	{
		UE_LOG(LogTemp, Log, TEXT("In Pistol ExtendedMag Border"));
		
		PistolExtendedMagIcon->SetBrushFromTexture(ItemData->ItemIcon); // 아이콘 설정
		InventoryComp->EquipAttachment(*AttachmentData, EAttachmentType::ExtendedMag, EWeaponType::Pistol); // 인벤토리 부착물 장비 장착 호출
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
		NewSlot->InventoryComp = InventoryComp;
		NewSlot->ItemID = Item.ItemID;
		NewSlot->ItemType = Item.ItemType;
		NewSlot->ItemQuantity = Item.ItemQuantity;
		NewSlot->Index = Index;
		
		NewSlot->SetItemData();

		ItemWrapBox->AddChild(NewSlot);
	}
}