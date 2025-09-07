// Fill out your copyright notice in the Description page of Project Settings.


#include "Oni.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/Animation/OniAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h" // GameFramekwork
#include "Item/PickupItem.h"
#include "Tanjiro/Tanjiro.h"
#include "Kismet/GameplayStatics.h"
#include "System/EnemyAttackState.h"
#include "System/EnemyRegistrySubsystem.h"
//#include ""

AOni::AOni()
{
	//OniAnimationComponent = CreateDefaultSubobject<UAnimationControlComponent>(TEXT("OniAnimationComponent"));
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->bUseControllerDesiredRotation = true;
		Move->bOrientRotationToMovement    = false;
		Move->RotationRate = FRotator(0, 720.f, 0); // 즉시 회전 가속
		Move->MaxWalkSpeed= 900.f; // 기본 600
	}
}

void AOni::BeginPlay()
{
	Super::BeginPlay();

	Target =  UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
	AnimInstance=  Cast<UOniAnimInstance>(GetMesh()->GetAnimInstance());
	
	RS = GetGameInstance()->GetSubsystem<UEnemyRegistrySubsystem>();
	RegisterEnemy();
	StatusInitialize(20.f, 10.f);
	TargetTag = "Tanjiro";
	//GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle,this,
	//&AOni::AttackMontage, 2.0f, true);
	//Target = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
	bCanAttack = false; // BT 조절함
}

void AOni::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (RS)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,TEXT("EndPlay"));
		RS->UnregisterEnemy(this);
	}
	Super::EndPlay(EndPlayReason);
}

void AOni::GetCounter(EAttackType _type, const FTransform& _socketPoint)
{
	//Super::GetCounter(_type, _socketPoint);
	AnimInstance->EffectorLocation= _socketPoint.GetLocation();
	AnimInstance->EffectorTransform  = _socketPoint;
	SetGameSpeed(0.3f);
	switch (_type)
	{
	case EAttackType::Left:
		AnimInstance->IK_L_Alpha = 1.0f;
		CurrentPlayMontage = LeftCounter;
		break;
	case EAttackType::LeftEnemyCombo:
		AnimInstance->IK_L_Alpha = 1.0f;
		CurrentPlayMontage = LeftComboCounter;
	case EAttackType::Right:
		AnimInstance->IK_R_Alpha = 1.0f;
		CurrentPlayMontage = RightCounter;	
		break;
	case EAttackType::RightEnemyCombo:
		AnimInstance->IK_R_Alpha = 1.0f;
		CurrentPlayMontage = RightComboCounter;
		break;
		default:
		CurrentPlayMontage = nullptr;
			break;
	}
	if (CurrentPlayMontage)
	{
		PlayAnimationMontage(CurrentPlayMontage);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red,TEXT("[Oni] CounterAnim =null "));
	}
	//GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	
	TWeakObjectPtr<AOni> WeakThis(this);
	FTimerHandle timeHandle;
	GetWorld()->GetTimerManager().SetTimer(timeHandle, [WeakThis, this]()
	{
		if (!WeakThis.IsValid()) return;
		AN_CounterEnd();
	}, 0.25f, false);
	
	
	//bIsTreeActive = false;
	//GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle,this,
	//&AOni::AttackMontage, 2.0f, true);
}

void AOni::AttackMontage()
{
	AnimInstance->IK_L_Alpha = 0.f;
	AnimInstance->IK_R_Alpha = 0.f;
	if (bAttackLeft)
	{
		//CurrentPlayMontage =  OniAnimationComponent->PlayByName("RightAttack");
		//PlayMontage(RightAttack);
		PlayAnimationMontage(RightAttack);
		ChangeAttackType(EAttackType::Right);
		bAttackLeft = false;
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f,FColor::White, TEXT("AttackType : Right"));
		
	}
	else
	{
		//PlayMontage(LeftAttack);
		//CurrentPlayMontage =  OniAnimationComponent->PlayByName("LeftAttack");
		PlayAnimationMontage(LeftAttack);
		ChangeAttackType(EAttackType::Left);
		bAttackLeft =true;
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f,FColor::White, TEXT("AttackType : Left"));
		
	}
}

void AOni::CheckCurrentHP()
{
	Super::CheckCurrentHP();

	GEngine->AddOnScreenDebugMessage(-1, 1.0f,FColor::Magenta,
	FString::Printf( TEXT("CheckCurrentHP : %f"), CurrentHP));


	if (CurrentHP <= 0)
	{
		Death();
	}
}

void AOni::Death()
{
	Super::Death();
	bCanAttack = false;
	//bIsTreeActive = false;
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn ;
	APickupItem* pickupItem = GetWorld()->SpawnActor<APickupItem>(DeathItemActor,GetActorLocation(), GetActorRotation(),param);
	AnimInstance->AnimType = EAnimType::Die;
	RS->UnregisterEnemy(this);
	
	//PlayAnimationMontage(DeathAnim);
	// 픽업아이템이 있으면 생성하기
	if (pickupItem)
	{
		
	}
	
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this,
		&AOni::DestroyOni, 5.0f ,false);
	
	//GetMesh()->SetSimulatePhysics(true);
}

