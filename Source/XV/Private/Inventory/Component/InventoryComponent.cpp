#include "Inventory/Component/InventoryComponent.h"
#include "Inventory/Component/ItemDataComponent.h"
#include "Character/XVCharacter.h"
#include "Weapon/GunBase.h"
#include "Inventory/Data/Item/ItemSlot.h"
#include "Inventory/Data/Item/ItemData.h"
#include "Inventory/Data/Item/ItemType.h"
#include "Inventory/Data/Item/ItemSFX.h"
#include "Inventory/UI/InventoryUI.h"
#include "Components/WidgetComponent.h"
#include "Inventory/Data/Attachment/AttachmentType.h"
#include "Item/InteractiveItem.h"
#include "Kismet/GameplayStatics.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	InventoryUI = nullptr;
	
	EquippedArmor = FEquippedArmor();
	RifleAttachment = FEquippedAttachment();
	PistolAttachment = FEquippedAttachment();
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	int32 Added = ItemSlots.Num();
	ItemSlots.SetNum(InventorySize);
	for (int32 Index = Added; Index < InventorySize; ++Index)
	{
		FItemSlot& ItemSlot = ItemSlots[Index];
		ResetSlot(&ItemSlot);
	}
	SetCurrentAMMO(EItemType::AMMORifle);
	SetCurrentAMMO(EItemType::AMMOPistol);
	UpdateInventory();

	
}

void UInventoryComponent::SetCurrentAMMO(EItemType ItemType)
{
	AActor* Owner = GetOwner();
	AXVCharacter* XVCharacter = Cast<AXVCharacter>(Owner);
	AGunBase* GunBase = XVCharacter->GetWeaponActor(ChangeItemTypeToWeaponType(ItemType));

	int32 CurrentAMMO = 0;
	for (FItemSlot& ItemSlot : ItemSlots)
	{
		if (ItemSlot.ItemType == ItemType)
		{
			CurrentAMMO += ItemSlot.ItemQuantity;
		}
	}
	GunBase->SetAMMO(CurrentAMMO);
}

EWeaponType UInventoryComponent::ChangeItemTypeToWeaponType(EItemType ItemType)
{
	switch (ItemType)
	{
	case EItemType::AMMORifle:
		return EWeaponType::Rifle;
	case EItemType::AMMOPistol:
		return EWeaponType::Pistol;
	}
	return EWeaponType::None;
}

void UInventoryComponent::ResetSlot(FItemSlot* ItemSlot)
{
	ItemSlot->ItemID = NAME_None;
	ItemSlot->ItemType = EItemType::None;
	ItemSlot->ItemQuantity = 0.f;
}

void UInventoryComponent::UpdateInventory()
{
	UE_LOG(LogTemp, Log, TEXT("Update Inventory!!!"));
	SortInventory();
	OnInventoryUpdated.Broadcast();
	
	AActor* Owner = GetOwner();
	AXVCharacter* XVCharacter = Cast<AXVCharacter>(Owner);
	XVCharacter->SetInventoryItem();
}

bool UInventoryComponent::PickUp(const FName& ItemID, const EItemType ItemType, float ItemQuantity)
{
	bool bIsFull = false;
	PrintInventory();
	bool bIsSuccess = false;
	while (ItemQuantity > 0 && !bIsFull)
	{
		int32 FoundIndex = -1;
		// 아이템 슬롯에 같은 아이템이 있을 때
		if (FindMatchingSlot(ItemID, FoundIndex))
		{
			AddToSlot(FoundIndex, 1);
			ItemQuantity --;
			UE_LOG(LogTemp, Log, TEXT("Add To Existing Slot!"))
			bIsSuccess = true;
		}
		// 아이템 슬롯에 같은 아이템이 없을 때
		else
		{
			UE_LOG(LogTemp, Log, TEXT("No Same Item In Slot"))
			// 슬롯에 넣을 곳이 있을 때
			int32 AvailableIndex = -1;
			if (AnyAvailableSlots(AvailableIndex))
			{
				// 새로운 슬롯에 아이템 넣기
				AddToNewSlot(ItemID, ItemType, 1, AvailableIndex);
				ItemQuantity --;
				UE_LOG(LogTemp, Log, TEXT("Add To New Slot!"))
				bIsSuccess = true;
			}
			// 슬롯에 넣을 곳이 없을 때
			else
			{
				UE_LOG(LogTemp, Log, TEXT("No Space In Slot"))
				bIsFull = true;
			}
		}
	}
	if (ItemType == EItemType::AMMORifle || ItemType == EItemType::AMMOPistol)
	{
		SetCurrentAMMO(ItemType);
	}
	UpdateInventory();

	if (bIsSuccess)
	{
		FItemSFX ItemSFX = GetItemSFX(ItemID);
		if (!ItemSFX.PickUp)
		{
			return false;
		}
		PlaySFX(ItemSFX.PickUp);
	}
	
	return bIsSuccess;
}

