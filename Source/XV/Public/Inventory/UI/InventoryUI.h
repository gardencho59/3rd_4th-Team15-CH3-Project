#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "Components/Border.h"
#include "Components/Image.h"
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

	UPROPERTY(meta = (BindWidget))
	UBorder* BorderHelmet;

	UPROPERTY(meta = (BindWidget))
	UBorder* BorderVest;

	UPROPERTY(meta = (BindWidget))
	UImage* HelmetIcon;
	
	UPROPERTY(meta = (BindWidget))
	UImage* VestIcon;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	UDataTable* ItemDataTable;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	UDataTable* ArmorDataTable;
	
public:
	
	void InitializeInventoryUI(UInventoryComponent* InInventoryComp);
};