FVector AOni::GetTanjiroLastLocation()
{
	if (Target == nullptr)
	{
		Target =  UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
		
		GEngine->AddOnScreenDebugMessage(-1, 1.0f,FColor::Magenta,
FString::Printf( TEXT("Tanjiro = null :")));
		return GetActorLocation();
	}

	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (FX_SurpriseAttackActor)
	{
		GetWorld()->SpawnActor<AActor>(FX_SurpriseAttackActor, GetActorLocation(),GetActorRotation(), param);
	}
	SurpriseAttackLoc = SurpriseHide();
	GetMesh()->SetVisibility(false);
	
	//const FRotator rot = ((Target->GetActorLocation() - NormalLoc).GetSafeNormal()).Rotation();
	//float yawOnly = rot.Yaw;
	//FQuat newQ = FRotator(0.f,yawOnly,0.f).Quaternion();
	//SetActorRotation(newQ);
	

	//FVector NormalLoc = FVector(GetActorLocation().X, GetActorLocation().Y, Target->GetActorLocation().Z);
	FVector Dir = Target->GetActorLocation() - GetActorLocation();
	FRotator YawOnly = Dir.Rotation();
	YawOnly.Pitch = 0.f;
	YawOnly.Roll  = 0.f;
	
	if (AController* C = GetController())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f,FColor::White,TEXT("Controller Rotaoitn"));
		C->SetControlRotation(YawOnly);
		SetActorRotation(YawOnly);
	}
	
	return SurpriseAttackLoc;
}

void AOni::OniSurpriseAttack()
{
	// 이펙트랑 같이 나오기
	GetMesh()->SetVisibility(true);
	
	//GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Magenta, TEXT("[Oni] SurpriseAttack"));

	if (!Target) return;
	DrawDebugSphere(GetWorld(), SurpriseAttackLoc, 32.f, 16,FColor::Red,false,2.0f);
	DrawDebugLine(GetWorld(),GetActorLocation(), SurpriseAttackLoc,FColor::Red,false, 2.0f, 0 ,2.0f);
	SetActorLocation(SurpriseAttackLoc, false);
	//SetActorRotation(SurpriseAttackRot);
	
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (FX_SurpriseAttackActor)
	{
		GetWorld()->SpawnActor<AActor>(FX_SurpriseAttackActor, SurpriseAttackLoc, SurpriseAttackRot, param);
	}
	if (SurpriseAttack)
	{
		PlayAnimationMontage(SurpriseAttack);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red, TEXT("[Oni] SurpriseAttackMontage Null"));
		UE_LOG(LogTemp, Warning, TEXT("[Oni] SurpriseAttackMontage Null"));
	}
}

FVector AOni::SurpriseHide()
{
	int32 ranX = FMath::RandRange(0, 1);
	int32 ranY = FMath::RandRange(0, 1);

	float Amount = 150.f;
	float xValue = 0, yValue =0;
	if (ranX ==0) xValue = -Amount;
	else xValue = Amount;
	if (ranY ==0) yValue = -Amount;
	else yValue = Amount;
	
	FVector CreatePoint = FVector(
	Target->GetActorLocation().X + xValue,
	Target->GetActorLocation().Y +yValue,
	Target->GetActorLocation().Z  + 200.f);
	return CreatePoint;
}

void AOni::TaskComboAttack()
{
	// z 위치 통일화
	if (Target == nullptr) return;
	// 1) 나 → 타겟 방향(수평면만)
	SetAttackDir(); 
	
	AnimInstance->AnimType = EAnimType::Punch1;
	AttackType = EAttackType::Right;
	ComboAttackCount =0;
}

AActor* AOni::GetTanjiroActor()
{
	if (Target == nullptr) return nullptr;
	
	ATanjiro* tan = Cast<ATanjiro>(Target);
	FText targetN = FText::FromString(Target->GetName());
	tan->OnOniStateChange.Broadcast(targetN, bCanAttack);

	SetAttackDir();
	return Target;
}

float AOni::GetDistance()
{
	if (Target == nullptr) return 0.f;
	return FVector::Dist(Target->GetActorLocation(), GetActorLocation());
}

void AOni::AN_ComboAttackEnd()
{
	ComboAttackCount++;
	if (ComboAttackCount == 1)
	{
		SetAttackDir();
		AnimInstance->AnimType = EAnimType::Punch2;
		AttackType = EAttackType::Left;
	}
	else if (ComboAttackCount == 2)
	{
		SetAttackDir();
		AnimInstance->AnimType = EAnimType::Kick;
		AttackType = EAttackType::Center;
	}
	else if (ComboAttackCount == 3)
	{
		AnimInstance->AnimType = EAnimType::Idle;
	}
	
	DrawDebugLine(GetWorld(),GetActorLocation(), Target->GetActorLocation(),
		FColor::Green,false ,1.0f, 0, 2.0f);

}

