#include "Item/InteractableItem.h"
#include "Components/SphereComponent.h"
#include "Inventory/Component/ItemDataComponent.h"

AInteractableItem::AInteractableItem()
{
	ItemUseTime = 0;
	ItemRemainTime = 0;
	
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(Scene);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Scene);
	
	ItemDataComp = CreateDefaultSubobject<UItemDataComponent>(TEXT("ItemData"));

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

void AInteractableItem::BeginPlay()
{
	Super::BeginPlay();

	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetMassOverrideInKg(NAME_None, 50.0f, true);
}

void AInteractableItem::Interact()
{
}

