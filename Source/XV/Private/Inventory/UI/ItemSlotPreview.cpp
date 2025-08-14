#include "Inventory/UI/ItemSlotPreview.h"
#include "Inventory/Data/Item/ItemData.h"

UItemSlotPreview::UItemSlotPreview(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UItemSlotPreview::NativeConstruct()
{
	Super::NativeConstruct();

	SetItemData();
}

void UItemSlotPreview::SetItemData()
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
		
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}
