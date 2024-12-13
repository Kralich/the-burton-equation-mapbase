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

DEFINE_FIELD(m_flEndTime, FIELD_FLOAT),

DEFINE_FIELD(pPlayer, FIELD_CLASSPTR),
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

	Precache();

	SetModel( STRING( GetModelName() ) );

	SetRenderMode(kRenderTransColor);
	vIntial = GetAbsOrigin();
	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_NOT_SOLID | FSOLID_TRIGGER);
	SetCollisionBoundsFromModel();
	//SetCollisionGroup(COLLISION_GROUP_INTERACTIVE);
	m_flEndTime = -1;
	SetTouch(&CPowerup::OnTouchPowerup);
}

void CPowerup::Precache( void )
{
	BaseClass::Precache();
	PrecacheScriptSound( "Powerup.Health_Pickup" );
	PrecacheScriptSound( "Powerup.Damage_Pickup" );
	PrecacheScriptSound( "Powerup.Damage_Countdown" );
	PrecacheScriptSound( "Powerup.Damage_CountdownFinal" );
	PrecacheScriptSound( "Powerup.Damage_End" );

	switch (iEffect)
	{
	case HASTE:
		SetModelName( AllocPooledString( "models/items/powerups/Goldenshoes.mdl" ) );
		break;
	case DAMAGE:
		SetModelName( AllocPooledString( "models/items/powerups/Goldengun.mdl" ) );
		break;
	case HEALTH:
		SetModelName( AllocPooledString( "models/items/powerups/Goldenhat.mdl" ) );
		break;
	};

	PrecacheModel( STRING( GetModelName() ) );
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
		player->EmitSound( "Powerup.Damage_Pickup" );
		player->bInDamageEffect = true;
		break;
	case HEALTH:
		player->EmitSound("Powerup.Health_Pickup");
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
	m_flEndTime = gpGlobals->curtime + flPowerupDuration;
	if (iEffect == DAMAGE)
	{
		// damage powerup has alternative behaviour
		RegisterThinkContext( "SoundContext" );
		RegisterThinkContext( "RemoveEffectContext" );
		SetContextThink( &CPowerup::PlaySound, gpGlobals->curtime + 1, "SoundContext" );
		SetContextThink( &CPowerup::RemoveEffect, m_flEndTime, "RemoveEffectContext" );
	}
	else
	{
		SetThink( &CPowerup::RemoveEffect );
		SetNextThink( m_flEndTime );
	}
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
		pPlayer->EmitSound( "Powerup.Damage_End" );
		break;
	}
	UTIL_Remove(this);
}

void CPowerup::PlaySound()
{
	if (pPlayer->bInDamageEffect)
	{
		if (gpGlobals->curtime > m_flEndTime - 5 && gpGlobals->curtime < m_flEndTime)
		{
			// about to end
			pPlayer->EmitSound( "Powerup.Damage_Countdown" );
		}
		else
		{
			pPlayer->EmitSound( "Powerup.Damage_CountdownFinal" );
		}
	}
	
	SetNextThink( gpGlobals->curtime + 1, "SoundContext" );
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