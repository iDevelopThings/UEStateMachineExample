#include "Rogue/Gameplay/Enemies/EnemyAiController.h"

#include "Components/SphereComponent.h"
#include "Rogue/Framework/StateMachine/GameStateMachineComponent.h"
#include "Rogue/GameFramework/RogueCharacter.h"
#include "Rogue/Gameplay/Enemies/Enemy.h"
#include "Rogue/Gameplay/Enemies/EnemyStatContainer.h"
#include "Rogue/Gameplay/Enemies/States/EnemyChasePlayerState.h"
#include "Rogue/Gameplay/Enemies/States/EnemyIdleState.h"
#include "Navigation/NavFilter_AIControllerDefault.h"
#include "Navigation/PathFollowingComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(EnemyAiController)


AEnemyAiController::AEnemyAiController() {
	// bSetControlRotationFromPawnOrientation = true;
	PrimaryActorTick.bCanEverTick = true;
	bAllowStrafe                  = true;
	DefaultNavigationFilterClass  = UNavFilter_AIControllerDefault::StaticClass();
}

void AEnemyAiController::UpdateReferences() {
	Pathing      = GetPathFollowingComponent();
	StateMachine = GetPawn()->GetComponentByClass<UGameStateMachineComponent>();
	Enemy        = GetPawn<AEnemy>();
}

void AEnemyAiController::BeginPlay() {
	Super::BeginPlay();
}

void AEnemyAiController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);
	UpdateReferences();
	
	Enemy->DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyAiController::OnDetectionSphereOverlapBegin);
	Enemy->DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyAiController::OnDetectionSphereOverlapEnd);

}

void AEnemyAiController::OnUnPossess() {
	Super::OnUnPossess();
	// UpdateReferences();
}

void AEnemyAiController::SortTargets() {
	FVector Loc = GetPawn()->GetActorLocation();

	// Sort targets by distance
	OverlappedTargets.Sort([Loc](ARogueCharacter& A, ARogueCharacter& B) {
		return FVector::DistSquared(A.GetActorLocation(), Loc) < FVector::DistSquared(B.GetActorLocation(), Loc);
	});
}

void AEnemyAiController::OnDetectionSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	ARogueCharacter* RogueCharacter = Cast<ARogueCharacter>(OtherActor);
	if (!RogueCharacter)
		return;

	OverlappedTargets.Add(RogueCharacter);

	SortTargets();

	OnTargetsUpdated.Broadcast(OverlappedTargets);


	// Only update target if we don't have one or this one is closer than the other
	/*
	if (Target != RogueCharacter)
		if (!Target || FVector::Dist(GetActorLocation(), RogueCharacter->GetActorLocation()) < FVector::Dist(GetActorLocation(), Target->GetActorLocation())) {
			SetTarget(RogueCharacter);
		}

	if (SetState(EEnemyState::AttackStrafing)) {
		// GAMELOG(Warning, "Attack strafe pls");
		return;
	}

	if (SetState(EEnemyState::Attacking)) {
		// GAMELOG(Warning, "Attack pls");
		return;
	}

	if (SetState(EEnemyState::Chasing)) {
		// GAMELOG(Warning, "chase pls");
		return;
	}*/

}

void AEnemyAiController::OnDetectionSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	const ARogueCharacter* RogueCharacter = Cast<ARogueCharacter>(OtherActor);
	if (!RogueCharacter)
		return;

	OverlappedTargets.Remove(RogueCharacter);

	SortTargets();

	// If we have another overlapped target, set it as the new target
	if (OverlappedTargets.Num() > 0) {
		SetTarget(*OverlappedTargets.CreateConstIterator());
		return;
	}

	// We have no one else to chase, so wander/idle
	StateMachine->SetInterruptState<UEnemyIdleState>(EStateInterruptPriority::Any);
}

void AEnemyAiController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) {
	Super::OnMoveCompleted(RequestID, Result);
	if (UBaseEnemyState* ChaseState = StateMachine->GetCurrentState<UBaseEnemyState>()) {
		ChaseState->OnMoveCompleted(Result);
	}
}

bool AEnemyAiController::TargetIsInRange() {
	if (!TargetIsStillValid())
		return false;

	return Enemy->GetDistanceTo(Target) < Enemy->Stats->AttackRange.Get();
}

bool AEnemyAiController::TargetIsStillValid() {
	if (!Target) return false;
	if (!Target->IsAlive()) return false;
	return true;
}

void AEnemyAiController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AEnemyAiController::SetTarget(ARogueCharacter* NewTarget, bool bSetChaseState) {
	Target = NewTarget;
	OnTargetUpdated.Broadcast(Target);
	if (Target && bSetChaseState) {
		if (UEnemyChasePlayerState* ChaseState = StateMachine->GetState<UEnemyChasePlayerState>()) {
			StateMachine->SetInterruptState(ChaseState, EStateInterruptPriority::Any);
		}
	}
}