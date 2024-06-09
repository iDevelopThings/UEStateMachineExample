#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyState.h"
#include "UObject/Object.h"
#include "EnemyFindNewTargetState.generated.h"

UCLASS()
class ROGUE_API UEnemyFindNewTargetState : public UBaseEnemyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Debug))
	double LastUpdatedTime = 0.0;

	virtual void OnEnter() override;
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnExit() override;
	virtual void OnPartialExit() override;
	virtual bool CanAutoExit() override;
	virtual bool CanFullExit() override;
	virtual void ModifyNextState(UBaseState*& NextState) override;
	virtual EStateInterruptPriority GetMinimumInterruptPriority() const override;

private:
	void ReEvaluateTarget();
};