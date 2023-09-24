#include	<xtl.h>
#include	<stdio.h>
#include	<ctype.h>
#include	"utstring.h"


//an attempt to do C# style string manip stuffs
//want StartsWith, trim, IndexOf, GetExtension, StripExtension
//ConvertPathSlashes, ConvertPathBackSlashes, StringToVec234,
//StringToMatrix, FloatToString, FloatArrayToString, VecToString,


//does pSZ start with Thing?
BOOL	SZ_StartsWith(const char *pSZ, const char *pszThing)
{
	int	len, cmp;

	if(pSZ == NULL || pszThing == NULL)
	{
		return	FALSE;
	}

	len	=strlen(pszThing);

	if(len <= 0)
	{
		return	FALSE;
	}

	cmp	=strncmp(pSZ, pszThing, len);

	return	(cmp == 0);
}

BOOL	SZ_StartsWithCCUT(const char *pSZ, const UT_string *pszThing)
{
	if(pSZ == NULL || pszThing == NULL)
	{
		return	FALSE;
	}
	return	SZ_StartsWith(pSZ, utstring_body(pszThing));
}

BOOL	SZ_StartsWithUTUT(const UT_string *pSZ, const UT_string *pszThing)
{
	if(pSZ == NULL || pszThing == NULL)
	{
		return	FALSE;
	}
	return	SZ_StartsWith(utstring_body(pSZ), utstring_body(pszThing));
}

BOOL	SZ_StartsWithUTCC(const UT_string *pSZ, const char *pszThing)
{
	if(pSZ == NULL || pszThing == NULL)
	{
		return	FALSE;
	}
	return	SZ_StartsWith(utstring_body(pSZ), pszThing);
}


//trim spaces, tabs, and junx from start and end, allocs a new copy
UT_string	*SZ_Trim(const char *pSZ)
{
	int			i, newStart, newEnd;
	int			newLen, len	=strlen(pSZ);
	UT_string	*pRet;
	char		*pRetBody;

	//trim start
	newStart	=0;
	for(i=0;i < len;i++)
	{
		if(isgraph(pSZ[i]))
		{
			newStart	=i;
			break;
		}
	}

	//trim end
	newEnd	=len - 1;
	for(i=len-1;i >= 0;i--)
	{
		if(isgraph(pSZ[i]))
		{
			newEnd	=i;
			break;
		}
	}

	utstring_new(pRet);

	//length of trimmed segment + space for null terminator
	newLen	=newEnd - newStart + 2;

	utstring_reserve(pRet, newLen);
	pRetBody	=utstring_body(pRet);

	//copy trimmed contents
	memcpy(pRetBody, pSZ + newStart, newLen - 1);

	//null terminate
	pRetBody[newLen - 1]	=0;

	//set internal length var
	pRet->i	=strlen(utstring_body(pRet));

	return	pRet;
}

UT_string	*SZ_TrimUT(const UT_string *pSZ)
{
	char	*pBody	=utstring_body(pSZ);

	return	SZ_Trim(pBody);
}


//find the index of cThing in pSZ, -1 if not found
int	SZ_IndexOf(const char *pSZ, char cThing)
{
	char	*pSpot;
	if(pSZ == NULL)
	{
		return	-1;
	}

	pSpot	=strchr(pSZ, cThing);
	if(pSpot == NULL)
	{
		return	-1;
	}
	return	(pSpot - pSZ);
}

int	SZ_IndexOfUT(const UT_string *pSZ, char cThing)
{
	return	SZ_IndexOf(utstring_body(pSZ), cThing);
}


//find the last index of cThing in pSZ, -1 if not found
int	SZ_LastIndexOf(const char *pSZ, char cThing)
{
	char	*pSpot;
	if(pSZ == NULL)
	{
		return	-1;
	}

	pSpot	=strrchr(pSZ, cThing);
	if(pSpot == NULL)
	{
		return	-1;
	}
	return	(pSpot - pSZ);
}

int	SZ_LastIndexOfUT(const UT_string *pSZ, char cThing)
{
	return	SZ_LastIndexOf(utstring_body(pSZ), cThing);
}


//return the substring beginning at startPos or NULL
UT_string	*SZ_SubStringStart(const char *pSZ, int startPos)
{
	int			len;
	UT_string	*pRet;

	if(pSZ == NULL)
	{
		return	NULL;
	}

	len	=strlen(pSZ);
	if(startPos >= len)
	{
		return	NULL;
	}

	utstring_new(pRet);

	utstring_printf(pRet, "%s", pSZ + startPos);

	return	pRet;
}

