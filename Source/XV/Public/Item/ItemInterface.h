#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

UINTERFACE(MinimalAPI)
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};

class XV_API IItemInterface
{
	GENERATED_BODY()
	
public:
	
	virtual void Interact() = 0;

};
