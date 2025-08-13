#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogXVGameMode, Log, All);

#include "GameFramework/GameMode.h"
#include "XVBaseGameMode.generated.h"

class ASpawnVolume;

UCLASS()
class XV_API AXVBaseGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	virtual void StartGame();
	virtual void FindSpawnVolume();
	virtual void SpawnEnemies();
	virtual void OnEnemyKilled();
	virtual void OnWaveTriggered();
	virtual void EndGame(bool bIsClear);

	UPROPERTY()
	TArray<ASpawnVolume*> ActiveSpawnVolumes;
};
