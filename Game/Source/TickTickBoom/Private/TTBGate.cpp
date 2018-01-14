

#include "TTBGate.h"
#include "Components/TimelineComponent.h"
#include "TTBGameBoard.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Components/PointLightComponent.h"

ATTBGate::ATTBGate()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	GateMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	InteriorLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("InteriorLight"));
	InteriorLight->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	InteriorLight->SetRelativeLocation(FVector(9.f, 0.f, -4.f));
	InteriorLight->SetIntensity(3.f);
	InteriorLight->SetLightColor(FLinearColor::Red, true);
	InteriorLight->SetAttenuationRadius(15.f);
	InteriorLight->bAffectDynamicIndirectLighting = true;
	//InteriorLight->bUseRayTracedDistanceFieldShadows = true;
}

void ATTBGate::FlipGate()
{
	DoFlipGate(Gameboard->GetGameboardTimeScale());
}

void ATTBGate::FlipGateTLCallback(float Val)
{
	GateMesh->SetRelativeRotation(FRotator(Val, 0.f, 0.f));
}

