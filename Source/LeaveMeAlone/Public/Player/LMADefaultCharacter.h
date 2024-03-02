// LeaveMeAlone Game by Netologiya. All RightsReserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/LMAHealthComponent.h"
#include "Components/LMAWeaponComponent.h"
#include "LMADefaultCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ULMAHealthComponent;
class UAnimMontage;

UCLASS()
class LEAVEMEALONE_API ALMADefaultCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALMADefaultCharacter();

	UFUNCTION()
	ULMAHealthComponent* GetHealthComponent() const;
	
	UFUNCTION(BlueprintCallable)
	void PauseCharacter();

	
	UFUNCTION(BlueprintCallable)
	float GetStamina() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY()
	UDecalComponent* CurrentCursor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Zoom", 
		meta = (ClampMin = "20", UIMin = "20", ClampMax = "500", UIMax = "500"))
	float StepArmLength = 70.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Zoom",
		meta = (ClampMin = "500", UIMin = "500", ClampMax = "3000", UIMax = "3000"))
	float MaxArmLength = 2000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Zoom", 
		meta = (ClampMin = "300", UIMin = "300", ClampMax = "500", UIMax = "500"))
	float MinArmLength = 400.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components|Health")
	ULMAHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina")
	float MaxStamina = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina", meta = (ToolTip = "Decrease per StaminaTimerRate"))
	float IncreaseStaminaSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina", meta = (ToolTip = "Increase per StaminaTimerRate"))
	float DecreaseStaminaSpeed = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina")
	float StaminaTimerRate = 0.05f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	ULMAWeaponComponent* WeaponComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	float YRotation = -75.0f;  // отвечает за поворот камеры по оси Y.
	float ArmLength = 1400.0f; // отвечает за длину штатива.
	float FOV = 55.0f;		   // отвечает за поле зрения камеры.

	float Stamina = 100.0f;
	bool SprintState = false;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void ZoomIn();
	void ZoomOut();

	void OnDeath();
	void OnHealthChanged(float NewHealth);
	void RotationPlayerOnCursor();

	void SprintActivate();
	void SprintDeActivate();
	void DelayedSprintStart();

	void CalculateStamina();	
	bool StaminaIsFull();
	bool StaminaIsEmpty();
	FTimerHandle StaminaTimerHandle;
	FTimerHandle DelayedStartTimerHandle;
};
