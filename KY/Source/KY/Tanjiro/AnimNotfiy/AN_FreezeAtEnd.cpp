// Fill out your copyright notice in the Description page of Project Settings.


#include "Tanjiro/AnimNotfiy/AN_FreezeAtEnd.h"
#include"Tanjiro/Tanjiro.h"
void UAN_FreezeAtEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (ATanjiro* owner= Cast<ATanjiro>(MeshComp->GetOwner()))
	{
		owner->AN_FreezeAtEnd();
	}
}
