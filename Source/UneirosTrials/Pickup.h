// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class UNEIROSTRIALS_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

	// return whether or not the pickup is active
	UFUNCTION(BlueprintPure, Category = "Pickup")
	bool IsActive();

	// allows other classes to safely change
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetActive(bool PickupState);

	// function to call when pickup is collected
	UFUNCTION(BlueprintNativeEvent)
	void WasCollected();
	virtual void WasCollected_Implementation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// true when pickup can be used and vice versa
	bool bIsActive;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// returns the mesh for the pickup
	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return PickupMesh; }

private:
	// represents the static mesh of the pickup
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", Meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PickupMesh;
	
};
