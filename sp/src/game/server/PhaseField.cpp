#include "cbase.h"
#include "PhaseField.h"

LINK_ENTITY_TO_CLASS(trigger_phase_field, CPhaseField);

BEGIN_DATADESC(CPhaseField)
DEFINE_KEYFIELD(m_sAnchor, FIELD_STRING, "anchor"),
DEFINE_KEYFIELD(m_bUnstable, FIELD_BOOLEAN, "unstable"),
DEFINE_FUNCTION(StartTouch),
DEFINE_FUNCTION(EndTouch),

DEFINE_INPUTFUNC(FIELD_VOID, "unstabilize", InputUnstabilize),
DEFINE_INPUTFUNC(FIELD_VOID, "stabilize", InputStabilize),

END_DATADESC()

void CPhaseField::Spawn()
{
	BaseClass::Spawn();
	BaseClass::InitTrigger();
	m_pAnchor = (CPhaseAnchor*)gEntList.FindEntityGeneric(NULL, STRING(m_sAnchor), this);
	if (m_pAnchor == NULL)
	{
		Warning("Anchor of %s was not found\n", GetEntityName().ToCStr());
	}
	SetTouch(&CPhaseField::StartTouch);
	SetSolid(SOLID_VPHYSICS);
	SetModel(STRING(GetModelName()));
	SetMoveType(MOVETYPE_NONE);
	VPhysicsInitShadow(false, false);
	m_fLastPhase = 0;
}

void CPhaseField::InputUnstabilize(inputdata_t& input)
{
	m_bUnstable = true;
}

void CPhaseField::InputStabilize(inputdata_t& input)
{
	m_bUnstable = false;
}

void CPhaseField::StartTouch(CBaseEntity* other)
{
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer || pPlayer != other)
		return;
	pPlayer->SetPhaseField(this);
	BaseClass::StartTouch(other);
}

void CPhaseField::EndTouch(CBaseEntity* other)
{
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer || pPlayer != other)
		return;
	pPlayer->SetPhaseField(NULL);
	BaseClass::EndTouch(other);
}

float CPhaseField::GetLastPhaseTime()
{
	return m_fLastPhase;
}

void CPhaseField::Phased()
{
	m_fLastPhase = gpGlobals->curtime;
}

CPhaseAnchor* CPhaseField::GetAnchor()
{
	return m_pAnchor;
}

bool CPhaseField::IsUnstable()
{
	return m_bUnstable;
}

void IN_PhaseDown(const CCommand& args)
{
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer)
		return;
	//DevMsg("Player OK\n");
	CPhaseField* pEnt = (CPhaseField*)pPlayer->GetPhaseField();
	if (!pEnt)
		return;
	//DevMsg("Field OK\n");

	if (gpGlobals->curtime - pEnt->GetLastPhaseTime() < 3)
		return;

	CPhaseAnchor* pAnchor = pEnt->GetAnchor();
	if (!pAnchor)
		return;
	//DevMsg("Anchor OK\n");
	CPhaseAnchor* pPair = pAnchor->GetPair();
	if (!pPair)
		return;
	//DevMsg("Pair OK\n");
	Vector vecPair = pPair->GetAbsOrigin();
	Vector vecAnchor = pAnchor->GetAbsOrigin();
	Vector vecDiff = vecPair - vecAnchor;
	Vector vecPlayer = pPlayer->GetAbsOrigin();
	pPlayer->SetAbsOrigin(vecPlayer + vecDiff);

	if (pEnt->IsUnstable())
	{
		CBaseEntity* around[32];
		int n = UTIL_EntitiesInSphere(around, 32, vecPlayer, 128, ~0);
		while (n--)
		{
			around[n]->SetAbsOrigin(around[n]->GetAbsOrigin() + vecDiff);
		}
	}
	
	/*CTakeDamageInfo dmginfo;
	dmginfo.AddDamageType(DMG_RADIATION);
	dmginfo.AddDamage(RandomInt(3, 7));
	pPlayer->TakeDamage(dmginfo);
	pPlayer->ViewPunch(QAngle(30, 20, 20));*/
	pPlayer->SetIntegrity(pPlayer->GetIntegrity() - 2);

	pPair->FirePhaseEvent(pPlayer);
}

void IN_PhaseUp(const CCommand& args)
{
	// nothing lol
}

static ConCommand bind_phase_up("+phase", IN_PhaseDown);
static ConCommand bind_phase_down("-phase", IN_PhaseUp);
