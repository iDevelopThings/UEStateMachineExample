#pragma once

#include "CoreMinimal.h"
#include "Rogue/Framework/StateMachine/GameStateMachineComponent.h"
#include "EnemyStateMachine.generated.h"


UCLASS(ClassGroup=(Custom), Blueprintable, DefaultToInstanced, EditInlineNew, meta=(BlueprintSpawnableComponent))
class ROGUE_API UEnemyStateMachine : public UGameStateMachineComponent
{
	GENERATED_BODY()

public:
	UEnemyStateMachine();

	virtual void Initialize() override;

};