

#include "TTB_CameraActor.h"


// Sets default values
ATTB_CameraActor::ATTB_CameraActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATTB_CameraActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATTB_CameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

