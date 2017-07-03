#include "cbase.h"
#include "vgui_controls/Frame.h"
#include "vgui_controls/HTML.h"
#include "vgui/ILocalize.h"
#include "filesystem.h"
#include "KeyValues.h"
#include "memdbgon.h"

class CScenarioWindow : public vgui::Frame {
public:
	DECLARE_CLASS_SIMPLE(CScenarioWindow, vgui::Frame);
	CScenarioWindow(vgui::VPANEL parent);
protected:
	virtual void OnCommand(const char* pcCommand);
};

CScenarioWindow::CScenarioWindow(vgui::VPANEL parent) : BaseClass(NULL, "ScenarioWindow")
{
	SetParent(parent);
	SetProportional(true);
	SetMouseInputEnabled(true);
	SetVisible(true);

	SetTitleBarVisible(true);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(true);
	SetSizeable(false);
	SetMoveable(false);

	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));
	SetSize(640, 480);
	SetPos(0, 0);
	wchar_t *tempString = g_pVGuiLocalize->Find("#Compliance_GUI_Scenarios");
	if (tempString)
		SetTitle(tempString, true);
	else
		SetTitle("Scenarios", true);

	LoadControlSettings("resource/ui/scenarios.res");

	KeyValues* kv = new KeyValues("scripts/scenarios.txt");
	kv->LoadFromFile(filesystem, "scripts/scenarios.txt", 0);

	for (KeyValues *sub = kv->GetFirstSubKey(); sub; sub = sub->GetNextKey())
	{
		DevMsg("%s\n", sub->GetName());
	}

	kv->Clear();
}

void CScenarioWindow::OnCommand(char const* cmd)
{

}