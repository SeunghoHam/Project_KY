// Fill out your copyright notice in the Description page of Project Settings.


#include "Tanjiro/Tanjiro.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tanjiro/Components/AnimationControlComponent.h"
#include "EngineUtils.h" // TActorIterator
#include "TanjiroController.h"
#include "Enemy/Oni.h"
#include "Tanjiro/Components/InventoryComponent.h"
#include "Tanjiro/Widgets/InGameWidget.h"

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
//#include "DataAsset/TanjiroAnimMontageMap.h"
#include "Components/SoundComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DataAsset/InputDataAsset.h"
#include "Components/SplineComponent.h"
#include "System/AudioManagerSubsystem.h"

// Sets default values
ATanjiro::ATanjiro()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FollowCamera =CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->bUsePawnControlRotation = true;
	FollowCamera->SetupAttachment(GetMesh(), FName(TEXT("HeadSocket")));
	// 틱 순서 보정
	FollowCamera->PrimaryComponentTick.bCanEverTick = true;
	FollowCamera->PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
	FollowCamera->AddTickPrerequisiteComponent(GetMesh());
	//FollowCamera->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("HeadSocket")));
	
	FollowCamera->SetRelativeLocation(CameraStart);
	//FollowCamera->SetRelativeLocation(FVector::ZeroVector);
	FollowCamera->SetRelativeRotation(FRotator(0.f, 0.f, 90.f));
	FollowCamera->SetFieldOfView(NormalFov);
	
	KatanaMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Katana"));
	KatanaMesh->SetupAttachment(GetMesh(), FName(TEXT("Katana")));
	
	//KatanaMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("Katana")));

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComp"));
	
	UCharacterMovementComponent* move= GetCharacterMovement();
	move->GravityScale = 3.0f;
	move->AirControl = 1.0f;
}

void ATanjiro::BeginPlay()
{
	Super::BeginPlay();

	TanjiroController = Cast<ATanjiroController>(GetController());
	TargetTag = FName(TEXT("Oni"));
	StatusInitialize(100.f, 10.f);
	
	if (InGameWidgetClass != nullptr)
	{
		if (!IsLocallyControlled()) return;
		InGameWidget =  CreateWidget<UInGameWidget>(GetWorld(), InGameWidgetClass,NAME_None);
		InGameWidget->AddToViewport(0);

		if (InGameWidget)
		{
			//    InventoryWidget->Init(Inventory, GetGameInstance<UMyGameInstance>()->ItemDB);
			InGameWidget->Init(InventoryComponent, ItemDB,10,5,FVector2D(128.f, 128.f));

			//InGameWidget->addu
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Tanjiro] InGameWidgetClass is Null"));
	}
	
	//CachedPPMaterial();
	if (FX_ParringAsset && KatanaMesh)
	{
		FX_Parring = UGameplayStatics::SpawnEmitterAttached(
			FX_ParringAsset, KatanaMesh, "CounterSocket",
			FVector::ZeroVector, FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget, false // 자동 파괴 X
		);
		if (FX_Parring)
		{
			FX_Parring->DeactivateSystem(); // 기본은 꺼둠
		}
	}
	/*
	if (MuzzleFX && GetMesh())
	{
		MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(
			MuzzleFX, GetMesh(), MuzzleSocketName,
			FVector::ZeroVector, FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget, false // 자동 파괴 X
		);
		if (MuzzlePSC)
		{
			MuzzlePSC->DeactivateSystem(); // 기본은 꺼둠
		}
	}
	// 사용할때
	if (MuzzlePSC)
	{
		MuzzlePSC->ActivateSystem(true);
		// 시간 지나면 끄거나, 타이머로 DeactivateSystem 호출
	}*/
}

