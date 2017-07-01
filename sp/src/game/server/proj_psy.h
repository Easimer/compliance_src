#pragma once

#include "baseprojectile.h"
#include "particle_system.h"

class CProjectilePsy : public CParticleSystem {
public:
	DECLARE_CLASS(CProjectilePsy, CParticleSystem);
#if !defined( CLIENT_DLL )
	DECLARE_DATADESC();
#endif
	CProjectilePsy();
	void Precache();
	void Spawn();
	void Think();
	void Touch(CBaseEntity* pOther);
private:
	Vector m_vVelocity;
	bool m_bGrace;
};

LINK_ENTITY_TO_CLASS(npc_proj_psy, CProjectilePsy);

BEGIN_DATADESC(CProjectilePsy)

END_DATADESC()