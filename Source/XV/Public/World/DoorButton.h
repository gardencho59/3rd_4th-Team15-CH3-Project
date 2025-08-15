#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorButton.generated.h"

UCLASS()
class XV_API ADoorButton : public AActor
{
	GENERATED_BODY()
	
public:
	ADoorButton();
	
	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* NewMaterial;
	
	UPROPERTY(EditAnywhere, Category = "Material")
	AActor* TargetActor;
	
	UFUNCTION(BlueprintCallable, Category = "Lock")
	void Unlock();

private:
	void ChangeMaterial();
};