void ATanjiro::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsAimSetting) // MoveAttack 준비단계(차징 시작)
	{
		AimTime += DeltaTime;
		float Alpha = FMath::Clamp(AimTime / FocusingTime, 0.f, 1.f);
		FVector newLoc = FMath::Lerp(CameraStart, CameraDownMove, Alpha);
		float fov = FMath::Lerp(NormalFov, ParringFov, Alpha);
		
		//FollowCamera->SetFieldOfView(fov);
		FollowCamera->SetRelativeLocation(newLoc);
		
		if (Alpha >= 1.0f) bIsAimSetting = false;
	}
	if (bIsMoveAttacking)
	{
		ElapsedTime += DeltaTime;
		float Alpha = FMath::Clamp(ElapsedTime / MoveAttackTime, 0.f, 1.f);
		float Smooth = Ease01(Alpha, 2.0f); // EaseEXp 2.0 ~ 2.4 권장
		FVector newLoc = FMath::Lerp(MoveAttack_Start, MoveAttack_End, Smooth);
		SetActorLocation(newLoc);
		
		FVector newCamLoc = FMath::Lerp(CameraDownMove, CameraStart, Alpha);
		float fov = FMath::Lerp(ParringFov, NormalFov, Alpha);
		
		//FollowCamera->SetFieldOfView(fov);
		FollowCamera->SetRelativeLocation(newCamLoc);
		
		if (ElapsedTime >= 1.0f)
			bIsMoveAttacking = false;
	}

	/*
	if (bIsJumpAttacking)
	{
		JumpAttackTime += DeltaTime;
		float alpha = FMath::Clamp(JumpAttackTime/JumpAttackDivide, 0.f,1.0f );
		FVector newLoc = FMath::Lerp(JumpAttack_Start, JumpAttack_End, alpha);
		FQuat newrot = FQuat::Slerp(Turn_Start.Quaternion(), Turn_End.Quaternion(),alpha);
		SetActorLocation(newLoc);
		SetActorRotation(newrot);
		
		if (JumpAttackTime >= 1.0f) bIsJumpAttacking = false;
	}*/
}
// Called to bind functionality to input
void ATanjiro::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		JumpAction = InputDataAsset->InputDataMap.Find("Jump")->Get();
		LMBAction = InputDataAsset->InputDataMap.Find("LMB")->Get();
		RMBAction	 = InputDataAsset->InputDataMap.Find("RMB")->Get();
		MoveAction = InputDataAsset->InputDataMap.Find("Move")->Get();
		LookAction = InputDataAsset->InputDataMap.Find("Look")->Get();
		ShiftAction = InputDataAsset->InputDataMap.Find("Shift")->Get();
		QAction = InputDataAsset->InputDataMap.Find("Q")->Get();
		EAction = InputDataAsset->InputDataMap.Find("E")->Get();
		TabAction = InputDataAsset->InputDataMap.Find("Tab")->Get();
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATanjiro::InputJumpPressed);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ATanjiro::InputJumpReleased);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATanjiro::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATanjiro::LookInput);
		//EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ATanjiro::LookInput);
		
		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &ATanjiro::OnShitPressed);
		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &ATanjiro::OnShiftReleased);

		EnhancedInputComponent->BindAction(LMBAction, ETriggerEvent::Started, this, &ATanjiro::Input_LMB_Pressed);
		EnhancedInputComponent->BindAction(LMBAction, ETriggerEvent::Completed, this, &ATanjiro::Input_LMB_Release);

		EnhancedInputComponent->BindAction(RMBAction, ETriggerEvent::Started, this, &ATanjiro::Input_RMB_Press);
		EnhancedInputComponent->BindAction(RMBAction, ETriggerEvent::Completed, this, &ATanjiro::Input_RMB_Release);

		EnhancedInputComponent->BindAction(QAction, ETriggerEvent::Started, this, &ATanjiro::Input_Q_Press);
		EnhancedInputComponent->BindAction(QAction, ETriggerEvent::Completed, this, &ATanjiro::Input_Q_Release);

		EnhancedInputComponent->BindAction(EAction, ETriggerEvent::Started, this, &ATanjiro::Input_E_Press);
		EnhancedInputComponent->BindAction(EAction, ETriggerEvent::Completed, this, &ATanjiro::Input_E_Release);
		
		EnhancedInputComponent->BindAction(TabAction, ETriggerEvent::Triggered, this, &ATanjiro::Input_Tab_Pressed);
	}
	else
	{
		//UE_LOG(LogKY, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

}
/*
void ATanjiro::CachedPPMaterial()
{
	UWorld* World = GetWorld();
	if (!World) return;

	//const FVector MyLoc = GetActorLocation();
	APostProcessVolume* PPV = GetLevelProcessVolue();
	if (!PPV) return;

	const TArray<FWeightedBlendable>& Blendables = PPV->Settings.WeightedBlendables.Array;
	// 필요하다면 멤버로 저장할 배열
	//CachedPPMIDs.Empty();

	for (const FWeightedBlendable& WB : Blendables)
	{
		if (!WB.Object) continue;

		if (UMaterialInterface* MI = Cast<UMaterialInterface>(WB.Object))
		{
			// 캐릭터 소유의 MID 생성 (파라미터 조작 용이)
			UMaterialInstanceDynamic* WhiteBlackMID = UMaterialInstanceDynamic::Create(MI, this);
			if (WhiteBlackMID)
			{
				CachedPPMid = WhiteBlackMID;
				//CachedPPMIDs.Add({ MID, WB.Weight });
			}
		}
		// 특수 케이스: 다른 블렌더블(커스텀 IBlendableInterface 구현체)일 수도 있으니 필요 시 처리
	}
}
*/

