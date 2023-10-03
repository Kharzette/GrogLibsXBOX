#pragma once
#include	<XTL.h>

typedef struct	Physics_t		Physics;

extern Physics	*Physics_Init();

extern void	Physics_Update(Physics *pPhys, float dt);

extern void	Physics_ClearVerticalMomentum(Physics *pPhys);
extern void	Physics_SetProps(Physics *pPhys, float mass, float it, float friction);
extern void	Physics_SetFriction(Physics *pPhys, float friction);
extern void	Physics_SetPosition(Physics *pPhys, const D3DXVECTOR3 *pPos);
extern void	Physics_ApplyForce(Physics *pPhys, const D3DXVECTOR3 *pForce);
extern void	Physics_ApplyTorque(Physics *pPhys, const D3DXVECTOR3 *pAxis);

extern void	Physics_GetVelocity(Physics *pPhys, D3DXVECTOR3 *pOutVel);
extern void	Physics_GetPosition(Physics *pPhys, D3DXVECTOR3 *pOutPos);
extern void	Physics_GetOrient(Physics *pPhys, D3DXQUATERNION *pOutOrient);