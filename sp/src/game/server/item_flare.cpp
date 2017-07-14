#include "cbase.h"
#include "items.h"
#include "hl2_player_shared.h"
#include "memdbgon.h"

class CItemFlare : public CItem {
public:
	DECLARE_CLASS(CItemFlare, CItem);

	void Spawn(void);
	void Precache(void);
	bool MyTouch(CBasePlayer *pPlayer);
};

LINK_ENTITY_TO_CLASS(item_flare, CItemFlare);
PRECACHE_REGISTER(item_flare);

void CItemFlare::Precache()
{
	PrecacheModel("models/props_junk/flare.mdl");
	PrecacheScriptSound("BaseCombatCharacter.AmmoPickup");
	BaseClass::Precache();
}

void CItemFlare::Spawn()
{
	Precache();
	SetModel("models/props_junk/flare.mdl");
	BaseClass::Spawn();
}

bool CItemFlare::MyTouch(CBasePlayer *pPlayer)
{
	CHL2_Player* pHLPlayer = dynamic_cast<CHL2_Player*>(pPlayer);
	if (!pHLPlayer)
		return false;

	if (pHLPlayer->m_HL2Local.m_nFlares == 10)
		return false;

	CSingleUserRecipientFilter user(pPlayer);
	user.MakeReliable();

	pHLPlayer->m_HL2Local.m_nFlares++;

	UserMessageBegin(user, "FlareUpdate");
	MessageWriteLong(pHLPlayer->m_HL2Local.m_nFlares);
	MessageEnd();

	EmitSound("BaseCombatCharacter.AmmoPickup");

	UTIL_Remove(this);

	return true;
}