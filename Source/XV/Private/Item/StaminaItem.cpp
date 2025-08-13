#include "Item/StaminaItem.h"
#include "Character/XVCharacter.h"
#include "Kismet/GameplayStatics.h"

AStaminaItem::AStaminaItem()
{
	Speed = 100.f;
	StaminaTime = 7.f;
}

void AStaminaItem::UseItem()
{
	Super::UseItem();

	if (AXVCharacter* XVCharacter = Cast<AXVCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{ // 속도 증가
		XVCharacter->SetSpeed(Speed);
		
		GetWorld()->GetTimerManager().SetTimer(
			StaminaTimerHandle,
			this,
			&AStaminaItem::FinishItem,
			StaminaTime,
			false
		);
	}
}

void AStaminaItem::FinishItem()
{ // 지속 시간 이후 원래 속도로 복구
	if (AXVCharacter* XVCharacter = Cast<AXVCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		XVCharacter->SetSpeed(-Speed);
	}
}
