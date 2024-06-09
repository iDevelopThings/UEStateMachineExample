#pragma once

#include "CoreMinimal.h"
#include "GameStateMachineComponent.h"
#include "Networking/ReplicatedUObject.h"
#include "StateInterruptPriority.h"
#include "UObject/Object.h"
#include "BaseState.generated.h"

class UGameStateMachineComponent;
class UBaseState;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStateMachineStateEnter, UBaseState*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStateMachineStateExit, UBaseState*);

USTRUCT(BlueprintType)
struct FModifyStateContext
{
	GENERATED_BODY()

	/**
	 * If we're calling a states->ModifyNextState, because we
	 * processed an AutoExit() and it returned true, then this will be true.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	bool bIsFromAutoExit = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	bool bIsFromRegularStateTransition = false;

	FModifyStateContext() = default;
	FModifyStateContext(bool bIsFromAutoExit, bool bIsFromRegularStateTransition)
		: bIsFromAutoExit(bIsFromAutoExit),
		  bIsFromRegularStateTransition(bIsFromRegularStateTransition) {}
};

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class ROGUE_API UBaseState : public UReplicatedUObject
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UGameStateMachineComponent> Machine;

	// Cached world reference to avoid fn call overheads repeatedly
	UPROPERTY()
	TObjectPtr<UWorld> World;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="StateMachine", meta=(AllowPrivateAccess))
	bool bIsInitialized = false;
	/**
	 * The total age of this object, since it was initialized
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="StateMachine", meta=(AllowPrivateAccess, Debug))
	double TotalAge = 0.0;
	/**
	 * The age since we entered this state
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="StateMachine", meta=(AllowPrivateAccess, Debug))
	double Age = 0.0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="StateMachine", meta=(AllowPrivateAccess))
	FName CustomStateName = NAME_None;

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="StateMachine")
	FModifyStateContext ModifyContext;

	FOnStateMachineStateEnter OnEntered;
	FOnStateMachineStateExit OnExited;

	virtual void Initialize(UGameStateMachineComponent* InMachine);

	virtual void PostInitProperties() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnInitialize(UGameStateMachineComponent* InMachine);

	virtual void OnEnter();
	virtual void OnUpdate(float DeltaTime);
	virtual void OnExit();
	virtual void OnPartialExit();

	virtual bool CanAutoExit();
	/**
	 * If we return false, then this state will go into "background" mode, `OnExit` will then be controlled by some external logic
	 * Meaning, if we change the state, or activate a new skill for example, it won't call `OnExit` on this state
	 */
	virtual bool CanFullExit() { return true; }

	virtual void ModifyNextState(UBaseState*& NextState);

	virtual EStateInterruptPriority GetMinimumInterruptPriority() const { return EStateInterruptPriority::Any; }

	FORCEINLINE virtual FName GetStateName() const {
		return CustomStateName;
	}

	template <typename T>
	T* GetState() {
		return Machine ? Machine->GetState<T>() : nullptr;
	}

	template <typename T>
	bool SetInterruptState(EStateInterruptPriority StateInterruptPriority = EStateInterruptPriority::Any) {
		if (!Machine)
			return false;

		return Machine->SetInterruptState<T>(StateInterruptPriority);
	}

	template <typename T>
	bool SetInterruptState(T*& OutState, EStateInterruptPriority StateInterruptPriority = EStateInterruptPriority::Any) {
		if (!Machine)
			return false;

		OutState = GetState<T>();

		return Machine->SetInterruptState(OutState, StateInterruptPriority);
	}

};