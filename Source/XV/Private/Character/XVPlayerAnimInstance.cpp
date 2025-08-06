#include "Character/XVPlayerAnimInstance.h"

void UXVPlayerAnimInstance::PlayAttackAnim()
{
	Montage_Play(AttackAnimMontage);
}

void UXVPlayerAnimInstance::PlayGunChangeAnim()
{
	Montage_Play(ChangeAnimMontage);
}

void UXVPlayerAnimInstance::PlayReloadAnim()
{
	Montage_Play(ReloadAnimMontage);
}

void UXVPlayerAnimInstance::PlayDieAnim()
{
	Montage_Play(DieAnimMontage);
}
