#include "cbase.h"

#include "GameUI/IGameUI.h"
#include "IGunsmith.h"
#include "vgui_controls\Frame.h"
#include "usermessages.h"
#include "basehlcombatweapon_shared.h"

class CGunsmith : public vgui::Frame{
public:
	DECLARE_CLASS_SIMPLE(CGunsmith, vgui::Frame);
	CGunsmith(vgui::VPANEL parent);
	~CGunsmith();

protected:
	virtual void OnCommand(const char* pcCommand);
};

void __MsgFunc_OpenGunsmith(bf_read& msg);

CGunsmith::CGunsmith(vgui::VPANEL parent) : BaseClass(NULL, "Gunsmith")
{
	SetParent(parent);
	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);

	SetProportional(true);
	SetVisible(false);
	SetMoveable(false);
	SetSizeable(false);
	SetTitleBarVisible(true);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(true);
	SetPaintBackgroundEnabled(true);
	SetPaintBorderEnabled(true);

	//SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));
	LoadControlSettings("resource/UI/gunsmith.res");

	usermessages->HookMessage("Gunsmith_Open", __MsgFunc_OpenGunsmith);
}

CGunsmith::~CGunsmith()
{
}

#define MM_CMD(command) if(!strcmp(cmd, command))
#define MM_CMD_START(command) if(strncmp(command, cmd, sizeof(command)) == 0)

void CGunsmith::OnCommand(char const* cmd)
{
	DevMsg("Gunsmith::OnCommand: %s\n", cmd);

	MM_CMD("Close")
	{
		SetVisible(false);
	}

	auto pPlayer = C_BasePlayer::GetLocalPlayer();
	if (!pPlayer)
		return;
	auto pWeapon = pPlayer->GetActiveWeapon();
	if (!pWeapon)
		return;
	auto pHLWeapon = dynamic_cast<C_BaseHLCombatWeapon*>(pWeapon);
	if (!pHLWeapon)
		return;
	MM_CMD("AddSuppressor")
	{
		pWeapon->m_bIsSuppressed = true;
		pWeapon->m_iSuppressorMaxDurability = pWeapon->GetMaxClip1();
		pWeapon->m_iSuppressorDurability = pWeapon->GetMaxClip1();
	}
	MM_CMD_START("SetColor")
	{
		char* buf = (char*)malloc(strlen(cmd) + 1);
		strcpy(buf, cmd);
		char* pCh;
		pCh = strtok(buf, " ");
		pCh = strtok(buf, " ");
		int nVal = Q_atoi(pCh);
		free(buf);
		DevMsg("Gunsmith: %d\n", nVal);
		
		
	}
}

class CIGunsmith : public IGunsmith {
public:
	void Create(vgui::VPANEL parent);
	void Destroy();
	void Activate();
	void Show();
private:
	CGunsmith* pInstance;
};

void CIGunsmith::Create(vgui::VPANEL parent)
{
	pInstance = new CGunsmith(parent);
}

void CIGunsmith::Destroy()
{
	if (pInstance)
	{
		pInstance->SetParent((vgui::VPANEL)NULL);
		delete pInstance;
	}
}

void CIGunsmith::Activate()
{
	if (pInstance)
		pInstance->Activate();
}

void CIGunsmith::Show()
{
	if (pInstance)
		pInstance->SetVisible(true);
}

static CIGunsmith gIGunsmith;
IGunsmith* g_pGunsmith = (IGunsmith*)&gIGunsmith;

void __MsgFunc_OpenGunsmith(bf_read& msg)
{
	gIGunsmith.Show();
}