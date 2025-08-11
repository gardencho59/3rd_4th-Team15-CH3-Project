#include "UIFollowerComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

UUIFollowerComponent::UUIFollowerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UUIFollowerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!TargetWidget) TargetWidget = FindWorldSideUI();
	if (!Mesh)         Mesh = AutoFindMesh();

	if (TargetWidget)
	{
		TargetWidget->SetWidgetSpace(EWidgetSpace::World);
		TargetWidget->SetTwoSided(true);
		TargetWidget->SetTranslucentSortPriority(TranslucencySortPriority);
	}

	CurrentSide = PrevSide = DefaultSide;
	bSnapped = bPrevSnapped = false;
}

void UUIFollowerComponent::SetAimState(bool bNewIsAiming, ESideUI AimSide)
{
	const bool bAimChanged = (bIsAiming != bNewIsAiming);
	bIsAiming = bNewIsAiming;
	CurrentSide = bNewIsAiming ? AimSide : DefaultSide;
	if (bAimChanged)
	{
		OnAimStateChanged.Broadcast(bIsAiming); // ★ BP에서 이걸 받음
	}
}

UWidgetComponent* UUIFollowerComponent::FindWorldSideUI() const
{
	if (const AActor* Owner = GetOwner())
	{
		TArray<UActorComponent*> Comps; Owner->GetComponents(UWidgetComponent::StaticClass(), Comps);
		for (UActorComponent* C : Comps)
			if (C && C->GetFName() == FName("WorldSideUI"))
				return Cast<UWidgetComponent>(C);
		return Owner->FindComponentByClass<UWidgetComponent>();
	}
	return nullptr;
}

USkeletalMeshComponent* UUIFollowerComponent::AutoFindMesh() const
{
	if (const AActor* Owner = GetOwner())
	{
		if (USkeletalMeshComponent* S = Owner->FindComponentByClass<USkeletalMeshComponent>()) return S;
		TArray<USkeletalMeshComponent*> All; Owner->GetComponents(All);
		if (All.Num()) return All[0];
	}
	return nullptr;
}

FVector UUIFollowerComponent::GetBasePos() const
{
	// 자유 위치 기준은 메쉬 소켓을 사용(기존대로)
	if (Mesh)
	{
		if (SocketName != NAME_None && Mesh->DoesSocketExist(SocketName))
			return Mesh->GetSocketLocation(SocketName);
		return Mesh->GetComponentLocation();
	}
	if (const AActor* Owner = GetOwner())
		return Owner->GetActorLocation() + FVector(0,0,90.f);
	return FVector::ZeroVector;
}

void UUIFollowerComponent::GetCameraBasis(FVector& OutCamLoc, FRotator& OutCamRot, FVector& OutFwd, FVector& OutRight) const
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->GetPlayerViewPoint(OutCamLoc, OutCamRot);
		OutFwd   = OutCamRot.Vector();
		OutRight = FRotationMatrix(OutCamRot).GetUnitAxis(EAxis::Y);
	}
	else
	{
		OutCamLoc = GetBasePos(); OutCamRot = FRotator::ZeroRotator;
		OutFwd = FVector::ForwardVector; OutRight = FVector::RightVector;
	}
}

bool UUIFollowerComponent::SideSphereTraceFromCapsule(const FVector& SideDir, float& OutHitDist, FHitResult& OutHit) const
{
	// ★ 캐릭터 캡슐 중심 기준(=캐릭터를 기준으로)에서 측면으로 스윕
	const ACharacter* Char = Cast<ACharacter>(GetOwner());
	FVector Anchor;
	if (Char && Char->GetCapsuleComponent())
	{
		const UCapsuleComponent* Cap = Char->GetCapsuleComponent();
		Anchor = Cap->GetComponentLocation() + FVector(0,0,CapsuleAnchorZ);
	}
	else
	{
		Anchor = GetOwner() ? GetOwner()->GetActorLocation() + FVector(0,0,CapsuleAnchorZ) : FVector::ZeroVector;
	}

	const FVector Start = Anchor;
	const FVector End   = Start + SideDir * TraceMaxDistance;

	FCollisionQueryParams P(SCENE_QUERY_STAT(SideUITrace), false, GetOwner());
	P.bTraceComplex = bTraceComplex;

	const bool bHit = GetWorld()->SweepSingleByChannel(
		OutHit, Start, End, FQuat::Identity, TraceChannel,
		FCollisionShape::MakeSphere(TraceRadius), P);

	if (bHit && OutHit.bBlockingHit)
	{
		OutHitDist = (OutHit.ImpactPoint - Start).Size();
		if (bDebugDraw) DebugDraw(Start, End, &OutHit);
		return true;
	}
	if (bDebugDraw) DebugDraw(Start, End, nullptr);
	return false;
}

