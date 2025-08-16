#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Inventory/Data/Item/ItemData.h"
#include "ItemDataComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XV_API UItemDataComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UItemDataComponent();

	void SetItemQuantity(int32 NewItemQuantity);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	FDataTableRowHandle ItemDataHandle;

	// 아이템별 획득 수량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	int32 ItemQuantity;
		

public:	
	FItemData* GetItemData();

	FORCEINLINE FName GetRowName() const {return ItemDataHandle.RowName;}

	FORCEINLINE int32 GetItemQuantity() const { return ItemQuantity; }
	
	void PickUp(AActor* Interactor);
};
