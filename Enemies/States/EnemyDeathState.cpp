#include "Rogue/Gameplay/Enemies/States/EnemyDeathState.h"
#include "GameFrameworkExtensions/Actors/ActorPoolSubsystem.h"
#include "Rogue/Gameplay/Enemies/Enemy.h"
#include "Rogue/Gameplay/Enemies/States/EnemyIdleState.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(EnemyDeathState)


void UEnemyDeathState::OnEnter() {
	Super::OnEnter();
	GetActorPool(this)->Return(Enemy);
	
	Machine->SetNextState<UEnemyIdleState>();
}

void UEnemyDeathState::OnUpdate(float DeltaTime) {
	Super::OnUpdate(DeltaTime);
}

void UEnemyDeathState::OnExit() {
	Super::OnExit();
}

void UEnemyDeathState::OnPartialExit() {
	Super::OnPartialExit();
}

bool UEnemyDeathState::CanAutoExit() {
	return Super::CanAutoExit();
}

bool UEnemyDeathState::CanFullExit() { return true; }

void UEnemyDeathState::ModifyNextState(UBaseState*& NextState) {
	Super::ModifyNextState(NextState);
}

EStateInterruptPriority UEnemyDeathState::GetMinimumInterruptPriority() const {
	return EStateInterruptPriority::Death;
}