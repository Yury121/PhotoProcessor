// testdll.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <Winbase.h>
#include "cvdface.h"

int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _DEBUG
	HMODULE hLib = ::LoadLibrary(_T("../cvdface/debug/cvdface.dll"));
#else
	HMODULE hLib = ::LoadLibrary(_T("../cvdface/release/cvdface.dll"));
#endif
	FARPROC cv = ::GetProcAddress(hLib, "cvfindext");

	char * data = "d:/Worker/PhotoProcessor/ocvtest/data/haarcascades/haarcascade_frontalface_alt.xml";
	char * fname = "d:/Worker/PhotoProcessor/ocvtest/lena.jpg";
	FRECT rect[20] = {};
	int sz = 20;
	if (cv)
		((void (__stdcall*)( char *, char *, FRECT *, int &, double ,int, int, int))(PROC) cv)(fname, data,(FRECT * )rect, sz, 1.1, 3, 30,30);
	printf("\nFound objects %d\n", sz );
	for(int i =0; i<sz; i++){
		printf("-> x = %d, y = %d; width = %d, heght = %d;\n",
			rect[i].x, rect[i].y, rect[i].width, rect[i].height);
	}


	return 0;
}

