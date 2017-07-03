#pragma once

#include <vgui_controls/Panel.h>

class INCPanel
{
public:
	virtual void		Create(vgui::VPANEL parent) = 0;
	virtual void		Destroy(void) = 0;
};

extern INCPanel* ncpanel;