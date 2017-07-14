#include "cbase.h"
#include "achievementmgr.h"
#include "baseachievement.h"

#ifdef GAME_DLL

#define ACHIEVEMENT_COMP_SCENARIOSPLAYED 115

class CAchievementScenariosPlayed : public CBaseAchievement {
protected:

	virtual void Init()
	{
		SetFlags(ACH_LISTEN_MAP_EVENTS | ACH_SAVE_WITH_GAME);
		SetGoal(1);
	}
	virtual void ListenForEvents()
	{
		ListenForGameEvent("scenario_started");
	}
	void FireGameEvent_Internal(IGameEvent *event)
	{
		IncrementCount();
	}
};

DECLARE_ACHIEVEMENT(CAchievementScenariosPlayed, ACHIEVEMENT_COMP_SCENARIOSPLAYED, "COMP_SCENARIOSPLAYED", 5);
#endif