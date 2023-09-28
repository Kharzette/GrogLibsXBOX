//#include	<xtl.h>
//#include	<d3d8.h>
#include	"PrimFactory.h"
#include	"GraphicsDevice.h"


typedef struct	VPosNormTex0_t
{
	D3DXVECTOR3		Position;
	D3DXVECTOR3		Normal;
	D3DXVECTOR2		TexCoord0;
}	VPosNormTex0;

#define	D3DFVF_POSNORMTEX	(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX0)

static	D3DXVECTOR3	UnitX	={	1.0f, 0.0f, 0.0f	};
static	D3DXVECTOR3	UnitY	={	0.0f, 1.0f, 0.0f	};
static	D3DXVECTOR3	UnitZ	={	0.0f, 0.0f, 1.0f	};
static	D3DXVECTOR3	xnorm	={	1.0f, 0.0f, 0.0f	};
static	D3DXVECTOR3	ynorm	={	0.0f, 1.0f, 0.0f	};
static	D3DXVECTOR3	znorm	={	0.0f, 0.0f, 1.0f	};
static	D3DXVECTOR3	nxnorm	={	-1.0f, 0.0f, 0.0f	};
static	D3DXVECTOR3	nynorm	={	0.0f, -1.0f, 0.0f	};
static	D3DXVECTOR3	nznorm	={	0.0f, 0.0f, -1.0f	};

PrimObject	*PF_CreateCubeFromCorners(const D3DXVECTOR3 *pCorners, GraphicsDevice *pGD)
{
	int				i;
	USHORT			idx, indexes[36];
	HRESULT			hres;
	VPosNormTex0	vpnt[24];
    PrimObject		*pObj;

	//cube corners
	D3DXVECTOR3	lowerTopRight, lowerTopLeft, lowerBotRight, lowerBotLeft;
	D3DXVECTOR3	upperTopRight, upperTopLeft, upperBotRight, upperBotLeft;

	lowerTopRight	=pCorners[0];
	lowerTopLeft	=pCorners[1];
	lowerBotRight	=pCorners[2];
	lowerBotLeft	=pCorners[3];
	upperTopRight	=pCorners[4];
	upperTopLeft	=pCorners[5];
	upperBotRight	=pCorners[6];
	upperBotLeft	=pCorners[7];

	//cube sides
	//top
	vpnt[0].Position	=upperTopLeft;
	vpnt[1].Position	=upperTopRight;
	vpnt[2].Position	=upperBotRight;
	vpnt[3].Position	=upperBotLeft;

	//bottom (note reversal)
	vpnt[7].Position	=lowerTopLeft;
	vpnt[6].Position	=lowerTopRight;
	vpnt[5].Position	=lowerBotRight;
	vpnt[4].Position	=lowerBotLeft;

	//top z side
	vpnt[11].Position	=upperTopLeft;
	vpnt[10].Position	=upperTopRight;
	vpnt[9].Position	=lowerTopRight;
	vpnt[8].Position	=lowerTopLeft;

	//bottom z side
	vpnt[12].Position	=upperBotLeft;
	vpnt[13].Position	=upperBotRight;
	vpnt[14].Position	=lowerBotRight;
	vpnt[15].Position	=lowerBotLeft;

	//-x side
	vpnt[16].Position	=upperTopLeft;
	vpnt[17].Position	=upperBotLeft;
	vpnt[18].Position	=lowerBotLeft;
	vpnt[19].Position	=lowerTopLeft;

	//+x side
	vpnt[23].Position	=upperTopRight;
	vpnt[22].Position	=upperBotRight;
	vpnt[21].Position	=lowerBotRight;
	vpnt[20].Position	=lowerTopRight;


	//normals
	vpnt[0].Normal	=ynorm;
	vpnt[1].Normal	=ynorm;
	vpnt[2].Normal	=ynorm;
	vpnt[3].Normal	=ynorm;

	vpnt[4].Normal	=nynorm;
	vpnt[5].Normal	=nynorm;
	vpnt[6].Normal	=nynorm;
	vpnt[7].Normal	=nynorm;

	vpnt[8].Normal	=znorm;
	vpnt[9].Normal	=znorm;
	vpnt[10].Normal	=znorm;
	vpnt[11].Normal	=znorm;

	vpnt[12].Normal	=nznorm;
	vpnt[13].Normal	=nznorm;
	vpnt[14].Normal	=nznorm;
	vpnt[15].Normal	=nznorm;

	vpnt[16].Normal	=nxnorm;
	vpnt[17].Normal	=nxnorm;
	vpnt[18].Normal	=nxnorm;
	vpnt[19].Normal	=nxnorm;

	vpnt[20].Normal	=xnorm;
	vpnt[21].Normal	=xnorm;
	vpnt[22].Normal	=xnorm;
	vpnt[23].Normal	=xnorm;

	//texcoords
	for(i=0;i < 24;i+=4)
	{
		vpnt[i].TexCoord0.x	=0.0f;
		vpnt[i].TexCoord0.y	=0.0f;

		vpnt[i + 1].TexCoord0.x	=1.0f;
		vpnt[i + 1].TexCoord0.y	=0.0f;

		vpnt[i + 2].TexCoord0.x	=1.0f;
		vpnt[i + 2].TexCoord0.y	=1.0f;

		vpnt[i + 3].TexCoord0.x	=0.0f;
		vpnt[i + 3].TexCoord0.y	=1.0f;
	}

	//indexes
	for(i=idx=0;i < 36;i+=6)
	{
		indexes[i]		=idx + 0;
		indexes[i + 1]	=idx + 2;
		indexes[i + 2]	=idx + 1;
		indexes[i + 3]	=idx + 0;
		indexes[i + 4]	=idx + 3;
		indexes[i + 5]	=idx + 2;

		idx	+=4;
	}

	//return object
	pObj	=malloc(sizeof(PrimObject));
	if(pObj == NULL)
	{
		return	NULL;
	}

	pObj->mVertCount	=24;
	pObj->mIndexCount	=36;
	pObj->mStride		=sizeof(VPosNormTex0);

	//make vertex buffer
	hres	=GD_CreateVertexBuffer(pGD, vpnt, pObj->mStride * 24, &pObj->mpVB);
	if(FAILED(hres))
	{
		free(pObj);
		return	NULL;
	}

	//make index buffer
	hres	=GD_CreateIndexBuffer(pGD, indexes, 36 * 2, &pObj->mpIB);
	if(FAILED(hres))
	{
		IDirect3DVertexBuffer8_Release(pObj->mpVB);
		free(pObj);
		return	NULL;
	}

	return	pObj;
}