void AOni::AN_ComboAttackPoint()
{
	OniAttack(); // Collision확인
	SetAttackDir();
	float dist =FVector::Dist(Target->GetActorLocation(), GetActorLocation());
	UE_LOG(LogTemp, Error, TEXT("Dist : %f"), dist);
	if (dist >= 150.0f)
	{
		switch (ComboAttackCount)
		{
		case 0:
			BP_AttackDirMove(80.f);
			break;
		case 1:
			BP_AttackDirMove(80.f);
			break;
		case 2:
			BP_AttackDirMove(20.f);
			break;
		case 3 :
			//BP_AttackDirMove(100.f);
			break;
			default:
			break;
		}
	}
}

void AOni::AN_CounterEnd()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f,FColor::White,TEXT("AN_CounterENd"));
	AnimInstance->IK_L_Alpha = 0.f;
	AnimInstance->IK_R_Alpha = 0.f;
	SetGameSpeed(1.f);
	AnimInstance->Montage_Stop(0.2f, CurrentPlayMontage);
}


void AOni::ChangePPOption(bool _enable)
{
	if (_enable)
	{
		GetMesh()->SetRenderCustomDepth(true);
		GetMesh()->CustomDepthStencilValue= 1;
		//GetMesh()->bRenderCustomDepth = false;
	}
	else
	{
		GetMesh()->SetRenderCustomDepth(false);
		GetMesh()->CustomDepthStencilValue= 0;
		
	}
}

void AOni::DestroyOni()
{
	Destroy();
}

void AOni::RegisterEnemy()
{
	UGameInstance* GI = GetGameInstance();
	UE_LOG(LogTemp, Warning, TEXT("[Enemy] GI=%s"), GI ? *GI->GetName() : TEXT("Game Instannce nullptr"));
	if (GI)
	{
		if (auto* R = GI->GetSubsystem<UEnemyRegistrySubsystem>())
		{
			R->RegisterEnemy(this);
			UE_LOG(LogTemp, Warning, TEXT("[Enemy]Registry OK"));
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Registry OK"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Enemy] Registry NOT found, retry next frame"));
			FTimerHandle Th;
			GetWorld()->GetTimerManager().SetTimer(Th, [this]()
			{
				if (auto* GI2 = GetGameInstance())
					if (auto* R2 = GI2->GetSubsystem<UEnemyRegistrySubsystem>())
					{
						R2->RegisterEnemy(this);
						UE_LOG(LogTemp, Warning, TEXT("[Enemy][Retry] Registry OK"));
					}
			}, 0.f, false);
		}
	}
	RS->RegisterEnemy(this);
}

void AOni::OniAttack()
{
	FVector halfSize = FVector(50.f, 50.f, 25.f);
	float end = 200.0f;
	AActor* ColTarget = CreateCheckCollision(GetActorForwardVector(),halfSize,150.f );
	if (ColTarget)
	{
		UGameplayStatics::ApplyDamage(
			ColTarget,
			10.f,
			UGameplayStatics::GetPlayerController(GetWorld(), 0),
			this,
			UDamageType::StaticClass());
	}
}

void AOni::SetAnimType(EAnimType _type)
{
	if (!AnimInstance) return;
	AnimInstance->AnimType = _type;
}

void AOni::SetAttackDir()
{
	AttackDir = Target->GetActorLocation() - GetActorLocation();
	AttackDir.Z = 0.f;
	if (AttackDir.IsNearlyZero()) return;
	
	FRotator YawOnly = AttackDir.Rotation();
	YawOnly.Pitch = 0.f;
	YawOnly.Roll  = 0.f;
	
	if (AController* C = GetController())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f,FColor::White,TEXT("Controller Rotaoitn"));
		C->SetControlRotation(YawOnly);
	}
	// 2) 회전 적용
	// ACharacter 계열이면 Controller를 돌리는 편이 안전
	/*
	if (ACharacter* Char = Cast<ACharacter>(this))
	{
		if (AController* C = Char->GetController())
		{
			C->SetControlRotation(YawOnly);
			// 회전을 컨트롤러가 주도하도록 세팅(필요시 한 번만 설정)
			if (UCharacterMovementComponent* Move = Char->GetCharacterMovement())
			{
				Move->bUseControllerDesiredRotation = true;
				Move->bOrientRotationToMovement    = false;
			}
		}
		else SetActorRotation(YawOnly);
	}
	else SetActorRotation(YawOnly);*/
	// return AttackDir;
}

void AOni::OnRequestAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("[Oni] OnRequestAttack"));

	// Possess 시간 주기
	TWeakObjectPtr<AOni> WeakThis(this);
	FTimerHandle timeHandle;
	GetWorld()->GetTimerManager().SetTimer(timeHandle, [WeakThis, this]()
	{
		if (!WeakThis.IsValid()) return;
		bCanAttack = true;
	}, 0.35f, false);
	
}

void AOni::OnReleaseAttack()
{
	AEnemyAttackState* EAS = Cast<AEnemyAttackState> (GetWorld()->GetGameState());
	EAS->ReleaseAttack(this);
}
