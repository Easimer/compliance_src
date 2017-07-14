#pragma once

#include "cbase.h"

class CGravityVortexController : public CBaseEntity
{
	DECLARE_CLASS(CGravityVortexController, CBaseEntity);
	DECLARE_DATADESC();

public:

	CGravityVortexController(void) : m_flEndTime(0.0f), m_flRadius(256), m_flStrength(256), m_flMass(0.0f) {}
	float	GetConsumedMass(void) const;

	static CGravityVortexController *Create(const Vector &origin, float radius, float strength, float duration);

private:

	void	ConsumeEntity(CBaseEntity *pEnt);
	void	PullPlayersInRange(void);
	bool	KillNPCInRange(CBaseEntity *pVictim, IPhysicsObject **pPhysObj);
	void	CreateDenseBall(void);
	void	PullThink(void);
public:
	void	StartPull(const Vector &origin, float radius, float strength, float duration);
private:

	float	m_flMass;		// Mass consumed by the vortex
	float	m_flEndTime;	// Time when the vortex will stop functioning
	float	m_flRadius;		// Area of effect for the vortex
	float	m_flStrength;	// Pulling strength of the vortex
};