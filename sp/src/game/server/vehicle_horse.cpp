#include "cbase.h"
#include "vehicle_base.h"
#include "soundent.h"
#include "vphysics/constraints.h"

#include "tier0/memdbgon.h"

#define HORSE_MODEL "models/dog.mdl"

class CPropHorse : public CPropVehicleDriveable
{
	DECLARE_CLASS(CPropHorse, CPropVehicleDriveable);
public:
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	void Think();
	void Precache();
	void Spawn();

	void EnterVehicle(CBaseCombatCharacter* pPlayer);
	void ExitVehicle(int nRole);

	int OnTakeDamage(const CTakeDamageInfo &info);

	void DriveVehicle(float flFrameTime, CUserCmd *ucmd, int iButtonsDown, int iButtonsReleased);

	void CreateDangerSounds();
	void ProcessMovement(CBasePlayer *pPlayer, CMoveData *pMoveData);
private:
	void CreateAntiFlipConstraint();
private:
	Vector			m_vecLastEyePos;
	Vector			m_vecLastEyeTarget;
	Vector			m_vecEyeSpeed;
	IPhysicsConstraint *m_pAntiFlipConstraint;	// A ragdoll constraint that prevents us from flipping.

	CNetworkVar(Vector, m_vecPhysVelocity);
};

IMPLEMENT_SERVERCLASS_ST(CPropHorse, DT_PropHorse)
SendPropVector(SENDINFO(m_vecPhysVelocity)),
END_SEND_TABLE();

LINK_ENTITY_TO_CLASS(prop_vehicle_horse, CPropHorse);

BEGIN_DATADESC(CPropHorse)
	DEFINE_FIELD(m_vecLastEyePos, FIELD_POSITION_VECTOR),
	DEFINE_FIELD(m_vecLastEyeTarget, FIELD_POSITION_VECTOR),
	DEFINE_FIELD(m_vecEyeSpeed, FIELD_VECTOR),
END_DATADESC()

void CPropHorse::Think()
{
	BaseClass::Think();
}

void CPropHorse::EnterVehicle(CBaseCombatCharacter* pPlayer)
{
	BaseClass::EnterVehicle(pPlayer);
	SetNavIgnore();
	m_VehiclePhysics.TurnOn();
}

void CPropHorse::Precache()
{
	//PrecacheModel(HORSE_MODEL);
	BaseClass::Precache();
}

void CPropHorse::Spawn()
{
	//SetModel(HORSE_MODEL);
	SetVehicleType(VEHICLE_TYPE_AIRBOAT_RAYCAST);
	SetCollisionGroup(COLLISION_GROUP_VEHICLE);
	BaseClass::Spawn();

	//AddSolidFlags(FSOLID_NOT_STANDABLE);
	SetAnimatedEveryTick(true);
	m_takedamage = DAMAGE_EVENTS_ONLY;

	IPhysicsObject *pPhysHorse = VPhysicsGetObject();
	if (pPhysHorse)
	{
		pPhysHorse->SetBuoyancyRatio(0.0f);
		PhysSetGameFlags(pPhysHorse, FVPHYSICS_HEAVY_OBJECT);
	}
	CreateAntiFlipConstraint();
}

void CPropHorse::ExitVehicle(int nRole)
{
	CBaseEntity *pDriver = GetDriver();

	//EnablePlayerBlocker( true );

	BaseClass::ExitVehicle(nRole);

	if (!pDriver)
		return;
	ClearNavIgnore();
	m_VehiclePhysics.TurnOff();
}

int CPropHorse::OnTakeDamage(const CTakeDamageInfo &info)
{
	// Do scaled up physics damage to the airboat
	CTakeDamageInfo physDmg = info;
	physDmg.ScaleDamage(5);
	if (physDmg.GetDamageType() & DMG_BLAST)
	{
		physDmg.SetDamageForce(info.GetDamageForce() * 10);
	}
	VPhysicsTakeDamage(physDmg);

	// Check to do damage to driver
	if (m_hPlayer != NULL)
	{
		// Don't pass along physics damage
		if (info.GetDamageType() & (DMG_CRUSH | DMG_RADIATION))
			return 0;

		// Take the damage (strip out the DMG_BLAST)
		CTakeDamageInfo playerDmg = info;

		// Mark that we're passing it to the player so the base player accepts the damage
		playerDmg.SetDamageType(info.GetDamageType() | DMG_VEHICLE);

		// Deal the damage to the passenger
		m_hPlayer->TakeDamage(playerDmg);
	}

	return 0;
}

