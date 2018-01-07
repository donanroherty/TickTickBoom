

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTBGameBoard.generated.h"

USTRUCT(BlueprintType)
struct FGameboardData 
{
	GENERATED_BODY()

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

	FGameboardData()
	{
		Cols = 3;
		Rows = 3;
		CycleCount = 10;
		SectionsMovePerCycle = 1;
		TimeScale = 1.0f;
	}
};

UCLASS()
class TICKTICKBOOM_API ATTBGameBoard : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameboardData GameboardData;

public:	
	// Sets default values for this actor's properties
	ATTBGameBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
