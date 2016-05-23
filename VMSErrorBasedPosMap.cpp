#include "VMSErrorBasedPosMap.h"

VMSErrorBasedPosMap::VMSErrorBasedPosMap(SixAxisLoadCellData *loadCellData)
{
	Initialize(loadCellData);
	nTargetDirections = 8;

	hIOMutex = CreateMutex(NULL, FALSE, NULL);

	delayThread = 1;

	_beginthread(VMSErrorBasedPosMap::staticExperimentCallback, 0, this);
}

VMSErrorBasedPosMap::~VMSErrorBasedPosMap(void)
{
	stopExperiment();
	kill = 1;
}

void VMSErrorBasedPosMap::experimentCallback(void)
{
	pLoadCellData->setFileName("test_file_vms_error.txt");

	while (!kill) {
		WaitForSingleObject(hIOMutex, INFINITE);
		experimentStateMachine();
		ReleaseMutex(hIOMutex);
	}
}

void VMSErrorBasedPosMap::staticExperimentCallback(void* a)
{
	((VMSErrorBasedPosMap*)a)->experimentCallback();
}

int VMSErrorBasedPosMap::updateCursorPos(void)
{
	double posX = scalingFactor*(pLoadCellData->outDisplayValues[1]);
	double posY = scalingFactor*(pLoadCellData->outDisplayValues[0] * -1);
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

int VMSErrorBasedPosMap::updateCursorVel()
{
	cursorVelX = pLoadCellData->velLoadCellValue[1];
	cursorVelY = pLoadCellData->velLoadCellValue[0];

	cursorVel = sqrt(cursorVelX*cursorVelX + cursorVelY*cursorVelY);

	return 0;
}

void VMSErrorBasedPosMap::showFeedbackState()
{
	char header[100];
	bIsCursorVisible = true;
	bIsTargetVisible = true;
	timeInTarget = timeData.getCurrentTime() - startTimeInTarget;
	if (timeInTarget > 2){
		bIsCursorClamped = false;
		pLoadCellData->stopRecording();
		sprintf_s(header, "Success: %d", !bIsTrialFail);
		pLoadCellData->writeHeader(header);
		if (!bIsTrialFail){
			currentTrial += 1;
		}
		bIsTrialFail = false;
		state = CENTER;
	}
}

void VMSErrorBasedPosMap::initialPhase()
{
	bIsTargetVisible = false;
	bIsCursorVisible = true;
	updateCursorPos();
	if (bIsStarted) {
		phase = FAMILIARIZATION;
		state = CENTER;
		initializeTargetPositions(1,nTargetDirections);
		currentTrial = 0;
		pLoadCellData->writeHeader("FAMILIARIZATION");
	}
}

void VMSErrorBasedPosMap::familiarizationPhase()
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

void VMSErrorBasedPosMap::baselinePhase()
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

void VMSErrorBasedPosMap::perturbationPhase()
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

void VMSErrorBasedPosMap::washoutPhase()
{
	reachStateMachine();
	if (currentTrial >= nTargets){
		phase = END;
	}
}

void VMSErrorBasedPosMap::endPhase()
{
	stopExperiment();
}

void VMSErrorBasedPosMap::experimentStateMachine(void)
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


