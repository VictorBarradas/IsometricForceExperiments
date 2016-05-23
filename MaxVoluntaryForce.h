#pragma once
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <ctime>
#include "TimeData.h"
#include "SixAxisLoadCellData.h"

class MaxVoluntaryForce
{
	int kill;
	int state;
	bool bIsRecording;
	double startTimeInWait;
	double timeInWait;
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

	int experimentStateMachineMVF();

	void initialState();
	void centerState();
	void forceState();
	void waitState();
	void endState();

	int nTargetDirections;
	int nTargetLoads;
	int nTargetRepetitions;
	int nTargets;
	int currentTarget;
	int restCount;
	double xPosTargets[100];
	double yPosTargets[100];
	double targetDirections[100];
	
	enum state_names_MVF
	{
		INITIAL = 0,
		CENTER = 1,
		WAIT = 2,
		FORCE = 3,
		END = 4
	};

public:

	bool bIsStarted;
	bool bIsCursorVisible;
	bool bIsTargetDirectionVisible;
	bool bIsCenterVisible;
	bool bIsInTarget;
	bool bIsOnDirection;
	bool bIsEndExperiment;
	bool bIsTimerCounting;
	bool bIsWaiting;

	TimeData timeData;

	double volatile cursorPosX;
	double volatile cursorPosY;
	double direction;
	double targetDirection;
	const double reachTolerance = 0.5;
	MaxVoluntaryForce(SixAxisLoadCellData*);
	~MaxVoluntaryForce();
	int startExperiment(void);
	int stopExperiment(void);
	int updateCursorPos(void);
	int initializeDirections(void);
	int shuffleTargets(double*, int);


	static void staticExperimentCallback(void*);
	void experimentCallback(void);
};