static void	Vec3Add3(D3DXVECTOR3 *pDest, const D3DXVECTOR3 *a,
			const D3DXVECTOR3 *b, const D3DXVECTOR3 *c)
{
	D3DXVec3Add(pDest, a, b);
	D3DXVec3Add(pDest, pDest, c);
}

PrimObject	*PF_CreateCube(float size, GraphicsDevice *pGD)
{
	D3DXVECTOR3	corners[8];
	D3DXVECTOR3	xScaled, yScaled, zScaled;
	D3DXVECTOR3	xNeg, yNeg, zNeg;

	D3DXVec3Scale(&xScaled, &UnitX, size);
	D3DXVec3Scale(&yScaled, &UnitY, size);
	D3DXVec3Scale(&zScaled, &UnitZ, size);

	D3DXVec3Scale(&xNeg, &UnitX, -size);
	D3DXVec3Scale(&yNeg, &UnitY, -size);
	D3DXVec3Scale(&zNeg, &UnitZ, -size);

	Vec3Add3(&corners[0], &yNeg, &xScaled, &zScaled);
	Vec3Add3(&corners[1], &yNeg, &xNeg, &zScaled);
	Vec3Add3(&corners[2], &yNeg, &xScaled, &zNeg);
	Vec3Add3(&corners[3], &yNeg, &xNeg, &zNeg);
	Vec3Add3(&corners[4], &yScaled, &xScaled, &zScaled);
	Vec3Add3(&corners[5], &yScaled, &xNeg, &zScaled);
	Vec3Add3(&corners[6], &yScaled, &xScaled, &zNeg);
	Vec3Add3(&corners[7], &yScaled, &xNeg, &zNeg);

	return	PF_CreateCubeFromCorners(corners, pGD);
}
