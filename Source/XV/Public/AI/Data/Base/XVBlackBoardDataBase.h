#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardData.h"
#include "AI/Data/Struct/XVAiDataStruct.h" // 추가됨
#include "XVBlackBoardDataBase.generated.h"

/**
 * 
 */
UCLASS()
class XV_API UXVBlackBoardDataBase : public UBlackboardData
{
	GENERATED_BODY()
public:
	UXVBlackBoardDataBase();
	
	UPROPERTY(EditAnywhere)
	FaiData_StatusStruct StatusData; // 속도 및 체력 설정

	UPROPERTY(EditAnywhere)
	FaiData_SightConfigStruct SightConfigData; // 눈 설정

	UPROPERTY(EditAnywhere)
	FaiData_HearingConfigStruct HearingConfigData;// 귀 설정

	UPROPERTY(EditAnywhere)
	FaiData_GeneralConfigStruct GeneralConfigData; // 일반 설정

	UPROPERTY(EditAnywhere)
	FaiData_ForBlackBoardStruct BlackBoardData; // 블랙보드용 데이터
};
