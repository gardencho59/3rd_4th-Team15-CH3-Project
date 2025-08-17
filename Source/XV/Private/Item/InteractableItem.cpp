#include "Item/InteractableItem.h"
#include "Components/SphereComponent.h"
#include "Inventory/Component/ItemDataComponent.h"
#include "Inventory/UI/InteractionUI.h"
#include "Components/WidgetComponent.h"

AInteractableItem::AInteractableItem()
{
	ItemUseTime = 0;
	ItemRemainTime = 0;
	
	PrimaryActorTick.bCanEverTick = false;

	// Scene = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	// Collision->SetupAttachment(Scene);
	SetRootComponent(Collision);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Collision);

	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetMassOverrideInKg(NAME_None, 500.0f, true);

	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMesh->SetCollisionObjectType(ECC_GameTraceChannel3);
	
	ItemDataComp = CreateDefaultSubobject<UItemDataComponent>(TEXT("ItemData"));

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(Collision);

	if (InteractionWidgetClass)
	{
		InteractionWidget->SetWidgetClass(InteractionWidgetClass);
	}
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawSize(FVector2D(300.0f, 100.0f));
	
	Tags.Add(FName("Interactable"));

}

void AInteractableItem::UseItem()
{
}

float AInteractableItem::GetItemTime()
{
	return ItemUseTime;
}

float AInteractableItem::GetItemRemainTime()
{
	return ItemRemainTime;
}

void AInteractableItem::Interact()
{
}

