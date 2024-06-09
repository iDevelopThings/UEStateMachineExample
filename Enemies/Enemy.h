#pragma once

#include "CoreMinimal.h"
#include "Events/IHasEventListeners.h"
#include "GameFramework/Character.h"
#include "GameFrameworkExtensions/Actors/IPoolable.h"
#include "Enemy.generated.h"

class UEnemyStateMachine;
class UEnemyType;
class UWidgetComponent;
class AEnemyAiController;
class ARogueCharacter;
class UEnemyStatContainer;

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle           = 0 UMETA(DisplayName = "Idle"),
	Chasing        = 1 UMETA(DisplayName = "Chasing"),
	AttackStrafing = 2 UMETA(DisplayName = "AttackStrafing"),
	Attacking      = 3 UMETA(DisplayName = "Attacking"),
	Dead           = 4 UMETA(DisplayName = "Dead"),
};

UCLASS(Abstract)
class ROGUE_API AEnemy : public ACharacter, public IPoolableActor, public IHasEventListeners
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	TObjectPtr<UEnemyType> Definition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	TObjectPtr<UEnemyStateMachine> StateMachine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* DetectionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UEnemyStatContainer> Stats;

	AEnemy(FObjectInitializer const& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void Tick(float DeltaTime) override;

	void SetupHealthBar();

	virtual void PreActivateActor(const FTransform& SpawnTransform) override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnHealthChanged(double NewHealth);

	void DeductHealth(double Amount);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


	int HitCount = 0;

};