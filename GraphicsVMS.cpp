#include "GraphicsVMS.h"

GraphicsVMS::GraphicsVMS(VisuomotorScaling *experiment)
{
	vmsExperiment = experiment;
	cursorZeroPosX = WINDOW_WIDTH / 2.0;
	cursorZeroPosY = WINDOW_HEIGHT / 2.0;
	Initialize();
}

void GraphicsVMS::Display()
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

void GraphicsVMS::renderEnvironment()
{

	if (vmsExperiment->bIsCursorVisible) {
		glColor3d(0.0, 1.0, 0.0);
		drawFilledCircle(8 * vmsExperiment->cursorPosX + cursorZeroPosX, 8 * vmsExperiment->cursorPosY + cursorZeroPosY, 20.0, 0.0, 0.0, 1.0);
	}

	if (vmsExperiment->bIsTargetVisible) {
		if (vmsExperiment->bIsRewardVisible){
			if (vmsExperiment->bIsGivenReward){
				glColor3d(0.0, 1.0, 0.0);
				drawFilledCircle(8 * vmsExperiment->targetPosX + cursorZeroPosX, 8 * vmsExperiment->targetPosY + cursorZeroPosY, 20.0 + 4 * vmsExperiment->reachTolerance, 0.0, 1.0, 0.0);
			}
			else{
				glColor3d(1.0, 0.0, 0.0);
				drawFilledCircle(8 * vmsExperiment->targetPosX + cursorZeroPosX, 8 * vmsExperiment->targetPosY + cursorZeroPosY, 20.0 + 4 * vmsExperiment->reachTolerance, 1.0, 0.0, 0.0);
			}
			
		}
		else{
			glColor3d(0.0, 0.0, 1.0);
			drawCircle(8 * vmsExperiment->targetPosX + cursorZeroPosX, 8 * vmsExperiment->targetPosY + cursorZeroPosY, 20.0 + 4 * vmsExperiment->reachTolerance, 30);
		}
	}

	if (vmsExperiment->bIsEndExperiment){
		sprintf_s(gStatusString[0], "The End");
		outputText(cursorZeroPosX - 30, cursorZeroPosY + 100, gStatusString[0]);
	}

	if (vmsExperiment->bIsTrialFail){
		if (vmsExperiment->bIsTooFast){
			sprintf_s(gStatusString[0], "Too Fast");
			outputText(10, 10, gStatusString[0]);
		}
		else{
			sprintf_s(gStatusString[0], "Too Slow");
			outputText(10, 10, gStatusString[0]);
		}
	}
	/*sprintf_s(gStatusString[0], "%d",vmsExperiment->bIsCursorMoving);
	outputText(10, 10, gStatusString[0]);*/

}

void GraphicsVMS::Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;

	case 'B':	// "Start experiment"
	case 'b':
		vmsExperiment->startExperiment();
		break;

	case VK_SPACE:

		break;

	default:
		break;
	}
}