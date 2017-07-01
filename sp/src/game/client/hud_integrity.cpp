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

void CHudIntegrity::Reset()
{
	SetBgColor(Color(0, 0, 0, 128));
	m_fIntegrity = 100;

	szLabel = L"INTEGRITY";
	szLabelSiz = wcslen(szLabel);
	szDigits[0] = '\0';
}

void CHudIntegrity::OnThink()
{
	auto pPlayer = C_BasePlayer::GetLocalPlayer();
	if (!pPlayer)
		return;
	m_fIntegrity = max(pPlayer->GetIntegrity(), 0);

	char szDigitsBuf[8];
	memset(szDigitsBuf, 0, 8);

	itoa((int)m_fIntegrity, szDigitsBuf, 10);
	for (int i = 0; i < 7; i++)
	{
		szDigits[i] = szDigitsBuf[i];
	}
}

void CHudIntegrity::Paint()
{
	//surface()->DrawSetColor(m_HullColor);
	vgui::surface()->DrawSetColor(m_HullColor);

	vgui::surface()->DrawSetTextFont(m_hTextFont);
	
	vgui::surface()->DrawSetTextColor(m_TextColor);
	vgui::surface()->DrawSetTextPos(text_xpos, text_ypos);
	vgui::surface()->DrawPrintText(szLabel, szLabelSiz);

	vgui::surface()->DrawSetTextFont(m_hTextFont2);
	vgui::surface()->DrawSetTextPos(digit_xpos, digit_ypos);
	vgui::surface()->DrawPrintText(szDigits, wcslen(szDigits));
	
}
