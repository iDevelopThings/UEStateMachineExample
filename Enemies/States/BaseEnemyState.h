#pragma once

#include "CoreMinimal.h"
#include "Rogue/Framework/StateMachine/BaseState.h"
#include "UObject/Object.h"
#include "BaseEnemyState.generated.h"

class AEnemy;
class AEnemyAiController;
class ARogueCharacter;
class UNavigationSystemV1;

UCLASS(Abstract)
class ROGUE_API UBaseEnemyState : public UBaseState
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	TObjectPtr<AEnemy> Enemy;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	TObjectPtr<AEnemyAiController> Controller;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	TObjectPtr<UNavigationSystemV1> NavSys;
	
	virtual void Initialize(UGameStateMachineComponent* InMachine) override;
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual bool OnMoveCompleted(struct FPathFollowingResult Result);

};