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
//oT0		texcoord0
//oT1		??
//oT2		??
//oT3		??
//oD0		Trilight colour at vertex
//oD1		??

//input registers
//v0		Position
//v1		Normal
//v2		TexCoord0 in xy

//mul position into world space
m4x4	r0, v0, c8

//mul by view
m4x4	r0, r0, c0

//mul by proj
m4x4	oPos, r0, c4

//world transform normal
m4x4	r1, v1, c8

//store texcoord in T0
mov		oT0, v2

//compute LdotN
dp3		r2, r1, c16.xyz

//get 0 in r5
sub		r5, r1, r1

//r3	=max(0, LdotN)
max		r3, r5, r2.x

//r4	=max(0, LdotN) * color2
mul		r4, r3, c15

//set r6 to 1 if LdotN is less than 0
slt		r6, r2, r5

//copy 1 into r8 from the w of a vertex
//hopefully this will always be one
mov		r8, v0.w

//this will put a 2 in r11
add		r11, r8, r8

//multiply r6 by 2, this will be 0 or 2
mul		r6, r6, r11

//puts the slt result in the -1 to 1 range
sub		r6, r6, r8

//put ABS LdotN in r7 by muling by either -1 or 1
mul		r7, r2, -r6

//r9	=1 - abs(LdotN)
sub		r9, r8, r7

//r9	=(color1 * (1 - abs(LdotN)))
mul		r9, r9, c14

//r10	=max(0, -LdotN)
max		r10, r5, -r2.x

//r10	=(color0 * max(0, -LdotN))
mul		r10, r10, c13

//r4	=(c2 * max(0, LdotN)) + (c1 * (1 - abs(LdotN)))
add		r4, r4, r9

//r4	=(c2 * max(0, LdotN)) + (c1 * (1 - abs(LdotN))) + (c0 * max(0, -LdotN))
add		r4, r4, r10

//D0	=r4 * material colour
mul		oD0.xyz, r4.xyz, c17.xyz

//alpha value direct move
mov		oD0.w, c17.w