// Authour: Antonio Morales.

#include "RWBY_CodenameColors.h"
#include "ClearAttackState.h"
#include "RWBY_CodenameColorsCharacter.h"
#include "RubyRose.h"

void UClearAttackState::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) {

	ARWBY_CodenameColorsCharacter* TestChar = Cast<ARWBY_CodenameColorsCharacter>(MeshComp->GetAnimInstance()->GetOwningActor());

	if (TestChar) {
		ARubyRose* TestRuby = Cast<ARubyRose>(TestChar);
		if (TestRuby) {
			//TestRuby->CurrentAttack = EAttacks::None;
			TestRuby->HitInputed = false;
			TestRuby->NextMontageSection = "";
		}
	}
}



