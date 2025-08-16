#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Inventory/Data/Armor/ArmorData.h"
#include "Inventory/Data/Attachment/AttachmentData.h"
#include "AttachmentSlot.generated.h"

class UInventoryUI;
class UInventoryComponent;
class UItemSlotPreview;
class UTexture2D;

enum class EItemType : uint8;
enum class EWeaponType : uint8;

UCLASS()
class XV_API UAttachmentSlot : public UUserWidget
{
	GENERATED_BODY()
protected:

	virtual void NativeConstruct() override;
	
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
public:

	UAttachmentSlot(const FObjectInitializer& ObjectInitializer);

	void SetWidgetData(FName NewItemID, EItemType NewItemType, UTexture2D* NewImageIcon, UInventoryUI* NewInventoryUI,
		UInventoryComponent* NewInventoryComponent, EArmorType NewArmorType, EAttachmentType NewAttachmentType, EWeaponType NewWeaponType);

	void EquipAttachment();
	
	void UnEquipAttachment();;

	UPROPERTY(meta=(BindWidget))
	UImage* ImageIcon;
				
	UPROPERTY()
	FName ItemID;

	UPROPERTY()
	UTexture2D* ItemTexture;

	UPROPERTY()
	EItemType ItemType;

	UPROPERTY()
	UInventoryUI* InventoryUI;
	
	UPROPERTY()
	UInventoryComponent* InventoryComp;

	UPROPERTY()
	FArmorData ArmorData;

	UPROPERTY()
	EArmorType ArmorType;

	UPROPERTY()
	FAttachmentData AttachmentData;

	UPROPERTY()
	EAttachmentType AttachmentType;

	UPROPERTY()
	EWeaponType WeaponType;

	bool IsEquipped;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UItemSlotPreview> ItemSlotPreviewClass;
};
