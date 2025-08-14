#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "XVGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyCountChanged, int32, RemainingEnemies);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionChanged, const FString&, MissionText);

class UUserWidget;

UCLASS()
class XV_API AXVGameState : public AGameState
{
	GENERATED_BODY()

public:
	AXVGameState();
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> GameUIClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules|Time")
	float TimeLimit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules|Spawn")
	TArray<int32> SpawnAllEnemyCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules|Spawn")
	int32 SpawnPatrolEnemyCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Rules|Wave")
	bool IsWaveTriggered;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Rules|Wave")
	bool CanActiveArrivalPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Rules|Wave")
	int32 SpawnedEnemyCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Rules|Wave")
	int32 KilledEnemyCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Rules|Mission")
	int32 CurrentMissionIdx;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnEnemyCountChanged OnEnemyCountChanged;
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnMissionChanged OnMissionChanged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
	TArray<FString> MissionTexts1 =
		{
			TEXT("Mission 1: Interact Button to Open Door"),
			TEXT("Mission 2: Kill All Enemies"),
			TEXT("Mission 3: Hack a Computer to Activate Interactive Button"),
			TEXT("Mission 4: Interact Button to Open Door"),
			TEXT("Mission 5: Kill All Enemies"),
			TEXT("Mission 6: Access System to Open Door"),
		};

	void BroadcastMission()
	{
		CurrentMissionIdx++;
		UE_LOG(LogTemp, Warning, TEXT("CurrentMissionIdx: %d"), CurrentMissionIdx);
		if (MissionTexts1[CurrentMissionIdx].IsValidIndex(CurrentMissionIdx))
		{
			OnMissionChanged.Broadcast(MissionTexts1[CurrentMissionIdx]);
		}
		
	}
	
	void EnemyKilled() const
	{
		OnEnemyCountChanged.Broadcast(SpawnedEnemyCount - KilledEnemyCount);
	}
	
	UFUNCTION(BlueprintCallable, Category="Game Rules")
	int32 GetRemainingEnemies() const
	{
		return SpawnedEnemyCount - KilledEnemyCount;
	}
};
