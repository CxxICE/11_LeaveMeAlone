// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "LMAAIController.h"
#include "LMAEnemyCharacter.h"

void ALMAAIController::OnPossess(APawn* InPawn) 
{
	Super::OnPossess(InPawn);
	const ALMAEnemyCharacter *AICharacter = Cast<ALMAEnemyCharacter>(InPawn);
	if (IsValid(AICharacter))
	{
		RunBehaviorTree(AICharacter->BehaviorTreeAsset);
	}
}
