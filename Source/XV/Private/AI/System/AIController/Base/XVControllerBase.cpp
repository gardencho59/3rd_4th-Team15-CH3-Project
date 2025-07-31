#include "AI/System/AIController/Base/XVControllerBase.h"

// 디버깅용 추가
#include "AI/DebugTool/DebugTool.h"
#include "GameFramework/Character.h"
DEFINE_LOG_CATEGORY(Log_XV_AI);

// 추가됨
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "XVCharacter.h" 


AXVControllerBase::AXVControllerBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// AI Perception 컴포넌트 생성
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	check(AIPerception);

	// 감각 설정 생성 (눈)
	AISightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	check(AISightConfig);

	// 감각 설정 생성 (귀)
	AIHearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	check(AIHearingConfig);

	//Blackboard Component 생성
	AIBlackBoard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoard"));
	check(AIBlackBoard);
	
	// AI Perception에 감각들 구성
	AIPerception->ConfigureSense(*AISightConfig);
	AIPerception->ConfigureSense(*AIHearingConfig);
	AIPerception->SetDominantSense(AISightConfig->GetSenseImplementation());
}

void AXVControllerBase::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

}

void AXVControllerBase::BeginPlay()
{
    Super::BeginPlay();

	// 로그 확인
	LogDataAssetValues();

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this,&AXVControllerBase::OnTargetInfoUpdated);

	if(!BehaviorTreeAsset) return;
	RunBehaviorTree(BehaviorTreeAsset);
	
}

void AXVControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
    
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;
    
	FVector MyLocation = ControlledPawn->GetActorLocation();
	AIBlackBoard->SetValueAsVector(TEXT("MyLocation"), MyLocation);
    
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && PC->GetPawn())
	{
		FVector PlayerLocation = PC->GetPawn()->GetActorLocation();
		AIBlackBoard->SetValueAsVector(TEXT("TargetLocation"), PlayerLocation);
	}
}


void AXVControllerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AIPerception && IsValid(AIPerception))
	{
		AIPerception->OnTargetPerceptionUpdated.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void AXVControllerBase::OnTargetInfoUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (AXVCharacter* TargetCharacter = Cast<AXVCharacter>(Actor))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Saw: %s"), *TargetCharacter->GetName()));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Missed: %s"), *Actor->GetName()));
	}

	AXVCharacter* TargetCharacter = Cast<AXVCharacter>(Actor);
	
	if (Stimulus.WasSuccessfullySensed() && TargetCharacter)
	{
		DrawDebugString(GetWorld(),Actor->GetActorLocation() + FVector(0, 0, 100),FString::Printf(TEXT("Saw: %s"), *Actor->GetName()),nullptr,FColor::Green,2.0f,true);
		AIBlackBoard->SetValueAsObject(TEXT("TargetActor"), TargetCharacter);
		AIBlackBoard->SetValueAsBool(TEXT("CanSeeTarget"), true);
	}
	else
	{
		DrawDebugString(GetWorld(),Actor->GetActorLocation() + FVector(0, 0, 100),FString::Printf(TEXT("Missed: %s"), *Actor->GetName()),nullptr,FColor::Red,2.0f,true);
		AIBlackBoard->SetValueAsObject(TEXT("TargetActor"), nullptr);
		AIBlackBoard->SetValueAsBool(TEXT("CanSeeTarget"), false);
	}

}

void AXVControllerBase::LogDataAssetValues() const
{
	// 소유 테스트 확인
	UE_LOG(Log_XV_AI, Warning, TEXT("[Sparta] AI Controller is controlling %s."), *GetPawn()->GetName());
	
	// Sight Config 적용된 값 확인
	UE_LOG(LogTemp, Warning, TEXT("--- Sight Configuration ---"));
	UE_LOG(LogTemp, Warning, TEXT("Sight Radius: %.2f"), AISightConfig->SightRadius);
	UE_LOG(LogTemp, Warning, TEXT("Lose Sight Radius: %.2f"), AISightConfig->LoseSightRadius);
	UE_LOG(LogTemp, Warning, TEXT("Peripheral Vision Angle: %.2f"), AISightConfig->PeripheralVisionAngleDegrees);
	UE_LOG(LogTemp, Warning, TEXT("Max Age: %.2f"), AISightConfig->GetMaxAge());

	// Hearing Config 적용된 값 확인
	UE_LOG(LogTemp, Warning, TEXT("--- Hearing Configuration ---"));
	UE_LOG(LogTemp, Warning, TEXT("Hearing Range: %.2f"), AIHearingConfig->HearingRange);
	UE_LOG(LogTemp, Warning, TEXT("Max Age: %.2f"), AIHearingConfig->GetMaxAge());

	// 적용된 감지 설정 확인
	UE_LOG(LogTemp, Warning, TEXT("--- Detection Settings ---"));
	UE_LOG(LogTemp, Warning, TEXT("Sight - Detect Enemies: %s"), AISightConfig->DetectionByAffiliation.bDetectEnemies ? TEXT("True") : TEXT("False"));
	UE_LOG(LogTemp, Warning, TEXT("Sight - Detect Neutrals: %s"), AISightConfig->DetectionByAffiliation.bDetectNeutrals ? TEXT("True") : TEXT("False"));
	UE_LOG(LogTemp, Warning, TEXT("Sight - Detect Friendlies: %s"), AISightConfig->DetectionByAffiliation.bDetectFriendlies ? TEXT("True") : TEXT("False"));
    
	UE_LOG(LogTemp, Warning, TEXT("Hearing - Detect Enemies: %s"), AIHearingConfig->DetectionByAffiliation.bDetectEnemies ? TEXT("True") : TEXT("False"));
	UE_LOG(LogTemp, Warning, TEXT("Hearing - Detect Neutrals: %s"), AIHearingConfig->DetectionByAffiliation.bDetectNeutrals ? TEXT("True") : TEXT("False"));
	UE_LOG(LogTemp, Warning, TEXT("Hearing - Detect Friendlies: %s"), AIHearingConfig->DetectionByAffiliation.bDetectFriendlies ? TEXT("True") : TEXT("False"));

	
} // DataAsset 값들을 로그로 출력하는 함수

