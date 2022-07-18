#include "PlayerNotifiStates.h"

#include "MainCharacter.h"

void UPlayerNotifiStates::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp)
	{
		MainPlayer = Cast<AMainCharacter>(MeshComp->GetOwner());
		if (MainPlayer)
		{
			
			AnimationName = Animation->GetName();
			if (AnimationName.Compare("P_GrabPistol") == 0 || AnimationName.Compare("P_EquipRifle") == 0) MainPlayer->AttachWeaponToHand();
			else if (AnimationName.Compare("P_HolsterPistol") == 0 || AnimationName.Compare("P_UnEquipRifle") == 0) MainPlayer->AttachWeaponToBack();			
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Casting to main player failed in PlayerNotifiStates.cpp."));
		}
	}	
}

void UPlayerNotifiStates::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MainPlayer)
	{
		if (AnimationName.Compare("P_GrabPistol") == 0 || AnimationName.Compare("P_EquipRifle") == 0) MainPlayer->EquipedWeapon();
		else if (AnimationName.Compare("P_HolsterPistol") == 0 || AnimationName.Compare("P_UnEquipRifle") == 0) MainPlayer->UnEquipedWeapon();
		else if (AnimationName.Compare("P_Reload_Pistol") == 0 || AnimationName.Compare("P_Reload_Rifle") == 0) MainPlayer->SetReloading(false);
	}
}