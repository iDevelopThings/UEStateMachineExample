#include "Rogue/Framework/StateMachine/States/BaseCharacterState.h"
#include "Rogue/Framework/StateMachine/GameStateMachineComponent.h"
#include "Rogue/GameFramework/RogueCharacter.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseCharacterState)

void UBaseCharacterState::Initialize(UGameStateMachineComponent* InMachine) {
	Super::Initialize(InMachine);
	Player    = Machine->Player;
	Character = Machine->Character;
	if (Character)
		Control = Character->Control;
}