#include "Item/ShieldItem.h"
#include "Character/XVCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Inventory/Component/ItemDataComponent.h"

AShieldItem::AShieldItem()
{
	Shield = 100.f;
	ItemUseTime = 30;
	ItemRemainTime = ItemUseTime;
}

void AShieldItem::UseItem()
{
	Super::UseItem();

	if (AXVCharacter* XVCharacter = Cast<AXVCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{ // 최대 체력 증가
		if (UInventoryComponent* InventoryComp = XVCharacter->GetInventoryComp())
		{
			InventoryComp->UseItem("ShieldItem", 1); // 인벤토리 수량 감소
			UE_LOG(LogTemp, Warning, TEXT("RowName : %s"), *ItemDataComp->GetRowName().ToString());
		}
		
		XVCharacter->SetMaxHealth(XVCharacter->GetMaxHealth() + Shield);
		XVCharacter->AddHealth(Shield);
		
		GetWorld()->GetTimerManager().SetTimer(
			ShieldTimerHandle,
			this,
			&AShieldItem::FinishItem,
			ItemUseTime,
			false
		);
	}
}

void AShieldItem::FinishItem()
{ // 지속 시간 이후 원래 체력으로 복구
	if (AXVCharacter* XVCharacter = Cast<AXVCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Finish Shield"));
		XVCharacter->SetMaxHealth(XVCharacter->GetMaxHealth()-Shield);
	}
}

float AShieldItem::GetItemTime()
{
	return ItemUseTime;
}

float AShieldItem::GetItemRemainTime()
{
	ItemRemainTime = GetWorld()->GetTimerManager().GetTimerRemaining(ShieldTimerHandle);
	return Super::GetItemRemainTime();
}
