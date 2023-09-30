#include	<xtl.h>
#include	<stdio.h>
#include	<math.h>
#include	<ctype.h>
#include	<assert.h>
#include	<sys/stat.h>
#include	<errno.h>
#include	"utstring.h"
#include	"Font.h"
#include	"../UtilityLib/MathCrap.h"
#include	"../UtilityLib/StringStuff.h"
#include	"../UtilityLib/ListStuff.h"
#include	"../UtilityLib/DictionaryStuff.h"
#include	"../UtilityLib/GraphicsDevice.h"

//shaders easier to step through
#define	SHADER_DEBUG

//this jam simplified version for XBOX shouldn't really need
//to store / track shaders as I'll have a very limited number

//data for games
//shaders, textures, fonts, etc
typedef struct	StuffKeeper_t
{
	DictSZ	*mpTextures;		//IDirect3DTexture8

	DictSZ	*mpFonts;			//Font from my Font.h
	DictSZ	*mpFontTextures;	//IDirect3DTexture8

}	StuffKeeper;

//context for iterators
typedef struct	StuffContext_t
{
	StuffKeeper		*mpSK;
	GraphicsDevice	*mpGD;
}	StuffContext;


DWORD LoadCompiledVShader(GraphicsDevice *pGD,
				const DWORD *pVertDecl, const char *pPath)
{
	size_t	amount;
	long	fileLen;
	DWORD	*pCode, vsHandle;
	FILE	*f	=fopen(pPath, "rb");

	//see how big the file is
	fseek(f, 0, SEEK_END);
	fileLen	=ftell(f);
	rewind(f);

	pCode	=malloc(fileLen);

	amount	=fread(pCode, 1, fileLen, f);
	assert(amount == fileLen);

	fclose(f);

	vsHandle	=GD_CreateVertexShader(pGD, pVertDecl, pCode);

	free(pCode);

	return	vsHandle;
}


DWORD LoadCompiledPShader(GraphicsDevice *pGD, const char *pPath)
{
	//compiled pshader code
	D3DPIXELSHADERDEF_FILE	psdf;
	DWORD					psHandle;

	size_t	amount;
	long	fileLen;
	FILE	*f	=fopen(pPath, "rb");

	//see how big the file is
	fseek(f, 0, SEEK_END);
	fileLen	=ftell(f);
	rewind(f);

	amount	=fread(&psdf, 1, sizeof(D3DPIXELSHADERDEF_FILE), f);
	assert(amount == fileLen);

	fclose(f);

	psHandle	=GD_CreatePixelShader(pGD, &psdf.Psd);

	return	psHandle;
}


static void	PreMultAndLinearRGB(BYTE **pRows, int width, int height)
{
	int		x, y;
	float	oo255	=1.0f / 255.0f;

	for(y=0;y < height;y++)
	{
		for(x=0;x < width;x++)
		{
			int	ofsX	=(x * 3);

			BYTE	cR	=pRows[y][ofsX];
			BYTE	cG	=pRows[y][ofsX + 1];
			BYTE	cB	=pRows[y][ofsX + 2];

			float	xc	=cR * oo255;
			float	yc	=cG * oo255;
			float	zc	=cB * oo255;

			//convert to linear
			xc	=misc_powf(xc, 2.2f);
			yc	=misc_powf(yc, 2.2f);
			zc	=misc_powf(zc, 2.2f);

			pRows[y][ofsX]		=(BYTE)(xc * 255.0f);
			pRows[y][ofsX + 1]	=(BYTE)(yc * 255.0f);
			pRows[y][ofsX + 2]	=(BYTE)(zc * 255.0f);
		}
	}
}

