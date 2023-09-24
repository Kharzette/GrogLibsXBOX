#include	<xtl.h>
#include	<d3d8.h>
#include	<stdio.h>
#include	"GraphicsDevice.h"


typedef struct GraphicsDevice_t
{
	//D3D interfaces
	Direct3D	*mpD3D;
	D3DDevice	*mpDevice;

	//size
	int	mWidth, mHeight;
}	GraphicsDevice;


//try to make a device
BOOL	GD_Init(GraphicsDevice **ppGD, int w, int h)
{
	HRESULT					hres;
	GraphicsDevice			*pGD;
	D3DPRESENT_PARAMETERS	pp;


	//alloc
	*ppGD	=malloc(sizeof(GraphicsDevice));
	memset(*ppGD, 0, sizeof(GraphicsDevice));

	pGD	=*ppGD;

	pGD->mWidth		=w;
	pGD->mHeight	=h;

	pGD->mpD3D	=Direct3DCreate8(D3D_SDK_VERSION);
	if(pGD->mpD3D == NULL)
	{
		free(pGD);
		printf("D3D init failed: %s\n", GetLastError());
		return	FALSE;
	}

	memset(&pp, 0, sizeof(pp));

	pp.BackBufferWidth			=w;
	pp.BackBufferHeight			=h;
	pp.BackBufferFormat			=D3DFMT_LIN_X8R8G8B8;
	pp.BackBufferCount			=1;
	pp.EnableAutoDepthStencil	=TRUE;
	pp.AutoDepthStencilFormat	=D3DFMT_D24S8;
	pp.SwapEffect				=D3DSWAPEFFECT_DISCARD;

	//not sure what this does yet
	pp.FullScreen_PresentationInterval	=D3DPRESENT_INTERVAL_ONE_OR_IMMEDIATE;

	hres	=IDirect3D8_CreateDevice(pGD->mpD3D, 0, D3DDEVTYPE_HAL, NULL,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &pGD->mpDevice);

	if(FAILED(hres))
	{
		printf("Error creating device: %X\n", hres);
		return	FALSE;
	}

	return	TRUE;
}


int	GD_GetWidth(const GraphicsDevice *pGD)
{
	return	pGD->mWidth;
}

int	GD_GetHeight(const GraphicsDevice *pGD)
{
	return	pGD->mHeight;
}


void	GD_Destroy(GraphicsDevice **ppGD)
{
	GraphicsDevice	*pGD	=*ppGD;

	D3DDevice_Release();

	*ppGD	=NULL;
}

//no initial data
D3DTexture	*GD_CreateTexture(GraphicsDevice *pGD, int w, int h,
	int numMips, DWORD usage, D3DFORMAT format)
{
	D3DTexture	*pTex;

	HRESULT	hres	=D3DXCreateTexture(pGD->mpDevice, w, h, numMips,
		usage, format, D3DPOOL_MANAGED, &pTex);

	if(hres != S_OK)
	{
		printf("Error creating texture: %X\n", hres);
		return	NULL;
	}
	return	pTex;
}

D3DTexture	*GD_MakeTexture(GraphicsDevice *pGD, BYTE **pRows, int w, int h, int rowPitch)
{
	HRESULT			hr;
	D3DLOCKED_RECT	lock;
	D3DTexture		*pRet;
	BOOL			bAlpha	=FALSE;

	//tex create
	hr	=D3DXCreateTexture(pGD->mpDevice, w, h, 0, 0,
		(bAlpha)? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8,
		D3DPOOL_MANAGED, &pRet);

	if(hr != S_OK)
	{
		printf("Error creating texture: %X\n", hr);
		return	NULL;
	}

	//get data into a linear chunk
	D3DTexture_LockRect(pRet, 0, &lock, NULL, 0);

	{
		BYTE	*pBytes	=(BYTE *)lock.pBits;
		int		x, y;

		if(rowPitch > (w * 3))
		{
			//already has alpha?
			for(y=0;y < h;y++)
			{
				memcpy(pBytes + (y * lock.Pitch), pRows[y], rowPitch);
			}
			bAlpha	=TRUE;
		}
		else
		{
			for(y=0;y < h;y++)
			{
				BYTE *pRow	=pBytes + (y * (w * lock.Pitch));

				for(x=0;x < w;x++)
				{
					int	ofs3	=x * 3;
					int	ofs4	=x * 4;

					pRow[ofs4]		=pRows[y][ofs3];
					pRow[ofs4 + 1]	=pRows[y][ofs3 + 1];
					pRow[ofs4 + 2]	=pRows[y][ofs3 + 2];
					pRow[ofs4 + 3]	=0xFF;
				}
			}
		}
	}
	D3DTexture_UnlockRect(pRet, 0);

	return	pRet;
}



DWORD	GD_CreateVertexShader(GraphicsDevice *pGD,
	const DWORD *pDecl,	const DWORD *pCodeBytes)
{
	DWORD	retHandle;

	HRESULT	hr	=IDirect3DDevice8_CreateVertexShader(pGD->mpDevice,
					pDecl, pCodeBytes, &retHandle, 0);

	if(hr != S_OK)
	{
		printf("Error creating vertex shader: %X\n", hr);
		return	-1;
	}
	return	retHandle;
}

DWORD	GD_CreatePixelShader(GraphicsDevice *pGD,
	const D3DPIXELSHADERDEF *pCodeBytes)
{
	DWORD	retHandle;
	HRESULT	hr	=IDirect3DDevice8_CreatePixelShader(pGD->mpDevice,
						pCodeBytes, &retHandle);

	if(hr != S_OK)
	{
		printf("Error creating pixel shader: %X\n", hr);
		return	-1;
	}
	return	retHandle;
}