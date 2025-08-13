#pragma once

#include "CoreMinimal.h"
#include "XVBaseGameMode.h"
#include "XVGameMode.generated.h"

UCLASS()
class XV_API AXVGameMode : public AXVBaseGameMode
{
	GENERATED_BODY()

public:
	AXVGameMode();

	virtual void BeginPlay() override;

	virtual void StartGame() override;
	virtual void FindSpawnVolume() override;
	virtual void SpawnEnemies() override;
	virtual void OnEnemyKilled() override;
	virtual void OnWaveTriggered() override;
	virtual void EndGame(bool bIsClear) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level")
	int32 MaxLevel;
	
};
