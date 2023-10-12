#pragma once
#include	<XTL.h>

extern float __cdecl	WrapAngleDegrees(float inDeg);
extern float __cdecl	WrapAngleRadians(float inRad);
extern void __cdecl		WrapAngleRadiansV3(D3DXVECTOR3 *pRad);
extern float __cdecl	Clamp2(float val, float min, float max);
extern float			Clamp(float val, float min, float max);
extern int				ClampInt(int val, int min, int max);
extern void				RotateVec(const D3DXQUATERNION *pQ, const D3DXVECTOR3 *pV,
							D3DXVECTOR3 *pDest);
