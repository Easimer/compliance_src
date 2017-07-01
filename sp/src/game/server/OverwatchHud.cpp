#include "cbase.h"
#include "tier0/memdbgon.h"

class COverwatchHud : public CPointEntity
{
public:
	DECLARE_CLASS(COverwatchHud, CPointEntity);

	void	Spawn(void);
	//void	Precache(void);

private:

	void InputShowHudHint(inputdata_t &inputdata);
	void InputHideHudHint(inputdata_t &inputdata);
	string_t m_iszMessage;
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(env_owhint, COverwatchHud);

BEGIN_DATADESC(COverwatchHud)

DEFINE_KEYFIELD(m_iszMessage, FIELD_STRING, "message"),
DEFINE_INPUTFUNC(FIELD_VOID, "ShowHudHint", InputShowHudHint),
DEFINE_INPUTFUNC(FIELD_VOID, "HideHudHint", InputHideHudHint),

END_DATADESC()

void COverwatchHud::Spawn(void)
{
	SetSolid(SOLID_NONE);
	SetMoveType(MOVETYPE_NONE);
}

void COverwatchHud::InputShowHudHint(inputdata_t &inputdata)
{
	CBaseEntity *pPlayer = NULL;
	if (inputdata.pActivator && inputdata.pActivator->IsPlayer())
	{
		pPlayer = inputdata.pActivator;
	}
	else
	{
		pPlayer = UTIL_GetLocalPlayer();
	}

	if (!pPlayer || !pPlayer->IsNetClient())
		return;

	CSingleUserRecipientFilter user((CBasePlayer *)pPlayer);
	user.MakeReliable();
	UserMessageBegin(user, "KeyHintText");
	WRITE_BYTE(1);	// one message
	WRITE_STRING(STRING(m_iszMessage));
	MessageEnd();
}

void COverwatchHud::InputHideHudHint(inputdata_t &inputdata)
{
	CBaseEntity *pPlayer = NULL;

	if (inputdata.pActivator && inputdata.pActivator->IsPlayer())
	{
		pPlayer = inputdata.pActivator;
	}
	else
	{
		pPlayer = UTIL_GetLocalPlayer();
	}

	if (!pPlayer || !pPlayer->IsNetClient())
		return;

	CSingleUserRecipientFilter user((CBasePlayer *)pPlayer);
	user.MakeReliable();
	UserMessageBegin(user, "KeyHintText");
	WRITE_BYTE(1);	// one message
	WRITE_STRING(STRING(NULL_STRING));
	MessageEnd();
}
