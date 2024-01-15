// LeaveMeAlone Game by Netologiya. All RightsReserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LMADefaultCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class LEAVEMEALONE_API ALMADefaultCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALMADefaultCharacter();

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
		meta = (ClampMin = "500", UIMin = "500", ClampMax = "2000", UIMax = "2000"))
	float MaxArmLength = 1400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Zoom", 
		meta = (ClampMin = "300", UIMin = "300", ClampMax = "500", UIMax = "500"))
	float MinArmLength = 400.0f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	float YRotation = -75.0f;  // отвечает за поворот камеры по оси Y.
	float ArmLength = 1400.0f; // отвечает за длину штатива.
	float FOV = 55.0f;		   // отвечает за поле зрения камеры.

	void MoveForward(float Value);
	void MoveRight(float Value);

	void ZoomIn();
	void ZoomOut();
};
