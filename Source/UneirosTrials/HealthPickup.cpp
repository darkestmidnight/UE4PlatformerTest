// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthPickup.h"
#include "Components/StaticMeshComponent.h"

// set default value
AHealthPickup::AHealthPickup()
{
	GetMesh()->SetSimulatePhysics(true);
}

void AHealthPickup::WasCollected_Implementation()
{
	// use the base pickup behavior
	Super::WasCollected_Implementation();
	// destroy the battery
	Destroy();
}



