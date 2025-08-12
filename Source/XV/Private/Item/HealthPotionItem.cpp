#include "Item/HealthPotionItem.h"
#include "Character/XVCharacter.h"
#include "Kismet/GameplayStatics.h"

AHealthPotionItem::AHealthPotionItem()
{
	ResetHealthPotion();
}
void AHealthPotionItem::ResetHealthPotion()
{
	TotalHealAmount = 30.f;					// 총 회복량
	HealDuration = 10.f;						// 회복 시간(초)
	HealPerTick = 0.f;						// 초당 회복량 (초기화 후 계산)
	RemainingHealAmount = TotalHealAmount;  // 처음에는 전량 남음
}

void AHealthPotionItem::BeginPlay()
{
	Super::BeginPlay();
	HealPerTick = TotalHealAmount / HealDuration;
}

void AHealthPotionItem::StartUse()
{
	if (RemainingHealAmount <= 0.f) return; // 이미 다 사용했으면 무시	
	if (!XVCharacter) return;

	// 1초마다 HealTick 실행
	GetWorld()->GetTimerManager().SetTimer(
		HealTimerHandle,
		this,
		&AHealthPotionItem::HealTick,
		2.0f,
		true
	);
}

void AHealthPotionItem::StopUse()
{
	GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);

	// 남은 양이 0 이하 → 파괴
	if (RemainingHealAmount <= 0.f)
	{
		XVCharacter->SetCurrentItem(nullptr);
		Destroy();
	}
}

void AHealthPotionItem::HealTick()
{
	if (!XVCharacter)
	{
		XVCharacter = Cast<AXVCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}
	float HealThisTick = FMath::Min(HealPerTick, RemainingHealAmount);
	
	XVCharacter->AddHealth(HealThisTick);
	RemainingHealAmount -= HealThisTick;
	UE_LOG(LogTemp, Warning, TEXT("Heal RemainingHealAmount: %f / %f"), RemainingHealAmount, TotalHealAmount);
	// 다 회복했으면 종료
	if (RemainingHealAmount <= 0.f)
	{
		StopUse();
	}
}

void AHealthPotionItem::UseItem()
{
	UE_LOG(LogTemp, Warning, TEXT("UseItem"));
	HealTick();
}

void AHealthPotionItem::Interact()
{
	Super::Interact();
}
