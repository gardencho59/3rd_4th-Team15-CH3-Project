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

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float RoateSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Deceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float BrakingFriction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool ControllerDesiredRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool OrientRotationToMovement;
};
