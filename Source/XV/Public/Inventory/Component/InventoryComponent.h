#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Data/Item/ItemData.h"
#include "Inventory/Data/Item/ItemSlot.h"
#include "Inventory/Data/Item/ItemSFX.h"
#include "Inventory/Data/Armor/ArmorData.h"
#include "Inventory/Data/Armor/ArmorType.h"
#include "Inventory/Data/Armor/EquippedArmor.h"
#include "Inventory/Data/Attachment/AttachmentData.h"
#include "Inventory/Data/Attachment/EquippedAttachment.h"
#include "Inventory/UI/InventoryUI.h"
#include "Weapon/WeaponTypes.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XV_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

	// 인벤토리 아이템
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FItemSlot> ItemSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 InventorySize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	UDataTable* ItemData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<UInventoryUI> InventoryUIClass;

	UPROPERTY()
	UInventoryUI* InventoryUI;
	
public:

	FOnInventoryUpdated OnInventoryUpdated;
	
	void UpdateInventory();

	bool PickUp(const FName& ItemID, const EItemType, float ItemQuantity);

	bool FindMatchingSlot(const FName& ItemID, int32& OutIndex);

	void AddToNewSlot(const FName& ItemID, const EItemType ItemType, const float ItemQuantity, const int32 Index);

	void AddToSlot(const int32 Index, const float ItemQuantity);

	bool AnyAvailableSlots(int32& OutIndex);

	void PlaySFX(USoundBase* Sound);

	void ToggleInventory();
	
	FVector GetDropLocation();

	void DropFromInventory(const FName ItemID, const EItemType, const int32 ItemQuantity, const int32 SlotIndex);

	void DropFromAttachment(const FName ItemID);

	void UseItem(const FName ItemID, const int32 ItemQuantity);

	void SortInventory();

	void AddToInventory(const FName ItemID);

	// Getter 함수
	
	float GetItemQuantity(const FName ItemID);

	FItemSFX GetItemSFX(const FName& ItemID);

	FItemData* GetItemData(const FName& ItemID);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PrintInventory(); // 디버깅용

	const TArray<FItemSlot>& GetItemSlots() const {return ItemSlots;}

	bool bIsFull = false;
	
	// 방어구 관련
	void EquipArmor(const FArmorData& NewArmor, EArmorType ArmorType);
	
	// UPROPERTY()
	// FOnArmorChanged OnArmorChanged;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	// FArmorData ArmorData;

	UPROPERTY()
	FEquippedArmor EquippedArmor; // 현재 장착중인 장비

	// 부착물 관련
	void EquipAttachment(const FAttachmentData& NewAttachment, EAttachmentType AttachmentType, EWeaponType WeaponType);

	UPROPERTY()
	FEquippedAttachment RifleAttachment; // 현재 장착중인 라이플 부착물
	
	UPROPERTY()
	FEquippedAttachment PistolAttachment; // 현재 장착중인 피스톨 부착물

	
	void UnEquipArmor(EArmorType ArmorType);
	void UnEquipAttachment(EAttachmentType AttachmentType, EWeaponType WeaponType);
};
