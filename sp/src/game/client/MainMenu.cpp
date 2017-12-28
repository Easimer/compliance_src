#include "cbase.h"

#include "GameUI/IGameUI.h"
#include "IMainMenu.h"
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

static ConVar cl_mainmenu_url("cl_mainmenu_url", "resource/menu2/index.html");

class CMainMenu : public vgui::HTML, public IGameEventListener2 {
public:
	DECLARE_CLASS_SIMPLE(CMainMenu, vgui::HTML);
	CMainMenu(vgui::VPANEL parent);
	~CMainMenu();

	void Activate();

	void ProcessChapters();

	//void OnThink();

	void FireGameEvent(IGameEvent* pEvent);
	void UpdateInGameStatus(bool bValue);

protected:
	void GetGameUIPtr();

	void AddChapter(const char* szTitle, const char* szMapname);
private:
	IGameUI* pGameUI;

	float m_flLoadTime;
	bool m_bLoadSuccess;

	bool m_bIsInGame;
	bool m_bWasInGame;

	MESSAGE_FUNC_PARAMS(OnMenuCommand, "CustomURL", url);
};

CMainMenu::CMainMenu(vgui::VPANEL parent) : BaseClass(NULL, "MainMenu", true)
{
	SetParent(parent);
	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);
	SetProportional(true);
	//SetSize(vgui::scheme()->GetProportionalScaledValue(640), vgui::scheme()->GetProportionalScaledValue(480));
	int w, h;
	engine->GetScreenSize(w, h);
	SetSize(w, h);
	
	SetPos(0, 0);
	SetVisible(true);
	SetPaintBackgroundEnabled(true);
	SetPaintBorderEnabled(false);

	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

	AddCustomURLHandler("menu://", this);

	m_bLoadSuccess = false;
	m_flLoadTime = gpGlobals->curtime + 2.0f;

	m_bIsInGame = engine->IsInGame();
	m_bWasInGame = m_bIsInGame;

	GetGameUIPtr();
	Assert(pGameUI != NULL);

	pGameUI->SetMainMenuOverride(GetVPanel());
	pGameUI->SetMainMenuOverride(GetVPanel());
	pGameUI->SetMainMenuOverride(GetVPanel());
	DevMsg("CMainMenu::constr\n");
	
	gameeventmanager->AddListener(this, "gameui_activated", false);
	MoveToFront();

#ifdef DEBUG
	// check if we are already in-game
	// without this, when we reload the menu in-game, 'mainmenu' is shown instead of 'gamemenu'
	// DEBUG'd because the end-user probably won't reload the HTML runtime
	if (engine->IsInGame() && !engine->IsLevelMainMenuBackground())
		RunJavascript("menu_ingame(true);");
#endif
	Activate();
}

void CMainMenu::FireGameEvent(IGameEvent* pEvent)
{
	DevLog("CMainMenu::FireGameEvent\n");
	//Activate();
}

void CMainMenu::UpdateInGameStatus(bool bValue)
{
	if (bValue && !m_bWasInGame)
	{
		m_bWasInGame = true;
		RunJavascript("menu_ingame(true);");
	}
	else if (!bValue && m_bWasInGame)
	{
		m_bWasInGame = false;
		RunJavascript("menu_ingame(false);");
	}
	MoveToFront();
}

//extern int		g_nCurrentChapterIndex;

void CMainMenu::Activate()
{
	DevMsg("CMainMenu::Activate\n");
	char buffer[1024];
	for (int i = 0; i < 8; i++)
	{
		OpenURL(filesystem->GetLocalPath(cl_mainmenu_url.GetString(), buffer, 1024), NULL, true);
		ThreadSleep(100);
	}

	ProcessChapters();
	
	/*DevMsg("CMainMenu::Current chapter is %d\n", g_nCurrentChapterIndex);

	const char* defaultVid = "test";
	char vidnamebuf[256];

	strncpy(vidnamebuf, defaultVid, 256);

	if (filesystem->FileExists("scripts/cmm_chapterbg.json"))
	{
		FileHandle_t pFile = filesystem->Open("scripts/cmm_chapterbg.json", "r");
		CUtlBuffer fbuf;
		filesystem->ReadFile("cmm_chapterbg.json", "scripts/", fbuf);
		json j = json::parse(fbuf.String());
		auto dict = j["chapter_bg"];
		char numbuf[16];
		Q_snprintf(numbuf, 16, "%d", g_nCurrentChapterIndex);
		if (dict[numbuf].is_string())
		{
			strncpy(vidnamebuf, dict[numbuf].get<const char*>(), 256);
		}
	}

	char jsbuf[256 + 32];

	Q_snprintf(jsbuf, 256 + 32, "addBackgroundVideo(%s);", "test");

	RunJavascript(jsbuf);*/

}

/*void CMainMenu::OnThink()
{
	BaseClass::OnThink();
	if (m_flLoadTime == -1 || gpGlobals->curtime < m_flLoadTime)
		return;
	
	Paint();
	m_flLoadTime = -1;
}*/

