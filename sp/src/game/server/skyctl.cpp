#include "cbase.h"
#include "baseentity.h"
#include "fmtstr.h"
#include "memdbgon.h"

static ConVar dss_mdr("dss_mdr", "10");

//#define DSS_DAY_LEN 10.0 // in-game minutes
#define DSS_DAY_LEN (dss_mdr.GetFloat())

class CDynamicSkyController : public CBaseEntity {
public:
	DECLARE_CLASS(CDynamicSkyController, CBaseEntity);
	DECLARE_DATADESC();
	virtual void Spawn();
	virtual void Think();
public:
	double m_flTimeOfDay;
	double m_flLastTimeOfDay;
};

void CDynamicSkyController::Spawn()
{
	m_flTimeOfDay = 0;
	SetThink(&CDynamicSkyController::Think);
	SetNextThink(gpGlobals->curtime + 1);
}

void CDynamicSkyController::Think()
{
	bool bOverflow = false;
	m_flLastTimeOfDay = m_flTimeOfDay;
	m_flTimeOfDay += 1.0 / (DSS_DAY_LEN * 60);
	if (m_flTimeOfDay >= 1)
	{
		m_flTimeOfDay -= 1;
		bOverflow = true;
	}
	SetThink(&CDynamicSkyController::Think);
	SetNextThink(gpGlobals->curtime + (DSS_DAY_LEN / 10));

	CSingleUserRecipientFilter user(UTIL_GetLocalPlayer());

	UserMessageBegin(user, "UpdateSky");
	MessageWriteFloat(m_flTimeOfDay);
	MessageEnd();

	int iPart = -1;

	if (bOverflow) // 12AM passed
		iPart = 0;
	else if (m_flTimeOfDay > 0.25 && m_flLastTimeOfDay < 0.25) // 6AM passed
		iPart = 1;
	else if (m_flTimeOfDay > 0.50 && m_flLastTimeOfDay < 0.50) // 12PM passed
		iPart = 2;
	else if (m_flTimeOfDay > 0.75 && m_flLastTimeOfDay < 0.75) // 6PM passed
		iPart = 3;

	DevMsg("Time is: %f\n", m_flTimeOfDay);

	if (iPart != -1)
	{
		DevMsg("AYY: %d\n", iPart);
		CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
		CFmtStr hint;
		hint.sprintf("#compliance_timeis_%d", iPart);
		UTIL_HudHintText(pPlayer, hint.Access());
		
		switch (iPart)
		{
		case 0:
			pPlayer->SetSuitUpdate("!HEV_TIME12AM", FALSE, 0);
			break;
		case 1:
			pPlayer->SetSuitUpdate("!HEV_TIME6AM", FALSE, 0);
			break;
		case 2:
			pPlayer->SetSuitUpdate("!HEV_TIME12PM", FALSE, 0);
			break;
		case 3:
			pPlayer->SetSuitUpdate("!HEV_TIME6PM", FALSE, 0);
			break;
		}

	}
	
}

BEGIN_DATADESC(CDynamicSkyController)
DEFINE_KEYFIELD(m_flTimeOfDay, FIELD_FLOAT, "timeofday"),
DEFINE_THINKFUNC(Think),
END_DATADESC()

LINK_ENTITY_TO_CLASS(env_dynamicsky_ctl, CDynamicSkyController);

void dss_timeofday_callback(const CCommand& cmd)
{
	if (cmd.ArgC() < 2)
		return;
	float val = Q_atof(cmd.ArgV()[1]);
	CBaseEntity* pEnt = gEntList.FindEntityByClassname(NULL, "env_dynamicsky_ctl");
	if (!pEnt)
		return;
	CDynamicSkyController* pSkyCtl = (CDynamicSkyController*)pEnt;
	while (val > 1)
		val -= 1;
	while (val < 0)
		val += 1;
	pSkyCtl->m_flTimeOfDay = val;
}

static ConCommand dss_timeofday("dss_timeofday", dss_timeofday_callback, "Sets the time of day. Range: [0; 1)");
