#include "Graphics.h"

Graphics *Graphics::instance = NULL;

void Graphics::Initialize()
{
	setInstance();
	int argc = 1;
	char *argv[1] = { (char*)"Something" };
	glutInit(&argc, argv);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Isometric Force Task");
	glutCreateMenu(NULL);
	glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);

	glutDisplayFunc(staticDisplayCallback);
	glutReshapeFunc(staticReshapeCallback);
	//atexit(terminateProgram);  // Called after glutMainLoop ends


	glutKeyboardFunc(staticKeyboardCallback);
	glutIdleFunc(staticIdleCallback);

	// Call the GLUT main loop
	glutMainLoop();
}

void Graphics::setInstance() {
	instance = this;
}

void Graphics::staticDisplayCallback() {
	instance->Display();
}

void Graphics::staticReshapeCallback(int width, int height){
	instance->Reshape(width, height);
}

void Graphics::staticKeyboardCallback(unsigned char key, int x, int y){
	instance->Keyboard(key, x, y);
}

void Graphics::staticIdleCallback(){
	instance->Idle();
}

void Graphics::Reshape(int width, int height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0.0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Graphics::Idle()
{
	// Test the update function here and compare the data logging to 
	// determine if we are not constraints
	glutPostRedisplay();
}

void Graphics::drawCircle(double cx, double cy, double r, int num_segments)
{
	float theta = 2 * 3.1415926 / float(num_segments);
	float tangetial_factor = tanf(theta);//calculate the tangential factor 

	float radial_factor = cosf(theta);//calculate the radial factor 

	float x = r;//we start at angle = 0 

	float y = 0;

	glBegin(GL_LINE_LOOP);
	for (int ii = 0; ii < num_segments; ii++)
	{
		glVertex2f(x + cx, y + cy);//output vertex 

		//calculate the tangential vector 
		//remember, the radial vector is (x, y) 
		//to get the tangential vector we flip those coordinates and negate one of them 

		float tx = -y;
		float ty = x;

		//add the tangential vector 

		x += tx * tangetial_factor;
		y += ty * tangetial_factor;

		//correct using the radial factor 

		x *= radial_factor;
		y *= radial_factor;
	}
	glEnd();
	glFlush();
}

void Graphics::drawFilledCircle(double x, double y, double r, double red, double green, double blue)
{
	const float PI = 3.14159;

	//fill the frame buffer with the background color
	//glClear(GL_COLOR_BUFFER_BIT);

	//fill a circle using a different color for each point
	//You get totally different drawings using the drawing
	//modes GL_POLYGON and GL_TRIANGLE_FAN
	glBegin(GL_POLYGON);

	//fill a circle using a triangle fan
	//glBegin(GL_TRIANGLE_FAN);
	//All triangles fan out starting with this point
	glVertex2f(x, y);
	for (int i = 0; i <= 361; i++)
	{
		glColor3f(red, green, blue);
		glVertex2f(r*cos(i*PI / 180) + x, r*sin(i*PI / 180) + y);
	}
	glEnd();

	//flush the buffer so the circle displays
	//immediately
	glFlush();
}

void Graphics::drawLine(double x_init, double y_init, double x_fin, double y_fin, double r, double g, double b)
{
	glLineWidth(2.5);
	glColor3f(r, g, b);
	glBegin(GL_LINES);
	glVertex2f(x_init, y_init);
	glVertex2f(x_fin, y_fin);
	glEnd();
	glFlush();
}

void Graphics::outputText(int x, int y, char *strOut)
{
	int len;
	glRasterPos2f((float)x, (float)WINDOW_HEIGHT - y - 25);
	len = (int)strlen(strOut);
	{
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, (UCHAR*)strOut);
	}
}