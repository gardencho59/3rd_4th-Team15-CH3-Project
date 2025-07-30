#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StarterActor.generated.h"

class UBoxComponent;
UCLASS()
class XV_API AStarterActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AStarterActor();		
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;

	UFUNCTION()
	virtual void OnStarterBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIdx,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	virtual void ActivateStarter(AActor* Activator);
};
