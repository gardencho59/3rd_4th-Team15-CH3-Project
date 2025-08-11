#include "World/XVDoor.h"
#include "Components/BoxComponent.h"

AXVDoor::AXVDoor()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(SceneRoot);
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(SceneRoot);

	OpenOffset = FVector(0.f, 0.f, 235.f);
	IsOpening = false;

	PrimaryActorTick.bCanEverTick = true;
}

void AXVDoor::BeginPlay()
{
	Super::BeginPlay();
	
	ClosedPos = Door->GetRelativeLocation();
}

void AXVDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsOpening)
	{
		const float Speed = 1.f;
		Door->SetRelativeLocation(FMath::VInterpTo(Door->GetRelativeLocation(), TargetPos, DeltaTime, Speed));
	
		const float Threshold = 15.0f;
		if (FVector::Dist(Door->GetRelativeLocation(), TargetPos) < Threshold)
		{	
			UE_LOG(LogTemp, Warning, TEXT("Door Opened"));
			PrimaryActorTick.bCanEverTick = false;
		}
	}
}

void AXVDoor::OpenDoor()
{
	TargetPos = ClosedPos + OpenOffset;
	IsOpening = true;
}