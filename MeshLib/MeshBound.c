#include	<xtl.h>
#include	<stdio.h>
#include	<string.h>


typedef struct	MeshBound_t
{
	D3DXVECTOR4	mSphere;	//center + radius
	D3DXVECTOR3	mBox[2];
	BOOL		mbChoice;	//true for box

	//bound for each part
	D3DXVECTOR3	**mpPartBoxes;
	D3DXVECTOR4	*mpPartSpheres;
	BOOL		*mpChoices;
}	MeshBound;


MeshBound	*MeshBound_Read(FILE *f)
{
	BYTE		choice;
	int			numParts, i;
	MeshBound	*pRet	=malloc(sizeof(MeshBound));

	fread(&pRet->mSphere, sizeof(D3DXVECTOR4), 1, f);
	fread(&pRet->mBox, sizeof(D3DXVECTOR3), 2, f);

	choice;
	fread(&choice, 1, 1, f);	//not sure the size of C# bool

	pRet->mbChoice	=(choice != 0);

	numParts;
	fread(&numParts, sizeof(int), 1, f);

	if(numParts == 0)
	{
		return	pRet;
	}

	pRet->mpPartBoxes	=malloc(sizeof(D3DXVECTOR3 *) * numParts);
	pRet->mpPartSpheres	=malloc(sizeof(D3DXVECTOR4) * numParts);
	pRet->mpChoices		=malloc(sizeof(BOOL) * numParts);

	for(i=0;i < numParts;i++)
	{
		pRet->mpPartBoxes[i]	=malloc(sizeof(D3DXVECTOR3) * 2);

		fread(&pRet->mpPartBoxes[i], sizeof(D3DXVECTOR3), 2, f);
		fread(&pRet->mpPartSpheres[i], sizeof(D3DXVECTOR4), 1, f);
		fread(&pRet->mpChoices[i], sizeof(BOOL), 1, f);
	}

	return	pRet;
}