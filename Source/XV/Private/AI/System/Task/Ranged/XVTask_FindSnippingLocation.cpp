#include "AI/System/Task/Ranged/XVTask_FindSnippingLocation.h"
#include "AI/DebugTool/DebugTool.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UXVTask_FindSnippingLocation::UXVTask_FindSnippingLocation()
{
	NodeName = TEXT("Find Snipping Location");
	SnippingLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UXVTask_FindSnippingLocation, SnippingLocationKey));
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UXVTask_FindSnippingLocation, TargetKey), AActor::StaticClass());
}

EBTNodeResult::Type UXVTask_FindSnippingLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;
	
	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return EBTNodeResult::Failed;
	
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return EBTNodeResult::Failed;
	
	AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!Target) return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

	FVector TargetLocation = Target->GetActorLocation();
	
	for (int i = 0; i < 20; ++i)
	{
		FVector RandomPoint;
		if (NavSys->K2_GetRandomReachablePointInRadius(this, TargetLocation, RandomPoint, SearchRadius))
		{
			float Distance = FVector::Dist(RandomPoint, TargetLocation);
			if (Distance >= MinRange && Distance <= MaxRange)
			{
				FHitResult Hit;
				bool bVisible = !GetWorld()->LineTraceSingleByChannel(
					Hit,
					RandomPoint,
					TargetLocation,
					ECC_Visibility);
				if (bVisible)
				{
					Blackboard->SetValueAsVector(SnippingLocationKey.SelectedKeyName, RandomPoint);
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}

	return EBTNodeResult::Failed;
}