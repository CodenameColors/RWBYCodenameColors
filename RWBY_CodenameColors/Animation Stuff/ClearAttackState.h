// Authour: Antonio Morales.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "ClearAttackState.generated.h"

/**
 * 
 */
UCLASS()
class RWBY_CODENAMECOLORS_API UClearAttackState : public UAnimNotify
{
	GENERATED_BODY()
	
		void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	
};
