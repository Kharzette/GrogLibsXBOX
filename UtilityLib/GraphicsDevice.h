#pragma	once
#include	<d3d8.h>

typedef struct GraphicsDevice_t	GraphicsDevice;


extern BOOL		GD_Init(GraphicsDevice **ppGD, int w, int h);
extern void		GD_Destroy(GraphicsDevice **ppGD);
extern HRESULT	GD_Present(GraphicsDevice *pGD);
extern HRESULT	GD_Clear(GraphicsDevice *pGD, D3DCOLOR clearColour);
extern HRESULT	GD_BeginScene(GraphicsDevice *pGD);
extern HRESULT	GD_EndScene(GraphicsDevice *pGD);

//gets
extern int	GD_GetWidth(const GraphicsDevice *pGD);
extern int	GD_GetHeight(const GraphicsDevice *pGD);

//resource creation
extern D3DTexture	*GD_MakeTexture(GraphicsDevice *pGD,
						BYTE **pRows, int w, int h, int rowPitch);
extern D3DTexture	*GD_CreateTexture(GraphicsDevice *pGD,
						int w, int h, int numMips,
						DWORD usage, D3DFORMAT format);

//shader creation stuff
extern DWORD	GD_CreateVertexShader(GraphicsDevice *pGD,
					const DWORD *pDecl,	const DWORD *pCodeBytes);
extern DWORD	GD_CreatePixelShader(GraphicsDevice *pGD,
					const D3DPIXELSHADERDEF *pCodeBytes);

