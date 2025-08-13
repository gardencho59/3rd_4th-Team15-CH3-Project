#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Data/ItemData.h"
#include "Inventory/Data/ItemSlot.h"
#include "Inventory/Data/ItemSFX.h"
#include "Inventory/UI/InventoryUI.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FItemSlot> ItemSlots; // 인벤토리에서 관리하는 아이템 데이터

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

	bool PickUp(const FName& ItemID, float ItemQuantity);

	bool FindMatchingSlot(const FName& ItemID, int32& OutIndex);

	void AddToNewSlot(const FName& ItemID, const float ItemQuantity, const int32 Index);

	void AddToSlot(const int32 Index, const float ItemQuantity);

	bool AnyAvailableSlots(int32& OutIndex);

	void PlaySFX(USoundBase* Sound);

	void ToggleInventory();

	FItemSFX GetItemSFX(const FName& ItemID);

	FItemData* GetItemData(const FName& ItemID);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PrintInventory(); // 디버깅용

	const TArray<FItemSlot>& GetItemSlots() const {return ItemSlots;}
};
