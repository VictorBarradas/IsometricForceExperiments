#include "MaxVoluntaryForce.h"

MaxVoluntaryForce::MaxVoluntaryForce(SixAxisLoadCellData *loadCellData)
{
	pLoadCellData = loadCellData;
	state = 0;
	bIsStarted = false;
	bIsCenterVisible = false;
	bIsEndExperiment = false;
	bIsWaiting = false;

	hIOMutex = CreateMutex(NULL, FALSE, NULL);
	kill = 0;
	delayThread = 1;

	_beginthread(MaxVoluntaryForce::staticExperimentCallback, 0, this);
}

MaxVoluntaryForce::~MaxVoluntaryForce(void)
{
	kill = 1;
}

void MaxVoluntaryForce::experimentCallback(void)
{
	pLoadCellData->setFileName("test_file_mvf.txt");
	
	while (!kill) {

		WaitForSingleObject(hIOMutex, INFINITE);
		experimentStateMachineMVF();
		ReleaseMutex(hIOMutex);
	}
}

void MaxVoluntaryForce::staticExperimentCallback(void* a)
{
	((MaxVoluntaryForce*)a)->experimentCallback();
}

int MaxVoluntaryForce::startExperiment(void)
{
	bIsStarted = 1;
	return 0;
}

int MaxVoluntaryForce::stopExperiment(void)
{
	pLoadCellData->closeRecordingFile();
	return 0;
}

int MaxVoluntaryForce::updateCursorPos(void)
{
	cursorPosX = pLoadCellData->outDisplayValues[1];
	cursorPosY = pLoadCellData->outDisplayValues[0]*-1;
	return 0;
}

int MaxVoluntaryForce::initializeDirections(void)
{
	nTargetDirections = 12;
	const double PI = 3.14159;
	for (int i = 0; i < nTargetDirections; i++){
		targetDirections[i] = i * 2 * PI / nTargetDirections;
	}
	shuffleTargets(targetDirections,nTargetDirections);
	return 0;
}

int MaxVoluntaryForce::shuffleTargets(double *x, int n)
{
	double t;
	srand(time(NULL));
	if (n > 1) {
		int i;
		for (i = 0; i < n - 1; i++)
		{
			int j = i + rand() / (RAND_MAX / (n - i) + 1);
			t = x[j];
			x[j] = x[i];
			x[i] = t;
		}
	}
	return 0;
}

int MaxVoluntaryForce::experimentStateMachineMVF(void)
{
	switch (state){
	case INITIAL:
		initialState();
		break;
	case CENTER:
		centerState();
		break;
	case FORCE:
		forceState();
		break;
	case WAIT:
		waitState();
		break;
	case END:
		endState();
		break;
	}
	return 0;
}

void MaxVoluntaryForce::initialState()
{
	initializeDirections();
	bIsCursorVisible = true;
	bIsTargetDirectionVisible = false;
	bIsCenterVisible = false;
	bIsOnDirection = false;
	updateCursorPos();
	currentTarget = 0;
	if (bIsStarted){
		startTimeInWait = timeData.getCurrentTime();
		state = WAIT;
	}
}

void MaxVoluntaryForce::waitState()
{
	updateCursorPos();
	bIsWaiting = true;
	bIsCursorVisible = true;
	bIsCenterVisible = true;
	bIsTargetDirectionVisible = false;
	timeInWait = timeData.getCurrentTime() - startTimeInWait;
	if (timeInWait > 5){
		state = CENTER;
		bIsWaiting = false;
		if (currentTarget < nTargetDirections){
			fprintf(pLoadCellData->dataFile, "Direction %.3f\n", targetDirections[currentTarget]*180/3.14159);
		}
	}
}

void MaxVoluntaryForce::centerState()
{
	updateCursorPos();
	bIsCursorVisible = true;
	bIsCenterVisible = true;
	bIsTargetDirectionVisible = false;
	if (cursorPosX >= -4 && cursorPosX <= 4 && cursorPosY >= -4 && cursorPosY <= 4){
		if (currentTarget > nTargetDirections - 1){
			state = END;
		}
		else{
			state = FORCE;
		}
	}
}

void MaxVoluntaryForce::forceState()
{
	updateCursorPos();
	bIsCursorVisible = true;
	bIsCenterVisible = false;
	bIsTargetDirectionVisible = true;
	targetDirection = targetDirections[currentTarget];
	cursorMagnitude = sqrt(cursorPosX*cursorPosX + cursorPosY*cursorPosY);
	pLoadCellData->startRecording();

	double targetX = cos(targetDirection);
	double targetY = sin(targetDirection);	
	double angleDifference = atan2(cursorPosX*targetY - cursorPosY*targetX, cursorPosX*targetX + cursorPosY*targetY);

	if (angleDifference > -0.1 && angleDifference < 0.1 && cursorMagnitude > 10){
		if (!bIsTimerCounting){
			bIsTimerCounting = true;
			startTimeInTarget = timeData.getCurrentTime();
		}
		bIsOnDirection = true;
	}
	else{
		bIsOnDirection = false;
		bIsTimerCounting = false;
	}
	if (bIsOnDirection){
		timeInTarget = timeData.getCurrentTime() - startTimeInTarget;
		if (timeInTarget >= 4){
			pLoadCellData->stopRecording();
			state = WAIT;
			startTimeInWait = timeData.getCurrentTime();
			currentTarget += 1;
		}
	}
}

void MaxVoluntaryForce::endState()
{
	updateCursorPos();
	bIsEndExperiment = true;
	stopExperiment();
	bIsCursorVisible = true;
	bIsCenterVisible = true;
	bIsTargetDirectionVisible = false;
}




