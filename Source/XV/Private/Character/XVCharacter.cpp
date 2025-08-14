#include "Character/XVCharacter.h"
#include "Character/XVPlayerController.h"
#include "Character/XVPlayerAnimInstance.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Weapon/GunBase.h"
#include "World/XVDoor.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AISense_Hearing.h" // AI 총소리 듣기 용입니다.
#include "System/XVBaseGameMode.h"
#include "Inventory/Component/InteractionComponent.h"
#include "Inventory/Component/InventoryComponent.h"
#include "Components/WidgetComponent.h"
#include "UIFollowerComponent.h"
#include "Item/HealthPotionItem.h"
#include "Item/InteractableItem.h"


void AXVCharacter::BroadcastHealth()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void AXVCharacter::BeginPlay()
{
	Super::BeginPlay();

	// BP에 붙어있는 C++ 컴포넌트(이름: UIFollower)를 잡아옴
	UIFollowerComp = FindComponentByClass<UUIFollowerComponent>();
	if (!UIFollowerComp)
	{
		UE_LOG(LogTemp, Error, TEXT("UIFollowerComponent not found on %s"), *GetName());
	}

	SetInventoryItem();
	BroadcastHealth();
}


AXVCharacter::AXVCharacter()
{
	PrimaryActorTick.bCanEverTick = false;		
	
	DefaultCameraLength = 90.0f; // 기본 스프링암 길이
	ZoomCameraLength = 20.0f; // 줌 스프링암 길이
	StandCameraOffset = FVector(-50.f, 30.f, 80.f); // 서 있을 때 카메라 높이
	SitCameraOffset = FVector(-50.f, 30.f, 30.f); // 앉아 있을 때 카메라 높이
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = DefaultCameraLength;  
	SpringArmComp->bUsePawnControlRotation = true;  
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;	

	// 인벤토리 관련
	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(RootComponent);
	
	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	InteractionComp = CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction"));

	NormalSpeed = 300.0f; // 기본 이동 속도
	SprintSpeed = 500.0f; // 달리기 속도
	SitSpeed = 250.0f; // 앉았을 때 속도

	bIsRun = false;
	bIsSit = false;
	bIsAim = false;
	bIsZooming = false;
	bIsLookLeft = false;
	bZoomLookLeft = false;
	bIsDie = false;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	// 방어구 관련
	HelmetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HelmetMesh"));
	HelmetMesh->SetupAttachment(RootComponent);
	VestMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VestMesh"));
	VestMesh->SetupAttachment(RootComponent);
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
	Door = nullptr;
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AXVCharacter::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AXVCharacter::OnEndOverlap);
}

UInventoryComponent* AXVCharacter::GetInventoryComp() const
{
	return InventoryComp;	
}

void AXVCharacter::SetHealth(float Value)
{
	CurrentHealth = FMath::Clamp( Value, 0.0f, MaxHealth);
	BroadcastHealth();
}

void AXVCharacter::SetMaxHealth(float Value)
{
	UE_LOG(LogTemp, Log, TEXT("SetMaxHealth : %f"), Value);
	MaxHealth = Value;
	BroadcastHealth();	
}

float AXVCharacter::GetHealth() const
{
	return CurrentHealth;
}
float AXVCharacter::GetMaxHealth() const
{
	return MaxHealth;
}
void AXVCharacter::AddHealth(float Value)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + Value, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Log, TEXT("AddHealth : %f, Now Health : %f"), Value , CurrentHealth);
	BroadcastHealth();
}

void AXVCharacter::AddDamage(float Value)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Value, 0.0f, MaxHealth);
	FString Str = FString::Printf(TEXT("%f Damaged, Now Health : %f"), Value, CurrentHealth);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Str);
	// 피격 애니메이션 추가
	
	BroadcastHealth();
	
	if (CurrentHealth <= 0.0f)
	{
		if (!bIsDie)
		{
			Die();
		}
	}
}

void AXVCharacter::Die()
{
	bIsDie = true;
	if (APlayerController* XVPlayerController = Cast<APlayerController>(GetController()))
	{
		UE_LOG(LogTemp, Log, TEXT("PlayerController: %s"), *XVPlayerController->GetName());
		XVPlayerController->DisableInput(XVPlayerController);
	}
	
	if (auto Anim = Cast<UXVPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		Anim->PlayDieAnim();
	}

	GetWorldTimerManager().SetTimer(
	DieTimerHandle,
	this,
	&AXVCharacter::OnDieAnimationFinished,
	2.0f,
	false
	);
}

