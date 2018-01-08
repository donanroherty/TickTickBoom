#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTBGate.generated.h"

UCLASS()
class TICKTICKBOOM_API ATTBGate : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	class ATTBGameBoard* Gameboard;

	// Sets default values for this actor's properties
	ATTBGate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
