// LeaveMeAlone Game by Netologiya. All RightsReserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LMAWeaponComponent.generated.h"

class ALMABaseWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEAVEMEALONE_API ULMAWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULMAWeaponComponent();
	
	void Fire();
	void StartLongFire();
	void StopLongFire();
	void ManualReload();
	void FireProhibition();
	void FirePermission();
	void ReloadProhibition();
	void ReloadPermission();
	bool IsReloading();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ALMABaseWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UAnimMontage* ReloadMontage;

	UFUNCTION(BlueprintCallable)
	bool GetCurrentAmmoWeapon(FAmmoWeapon& AmmoWeapon) const;

private:

	UPROPERTY()
	ALMABaseWeapon* Weapon = nullptr;	

	bool AnimReloading = false;
	bool LongFireActivated = false;
	bool FireProhibited = false;
	bool ReloadProhibited = false;

	void SpawnWeapon();

	void Reload();
	void AutoReload();
	void InitAnimNotify();
	void OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh);
	bool CanReload() const;	
	
};
