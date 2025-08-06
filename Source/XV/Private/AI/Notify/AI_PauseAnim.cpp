#include "AI/Notify/AI_PauseAnim.h"

#include "AIController.h"
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
					Character->Destroy();
				}
			}
		}
	}

}

