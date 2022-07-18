#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "PlayerNotifies.generated.h"

/**
 * 
 */
UCLASS()
class RESUMESHOWCASE_API UPlayerNotifies : public UAnimNotify
{
	GENERATED_BODY()
private:	
	//called in aniamation
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
