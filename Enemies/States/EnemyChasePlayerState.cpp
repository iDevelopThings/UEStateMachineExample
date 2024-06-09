#include "Rogue/Gameplay/Enemies/States/EnemyChasePlayerState.h"
#include "Rogue/Framework/StateMachine/GameStateMachineComponent.h"
#include "Rogue/GameFramework/RogueCharacter.h"
#include "Rogue/Gameplay/Enemies/Enemy.h"
#include "Rogue/Gameplay/Enemies/EnemyAiController.h"
#include "Rogue/Gameplay/Enemies/EnemyStatContainer.h"
#include "Rogue/Gameplay/Enemies/States/EnemyAttackState.h"
#include "Rogue/Gameplay/Enemies/States/EnemyFindNewTargetState.h"
#include "Rogue/Gameplay/Enemies/States/EnemyStrafeState.h"
#include "Navigation/PathFollowingComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(EnemyChasePlayerState)

void UEnemyChasePlayerState::OnEnter() {
	Super::OnEnter();
	Target = Controller->Target;

	SelectedRandomRange = FMath::RandRange(Enemy->Stats->AttackRange.Get() / 2, Enemy->Stats->AttackRange.Get());

	// Controller->SetFocus(Target, EAIFocusPriority::Move);
	Controller->MoveToActor(Target, SelectedRandomRange);
}

void UEnemyChasePlayerState::OnUpdate(float DeltaTime) {
	Super::OnUpdate(DeltaTime);

	DistanceFromTarget = Enemy->GetDistanceTo(Target);
	bIsInRangeOfTarget = DistanceFromTarget < SelectedRandomRange;
}

void UEnemyChasePlayerState::OnExit() {
	Super::OnExit();
	if (Machine->bIsDestroying)
		return;

	Controller->StopMovement();
}

void UEnemyChasePlayerState::OnPartialExit() {
	Super::OnPartialExit();
}

bool UEnemyChasePlayerState::CanAutoExit() {
	if (Target == nullptr)
		return true;
	if (!Target->IsAlive())
		return true;

	// Check our distance to the target, and if we are close enough, we should switch to attack state
	if (bIsInRangeOfTarget) {
		return true;
	}

	return false;
}

bool UEnemyChasePlayerState::CanFullExit() { return true; }

void UEnemyChasePlayerState::ModifyNextState(UBaseState*& NextState) {
	if (ModifyContext.bIsFromAutoExit) {
		if (bIsInRangeOfTarget) {
			NextState = GetState<UEnemyAttackState>();
			return;
		}

		NextState = GetState<UEnemyFindNewTargetState>();

	}

}

EStateInterruptPriority UEnemyChasePlayerState::GetMinimumInterruptPriority() const {
	return EStateInterruptPriority::Any;
}

bool UEnemyChasePlayerState::OnMoveCompleted(FPathFollowingResult Result) {
	if (!Super::OnMoveCompleted(Result)) {
		return false;
	}
	return SetInterruptState<UEnemyStrafeState>();
}