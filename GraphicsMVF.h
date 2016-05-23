#include <stdio.h>
#include <string.h>
#include "Graphics.h"
#include "MaxVoluntaryForce.h"

class GraphicsMVF : public Graphics
{
private:
	MaxVoluntaryForce *mvfExperiment;
	int cursorZeroPosX;
	int cursorZeroPosY;
	char gStatusString[10][300];
public:
	GraphicsMVF(MaxVoluntaryForce*);
	void Display();
	void Keyboard(unsigned char, int, int);
	void renderEnvironment();
};