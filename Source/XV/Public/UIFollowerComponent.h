#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UIFollowerComponent.generated.h"

class UWidgetComponent;
class USkeletalMeshComponent;

UENUM(BlueprintType)
enum class ESideUI : uint8 { Left, Right };
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimStateChanged, bool, bIsAiming);

UCLASS(ClassGroup=(UI), meta=(BlueprintSpawnableComponent))
class XV_API UUIFollowerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category="UI")
	FAimStateChanged OnAimStateChanged;
	
	UUIFollowerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ===== Target =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Target")
	UWidgetComponent* TargetWidget = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Target")
	USkeletalMeshComponent* Mesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Target")
	FName SocketName = FName("spine_03"); // 자유

	// ===== Placement (free) =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Offset")
	float ForwardOffset = 35.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Offset")
	float LateralOffset = 70.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Offset")
	float VerticalOffset = 10.f;

	// ===== Wall snap (캐릭터 캡슐 중심 기준) =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wall")
	float TraceMaxDistance = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wall")
	float TraceRadius = 12.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wall")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wall")
	bool bTraceComplex = true;

	// “붙기/떨어지기” 임계값 분리(히스테리시스)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wall")
	float SnapEnterDistance = 140.f;   // 이 이하이면 붙기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wall")
	float SnapExitDistance  = 170.f;   // 이 이상이면 떨어짐

	// 벽에서 띄우는 기본 간격 + 추가 패딩(뚫림 방지)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wall")
	float WallOffset = 7.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wall")
	float NoClipPadding = 6.f;         // 카메라 시야 체크 후 부족하면 자동으로 더 밀어냄(코드에서 추가)

	// 캡슐 중심의 높이 오프셋(어깨 높이로 맞추고 싶을 때)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wall")
	float CapsuleAnchorZ = 40.f;

	// 즉시 전환(텔레포트)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behavior")
	bool bInstantSwitch = true;

	// 항상 카메라 Yaw만 바라보기(벽에서도 회전 뒤집힘 방지)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behavior")
	bool bFaceCameraYawOnly = true;

	// 비조준 기본 사이드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behavior")
	ESideUI DefaultSide = ESideUI::Right;

	// 깊이로 가려지는 상황 최소화(렌더 우선순위)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Render")
	int32 TranslucencySortPriority = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	bool bDebugDraw = false;

	UFUNCTION(BlueprintCallable, Category="UI")
	void SetAimState(bool bNewIsAiming, ESideUI AimSide);

private:
	bool bIsAiming = false;
	ESideUI CurrentSide = ESideUI::Right;
	ESideUI PrevSide    = ESideUI::Right;
	bool bSnapped = false, bPrevSnapped = false;

	// helpers
	UWidgetComponent* FindWorldSideUI() const;
	USkeletalMeshComponent* AutoFindMesh() const;
	FVector GetBasePos() const;
	void    GetCameraBasis(FVector& OutCamLoc, FRotator& OutCamRot, FVector& OutFwd, FVector& OutRight) const;

	// ★ 핵심: 캡슐 중심(캐릭터 기준)에서 좌/우로 스윕
	bool SideSphereTraceFromCapsule(const FVector& SideDir, float& OutHitDist, FHitResult& OutHit) const;

	// 벽에 붙을 때 카메라 시야로 가려지면 추가로 더 밀어내기
	void PushOutUntilVisible(FVector CamLoc, const FHitResult& Hit, float& InOutOffset, int32 MaxSteps = 3) const;

	void DebugDraw(const FVector& Start, const FVector& End, const FHitResult* Hit) const;
};
