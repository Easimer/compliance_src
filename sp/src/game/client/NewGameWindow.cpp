#include "cbase.h"

#include "GameUI/IGameUI.h"
#include "INewGameWindow.h"
#include "vgui_controls\Frame.h"
#include "vgui_controls/HTML.h"
#include "vgui_controls\ListPanel.h"
#include "vgui/ISurface.h"
#include "filesystem.h"
#include "cdll_client_int.h"
#include "cdll_int.h"
#include <vgui/ILocalize.h>
#include "ienginevgui.h"
//#include "json.hpp"
#include <vgui/IScheme.h>

//using namespace nlohmann;

static ConVar cl_newgamewindow_url("cl_newgamewindow_url", "resource/newgame/index.html");

class CNewGameWindowHTML : public vgui::HTML {
public:
	DECLARE_CLASS_SIMPLE(CNewGameWindowHTML, vgui::HTML);
	CNewGameWindowHTML(vgui::Panel* parent);
	~CNewGameWindowHTML();

	void Activate();

private:
	MESSAGE_FUNC_PARAMS(OnMenuCommand, "CustomURL", url);

private:
	IGameUI* m_pGameUI;
};

CNewGameWindowHTML::CNewGameWindowHTML(vgui::Panel* parent) : BaseClass(parent, "NewGameWindowHTML", true)
{
	//SetParent(parent);
	//SetParent(parent->ToHandle());
	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);
	SetProportional(true);
	int w, h;
	parent->GetSize(w, h);
	SetSize(w, h);

	SetVisible(true);
	SetPaintBackgroundEnabled(true);
	SetPaintBorderEnabled(false);

	CreateInterfaceFn gameUIFactory = Sys_GetFactory("gameui.dll");
	m_pGameUI = (IGameUI *)gameUIFactory(GAMEUI_INTERFACE_VERSION, NULL);

	Activate();
}

CNewGameWindowHTML::~CNewGameWindowHTML()
{
}

void CNewGameWindowHTML::Activate()
{
	char buffer[1024];
	OpenURL(filesystem->GetLocalPath(cl_newgamewindow_url.GetString(), buffer, 1024), NULL, true);
}

#define MM_CMD(command) if(!strcmp(szCmd, command))

void CNewGameWindowHTML::OnMenuCommand(KeyValues* pKv)
{
	char* empty = "\"\"";
	char szUrl[256];
	char* pszMsg = (char*)pKv->GetString("url");
	Q_strncpy(szUrl, pszMsg, 256);
	//free(pszMsg);
	char* szCmd = (char*)(Q_strnchr(szUrl, '/', 7) + 2);
	char* szParam = (char*)(Q_strnchr(szCmd, '/', 32) + 1);
	if (szParam == (char*)1)
		szParam = empty;
	else
		*(szParam - 1) = '\0';
	DevMsg("MenuCommand: URL: %s CMD: %s PARAM: %s\n", szUrl, szCmd, szParam);
	MM_CMD("load_map")
	{
		char buf[64];
		Q_snprintf(buf, 64, "engine map %s", szParam);
		m_pGameUI->SendMainMenuCommand(buf);
		return;
	}
	MM_CMD("close")
	{
		GetParent()->SetVisible(false);
		return;
	}
}

class CNewGameWindow : public vgui::Frame {
public:
	DECLARE_CLASS_SIMPLE(CNewGameWindow, vgui::Frame);
	CNewGameWindow(vgui::VPANEL parent);
	~CNewGameWindow();
private:
	CNewGameWindowHTML* m_pHTML;
};

CNewGameWindow::CNewGameWindow(vgui::VPANEL parent) : BaseClass(NULL, "NewGameWindow")
{
	SetParent(parent);

	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);

	SetProportional(true);
	SetTitleBarVisible(false);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(false);
	SetSizeable(false);
	SetMoveable(false);
	SetVisible(false);

	m_pHTML = new CNewGameWindowHTML(this);

	//SetTitle("New Game", true);
	LoadControlSettings("resource/ui/newgame.res");

	//SetSize(200, 150);
	//SetPos(100, 100);

	DevMsg("CNewGameWindow::constr\n");

}

CNewGameWindow::~CNewGameWindow()
{
	/*if (pGameUI)
		pGameUI->SetMainMenuOverride(iOriginalMenu);*/
}

class CINewGameWindow : public INewGameWindow {
public:
	void Create(vgui::VPANEL parent);
	void Destroy();
	void Show(bool show);
private:
	CNewGameWindow* pInstance;
};

void CINewGameWindow::Create(vgui::VPANEL parent)
{
	DevMsg("CINewGameWindow::Create\n");
	if (pInstance)
		Destroy();
	pInstance = new CNewGameWindow(parent);
}

void CINewGameWindow::Destroy()
{
	DevMsg("CINewGameWindow::Destroy\n");
	if (pInstance)
	{
		pInstance->SetParent((vgui::VPANEL)NULL);
		delete pInstance;
		pInstance = NULL;
	}
}

void CINewGameWindow::Show(bool show)
{
	pInstance->SetVisible(show);
}

static CINewGameWindow gINewGameWindow;
INewGameWindow* g_pNewGameWindow = (INewGameWindow*)&gINewGameWindow;

void reload_ngw_callback(const CCommand& cmd)
{
	//gINewGameWindow.Reload();
	gINewGameWindow.Destroy();
	gINewGameWindow.Create(enginevgui->GetPanel(PANEL_GAMEUIDLL));
}

static ConCommand reload_menu("reload_ngw", &reload_ngw_callback);

void ngw_toggle_callback(const CCommand& cmd)
{
	gINewGameWindow.Show(true);
}

static ConCommand ngw_toggle("ngw_toggle", &ngw_toggle_callback);
