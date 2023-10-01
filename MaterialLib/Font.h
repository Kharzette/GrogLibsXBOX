#pragma once
#include	<xtl.h>
#include	"utstring.h"

typedef struct	Font_t	Font;

extern Font			*Font_Create(UT_string *pPath);
extern Font			*Font_CreateCCP(const char *pPath);
extern BYTE			Font_GetCharacterWidth(Font *pFont, char c);
extern BYTE			Font_GetCharacterHeight(Font *pFont);
extern D3DXVECTOR2	Font_GetUV(Font *pFont, char letter, int quadIndex);