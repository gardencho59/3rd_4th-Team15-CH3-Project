#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Engine/DataTable.h"
#include "ItemSlotPreview.generated.h"

UCLASS()

class XV_API UItemSlotPreview : public UUserWidget
{
	GENERATED_BODY()

public:

	UItemSlotPreview(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	UDataTable* ItemData;
	
public:
	void SetItemData();

	UPROPERTY()
	FName ItemID;

	UPROPERTY()
	float ItemQuantity;
};