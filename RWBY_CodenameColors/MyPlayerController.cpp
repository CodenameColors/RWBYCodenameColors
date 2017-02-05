// Authour: Antonio Morales.

#include "RWBY_CodenameColors.h"
#include "MyPlayerController.h"
#include "RWBY_CodenameColorsCharacter.h"


void AMyPlayerController::OnKilled() {

	AGameMode* GameMode = GetWorld()->GetAuthGameMode();
	ARWBY_CodenameColorsCharacter* CurrentlyPlaying = Cast<ARWBY_CodenameColorsCharacter>(this->GetActorClass());

	if (CurrentlyPlaying) {
		CurrentPlayerClass = CurrentlyPlaying;
	}

	UnPossess();
	GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &AMyPlayerController::Respawn, 5.0f);
}

void AMyPlayerController::Respawn() {

	AGameMode* GameMode = GetWorld()->GetAuthGameMode();

	if (GameMode) {
		APawn * NewPawn = GameMode->SpawnDefaultPawnFor(this, StartSpot.Get());

		Possess(NewPawn);
	}
}

void AMyPlayerController::SetMaxSpeed() {
	GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = 120;

}