void AXVCharacter::OnDieAnimationFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("EndGame"));
	if (AXVBaseGameMode* XVBaseGameMode = Cast<AXVBaseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		XVBaseGameMode->EndGame(false);
	}
}

void AXVCharacter::SetSpeed(float Value)
{
	NormalSpeed += Value;  // 기본 이동 속도
	SprintSpeed += Value;  // 달리기 속도
	SitSpeed += Value;     // 앉았을 때 속도
}

// 헬멧 변경

void AXVCharacter::SetHelmet(const FArmorData& NewArmor, EArmorType Armor)
{
	UE_LOG(LogTemp, Log, TEXT("SetHelmet"));
	if (Armor == EArmorType::Helmet)
	{
		UE_LOG(LogTemp, Log, TEXT("EArmorType : Helmet"));
		HelmetMesh->SetStaticMesh(NewArmor.ArmorMesh);
		SetMaxHealth(CurrentHealth + 50); // 테스트로 일단 하드코딩
		AddHealth(50);
	}
	if (Armor == EArmorType::Vest)
	{
		UE_LOG(LogTemp, Log, TEXT("EArmorType : Vest"));
		VestMesh->SetStaticMesh(NewArmor.ArmorMesh);
		SetMaxHealth(CurrentHealth + 80); // 테스트로 일단 하드코딩
		AddHealth(80);
	}
}
// 갑옷 변경
void AXVCharacter::SetVest(UStaticMesh* NewVest)
{
	if (VestMesh)
	{
		VestMesh->SetStaticMesh(NewVest);
		SetMaxHealth(CurrentHealth + 70); // 테스트로 일단 하드코딩
		AddHealth(70);
	}
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

		CurrentWeaponActor = Cast<AGunBase>(SubWeapon->GetChildActor());
		SetCameraShake(CurrentWeaponActor->GetCameraShake());
		Anim->PlayWeaponEquipAnim(CurrentWeaponActor);
		break;
		
	case EWeaponType::Rifle:
		// 무기 교체 애니메이션 및 사운드 재생 필요
		SubWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Pistol_Unequipped"));
		PrimaryWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Rifle_Equipped"));

		CurrentWeaponActor = Cast<AGunBase>(PrimaryWeapon->GetChildActor());
		SetCameraShake(CurrentWeaponActor->GetCameraShake());
		Anim->PlayWeaponEquipAnim(CurrentWeaponActor);
		break;

	case EWeaponType::ShotGun:
		// 무기 교체 애니메이션 및 사운드 재생 필요
		SubWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Pistol_Unequipped"));
		PrimaryWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Rifle_Equipped"));
		break;
	
	default:
		SubWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Pistol_Unequipped"));
		PrimaryWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Rifle_Unequipped"));

		CurrentWeaponActor = Cast<AGunBase>(SubWeapon->GetChildActor());
		SetCameraShake(nullptr);
		Anim->PlayWeaponUnequipAnim();
		break;
	}
	// 무기 장착/해제 로직 끝난 다음…
	OnCurrentWeaponChanged.Broadcast(CurrentWeaponActor);

	// (선택) 새 무기의 현재 탄약 값도 즉시 반영되도록 한 번 더 쏴주기
	if (CurrentWeaponActor)
	{
		CurrentWeaponActor->OnMagAmmoChanged.Broadcast(
			CurrentWeaponActor->GetCurrentAmmo(),
			CurrentWeaponActor->GetMagSize()
		);
		CurrentWeaponActor->OnReserveAmmoChanged.Broadcast(
			CurrentWeaponActor->GetRemainingAmmo()
		);
	}
}

void AXVCharacter::SetCameraShake(TSubclassOf<class UCameraShakeBase> Shake)
{
	CameraShake = Shake;
	if (!CameraShake)
	{
		UE_LOG(LogTemp, Log, TEXT("CameraShake: null"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("CameraShake: %s"), *CameraShake->GetName());
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
		//UE_LOG(LogTemp, Log, TEXT("Overlap Actor: %s"), *OtherActor->GetName());
		// 문인지 확인
		if (AXVDoor* OverlapDoor = Cast<AXVDoor>(OtherActor))
		{
			Door = OverlapDoor;
			UE_LOG(LogTemp, Log, TEXT("Elevator: %s"), *Door->GetName());
		}
	}
}

void AXVCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		//UE_LOG(LogTemp, Log, TEXT("End Overlap Actor: %s"), *OtherActor->GetName());
		// 엘리베이터 클래스인지 확인
		if (Door == Cast<AXVDoor>(OtherActor))
		{			
			Door = nullptr;
			if (!Door)
			{
				UE_LOG(LogTemp, Log, TEXT("Elevator: null"));
			}
		}
	}
}

