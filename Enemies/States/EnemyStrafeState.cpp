#include "Rogue/Gameplay/Enemies/States/EnemyStrafeState.h"
#include "Rogue/GameFramework/RogueCharacter.h"
#include "Rogue/Gameplay/Enemies/Enemy.h"
#include "Rogue/Gameplay/Enemies/EnemyAiController.h"
#include "Rogue/Gameplay/Enemies/EnemyStatContainer.h"
#include "Rogue/Gameplay/Enemies/States/EnemyAttackState.h"
#include "Rogue/Gameplay/Enemies/States/EnemyFindNewTargetState.h"
#include "Rogue/Gameplay/Enemies/States/EnemyIdleState.h"
#include "Navigation/NavFilter_AIControllerDefault.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(EnemyStrafeState)


void UEnemyStrafeState::OnEnter() {
	Super::OnEnter();
	TryStrafe();
}

void UEnemyStrafeState::OnUpdate(float DeltaTime) {
	Super::OnUpdate(DeltaTime);

	// IF we're already following path, we don't need to do anything
	if (Controller->GetMoveStatus() == EPathFollowingStatus::Moving) {
		// DrawDebugSphere(World, Enemy->GetActorLocation() + FVector(0, 0, 100), 15, 16, FColor::Blue, false, 1.0f, 0, 4);
		return;
	}

	if (Controller->GetMoveStatus() == EPathFollowingStatus::Idle) {
		TryStrafe();
	}


	/*if (World->TimeSince(LastStrafedAt) > StrafeCooldownTime) {
		LastStrafedAt = World->GetTimeSeconds();
		DrawDebugSphere(World, Enemy->GetActorLocation() + FVector(0, 0, 100), 15, 16, FColor::Red, false, 1.0f, 0, 4);
		TryStrafe();
		return;
	}

	DrawDebugSphere(World, Enemy->GetActorLocation() + FVector(0, 0, 100), 15, 16, FColor::Cyan, false, 1.0f, 0, 4);*/
}

void UEnemyStrafeState::OnExit() {
	Super::OnExit();
}

void UEnemyStrafeState::OnPartialExit() {
	Super::OnPartialExit();
}

bool UEnemyStrafeState::CanAutoExit() {
	// If we have no targets, then we have nothing to strafe anymore...
	if (Controller->OverlappedTargets.IsEmpty()) {
		return true;
	}

	if (!Controller->TargetIsStillValid()) {
		return true;
	}

	return false;
}

bool UEnemyStrafeState::CanFullExit() { return true; }

void UEnemyStrafeState::ModifyNextState(UBaseState*& NextState) {
	if (ModifyContext.bIsFromAutoExit) {
		if (Controller->OverlappedTargets.IsEmpty()) {
			NextState = GetState<UEnemyIdleState>();
			return;
		}
		if (!Controller->TargetIsStillValid()) {
			NextState = GetState<UEnemyFindNewTargetState>();
			return;
		}
	}
}

EStateInterruptPriority UEnemyStrafeState::GetMinimumInterruptPriority() const {
	return EStateInterruptPriority::Any;
}

bool UEnemyStrafeState::TryFindNewStrafeLocation(int32 CurrentRetryAttempt) {
	if (!NavSys)
		return false;
	if (!Controller->TargetIsStillValid())
		return false;
	if (CurrentRetryAttempt > MaxRetryAttempts)
		return false;

	const FVector TargetLocation = Enemy->GetActorLocation();
	// const FVector TargetLocation = Controller->Target->GetActorLocation();

	while (CurrentRetryAttempt++ < MaxRetryAttempts) {
		if (!NavSys->GetRandomReachablePointInRadius(TargetLocation, Enemy->Stats->StrafingRadius.Get(), StrafeLocation, nullptr, nullptr)) {
			continue;
		}

		const EPathFollowingRequestResult::Type Result = Controller->MoveToLocation(
			StrafeLocation.Location,
			-1, true, true,
			false, true, UNavFilter_AIControllerDefault::StaticClass()
		);

		if (Result != EPathFollowingRequestResult::Type::RequestSuccessful) {
			continue;
		}

		// Controller->SetFocalPoint(StrafeLocation.Location);

		break;
	}


	return StrafeLocation.HasNodeRef();
}