bool UInventoryComponent::FindMatchingSlot(const FName& ItemID, int32& OutIndex)
{
	for (int32 Index = 0; Index < ItemSlots.Num(); ++Index)
	{
		const FItemSlot& Slot = ItemSlots[Index];

		if (Slot.ItemID == NAME_None)
		{
			return false;
		}

		FItemData* ItemRow= GetItemData(Slot.ItemID);
		if (!ItemRow)
		{
			return false;
		}
		
		if (ItemID == Slot.ItemID && Slot.ItemQuantity < ItemRow->SlotQuantity)
		{
			OutIndex = Index;
			return true;
		}
	}
	OutIndex = -1;
	return false;
}

void UInventoryComponent::AddToNewSlot(const FName& ItemID, const EItemType ItemType, const float ItemQuantity, int32 Index)
{
	if (!ItemSlots.IsValidIndex(Index))
	{
		return;
	}
	
	ItemSlots[Index].ItemID = ItemID;
	ItemSlots[Index].ItemType = ItemType;
	ItemSlots[Index].ItemQuantity = ItemQuantity;
}

void UInventoryComponent::AddToSlot(const int32 Index, const float ItemQuantity)
{
	if (!ItemSlots.IsValidIndex(Index))
	{
		return;
	}
	
	ItemSlots[Index].ItemQuantity += ItemQuantity;
}

bool UInventoryComponent::AnyAvailableSlots(int32& OutIndex)
{
	for (int32 Index = 0; Index < ItemSlots.Num(); ++Index)
	{
		const FItemSlot& Slot = ItemSlots[Index];
		if (Slot.ItemQuantity == 0)
		{
			OutIndex = Index;
			return true;
		}
	}
	OutIndex = -1;
	return false;
}

void UInventoryComponent::PlaySFX(USoundBase* Sound)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	
	if (!Sound)
	{
		return;
	}
	
	FVector Location = Owner->GetActorLocation();
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location);
}

void UInventoryComponent::ToggleInventory()
{
	if (!InventoryUIClass)
	{
		return;
	}
	
	if (!InventoryUI)
	{
		InventoryUI = CreateWidget<UInventoryUI>(GetWorld(), InventoryUIClass);
		if (InventoryUI)
		{
			InventoryUI->AddToViewport();
			InventoryUI->InitializeInventoryUI(this);
		}
	}
	else
    {
        InventoryUI->RemoveFromParent();
        InventoryUI = nullptr;
    }
}

FVector UInventoryComponent::GetDropLocation()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return FVector::ZeroVector;
	}
	const FVector Location = Owner->GetActorLocation();
	const FVector ForwardVector = Owner->GetActorForwardVector();

	const float ConeHalfAngleRad = FMath::DegreesToRadians(55.0f);
	const FVector RandomDirection = FMath::VRandCone(ForwardVector, ConeHalfAngleRad);
	
	const float DropDistance = 100.0f;
	
	return Location + RandomDirection * DropDistance;
}

void UInventoryComponent::DropFromInventory(const FName ItemID, const EItemType ItemType, const int32 ItemQuantity, const int32 SlotIndex)
{
	
	
	if (ItemType == EItemType::AMMOPistol || ItemType == EItemType::AMMORifle)
	{
		AInteractiveItem* DroppedItem = SpawnItem(ItemID);
		SetItemQuantity(DroppedItem, SlotIndex);
		DroppedItem->EnablePhysics();
	}
	else
	{
		for (int i = 0; i < ItemSlots[SlotIndex].ItemQuantity; ++i)
		{
			AInteractiveItem* DroppedItem = SpawnItem(ItemID);
			DroppedItem->EnablePhysics();
		}
	}
	
	
	ResetSlot(&ItemSlots[SlotIndex]);

	if (ItemType == EItemType::AMMORifle || ItemType == EItemType::AMMOPistol)
	{
		SetCurrentAMMO(ItemType);
	}
	UpdateInventory();
	
	FItemSFX ItemSFX = GetItemSFX(ItemID);
	if (!ItemSFX.Drop)
	{
		return;
	}
	PlaySFX(ItemSFX.Drop);
}

