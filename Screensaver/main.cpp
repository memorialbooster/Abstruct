#include <windows.h>

#include <scrnsave.h>
#include <commctrl.h>

#pragma comment(lib, "scrnsave.lib")
#pragma comment(lib, "ComCtl32.lib")

#include <gl/gl.h>
//#include <gl/glu.h>

#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "glu32.lib")

#include "../mainwindow.h"

LONG WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case WM_CREATE:				// Creation of the screen saver window
    {
    }
    break;
    case WM_ERASEBKGND:			// Erases the screen saver background
    {
    }
    break;
    case WM_TIMER:				// Handles the timer
    {
    }
    break;
    case WM_DESTROY:			// Cleans up the screen saver window
    {
    }
    break;
    case WM_PAINT:				// Draws to the window
    {
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
