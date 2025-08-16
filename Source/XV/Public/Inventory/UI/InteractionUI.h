#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/Data/Item/ItemData.h"
#include "InteractionUI.generated.h"

UCLASS()
class XV_API UInteractionUI : public UUserWidget
{
	GENERATED_BODY()

public:

	UInteractionUI(const FObjectInitializer& ObjectInitializer);

	void SetItemData(FItemData NewItemData, int32 NewItemQuantity, bool IsVisible);
	void SetUI();

protected:
	
	virtual void NativePreConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage;

	UPROPERTY()
	class UTexture2D* ItemIcon;

	UPROPERTY()
	FItemData ItemData;

	UPROPERTY()
	int32 ItemQuantity;
	
	bool bIsVisible;
};
