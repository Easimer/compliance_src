#pragma once

#include <vgui_controls/Panel.h>

class IScenarioPanel
{
public:
	virtual void		Create(vgui::VPANEL parent) = 0;
	virtual void		Destroy(void) = 0;
};

extern IScenarioPanel* ncpanel;