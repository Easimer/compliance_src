#pragma once
#include "triggers.h"
#include "PhaseAnchor.h"

class CPhaseField : public CBaseTrigger
{
public:
	DECLARE_CLASS(CPhaseField, CBaseTrigger);
	DECLARE_DATADESC();

	void Spawn();

	void StartTouch(CBaseEntity *pOther);
	void EndTouch(CBaseEntity *pOther);

	CPhaseAnchor* GetAnchor();
	bool IsUnstable();

	void InputUnstabilize(inputdata_t &inputdata);
	void InputStabilize(inputdata_t &inputdata);

	float GetLastPhaseTime();
	void Phased();

private:
	CPhaseAnchor* m_pAnchor;
	string_t m_sAnchor;
	bool m_bUnstable;

	float m_fLastPhase;
};