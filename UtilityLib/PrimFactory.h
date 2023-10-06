#pragma once
#include	<xtl.h>
#include	<D3D8.h>
#include	"GraphicsDevice.h"


typedef struct	PrimObject_t
{
	LPDIRECT3DVERTEXBUFFER8	mpVB;
	LPDIRECT3DINDEXBUFFER8	mpIB;

	int		mVertCount;		//num verts
	int		mIndexCount;	//num indexes
	UINT	mStride;
}	PrimObject;

PrimObject	*PF_CreateCubeFromCorners(const D3DXVECTOR3 *pCorners, GraphicsDevice *pGD);
PrimObject	*PF_CreateCube(float size, GraphicsDevice *pGD);
PrimObject	*PF_CreateSphere(GraphicsDevice *pGD,
				D3DXVECTOR3 center, float radius);
