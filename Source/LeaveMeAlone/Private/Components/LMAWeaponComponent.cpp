// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "Components/LMAWeaponComponent.h"
#include "Animations/LMAReloadFinishedAnimNotify.h"
#include "GameFramework/Character.h"
#include "Weapons/LMABaseWeapon.h"

// Sets default values for this component's properties
ULMAWeaponComponent::ULMAWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULMAWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();
	InitAnimNotify();	
}


void ULMAWeaponComponent::Fire() 
{
	if (IsValid(Weapon) && !AnimReloading && !FireProhibited)
	{
		Weapon->Fire();
	}
}

void ULMAWeaponComponent::StartLongFire()
{
	if (IsValid(Weapon))
	{
		LongFireActivated = true;
				
		if (!AnimReloading && !FireProhibited)
		{
			Weapon->StartLongFire();
		}		
	}	
}

void ULMAWeaponComponent::StopLongFire() 
{
	if (IsValid(Weapon))
	{
		Weapon->StopLongFire();
		LongFireActivated = false;
	}
}

void ULMAWeaponComponent::Reload()
{
	if (CanReload())
	{
		AnimReloading = true;
		if (LongFireActivated)
		{
			Weapon->StopLongFire();			
		}
		ACharacter* Character = Cast<ACharacter>(GetOwner());
		Character->PlayAnimMontage(ReloadMontage);		
	}	
}

void ULMAWeaponComponent::AutoReload() 
{
	Reload();		
}

void ULMAWeaponComponent::ManualReload() 
{
	Reload();
}

void ULMAWeaponComponent::FireProhibition() 
{
	FireProhibited = true;
	if (LongFireActivated)
	{
		Weapon->StopLongFire();
	}
}

void ULMAWeaponComponent::FirePermission() 
{
	FireProhibited = false;
	if (LongFireActivated)
	{
		StartLongFire();
	}
}

void ULMAWeaponComponent::ReloadProhibition() 
{
	ReloadProhibited = true;
}

void ULMAWeaponComponent::ReloadPermission() 
{
	ReloadProhibited = false;
}

bool ULMAWeaponComponent::IsReloading()
{
	return AnimReloading;
}

bool ULMAWeaponComponent::GetCurrentAmmoWeapon(FAmmoWeapon& AmmoWeapon) const
{	
	if (IsValid(Weapon))
	{
		AmmoWeapon = Weapon->GetCurrentAmmoWeapon();
		return true;
	}
	return false;	
}

// Called every frame
void ULMAWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void ULMAWeaponComponent::SpawnWeapon() 
{
	Weapon = GetWorld()->SpawnActor<ALMABaseWeapon>(WeaponClass);
	if (IsValid(Weapon))
	{
		const auto Character = Cast<ACharacter>(GetOwner());
		if (IsValid(Character))
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
			Weapon->AttachToComponent(Character->GetMesh(), AttachmentRules, "r_Weapon_Socket");
		}
		Weapon->OnClipEmpty.AddUObject(this, &ULMAWeaponComponent::AutoReload);
	}
}

void ULMAWeaponComponent::InitAnimNotify() 
{
	if (IsValid(ReloadMontage))
	{
		const auto NotifiesEvents = ReloadMontage->Notifies;
		for (auto NotifyEvent : NotifiesEvents)
		{
			auto ReloadFinish = Cast<ULMAReloadFinishedAnimNotify>(NotifyEvent.Notify);
			if (ReloadFinish)
			{
				ReloadFinish->OnNotifyReloadFinished.AddUObject(this, &ULMAWeaponComponent::OnNotifyReloadFinished);
				break;
			}
		}
	}
}

void ULMAWeaponComponent::OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh) 
{
	const auto Character = Cast<ACharacter>(GetOwner());
	if (Character->GetMesh() == SkeletalMesh)
	{
		AnimReloading = false;
		Weapon->ChangeClip();
		if (LongFireActivated)
		{
			StartLongFire();
		}
	}
}

bool ULMAWeaponComponent::CanReload() const
{
	return !AnimReloading && !(Weapon->IsCurrentClipFull()) && Weapon->IsClipAvailable() && !ReloadProhibited;
}

