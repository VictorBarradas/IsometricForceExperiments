#pragma once
#include <stdio.h>
#include <string.h>
#include "Graphics.h"
#include "SDNMeasurement.h"

class GraphicsSDN : public Graphics
{
private:
	SDNMeasurement *sdnExperiment;
	int cursorZeroPosX;
	int cursorZeroPosY;
	char gStatusString[10][300];
public:
	GraphicsSDN(SDNMeasurement*);
	void Display();
	void Keyboard(unsigned char, int, int);
	void renderEnvironment();
};