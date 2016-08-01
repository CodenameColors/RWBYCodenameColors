// Authour: Antonio Morales.

#include "RWBY_CodenameColors.h"
#include "MyNativeAnimNotify.h"
#include "RWBY_CodenameColorsCharacter.h"
#include "RubyRose.h"


void UMyNativeAnimNotify::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation){
	
	ARWBY_CodenameColorsCharacter* TestChar = Cast<ARWBY_CodenameColorsCharacter>(MeshComp->GetAnimInstance()->GetOwningActor());

	if ( TestChar ) {
		if (TestChar->bMeleeAttacking) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Hit Notify"));
	
		}
		else {
			ARubyRose* Ruby = Cast<ARubyRose>(TestChar);
			if (Ruby) {
				//Ruby->MoveToSection();
				MeshComp->GetAnimInstance()->Montage_JumpToSection(FName("Attack End"), Ruby->Melee);
				Ruby->LastHitActor = nullptr;
			}
		}
	}

}
