#include "cbase.h"
#include "materialsystem/IMaterialProxy.h"
#include "materialsystem/IMaterialVar.h"
#include "toolframework_client.h"
#include "usermessages.h"

// 
// Purpose: Material proxy that changes the chromatic aberration effect's strength
// when the player is hit
//

static bool g_bPlayerHitMsgFuncHooked = false;
static struct {
	float lastupdate;
	float acc;
} g_playerhit;

void __MsgFunc_PlayerHit(bf_read& msg)
{
	int dmgsave = msg.ReadByte();
	int dmgtake = msg.ReadByte();
	int visibledmgbits = msg.ReadLong();
	float dx = msg.ReadFloat();
	float dy = msg.ReadFloat();
	float dz = msg.ReadFloat();

	float add = (float)dmgtake * 10;
	Warning("MsgFunc_PlayerHit! %d %d %d %f %f %f. Added: %f\n", dmgsave, dmgtake, visibledmgbits, dx, dy, dz, add);
	g_playerhit.acc += add;
}

class CPlayerHitMaterialProxy : public IMaterialProxy {
public:
	DECLARE_CLASS_NOBASE(CPlayerHitMaterialProxy);
	DECLARE_CLIENTCLASS_NOBASE();
	bool Init(IMaterial* pMaterial, KeyValues* pKeyValues);
	void OnBind(void* pC_BaseEntity);
	void Release();
	IMaterial* GetMaterial();
private:
	IMaterialVar *m_pScaleVar;
};

BEGIN_RECV_TABLE_NOBASE(CPlayerHitMaterialProxy, DT_PlayerDamage)
END_RECV_TABLE()

EXPOSE_INTERFACE(CPlayerHitMaterialProxy, IMaterialProxy, "PlayerDamage" IMATERIAL_PROXY_INTERFACE_VERSION);

bool CPlayerHitMaterialProxy::Init(IMaterial* pMaterial, KeyValues* pKeyValues)
{
	bool foundVar;
	m_pScaleVar = pMaterial->FindVar("$scale", &foundVar, true);
	if (foundVar)
	{
		Warning("CPlayerHitMaterialProxy::Init: Found var\n");
		if (!g_bPlayerHitMsgFuncHooked)
		{
			usermessages->HookMessage("Damage", __MsgFunc_PlayerHit);
			g_bPlayerHitMsgFuncHooked = true;
			return true;
		}
	}
	return false;
}

void CPlayerHitMaterialProxy::Release()
{

}

IMaterial* CPlayerHitMaterialProxy::GetMaterial()
{
	return m_pScaleVar->GetOwningMaterial();
}

void CPlayerHitMaterialProxy::OnBind(void* pC_BaseEntity)
{
	if (ToolsEnabled())
		ToolFramework_RecordMaterialParams(GetMaterial());
	if (g_playerhit.acc > 0)
	{
		//g_playerhit.acc -= (gpGlobals->curtime - g_playerhit.lastupdate) * 5;
		if (g_playerhit.acc < 0)
			g_playerhit.acc = 0;
	}
	//Warning("PlayerHit-bound: %u\n", m_pScaleVar);
	if (m_pScaleVar)
	{
		m_pScaleVar->SetFloatValue(g_playerhit.acc);
		//Warning("PlayerHit-acc: %f\n", g_playerhit.acc);
	}
}