void ATanjiro::CachedPPMaterialSetting(float value)
{
	UWorld* World = GetWorld();
	if (!World || !WhiteBlackMat) return;

	for (TActorIterator<APostProcessVolume> It(World); It; ++It)
	{
		APostProcessVolume* PPV = *It;
		if (!PPV || !PPV->bEnabled) continue;

		// 예: 언바운드로 전역 적용되는 PPV만 바꾸기
		if (PPV->bUnbound)
		{
			// Array 비우고 새 값 넣기
			PPV->Settings.WeightedBlendables.Array.Empty();

			// 새 WeightedBlendable 추가
			FWeightedBlendable NewWB(value, WhiteBlackMat); // 1.0 = Weight
			PPV->Settings.WeightedBlendables.Array.Add(NewWB);

			UE_LOG(LogTemp, Log, TEXT("PostProcessVolume material changed!"));
		}
	}
}

APostProcessVolume* ATanjiro::GetLevelProcessVolue()
{
	APostProcessVolume* Best = nullptr;
	float BestPriority = -FLT_MAX;

	for (TActorIterator<APostProcessVolume> It(GetWorld()); It; ++It)
	{
		APostProcessVolume* PPV = *It;
		if (!PPV || !PPV->bEnabled) continue;

		//const bool bAffects =
		//	PPV->bUnbound || PPV->EncompassesPoint(PawnLoc);

		if (PPV->Priority > BestPriority)// && bAffects)
		{
			Best = PPV;
			BestPriority = PPV->Priority;
		}
	}
	return Best;
}

void ATanjiro::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ATanjiro::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);
}

void ATanjiro::OnShitPressed()
{
	if (bisShiftPressed) return;
	bisShiftPressed =true;
	BP_AvoidMove();
	bIsAvoiding = true;
	SoundComponent->Play2DHandle(ESoundName::effect_Avoid);
	//UCharacterMovementComponent* move= GetCharacterMovement();
	//move->MaxWalkSpeed = 1200.f;
	//GetMovementComponent()->setwalk
}

void ATanjiro::OnShiftReleased()
{
	bisShiftPressed = false;
	//UCharacterMovementComponent* move= GetCharacterMovement();
	//move->MaxWalkSpeed = 600.f;
}

void ATanjiro::InputJumpPressed()
{
	if (WidgetActiveNumber != 0) return;
	if (bJumpPressed) return;
	bJumpPressed = true;
	
	if (bIsQPressed)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("Big Jump"));
		GetCharacterMovement()->JumpZVelocity = MinJumpVelocity + 1000.f;
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("Normal Jump"));
		GetCharacterMovement()->JumpZVelocity = MinJumpVelocity;
		
	}
	Jump();
	//JumpTimer=0.0f;
	//CurrentJumpVelocity  = MinJumpVelocity;
}

void ATanjiro::InputJumpReleased()
{
	bJumpPressed=  false;
	/*
	// JumpTimer 0 ~ 2
	float Add = 2000.f * JumpTimer;
	GetCharacterMovement()->JumpZVelocity = MinJumpVelocity + Add;
	Jump();
	//SetJumpHeightMeters()
	*/
}

