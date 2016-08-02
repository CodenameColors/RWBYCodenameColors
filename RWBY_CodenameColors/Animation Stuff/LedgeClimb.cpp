// Authour: Antonio Morales.

#include "RWBY_CodenameColors.h"
#include "LedgeClimb.h"
#include "RWBY_CodenameColorsCharacter.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

void ULedgeClimb::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) {

	
	ARWBY_CodenameColorsCharacter* TestChar = Cast<ARWBY_CodenameColorsCharacter>(MeshComp->GetAnimInstance()->GetOwningActor());

	if (TestChar) {
		
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;

		UKismetSystemLibrary::MoveComponentTo(TestChar->GetRootComponent(), TestChar->ClimbPosition, TestChar->GetActorRotation() , false, false, .35f, EMoveComponentAction::Move, LatentInfo);
	}
	
}


