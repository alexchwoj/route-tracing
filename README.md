# route-tracing
Path layout for San Andreas Multiplayer.

## Implementation

1. Download the latest version in [releases](https://github.com/RealAtom/route-tracing/releases)
2. Put the `route-tracing.inc` file in `pawno/include`
3. Enter this line in your initial script
```pawn
#include <route-tracing>
```
4. Install [Modern GPS](https://github.com/kristoisberg/samp-gps-plugin)
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

## Demostration
![](https://i.imgur.com/hn8QaUV.png)

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

Donate (BTC)

`1CMEPuTPtEKRRbPokRp9vTqoJNVNmm1GXt`