static void	PreMultAndLinearRGBA(BYTE **pRows, int width, int height)
{
	int		x, y;
	float	oo255	=1.0f / 255.0f;

	for(y=0;y < height;y++)
	{
		for(x=0;x < width;x++)
		{
			int	ofsX	=(x * 4);

			BYTE	cR	=pRows[y][ofsX];
			BYTE	cG	=pRows[y][ofsX + 1];
			BYTE	cB	=pRows[y][ofsX + 2];
			BYTE	cA	=pRows[y][ofsX + 3];

			float	xc	=cR * oo255;
			float	yc	=cG * oo255;
			float	zc	=cB * oo255;
			float	wc	=cA * oo255;

			//convert to linear
			xc	=misc_powf(xc, 2.2f);
			yc	=misc_powf(yc, 2.2f);
			zc	=misc_powf(zc, 2.2f);

			//premultiply alpha
			xc	*=wc;
			yc	*=wc;
			zc	*=wc;

			pRows[y][ofsX]		=(BYTE)(xc * 255.0f);
			pRows[y][ofsX + 1]	=(BYTE)(yc * 255.0f);
			pRows[y][ofsX + 2]	=(BYTE)(zc * 255.0f);
		}
	}
}

/*
static IDirect3DTexture8 *LoadTexture(GraphicsDevice *pGD, const UT_string *pPath)
{
	FILE	*f	=fopen(utstring_body(pPath), "rb");
	if(f == NULL)
	{
		return	NULL;
	}

	D3DXCreateTextureFromFile()


	png_structp	pPng	=png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(pPng == NULL)
	{
		return	NULL;
	}

	png_infop	pInfo	=png_create_info_struct(pPng);
	if(pInfo == NULL)
	{
		return	NULL;
	}

	if(setjmp(png_jmpbuf(pPng)))
	{
		return	NULL;
	}

	png_init_io(pPng, f);
	png_read_info(pPng, pInfo);

	png_uint_32	width	=png_get_image_width(pPng, pInfo);
	png_uint_32	height	=png_get_image_height(pPng, pInfo);

	png_byte	colType		=png_get_color_type(pPng, pInfo);
	png_byte	bitDepth	=png_get_bit_depth(pPng, pInfo);

	int	numPasses	=png_set_interlace_handling(pPng);
	
	if(bitDepth == 16)
	{
		png_set_strip_16(pPng);
	}
	
	if(colType == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_palette_to_rgb(pPng);
	}
	
	//PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if(colType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
	{
		png_set_expand_gray_1_2_4_to_8(pPng);
	}
	
	if(png_get_valid(pPng, pInfo, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(pPng);
	}
	
	if(colType == PNG_COLOR_TYPE_GRAY ||
		colType == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		png_set_gray_to_rgb(pPng);
	}

	png_read_update_info(pPng, pInfo);

	if(setjmp(png_jmpbuf(pPng)))
	{
		return	NULL;
	}

	int	rowPitch	=png_get_rowbytes(pPng, pInfo);

	png_bytep	*pRows	=malloc(sizeof(png_bytep) * height);
	for(int y=0;y < height;y++)
	{
		pRows[y]	=malloc(rowPitch);
	}

	png_read_image(pPng, pRows);

	fclose(f);

	if(colType == PNG_COLOR_TYPE_RGB)
	{
		PreMultAndLinearRGB(pRows, width, height);
	}
	else if(colType == PNG_COLOR_TYPE_RGBA)
	{
		PreMultAndLinearRGBA(pRows, width, height);
	}
	else if(colType == PNG_COLOR_TYPE_PALETTE)
	{
		PreMultAndLinearRGB(pRows, width, height);
	}
	else
	{
		printf("png %s color type %d unsupported!\n", utstring_body(pPath), colType);

		png_destroy_read_struct(&pPng, &pInfo, NULL);
		for(int y=0;y < height;y++)
		{
			free(pRows[y]);
		}
		free(pRows);
		return	NULL;
	}

	png_destroy_read_struct(&pPng, &pInfo, NULL);

	IDirect3DTexture8	*pTex	=GD_MakeTexture(pGD, pRows, width, height, rowPitch);

	//free data
	for(int y=0;y < height;y++)
	{
		free(pRows[y]);
	}
	free(pRows);

	return	pTex;
}


static void LoadResourcesDir(GraphicsDevice *pGD, StuffKeeper *pSK, const char *szDir)
{
	if(!FileStuff_DirExists(szDir))
	{
		return;
	}

	UT_string	*pTexDir, *pFilePath;
	utstring_new(pTexDir);
	utstring_new(pFilePath);

	utstring_printf(pTexDir, "%s", szDir);
	
	DIR	*pDir	=opendir(utstring_body(pTexDir));
	for(;;)
	{
		struct dirent	*pDE	=readdir(pDir);
		if(pDE == NULL)
		{
			break;
		}

		int	len	=strlen(pDE->d_name);
		if(len < 3)
		{
			continue;	//probably . or ..
		}

		utstring_clear(pFilePath);
		utstring_printf(pFilePath, "%s/%s", utstring_body(pTexDir), pDE->d_name);

		struct stat	fileStuff;
		int	res	=stat(utstring_body(pFilePath), &fileStuff);
		if(res)
		{
			FileStuff_PrintErrno(res);
			continue;
		}

		//check for subdir
		if(S_ISDIR(fileStuff.st_mode))
		{
			//directory recurse
			LoadResourcesDir(pGD, pSK, utstring_body(pFilePath));
			continue;
		}

		UT_string	*pExt	=SZ_GetExtension(pDE->d_name);
		if(pExt == NULL)
		{
			continue;
		}

		//png?
		int	upper	=strncmp(utstring_body(pExt), ".PNG", utstring_len(pExt));
		int	lower	=strncmp(utstring_body(pExt), ".png", utstring_len(pExt));

		utstring_done(pExt);

		if(upper && lower)
		{
			continue;
		}

		//regular file?
		if(S_ISREG(fileStuff.st_mode))
		{
			//isolate the filename without the extension
			UT_string	*pExtLess	=SZ_StripExtensionUT(pFilePath);
			UT_string	*pJustName	=SZ_SubStringUTStart(pExtLess, 9);

			IDirect3DTexture8	*pTex	=LoadTexture(pGD, pFilePath);
			if(pTex != NULL)
			{
				DictSZ_Add(&pSK->mpTextures, pJustName, pTex);
			}

			utstring_done(pExtLess);
			utstring_done(pJustName);
		}
	}
	closedir(pDir);

	utstring_done(pTexDir);
	utstring_done(pFilePath);
}


static void LoadResources(GraphicsDevice *pGD, StuffKeeper *pSK)
{
	LoadResourcesDir(pGD, pSK, "Textures");
}


static void	CreateVShaderCB(const UT_string *pKey, const void *pValue, void *pContext)
{
	StuffContext	*pCon	=pContext;

	const ShaderBytes	*pSB	=pValue;

	ID3D11VertexShader	*pVS	=GD_CreateVertexShader(
									pCon->mpGD, pSB->mpBytes, pSB->mLen);
	if(pVS != NULL)
	{
		DictSZ_Add(&pCon->mpSK->mpVShaders, pKey, pVS);
	}
}

static void	CreatePShaderCB(const UT_string *pKey, const void *pValue, void *pContext)
{
	StuffContext	*pCon	=pContext;

	const ShaderBytes	*pSB	=pValue;

	ID3D11PixelShader	*pPS	=GD_CreatePixelShader(
									pCon->mpGD, pSB->mpBytes, pSB->mLen);
	if(pPS != NULL)
	{
		DictSZ_Add(&pCon->mpSK->mpPShaders, pKey, pPS);
	}
}

static void	CreateSRVCB(const UT_string *pKey, const void *pValue, void *pContext)
{
	StuffContext	*pCon	=pContext;

	const IDirect3DTexture8	*pTex	=pValue;

	D3D11_TEXTURE2D_DESC	desc;
	pTex->lpVtbl->GetDesc((IDirect3DTexture8 *)pTex, &desc);

	ID3D11Resource	*pRes;
	pTex->lpVtbl->QueryInterface((IDirect3DTexture8 *)pTex, &IID_ID3D11Resource, (void **)&pRes);
	if(pRes == NULL)
	{
		printf("Error getting resource interface from a texture!\n");
		return;
	}

	ID3D11ShaderResourceView	*pSRV	=GD_CreateSRV(pCon->mpGD, pRes, desc.Format);
	if(pSRV != NULL)
	{
		DictSZ_Add(&pCon->mpSK->mpSRVs, pKey, pSRV);
	}

	pRes->lpVtbl->Release(pRes);
}

static void	CreateFontSRVCB(const UT_string *pKey, const void *pValue, void *pContext)
{
	StuffContext	*pCon	=pContext;

	const IDirect3DTexture8	*pTex	=pValue;

	D3D11_TEXTURE2D_DESC	desc;
	pTex->lpVtbl->GetDesc((IDirect3DTexture8 *)pTex, &desc);

	ID3D11Resource	*pRes;
	pTex->lpVtbl->QueryInterface((IDirect3DTexture8 *)pTex, &IID_ID3D11Resource, (void **)&pRes);
	if(pRes == NULL)
	{
		printf("Error getting resource interface from a texture!\n");
		return;
	}

	ID3D11ShaderResourceView	*pSRV	=GD_CreateSRV(pCon->mpGD, pRes, desc.Format);
	if(pSRV != NULL)
	{
		DictSZ_Add(&pCon->mpSK->mpFontSRVs, pKey, pSRV);
	}

	pRes->lpVtbl->Release(pRes);
}


static void CreateShadersFromCode(StuffKeeper *pSK, GraphicsDevice *pGD)
{
	StuffContext	sc;

	sc.mpGD	=pGD;
	sc.mpSK	=pSK;
	
	DictSZ_ForEach(pSK->mpVSCode, CreateVShaderCB, &sc);
	DictSZ_ForEach(pSK->mpPSCode, CreatePShaderCB, &sc);
}


void LoadFonts(GraphicsDevice *pGD, StuffKeeper *pSK)
{
	if(!FileStuff_DirExists("Fonts"))
	{
		return;
	}

	UT_string	*pFontDir, *pFilePath;
	utstring_new(pFontDir);
	utstring_new(pFilePath);

	utstring_printf(pFontDir, "%s", "Fonts");
	
	DIR	*pDir	=opendir(utstring_body(pFontDir));
	for(;;)
	{
		struct dirent	*pDE	=readdir(pDir);
		if(pDE == NULL)
		{
			break;
		}

		int	len	=strlen(pDE->d_name);
		if(len < 3)
		{
			continue;	//probably . or ..
		}

		UT_string	*pExt	=SZ_GetExtension(pDE->d_name);
		if(pExt == NULL)
		{
			continue;
		}

		//png?
		int	upper	=strncmp(utstring_body(pExt), ".PNG", utstring_len(pExt));
		int	lower	=strncmp(utstring_body(pExt), ".png", utstring_len(pExt));

		utstring_done(pExt);

		if(upper && lower)
		{
			continue;
		}

		utstring_clear(pFilePath);
		utstring_printf(pFilePath, "%s/%s", utstring_body(pFontDir), pDE->d_name);

		struct stat	fileStuff;
		int	res	=stat(utstring_body(pFilePath), &fileStuff);
		if(res)
		{
			FileStuff_PrintErrno(res);
			continue;
		}

		//regular file?
		if(S_ISREG(fileStuff.st_mode))
		{
			//isolate the filename without the extension
			UT_string	*pExtLess	=SZ_StripExtensionUT(pFilePath);
			UT_string	*pJustName	=SZ_SubStringUTStart(pExtLess, 6);

			IDirect3DTexture8	*pTex	=LoadTexture(pGD, pFilePath);
			if(pTex != NULL)
			{
				DictSZ_Add(&pSK->mpFontTextures, pJustName, pTex);

				//make the actual font
				utstring_clear(pFilePath);
				utstring_printf(pFilePath, "%s.dat", utstring_body(pExtLess));

				Font	*pFont	=Font_Create(pFilePath);
				if(pFont == NULL)
				{
					printf("Error reading font %s\n", utstring_body(pFilePath));
				}
				else
				{
					DictSZ_Add(&pSK->mpFonts, pJustName, pFont);
				}
			}
			utstring_done(pExtLess);
			utstring_done(pJustName);
		}
	}
	closedir(pDir);

	utstring_done(pFontDir);
	utstring_done(pFilePath);
}


static void	CreateSRVs(GraphicsDevice *pGD, StuffKeeper *pSK)
{
	StuffContext	sc;

	sc.mpGD	=pGD;
	sc.mpSK	=pSK;
	
	DictSZ_ForEach(pSK->mpTextures, CreateSRVCB, &sc);
	DictSZ_ForEach(pSK->mpFontTextures, CreateFontSRVCB, &sc);
}


static void MakeCommonRenderStates(GraphicsDevice *pGD, StuffKeeper *pSK)
{
	D3D11_SAMPLER_DESC	sampDesc;

	sampDesc.AddressU			=D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV			=D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW			=D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.BorderColor[0]		=1.0f;	//never use this
	sampDesc.BorderColor[1]		=0.0f;	//never use this
	sampDesc.BorderColor[2]		=0.0f;	//never use this
	sampDesc.BorderColor[3]		=1.0f;	//never use this
	sampDesc.ComparisonFunc		=D3D11_COMPARISON_NEVER;
	sampDesc.Filter				=D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.MaxAnisotropy		=16;
	sampDesc.MaxLOD				=0;//D3D11_FLOAT32_MAX;
	sampDesc.MinLOD				=0;
	sampDesc.MipLODBias			=0.0f;

	ID3D11SamplerState	*pSS	=GD_CreateSamplerState(pGD, &sampDesc);
	if(pSS == NULL)
	{
		printf("Error creating sampler state!\n");
		return;
	}
	DictSZ_Addccp(&pSK->mpSSs, "LinearClamp", pSS);

	//make same but wrapping textures
	sampDesc.AddressU	=D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV	=D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW	=D3D11_TEXTURE_ADDRESS_WRAP;
	pSS	=GD_CreateSamplerState(pGD, &sampDesc);
	if(pSS == NULL)
	{
		printf("Error creating sampler state!\n");
		return;
	}
	DictSZ_Addccp(&pSK->mpSSs, "LinearWrap", pSS);

	//point filter for a pixelly look
	sampDesc.Filter	=D3D11_FILTER_MIN_MAG_MIP_POINT;
	pSS	=GD_CreateSamplerState(pGD, &sampDesc);
	if(pSS == NULL)
	{
		printf("Error creating sampler state!\n");
		return;
	}
	DictSZ_Addccp(&pSK->mpSSs, "PointWrap", pSS);

	//point clamp
	sampDesc.AddressU	=D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV	=D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW	=D3D11_TEXTURE_ADDRESS_CLAMP;
	pSS	=GD_CreateSamplerState(pGD, &sampDesc);
	if(pSS == NULL)
	{
		printf("Error creating sampler state!\n");
		return;
	}
	DictSZ_Addccp(&pSK->mpSSs, "PointClamp", pSS);

	//depth stencils
	D3D11_DEPTH_STENCILOP_DESC	dsd;
	dsd.StencilDepthFailOp	=D3D11_STENCIL_OP_KEEP;
	dsd.StencilFailOp		=D3D11_STENCIL_OP_KEEP;
	dsd.StencilPassOp		=D3D11_STENCIL_OP_KEEP;
	dsd.StencilFunc			=D3D11_COMPARISON_ALWAYS;

	D3D11_DEPTH_STENCIL_DESC	dssDesc;
	dssDesc.DepthEnable			=true;
	dssDesc.DepthWriteMask		=D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc			=D3D11_COMPARISON_LESS;
	dssDesc.StencilEnable		=false;
	dssDesc.StencilReadMask		=0;
	dssDesc.StencilWriteMask	=0;
	dssDesc.FrontFace			=dsd;
	dssDesc.BackFace			=dsd;

	ID3D11DepthStencilState	*pDSS	=GD_CreateDepthStencilState(pGD, &dssDesc);
	if(pDSS == NULL)
	{
		printf("Error creating depth stencil state!\n");
		return;
	}
	DictSZ_Addccp(&pSK->mpDSSs, "EnableDepth", pDSS);

	//equal test for shadows
	dssDesc.DepthWriteMask	=D3D11_DEPTH_WRITE_MASK_ZERO;
	dssDesc.DepthFunc		=D3D11_COMPARISON_EQUAL;
	pDSS	=GD_CreateDepthStencilState(pGD, &dssDesc);
	if(pDSS == NULL)
	{
		printf("Error creating depth stencil state!\n");
		return;
	}
	DictSZ_Addccp(&pSK->mpDSSs, "ShadowDepth", pDSS);

	//no depth
	dssDesc.DepthEnable	=false;
	dssDesc.DepthFunc	=D3D11_COMPARISON_ALWAYS;
	pDSS	=GD_CreateDepthStencilState(pGD, &dssDesc);
	if(pDSS == NULL)
	{
		printf("Error creating depth stencil state!\n");
		return;
	}
	DictSZ_Addccp(&pSK->mpDSSs, "DisableDepth", pDSS);

	//no depth write
	dssDesc.DepthEnable	=true;
	dssDesc.DepthFunc	=D3D11_COMPARISON_LESS;
	pDSS	=GD_CreateDepthStencilState(pGD, &dssDesc);
	if(pDSS == NULL)
	{
		printf("Error creating depth stencil state!\n");
		return;
	}
	DictSZ_Addccp(&pSK->mpDSSs, "DisableDepthWrite", pDSS);

	//no depth test
	dssDesc.DepthWriteMask	=D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc		=D3D11_COMPARISON_ALWAYS;
	pDSS	=GD_CreateDepthStencilState(pGD, &dssDesc);
	if(pDSS == NULL)
	{
		printf("Error creating depth stencil state!\n");
		return;
	}
	DictSZ_Addccp(&pSK->mpDSSs, "DisableDepthTest", pDSS);

	//blend states... all of these will set a single target
	//and use that if multiple rendertargets are in use
	//but each one can have a seperate if needed
	D3D11_RENDER_TARGET_BLEND_DESC	rtbDesc, nullDesc={0};
	rtbDesc.BlendEnable				=true;
	rtbDesc.SrcBlend				=D3D11_BLEND_ONE;
	rtbDesc.DestBlend				=D3D11_BLEND_INV_SRC_ALPHA;
	rtbDesc.BlendOp					=D3D11_BLEND_OP_ADD;
	rtbDesc.SrcBlendAlpha			=D3D11_BLEND_ZERO;
	rtbDesc.DestBlendAlpha			=D3D11_BLEND_ZERO;
	rtbDesc.BlendOpAlpha			=D3D11_BLEND_OP_ADD;
	rtbDesc.RenderTargetWriteMask	=D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_BLEND_DESC	blendDesc;
	blendDesc.AlphaToCoverageEnable		=false;
	blendDesc.IndependentBlendEnable	=false;	//set this to true for using all 8 different
	blendDesc.RenderTarget[0]			=rtbDesc;
	blendDesc.RenderTarget[1]			=nullDesc;
	blendDesc.RenderTarget[2]			=nullDesc;
	blendDesc.RenderTarget[3]			=nullDesc;
	blendDesc.RenderTarget[4]			=nullDesc;
	blendDesc.RenderTarget[5]			=nullDesc;
	blendDesc.RenderTarget[6]			=nullDesc;
	blendDesc.RenderTarget[7]			=nullDesc;

	ID3D11BlendState	*pBS	=GD_CreateBlendState(pGD, &blendDesc);
	if(pBS == NULL)
	{
		printf("Error creating blend state!\n");
		return;
	}
	DictSZ_Addccp(&pSK->mpBlends, "AlphaBlending", pBS);

	//multichannedepth I think this is for my 8 point shadows at once attempt
	rtbDesc.SrcBlend		=D3D11_BLEND_ONE;
	rtbDesc.DestBlend		=D3D11_BLEND_ONE;
	rtbDesc.BlendOp			=D3D11_BLEND_OP_MIN;
	rtbDesc.SrcBlendAlpha	=D3D11_BLEND_ONE;
	rtbDesc.DestBlendAlpha	=D3D11_BLEND_ONE;
	rtbDesc.BlendOpAlpha	=D3D11_BLEND_OP_MIN;

	blendDesc.RenderTarget[0]	=rtbDesc;
	pBS	=GD_CreateBlendState(pGD, &blendDesc);
	if(pBS == NULL)
	{
		printf("Error creating blend state!\n");
		return;
	}
	DictSZ_Addccp(&pSK->mpBlends, "MultiChannelDepth", pBS);

	//typical shadow map blending
	rtbDesc.BlendOp			=D3D11_BLEND_OP_REV_SUBTRACT;
	rtbDesc.BlendOpAlpha	=D3D11_BLEND_OP_ADD;

	blendDesc.RenderTarget[0]	=rtbDesc;
	pBS	=GD_CreateBlendState(pGD, &blendDesc);
	if(pBS == NULL)
	{
		printf("Error creating blend state!\n");
		return;
	}
	DictSZ_Addccp(&pSK->mpBlends, "ShadowBlending", pBS);

	rtbDesc.BlendEnable		=false;
	
	blendDesc.RenderTarget[0]	=rtbDesc;
	pBS	=GD_CreateBlendState(pGD, &blendDesc);
	if(pBS == NULL)
	{
		printf("Error creating blend state!\n");
		return;
	}
	DictSZ_Addccp(&pSK->mpBlends, "NoBlending", pBS);
}


static void	CreateInputLayouts(GraphicsDevice *pGD, StuffKeeper *pSK)
{
	//load EntryLayouts, this maps from a shader entry point
	//to a layout name (such as VPosNormBone etc...)
	FILE	*f	=fopen("Shaders/EntryLayouts.txt", "r");
	if(f == NULL)
	{
		printf("Couldn't open entry layouts file!\n");
		return;
	}
	ReadEntryLayouts(f, &pSK->mpEntryLayouts);

	//fill the layouts dictionary
	MakeLayouts(pGD, &pSK->mpLayouts, pSK->mpVSCode);
}
*/

StuffKeeper	*StuffKeeper_Create(GraphicsDevice *pGD)
{
	int			numTex, numFonts;
	StuffKeeper	*pRet	=malloc(sizeof(StuffKeeper));

	//prepare all dictionaries for new data
	DictSZ_New(&pRet->mpTextures);
	DictSZ_New(&pRet->mpFonts);
	DictSZ_New(&pRet->mpFontTextures);

	numTex			=DictSZ_Count(pRet->mpTextures);
	numFonts		=DictSZ_Count(pRet->mpFonts);

	return	pRet;
}

IDirect3DTexture8	*StuffKeeper_GetTexture2D(StuffKeeper *pSK, const char *pName)
{
	return	DictSZ_GetValueccp(pSK->mpTextures, pName);
}
