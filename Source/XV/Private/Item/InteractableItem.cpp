#include "Item/InteractableItem.h"
#include "Components/SphereComponent.h"
#include "Inventory/Component/ItemDataComponent.h"
#include "Inventory/UI/InteractionUI.h"
#include "Components/WidgetComponent.h"

AInteractableItem::AInteractableItem()
{
	ItemUseTime = 0;
	ItemRemainTime = 0;
	
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMesh->SetCollisionObjectType(ECC_GameTraceChannel3);
	
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

void AInteractableItem::BeginPlay()
{
	Super::BeginPlay();
	
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetMassOverrideInKg(NAME_None, 500.0f, true);
}

void AInteractableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (InteractionWidget)
	{
		FRotator CurrentRot = InteractionWidget->GetComponentRotation();
		InteractionWidget->SetWorldRotation(FRotator(0.0f, CurrentRot.Yaw, 0.0f));
	}
}

void AInteractableItem::Interact()
{
}

