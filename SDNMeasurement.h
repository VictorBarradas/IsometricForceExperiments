#pragma once
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <ctime>
#include "TimeData.h"
#include "SixAxisLoadCellData.h"

class SDNMeasurement
{
	int kill;
	int state;
	bool bIsRecording;
	double startTimeInCenter;
	double timeInCenter;
	double startTimeInTarget;
	double timeInTarget;
	double startTimeInFail;
	double timeInFail;
	double startTimeRest;
	double timeRest;
	double cursorMagnitude;
	double targetMagnitude;

	HANDLE hIOMutex;
	DWORD delayThread;

	SixAxisLoadCellData *pLoadCellData;

	int experimentStateMachine(void);

	void initialState(void);
	void centerState(void);
	void reachState(void);
	void holdState(void);
	void failState(void);
	void restState(void);
	void waitCenterState(void);
	void endState(void);

	int nTargetDirections;
	int nTargetLoads;
	int nTargetRepetitions;
	int nTargets;
	int currentTarget;
	int restCount;
	double xPosTargets[100];
	double yPosTargets[100];
	double targetDirections[100];

	enum state_names_STD
	{
		INITIAL = 0,
		CENTER = 1,
		REACH = 2,
		HOLD = 3,
		END = 4,
		FAIL = 5,
		REST = 6,
		WAIT_CENTER = 7
	};

public:

	bool bIsStarted;
	bool bIsTargetVisible;
	bool bIsCursorVisible;
	bool bIsInTarget;
	bool bIsEndExperiment;
	bool bIsTimerCounting;
	bool bIsTrialFail;
	bool bIsResting;

	TimeData timeData;

	double volatile cursorPosX;
	double volatile cursorPosY;
	double targetPosX;
	double targetPosY;
	const double reachTolerance = 0.5;
	SDNMeasurement(SixAxisLoadCellData*);
	~SDNMeasurement();
	int startExperiment(void);
	int stopExperiment(void);
	int updateCursorPos(void);
	int initializeTargetPositions(void);
	int shuffleTargets(double*, double*, int);
	
	static void staticExperimentCallback(void*);
	void experimentCallback(void);
};