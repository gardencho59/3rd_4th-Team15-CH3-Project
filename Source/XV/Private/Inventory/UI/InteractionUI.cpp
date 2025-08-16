#include "Inventory/UI/InteractionUI.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/Component/ItemDataComponent.h"


UInteractionUI::UInteractionUI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UInteractionUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetVisibility(ESlateVisibility::Collapsed);
}


void UInteractionUI::SetItemData(FItemData NewItemData, int32 NewItemQuantity, bool IsVisible)
{
	if (!IsVisible)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	ItemData = NewItemData;
	ItemQuantity = NewItemQuantity;
	bIsVisible = IsVisible;
	SetUI();
}

void UInteractionUI::SetUI()
{
	if (!TextBlock || !ItemImage)
	{
		return;
	}

	FText Message;
	if (ItemData.ItemType == EItemType::AMMO)
	{
		Message = FText::Format(
			FText::FromString("{0} x {1}"),
			FText::FromName(ItemData.ItemName),
			FText::AsNumber(ItemQuantity));
	}
	else
	{
		Message = FText::FromName(ItemData.ItemName);
	}
		
	TextBlock->SetText(Message);
	ItemImage->SetBrushFromTexture(ItemData.ItemIcon);
	
	SetVisibility(ESlateVisibility::Visible);

}
