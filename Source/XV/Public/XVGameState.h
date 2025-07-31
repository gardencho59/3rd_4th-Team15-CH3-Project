#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "XVGameState.generated.h"

UCLASS()
class XV_API AXVGameState : public AGameState
{
	GENERATED_BODY()

public:
	AXVGameState();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules")
	float TimeLimit;
	
	int32 SpawnedEnemyCount;
	int32 KilledEnemyCount;
};
