#include "Item/HealthPotionItem.h"
#include "Character/XVCharacter.h"
#include "Kismet/GameplayStatics.h"

AHealthPotionItem::AHealthPotionItem()
{
	ChargeTime = 3.0f;
	HealAmount = 30.f;
	bIsUsing = false;
}

void AHealthPotionItem::StartUse()
{
	//UE_LOG(LogTemp, Warning, TEXT("charge Time : %.2f / %.2f") , GetChargeCurrentTime(), GetChargeTime());
	if (bIsUsing) return;
	UE_LOG(LogTemp, Warning, TEXT("StartUse called"));
	bIsUsing = true;
	
	GetWorld()->GetTimerManager().SetTimer(HealTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		FinishUse();
	}), ChargeTime, false);
	/*// ChargeTime 후 FinishUse 호출
	GetWorld()->GetTimerManager().SetTimer(
		HealTimerHandle,
		this,
		&AHealthPotionItem::Test,
		1.0f,
		false
	);*/
}

void AHealthPotionItem::StopUse()
{
	// 키를 뗐으면 타이머 취소
	UE_LOG(LogTemp, Warning, TEXT("StopUse called, cancelling timer"));
	GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
	bIsUsing = false;
}

void AHealthPotionItem::FinishUse()
{
	UE_LOG(LogTemp, Warning, TEXT("Finish Use"));
	if (!XVCharacter)
	{
		XVCharacter = Cast<AXVCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}	
	if (XVCharacter)
	{
		// 캐릭터 체력 회복
		XVCharacter->AddHealth(HealAmount);
		bIsUsing = false;
		
		// 아이템 삭제
		XVCharacter->SetCurrentItem(nullptr);
		Destroy();
	}
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
	float Remaining = GetWorld()->GetTimerManager().GetTimerRemaining(HealTimerHandle);
	return Remaining;
}

float AHealthPotionItem::GetChargeCurrentTime() const
{
	float CurrentTime = ChargeTime- GetWorld()->GetTimerManager().GetTimerRemaining(HealTimerHandle);
	return CurrentTime;
}

void AHealthPotionItem::Interact()
{
	Super::Interact();
}