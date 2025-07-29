#include "XVTASK_IsNearPlayer.h"

// 추가됨
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UXVTASK_IsNearPlayer::UXVTASK_IsNearPlayer()
{
	NodeName = TEXT("MyLocation"); 
	MyLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UXVTASK_IsNearPlayer, MyLocationKey));

	NodeName = TEXT("PlayerLocation"); 
	PlayerLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UXVTASK_IsNearPlayer, PlayerLocationKey));
}

EBTNodeResult::Type UXVTASK_IsNearPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 오너 확인
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	// 폰 존재 확인
	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return EBTNodeResult::Failed;
	
	// 블랙보드 컴포넌트 가져오기
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("BlackboardComponent is not found!"));
		return EBTNodeResult::Failed;
	}

	// 두 위치 값 가져오기
	FVector MyLocation = BlackboardComp->GetValueAsVector(MyLocationKey.SelectedKeyName);
	FVector PlayerLocation = BlackboardComp->GetValueAsVector(PlayerLocationKey.SelectedKeyName);

	// 두 벡터 사이의 거리 계산
	float Distance = FVector::Distance(MyLocation, PlayerLocation);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Distance : %f"), Distance));
	
	// 100보다 작으면 Succeeded, 아니면 Failed 반환
	if(Distance < 100.0f)
	{
		BlackboardComp->SetValueAsBool(TEXT("AIIsAttacking"), true);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("AIIsAttacking : true"));
		return EBTNodeResult::Succeeded;
	}
	else
	{
		BlackboardComp->SetValueAsBool(TEXT("AIIsAttacking"), false);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("AIIsAttacking : false"));
		return EBTNodeResult::Failed;
	}
	
}
