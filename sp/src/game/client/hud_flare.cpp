#include "cbase.h"
#include "hud.h"
#include "hud_macros.h"
#include "hudelement.h"
#include "hud_numericdisplay.h"
#include "c_basehlplayer.h"
#include "memdbgon.h"

class CHudFlare : public CHudElement, public CHudNumericDisplay {
	DECLARE_CLASS_SIMPLE(CHudFlare, CHudNumericDisplay);
public:
	CHudFlare(const char *pElementName);
	virtual void Init(void);
	virtual void VidInit(void);
	virtual void Reset(void);
	void MsgFunc_FlareUpdate(bf_read &msg);
	void Paint(void);
private:
	CHudTexture *m_iconFlare;
};

DECLARE_HUDELEMENT(CHudFlare);
DECLARE_HUD_MESSAGE(CHudFlare, FlareUpdate);

CHudFlare::CHudFlare(const char *pElementName) : CHudElement(pElementName), CHudNumericDisplay(NULL, "HudFlare")
{
	SetHiddenBits(HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT);
}

void CHudFlare::Init()
{
	HOOK_HUD_MESSAGE(CHudFlare, FlareUpdate);
	Reset();
}

void CHudFlare::Reset()
{
	SetDisplayValue(0);
	m_iconFlare = gHUD.GetIcon("icon_flare");
}

void CHudFlare::VidInit()
{
	Reset();
}

void CHudFlare::MsgFunc_FlareUpdate(bf_read &msg)
{
	SetDisplayValue(msg.ReadLong());
}

void CHudFlare::Paint()
{
	BaseClass::Paint();
	if (m_iconFlare)
	{
		m_iconFlare->DrawSelf(4, 4, GetFgColor());
	}
}