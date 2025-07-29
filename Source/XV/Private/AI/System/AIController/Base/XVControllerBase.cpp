#include "AI/System/AIController/Base/XVControllerBase.h"

// 디버깅용 추가
#include "AI/DebugTool/DebugTool.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
DEFINE_LOG_CATEGORY(Log_XV_AI);

// 추가됨
#include "AI/Data/Base/XVBlackBoardDataBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"

AXVControllerBase::AXVControllerBase()
	: RoateSpeed(480.f)
	, Acceleration(800.f)
	, Deceleration(400.f)
	, BrakingFriction(0.5f)
	, ControllerDesiredRotation(true)
	, OrientRotationToMovement(true)
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

	ACharacter* AICharacter = Cast<ACharacter>(InPawn);
	if (!AICharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid AI Character"));
		return;
	}

	auto* MovementComp = AICharacter->GetCharacterMovement();
	if (!MovementComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get movement component"));
		return;
	}
	
	// 회전 속도 조정
	MovementComp->RotationRate = FRotator(0.0f, RoateSpeed, 0.0f); // 부드러운 회전
          
	// 가속/감속 설정
	MovementComp->MaxAcceleration = Acceleration;			  // 가속도
	MovementComp->BrakingDecelerationWalking = Deceleration;  // 감속도
	MovementComp->BrakingFrictionFactor = BrakingFriction;    // 마찰 계수
          
	// 이동 보간 설정
	MovementComp->bUseControllerDesiredRotation = ControllerDesiredRotation;  // 컨트롤러 방향으로 부드럽게 회전
	MovementComp->bOrientRotationToMovement = OrientRotationToMovement;       // 이동 방향으로 캐릭터 회전
}

void AXVControllerBase::BeginPlay()
{
    Super::BeginPlay();

	// DataAsset 값으로 설정 적용
	ApplyDataAssetSettings();
	
	// 로그 확인
	LogDataAssetValues();

	// 블랙보드 업데이트
	UpdateBlackboardFromDataAsset();

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this,&AXVControllerBase::OnTargetInfoUpdated);

	UE_LOG(Log_XV_AI, Warning, TEXT("BehaviorTreeAsset is %s"), BehaviorTreeAsset ? TEXT("Valid") : TEXT("NULL"));
	StartBehaviorTree();
	
}

void AXVControllerBase::OnTargetInfoUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		DrawDebugString(GetWorld(),Actor->GetActorLocation() + FVector(0, 0, 100),FString::Printf(TEXT("Saw: %s"), *Actor->GetName()),nullptr,FColor::Green,2.0f,true);
		AIBlackBoard->SetValueAsObject(TEXT("TargetActor"), Actor);
		AIBlackBoard->SetValueAsVector(TEXT("TargetLocation"), Actor->GetActorLocation());
		AIBlackBoard->SetValueAsBool(TEXT("CanSeeTarget"), true);
		AIBlackBoard->SetValueAsVector(TEXT("MyLocation"), this->GetPawn()->GetActorLocation());
	}
	else
	{
		DrawDebugString(GetWorld(),Actor->GetActorLocation() + FVector(0, 0, 100),FString::Printf(TEXT("Missed: %s"), *Actor->GetName()),nullptr,FColor::Red,2.0f,true);
		AIBlackBoard->SetValueAsObject(TEXT("TargetActor"), nullptr);
		AIBlackBoard->SetValueAsVector(TEXT("LastTargetLocation"), Actor->GetActorLocation());
		AIBlackBoard->SetValueAsBool(TEXT("CanSeeTarget"), false);
		AIBlackBoard->SetValueAsVector(TEXT("MyLocation"), this->GetPawn()->GetActorLocation());
	}
}

void AXVControllerBase::StartBehaviorTree()
{
	if(!BehaviorTreeAsset) return;
	RunBehaviorTree(BehaviorTreeAsset);
}

