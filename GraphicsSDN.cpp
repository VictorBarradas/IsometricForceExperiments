#include "GraphicsSDN.h"

GraphicsSDN::GraphicsSDN(SDNMeasurement *experiment)
{
	sdnExperiment = experiment;
	cursorZeroPosX = WINDOW_WIDTH / 2.0;
	cursorZeroPosY = WINDOW_HEIGHT / 2.0;
	Initialize();
}

void GraphicsSDN::Display()
{
	// Clear frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(0.5f, 0.5f, 0.5f);

	//sprintf_s(gStatusString[0], 
	//	"%f", 
	//	sqrt(gMVF.cursorPosX*gMVF.cursorPosX+gMVF.cursorPosY*gMVF.cursorPosY)//gPerformanceTimer.getCurrentTime()
	//);

	glColor3f(0.0f, 0.9f, 0.0f);
	//if (gMVF.bIsSTD) {
	renderEnvironment();
	//}
	/*else if (gMVF.bIsMVF) {
	renderConsoleMVF();
	}*/

	// Present frame buffer
	glutSwapBuffers();

	// Recall Display at next frame
	// glutRedisplay();
}

void GraphicsSDN::renderEnvironment()
{

	if (sdnExperiment->bIsCursorVisible) {
		glColor3d(0.0,1.0,0.0);
		drawFilledCircle(8 * sdnExperiment->cursorPosX + cursorZeroPosX, 8 * sdnExperiment->cursorPosY + cursorZeroPosY, 20.0, 0.0, 0.0, 1.0);
	}

	if (sdnExperiment->bIsTargetVisible) {
		glColor3d(0.0, 0.0, 1.0);
		drawCircle(8 * sdnExperiment->targetPosX + cursorZeroPosX, 8 * sdnExperiment->targetPosY + cursorZeroPosY, 20.0 + 4 * sdnExperiment->reachTolerance, 30);
	}

	if (sdnExperiment->bIsTrialFail){
		sprintf_s(gStatusString[0], "Failed attempt");
		outputText(10, 10, gStatusString[0]);
	}

	if (sdnExperiment->bIsResting){
		sprintf_s(gStatusString[0], "Take a Break");
		outputText(10, 10, gStatusString[0]);
	}

	if (sdnExperiment->bIsEndExperiment){
		sprintf_s(gStatusString[0], "The End");
		outputText(cursorZeroPosX - 30, cursorZeroPosY + 100, gStatusString[0]);
	}

}

void GraphicsSDN::Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;

	case 'B':	// "Start experiment"
	case 'b':
		sdnExperiment->startExperiment();
		break;

	case VK_SPACE:

		break;

	default:
		break;
	}
}