#pragma once


typedef struct	Mesh_t				Mesh;
typedef struct	GraphicsDevice_t	GraphicsDevice;


Mesh	*Mesh_Read(GraphicsDevice *pGD, const char *szFileName);
void	Mesh_Draw(Mesh *pMesh, GraphicsDevice *pGD);