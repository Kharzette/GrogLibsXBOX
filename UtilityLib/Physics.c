#include	<XTL.h>

//this is all derived from
//https://gafferongames.com/post/integration_basics/
//which is a super good article

typedef struct	PhyState_t
{
	D3DXVECTOR3		mPosition, mMomentum, mAngMomentum;
	D3DXQUATERNION	mOrient;

	D3DXVECTOR3		mVelocity, mAngVelocity;
	D3DXQUATERNION	mSpin;

	float	mMass, mInertiaTensor, mFriction;
}	PhyState;

typedef struct	Derivative_t
{
	D3DXVECTOR3		mVelocity, mForce, mTorque;
	D3DXQUATERNION	mSpin;
}	Derivative;

typedef struct	Physics_t
{
	PhyState	mPrev, mCur;

	//external forces
	D3DXVECTOR3	mExternalDV;
}	Physics;

static void	Recalculate(PhyState *p)
{
	D3DXVec3Scale(&p->mVelocity, &p->mMomentum, 1.0f / p->mMass);
	D3DXVec3Scale(&p->mAngVelocity, &p->mAngMomentum, 1.0f / p->mInertiaTensor);

	D3DXQuaternionNormalize(&p->mOrient, &p->mOrient);

	p->mSpin.x	=p->mAngVelocity.x * 0.5f;
	p->mSpin.y	=p->mAngVelocity.y * 0.5f;
	p->mSpin.z	=p->mAngVelocity.z * 0.5f;
	p->mSpin.w	=0.0f;

	p->mSpin.x	*=p->mOrient.x;
	p->mSpin.y	*=p->mOrient.y;
	p->mSpin.z	*=p->mOrient.z;
	p->mSpin.w	*=p->mOrient.w;
}

static void Forces(Physics *pPhys, const PhyState *pState, D3DXVECTOR3 *pForce, D3DXVECTOR3 *pTorque)
{
	//friction
	D3DXVECTOR3	frictionForce, frictionTorque;
	D3DXVec3Scale(&frictionForce, &pState->mVelocity, pState->mMass);
	D3DXVec3Scale(&frictionForce, &frictionForce, -pState->mFriction);

	D3DXVec3Scale(&frictionTorque, &pState->mAngVelocity, pState->mMass);
	D3DXVec3Scale(&frictionTorque, &frictionTorque, -pState->mInertiaTensor);

	D3DXVec3Add(pForce, &pPhys->mExternalDV, &frictionForce);
	pTorque->x	=pTorque->y	=pTorque->z	=0.0f;

	//clear after update
	pPhys->mExternalDV.x	=pPhys->mExternalDV.y	=pPhys->mExternalDV.z	=0.0f;
}

static void	Evaluate(const Physics *pPhys, const PhyState *pState, float dt, Derivative *pDer)
{
	pDer->mVelocity	=pState->mVelocity;
	pDer->mSpin		=pState->mSpin;

	Forces(pPhys, pState, &pDer->mForce, &pDer->mTorque);
}

static void Evaluate2(const Physics *pPhys, PhyState *pState, float dt,
	const Derivative *pDerIn, Derivative *pDerOut)
{
	D3DXVECTOR3	scaled;

	//pos +=derIn.velocity * dt
	D3DXVec3Scale(&scaled, &pDerIn->mVelocity, dt);
	D3DXVec3Add(&pState->mPosition, &scaled, &pState->mPosition);

	//momentum +=derIn.force * dt
	D3DXVec3Scale(&scaled, &pDerIn->mForce, dt);
	D3DXVec3Add(&pState->mMomentum, &scaled, &pState->mMomentum);

	pState->mOrient.x	+=pDerIn->mSpin.x * dt;
	pState->mOrient.y	+=pDerIn->mSpin.y * dt;
	pState->mOrient.z	+=pDerIn->mSpin.z * dt;
	pState->mOrient.w	+=pDerIn->mSpin.w * dt;

	//angmomentum +=derIn.torque * dt
	D3DXVec3Scale(&scaled, &pDerIn->mTorque, dt);
	D3DXVec3Add(&pState->mAngMomentum, &scaled, &pState->mAngMomentum);

	Recalculate(pState);

	pDerOut->mVelocity	=pState->mVelocity;
	pDerOut->mSpin		=pState->mSpin;

	Forces(pPhys, pState, &pDerOut->mForce, &pDerOut->mTorque);
}


