// Item/HealthPotionItem.h
#pragma once

#include "CoreMinimal.h"
#include "Item/InteractableItem.h"
#include "HealthPotionItem.generated.h"

// 진행 UI용 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChargeStart, float, Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChargeProgress, float, Percent); // 0~1
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChargeCancel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChargeFinish);

UCLASS()
class XV_API AHealthPotionItem : public AInteractableItem
{
	GENERATED_BODY()
public:
	AHealthPotionItem();

	// 아이템 인터페이스
	virtual void UseItem() override;

	// 사용 제어
	void StartUse();
	void StopUse();

	// 진행 조회 (BP에서 읽을 수 있게)
	UFUNCTION(BlueprintPure) float GetChargeTime() const;
	UFUNCTION(BlueprintPure) float GetChargeRemainTime() const;
	UFUNCTION(BlueprintPure) float GetChargeCurrentTime() const;
	UFUNCTION(BlueprintPure) float GetChargePercent() const;
	UFUNCTION(BlueprintPure) bool  IsUsing() const { return bIsUsing; }

	// 진행 이벤트 (HUD/3D 위젯에서 바인딩)
	UPROPERTY(BlueprintAssignable, Category="Potion|Events")
	FOnChargeStart OnChargeStart;
	UPROPERTY(BlueprintAssignable, Category="Potion|Events")
	FOnChargeProgress OnChargeProgress;
	UPROPERTY(BlueprintAssignable, Category="Potion|Events")
	FOnChargeCancel OnChargeCancel;
	UPROPERTY(BlueprintAssignable, Category="Potion|Events")
	FOnChargeFinish OnChargeFinish;

protected:
	UFUNCTION() void FinishUse();              // 회복 실행(완료)
	virtual void Interact() override;         // 필요시 상호작용 확장

	// 설정값
	UPROPERTY(EditDefaultsOnly, Category="Potion")
	float ChargeTime = 3.0f;                 // 꾹 누를 시간
	UPROPERTY(EditDefaultsOnly, Category="Potion")
	float HealAmount = 30.f;                 // 회복량
	UPROPERTY(EditDefaultsOnly, Category="Potion")
	float ProgressTickInterval = 0.02f;      // 진행률 틱(50fps)

	// 타이머
	FTimerHandle HealTimerHandle;            // 완료 타이머
	FTimerHandle ProgressTickHandle;         // 진행률 틱

private:
	bool bIsUsing = false;

	UFUNCTION() void TickProgress();
};
