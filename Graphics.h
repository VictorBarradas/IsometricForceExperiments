#pragma once
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "include/freeglut.h"

#ifdef _WIN64
#	pragma comment(lib, "lib/x64/freeglut.lib")
#	pragma comment(lib, "opengl32.lib")
#	pragma comment(lib, "glu32.lib")
#else
#	pragma comment(lib, "lib/x86/glut32.lib")
#	pragma comment(lib, "opengl32.lib")
#	pragma comment(lib, "glu32.lib")
#endif

class Graphics
{
private:
	static void staticDisplayCallback();
	static void staticReshapeCallback(int, int);
	static void staticKeyboardCallback(unsigned char, int, int);
	static void staticIdleCallback();
	
	void Reshape(int,int);
	void Idle();

	void renderEnvironment();

protected:
	static Graphics *instance;

public:
	const int WINDOW_WIDTH = 1280;
	const int WINDOW_HEIGHT = 1024;

	virtual void Display() = 0;
	virtual void Keyboard(unsigned char, int, int) = 0;

	void Initialize();
	void drawFilledCircle(double, double, double, double, double, double);
	void drawCircle(double, double, double, int);
	void drawLine(double, double, double, double, double, double, double);
	void outputText(int, int, char*);

	void setInstance();
};
