// Authour: Antonio Morales.

#include "RWBY_CodenameColors.h"
#include "MyPlayerController.h"


void AMyPlayerController::OnKilled() {

	UnPossess();
	GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &AMyPlayerController::Respawn, 5.0f);
}

void AMyPlayerController::Respawn() {

	AGameMode* GameMode = GetWorld()->GetAuthGameMode();

	if (GameMode) {
		APawn * NewPawn = GameMode->SpawnDefaultPawnFor(this, StartSpot.Get());;
		Possess(NewPawn);
	}
}