void AXVCharacter::OnWeaponOverlapBegin(AGunBase* Weapon)
{
	CurrentOverlappingWeapon = Weapon;

	if (Weapon)
	{
		//CurrentWeaponType = Weapon->GetWeaponType();
		//FString WeaponTypeName = UEnum::GetValueAsString(Weapon->GetWeaponType());
		//UE_LOG(LogTemp, Log, TEXT("Overlapping Weapon Type: %s"), *WeaponTypeName);
	}
}

void AXVCharacter::OnWeaponOverlapEnd(const AGunBase* Weapon)
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
					ETriggerEvent::Started,
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
	    	if (PlayerController->InventoryAction)
	    	{
	    		// IA_Reload I키 누를 때 Inventory() 호출
	    		EnhancedInput->BindAction(
					PlayerController->InventoryAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::Inventory
				);
	    	}
	    	if (PlayerController->ItemInteractAction)
	    	{
	    		// IA_Reload G키 누를 때 ItemInteract() 호출
	    		EnhancedInput->BindAction(
					PlayerController->ItemInteractAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::ItemInteract
				);
	    	}
	    	if (PlayerController->ChangeZoomLeftAction)
	    	{
	    		// IA_ChangeZoomLeft Q키 누를 때 ItemInteract() 호출
	    		EnhancedInput->BindAction(
					PlayerController->ChangeZoomLeftAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::ChangeLeftZoom
				);
	    	}
	    	if (PlayerController->ChangeZoomRightAction)
	    	{
	    		// IA_ChangeZoomRight E키 누를 때 ChangeRightZoom() 호출
	    		EnhancedInput->BindAction(
					PlayerController->ChangeZoomRightAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::ChangeRightZoom
				);
	    	}
	    	if (PlayerController->HealAction)
	    	{
	    		EnhancedInput->BindAction(
					PlayerController->HealAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::StartUseCurrentItem   // ★ 변경
				);
	    		EnhancedInput->BindAction(
					PlayerController->HealAction,
					ETriggerEvent::Completed,
					this,
					&AXVCharacter::StopUseCurrentItem    // ★ 변경
				);
	    	}	
	    }
	}
}

void AXVCharacter::Move(const FInputActionValue& Value)
{
	if (bIsDie) return;
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
	if (bIsDie) return;
	if (Value.Get<bool>() && !bIsSit)
	{
		Jump();
	}
}

void AXVCharacter::StopJump(const FInputActionValue& Value)
{
	if (bIsDie) return;
	if (!Value.Get<bool>())
	{
		StopJumping();
	}
}

void AXVCharacter::Look(const FInputActionValue& Value)
{
	if (bIsDie) return;
	FVector2D LookInput = Value.Get<FVector2D>();
	
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
	if (LookInput.X > 0.f)
	{
		// 오른쪽으로 회전 중
		bIsLookLeft = false;
	}
	else if (LookInput.X < 0.f)
	{
		// 왼쪽으로 회전 중
		bIsLookLeft = true;
	}
	TurnRate = FMath::Clamp(LookInput.X, -1 ,1);
}

void AXVCharacter::StartSprint(const FInputActionValue& Value)
{	
	if (bIsDie) return;
	if (bIsZooming || bIsAim) // 줌 중이면 해제
	{
		StopZoomManual();
	}
	
	if (CurrentWeaponActor)
	{
		if (CurrentWeaponActor->IsReloading()) return;
	}
	
	if (GetCharacterMovement() && !bIsSit)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bIsRun = true;
	}
}

void AXVCharacter::StopSprint(const FInputActionValue& Value)
{
	if (bIsDie) return;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	bIsRun = false;
}

