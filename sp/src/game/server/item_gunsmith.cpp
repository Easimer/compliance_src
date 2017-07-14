#include "cbase.h"
#include "items.h"
#include "memdbgon.h"

class CItemGunsmith : public CItem {
public:
	DECLARE_CLASS(CItemGunsmith, CItem);

	void Spawn(void);
	void Precache(void);
	bool MyTouch(CBasePlayer *pPlayer);

	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
};

LINK_ENTITY_TO_CLASS(item_Gunsmith, CItemGunsmith);
PRECACHE_REGISTER(item_Gunsmith);

void CItemGunsmith::Precache()
{
	PrecacheModel("models/items/battery.mdl");
	PrecacheScriptSound("BaseCombatCharacter.AmmoPickup");
	BaseClass::Precache();
}

void CItemGunsmith::Spawn()
{
	Precache();
	SetModel("models/items/battery.mdl");
	BaseClass::Spawn();
}

bool CItemGunsmith::MyTouch(CBasePlayer *pPlayer)
{
	return false;
}

void CItemGunsmith::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer)
		return;
	if (pActivator != pPlayer)
		return;

	CSingleUserRecipientFilter user(pPlayer);
	user.MakeReliable();

	UserMessageBegin(user, "Gunsmith_Open");
	MessageEnd();
}