static void	Integrate(const Physics *pPhys, PhyState *pState, float dt)
{
	D3DXVECTOR3	accum;
	Derivative	a, b, c, d;

	Evaluate(pPhys, pState, dt, &a);
	Evaluate2(pPhys, pState, dt * 0.5f, &a, &b);
	Evaluate2(pPhys, pState, dt * 0.5f, &b, &c);
	Evaluate2(pPhys, pState, dt, &c, &d);

	//(a.mVelocity + 2.0f * (b.mVelocity + c.mVelocity) + d.mVelocity)
	D3DXVec3Add(&accum, &b.mVelocity, &c.mVelocity);
	D3DXVec3Scale(&accum, &accum, 2.0f);
	D3DXVec3Add(&accum, &accum, &d.mVelocity);
	D3DXVec3Add(&accum, &accum, &a.mVelocity);

	//pos +=(1 / 6 * dt * accum)
	D3DXVec3Scale(&accum, &accum, 1.0f / 6.0f * dt);
	D3DXVec3Add(&pState->mPosition, &pState->mPosition, &accum);

	//(a.mForce + 2.0f * (b.mForce + c.mForce) + d.mForce)
	D3DXVec3Add(&accum, &b.mForce, &c.mForce);
	D3DXVec3Scale(&accum, &accum, 2.0f);
	D3DXVec3Add(&accum, &accum, &d.mForce);
	D3DXVec3Add(&accum, &accum, &a.mForce);

	//momentum +=(1 / 6 * dt * accum)
	D3DXVec3Scale(&accum, &accum, 1.0f / 6.0f * dt);
	D3DXVec3Add(&pState->mMomentum, &pState->mMomentum, &accum);

	pState->mOrient.x	+=(a.mSpin.x + ((b.mSpin.x + c.mSpin.x) * 2.0f) + d.mSpin.x) * (1.0f / 6.0f * dt);
	pState->mOrient.y	+=(a.mSpin.y + ((b.mSpin.y + c.mSpin.y) * 2.0f) + d.mSpin.y) * (1.0f / 6.0f * dt);
	pState->mOrient.z	+=(a.mSpin.z + ((b.mSpin.z + c.mSpin.z) * 2.0f) + d.mSpin.z) * (1.0f / 6.0f * dt);
	pState->mOrient.w	+=(a.mSpin.w + ((b.mSpin.w + c.mSpin.w) * 2.0f) + d.mSpin.w) * (1.0f / 6.0f * dt);

	//(a.mTorque + 2.0f * (b.mTorque + c.mTorque) + d.mTorque)
	D3DXVec3Add(&accum, &b.mTorque, &c.mTorque);
	D3DXVec3Scale(&accum, &accum, 2.0f);
	D3DXVec3Add(&accum, &accum, &d.mTorque);
	D3DXVec3Add(&accum, &accum, &a.mTorque);

	//angmomentum +=(1 / 6 * dt * accum)
	D3DXVec3Scale(&accum, &accum, 1.0f / 6.0f * dt);
	D3DXVec3Add(&pState->mAngMomentum, &pState->mAngMomentum, &accum);

	Recalculate(pState);
}


Physics	*Physics_Init()
{
	Physics	*pRet	=malloc(sizeof(Physics));

	memset(pRet, 0, sizeof(Physics));

	D3DXQuaternionIdentity(&pRet->mCur.mOrient);

	return	pRet;
}

void	Physics_Update(Physics *pPhys, float dt)
{
	pPhys->mPrev	=pPhys->mCur;

	Integrate(pPhys, &pPhys->mCur, dt);
}

//I think this is for biped stuff?
void	Physics_ClearVerticalMomentum(Physics *pPhys)
{
	pPhys->mCur.mMomentum.y	=0.0f;
}

void	Physics_SetProps(Physics *pPhys, float mass, float it, float friction)
{
	pPhys->mCur.mMass			=mass;
	pPhys->mCur.mInertiaTensor	=it;
	pPhys->mCur.mFriction		=friction;

	Recalculate(&pPhys->mCur);
	pPhys->mPrev	=pPhys->mCur;
}

void	Physics_SetFriction(Physics *pPhys, float friction)
{
	pPhys->mCur.mFriction	=friction;
}

void	Physics_SetPosition(Physics *pPhys, const D3DXVECTOR3 *pPos)
{
	pPhys->mCur.mPosition	=*pPos;
}

void	Physics_ApplyForce(Physics *pPhys, const D3DXVECTOR3 *pForce)
{
	D3DXVECTOR3	forceOverMass;

	D3DXVec3Scale(&forceOverMass, pForce, 1.0f / pPhys->mCur.mMass);

	D3DXVec3Add(&pPhys->mExternalDV, &pPhys->mExternalDV, &forceOverMass);
}

void	Physics_GetVelocity(const Physics *pPhys, D3DXVECTOR3 *pOutVel)
{
	*pOutVel	=pPhys->mCur.mVelocity;
}

void	Physics_GetPosition(const Physics *pPhys, D3DXVECTOR3 *pOutPos)
{
	*pOutPos	=pPhys->mCur.mPosition;
}

void	Physics_GetOrient(const Physics *pPhys, D3DXQUATERNION *pOutOrient)
{
	*pOutOrient	=pPhys->mCur.mOrient;
}