// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "RWBY_CodenameColors.h"
#include "RWBY_CodenameColorsGameMode.h"
#include "RWBY_CodenameColorsCharacter.h"
#include "MyPlayerController.h"

ARWBY_CodenameColorsGameMode::ARWBY_CodenameColorsGameMode()
{
	PlayerControllerClass = AMyPlayerController::StaticClass();
}
