#pragma once

#include "CoreMinimal.h"
#include "../BaseState.h"
#include "BaseCharacterState.generated.h"

class ARogueCharacter;
class ARoguePlayerController;

UCLASS(Abstract)
class ROGUE_API UBaseCharacterState : public UBaseState
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleInstanceOnly, Category="StateMachine|CharacterState")
	ARogueCharacter* Character;
	UPROPERTY(VisibleInstanceOnly, Category="StateMachine|CharacterState")
	ARoguePlayerController* Player;
	UPROPERTY(VisibleInstanceOnly, Category="StateMachine|CharacterState")
	TObjectPtr<class URogueCharacterControlComponent> Control;
	
	virtual void Initialize(UGameStateMachineComponent* InMachine) override;

};
