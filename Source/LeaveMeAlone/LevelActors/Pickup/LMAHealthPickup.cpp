// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "LevelActors/Pickup/LMAHealthPickup.h"
#include "Components/LMAHealthComponent.h"
#include "Components/SphereComponent.h"
#include "Player/LMADefaultCharacter.h"

// Sets default values
ALMAHealthPickup::ALMAHealthPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	SetRootComponent(SphereComponent);
}

void ALMAHealthPickup::NotifyActorBeginOverlap(AActor* OtherActor) 
{
	ALMADefaultCharacter *Player = Cast<ALMADefaultCharacter>(OtherActor);
	if (IsValid(Player))
	{
		Super::NotifyActorBeginOverlap(OtherActor);
		ALMADefaultCharacter* const Character = Cast<ALMADefaultCharacter>(OtherActor);		
		if (GivePickup(Character))
		{
			PickupWasTaken();
		}
	}	
}

bool ALMAHealthPickup::GivePickup(ALMADefaultCharacter* Character)
{
	ULMAHealthComponent *HealthComponent = Character->GetHealthComponent();
	if (!IsValid(HealthComponent))
	{
		return false;
	}
	return HealthComponent->AddHealth(HealthFromPickup);
	
}

void ALMAHealthPickup::PickupWasTaken() 
{
	Destroy();
	//SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//GetRootComponent()->SetVisibility(false, true);
	//FTimerHandle RespawnTimerHandle;
	//GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ALMAHealthPickup::RespawnPickup, RespawnTime);
}

void ALMAHealthPickup::RespawnPickup() 
{
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	GetRootComponent()->SetVisibility(true, true);
}