void ATanjiro::Input_LMB_Pressed()
{
	if (WidgetActiveNumber != 0) return;
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue,TEXT("LMBPress"));
	
	if (bIsQPressed)
	{
		// 직선으로 라인트레이스 + 해당 위치로 빠르게 이동 + 프로세싱 효과
		
		//MoveAttack(); //맞은 대상이 있음.
		MoveAttackTargetCheck();
	}
	else if (bIsEPressed)
	{
		// 카운터 공격 방향은 반대로
		ChangeAttackType(EAttackType::LeftParring);
		bIsParringEnable = true;
		TanjiroParring();//EAttackType::Right); // 적공격이 Right일때
		BP_ParringCamera(true);

	}
	else
	{
		SoundComponent->Play2DHandle(ESoundName::effect_Swing);
		CurrentTargetOni = Cast<AOni>(FindNearestEnemy(400.f));
		if (CurrentTargetOni)
		{
			FVector dir = (CurrentTargetOni->GetActorLocation() -GetActorLocation()).GetSafeNormal();
			FVector targetLoc = CurrentTargetOni->GetActorLocation() - (dir * 100.f);
			//SetActorLocation(targetLoc);
			
			FRotator targetRot = dir.Rotation();
			float yawOnly = targetRot.Yaw;
			FRotator targetrot = FRotator( 0.f , yawOnly , 0);
			//Controller->SetControlRotation(FRotator(0.f, yawOnly, 0.f));
			//FollowCamera->SetWorldRotation(dir.Rotation());
			BP_AttackMove(GetActorLocation(),GetActorRotation(),targetLoc,targetrot);
		}
		
		PlayAnimationMontage(LeftAttack);
		ChangeAttackType(EAttackType::Left);
	};
}

void ATanjiro::Input_LMB_Release()
{
	if (WidgetActiveNumber != 0) return;
	if (bIsParringEnable)
	{
		bIsParringEnable = false;
		ParringMotion(false);
		BP_ParringCamera(false);	
	}
}

void ATanjiro::Input_RMB_Press()
{
	if (WidgetActiveNumber != 0) return;
	//TanjiroAnimationComponent->PlayByName("RightCounter");
	//JumpAttack();

	if (bIsQPressed)
	{
		
	}
	else if (bIsEPressed)
	{
		// 카운터 공격 방향은 반대로
		ChangeAttackType(EAttackType::RightParring);
		bIsParringEnable = true;
		TanjiroParring();//EAttackType::Left);
		BP_ParringCamera(true);
		BP_ParringCameraMove(true);
	}
	else
	{
		SoundComponent->Play2DHandle(ESoundName::effect_Swing);
		CurrentTargetOni = Cast<AOni>(FindNearestEnemy(250.f));
		PlayAnimationMontage(RightAttack);
		ChangeAttackType(EAttackType::Right);
	}
}

void ATanjiro::Input_RMB_Release()
{
	if (WidgetActiveNumber != 0) return;
	if (bIsParringEnable)
	{
		bIsParringEnable = false;
		ParringMotion(false);
		BP_ParringCamera(false);
		BP_ParringCameraMove(false);
	}
}

void ATanjiro::Input_Q_Press()
{
	if (bIsQPressed) return;
	bIsQPressed = true;
}

void ATanjiro::Input_Q_Release()
{
	bIsQPressed = false;
}

void ATanjiro::Input_E_Press()
{
	if (WidgetActiveNumber != 0) return;
	if (bIsEPressed)return;
	bIsEPressed = true;
	
	//FollowCamera->SetFieldOfView(120.f);
}

void ATanjiro::Input_E_Release()
{
	if (WidgetActiveNumber != 0) return;
	bIsEPressed =false;
	if (bIsParringEnable)
	{
		bIsParringEnable = false;
		ParringMotion(false);
		BP_ParringCamera(false);
		if (AttackType == EAttackType::RightParring) BP_ParringCameraMove(false);
	}
	//FollowCamera->SetFieldOfView(90.f);
}

