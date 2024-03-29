/*
* route-tracing
* Developed by alexchwoj
* GitHub: github.com/alexchwoj
* Special thanks: Boorz, Marski, Kristo, Katursis, AmyrAhmady
*/

#if defined ROUTE_TRACING_INCLUDED
	#endinput
#endif
#define ROUTE_TRACING_INCLUDED

// Library
#include <a_samp> // Source: https://sa-mp.com

#if !defined _gps_included
	#include <GPS> // Source: https://github.com/kristoisberg/samp-gps-plugin
#endif

#if !defined PAWNRAKNET_INC_
	#include <Pawn.RakNet> // Source: https://github.com/katursis/Pawn.RakNet
#endif

// Configuration
#define ROUTE_TRACING_VERSION "0.0.2" // Version of route-tracing
#define MAX_ROUTE_GANGZONES 512 // Maximum number of gangzones to use (default: 512)
#define ROUTE_UPDATE_DELAY 1000 // Delay to recalculate the route (default: 1000)
#define MAX_PLAYER_GANGZONES 512 // Size of gangzones array (default: 512)
#define ROUTE_TRACING_DEBUG // This triggers messages about the execution and calculation of the nodes, it is not recommended for the production environment

// Headers
forward OnRouteCalculated(Path:pathid, playerid);

static enum _:player_gps
{
	bool:gps_Activated,
	gps_Color,
	gps_Timer
}

static GPS_PLAYER[MAX_PLAYERS][player_gps];
static GPS_PLAYER_GANGZONES[MAX_PLAYERS][MAX_ROUTE_GANGZONES];
static Float:GPS_PLAYER_POSITION[MAX_PLAYERS][3];

static enum _:player_gangzone
{
	Float:gangzone_MinX,
	Float:gangzone_MinY,
	Float:gangzone_MaxX,
	Float:gangzone_MaxY,
	gangzone_Color,
	gangzone_Flash_Color,
	bool:gangzone_Used,
	bool:gangzone_Shown,
	bool:gangzone_Flashing
}

static PLAYER_GANGZONE[MAX_PLAYERS][MAX_PLAYER_GANGZONES][player_gangzone];

// Gangzone
PlayerGangZoneCreate(playerid, Float:minx, Float:miny, Float:maxx, Float:maxy)
{
	new idx;
	for (new i = 0; i < MAX_PLAYER_GANGZONES; ++i)
	{
		if (!PLAYER_GANGZONE[playerid][i][gangzone_Used]) 
		{
			idx = i;
			break;
		}
	}

	if (idx > MAX_PLAYER_GANGZONES - 1)
		return -1;

	PLAYER_GANGZONE[playerid][idx][gangzone_MinX] = minx;
	PLAYER_GANGZONE[playerid][idx][gangzone_MinY] = miny;
	PLAYER_GANGZONE[playerid][idx][gangzone_MaxX] = maxx;
	PLAYER_GANGZONE[playerid][idx][gangzone_MaxY] = maxy;
	PLAYER_GANGZONE[playerid][idx][gangzone_Color] = 0xFFFFFFAA;
	PLAYER_GANGZONE[playerid][idx][gangzone_Flash_Color] = 0xFF0000AA;
	PLAYER_GANGZONE[playerid][idx][gangzone_Used] = true;
	PLAYER_GANGZONE[playerid][idx][gangzone_Shown] = false;
	PLAYER_GANGZONE[playerid][idx][gangzone_Flashing] = false;
	return idx;
}

PlayerGangZoneShow(playerid, gangzoneid, color = 0xFFFFFFAA)
{
	if (!PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Used] || gangzoneid > MAX_PLAYER_GANGZONES - 1 || gangzoneid < 0)
		return 0;

	PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Color] = color;
	PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Shown] = true;
	new abgr_color = (((color << 16) | color & 0xFF00) << 8) | (((color >>> 16) | color & 0xFF0000) >>> 8);

	new BitStream:bs = BS_New();
		
	BS_WriteValue(
		bs,
		PR_UINT16, 1023 - gangzoneid,
		PR_FLOAT, PLAYER_GANGZONE[playerid][gangzoneid][gangzone_MinX],
		PR_FLOAT, PLAYER_GANGZONE[playerid][gangzoneid][gangzone_MinY],
		PR_FLOAT, PLAYER_GANGZONE[playerid][gangzoneid][gangzone_MaxX],
		PR_FLOAT, PLAYER_GANGZONE[playerid][gangzoneid][gangzone_MaxY],
		PR_UINT32, abgr_color
	);
		
	PR_SendRPC(bs, playerid, 0x6C);
	BS_Delete(bs);
	return 1;
}

