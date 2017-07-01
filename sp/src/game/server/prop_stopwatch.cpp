#include "cbase.h"
#include "props.h"
#include "physics_shared.h"
#include "physics_saverestore.h"

#define MODEL "models/props_combine/breenclock.mdl"

bool gbTimeStopped = false;

class CPropStopwatch : public CPhysicsProp {
public:
	DECLARE_CLASS(CPropStopwatch, CPhysicsProp);
	//DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
	void Precache();
	void Spawn();
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void Think();
	void StartTime();
	void StopTime();

private:
	CBaseEntity* m_apStoppedEntities[128];
	int m_nStoppedEntities;
};

BEGIN_DATADESC(CPropStopwatch)
DEFINE_THINKFUNC(Think)
END_DATADESC()
LINK_ENTITY_TO_CLASS(prop_stopwatch, CPropStopwatch);

void CPropStopwatch::Precache()
{
	PrecacheModel(MODEL);
	PrecacheScriptSound("Clock.Slowdown");
	PrecacheScriptSound("Clock.Speedup");
	BaseClass::Precache();
}

void CPropStopwatch::Spawn()
{
	Precache();
	SetModel(MODEL);
	// TODO: need a proper mechanism to reset this on mapload
	//		 because of this, do not spawn these clocks dinamically, at least when they are already used
	gbTimeStopped = false;
	SetThink(&CPropStopwatch::Think);
	SetNextThink(TICK_NEVER_THINK);
	
	BaseClass::Spawn();
}

void CPropStopwatch::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (gbTimeStopped)
		StartTime();
	else
		StopTime();
}

void CPropStopwatch::StartTime()
{
	DevMsg("Time is now resumed\n");
	gbTimeStopped = false;
	EmitSound("Clock.Speedup");
	/*for (int i = 0; i < m_nStoppedEntities; i++)
	{
		try {
			auto pPhys = m_apStoppedEntities[i]->VPhysicsGetObject();
			if (pPhys)
				pPhys->EnableMotion(true);
		} catch(std::exception) {}
	}
	m_nStoppedEntities = 0;*/
	SetNextThink(TICK_NEVER_THINK);
}

void CPropStopwatch::StopTime()
{
	DevMsg("Time is now stopped\n");
	gbTimeStopped = true;
	EmitSound("Clock.Slowdown");
	/*m_nStoppedEntities = UTIL_EntitiesInSphere(m_apStoppedEntities, 128, GetAbsOrigin(), 512, 0);
	DevMsg("%d entities stopped\n", m_nStoppedEntities);
	for (int i = 0; i < m_nStoppedEntities; i++)
	{
		CBaseEntity* pEnt = m_apStoppedEntities[i];
		if (pEnt == this)
			continue;
		auto pPhys = pEnt->VPhysicsGetObject();
		if (pPhys)
			pPhys->EnableMotion(false);
	}*/
	//SetNextThink(gpGlobals->curtime + 5);
}

void CPropStopwatch::Think()
{
	StartTime();
	SetNextThink(TICK_NEVER_THINK);
}
