// Fill out your copyright notice in the Description page of Project Settings.

#include "PlatformPickup.h"
#include "Components/StaticMeshComponent.h"

APlatformPickup::APlatformPickup()
{
	GetMesh()->SetSimulatePhysics(true);
}




