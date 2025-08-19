#include "Item/InteractiveItem.h"
#include "Components/SphereComponent.h"
#include "Inventory/Component/ItemDataComponent.h"
#include "Inventory/UI/InteractionUI.h"
#include "Components/WidgetComponent.h"

AInteractiveItem::AInteractiveItem()
{
	ItemUseTime = 0;
	ItemRemainTime = 0;
	
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMesh->SetCollisionObjectType(ECC_GameTraceChannel3);
	StaticMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	
	ItemDataComp = CreateDefaultSubobject<UItemDataComponent>(TEXT("ItemData"));

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(StaticMesh);

	if (InteractionWidgetClass)
	{
		InteractionWidget->SetWidgetClass(InteractionWidgetClass);
	}
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawSize(FVector2D(300.0f, 100.0f));
	
	// Tags.Add(FName("Interactable"));

}

void AInteractiveItem::UseItem()
{
}

float AInteractiveItem::GetItemTime()
{
	return ItemUseTime;
}

float AInteractiveItem::GetItemRemainTime()
{
	return ItemRemainTime;
}

void AInteractiveItem::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractiveItem::EnablePhysics()
{
	StaticMesh->SetSimulatePhysics(true);
}

void AInteractiveItem::Interact()
{
}

