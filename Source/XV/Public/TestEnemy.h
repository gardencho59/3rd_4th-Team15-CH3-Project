#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestEnemy.generated.h"

class UBoxComponent;
class USkeletalMeshComponent;

UCLASS()
class XV_API ATestEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestEnemy();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName EnemyType;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* SkeletalMeshComponent;

	UFUNCTION()
	virtual void OnEnemyBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIdx,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	virtual void ActivateEnemy(AActor* Activator);
	virtual FName GetEnemyType() const;
};
