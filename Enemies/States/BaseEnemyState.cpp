#include "Rogue/Gameplay/Enemies/States/BaseEnemyState.h"
#include "Rogue/Framework/StateMachine/GameStateMachineComponent.h"
#include "Rogue/GameFramework/RogueCharacter.h"
#include "Rogue/Gameplay/Enemies/Enemy.h"
#include "Rogue/Gameplay/Enemies/EnemyAiController.h"
#include "Rogue/RogueLogs.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseEnemyState)

void UBaseEnemyState::Initialize(UGameStateMachineComponent* InMachine) {
	Super::Initialize(InMachine);
}

void UBaseEnemyState::OnEnter() {
	Super::OnEnter();

	NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	if (!NavSys) {
		GAMELOG(Error, "Navigation System is null");
	}

	AEnemy* EnemyActor = Machine->GetOwner<AEnemy>();
	if (EnemyActor) {
		Enemy = EnemyActor;

		Controller = EnemyActor->GetController<AEnemyAiController>();
	}
}

void UBaseEnemyState::OnExit() {
	Super::OnExit();
}

bool UBaseEnemyState::OnMoveCompleted(FPathFollowingResult Result) {
	return Result.Code == EPathFollowingResult::Success;
}
