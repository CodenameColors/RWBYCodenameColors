// Authour: Antonio Morales.

#pragma once

#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RWBY_CODENAMECOLORS_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
		FTimerHandle TimerHandle_Respawn;

public:

	void OnKilled();

	void Respawn();

	void SetMaxSpeed();
	
	
};