void ATanjiro::Input_Tab_Pressed()
{
	InGameWidget->ToggleSwitcher();
	
	if (WidgetActiveNumber ==0)
	{
		WidgetActiveNumber++;
		WidgetActiveNumber = FMath::Clamp(WidgetActiveNumber, 0,1);
		FInputModeGameAndUI Mode;
		Mode.SetWidgetToFocus(InGameWidget->TakeWidget());
		TanjiroController->SetInputMode(Mode);
		TanjiroController->bShowMouseCursor = true;
	}
	else
	{
		WidgetActiveNumber--;
		WidgetActiveNumber = FMath::Clamp(WidgetActiveNumber, 0,1);
		FInputModeGameOnly Mode;
		TanjiroController->SetInputMode(Mode);
		TanjiroController->bShowMouseCursor =false;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Tab Pressed"));
}

void ATanjiro::CheckCurrentHP()
{
	Super::CheckCurrentHP();
	//GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::White, FString::Printf(TEXT(" currentHP : %f"), CurrentHP));
	PlayAnimationMontage(HitMontage);
}

void ATanjiro::AN_AttackPoint()
{
	TanjiroController->ClientStartCameraShake(DefaultCameraShake.Get());
	FVector HalfVector = FVector(50.f, 50.f, 25.0f);
	float End = 200.f;
	
	AOni* target  = Cast<AOni>(AttackPoint(GetActorForwardVector(),HalfVector,End));
	if (target != nullptr)
	{
		SpawnBoodActor();
		SoundComponent->PlayAtLocation(ESoundName::effect_Hit,target->GetActorLocation());
		UGameplayStatics::ApplyDamage(
			target,
			10.0f,
			TanjiroController,this,UDamageType::StaticClass()
			);
		// TODO : Target에게 데미지 발생
	}
	
}

void ATanjiro::AN_FreezeAtEnd()
{
	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	//1Anim->Montage_SetPosition(CurrentPlayMontage, 1.f);
	Anim->Montage_Pause(CurrentPlayMontage);

	GetWorld()->GetTimerManager().SetTimer(FreezeTimerHandle, this, &ATanjiro::UnFreeze, 0.5f, false);
}

void ATanjiro::UnFreeze()
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green,TEXT("UnFreeze"));
	// 입력 가능상태도 여기서 변경
	//UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	//Anim->Montage_Resume(currentplya)
}

void ATanjiro::PlayParringEffect()
{
	if (!FX_ParringAsset) return;
	UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAttached(
	 FX_ParringAsset,                          // Template
	 KatanaMesh,                          // Attach To
	 "CounterSocket",                  // Socket Name
	 FVector::ZeroVector,               // Location offset (소켓 기준)
	 FRotator::ZeroRotator,             // Rotation offset
	 EAttachLocation::SnapToTarget,     // 소켓 변환에 스냅
	 true                               // bAutoDestroy (원샷이면 true 권장)
 );
}

void ATanjiro::SpawnImpactFX(UObject* WorldContext, UNiagaraSystem* System, const FVector& ImpactPoint,
	const FVector& Dir, const FVector& Scale)
{
	if (!WorldContext || !System) return;

	// 법선 방향을 "앞(Forward, X축)" 으로 삼는 회전값
	//const FRotator Rot = FRotationMatrix::MakeFromX(ImpactNormal).Rotator();
	const FRotator Rot = FRotationMatrix::MakeFromX(Dir).Rotator();
	// 방향만 알고있으면 FRotationMatrix::MakeFromX(Direction) 으로 설정하면 됨
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		WorldContext,
		System,
		ImpactPoint,                 // 원하는 월드 위치
		Rot,                         // 원하는 월드 회전
		Scale,               // 스케일
		true,                        // bAutoDestroy (원샷이면 true)
		true,                        // bAutoActivate
		ENCPoolMethod::AutoRelease,  // 풀링 방법(권장)
		true                         // bPreCullCheck
	);
}