/*bool UEnemyStrafeState::TryStrafeLeftRight(int32 CurrentRetryAttempt) {
	if (!NavSys || !Controller->TargetIsStillValid() || CurrentRetryAttempt > MaxRetryAttempts)
		return false;

	const FVector CurrentLocation       = Enemy->GetActorLocation();
	const FVector TargetLocation        = Controller->Target->GetTargetLocation();
	const float CurrentDistanceToTarget = (TargetLocation - CurrentLocation).Size();
	const FVector DirectionToTarget     = (TargetLocation - CurrentLocation).GetSafeNormal();
	const FVector RightVector           = FVector::CrossProduct(FVector::UpVector, DirectionToTarget).GetSafeNormal();
	const FVector ForwardVector         = DirectionToTarget; // Enemy's forward direction towards the target
	const FVector BackwardVector        = -DirectionToTarget; // Enemy's backward direction from the target

	double StrafeDistanceRadius = FMath::RandRange(Enemy->Stats.StrafingRadius.Get() / 2, Enemy->Stats.StrafingRadius.Get() );
	
	// Define potential strafe directions
	TArray<FVector> StrafeDirections = {
		RightVector,          // Right
		-RightVector,         // Left
		ForwardVector,        // Forward
		BackwardVector        // Backward
	};

	// Randomly select a strafe direction
	int32 DirectionIndex      = FMath::RandRange(0, StrafeDirections.Num() - 1);
	FVector SelectedDirection = StrafeDirections[DirectionIndex];

	// If selected direction is forward and distance is less than half attack range, re-select excluding forward
	if (SelectedDirection == ForwardVector && CurrentDistanceToTarget <= Enemy->Stats.AttackRange.Get()) {
		StrafeDirections.RemoveAt(2); // Remove forward direction
		DirectionIndex    = FMath::RandRange(0, StrafeDirections.Num() - 1);
		SelectedDirection = StrafeDirections[DirectionIndex];
	}

	FVector StrafeLocationPos = CurrentLocation + SelectedDirection * StrafeDistanceRadius;

	if (!NavSys->ProjectPointToNavigation(StrafeLocationPos, StrafeLocation, FVector(StrafeDistanceRadius))) {
		return TryStrafeLeftRight(CurrentRetryAttempt + 1);
	}

	const EPathFollowingRequestResult::Type Result = Controller->MoveToLocation(
		StrafeLocation.Location,
		-1, true, true, false, true,
		UNavFilter_AIControllerDefault::StaticClass()
	);

	if (Result != EPathFollowingRequestResult::Type::RequestSuccessful) {
		return TryStrafeLeftRight(CurrentRetryAttempt + 1);
	}

	// Controller->SetFocalPoint(StrafeLocation.Location);

	return true;
}*/


bool UEnemyStrafeState::TryStrafeLeftRight(int32 CurrentRetryAttempt) {
	if (!NavSys || !Controller->TargetIsStillValid() || CurrentRetryAttempt > MaxRetryAttempts)
		return false;

	const FVector CurrentLocation   = Enemy->GetActorLocation();
	const FVector TargetLocation    = Controller->Target->GetTargetLocation();
	const FVector DirectionToTarget = (TargetLocation - CurrentLocation).GetSafeNormal();
	const FVector RightVector       = FVector::CrossProduct(FVector::UpVector, DirectionToTarget).GetSafeNormal();

	const double StrafeRadius = Enemy->Stats->StrafingRadius.Get();
	const double AttackRange  = Enemy->Stats->AttackRange.Get();

	const double StrafeDistanceRadius = FMath::RandRange(StrafeRadius / 2, StrafeRadius);


	// Calculate potential strafe positions from the enemy's current location, not the target's
	const FVector LeftStrafeLocation  = CurrentLocation - RightVector * StrafeDistanceRadius;
	const FVector RightStrafeLocation = CurrentLocation + RightVector * StrafeDistanceRadius;

	// Ensure the strafe locations do not exceed the maximum radius from the target
	const FVector CorrectedLeftStrafeLocation = (LeftStrafeLocation - TargetLocation).Size() > AttackRange ?
	                                            TargetLocation - RightVector * StrafeDistanceRadius :
	                                            LeftStrafeLocation;

	const FVector CorrectedRightStrafeLocation = (RightStrafeLocation - TargetLocation).Size() > AttackRange ?
	                                             TargetLocation + RightVector * StrafeDistanceRadius :
	                                             RightStrafeLocation;

	// Randomly choose between corrected left and right strafe locations
	const FVector ChosenStrafeLocation = (FMath::RandBool() ? CorrectedLeftStrafeLocation : CorrectedRightStrafeLocation);

	if (!NavSys->ProjectPointToNavigation(ChosenStrafeLocation, StrafeLocation, FVector(StrafeDistanceRadius))) {
		// Retry with increased attempt count if projection fails
		return TryStrafeLeftRight(CurrentRetryAttempt + 1);
	}

	const EPathFollowingRequestResult::Type Result = Controller->MoveToLocation(
		StrafeLocation.Location,
		-1, true, true, false, true,
		UNavFilter_AIControllerDefault::StaticClass()
	);

	if (Result != EPathFollowingRequestResult::Type::RequestSuccessful) {
		// Retry with increased attempt count if move request fails
		return TryStrafeLeftRight(CurrentRetryAttempt + 1);
	}

	// Controller->SetFocalPoint(StrafeLocation.Location);

	DrawDebugSphere(World, StrafeLocation.Location, 50, 16, FColor::Green, false, 1.0f, 0, 4);

	// Controller->SetFocalPoint(StrafeLocation.Location);

	return true;
}


bool UEnemyStrafeState::TryStrafe() {
	if (!TryStrafeLeftRight()) {
		SetInterruptState<UEnemyIdleState>();
		return false;
	}

	// If we fail, we'll go into idle state which should force reevaluation of the state machine 
	/*if (!TryFindNewStrafeLocation()) {
		SetInterruptState<UEnemyIdleState>();
		return false;
	}*/

	return true;
}

bool UEnemyStrafeState::OnMoveCompleted(FPathFollowingResult Result) {
	if (!Super::OnMoveCompleted(Result)) {
		return false;
	}

	return SetInterruptState<UEnemyAttackState>();
}