void CPropHorse::DriveVehicle(float flFrameTime, CUserCmd *ucmd, int iButtonsDown, int iButtonsReleased)
{
	m_VehiclePhysics.UpdateDriverControls(ucmd, TICK_INTERVAL);

	// Save this data.
	m_flThrottle = m_VehiclePhysics.GetThrottle();
	m_nSpeed = m_VehiclePhysics.GetSpeed();
	m_nRPM = m_VehiclePhysics.GetRPM();
}

void CPropHorse::ProcessMovement(CBasePlayer *pPlayer, CMoveData *pMoveData)
{
	BaseClass::ProcessMovement(pPlayer, pMoveData);

	if (gpGlobals->frametime != 0)
	{
		CreateDangerSounds();
		// Play a sound around us to make NPCs pay attention to us
		if (m_VehiclePhysics.GetThrottle() > 0)
		{
			CSoundEnt::InsertSound(SOUND_PLAYER_VEHICLE, pPlayer->GetAbsOrigin(), 3500, 0.1f, pPlayer, SOUNDENT_CHANNEL_REPEATED_PHYSICS_DANGER);
		}
	}

	Vector vecVelocityWorld;
	GetVelocity(&vecVelocityWorld, NULL);
	Vector vecVelocityLocal;
	WorldToEntitySpace(GetAbsOrigin() + vecVelocityWorld, &vecVelocityLocal);

	m_vecPhysVelocity = vecVelocityLocal;
}

void CPropHorse::CreateDangerSounds()
{
	QAngle vehicleAngles = GetLocalAngles();
	Vector vecStart = GetAbsOrigin();
	Vector vecDir, vecRight;

	GetVectors(&vecDir, &vecRight, NULL);

	const float soundDuration = 0.25;
	float speed = m_VehiclePhysics.GetHLSpeed();

	// Make danger sounds ahead of the vehicle
	if (fabs(speed) > 120)
	{
		Vector	vecSpot;

		float steering = m_VehiclePhysics.GetSteering();
		if (steering != 0)
		{
			if (speed > 0)
			{
				vecDir += vecRight * steering * 0.5;
			}
			else
			{
				vecDir -= vecRight * steering * 0.5;
			}
			VectorNormalize(vecDir);
		}

		const float radius = speed * 0.4;

		// 0.7 seconds ahead
		vecSpot = vecStart + vecDir * (speed * 0.7f);
		CSoundEnt::InsertSound(SOUND_DANGER, vecSpot, radius, soundDuration, this, SOUNDENT_CHANNEL_REPEATED_DANGER);
		CSoundEnt::InsertSound(SOUND_PHYSICS_DANGER, vecSpot, radius, soundDuration, this, SOUNDENT_CHANNEL_REPEATED_PHYSICS_DANGER);
		//NDebugOverlay::Box(vecSpot, Vector(-radius,-radius,-radius),Vector(radius,radius,radius), 255, 0, 255, 0, soundDuration);
	}
}

void CPropHorse::CreateAntiFlipConstraint()
{
	constraint_ragdollparams_t ragdoll;
	ragdoll.Defaults();

	// Don't prevent the boat from moving, just flipping.
	ragdoll.onlyAngularLimits = true;

	// Put the ragdoll constraint in the space of the airboat.
	SetIdentityMatrix(ragdoll.constraintToAttached);
	BuildObjectRelativeXform(g_PhysWorldObject, VPhysicsGetObject(), ragdoll.constraintToReference);

	ragdoll.axes[0].minRotation = -100;
	ragdoll.axes[0].maxRotation = 100;
	ragdoll.axes[1].minRotation = -100;
	ragdoll.axes[1].maxRotation = 100;
	ragdoll.axes[2].minRotation = -180;
	ragdoll.axes[2].maxRotation = 180;

	m_pAntiFlipConstraint = physenv->CreateRagdollConstraint(g_PhysWorldObject, VPhysicsGetObject(), NULL, ragdoll);

	//NDebugOverlay::Cross3DOriented( ragdoll.constraintToReference, 128, 255, true, 100 );
}
