#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "AttachmentSlot.generated.h"

class UInventoryUI;
class UInventoryComponent;
class UItemSlotPreview;

UCLASS()
class XV_API UAttachmentSlot : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	void UnEquipped();
	
public:

	UAttachmentSlot(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(meta=(BindWidget))
	UImage* ImageIcon;
			
	UPROPERTY()
	FName ItemID;

	UPROPERTY()
	UInventoryUI* InventoryUI;
	
	UPROPERTY()
	UInventoryComponent* InventoryComp;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UItemSlotPreview> ItemSlotPreviewClass;
};
