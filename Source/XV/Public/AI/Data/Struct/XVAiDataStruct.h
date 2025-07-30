#pragma once

#include "CoreMinimal.h"
#include "XVAiDataStruct.generated.h"

/**
 * 
 */

USTRUCT(Atomic, BlueprintType)
struct FaiData_StatusStruct // 속도 및 체력 설정 (AI)
{
	GENERATED_BODY()

	FaiData_StatusStruct() = default;

public:

	// AI 공격 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AIAttackRadius = 100.f;
	
	// 현재 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AICurrentSpeed = 10.f;

	//공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AIDamage = 25.0f;

	// 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AICurrenHealth = 100.0f;

	// 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AIMaxHealth = 100.0f;
};

USTRUCT(Atomic, BlueprintType)
struct FaiData_SightConfigStruct // 눈 설정 (AI)
{
	GENERATED_BODY()

	FaiData_SightConfigStruct() = default;

public:
	// 시야 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AISightRadius = 1500.0f;

	// 시야를 잃는 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AILoseSightRadius = 1800.0f;

	// 시야각
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AIPeripheralVisionAngleDegrees = 75.0f;

	// 시야 기억력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AISightSetMaxAge = 8.0f;
};

USTRUCT(Atomic, BlueprintType)
struct FaiData_HearingConfigStruct // 귀 설정 (AI)
{
	GENERATED_BODY()

	FaiData_HearingConfigStruct() = default;

public:
	// 청각 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AIHearingRange = 1200.0f;
	
	// 청각 기억력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AIHearingSetMaxAge = 6.0f;
};

USTRUCT(Atomic, BlueprintType)
struct FaiData_GeneralConfigStruct // 일반적인 설정 (AI)
{
	GENERATED_BODY()

	FaiData_GeneralConfigStruct() = default;

public:
	// 적 감지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AIbDetectEnemies = true;

	// 중립 감지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AIbDetectNeutrals = true;

	// 아군 감지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AIbDetectFriendlies = true;
};

USTRUCT(Atomic, BlueprintType)
struct FaiData_ForBlackBoardStruct // 블랙보드에 담을 데이터 설정 (BlackBoard)
{
	GENERATED_BODY()

	FaiData_ForBlackBoardStruct() = default;

// 위치 변수
public:
	// 플레이어
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> TargetActor = nullptr;

	// 플레이어 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetLocation = FVector::ZeroVector;
	
	// 플레이어 마지막 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LastTargetLocation = FVector::ZeroVector;

	// AI의 현재 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MyLocation = FVector::ZeroVector;

// bool 변수
public:
	// 타겟 탐지 여부 (기본 false)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AICanSeeTarget = false;

	// 조사 중 상태 초기화 (기본 false)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AIIsInvestigating = false;

	// 공격 상태 여부 (기본 false)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AIIsAttacking = false;
};

class XV_API XVAiDataStruct
{
public:
	XVAiDataStruct();
	~XVAiDataStruct();
};