#include "cbase.h"
#include "CustomizableWeapon.h"
#include "usermessages.h"

class CWeaponColor_Sender : public CBaseEntity {
public:
	DECLARE_CLASS_NOBASE(CWeaponColor_Sender);
	DECLARE_SERVERCLASS_NOBASE();

	CNetworkVar(int, m_iColor);
};

BEGIN_SEND_TABLE_NOBASE(CWeaponColor_Sender, DT_WeaponColor)
SendPropInt(SENDINFO(m_iColor)),
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(internal_weaponcolor, CWeaponColor_Sender);

void change_weapon_color_func(int arg);

void __MsgFunc_Gunsmith_SetColor(bf_read& msg)
{
	change_weapon_color_func(msg.ReadVarInt32());
}

CWeaponColor_Sender* CreateWeaponColor()
{
	return (CWeaponColor_Sender*)CBaseEntity::Create("internal_Weaponcolor", vec3_origin, QAngle(0, 0, 0));;
}

void WeaponColor_WeaponSwitch(CBasePlayer* pPlayer)
{
	CWeaponColor_Sender* pColorSender = dynamic_cast<CWeaponColor_Sender*>(gEntList.FindEntityByClassname(NULL, "internal_weaponcolor"));
	if (!pColorSender)
	{
		pColorSender = CreateWeaponColor();
	}
	if (!pPlayer)
		return;
	CBaseHLCombatWeapon* pWeapon = dynamic_cast<CBaseHLCombatWeapon*>(pPlayer->GetActiveWeapon());
	if (!pWeapon)
		return;

	pColorSender->m_iColor.Set(pWeapon->m_iCustomColor.Get());
}

void change_weapon_color_callback(const CCommand& cmd)
{
	if (cmd.ArgC() < 2)
	{
		DevMsg("failed: less than 2 args\n");
		return;
	}

//	change_weapon_color_func(Q_atoi(cmd.ArgV[1]));
}

void change_weapon_color_func(int arg)
{
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer)
		return;
	CBaseHLCombatWeapon* pWeapon = dynamic_cast<CBaseHLCombatWeapon*>(pPlayer->GetActiveWeapon());
	if (!pWeapon)
		return;

	pWeapon->m_iCustomColor = arg;
	CWeaponColor_Sender* pColorSender = dynamic_cast<CWeaponColor_Sender*>(gEntList.FindEntityByClassname(NULL, "internal_weaponcolor"));
	if (!pColorSender)
	{
		pColorSender = CreateWeaponColor();
	}
	pColorSender->m_iColor = arg;
}

static ConCommand change_weapon_color("change_weapon_color", change_weapon_color_callback);