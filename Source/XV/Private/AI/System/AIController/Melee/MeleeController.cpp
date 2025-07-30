#include "AI/System/AIController/Melee/MeleeController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"


void AMeleeController::BeginPlay()
{
	Super::BeginPlay();

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("MeleeController BeginPlay"));
	
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this,&AMeleeController::OnTargetUpdated);
	
}

void AMeleeController::OnTargetUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	AIBlackBoard->SetValueAsBool(TEXT("AIIsAttacking"), true);
}