PlayerGangZoneHide(playerid, gangzoneid)
{
	if (!PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Used] || 
		!PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Shown] ||
		gangzoneid > MAX_PLAYER_GANGZONES - 1 || gangzoneid < 0) return 0;

	PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Shown] = false;
	new BitStream:bs = BS_New();
		
	BS_WriteValue(
		bs,
		PR_UINT16, 1023 - gangzoneid
	);
		
	PR_SendRPC(bs, playerid, 0x78);
	BS_Delete(bs);
	return 1;
}

/*
* Note:
* PlayerGangZoneFlash and PlayerGangZoneStopFlash they remain as stock since they
* are not used, at least in route-tracing, I leave it in case someone wants to make a profit
*/
stock PlayerGangZoneFlash(playerid, gangzoneid, color = 0xFF0000AA)
{
	if (!PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Used] || gangzoneid > MAX_PLAYER_GANGZONES - 1 || gangzoneid < 0)
		return 0;

	PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Flash_Color]  = color;
	PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Flashing] = true;
	new abgr_color = (((color << 16) | color & 0xFF00) << 8) | (((color >>> 16) | color & 0xFF0000) >>> 8);

	new BitStream:bs = BS_New();
		
	BS_WriteValue(
		bs,
		PR_UINT16, 1023 - gangzoneid,
		PR_UINT32, abgr_color
	);
		
	PR_SendRPC(bs, playerid, 0x79);
	BS_Delete(bs);
	return 1;
}

stock PlayerGangZoneStopFlash(playerid, gangzoneid)
{
	if (!PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Used] ||
		!PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Flashing] ||
		gangzoneid > MAX_PLAYER_GANGZONES - 1 || gangzoneid < 0) return 0;

	PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Flashing] = false;
	new abgr_color = (((color << 16) | color & 0xFF00) << 8) | (((color >>> 16) | color & 0xFF0000) >>> 8);

	new BitStream:bs = BS_New();
		
	BS_WriteValue(
		bs,
		PR_UINT16, 1023 - gangzoneid
	);
		
	PR_SendRPC(bs, playerid, 0x55);
	BS_Delete(bs);
	return 1;
}

PlayerGangZoneDestroy(playerid, gangzoneid)
{
	if (!PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Used] || gangzoneid > MAX_PLAYER_GANGZONES - 1 || gangzoneid < 0)
		return 0;
	
	if (PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Shown])
		PlayerGangZoneHide(playerid, gangzoneid);
	
	PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Flashing] = false;
	PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Used] = false;
	PLAYER_GANGZONE[playerid][gangzoneid][gangzone_MinX] = 0.0;
	PLAYER_GANGZONE[playerid][gangzoneid][gangzone_MinY] = 0.0;
	PLAYER_GANGZONE[playerid][gangzoneid][gangzone_MaxX] = 0.0;
	PLAYER_GANGZONE[playerid][gangzoneid][gangzone_MaxY] = 0.0;
	PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Color] = 0;
	PLAYER_GANGZONE[playerid][gangzoneid][gangzone_Flash_Color] = 0;
	return 1;
}

// Tracing
stock StartTracing(playerid, Float:x, Float:y, Float:z)
{
	CancelTracing(playerid);
	GPS_PLAYER[playerid][gps_Timer] = SetTimerEx("UpdatePlayerTracing", ROUTE_UPDATE_DELAY, true, "ifff", playerid, x, y, z);
	return 1;
}

stock CancelTracing(playerid)
{
	#if defined ROUTE_TRACING_DEBUG
		printf("[route-tracing] Tracing cancelled (playerid: %d)", playerid);
	#endif

	KillTimer(GPS_PLAYER[playerid][gps_Timer]);
	if (GPS_PLAYER[playerid][gps_Activated])
	{
		// Cancel update
		GPS_PLAYER[playerid][gps_Activated] = false;

		// Destroy gangzones
		for(new i; i < MAX_ROUTE_GANGZONES; i ++)
		{
			if (GPS_PLAYER_GANGZONES[playerid][i] != -1)
			{
			    PlayerGangZoneDestroy(playerid, GPS_PLAYER_GANGZONES[playerid][i]);
			    GPS_PLAYER_GANGZONES[playerid][i] = -1;
			}
		}
	}
	return 1;
}

stock SetTracingColor(playerid, color)
{
	#if defined ROUTE_TRACING_DEBUG
		printf("[route-tracing] Route color updated (playerid: %d, color: %d)", playerid, color);
	#endif

	GPS_PLAYER[playerid][gps_Color] = color;
	return 1;
}

