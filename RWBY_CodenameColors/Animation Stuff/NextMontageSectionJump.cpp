// Authour: Antonio Morales.

#include "RWBY_CodenameColors.h"
#include "NextMontageSectionJump.h"
#include "RWBY_CodenameColorsCharacter.h"
#include "RubyRose.h"


void UNextMontageSectionJump::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) {

	ARWBY_CodenameColorsCharacter* TestChar = Cast<ARWBY_CodenameColorsCharacter>(MeshComp->GetAnimInstance()->GetOwningActor());

	if (TestChar) {

		ARubyRose* Ruby = Cast<ARubyRose>(TestChar);
		if (Ruby) {
			
			if (Ruby->NextMontageSection == "None") {
				MeshComp->GetAnimInstance()->Montage_JumpToSection(FName("End"), Ruby->CurrentMontagePlaying);
				TestChar->GetCharacterMovement()->MaxWalkSpeed = 600;
				Ruby->LastHitActor = nullptr;
			}
			else {
				MeshComp->GetAnimInstance()->Montage_JumpToSection(FName(*Ruby->NextMontageSection), Ruby->CurrentMontagePlaying);
			}

		}
	}
}
