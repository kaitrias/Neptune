//from https://stackoverflow.com/questions/14148758/how-to-capture-the-desktop-in-opencv-ie-turn-a-bitmap-into-a-mat

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <Windows.h>
#include <iostream>
#include <string>
#include "Tchar.h"
using namespace std;
using namespace cv;

Mat hwnd2mat(HWND hwnd) {

	HDC hwindowDC, hwindowCompatibleDC;

	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	Mat src;
	BITMAPINFOHEADER  bi;

	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwnd, &windowsize);

	srcheight = windowsize.bottom * 2;
	srcwidth = windowsize.right * 2;
	height = windowsize.bottom;  //change this to whatever size you want to resize to
	width = windowsize.right;

	src.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	DeleteObject(hbwindow); DeleteDC(hwindowCompatibleDC); ReleaseDC(hwnd, hwindowDC);

	return src;
}

int main(int, char**) {
	TCHAR buff[MAX_PATH];
	char title[500];

	DWORD pid = 9936;
	TCHAR className[256] = _T("Qt5QWindowIcon");

	GetConsoleTitleA(title, 500);


	HWND desktop = FindWindow(NULL, _T("MEmuConsole"));
	//HWND shell = GetShellWindow();


	//Code from http://codewee.com/view.php?idx=18

	HWND hCurWnd = GetTopWindow(0);
	while (hCurWnd != NULL)
	{
		DWORD cur_pid;
		DWORD dwTheardId = GetWindowThreadProcessId(hCurWnd, &cur_pid);

		//if (cur_pid == pid)
		//{
		if (IsWindowVisible(hCurWnd) != 0)
		{
			TCHAR szClassName[256];
			GetClassName(hCurWnd, szClassName, 256);
			cout << szClassName;
			if (_tcscmp(szClassName, className) == 0) {
				desktop = hCurWnd;
				break;
			}
		}
		//}
		hCurWnd = GetNextWindow(hCurWnd, GW_HWNDNEXT);
	}


	//HWND desktop = ::FindWindowEx(0, 0, 0, 0);
	Mat frame;

	for (;;) {
		frame = hwnd2mat(desktop);
		try
		{
			imshow("desktop", frame);
		}
		catch (cv::Exception & e)
		{
			cerr << e.msg << endl; // output exception message
		}
		
		if (waitKey(30) >= 0) break;
	}
	return 0;
}