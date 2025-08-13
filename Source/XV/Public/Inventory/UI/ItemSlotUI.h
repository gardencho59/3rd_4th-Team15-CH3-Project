#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/DataTable.h"
#include "ItemSlotPreview.h"
#include "ItemSlotUI.generated.h"

UCLASS()
class XV_API UItemSlotUI : public UUserWidget
{
	GENERATED_BODY()

public:

	UItemSlotUI(const FObjectInitializer& ObjectInitializer);

protected:
	
	virtual void NativeConstruct() override;
	
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	

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
	class UInventoryComponent* InventoryComp;
	
	UPROPERTY()
	FName ItemID;

	UPROPERTY()
	float ItemQuantity;

	UPROPERTY()
	int32 Index;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UItemSlotPreview> ItemSlotPreviewClass;
};
