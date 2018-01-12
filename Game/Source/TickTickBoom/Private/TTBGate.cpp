

#include "TTBGate.h"
#include "Components/TimelineComponent.h"
#include "TTBGameBoard.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ATTBGate::ATTBGate()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	GateMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	FlipGateTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("FlipGateTimeline"));
}

void ATTBGate::FlipGate()
{
	if (!FlipGateCurve)
		return;

	FOnTimelineFloat tickCallback{};	// Called on each tick of the timeline
	tickCallback.BindUFunction(this, FName{ TEXT("FlipGateTLCallback") });
	FlipGateTimeline->AddInterpFloat(FlipGateCurve, tickCallback, FName{ TEXT("FlipGate") });
	FlipGateTimeline->SetPlayRate(Gameboard->GetGameboardTimeScale());

	FlipGateTimeline->PlayFromStart();
}

void ATTBGate::FlipGateTLCallback(float Val)
{
	GateMesh->SetRelativeRotation(FRotator(Val, 0.f, 0.f));
}

