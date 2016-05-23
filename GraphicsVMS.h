#pragma once
#include <stdio.h>
#include <string.h>
#include "Graphics.h"
#include "VMSErrorBasedPosMap.h"

class GraphicsVMS : public Graphics
{
private:
	//VMSErrorBasedPosMap *vmsExperiment;
	VisuomotorScaling *vmsExperiment;
	int cursorZeroPosX;
	int cursorZeroPosY;
	char gStatusString[10][300];
public:
	GraphicsVMS(VisuomotorScaling*);
	void Display();
	void Keyboard(unsigned char, int, int);
	void renderEnvironment();
};