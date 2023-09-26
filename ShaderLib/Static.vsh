xvs.1.1

//constants
//c0-c3		view matrix
//c4-c7		projection matrix
//c8-11		world matrix
//c12		eye position
//trilight stuff http://home.comcast.net/~tom_forsyth/blog.wiki.html#Trilights
//c13		light color 0 for trilight
//c14		light 1
//c15		light 2
//c16		light direction + specular power in w
//c17		material colour
//c18		specular colour

//this is a port of WNormWPosVS from shaderlib

//output registers
//oPos		screenspace position
//oPts		??
//oT0		worldspace normal
//oT1		worldspace position
//oT2		??
//oT3		??
//oD0		??
//oD1		??

//input registers
//v0		Position
//v1		Normal
//v2		TexCoord0 in xy

//store world matrix in r0
mov		r0, c8
mov		r1, c9
mov		r2, c10
mov		r3, c11

//mul and store worldview matrix in r4-r7
mul		r4, c8, r0
mul		r5, c9, r1
mul		r6, c10, r2
mul		r7, c11, r3

//mul wv by projection
mul		r4, r4, c4
mul		r5, r5, c5
mul		r6, r6, c6
mul		r7, r7, c7

//transform position
m4x4	oPos, v0, r4

//world transform normal and store in T0
m4x4	oT0, v1, r0

//world transform position and store in T1
m4x4	oT1, v2, r0