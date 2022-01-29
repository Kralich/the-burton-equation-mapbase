#include <cbase.h>
#include <tbe/tbe_powerup.h>
#include <convar.h>



#ifdef CLIENT_DLL
LINK_ENTITY_TO_CLASS( info_powerup_spawn, C_Powerup);
PRECACHE_REGISTER(C_Powerup);
BEGIN_DATADESC(C_Powerup)
#else
LINK_ENTITY_TO_CLASS(info_powerup_spawn, CPowerup);
PRECACHE_REGISTER(CPowerup);
BEGIN_DATADESC(CPowerup)
DEFINE_THINKFUNC(RemoveEffect),
DEFINE_OUTPUT(onPickup, "OnPickup"),
DEFINE_ENTITYFUNC(OnTouchPowerup),
DEFINE_KEYFIELD(iEffect, FIELD_INTEGER, "effect"),
DEFINE_KEYFIELD(flPowerupDuration, FIELD_FLOAT, "Duration"),
#endif


END_DATADESC()




#ifndef CLIENT_DLL
ConVarRef HastePowerupMult = ConVarRef("tbe_powerup_haste_multiplier");
//dmg convar in takedamageinfo.cpp
ConVar Healthpowerupset("tbe_powerup_health_sethealth", "250", 0, "how much health the health powerup sets your health to");

IMPLEMENT_SERVERCLASS_ST(CPowerup, DT_PowerUp)
SendPropVector(SENDINFO(vIntial), -1, SPROP_COORD | SPROP_CHANGES_OFTEN, 0.0f, HIGH_DEFAULT, SendProxy_Origin),
END_SEND_TABLE();

void CPowerup::Spawn()
{
	BaseClass::Spawn();
	PrecacheScriptSound("SuitRecharge.Start");
	PrecacheScriptSound("AlyxEMP.Charge");
	PrecacheScriptSound("WallHealth.Start");


	switch (iEffect)
	{
	case HASTE:
		SetModel("models/items/powerups/Goldenshoes.mdl");
		break;
	case DAMAGE:
		SetModel("models/items/powerups/Goldengun.mdl");
		break;
	case HEALTH:
		SetModel("models/items/powerups/Goldenhat.mdl");
		break;
	};
	SetRenderMode(kRenderTransColor);
	vIntial = GetAbsOrigin();
	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_NOT_SOLID | FSOLID_TRIGGER);
	SetCollisionBoundsFromModel();
	//SetCollisionGroup(COLLISION_GROUP_INTERACTIVE);
	SetTouch(&CPowerup::OnTouchPowerup);
}
extern ConVar  hl2_sprintspeed;
extern ConVar  hl2_normspeed;




void CPowerup::OnTouchPowerup(CBaseEntity *pOther)
{
	//BaseClass::Touch(pOther);
	if(!pOther->IsPlayer())
		return;
	if (!cantouch)
		return;
	CBasePlayer *player = ToBasePlayer(pOther);
	if (!player)
		return;
	switch (iEffect)
	{
	case HASTE:
		
		player->EmitSound("SuitRecharge.Start");
		player->bInHasteEffect = true;
		player->SetMaxSpeed(player->GetPlayerMaxSpeed() * HastePowerupMult.GetFloat());
		break;
	case DAMAGE:
		
		player->EmitSound("AlyxEMP.Charge");
		player->bInDamageEffect = true;
		break;
	case HEALTH:
		player->EmitSound("WallHealth.Start");
		player->SetHealth(Healthpowerupset.GetInt());
		break;
	}
	onPickup.FireOutput(this, this, 0);
	if (iEffect == HEALTH)
	{
		UTIL_Remove(this);
		return;
	}
	pPlayer = player;
	SetRenderColorA(0);
	SetThink(&CPowerup::RemoveEffect);
	SetNextThink(gpGlobals->curtime + flPowerupDuration);
	cantouch = false;
}

void CPowerup::RemoveEffect()
{
	switch (iEffect)
	{
	case HASTE:
		pPlayer->bInHasteEffect = false;
		pPlayer->SetMaxSpeed(hl2_normspeed.GetFloat());
		break;
	case DAMAGE:
		pPlayer->bInDamageEffect = false;
		break;
	}
	UTIL_Remove(this);
}
#else
IMPLEMENT_CLIENTCLASS_DT(C_Powerup, DT_PowerUp, CPowerup)
RecvPropVector(RECVINFO(vIntial)),
END_RECV_TABLE();

void C_Powerup::Spawn()
{
	qaIntial = GetLocalAngles();
//	vIntial = GetAbsOrigin();

}

void C_Powerup::OnDataChanged(DataUpdateType_t type)
{
	BaseClass::OnDataChanged(type);

	if ((type == DATA_UPDATE_CREATED))
	{
		SetNextClientThink(CLIENT_THINK_ALWAYS);
	}
}

void C_Powerup::ClientThink()
{
	Quake3Bob(this, vIntial);
	Quake3Rotate(this, qaIntial);
}


#endif