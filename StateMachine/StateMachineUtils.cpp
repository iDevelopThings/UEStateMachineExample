#include "Rogue/Framework/StateMachine/StateMachineUtils.h"
#include "Rogue/Framework/StateMachine/GameStateMachineComponent.h"

/*UGameStateMachine* FindStateMachineWithName(UObject* Outer, const FName& MachineName) {
	UGameStateMachine* LocatedMachine = nullptr;

	ForEachObjectWithOuterBreakable(Outer, [&](UObject* Object) {
		UGameStateMachine* StateMachine = Cast<UGameStateMachine>(Object);
		if (StateMachine && StateMachine->GetMachineName() == MachineName) {
			LocatedMachine = StateMachine;
			return false;
		}
		return true;
	}, true);

	return LocatedMachine;
}
UGameStateMachine* FindStateMachineWithName(const FName& MachineName) {
	UGameStateMachine* LocatedMachine = nullptr;

	ForEachObjectOfClass(UGameStateMachine::StaticClass(), [&](UObject* Object) {
		if (LocatedMachine != nullptr) {
			return;
		}

		UGameStateMachine* StateMachine = Cast<UGameStateMachine>(Object);
		if (StateMachine && StateMachine->GetMachineName() == MachineName) {
			LocatedMachine = StateMachine;
		}
	});

	return LocatedMachine;
}*/

UGameStateMachineComponent* FindStateMachineComponentWithName(const FName& MachineName) {
	UGameStateMachineComponent* LocatedMachine = nullptr;

	ForEachObjectOfClass(UGameStateMachineComponent::StaticClass(), [&](UObject* Object) {
		if (LocatedMachine != nullptr) {
			return;
		}

		UGameStateMachineComponent* StateMachine = Cast<UGameStateMachineComponent>(Object);
		if (StateMachine && StateMachine->GetMachineName() == MachineName) {
			LocatedMachine = StateMachine;
		}
	});

	return LocatedMachine;
}

UGameStateMachineComponent* FindStateMachineWithName(AActor* Actor, const FName& MachineName) {
	if (!Actor) return nullptr;

	TInlineComponentArray<UGameStateMachineComponent*> StateMachineComponents(Actor);
	for (UGameStateMachineComponent* Component : StateMachineComponents) {
		if (Component->GetMachineName() == MachineName) {
			return Component;
		}
	}

	return nullptr;
}
