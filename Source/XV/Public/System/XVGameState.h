#pragma once

#include "CoreMinimal.h"
#include "XVGameInstance.h"
#include "GameFramework/GameState.h"
#include "XVGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyCountChanged, int32, TotalKilledEnemyCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMissionChanged, const FString&, MissionTitle, const FString&, MissionDescription);

class UUserWidget;

USTRUCT(BlueprintType)
struct FMissionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	FString MissionTitle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	FString MissionDescription;
};
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
	TArray<FMissionData> Missions =
		{
			{ TEXT("Mission 1"), TEXT("Interact Button to Open Door") },
			{ TEXT("Mission 2"), TEXT("Kill All Enemies") },
			{ TEXT("Mission 3"), TEXT("Hack a Computer to Activate Interactive Button") },
			{ TEXT("Mission 4"), TEXT("Interact Button to Open Door") },
			{ TEXT("Mission 5"), TEXT("Kill All Enemies") },
			{ TEXT("Mission 6"), TEXT("Access System to Open Door") },
			{ TEXT("Mission 7"), TEXT("Kill All Enemies & Hold on Until the Door Opens") },
			{ TEXT("Mission 8"), TEXT("BOSS") },
		};

	void BroadcastMission()
	{
		CurrentMissionIdx++;
		if (CurrentMissionIdx < Missions.Num())
		{
			if (Missions.IsValidIndex(CurrentMissionIdx))
			{
				OnMissionChanged.Broadcast(Missions[CurrentMissionIdx].MissionTitle, Missions[CurrentMissionIdx].MissionDescription);
			}
		}
	}
	
	void EnemyKilled() const
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
			{
				XVGI->TotalKilledEnemyCount++;
				OnEnemyCountChanged.Broadcast(GetTotalEnemyKilledCount());
			}
		}
	}
	
	UFUNCTION(BlueprintCallable, Category="Game Rules")
	int32 GetTotalEnemyKilledCount() const
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
			{
				return XVGI->TotalKilledEnemyCount;
			}
		}
		return 0;
	}
};
