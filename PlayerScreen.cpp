#include "PlayerScreen.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"

bool UPlayerScreen::Initialize()
{
	bool bSuccess = Super::Initialize();
	if (Txt_Ammo != nullptr) UE_LOG(LogTemp, Warning, TEXT("Ammo Text Block found."))
	else UE_LOG(LogTemp, Error, TEXT("Ammo Text Block not found."));
	
	if (Txt_Mag != nullptr) UE_LOG(LogTemp, Warning, TEXT("Mag Text Block found."))
	else UE_LOG(LogTemp, Error, TEXT("Mag Text Block not found."));

	if (Img_Aim_Reticle != nullptr) UE_LOG(LogTemp, Warning, TEXT("Aim Reticle found."))
	else UE_LOG(LogTemp, Error, TEXT("Aim Reticle not found."));

	if (HealthBar != nullptr) UE_LOG(LogTemp, Warning, TEXT("HealthBar found."))
	else UE_LOG(LogTemp, Error, TEXT("HealthBar not found."));

	if (Txt_Slash != nullptr) UE_LOG(LogTemp, Warning, TEXT("Slash text found."))
	else UE_LOG(LogTemp, Error, TEXT("Slash text not found."));

	return bSuccess;
}

void UPlayerScreen::HideEverything()
{
	if (Txt_Ammo) Txt_Ammo->SetVisibility(ESlateVisibility::Hidden);
	if (Txt_Mag) Txt_Mag->SetVisibility(ESlateVisibility::Hidden);
	if (Txt_Slash) Txt_Slash->SetVisibility(ESlateVisibility::Hidden);
	if (Img_Aim_Reticle) Img_Aim_Reticle->SetVisibility(ESlateVisibility::Hidden);
	if (HealthBar) HealthBar->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerScreen::HideWeaponRelated()
{
	if (Txt_Ammo) Txt_Ammo->SetVisibility(ESlateVisibility::Hidden);
	if (Txt_Mag) Txt_Mag->SetVisibility(ESlateVisibility::Hidden);
	if (Txt_Slash) Txt_Slash->SetVisibility(ESlateVisibility::Hidden);
	if (Img_Aim_Reticle) Img_Aim_Reticle->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerScreen::ShowEverything()
{
	if (Txt_Ammo) Txt_Ammo->SetVisibility(ESlateVisibility::Visible);
	if (Txt_Mag) Txt_Mag->SetVisibility(ESlateVisibility::Visible);
	if (Txt_Slash) Txt_Slash->SetVisibility(ESlateVisibility::Visible);
	if (Img_Aim_Reticle) Img_Aim_Reticle->SetVisibility(ESlateVisibility::Visible);
	if (HealthBar) HealthBar->SetVisibility(ESlateVisibility::Visible);
}

void UPlayerScreen::ShowWeaponRelated()
{
	if (Txt_Ammo) Txt_Ammo->SetVisibility(ESlateVisibility::Visible);
	if (Txt_Mag) Txt_Mag->SetVisibility(ESlateVisibility::Visible);
	if (Txt_Slash) Txt_Slash->SetVisibility(ESlateVisibility::Visible);
	if (Img_Aim_Reticle) Img_Aim_Reticle->SetVisibility(ESlateVisibility::Visible);
}

void UPlayerScreen::SetAmmoText(int ammo)
{	
	FString ammo_str = FString::FromInt(ammo);
	if (Txt_Ammo) Txt_Ammo->SetText(FText::FromString(FString::FromInt(ammo)));
	if (Txt_Ammo == 0)
	{
		LowAmmoDisplay();
	}
	else
	{
		NormalDisplay();
	}
}

void UPlayerScreen::SetMagText(int mag)
{
	FString mag_str = FString::FromInt(mag);
	if (Txt_Mag) Txt_Mag->SetText(FText::FromString(FString::FromInt(mag)));
}

void UPlayerScreen::ShowHealthRelated()
{
	if (HealthBar) HealthBar->SetVisibility(ESlateVisibility::Visible);
}

void UPlayerScreen::SetHealth(float health, bool bnormalised)
{
	if (HealthBar) HealthBar->SetPercent( bnormalised ? health : health / 100.f);
}