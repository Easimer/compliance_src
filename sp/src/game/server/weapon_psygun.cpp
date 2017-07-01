#include "cbase.h"
#include "weapon_psygun.h"
#include "particle_system.h"

//#include "hl2_player_shared.h"
#include "basecombatcharacter.h"

CWeaponPsyArm::CWeaponPsyArm()
{
	//m_iNextAttack = gpGlobals->curtime;
}

void CWeaponPsyArm::Precache()
{
	PrecacheParticleSystem("psy_hand_fire");
	//PrecacheModel("models/weapons/v_psyarm");
	BaseClass::Precache();
}

void CWeaponPsyArm::Spawn(void)
{
	Precache();
	SetThink(&CWeaponPsyArm::Think);
	SetNextThink(gpGlobals->curtime + 0.5f);

	pPSys = NULL;

	BaseClass::Spawn();
}

void CWeaponPsyArm::Think(void)
{
	if (m_iClip1 < 200)
		m_iClip1++;
	SetNextThink(gpGlobals->curtime + 0.5f);
	/*if (!pPSys)
	{
		Vector vParticle;
		QAngle aParticle;
		if (!GetAttachment("particle", vParticle, aParticle))
		{
			DevMsg("[psyarm] weapon particle attach err\n");
			return;
		}
	}*/
}

void CWeaponPsyArm::PrimaryAttack()
{
	auto pOwner = GetOwner();
	if (!pOwner)
		return;

	m_flNextPrimaryAttack = gpGlobals->curtime + 0.1f;
	trace_t tr;

	Vector vStart, vDir;
	QAngle aStart;
	if (!GetAttachment("particle", vStart, aStart))
	{
		DevMsg("[psyarm] weapon particle attach err\n");
		return;
	}
	AngleVectors(aStart, &vDir);

	SendWeaponAnim(GetPrimaryAttackActivity());
	WeaponSound(SINGLE);

	BaseClass::PrimaryAttack();
	m_iClip1--;

	CBaseEntity::Create("npc_proj_psy", vStart, aStart, GetOwner());
}

Activity CWeaponPsyArm::GetPrimaryAttackActivity(void)
{
	return ACT_VM_PRIMARYATTACK;
}

Activity CWeaponPsyArm::GetSecondaryAttackActivity(void)
{
	return ACT_VM_PRIMARYATTACK;
}

Activity CWeaponPsyArm::GetReloadActivity(void)
{
	return ACT_VM_FIZZLE;
}