void UInventoryComponent::DropFromAttachment(const FName ItemID)
{

	FItemData* ItemRow = GetItemData(ItemID);

	if (!ItemRow || !ItemRow->ItemClass)
	{
		UE_LOG(LogTemp, Log, TEXT("!ItemRow || !ItemRow->ItemClass, %s"), *ItemRow->ItemName.ToString());
		return;
	}

	AInteractiveItem* DroppedItem = GetWorld()->SpawnActor<AInteractiveItem>(
		ItemRow->ItemClass,
		GetDropLocation(),
		FRotator::ZeroRotator);
	DroppedItem->EnablePhysics();

	UpdateInventory();
	
	FItemSFX ItemSFX = GetItemSFX(ItemID);
	if (!ItemSFX.Drop)
	{
		return;
	}
	PlaySFX(ItemSFX.Drop);
	
}

AInteractiveItem* UInventoryComponent::SpawnItem(FName ItemID)
{
	FItemData* ItemRow = GetItemData(ItemID);
	if (!ItemRow || !ItemRow->ItemClass)
	{
		UE_LOG(LogTemp, Log, TEXT("!ItemRow || !ItemRow->ItemClass, %s"), *ItemRow->ItemName.ToString());
		return nullptr;
	}

	return GetWorld()->SpawnActor<AInteractiveItem>(
		ItemRow->ItemClass,
		GetDropLocation(),
		FRotator::ZeroRotator);
}

void UInventoryComponent::SetItemQuantity(AActor* DroppedItem, const int32 SlotIndex)
{
	UItemDataComponent* ItemDataComp = DroppedItem->FindComponentByClass<UItemDataComponent>();
	if (ItemDataComp)
	{
		ItemDataComp->SetItemQuantity(ItemSlots[SlotIndex].ItemQuantity);
	}
}

void UInventoryComponent::UseItem(const FName ItemID, const int32 ItemQuantity)
{
	for (FItemSlot& ItemSlot : ItemSlots)
	{
		if (ItemSlot.ItemID == ItemID)
		{
			if (ItemSlot.ItemQuantity < ItemQuantity)
			{
				UE_LOG(LogTemp, Log, TEXT("Not Enough Item In ItemSlot!"));
				return;
			}

			UE_LOG(LogTemp, Log, TEXT("Use Item: %s Item Quantity: %d "), *ItemID.ToString(), ItemQuantity);
			ItemSlot.ItemQuantity -= ItemQuantity;
			if (ItemSlot.ItemQuantity == 0)
			{
				ResetSlot(&ItemSlot);
			}
			UpdateInventory();
			return;
		}
	}
}

int32 UInventoryComponent::UseAMMO(EWeaponType WeaponType, int32 ItemQuantity)
{

	switch (WeaponType)
	{
	case EWeaponType::Rifle:
		return ConsumeAMMO(EItemType::AMMORifle, ItemQuantity);

	case EWeaponType::Pistol:
		return ConsumeAMMO(EItemType::AMMOPistol, ItemQuantity);
	}
	return 0.f;
}

int32 UInventoryComponent::ConsumeAMMO(EItemType ItemType, int32 ItemQuantity)
{
	TArray<FItemSlot*> FoundData;
	int32 CurrentAMMO = 0;
	int32 TargetAMMO = 0;

	for (FItemSlot& ItemSlot : ItemSlots)
	{
		if (ItemSlot.ItemType == ItemType)
		{
			FoundData.Add(&ItemSlot);
			CurrentAMMO += ItemSlot.ItemQuantity;
		}
	}
		
	if (CurrentAMMO == 0)
	{
		return TargetAMMO;
	}
	for (int32 i = FoundData.Num() - 1; i >= 0; --i)
	{
		int32 NeedAMMO = ItemQuantity - TargetAMMO;
			
		if (FoundData[i]->ItemQuantity < NeedAMMO)
		{
			TargetAMMO += FoundData[i]->ItemQuantity;
			ResetSlot(FoundData[i]);
		}
		else
		{
			TargetAMMO += NeedAMMO;
			FoundData[i]->ItemQuantity -= NeedAMMO;
		}
		if (TargetAMMO >= ItemQuantity)
		{
			break;
		}
	}
	SetCurrentAMMO(ItemType);
	UpdateInventory();
	return TargetAMMO;
}

