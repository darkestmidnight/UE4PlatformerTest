// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "PlatformPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// create the static mesh for the projectile
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	RootComponent = ProjectileMesh;
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	//ProjectileMesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// called whenever an actor touches the projectile
void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse, const FHitResult& Hit)
{
	//AProjectile EventInstigator;
	//EventInstigator.Instigator = Instigator;
	
	//const FVector BackwardDir = OtherActor->GetRootComponent()->GetRightVector();

	const FVector ActorImpulse = FVector(10000, 0, 0);

	if (OtherActor && (OtherActor != this) && OtherComponent)
	{
		// logging a debug message
		
		FString ProjectileDebugString = GetName();
		UE_LOG(LogClass, Log, TEXT("You have touched %s"), *ProjectileDebugString);
		OtherComponent->SetSimulatePhysics(true);
		OtherComponent->AddForce(FVector::RightVector * OtherComponent->GetBodyInstance()->GetBodyMass() * 1000);
		//UGameplayStatics::ApplyDamage(OtherActor, 100.0f, EventInstigator.GetInstigatorController(), this, ProjectileDamage);
	}
	
}
