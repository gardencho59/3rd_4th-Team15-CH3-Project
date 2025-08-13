#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Inventory/Data/ItemData.h"
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

	// 아이템별 획득 수량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	float ItemQuantity;
		
	FItemData* GetItemData();

public:
	FName GetRowName();
	
	void PickUp(AActor* Interactor);
};
