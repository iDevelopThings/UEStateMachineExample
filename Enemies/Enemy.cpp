#include "Rogue/Gameplay/Enemies/Enemy.h"
#include "EnemyStateMachine.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Rogue/GameDamageEvent.h"
#include "Rogue/Rogue.h"
#include "Rogue/Framework/StateMachine/GameStateMachineComponent.h"
#include "Rogue/GameFramework/RogueCharacter.h"
#include "Rogue/Gameplay/Enemies/EnemyAiController.h"
#include "Rogue/Gameplay/Enemies/EnemyStatContainer.h"
#include "Rogue/Gameplay/Enemies/EnemyType.h"
#include "Rogue/Gameplay/Enemies/States/EnemyDeathState.h"
#include "Rogue/Gameplay/Player/PlayerCreditValue.h"
#include "Rogue/RogueLogs.h"
#include "Timed/Debouncer.h"
#include "Rogue/UI/Overlays/EnemyWorldHealthBar.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(Enemy)

AEnemy::AEnemy(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer) {
	PrimaryActorTick.bCanEverTick          = true;
	bReplicateUsingRegisteredSubObjectList = true;

	AIControllerClass = AEnemyAiController::StaticClass();
	AutoPossessAI     = EAutoPossessAI::PlacedInWorldOrSpawned;

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetSphereRadius(500.0f);
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionSphere->SetCollisionObjectType(ECC_Enemy);
	DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECC_Player, ECR_Overlap);
	DetectionSphere->SetCollisionResponseToChannel(TC_Player, ECR_Overlap);
	DetectionSphere->CanCharacterStepUpOn = ECB_No;

	GetCapsuleComponent()->CanCharacterStepUpOn = ECB_No;
	GetCapsuleComponent()->SetCollisionObjectType(ECC_WorldDynamic);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(OC_Enemy, ECR_Overlap);


	GetMesh()->CanCharacterStepUpOn = ECB_No;
	GetMesh()->SetCollisionObjectType(ECC_Enemy);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);

	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(RootComponent);

	StateMachine = CreateDefaultSubobject<UEnemyStateMachine>(TEXT("StateMachine"));
	AddOwnedComponent(StateMachine);

	Stats = CreateDefaultSubobject<UEnemyStatContainer>(TEXT("Stats"));
	Stats->SetIsReplicated(true);
}

void AEnemy::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);
	if (!HasAuthority())
		return;
	StateMachine->Initialize();
	StateMachine->SetNextStateToMain();


}

void AEnemy::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		Stats->Health.Subscribe([this](const double& NewHealth) {
			if (NewHealth <= 0) {
				StateMachine->SetInterruptState<UEnemyDeathState>();
			}
		});

	}

	Stats->Initialize();

	if (auto Movement = Cast<UCharacterMovementComponent>(GetMovementComponent())) {
		Movement->MaxWalkSpeed = Stats->MovementSpeed.Get();
	}

	SetupHealthBar();

}

void AEnemy::PreActivateActor(const FTransform& SpawnTransform) {
	if (HasAuthority()) {
		Stats->Initialize();
	}

	SetupHealthBar();
}

void AEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// DOREPLIFETIME_CONDITION(AEnemy, Stats, COND_SkipOwner);
}

void AEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (!HasAuthority())
		return;
}

void AEnemy::SetupHealthBar() {
	if (!HealthBar) {
		return;
	}
	UEnemyWorldHealthBar* HealthBarWidget = Cast<UEnemyWorldHealthBar>(HealthBar->GetWidget());
	if (!HealthBarWidget) {
		GAMELOG(Error, "HealthBar widget not found for enemy %s", *GetName());
		return;
	}
	HealthBarWidget->Setup(this);
}


void AEnemy::Multicast_OnHealthChanged_Implementation(double NewHealth) {}

void AEnemy::DeductHealth(double Amount) {
	if (!HasAuthority()) {
		GAMELOG(Error, "DeductHealth called on client for enemy %s", *GetName());
		return;
	}

	Stats->Health -= Amount;

	// Multicast_OnHealthChanged(Health);
	// if (Health > 0) {
	// return;
	// }
	// SetState(EEnemyState::Dead);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	if (!HasAuthority()) {
		GAMELOG(Error, "TakeDamage called on client for enemy %s", *GetName());
		return 0;
	}

	if (!DamageEvent.IsOfType(FGameDamageEventImpl::ClassID)) {
		GAMELOG(Error, "DamageEvent is not of type FGameDamageEventImpl");
		return 0;
	}

	UDamageType const* const DamageTypeCDO = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();

	FGameDamageEventImpl* const PointDamageEvent = (FGameDamageEventImpl*)&DamageEvent;
	if (DamageAmount <= 0) {
		return 0;
	}

	ReceivePointDamage(DamageAmount, DamageTypeCDO, PointDamageEvent->HitInfo.ImpactPoint, PointDamageEvent->HitInfo.ImpactNormal, PointDamageEvent->HitInfo.Component.Get(), PointDamageEvent->HitInfo.BoneName, PointDamageEvent->ShotDirection, EventInstigator, DamageCauser, PointDamageEvent->HitInfo);
	OnTakePointDamage.Broadcast(this, DamageAmount, EventInstigator, PointDamageEvent->HitInfo.ImpactPoint, PointDamageEvent->HitInfo.Component.Get(), PointDamageEvent->HitInfo.BoneName, PointDamageEvent->ShotDirection, DamageTypeCDO, DamageCauser);

	// Notify the component
	UPrimitiveComponent* const PrimComp = PointDamageEvent->HitInfo.Component.Get();
	if (PrimComp) {
		PrimComp->ReceiveComponentDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}

	DeductHealth(DamageAmount);

	if (EventInstigator && Stats->Health <= 0) {
		// TODO: Scaling plz sir
		GetCredits(EventInstigator)->Add(Definition->BaseCreditReward);
	}

	FHitResult Hit;
	FVector ImpulseDir;
	DamageEvent.GetBestHitInfo(this, (EventInstigator ? EventInstigator->GetPawn() : nullptr), Hit, ImpulseDir);

	FVector DrawLoc = FVector(0, 0, 20 * HitCount);
	DrawDebugString(
		GetWorld(), DrawLoc,
		FString::Printf(TEXT("%s%.0f"), PointDamageEvent->Params.bIsCritical ? TEXT("CRIT ") : TEXT(""), DamageAmount),
		this,
		PointDamageEvent->Params.bIsCritical ? FColor::Red : FColor::White,
		0.2f, true
	);
	HitCount++;

	DEBOUNCE(0.2f) [this] {
		HitCount = 0;
	};

	return DamageAmount;
}