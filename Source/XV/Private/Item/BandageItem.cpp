#include "Item/BandageItem.h"
#include "Character/XVCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory/Component/InventoryComponent.h"

ABandageItem::ABandageItem()
{
	HealAmount = 15.f;
}

void ABandageItem::UseItem()
{
	Super::UseItem();

	if (AXVCharacter* XVCharacter = Cast<AXVCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{ // 최대 체력 증가
		if (UInventoryComponent* InventoryComp = XVCharacter->GetInventoryComp())
		{
			InventoryComp->UseItem("BandagePotion", 1);// 인벤토리 수량 감소
			UE_LOG(LogTemp, Warning, TEXT("Use Bandage"));
		}
		XVCharacter->AddHealth(HealAmount);
	}
	Destroy();
}
