#include	<XTL.h>
#include	<D3DX8Math.h>


float __cdecl	WrapAngleDegrees(float inDeg)
{
	float	retDeg, degWrap	=360.0f;

	__asm	fld		degWrap;
	__asm	fld		inDeg;
	__asm	fprem1;
	__asm	fstp	retDeg;
	__asm	fstp	degWrap;	//just get this off the fp stack

	return	retDeg;
}

float __cdecl	WrapAngleRadians(float inRad)
{
	float	retRad, radWrap	=D3DX_PI * 2.0f;

	__asm	fld		radWrap;
	__asm	fld		inRad;
	__asm	fprem1;
	__asm	fstp	retRad;
	__asm	fstp	radWrap;	//just get this off the fp stack

	return	retRad;
}

//messing around with fcmov, doesn't work
float __cdecl	Clamp2(float val, float min, float max)
{
	float	fret;

	__asm	fld		min;
	__asm	fld		val;
	__asm	fcom	st(1);
	__asm	fcmovb	st(0), st(1);
	__asm	fstp	fret;
	__asm	fstp	max;

	return	fret;
}

float	Clamp(float val, float min, float max)
{
	if(val > max)
	{
		return	max;
	}

	if(val < min)
	{
		return	min;
	}

	return	val;
}