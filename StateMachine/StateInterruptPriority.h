#pragma once

#include "CoreMinimal.h"
#include "StateInterruptPriority.generated.h"

UENUM(BlueprintType)
enum class EStateInterruptPriority : uint8
{
	Any   = 0 UMETA(DisplayName = "Any"),
	Skill = 1 UMETA(DisplayName = "Skill"),
	Pain  = 2 UMETA(DisplayName = "Pain"),
	Death = 3 UMETA(DisplayName = "Death"),
};