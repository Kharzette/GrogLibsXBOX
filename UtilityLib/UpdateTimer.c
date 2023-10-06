#include	<xtl.h>
#include	<assert.h>
#include	<minmax.h>


//This time helper works by measuring the time a "frame" took
//and dishing out slices of time for use by updates.
//
//Update is for things like physics that use discreet slices to
//approximate something continuous, like the parabola of a jump arc.
//
//RenderUpdate is always an entire frame's time with no subdivision.
//This can be used for things in constant motion like a moving UV set
//for flowing lava, or a blinking cursor.
//
//A typical game loop would look like:
//while(bRunning)
//{
//	UpdateTimer_Stamp(pUT);
//	while(UpdateTimer_GetUpdateDeltaSeconds(pUT) > 0.0f)
//	{
//		HandleInputs();
//		UpdateGame();	
//		UpdateTimer_UpdateDone(pUT);
//	}
//	float	dt	=UpdateTimer_GetRenderUpdateDeltaSeconds(pUT);
//	RenderUpdate(dt);
//	DrawStuff();
//	Present()
//}
typedef	struct	UpdateTimer_t
{
	//timer frequency
	UINT64	mTicsPerSecond;

	//time stats
	UINT64	mLastTimeStamp;	//previous time
	UINT64	mTimeNow;		//time as of last Stamp() call
	UINT64	mMaxDelta;		//biggest allowed deltatime

	//time step related
	BOOL	mbFixedStep;		//use a fixed time stamp for game/physics updates?
	BOOL	mbSpendRemainder;	//spend or roll over the small remainder?
	UINT64	mStep;				//fixed step in tics
	INT64	mFullUpdateTime;	//counts down over the updates for this frame
}	UpdateTimer;


//forward declarations of stuff at the bottom
static UINT64 Delta(const UpdateTimer *pUT);
static float TicsToSeconds(const UpdateTimer *pUT, UINT64 tics);
static float TicsToMilliSeconds(const UpdateTimer *pUT, UINT64 tics);
static UINT64 SecondsToTics(const UpdateTimer *pUT, float seconds);
static UINT64 MilliSecondsToTics(const UpdateTimer *pUT, float milliSeconds);


//Fixed doles out time in predictable slices, nonfixed the delta fluctuates.
//Spend remainder is only valid for fixed mode and consumes the remaining time
//after the fixed slices have been consumed.
UpdateTimer	*UpdateTimer_Create(BOOL bFixed, BOOL bSpendRemainder)
{
	UpdateTimer		*pRet	=malloc(sizeof(UpdateTimer));
	LARGE_INTEGER	freq;
	QueryPerformanceFrequency(&freq);


	memset(pRet, 0, sizeof(UpdateTimer));

	//store freq for conversions
	pRet->mTicsPerSecond	=freq.QuadPart;

	pRet->mbFixedStep		=bFixed;
	pRet->mbSpendRemainder	=bSpendRemainder;

	pRet->mMaxDelta	=SecondsToTics(pRet, 0.1f);				//default
	pRet->mStep		=MilliSecondsToTics(pRet, 16.6666f);	//default 60hz

	//get a first time reading
	QueryPerformanceCounter(&freq);
	pRet->mTimeNow	=freq.QuadPart;

	return	pRet;
}


//Mark the top of an update loop by getting the time elapsed since
//the last time this was called.
void UpdateTimer_Stamp(UpdateTimer *pUT)
{
	LARGE_INTEGER	t;

	pUT->mLastTimeStamp	=pUT->mTimeNow;

	QueryPerformanceCounter(&t);
	pUT->mTimeNow	=t.QuadPart;

	pUT->mFullUpdateTime	+=Delta(pUT);
}


//this returns a slice of time for updating
UINT64	UpdateTimer_GetUpdateDeltaTics(const UpdateTimer *pUT)
{
	if(pUT->mbFixedStep)
	{
		if(pUT->mFullUpdateTime >= (INT64)pUT->mStep)
		{
			return	pUT->mStep;
		}

		//if spend remainder is on, return a smaller timeslice to
		//use up all of the time.  This is contrary to the thinking
		//behind fixed time stepping, so use with caution
		if(pUT->mbSpendRemainder && pUT->mFullUpdateTime > 0)
		{
			return	pUT->mFullUpdateTime;
		}

		//Here there will likely be a small piece of time left.
		//This time can be left to process the next frame, or
		//spent as a fixed timeslice that will cut into next
		//frame's time a bit.

		//I think maybe the best thing to do is see if the remainder
		//is nearer to a fixed time slice than zero, and if so, spend
		//a fixed time slice and go negative, otherwise spend it next frame
		{
			INT64	halfStep	=pUT->mStep / 2;
			if(pUT->mFullUpdateTime >= halfStep)
			{
				return	pUT->mStep;
			}
		}
		return	0L;
	}
	else
	{
		return	pUT->mFullUpdateTime;
	}
}


//render updates just return the entire deltatime in one slice
UINT64 UpdateTimer_GetRenderUpdateDeltaTics(const UpdateTimer *pUT)
{
	return	Delta(pUT);
}


