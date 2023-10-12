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

	__asm	fld		radWrap;	//radWrap
	__asm	fld		inRad;		//inRad		radWrap
	__asm	fprem1;				//remnd		radWrap
	__asm	fstp	retRad;		//radWrap
	__asm	fstp	radWrap;	//

	return	retRad;
}

//wrap in place a pyr vector
void __cdecl	WrapAngleRadiansV3(D3DXVECTOR3 *pRad)
{
	float	radWrap	=D3DX_PI * 2.0f;

	__asm	mov		eax, pRad

	__asm	fld		radWrap;	//radWrap
	__asm	fld		[eax];		//pRad.x	radWrap
	__asm	fprem1;				//remnd		radWrap
	__asm	fstp	[eax];		//radWrap

	__asm	fld		[eax]+4;	//pRad.y	radWrap
	__asm	fprem1;				//remnd		radWrap
	__asm	fstp	[eax]+4;	//radWrap

	__asm	fld		[eax]+8;	//pRad.z	radWrap
	__asm	fprem1;				//remnd		radWrap
	__asm	fstp	[eax]+8;	//radWrap

	__asm	fstp	radWrap;	//just get this off the fp stack
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

int	ClampInt(int val, int min, int max)
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

//adapted this from CGLM
//I use it with linux / vulkan alot
//No idea how it works!
void	RotateVec(const D3DXQUATERNION *pQ, const D3DXVECTOR3 *pV,
						D3DXVECTOR3 *pDest)
{
	D3DXQUATERNION	qn;
	D3DXVECTOR3		forward, v1, v2;

	//normalize into qn
	D3DXQuaternionNormalize(&qn, pQ);

	//CGLM calls this the imaginary part
	forward.x	=qn.x;
	forward.y	=qn.y;
	forward.z	=qn.z;

	D3DXVec3Scale(&v1, &forward, 2.0f * D3DXVec3Dot(&forward, pV));
	D3DXVec3Scale(&v2, pV, qn.w * qn.w - D3DXVec3Dot(&forward, &forward));
	D3DXVec3Add(&v1, &v1, &v2);

	D3DXVec3Cross(&v2, &forward, pV);
	D3DXVec3Scale(&v2, &v2, 2.0f * qn.w);

	D3DXVec3Add(pDest, &v1, &v2);
}
