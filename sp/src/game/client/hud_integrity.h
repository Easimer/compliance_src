#pragma once
#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "vgui_helpers.h"
#include "vgui\IPanel.h"
#include "hud_numericdisplay.h"

class CHudIntegrity : public CHudNumericDisplay, public CHudElement {
	DECLARE_CLASS_SIMPLE(CHudIntegrity, CHudNumericDisplay);
public:
	CHudIntegrity(const char* pElementName);
	virtual void Init(void);
	virtual void VidInit(void);
	virtual void Reset(void);
	virtual void OnThink(void);
	virtual void Paint(void);
private:
	CPanelAnimationVar(Color, m_HullColor, "HullColor", "255 0 0 255");
	CPanelAnimationVar(Color, m_TextColor, "Normal", "255 208 64 255");
	CPanelAnimationVar(Color, m_ChunkColor, "ChunkColor", "255 220 0 220");
	float m_fIntegrity;
};
