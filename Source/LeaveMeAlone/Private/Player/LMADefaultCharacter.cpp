// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "Player/LMADefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ALMADefaultCharacter::ALMADefaultCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true);	
	SpringArmComponent->TargetArmLength = ArmLength;
	SpringArmComponent->SetRelativeRotation(FRotator(YRotation, 0.0f, 0.0f));
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bEnableCameraLag = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetFieldOfView(FOV);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");
}

ULMAHealthComponent* ALMADefaultCharacter::GetHealthComponent() const
{
	
	return HealthComponent;
	
}

// Called when the game starts or when spawned
void ALMADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(CursorMaterial))
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}
	if (ArmLength < MinArmLength)
	{
		ArmLength = MinArmLength;
	}
	else if (ArmLength > MaxArmLength)
	{
		ArmLength = MaxArmLength;
	}
	SpringArmComponent->TargetArmLength = ArmLength;

	OnHealthChanged(HealthComponent->GetHealth());

	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);
	HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);

	Stamina = MaxStamina;
}

// Called every frame
void ALMADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!(HealthComponent->IsDead()))
	{
		RotationPlayerOnCursor();
	}	
}

// Called to bind functionality to input
void ALMADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ALMADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALMADefaultCharacter::MoveRight);

	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &ALMADefaultCharacter::ZoomIn);
	PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, this, &ALMADefaultCharacter::ZoomOut);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ALMADefaultCharacter::SprintActivate);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ALMADefaultCharacter::SprintDeActivate);
}

void ALMADefaultCharacter::MoveForward(float Value) 
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void ALMADefaultCharacter::MoveRight(float Value) 
{
	AddMovementInput(GetActorRightVector(), Value);
}

void ALMADefaultCharacter::ZoomIn() 
{
	ArmLength -= StepArmLength;
	if (ArmLength < MinArmLength)
	{
		ArmLength = MinArmLength;
	}	
	SpringArmComponent->TargetArmLength = ArmLength;
}

void ALMADefaultCharacter::ZoomOut() 
{	
	ArmLength += StepArmLength;
	if (ArmLength > MaxArmLength)
	{
		ArmLength = MaxArmLength;
	}	
	SpringArmComponent->TargetArmLength = ArmLength;
}

void ALMADefaultCharacter::OnDeath() 
{
	CurrentCursor->DestroyRenderState_Concurrent();

	PlayAnimMontage(DeathMontage);
	GetCharacterMovement()->DisableMovement();
	SetLifeSpan(5.0f);

	if (IsValid(Controller))
	{
		Controller->ChangeState(NAME_Spectating);
	}	
}

void ALMADefaultCharacter::OnHealthChanged(float NewHealth) 
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Health = %f"), NewHealth));
}

void ALMADefaultCharacter::RotationPlayerOnCursor() 
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PC))
	{
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (IsValid(CurrentCursor))
		{
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		}
	}
}

void ALMADefaultCharacter::SprintActivate() 
{
	SprintState = true;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	
	GetWorldTimerManager().SetTimer(StaminaTimerHandle, this, &ALMADefaultCharacter::CalculateStamina, StaminaTimerRate, true);	
}

void ALMADefaultCharacter::SprintDeActivate() 
{
	SprintState = false;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;	
}

void ALMADefaultCharacter::CalculateStamina()
{
	if (SprintState && !FMath::IsNearlyEqual((GetCharacterMovement()->Velocity).Length(), 0.0f))
	{
		Stamina = FMath::Clamp(Stamina - DecreaseStaminaSpeed, 0.0f, MaxStamina);
		if (StaminaIsEmpty())
		{
			SprintDeActivate();
		}		
	}
	else
	{
		Stamina = FMath::Clamp(Stamina + IncreaseStaminaSpeed, 0.0f, MaxStamina);
		if (StaminaIsFull() && !SprintState)
		{
			GetWorldTimerManager().ClearTimer(StaminaTimerHandle);
		}
	}	
	GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Blue, FString::Printf(TEXT("Stamina = %f"), Stamina));
}

bool ALMADefaultCharacter::StaminaIsFull()
{
	return FMath::IsNearlyEqual(Stamina, MaxStamina);
}

bool ALMADefaultCharacter::StaminaIsEmpty()
{
	return FMath::IsNearlyEqual(Stamina, 0.0f);
}

