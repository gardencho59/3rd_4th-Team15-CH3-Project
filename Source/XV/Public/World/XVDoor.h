#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XVDoor.generated.h"

class UBoxComponent;

UCLASS()
class XV_API AXVDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	AXVDoor();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SceneRoot")
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	UBoxComponent* DoorBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Door")
	UBoxComponent* ExtraBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh|Door", Instanced, meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* Door;

	FVector ClosedPos;
	FVector OpenOffset;
	FVector TargetPos;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door")
	bool IsOpening;
	float OpenTime;
	
	FTimerHandle DoorOpenTimerInWave;
	
	void OpenDoor(UBoxComponent* TriggerBox);
	void SetDoorOpening();
};