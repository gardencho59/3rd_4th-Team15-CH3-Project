#include "Inventory/Component/InventoryComponent.h"
#include "Character/XVCharacter.h"
#include "Inventory/Data/Item/ItemSlot.h"
#include "Inventory/Data/Item/ItemData.h"
#include "Inventory/Data/Item/ItemSFX.h"
#include "Inventory/UI/InventoryUI.h"
#include "Components/WidgetComponent.h"
#include "Item/InteractableItem.h"
#include "Kismet/GameplayStatics.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	InventoryUI = nullptr;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	int32 Added = ItemSlots.Num();
	ItemSlots.SetNum(InventorySize);
	for (int32 Index = Added; Index < InventorySize; ++Index)
	{
		FItemSlot& ItemSlot = ItemSlots[Index];
		ItemSlot.ItemID = NAME_None;
		ItemSlot.ItemQuantity = 0.f;
	}
	UpdateInventory();
}

void UInventoryComponent::UpdateInventory()
{
	UE_LOG(LogTemp, Log, TEXT("Update Inventory!!!"));
	OnInventoryUpdated.Broadcast();
}

bool UInventoryComponent::PickUp(const FName& ItemID, float ItemQuantity)
{
	PrintInventory();
	bool bIsFull = false;
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
				AddToNewSlot(ItemID, 1, AvailableIndex);
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

void UInventoryComponent::AddToNewSlot(const FName& ItemID, const float ItemQuantity, const int32 Index)
{
	if (!ItemSlots.IsValidIndex(Index))
	{
		return;
	}
	
	ItemSlots[Index].ItemID = ItemID;
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

void UInventoryComponent::DropFromInventory(const FName ItemID, const int32 ItemQuantity, const int32 SlotIndex)
{
	PrintInventory();
	for (int Index = 0; Index < ItemQuantity; ++Index)
	{
		FItemData* ItemRow = GetItemData(ItemID);

		if (!ItemRow || !ItemRow->ItemClass)
		{
			UE_LOG(LogTemp, Log, TEXT("!ItemRow || !ItemRow->ItemClass, %s"), *ItemRow->ItemName.ToString());
			return;
		}

		AActor* DroppedItem = GetWorld()->SpawnActor<AInteractableItem>(
			ItemRow->ItemClass,
			GetDropLocation(),
			FRotator::ZeroRotator);
	}

	ItemSlots[SlotIndex].ItemID = NAME_None;
	ItemSlots[SlotIndex].ItemQuantity = 0;
	
	UpdateInventory();
	
	FItemSFX ItemSFX = GetItemSFX(ItemID);
	if (!ItemSFX.Drop)
	{
		return;
	}
	PlaySFX(ItemSFX.Drop);
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
				ItemSlot.ItemID = NAME_None;
				ItemSlot.ItemQuantity = 0;
			}
			UpdateInventory();
			return;
		}
	}
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
		UE_LOG(LogTemp, Log, TEXT("ItemID: %s, Quantity: %f"), *Slot.ItemID.ToString(), Slot.ItemQuantity);
	}
}

void UInventoryComponent::EquipArmor(const FArmorData& NewArmor, EArmorType ArmorType)
{
	AActor* Owner = GetOwner();
	AXVCharacter* XVCharacter = Cast<AXVCharacter>(Owner);
	XVCharacter->SetHelmet(NewArmor, ArmorType);
	
	switch (ArmorType)
	{
	case EArmorType::Helmet:
		UE_LOG(LogTemp, Log, TEXT("Equip Helmet!!"));
		EquippedArmor.Helmet = NewArmor;
		break;
	case EArmorType::Vest:
		UE_LOG(LogTemp, Log, TEXT("Equip Vest!!"));
		EquippedArmor.Vest = NewArmor;
		break;
	}
	for (FItemSlot& ItemSlot : ItemSlots)
	{
		if (ItemSlot.ItemID == NewArmor.ArmorName)
		{
			ItemSlot.ItemID = NAME_None;
			ItemSlot.ItemQuantity = 0;
			UpdateInventory();
			return;
		}
	}
	// Character 아이템 장착 함수 수행
	//OnArmorChanged.Broadcast();

}

// chracterclass BeginPlay()
// InventoryComp->OnArmorChanged.AddDynamic(this, &AMyCharacter::함수 이름);