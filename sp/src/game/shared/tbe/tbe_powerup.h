#include <cbase.h>
#ifndef CLIENT_DLL
class CPowerup : public CBaseAnimating
{
public:
	void Spawn();
	virtual void Precache();
	void RemoveEffect();
	void PlaySound();
	CBasePlayer* pPlayer;

	int iEffect;
	float flPowerupDuration;
	float m_flEndTime;
	bool cantouch = true;

	COutputEvent onPickup;

	int UpdateTransmitState()	// always send to all clients
	{
		return SetTransmitState(FL_EDICT_ALWAYS);
	}

	
	DECLARE_CLASS(CPowerup, CBaseAnimating);
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
private:
	void OnTouchPowerup(CBaseEntity* pOther);
	CNetworkVector(vIntial);
};
#else
class C_Powerup : public C_BaseAnimating
{
public:
	void Spawn();
	void OnDataChanged(DataUpdateType_t type);
	void ClientThink(void);
	int iEffect;
	//float flPowerupDuration;
	Vector vIntial;
	QAngle qaIntial;

	
	DECLARE_CLASS(C_Powerup, CBaseAnimating);
	DECLARE_DATADESC();
	DECLARE_CLIENTCLASS();

	
};
#endif



enum PowerUpEFfect
{
	HASTE =1,
	DAMAGE,
	HEALTH
};


//taken from dmc:r
#define ITEM_ROTATION_RATE	( 360.0f / 4.0f )
#define ITEM_BOB_DISTANCE 4.0

inline void Quake3Bob(CBaseEntity* pEntity, Vector vecBaseOrigin)
{
	float bobscale = 4.0 + (pEntity->entindex() * 0.001);
	float boboffset = (cos((gpGlobals->curtime + 1000) * bobscale) * ITEM_BOB_DISTANCE) + ITEM_BOB_DISTANCE;

	pEntity->SetLocalOrigin(vecBaseOrigin + Vector{ 0.0, 0.0, boboffset });
}

inline void Quake3Rotate(CBaseEntity* pEntity, QAngle& vecAngles)
{
	vecAngles.y += ITEM_ROTATION_RATE * gpGlobals->frametime;
	if (vecAngles.y >= 360)
		vecAngles.y -= 360;

	pEntity->SetLocalAngles(vecAngles);
}
