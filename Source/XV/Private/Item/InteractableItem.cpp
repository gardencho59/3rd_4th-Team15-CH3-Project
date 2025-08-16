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

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(Scene);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Scene);
	
	ItemDataComp = CreateDefaultSubobject<UItemDataComponent>(TEXT("ItemData"));

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(RootComponent);

	if (InteractionWidgetClass)
	{
		InteractionWidget->SetWidgetClass(InteractionWidgetClass);
	}
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawSize(FVector2D(300.0f, 100.0f));
	
	Tags.Add(FName("Interactable"));
	
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetCollisionObjectType(ECC_GameTraceChannel3);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
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