void AXVControllerBase::ApplyDataAssetSettings()
{
	if (!AIBlackboardAsset) return;

	// SightConfig에 DataAsset 값 적용
	AISightConfig->SightRadius = AIBlackboardAsset->SightConfigData.AISightRadius;
	AISightConfig->LoseSightRadius = AIBlackboardAsset->SightConfigData.AILoseSightRadius;
	AISightConfig->PeripheralVisionAngleDegrees = AIBlackboardAsset->SightConfigData.AIPeripheralVisionAngleDegrees;
	AISightConfig->SetMaxAge(AIBlackboardAsset->SightConfigData.AISightSetMaxAge);

	// HearingConfig에 DataAsset 값 적용
	AIHearingConfig->HearingRange = AIBlackboardAsset->HearingConfigData.AIHearingRange;
	AIHearingConfig->SetMaxAge(AIBlackboardAsset->HearingConfigData.AIHearingSetMaxAge);

	// 감지 설정 적용 (눈)
	AISightConfig->DetectionByAffiliation.bDetectEnemies = AIBlackboardAsset->GeneralConfigData.AIbDetectEnemies;
	AISightConfig->DetectionByAffiliation.bDetectNeutrals = AIBlackboardAsset->GeneralConfigData.AIbDetectNeutrals;
	AISightConfig->DetectionByAffiliation.bDetectFriendlies = AIBlackboardAsset->GeneralConfigData.AIbDetectFriendlies;

	// 감지 설정 적용 (귀)
	AIHearingConfig->DetectionByAffiliation.bDetectEnemies = AIBlackboardAsset->GeneralConfigData.AIbDetectEnemies;
	AIHearingConfig->DetectionByAffiliation.bDetectNeutrals = AIBlackboardAsset->GeneralConfigData.AIbDetectNeutrals;
	AIHearingConfig->DetectionByAffiliation.bDetectFriendlies = AIBlackboardAsset->GeneralConfigData.AIbDetectFriendlies;

	AIPerception->RequestStimuliListenerUpdate();
}

void AXVControllerBase::UpdateBlackboardFromDataAsset()
{
	if (!AIBlackboardAsset || !AIBlackBoard) return;
	
	// StatusData를 블랙보드에 업데이트
	AIBlackBoard->SetValueAsFloat(TEXT("CurrentSpeed"), AIBlackboardAsset->StatusData.AICurrentSpeed);
	AIBlackBoard->SetValueAsFloat(TEXT("Damage"), AIBlackboardAsset->StatusData.AIDamage);
	AIBlackBoard->SetValueAsFloat(TEXT("CurrentHealth"), AIBlackboardAsset->StatusData.AICurrenHealth);
	AIBlackBoard->SetValueAsFloat(TEXT("MaxHealth"), AIBlackboardAsset->StatusData.AIMaxHealth);
	
	// SightConfig 데이터를 블랙보드에 업데이트
	AIBlackBoard->SetValueAsFloat(TEXT("SightRadius"), AIBlackboardAsset->SightConfigData.AISightRadius);
	AIBlackBoard->SetValueAsFloat(TEXT("LoseSightRadius"), AIBlackboardAsset->SightConfigData.AILoseSightRadius);
	AIBlackBoard->SetValueAsFloat(TEXT("PeripheralVisionAngle"), AIBlackboardAsset->SightConfigData.AIPeripheralVisionAngleDegrees);
	AIBlackBoard->SetValueAsFloat(TEXT("SightSetMaxAge"), AIBlackboardAsset->SightConfigData.AISightSetMaxAge);
	
	// HearingConfig 데이터를 블랙보드에 업데이트
	AIBlackBoard->SetValueAsFloat(TEXT("HearingRange"), AIBlackboardAsset->HearingConfigData.AIHearingRange);
	AIBlackBoard->SetValueAsFloat(TEXT("HearingSetMaxAge"), AIBlackboardAsset->HearingConfigData.AIHearingSetMaxAge);
	
	// GeneralConfig 데이터를 블랙보드에 업데이트
	AIBlackBoard->SetValueAsBool(TEXT("DetectEnemies"), AIBlackboardAsset->GeneralConfigData.AIbDetectEnemies);
	AIBlackBoard->SetValueAsBool(TEXT("DetectNeutrals"), AIBlackboardAsset->GeneralConfigData.AIbDetectNeutrals);
	AIBlackBoard->SetValueAsBool(TEXT("DetectFriendlies"), AIBlackboardAsset->GeneralConfigData.AIbDetectFriendlies);

	// 위치 데이터를 블랙보드에 업데이트
	AIBlackBoard->SetValueAsObject(TEXT("TargetActor"), AIBlackboardAsset->BlackBoardData.TargetActor);
	AIBlackBoard->SetValueAsVector(TEXT("TargetLocation"), AIBlackboardAsset->BlackBoardData.TargetLocation);
	AIBlackBoard->SetValueAsVector(TEXT("LastTargetLocation"), AIBlackboardAsset->BlackBoardData.LastTargetLocation);
	AIBlackBoard->SetValueAsVector(TEXT("MyLocation"), AIBlackboardAsset->BlackBoardData.MyLocation);
	
	// 퍼셉션 관련 bool 데이터를 블랙보드에 업데이트
	AIBlackBoard->SetValueAsBool(TEXT("CanSeeTarget"), AIBlackboardAsset->BlackBoardData.AICanSeeTarget);
	AIBlackBoard->SetValueAsBool(TEXT("IsInvestigating"), AIBlackboardAsset->BlackBoardData.AIIsInvestigating);
	AIBlackBoard->SetValueAsBool(TEXT("AIIsAttacking"), AIBlackboardAsset->BlackBoardData.AIIsAttacking);
	
} // 퍼셉션 관련 데이터만 업데이트

