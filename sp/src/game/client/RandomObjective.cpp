#include "cbase.h"
#include "memdbgon.h"

class C_RandomObjective : public C_BaseEntity {
public:
	DECLARE_CLASS(C_RandomObjective, C_BaseEntity);
	DECLARE_CLIENTCLASS();
	C_RandomObjective();
private:
	bool m_bAllowItem;
	bool m_bAllowEnemy;
};

IMPLEMENT_CLIENTCLASS_DT(C_RandomObjective, DT_RandomObjective, CRandomObjective)
RecvPropBool(RECVINFO(m_bAllowItem)),
RecvPropBool(RECVINFO(m_bAllowEnemy)),
END_RECV_TABLE()

C_RandomObjective::C_RandomObjective()
{
	m_bAllowItem = false;
	m_bAllowEnemy = false;
}