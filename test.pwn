#include <a_samp>
#include <core>
#include <float>
#include "../../route-tracing.inc"

main()
{
	print("Running: route-tracing Server");
}

public OnPlayerConnect(playerid)
{
	GameTextForPlayer(playerid,"~w~route-tracing", 5000, 5);
	return 1;
}

public OnPlayerDeath(playerid, killerid, reason)
{
	SendDeathMessage(killerid, playerid, reason);
 	return 1;
}

public OnPlayerRequestClass(playerid, classid)
{
    SetSpawnInfo(playerid, 0, 167, 1589.339843, -1054.775756, 23.633340, 90.0, -1, -1, -1, -1, -1, -1);
	return 0;
}


public OnPlayerCommandText(playerid, cmdtext[])
{
	if (strcmp("/mp5", cmdtext, true, 10) == 0)
	{
		GivePlayerWeapon(playerid, 29, 100);
		return 1;
	}

	if (strcmp("/testveh", cmdtext, true, 10) == 0)
	{
		CreateVehicle(411, 1589.339843, -1054.775756, 23.633340, 90.0, -1, -1, 0, 0);
		return 1;
	}

	if(strcmp("/red", cmdtext, true, 10) == 0)
	{
		SetTracingColor(playerid, 0xEE3636FF);
		return 1;
	}

	if(strcmp("/green", cmdtext, true, 10) == 0)
	{
		SetTracingColor(playerid, 0x51D14EFF);
		return 1;
	}

	if(strcmp("/yellow", cmdtext, true, 10) == 0)
	{
		SetTracingColor(playerid, 0xF9D632FF);
		return 1;
	}
	return 0;
}

public OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
	StartTracing(playerid, fX, fY, fZ);
	return 1;
}

public OnGameModeInit()
{
	SetGameModeText("route-tracing");
	UsePlayerPedAnims();
	return 1;
}