//This subtracts the timeslice from FullUpdateTime, so should be
//done at the bottom of an update loop
void UpdateTimer_UpdateDone(UpdateTimer *pUT)
{
	if(pUT->mbFixedStep)
	{
		if(pUT->mFullUpdateTime >= (INT64)pUT->mStep)
		{
			pUT->mFullUpdateTime	-=pUT->mStep;
		}
		else
		{
			if(pUT->mbSpendRemainder)
			{
				pUT->mFullUpdateTime	=0;
			}
			else
			{
				//see if a fixed step was spent, sending FullUpdate negative
				INT64	halfStep	=pUT->mStep / 2;
				if(pUT->mFullUpdateTime >= halfStep)
				{
					//go negative
					pUT->mFullUpdateTime	-=pUT->mStep;
				}
			}
		}
	}
	else
	{
		pUT->mFullUpdateTime	=0;
	}

	assert(pUT->mFullUpdateTime <= (INT64)pUT->mMaxDelta);
}


//allows a user preference on the maximum
//deltatime allowed (for game stability)
void UpdateTimer_SetMaxDeltaTics(UpdateTimer *pUT, UINT64 tics)
{
	pUT->mMaxDelta	=tics;
}


//user setting for the fixed timeslice amount used
void UpdateTimer_SetFixedTimeStepTics(UpdateTimer *pUT, UINT64 tics)
{
	pUT->mStep	=tics;
}


//returns the time amount between this frame and last
static UINT64 Delta(const UpdateTimer *pUT)
{
	UINT64	delta	=pUT->mTimeNow - pUT->mLastTimeStamp;

	//In addition to extra long frames sometimes
	//(like sitting at a breakpoint), the highpart
	//of the performance counter stamp might change
	//giving a large delta.  This will guard against that.
	return	min(delta, pUT->mMaxDelta);
}


//helper functions to return delta times in convenient units
float UpdateTimer_GetUpdateDeltaSeconds(UpdateTimer *pUT)
{
	UINT64	tics	=UpdateTimer_GetUpdateDeltaTics(pUT);

	return	TicsToSeconds(pUT, tics);
}

float UpdateTimer_GetUpdateDeltaMilliSeconds(UpdateTimer *pUT)
{
	UINT64	tics	=UpdateTimer_GetUpdateDeltaTics(pUT);

	return	TicsToMilliSeconds(pUT, tics);
}

float UpdateTimer_GetRenderUpdateDeltaSeconds(UpdateTimer *pUT)
{
	return	TicsToSeconds(pUT, UpdateTimer_GetRenderUpdateDeltaTics(pUT));
}

float UpdateTimer_GetRenderUpdateDeltaMilliSeconds(UpdateTimer *pUT)
{
	return	TicsToMilliSeconds(pUT, UpdateTimer_GetRenderUpdateDeltaTics(pUT));
} 


//helper for setting time amounts in convenient units
void UpdateTimer_SetMaxDeltaMilliSeconds(UpdateTimer *pUT, float milliSeconds)
{
	pUT->mMaxDelta	=MilliSecondsToTics(pUT, milliSeconds);
}

void UpdateTimer_SetMaxDeltaSeconds(UpdateTimer *pUT, float seconds)
{
	pUT->mMaxDelta	=SecondsToTics(pUT, seconds);
}

void UpdateTimer_SetFixedTimeStepSeconds(UpdateTimer *pUT, float seconds)
{
	pUT->mStep	=SecondsToTics(pUT, seconds);
}

void UpdateTimer_SetFixedTimeStepMilliSeconds(UpdateTimer *pUT, float milliSeconds)
{
	pUT->mStep	=MilliSecondsToTics(pUT, milliSeconds);
}


//static stuff to help with unit conversion
//from here down
static UINT64 UpdateTimer_DeltaTics(UpdateTimer *pUT)
{
	return	Delta(pUT);
}

//deltas are reasonably safe in floats
static float UpdateTimer_DeltaSeconds(UpdateTimer *pUT)
{
	UINT64	tics	=Delta(pUT);

	return	TicsToSeconds(pUT, tics);
}

//deltas are reasonably safe in floats
static float UpdateTimer_DeltaMilliSeconds(UpdateTimer *pUT)
{
	UINT64	tics	=Delta(pUT);

	return	TicsToMilliSeconds(pUT, tics);
}

static float TicsToSeconds(const UpdateTimer *pUT, UINT64 tics)
{
	double	secs	=(double)tics / pUT->mTicsPerSecond;

	return	(float)secs;
}

static float TicsToMilliSeconds(const UpdateTimer *pUT, UINT64 tics)
{
	double	msecs	=(double)tics / (pUT->mTicsPerSecond / 1000);

	return	(float)msecs;
}

static UINT64 SecondsToTics(const UpdateTimer *pUT, float seconds)
{
	UINT64	tics	=(UINT64)(seconds * pUT->mTicsPerSecond);

	return	tics;
}

static UINT64 MilliSecondsToTics(const UpdateTimer *pUT, float milliSeconds)
{
	UINT64	tics	=(UINT64)(milliSeconds * pUT->mTicsPerSecond);

	return	tics / 1000;
}
