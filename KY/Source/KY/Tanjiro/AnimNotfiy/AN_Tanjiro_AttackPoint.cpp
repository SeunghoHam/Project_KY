// Fill out your copyright notice in the Description page of Project Settings.


#include "Tanjiro/AnimNotfiy/AN_Tanjiro_AttackPoint.h"

#include "Tanjiro/Tanjiro.h"

void UAN_Tanjiro_AttackPoint::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (ATanjiro* tanjiro = Cast<ATanjiro>( MeshComp->GetOwner()))
	{
		tanjiro->AN_AttackPoint();
	}
}
