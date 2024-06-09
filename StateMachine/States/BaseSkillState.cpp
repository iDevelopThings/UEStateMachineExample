#include "Rogue/Framework/StateMachine/States/BaseSkillState.h"
#include "Rogue/Framework/Skills/GenericSkill.h"
#include "Rogue/Framework/Skills/SkillManagerComponent.h"
#include "Rogue/Framework/StateMachine/GameStateMachineComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseSkillState)

void UBaseSkillState::Initialize(UGameStateMachineComponent* InMachine) {
	Super::Initialize(InMachine);
}

void UBaseSkillState::SetSkill(UGenericSkill* GenericSkill) {
	Skill        = GenericSkill;
	SkillManager = Skill->GetManager();
}

void UBaseSkillState::OnEnter() {
	Super::OnEnter();
}

void UBaseSkillState::OnUpdate(float DeltaTime) {
	Super::OnUpdate(DeltaTime);
}

void UBaseSkillState::OnExit() {
	Super::OnExit();
	if (Skill && Skill->AllStates.Last() == this) {
		Skill->OnSkillStateExited();
	}
}

void UBaseSkillState::ModifyNextState(UBaseState*& NextState) {
	Super::ModifyNextState(NextState);
}