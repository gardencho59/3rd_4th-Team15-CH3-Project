#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/DataTable.h"
#include "ItemSlotUI.generated.h"

UCLASS()
class XV_API UItemSlotUI : public UUserWidget
{
	GENERATED_BODY()

public:

	UItemSlotUI(const FObjectInitializer& ObjectInitializer);

protected:
	
	virtual void NativePreConstruct() override;

	UPROPERTY()
	class UInventoryComponent* InventoryComp;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuantityText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	UDataTable* ItemData;
	
public:
	void SetItemData();
	
	UPROPERTY()
	FName ItemID;

	UPROPERTY()
	float ItemQuantity;
};
