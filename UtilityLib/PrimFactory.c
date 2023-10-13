#include	<xtl.h>
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


PrimObject	*PF_CreateSphere(GraphicsDevice *pGD,
			D3DXVECTOR3 center, float radius)
{
	int	theta, phi, i, ofs;

	//density
	int	dtheta	=10;
	int	dphi	=10;

	VPosNormTex0	*pVPNT;
	PrimObject		*pObj;
	HRESULT			hres;

	D3DXVECTOR3	*pPoints;
	USHORT		*pInds;
	
	//run through the build to
	//count how much space is needed
	USHORT	curIdx	=0;
	int		indIdx	=0;
	for(theta=-90;theta <= 0-dtheta;theta += dtheta)
	{
		for(phi=0;phi <= 360-dphi;phi += dphi)
		{
			D3DXVECTOR3	pos	={	0.0f, 0.0f, 0.0f	};

			float	rtheta	=D3DXToRadian(theta);
			float	rdtheta	=D3DXToRadian(dtheta);
			float	rphi	=D3DXToRadian(phi);
			float	rdphi	=D3DXToRadian(dphi);

			if(theta > -90 && theta < 0)
			{
				indIdx++;
				indIdx++;
				indIdx++;
				indIdx++;
				indIdx++;
				indIdx++;

				curIdx	+=4;
			}
			else
			{
				indIdx++;
				indIdx++;
				indIdx++;

				curIdx	+=3;
			}
		}
	}

	pPoints	=malloc(sizeof(D3DXVECTOR3) * curIdx);
	pInds	=malloc(2 * indIdx * 2);

	//build and index a hemisphere
	curIdx	=0;
	indIdx	=0;
	for(theta=-90;theta <= 0-dtheta;theta += dtheta)
	{
		for(phi=0;phi <= 360-dphi;phi += dphi)
		{
			D3DXVECTOR3	pos	={	0.0f, 0.0f, 0.0f	};

			float	rtheta	=D3DXToRadian(theta);
			float	rdtheta	=D3DXToRadian(dtheta);
			float	rphi	=D3DXToRadian(phi);
			float	rdphi	=D3DXToRadian(dphi);

			pos.x	=(float)(cos(rtheta) * cos(rphi));
			pos.y	=(float)(cos(rtheta) * sin(rphi));
			pos.z	=(float)sin(rtheta);

			pPoints[curIdx]	=pos;
			
			pos.x	=(float)(cos((rtheta + rdtheta)) * cos(rphi));
			pos.y	=(float)(cos((rtheta + rdtheta)) * sin(rphi));
			pos.z	=(float)sin((rtheta + rdtheta));

			pPoints[curIdx + 1]	=pos;

			pos.x	=(float)(cos((rtheta + rdtheta)) * cos((rphi + rdphi)));
			pos.y	=(float)(cos((rtheta + rdtheta)) * sin((rphi + rdphi)));
			pos.z	=(float)sin((rtheta + rdtheta));

			pPoints[curIdx + 2]	=pos;

			if(theta > -90 && theta < 0)
			{
				pos.x	=(float)(cos(rtheta) * cos((rphi + rdphi)));
				pos.y	=(float)(cos(rtheta) * sin((rphi + rdphi)));
				pos.z	=(float)sin(rtheta);

				pPoints[curIdx + 3]	=pos;

				pInds[indIdx++]	=curIdx;
				pInds[indIdx++]	=((USHORT)(curIdx + 1));
				pInds[indIdx++]	=((USHORT)(curIdx + 2));
				pInds[indIdx++]	=((USHORT)(curIdx + 0));
				pInds[indIdx++]	=((USHORT)(curIdx + 2));
				pInds[indIdx++]	=((USHORT)(curIdx + 3));

				curIdx	+=4;
			}
			else
			{
				pInds[indIdx++]	=(curIdx);
				pInds[indIdx++]	=((USHORT)(curIdx + 1));
				pInds[indIdx++]	=((USHORT)(curIdx + 2));

				curIdx	+=3;
			}
		}
	}

	pVPNT	=malloc(sizeof(VPosNormTex0) * curIdx * 2);

	//copy in hemisphere
	for(i=0;i < curIdx;i++)
	{
		D3DXVec3Normalize(&pVPNT[i].Normal, &pPoints[i]);

		D3DXVec3Scale(&pVPNT[i].Position, &pVPNT[i].Normal, radius);

		//in case you want something offcenter!?
		D3DXVec3Add(&pVPNT[i].Position, &center, &pVPNT[i].Position);

		//I might just do a cubemap lookup from the normal?
		pVPNT[i].TexCoord0.x	=pVPNT[i].TexCoord0.y	=0.0f;
	}

	//dupe for the other half
	ofs	=curIdx;
	for(i=ofs;i < (curIdx + ofs);i++)
	{
		//mirror along z
		pVPNT[i].Position.x	=pPoints[i - ofs].x;
		pVPNT[i].Position.y	=pPoints[i - ofs].y;
		pVPNT[i].Position.z	=-pPoints[i - ofs].z;

		//make normal
		D3DXVec3Normalize(&pVPNT[i].Normal, &pVPNT[i].Position);

		D3DXVec3Scale(&pVPNT[i].Position, &pVPNT[i].Normal, radius);

		//in case you want something offcenter!?
		D3DXVec3Add(&pVPNT[i].Position, &center, &pVPNT[i].Position);

		//I might just do a cubemap lookup from the normal?
		pVPNT[i].TexCoord0.x	=pVPNT[i].TexCoord0.y	=1.0f;	//set to 1 for debugging
	}

	//indexes for the other half
	for(i=0;i < indIdx;i++)
	{
		pInds[i + indIdx]	=pInds[(indIdx - 1) - i] + curIdx;
	}

	//return object
	pObj	=malloc(sizeof(PrimObject));
	if(pObj == NULL)
	{
		return	NULL;
	}

	pObj->mVertCount	=curIdx;
	pObj->mIndexCount	=indIdx * 2;
	pObj->mStride		=sizeof(VPosNormTex0);

	//make vertex buffer
	hres	=GD_CreateVertexBuffer(pGD, pVPNT, pObj->mStride * curIdx * 2, &pObj->mpVB);
	if(FAILED(hres))
	{
		free(pObj);
		return	NULL;
	}

	//make index buffer
	hres	=GD_CreateIndexBuffer(pGD, pInds, indIdx * 2 * 2, &pObj->mpIB);
	if(FAILED(hres))
	{
		IDirect3DVertexBuffer8_Release(pObj->mpVB);
		free(pObj);
		return	NULL;
	}

	free(pPoints);
	free(pInds);
	free(pVPNT);

	return	pObj;
}