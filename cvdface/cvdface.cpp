// cvdface.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <opencv2/objdetect/objdetect.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>


#include <cctype>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>

#include "cvdface.h"
using namespace std;
using namespace cv;


#ifdef _DEBUG 
#pragma comment( lib, "opencv_highgui246d.lib")
#pragma comment( lib, "opencv_core246d.lib")
#pragma comment( lib, "opencv_imgproc246d.lib")
//#pragma comment( lib, "zlibd.lib")
//#pragma comment( lib, "IlmImfd.lib")
//#pragma comment( lib, "libjasperd.lib")
//#pragma comment( lib, "libjpegd.lib")
//#pragma comment( lib, "libpng.lib")
//#pragma comment( lib, "libtiffd.lib")
#pragma comment( lib, "opencv_objdetect246d.lib")
//#pragma comment( lib, "opencv_gpu246d.lib")


//#pragma comment( lib, "opencv_features2d246.lib")
//#pragma comment( lib, "opencv_haartraining_engined.lib")



#else
#pragma comment( lib, "opencv_highgui246.lib")
#pragma comment( lib, "opencv_core246.lib")
#pragma comment( lib, "opencv_imgproc246.lib")
//#pragma comment( lib, "libtiff.lib")
//#pragma comment( lib, "libpngd.lib")
//#pragma comment( lib, "libjpeg.lib")
//#pragma comment( lib, "libjasper.lib")
//#pragma comment( lib, "IlmImf.lib")
//#pragma comment( lib, "zlib.lib")
#pragma comment( lib, "opencv_objdetect246.lib")
//#pragma comment( lib, "opencv_gpu246.lib")
//#pragma comment( lib, "opencv_haartraining_engine.lib")

#endif


extern "C" __declspec( dllexport ) void __stdcall 
cvfind(char * ffname, char * data, FRECT * dfaces, int & szface)
{
    CascadeClassifier cascade;
    Mat image;

    double scale = 1;
	memset((unsigned char *) dfaces, 0, sizeof(FRECT)*szface);
	string fname = ffname;
	string profile = data;
	if (profile.empty()) return;
	if (fname.empty()) return;
	
	if (!cascade.load(profile)) return;
	if (fname.size()){
		image = imread(fname);
	}

    int i = 0;
    double t = 0;
    vector<Rect> faces;
    Mat gray, smallImg( cvRound (image.rows/scale), cvRound(image.cols/scale), CV_8UC1 );
    cvtColor( image, gray, CV_BGR2GRAY );
    resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );
	FRECT f1;
    t = (double)cvGetTickCount();
	/* =============================
	void CascadeClassifier::detectMultiScale( const Mat& image,
		vector<Rect>& objects, double scaleFactor=1.1, int minNeighbors=3, int
		flags=0, Size minSize=Size());
image -- Matrix of type CV 8U containing the image in which to detect objects.
objects -- Vector of rectangles such that each rectangle contains the detected object.
scaleFactor -- Specifies how much the image size is reduced at each image scale.
minNeighbors -- Speficifes how many neighbors should each candiate rectangle have to retain it.
				flags This parameter is not used for new cascade and have the same meaning for old cascade
				as in function cvHaarDetectObjects.
minSize --	The minimum possible object size. Objects smaller than that are ignored.
	*/
    cascade.detectMultiScale( smallImg, faces, 1.1, 2, 0 
		//|CV_HAAR_FIND_BIGGEST_OBJECT 
		//|CV_HAAR_DO_ROUGH_SEARCH
        |CV_HAAR_SCALE_IMAGE   ,     Size(30, 30) );
	int k = 0;
	for(i=0; i< faces.size(); i++){
		dfaces[i].x = faces[i].x;
		dfaces[i].y = faces[i].y;
		dfaces[i].width = faces[i].width;
		dfaces[i].height = faces[i].height;
		k++;
		if (k >= szface) 
			break;
		//dfaces.push_back(f1);


	}
	szface = k;


	return;
}


/*======================================*/

extern "C" __declspec( dllexport ) void __stdcall 
cvfindext(char * ffname, char * data, FRECT * dfaces, int & szface, 
		  double scaleFactor, int minNeighbors, int minw, int minh)
{
    CascadeClassifier cascade;
    Mat image;

    double scale = 1;
	memset((unsigned char *) dfaces, 0, sizeof(FRECT)*szface);
	string fname = ffname;
	string profile = data;
	if (profile.empty()) return;
	if (fname.empty()) return;
	
	if (!cascade.load(profile)) return;
	if (fname.size()){
		image = imread(fname);
	}

    int i = 0;
    double t = 0;
    vector<Rect> faces;
    Mat gray, smallImg( cvRound (image.rows/scale), cvRound(image.cols/scale), CV_8UC1 );
    cvtColor( image, gray, CV_BGR2GRAY );
    resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );
	FRECT f1;
    t = (double)cvGetTickCount();
	/* =============================
	void CascadeClassifier::detectMultiScale( const Mat& image,
		vector<Rect>& objects, double scaleFactor=1.1, int minNeighbors=3, int
		flags=0, Size minSize=Size());
image -- Matrix of type CV 8U containing the image in which to detect objects.
objects -- Vector of rectangles such that each rectangle contains the detected object.
scaleFactor -- Specifies how much the image size is reduced at each image scale.
minNeighbors -- Speficifes how many neighbors should each candiate rectangle have to retain it.
				flags This parameter is not used for new cascade and have the same meaning for old cascade
				as in function cvHaarDetectObjects.
minSize --	The minimum possible object size. Objects smaller than that are ignored.
	*/
    cascade.detectMultiScale( smallImg, faces, scaleFactor, minNeighbors, 0 
		//|CV_HAAR_FIND_BIGGEST_OBJECT 
		//|CV_HAAR_DO_ROUGH_SEARCH
        |CV_HAAR_SCALE_IMAGE   ,     Size(minw, minh) );
	int k = 0;
	for(i=0; i< faces.size(); i++){
		dfaces[i].x = faces[i].x;
		dfaces[i].y = faces[i].y;
		dfaces[i].width = faces[i].width;
		dfaces[i].height = faces[i].height;
		k++;
		if (k >= szface) 
			break;
		//dfaces.push_back(f1);


	}
	szface = k;


	return;
}
