#include "Character/XVCharacter.h"
#include "Character/XVPlayerController.h"
#include "Character/XVPlayerAnimInstance.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "BaseGun.h"
#include "World/ElevatorDoor.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AISense_Hearing.h" // AI 총소리 듣기 용입니다.
#include "System/XVBaseGameMode.h"

AXVCharacter::AXVCharacter()
{
	PrimaryActorTick.bCanEverTick = false;		
	
	DefaultCameraLength = 250.0f;
	ZoomCameraLength = 100.0f;
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = DefaultCameraLength;  
	SpringArmComp->bUsePawnControlRotation = true;  
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	NormalSpeed = 300.0f;
	SprintSpeed = 500.0f;
	SitSpeed = 250.0f;

	bIsRun = false;
	bIsSit = false;
	bIsAim = false;
	bIsZooming = false;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	// 메인 무기 == Rifle or Shotgun
	PrimaryWeaponOffset = CreateDefaultSubobject<USceneComponent>(TEXT("PrimaryWeaponOffset"));
	PrimaryWeaponOffset->SetupAttachment(GetMesh(), TEXT("Rifle_Unequipped"));

	PrimaryWeapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("PrimaryWeapon"));
	PrimaryWeapon->SetupAttachment(PrimaryWeaponOffset);
	PrimaryWeapon->SetChildActorClass(BPPrimaryWeapon);

	// 서브 무기 == Pistol
	SubWeaponOffset = CreateDefaultSubobject<USceneComponent>(TEXT("SubWeaponOffset"));
	SubWeaponOffset->SetupAttachment(GetMesh(), TEXT("Pistol_Unequipped"));

	SubWeapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("SubWeapon"));
	SubWeapon->SetupAttachment(SubWeaponOffset);
	SubWeapon->SetChildActorClass(BPSubWeapon);
	
	//SetWeapon(EWeaponType::Pistol); // 시작할 때 들고 있는 무기 = Pistol
	MainWeaponType = EWeaponType::Rifle; // 주 무기 초기화
	SubWeaponType = EWeaponType::Pistol; // 보조 무기 초기화
	
	// 체력 세팅
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	//오버랩 이벤트
	Elevator = nullptr;
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AXVCharacter::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AXVCharacter::OnEndOverlap);
}

void AXVCharacter::SetHealth(float Value)
{
	CurrentHealth = FMath::Clamp( Value, 0.0f, MaxHealth);
}

float AXVCharacter::GetHealth() const
{
	return CurrentHealth;
}

void AXVCharacter::AddHealth(float Value)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + Value, 0.0f, MaxHealth);
}

void AXVCharacter::AddDamage(float Value)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Value, 0.0f, MaxHealth);
	FString Str = FString::Printf(TEXT("%f Damaged, Now Health : %f"), Value, CurrentHealth);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Str);
	// 피격 애니메이션 추가
	
	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
}

void AXVCharacter::Die()
{
	AXVBaseGameMode* XVBaseGameMode = Cast<AXVBaseGameMode>(GetWorld()->GetAuthGameMode());
	auto Anim = Cast<UXVPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (Anim)
	{
		Anim->PlayDieAnim();
	}
	XVBaseGameMode->EndGame(false);
}

void AXVCharacter::SetWeapon(EWeaponType Weapon)
{ // 일단 타입마다 필요한게 있을 까 싶어 나눴는데 추가 기능 없으면 간략하게 변경해도 될듯
	if (CurrentWeaponType == Weapon) // 현재 무기와 동일한 무기가 매개변수로 들어온 경우 => 장착 해제
	{
		CurrentWeaponType = EWeaponType::None;
		CurrentWeaponActor = nullptr;
	} 
	else
	{
		CurrentWeaponType = Weapon;
	}

	FString WeaponTypeName = StaticEnum<EWeaponType>()->GetNameStringByValue((int64)CurrentWeaponType);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, WeaponTypeName);

	auto Anim = Cast<UXVPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	
	switch (CurrentWeaponType)
	{
	case EWeaponType::Pistol:
		// 무기 교체 애니메이션 및 사운드 재생 필요
		SubWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Pistol_Equipped"));
		PrimaryWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Rifle_Unequipped"));

		CurrentWeaponActor = Cast<ABaseGun>(SubWeapon->GetChildActor());
		Anim->PlayGunChangeAnim();
		break;
		
	case EWeaponType::Rifle:
		// 무기 교체 애니메이션 및 사운드 재생 필요
		SubWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Pistol_Unequipped"));
		PrimaryWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Rifle_Equipped"));

		CurrentWeaponActor = Cast<ABaseGun>(PrimaryWeapon->GetChildActor());
		Anim->PlayGunChangeAnim();
		break;

	case EWeaponType::ShotGun:
		// 무기 교체 애니메이션 및 사운드 재생 필요
		SubWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Pistol_Unequipped"));
		PrimaryWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Rifle_Equipped"));
		break;
	
	default:
		SubWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Pistol_Unequipped"));
		PrimaryWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Rifle_Unequipped"));

		CurrentWeaponActor = Cast<ABaseGun>(SubWeapon->GetChildActor());
		Anim->PlayGunChangeAnim();
		break;
	}
}

