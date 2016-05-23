#include "SDNMeasurement.h"

SDNMeasurement::SDNMeasurement(SixAxisLoadCellData *loadCellData)
{
	pLoadCellData = loadCellData;
	state = 0;
	bIsStarted = false;
	bIsEndExperiment = false;

	hIOMutex = CreateMutex(NULL, FALSE, NULL);
	kill = 0;
	delayThread = 1;

	_beginthread(SDNMeasurement::staticExperimentCallback, 0, this);
}

SDNMeasurement::~SDNMeasurement(void)
{
	stopExperiment();
	kill = 1;
}

void SDNMeasurement::experimentCallback(void)
{
	pLoadCellData->setFileName("test_file_sdn.txt");

	while (!kill) {
		WaitForSingleObject(hIOMutex, INFINITE);
		experimentStateMachine();
		ReleaseMutex(hIOMutex);
	}
}

void SDNMeasurement::staticExperimentCallback(void* a)
{
	((SDNMeasurement*)a)->experimentCallback();
}

int SDNMeasurement::startExperiment(void)
{
	bIsStarted = true;
	return 0;
}

int SDNMeasurement::stopExperiment(void)
{
	pLoadCellData->closeRecordingFile();
	return 0;
}

int SDNMeasurement::updateCursorPos(void)
{
	cursorPosX = pLoadCellData->outDisplayValues[1];
	cursorPosY = pLoadCellData->outDisplayValues[0] * -1;
	return 0;
}

int SDNMeasurement::initializeTargetPositions(void){
	int index;
	double PI = 3.14159;
	nTargetDirections = 12;
	nTargetLoads = 4;
	nTargetRepetitions = 2;
	nTargets = nTargetDirections*nTargetLoads*nTargetRepetitions;

	double loadLevels[4] = { 10, 15, 20, 25 };

	for (int i = 0; i < nTargetDirections; i++){
		for (int j = 0; j < nTargetLoads; j++){
			for (int k = 0; k < nTargetRepetitions; k++){
				index = k + j*nTargetRepetitions + i*nTargetLoads*nTargetRepetitions;
				xPosTargets[index] = loadLevels[j] * cos(i * 2 * PI / nTargetDirections);
				yPosTargets[index] = loadLevels[j] * sin(i * 2 * PI / nTargetDirections);
			}
		}
	}
	shuffleTargets(xPosTargets, yPosTargets, nTargets);
	return 0;
}

int SDNMeasurement::shuffleTargets(double *x, double *y, int n)
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

			t = y[j];
			y[j] = y[i];
			y[i] = t;
		}
	}
	return 0;
}

void SDNMeasurement::initialState()
{
	initializeTargetPositions();
	bIsTargetVisible = false;
	bIsCursorVisible = true;
	currentTarget = 0;
	restCount = 0;
	updateCursorPos();
	if (bIsStarted) {
		state = CENTER;
	}
}

void SDNMeasurement::centerState()
{
	bIsCursorVisible = true;
	bIsTargetVisible = true;
	bIsInTarget = false;
	targetPosX = 0;
	targetPosY = 0;
	updateCursorPos();
	if (currentTarget >= nTargets){
		state = END;
	}
	else{
		if (restCount >= 10){
			startTimeRest = timeData.getCurrentTime();
			state = REST;
		}
		else{
			if (cursorPosX >= -4 && cursorPosX <= 4 && cursorPosY >= -4 && cursorPosY <= 4) {
				startTimeInCenter = timeData.getCurrentTime();
				state = WAIT_CENTER;
			}
		}

	}
}

void SDNMeasurement::reachState() {
	updateCursorPos();
	pLoadCellData->startRecording();
	if (cursorPosX >= targetPosX - reachTolerance && cursorPosX <= targetPosX + reachTolerance && cursorPosY >= targetPosY - reachTolerance
		&& cursorPosY <= targetPosY + reachTolerance) {
		bIsInTarget = true;
		if (!bIsTimerCounting) {
			startTimeInTarget = timeData.getCurrentTime();
		}
	}
	else {
		bIsInTarget = false;
		bIsTimerCounting = false;
	}
	if (bIsInTarget) {
		timeInTarget = timeData.getCurrentTime() - startTimeInTarget;
		bIsTimerCounting = true;
		if (timeInTarget >= 0.5) {
			state = HOLD;
			pLoadCellData->stopRecording();
			fprintf(pLoadCellData->dataFile, "Hold\n");
			startTimeInTarget = timeData.getCurrentTime();
		}
	}
}

void SDNMeasurement::holdState()
{
	updateCursorPos();
	pLoadCellData->startRecording();
	bIsCursorVisible = false;
	bIsTargetVisible = false;
	timeInTarget = timeData.getCurrentTime() - startTimeInTarget;
	cursorMagnitude = sqrt(cursorPosX*cursorPosX + cursorPosY*cursorPosY);
	targetMagnitude = sqrt(targetPosX*targetPosX + targetPosY*targetPosY);
	if (timeInTarget >= 2) {
		pLoadCellData->stopRecording();
		fprintf(pLoadCellData->dataFile, "Reach Success: 1\n");
		currentTarget += 1;
		restCount += 1;
		state = CENTER;
	}
	if (cursorMagnitude > targetMagnitude + 4 || cursorMagnitude < targetMagnitude - 4){
		state = FAIL;
		startTimeInFail = timeData.getCurrentTime();
	}
}

void SDNMeasurement::failState()
{
	updateCursorPos();
	pLoadCellData->stopRecording();
	bIsCursorVisible = true;
	bIsTargetVisible = false;
	bIsTrialFail = true;
	timeInFail = timeData.getCurrentTime() - startTimeInFail;
	if (timeInFail >= 2){
		state = CENTER;
		bIsTrialFail = false;
		fprintf(pLoadCellData->dataFile, "Reach Success: 0\n", targetPosX, targetPosY);
	}
}

void SDNMeasurement::restState()
{
	updateCursorPos();
	bIsResting = true;
	timeRest = timeData.getCurrentTime() - startTimeRest;
	if (timeRest > 10){
		restCount = 0;
		bIsResting = false;
		state = CENTER;
	}
}

void SDNMeasurement::waitCenterState()
{
	updateCursorPos();
	bIsCursorVisible = true;
	bIsTargetVisible = true;
	if (cursorPosX >= -4 && cursorPosX <= 4 && cursorPosY >= -4 && cursorPosY <= 4) {
		timeInCenter = timeData.getCurrentTime() - startTimeInCenter;
		if (timeInCenter > 1) {
			targetPosX = xPosTargets[currentTarget];
			targetPosY = yPosTargets[currentTarget];
			fprintf(pLoadCellData->dataFile, "Target X %.3f Y %.3f\n", targetPosX, targetPosY);
			fprintf(pLoadCellData->dataFile, "Reach\n");
			state = REACH;
		}
	}
	else
	{
		state = CENTER;
	}
}

void SDNMeasurement::endState()
{
	updateCursorPos();
	bIsEndExperiment = true;
	stopExperiment();
	bIsCursorVisible = true;
	bIsTargetVisible = true;
}

int SDNMeasurement::experimentStateMachine(void)
{
	switch (state) {
	case INITIAL:
		initialState();
		break;

	case CENTER:
		centerState();
		break;

	case REACH:
		reachState();
		break;

	case HOLD:
		holdState();

		break;
	case FAIL:
		failState();
		break;

	case REST:
		restState();
		break;

	case WAIT_CENTER:
		waitCenterState();
		break;

	case END:
		endState();
		break;

	default:
		break;
	}


	return 0;
}

