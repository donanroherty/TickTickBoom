

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTBBoardFactory.generated.h"

UCLASS()
class TICKTICKBOOM_API ATTBBoardFactory : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	class USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere)
		class UBillboardComponent* Marker;

	class USceneComponent* BoardParent;

	/* The data table that defines each stages board attributes */
	UPROPERTY(EditDefaultsOnly)
	class UDataTable* GameStageData;

	/* The class the gameboards are spawned from */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ATTBGameBoard> GameboardClass;

	UPROPERTY(BlueprintReadWrite)
	TArray<class ATTBGameBoard*> Gameboards;

	TArray<UChildActorComponent*> GameboardComps;

	/* The distance between gameboards */
	UPROPERTY(EditDefaultsOnly)
	float BoardSpacing;

	ATTBBoardFactory();

protected:

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;


	void CreateBoards();

public:
	/* Returns the gameboard for the active level */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameboard")
	ATTBGameBoard * GetGameBoard();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameboard")
	int32 GetMaxStageCount();
};
