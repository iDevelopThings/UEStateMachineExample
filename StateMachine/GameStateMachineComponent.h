#pragma once

#include "CoreMinimal.h"
#include "Rogue/GameFramework/Extensions/BaseActorComponent.h"
#include "StateInterruptPriority.h"
#include "GameStateMachineComponent.generated.h"

class UBaseState;
class ARoguePlayerController;
class ARogueCharacter;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnModifyNextState, UBaseState*&);
DECLARE_MULTICAST_DELEGATE(FOnStateMachineStatesReplicated);

DECLARE_STATS_GROUP_SORTBYNAME(TEXT("StateMachine"), STATGROUP_StateMachine, STATCAT_Advanced);

UCLASS(ClassGroup=(Custom), Blueprintable, DefaultToInstanced, EditInlineNew, meta=(BlueprintSpawnableComponent))
class ROGUE_API UGameStateMachineComponent : public UBaseActorComponent
{
	GENERATED_BODY()

	FOnModifyNextState OnModifyNextState;

public:
	/**
	 * A custom name for the machine, if one isn't provided, the machine will use the class name.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CustomName = NAME_None;
	/**
	 * The initial state of the machine when it's first initialized.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(GetOptions="GetInitialStateNameOptions"))
	FName InitialStateName = NAME_None;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool bIsDestroying = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDisable = false;

	UPROPERTY(ReplicatedUsing=OnRep_AllStates, EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<UBaseState*> AllStates;

	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, Category="PreviousState")
	TObjectPtr<UBaseState> PreviousState;
	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, Category="CurrentState")
	TObjectPtr<UBaseState> CurrentState;
	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, Category="NextState")
	TObjectPtr<UBaseState> NextState;
	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, Category="BackgroundStates")
	TArray<UBaseState*> ActiveBackgroundStates;

	FOnStateMachineStatesReplicated OnStateMachineStatesReplicated;

	UGameStateMachineComponent();

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitProperties() override;

	virtual void Initialize();

	void RegisterState(UBaseState* State);

	// Check if the current state can be interrupted by a given priority
	bool CanInterruptState(EStateInterruptPriority InterruptPriority) const;

	template <typename T>
	bool SetInterruptState(EStateInterruptPriority InterruptPriority = EStateInterruptPriority::Any) {
		return this->SetInterruptState(this->GetState<T>(), InterruptPriority);
	}

	// Attempt to set a new state if it meets interrupt priority criteria
	bool SetInterruptState(UBaseState* NewState, EStateInterruptPriority InterruptPriority);

	bool HasPendingState() const { return NextState != nullptr; }

	void SetNextState(UBaseState* NewState);

	template <typename T>
	void SetNextState(UClass* NewStateType = T::StaticClass()) {
		this->SetNextState(GetState(NewStateType));
	}

	void SetNextStateToMain();

	void ChangeState(UBaseState* NewState);

	UBaseState* GetState(FName StateName) const;
	UBaseState* GetState(UClass* StateType) const;

	template <typename T>
	T* GetState() const {
		return Cast<T>(this->GetState(T::StaticClass()));
	}
	
	template <typename T>
	FORCEINLINE T* GetCurrentState() const { return Cast<T>(CurrentState); }

	bool IsInState(UBaseState* State) const;
	bool IsInState(UClass* StateType) const;

	FName GetMachineName() const;

	template <typename T>
	T* RegisterNewState() {
		return Cast<T>(RegisterNewState(T::StaticClass()));
	}
	UBaseState* RegisterNewState(UClass* StateTypeClass);

private:
	UFUNCTION(CallInEditor)
	TArray<FName> GetInitialStateNameOptions() const;

	UFUNCTION()
	void OnRep_AllStates();


};