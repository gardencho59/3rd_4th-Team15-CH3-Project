// UIFollowerComponent.cpp

#include "UIFollowerComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

UUIFollowerComponent::UUIFollowerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UUIFollowerComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!TargetWidget)
    {
        TargetWidget = GetOwner()->FindComponentByClass<UWidgetComponent>();
        if (TargetWidget)
        {
            UE_LOG(LogTemp, Warning, TEXT("UIFollowerComponent: TargetWidget assigned automatically"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("UIFollowerComponent: No WidgetComponent found on owner!"));
        }
    }
}

void UUIFollowerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateWidgetLocation();
}

void UUIFollowerComponent::UpdateWidgetLocation()
{
    if (!TargetWidget) return;

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter) return;

    USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
    if (!Mesh) return;

    FVector StartLocation = Mesh->GetSocketLocation("head");
    FVector ForwardVector = OwnerCharacter->GetActorForwardVector();
    FVector EndLocation = StartLocation + ForwardVector * TraceDistance;

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwnerCharacter);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_Visibility,
        QueryParams
    );

    if (bHit)
    {
        FVector HitLocation = HitResult.ImpactPoint;
        FVector HitNormal = HitResult.ImpactNormal;

        FVector WallRight = FVector::CrossProduct(HitNormal, FVector::UpVector).GetSafeNormal();
        FVector ViewDirection = OwnerCharacter->GetControlRotation().Vector();

        float Dot = FVector::DotProduct(ViewDirection, WallRight);

        const float LeftSideDistance = 200.f;
        const float RightSideDistance = 0.f;

        FVector SideOffset = (Dot > 0.f) 
            ? (WallRight * RightSideDistance) 
            : (-WallRight * LeftSideDistance);

        FVector WidgetLocation = HitLocation + HitNormal * 10.f + SideOffset;
        FRotator WidgetRotation = HitNormal.ToOrientationRotator();

        TargetWidget->SetWorldLocationAndRotation(WidgetLocation, WidgetRotation);
    }
    else
    {
        FVector DefaultLocation = StartLocation + ForwardVector * TraceDistance + DefaultOffset - ForwardVector * 50.f;
        TargetWidget->SetWorldLocation(DefaultLocation);

        FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(DefaultLocation, StartLocation);
        LookAtRotation.Yaw += 20.0f;

        TargetWidget->SetWorldRotation(LookAtRotation);
    }

#if WITH_EDITOR
    DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 0.1f, 0, 1.0f);
#endif
}

