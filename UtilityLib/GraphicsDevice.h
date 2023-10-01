#pragma	once
#include	<d3d8.h>

typedef struct GraphicsDevice_t	GraphicsDevice;


extern BOOL		GD_Init(GraphicsDevice **ppGD, int w, int h);
extern void		GD_Destroy(GraphicsDevice **ppGD);

extern HRESULT	GD_Present(GraphicsDevice *pGD);
extern HRESULT	GD_Clear(GraphicsDevice *pGD, D3DCOLOR clearColour);
extern HRESULT	GD_BeginScene(GraphicsDevice *pGD);
extern HRESULT	GD_EndScene(GraphicsDevice *pGD);
extern HRESULT	GD_DrawIndexedPrimitive(GraphicsDevice *pGD, D3DPRIMITIVETYPE primType,
								UINT startIdx, UINT primCount);
extern HRESULT	GD_DrawVertices(GraphicsDevice *pGD, D3DPRIMITIVETYPE primType,
								UINT startIdx, UINT vCount);

//gets
extern int	GD_GetWidth(const GraphicsDevice *pGD);
extern int	GD_GetHeight(const GraphicsDevice *pGD);

//sets
extern HRESULT	GD_SetVShaderConstant(GraphicsDevice *pGD,
					int regNum, void *data, DWORD count);
extern HRESULT	GD_SetPShaderConstant(GraphicsDevice *pGD,
					int regNum, void *data, DWORD count);
extern HRESULT	GD_SetRenderState(GraphicsDevice *pGD,
					D3DRENDERSTATETYPE state, DWORD val);
extern HRESULT	GD_SetVertexShader(GraphicsDevice *pGD, DWORD handle);
extern HRESULT	GD_SetPixelShader(GraphicsDevice *pGD, DWORD handle);
extern HRESULT	GD_SetStreamSource(GraphicsDevice *pGD, UINT streamNum,
					D3DVertexBuffer *pVB, UINT stride);
extern HRESULT	GD_SetIndices(GraphicsDevice *pGD, D3DIndexBuffer *pInds, UINT baseVert);

//resource creation / destruction
extern D3DTexture	*GD_MakeTexture(GraphicsDevice *pGD,
						BYTE **pRows, int w, int h, int rowPitch);
extern D3DTexture	*GD_CreateTexture(GraphicsDevice *pGD,
						int w, int h, int numMips,
						DWORD usage, D3DFORMAT format);
extern HRESULT	GD_CreateVertexBuffer(GraphicsDevice *pGD, void *pVertData,
					DWORD len, LPDIRECT3DVERTEXBUFFER8 *ppVB);
extern HRESULT	GD_CreateIndexBuffer(GraphicsDevice *pGD, void *pIdxData,
					DWORD len, LPDIRECT3DINDEXBUFFER8 *ppIB);
extern HRESULT	GD_CreateTextureFromFile(GraphicsDevice *pGD,
										 LPDIRECT3DTEXTURE8 *ppTex, const char *pFileName);

//shader creation stuff
extern DWORD	GD_CreateVertexShader(GraphicsDevice *pGD,
					const DWORD *pDecl,	const DWORD *pCodeBytes);
extern DWORD	GD_CreatePixelShader(GraphicsDevice *pGD,
					const D3DPIXELSHADERDEF *pCodeBytes);

