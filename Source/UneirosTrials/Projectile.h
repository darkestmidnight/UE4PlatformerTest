// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class UNEIROSTRIALS_API AProjectile : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// damages the character that touches it
	UFUNCTION()
		//virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent * OtherComp, int32
			//OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
		virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
			FVector NormalImpulse, const FHitResult& Hit);

	// damage type to use
	UPROPERTY(EditAnywhere, Category = "DamageType")
	TSubclassOf<UDamageType> ProjectileDamage;

	// base damage of the base projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float BaseDamage;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// returns the mesh for the projectile
	FORCEINLINE class UStaticMeshComponent * GetMesh() const { return ProjectileMesh; }
	
private:
	// represents the mesh for the projectile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent * ProjectileMesh;
	
};
