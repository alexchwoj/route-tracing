/*
* route-tracing
* Developed by Atom
* GitHub: github.com/RealAtom
*/

#if defined _ROUTE_TRACING_INCLUDED
	#endinput
#endif
#define _ROUTE_TRACING_INCLUDED

#include <a_samp>
#include <GPS>

/* Core */
enum player_gps
{
	bool:gps_activated,
	gps_color,
	gps_timer
}
new GPS_PLAYER[MAX_PLAYERS][player_gps];

/* Tracing */
stock StartTracing(playerid, Float:x, Float:y, Float:z)
{
	GPS_PLAYER[playerid][gps_activated] = true;
	return 1;
}

stock CancelTracing(playerid)
{
	if (GPS_PLAYER[playerid][gps_activated] == true)
	{
		GPS_PLAYER[playerid][gps_activated] = false;
	}
	return 1;
}

stock SetTracingColor(playerid, color)
{
	GPS_PLAYER[playerid][gps_color] = color;
	return 1;
}

stock GetTracingColor(playerid)
{
	return GPS_PLAYER[playerid][gps_color];
}

/* Updating */
forward UpdatePlayerTracing(playerid, Float:x, Float:y, Float:z);
public UpdatePlayerTracing(playerid, Float:x, Float:y, Float:z)
{
	new 
		Float:pos[3],
		MapNode:init,
		MapNode:final
	;
	
	GetPlayerPos(playerid, pos[0], pos[1], pos[3]);
	return 1;
}

/* Reset */
public OnPlayerDisconnect(playerid)
{
	CancelTracing(playerid);

	#if defined RT_OnPlayerDisconnect
		return RT_OnPlayerDisconnect(playerid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect RT_OnPlayerDisconnect
#if defined RT_OnPlayerDisconnect
	forward RT_OnPlayerDisconnect(playerid);
#endif