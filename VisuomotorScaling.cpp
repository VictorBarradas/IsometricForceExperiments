#include "VisuomotorScaling.h"

void VisuomotorScaling::Initialize(SixAxisLoadCellData *loadCellData)
{
	pLoadCellData = loadCellData;
	phase = INITIAL;
	state = CENTER;
	currentTrial = 0;
	scalingFactor = 1.0;
	bIsStarted = false;
	bIsEndExperiment = false;
	bIsCursorClamped = false;
	bIsTargetVisible = false;
	bIsCursorVisible = false;
	bIsTrialFail = false;
	bIsTooFast = false;
	bIsInTarget = false;
	bIsTimerCounting = false;
	bIsResting = false;
	bIsCursorMoving = false;
	bIsGivenReward = false;
	bIsRewardVisible = false;
	reachTolerance = 0.5;
	writingSuccess = 0;
	velThreshold = 10;
	upperTimeThreshold = 0.9;
	lowerTimeThreshold = 0.4;
	kill = 0;
	/*delayThread = 1;*/
}

void VisuomotorScaling::experimentCallback(void)
{
	pLoadCellData->setFileName("test_file_vms.txt");

	while (!kill) {
		WaitForSingleObject(hIOMutex, INFINITE);
		experimentStateMachine();
		ReleaseMutex(hIOMutex);
	}
}

void VisuomotorScaling::staticExperimentCallback(void* a)
{
	((VisuomotorScaling*)a)->experimentCallback();
}

int VisuomotorScaling::startExperiment(void)
{
	bIsStarted = true;
	return 0;
}

int VisuomotorScaling::stopExperiment(void)
{
	bIsEndExperiment = true;
	pLoadCellData->closeRecordingFile();
	return 0;
}

int VisuomotorScaling::initializeTargetPositions(int nCycleRepetitions, int nTargetDirections){
	int index;
	double PI = 3.14159;
	nTargets = nTargetDirections*nCycleRepetitions;

	double targetRadius = 15;

	for (int i = 0; i < nTargetDirections; i++){
		xPosTargets[i] = targetRadius * cos(i * 2 * PI / nTargetDirections);
		yPosTargets[i] = targetRadius * sin(i * 2 * PI / nTargetDirections);
	}

	srand(time(NULL));
	for (int i = 0; i < nCycleRepetitions; i++){
		for (int j = 0; j < nTargetDirections; j++){
			index = j + nTargetDirections*i;
			xShuffledTargets[index] = xPosTargets[j];
			yShuffledTargets[index] = yPosTargets[j];
		}
		shuffleTargets(xShuffledTargets+i*nTargetDirections, yShuffledTargets+i*nTargetDirections, nTargetDirections);
	}	
	return 0;
}

