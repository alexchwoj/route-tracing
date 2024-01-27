# route-tracing
Path layout for San Andreas Multiplayer.

## Read
Just a friendly note, if you're another Hispanic Roleplayer planning to use it for your low-quality server, please consider leaving me a star, at the very least.

## Implementation

1. Download the latest version in [releases](https://github.com/alexchwoj/route-tracing/releases)
2. Enter this line in your initial script
```pawn
#include <route-tracing>
```
3. Install [Modern GPS](https://github.com/kristoisberg/samp-gps-plugin)
4. Install [Pawn.RakNet](https://github.com/katursis/Pawn.RakNet)
5. Put `GPS.dat` file in `scriptfiles/`

## Example
In the `test.pwn` file you have a usage example.

```pawn
public OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
	StartTracing(playerid, fX, fY, fZ);
	return 1;
}
```

## How works?
So that routes can be drawn at great distances route-tracing has built-in functions to create gangzones per player, using Pawn.RakNet, 512 gang zones are reserved for operation. Before it was not used, making the limit of 50 gangzones making no more than 20 players can use them at the same time.

## Demostration
![](https://i.imgur.com/hn8QaUV.png)
![](https://media.discordapp.net/attachments/804497389678362684/805292769882669146/unknown.png)

## Fuctions
```pawn
StartTracing(playerid, Float:x, Float:y, Float:z);
CancelTracing(playerid);
SetTracingColor(playerid, color);
GetTracingColor(playerid);
```

## Credits
* [Atom](https://github.com/RealAtom) - Programming
* [kristo](https://github.com/kristoisberg) - Modern GPS Plugin
* [katursis](https://github.com/katursis) - Pawn.RakNet
