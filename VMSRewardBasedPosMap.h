#pragma once
#include "SixAxisLoadCellData.h"
#include "VisuomotorScaling.h"

class VMSRewardBasedPosMap : public VisuomotorScaling
{
public:

	HANDLE hIOMutex;
	DWORD delayThread;

	void experimentStateMachine();
	void centerState();
	void showFeedbackState();

	void initialPhase(void);
	void familiarizationPhase();
	void baselinePhase();
	void perturbationPhase();
	void washoutPhase();
	void endPhase();

	enum phase_names
	{
		INITIAL = 0,
		FAMILIARIZATION = 1,
		BASELINE = 2,
		PERTURBATION = 3,
		WASHOUT = 4,
		END = 5
	};

	VMSRewardBasedPosMap(SixAxisLoadCellData*);
	~VMSRewardBasedPosMap();

	int updateCursorPos();
	int updateCursorVel();

	static void staticExperimentCallback(void*);
	void experimentCallback(void);

};