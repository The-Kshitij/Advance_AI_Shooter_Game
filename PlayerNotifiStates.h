
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "PlayerNotifiStates.generated.h"

/**
 * 
 */
UCLASS()
class RESUMESHOWCASE_API UPlayerNotifiStates : public UAnimNotifyState
{
	GENERATED_BODY()
private:
	FString AnimationName;
	class AMainCharacter* MainPlayer;

	void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