EWeaponType AXVCharacter::GetWeapon() const
{
	return CurrentWeaponType;
}

bool AXVCharacter::GetISRun() const
{
	return bIsRun;
}

bool AXVCharacter::GetIsSit() const
{
	return bIsSit;
}

bool AXVCharacter::GetIsAim() const
{
	return bIsAim;
}

float AXVCharacter::GetTurnRate() const
{
	return TurnRate;
}

void AXVCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		UE_LOG(LogTemp, Log, TEXT("Overlap Actor: %s"), *OtherActor->GetName());
		// 엘리베이터 클래스인지 확인
		AElevatorDoor* OverlapElevator = Cast<AElevatorDoor>(OtherActor);
		if (OverlapElevator)
		{
			Elevator = OverlapElevator;
			UE_LOG(LogTemp, Log, TEXT("Elevator: %s"), *Elevator->GetName());
		}
	}
}

void AXVCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		UE_LOG(LogTemp, Log, TEXT("End Overlap Actor: %s"), *OtherActor->GetName());
		// 엘리베이터 클래스인지 확인
		if (AElevatorDoor* OverlapElevator = Cast<AElevatorDoor>(OtherActor))
		{			
			Elevator = nullptr;
			if (!Elevator)
			{
				UE_LOG(LogTemp, Log, TEXT("Elevator: null"));
			}
		}
	}
}

void AXVCharacter::OnWeaponOverlapBegin(ABaseGun* Weapon)
{
	CurrentOverlappingWeapon = Weapon;

	if (Weapon)
	{
		//CurrentWeaponType = Weapon->GetWeaponType();
		FString WeaponTypeName = UEnum::GetValueAsString(Weapon->GetWeaponType());
		UE_LOG(LogTemp, Log, TEXT("Overlapping Weapon Type: %s"), *WeaponTypeName);
	}
}

void AXVCharacter::OnWeaponOverlapEnd(const ABaseGun* Weapon)
{
	if (CurrentOverlappingWeapon == Weapon)
	{
		CurrentOverlappingWeapon = nullptr;
		//CurrentWeaponType = EWeaponType::None;
	}
}

void AXVCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
	    // IA를 가져오기 위해 현재 소유 중인 Controller를 ASpartaPlayerController로 캐스팅
	    if (AXVPlayerController* PlayerController = Cast<AXVPlayerController>(GetController()))
	    {
	        if (PlayerController->MoveAction)
	        {
	            // IA_Move 액션 키를 "키를 누르고 있는 동안" Move() 호출
	            EnhancedInput->BindAction(
                    PlayerController->MoveAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AXVCharacter::Move
                );
	        }
            
	        if (PlayerController->JumpAction)
	        {
	            // IA_Jump 액션 키를 "키를 누르고 있는 동안" StartJump() 호출
	            EnhancedInput->BindAction(
                    PlayerController->JumpAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AXVCharacter::StartJump
                );
                
	            // IA_Jump 액션 키에서 "손을 뗀 순간" StopJump() 호출
	            EnhancedInput->BindAction(
                    PlayerController->JumpAction,
                    ETriggerEvent::Completed,
                    this,
                    &AXVCharacter::StopJump
                );
	        }
            
	        if (PlayerController->LookAction)
	        {
	            // IA_Look 액션 마우스가 "움직일 때" Look() 호출
	            EnhancedInput->BindAction(
                    PlayerController->LookAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AXVCharacter::Look
                );
	        }
            
	        if (PlayerController->SprintAction)
	        {
	            // IA_Sprint 액션 키를 "누르고 있는 동안" StartSprint() 호출
	            EnhancedInput->BindAction(
                    PlayerController->SprintAction,
                    ETriggerEvent::Triggered, 
                    this, 
                    &AXVCharacter::StartSprint
                );
	            // IA_Sprint 액션 키에서 "손을 뗀 순간" StopSprint() 호출
	            EnhancedInput->BindAction(
                    PlayerController->SprintAction, 
                    ETriggerEvent::Completed, 
                    this, 
                    &AXVCharacter::StopSprint
                );
	        }
	    	
	    	if (PlayerController->FireAction)
	    	{
	    		// IA_Fire 마우스 좌클릭 할 때 Fire() 호출
	    		EnhancedInput->BindAction(
					PlayerController->FireAction,
					ETriggerEvent::Triggered,
					this,
					&AXVCharacter::Fire
				);
	    	}
	    	if (PlayerController->ZoomAction)
	    	{
	    		// IA_Zoom 마우스 우클릭 할 때 StartZoom() 호출
	    		EnhancedInput->BindAction(
					PlayerController->ZoomAction,
					ETriggerEvent::Triggered,
					this,
					&AXVCharacter::StartZoom
				);
	    		// IA_Zoom 마우스 우클릭 끝날 때 StopZoom() 호출
	    		EnhancedInput->BindAction(
					PlayerController->ZoomAction,
					ETriggerEvent::Completed,
					this,
					&AXVCharacter::StopZoom
				);
	    	}
	    	
	    	if (PlayerController->SitAction)
	    	{
	    		// IA_Sit 컨트롤 누를 때 Sit() 호출
	    		EnhancedInput->BindAction(
					PlayerController->SitAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::Sit
				);
	    	}

	    	if (PlayerController->PickUpAction)
	    	{
	    		// IA_PickUp E 누를 때 PickUpWeapon() 호출
	    		EnhancedInput->BindAction(
					PlayerController->PickUpAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::PickUpWeapon
				);
	    	}
	    	if (PlayerController->MainWeaponAction)
	    	{
	    		// IA_MainWeapon 1 누를 때 ChangeToMainWeapon() 호출
	    		EnhancedInput->BindAction(
					PlayerController->MainWeaponAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::ChangeToMainWeapon
				);
	    	}	    	
	    	if (PlayerController->SubWeaponAction)
	    	{
	    		// IA_SubWeapon 2 누를 때 ChangeToSubWeapon() 호출
	    		EnhancedInput->BindAction(
					PlayerController->SubWeaponAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::ChangeToSubWeapon
				);
	    	}
	    	if (PlayerController->OpenDoorAction)
            {
	    		// IA_OpenDoor F키 누를 때 OpenDoor() 호출
            	EnhancedInput->BindAction(
            		PlayerController->OpenDoorAction,
            		ETriggerEvent::Started,
            		this,
            		&AXVCharacter::OpenDoor
            	);
            }
	    	if (PlayerController->ReloadAction)
	    	{
	    		// IA_Reload R키 누를 때 Reload() 호출
	    		EnhancedInput->BindAction(
					PlayerController->ReloadAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::Reload
				);
	    	}	    	
	    }
	}
}

void AXVCharacter::Move(const FInputActionValue& Value)
{
    if (!Controller) return;
    
    const FVector2D MoveInput = Value.Get<FVector2D>();

    if (!FMath::IsNearlyZero(MoveInput.X))
    {
    	FRotator ControlRotation = Controller->GetControlRotation();
    	FRotator YawRotation(0, ControlRotation.Yaw, 0); // Pitch, Roll 제거 (수평 회전만)
    	FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    	
       AddMovementInput(ForwardDirection, MoveInput.X);
    }

    if (!FMath::IsNearlyZero(MoveInput.Y))
    {
       FRotator ControlRotation = Controller->GetControlRotation();
       FRotator YawRotation(0, ControlRotation.Yaw, 0);
       FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
       
       AddMovementInput(RightDirection, MoveInput.Y);
    }
}

void AXVCharacter::StartJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>() && !bIsSit)
	{
		Jump();
	}
}

void AXVCharacter::StopJump(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		StopJumping();
	}
}

void AXVCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();
	
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
	TurnRate = FMath::Clamp(LookInput.X, -1 ,1);
}

