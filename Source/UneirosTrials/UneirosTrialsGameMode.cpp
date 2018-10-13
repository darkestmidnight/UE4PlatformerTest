// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "UneirosTrialsGameMode.h"
#include "UneirosTrialsCharacter.h"
#include "Pickup.h"
#include "UObject/ConstructorHelpers.h"

AUneirosTrialsGameMode::AUneirosTrialsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}