#define MM_CMD(command) if(!strcmp(szCmd, command))
void CMainMenu::OnMenuCommand(KeyValues* pKv)
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
	MM_CMD("newgame")
	{
		pGameUI->SendMainMenuCommand("OpenNewGameDialog");
		return;
	}
	MM_CMD("startscenario")
	{

	}
	MM_CMD("loadgame")
	{
		pGameUI->SendMainMenuCommand("OpenLoadGameDialog");
		return;
	}
	MM_CMD("savegame")
	{
		pGameUI->SendMainMenuCommand("OpenSaveGameDialog");
		return;
	}
	MM_CMD("options")
	{
		pGameUI->SendMainMenuCommand("OpenOptionsDialog");
		return;
	}
	MM_CMD("startchapter")
	{
		char buf[64];
		Q_snprintf(buf, 64, "engine map %s", szParam);
		pGameUI->SendMainMenuCommand(buf);
		return;
	}
	MM_CMD("startscenario")
	{
		char buf[64];
		Q_snprintf(buf, 64, "engine map %s", szParam);
		pGameUI->SendMainMenuCommand(buf);
		return;
	}
	MM_CMD("quit")
	{
		pGameUI->SendMainMenuCommand("Quit");
		return;
	}
	MM_CMD("resume")
	{
		pGameUI->SendMainMenuCommand("ResumeGame");
		return;
	}
	MM_CMD("vconsole")
	{
		DevMsg(szParam);
		return;
	}
	DevMsg("MenuCommand: URL: %s CMD: %s PARAM: %s\n", szUrl, szCmd, szParam);
}

void CMainMenu::ProcessChapters()
{
	char buffer[256];
	DevMsg("Reading chapters\n");
	FileHandle_t hChaptersFile = filesystem->Open("scripts/newch_chaptertitles.txt", "r");

	int iChapter = 0;

	while (!filesystem->EndOfFile(hChaptersFile))
	{
		filesystem->ReadLine(buffer, 256, hChaptersFile);
		DevMsg("Chapter prefix: %s\n", buffer);
		if (buffer[0] != '#')
			break;
		for (int j = 0; j < 256; j++)
		{
			if (!buffer[j])
				break;
			if (buffer[j] == '\n')
			{
				buffer[j] = '\0';
				break;
			}
		}

		char buftitle[262];
		//char bufdesc[262];

		strcpy_s(buftitle, buffer);
		strcat_s(buftitle, "_Title");
		const char* pTitle = g_pVGuiLocalize->FindAsUTF8(buftitle);

		//strcpy_s(bufdesc, buffer);
		//strcat_s(bufdesc, "_Desc");
		//const char* pDesc = g_pVGuiLocalize->FindAsUTF8(bufdesc);

		char mapnamebuf[32];

		Q_snprintf(mapnamebuf, 32, "c%ua0", iChapter);

		AddChapter(pTitle, mapnamebuf);

		iChapter++;
	}
}

void CMainMenu::AddChapter(const char* buftitle, const char* mapnamebuf)
{
	char chapterelement[1024];
	Q_snprintf(chapterelement, 1024, "menu_addchapter([\"%s\", \"%s\"]);\n", buftitle, mapnamebuf);
	DevMsg("CMainMenu::AddChapter: %s\n", chapterelement);
	RunJavascript(chapterelement);
}

CMainMenu::~CMainMenu()
{
	/*if (pGameUI)
		pGameUI->SetMainMenuOverride(iOriginalMenu);*/
	pGameUI->SetMainMenuOverride((vgui::VPANEL)NULL);
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
	void Reload();
	void UpdateInGameStatus(bool bValue);
private:
	CMainMenu* pInstance;
};

void CIMainMenu::Create(vgui::VPANEL parent)
{
	DevMsg("CIMainMenu::Create\n");
	if (pInstance)
		Destroy();
	pInstance = new CMainMenu(parent);
	Destroy();
	pInstance = new CMainMenu(parent);
	Reload();
}

void CIMainMenu::Destroy()
{
	DevMsg("CIMainMenu::Destroy\n");
	if (pInstance)
	{
		pInstance->SetParent((vgui::VPANEL)NULL);
		delete pInstance;
		pInstance = NULL;
	}
}

void CIMainMenu::Activate()
{
	DevMsg("CIMainMenu::Activate\n");
	pInstance->Activate();
}

void CIMainMenu::Reload()
{
	if (pInstance)
	{
		char buffer[1024];
		filesystem->GetLocalPath(cl_mainmenu_url.GetString(), buffer, 1024);
		pInstance->OpenURL(buffer, NULL, true);
		DevMsg("CIMainmenu filename: %s\n", buffer);
	}
}

void CIMainMenu::UpdateInGameStatus(bool bValue)
{
	if (pInstance)
	{
		pInstance->UpdateInGameStatus(bValue);
	}
}

static CIMainMenu gIMainMenu;
IMainMenu* g_pMainMenu = (IMainMenu*)&gIMainMenu;

void reload_menu_callback(const CCommand& cmd)
{
	//gIMainMenu.Reload();
	gIMainMenu.Destroy();
	gIMainMenu.Create(enginevgui->GetPanel(PANEL_GAMEUIDLL));
	gIMainMenu.UpdateInGameStatus(engine->IsInGame() && !engine->IsLevelMainMenuBackground());
}

static ConCommand reload_menu("reload_menu", &reload_menu_callback);