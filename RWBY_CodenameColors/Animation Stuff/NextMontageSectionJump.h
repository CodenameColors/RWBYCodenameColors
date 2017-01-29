// Authour: Antonio Morales.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "NextMontageSectionJump.generated.h"

/**
 * 
 */
UCLASS()
class RWBY_CODENAMECOLORS_API UNextMontageSectionJump : public UAnimNotify
{
	GENERATED_BODY()
	
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
