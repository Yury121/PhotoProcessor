#ifndef _CVDFACE_VIL_1_H_ 
#define  _CVDFACE_VIL_1_H_

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

extern "C" __declspec( dllexport ) void __stdcall 
cvfind(char * ffname, char * data, FRECT * dfaces, int & szface);


#endif