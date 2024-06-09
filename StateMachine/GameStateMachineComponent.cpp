#include "Rogue/Framework/StateMachine/GameStateMachineComponent.h"
#include "Rogue/Framework/StateMachine/BaseState.h"
#include "Rogue/GameFramework/RogueCharacter.h"
#include "Rogue/GameFramework/RoguePlayerController.h"
#include "Rogue/RogueLogs.h"
#include "Net/UnrealNetwork.h"
#include "Reflection/UClassUtils.h"
#include "Traits/Stats.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(GameStateMachineComponent)

UGameStateMachineComponent::UGameStateMachineComponent() {
	PrimaryComponentTick.bCanEverTick      = true;
	bReplicateUsingRegisteredSubObjectList = true;
	SetIsReplicatedByDefault(true);
}

void UGameStateMachineComponent::BeginPlay() {
	Super::BeginPlay();
	// if (!HasAuthority())
	// return;

	Initialize();
}

void UGameStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	DECLARE_SCOPE_CYCLE_COUNTER_FN(TickComponent, StateMachine);

	if (bDisable)
		return;

	if (!HasAuthority()) {
		return;
	}

	if (NextState != nullptr) {
		ChangeState(NextState);
	}
	if (CurrentState) {
		CurrentState->OnUpdate(DeltaTime);

		if (CurrentState->CanAutoExit()) {
			CurrentState->ModifyContext = FModifyStateContext(true, false);
			UBaseState* Next            = NextState.Get();
			CurrentState->ModifyNextState(Next);
			if (Next != NextState) {
				SetNextState(Next);
			}
		}
	}

	// Process ActiveBackgroundStates in reverse order to allow for removal
	for (int32 i = ActiveBackgroundStates.Num() - 1; i >= 0; i--) {
		UBaseState* State = ActiveBackgroundStates[i];
		if (!State) {
			ActiveBackgroundStates.RemoveAt(i);
			continue;
		}
		State->OnUpdate(DeltaTime);
		if (State->CanAutoExit()) {
			State->OnExit();
			ActiveBackgroundStates.RemoveAt(i);
		}
	}

}

void UGameStateMachineComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGameStateMachineComponent, AllStates);
	DOREPLIFETIME(UGameStateMachineComponent, PreviousState);
	DOREPLIFETIME(UGameStateMachineComponent, CurrentState);
	DOREPLIFETIME(UGameStateMachineComponent, NextState);
	DOREPLIFETIME(UGameStateMachineComponent, ActiveBackgroundStates);
}

void UGameStateMachineComponent::PostInitProperties() {
	Super::PostInitProperties();
	CustomName = CustomName.IsNone() ? GetClass()->GetFName() : CustomName;
}

void UGameStateMachineComponent::Initialize() {
	DECLARE_SCOPE_CYCLE_COUNTER_FN(Initialize, StateMachine);

	for (UBaseState* State : AllStates) {
		RegisterState(State);
	}

	if (IsLocallyControlled()) {
		OnRep_AllStates();
	}
}

void UGameStateMachineComponent::RegisterState(UBaseState* State) {
	DECLARE_SCOPE_CYCLE_COUNTER_FN(RegisterState, StateMachine);

	if (!State) return;
	State->Initialize(this);

	AddReplicatedSubObject(State);
	State->SetOwner(GetOwner());

	if (!AllStates.Contains(State)) {
		AllStates.Add(State);
	}
}

bool UGameStateMachineComponent::CanInterruptState(EStateInterruptPriority InterruptPriority) const {
	DECLARE_SCOPE_CYCLE_COUNTER_FN(CanInterruptState, StateMachine);

	if (NextState) {
		return NextState->GetMinimumInterruptPriority() <= InterruptPriority;
	}
	if (CurrentState) {
		return CurrentState->GetMinimumInterruptPriority() <= InterruptPriority;
	}
	return true;
}

bool UGameStateMachineComponent::SetInterruptState(UBaseState* NewState, EStateInterruptPriority InterruptPriority) {
	DECLARE_SCOPE_CYCLE_COUNTER_FN(SetInterruptState, StateMachine);

	if (CanInterruptState(InterruptPriority)) {
		SetNextState(NewState);
		return true;
	}
	return false;
}

void UGameStateMachineComponent::SetNextState(UBaseState* NewState) {
	DECLARE_SCOPE_CYCLE_COUNTER_FN(SetNextState, StateMachine);

	UBaseState* ModifiableState = NewState;
	// Trigger the delegate, allowing bound functions to modify the next state
	OnModifyNextState.Broadcast(ModifiableState);
	// Proceed to change the state with the potentially modified next state
	ChangeState(ModifiableState);
}

