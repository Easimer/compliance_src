#include "cbase.h"
#include "memdbgon.h"

enum ScenarioType_t {
	ST_CTF, // Capture the Flag
	ST_TARGET, // Kill the Target
	ST_FFA, // Kill Everyone
	ST_ESCORT, // Escort
	ST_MAX = 4
};

enum EnemyType_t {
	ET_HUMAN,
	ET_COMBINE,
	ET_XENIAN,
	ET_ANTLION,
	ET_MAX = 4
};

#define RULE(rule) (rule)
#define RULE_EXCLUSION(t1, t2) ((1 << t1) ^ (1 << t2)) // at least one, but not both
#define RULE_INCLUSION(t1, t2) ((1 << t1) | (1 << t2)) // at least one

#define RULE_EX RULE_EXCLUSION
#define RULE_IN RULE_INCLUSION

// Human OR Combine OR Zombie OR Antlion
#define RULE_HL2 RULE(RULE_EX(ET_HUMAN, RULE_EX(ET_COMBINE, RULE_EX(ET_XENIAN, ET_ANTLION))))


static struct { char* szName; EnemyType_t iType; bool bHaveWeapon; } enemy_types[] = {
	// Combine
	{ "npc_combine_s", ET_COMBINE, true},
	{ "npc_metropolice", ET_COMBINE, true},
	{ "npc_manhack", ET_COMBINE, false},
	// Human
	{ "npc_citizen", ET_HUMAN, true},
	// Xenian
	{ "npc_headcrab", ET_XENIAN, false},
	{ "npc_zombie", ET_XENIAN, false},
	{ "npc_fastzombie", ET_XENIAN, false},
	{ "npc_zombine", ET_XENIAN, false},
	{ "npc_antlion", ET_ANTLION, false},
	NULL
};

#define ENEMY_TYPES 6

static struct { char* szWeaponName; char* szAmmoName; } weapon_types[] = {
	{ "weapon_pistol", "item_ammo_pistol" },
	{ "weapon_smg1", "item_ammo_smg1" },
	{ "weapon_ar2", "item_ammo_ar2" },
};

#define WEAPON_TYPES 3

static char* item_types[] = {
	"item_healthkit",
	"item_healthvial",
	"item_battery"
};

#define ITEM_TYPES 3

class CObjective : public CLogicalEntity {
public:
	DECLARE_CLASS(CObjective, CLogicalEntity);
	void Spawn();
	void SetObjective(ScenarioType_t iType, CItem* pTarget);
	void SetObjective(ScenarioType_t iType, CBaseCombatCharacter* pTarget);
	ScenarioType_t GetType();
	void* GetTarget();
private:
	ScenarioType_t iST;
	CItem* pTargetItem;
	CBaseCombatCharacter* pTargetCharacter;
};

// Generates a scenario
class CScenarioGenerator : public CLogicalEntity {
public:
	DECLARE_CLASS(CScenarioGenerator, CLogicalEntity);
	DECLARE_DATADESC();
	void Spawn();
	void Think();
private:
	void Generate();
};

BEGIN_DATADESC(CScenarioGenerator)
DEFINE_THINKFUNC(Think),
END_DATADESC()

LINK_ENTITY_TO_CLASS(rnd_generator, CScenarioGenerator)

class CRandomObjective : public CLogicalEntity {
public:
	DECLARE_CLASS(CRandomObjective, CLogicalEntity);
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
private:
	CNetworkVar(bool, m_bAllowItem); // Allow objective to be CTF
	CNetworkVar(bool, m_bAllowEnemy); // Allow objective to be an NPC
};

BEGIN_DATADESC(CRandomObjective)
DEFINE_KEYFIELD(m_bAllowItem, FIELD_BOOLEAN, "AllowItem"),
DEFINE_KEYFIELD(m_bAllowEnemy, FIELD_BOOLEAN, "AllowEnemy"),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CRandomObjective, DT_RandomObjective)
SendPropBool(SENDINFO(m_bAllowItem)),
SendPropBool(SENDINFO(m_bAllowEnemy)),
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(rnd_enemy_spawn, CLogicalEntity);
LINK_ENTITY_TO_CLASS(rnd_supply_spawn, CLogicalEntity);
LINK_ENTITY_TO_CLASS(rnd_objective, CRandomObjective);

void CScenarioGenerator::Spawn()
{
	BaseClass::Spawn();
	Generate();
	SetThink(&CScenarioGenerator::Think);
	SetNextThink(gpGlobals->curtime + 0.2);
}

