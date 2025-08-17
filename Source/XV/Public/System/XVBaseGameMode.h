#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "XVBaseGameMode.generated.h"

class ASpawnVolume;

UCLASS()
class XV_API AXVBaseGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AXVBaseGameMode();
	
	virtual void BeginPlay() override;
	
	virtual void StartGame();
	virtual void FindSpawnVolume();
	virtual void SpawnEnemies();
	virtual void OnEnemyKilled();
	virtual void OnWaveTriggered();
	virtual void RespawnPlayer(AController* Controller, float RespawnDelay = 2.0f);
	virtual void EndGame(bool bIsClear);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level")
	int32 MaxLevel;

	UPROPERTY()
	TArray<ASpawnVolume*> ActiveSpawnVolumes;
};