void UInventoryComponent::SortInventory()
{
	ItemSlots.Sort([](const FItemSlot& A, const FItemSlot& B)
	{
		if (A.ItemType == B.ItemType)
		{
			if (A.ItemQuantity == B.ItemQuantity)
			{
				return A.ItemID.ToString() < B.ItemID.ToString();
			}
			return A.ItemQuantity > B.ItemQuantity;
		}
		return static_cast<uint8>(A.ItemType) < static_cast<uint8>(B.ItemType);
	});
}

bool UInventoryComponent::AddToInventory(const FName ItemID)
{
	int32 AvailableIndex = -1;
	if (AnyAvailableSlots(AvailableIndex))
	{
		FItemData* ItemRow = GetItemData(ItemID);
		if (!ItemRow)
		{
			return false;
		}
		
		AddToNewSlot(ItemID, ItemRow->ItemType, 1, AvailableIndex);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Inventory Is Full!!"));
		return false;
	}
	UpdateInventory();
	
	FItemSFX ItemSFX = GetItemSFX(ItemID);
	if (!ItemSFX.Move)
	{
		return true;
	}
	PlaySFX(ItemSFX.Move);

	return true;
}

float UInventoryComponent::GetItemQuantity(const FName ItemID)
{
	for (FItemSlot& ItemSlot : ItemSlots)
	{
		if (ItemSlot.ItemID == ItemID)
		{
			return ItemSlot.ItemQuantity;
		}
	}
	return 0.0f;
}

FItemSFX UInventoryComponent::GetItemSFX(const FName& ItemID)
{
	FItemData* ItemRow= GetItemData(ItemID);
	if (!ItemRow)
	{
		return FItemSFX();
	}
	return ItemRow->ItemSFX;
}


FItemData* UInventoryComponent::GetItemData(const FName& ItemID)
{
	if (!ItemData)
	{
		return nullptr;
	}

	return ItemData->FindRow<FItemData>(ItemID, TEXT("Get Item Row in Inventory Component"));
}


void UInventoryComponent::PrintInventory()
{
	for (const FItemSlot& Slot : ItemSlots)
	{
		UE_LOG(LogTemp, Log, TEXT("ItemID: %s, ITemType: %s, Quantity: %f"), *Slot.ItemID.ToString(), *StaticEnum<EItemType>()->GetNameStringByValue((int64)Slot.ItemType), Slot.ItemQuantity);
	}
}

void UInventoryComponent::EquipArmor(const FArmorData& NewArmor, EArmorType ArmorType)
{
	AActor* Owner = GetOwner();
	AXVCharacter* XVCharacter = Cast<AXVCharacter>(Owner);
	XVCharacter->SetArmor(NewArmor, ArmorType);
	
	switch (ArmorType)
	{
	case EArmorType::Helmet:
		UE_LOG(LogTemp, Log, TEXT("Equip Helmet!!"));
		if (EquippedArmor.Helmet.ArmorName == NewArmor.ArmorName) return;
		EquippedArmor.Helmet = NewArmor;
		break;
	case EArmorType::Vest:
		UE_LOG(LogTemp, Log, TEXT("Equip Vest!!"));
		if (EquippedArmor.Vest.ArmorName == NewArmor.ArmorName) return;
		EquippedArmor.Vest = NewArmor;
		break;
	}
	
	FItemSFX ItemSFX = GetItemSFX(NewArmor.ArmorName);
	if (!ItemSFX.Move)
	{
		return;
	}
	PlaySFX(ItemSFX.Move);
	
	for (FItemSlot& ItemSlot : ItemSlots)
	{
		if (ItemSlot.ItemID == NewArmor.ArmorName)
		{
			UE_LOG(LogTemp, Log, TEXT("NewArmor: %s"), *NewArmor.ArmorName.ToString());
			ResetSlot(&ItemSlot);
			UpdateInventory();
			return;
		}
	}

}

