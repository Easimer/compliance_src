#pragma once

#include <vgui_controls\Panel.h>

class IGunsmith {
public:
	virtual void Create(vgui::VPANEL parent) = 0;
	virtual void Destroy(void) = 0;
	virtual void Activate(void) = 0;
};

extern IGunsmith* g_pGunsmith;