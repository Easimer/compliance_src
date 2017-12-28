#pragma once

#include <vgui_controls\Panel.h>

class INewGameWindow {
public:
	virtual void Create(vgui::VPANEL parent) = 0;
	virtual void Destroy(void) = 0;
};

extern INewGameWindow* g_pNewGameWindow;
