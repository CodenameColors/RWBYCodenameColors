// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "RWBY_CodenameColors.h"
#include "RWBY_CodenameColorsGameMode.h"
#include "RWBY_CodenameColorsCharacter.h"

ARWBY_CodenameColorsGameMode::ARWBY_CodenameColorsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("Blueprint'/Game/SideScrollerCPP/Blueprints/BaseCharacter.BaseCharacter'"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
