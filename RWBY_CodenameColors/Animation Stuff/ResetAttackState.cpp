// Authour: Antonio Morales.

#include "RWBY_CodenameColors.h"
#include "ResetAttackState.h"
#include "RWBY_CodenameColorsCharacter.h"
#include "RubyRose.h"

void UResetAttackState::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) {

	ARWBY_CodenameColorsCharacter* TestChar = Cast<ARWBY_CodenameColorsCharacter>(MeshComp->GetAnimInstance()->GetOwningActor());

	if (TestChar) {
		TestChar->LastHitActor = nullptr;
		TestChar->bMeleeAttacking = false;
	}
}