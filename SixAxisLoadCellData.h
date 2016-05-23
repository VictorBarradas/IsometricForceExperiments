#pragma once
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <time.h>
#include "include/NIDAQmx.h"
#include "TimeData.h"

#ifdef _WIN64
#	pragma comment(lib, "lib/x64/NIDAQmx.lib")
#else
#	pragma comment(lib, "lib/x86/NIDAQmx.lib")
#endif

class SixAxisLoadCellData
{
	static const int numChannels = 6;
	double calMatrix[6][6];
	FILE *loadCellConfigFile;

	double loadCellIntercept[numChannels];
	double loadCellSlope[numChannels];

	int kill;
	HANDLE hIOMutex;
	DWORD delayThread;

	// Recording Multiple Trials
	bool bClosingFile;
	char sDirectoryContainer[512];
	char sDataHeader[600];
	int fileOpenCounter;
	char sFileName[400];
	char gHeader[400];
	
	bool bInSingleTrial;

	TaskHandle  taskHandle;

public:
	bool bIsRecording;
	bool bIsWriteHeader;
	TimeData timeData;
	FILE *dataFile;

	double loadCellValue[6];
	double pastLoadCellValue[6];
	double windowLoadCellValue[6];
	int windowSize;
	double velLoadCellValue[6];
	double biasValue[6];
	SixAxisLoadCellData(char* sDirectoryContainer, char* sDataHeader);
	~SixAxisLoadCellData();
	int getLoadCellData();
	int getBiasValue();
	double volatile outDisplayValues[6];
	double volatile outLoggingValues[6];

	double currSampleTime;
	double pastSampleTime;

	int sampleCounter;

	static void staticLoadCellCallback(void*);
	void loadCellCallback(void);


	// Recording Multiple Trials
	int startRecording(void);
	int stopRecording(void);
	int writeHeader(char *);
	int closeRecordingFile(void);
	int setFileName(char *);
};