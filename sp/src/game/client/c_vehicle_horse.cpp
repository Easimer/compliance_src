#include "cbase.h"
#include "c_prop_vehicle.h"
#include "c_basehlplayer.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C_PropHorse : public C_PropVehicleDriveable
{
	DECLARE_CLASS(C_PropHorse, C_PropVehicleDriveable);

public:

	DECLARE_CLIENTCLASS();
	DECLARE_INTERPOLATION();
	DECLARE_DATADESC();

	C_PropHorse();
	void OnEnteredVehicle(C_BasePlayer *pPlayer);
private:
	Vector		m_vecLastEyePos;
	Vector		m_vecLastEyeTarget;
	Vector		m_vecEyeSpeed;
	float			m_flUpdateTime;
	Vector			m_vecPhysVelocity;
};

IMPLEMENT_CLIENTCLASS_DT(C_PropHorse, DT_PropHorse, CPropHorse)
RecvPropVector(RECVINFO(m_vecPhysVelocity)),
END_RECV_TABLE()
BEGIN_DATADESC(C_PropHorse)
DEFINE_FIELD(m_vecLastEyePos, FIELD_POSITION_VECTOR),
DEFINE_FIELD(m_vecLastEyeTarget, FIELD_POSITION_VECTOR),
DEFINE_FIELD(m_vecEyeSpeed, FIELD_VECTOR),
//DEFINE_FIELD( m_flViewAngleDeltaTime, FIELD_FLOAT ),
END_DATADESC()

C_PropHorse::C_PropHorse()
{
	m_vecEyeSpeed.Init();
}

void C_PropHorse::OnEnteredVehicle(C_BasePlayer *pPlayer)
{
	int eyeAttachmentIndex = LookupAttachment("vehicle_driver_eyes");
	Vector vehicleEyeOrigin;
	QAngle vehicleEyeAngles;
	GetAttachment(eyeAttachmentIndex, vehicleEyeOrigin, vehicleEyeAngles);

	m_vecLastEyeTarget = vehicleEyeOrigin;
	m_vecLastEyePos = vehicleEyeOrigin;
	m_vecEyeSpeed = vec3_origin;
}