void CScenarioGenerator::Generate()
{
	ScenarioType_t st;
	CBaseEntity* pEnt;
	CObjective* pObj = NULL;
	int weapon_flags = 0;

	float fStart = gpGlobals->curtime;

	st = (ScenarioType_t)(rand() % ST_MAX);

	CBroadcastRecipientFilter filter;
	g_pGameRules->MarkAchievement(filter, "COMP_SCENARIOSPLAYED");

	DevMsg("Spawning enemies\n");

	// Get enemy spawns

	EnemyType_t et = (EnemyType_t)(rand() % ET_MAX);

	pEnt = gEntList.FindEntityByClassname(NULL, "rnd_enemy_spawn");
	while (pEnt)
	{
		int i = rand() % ENEMY_TYPES;
		int nTries = 0;
		while (enemy_types[i].iType != et)
		{
			i = rand() % ENEMY_TYPES;
			nTries++;
			if (nTries == 10) // give up, spawn first enemy in that category
			{
				for (int j = 0; j < ENEMY_TYPES; j++)
				{
					if (enemy_types[j].iType == et)
					{
						i = j;
						goto givenUp;
					}
				}
				// Looks like there is no NPC in that category, don't spawn anything
				goto next_enemy;
			}
		}

		givenUp:
		
		DevMsg("Enemy: %s\n", enemy_types[i].szName);
		CBaseCombatCharacter* pEnemy = (CBaseCombatCharacter*)CBaseEntity::Create(enemy_types[i].szName, pEnt->GetAbsOrigin(), pEnt->GetAbsAngles());
		pEnemy->AddClassRelationship(CLASS_PLAYER, Disposition_t::D_HT, 100);
		if (enemy_types[i].bHaveWeapon)
			pEnemy->Weapon_Equip((CBaseCombatWeapon*)CBaseCombatWeapon::Create("weapon_smg1", pEnemy->GetAbsOrigin(), pEnemy->GetAbsAngles()));
		
	next_enemy:
		UTIL_Remove(pEnt);
		pEnt = gEntList.FindEntityByClassname(pEnt, "rnd_enemy_spawn");
	}

	// Get supply spawns
	DevMsg("Spawning weapons\n");
	pEnt = gEntList.FindEntityByClassname(NULL, "rnd_supply_spawn");
	while (pEnt)
	{
		int i;
		if ((rand() & 1)) // should we spawn weapon/ammo or item
		{
			i = rand() % WEAPON_TYPES;
			if ((rand() & 1)) // should we spawn ammo or weapon?
			{
				if (weapon_flags & (1 << i)) // only if we've already spawned the weapon
				{
					DevMsg("Spawning ammo for %s\n", weapon_types[i].szWeaponName);
					CBaseEntity::Create(weapon_types[i].szAmmoName, pEnt->GetAbsOrigin(), pEnt->GetAbsAngles());
					goto next_item;
				}
			}
			DevMsg("Spawning weapon %s\n", weapon_types[i].szWeaponName);
			CBaseEntity::Create(weapon_types[i].szWeaponName, pEnt->GetAbsOrigin(), pEnt->GetAbsAngles());
			weapon_flags |= (1 << i); // mark weapon spawned
		}
		else
		{
			i = rand() % ITEM_TYPES;
			DevMsg("Spawning item %s\n", item_types[i]);
			CBaseEntity::Create(item_types[i], pEnt->GetAbsOrigin(), pEnt->GetAbsAngles());
		}
	next_item:
		UTIL_Remove(pEnt);
		pEnt = gEntList.FindEntityByClassname(pEnt, "rnd_supply_spawn");
	}

	// Give player weapons

	

	DevMsg("Scenario generated under %f seconds\n", gpGlobals->curtime - fStart);

	return;
	pEnt = gEntList.FindEntityByClassname(NULL, "rnd_objective");
	if (pEnt)
	{
		pObj = (CObjective*)CBaseEntity::Create("scenario_objective", pEnt->GetAbsOrigin(), pEnt->GetAbsAngles());
	}
}

void CScenarioGenerator::Think()
{
	int nPlayerWeapons = 0;
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer)
	{
		DevMsg("NO PLAYER!!!\n");
		SetNextThink(gpGlobals->curtime + 0.2);
		return;
	}
	else
	{
		for (int i = 0; i < max(4, (rand() % 16)); i++)
		{
			int w = rand() % WEAPON_TYPES;
			if (rand() & 1)
			{
				if (nPlayerWeapons & (1 << w))
				{
					CBaseEntity::Create(weapon_types[w].szAmmoName, pPlayer->GetAbsOrigin(), pPlayer->GetAbsAngles());
					continue;
				}
			}
			CBaseEntity::Create(weapon_types[w].szWeaponName, pPlayer->GetAbsOrigin(), pPlayer->GetAbsAngles());
		}
		UTIL_Remove(this);
	}
}
