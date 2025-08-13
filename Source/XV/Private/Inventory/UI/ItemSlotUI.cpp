#include "Inventory/UI/ItemSlotUI.h"
#include "Components/Image.h"
#include "Inventory/Data/ItemData.h"
#include "Components/TextBlock.h"

UItemSlotUI::UItemSlotUI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UItemSlotUI::NativePreConstruct()
{
	Super::NativePreConstruct();

	SetItemData();
}

void UItemSlotUI::SetItemData()
{

	if (!ItemData || !ItemIcon)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	FItemData* ItemRow = ItemData->FindRow<FItemData>(ItemID, TEXT("Get Item Row"));
	if (ItemRow)
	{
		
		ItemIcon->SetBrushFromTexture(ItemRow->ItemIcon);
		ItemText->SetText(FText::FromName(ItemRow->ItemName));
		QuantityText->SetText(FText::AsNumber(ItemQuantity));
		
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}

}
