// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "UneirosTrialsCharacter.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Pickup.h"
#include "PlatformPickup.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

//////////////////////////////////////////////////////////////////////////
// AUneirosTrialsCharacter

AUneirosTrialsCharacter::AUneirosTrialsCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// base health and power of the character
	CharacterHealth = 1000.0f;
	SoulPower = 1000.0f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// create a collectionSphere
	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->SetupAttachment(RootComponent);
	CollectionSphere->SetSphereRadius(200.0f);
	
	// create and set TriggerCapsule. Its properties too
	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TriggerCapsule"));
	TriggerCapsule->SetupAttachment(RootComponent);
	TriggerCapsule->InitCapsuleSize(42.0f, 96.0f);
	TriggerCapsule->SetCollisionProfileName(TEXT("Trigger"));

	// The default events that will happen on overlaps
	TriggerCapsule->OnComponentBeginOverlap.AddDynamic(this, &AUneirosTrialsCharacter::OnOverlapBegin);
	TriggerCapsule->OnComponentEndOverlap.AddDynamic(this, &AUneirosTrialsCharacter::OnOverlapEnd);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUneirosTrialsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Collect", IE_Pressed, this, &AUneirosTrialsCharacter::CollectPickups);

	PlayerInputComponent->BindAxis("MoveForward", this, &AUneirosTrialsCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AUneirosTrialsCharacter::MoveRight);
	
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AUneirosTrialsCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AUneirosTrialsCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AUneirosTrialsCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AUneirosTrialsCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AUneirosTrialsCharacter::OnResetVR);
}


void AUneirosTrialsCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AUneirosTrialsCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AUneirosTrialsCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AUneirosTrialsCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AUneirosTrialsCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AUneirosTrialsCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AUneirosTrialsCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AUneirosTrialsCharacter::CollectPickups()
{
	// get all overlapping actors and store them in an array
	TArray<AActor*> CollectedActors;
	CollectionSphere->GetOverlappingActors(CollectedActors);
	
	// for each Actor we collected
	for (int32 iCollected = 0; iCollected < CollectedActors.Num(); ++iCollected)
	{
		// cast the actor to APickup
		APickup* const TestPickup = Cast<APickup>(CollectedActors[iCollected]);
		// If the cast is successful and the pickup is valid and active
		if (TestPickup && !TestPickup->IsPendingKill() && TestPickup->IsActive())
		{
			// Call the pickup's WasCollected function
			TestPickup->WasCollected();
			// deactivate the pickup
			TestPickup->SetActive(false);
		}
	}
}

// changes character jump velocity to higher whenever trigger volume is entered
void AUneirosTrialsCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent * OtherComp, int32
	OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{

		ACharacter * MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (MyCharacter && MyCharacter->IsJumpProvidingForce() == false)
		{
			GetCharacterMovement()->JumpZVelocity = 2000.f;
		}
		/*if (MyCharacter)
		{
			MyCharacter->GetMesh()->SetSimulatePhysics(true);
			MyCharacter->GetMovementComponent()->MovementState.bCanJump = false;
		}*/
	}
}

// changes character jump velocity to normal whenever trigger volume is left
void AUneirosTrialsCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent * OtherComp, int32
	OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		ACharacter * MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (MyCharacter)
		{
			GetCharacterMovement()->JumpZVelocity = 600.f;
		}
		/*if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Overlap is now done"));
		}*/
	}
}

float AUneirosTrialsCharacter::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, 
	AActor * DamageCauser)
{
	ACharacter * MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	CharacterHealth = CharacterHealth - Damage;

	FString ProjectileDebugString = GetName();
	UE_LOG(LogClass, Log, TEXT("You are damaged by %s"), *ProjectileDebugString);

	if (CharacterHealth <= 0.0f)
	{
		if (MyCharacter)
		{
			MyCharacter->GetMesh()->SetSimulatePhysics(true);
		}
	}
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}