xvs.1.1

//constants
//c0-c3		view matrix
//c4-c7		projection matrix
//c8-11		world matrix
//c12		eye position

//output registers
//oPos		screenspace position
//oPts		??
//oT0		??
//oT1		??
//oT2		??
//oT3		??
//oD0		??
//oD1		??

//input registers
//v0		Position

//mul by view
m4x4	r0, v0, c0

//mul by proj
m4x4	oPos, r0, c4
