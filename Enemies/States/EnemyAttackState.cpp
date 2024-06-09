#include "Rogue/Gameplay/Enemies/States/EnemyAttackState.h"
#include "Rogue/GameFramework/RogueCharacter.h"
#include "Rogue/Gameplay/Enemies/Enemy.h"
#include "Rogue/Gameplay/Enemies/EnemyAiController.h"
#include "Rogue/Gameplay/Enemies/EnemyStatContainer.h"
#include "Rogue/Gameplay/Enemies/States/EnemyChasePlayerState.h"
#include "Rogue/Gameplay/Enemies/States/EnemyFindNewTargetState.h"
#include "Rogue/Gameplay/Enemies/States/EnemyStrafeState.h"
#include "Navigation/PathFollowingComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(EnemyAttackState)


void UEnemyAttackState::OnEnter() {
	Super::OnEnter();
	LastAttackedAt   = GetWorld()->GetTimeSeconds();
	LastRandomTickAt = GetWorld()->GetTimeSeconds();
	LastRandomChance = 100;
	// Controller->SetFocus(Controller->Target, EAIFocusPriority::Gameplay);
}

void UEnemyAttackState::OnUpdate(float DeltaTime) {
	Super::OnUpdate(DeltaTime);

	/*if (World->TimeSince(LastRandomTickAt) >= TickRandomRate) {
		LastRandomTickAt = World->GetTimeSeconds();

		// We have a 20% chance to change into a strafe state
		LastRandomChance = FMath::RandRange(0, 100);
		if (LastRandomChance < 20) {
			SetInterruptState<UEnemyStrafeState>();
			return;
		}
	}*/

	bHasLineOfSight = Controller->LineOfSightTo(Controller->Target);
	// If we no longer have LOS, we should change to chase state
	if (!bHasLineOfSight) {
		SetInterruptState<UEnemyChasePlayerState>();
		return;
	}

	if (World->TimeSince(LastAttackedAt) < Enemy->Stats->AttackSpeed.Get()) {
		return;
	}

	LastAttackedAt = World->GetTimeSeconds();
	DrawDebugLine(World, Enemy->GetActorLocation(), Controller->Target->GetActorLocation(), FColor::Red, false, 1.0f, 0, 4);

	if (FMath::RandBool())
		SetInterruptState<UEnemyStrafeState>();
}

void UEnemyAttackState::OnExit() {
	Super::OnExit();
}

void UEnemyAttackState::OnPartialExit() {
	Super::OnPartialExit();
}

bool UEnemyAttackState::CanAutoExit() {
	if (!Controller->TargetIsInRange()) {
		return true;
	}
	if (!Controller->TargetIsStillValid()) {
		return true;
	}
	return false;
}

bool UEnemyAttackState::CanFullExit() { return true; }

void UEnemyAttackState::ModifyNextState(UBaseState*& NextState) {
	if (ModifyContext.bIsFromAutoExit) {

		if (!Controller->TargetIsStillValid()) {
			NextState = GetState<UEnemyFindNewTargetState>();
			return;
		}

		if (!Controller->TargetIsInRange()) {
			NextState = GetState<UEnemyChasePlayerState>();
			return;
		}

	}
}

EStateInterruptPriority UEnemyAttackState::GetMinimumInterruptPriority() const {
	return EStateInterruptPriority::Any;
}

bool UEnemyAttackState::OnMoveCompleted(FPathFollowingResult Result) {
	return Super::OnMoveCompleted(Result);
}