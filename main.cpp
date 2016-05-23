// Cadaver Experiment Project
// =============================================================
//
// Developers:
// Victor Barradas	vrbarradasp@gmail.com
//
// =============================================================

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "include/freeglut.h"
#include "TimeData.h"	// Class for performance timer 
#include "SixAxisLoadCellData.h"
#include "MaxVoluntaryForce.h"
#include "SDNMeasurement.h"
#include "VMSErrorBasedPosMap.h"
#include "VMSRewardBasedPosMap.h"
#include "VMSErrorBasedVelMap.h"
#include "GraphicsMVF.h"
#include "GraphicsSDN.h"
#include "GraphicsVMS.h"


#ifdef _WIN64
#	pragma comment(lib, "lib/x64/freeglut.lib")
#	pragma comment(lib, "opengl32.lib")
#	pragma comment(lib, "glu32.lib")
#else
#	pragma comment(lib, "lib/x86/glut32.lib")
#	pragma comment(lib, "opengl32.lib")
#	pragma comment(lib, "glu32.lib")
#endif

// Function Prototypes
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
bool CreateMainWindow(HINSTANCE, int);
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM);
int initializeProgram();
void terminateProgram();
int update();

int countServerName = 0;

// Global Variables

TimeData gPerformanceTimer; 
SixAxisLoadCellData gLoadCellData = SixAxisLoadCellData(
	"C:\\data\\%s",
	""
	);

bool bIsRunning = false;

HINSTANCE hInst;
TCHAR ch = ' ';			// Character entered
FILE *fileConsole;

// Constants
const char CLASS_NAME[] = "WinMain";
const char APP_TITLE[] = "Hello World";	// Title bar text
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 1024;

int update() 
{
	return 0;
}

// Function called at init
int initializeProgram()
{
	return 0;
}

// Function called at exit
void terminateProgram()
{
}

//==================================================
// Starting point for a Windows application
// Parameters:
//		hInstance. Handle to the current instance of the application
//		hPrevInstance. Always NULL, obsolete parameter
//		lpCmdLine. Pointer to null-terminated string of command line args
//		nCmdShow. Specifies how the window is to be shown
//==================================================
int WINAPI WinMain( HINSTANCE	hInstance,
				   HINSTANCE	hPrevInstance,
				   LPSTR		lpCmdLine,
				   int			nCmdShow)
{
	MSG msg;
	// MessageBoxA(NULL, "akljrghjerk","ajkslg", MB_OK);

	// Create the window
	if (!CreateMainWindow(hInstance, nCmdShow)) {
		return false;
	}
	countServerName = 0;

	initializeProgram();

	/*SDNMeasurement gSDN(&gLoadCellData);
	GraphicsSDN gGraphics = GraphicsSDN(&gSDN);*/

	/*MaxVoluntaryForce gMVF(&gLoadCellData);
	GraphicsMVF gGraphics = GraphicsMVF(&gMVF);*/

	/*VMSErrorBasedPosMap gVMS(&gLoadCellData);
	GraphicsVMS gGraphics = GraphicsVMS(&gVMS);*/

	VMSRewardBasedPosMap gVMS(&gLoadCellData);
	GraphicsVMS gGraphics = GraphicsVMS(&gVMS);

	/*VMSErrorBasedVelMap gVMS(&gLoadCellData);
	GraphicsVMS gGraphics = GraphicsVMS(&gVMS);*/
	
	return msg.wParam;
}

//==================================================
// Window event callback function
//==================================================
LRESULT WINAPI WinProc( HWND	hWnd,
					   UINT	msg,
					   WPARAM	wParam,
					   LPARAM	lParam)
{
	switch(msg) {
	case WM_DESTROY:
		// Tell Windows to kill this program
		PostQuitMessage(0);
		return 0;
	case WM_CHAR:
		switch(wParam) {
		case 27:
			PostQuitMessage(WM_QUIT);
			return 0;
		default:
			ch = (TCHAR) wParam;	// Get the character
			return 0;
		}
	default:
		return DefWindowProc( hWnd, msg, wParam, lParam);
	}
}

//==================================================
// Create the window
// Returns: false on error
//==================================================
bool CreateMainWindow(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wcx;
	HWND hWnd;

	// Fill in the window class structure with parameters
	// that describe the main window
	wcx.cbSize = sizeof(wcx);				// Size of structure
	wcx.style = CS_HREDRAW | CS_VREDRAW;	// Redraw if size changes
	wcx.lpfnWndProc = WinProc;				// Points to window procedure
	wcx.cbClsExtra = 0;						// No extra class memory
	wcx.cbWndExtra = 0;						// Handle to instance
	wcx.hInstance = hInstance;				
	wcx.hIcon = NULL;						
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);	// Predefined arrow
	// Background brush
	wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcx.lpszMenuName = NULL;				// Name of menu resource
	wcx.lpszClassName = CLASS_NAME;			// Name of window class
	wcx.hIconSm = NULL;						// Small class icon

	// Register the window class
	// RegisterClassEx returns 0 on error
	if(RegisterClassEx(&wcx) == 0) {		// If error
		return false;
	}
	// Create window
	hWnd = CreateWindow(
		CLASS_NAME,				// Name of the window class
		APP_TITLE,				// Title bar text
		WS_OVERLAPPEDWINDOW,	// Window style
		CW_USEDEFAULT,			// Default horizontal position of window
		CW_USEDEFAULT,			// Defautl vertical position of window
		WINDOW_WIDTH,			// Width of window
		WINDOW_HEIGHT,			// Height of window
		(HWND) NULL,			// No parent window
		(HMENU) NULL,			// No menu
		hInstance,				// Handle to application instance
		(LPVOID) NULL);			// No window parameters

	// If there was an error creating the window
	if(!hWnd) {
		return false;
	}
	// ShowWindow(hWnd, nCmdShow);
	// Send a WM_PAINT message to the window procedure
	UpdateWindow(hWnd);

	return true;
}



