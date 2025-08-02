#include "XVCharacter.h"
#include "XVPlayerController.h"
#include "XVPlayerAnimInstance.h"
#include "EnhancedInputComponent.h"
#include "AssetTypeActions/AssetDefinition_SoundBase.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AISense_Hearing.h" // AI 총소리 듣기 용입니다.

AXVCharacter::AXVCharacter()
{
	PrimaryActorTick.bCanEverTick = false;		

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 250.0f;  
	SpringArmComp->bUsePawnControlRotation = true;  
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	GunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	GunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
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
	    		EnhancedInput->BindAction(
					PlayerController->ZoomAction,
					ETriggerEvent::Triggered,
					this,
					&AXVCharacter::StartZoom
				);

	    		EnhancedInput->BindAction(
					PlayerController->ZoomAction,
					ETriggerEvent::Completed,
					this,
					&AXVCharacter::StopZoom
				);
	    	}
	    	
	    	if (PlayerController->SitAction)
	    	{
	    		EnhancedInput->BindAction(
					PlayerController->SitAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::Sit
				);
	    	}
	    }
	}
}

void AXVCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;
	
	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void AXVCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();
	}
}

void AXVCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void AXVCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();
	
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AXVCharacter::StartSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void AXVCharacter::StopSprint(const FInputActionValue& value)
{
	TargetWalkSpeed = NormalSpeed;
	CurrentWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	// 보간 타이머 실행
	GetWorld()->GetTimerManager().SetTimer(
		WalkSpeedInterpTimerHandle,
		this,
		&AXVCharacter::InterpWalkSpeed,
		0.01f,
		true
	);
}

void AXVCharacter::InterpWalkSpeed()
{

	if (!GetCharacterMovement()) return;

	// 보간
	CurrentWalkSpeed = FMath::FInterpTo(CurrentWalkSpeed, TargetWalkSpeed, GetWorld()->GetDeltaSeconds(), InterpSpeed);
	GetCharacterMovement()->MaxWalkSpeed = CurrentWalkSpeed;
	UE_LOG(LogTemp, Warning, TEXT("%f"), CurrentWalkSpeed);
	
	// 충분히 가까워지면 보간 종료
	if (FMath::IsNearlyEqual(CurrentWalkSpeed, TargetWalkSpeed, 1.0f))
	{
		GetCharacterMovement()->MaxWalkSpeed = TargetWalkSpeed;
		GetWorld()->GetTimerManager().ClearTimer(WalkSpeedInterpTimerHandle);
	}
}

void AXVCharacter::Fire(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Fire"));
		auto anim = Cast<UXVPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		anim->PlayAttackAnim();

		auto controller = GetWorld()->GetFirstPlayerController();
		controller->PlayerCameraManager->StartCameraShake(CameraShake);

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

void AXVCharacter::Sit(const FInputActionValue& value)
{
	if (!bIsSit)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Sit"));
		bIsSit = true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Stand"));
		bIsSit = false;
	}
}

void AXVCharacter::StartZoom(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		SpringArmComp->TargetArmLength = 100.0f;
	}
}

void AXVCharacter::StopZoom(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		SpringArmComp->TargetArmLength = 250.0f;
	}
}