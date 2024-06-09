#include "EnemyStateMachine.h"
#include "States/EnemyAttackState.h"
#include "States/EnemyChasePlayerState.h"
#include "States/EnemyDeathState.h"
#include "States/EnemyFindNewTargetState.h"
#include "States/EnemyIdleState.h"
#include "States/EnemyStrafeState.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(EnemyStateMachine)

UEnemyStateMachine::UEnemyStateMachine() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyStateMachine::Initialize() {
	Super::Initialize();

	// RegisterNewState<UEnemyIdleState>();
	// RegisterNewState<UEnemyFindNewTargetState>();
	// RegisterNewState<UEnemyChasePlayerState>();
	// RegisterNewState<UEnemyAttackState>();
	// RegisterNewState<UEnemyStrafeState>();
	// RegisterNewState<UEnemyDeathState>();
}