#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Inventory/Data/ItemSlot.h"
#include "ItemDataComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XV_API UItemDataComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UItemDataComponent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	FDataTableRowHandle ItemDataHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	float ItemQuantity;
		
	FItemSlot* GetItemData();

public:	
	FName GetRowName();

	void PickUp();
};
