#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyTargetData.h"
#include "EnemyAiController.generated.h"

class UGameStateMachineComponent;
class ARogueCharacter;
class AEnemy;


DECLARE_MULTICAST_DELEGATE_OneParam(FOnTargetListUpdated, TSet<ARogueCharacter*> OverlappedTargets);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTargetUpdated, FEnemyTargetData Target);

UCLASS(ClassGroup = AI, BlueprintType, Blueprintable)
class ROGUE_API AEnemyAiController : public AAIController
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UPathFollowingComponent> Pathing;
	UPROPERTY()
	TObjectPtr<UGameStateMachineComponent> StateMachine;

	UPROPERTY()
	AEnemy* Enemy;

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	TSet<ARogueCharacter*> OverlappedTargets;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	FEnemyTargetData Target;

	FOnTargetListUpdated OnTargetsUpdated;
	FOnTargetUpdated OnTargetUpdated;

	AEnemyAiController();

private:
	void UpdateReferences();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION()
	void OnDetectionSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnDetectionSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	virtual void Tick(float DeltaTime) override;
	
	void SetTarget(ARogueCharacter* NewTarget, bool bSetChaseState = true);
	void SortTargets();

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	bool TargetIsInRange();
	bool TargetIsStillValid();

};