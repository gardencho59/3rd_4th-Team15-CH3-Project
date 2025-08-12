#include "Inventory/UI/InteractionUI.h"
#include "Components/TextBlock.h"


UInteractionUI::UInteractionUI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MessageTextBlock = nullptr;
	Message = FText::GetEmpty();
}

void UInteractionUI::NativePreConstruct()
{
	Super::NativePreConstruct();

	SetUI();
}

void UInteractionUI::SetMessage(const FText& NewMessage)
{
	Message = NewMessage;
	SetUI();
}

void UInteractionUI::SetUI()
{
	if (!MessageTextBlock)
	{
		return;
	}
	
	MessageTextBlock->SetText(Message);
	
	if (!Message.IsEmpty())
	{
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}
