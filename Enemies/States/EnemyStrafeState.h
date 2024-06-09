#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyState.h"
#include "UObject/Object.h"
#include "EnemyStrafeState.generated.h"

UCLASS()
class ROGUE_API UEnemyStrafeState : public UBaseEnemyState
{
	GENERATED_BODY()

public:
	/**
	 * The last time we strafed, used with StrafeCooldownTime to determine if we can strafe again.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta=(Debug))
	double LastStrafedAt = 0;

	/**
	 * After we successfully strafe, we'll wait this amount of time before we strafe again.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Debug))
	double StrafeCooldownTime = 5.0;
	/**
	 * If we fail to find a navigatable point in our radius when strafing, we'll retry this many times.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Debug))
	int32 MaxRetryAttempts = 3;

	/**
	 * The current target strafe location
	 */
	FNavLocation StrafeLocation;

	virtual void OnEnter() override;
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnExit() override;
	virtual void OnPartialExit() override;
	virtual bool CanAutoExit() override;
	virtual bool CanFullExit() override;
	virtual void ModifyNextState(UBaseState*& NextState) override;
	virtual EStateInterruptPriority GetMinimumInterruptPriority() const override;

protected:
	bool TryFindNewStrafeLocation(int32 CurrentRetryAttempt = 0);
	bool TryStrafeLeftRight(int32 CurrentRetryAttempt = 0);
	bool TryStrafe();
	virtual bool OnMoveCompleted(FPathFollowingResult Result) override;
};