// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupItem.h"

#include "Components/SphereComponent.h"
#include "Tanjiro/Tanjiro.h"
#include "Tanjiro/Components/InventoryComponent.h"

// Sets default values
APickupItem::APickupItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SC = CreateDefaultSubobject<USceneComponent>(TEXT("SC"));
	RootComponent = SC;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SC);
	SphereCollision =CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SphereCollision->SetupAttachment(SC);
	
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &APickupItem::OnOverlap);
}

// Called when the game starts or when spawned
void APickupItem::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickupItem::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Tanjiro"))
	{
		ATanjiro* tanjiro = Cast<ATanjiro>(OtherActor);
		const int32 Left = tanjiro->InventoryComponent->AddById(ItemId, Count); // 남은 수(Left) 반환
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White,
				FString::Printf(TEXT("Picked %s x%d (Left:%d) by %s"),
					*ItemId.ToString(), Count, Left, *OtherActor->GetName()));
		}
		// UI 갱신은 Inv 안에서 BroadcastChanged()가 호출되어 위젯이 자동 Refresh됨
		// 남은 아이템이 없다면 제거
		if (Left <= 0)
		{
			Destroy();
		}
		else
		{
			Count = Left;
			// TODO: 남은 수량을 월드 메시/위젯으로 갱신해주고 싶다면 여기서 처리
		}
	}
}

