#include "cbase.h"

#include "GameUI/IGameUI.h"
#include "IMainMenu.h"
#include "vgui_controls\Frame.h"
#include "vgui_controls\ListPanel.h"

class CMainMenu : public vgui::Frame{
public:
	DECLARE_CLASS_SIMPLE(CMainMenu, vgui::Frame);
	CMainMenu(vgui::VPANEL parent);
	~CMainMenu();

protected:
	virtual void OnCommand(const char* pcCommand);

	void GetGameUIPtr();
private:
	IGameUI* pGameUI;
	vgui::VPANEL iOriginalMenu;

	vgui::ListPanel* pMenuItems;
};

CMainMenu::CMainMenu(vgui::VPANEL parent) : BaseClass(NULL, "MainMenu")
{
	SetParent(parent);
	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);
	
	SetProportional(true);
	SetVisible(true);
	SetMoveable(false);
	SetSizeable(false);
	SetTitleBarVisible(false);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(false);
	SetPaintBackgroundEnabled(true);
	SetPaintBorderEnabled(false);	

	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));
	LoadControlSettings("resource/UI/mainmenu.res");

	GetGameUIPtr();

	Assert(pGameUI != NULL);

	iOriginalMenu = parent;

	pGameUI->SetMainMenuOverride(GetVPanel());
}

CMainMenu::~CMainMenu()
{
	/*if (pGameUI)
		pGameUI->SetMainMenuOverride(iOriginalMenu);*/
	pGameUI->SetMainMenuOverride((vgui::VPANEL)NULL);
}

#define MM_CMD(command) if(!strcmp(cmd, command))

void CMainMenu::OnCommand(char const* cmd)
{
	MM_CMD("OpenCampaignDialog")
		pGameUI->SendMainMenuCommand("OpenNewGameDialog");
	MM_CMD("QuitGame")
		pGameUI->SendMainMenuCommand("Quit");
	MM_CMD("ContinueGame")
		pGameUI->SendMainMenuCommand("engine load autosave");
	MM_CMD("OpenScenarioDialog")
	{}
	MM_CMD("OpenOptions")
		pGameUI->SendMainMenuCommand("OpenOptionsDialog");

}

void CMainMenu::GetGameUIPtr()
{
	CreateInterfaceFn gameUIFactory = Sys_GetFactory("gameui.dll");
	pGameUI = (IGameUI *)gameUIFactory(GAMEUI_INTERFACE_VERSION, NULL);
}

class CIMainMenu : public IMainMenu {
public:
	void Create(vgui::VPANEL parent);
	void Destroy();
	void Activate();
private:
	CMainMenu* pInstance;
};

void CIMainMenu::Create(vgui::VPANEL parent)
{
	pInstance = new CMainMenu(parent);
}

void CIMainMenu::Destroy()
{
	if (pInstance)
	{
		pInstance->SetParent((vgui::VPANEL)NULL);
		delete pInstance;
	}
}

void CIMainMenu::Activate()
{
	if (pInstance)
		pInstance->Activate();
}

static CIMainMenu gIMainMenu;
IMainMenu* g_pMainMenu = (IMainMenu*)&gIMainMenu;