int VisuomotorScaling::shuffleTargets(double *x, double *y, int n)
{
	double t;
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

void VisuomotorScaling::initialPhase()
{
	bIsTargetVisible = false;
	bIsCursorVisible = true;
	updateCursorPos();
	if (bIsStarted) {
		phase = FAMILIARIZATION;
		state = CENTER;
		initializeTargetPositions(5, nTargetDirections);
		currentTrial = 0;
		pLoadCellData->writeHeader("FAMILIARIZATION");
	}
}

void VisuomotorScaling::familiarizationPhase()
{
	reachStateMachine();

	if (currentTrial >= nTargets){
		phase = BASELINE;
		state = CENTER;
		initializeTargetPositions(10, nTargetDirections);
		currentTrial = 0;
		do{
			writingSuccess = pLoadCellData->writeHeader("BASELINE");
		} while (writingSuccess == 1);
	}

}

void VisuomotorScaling::baselinePhase()
{
	int success;
	reachStateMachine();
	if (currentTrial >= nTargets){
		scalingFactor = 0.5;
		phase = PERTURBATION;
		state = CENTER;
		initializeTargetPositions(10, nTargetDirections);
		currentTrial = 0;
		do{
			writingSuccess = pLoadCellData->writeHeader("PERTURBATION");
		} while (writingSuccess == 1);
	}
}

void VisuomotorScaling::perturbationPhase()
{
	int success;
	reachStateMachine();
	if (currentTrial >= nTargets){
		scalingFactor = 1.0;
		phase = WASHOUT;
		state = CENTER;
		initializeTargetPositions(5, nTargetDirections);
		currentTrial = 0;
		do{
			writingSuccess = pLoadCellData->writeHeader("WASHOUT");
		} while (writingSuccess == 1);
	}
}

void VisuomotorScaling::washoutPhase()
{
	reachStateMachine();
	if (currentTrial >= nTargets){
		phase = END;
	}
}

void VisuomotorScaling::endPhase()
{
	stopExperiment();
}

void VisuomotorScaling::experimentStateMachine(void)
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

void VisuomotorScaling::centerState()
{
	bIsCursorVisible = true;
	bIsTargetVisible = true;
	bIsInTarget = false;
	targetPosX = 0;
	targetPosY = 0;
	updateCursorPos();

	if (cursorPosX >= -4 && cursorPosX <= 4 && cursorPosY >= -4 && cursorPosY <= 4) {
		startTimeInCenter = timeData.getCurrentTime();
		state = WAIT_CENTER;
	}

}

void VisuomotorScaling::waitCenterState()
{
	char header[400];
	updateCursorPos();
	bIsCursorVisible = true;
	bIsTargetVisible = true;
	if (cursorPosX >= -4 && cursorPosX <= 4 && cursorPosY >= -4 && cursorPosY <= 4) {
		timeInCenter = timeData.getCurrentTime() - startTimeInCenter;
		if (timeInCenter > 1) {
			targetPosX = xShuffledTargets[currentTrial];
			targetPosY = yShuffledTargets[currentTrial];
			sprintf_s(header, "Target X %.3f Y %.3f\nReach", targetPosX, targetPosY);
			pLoadCellData->writeHeader(header);
			state = REACH;
			bIsCursorMoving = false;
			pLoadCellData->startRecording();
			startTimeInReach = timeData.getCurrentTime();
		}
	}
	else
	{
		state = CENTER;
	}
}

void VisuomotorScaling::waitTargetState()
{
	char header[100];
	bIsCursorVisible = true;
	bIsTargetVisible = true;
	updateCursorPos();
	timeInTarget = timeData.getCurrentTime() - startTimeInTarget;
	if (timeInTarget > 2){
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

void VisuomotorScaling::reachState()
{	
	if (phase == FAMILIARIZATION){
		bIsCursorClamped = false;
		bIsCursorVisible = true;
		bIsTargetVisible = true;
	}
	else
	{
		bIsCursorClamped = true;
		bIsCursorVisible = false;
		bIsTargetVisible = true;
	}
	updateCursorPos();
	updateCursorVel();

	if (cursorVel > velThreshold)
	{
		bIsCursorMoving = true;
	}

	if (bIsCursorMoving && cursorVel < velThreshold){
		timeInReach = timeData.getCurrentTime() - startTimeInReach;
		pLoadCellData->writeHeader("Hold");
		if (timeInReach < upperTimeThreshold && timeInReach > lowerTimeThreshold){
			switch (phase){
			case FAMILIARIZATION:
				state = WAIT_TARGET;
				startTimeInTarget = timeData.getCurrentTime();
				break;
			case BASELINE:
			case PERTURBATION:
			case WASHOUT:
				state = SHOW_FEEDBACK;
				startTimeInTarget = timeData.getCurrentTime();
			default:
				break;
			}
		}
		else{
			state = FAIL_TRIAL;
			startTimeInFail = timeData.getCurrentTime();
		}
		
	}	
}

void VisuomotorScaling::failTrialState()
{
	bIsTrialFail = true;
	if (timeInReach < lowerTimeThreshold){
		bIsTooFast = true;
	}
	else{
		bIsTooFast = false;
	}
	timeInFail = timeData.getCurrentTime() - startTimeInFail;
	if (timeInFail >= 1){
		state = WAIT_TARGET;		
	}
}

void VisuomotorScaling::reachStateMachine()
{
	switch (state) {
	case CENTER:
		centerState();
		break;

	case REACH:
		reachState();
		break;

	case SHOW_FEEDBACK:
		showFeedbackState();
		break;

	case WAIT_TARGET:
		waitTargetState();
		break;

	case WAIT_CENTER:
		waitCenterState();
		break;

	case FAIL_TRIAL:
		failTrialState();
		break;

	default:
		break;
	}
}

