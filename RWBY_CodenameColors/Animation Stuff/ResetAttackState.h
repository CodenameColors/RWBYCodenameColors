// Authour: Antonio Morales.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "ResetAttackState.generated.h"

/**
 * 
 */
UCLASS()
class RWBY_CODENAMECOLORS_API UResetAttackState : public UAnimNotify
{
	GENERATED_BODY()
	
		void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	
};
