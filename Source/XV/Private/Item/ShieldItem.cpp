#include "Item/ShieldItem.h"
#include "Character/XVCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Inventory/Component/ItemDataComponent.h"

AShieldItem::AShieldItem()
{
	Shield = 100.0f;
	ItemUseTime = 30.0f;
	ItemRemainTime = ItemUseTime;
}

void AShieldItem::UseItem()
{
	Super::UseItem();

	if (AXVCharacter* XVCharacter = Cast<AXVCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{ // 최대 체력 증가
		if (UInventoryComponent* InventoryComp = XVCharacter->GetInventoryComp())
		{
			InventoryComp->UseItem("ShieldPotion", 1);// 인벤토리 수량 감소
			UE_LOG(LogTemp, Warning, TEXT("Use Shield"));
		}
		XVCharacter->ShieldItem(Shield, ItemUseTime);
	}
	Destroy();
}

void AShieldItem::FinishItem()
{ 

}
