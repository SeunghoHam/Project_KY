// Fill out your copyright notice in the Description page of Project Settings.


#include "System/EnemyAttackState.h"
#include "System/EnemyRegistrySubsystem.h"
#include "Enemy/Oni.h"

void AEnemyAttackState::BeginPlay()
{
	Super::BeginPlay();
	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		if (auto* R = GI->GetSubsystem<UEnemyRegistrySubsystem>())
		{
			R->OnEnemyRegistered.AddUObject(this, &AEnemyAttackState::HandleEnemyRegistered);
			R->OnEnemyUnregistered.AddUObject(this, &AEnemyAttackState::HandleEnemyUnregistered);
			UE_LOG(LogTemp, Warning, TEXT("[Enemy] Subsystem=%s"), R ? *R->GetName() : TEXT("nullptr"));
		}
	}
	else
	{
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(CheckTimer, this ,&AEnemyAttackState::CheckCurrentAttakcStateQueue, 2.0f, true);
}

bool AEnemyAttackState::RequestAttack(AOni* Enemy)
{
	UE_LOG(LogTemp, Warning, TEXT("[EAS] RequestAttack"));
	CleanupInvalids();
	if (!CurrentAttacker.IsValid())
	{
		Grant(Enemy);
		return true;
	}
	
	// 이미 누군가 공격 중 → 대기열에 넣고 거절
	Enqueue(Enemy);
	return false;
}

void AEnemyAttackState::ReleaseAttack(AOni* Enemy)
{
	if (CurrentAttacker.Get() == Enemy)
	{
		CurrentAttacker = nullptr;
		PromoteNext();
	}
	else
	{
		// 대기열에 있었다면 제거
		RemoveFromQueue(Enemy);
	}
}

void AEnemyAttackState::CheckCurrentAttakcStateQueue()
{
	if (CurrentAttacker != nullptr)
		UE_LOG(LogTemp, Warning, TEXT("Count : %d, Name : %s"), WaitQueue.Num(), *CurrentAttacker->GetName());
}

void AEnemyAttackState::OnEnemyRemoved(AOni* Enemy)
{
	if (CurrentAttacker.Get() == Enemy)
	{
		CurrentAttacker = nullptr;
	}
	RemoveFromQueue(Enemy);
	PromoteNext();
}

void AEnemyAttackState::HandleEnemyRegistered(AOni* actor)
{
	//GEngine->AddOnwScreenDebugMessage(-1,1.0f, FColor::White, TEXT("[State] EnemyRegister"));
	UE_LOG(LogTemp, Warning, TEXT("[GameState] HandleRegister"))
	RequestAttack(actor);
}

void AEnemyAttackState::HandleEnemyUnregistered(AOni* actor)
{
	OnEnemyRemoved(actor);
}

// 공격권 부여: Enemy는 여기서 BT 키 세팅/모션 시작
void AEnemyAttackState::Grant(AOni* Enemy)
{
	UE_LOG(LogTemp, Warning, TEXT("[EAS] Grant : %s"), *Enemy->GetName());
	CurrentAttacker = Enemy;
	CurrentAttacker->OnRequestAttack();
	// 예: 인터페이스/이벤트/BT 블랙보드로 "Attacking=true" 전달
	// IEnemyCombatInterface::Execute_OnAttackGranted(Enemy);
}

void AEnemyAttackState::Enqueue(AOni* Enemy)
{
	if (!Enemy) return;
	if (WaitQueue.Contains(Enemy)) return;
	WaitQueue.Add(Enemy);
}

void AEnemyAttackState::RemoveFromQueue(AOni* Enemy)
{
	WaitQueue.Remove(TWeakObjectPtr<AOni>(Enemy));
}

void AEnemyAttackState::PromoteNext()
{
	CleanupInvalids();

	if (CurrentAttacker.IsValid()) return;
	if (WaitQueue.Num() == 0) return;

	// 우선순위가 필요하면 여기서 정렬/선택
	int32 BestIdx = SelectBestCandidateIndex(); // 아래 참고
	if (BestIdx != INDEX_NONE)
	{
		TWeakObjectPtr<AOni> Next = WaitQueue[BestIdx];
		WaitQueue.RemoveAtSwap(BestIdx);
		if (Next.IsValid()) Grant(Next.Get());
	}
}

int32 AEnemyAttackState::SelectBestCandidateIndex()
{
	int32 BestIdx = INDEX_NONE;
	float BestScore = -FLT_MAX;

	for (int32 i=0; i<WaitQueue.Num(); ++i)
	{
		AOni* E = WaitQueue[i].Get();
		if (!E) continue;

		const float Score = ComputeAttackScore(E); // 아래 참고
		if (Score > BestScore)
		{
			BestScore = Score;
			BestIdx = i;
		}
	}
	return BestIdx;
}

float AEnemyAttackState::ComputeAttackScore(AOni* E) const
{
	// 예시: 플레이어와의 거리 역수 + 시야 보너스 + 역할 보너스 - 최근공격 패널티
	// 실제 게임 데이터로 치환
	return 0.0f;
}

void AEnemyAttackState::CleanupInvalids()
{
	if (CurrentAttacker.IsValid() == false) CurrentAttacker = nullptr;
	WaitQueue.RemoveAll([](const TWeakObjectPtr<AOni>& P){ return !P.IsValid(); });
}
