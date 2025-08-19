
#pragma once

#include <Inventory/Data/Attachment/EquippedAttachment.h>

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Inventory/Data/Armor/EquippedArmor.h"
#include "XVGameInstance.generated.h"

struct FItemSlot;

UCLASS()
class XV_API UXVGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UXVGameInstance();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Data")
	int32 CurrentLevelIdx;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Data")
	int32 CurrentMissionIdx;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Data")
	bool WasCinePlay;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Data")
	int32 TotalKilledEnemyCount;
	
	UPROPERTY()
	TArray<FItemSlot> SavedItemSlots;
	UPROPERTY()
	FEquippedArmor SavedEquippedArmor;
	UPROPERTY()
	FEquippedAttachment SavedRifleAttachment;
	UPROPERTY()
	FEquippedAttachment SavedPistolAttachment;

	UPROPERTY()
	float SavedHealth;
	
	void SaveInventory(class UInventoryComponent* Inventory);
	void LoadInventory(class UInventoryComponent* Inventory);
};