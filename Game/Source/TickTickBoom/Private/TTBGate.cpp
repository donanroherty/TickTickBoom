

#include "TTBGate.h"
#include "Components/TimelineComponent.h"
#include "TTBGameBoard.h"
#include "Components/StaticMeshComponent.h"

ATTBGate::ATTBGate()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	GateMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

void ATTBGate::FlipGate()
{
	DoFlipGate(Gameboard->GetGameboardTimeScale());
}

void ATTBGate::FlipGateTLCallback(float Val)
{
	GateMesh->SetRelativeRotation(FRotator(Val, 0.f, 0.f));
}

