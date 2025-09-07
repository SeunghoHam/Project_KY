// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterBase.h"
#include "Tanjiro/Components/SoundComponent.h"
#include "GameFramework/Character.h"
#include "DataAsset/TanjiroAnimMontageMap.h"
#include "Item/PickupItem.h"
#include "Kismet/GameplayStatics.h"

//#include "Tanjiro/Components/AnimationControlComponent.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SoundComponent = CreateDefaultSubobject<USoundComponent>(TEXT("SoundComponent"));
	
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

float ACharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	GetDamage = DamageAmount;
	CheckCurrentHP();
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
/*
void ACharacterBase::PlayMontage(UAnimMontage* montage)
{
	
}
*/

void ACharacterBase::PlayAnimationMontage(UAnimMontage* montage)
{
	if (!montage) return;
	UAnimInstance* inst = GetMesh()->GetAnimInstance();
	inst->Montage_Play(montage);
}

void ACharacterBase::OpenCounterWindow()
{
	bIsCanCounter =true;
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f,FColor::White, TEXT("Counter : open"));
}

void ACharacterBase::CloseCounterWindow()
{
	bIsCanCounter = false;
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f,FColor::White, TEXT("Counter : close"));
}

AActor* ACharacterBase::AttackPoint(FVector _dir,FVector _halfSize, float _end)
{
	return CreateCheckCollision(_dir,_halfSize, _end);	
}

AActor* ACharacterBase::CreateCheckCollision(FVector _dir, FVector _halfSize, float _end)
{
	//_halfSize = FVector(50.f, 50.f, 25.0f);
	//_end = 200.f;
	//FVector Forward = GetActorForwardVector();
	const FVector F = _dir;
	const FVector CollisionStart = GetActorLocation() + F * 50.f;
	const FVector CollisionEnd = GetActorLocation() + F * _end;
	

	TArray<FHitResult> OutHits;
	FRotator Orient = GetActorRotation();
	FQuat Rot = Orient.Quaternion();

	FCollisionShape BoxShape = FCollisionShape::MakeBox(_halfSize);
	// FCollisionQueryParams& Params;
	bool bHit = GetWorld()->SweepMultiByChannel(
		OutHits,
		CollisionStart,
		CollisionEnd,
		Rot,
		ECC_Visibility,
		BoxShape);

	const float Travel = FMath::Max(_end - 50.f, 0.f);
	const float TotalLen = Travel + 2.f * _halfSize.X; // = Length
	const float HalfTotalLen = TotalLen * 0.5f; // = HalfX + Travel/2
	const FVector CoverCenter = CollisionStart + F * HalfTotalLen;
	const FVector CoverExtent = FVector(HalfTotalLen, _halfSize.Y, _halfSize.Z);
	
	//DrawDebugSphere(GetWorld(), CollisionStart, 50.0f, 16, FColor::Blue, false, 5.0f);
	//DrawDebugSphere(GetWorld(), CollisionEnd, 50.0f, 16, FColor::Green, false, 5.0f);
	DrawDebugBox(GetWorld(), CoverCenter, CoverExtent, Rot, FColor::Blue, false, 5.0f);
	if (bHit)
	{
		for (const FHitResult& HitActor : OutHits)
		{
			if (HitActor.GetActor()->ActorHasTag(TargetTag))
			{
				return HitActor.GetActor();
			}
		}
	}
	return nullptr;
}




void ACharacterBase::SuccessCounter()
{
	//AnimationComponent->StopMontage(CurrentPlayMontage);
}

// _type은 공격쪽. Right라면 Left가 실행되도록
void ACharacterBase::GetCounter(EAttackType _type,const FTransform& _socketPoint)
{

	//AnimationComponent->StopMontage(CurrentPlayMontage);
}

void ACharacterBase::CheckCurrentHP()
{
	CurrentHP = FMath::Clamp(CurrentHP - GetDamage, 0.f, MaxHP);
	if (CurrentHP <= 0.f)
	{
		Death();
	}
}


void ACharacterBase::StatusInitialize(float maxHp, float maxMp)
{
	MaxHP = maxHp;
	MaxMP = maxMp;
	CurrentHP = maxHp;
	CurrentMP = maxMp;
}

void ACharacterBase::Death()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red,TEXT("죽었다"));
}


// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);
}

void ACharacterBase::SetGameSpeed(float _speed)
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), _speed);
}

