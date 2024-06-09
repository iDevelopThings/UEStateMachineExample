#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyState.h"
#include "UObject/Object.h"
#include "EnemyAttackState.generated.h"

UCLASS()
class ROGUE_API UEnemyAttackState : public UBaseEnemyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Debug))
	double TickRandomRate = 0.5f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta=(Debug))
	double LastRandomTickAt = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta=(Debug))
	int LastRandomChance = 100;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta=(Debug))
	double LastAttackedAt = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta=(Debug))
	bool bHasLineOfSight = false;

	virtual void OnEnter() override;
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnExit() override;
	virtual void OnPartialExit() override;
	virtual bool CanAutoExit() override;
	virtual bool CanFullExit() override;
	virtual void ModifyNextState(UBaseState*& NextState) override;
	virtual EStateInterruptPriority GetMinimumInterruptPriority() const override;
	virtual bool OnMoveCompleted(FPathFollowingResult Result) override;
};