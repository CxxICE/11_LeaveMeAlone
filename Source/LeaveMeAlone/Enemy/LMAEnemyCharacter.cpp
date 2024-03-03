// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "LMAEnemyCharacter.h"
#include "Components/LMAHealthComponent.h"

// Sets default values
ALMAEnemyCharacter::ALMAEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");
}



