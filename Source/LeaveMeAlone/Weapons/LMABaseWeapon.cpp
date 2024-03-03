// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "Weapons/LMABaseWeapon.h"
#include "Player/LMADefaultCharacter.h"
#include "Kismet/GameplayStatics.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeapon, All, All);



// Sets default values
ALMABaseWeapon::ALMABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	WeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	SetRootComponent(WeaponComponent);

}

void ALMABaseWeapon::Fire() 
{
	if (CurrentAmmoWeapon.Bullets > 0)
	{
		Shoot();
	}	
}

void ALMABaseWeapon::StartLongFire() 
{
	Fire();
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ALMABaseWeapon::Fire, FireTimerRate, true);
}

void ALMABaseWeapon::StopLongFire() 
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ALMABaseWeapon::ChangeClip() 
{
	if (AmmoWeapon.Infinite)
	{
		CurrentAmmoWeapon.Bullets = AmmoWeapon.Bullets;
	}
	else if (CurrentAmmoWeapon.Clips > 0)
	{
		CurrentAmmoWeapon.Bullets = AmmoWeapon.Bullets;
		--CurrentAmmoWeapon.Clips;
	}		
}

bool ALMABaseWeapon::IsCurrentClipFull() 
{
	return CurrentAmmoWeapon.Bullets == AmmoWeapon.Bullets;
}

bool ALMABaseWeapon::IsClipAvailable()
{
	return CurrentAmmoWeapon.Clips > 0 || AmmoWeapon.Infinite;
}

FAmmoWeapon ALMABaseWeapon::GetCurrentAmmoWeapon() const
{
	return CurrentAmmoWeapon;	
}

// Called when the game starts or when spawned
void ALMABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmoWeapon = AmmoWeapon;
}

void ALMABaseWeapon::SpawnTrace(const FVector& TraceStart, const FVector& TraceEnd)
{
	UNiagaraComponent* const TraceFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceEffect, TraceStart);
	if (IsValid(TraceFX))
	{
		TraceFX->SetNiagaraVariableVec3(TraceName, TraceEnd);
	}
}

void ALMABaseWeapon::SpawnFireHit(const FVector& HitVector, const FVector& ContrFireVector)
{
	UNiagaraComponent* const TraceFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FireHitEffect, HitVector);
	if (IsValid(TraceFX))
	{
		TraceFX->SetNiagaraVariableVec3(FireHitDirection, ContrFireVector);
	}
}

void ALMABaseWeapon::MakeDamage(const FHitResult& HitResult) 
{
	AActor *const Zombie = HitResult.GetActor();
	if (!Zombie) return;
	APawn *const Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Pawn) return;
	APlayerController *const Controller = Pawn->GetController<APlayerController>();
	if (!Controller) return;
	
	Zombie->TakeDamage(Damage, FDamageEvent(), Controller, this);
}

void ALMABaseWeapon::Shoot()
{

	if (IsValid(FireMontage))
	{
		ACharacter *Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);		
		if (IsValid(Player))
		{
			Player->PlayAnimMontage(FireMontage);
		}
	}
	const FTransform SocketTransform = WeaponComponent->GetSocketTransform("Muzzle");
	const FVector TraceStart = SocketTransform.GetLocation();
	const FVector ShootDirection = SocketTransform.GetRotation().GetForwardVector();
	const FVector TraceEnd = TraceStart + ShootDirection * TraceDistance;
	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Black, false, 1.0f, 0, 2.0f);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootWave, TraceStart);

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
	FVector TracerEnd = TraceEnd;
	FVector ContrFire = TraceStart - TraceEnd;
	if (HitResult.bBlockingHit)
	{
		//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.0f, 24, FColor::Red, false, 1.0f);
		MakeDamage(HitResult);
		TracerEnd = HitResult.ImpactPoint;
		SpawnFireHit(TracerEnd, ContrFire);
	}
	UNiagaraComponent *const TraceFX1 = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FireEffect, TraceStart);
	SpawnTrace(TraceStart, TracerEnd);	
	
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootWave, TraceStart);

	DecrementBullets();
}

void ALMABaseWeapon::DecrementBullets() 
{
	CurrentAmmoWeapon.Bullets--;
	//UE_LOG(LogWeapon, Display, TEXT("Bullets = %s"), *FString::FromInt(CurrentAmmoWeapon.Bullets));

	if (IsCurrentClipEmpty())
	{
		OnClipEmpty.Broadcast();		
	}
}

bool ALMABaseWeapon::IsCurrentClipEmpty() const
{
	return CurrentAmmoWeapon.Bullets == 0;
}

