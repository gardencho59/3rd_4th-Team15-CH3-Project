#include "AI/Notify/AI_PauseAnim.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/Character.h"

void UAI_PauseAnim::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (MeshComp)
	{
		MeshComp->bPauseAnims = true;
        
		// 1. Owner 가져오기 (ACharacter 또는 Pawn)
		AActor* OwnerActor = MeshComp->GetOwner();
		if (OwnerActor)
		{
			// 2. Character 형태로 캐스팅 (필요시)
			ACharacter* Character = Cast<ACharacter>(OwnerActor);
			if (Character)
			{
				// 3. 컨트롤러 가져오기
				AController* Controller = Character->GetController();

				// 4. AIController로 캐스팅 (만약 AI컨트롤러 사용 중이면)
				AAIController* AIController = Cast<AAIController>(Controller);
				if (AIController)
				{
					AIController->ClearFocus(EAIFocusPriority::Gameplay);
					
					AIController->BrainComponent->StopLogic(TEXT("Dead"));

					UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(AIController->BrainComponent);
					if (BTComp)
					{
						BTComp->StopTree(EBTStopMode::Safe); // 또는 ForceImmediate
					}

					// 타이머로 파괴
					FTimerHandle DestroyTimerHandle;
					if (UWorld* World = Character->GetWorld())
					{
						World->GetTimerManager().SetTimer(DestroyTimerHandle,[Character](){if (IsValid(Character)){Character->Destroy();}},DestroyDelayTime,false);
					}
				}
			}
		}
	}
}
