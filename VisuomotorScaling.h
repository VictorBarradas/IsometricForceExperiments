#pragma once
#include <stdio.h>
//#include <time.h>
#include <math.h>
#include "TimeData.h"
#include "SixAxisLoadCellData.h"

class VisuomotorScaling
{
public:
	int kill;

	int phase;
	int state;

	//bool bIsRecording;
	double startTimeInCenter;
	double timeInCenter;
	double startTimeInTarget;
	double timeInTarget;
	double startTimeInReach;
	double timeInReach;
	double startTimeInFail;
	double timeInFail;
	
	HANDLE hIOMutex;
	DWORD delayThread;

	SixAxisLoadCellData *pLoadCellData;

	void experimentStateMachine();
	void reachStateMachine();

	void initialPhase(void);
	void familiarizationPhase();
	void baselinePhase();
	void perturbationPhase();
	void washoutPhase();
	void endPhase();

	virtual void centerState(void);
	void reachState(void);
	void waitCenterState(void);
	void waitTargetState(void);
	virtual void showFeedbackState(void) = 0;
	void failTrialState(void);

	int currentTrial;
	int nTargetDirections;
	int nTargets;
	int restCount;
	double xPosTargets[100];
	double yPosTargets[100];
	double xShuffledTargets[1000];
	double yShuffledTargets[1000];

	enum state_names
	{
		INITIAL = 0,
		FAMILIARIZATION = 1,
		BASELINE = 2,
		PERTURBATION = 3,
		WASHOUT = 4,
		END = 5
	};

	enum reach_state_names
	{
		CENTER = 10,
		REACH = 20,
		SHOW_FEEDBACK = 30,
		WAIT_CENTER = 40,
		WAIT_TARGET = 50,
		FAIL_TRIAL = 60
	};
	
	bool bIsStarted;
	bool bIsTargetVisible;
	bool bIsCursorVisible;
	bool bIsInTarget;
	bool bIsEndExperiment;
	bool bIsTimerCounting;
	bool bIsTrialFail;
	bool bIsResting;
	bool bIsCursorMoving;
	bool bIsCursorClamped;
	bool bIsTooFast;
	bool bIsGivenReward;
	bool bIsRewardVisible;

	TimeData timeData;

	double volatile cursorPosX;
	double volatile cursorPosY;
	double cursorVelX;
	double cursorVelY;
	double cursorVel;
	double targetPosX;
	double targetPosY;
	double reachTolerance;
	int startExperiment();
	int stopExperiment();
	virtual int updateCursorPos() = 0;
	virtual int updateCursorVel() = 0;
	int initializeTargetPositions(int, int);
	int shuffleTargets(double*, double*, int);

	static void staticExperimentCallback(void*);
	void experimentCallback(void);

	void Initialize(SixAxisLoadCellData*);

	double scalingFactor;
	double velThreshold;
	double upperTimeThreshold;
	double lowerTimeThreshold;

	int writingSuccess;
};