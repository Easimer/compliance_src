#include "cbase.h"
#include "items.h"
#include "memdbgon.h"

class CItemSuppKit : public CItem {
public:
	DECLARE_CLASS(CItemSuppKit, CItem);

	void Spawn(void);
	void Precache(void);
	bool MyTouch(CBasePlayer *pPlayer);
};

LINK_ENTITY_TO_CLASS(item_suppkit, CItemSuppKit);
PRECACHE_REGISTER(item_suppkit);

void CItemSuppKit::Precache()
{
	PrecacheModel("models/items/suppkit.mdl");
	PrecacheScriptSound("BaseCombatCharacter.AmmoPickup");
	BaseClass::Precache();
}

void CItemSuppKit::Spawn()
{
	Precache();
	SetModel("models/items/suppkit.mdl");
	BaseClass::Spawn();
}

bool CItemSuppKit::MyTouch(CBasePlayer *pPlayer)
{
	CBaseCombatWeapon* pWeapon;
	int i;
	for (i = 0; i < MAX_WEAPONS; i++)
	{
		pWeapon = pPlayer->GetWeapon(i);
		if (!pWeapon)
			continue;
		if (pWeapon->m_bIsSuppressed)
			pWeapon->m_iSuppressorDurability = pWeapon->m_iSuppressorMaxDurability;
	}
	if (i != 0)
	{
		CSingleUserRecipientFilter user(pPlayer);
		user.MakeReliable();

		UserMessageBegin(user, "ItemPickup");
		WRITE_STRING(GetClassname());
		MessageEnd();

		EmitSound("BaseCombatCharacter.AmmoPickup");

		UTIL_Remove(this);

		return true;
	}

	return false;
}