void AXVControllerBase::LogDataAssetValues() const
{
	// 소유 테스트 확인
	UE_LOG(Log_XV_AI, Warning, TEXT("[Sparta] AI Controller is controlling %s."), *GetPawn()->GetName());
	
	// SightConfigData 출력 테스트 (DataAsset에서 가져오기)
	UE_LOG(LogTemp, Warning, TEXT("--- Sight Configuration ---"));
	UE_LOG(Log_XV_AI, Warning, TEXT("AI character Sight Radius : %.2f"), AIBlackboardAsset->SightConfigData.AISightRadius);
	UE_LOG(Log_XV_AI, Warning, TEXT("AI character Lose Sight Radius : %.2f"), AIBlackboardAsset->SightConfigData.AILoseSightRadius);
	UE_LOG(Log_XV_AI, Warning, TEXT("AI character Peripheral Vision Angle : %.2f"), AIBlackboardAsset->SightConfigData.AIPeripheralVisionAngleDegrees);

	// HearingConfigData 출력 테스트 (DataAsset에서 가져오기)
	UE_LOG(LogTemp, Warning, TEXT("--- Hearing Configuration ---"));
	UE_LOG(Log_XV_AI, Warning, TEXT("AI Character %s - Hearing Config (From DataAsset):"), *GetName());
	UE_LOG(Log_XV_AI, Warning, TEXT("  Hearing Sight Radius: %.2f"), AIBlackboardAsset->HearingConfigData.AIHearingRange);
	UE_LOG(Log_XV_AI, Warning, TEXT("  Hearing Max age: %.2f"), AIBlackboardAsset->HearingConfigData.AIHearingSetMaxAge);

	// GeneralConfigData 출력 테스트 (DataAsset에서 가져오기)
	UE_LOG(LogTemp, Warning, TEXT("--- Detection Settings ---"));
	UE_LOG(Log_XV_AI, Warning, TEXT("AI Character %s - General Config (From DataAsset):"), *GetName());
	UE_LOG(Log_XV_AI, Warning, TEXT("  Detect Enemies: %s"), AIBlackboardAsset->GeneralConfigData.AIbDetectEnemies ? TEXT("True") : TEXT("False"));
	UE_LOG(Log_XV_AI, Warning, TEXT("  Detect Neutrals: %s"), AIBlackboardAsset->GeneralConfigData.AIbDetectNeutrals ? TEXT("True") : TEXT("False"));
	UE_LOG(Log_XV_AI, Warning, TEXT("  Detect Friendlies: %s"), AIBlackboardAsset->GeneralConfigData.AIbDetectFriendlies ? TEXT("True") : TEXT("False"));

	
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