void AXVCharacter::StartSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement() && !bIsSit)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bIsRun = true;
	}
}

void AXVCharacter::StopSprint(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	bIsRun = false;
}

void AXVCharacter::Fire(const FInputActionValue& Value)
{
	if (Value.Get<bool>() && CurrentWeaponType != EWeaponType::None && !bIsRun) // 빈 손 일 때, 달릴 때 발사 금지
	{
		auto Anim = Cast<UXVPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		if (Anim)
		{
			Anim->PlayAttackAnim();
		}

		if (CurrentWeaponActor)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Fire"));
			CurrentWeaponActor->FireBullet();
		}

		auto XVController = GetWorld()->GetFirstPlayerController();
		XVController->PlayerCameraManager->StartCameraShake(CameraShake);

		// AI 소리 듣기용입니다.
		UAISense_Hearing::ReportNoiseEvent
		(
		GetWorld(),
		GetActorLocation(),			 // 소리 발생 위치
		1.0f,						 // 소리 Loudness (1.0은 일반, 더 크면 더 멀리 들림)
		this,						 // 소리 낸 Actor
		0.f,						 // 일정 지연시간
		FName(TEXT("WeaponFire"))	 // 이벤트 식별 태그(선택)
		);
	}
}

void AXVCharacter::Sit(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		if (!bIsSit)
		{
		
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Sit"));
			GetCharacterMovement()->MaxWalkSpeed = SitSpeed;
			bIsSit = true;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Stand"));
			GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
			bIsSit = false;
		}
	}	
}

void AXVCharacter::StartZoom(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		bIsAim = true;

		if (!bIsZooming)
		{
			bIsZooming = true;
			GetWorld()->GetTimerManager().SetTimer(
				ZoomTimerHandle,
				this,
				&AXVCharacter::UpdateZoom,
				0.01f,
				true
			);
		}
	}
}

void AXVCharacter::StopZoom(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		bIsAim = false;

		if (!bIsZooming)
		{
			bIsZooming = true;
			GetWorld()->GetTimerManager().SetTimer(
				ZoomTimerHandle,
				this,
				&AXVCharacter::UpdateZoom,
				0.01f,
				true
			);
		}
	}
}

void AXVCharacter::UpdateZoom()
{
	float TargetLength = bIsAim ? ZoomCameraLength : DefaultCameraLength;
	float CurrentLength = SpringArmComp->TargetArmLength;

	// 보간 계산
	float NewLength = FMath::FInterpTo(CurrentLength, TargetLength, GetWorld()->GetDeltaSeconds(), ZoomInterpSpeed);
	SpringArmComp->TargetArmLength = NewLength;
	
	if (FMath::Abs(NewLength - TargetLength) < 1.f)
	{
		SpringArmComp->TargetArmLength = TargetLength;
		GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);
		bIsZooming = false;
	}
}

void AXVCharacter::PickUpWeapon(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		if (CurrentOverlappingWeapon)
		{
			// 메인 총 및 현재 장착 총 변경
			MainWeaponType = CurrentOverlappingWeapon->GetWeaponType();
			CurrentWeaponType = MainWeaponType;
			UE_LOG(LogTemp, Log, TEXT("Picked up Weapon Type: %d"), (uint8)CurrentWeaponType);

			SetWeapon(CurrentWeaponType);
			
			// 무기 액터 파괴
			CurrentOverlappingWeapon->Destroy();
			// 무기 획득 후 오버랩 무기 초기화
			CurrentOverlappingWeapon = nullptr;
		}
	}
}

void AXVCharacter::ChangeToMainWeapon(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Change To MainWeapon"));
	SetWeapon(MainWeaponType);
}

void AXVCharacter::ChangeToSubWeapon(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Change To SubWeapon"));
	SetWeapon(SubWeaponType);
}

void AXVCharacter::OpenDoor(const FInputActionValue& Value)
{
	if (Elevator)
	{
		Elevator->OpenDoor();
	}		
}

void AXVCharacter::Reload(const FInputActionValue& Value)
{
	if (CurrentWeaponType != EWeaponType::None)
	{		
		UE_LOG(LogTemp, Warning, TEXT("Reload"));
		auto Anim = Cast<UXVPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		if (Anim)
		{
			Anim->PlayReloadAnim();
		}
		//BPCurrentWeapon->Reload();
	}
}
