// Authour: Antonio Morales.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "MyNativeAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class RWBY_CODENAMECOLORS_API UMyNativeAnimNotify : public UAnimNotify
{
	GENERATED_BODY()
	
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, Category = Parameters)
		float TestFloatParameter;
	UPROPERTY(EditAnywhere, Category = Parameters)
		UClass* TestClassParameter;
	UPROPERTY(EditAnywhere, Category = Parameters)
		TArray<int32> TestArrayParameter;
	
	
};
