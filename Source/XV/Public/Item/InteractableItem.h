#pragma once

#include "CoreMinimal.h"
#include "ItemInterface.h"
#include "GameFramework/Actor.h"
#include "InteractableItem.generated.h"

class AXVCharacter;

UCLASS()
class XV_API AInteractableItem : public AActor, public IItemInterface
{
	GENERATED_BODY()

public:
	AInteractableItem();

	virtual void UseItem();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	class USphereComponent* Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UItemDataComponent* ItemDataComp;

	virtual void Interact() override;
};
