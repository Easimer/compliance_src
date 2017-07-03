#include "cbase.h"
#include "basecombatweapon_shared.h"
#include "basegrenade_shared.h"
#include "Sprite.h"
#include "soundent.h"
#include "memdbgon.h"

#define GRENADE_MODEL "models/weapons/w_grenade.mdl"
#define DECOY_VOLUME 2000

class CPropDecoy : public CBaseGrenade {
public:
	DECLARE_CLASS(CPropDecoy, CBaseGrenade);
	DECLARE_DATADESC();
	void Precache();
	void Spawn();
	void Think();

	void SetVelocity(const Vector& velocity, const AngularImpulse& angularVelocity);
private:
	int m_nBeep;
	CSprite* m_pMainGlow;
};

LINK_ENTITY_TO_CLASS(npc_grenade_decoy, CPropDecoy);

BEGIN_DATADESC(CPropDecoy)
DEFINE_THINKFUNC(Think),
END_DATADESC()

void CPropDecoy::Precache()
{
	BaseClass::Precache();
	PrecacheModel(GRENADE_MODEL);
	PrecacheScriptSound("Grenade.Blip");
	PrecacheModel("sprites/redglow1.vmt");
}

void CPropDecoy::Spawn()
{
	Precache();

	SetModel(GRENADE_MODEL);

	SetSize(-Vector(4, 4, 4), Vector(4, 4, 4));
	SetCollisionGroup(COLLISION_GROUP_WEAPON);
	VPhysicsInitNormal(SOLID_BBOX, 0, false);

	AddSolidFlags(FSOLID_NOT_STANDABLE);

	m_pMainGlow = CSprite::SpriteCreate("sprites/redglow1.vmt", GetLocalOrigin(), false);

	int	nAttachment = LookupAttachment("fuse");

	if (m_pMainGlow != NULL)
	{
		m_pMainGlow->FollowEntity(this);
		m_pMainGlow->SetAttachment(this, nAttachment);
		m_pMainGlow->SetTransparency(kRenderGlow, 255, 255, 255, 200, kRenderFxNoDissipation);
		m_pMainGlow->SetScale(0.2f);
		m_pMainGlow->SetGlowProxySize(4.0f);
	}

	SetThink(&CPropDecoy::Think);
	SetNextThink(gpGlobals->curtime + 3);

	BaseClass::Spawn();
}

void CPropDecoy::Think()
{
	EmitSound("Grenade.Blip");
	CSoundEnt::InsertSound(SOUND_COMBAT, GetAbsOrigin(), DECOY_VOLUME, 0.2, this);
	SetNextThink(gpGlobals->curtime + 2);
	m_nBeep++;
	if (m_nBeep > 10)
		UTIL_Remove(this);
}

void CPropDecoy::SetVelocity(const Vector &velocity, const AngularImpulse &angVelocity)
{
	IPhysicsObject *pPhysicsObject = VPhysicsGetObject();
	if (pPhysicsObject)
	{
		pPhysicsObject->AddVelocity(&velocity, &angVelocity);
	}
}

CBaseGrenade *Decoy_Create(const Vector &position, const QAngle &angles, const Vector &velocity, const AngularImpulse &angVelocity, CBaseEntity *pOwner)
{
	// Don't set the owner here, or the player can't interact with grenades he's thrown
	CPropDecoy *pDecoy = (CPropDecoy*)CBaseEntity::Create("npc_grenade_decoy", position, angles, pOwner);

	pDecoy->SetVelocity(velocity, angVelocity);
	pDecoy->SetThrower(ToBaseCombatCharacter(pOwner));
	pDecoy->m_takedamage = DAMAGE_EVENTS_ONLY;

	return pDecoy;
}

class CWeaponDecoy : public CBaseCombatWeapon {
public:
	DECLARE_CLASS(CWeaponDecoy, CBaseCombatWeapon);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	void Spawn(void);
	void Precache(void);
	void PrimaryAttack(void);
	int GetMinBurst(){ return 1; }
	int GetMaxBurst(){ return 3; }
	float GetFireRate(void) { return 1.0f; }
	const Vector& GetBulletSpread(void) { static Vector cone = VECTOR_CONE_1DEGREES; return cone; }
	DECLARE_ACTTABLE();
	DECLARE_DATADESC();
};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponDecoy, DT_WeaponDecoy)

BEGIN_NETWORK_TABLE(CWeaponDecoy, DT_WeaponDecoy)
END_NETWORK_TABLE()

PRECACHE_WEAPON_REGISTER(weapon_decoy);

BEGIN_DATADESC(CWeaponDecoy)
END_DATADESC()

BEGIN_PREDICTION_DATA(CWeaponDecoy)
END_PREDICTION_DATA()

#ifndef CLIENT_DLL
LINK_ENTITY_TO_CLASS(weapon_decoy, CWeaponDecoy);
#endif

acttable_t	CWeaponDecoy::m_acttable[] =
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

IMPLEMENT_ACTTABLE(CWeaponDecoy);

void CWeaponDecoy::Precache(void)
{
	UTIL_PrecacheOther("npc_grenade_decoy");
	BaseClass::Precache();
}

void CWeaponDecoy::Spawn(void)
{
	Precache();
	m_bIsSuppressed = true;
	m_iSuppressorDurability = 100;
	m_iSuppressorMaxDurability = 100;

	BaseClass::Spawn();
}

void CWeaponDecoy::PrimaryAttack()
{
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer)
		return;
	Vector	vecEye = pPlayer->EyePosition();
	Vector	vForward, vRight;

	pPlayer->EyeVectors(&vForward, &vRight, NULL);
	Vector vecSrc = vecEye + vForward * 18.0f + vRight * 8.0f;
	//	vForward[0] += 0.1f;
	vForward[2] += 0.1f;

	Vector vecThrow;
	pPlayer->GetVelocity(&vecThrow, NULL);
	vecThrow += vForward * 1200;
	Decoy_Create(vecSrc, vec3_angle, vecThrow, AngularImpulse(600, random->RandomInt(-1200, 1200), 0), pPlayer);

	m_flNextPrimaryAttack = gpGlobals->curtime + 1;
	BaseClass::PrimaryAttack();
}