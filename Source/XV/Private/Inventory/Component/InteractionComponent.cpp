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

void UInteractionComponent::InteractionTrace()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	
	FHitResult HitResult;
	FVector Start = Owner->GetActorLocation();
	FVector ForwardVector = Owner->GetActorForwardVector();
	FVector End = Start + ForwardVector * 100.0f;
	float SphereRadius = 50.0f;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);
	
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(SphereRadius),
		Params);

	if (bHit)
	{
		TargetActor = HitResult.GetActor();
		if (TargetActor && TargetActor->GetClass()->ImplementsInterface(UItemInterface::StaticClass()))
		{
			UItemDataComponent* ItemDataComp = TargetActor->FindComponentByClass<UItemDataComponent>();
			if (ItemDataComp)
			{
				FName RowName = ItemDataComp->GetRowName();
				if (UInteractionUI* UIInstance = GetUIInstance())
				{
					FString FormattedMessage = FString::Printf(TEXT("Press E : %s"), *RowName.ToString());
					UIInstance->SetMessage(FText::FromString(FormattedMessage));
				}
			}

		}
	}
	else
	{
		TargetActor = nullptr;
		if (UInteractionUI* UIInstance = GetUIInstance())
		{
			UIInstance->SetMessage(FText::GetEmpty());
		}
	}
}

UInteractionUI* UInteractionComponent::GetUIInstance()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	UWidgetComponent* WidgetComp = Owner->FindComponentByClass<UWidgetComponent>();
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