void UGameStateMachineComponent::SetNextStateToMain() {
	DECLARE_SCOPE_CYCLE_COUNTER_FN(SetNextStateToMain, StateMachine);

	if (UBaseState* MainState = GetState(InitialStateName)) {
		SetNextState(MainState);
	} else {
		SetNextState(nullptr);
	}
}

void UGameStateMachineComponent::ChangeState(UBaseState* NewState) {
	DECLARE_SCOPE_CYCLE_COUNTER_FN(ChangeState, StateMachine);

	if (!HasAuthority()) {
		GAMELOG(Error, "ChangeState called on client");
		return;
	}
	if (NewState && NewState == NextState) {
		NextState = nullptr;
	}
	if (CurrentState != nullptr) {
		if (!CurrentState->ModifyContext.bIsFromAutoExit) {
			CurrentState->ModifyContext = FModifyStateContext(false, false);
			CurrentState->ModifyNextState(NewState);
		}
		if (CurrentState->CanFullExit()) {
			CurrentState->OnExit();
		} else {
			CurrentState->OnPartialExit();
			ActiveBackgroundStates.Add(CurrentState);
		}
	}
	PreviousState = CurrentState;
	CurrentState  = NewState;
	if (CurrentState != nullptr) {
		CurrentState->OnEnter();
	}
}

UBaseState* UGameStateMachineComponent::GetState(FName StateName) const {
	DECLARE_SCOPE_CYCLE_COUNTER_FN(GetState, StateMachine);

	for (UBaseState* State : AllStates) {
		if (State && State->GetStateName() == StateName) {
			return State;
		}
	}
	return nullptr;
}

UBaseState* UGameStateMachineComponent::GetState(UClass* StateType) const {
	DECLARE_SCOPE_CYCLE_COUNTER_FN(GetState, StateMachine);

	UClass* NativeClass = GetParentNativeClass(StateType);

	for (UBaseState* State : AllStates) {
		if (State && GetParentNativeClass(State->GetClass()) == NativeClass) {
			return State;
		}
	}

	return nullptr;
}

bool UGameStateMachineComponent::IsInState(UBaseState* State) const {
	DECLARE_SCOPE_CYCLE_COUNTER_FN(IsInState, StateMachine);

	return CurrentState == State;
}

bool UGameStateMachineComponent::IsInState(UClass* StateType) const {
	DECLARE_SCOPE_CYCLE_COUNTER_FN(IsInStateClass, StateMachine);
	return CurrentState && CurrentState->GetClass() == StateType;
}

void UGameStateMachineComponent::BeginDestroy() {
	bIsDestroying = true;
	if (CurrentState) {
		// CurrentState->OnExit();
		CurrentState = nullptr;
	}
	Super::BeginDestroy();
}

FName UGameStateMachineComponent::GetMachineName() const { return CustomName; }

UBaseState* UGameStateMachineComponent::RegisterNewState(UClass* StateTypeClass) {
	DECLARE_SCOPE_CYCLE_COUNTER_FN(RegisterNewState, StateMachine);

	UBaseState* State = NewObject<UBaseState>(this, StateTypeClass);
	RegisterState(State);
	return State;
}

TArray<FName> UGameStateMachineComponent::GetInitialStateNameOptions() const {
	TArray<FName> Names;

	for (const UBaseState* State : AllStates) {
		if (!State) continue;
		FName Name = State->GetStateName();
		if (Name == NAME_None || Name == "BaseState") {
			Name = GetParentNativeClass(State->GetClass())->GetFName();
		}

		Names.Add(Name);
	}

	TArray<UClass*> Classes;
	FindAllSubclasses<UBaseState>(Classes);
	for (UClass* Class : Classes) {
		Names.AddUnique(GetParentNativeClass(Class)->GetFName());
	}

	return Names;
}

void UGameStateMachineComponent::OnRep_AllStates() {
	/*
	StateMap.Empty();
	StateTypeMap.Empty();
	for (UBaseState* State : AllStates) {
		if (!State)
			continue;

		State->Initialize(this);

		UClass* StateClass = GetParentNativeClass(State->GetClass());
		FName StateName    = State->GetStateName();

		if (!StateMap.Contains(StateName)) {
			StateMap.Add(StateName, State);
		}

		if (!StateTypeMap.Contains(StateClass)) {
			StateTypeMap.Add(StateClass, State);
		}

	}
	*/

	OnStateMachineStatesReplicated.Broadcast();
}