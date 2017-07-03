#include "cbase.h"
#include <vgui/VGUI.h>
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui/IPanel.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ListViewPanel.h>
#include "INewChapters.h"
#include "memdbgon.h"

class CNCPanel : public vgui::Frame {
public:
	DECLARE_CLASS_SIMPLE(CNCPanel, vgui::Frame);
	CNCPanel(vgui::VPANEL parent);

	void SetVisible2(bool bVisible);
protected:
	//virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);
private:
	vgui::ListViewPanel* m_pChapterList;
};

CNCPanel::CNCPanel(vgui::VPANEL parent) : BaseClass(NULL, "NCPanel")
{
	SetParent(parent);

	SetProportional(false);
	SetTitleBarVisible(true);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(true);
	SetSizeable(false);
	SetMoveable(false);

	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);

	SetVisible2(false);

	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));
	
	SetSize(360, 540);
	SetPos(100, 100);
	wchar_t *tempString = g_pVGuiLocalize->Find("#GameUI_StartNewGame");
	if (tempString)
		SetTitle(tempString, true);
	else
		SetTitle("New Game", true);

	//vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

	LoadControlSettings("resource/ui/ncpanel.res");

	DevMsg("NCPanel has been constructed\n");
}

void CNCPanel::OnCommand(const char* pcCommand)
{
	DevMsg("Command: %s\n", pcCommand);
	if (!Q_stricmp(pcCommand, "newgame"))
	{
		DevMsg("NEW GAME STUB\n");
	}
	else if (!Q_stricmp(pcCommand, "nonewgame"))
	{
		DevMsg("CLOSE\n");
		Close();
	}
}

void CNCPanel::SetVisible2(bool bVisible)
{
	SetVisible(bVisible);
}

class CNCPanelInterface : public INCPanel
{
private:
	CNCPanel *NCPanel;
public:
	CNCPanelInterface()
	{
		NCPanel = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		NCPanel = new CNCPanel(parent);
	}
	void Destroy()
	{
		if (NCPanel)
		{
			NCPanel->SetParent((vgui::Panel *)NULL);
			delete NCPanel;
		}
	}
	void SetVisible(bool bVisible)
	{
		NCPanel->SetVisible2(bVisible);
	}
};
static CNCPanelInterface g_NCPanel;
INCPanel* ncpanel = (INCPanel*)&g_NCPanel;

void CNCPanel_Open(const CCommand& cmd)
{
	g_NCPanel.SetVisible(true);
}

static ConCommand cmd_nc_open("nc_open", CNCPanel_Open);