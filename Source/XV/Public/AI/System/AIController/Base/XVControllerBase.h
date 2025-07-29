#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "XVControllerBase.generated.h"

class UAISenseConfig_Hearing;
class UAISenseConfig_Sight;
class UAIPerceptionComponent;
class UBlackboardComponent;
class UXVDataAssetBase;
class UXVBlackBoardDataBase;

UCLASS()
class XV_API AXVControllerBase : public AAIController
{
	GENERATED_BODY()

#pragma region LifeCycle // 라이프 사이클 관련 함수 모음
//---------------------------------------------------------------------------------------------------------------------//
public:
	AXVControllerBase();
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
#pragma endregion
	
#pragma region AIAction // AI 행동 관련 바인딩 함수
//---------------------------------------------------------------------------------------------------------------------//
protected:
	
	UFUNCTION()
	void OnTargetInfoUpdated(AActor* Actor, FAIStimulus Stimulus); // 감지한 타겟 정보 업데이트
#pragma endregion 

#pragma region Functions // 기타 함수들 모음 (리플렉션 등록 X)
//---------------------------------------------------------------------------------------------------------------------//
public:
	// 블랙 보드 getter 
	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return AIBlackBoard; }

	// 블랙보드 업데이트 함수들
	void UpdateBlackboardFromDataAsset();         

private:
	// DataAsset 설정을 Config에 적용하는 함수
	void ApplyDataAssetSettings();
	
	// DataAsset 값들을 로그로 출력하는 함수 (퍼셉션 관련 필수만)
	void LogDataAssetValues() const;

	// 비해비어 트리 시작 함수
	void StartBehaviorTree();
	
#pragma endregion 
	
#pragma region Var_And_Components // 변수 및 컴포넌트 모음
//---------------------------------------------------------------------------------------------------------------------//
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float RoateSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Deceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float BrakingFriction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool ControllerDesiredRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool OrientRotationToMovement;
	
	// 블랙보드 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UXVBlackBoardDataBase> AIBlackboardAsset;

	// 비해비어 트리
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;
	
protected:
	// 퍼셉션 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	// 눈
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> AISightConfig;

	// 귀
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Hearing> AIHearingConfig;

	// 블랙 보드 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UBlackboardComponent> AIBlackBoard;
#pragma endregion 
};