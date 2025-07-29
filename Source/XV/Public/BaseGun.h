// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunInterface.h"
#include "GameFramework/Actor.h"
#include "BaseGun.generated.h"

UCLASS()
class XV_API ABaseGun : public AActor, public IGunInterface
{
	GENERATED_BODY()
	
public:	
	ABaseGun();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnGunOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
	UFUNCTION()
	virtual void OnGunEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	virtual void FireBullet() override;
	virtual FName GetGunType() const override;

};
