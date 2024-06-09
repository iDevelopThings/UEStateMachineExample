#pragma once

#include "CoreMinimal.h"

class UGameStateMachineComponent;

/**
 * Searches for a state machine with the given name in the given outer object.
 */
// UGameStateMachine* FindStateMachineWithName(UObject* Outer, const FName& MachineName);
/**
 * Searches globally over all state machine objects for a state machine with the given name.
 */
// UGameStateMachine* FindStateMachineWithName(const FName& MachineName);
UGameStateMachineComponent* FindStateMachineComponentWithName(const FName& MachineName);
/**
 * Searches for a state machine component with the given name in the given actor.
 */
UGameStateMachineComponent* FindStateMachineWithName(AActor* Actor, const FName& MachineName);
