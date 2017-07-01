#include "cbase.h"
#include "PhaseAnchor.h"

LINK_ENTITY_TO_CLASS(info_phase_anchor, CPhaseAnchor);

BEGIN_DATADESC(CPhaseAnchor)
DEFINE_KEYFIELD(m_sPair, FIELD_STRING, "pair"),
DEFINE_OUTPUT(m_OnPhasing, "OnPhasing"),
END_DATADESC()

CPhaseAnchor::CPhaseAnchor()
{
	
}

void CPhaseAnchor::Spawn()
{
	m_pPair = (CPhaseAnchor*)gEntList.FindEntityGeneric(NULL, STRING(m_sPair), this);
	if (m_pPair == NULL)
	{
		Warning("Anchor pair of %s was not found\n", GetEntityName().ToCStr());
	}
}

CPhaseAnchor* CPhaseAnchor::GetPair()
{
	return m_pPair;
}

void CPhaseAnchor::FirePhaseEvent(CBaseEntity* pActivator)
{
	m_OnPhasing.FireOutput(pActivator, this);
}
