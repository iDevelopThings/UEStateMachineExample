#include "Rogue/Gameplay/Enemies/States/EnemyFindNewTargetState.h"
#include "Rogue/GameFramework/RogueCharacter.h"
#include "Rogue/Gameplay/Enemies/EnemyAiController.h"
#include "Rogue/Gameplay/Enemies/States/EnemyChasePlayerState.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(EnemyFindNewTargetState)


void UEnemyFindNewTargetState::OnEnter() {
	Super::OnEnter();
	ReEvaluateTarget();
}

void UEnemyFindNewTargetState::OnUpdate(float DeltaTime) {
	Super::OnUpdate(DeltaTime);

	if (World->TimeSince(LastUpdatedTime) > 0.5f) {
		LastUpdatedTime = World->GetTimeSeconds();
		ReEvaluateTarget();
	}
}

void UEnemyFindNewTargetState::OnExit() {
	Super::OnExit();
}

void UEnemyFindNewTargetState::OnPartialExit() {
	Super::OnPartialExit();
}

bool UEnemyFindNewTargetState::CanAutoExit() {
	return Controller->Target != nullptr;
}

bool UEnemyFindNewTargetState::CanFullExit() { return true; }

void UEnemyFindNewTargetState::ModifyNextState(UBaseState*& NextState) {
	if (ModifyContext.bIsFromAutoExit && Controller->Target) {
		NextState = GetState<UEnemyChasePlayerState>();
	}
}

EStateInterruptPriority UEnemyFindNewTargetState::GetMinimumInterruptPriority() const {
	return EStateInterruptPriority::Any;
}

void UEnemyFindNewTargetState::ReEvaluateTarget() {
	Controller->SortTargets();

	for (ARogueCharacter* ThisTarget : Controller->OverlappedTargets) {
		if (!Controller->LineOfSightTo(ThisTarget))
			continue;
		Controller->SetTarget(ThisTarget, false);
		break;
	}
}