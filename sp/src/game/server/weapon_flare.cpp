#include "cbase.h"
#include "basehlcombatweapon_shared.h"
#include "props.h"
#include "weapon_flaregun.h"
#include "hl2_player.h"
#include "in_buttons.h"
#include "memdbgon.h"

static ConVar sk_plr_dmg_thrown("sk_plr_dmg_thrown", "0");
static ConVar sk_npc_dmg_thrown("sk_npc_dmg_thrown", "0");
static ConVar sk_max_thrown("sk_max_thrown", "0");
#if 0
class CWeaponFlare : public CBaseHLCombatWeapon {
public:
	DECLARE_CLASS(CWeaponFlare, CBaseHLCombatWeapon);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CWeaponFlare();
	void Precache(void);
	void Spawn(void);
	void PrimaryAttack(void);

	void ItemPostFrame();

public:
	DECLARE_ACTTABLE();
	DECLARE_DATADESC();

private:
};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponFlare, DT_WeaponFlare)

BEGIN_NETWORK_TABLE(CWeaponFlare, DT_WeaponFlare)
END_NETWORK_TABLE()

PRECACHE_WEAPON_REGISTER(weapon_flare);

BEGIN_DATADESC(CWeaponFlare)
END_DATADESC()

BEGIN_PREDICTION_DATA(CWeaponFlare)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(weapon_flare, CWeaponFlare);

acttable_t	CWeaponFlare::m_acttable[] =
{
	{ ACT_RANGE_ATTACK1, ACT_RANGE_ATTACK_SLAM, true },
};

IMPLEMENT_ACTTABLE(CWeaponFlare);

#define WEAPON_FLARE_DURATION 30

CWeaponFlare::CWeaponFlare()
{
	m_bFiresUnderwater = false;
}

void CWeaponFlare::Precache()
{
	BaseClass::Precache();
	UTIL_PrecacheOther("env_flare");
	PrecacheModel("models/props_junk/flare.mdl");
}

void CWeaponFlare::Spawn()
{
	BaseClass::Spawn();
	m_flNextPrimaryAttack = gpGlobals->curtime + 1.0f;
}

void CWeaponFlare::PrimaryAttack()
{
	if (gpGlobals->curtime < m_flNextPrimaryAttack)
		return;

	m_flNextPrimaryAttack = gpGlobals->curtime + 1.0f;

	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
	if (!pPlayer)
		return;

	CHL2_Player* pHLPlayer = ((CHL2_Player*)pPlayer);

	if (pHLPlayer->m_HL2Local.m_nFlares == 0)
		return;

	Vector	vecEye = pPlayer->EyePosition();
	Vector	vForward, vRight;

	pPlayer->EyeVectors(&vForward, &vRight, NULL);
	Vector vecSrc = vecEye + vForward * 18.0f + vRight * 8.0f + Vector(0, 0, -8);

	vForward[2] += 0.1f;

	Vector vecThrow;
	pPlayer->GetVelocity(&vecThrow, NULL);
	vecThrow += vForward * 350 + Vector(0, 0, 50);
	AngularImpulse impulse(200, random->RandomInt(-600, 600), 0);

	//Fraggrenade_Create( vecSrc, vec3_angle, vecThrow, AngularImpulse(200,random->RandomInt(-600,600),0), pPlayer, GRENADE_TIMER, false );

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);
	WeaponSound(SPECIAL1);

	CPhysicsProp* pProp = (CPhysicsProp*)CPhysicsProp::CreateNoSpawn("prop_physics", vecSrc, vec3_angle, pPlayer);
	pProp->SetModel("models/props_junk/flare.mdl");
	DispatchSpawn(pProp);
	IPhysicsObject* pPhysObj = pProp->VPhysicsGetObject();
	if (pPhysObj)
		pPhysObj->AddVelocity(&vecThrow, &impulse);

	pProp->Use(pPlayer, pPlayer, USE_ON, 1);
	pProp->Touch(pPlayer);

	pHLPlayer->m_HL2Local.m_nFlares--;

	CSingleUserRecipientFilter user(pPlayer);
	UserMessageBegin(user, "FlareUpdate");
	MessageWriteLong(pHLPlayer->m_HL2Local.m_nFlares);
	MessageEnd();
}

void CWeaponFlare::ItemPostFrame()
{
	CBasePlayer *pOwner = ToBasePlayer(GetOwner());
	if (pOwner == NULL)
		return;

	if (pOwner->m_nButtons & IN_ATTACK)
	{
		PrimaryAttack();
	}
	else
	{
		WeaponIdle();
	}
}
#endif

static void ThrowFlare(const CCommand& cmd)
{
	CHL2_Player* pPlayer = (CHL2_Player*)UTIL_GetLocalPlayer();
	if (!pPlayer)
		return;
	if (pPlayer->m_HL2Local.m_nFlares == 0)
	{
		pPlayer->EmitSound("HL2Player.UseDeny");
		return;
	}
	pPlayer->m_HL2Local.m_nFlares--;

	Vector	vecEye = pPlayer->EyePosition();
	Vector	vForward, vRight;

	pPlayer->EyeVectors(&vForward, &vRight, NULL);
	Vector vecSrc = vecEye + vForward * 18.0f + vRight * 8.0f + Vector(0, 0, -8);

	vForward[2] += 0.1f;

	Vector vecThrow;
	pPlayer->GetVelocity(&vecThrow, NULL);
	vecThrow += vForward * 350 + Vector(0, 0, 50);
	AngularImpulse impulse(200, random->RandomInt(-600, 600), 0);

	pPlayer->EmitSound("WeaponFrag.Roll");

	CPhysicsProp* pProp = (CPhysicsProp*)CPhysicsProp::CreateNoSpawn("prop_physics", vecSrc, vec3_angle, pPlayer);
	pProp->SetModel("models/props_junk/flare.mdl");
	DispatchSpawn(pProp);
	IPhysicsObject* pPhysObj = pProp->VPhysicsGetObject();
	if (pPhysObj)
		pPhysObj->AddVelocity(&vecThrow, &impulse);

	pProp->Use(pPlayer, pPlayer, USE_ON, 1);
	pProp->Touch(pPlayer);

	CSingleUserRecipientFilter user(pPlayer);
	UserMessageBegin(user, "FlareUpdate");
	MessageWriteLong(pPlayer->m_HL2Local.m_nFlares);
	MessageEnd();
}

static ConCommand throwflare_plus("-throwflare", &ThrowFlare, "Throws a flare\n");