stock GetTracingColor(playerid)
{
	return GPS_PLAYER[playerid][gps_Color];
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
		Float:px,
		Float:py,
		Float:pz,
		Float:init_distance,
		MapNode:point_x,
		MapNode:point_y
	;

	// Thanks to Markski for this arrangement in the distance check
	if (GPS_PLAYER[playerid][gps_Activated])
	{
		if (IsPlayerInRangeOfPoint(playerid, 3.5, GPS_PLAYER_POSITION[playerid][0], GPS_PLAYER_POSITION[playerid][1], GPS_PLAYER_POSITION[playerid][2])) return 0;
	}
	else
	{
		GPS_PLAYER[playerid][gps_Activated] = true;
	}

	// Update position
	GetPlayerPos(playerid, px, py, pz);

	GPS_PLAYER_POSITION[playerid][0] = px;
	GPS_PLAYER_POSITION[playerid][1] = py;
	GPS_PLAYER_POSITION[playerid][2] = pz;

	init_distance = ReturnRouteDistance(x, y, px, py);

	if (init_distance <= 2.0) return CancelTracing(playerid);
	if (GetClosestMapNodeToPoint(px, py, pz, point_x) != 0) return 0;
	if (GetClosestMapNodeToPoint(x, y, z, point_y)) return 0;
	if (FindPathThreaded(point_x, point_y, "OnRouteCalculated", "i", playerid) != GPS_ERROR_NONE)
	{
		print("[route-tracing] UpdatePlayerTracing has referenced an invalid node.");
	}
	return 1;
}

public OnRouteCalculated(Path:pathid, playerid)
{
	if (!IsValidPath(pathid))
		return 0;

	// Destroy old gangzones
	for(new i; i < MAX_ROUTE_GANGZONES; i ++)
	{
		if (GPS_PLAYER_GANGZONES[playerid][i] != -1)
		{
			PlayerGangZoneDestroy(playerid, GPS_PLAYER_GANGZONES[playerid][i]);
			GPS_PLAYER_GANGZONES[playerid][i] = -1;
		}
	}

	new
		index,
		size,
		MapNode:nodeid,
		Float:length,
		Float:map_x,
		Float:map_y,
		Float:map_z,
		Float:map_px,
		Float:map_py,
		Float:map_pz
	;

	GetPathSize(pathid, size);
	GetPathLength(pathid, length);

	if (size == 1)
		return CancelTracing(playerid);

	// Create gangzones
	GetPlayerPos(playerid, map_px, map_py, map_pz);
	GetClosestMapNodeToPoint(map_px, map_py, map_pz, nodeid);
	GetMapNodePos(nodeid, map_px, map_py, map_pz);

	for(new x = 0; x < MAX_ROUTE_GANGZONES; x ++)
	{
		GetPathNode(pathid, x, nodeid);
		GetPathNodeIndex(pathid, nodeid, index);
		GetMapNodePos(nodeid, map_x, map_y, map_z);

		if (x == index)
		{
			new node_size = floatround( ReturnRouteDistance( map_px, map_py, map_x, map_y ) / 12.5 );

			for(new i = 1; i <= node_size; i++)
			{
				// Formula for drawing the route
				new
					Float:node_x,
					Float:node_y
				;

				if (i != 0)
				{
					node_x = map_px + (((map_x - map_px) / node_size) * i);
					node_y = map_py + (((map_y - map_py) / node_size) * i);
				}
				else
				{
					node_x = map_px;
					node_y = map_py;
				}

				new slot;
				while(slot <= MAX_ROUTE_GANGZONES)
				{
					if (slot == MAX_ROUTE_GANGZONES)
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

				if (slot == -1)
					return 1;

				GPS_PLAYER_GANGZONES[playerid][slot] = PlayerGangZoneCreate(
					playerid,
					node_x - 12.5,
					node_y - 12.5,
					node_x + 12.5,
					node_y + 12.5
				);

				PlayerGangZoneShow(playerid, GPS_PLAYER_GANGZONES[playerid][slot], GPS_PLAYER[playerid][gps_Color]);
			}
		}

		map_px = map_x + 0.5;
		map_py = map_y + 0.5;
	}
    return 1;
}

// Hooks
public OnGameModeInit()
{
	#if defined ROUTE_TRACING_DEBUG
		printf("[route-tracing] Hook call: OnGameModeInit()");
	#endif

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
	#if defined ROUTE_TRACING_DEBUG
		printf("[route-tracing] Hook call: OnPlayerConnect(playerid: %d)", playerid);
	#endif

	GPS_PLAYER[playerid][gps_Color] = 0x9C0DE6FF;

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
	#if defined ROUTE_TRACING_DEBUG
		printf("[route-tracing] Hook call: OnPlayerDisconnect(playerid: %d, reason: %d)", playerid, reason);
	#endif

	CancelTracing(playerid);

	for (new i = 0; i < MAX_PLAYER_GANGZONES; ++i)
		PLAYER_GANGZONE[playerid][i][gangzone_Used] = false;

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