void AXVCharacter::Fire(const FInputActionValue& Value)
{
	if (bIsDie) return;
	if (Value.Get<bool>() && CurrentWeaponType != EWeaponType::None && !bIsRun) // 빈 손 일 때, 달릴 때 발사 금지
	{
		if (!CurrentWeaponActor->IsCanFire()) return;
		auto Anim = Cast<UXVPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		if (Anim)
		{
			Anim->PlayAttackAnim(CurrentWeaponActor);
		}

		if (CurrentWeaponActor)
		{
			CurrentWeaponActor->FireBullet();
		}

		auto XVController = GetWorld()->GetFirstPlayerController();
		if (CameraShake)
		{
			XVController->PlayerCameraManager->StartCameraShake(CameraShake);
		}

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
	if (bIsDie) return;
	if (GetCharacterMovement())
	{		
		if (!bIsSit)
		{		
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Sit"));
			GetCharacterMovement()->MaxWalkSpeed = SitSpeed;
			bIsRun = false;
			bIsSit = true;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Stand"));
			GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
			bIsSit = false;
		}
		if (!bIsSetCameraOffset)
		{
			bIsSetCameraOffset = true;
			GetWorld()->GetTimerManager().SetTimer(
				CameraOffsetTimerHandle,
				this,
				&AXVCharacter::UpdateCameraOffset,
				0.01f,
				true
			);
		}
	}	
}

void AXVCharacter::UpdateCameraOffset()
{
	FVector TargetOffset = bIsSit ? SitCameraOffset : StandCameraOffset;
	FVector CurrentOffset =  SpringArmComp->GetRelativeLocation();

	FVector NewOffset = FMath::VInterpTo(CurrentOffset, TargetOffset, GetWorld()->GetDeltaSeconds(), CameraOffsetInterpSpeed);
	SpringArmComp->SetRelativeLocation(NewOffset);

	if (FVector::Dist(NewOffset, TargetOffset) < 1.f)
	{
		SpringArmComp->SetRelativeLocation(TargetOffset);
		GetWorld()->GetTimerManager().ClearTimer(CameraOffsetTimerHandle);
		bIsSetCameraOffset = false;
	}
}

void AXVCharacter::StartZoom(const FInputActionValue& Value)
{
	bIsAim = true;
	bZoomLookLeft = false; // 기본 줌은 False로 고정 -> 캐릭터가 왼쪽으로
	if (UIFollowerComp)
	{
		UIFollowerComp->SetAimState(true, bZoomLookLeft ? ESideUI::Left : ESideUI::Right);
	}
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

void AXVCharacter::StopZoom(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		StopZoomManual();
	}
}

void AXVCharacter::StopZoomManual()
{
	// 줌 해제 신호
	bIsAim = false;
	if (UIFollowerComp)
	{
		UIFollowerComp->SetAimState(false, ESideUI::Right);
	}
	// 이미 보간중이 아니면 보간 타이머 시작
	if (!bIsZooming)
	{
		bIsZooming = true;
		UpdateZoom(); // 즉시 한번 호출해서 반응성 보정
		GetWorld()->GetTimerManager().SetTimer(
			ZoomTimerHandle,
			this,
			&AXVCharacter::UpdateZoom,
			0.01f,
			true
		);
	}
}

void AXVCharacter::ChangeLeftZoom(const FInputActionValue& Value)
{
	SetZoomDirection(true);
}
void AXVCharacter::ChangeRightZoom(const FInputActionValue& Value)
{
	SetZoomDirection(false);
}

void AXVCharacter::SetZoomDirection(bool bIsLookLeftDirection)
{
	if (bIsAim) // 줌 상태일 때만 작동
	{
		bZoomLookLeft = bIsLookLeftDirection;
		UpdateZoom();
		GetWorld()->GetTimerManager().SetTimer(
			ZoomTimerHandle,
			this,
			&AXVCharacter::UpdateZoom,
			0.01f,
			true
		);
	}
}

void AXVCharacter::UpdateZoom()
{
	// 줌 거리 보간
	float TargetLength = bIsAim ? ZoomCameraLength : DefaultCameraLength;
	float CurrentLength = SpringArmComp->TargetArmLength;

	float NewLength = FMath::FInterpTo(CurrentLength, TargetLength, GetWorld()->GetDeltaSeconds(), ZoomInterpSpeed);
	SpringArmComp->TargetArmLength = NewLength;

	// 카메라 위치 보간 조건에 따라 설정
	FVector TargetLocation;

	if (!bIsAim) // 조준 중이 아니면 카메라 원래 대로
	{
		TargetLocation = FVector::ZeroVector;
	}
	else // 현재 바라보고 있는 방향에 따라 카메라 위치 이동
	{
		TargetLocation = bZoomLookLeft ? FVector(0.f, -90.f, 0.f) : FVector(0.f, 40.f, 0.f);
	}

	FVector CurrentLocation = CameraComp->GetRelativeLocation();
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, GetWorld()->GetDeltaSeconds(), ZoomInterpSpeed);
	CameraComp->SetRelativeLocation(NewLocation);

	// 줌 및 위치 전환 완료 조건
	bool bZoomDone = FMath::Abs(NewLength - TargetLength) < 1.f;
	bool bLocationDone = FVector::Dist(NewLocation, TargetLocation) < 1.f;

	if (bZoomDone && bLocationDone)
	{
		SpringArmComp->TargetArmLength = TargetLength;
		CameraComp->SetRelativeLocation(TargetLocation);

		GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);
		bIsZooming = false;
	}
}