void UInventoryComponent::EquipAttachment(const FAttachmentData& NewAttachment, EAttachmentType AttachmentType, EWeaponType WeaponType)
{
	// 무기 부착 관련 함수
	AActor* Owner = GetOwner();
	AXVCharacter* XVCharacter = Cast<AXVCharacter>(Owner);
	AGunBase* GunBase = XVCharacter->GetWeaponActor(WeaponType);
	
	UE_LOG(LogTemp, Log, TEXT("WeaponType: %s"), *StaticEnum<EWeaponType>()->GetNameStringByValue(static_cast<int64>(WeaponType)));
	
	switch (AttachmentType)
	{
	case EAttachmentType::Silencer:
		switch (WeaponType)
		{
		case EWeaponType::Rifle:
			UE_LOG(LogTemp, Log, TEXT("Equip Silencer To Rifle"));
			if (RifleAttachment.Silencer.AttachmentName == NewAttachment.AttachmentName) return;
			RifleAttachment.Silencer = NewAttachment;
			GunBase->AttachSilencer();
			break;
		case EWeaponType::Pistol:
			UE_LOG(LogTemp, Log, TEXT("Equip Silencer To Pistol"));
			if (PistolAttachment.Silencer.AttachmentName == NewAttachment.AttachmentName) return;
			PistolAttachment.Silencer = NewAttachment;
			GunBase->AttachSilencer();
			break;
		}
		break;
	case EAttachmentType::ExtendedMag:
		switch (WeaponType)
		{
		case EWeaponType::Rifle:
			UE_LOG(LogTemp, Log, TEXT("Equip ExtendedMag To Rifle"));
			if (RifleAttachment.ExtendedMag.AttachmentName == NewAttachment.AttachmentName) return;
			RifleAttachment.ExtendedMag = NewAttachment;
			GunBase->AttachExtendedMag();
			break;
		case EWeaponType::Pistol:
			UE_LOG(LogTemp, Log, TEXT("Equip ExtendedMag To Pistol"));
			if (PistolAttachment.ExtendedMag.AttachmentName == NewAttachment.AttachmentName) return;
			PistolAttachment.ExtendedMag = NewAttachment;
			GunBase->AttachExtendedMag();
			break;
		}
		break;
	}
			
	FItemSFX ItemSFX = GetItemSFX(NewAttachment.AttachmentName);
	if (!ItemSFX.Move)
	{
		return;
	}
	PlaySFX(ItemSFX.Move);
	
	for (FItemSlot& ItemSlot : ItemSlots)
	{
		if (ItemSlot.ItemID == NewAttachment.AttachmentName)
		{
			UE_LOG(LogTemp, Log, TEXT("NewAttachment: %s"), *NewAttachment.AttachmentName.ToString());
			ResetSlot(&ItemSlot);
			UpdateInventory();
			return;
		}
	}
}

void UInventoryComponent::UnEquipArmor(EArmorType ArmorType)
{
	// 방어구 장착 해제 필요
	AActor* Owner = GetOwner();
	AXVCharacter* XVCharacter = Cast<AXVCharacter>(Owner);
	XVCharacter->UnEquipArmor(ArmorType);
	
	switch (ArmorType)
	{
	case EArmorType::Helmet:
		UE_LOG(LogTemp, Log, TEXT("UnEquip Helmet!!"));
		EquippedArmor.Helmet = FArmorData();
		break;
	case EArmorType::Vest:
		UE_LOG(LogTemp, Log, TEXT("UnEquip Helmet!!"));
		EquippedArmor.Vest = FArmorData();
		break;
	}
}

void UInventoryComponent::UnEquipAttachment(EAttachmentType AttachmentType, EWeaponType WeaponType)
{
	// 총기 부착물 장착 해제
	AActor* Owner = GetOwner();
	AXVCharacter* XVCharacter = Cast<AXVCharacter>(Owner);
	AGunBase* GunBase = XVCharacter->GetWeaponActor(WeaponType);
	
	switch (WeaponType)
	{
	case EWeaponType::Rifle:
		switch (AttachmentType)
		{
		case EAttachmentType::Silencer:
			UE_LOG(LogTemp, Log, TEXT("UnEquip Silencer!!"));
			RifleAttachment.Silencer = FAttachmentData();
			GunBase->DetachSilencer();
			break;
		case EAttachmentType::ExtendedMag:
			UE_LOG(LogTemp, Log, TEXT("UnEquip ExtendedMag!!"));
			RifleAttachment.ExtendedMag = FAttachmentData();
			GunBase->DetachExtendedMag();
			break;
		}
		break;
	case EWeaponType::Pistol:
		switch (AttachmentType)
		{
		case EAttachmentType::Silencer:
			UE_LOG(LogTemp, Log, TEXT("UnEquip Silencer!!"));
			PistolAttachment.Silencer = FAttachmentData();
			GunBase->DetachSilencer();
			break;
		case EAttachmentType::ExtendedMag:
			UE_LOG(LogTemp, Log, TEXT("UnEquip ExtendedMag!!"));
			PistolAttachment.ExtendedMag = FAttachmentData();
			GunBase->DetachExtendedMag();
			break;
		}
		break;
	}
}