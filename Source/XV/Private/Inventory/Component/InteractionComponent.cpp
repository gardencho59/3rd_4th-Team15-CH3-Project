#include "Inventory/Component/InteractionComponent.h"
#include "Item/ItemInterface.h"
#include "Components/WidgetComponent.h"
#include "Inventory/Component/ItemDataComponent.h"
#include "Inventory/UI/InteractionUI.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	TargetActor = nullptr;
	
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(
		InteractionTimerHandle,
		this,
		&UInteractionComponent::InteractionTrace,
		0.1f,
		true);
}

void UInteractionComponent::InteractionTrace()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	
	TArray<FHitResult> HitResults;
	FVector Start = Owner->GetActorLocation();
	Start.Z -= 30.0f;
	FVector ForwardVector = Owner->GetActorForwardVector();
	FVector End = Start + ForwardVector * 100.0f;
	float SphereRadius = 70.0f;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	// DrawDebugSphere(GetWorld(), Start, SphereRadius, 10, FColor::Green, false, 10, 1, 0.2);
	// DrawDebugSphere(GetWorld(), End, SphereRadius, 10, FColor::Green, false, 10, 1, 0.2);
	
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_GameTraceChannel3,
		FCollisionShape::MakeSphere(SphereRadius),
		Params);
		
	const FHitResult* ClosestHit = nullptr;
	float MinDistance = MAX_FLT;
        
	for (const FHitResult& HitResult : HitResults)
	{
		float Dist = FVector::Dist(HitResult.ImpactPoint, Owner->GetActorLocation());
		if (Dist < MinDistance)
		{
			MinDistance = Dist;
			ClosestHit = &HitResult;
		}
	}
	
	if (ClosestHit)
	{
		AActor* NewTargetActor = ClosestHit->GetActor();
		if (NewTargetActor != TargetActor)
		{
			SetUI(FItemData(), 0, false);
		}
		TargetActor = NewTargetActor;
		if (TargetActor && TargetActor->GetClass()->ImplementsInterface(UItemInterface::StaticClass())) // && TargetActor->Tags.Contains(FName("Interactable")))
		{
			UItemDataComponent* ItemDataComp = TargetActor->FindComponentByClass<UItemDataComponent>();
			if (ItemDataComp)
			{
				FItemData* ItemData = ItemDataComp->GetItemData();
				if (!ItemData)
				{
					return;
				}
				int32 ItemQuantity = ItemDataComp->GetItemQuantity();
				SetUI(*ItemData, ItemQuantity, true);
			}
		}
	}
	else
	{
		SetUI(FItemData(), 0, false);
		TargetActor = nullptr;
	}
}

UInteractionUI* UInteractionComponent::GetUIInstance(AActor* Item)
{
	if (!Item)
	{
		return nullptr;
	}
	UWidgetComponent* WidgetComp = Item->FindComponentByClass<UWidgetComponent>();
	if (!WidgetComp)
	{
		return nullptr;
	}

	UUserWidget* UserWidget = WidgetComp->GetUserWidgetObject();
	if (!UserWidget)
	{
		return nullptr;
	}

	return Cast<UInteractionUI>(UserWidget);
}

void UInteractionComponent::SetUI(const FItemData ItemData, int32 ItemQuantity, bool IsVisible)
{
	if (UInteractionUI* UIInstance = GetUIInstance(TargetActor))
	{
		UIInstance->SetItemData(ItemData, ItemQuantity, IsVisible);
	}
}

void UInteractionComponent::HandleItemInteract()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	
	if (!TargetActor)
	{
		return;
	}

	UItemDataComponent* ItemDataComp = TargetActor->FindComponentByClass<UItemDataComponent>();
	if (!ItemDataComp)
	{
		return;
	}
	
	ItemDataComp->PickUp(Owner);
}

AActor* UInteractionComponent::GetActor()
{
	return TargetActor;
}