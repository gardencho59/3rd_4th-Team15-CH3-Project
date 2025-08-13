#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "Inventory/UI/ItemSlotUI.h"
#include "InventoryUI.generated.h"

class UInventoryComponent;

UCLASS()
class XV_API UInventoryUI : public UUserWidget
{
	GENERATED_BODY()

public:

	UInventoryUI(const FObjectInitializer& ObjectInitializer);
	
protected:
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	void CreateItemSlots();

	UPROPERTY()
	UInventoryComponent* InventoryComp;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UItemSlotUI> ItemSlotWidgetClass;
	
	UPROPERTY(meta = (BindWidget))
	UWrapBox* ItemWrapBox;

	
public:
	
	void InitializeInventoryUI(UInventoryComponent* InInventoryComp);
};