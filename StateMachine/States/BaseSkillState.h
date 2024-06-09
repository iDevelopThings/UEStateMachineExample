#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterState.h"
#include "BaseSkillState.generated.h"

class USkillManagerComponent;
class UGenericSkill;

UCLASS(Abstract)
class ROGUE_API UBaseSkillState : public UBaseCharacterState
{
	GENERATED_BODY()

	
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "StateMachine|SkillState", meta=(Debug))
	TObjectPtr<UGenericSkill> Skill;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "StateMachine|SkillState", meta=(Debug))
	TObjectPtr<USkillManagerComponent> SkillManager;
	
	virtual void Initialize(UGameStateMachineComponent* InMachine) override;
	virtual void SetSkill(UGenericSkill* GenericSkill);

	virtual void OnEnter() override;
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnExit() override;
	virtual void ModifyNextState(UBaseState*& NextState) override;
};