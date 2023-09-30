
float __cdecl	WrapAngleDegrees(float inDeg)
{
	float	retDeg, degWrap	=360;

	__asm	fld		degWrap;
	__asm	fld		inDeg;
	__asm	fprem1;
	__asm	fstp	retDeg;
	__asm	fstp	degWrap;	//just get this off the fp stack

	return	retDeg;
}

