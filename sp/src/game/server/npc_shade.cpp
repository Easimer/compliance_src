#include "cbase.h"
#include "ai_basenpc.h"
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

enum {

	NEXT_TASK
};

enum {
	SCHED_SHADE_WANDER,
	SCHED_SHADE_WANDER_FAIL,
	NEXT_SCHEDULE
};

class CNPC_Shade : public CAI_BaseNPC
{
	DECLARE_CLASS(CNPC_Shade, CAI_BaseNPC);
public:
	void Spawn();
	void Precache();
	int SelectSchedule();
	int SelectFailSchedule(int failedSchedule, int failedTask, AI_TaskFailureCode_t taskFailCode);
	Class_T Classify();
	void NPCThink();
public:
	DEFINE_CUSTOM_AI;
	DECLARE_DATADESC();
};

void CNPC_Shade::Precache()
{
	PrecacheModel("models/zombie/fast.mdl");
	BaseClass::Precache();
}

void CNPC_Shade::NPCThink()
{
	DevMsg("SetSchedule");
	SetSchedule(SCHED_SHADE_WANDER);
	SetNextThink(-1);
	BaseClass::NPCThink();
}

void CNPC_Shade::Spawn()
{
	Precache();
	SetModel("models/zombie/fast.mdl");
	SetBloodColor(BLOOD_COLOR_ZOMBIE);

	SetHullSizeNormal(true);
	SetDefaultEyeOffset();
	SetActivity(ACT_IDLE);
	SetSolid(SOLID_BBOX);

	SetSchedule(SCHED_FALL_TO_GROUND);
	SetThink(&CNPC_Shade::NPCThink);
	SetNextThink(gpGlobals->curtime + 3);

	SetNavType(NAV_GROUND);
	SetMoveType(MOVETYPE_STEP);

	BaseClass::Spawn();
}

Class_T CNPC_Shade::Classify()
{
	return CLASS_NONE;
}

int CNPC_Shade::SelectSchedule()
{
	return SCHED_SHADE_WANDER;
}

int CNPC_Shade::SelectFailSchedule(int failedSchedule, int failedTask, AI_TaskFailureCode_t taskFailCode)
{
	if (failedSchedule == SCHED_SHADE_WANDER)
	{
		return SCHED_SHADE_WANDER_FAIL;
	}
	return BaseClass::SelectFailSchedule(failedSchedule, failedTask, taskFailCode);
}

LINK_ENTITY_TO_CLASS(npc_shade, CNPC_Shade);

BEGIN_DATADESC(CNPC_Shade)
DEFINE_THINKFUNC(NPCThink),
END_DATADESC()

AI_BEGIN_CUSTOM_NPC(npc_shade, CNPC_Shade)
	DEFINE_SCHEDULE
	(
	SCHED_NONE,
	"   Tasks"
	"       TASK_SET_SCHEDULE				SCHEDULE:SCHED_SHADE_WANDER"
	)
	DEFINE_SCHEDULE
	(
	SCHED_SHADE_WANDER,

	"	Tasks"
	"		TASK_STOP_MOVING				0"
	"		TASK_WANDER						480384" // 4 feet to 32 feet
	"		TASK_WALK_PATH					0"
	"		TASK_STOP_MOVING				0"
	"		TASK_SET_SCHEDULE				SCHEDULE:SCHED_SHADE_WANDER" // keep doing it
	)
	DEFINE_SCHEDULE
	(
	SCHED_SHADE_WANDER_FAIL,

	"	Tasks"
	"		TASK_STOP_MOVING		0"
	"		TASK_WAIT				1"
	"		TASK_SET_SCHEDULE		SCHEDULE:SCHED_ZOMBIE_WANDER_MEDIUM"
	)
AI_END_CUSTOM_NPC()

