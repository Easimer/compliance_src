#include "cbase.h"
#include "particle_parse.h"

class CNPCParticleStorm : public CBaseEntity {
public:
	DECLARE_CLASS(CNPCParticleStorm, CBaseEntity);
	void Spawn();
	void Precache();
	void Think();
	void InputSetTarget(inputdata_t& input);
	void InputClearTarget(inputdata_t& input);
	void InputVanish(inputdata_t& input);
	void InputSilence(inputdata_t& input);
private:
	void SetTarget(const char* szName);
	void SetTarget(string_t szName);
private:
	string_t m_iszTarget;
	EHANDLE m_hCurrentTarget;
	int m_nSoundCounter;
	bool m_bHasParticleSystem;
	bool m_bSilent;
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(npc_particlestorm, CNPCParticleStorm);

BEGIN_DATADESC(CNPCParticleStorm)
DEFINE_KEYFIELD(m_iszTarget, FIELD_STRING, "target"),
DEFINE_THINKFUNC(Think),
DEFINE_INPUTFUNC(FIELD_STRING, "SetTarget", InputSetTarget),
DEFINE_INPUTFUNC(FIELD_VOID, "ClearTarget", InputClearTarget),
DEFINE_INPUTFUNC(FIELD_VOID, "Vanish", InputVanish),
DEFINE_INPUTFUNC(FIELD_VOID, "Silence", InputSilence),
END_DATADESC()

void CNPCParticleStorm::Spawn()
{
	Precache();
	SetThink(&CNPCParticleStorm::Think);
	SetNextThink(gpGlobals->curtime + 0.2);

	m_bHasParticleSystem = false;
	m_bSilent = false;

	if (strlen(m_iszTarget.ToCStr()) != 0)
	{
		SetTarget(m_iszTarget.ToCStr());
	}

	m_nSoundCounter = 0;

	BaseClass::Spawn();
}

void CNPCParticleStorm::Precache()
{
	PrecacheParticleSystem("particle_storm_1");
	PrecacheParticleSystem("particle_storm_2");
	PrecacheParticleSystem("particle_storm_electro");
	PrecacheParticleSystem("particle_storm_composite");
	PrecacheScriptSound("NPC_ParticleStorm.Hit");
	PrecacheScriptSound("NPC_ParticleStorm.Grind");
	BaseClass::Precache();
}

void CNPCParticleStorm::SetTarget(const char* szName)
{
	m_iszTarget = MAKE_STRING(szName);
	m_hCurrentTarget = gEntList.FindEntityGeneric(NULL, szName, this);
	if (m_hCurrentTarget.Get())
		DevMsg("[ParticleStorm] set target to %s\n", szName);
}

void CNPCParticleStorm::SetTarget(string_t iszName)
{
	SetTarget(STRING(iszName));
}

void CNPCParticleStorm::InputSetTarget(inputdata_t& input)
{
	const char* szTarget = input.value.String();
	SetTarget(szTarget);
}

void CNPCParticleStorm::InputClearTarget(inputdata_t& input)
{
	m_hCurrentTarget = NULL;
}

void CNPCParticleStorm::InputVanish(inputdata_t& input)
{
	DevMsg("[ParticleStorm] Vanish\n");
	StopParticleEffects(this);
	SetNextThink(TICK_NEVER_THINK);
	StopSound("NPC_ParticleStorm.Hit");
	StopSound("NPC_ParticleStorm.Grind");
	UTIL_Remove(this);
}

void CNPCParticleStorm::InputSilence(inputdata_t& input)
{
	m_bSilent = true;
}

void CNPCParticleStorm::Think()
{
	Vector vPos, vTPos, vDir;
	CBaseEntity* pEnt = m_hCurrentTarget.Get();
	if (pEnt)
	{
		vPos = GetAbsOrigin();
		vTPos = pEnt->GetAbsOrigin() + Vector(0, 0, 32);
		vDir = (vTPos - vPos).Normalized();
		SetAbsOrigin(vPos + vDir * 10);
	}
	else
	{
		if (strlen(STRING(m_iszTarget)) > 0)
		{
			SetTarget(m_iszTarget);
		}
		// Follow player if no target is set
		// To avoid this behavior, have a fake target set
		CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
		if (pPlayer)
		{
			if (pPlayer->IsDead() || pPlayer->GetHealth() <= 0)
			{
				DevMsg("[ParticleStorm] Player dead, vanishing\n");
				inputdata_t fake;
				InputVanish(fake);
				return;
			}
			vPos = GetAbsOrigin();
			vTPos = pPlayer->GetAbsOrigin() + Vector(0, 0, 64);
			vDir = (vTPos - vPos).Normalized();
			SetAbsOrigin(vPos + vDir * 10);
		}
	}

	// TODO: broken, you should parent an info_particle_system to the NPC
	if (!m_bHasParticleSystem)
	{
		m_bHasParticleSystem = true;
		DispatchParticleEffect("particle_storm_composite", PATTACH_ABSORIGIN_FOLLOW, this);
	}

	// Damage at most 64 fellow entities in our vicinity

	CBaseEntity* apTargets[64];
	size_t nEnts = UTIL_EntitiesInSphere(apTargets, 128, GetAbsOrigin(), 16, 0);
	bool bSFXPlayed = false;

	for (size_t i = 0; i < nEnts; i++)
	{
		if (apTargets[i])
		{
			if (apTargets[i] == this)
				continue;
			auto pChar = ToBaseCombatCharacter(apTargets[i]);
			if (pChar)
			{
				CTakeDamageInfo dmg;
				dmg.AddDamage(12);
				dmg.AddDamageType(DMG_SLASH);
				pChar->TakeDamage(dmg);
				if (!bSFXPlayed)
				{
					if (!m_bSilent)
						EmitSound("NPC_ParticleStorm.Hit");
					bSFXPlayed = true;
				}
				
			}
		}
	}

	if (m_nSoundCounter == 5)
	{
		if (!m_bSilent)
			EmitSound("NPC_ParticleStorm.Grind");
		m_nSoundCounter = 0;
	}
	m_nSoundCounter++;

	SetNextThink(gpGlobals->curtime + 0.2);
}