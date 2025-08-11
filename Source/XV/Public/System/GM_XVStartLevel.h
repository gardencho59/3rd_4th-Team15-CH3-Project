#pragma once

#include "CoreMinimal.h"
#include "XVBaseGameMode.h"
#include "GM_XVStartLevel.generated.h"

UCLASS()
class XV_API AGM_XVStartLevel : public AXVBaseGameMode
{
	GENERATED_BODY()

public:
	AGM_XVStartLevel();
	
	virtual void BeginPlay() override;
	
	virtual void StartGame()override;
	virtual void SpawnEnemies() override;
	virtual void OnEnemyKilled() override;
	virtual void OnWaveTriggered() override;
	void DelayedWaveTrigger();
	virtual void EndGame(bool bIsClear) override;

	bool IsOutdoor;
	bool bWavePending;
	float Delay;

	FTimerHandle Delayer;
	FTimerHandle WaveTriggerDelayHandle;
};
