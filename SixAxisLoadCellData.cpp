#include "SixAxisLoadCellData.h"

SixAxisLoadCellData::SixAxisLoadCellData(char *sDirectoryContainer, char *sDataHeader)
{
	bInSingleTrial = false;
	bClosingFile = false;
	strcpy_s(this->sDirectoryContainer, sDirectoryContainer);
	strcpy_s(this->sDataHeader, sDataHeader);
	fileOpenCounter = 0;
	bIsRecording = false;
	taskHandle = 0;

	calMatrix[0][0] = -1.37978;		calMatrix[0][1] = -0.02603;		calMatrix[0][2] = -7.77171;		calMatrix[0][3] = 95.81115;  calMatrix[0][4] = 11.02341;	calMatrix[0][5] = -93.04872;
	calMatrix[1][0] = 5.54331;		calMatrix[1][1] = -107.44245;	calMatrix[1][2] = -3.42957;		calMatrix[1][3] = 54.95624;  calMatrix[1][4] = -5.22543;	calMatrix[1][5] = 54.20996;
	calMatrix[2][0] = -135.88312;	calMatrix[2][1] = 3.04389;		calMatrix[2][2] = -136.46379;	calMatrix[2][3] = 4.18290;   calMatrix[2][4] = -133.84317;	calMatrix[2][5] = 1.94456;
	calMatrix[3][0] = 0.04891;		calMatrix[3][1] = -0.73492;		calMatrix[3][2] = 2.24427;		calMatrix[3][3] = 0.30337;   calMatrix[3][4] = -2.26584;	calMatrix[3][5] = 0.39437;
	calMatrix[4][0] = -2.49509;		calMatrix[4][1] = 0.05411;		calMatrix[4][2] = 1.35142;		calMatrix[4][3] = -0.69423;  calMatrix[4][4] = 1.18852;		calMatrix[4][5] = 0.62669;
	calMatrix[5][0] = -0.10792;		calMatrix[5][1] = 1.38479;		calMatrix[5][2] = -0.10145;		calMatrix[5][3] = 1.42217;   calMatrix[5][4] = -0.09800;	calMatrix[5][5] = 1.36918;

	DAQmxCreateTask("", &taskHandle);
	DAQmxCreateAIVoltageChan(taskHandle, "Dev1/ai0:5", "", DAQmx_Val_Cfg_Default, -10.0, 10.0, DAQmx_Val_Volts, NULL);
	DAQmxCfgSampClkTiming(taskHandle, "", 2000.0, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 1000);
	getBiasValue();

	hIOMutex = CreateMutex(NULL, FALSE, NULL);
	kill = 0;
	delayThread = 1;

	_beginthread(SixAxisLoadCellData::staticLoadCellCallback, 0, this);
}

SixAxisLoadCellData::~SixAxisLoadCellData(void)
{
	DAQmxStopTask(taskHandle);
	DAQmxClearTask(taskHandle);
	bIsRecording = false;
	kill = 1;
}

void SixAxisLoadCellData::loadCellCallback(void)
{
	// Query data looop
	getLoadCellData();
	currSampleTime = timeData.getCurrentTime();
	sampleCounter = 1;
	windowSize = 30;
	while (!kill) {

		//Sleep(delayThread);
		
		WaitForSingleObject(hIOMutex, INFINITE);
			
		// Query data for this frame
		currSampleTime = timeData.getCurrentTime();
		getLoadCellData();
		for (int i = 0; i < numChannels; i++) {
			outDisplayValues[i] = loadCellValue[i];
			outLoggingValues[i] = loadCellValue[i];
			if (sampleCounter <= windowSize){
				windowLoadCellValue[i] += loadCellValue[i];
			}
		}
		
		if (sampleCounter == windowSize){
			for (int i = 0; i < numChannels; i++){
				windowLoadCellValue[i] /= windowSize;
			}
			for (int i = 0; i < numChannels; i++) {
				velLoadCellValue[i] = (windowLoadCellValue[i] - pastLoadCellValue[i]) / (currSampleTime - pastSampleTime);
			}			
			for (int i = 0; i < numChannels; i++) {
				pastLoadCellValue[i] = windowLoadCellValue[i];
			}
			for (int i = 0; i < numChannels; i++){
				windowLoadCellValue[i] = 0;
			}
			pastSampleTime = currSampleTime;
			sampleCounter = 1;
		}
		else
		{
			sampleCounter += 1;
		}
			
		if (bInSingleTrial) {
			 // Recording for this frame
			if (fileOpenCounter == 0) {
				dataFile = fopen(sFileName, "w");
				if (dataFile == NULL) {
					MessageBoxA(
						NULL,
						"Could not open data file",
						"File Error",
						MB_OK
						);
				}

				fprintf(
					dataFile,
					sDataHeader
					);
				fileOpenCounter++;
			}

			if (fileOpenCounter == 1 && bIsWriteHeader){
				fprintf(dataFile, "%s\n", gHeader);
				bIsWriteHeader = false;
			}

			if (fileOpenCounter == 1 && bIsRecording) {
				fprintf(dataFile, "%f", currSampleTime);
				for (int i = 0; i < numChannels; i++) {
					fprintf(
						dataFile,
						",%.3f",
						outLoggingValues[i]
						);
				}
				fprintf(
					dataFile,
					"\n"
					);
			}

			if (fileOpenCounter == 1 && bClosingFile) {
				if (dataFile != NULL) {
					fclose(dataFile);
				}
				 // If this was successful then exit the single trial and reset flags
				dataFile = NULL;
				fileOpenCounter = 0;
				bClosingFile = false;
				bInSingleTrial = false;

			}

		}
		ReleaseMutex(hIOMutex);
	}

}

void SixAxisLoadCellData::staticLoadCellCallback(void* a) {
	((SixAxisLoadCellData*)a)->loadCellCallback();
}

int SixAxisLoadCellData::getLoadCellData()
{
	double tempLoadCellValue[8];
	double acc;
	int32 read;
	
	DAQmxReadAnalogF64(taskHandle, 1, 10.0, DAQmx_Val_GroupByChannel, tempLoadCellValue, numChannels, &read, NULL);
	for (int i = 0; i < 6; i++){
		acc = 0;
		for (int j = 0; j < 6; j++){
			acc = acc + (tempLoadCellValue[j] - biasValue[j]) * calMatrix[i][j];
		}
		loadCellValue[i] = acc;
	}
	
	return 0;
}

int SixAxisLoadCellData::getBiasValue()
{
	int32 read;

	DAQmxReadAnalogF64(taskHandle, 1, 10.0, DAQmx_Val_GroupByChannel, biasValue, numChannels, &read, NULL);
	return 0;
}

int SixAxisLoadCellData::stopRecording(void)
{
	bIsRecording = false;
	return 0;
}

int SixAxisLoadCellData::startRecording(void)
{
	bIsRecording = true;
	return 0;
}

int SixAxisLoadCellData::writeHeader(char *header)
{
	if (!bIsWriteHeader){
		sprintf_s(gHeader, header);
		bIsWriteHeader = true;
		return 0;
	}
	else{
		return 1;
	}
}

int SixAxisLoadCellData::closeRecordingFile()
{
	bClosingFile = true;
	stopRecording();
	return 0;
}

int SixAxisLoadCellData::setFileName(char *inFileName)
{
	sprintf_s(
		sFileName,
		sDirectoryContainer,
		inFileName
		);
	bInSingleTrial = true;
	return 0;
}