UT_string	*SZ_SubStringStartEnd(const char *pSZ, int startPos, int endPos)
{
	int			len, newLen;
	UT_string	*pRet;

	if(pSZ == NULL)
	{
		return	NULL;
	}

	len		=strlen(pSZ);
	newLen	=endPos - startPos;
	if(startPos >= len || endPos >= len || newLen >= len)
	{
		return	NULL;
	}


	utstring_new(pRet);
	utstring_reserve(pRet, newLen + 1);

	memcpy(utstring_body(pRet), pSZ + startPos, newLen);

	utstring_body(pRet)[newLen]	=0;	//null terminate
	
	//set internal length var
	pRet->i	=strlen(utstring_body(pRet));

	return	pRet;
}

UT_string	*SZ_SubStringUTStart(const UT_string *pSZ, int startPos)
{
	return	SZ_SubStringStart(utstring_body(pSZ), startPos);
}

UT_string	*SZ_SubStringUTStartEnd(const UT_string *pSZ, int startPos, int endPos)
{
	return	SZ_SubStringStartEnd(utstring_body(pSZ), startPos, endPos);
}


//return the extension of a filename or NULL if none
UT_string	*SZ_GetExtension(const char *pSZ)
{
	int			dotPos;
	UT_string	*pRet;

	if(pSZ == NULL)
	{
		return	NULL;
	}

	dotPos	=SZ_LastIndexOf(pSZ, '.');
	if(dotPos == -1)
	{
		return	NULL;
	}

	utstring_new(pRet);

	utstring_printf(pRet, "%s", pSZ + dotPos);

	return	pRet;
}

UT_string	*SZ_GetExtensionUT(UT_string *pSZ)
{
	return	SZ_GetExtension(utstring_body(pSZ));
}


//remove the extension from the filename if there is one
//Returns a new string or NULL if the input was NULL
UT_string	*SZ_StripExtension(const char *pSZ)
{
	int			dotPos;
	UT_string	*pRet;

	if(pSZ == NULL)
	{
		return	NULL;
	}

	utstring_new(pRet);

	//copy
	utstring_printf(pRet, "%s", pSZ);

	dotPos	=SZ_LastIndexOf(pSZ, '.');
	if(dotPos == -1)
	{
		return	pRet;
	}

	//null terminate at .
	utstring_body(pRet)[dotPos]	=0;

	//set internal length var
	pRet->i	=strlen(utstring_body(pRet));

	return	pRet;
}

UT_string	*SZ_StripExtensionUT(const UT_string *pSZ)
{
	return	SZ_StripExtension(utstring_body(pSZ));
}


//convert a path's slashes from \ to /, returns a new utstring or NULL if pSZ is NULL
UT_string	*SZ_ConvertPathSlashes(const char *pSZ)
{
	UT_string	*pRet;

	if(pSZ == NULL)
	{
		return	NULL;
	}

	utstring_new(pRet);

	//copy
	utstring_printf(pRet, "%s", pSZ);

	for(;;)
	{
		int	idx	=SZ_IndexOfUT(pRet, '\\');
		if(idx == -1)
		{
			return	pRet;
		}

		//replace
		utstring_body(pRet)[idx]	='/';
	}
	return	pRet;
}

UT_string	*SZ_ConvertPathSlashesUT(const UT_string *pSZ)
{
	return	SZ_ConvertPathSlashes(utstring_body(pSZ));
}

//convert a path's slashes from / to \, returns a new utstring or NULL if pSZ is NULL
UT_string	*SZ_ConvertPathBackSlashes(const char *pSZ)
{
	UT_string	*pRet;

	if(pSZ == NULL)
	{
		return	NULL;
	}

	utstring_new(pRet);

	//copy
	utstring_printf(pRet, "%s", pSZ);

	for(;;)
	{
		int	idx	=SZ_IndexOfUT(pRet, '/');
		if(idx == -1)
		{
			return	pRet;
		}

		//replace
		utstring_body(pRet)[idx]	='\\';
	}
	return	pRet;
}

UT_string	*SZ_ConvertPathBackSlashesUT(const UT_string *pSZ)
{
	return	SZ_ConvertPathBackSlashes(utstring_body(pSZ));
}

//I really hate non ascii
//caller responsible for freeing
wchar_t	*SZ_ConvertToWCHAR(const UT_string *pSZ)
{
	int	len	=utstring_len(pSZ);

	wchar_t	*pBuf	=malloc(sizeof(wchar_t) * (len + 1));
	mbstowcs(pBuf, utstring_body(pSZ), len);

	return	pBuf;
}

UT_string	*SZ_ReadString(FILE *f)
{
	UT_string	*pRet;
	BYTE		len;

	fread(&len, 1, 1, f);

	pRet	=malloc(sizeof(UT_string));
	pRet->d	=calloc(len + 1, 1);

	fread(pRet->d, 1, len, f);

	pRet->i	=len;
	pRet->n	=len + 1;

	return	pRet;
}