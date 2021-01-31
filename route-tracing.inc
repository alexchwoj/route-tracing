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
#include <Pawn.RakNet>

#define ROUTE_TRACING_VERSION "0.0.2"

// Headers
forward OnRouteCalculated(Path:pathid, playerid);

enum player_gps
{
	bool:gps_activated,
	gps_color,
	gps_timer
}
static GPS_PLAYER[MAX_PLAYERS][player_gps];
static GPS_PLAYER_GANGZONES[MAX_PLAYERS][50];
static Float:GPS_PLAYER_POSITION[MAX_PLAYERS][3];

// Tracing
stock StartTracing(playerid, Float:x, Float:y, Float:z)
{
	CancelTracing(playerid);
	GPS_PLAYER[playerid][gps_timer] = SetTimerEx("UpdatePlayerTracing", 500, true, "ifff", playerid, x, y, z);
	return 1;
}

stock CancelTracing(playerid)
{
	if (GPS_PLAYER[playerid][gps_activated])
	{
		GPS_PLAYER[playerid][gps_activated] = false;
		KillTimer(GPS_PLAYER[playerid][gps_timer]);

		for(new i; i < 50; i ++)
		{
			if (GPS_PLAYER_GANGZONES[playerid][i] != -1)
			{
			    GangZoneDestroy(GPS_PLAYER_GANGZONES[playerid][i]);
			    GPS_PLAYER_GANGZONES[playerid][i] = -1;
			}
		}
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

// Updating
stock Float:ReturnRouteDistance(Float:init_x, Float:init_y, Float:final_x, Float:final_y)
{
	new
		Float:dis_x = floatpower( floatsub( init_x, final_x ), 2.0 ),
		Float:dis_y = floatpower( floatsub( init_y, final_y ), 2.0 )
	;

	return floatsqroot( floatadd( dis_x, dis_y ) );
}

forward UpdatePlayerTracing(playerid, Float:x, Float:y, Float:z);
public UpdatePlayerTracing(playerid, Float:x, Float:y, Float:z)
{
	new
		Float:pos[3],
		Float:init_distance,
		MapNode:point[2]
	;

	if (GPS_PLAYER[playerid][gps_activated])
	{
		if (IsPlayerInRangeOfPoint(playerid, 3.5, GPS_PLAYER_POSITION[playerid][0], GPS_PLAYER_POSITION[playerid][1], GPS_PLAYER_POSITION[playerid][2])) return 0;
	}
	else
	{
		GPS_PLAYER[playerid][gps_activated] = true;
	}

	GetPlayerPos(playerid, pos[0], pos[1], pos[2]);

	GPS_PLAYER_POSITION[playerid][0] = pos[0];
	GPS_PLAYER_POSITION[playerid][1] = pos[1];
	GPS_PLAYER_POSITION[playerid][2] = pos[2];

	init_distance = ReturnRouteDistance(x, y, pos[0], pos[1]);

	if (init_distance <= 2.0) return CancelTracing(playerid);
	if (GetClosestMapNodeToPoint(pos[0], pos[1], pos[2], point[0]) != 0) return 0;
	if (GetClosestMapNodeToPoint(x, y, z, point[1])) return 0;
	if (FindPathThreaded(point[0], point[1], "OnRouteCalculated", "i", playerid) != GPS_ERROR_NONE)
	{
		print("[route-tracing] UpdatePlayerTracing ha refenciado un nodo invalido.");
	}
	return 1;
}

public OnRouteCalculated(Path:pathid, playerid)
{
	if (!IsValidPath(pathid)) return 0;

	// Destroy old gangzones
	for(new i; i < 50; i ++)
	{
		if (GPS_PLAYER_GANGZONES[playerid][i] != -1)
		{
			GangZoneDestroy(GPS_PLAYER_GANGZONES[playerid][i]);
			GPS_PLAYER_GANGZONES[playerid][i] = -1;
		}
	}

	new
		index,
		size,
		MapNode:nodeid,
		Float:length,
		Float:pos[6]
	;

	GetPathSize(pathid, size);
	GetPathLength(pathid, length);

	if (size == 1) return CancelTracing(playerid);

	GetPlayerPos(playerid, pos[3], pos[4], pos[5]);
	GetClosestMapNodeToPoint(pos[3], pos[4], pos[5], nodeid);
	GetMapNodePos(nodeid, pos[3], pos[4], pos[5]);

	for(new x = 0; x < 50; x ++)
	{
		GetPathNode(pathid, x, nodeid);
		GetPathNodeIndex(pathid, nodeid, index);
		GetMapNodePos(nodeid, pos[0], pos[1], pos[2]);

		if (x == index)
		{
			new node_size = floatround( ReturnRouteDistance( pos[3], pos[4], pos[0], pos[1] ) / 12.5 );

			for(new i = 1; i <= node_size; i++)
			{
				new Float:node_pos[2];

				if (i != 0)
				{
					node_pos[0] = pos[3] + (((pos[0] - pos[3]) / node_size) * i);
					node_pos[1] = pos[4] + (((pos[1] - pos[4]) / node_size) * i);
				}
				else
				{
					node_pos[0] = pos[3];
					node_pos[1] = pos[4];
				}

				new slot;
				while(slot <= 50)
				{
					if (slot == 50)
					{
						slot = -1;
						break;
					}

					if (GPS_PLAYER_GANGZONES[playerid][slot] == -1)
					{
						break;
					}
					slot ++;
				}

				if (slot == -1) return 1;

				GPS_PLAYER_GANGZONES[playerid][slot] = GangZoneCreate(
					node_pos[0] - 12.5,
					node_pos[1] - 12.5,
					node_pos[0] + 12.5,
					node_pos[1] + 12.5
				);

				GangZoneShowForPlayer(playerid, GPS_PLAYER_GANGZONES[playerid][slot], GPS_PLAYER[playerid][gps_color]);
			}
		}

		pos[3] = pos[0] + 0.5;
		pos[4] = pos[1] + 0.5;
	}
    return 1;
}

// Hooks
public OnGameModeInit()
{
	printf("[-] route-tracing v"ROUTE_TRACING_VERSION" by Atom initialized successfully [-]");

	#if defined RT_OnGameModeInit
		return RT_OnGameModeInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

#define OnGameModeInit RT_OnGameModeInit
#if defined RT_OnGameModeInit
	forward RT_OnGameModeInit();
#endif

public OnPlayerConnect(playerid)
{
	GPS_PLAYER[playerid][gps_color] = 0x9C0DE6FF;

	#if defined RT_OnPlayerConnect
		return RT_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect RT_OnPlayerConnect
#if defined RT_OnPlayerConnect
	forward RT_OnPlayerConnect(playerid);
#endif

public OnPlayerDisconnect(playerid, reason)
{
	CancelTracing(playerid);

	#if defined RT_OnPlayerDisconnect
		return RT_OnPlayerDisconnect(playerid, reason);
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
	forward RT_OnPlayerDisconnect(playerid, reason);
#endif
