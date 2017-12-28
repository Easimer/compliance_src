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
	void InputSetTime(inputdata_t& input);
	void InputTriggerSuit(inputdata_t& input);
	void InputPause(inputdata_t& input);
	void InputResume(inputdata_t& input);
private:
	void TriggerSuit();

public:
	float m_flTimeOfDay;
	float m_flLastTimeOfDay;
	int m_iPaused;
};

void CDynamicSkyController::Spawn()
{
	SetThink(&CDynamicSkyController::Think);
	SetNextThink(gpGlobals->curtime + 1);
}

void CDynamicSkyController::Think()
{
	if (m_iPaused)
	{
		SetNextThink(gpGlobals->curtime + 1);
		return;
	}
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

	if (
		(m_flLastTimeOfDay <= 1 && m_flTimeOfDay >= 0) ||
		(m_flTimeOfDay > 0.25 && m_flLastTimeOfDay < 0.25) ||
		(m_flTimeOfDay > 0.50 && m_flLastTimeOfDay < 0.50) ||
		(m_flTimeOfDay > 0.75 && m_flLastTimeOfDay < 0.75)
		)
		TriggerSuit();
}

void CDynamicSkyController::InputSetTime(inputdata_t & input)
{
	float val = input.value.Float();
	if (val < 0)
		val = 0;
	else if (val > 1.0)
		val = 1;
	m_flLastTimeOfDay = input.value.Float();
	m_flTimeOfDay = input.value.Float();
}

void CDynamicSkyController::InputTriggerSuit(inputdata_t & input)
{
	TriggerSuit();
}

void CDynamicSkyController::InputPause(inputdata_t & input)
{
	m_iPaused = 1;
}

void CDynamicSkyController::InputResume(inputdata_t & input)
{
	m_iPaused = 0;
	Think();
}

void CDynamicSkyController::TriggerSuit()
{
	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();

//	int iPart = -1;
	if (m_flTimeOfDay >= 0 && m_flTimeOfDay < 0.25)
	{
		pPlayer->SetSuitUpdate("!HEV_TIME12AM", FALSE, 0);
		UTIL_HudHintText(pPlayer, "#compliance_timeis_0");
	}
	else if (m_flTimeOfDay >= 0.25 && m_flTimeOfDay < 0.50)
	{
		pPlayer->SetSuitUpdate("!HEV_TIME6AM", FALSE, 0);
		UTIL_HudHintText(pPlayer, "#compliance_timeis_1");
	}
	else if (m_flTimeOfDay >= 0.50 && m_flTimeOfDay < 0.75)
	{
		pPlayer->SetSuitUpdate("!HEV_TIME12PM", FALSE, 0);
		UTIL_HudHintText(pPlayer, "#compliance_timeis_2");
	}
	else if (m_flTimeOfDay >= 0.75 && m_flTimeOfDay < 1.00)
	{
		pPlayer->SetSuitUpdate("!HEV_TIME6PM", FALSE, 0);
		UTIL_HudHintText(pPlayer, "#compliance_timeis_3");
	}

	DevMsg("Time is: %f\n", m_flTimeOfDay);

	/*if (iPart != -1)
	{
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

	}*/
}

BEGIN_DATADESC(CDynamicSkyController)
DEFINE_KEYFIELD(m_flTimeOfDay, FIELD_FLOAT, "timeofday"),
DEFINE_KEYFIELD(m_iPaused, FIELD_INTEGER, "paused"),
DEFINE_THINKFUNC(Think),
DEFINE_INPUTFUNC(FIELD_FLOAT, "SetTime", InputSetTime),
DEFINE_INPUTFUNC(FIELD_VOID, "TriggerSuit", InputTriggerSuit),
DEFINE_INPUTFUNC(FIELD_VOID, "Resume", InputResume),
DEFINE_INPUTFUNC(FIELD_VOID, "Pause", InputPause),
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
