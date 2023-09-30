#pragma once
#include	<xtl.h>

//forward struct decs
typedef struct	StuffKeeper_t		StuffKeeper;
typedef struct	GraphicsDevice_t	GraphicsDevice;

//helpers
extern DWORD	LoadCompiledVShader(GraphicsDevice *pGD,
					const DWORD *pVertDecl, const char *pPath);
extern DWORD	LoadCompiledPShader(GraphicsDevice *pGD, const char *pPath);


//eventually stuff here about loading shaders and textures
extern StuffKeeper	*StuffKeeper_Create(GraphicsDevice *pGD);

//stored stuff gets
extern IDirect3DTexture8	*StuffKeeper_GetTexture2D(StuffKeeper *pSK, const char *pName);
extern DWORD				StuffKeeper_GetVertexShader(StuffKeeper *pSK, const char *pName);
extern DWORD				StuffKeeper_GetPixelShader(StuffKeeper *pSK, const char *pName);
