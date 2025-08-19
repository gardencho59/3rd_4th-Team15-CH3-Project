#pragma once

#include "CoreMinimal.h"
#include "XVBaseGameMode.h"
#include "XVGameMode.generated.h"

UCLASS()
class XV_API AXVGameMode : public AXVBaseGameMode
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void StartGame() override;
	virtual void FindSpawnVolume() override;
	virtual void SpawnEnemies() override;
	virtual void OnEnemyKilled() override;
	virtual void OnWaveTriggered() override;
	virtual void EndGame(bool bIsClear) override;

	UFUNCTION(BlueprintImplementableEvent, Category="Game")
	void GameClear();
};