void ATanjiro::JumpAttack()
{
	float Distance=  2000.f;
	// 1) 카메라(플레이어 뷰포인트) 얻기
	FVector CamLoc; FRotator CamRot;
	if (AController* C = GetController())
	{
		// 플레이어인 경우: 실제 화면 카메라 기준(줌/FOV 반영)
		C->GetPlayerViewPoint(CamLoc, CamRot);
	}
	/*
	else if (FollowCamera)
	{
		// 컨트롤러가 아직 없으면 카메라 컴포넌트 기준
		CamLoc = FollowCamera->GetComponentLocation();
		CamRot = FollowCamera->GetComponentRotation();
	}
	else
	{
		// 최후 보루: Pawn 기본 시점
		CamLoc = GetPawnViewLocation();
		CamRot = GetViewRotation();
	}*/
	
	AOni* oni = LineTraceFront();
	if (oni != nullptr)
	{
		bIsJumpAttacking = true;
		JumpAttackTime= 0.f;
		JumpAttack_Start = GetActorLocation();
		//FVector Target =  OutHit.GetActor()->GetActorLocation();
		FVector normal = FVector(GetActorLocation().X, GetActorLocation().Y, oni->GetActorLocation().Z);
		FVector normalDir = (normal - oni->GetActorLocation()).GetSafeNormal();
		
		//FVector ActorDir =  oni->GetActorForwardVector();
		JumpAttack_End  =  oni->GetActorLocation() - (normalDir * 100.f);

		Turn_Start = GetActorRotation();
		Turn_End = oni->GetActorRotation();
		float dist = FVector::Dist(JumpAttack_Start, JumpAttack_End);
		//JumpAttackDivide = 0.25f;
	}
}
void ATanjiro::MoveAttackTargetCheck()
{
	FVector ForwardDir = FollowCamera->GetForwardVector();
	FVector halfSize = FVector(100.f, 100.f, 100.f);
	float end = 2500.0f;
	
	CurrentTargetOni = Cast<AOni>( CreateCheckCollision(ForwardDir, halfSize, end));
	if (CurrentTargetOni)
	{
		CachedPPMaterialSetting(1.0f);
		// 입력 막기단계
		FVector Dir = (GetActorLocation()- CurrentTargetOni->GetActorLocation()).GetSafeNormal();
		FVector SpawnPoint = CurrentTargetOni->GetActorLocation();// + (Dir * 150.f);
		SpawnImpactFX(GetWorld(),FX_TargetSelect,SpawnPoint,Dir);
		SpawnMoveAttackFireRoad(CurrentTargetOni->GetActorLocation() + FVector(0.f, 0.f , -40.f));

		PlayAnimationMontage(Charging);
		CurrentTargetOni->SetAnimType(EAnimType::Idle);
		
		FTimerHandle SlowHandle;
		GetWorld()->GetTimerManager().SetTimer(SlowHandle, this,
			&ATanjiro::MoveAttackSlow, 0.2f, false);
		
		//BP_MoveAttack();
		GetWorld()->GetTimerManager().ClearTimer(MoveAttackTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(MoveAttackTimerHandle, this,
			&ATanjiro::MoveAttack, 0.4f, false);
		//bIsAimSetting =true;
		//AimTime = 0.f;
	}
}

void ATanjiro::MoveAttackSlow()
{
	SetGameSpeed(0.2f);
}


void ATanjiro::MoveAttack() // AimFocus단계
{
	CachedPPMaterialSetting(0.0f);
	SetGameSpeed(1.0f);
	AimTime= 0.f;
	bIsAimSetting = true;
	MoveAttackSubEffect();
	GetWorld()->GetTimerManager().ClearTimer(MoveAttackTimerHandle);
	//GetWorld()->GetTimerManager().ClearTimer(MoveAttackEffectTimerHandle);
	//GetWorld()->GetTimerManager().SetTimer(MoveAttackEffectTimerHandle, this,
	//&ATanjiro::MoveAttackSubEffect, FocusingTime, false);
	GetWorld()->GetTimerManager().SetTimer(MoveAttackTimerHandle, this,
			&ATanjiro::MoveAttack2, FocusingTime + 0.1f, false);
}

void ATanjiro::MoveAttackSubEffect()
{
	// 팡 터지는 소리랑 이펙트
}

void ATanjiro::MoveAttack2()
{
	bIsMoveAttacking =true;
	ElapsedTime = 0.f;
	PlayAnimationMontage(LeftAttack);
	MoveAttack_Start = GetActorLocation();
	FVector normal = FVector(GetActorLocation().X, GetActorLocation().Y, CurrentTargetOni->GetActorLocation().Z);
	FVector normalDir = (normal - CurrentTargetOni->GetActorLocation()).GetSafeNormal();
	MoveAttack_End  =  CurrentTargetOni->GetActorLocation() + (normalDir * 200.f);
	//DrawDebugSphere(GetWorld(), MoveAttack_End, 64.f,16, FColor::White, false, 2.0f);
	// 팡 소리도 나오게 하기
	/*
	GetWorld()->GetTimerManager().ClearTimer(MoveAttackTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(MoveAttackTimerHandle, this,
	&ATanjiro::MoveAttack2, MoveAttackTime * 0.5f, false);*/
}

void ATanjiro::ParringMotion(bool _value)
{
	//if (Parring_Start == nullptr) return;
	if (_value == true)
	{
		SetAnimState(EAnimState::Parring);
	}
	else
	{
		SetAnimState(EAnimState::Idle);
	}
	//PlayAnimMontage(Parring_Start);
}


void ATanjiro::localPlayMontage(UAnimMontage* montage)
{
	UAnimInstance* inst = GetMesh()->GetAnimInstance();
	inst->Montage_Play(montage);
}

AActor* ATanjiro::FindNearestEnemy(float _dist)
{
	//float ClosestDist = 200.f;
	AActor* Closest = nullptr;

	for (TActorIterator<AOni> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(TargetTag))
		{
			float Dist = FVector::Dist(GetActorLocation(), It->GetActorLocation());
			//DrawDebugLine(GetWorld(), GetActorLocation(), It->GetActorLocation(), FColor::Green, false, 1.0f);
			if (Dist < _dist)
			{
				Closest = *It;
				_dist = Dist;
			}
		}
	}
	return Closest;
}



