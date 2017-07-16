#pragma once

#include <vgui_controls\Panel.h>

class IMainMenu {
public:
	virtual void Create(vgui::VPANEL parent) = 0;
	virtual void Destroy(void) = 0;
	virtual void Activate(void) = 0;
	virtual void UpdateInGameStatus(bool bValue) = 0;
};

extern IMainMenu* g_pMainMenu;