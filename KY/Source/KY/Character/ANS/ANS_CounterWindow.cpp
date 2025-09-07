// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ANS/ANS_CounterWindow.h"
#include "Character/CharacterBase.h"
void UANS_CounterWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (ACharacterBase* owner = Cast<ACharacterBase>( MeshComp->GetOwner()))
	{
		owner->OpenCounterWindow();
	}
}

void UANS_CounterWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (ACharacterBase* owner = Cast<ACharacterBase>( MeshComp->GetOwner()))
	{
		owner->CloseCounterWindow();
	}
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