AOni* ATanjiro::LineTraceFront() const
{
	float Distance=  2000.f;
	// 1) 카메라(플레이어 뷰포인트) 얻기
	FVector CamLoc; FRotator CamRot;
	if (AController* C = GetController())
	{
		// 플레이어인 경우: 실제 화면 카메라 기준(줌/FOV 반영)
		C->GetPlayerViewPoint(CamLoc, CamRot);
	}
	else if (FollowCamera)
	{
		// 컨트롤러가 아직 없으면 카메라 컴포넌트 기준
		CamLoc = FollowCamera->GetComponentLocation();
		CamRot = FollowCamera->GetComponentRotation();
	}
	else
	{
		// 최후 보루: Pawn 기본 시점
		CamLoc = GetPawnViewLocation();
		CamRot = GetViewRotation();
	}

	const FVector Start = CamLoc;
	const FVector End   = Start + CamRot.Vector() * Distance;

	// 2) 충돌 파라미터: 자기 자신(및 무기·자식 컴포넌트) 무시
	FCollisionQueryParams Params(SCENE_QUERY_STAT(FPLineTrace), /*bTraceComplex=*/true, this);
	Params.bReturnPhysicalMaterial = false; // 맞춘 표면 재질 필요 시
	// 필요하면 더 무시할 액터/컴포넌트를 추가
	// Params.AddIgnoredActor(WeaponActor);

	//DrawDebugLine(GetWorld(),Start,End,FColor::Green, false, 1.0f);
	FHitResult OutHit;
	TEnumAsByte<ECollisionChannel> Channel;
	// 3) 단일 라인트레이스
	const bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, Channel, Params);
	if (bHit)
	{
		//FVector point = OutHit.ImpactPoint;
		AOni* oni = TraceSphere(OutHit.ImpactPoint);
		if (oni != nullptr)
		{
			return oni;

		}
	}

	return nullptr;
}


AOni* ATanjiro::TraceSphere(FVector _point) const
{
	float _radius = 400.f;
	//FVector Forward = GetActorForwardVector();
	FVector CollisionStart = _point + FVector(-100.f, 0.f,0.f); //(GetActorForwardVector() * 50) + GetActorLocation();
	FVector CollisionEnd = _point  + FVector(100.f, 0.f,0.f); //(GetActorForwardVector() * 200) + GetActorLocation();

	TArray<FHitResult> OutHits;
	FRotator Orient = FRotator::ZeroRotator;
	FQuat Rot = Orient.Quaternion();
	FCollisionShape BoxShape = FCollisionShape::MakeSphere(_radius);
	// FCollisionQueryParams& Params;
	bool bHit = GetWorld()->SweepMultiByChannel(
		OutHits,
		CollisionStart,
		CollisionEnd,
		Rot,
		ECC_Visibility,
		BoxShape);
	if (bHit)
	{
		for (const FHitResult& HitActor : OutHits)
		{
			if (HitActor.GetActor()->ActorHasTag("Oni"))
			{
				return Cast<AOni>(HitActor.GetActor());
			}
		}
	}
	return nullptr;
}


void ATanjiro::SuccessCounter()
{
	//Super::SuccessCounter();
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue,TEXT("Success Counter"));
	if (ParringCameraShake) TanjiroController->ClientStartCameraShake(ParringCameraShake.Get());
	BP_ParringBackMove();
	FTransform SocketTF = GetKatanaSocket();
	//CurrentTargetOni->GetCounter(CurrentTargetOni->GetAttackType(), SocketTF);
	CurrentTargetOni->GetCounter(SocketTF);
	SoundComponent->Play2DHandle(ESoundName::effect_Parring);

	if (FX_Parring)
	{	
		//DrawDebugSphere(GetWorld(), ParringFX->GetComponentLocation(), 32.f, 156, FColor::Magenta, false, 3.0f);
		FX_Parring->ActivateSystem(true);
		GetWorld()->GetTimerManager().SetTimer(ParringEffectTimerHandle, this ,&ATanjiro::EndParringEffect, 0.5f ,false);
	}
}

