// Authour: Antonio Morales.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "LedgeClimb.generated.h"

/**
 * 
 */
UCLASS()
class RWBY_CODENAMECOLORS_API ULedgeClimb : public UAnimNotify
{
	GENERATED_BODY()
	
		void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	
};
