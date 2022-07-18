#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerScreen.generated.h"

/**
 * 
 */
UCLASS()
class RESUMESHOWCASE_API UPlayerScreen : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* Txt_Ammo;

	//hide the everything of the player
	void HideEverything();
	//hides only ammo text, mag text and img_aim_reticle
	void HideWeaponRelated();
	//show everything
	void ShowEverything();
	//to set the ammo text
	void SetAmmoText(int ammo);
	//to set the mag text
	void SetMagText(int mag);
	//to show weapon realated things
	void ShowWeaponRelated();
	//to show health realted things
	void ShowHealthRelated();
	//to set health
	void SetHealth(float health, bool bnormalised = false);

	UFUNCTION(BlueprintImplementableEvent)
		void LowAmmoDisplay();
	UFUNCTION(BlueprintImplementableEvent)
		void NormalDisplay();

private:	
	UPROPERTY(meta = (BindWidget))
		class UImage* Img_Aim_Reticle;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* Txt_Slash;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* Txt_Mag;
	
	bool Initialize();
};