void AXVCharacter::PickUpWeapon(const FInputActionValue& Value)
{
	if (bIsDie) return;
	if (Value.Get<bool>())
	{
		if (CurrentOverlappingWeapon)
		{
			UE_LOG(LogTemp, Log, TEXT("PickUp"));			
		}
	}
}

void AXVCharacter::ChangeToMainWeapon(const FInputActionValue& Value)
{
	if (bIsDie) return;
	UE_LOG(LogTemp, Warning, TEXT("Change To MainWeapon"));
	SetWeapon(MainWeaponType);
}

void AXVCharacter::ChangeToSubWeapon(const FInputActionValue& Value)
{
	if (bIsDie) return;
	UE_LOG(LogTemp, Warning, TEXT("Change To SubWeapon"));
	SetWeapon(SubWeaponType);
}

void AXVCharacter::OpenDoor(const FInputActionValue& Value)
{
	if (bIsDie) return;
	if (Door)
	{
		Door->OpenDoor();
	}		
}

void AXVCharacter::Reload(const FInputActionValue& Value)
{
	if (bIsDie || bIsRun) return;
	if (CurrentWeaponType != EWeaponType::None && !CurrentWeaponActor->IsReloading())
	{
		auto Anim = Cast<UXVPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		if (Anim)
		{
			Anim->PlayReloadAnim(CurrentWeaponActor);
		}
		CurrentWeaponActor->Reload();
	}
}

void AXVCharacter::Inventory(const FInputActionValue& Value)
{
	if (!InventoryComp)
	{
		return;
	}
	InventoryComp->ToggleInventory();
}


void AXVCharacter::ItemInteract(const FInputActionValue& Value)
{
	if (!InteractionComp) return;

	InteractionComp->HandleItemInteract();
	SetInventoryItem();
}


void AXVCharacter::SetCurrentItem(AInteractableItem* Item)
{
	if (CurrentItem == Item) return;        // ★ 중복 방지

	CurrentItem = Item;
	OnCurrentItemChanged.Broadcast(CurrentItem);   // HUD/3D 위젯에 알림
}

void AXVCharacter::StartUseCurrentItem()
{
	if (bIsDie) return;
	
	if (!CurrentItem)
	{
		if (HealthPotionCount <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("No health potions."));
			return;
		}
		SpawnPotionForUse(); // SetCurrentItem 내부에서 브로드캐스트 됨
	}

	if (AHealthPotionItem* Potion = Cast<AHealthPotionItem>(CurrentItem))
	{
		UE_LOG(LogTemp, Log, TEXT("StartUseCurrentItem: HealthPotion StartUse"));
		Potion->StartUse();
	}
	else if (CurrentItem)
	{
		//CurrentItem = InventoryComp->GetItemData("HealthPotion")->ItemClass;
		CurrentItem->UseItem();		
	}
}

void AXVCharacter::StopUseCurrentItem()
{
	if (bIsDie) return;

	if (AHealthPotionItem* Potion = Cast<AHealthPotionItem>(CurrentItem))
	{
		UE_LOG(LogTemp, Log, TEXT("StopUseCurrentItem: HealthPotion StopUse"));
		Potion->StopUse();
	}
}

void AXVCharacter::SetInventoryItem()
{		
	// 초기 아이템 세팅
	HealthPotionCount = InventoryComp->GetItemQuantity("HealthPotion");
	OnHealthPotionCountChanged.Broadcast(HealthPotionCount);
}

AHealthPotionItem* AXVCharacter::SpawnPotionForUse()
{
	if (!GetWorld()) return nullptr;

	FActorSpawnParameters Params;
	Params.Owner = this;
	// 충돌로 인해 스폰 실패하지 않도록
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector Loc = GetActorLocation() + GetActorForwardVector() * 40.f + FVector(0,0,10);
	const FRotator Rot = GetActorRotation();

	AHealthPotionItem* NewPotion =
		GetWorld()->SpawnActor<AHealthPotionItem>(AHealthPotionItem::StaticClass(), Loc, Rot, Params);

	if (NewPotion)
	{
		// HUD/3D 위젯이 새 포션에 다시 바인딩되도록
		SetCurrentItem(NewPotion);
	}
	return NewPotion;
}
