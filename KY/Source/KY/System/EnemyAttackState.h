// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EnemyAttackState.generated.h"

/**
 * 
 */
class AOni;
struct FTimerHandle;
class UEnemyRegistrySubsystem;
UCLASS()
class KY_API AEnemyAttackState : public AGameStateBase
{
	GENERATED_BODY()
protected:
    virtual void BeginPlay() override;
    //virtual void Tick(float DeltaSeconds) override;
public:
    
	  // 요청: 내가 공격해도 될까?
    UFUNCTION(BlueprintCallable)
    bool RequestAttack(AOni* Enemy);
    
    // 종료: 내 공격 끝났어
    UFUNCTION(BlueprintCallable)
    void ReleaseAttack(AOni* Enemy);

    FTimerHandle CheckTimer;
    void CheckCurrentAttakcStateQueue();

    // 적이 파괴되거나 비활성화될 때 안전 처리
    void OnEnemyRemoved(AOni* Enemy);

    AOni* GetCurrentAttacker() const { return CurrentAttacker.Get(); }

private:
    UPROPERTY() TWeakObjectPtr<AOni> CurrentAttacker;
    UPROPERTY() TArray<TWeakObjectPtr<AOni>> WaitQueue; // 간단히 FIFO. 필요시 힙/정렬로 교체

    void HandleEnemyRegistered(AOni* actor);
    void HandleEnemyUnregistered(AOni* actor);
private:
    void Grant(AOni* Enemy); // 공격권 부여 
    void Enqueue(AOni* Enemy);
    void RemoveFromQueue(AOni* Enemy);

    void PromoteNext();

    // 거리/각도/시야/쿨다운 점수를 섞어 Best 선정
    int32 SelectBestCandidateIndex();

    float ComputeAttackScore(AOni* E) const;
    void CleanupInvalids();
};
