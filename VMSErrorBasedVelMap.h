#pragma once
#include "SixAxisLoadCellData.h"
#include "VisuomotorScaling.h"

class VMSErrorBasedVelMap : public VisuomotorScaling
{
public:

	HANDLE hIOMutex;
	DWORD delayThread;

	void experimentStateMachine();

	void initialPhase(void);
	void familiarizationPhase();
	void baselinePhase();
	void perturbationPhase();
	void washoutPhase();
	void endPhase();

	double pastCursorVelX;
	double pastCursorVelY;
	double posX;
	double posY;

	enum phase_names
	{
		INITIAL = 0,
		FAMILIARIZATION = 1,
		BASELINE = 2,
		PERTURBATION = 3,
		WASHOUT = 4,
		END = 5
	};

	VMSErrorBasedVelMap(SixAxisLoadCellData*);
	~VMSErrorBasedVelMap();

	int updateCursorPos();
	int updateCursorVel();

	static void staticExperimentCallback(void*);
	void experimentCallback(void);

};