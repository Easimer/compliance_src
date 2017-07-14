#include "cbase.h"
#include "materialsystem/IMaterialProxy.h"
#include "materialsystem/IMaterialVar.h"
#include "toolframework_client.h"
#include "basehlcombatweapon_shared.h"

class CWeaponMatProxy : public IMaterialProxy {
public:
	DECLARE_CLASS_NOBASE(CWeaponMatProxy);
	DECLARE_CLIENTCLASS_NOBASE();
	bool Init(IMaterial* pMaterial, KeyValues* pKeyValues);
	void OnBind(void* pC_BaseEntity);
	void Release();
	IMaterial* GetMaterial();
protected:
	int m_iCustomColor;
private:
	IMaterialVar *m_ColorVar;
	int m_nMaxSkins;
};

BEGIN_RECV_TABLE_NOBASE(CWeaponMatProxy, DT_WeaponColor)
RecvPropInt(RECVINFO(m_iCustomColor)),
END_RECV_TABLE()

EXPOSE_INTERFACE(CWeaponMatProxy, IMaterialProxy, "WeaponColor" IMATERIAL_PROXY_INTERFACE_VERSION);

bool CWeaponMatProxy::Init(IMaterial* pMaterial, KeyValues* pKeyValues)
{
	bool foundVar;
	m_ColorVar = pMaterial->FindVar("$frame", &foundVar, false);
	m_nMaxSkins = pKeyValues->GetInt("max_skins", 1);
	if (m_nMaxSkins == 1)
	{
		DevMsg("WeaponMatProxy applied, yet the material only has 1 skin\n");
	}
	return foundVar;
}

void CWeaponMatProxy::Release()
{

}

IMaterial* CWeaponMatProxy::GetMaterial()
{
	return m_ColorVar->GetOwningMaterial();
}

void CWeaponMatProxy::OnBind(void* pC_BaseEntity)
{
	if (m_ColorVar)
	{
		auto pPlayer = C_BasePlayer::GetLocalPlayer();
		if (!pPlayer)
			return;
		auto pWeapon = pPlayer->GetActiveWeapon();
		if (!pWeapon)
			return;
		auto pHLWeapon = dynamic_cast<C_BaseHLCombatWeapon*>(pWeapon);
		if (!pHLWeapon)
			return;
		int i = pHLWeapon->m_iCustomColor.Get();
		if (i >= m_nMaxSkins)
			i = 0;
		m_ColorVar->SetIntValue(i);
	}
	if (ToolsEnabled())
		ToolFramework_RecordMaterialParams(GetMaterial());
}