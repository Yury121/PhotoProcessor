#ifndef _CVDFACE_VIL_IMP_H_ 
#define  _CVDFACE_VIL_IMP_H_

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

typedef struct _frect{ //face rect struct
	int x;
	int y;
	int width;
	int height;
	_frect(){
		x=0;
		y=0;
		width =0;
		height=0;
	};
} FRECT, *PFRECT;

//extern "C" __declspec( dllexport ) void __stdcall cvfind(std::string fname, std::string profile, std::vector<FRECT> &dfaces);





#endif