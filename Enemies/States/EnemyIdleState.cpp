#include "Rogue/Gameplay/Enemies/States/EnemyIdleState.h"
#include "Rogue/Gameplay/Enemies/States/EnemyChasePlayerState.h"
#include "Rogue/Gameplay/Enemies/States/EnemyFindNewTargetState.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(EnemyIdleState)


void UEnemyIdleState::OnEnter() {
	Super::OnEnter();
}

void UEnemyIdleState::OnUpdate(float DeltaTime) {
	Super::OnUpdate(DeltaTime);
	if (Age <= 0.5f)
		return;

	SetInterruptState<UEnemyFindNewTargetState>();
}

void UEnemyIdleState::OnExit() {
	Super::OnExit();
}

void UEnemyIdleState::OnPartialExit() {
	Super::OnPartialExit();
}

bool UEnemyIdleState::CanAutoExit() {
	return Super::CanAutoExit();
}

bool UEnemyIdleState::CanFullExit() { return true; }

void UEnemyIdleState::ModifyNextState(UBaseState*& NextState) {
	Super::ModifyNextState(NextState);
}

EStateInterruptPriority UEnemyIdleState::GetMinimumInterruptPriority() const {
	return EStateInterruptPriority::Any;
}