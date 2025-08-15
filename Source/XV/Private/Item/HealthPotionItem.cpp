// Item/HealthPotionItem.cpp
#include "Item/HealthPotionItem.h"
#include "Character/XVCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Inventory/Component/ItemDataComponent.h"

AHealthPotionItem::AHealthPotionItem()
{
	ChargeTime = 2.0f;
	HealAmount = 30.f;
	bIsUsing = false;
}

	void AHealthPotionItem::StartUse()
{
	if (bIsUsing || !GetWorld()) return;

	bIsUsing = true;
	OnChargeStart.Broadcast(ChargeTime);
	OnChargeProgress.Broadcast(0.f);  // ★ 시작하자마자 0% 보장

	GetWorld()->GetTimerManager().SetTimer(
		ProgressTickHandle, this, &AHealthPotionItem::TickProgress,
		ProgressTickInterval, true
	);

	GetWorld()->GetTimerManager().SetTimer(
		HealTimerHandle, this, &AHealthPotionItem::FinishUse,
		ChargeTime, false
	);
}

void AHealthPotionItem::StopUse()
{
	if (!GetWorld()) return;

	// 취소
	GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(ProgressTickHandle);
	bIsUsing = false;
	OnChargeCancel.Broadcast();
}

void AHealthPotionItem::TickProgress()
{
	OnChargeProgress.Broadcast(GetChargePercent());
}

void AHealthPotionItem::FinishUse()
{
	if (!GetWorld()) return;

	// 진행률 마감
	GetWorld()->GetTimerManager().ClearTimer(ProgressTickHandle);
	OnChargeProgress.Broadcast(1.f);
	OnChargeFinish.Broadcast();

	// 회복/소모 처리
	if (AXVCharacter* XVCharacter = Cast<AXVCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (UInventoryComponent* InventoryComp = XVCharacter->GetInventoryComp())
		{
			InventoryComp->UseItem("HealthPotion", 1); // 인벤토리 수량 감소
			UE_LOG(LogTemp, Warning, TEXT("RowName : %s"), *ItemDataComp->GetRowName().ToString());
		}

		XVCharacter->AddHealth(HealAmount);
		
		XVCharacter->ConsumeHealthPotion();          // 인벤 개수 1 감소 + 델리게이트 방송
		XVCharacter->SetCurrentItem(nullptr);        // 손에서 내려놓기(선택)
	}

	bIsUsing = false;
	Destroy();
}

void AHealthPotionItem::UseItem()
{
	StartUse();
}

float AHealthPotionItem::GetChargeTime() const
{
	return ChargeTime;
}

float AHealthPotionItem::GetChargeRemainTime() const
{
	if (!GetWorld()) return ChargeTime;

	const FTimerManager& TM = GetWorld()->GetTimerManager();
	// 타이머가 꺼져 있으면 "아직 전혀 진행 안 됨"으로 해석
	if (!TM.IsTimerActive(HealTimerHandle))
		return ChargeTime;

	return TM.GetTimerRemaining(HealTimerHandle);
}

float AHealthPotionItem::GetChargeCurrentTime() const
{
	if (!GetWorld()) return 0.f;

	const FTimerManager& TM = GetWorld()->GetTimerManager();
	// 타이머가 꺼져 있으면 진행시간 0
	if (!TM.IsTimerActive(HealTimerHandle))
		return 0.f;

	const float Remaining = TM.GetTimerRemaining(HealTimerHandle);
	return FMath::Clamp(ChargeTime - Remaining, 0.f, ChargeTime);
}

float AHealthPotionItem::GetChargePercent() const
{
	// 사용 중이 아니면 0%, 사용 중이면 현재/총시간
	if (!bIsUsing || ChargeTime <= 0.f) return 0.f;
	return FMath::Clamp(GetChargeCurrentTime() / ChargeTime, 0.f, 1.f);
}

void AHealthPotionItem::Interact()
{
	Super::Interact();
	// 필요 시 상호작용 확장
}
