#include "GraphicsMVF.h"

GraphicsMVF::GraphicsMVF(MaxVoluntaryForce *experiment)
{
	mvfExperiment = experiment;
	cursorZeroPosX = WINDOW_WIDTH / 2.0;
	cursorZeroPosY = WINDOW_HEIGHT / 2.0;
	Initialize();
}

void GraphicsMVF::Display()
{
	// Clear frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glColor3f(0.5f, 0.5f, 0.5f);

	renderEnvironment();

	// Present frame buffer
	glutSwapBuffers();

	// Recall Display at next frame
	// glutRedisplay();
}

void GraphicsMVF::renderEnvironment()
{
	if (mvfExperiment->bIsTargetDirectionVisible) {
		double direction = mvfExperiment->targetDirection;
		drawLine(cursorZeroPosX, cursorZeroPosY, cursorZeroPosX + 2000 * cos(direction), cursorZeroPosY + 2000 * sin(direction), 1.0, 1.0, 1.0);
		sprintf_s(gStatusString[0], "Go!");
		outputText(10, 10, gStatusString[0]);
	}

	if (mvfExperiment->bIsCursorVisible) {
		glColor3f(0.0f, 0.9f, 0.0f);
		drawFilledCircle(2.0 * mvfExperiment->cursorPosX + cursorZeroPosX, 2.0 * mvfExperiment->cursorPosY + cursorZeroPosY, 20.0, 0.0, 0.0, 1.0);
	}

	if (mvfExperiment->bIsCenterVisible) {
		glColor3f(0.0f, 0.9f, 0.9f);
		drawCircle(cursorZeroPosX, cursorZeroPosY, 20, 30);
	}

	if (mvfExperiment->bIsWaiting){
		sprintf_s(gStatusString[0], "Take a Break");
		outputText(10, 10, gStatusString[0]);
	}

	if (mvfExperiment->bIsEndExperiment){
		sprintf_s(gStatusString[0], "The End");
		outputText(cursorZeroPosX - 30, cursorZeroPosY + 100, gStatusString[0]);
	}

}

void GraphicsMVF::Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;

	case 'B':	// "Start experiment"
	case 'b':
		mvfExperiment->startExperiment();
		break;

	case VK_SPACE:

		break;

	default:
		break;
	}
}