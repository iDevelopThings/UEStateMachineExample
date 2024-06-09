#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyState.h"
#include "UObject/Object.h"
#include "EnemyDeathState.generated.h"

UCLASS()
class ROGUE_API UEnemyDeathState : public UBaseEnemyState
{
	GENERATED_BODY()


public:
	virtual void OnEnter() override;
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnExit() override;
	virtual void OnPartialExit() override;
	virtual bool CanAutoExit() override;
	virtual bool CanFullExit() override;
	virtual void ModifyNextState(UBaseState*& NextState) override;
	virtual EStateInterruptPriority GetMinimumInterruptPriority() const override;
};