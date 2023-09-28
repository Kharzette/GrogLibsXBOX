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
//oT2		texcoord0
//oT3		??
//oD0		??
//oD1		??

//input registers
//v0		Position
//v1		Normal
//v2		TexCoord0 in xy

//mul position into world space
m4x4	r0, v0, c8

//store world position in T1
mov		oT1, r0

//mul by view
m4x4	r0, r0, c0

//mul by proj
m4x4	oPos, r0, c4

//world transform normal and store in T0
m4x4	oT0, v1, r0

//store texcoord in T2
mov		oT2, v2