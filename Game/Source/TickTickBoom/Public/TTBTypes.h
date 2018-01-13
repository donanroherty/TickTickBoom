#pragma once

#include "CoreMinimal.h"
#include "TickTickBoom.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "TTBTypes.generated.h"


// Gameboard types
USTRUCT(BlueprintType)
struct FGameboardData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 Cols;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 Rows;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 CycleCount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 SectionsMovePerCycle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float TimeScale;
};

USTRUCT(BlueprintType)
struct FButtonGrid
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<class ATTBButton*> Rows;
};

UENUM(BlueprintType)
enum class EBoardState : uint8
{
	BS_PreCycle		UMETA(DisplayName = "PreCycle"),
	BS_Cycle 		UMETA(DisplayName = "Cycle"),
	BS_PostCycle	UMETA(DisplayName = "PostCycle")
};

UENUM(BlueprintType)
enum class EDirection : uint8
{
	MD_Forward		UMETA(DisplayName = "Forward"),
	MD_Backward		UMETA(DisplayName = "Backward"),
};

UENUM(BlueprintType)
enum class EGridSectionType : uint8
{
	GST_Column		UMETA(DisplayName = "Column"),
	GST_Row			UMETA(DisplayName = "Row"),
};

// Button types
UENUM(BlueprintType)
enum class ETubeAction : uint8
{
	TS_Retract	UMETA(DisplayName = "Retract"),
	TS_Extend	UMETA(DisplayName = "Extend"),
};

UENUM(BlueprintType)
enum class EColorFunction : uint8
{
	CC_Blink		UMETA(DisplayName = "Blink"),
	CC_BlinkAndHold	UMETA(DisplayName = "BlinkAndHold"),
	CC_FadeIn		UMETA(DisplayName = "FadeIn"),
	CC_FadeOut		UMETA(DisplayName = "FadeOut"),
	CC_SlowFadeOut	UMETA(DisplayName = "SlowFadeOut"),
};