#include "CXPlayerState.h"
#include "Net/UnrealNetwork.h"
void ACXPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
 Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 DOREPLIFETIME_CONDITION(ACXPlayerState, Coins, COND_OwnerOnly);
 DOREPLIFETIME_CONDITION(ACXPlayerState, BonusMaxSpeed, COND_OwnerOnly);
}
bool ACXPlayerState::AddCoins(int32 Amount)
{
 if (!HasAuthority() || Amount <= 0 || Coins > MAX_int32 - Amount) return false;
 Coins += Amount; ForceNetUpdate(); return true;
}
bool ACXPlayerState::RemoveCoins(int32 Amount)
{
 if (!HasAuthority() || Amount <= 0 || Coins < Amount) return false;
 Coins -= Amount; ForceNetUpdate(); return true;
}
bool ACXPlayerState::AddBonusMaxSpeed(float Amount)
{
 if (!HasAuthority() || !FMath::IsFinite(Amount) || Amount <= 0.f || !FMath::IsFinite(BonusMaxSpeed + Amount)) return false;
 BonusMaxSpeed += Amount; ForceNetUpdate(); return true;
}
