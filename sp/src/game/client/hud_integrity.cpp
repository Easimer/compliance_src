#include "cbase.h"
#include "hud.h"
#include "hud_integrity.h"
#include "hud_macros.h" 
#include "iclientmode.h"
#include "vgui/ISurface.h"
#include "vgui/ILocalize.h"
#include "c_baseplayer.h"
#include "tier0/memdbgon.h" 

DECLARE_HUDELEMENT(CHudIntegrity);
CHudIntegrity::CHudIntegrity(const char* pElementName) : CHudElement(pElementName), BaseClass(NULL, "HudIntegrity")
{
	vgui::Panel * pParent = g_pClientMode->GetViewport();
	SetParent(pParent);
	SetHiddenBits(HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT);
}

void CHudIntegrity::Init()
{
	Reset();
}

void CHudIntegrity::VidInit()
{
	Reset();
}

void CHudIntegrity::Reset()
{
	SetBgColor(Color(0, 0, 0, 128));
	m_fIntegrity = 100;
	SetShouldDisplayValue(true);

	wchar_t *tempString = g_pVGuiLocalize->Find("#Compliance_Hud_Integrity");

	if (tempString)
	{
		SetLabelText(tempString);
	}
	else
	{
		SetLabelText(L"INTEGRITY");
	}

	SetDisplayValue(m_fIntegrity);
}

void CHudIntegrity::OnThink()
{
	auto pPlayer = C_BasePlayer::GetLocalPlayer();
	if (!pPlayer)
		return;
	m_fIntegrity = max(pPlayer->GetIntegrity(), 0);
	SetDisplayValue(m_fIntegrity);
}

void CHudIntegrity::Paint()
{
	BaseClass::Paint();	
}
