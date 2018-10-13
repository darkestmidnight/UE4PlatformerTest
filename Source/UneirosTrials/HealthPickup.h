// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "HealthPickup.generated.h"

/**
 * 
 */
UCLASS()
class UNEIROSTRIALS_API AHealthPickup : public APickup
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AHealthPickup();

	// override the WasCollected function -- use Implementation because its Blueprint Native Event
	void WasCollected_Implementation() override;
	
	
	
};
