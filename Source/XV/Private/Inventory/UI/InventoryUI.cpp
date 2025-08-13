#include "Inventory/UI/InventoryUI.h"
#include "Inventory/UI/ItemSlotUI.h"
#include "Inventory/Component/InventoryComponent.h"

UInventoryUI::UInventoryUI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UInventoryUI::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}
	
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(TakeWidget());
	
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
}
 
void UInventoryUI::NativeDestruct()
{
	Super::NativeDestruct();
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}
	
	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->bShowMouseCursor = false;
}

bool UInventoryUI::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (!InOperation)
	{
		return false;
	}
	
	return true;
}
void UInventoryUI::InitializeInventoryUI(UInventoryComponent* InInventoryComp)
{
	if (!InInventoryComp)
	{
		return;
	}
	InventoryComp = InInventoryComp;

	InventoryComp->OnInventoryUpdated.RemoveAll(this);
	InventoryComp->OnInventoryUpdated.AddUObject(this, &UInventoryUI::CreateItemSlots);

	CreateItemSlots();
}

void UInventoryUI::CreateItemSlots()
{
	if (!InventoryComp || !ItemSlotWidgetClass || !ItemWrapBox)
	{
		return;
	}
	ItemWrapBox->ClearChildren();

	const TArray<FItemSlot>& ItemSlots = InventoryComp->GetItemSlots();
	for (int32 Index = 0; Index < ItemSlots.Num(); ++Index)
	{
		const FItemSlot& Item = ItemSlots[Index];
		UItemSlotUI* NewSlot = CreateWidget<UItemSlotUI>(GetWorld(), ItemSlotWidgetClass);
		if (!NewSlot)
		{
			return;
		}
		NewSlot->InventoryComp = InventoryComp;
		NewSlot->ItemID = Item.ItemID;
		NewSlot->ItemQuantity = Item.ItemQuantity;
		NewSlot->Index = Index;
		
		NewSlot->SetItemData();

		ItemWrapBox->AddChild(NewSlot);
	}
}