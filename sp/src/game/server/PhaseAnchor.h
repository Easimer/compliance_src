#include "cbase.h"

class CPhaseAnchor : public CLogicalEntity
{
public:
	DECLARE_CLASS(CPhaseAnchor, CLogicalEntity);

	DECLARE_DATADESC();
	CPhaseAnchor();
	void Spawn();
	CPhaseAnchor* GetPair();

	void FirePhaseEvent(CBaseEntity* pActivator);
private:
	string_t m_sPair;
	CPhaseAnchor* m_pPair;
	COutputEvent m_OnPhasing;
};
