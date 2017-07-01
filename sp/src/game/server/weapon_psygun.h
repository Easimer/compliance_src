#pragma once

#include "basehlcombatweapon_shared.h"
#include "particle_system.h"

#ifdef CLIENT_DLL
#define CWeaponPsyArm C_WeaponPsyArm
#endif

class CWeaponPsyArm : public CBaseHLCombatWeapon
{
public:
	DECLARE_CLASS(CWeaponPsyArm, CBaseHLCombatWeapon);
	CWeaponPsyArm();
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	void Precache(void);
	void Spawn(void);
	void PrimaryAttack(void);
	void Think(void);
	//void SecondaryAttack(void);
	//void TertiaryAttack(void);
	void DryFire(void);
	//void Think(void);
	Activity GetPrimaryAttackActivity(void);
	Activity GetSecondaryAttackActivity(void);
	Activity GetReloadActivity(void);
	int GetMinBurst(){ return 1; }
	int GetMaxBurst(){ return 3; }
	float GetFireRate(void) { return 0.1f; }
	DECLARE_ACTTABLE();
	DECLARE_DATADESC();
private:
	int m_iCurrentElement;
	CParticleSystem* pPSys;
};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponPsyArm, DT_WeaponPsyArm)

BEGIN_NETWORK_TABLE(CWeaponPsyArm, DT_WeaponPsyArm)
#ifdef CLIENT_DLL
#else
#endif
END_NETWORK_TABLE()

PRECACHE_WEAPON_REGISTER(weapon_psyarm);

BEGIN_DATADESC(CWeaponPsyArm)
DEFINE_THINKFUNC(Think)
END_DATADESC()

BEGIN_PREDICTION_DATA(CWeaponPsyArm)
END_PREDICTION_DATA()

#ifndef CLIENT_DLL
LINK_ENTITY_TO_CLASS(weapon_psyarm, CWeaponPsyArm);
#endif

acttable_t	CWeaponPsyArm::m_acttable[] =
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

IMPLEMENT_ACTTABLE(CWeaponPsyArm);
