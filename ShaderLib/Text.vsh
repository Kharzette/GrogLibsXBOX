xvs.1.1

#pragma screenspace

//constants
//c0		text origin, scale in zw
//c1		-0.5, 1, 0, 2

//output registers
//oPos		screenspace position
//oT0		texcoord0

//input registers
//v0		Position in xy, uv in zw

//position

//scale position
mul		r0, v0.xy, c0.zw

//add origin
add		oPos.xy, r0.xy, c0.xy

//set z and w to 1
mov		oPos.zw, c1.y;

//texcoords
mov		oT0.xy, v0.zw

//set texcoords z and w to 1
//This has a crazy effect on the fetch
//if you don't do this
mov		oT0.zw, c1.y