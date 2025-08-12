#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionUI.generated.h"

UCLASS()
class XV_API UInteractionUI : public UUserWidget
{
	GENERATED_BODY()

public:

	UInteractionUI(const FObjectInitializer& ObjectInitializer);

	void SetMessage(const FText& Message);
	void SetUI();

protected:
	
	virtual void NativePreConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MessageTextBlock;
	
	FText Message;
};
