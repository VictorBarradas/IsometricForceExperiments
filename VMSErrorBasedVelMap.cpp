#include "VMSErrorBasedVelMap.h"

VMSErrorBasedVelMap::VMSErrorBasedVelMap(SixAxisLoadCellData *loadCellData)
{
	Initialize(loadCellData);
	nTargetDirections = 8;
	posX = 0;
	posY = 0;
	pastCursorVelX = 0;
	pastCursorVelY = 0;
	velThreshold = 1;
	upperTimeThreshold = 0.9;
	lowerTimeThreshold = 0.2;

	hIOMutex = CreateMutex(NULL, FALSE, NULL);

	delayThread = 1;

	_beginthread(VMSErrorBasedVelMap::staticExperimentCallback, 0, this);
}

VMSErrorBasedVelMap::~VMSErrorBasedVelMap(void)
{
	stopExperiment();
	kill = 1;
}

void VMSErrorBasedVelMap::experimentCallback(void)
{
	pLoadCellData->setFileName("test_file_vms.txt");

	while (!kill) {
		WaitForSingleObject(hIOMutex, INFINITE);
		experimentStateMachine();
		ReleaseMutex(hIOMutex);
	}
}

void VMSErrorBasedVelMap::staticExperimentCallback(void* a)
{
	((VMSErrorBasedVelMap*)a)->experimentCallback();
}

int VMSErrorBasedVelMap::updateCursorPos(void)
{
	updateCursorVel();
	posX = posX + (cursorVelX + pastCursorVelX)*0.000005;
	posY = posY + (cursorVelY + pastCursorVelY)*0.000005;
	pastCursorVelX = cursorVelX;
	pastCursorVelY = cursorVelY;
	if (bIsCursorClamped){
		double targetX = xShuffledTargets[currentTrial];
		double targetY = yShuffledTargets[currentTrial];
		double angleDifference = atan2(posX*targetY - posY*targetX, posX*targetX + posY*targetY);
		if (abs(angleDifference) < 5.0*3.14159 / 180.0){
			double magnitude = sqrt(posX*posX + posY*posY);
			double directionTarget = atan2(targetY, targetX);
			cursorPosX = magnitude*cos(directionTarget);
			cursorPosY = magnitude*sin(directionTarget);
		}
		else{
			cursorPosX = posX;
			cursorPosY = posY;
		}
	}
	else{
		cursorPosX = posX;
		cursorPosY = posY;
	}
	return 0;
}

int VMSErrorBasedVelMap::updateCursorVel()
{
	cursorVelX = scalingFactor*pLoadCellData->outDisplayValues[1];
	cursorVelY = -scalingFactor*pLoadCellData->outDisplayValues[0];

	cursorVel = sqrt(cursorVelX*cursorVelX + cursorVelY*cursorVelY);

	return 0;
}

void VMSErrorBasedVelMap::initialPhase()
{
	bIsTargetVisible = false;
	bIsCursorVisible = true;
	updateCursorPos();
	if (bIsStarted) {
		phase = FAMILIARIZATION;
		state = CENTER;
		initializeTargetPositions(1, nTargetDirections);
		currentTrial = 0;
		pLoadCellData->writeHeader("FAMILIARIZATION");
	}
}

void VMSErrorBasedVelMap::familiarizationPhase()
{
	reachStateMachine();

	if (currentTrial >= nTargets){
		phase = BASELINE;
		state = CENTER;
		initializeTargetPositions(1, nTargetDirections);
		currentTrial = 0;
		do{
			writingSuccess = pLoadCellData->writeHeader("BASELINE");
		} while (writingSuccess == 1);
	}

}

void VMSErrorBasedVelMap::baselinePhase()
{
	int success;
	reachStateMachine();
	if (currentTrial >= nTargets){
		scalingFactor = 0.5;
		phase = PERTURBATION;
		state = CENTER;
		initializeTargetPositions(1, nTargetDirections);
		currentTrial = 0;
		do{
			writingSuccess = pLoadCellData->writeHeader("PERTURBATION");
		} while (writingSuccess == 1);
	}
}

void VMSErrorBasedVelMap::perturbationPhase()
{
	int success;
	reachStateMachine();
	if (currentTrial >= nTargets){
		scalingFactor = 1.0;
		phase = WASHOUT;
		state = CENTER;
		initializeTargetPositions(1, nTargetDirections);
		currentTrial = 0;
		do{
			writingSuccess = pLoadCellData->writeHeader("WASHOUT");
		} while (writingSuccess == 1);
	}
}

void VMSErrorBasedVelMap::washoutPhase()
{
	reachStateMachine();
	if (currentTrial >= nTargets){
		phase = END;
	}
}

void VMSErrorBasedVelMap::endPhase()
{
	stopExperiment();
}

void VMSErrorBasedVelMap::experimentStateMachine(void)
{
	switch (phase) {
	case INITIAL:
		initialPhase();
		break;
	case FAMILIARIZATION:
		familiarizationPhase();
		break;

	case BASELINE:
		baselinePhase();
		break;

	case PERTURBATION:
		perturbationPhase();
		break;

	case WASHOUT:
		washoutPhase();
		break;

	case END:
		endPhase();
		break;

	default:
		break;
	}
}


