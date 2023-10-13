xvs.1.1

//this will have a c side scale factor
//Huge planets will have a different z scale and such

//constants
//c0-c3		wvp matrix
//c8-11		world matrix
//c16		light direction + specular power in w
//c17		material colour

//output registers
//oPos		screenspace position
//oPts		??
//oT0		texcoord0
//oT1		??
//oT2		??
//oT3		??
//oD0		material colour
//oD1		??

//input registers
//v0		Position
//v1		Normal
//v2		TexCoord0 in xy

//mul position into screen space
m4x4	oPos, v0, c0

//store object space normal in T0
mov		oT0, v1

//world transform normal into R1
m4x4	r1, v1, c8

//normalize r1

//get vector length squared in r3.w
dp3		r3, r1, r1

//get vector length in r4
rsq		r4, r3

//mul to normalize
mul		r5, r1, r4

//compute LdotN
dp3		r2, r5, c16.xyz

//store material colour * LdotN
mul		oD0, r2, c17