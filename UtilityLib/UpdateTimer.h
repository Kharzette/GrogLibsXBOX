#pragma once
#include	<basetsd.h>

typedef struct	UpdateTimer_t	UpdateTimer;

UpdateTimer	*UpdateTimer_Create(BOOL bFixed, BOOL bSpendRemainder);

//top and bottom of update loop
void UpdateTimer_Stamp(UpdateTimer *pUT);
void UpdateTimer_UpdateDone(UpdateTimer *pUT);

//return a time slice
UINT64 UpdateTimer_GetUpdateDeltaTics(const UpdateTimer *pUT);
float UpdateTimer_GetUpdateDeltaSeconds(const UpdateTimer *pUT);
float UpdateTimer_GetUpdateDeltaMilliSeconds(const UpdateTimer *pUT);

//return render time slice (always the entire deltaTime)
UINT64 UpdateTimer_GetRenderUpdateDeltaTics(const UpdateTimer *pUT);
float UpdateTimer_GetRenderUpdateDeltaSeconds(const UpdateTimer *pUT);
float UpdateTimer_GetRenderUpdateDeltaMilliSeconds(const UpdateTimer *pUT);

//Set fixed timeslice and max deltatime
void UpdateTimer_SetMaxDeltaTics(UpdateTimer *pUT, UINT64 tics);
void UpdateTimer_SetMaxDeltaMilliSeconds(UpdateTimer *pUT, float milliSeconds);
void UpdateTimer_SetMaxDeltaSeconds(UpdateTimer *pUT, float seconds);
void UpdateTimer_SetFixedTimeStepTics(UpdateTimer *pUT, UINT64 tics);
void UpdateTimer_SetFixedTimeStepSeconds(UpdateTimer *pUT, float seconds);
void UpdateTimer_SetFixedTimeStepMilliSeconds(UpdateTimer *pUT, float milliSeconds);