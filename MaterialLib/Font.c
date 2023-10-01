#include	<xtl.h>
#include	<stdio.h>
#include	<ctype.h>
#include	<assert.h>
#include	<utstring.h>


typedef struct	Font_t
{
	int		mWidth, mHeight;
	int		mCellWidth, mCellHeight;
	BYTE	mStartChar;

	BYTE	*mpWidths;
}	Font;


Font	*Font_CreateCCP(const char *pPath)
{
	Font	*pRet;
	FILE	*f	=fopen(pPath, "rb");
	if(f == NULL)
	{
		return	NULL;
	}

	pRet	=malloc(sizeof(Font));

	fread(&pRet->mWidth, sizeof(int), 1, f);
	fread(&pRet->mHeight, sizeof(int), 1, f);
	fread(&pRet->mCellWidth, sizeof(int), 1, f);
	fread(&pRet->mCellHeight, sizeof(int), 1, f);
	fread(&pRet->mStartChar, sizeof(BYTE), 1, f);

	pRet->mpWidths	=malloc(255);

	fread(pRet->mpWidths, 1, 255, f);

	fclose(f);

	return	pRet;
}

Font	*Font_Create(UT_string *pPath)
{
	return	Font_CreateCCP(utstring_body(pPath));
}

BYTE	Font_GetCharacterWidth(Font *pFont, char c)
{
	return	pFont->mpWidths[c];
}

BYTE	Font_GetCharacterHeight(Font *pFont)
{
	return	pFont->mCellHeight;
}

D3DXVECTOR2	Font_GetUV(Font *pFont, char letter, int quadIndex)
{
	int	posOffset	=letter - pFont->mStartChar;
	int	numColumns	=pFont->mWidth / pFont->mCellWidth;
	int	yOffset		=0;
	int	charWidth	=Font_GetCharacterWidth(pFont, letter);

	D3DXVECTOR2	ret	={	0.0f, 0.0f	};

	if(posOffset < 0)
	{
		return	ret;
	}

	while(posOffset >= numColumns)
	{
		yOffset++;
		posOffset	-=numColumns;
	}

	ret.x	=posOffset * pFont->mCellWidth;
	ret.y	=yOffset * pFont->mCellHeight;

	switch(quadIndex)
	{
		case	0:	//topleft
			break;

		case	1:	//topright
			ret.x	+=charWidth;
			break;

		case	2:	//bottomright
			ret.x	+=charWidth;
			ret.y	+=pFont->mCellHeight;
			break;

		case	3:	//bottomleft
			ret.y	+=pFont->mCellHeight;
			break;
	}

	ret.x	/=pFont->mWidth;
	ret.y	/=pFont->mHeight;

	return	ret;
}