void ATanjiro::TanjiroParring() //AttackType _enemyAttackType)
{
	// LMB 카운터는 적 공격 타입이 Right 일때 카운터 성공으로
	ParringMotion(true);
	CurrentTargetOni = Cast<AOni>(FindNearestEnemy(400.f));
	if (CurrentTargetOni)
	{
		if (CurrentTargetOni->CounterWindowIsOpen())//CurrentTargetOni->GetAttackType() == _enemyAttackType && )
		{
			if (CurrentTargetOni->GetAttackType() == EAttackType::Center)
			{
				SuccessCounter();
				return;
			}
			if (AttackType == EAttackType::LeftParring)
			{
				if (CurrentTargetOni->GetAttackType() == EAttackType::Right ||
					CurrentTargetOni->GetAttackType() == EAttackType::RightEnemyCombo) SuccessCounter();
			}
			else if (AttackType == EAttackType::RightParring)
			{
				if (CurrentTargetOni->GetAttackType() == EAttackType::Left ||
					CurrentTargetOni->GetAttackType() == EAttackType::LeftEnemyCombo) SuccessCounter(); 
			}
			
		}
	}
}


// 패링했을때 서로의 주먹 - 검 위치가 맞물리도록
FTransform ATanjiro::GetKatanaSocket() const
{
	if (!KatanaMesh) return FTransform::Identity;
	// 소켓 이름1
	static FName SocketName = TEXT("CounterSocket");
	// 소켓의 월드 위치
	//SocketLoc =
	//FVector outloc;
	//FRotator outRot;
	 FTransform SocketTransform = KatanaMesh->GetSocketTransform(SocketName, RTS_World);
	return SocketTransform;
	//KatanaMesh->GetSocketWorldLocationAndRotation(SocketName, outloc,outRot);
	//return outloc;
	// 소켓의 월드 회전
//	FRotator SocketRot = KatanaMesh->GetSocketRotation(SocketName);

	// 월드 변환(위치 + 회전 + 스케일)
//	 SocketTransform = KatanaMesh->GetSocketTransform(SocketName, RTS_World);
}

void ATanjiro::SpawnBoodActor()
{
	if (BloodEffectActor == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue,TEXT("BloodActor is None"));
		return;
	}
	FTransform tf =  GetKatanaSocket();
	FVector loc = CurrentTargetOni->GetActorLocation() + FVector(0.f,0.f, -50.f);
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* blood = GetWorld()->SpawnActor<AActor>(BloodEffectActor, loc,FRotator::ZeroRotator,param);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue,
	//	FString::Printf( TEXT("BloodActor : %s"), *blood->GetName()));
	//blood->lifetime(1.0f);
}

void ATanjiro::SpawnMoveAttackFireRoad(const FVector& _enemyLocation)
{
	if (BloodEffectActor == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue,TEXT("FireRoadActor is None"));
		return;
	}
	FVector loc = GetActorLocation()  - FVector(0.f, 0.f, 40.f);
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* road = GetWorld()->SpawnActor<AActor>(MoveAttackRoadEffectActor, GetActorLocation(),FRotator::ZeroRotator,param);
	USplineComponent* spline =  road->FindComponentByClass<USplineComponent>();
	TArray<FVector> WorldPoints;
	WorldPoints.Add(loc);
	WorldPoints.Add(_enemyLocation);

	spline->ClearSplinePoints(false);
	spline->SetSplinePoints(WorldPoints, ESplineCoordinateSpace::World,true); // true : UpdateSpline
	
}

void ATanjiro::EndParringEffect()
{
	FX_Parring->DeactivateSystem();
}

float ATanjiro::Ease01(float Alpha, float Exp) const
{
	Alpha = FMath::Clamp(Alpha, 0.f, 1.f);
	return FMath::InterpEaseInOut(0.f, 1.0f, Alpha, Exp);
}

void ATanjiro::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}
void ATanjiro::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}


void ATanjiro::DoJumpStart()
{
	//if (bJumpPressed) return;
	//bJumpPressed = true;
	
	//JumpKeyHoldTime+=;
}

void ATanjiro::DoJumpEnd()
{
	//bJumpPressed=  false;
	//Jump();
	
	//StopJumping();
}
