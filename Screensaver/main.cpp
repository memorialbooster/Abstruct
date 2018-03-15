#include <windows.h>

#include <scrnsave.h>
#include <commctrl.h>

#pragma comment(lib, "scrnsave.lib")
#pragma comment(lib, "ComCtl32.lib")

#include <gl/gl.h>
//#include <gl/glu.h>

#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "glu32.lib")

#include <wingdi.h>

#include "../mainwindow.h"

MainWindow *mainWindow = nullptr;
std::vector<MONITORINFO> monInf;

#include <iostream> // debug!!!
#include <fstream> // debug!!!
std::ofstream myfile; // debug!!!

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor,
    HDC      hdcMonitor,
    LPRECT   lprcMonitor,
    LPARAM   dwData)
{
    MONITORINFO info;
    info.cbSize = sizeof(info);

    if (GetMonitorInfo(hMonitor, &info))
    {
        monInf.push_back(info);
    }

    return true;
}

VOID CALLBACK TimerRedraw(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    myfile << "TimerRedraw: hWnd=" << hWnd << " uMsg=" << uMsg << " idEvent=" << idEvent << "\n"; // debug!!!
    HDC hdc = GetDC(hWnd);
    mainWindow->timerDrawScene();
    SwapBuffers(hdc);
}

LONG WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    static UINT		uTimer = 0;		// Timer for the screen saver
    static HDC hdc;
    static HGLRC hglrc;
    myfile << "ScreenSaverProc: hWnd=" << hWnd << " message=" << message << "\n"; // debug!!!
    switch (message)
    {
    case WM_CREATE:				// Creation of the screen saver window
    {
        myfile.open("debug_out.log"); // debug!!!

        hdc = GetDC(hWnd);

        static PIXELFORMATDESCRIPTOR pfd = {
            sizeof(PIXELFORMATDESCRIPTOR),		// Size of struct
            1,									// Version of struct
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // 
            PFD_TYPE_RGBA,				// Always put this
            32,							// Always put this, size of the color buffer
            0,0,0,0,0,0,0,0,0,0,0,0,0,	// You have no control over
            16,							// Depth of the Z buffer
            8,							// Stencil buffer
            0,0,0,0,0,0 };				// Either obsolete, or we don't care about

        int nPF = ChoosePixelFormat(hdc, &pfd);

        SetPixelFormat(hdc, nPF, &pfd);

        // Create rendering context
        hglrc = wglCreateContext(hdc);
        wglMakeCurrent(hdc, hglrc);

        //Lime Green means its a-workin'!
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        //glEnable(GL_DEPTH_TEST);
        glShadeModel(GL_SMOOTH);
        //glClearDepth(1.0f);
        //glClearStencil(0);
        //glDepthFunc(GL_LEQUAL);
        //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        //glEnable(GL_TEXTURE_2D);

        //glEnable(GL_COLOR_MATERIAL);
        //glShadeModel(GL_SMOOTH);
        //glColorMaterial(GL_FRONT, GL_DIFFUSE);
        //glEnable(GL_LIGHTING);

        //Resizing the Window. Only gets called once
        //I don't think you can make a ScreenSaver that can be resized :P
        unsigned int nWidth = GetSystemMetrics(SM_CXSCREEN);
        unsigned int nHeight = GetSystemMetrics(SM_CYSCREEN);
        unsigned int monNum = GetSystemMetrics(SM_CMONITORS);
        unsigned int xVitual = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        unsigned int yVitual = GetSystemMetrics(SM_CYVIRTUALSCREEN);

        {
            myfile << "#######################\n"; // debug!!!
            myfile << "# monNum      = " << monNum << "\n"; // debug!!!
            myfile << "# xVitual     = " << xVitual << "\n"; // debug!!!
            myfile << "# yVitual     = " << yVitual << "\n"; // debug!!!
            myfile << "# nWidth      = " << nWidth << "\n"; // debug!!!
            myfile << "# nHeight     = " << nHeight << "\n"; // debug!!!
            myfile << "#######################\n"; // debug!!!
        }

        if (monNum == 2)
        {
            monInf.reserve(2);
        }

        EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);

        //glViewport(0, 0, nWidth, nHeight);
        glViewport(0, 0, xVitual, yVitual);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, nWidth, 0, nHeight, -1, 2);
        //glOrtho(0, 680, 0, 600, -1, 2);
        //gluPerspective(35.0f, float(nWidth) / float(nHeight), 1.0f, 500.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        mainWindow = new MainWindow(&monInf);

        //uTimer = (UINT)SetTimer(hWnd, 2, 10, (TIMERPROC)TimerRedraw);
        uTimer = (UINT)SetTimer(hWnd, 1, USER_TIMER_MINIMUM, NULL);
    }
    break;
    case WM_ERASEBKGND:			// Erases the screen saver background
    {
    }
    break;
    case WM_TIMER:				// Handles the timer
    {
        myfile << "ScreenSaverProc: WM_TIMER\n"; // debug!!!
        mainWindow->timerDrawScene();
        SwapBuffers(hdc);
    }
    break;
    case WM_DESTROY:			// Cleans up the screen saver window
    {
    }
    break;
    case WM_PAINT:				// Draws to the window
    {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hWnd, &ps);
        myfile << "ScreenSaverProc: WM_PAINT\n"; // debug!!!
        mainWindow->paintGL();
        SwapBuffers(hdc);
        EndPaint(hWnd, &ps);
    }
    break;
    default:
    {
        return DefScreenSaverProc(hWnd, message, wparam, lparam);
    }
    }

    return 0;
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
    return TRUE;
}
