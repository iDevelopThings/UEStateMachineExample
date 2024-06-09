#include "Rogue/Framework/StateMachine/BaseState.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseState)

void UBaseState::Initialize(UGameStateMachineComponent* InMachine) {
	Machine        = InMachine;
	bIsInitialized = true;
	TotalAge       = 0;
	Age            = 0;

	if (CustomStateName == NAME_None && CustomStateName != "BaseState") {
		CustomStateName = GetParentNativeClass(GetClass())->GetFName();
	}

	BP_OnInitialize(InMachine);
}

void UBaseState::PostInitProperties() {
	Super::PostInitProperties();

	if (CustomStateName == NAME_None || CustomStateName == "BaseState") {
		FName NewName = GetParentNativeClass(GetClass())->GetFName();
		if (CustomStateName != NewName && NewName != "BaseState") {
			CustomStateName = NewName;
		}
	}
}

void UBaseState::OnEnter() {
	World = GetWorld();
	OnEntered.Broadcast(this);
	Age = 0;
}

void UBaseState::OnUpdate(float DeltaTime) {
	TotalAge += DeltaTime;
	Age += DeltaTime;
}

void UBaseState::OnExit() {
	OnExited.Broadcast(this);
}

void UBaseState::OnPartialExit() {}

bool UBaseState::CanAutoExit() { return false; }

void UBaseState::ModifyNextState(UBaseState*& NextState) {}