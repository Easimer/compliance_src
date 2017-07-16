#include "cbase.h"
#include <vgui/VGUI.h>
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui/IPanel.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ListPanel.h>
#include <vgui_controls/PanelListPanel.h>
#include <vgui_controls/Label.h>
#include "filesystem.h"
#include "filesystem_helpers.h"
#include "INewChapters.h"
#include "memdbgon.h"

struct ChapterEntry {
	char m_iszTitle[256];
	char m_iszDescription[256];
};

#define MAX_CHAPTERS 32

class CNCPanel : public vgui::Frame {
public:
	DECLARE_CLASS_SIMPLE(CNCPanel, vgui::Frame);
	CNCPanel(vgui::VPANEL parent);

	void SetVisible2(bool bVisible);
protected:
	//virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);
private:
	//vgui::PanelListPanel* m_pChapterList;
	vgui::ListPanel* m_pChapterList;
	ChapterEntry m_entries[MAX_CHAPTERS];
};

class CNCChapterPanel : public vgui::Panel {
public:
	DECLARE_CLASS_SIMPLE(CNCChapterPanel, vgui::Panel);
	CNCChapterPanel(vgui::VPANEL parent);
	CNCChapterPanel(vgui::VPANEL parent, ChapterEntry& e);
private:
	void InitCommon(vgui::VPANEL parent);
private:
	vgui::Label* m_pTitleLabel;
	vgui::Label* m_pDescLabel;
	//vgui::Image* m_pImage;
	ChapterEntry m_ChapterEntry;
};

CNCChapterPanel::CNCChapterPanel(vgui::VPANEL parent) : BaseClass(NULL, "ChapterPanel")
{
	strcpy_s(m_ChapterEntry.m_iszTitle, "!!! Untitled !!!");
	strcpy_s(m_ChapterEntry.m_iszDescription, "(error)");
	InitCommon(parent);
}

CNCChapterPanel::CNCChapterPanel(vgui::VPANEL parent, ChapterEntry& e) : BaseClass(NULL, "ChapterPanel")
{
	strcpy_s(m_ChapterEntry.m_iszTitle, e.m_iszTitle);
	strcpy_s(m_ChapterEntry.m_iszDescription, e.m_iszDescription);
	InitCommon(parent);
}

void CNCChapterPanel::InitCommon(vgui::VPANEL parent)
{
	SetParent(parent);
	SetProportional(true);
	SetMouseInputEnabled(true);
	SetVisible(true);
	m_pTitleLabel = new vgui::Label(this, "ChapterTitle", m_ChapterEntry.m_iszTitle);
	m_pTitleLabel->SetContentAlignment(vgui::Label::Alignment::a_west);
	m_pTitleLabel->SetPos(10, 10);
	m_pTitleLabel->SetSize(250, 30);
	
	m_pDescLabel = new vgui::Label(this, "ChapterDescription", m_ChapterEntry.m_iszDescription);
	m_pDescLabel->SetPos(10, 40);
	m_pDescLabel->SetSize(100, 80);
	m_pDescLabel->SetWrap(true);

	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));
	SetSize(310, 96);
	SetPos(10, 10);
}

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

	//m_pChapterList = new vgui::PanelListPanel(this, "ChapterList");
	m_pChapterList = new vgui::ListPanel(this, "ChapterList");
	//m_pChapterList->SetPinCorner(vgui::Panel::PinCorner_e::PIN_TOPLEFT, 0, 0);

	char buffer[256];
	int i = 0;

	DevMsg("Reading chapters\n");
	FileHandle_t hChaptersFile = filesystem->Open("scripts/newch_chaptertitles.txt", "r");

	KeyValues* kv = new KeyValues("chapters");

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

		char buf2[262];

		strcpy_s(buf2, buffer);
		strcat_s(buf2, "_Title");
		const char* pTitle = g_pVGuiLocalize->FindAsUTF8(buf2);
		strcpy_s(m_entries[i].m_iszTitle, pTitle);

		strcpy_s(buf2, buffer);
		strcat_s(buf2, "_Desc");
		const char* pDesc = g_pVGuiLocalize->FindAsUTF8(buf2);
		strcpy_s(m_entries[i].m_iszDescription, pDesc);

		DevMsg("Title: %s\n\t%s\n", m_entries[i].m_iszTitle, m_entries[i].m_iszDescription);

		kv->SetString(m_entries[i].m_iszTitle, m_entries[i].m_iszDescription);

		//CNCChapterPanel* pChPanel = new CNCChapterPanel(m_pChapterList->GetVPanel(), m_entries[i]);

		//m_pChapterList->AddItem(NULL, pChPanel);

		i++;
		if (i == MAX_CHAPTERS)
			break;
	}

	

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