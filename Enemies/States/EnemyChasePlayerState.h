#pragma once

#include "CoreMinimal.h"
#include "../EnemyTargetData.h"
#include "BaseEnemyState.h"
#include "UObject/Object.h"
#include "EnemyChasePlayerState.generated.h"

class ARogueCharacter;

UCLASS()
class ROGUE_API UEnemyChasePlayerState : public UBaseEnemyState
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta=(Debug))
	double DistanceFromTarget = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta=(Debug))
	bool bIsInRangeOfTarget = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta=(Debug))
	double SelectedRandomRange = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta=(Debug))
	FEnemyTargetData Target;

	virtual void OnEnter() override;
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnExit() override;
	virtual void OnPartialExit() override;
	virtual bool CanAutoExit() override;
	virtual bool CanFullExit() override;
	virtual void ModifyNextState(UBaseState*& NextState) override;
	virtual EStateInterruptPriority GetMinimumInterruptPriority() const override;

	virtual bool OnMoveCompleted(struct FPathFollowingResult Result) override;
};