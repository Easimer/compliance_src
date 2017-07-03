#pragma once

#include "cbase.h"
#include "basehlcombatweapon_shared.h"
#include "soundent.h"

class CWeaponSilenced : public CBaseHLCombatWeapon
{
public:
	DECLARE_CLASS(CWeaponSilenced, CBaseHLCombatWeapon);
	CWeaponSilenced();
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	void Precache(void);
	void Spawn(void);
	void PrimaryAttack(void);
	void DryFire(void);
	bool Reload(void);
	int GetMinBurst(){ return 1; }
	int GetMaxBurst(){ return 3; }
	float GetFireRate(void) { return 0.5f; }
	const Vector& GetBulletSpread(void) { return m_vSpreadCone; }
	DECLARE_ACTTABLE();
	DECLARE_DATADESC();
private:
	Vector m_vSpreadCone;
};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponSilenced, DT_WeaponSilenced)

BEGIN_NETWORK_TABLE(CWeaponSilenced, DT_WeaponSilenced)
END_NETWORK_TABLE()

PRECACHE_WEAPON_REGISTER(weapon_silenced);

BEGIN_DATADESC(CWeaponSilenced)
END_DATADESC()

BEGIN_PREDICTION_DATA(CWeaponSilenced)
END_PREDICTION_DATA()

#ifndef CLIENT_DLL
LINK_ENTITY_TO_CLASS(weapon_silenced, CWeaponSilenced);
#endif

acttable_t	CWeaponSilenced::m_acttable[] =
{
	{ ACT_IDLE, ACT_IDLE_PISTOL, true },
	{ ACT_IDLE_ANGRY, ACT_IDLE_ANGRY_PISTOL, true },
	{ ACT_RANGE_ATTACK1, ACT_RANGE_ATTACK_PISTOL, true },
	{ ACT_RELOAD, ACT_RELOAD_PISTOL, true },
	{ ACT_WALK_AIM, ACT_WALK_AIM_PISTOL, true },
	{ ACT_RUN_AIM, ACT_RUN_AIM_PISTOL, true },
	{ ACT_GESTURE_RANGE_ATTACK1, ACT_GESTURE_RANGE_ATTACK_PISTOL, true },
	{ ACT_RELOAD_LOW, ACT_RELOAD_PISTOL_LOW, false },
	{ ACT_RANGE_ATTACK1_LOW, ACT_RANGE_ATTACK_PISTOL_LOW, false },
	{ ACT_COVER_LOW, ACT_COVER_PISTOL_LOW, false },
	{ ACT_RANGE_AIM_LOW, ACT_RANGE_AIM_PISTOL_LOW, false },
	{ ACT_GESTURE_RELOAD, ACT_GESTURE_RELOAD_PISTOL, false },
	{ ACT_WALK, ACT_WALK_PISTOL, false },
	{ ACT_RUN, ACT_RUN_PISTOL, false },
};

IMPLEMENT_ACTTABLE(CWeaponSilenced);

#define SILENCED_PISTOL_ATTACK_TIME 0.5

CWeaponSilenced::CWeaponSilenced()
{
	m_bFiresUnderwater = true;
	m_fMinRange1 = 24;
	m_fMaxRange1 = 1500;
	m_fMinRange2 = 24;
	m_fMaxRange2 = 200;
	m_vSpreadCone = VECTOR_CONE_1DEGREES;
	m_bIsSuppressed = true;
	m_iSuppressorDurability = 16;
	m_iSuppressorMaxDurability = 16;
}

void CWeaponSilenced::Precache(void)
{
	BaseClass::Precache();
}

void CWeaponSilenced::Spawn(void)
{
	BaseClass::Spawn();
	// pls always set these after spawn
	m_bIsSuppressed = true;
	m_iSuppressorDurability = 16;
	m_iSuppressorMaxDurability = 16;
}

void CWeaponSilenced::DryFire(void)
{
	WeaponSound(EMPTY);
	SendWeaponAnim(ACT_VM_DRYFIRE);
	m_flNextPrimaryAttack = gpGlobals->curtime + SILENCED_PISTOL_ATTACK_TIME;
}

void CWeaponSilenced::PrimaryAttack(void)
{
	if (m_iSuppressorDurability == 0)
	{
		CSoundEnt::InsertSound(SOUND_COMBAT, GetAbsOrigin(), SOUNDENT_VOLUME_PISTOL, 0.2, GetOwner());
		EmitSound("Weapon_Pistol.Single");
	}

	CBasePlayer *pOwner = ToBasePlayer(GetOwner());

	if (pOwner)
	{
		// Each time the player fires the pistol, reset the view punch. This prevents
		// the aim from 'drifting off' when the player fires very quickly. This may
		// not be the ideal way to achieve this, but it's cheap and it works, which is
		// great for a feature we're evaluating. (sjb)
		pOwner->ViewPunchReset();
	}

	BaseClass::PrimaryAttack();

	m_iPrimaryAttacks++;
	m_flNextPrimaryAttack = gpGlobals->curtime + SILENCED_PISTOL_ATTACK_TIME;
}

bool CWeaponSilenced::Reload(void)
{
	bool fRet = DefaultReload(GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD);
	if (fRet)
	{
		WeaponSound(RELOAD);
	}
	return fRet;
}

void CHEAT_PutSuppressor(const CCommand& cmd)
{
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer)
		return;
	CBaseCombatWeapon* pWeapon = pPlayer->GetActiveWeapon();
	if (!pWeapon)
		return;
	pWeapon->m_bIsSuppressed = true;
}

void CHEAT_SetSuppressorDurability(const CCommand& cmd)
{
	if (cmd.ArgC() < 2)
		return;
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer)
		return;
	CBaseCombatWeapon* pWeapon = pPlayer->GetActiveWeapon();
	if (!pWeapon)
		return;
	int nVal;
	const char* szVal = cmd.ArgV()[1];
	nVal = V_atoi(szVal);
	pWeapon->m_iSuppressorDurability = nVal;
}
void CHEAT_SetSuppressorMaxDurability(const CCommand& cmd)
{
	if (cmd.ArgC() < 2)
		return;
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer)
		return;
	CBaseCombatWeapon* pWeapon = pPlayer->GetActiveWeapon();
	if (!pWeapon)
		return;
	int nVal;
	const char* szVal = cmd.ArgV()[1];
	nVal = V_atoi(szVal);
	pWeapon->m_iSuppressorMaxDurability = nVal;
}

static ConCommand sv_putsuppressor("sv_putsuppressor", CHEAT_PutSuppressor, "Puts suppressor on the current weapon");
static ConCommand sv_setsuppressor("sv_setsuppressor_dur", CHEAT_SetSuppressorDurability, "Sets the current weapons's suppressor durability");
static ConCommand sv_setsuppressormax("sv_setsuppressor_maxdur", CHEAT_SetSuppressorMaxDurability, "Sets the current weapons's maximum suppressor durability");
