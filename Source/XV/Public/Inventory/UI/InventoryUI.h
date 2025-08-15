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

	// 방어구 관련
	
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

	// 라이플 부착물 관련 
	
	UPROPERTY(meta = (BindWidget))
	UBorder* RifleSilencer;

	UPROPERTY(meta = (BindWidget))
	UBorder* RifleExtendedMag;

	UPROPERTY(meta = (BindWidget))
	UImage* RifleSilencerIcon;
	
	UPROPERTY(meta = (BindWidget))
	UImage* RifleExtendedMagIcon;


	// 피스톨 부착물 관련 
	
	UPROPERTY(meta = (BindWidget))
	UBorder* PistolSilencer;
	
	UPROPERTY(meta = (BindWidget))
	UBorder* PistolExtendedMag;

	UPROPERTY(meta = (BindWidget))
	UImage* PistolSilencerIcon;
	
	UPROPERTY(meta = (BindWidget))
	UImage* PistolExtendedMagIcon;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	UDataTable* AttachmentDataTable;
	
public:
	
	void InitializeInventoryUI(UInventoryComponent* InInventoryComp);
};