xvs.1.1

#pragma screenspace

//constants
//c0		text origin, scale in zw
//c1		-0.5 in x, 1 in y

//output registers
//oPos		screenspace position
//oT0		texcoord0

//input registers
//v0		Position in xy, uv in zw

//position

//scale position
mul		r0, v0.xy, c0.zw
add		oPos.xy, r0.xy, c0.xy
mov		oPos.z, c1.x;
mov		oPos.w, c1.y;

//texcoords
mov		oT0, v0.zw