#include "cbase.h"
#include "hud.h"
#include "hud_integrity.h"
#include "hud_macros.h" 
#include "iclientmode.h"
#include "vgui/ISurface.h"
#include "vgui/ILocalize.h"
#include "c_baseplayer.h"
#include "tier0/memdbgon.h" 

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_numericdisplay.h"
#include "vgui_helpers.h"
#include "vgui\IPanel.h"
#include "hl2\hl2_player_shared.h"

class CHudSuppressor : public CHudElement, public CHudNumericDisplay {
	DECLARE_CLASS_SIMPLE(CHudSuppressor, CHudNumericDisplay);
public:
	CHudSuppressor(const char* pElementName);
	virtual void Init(void);
	virtual void VidInit(void);
	virtual void Reset(void);
	//virtual void OnThink(void);

	//void UpdateValues();

	void MsgFunc_UpdateWeapon(bf_read& msg);
protected:
	//virtual void Paint();
private:
	CPanelAnimationVar(Color, m_HullColor, "HullColor", "255 0 0 255");
	CPanelAnimationVar(Color, m_TextColor, "Normal", "255 208 64 255");
	CPanelAnimationVar(Color, m_ChunkColor, "ChunkColor", "255 220 0 220");
	int m_iDurability;
	int m_iMaxDurability;
	int m_nBarHeight;
	float m_fDurabilityPercent;
};

DECLARE_HUDELEMENT(CHudSuppressor);
DECLARE_HUD_MESSAGE(CHudSuppressor, UpdateWeapon);

CHudSuppressor::CHudSuppressor(const char* pElementName) : CHudElement(pElementName), BaseClass(NULL, "HudSuppressor")
{
	vgui::Panel * pParent = g_pClientMode->GetViewport();
	SetParent(pParent);
	SetHiddenBits(HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT);
}

void CHudSuppressor::Init()
{
	HOOK_HUD_MESSAGE(CHudSuppressor, UpdateWeapon);
	Reset();
}

void CHudSuppressor::VidInit()
{
	Reset();
}

void CHudSuppressor::Reset()
{
	BaseClass::Reset();
	SetBgColor(Color(0, 0, 0, 128));
	m_iDurability = 0;
	m_iMaxDurability = 32;
	m_nBarHeight = GetTall() / 3;
	SetPaintEnabled(false);
	SetPaintBackgroundEnabled(false);
	SetShouldDisplayValue(false);

	wchar_t *tempString = g_pVGuiLocalize->Find("#Compliance_Hud_Suppressor");

	if (tempString)
	{
		SetLabelText(tempString);
	}
	else
	{
		SetLabelText(L"SUPPRESSOR");
	}

	SetDisplayValue(0);
}

void CHudSuppressor::MsgFunc_UpdateWeapon(bf_read& msg)
{
	int nPercent = msg.ReadLong();
	if (nPercent == -1)
	{
		SetPaintEnabled(false);
		SetPaintBackgroundEnabled(false);
		SetShouldDisplayValue(false);
		SetDisplayValue(0);
	}
	else
	{
		SetPaintEnabled(true);
		SetPaintBackgroundEnabled(true);
		SetShouldDisplayValue(true);
		SetDisplayValue(nPercent);
	}
}