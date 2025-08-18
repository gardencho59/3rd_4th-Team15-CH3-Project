#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "Components/Border.h"
#include "AttachmentSlot.h"
#include "Inventory/Data/Armor/ArmorType.h"
#include "Inventory/Data/Attachment/AttachmentType.h"
#include "Inventory/UI/ItemSlotUI.h"
#include "InventoryUI.generated.h"

class UInventoryComponent;

struct FItemData;
struct FArmorData;
struct FAttachmentData;

enum class EWeaponType : uint8;

UCLASS()
class XV_API UInventoryUI : public UUserWidget
{
	GENERATED_BODY()

public:

	UInventoryUI(const FObjectInitializer& ObjectInitializer);
	
	bool IsOverInventory(FVector2D ScreenPos);

	bool IsOverBorder(UBorder* Border, FVector2D ScreenPos);

	bool IsOverAnyBorder(FVector2D ScreenPos);
	
protected:
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	void CreateItemSlots();

	void CreateEquipmentSlots();

	void SetArmorSlots(UAttachmentSlot* Widget, FName ItemName, EArmorType ArmorType);
	
	void SetAttachmentSlots(UAttachmentSlot* Widget, FName ItemName, EAttachmentType AttachmentType, EWeaponType WeaponType);

	void EquipArmor(UAttachmentSlot* Widget, FItemData& ItemData, FArmorData& ArmorData, EArmorType ArmorType);

	void EquipAttachment(UAttachmentSlot* Widget, FItemData& ItemData, FAttachmentData& AttachmentData, EAttachmentType AttachmentType, EWeaponType WeaponType);

	UPROPERTY()
	UInventoryComponent* InventoryComp;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UItemSlotUI> ItemSlotWidgetClass;
	
	UPROPERTY(meta = (BindWidget))
	UWrapBox* ItemWrapBox;
	
	UPROPERTY(meta = (BindWidget))
	UBorder* BorderInventory;

	UPROPERTY(meta = (BindWidget))
	UBorder* BorderEquipment;
	
	UPROPERTY(meta = (BindWidget))
	UBorder* BorderRifle;
	
	UPROPERTY(meta = (BindWidget))
	UBorder* BorderPistol;
	
	// 방어구 관련
	
	UPROPERTY(meta = (BindWidget))
	UBorder* BorderHelmet;

	UPROPERTY(meta = (BindWidget))
	UBorder* BorderVest;
	
	UPROPERTY(meta = (BindWidget))
	UAttachmentSlot* WBP_Helmet;

	UPROPERTY(meta = (BindWidget))
	UAttachmentSlot* WBP_Vest;
	
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
	UAttachmentSlot* WBP_RifleSilencer;

	UPROPERTY(meta = (BindWidget))
	UAttachmentSlot* WBP_RifleExtendedMag;

	// 피스톨 부착물 관련 
	
	UPROPERTY(meta = (BindWidget))
	UBorder* PistolSilencer;
	
	UPROPERTY(meta = (BindWidget))
	UBorder* PistolExtendedMag;
	
	UPROPERTY(meta = (BindWidget))
	UAttachmentSlot* WBP_PistolSilencer;

	UPROPERTY(meta = (BindWidget))
	UAttachmentSlot* WBP_PistolExtendedMag;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	UDataTable* AttachmentDataTable;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UItemSlotPreview> ItemSlotPreviewClass;
	
public:
	
	void InitializeInventoryUI(UInventoryComponent* InInventoryComp);
};