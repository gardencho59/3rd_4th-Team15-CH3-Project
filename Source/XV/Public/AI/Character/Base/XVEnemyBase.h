#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "XVEnemyBase.generated.h"


class UAIConfigComponent;
class UXVDataAssetBase;

UCLASS()
class XV_API AXVEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	AXVEnemyBase();
protected:
	virtual void BeginPlay() override;
	
};
