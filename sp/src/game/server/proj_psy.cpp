#include "cbase.h"
#include "proj_psy.h"

CProjectilePsy::CProjectilePsy()
{
	m_iszEffectName = MAKE_STRING("psy_hand_fire");
}

void CProjectilePsy::Precache()
{
	PrecacheParticleSystem("psy_hand_fire");
	BaseClass::Precache();
}

void CProjectilePsy::Spawn()
{
	Precache();

	m_bGrace = true;
	m_takedamage = DAMAGE_YES;
	m_iHealth = 1;
	m_iszEffectName = MAKE_STRING("psy_hand_fire");
	
	SetSize(-Vector(4, 4, 4), Vector(4, 4, 4));
	SetCollisionGroup(COLLISION_GROUP_WEAPON);
	VPhysicsInitNormal(SOLID_BBOX, 0, false);
	AddSolidFlags(FSOLID_NOT_SOLID | FSOLID_TRIGGER);

	AngleVectors(GetAbsAngles(), &m_vVelocity);
	m_vVelocity *= 10;
	SetAbsVelocity(m_vVelocity);

	SetNextThink(gpGlobals->curtime + 0.2f);

	DevMsg("ayy lmao familia\n");
	StartParticleSystem();

	BaseClass::Spawn();
}

void CProjectilePsy::Think()
{
	m_bGrace = false;
	DevMsg("Grace period over\n");
	BaseClass::Think();
}

void CProjectilePsy::Touch(CBaseEntity* pOther)
{
	if (m_bGrace || pOther == GetOwnerEntity())
		return;
	DevMsg("TOUCH :D\n");
	if (pOther->IsCombatCharacter())
	{
		CTakeDamageInfo dmg;
		dmg.AddDamage(10);
		dmg.AddDamageType(DMG_PLASMA);
		auto pChar = ToBaseCombatCharacter(pOther);
		pChar->TakeDamage(dmg);
	}
	//UTIL_Remove(this);
}
