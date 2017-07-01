#pragma once
#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "vgui_helpers.h"
#include "vgui\IPanel.h"

class CHudIntegrity : public CHudElement, public vgui::Panel {
	DECLARE_CLASS_SIMPLE(CHudIntegrity, vgui::Panel);
public:
	CHudIntegrity(const char* pElementName);
	virtual void Init(void);
	virtual void Reset(void);
	virtual void OnThink(void);
protected:
	virtual void Paint();
private:
	CPanelAnimationVar(Color, m_HullColor, "HullColor", "255 0 0 255");
	CPanelAnimationVar(Color, m_TextColor, "Normal", "255 208 64 255");
	CPanelAnimationVar(vgui::HFont, m_hTextFont, "TextFont", "HUDBarText");
	CPanelAnimationVar(vgui::HFont, m_hTextFont2, "TextFont", "HUDNumbers");
	CPanelAnimationVarAliasType(float, text_xpos, "text_xpos", "2", "proportional_float");
	CPanelAnimationVarAliasType(float, text_ypos, "text_ypos", "2", "proportional_float");
	CPanelAnimationVarAliasType(float, digit_xpos, "digit_xpos", "2", "proportional_float");
	CPanelAnimationVarAliasType(float, digit_ypos, "digit_ypos", "2", "proportional_float");
	float m_fIntegrity;
	wchar_t* szLabel;
	size_t szLabelSiz;

	wchar_t szDigits[8];
};
