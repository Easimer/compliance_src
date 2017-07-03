#include "cbase.h"
#include "hl2_player.h"

CSuitPowerDevice SuitDeviceXRay(bits_SUIT_DEVICE_XRAY, 20);

void IN_XRayOn(const CCommand& args)
{
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer)
		return;
	CHL2_Player* pHL2Player = dynamic_cast<CHL2_Player*>(pPlayer);
	if (!pHL2Player)
		return;
	pHL2Player->SuitPower_AddDevice(SuitDeviceXRay);

	CBaseEntity* pNPC = gEntList.FindEntityByClassname(NULL, "npc_*");
	while (pNPC)
	{
		CBaseCombatCharacter* pCombatChar = ToBaseCombatCharacter(pNPC);
		if (pCombatChar)
		{
			if ((pPlayer->GetAbsOrigin() - pCombatChar->GetAbsOrigin()).Length() <= 512)
			{
				pCombatChar->AddGlowEffect();
			}
		}
		pNPC = gEntList.FindEntityByClassname(pNPC, "npc_*");
	}
}

void IN_XRayOff(const CCommand& args)
{
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer)
		return;
	CHL2_Player* pHL2Player = dynamic_cast<CHL2_Player*>(pPlayer);
	if (!pHL2Player)
		return;
	pHL2Player->SuitPower_RemoveDevice(SuitDeviceXRay);

	CBaseEntity* pNPC = gEntList.FindEntityByClassname(NULL, "npc_*");
	while (pNPC)
	{
		CBaseCombatCharacter* pCombatChar = ToBaseCombatCharacter(pNPC);
		if (pCombatChar)
		{
			//if ((GetAbsOrigin() - pCombatChar->GetAbsOrigin()).Length() <= 512)
			{
				pCombatChar->RemoveGlowEffect();
			}
		}
		pNPC = gEntList.FindEntityByClassname(pNPC, "npc_*");
	}
}

static ConCommand bind_xray_on("+xray", IN_XRayOn);
static ConCommand bind_xray_off("-xray", IN_XRayOff);