void UUIFollowerComponent::PushOutUntilVisible(FVector CamLoc, const FHitResult& Hit, float& InOutOffset, int32 MaxSteps) const
{
	// 카메라→UI 위치 시야가 막히면(벽에 살짝 파묻히는 경우) 법선 방향으로 조금씩 더 밀어낸다.
	FCollisionQueryParams P(SCENE_QUERY_STAT(SideUILoS), false, GetOwner());
	for (int i=0; i<MaxSteps; ++i)
	{
		const FVector TestLoc = Hit.ImpactPoint + Hit.ImpactNormal * (InOutOffset) + FVector(0,0,VerticalOffset);
		FHitResult LosHit;
		const bool bBlocked = GetWorld()->LineTraceSingleByChannel(LosHit, CamLoc, TestLoc, TraceChannel, P);
		if (!bBlocked) break;
		InOutOffset += 4.f; // 한 스텝 더 밀기
	}
}

void UUIFollowerComponent::DebugDraw(const FVector& Start, const FVector& End, const FHitResult* Hit) const
{
	DrawDebugLine(GetWorld(), Start, End, Hit ? FColor::Green : FColor::Red, false, 0.05f, 0, 1.5f);
	DrawDebugSphere(GetWorld(), End, TraceRadius, 12, FColor::Blue, false, 0.05f);
	if (Hit)
	{
		DrawDebugPoint(GetWorld(), Hit->ImpactPoint, 10.f, FColor::Yellow, false, 0.25f);
		DrawDebugLine(GetWorld(), Hit->ImpactPoint, Hit->ImpactPoint + Hit->ImpactNormal * 30.f, FColor::Cyan, false, 0.25f, 0, 2.f);
	}
}

void UUIFollowerComponent::TickComponent(float DeltaTime, ELevelTick, FActorComponentTickFunction*)
{
	if (!TargetWidget) return;

	// 카메라/기준
	const FVector BasePos = GetBasePos();
	FVector CamLoc; FRotator CamRot; FVector CamFwd; FVector CamRight;
	GetCameraBasis(CamLoc, CamRot, CamFwd, CamRight);

	const bool bUseLeft = (bIsAiming ? CurrentSide : DefaultSide) == ESideUI::Left;
	const FVector SideDir = bUseLeft ? -CamRight : CamRight;

	// 자유 위치(벽 스냅 전): 메쉬 소켓 기준
	const FVector FreeLoc = BasePos + SideDir*LateralOffset + CamFwd*ForwardOffset + FVector(0,0,VerticalOffset);

	FRotator FaceCam = UKismetMathLibrary::FindLookAtRotation(FreeLoc, CamLoc);
	FaceCam.Pitch = 0.f; FaceCam.Roll = 0.f;

	// ★ 벽 감지는 "캐릭터 캡슐 중심"에서 좌/우로
	float HitDist = BIG_NUMBER;
	FHitResult Hit;
	const bool bHasHit = SideSphereTraceFromCapsule(SideDir, HitDist, Hit);

	// 히스테리시스
	if (bHasHit)
	{
		if (!bSnapped && HitDist <= SnapEnterDistance) bSnapped = true;
		else if (bSnapped && HitDist >= SnapExitDistance) bSnapped = false;
	}
	else bSnapped = false;

	// 최종 위치/회전
	FVector DesiredLoc = FreeLoc;
	FRotator DesiredRot = FaceCam;

	if (bSnapped && bHasHit)
	{
		float Out = WallOffset + NoClipPadding;
		// 카메라 시야가 막히면 추가로 더 밀어냄(최대 3번)
		PushOutUntilVisible(CamLoc, Hit, Out, 3);

		DesiredLoc = Hit.ImpactPoint + Hit.ImpactNormal * Out + FVector(0,0,VerticalOffset);

		// 항상 카메라 Yaw만 바라봄(회전 뒤집힘 방지)
		DesiredRot = UKismetMathLibrary::FindLookAtRotation(DesiredLoc, CamLoc);
		DesiredRot.Pitch = 0.f; DesiredRot.Roll = 0.f;
	}

	// 사이드/스냅 변화 시 즉시 전환
	const bool bSideChanged = (PrevSide != (bIsAiming ? CurrentSide : DefaultSide));
	const bool bSnapChanged = (bSnapped != bPrevSnapped);
	const bool bInstant = bInstantSwitch && (bSideChanged || bSnapChanged);

	if (bInstant)
	{
		TargetWidget->SetWorldLocation(DesiredLoc);
		TargetWidget->SetWorldRotation(DesiredRot);
	}
	else
	{
		// 혹시 부드럽게 쓰고 싶을 때를 위한 최소 보간
		const float LocSpd = 14.f, RotSpd = 14.f;
		TargetWidget->SetWorldLocation(FMath::VInterpTo(TargetWidget->GetComponentLocation(), DesiredLoc, DeltaTime, LocSpd));
		TargetWidget->SetWorldRotation(FMath::RInterpTo(TargetWidget->GetComponentRotation(), DesiredRot, DeltaTime, RotSpd));
	}

	PrevSide = (bIsAiming ? CurrentSide : DefaultSide);
	bPrevSnapped = bSnapped;
}
