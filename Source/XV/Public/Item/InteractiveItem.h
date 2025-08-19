#pragma once

#include "CoreMinimal.h"
#include "ItemInterface.h"
#include "GameFramework/Actor.h"
#include "InteractiveItem.generated.h"

class AXVCharacter;

UCLASS()
class XV_API AInteractiveItem : public AActor, public IItemInterface
{
	GENERATED_BODY()

public:
	AInteractiveItem();

	virtual void UseItem();
	virtual float GetItemTime();
	virtual float GetItemRemainTime();

	void EnablePhysics();
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|UI")
	class UWidgetComponent* InteractionWidget;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> InteractionWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Item")
	float ItemUseTime; // 아이템 지속 시간
	UPROPERTY(EditAnywhere, Category = "Item")
	float ItemRemainTime; // 아이템 지속 시간 남은 시간

	virtual void Interact() override;
};
