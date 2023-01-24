// vklt.h header file 
#ifndef VKLT_VER_1_33978AE6_9BBD_46a0_B67D
#define VKLT_VER_1_33978AE6_9BBD_46a0_B67D

#include <stdlib.h>
//----------------------------------------------------------------
// TYPEDEFS
//----------------------------------------------------------------
#define KLT_BOOL int
#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

typedef float KLT_locType;
typedef unsigned char KLT_PixelType;

#define KLT_TRACKED           0
#define KLT_NOT_FOUND        -1
#define KLT_SMALL_DET        -2
#define KLT_MAX_ITERATIONS   -3
#define KLT_OOB              -4
#define KLT_LARGE_RESIDUE    -5

#define MAX_KERNEL_WIDTH 	71

#define MAXPOINTNUM 128

//----------------------------------------------------------------
typedef struct  {
  int ncols;
  int nrows;
  float *data;
}  _KLT_FloatImageRec, *_KLT_FloatImage;

//----------------------------------------------------------------
typedef struct  {
  int width;
  float data[MAX_KERNEL_WIDTH];
}  ConvolutionKernel;
//----------------------------------------------------------------

typedef struct  tagKLT_TrackingContextRec{
	/* Available to user */
	int mindist;			/* min distance b/w features */
	int window_width, window_height;
	KLT_BOOL sequentialMode;	/* whether to save most recent image to save time */
	/* can set to TRUE manually, but don't set to */
	/* FALSE manually */
	KLT_BOOL smoothBeforeSelecting;	/* whether to smooth image before */
	/* selecting features */
	KLT_BOOL writeInternalImages;	/* whether to write internal images */
	/* tracking features */
	KLT_BOOL lighting_insensitive;  /* whether to normalize for gain and bias (not in original algorithm) */

	/* Available, but hopefully can ignore */
	int min_eigenvalue;		/* smallest eigenvalue allowed for selecting */
	float min_determinant;	/* th for determining lost */
	float min_displacement;	/* th for stopping tracking when pixel changes little */
	int max_iterations;		/* th for stopping tracking when too many iterations */
	float max_residue;		/* th for stopping tracking when residue is large */
	float grad_sigma;
	float smooth_sigma_fact;
	float pyramid_sigma_fact;
	float step_factor;  /* size of Newton steps; 2.0 comes from equations, 1.0 seems to avoid overshooting */
	int nSkippedPixels;		/* # of pixels skipped when finding features */
	int borderx;			/* border in which features will not be found */
	int bordery;
	int nPyramidLevels;		/* computed from search_ranges */
	int subsampling;		/* 		" */
	
	/* for affine mapping */ 
	int affine_window_width, affine_window_height;
	int affineConsistencyCheck; /* whether to evaluates the consistency of features with affine mapping 
								-1 = don't evaluates the consistency
								0 = evaluates the consistency of features with translation mapping
								1 = evaluates the consistency of features with similarity mapping
								2 = evaluates the consistency of features with affine mapping
								*/
	int affine_max_iterations;  
	float affine_max_residue;
	float affine_min_displacement;        
	float affine_max_displacement_differ; /* th for the difference between the displacement calculated 
										  by the affine tracker and the frame to frame tracker in pel*/

	/* User must not touch these */
	void *pyramid_last;
	void *pyramid_last_gradx;
	void *pyramid_last_grady;
}  KLT_TrackingContextRec, *KLT_TrackingContext;
//----------------------------------------------------------------
typedef struct _tagKLT_FeatureRec {
	float x;
	float y;
	int val;	
	int control;			// control this point
	/* for affine mapping */
	_KLT_FloatImage aff_img; 
	_KLT_FloatImage aff_img_gradx;
	_KLT_FloatImage aff_img_grady;
	float aff_x;
	float aff_y;
	float aff_Axx;
	float aff_Ayx;
	float aff_Axy;
	float aff_Ayy;
}  KLT_FeatureRec, *KLT_Feature;
//----------------------------------------------------------------
typedef struct  tagKLT_FeautureList{
	int nFeatures;
	KLT_Feature *feature;
}  KLT_FeatureListRec, *KLT_FeatureList;
//----------------------------------------------------------------
typedef struct  {
	int nFrames;
	KLT_Feature *feature;
}  KLT_FeatureHistoryRec, *KLT_FeatureHistory;
//----------------------------------------------------------------
typedef struct  {
	int nFrames;
	int nFeatures;
	KLT_Feature **feature;
}  KLT_FeatureTableRec, *KLT_FeatureTable;
//----------------------------------------------------------------

typedef struct  {
	int subsampling;
	int nLevels;
	_KLT_FloatImage *img;
	int *ncols, *nrows;
} _KLT_PyramidRec, *_KLT_Pyramid;
//----------------------------------------------------------------
typedef float *_FloatWindow;
typedef enum {SELECTING_ALL, REPLACING_SOME} selectionMode;

typedef struct _tagIMGPROCSTRUCT{
	unsigned __int32 iMask;			// initialization mask
	int ncols;						//size
	int nrows;
	int cntimg;						// number of images
	int * pointlist;
	unsigned char * featuremap;		// 
	unsigned char * img;			// processed images
	unsigned char * img2;			// processed second images 
	KLT_TrackingContext tc;
	KLT_FeatureList fl;				// feature list
	_KLT_FloatImage gradx;			// temporary image
	_KLT_FloatImage grady;			// temporary image
	_KLT_FloatImage floatimg;		// temporary image
	_KLT_FloatImage tmpimg;			// temporary image
	_KLT_FloatImage cstmpimg;		// temporary image for convolution kernel

}IMGPROCSTRUCT, *PIMGPROCSTRUCT;

// CONSTANTS

 const int mindist = 5;
 const int window_size = 7;
 const int min_eigenvalue = 1;
 const float min_determinant = 0.01f;
 const float min_displacement = 0.1f;
 const int max_iterations = 10;
 const float max_residue = 10.0f;
 const float grad_sigma = 1.0f;
 const float smooth_sigma_fact = 0.1f;
 const float pyramid_sigma_fact = 0.9f;
 const float step_factor = 1.0f;
 const KLT_BOOL sequentialMode = FALSE;
 const KLT_BOOL lighting_insensitive = FALSE;
/* for affine mapping*/
 const int affineConsistencyCheck = -1;
 const int affine_window_size = 15;
 const int affine_max_iterations = 10;
 const float affine_max_residue = 10.0;
 const float affine_min_displacement = 0.02f;
 const float affine_max_displacement_differ = 1.5f;

 const KLT_BOOL smoothBeforeSelecting = TRUE;
 const KLT_BOOL writeInternalImages = FALSE;
 const int search_range = 15;
 const int nSkippedPixels = 3;







// EXTERNALS

 extern unsigned char * wBuffer;
 extern TCHAR errout[2048];

 extern int KLT_verbose;



//FUNCTIONS


void _sortPointList(int *pointlist, int npoints);
//static void _fillFeaturemap(int x, int y, unsigned char *featuremap, int mindist, int ncols, int nrows);

////////////////////////////////////////////////////////////////////////////////////////


const __int32 NCH_FILTER1[] = {1,1,1, 1,1,1, 1,1,1, 1,9};
const __int32 NCH_FILTER2[] = {1,1,1, 1,2,1, 1,1,1, 1,10};
const __int32 NCH_FILTER3[] = {1,2,1, 2,4,2, 1,2,1, 1,16};

const __int32 HCH_FILTER1[] = {0,-1,0, -1,5,-1,    0,-1,0};
const __int32 HCH_FILTER2[] = {-1,-1,-1, -1,9,-1, -1,-1,-1};
const __int32 HCH_FILTER3[] = {1,-2,1, -2,5,-2, 1,-2,1};
const __int32 HCH_FILTER4[] = {1,1,1, 1,-9,1, 1, 1, 1};
const __int32 HCH_FILTER5[] = {-1,2,-1, 2,-5,2, -1,2,-1};

const __int32 LCH_FILTER1[] = {1, -2, 1, -2, 0, -2, 1, -2,1};
const __int32 LCH_FILTER2[] = {-1, -1, -1,   -1,  12,  -1, -1, -1,-1};
const __int32 LCH_FILTER3[] = {-1, 1, 1,   -1, -2,  1,  -1, 1,  1};
const __int32 LCH_FILTER4[] = {-1, -2, -1, -2, 12, -2,  -1, -2,-1};
const __int32 LCH_FILTER5[] = {1, -2, 1,   -2, -8, -2,  1, -2,1};
const __int32 LCH_FILTER6[] = {-1, 1, 1,	-1, -2,  1, -1, 1,  1};
const __int32 LCH_FILTER7[] = {1,	 1,	 1,	 1,  4,  1, -1, -1,-1};


const unsigned char GAMMA0_4[]={0,28,37,43,48,53,57,61,64,67,70,73,75,78,80,82,84,86,88,90,92,94,96,97,99,101,102,104,105,107,108,110,111,113,114,115,117,118,119,120,122,123,124,125,126,127,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,149,150,151,152,153,154,155,155,156,157,158,159,160,160,161,162,163,164,164,165,166,167,167,168,169,170,170,171,172,173,173,174,175,175,176,177,177,178,179,179,180,181,182,182,183,183,184,185,185,186,187,187,188,189,189,190,190,191,192,192,193,194,194,195,195,196,197,197,198,198,199,199,200,201,201,202,202,203,203,204,205,205,206,206,207,207,208,208,209,209,210,211,211,212,212,213,213,214,214,215,215,216,216,217,217,218,218,219,219,220,220,221,221,222,222,223,223,224,224,225,225,226,226,227,227,228,228,229,229,230,230,230,231,231,232,232,233,233,234,234,235,235,235,236,236,237,237,238,238,239,239,240,240,240,241,241,242,242,243,243,243,244,244,245,245,246,246,246,247,247,248,248,248,249,249,250,250,251,251,251,252,252,253,253,253,254,254,255,255};
const unsigned char GAMMA0_6[]={0,9,14,18,21,24,27,29,32,34,37,39,41,43,45,47,48,50,52,54,55,57,59,60,62,63,65,66,68,69,71,72,73,75,76,77,79,80,81,83,84,85,86,88,89,90,91,92,94,95,96,97,98,99,100,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,137,138,139,140,141,142,143,144,145,145,146,147,148,149,150,151,151,152,153,154,155,156,156,157,158,159,160,161,161,162,163,164,165,165,166,167,168,169,169,170,171,172,173,173,174,175,176,176,177,178,179,179,180,181,182,182,183,184,185,185,186,187,188,188,189,190,191,191,192,193,194,194,195,196,196,197,198,199,199,200,201,201,202,203,203,204,205,206,206,207,208,208,209,210,210,211,212,212,213,214,214,215,216,216,217,218,218,219,220,220,221,222,222,223,224,224,225,226,226,227,228,228,229,230,230,231,231,232,233,233,234,235,235,236,237,237,238,238,239,240,240,241,242,242,243,243,244,245,245,246,247,247,248,248,249,250,250,251,251,252,253,253,254,254,255};
const unsigned char GAMMA0_8[]={0,3,5,7,9,11,13,14,16,18,19,21,22,24,25,26,28,29,31,32,33,35,36,37,39,40,41,42,44,45,46,47,48,50,51,52,53,54,56,57,58,59,60,61,63,64,65,66,67,68,69,70,71,73,74,75,76,77,78,79,80,81,82,83,84,85,86,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,140,141,142,143,144,145,146,147,148,149,150,151,151,152,153,154,155,156,157,158,159,160,161,161,162,163,164,165,166,167,168,169,169,170,171,172,173,174,175,176,177,177,178,179,180,181,182,183,183,184,185,186,187,188,189,190,190,191,192,193,194,195,196,196,197,198,199,200,201,202,202,203,204,205,206,207,207,208,209,210,211,212,212,213,214,215,216,217,217,218,219,220,221,222,222,223,224,225,226,227,227,228,229,230,231,232,232,233,234,235,236,236,237,238,239,240,240,241,242,243,244,245,245,246,247,248,249,249,250,251,252,253,253,254,255};
const unsigned char GAMMA1_0[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255};
const unsigned char GAMMA1_2[]={0,0,1,1,2,2,3,3,4,5,5,6,7,7,8,9,9,10,11,11,12,13,13,14,15,16,16,17,18,19,20,20,21,22,23,24,24,25,26,27,28,28,29,30,31,32,33,34,34,35,36,37,38,39,40,40,41,42,43,44,45,46,47,48,49,49,50,51,52,53,54,55,56,57,58,59,60,61,62,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,130,131,132,133,134,135,136,137,138,139,140,141,142,144,145,146,147,148,149,150,151,152,153,155,156,157,158,159,160,161,162,163,165,166,167,168,169,170,171,172,173,175,176,177,178,179,180,181,183,184,185,186,187,188,189,191,192,193,194,195,196,197,199,200,201,202,203,204,205,207,208,209,210,211,212,214,215,216,217,218,219,221,222,223,224,225,226,228,229,230,231,232,234,235,236,237,238,239,241,242,243,244,245,247,248,249,250,251,253,254,255};
const unsigned char GAMMA1_4[]={0,0,0,1,1,1,1,2,2,2,3,3,4,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,12,12,13,13,14,15,15,16,16,17,18,18,19,20,20,21,22,22,23,24,25,25,26,27,28,28,29,30,31,31,32,33,34,34,35,36,37,38,38,39,40,41,42,43,43,44,45,46,47,48,49,49,50,51,52,53,54,55,56,57,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,103,104,105,106,107,108,109,110,111,112,113,115,116,117,118,119,120,121,122,124,125,126,127,128,129,130,132,133,134,135,136,137,139,140,141,142,143,145,146,147,148,149,151,152,153,154,155,157,158,159,160,161,163,164,165,166,168,169,170,171,173,174,175,176,178,179,180,181,183,184,185,187,188,189,190,192,193,194,196,197,198,200,201,202,203,205,206,207,209,210,211,213,214,215,217,218,219,221,222,223,225,226,227,229,230,232,233,234,236,237,238,240,241,242,244,245,247,248,249,251,252,254,255};
const unsigned char GAMMA1_6[]={0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,3,3,3,4,4,4,5,5,5,6,6,7,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,16,16,17,18,18,19,19,20,21,21,22,23,23,24,25,25,26,27,27,28,29,29,30,31,31,32,33,34,34,35,36,37,38,38,39,40,41,42,42,43,44,45,46,46,47,48,49,50,51,52,53,53,54,55,56,57,58,59,60,61,62,63,64,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,83,84,85,86,87,88,89,90,91,92,93,94,95,97,98,99,100,101,102,103,104,106,107,108,109,110,111,113,114,115,116,117,119,120,121,122,123,125,126,127,128,130,131,132,133,135,136,137,138,140,141,142,143,145,146,147,149,150,151,153,154,155,157,158,159,161,162,163,165,166,167,169,170,171,173,174,176,177,178,180,181,183,184,185,187,188,190,191,193,194,196,197,198,200,201,203,204,206,207,209,210,212,213,215,216,218,219,221,222,224,225,227,228,230,231,233,235,236,238,239,241,242,244,245,247,249,250,252,253,255};
const unsigned char GAMMA1_8[]={0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,2,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,6,6,6,7,7,8,8,8,9,9,10,10,10,11,11,12,12,13,13,14,14,15,15,16,16,17,17,18,18,19,19,20,21,21,22,22,23,24,24,25,26,26,27,28,28,29,30,30,31,32,32,33,34,35,35,36,37,38,38,39,40,41,41,42,43,44,45,46,46,47,48,49,50,51,52,53,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,86,87,88,89,90,91,92,93,95,96,97,98,99,100,102,103,104,105,107,108,109,110,111,113,114,115,116,118,119,120,122,123,124,126,127,128,129,131,132,134,135,136,138,139,140,142,143,145,146,147,149,150,152,153,154,156,157,159,160,162,163,165,166,168,169,171,172,174,175,177,178,180,181,183,184,186,188,189,191,192,194,195,197,199,200,202,204,205,207,208,210,212,213,215,217,218,220,222,224,225,227,229,230,232,234,236,237,239,241,243,244,246,248,250,251,253,255};
const unsigned char GAMMA2_0[]={0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,7,7,7,8,8,8,9,9,9,10,10,11,11,11,12,12,13,13,14,14,15,15,16,16,17,17,18,18,19,19,20,20,21,21,22,23,23,24,24,25,26,26,27,28,28,29,30,30,31,32,32,33,34,35,35,36,37,38,38,39,40,41,42,42,43,44,45,46,47,47,48,49,50,51,52,53,54,55,56,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,73,74,75,76,77,78,79,80,81,82,84,85,86,87,88,89,91,92,93,94,95,97,98,99,100,102,103,104,105,107,108,109,111,112,113,115,116,117,119,120,121,123,124,126,127,128,130,131,133,134,136,137,139,140,142,143,145,146,148,149,151,152,154,155,157,158,160,162,163,165,166,168,170,171,173,175,176,178,180,181,183,185,186,188,190,192,193,195,197,199,200,202,204,206,207,209,211,213,215,217,218,220,222,224,226,228,230,232,233,235,237,239,241,243,245,247,249,251,253,255};
const unsigned char GAMMA4_0[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,7,7,7,7,8,8,8,9,9,9,9,10,10,11,11,11,12,12,13,13,13,14,14,15,15,16,16,17,17,18,18,19,19,20,21,21,22,23,23,24,25,25,26,27,27,28,29,30,31,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,52,53,54,55,57,58,59,61,62,63,65,66,68,69,71,72,74,75,77,79,80,82,84,85,87,89,91,93,95,96,98,100,102,104,107,109,111,113,115,117,120,122,124,126,129,131,134,136,139,141,144,146,149,152,155,157,160,163,166,169,172,175,178,181,184,187,190,194,197,200,203,207,210,214,217,221,224,228,232,236,239,243,247,251,255};



const __int32 GRAD_FILTERX[] = {-1, 0, 1, -2, 0, 2,  -1, 0, 1};
const __int32 GRAD_FILTERY[] = {1, 2, 1, 0, 0, 0,  -1, -2, -1};
const __int32 GRAD_FILTERX1[] = {-1, 0, 1, -4, 0, 4,  -1, 0, 1};
const __int32 GRAD_FILTERY1[] = {1, 4, 1, 0, 0, 0,  -1, -4, -1};
const __int32 GRAD_FILTERX2[] = {0, 0, 0, -4, 0, 4,  0, 0,0};
const __int32 GRAD_FILTERY2[] = {0, 4, 0, 0, 0, 0,  0, -4, 0};


// ---------------------------------------------------------------------------------------------
// FilterWin(HCH/NCH) run filter HCH/NCH to window and return the medium light of transformed window
// FilterWin32(HCH/NCH) run filter HCH/NCH to window and return the medium light of transformed window 
// filter	- pointer to used filter
// img		- source image (unsigned char fir FilterWin and __int32 for filterWin32 )
// outimg	- output img (unsigned char fir FilterWin and __int32 for filterWin32 )
// mwidth	- image width
// mheight	- image height
// wwidth	- 1/2 window width
// wheight	- 1/2 window height
// x,y		- center of window
// ---------------------------------------------------------------------------------------------

__int32 FilterWinHCH(__int32 * filter, unsigned char * img, unsigned char * outimg, int x, int y, int mwidth, int mheght, int wwidth, int wheight);
__int32 FilterWinNCH(__int32 * filter, unsigned char * img, unsigned char * outimg, int x, int y, int mwidth, int mheght, int wwidth, int wheight);
__int32 FilterWin32HCH(__int32 * filter, __int32 * img, __int32 * outimg, int x, int y, int mwidth, int mheght, int wwidth, int wheight);
__int32 FilterWin32NCH(__int32 * filter, __int32 * img, __int32 * outimg, int x, int y, int mwidth, int mheght, int wwidth, int wheight);

__int32 FilterGrad2(unsigned char * img, unsigned char * outimg, unsigned char * tmpimg, int x, int y, int mwidth, int mheght, int wwidth, int wheight);

//-------------------------------------------------------------------------------------------------

unsigned __int32  evCalc4(unsigned char a, unsigned char b, unsigned char c, unsigned char d,  unsigned __int32 &lam1,unsigned __int32 &lam2);
void DiffFrame(int width, int height,  unsigned char * input1,  unsigned char * input2,  unsigned char * output );
__int32 DinDiap(unsigned char * img, unsigned char * outimg, int x, int y, int mwidth, int mheght, int wwidth, int wheight);




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													INTKLT Structures and functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct  tagINT_TrackContext{
	/* Available to user */
//	int mindist;			/* min distance b/w features */
	int window_width, window_height;
	bool sequentialMode;	/* whether to save most recent image to save time */
	/* can set to TRUE manually, but don't set to */
	/* FALSE manually */
//	KLT_BOOL smoothBeforeSelecting;	/* whether to smooth image before */
	/* selecting features */
	/* tracking features */
//	KLT_BOOL lighting_insensitive;  /* whether to normalize for gain and bias (not in original algorithm) */

	/* Available, but hopefully can ignore */
	int min_eigenvalue;		/* smallest eigenvalue allowed for selecting */
	int min_determinant;	/* th for determining lost */
	int min_displacement;	/* th for stopping tracking when pixel changes little */
	int max_iterations;		/* th for stopping tracking when too many iterations */
	int max_residue;		/* th for stopping tracking when residue is large */
//	float grad_sigma;
//	float smooth_sigma_fact;
//	float pyramid_sigma_fact;
	int step_factor;  /* size of Newton steps; 2.0 comes from equations, 1.0 seems to avoid overshooting */
	int nSkippedPixels;		/* # of pixels skipped when finding features */
	int borderx;			/* border in which features will not be found */
	int bordery;
//	int nPyramidLevels;		/* computed from search_ranges */
	int subsampling;		/* 		" */
	
	/* for affine mapping */ 
//	int affine_window_width, affine_window_height;
/*	int affineConsistencyCheck; // whether to evaluates the consistency of features with affine mapping 
								// -1 = don't evaluates the consistency
								//  0 = evaluates the consistency of features with translation mapping
								//  1 = evaluates the consistency of features with similarity mapping
								//  2 = evaluates the consistency of features with affine mapping
								*/
//	int affine_max_iterations;  
//	float affine_max_residue;
//	float affine_min_displacement;        
//	float affine_max_displacement_differ; // th for the difference between the displacement calculated 
										  // by the affine tracker and the frame to frame tracker in pel*/

	/* User must not touch these */
//	void *pyramid_last;
//	void *pyramid_last_gradx;
//	void *pyramid_last_grady;
}  INT_TrackContextRec, *INT_TrackContext;


typedef struct _tagIntImage{
	int cols;
	int rows;
	int rest;
	__int32 * data;
	_tagIntImage(){cols = rows = rest = 0; data =0;};
	bool CreateIntImage(int width, int height, int irest){
		cols = width;
		rows = height;
		rest = irest;
		if (data) free(data);
		data = (__int32 *) malloc(cols*rows*sizeof(__int32));
		if (data){
			memset(data, 0, cols*rows);
			return true;
		}
		return false;
	};
	bool CreateIntImage(int width, int height){
		return CreateIntImage(width, height, 0);
	};
	void FreeIntImage(){
		if (data) delete data;
		rows = cols = 0;
		data =0;
	};

}IntImage, *PINTIMAGE;

typedef struct _tagINT_FeatureRec {
	__int32 x;	// 24.8
	__int32 y;  // 24.8
	int val;	
//	int control;			// control this point
	/* for affine mapping */
//	IntImage aff_img; 
//	IntImage aff_img_gradx;
//	IntImage aff_img_grady;
	__int32 aff_x;
	__int32 aff_y;
// test	__int32 aff_Axx;
// test	__int32 aff_Ayx;
// test	__int32 aff_Axy;
// test	__int32 aff_Ayy;
	void Clear(){
		x =0;
		y =0;
		val =0;
//		control =0;
		aff_x = 0;
		aff_y = 0;
//		aff_img = 0;
//		aff_img_gradx = 0;
//		aff_img_grady = 0;
	};
}  INT_FeatureRec, *INT_Feature;
//----------------------------------------------------------------
typedef struct  tagINT_FeautureList{
	int nFeatures;
	INT_FeatureRec * feature;

	tagINT_FeautureList(){
		nFeatures = 0;
		feature = 0;
	};
	bool CreateList(int cnt){
		nFeatures = cnt;
		if (feature) free(feature);
		feature = (INT_FeatureRec *) malloc(max(cnt,MAXPOINTNUM)*sizeof(INT_FeatureRec));
		if (feature) return true;
		return false;
	};
	void FreeList(){
		if (feature) delete feature;
		feature = 0;
		nFeatures = 0;
	};
	void Clear(){
		for(int i = 0; i <nFeatures; i++) 
			feature[i].Clear();
	}
	tagINT_FeautureList & operator = (const tagINT_FeautureList& fl){
		int k;
		int s = min(nFeatures, fl.nFeatures);
		for (k = 0; k< s; k++){
//			memcpy(&feature[k], &fl.feature[k], sizeof(INT_FeatureRec));
			feature[k].Clear();
			feature[k].x = fl.feature[k].x;
			feature[k].y = fl.feature[k].y;
			feature[k].val = fl.feature[k].val;
		}
		for (k; k< nFeatures; k++){
			feature[k].Clear();
		}
		return *this;

	};
	
} INT_FeatureList, *INT_FeatureListRec;

extern int _iMaxX;
extern int _iMinX;
extern int _iMaxY;
extern int _iMinY;
extern int _iBoundLen;

typedef struct _tagBOUNDGOODPOINT{
	#define MINBOUNDLEN 16
	#define MAXBOUNDLEN 32
	#define DEFTTL	25
	RECT rect; // bounded rect
	__int32 x;		// coords center mass
	__int32 y;
	volatile __int32 radius; // square of max length to center
	__int32 count;	// number of points
	__int32 minrad;
	__int32 ax;		// logical center x
	__int32 ay;		// logical center y
	__int32 vx;		// speed
	__int32 vy;		// speed
	int		val;	// testing value
	int		ttl;	// time to live; count information
	bool	IsSelect;	// check if rectangle was selected
	
	_tagBOUNDGOODPOINT(){
		memset((unsigned char *) &rect, 0, sizeof(RECT));
		x =0;  
		y =0;
		radius = MINBOUNDLEN*MINBOUNDLEN; // max length to center
		minrad = MINBOUNDLEN/2;
		count = 0;
		ax = ay = 0;
		val = 0;
		vx = vy =0;
		IsSelect = false;
		ttl = DEFTTL;
	};
	void SetRadius(int rad){
		if (rad > 0 && rad < MAXBOUNDLEN){
			minrad = rad;
			radius = rad*rad;
		}

	}
	
	_tagBOUNDGOODPOINT& operator = (const _tagBOUNDGOODPOINT& bp)
	{
		x = bp.x;
		y = bp.y;
		ax = bp.ax;
		ay = bp.ay;
		count = bp.count;
		IsSelect = bp.IsSelect;
		minrad = bp.minrad;
		vx = bp.vx;
		vy = bp.vy;
		val = bp.val;
		radius = bp.radius;
		rect = bp.rect;
		ttl = bp.ttl;	//
		return *this;
	};
	// add rect to rect
#if 0 //09-04-2014
	_tagBOUNDGOODPOINT& operator += (const _tagBOUNDGOODPOINT& bp)
	{
		if (!count) {*this = bp;}
		else{
			if (bp.count){
				rect.left = min(rect.left, bp.rect.left);
				rect.right = max(rect.right, bp.rect.right);
				rect.top = min(rect.top, bp.rect.top);
				rect.bottom = max(rect.bottom, bp.rect.bottom);
				val += bp.val;// max(val, bp.val);
				count = count + bp.count;
				x = (x+bp.x)/2;
				y = (y+bp.y)/2;
				IsSelect = max(IsSelect, bp.IsSelect);
				ax = (ax + bp.ax)/2;//max(ax, bp.ax);
				ay = (ay + bp.ay)/2;//max(ay, bp.ay);
				minrad = max(minrad, bp.minrad);
				vx = (vx + bp.vx)/2; //max(vx,bp.vx);
				vy = (vy + bp.vy)/2; //max(vy, bp.vy);
				radius = max(radius, bp.radius);
				ttl = DEFTTL;
			}
		}
		return *this;
	};
#endif
	_tagBOUNDGOODPOINT& operator += (const _tagBOUNDGOODPOINT& bp)
	{
		if (!count) {*this = bp;}
		else{
			if (bp.count){
				if (bp.val > val/2){
					val = (val + bp.val)/2;// max(val, bp.val);
					count = count + bp.count;
					x = (x+bp.x)/2;
					y = (y+bp.y)/2;
					IsSelect = max(IsSelect, bp.IsSelect);
					ax = (ax + bp.ax)/2;//max(ax, bp.ax);
					ay = (ay + bp.ay)/2;//max(ay, bp.ay);
					minrad = max(minrad, bp.minrad);
					vx = (vx + bp.vx)/2; //max(vx,bp.vx);
					vy = (vy + bp.vy)/2; //max(vy, bp.vy);
					radius = max(radius, bp.radius);
					
					rect.left = min(rect.left, bp.rect.left);
					rect.right = max(rect.right, bp.rect.right);
					rect.top = min(rect.top, bp.rect.top);
					rect.bottom = max(rect.bottom, bp.rect.bottom);

			rect.left = max(x-minrad, _iMinX);
			rect.right = min(x+minrad, _iMaxX); // max right = 800 !!! for our solution only
			rect.top = max(_iMinY, y-minrad);
			rect.bottom = min(y+minrad, _iMaxY); // max bottom = 600 !!! for our solution only



				}
				ttl = DEFTTL;
			}
		}
		return *this;
	};
#if 0
	inline void MovePlus(int px, int py, int nrows, int ncols){
		x = min(x+px, ncols - MINBOUNDLEN);
		y = min (y+py, nrows - MINBOUNDLEN);
		rect.left = min(ncols, rect.left + px);
		rect.right = min(ncols, rect.right + px);
		rect.top = min(nrows, rect.top + py);
		rect.bottom = min(ncols, rect.bottom + py);
	};
	inline void MovePlus(POINT p, int nrows, int ncols){
		MovePlus(p.x, p.y, nrows, ncols);
	};
	inline void MoveMinus(int px, int py){
		x = max(x- px, 0);
		y = max (y - py, 0);
		rect.left = max(0, rect.left - px);
		rect.right = max(0, rect.right - px);
		rect.top = max(0, rect.top - py);
		rect.bottom = max(0, rect.bottom - py);
	};
	inline void MovePlus(POINT p){
		MoveMinus(p.x, p.y);
	};
#endif
	inline void Move(int px, int py, int nrows, int ncols){
		x = max(_iMinX,min(x+px, _iMaxX ));
		y = max(_iMinY, min(y+py, _iMaxY));
		rect.left = max(_iMinX, min(_iMaxX, rect.left + px));
		rect.right = max(_iMinX,min(_iMaxX, rect.right + px));
		rect.top = max(_iMinY,min(_iMaxY, rect.top + py));
		rect.bottom = max(_iMinY,min(_iMaxY, rect.bottom + py));
	};
	inline void Move(POINT p, int nrows, int ncols){
		Move(p.x, p.y, nrows, ncols);
	};


	inline void Extend(int dist, int ncols, int nrows)	{//extend of distance
		if (count){
//			rect.left =max(MINBOUNDLEN, rect.left - dist);
//			rect.right = min(rect.right+dist, ncols);
//			::InterlockedExchange(&rect.top, max(MINBOUNDLEN, rect.top-dist));
//			::InterlockedExchange(&rect.bottom, min(rect.bottom+dist, nrows));
//			radius = (rect.right - rect.left)*(rect.bottom - rect.top)/4;

			::InterlockedExchange(&rect.left,max(_iMinX, rect.left - dist));
			::InterlockedExchange(&rect.right, min(rect.right+dist, _iMaxX));
			::InterlockedExchange(&rect.top, max(_iMinY, rect.top-dist));
			::InterlockedExchange(&rect.bottom, min(rect.bottom+dist, _iMaxY));
			radius = (rect.right - rect.left)*(rect.bottom - rect.top)/4;

		}
	};
	int GetLen(int px, int py){//return length from center
		if (count == 0){
			return 0x7FFFFFFF; // no length
		}
		return( (x-px)*(x-px) + (y-py)*(y-py));
	}
	int GetMaxDist(int px, int py){
		if (!count) return -1; //?
		if (IsPointToRect(px,py)) return 0;
		return max(max(px -x, x -px), max(py-y, y-py));

	}
	int GetMaxDist(POINT p){
		return GetMaxDist(p.x, p.y);
	};


	inline bool IsPointToRect(int px, int py){ // checks is point to bounded rect
		if (!count) return false; //?
		if ((px >= rect.left)&&(px <=rect.right)&&( py >=rect.top)&&(py <=rect.bottom)) 
			return true;
		return false;
	}
	inline bool IsPointToRect(POINT p){ 
		return IsPointToRect(p.x, p.y);
	};
	inline bool IsPointDistance(int px, int py, int dist){
		if (IsPointToRect(px,py)) 
			return true;
		if ((px > x - dist-5)&&(px< x + dist+5)&&(py > y-dist-5)&&(py< y + dist+5))
			return true;
		return false;
	};
	inline bool IsPointDistance(POINT p, int dist){
		return IsPointToRect(p.x,p.y);
	}
	inline bool CheckAndUpdate(int px, int py, int pval){
		if (count == 0){ // add point and update val
			CheckPoint(px, py);
			val = pval;
			return true;
		}
		// rect not empty
		if ((abs(x - px) <= minrad)&&(abs(y-py)<= minrad)){
	//		if (pval < val/2){
	//			return true;
	//		}
			int oval = (val+pval)/2; 
			val = val +pval;
			//minrad = minrad + max(abs(px-x), abs(py-y))*pval/val;
			minrad = max(minrad, max(abs(px-x), abs(py-y)));
			if (minrad > MAXBOUNDLEN)  minrad = MAXBOUNDLEN;

			val = oval;
			// point into rect
			x = (x+px)/2;
			//x = (x*oval+px*pval)/val;
			y = (y+py)/2;
			//y = (y*oval + py*pval)/val;
			rect.left = max(x-minrad, _iMinX);
			rect.right = min(x+minrad, _iMaxX); // max right = 800 !!! for our solution only
			rect.top = max(_iMinY, y-minrad);
			rect.bottom = min(y+minrad, _iMaxY); // max bottom = 600 !!! for our solution only
			count++;
			return true;
		}
		// check that near
		return false;
	};
	inline bool CheckAndUpdate(POINT p, int pval){
		return CheckAndUpdate(p.x,p.y, pval);
	};
	 //if point to bounded add point to rect
	bool CheckPoint(int px, int py){
		if (count == 0){ //if no points add points to line
			x = px;
			y = py;
	//		radius = 25;  // 5 pixels bound step
			rect.left = max(x-minrad, _iMinX);
			rect.right = min(x+minrad, _iMaxX); // max right = 800 !!! for our solution only
			rect.top = max(_iMinY, y-minrad);
			rect.bottom = min(y+minrad, _iMaxY); // max bottom = 600 !!! for our solution only
			count++;
			return true;
		}// end count==0
		if (px > rect.left && px < rect.right && py > rect.top && py < rect.bottom){
			x = (x*count + px)/(count+1);
			y = (y*count+ py)/(count+1);
			rect.left = max(x-minrad, _iMinX);
			rect.right = min(x+minrad, _iMaxX); // max right = 800 !!! for our solution only
			rect.top = max(_iMinY, y-minrad);
			rect.bottom = min(y+minrad, _iMaxY); // max bottom = 600 !!! for our solution only
			count++;
//			int rad = (rect.right - rect.left)/2;
	//		radius = (rect.right - rect.left)*(rect.bottom - rect.top)/4;
//			rect.left = x -rad; rect.right = x + rad;
//			rad = (rect.bottom - rect.top)/2;
//			rect.top = y - rad;
//			rect.bottom = y + rad;

			return true;
		}
		if (((px-x)*(px-x) < radius)&&( (py-y)*(py-y) < radius)){
			int rad = max(max(px - x, x-px), max(y-py, py-y) ); 
			if (rad > MAXBOUNDLEN) 
				return false;
			x = (x+px)/2;//(x*count + px)/(count+1);
			y = (y+py)/2;//(y*count+ py)/(count+1);
			count++;
//			rect.left = min(rect.left, px-5);
//			rect.right = max(rect.right, px+5);
//			rect.top = min(rect.top, py-5);
//			rect.bottom = max(rect.bottom, py+5);
			rect.left = max(x-minrad, _iMinX);
			rect.right = min(x+minrad, _iMaxX); // max right = 800 !!! for our solution only
			rect.top = max(_iMinY, y-minrad);
			rect.bottom = min(y+minrad, _iMaxY); // max bottom = 600 !!! for our solution only
			rad = max(x - rect.left, y-rect.top);
			rad = max(rad, rect.right - x);
			rad = max(rad, rect.bottom -y);
			radius = max(radius, rad*rad);
			return true;
		}
		return false;
	};
	bool CheckPoint(POINT p){
		return CheckPoint (p.x, p.y);
	};

	void SetDisplasment(__int32 dx, __int32 dy){
		ax = dx;
		ay = dy;
	}

	// clear rect
	void Clear(){
		memset((unsigned char *) &rect, 0, sizeof(RECT));
		x =0;  
		y =0;
		radius = MINBOUNDLEN*MINBOUNDLEN; // max length to center
		minrad = MINBOUNDLEN/2;
		count = 0;
		ax = ay = 0;
		val = 0;
		vx = vy =0;
		IsSelect = false;
		ttl = DEFTTL;

	};

}BOUNDGOODPOINT, *PBOUNDGOODPOINT;

typedef struct _tagARRGOODRECT{
	int num;	// number of rect;
	BOUNDGOODPOINT * br; // pointer to array of founded rect
	_tagARRGOODRECT(){
		num =0;
		br = 0;
	};
	~_tagARRGOODRECT(){
		if (br) free(br);
		br = 0;
		num = 0;
	};
	bool Initilase(int n){
		if (br) {
			if (n > num)
				return false; 
			if (n <= num) 
				num = n;
			return true;
		}
		br = (BOUNDGOODPOINT *) malloc(sizeof(BOUNDGOODPOINT)*n);
		if (!br) return false;
		num = n;
		for (int i = 0; i<num; i++) 
			br[i].Clear();
		return true;
	};
	void Reliase(){
		if (br) free(br);
		br = 0;
		num = 0;
	};
	inline bool AddPointUpdate(int x, int y, int val){
		int  k =0;
		int len1, len = MAXBOUNDLEN;// 0x7fffffff;
		for (int i=0; i<num; i++){
			if (br[i].count == 0) {
				if (k==0) k = i;
				break;
			}
			len1 = br[i].GetMaxDist(x,y);
			if (len1 == 0){
				k =i;
				break;
			}
			if (len1 < len){
				k =i; len = len1;
			}
		}
		for (int i = k; i< num; i++){
			if (br[i].CheckAndUpdate(x,y, val)){
				return true;
			}
		}
		return false;
	}
	bool AddPoint(int x, int y){
		if (x*y ==0) return false;
		int k = 0;
		int len1, len = MAXBOUNDLEN;// 0x7fffffff;
		for (int i=0; i<num; i++){
			if (br[i].count == 0)
				break;
			len1 = br[i].GetLen(x,y);
			if (len1 < len){
				 k = i;
				 len = len1;
			}
		}
		for (int i = k; i<num; i++){
			if (br[i].CheckPoint(x,y)){
				Normalise();
				return true;
			}
				
		}
		return false;
	};
	int IsPointInto(int x, int y){
		for (int i = 0; i<num; i++){
			if (br[i].IsPointToRect(x,y))
				return i;
		}
		return -1;
	};
	int IsPointInto(POINT p){
		return IsPointInto(p.x, p.y);
	};
	
	bool AddPoint(POINT p){
		return AddPoint(p.x, p.y);
	};
	void SetRadius(int rad){
		for (int i = 0; i<num; i++) br[i].SetRadius(rad);
	}
	void Clear(){
		for (int i = 0; i<num; i++) br[i].Clear();
	};

	void Normalise(){
//		int dx,dy,radx,rady;
		int k = 0;
#if 0
		for (int i = 1; i< num; i++){
			if (!br[i].count) continue; // skip not active element
			if (br[i-1].IsPointToRect(br[i].x, br[i].y)){// cenetr into rect
				br[i-1] += br[i]; //
				br[i].Clear();
				continue;
			}
			// if rect one in other combin rect
			if ( (br[i].rect.left > br[i-1].rect.left) && (br[i].rect.right < br[i-i].rect.right) 
				&& ( br[i].rect.top > br[i-1].rect.top) && (br[i].rect.bottom < br[i-1].rect.bottom) ) {
				br[i-1] += br[i]; //
				br[i].Clear();
				continue;
			}
			if (br[i-1].IsPointDistance(br[i].x, br[i].y, MAXBOUNDLEN)){
				br[i-1] += br[i]; //
				br[i].Clear();
				continue;
			}
		}//end main loop
#endif
#if 0 //08-04-2014
		k = 0;
		for (int i = 1; i< num; i++){
			if (!br[i].count) continue; // skip not active element
			if (br[k].count == 0){
				br[k] = br[i];
//				k++;
				br[i].Clear();
				continue;
			}
			if (br[k].IsPointToRect(br[i].x, br[i].y)){// cenetr into rect
				br[k] += br[i]; //
				br[i].Clear();
				k++;
				continue;
			}
			// if rect one in other combin rect
			if ( (br[i].rect.left > br[k].rect.left) && (br[i].rect.right < br[k].rect.right) 
				&& ( br[i].rect.top > br[k].rect.top) && (br[i].rect.bottom < br[k].rect.bottom) ) {
				br[k] += br[i]; //
				br[i].Clear();
				k++;
				continue;
			}
			if (br[k].IsPointDistance(br[i].x, br[i].y, MAXBOUNDLEN)){
				br[k] += br[i]; //
				br[i].Clear();
				k++;
				continue;
			}
		}//end main loop
#endif
		// 08-04-2014
		int i =0;
		for (k=0; k<num-1; k++){
			if (br[k].count == 0){
				for(i=k+1; i<num; i++){
					if (br[i].count ){
						br[k] = br[i];
						br[i].Clear();
						break;
					}
				}
				if(i==num) break;
			}
			for(i=k+1; i<num; i++){
				if (br[i].count == 0) continue;
				if (br[k].IsPointToRect(br[i].x, br[i].y)){// cenetr into rect
					br[k] += br[i]; //
					br[i].Clear();
				//	k++;
					continue;
				}
				if ( br[i].IsPointToRect(br[k].x,br[k].y) ) {
						br[k] += br[i]; //
						br[i].Clear();
					//	k++;
						continue;
				}
				if (br[k].IsPointDistance(br[i].x, br[i].y, MAXBOUNDLEN/2)){
					br[i] += br[k]; //
					br[k] = br[i];
					br[i].Clear();
				//	k++;
					continue;
				}
			}//end enetering loop

		}
#if 0
		for (int i = num-1; i>0; i--){
			if (!br[i].count) continue; // skip not active element
			if (!br[i-1].count){
				br[i-1] = br[i];
				br[i].Clear();
				continue;
			}
			if (br[i-1].IsPointToRect(br[i].x, br[i].y)){// cenetr into rect
				br[i-1] += br[i]; //
				br[i].Clear();
				continue;
			}
			// if rect one in other combin rect
			if ( (br[i].rect.left > br[i-1].rect.left) && (br[i].rect.right < br[i-i].rect.right) 
				&& ( br[i].rect.top > br[i-1].rect.top) && (br[i].rect.bottom < br[i-1].rect.bottom) ) {
				br[i-1] += br[i]; //
				br[i].Clear();
				continue;
			}
			if (br[i-1].IsPointDistance(br[i].x, br[i].y, MAXBOUNDLEN)){
				br[i-1] += br[i]; //
				br[i].Clear();
				continue;
			}
		}//end main loop
#endif
	}
#if 0
	void Normalise(){
		int dx,dy, radx, rady;
		for (int i = num-1; i > 0; i--){
			if (!br[i].count) continue;
			// if rect one in other combin rect
			if ( (br[i].rect.left > br[i-1].rect.left) && (br[i].rect.right < br[i-i].rect.right) 
				&& ( br[i].rect.top > br[i-1].rect.top) && (br[i].rect.bottom < br[i-1].rect.bottom) ) {
					// br[i] into br[i-1]
				br[i-1].x = (br[i-1].count *br[i-1].x + br[i].count*br[i].x)/(br[i].count + br[i-1].count); //new center x
				br[i-1].y = (br[i-1].count *br[i-1].y + br[i].count*br[i].y)/(br[i].count + br[i-1].count); // new center y 
				br[i-1].count = br[i].count + br[i-1].count;												// new point count
				br[i].Clear();
				continue;
			}
				
			if ( (br[i-1].rect.left > br[i].rect.left) && (br[i-1].rect.right < br[i].rect.right) 
				&& ( br[i-1].rect.top > br[i].rect.top) && (br[i-1].rect.bottom < br[i].rect.bottom) ) {
					// br[i-1] into br[i]
				br[i-1].x = (br[i-1].count *br[i-1].x + br[i].count*br[i].x)/(br[i].count + br[i-1].count); //new center x
				br[i-1].y = (br[i-1].count *br[i-1].y + br[i].count*br[i].y)/(br[i].count + br[i-1].count); // new center y 
				br[i-1].count = br[i].count + br[i-1].count;												// new point count
				br[i-1].rect.left = br[i].rect.left;
				br[i-1].rect.right = br[i].rect.right;
				br[i-1].rect.top = br[i].rect.top;
				br[i-1].rect.bottom = br[i].rect.bottom;
				br[i].Clear();
				continue;
			}



			radx = max(br[i].x - br[i-1].x, br[i-1].x - br[i].x);
			rady = max(br[i].y - br[i-1].y, br[i-1].y - br[i].y); 
			if ( (radx > MAXBOUNDLEN)||(rady > MAXBOUNDLEN))
				continue;
			dx = br[i-1].x - br[i].x;
			dy = br[i-1].y - br[i].y;

			if (( dx < (br[i-1].rect.right - br[i].rect.left)/2)&& ( dy < (br[i-1].rect.bottom - br[i].rect.top)/2)){
				br[i-1].x = (br[i-1].count *br[i-1].x + br[i].count*br[i].x)/(br[i].count + br[i-1].count); //new center x
				br[i-1].y = (br[i-1].count *br[i-1].y + br[i].count*br[i].y)/(br[i].count + br[i-1].count); // new center y 
				br[i-1].count = br[i].count + br[i-1].count;												// new point count
				br[i-1].rect.left = min(br[i-1].rect.left, br[i].rect.left);
				br[i-1].rect.right = max(br[i-1].rect.right, br[i].rect.right);
				br[i-1].rect.top = min(br[i-1].rect.top, br[i].rect.top);
				br[i-1].rect.bottom = max(br[i-1].rect.bottom, br[i].rect.bottom);
				br[i].Clear();
			}
		}
	}
#endif

}ARRGOODRECT, *PARRGOODRECT;

typedef struct _INTIMGSTRUCT{
	unsigned __int32 iMask;			// initialization mask
	int ncols;						//size
	int nrows;
	int cntimg;						// number of images
	int * pointlist;
	unsigned char * featuremap;		// 
	unsigned char * img;			// processed images
	unsigned char * img2;			// processed second images 
//	unsigned char * img3;			// working  image 
	INT_TrackContextRec tc;
	INT_FeatureList fl;				// feature list
	IntImage gradx;			// temporary image
	IntImage grady;			// temporary image
	IntImage floatimg;		// temporary image
	IntImage tmpimg;			// temporary image
	IntImage cstmpimg;		// temporary image for convolution kernel
	ARRGOODRECT gRect;

// functions
	_INTIMGSTRUCT(){
		iMask = 0;
		ncols = nrows = cntimg = 0;
		pointlist = 0;
		featuremap = img = img2  /*= img3*/ = 0;
		tc.bordery = tc.borderx = 10;
		tc.max_iterations = 10;
		tc.min_determinant = 1;
		tc.min_eigenvalue = 1;
		tc.min_displacement = 2;
//		tc.mindist = 15;
		tc.nSkippedPixels = 3;
		tc.sequentialMode = SELECTING_ALL;
		tc.window_width = tc.window_height = 8;
		
	}

	void Release(){
		if (iMask && 0x80000000){
			if(img){ free(img); img = 0; iMask &= 0x7FFFFFFF;}
		}
		if (iMask && 0x40000000){
			if(img2) {free(img2); img2 = 0; iMask &= 0xBFFFFFFF;}
		}
//		if (iMask && 0x20000000){
//			if (img3) {free(img3); img3 = 0; iMask &= 0xDFFFFFFF;}
//		}
		if (iMask && 0x10000000){
			if (pointlist){ free(pointlist); pointlist = 0; iMask &= 0xEFFFFFFF;}
		}
		if (iMask && 0x08000000){
			if (featuremap) { free(featuremap); featuremap = 0; iMask &= 0xF7FFFFFF;}
		}
		fl.FreeList(); iMask &= 0xFBFFFFFF;				// feature list
		gradx.FreeIntImage(); iMask &= 0xFDFFFFFF; 			// temporary image
		grady.FreeIntImage(); iMask &= 0xFEFFFFFF; 			// temporary image
		floatimg.FreeIntImage(); iMask &= 0xFF7FFFFF;
		tmpimg.FreeIntImage();   iMask &= 0xFFBFFFFF;			// temporary image
		cstmpimg.FreeIntImage(); iMask &= 0xFFDFFFFF;		// temporary image for convolution kernel
		gRect.Reliase(); iMask &= 0xFFEFFFFF;

		iMask = 0;
	};
	bool Initilise(int width, int height, int num){
		ncols = width;						//size
		nrows = height;

		tc.borderx = tc.bordery = 24;
		tc.max_iterations = 10;
		tc.min_determinant = 1;
		tc.min_eigenvalue = 32;
		tc.nSkippedPixels = 3;
		tc.sequentialMode = SELECTING_ALL;
		//tc.window_width = tc.window_height = 6;
		

		img = (unsigned char *) malloc((ncols+3)/4*nrows*4);			// processed images
		if (img) iMask |= 0x80000000;
		img2 = (unsigned char *)  malloc((ncols+3)/4*nrows*4);			// processed images
		if (img2) iMask |= 0x40000000;
//		img3= (unsigned char *) malloc((ncols+3)/4*nrows*4);			// processed images
//		if (img) iMask |= 0x20000000;
		pointlist = (int *) malloc(3*sizeof(int)*ncols*nrows);
		if (pointlist) iMask |= 0x10000000;
		featuremap = (unsigned char *) malloc(ncols * nrows * sizeof(unsigned char));
		if (featuremap) iMask |= 0x08000000;
		if (fl.CreateList(num)) iMask |= 0x04000000;				// feature list
		if (gradx.CreateIntImage(ncols,nrows)) iMask |= 0x02000000; 			// temporary image
		if (grady.CreateIntImage(ncols, nrows)) iMask |= 0x01000000; 			// temporary image
		if (floatimg.CreateIntImage(ncols,nrows)) iMask |= 0x00800000;
		if (tmpimg.CreateIntImage(ncols,nrows)) iMask |= 0x00400000;			// temporary image
		if (cstmpimg.CreateIntImage(ncols,nrows)) iMask |= 0x00200000;		// temporary image for convolution kernel
		if (gRect.Initilase(10) ) { 
			/*gRect.SetRadius(tc.borderx/2);*/ 
			iMask |= 0x00100000;
		}
		if (iMask == 0xDFF00000){
			return true;
		}
		Release();
		return false;
	};
#if 0
	// num - number featurelist record
	bool InitFromBitmap(int num, BITMAPINFOHEADER &bih, unsigned char * bmp){
		if (Initilise(bih.biWidth, bih.biHeight, num)){
			int bpp = bih.biBitCount;
			switch(bpp){
				case 32:
					bpp = 4; break;
				case 24:
					bpp = 3; break;
				case 16:
					bpp = 2; break;
				case 8:
					bpp = 1; break;
				default:
					bpp = 0; break;
			}
			bpp = 4;
			if (bpp != 0){
				unsigned char * ptr, * ptr1; 
				int val;
				for(int i = 0; i< bih.biHeight; i++){
					ptr1 = img+i*((bih.biWidth+3)>>2)*4;
					memset(ptr1,0xff,((bih.biWidth+3)>>2)*4);
					for(int j = 0; j<bih.biWidth; j++){
						ptr = bmp+ (i*bih.biWidth+j)*bpp;
						val =0;
						for (int k =0; k< bpp; k++){ if (k < 3)	val += ptr[k];	}
						if (bpp == 4) ptr1[j] = val/3; 
						else	ptr1[j] = val/bpp;					
					}
				}
#if 0
				for (int i = 0; i < bih.biSizeImage; i++){ // load and convert first images to struct
					ptr = bmp  +i*bpp; val =0;
					for (int k =0; k< bpp; k++){ if (k < 3)	val += ptr[k];	}
					if (bpp == 4){	img[i] = val/3;		}
					else	img[i] = val/bpp;
				}//end
#endif			
				ncols = ((bih.biWidth+3)>>2)*4;
				return 	true;
			}

		}
		Release();
		return false;
	}

	bool LoadBitmap(int nimg, int num, BITMAPINFOHEADER bi, unsigned char * bmp){
		if (iMask != 0xFFF00000){
			if (nimg == 0) 
				return InitFromBitmap(num, bi, bmp);
		}
		if ((ncols < bi.biWidth) || (nrows != bi.biHeight))
			return false;
		unsigned char * iptr = img;
		switch (nimg){
			case 1:
				iptr = img2;
				break;
			case 2:
				iptr = img3;
				break;
			default:
				break;
		}
		int bpp = bi.biBitCount;
		switch(bpp){
				case 32:
					bpp = 4; break;
				case 24:
					bpp = 3; break;
				case 16:
					bpp = 2; break;
				case 8:
					bpp = 1; break;
				default:
					bpp = 0; break;
		}
		bpp = 4;
		if (bpp != 0){
				unsigned char * ptr, * ptr1; 
				int val;
				for(int i = 0; i< bi.biHeight; i++){
					ptr1 = img+i*((bi.biWidth+3)>>2)*4;
					memset(ptr1,0xff,((bi.biWidth+3)>>2)*4);
					for(int j = 0; j<bi.biWidth; j++){
						ptr = bmp+ (i*bi.biWidth+j)*bpp;
						val =0;
						for (int k =0; k< bpp; k++){ if (k < 3)	val += ptr[k];	}
						if (bpp == 4) ptr1[j] = val/3; 
						else	ptr1[j] = val/bpp;					
					}
				}
#if 0
				for (int i = 0; i < bih.biSizeImage; i++){ // load and convert first images to struct
					ptr = bmp  +i*bpp; val =0;
					for (int k =0; k< bpp; k++){ if (k < 3)	val += ptr[k];	}
					if (bpp == 4){	img[i] = val/3;		}
					else	img[i] = val/bpp;
				}//end
#endif			
				ncols = ((bi.biWidth+3)>>2)*4;
				return 	true;
			}
		return false;
	}
	#endif
}INTIMGSTRUCT, *PINTIMGSTRUCT;

#if 0
typedef struct _GISTOGRAMM{
#define GESTSIZE 64
	unsigned __int32 distr[GESTSIZE];
	__int32 maxpos;	// maximum position;
	__int32 mindif;	// minimum difference;
	__int32 maxdif; // maximum difference;
	__int32 minpos; // maximum difference;
	unsigned __int32 count;	// count pixels
	__int32 mto;
	_GISTOGRAMM(){
		memset(this, 0, sizeof(_GISTOGRAMM));
	};

	inline void addvalue(unsigned char data){
		distr[(data>>2)]++;
		count++;
	};
#if 0
	__int32 getmax(){
		if (!count) return 0;
		for (int i =0; i < 64; i ++){
			if (distr[i] > distr[maxpos]) {
				mindif = distr[i] - distr[maxpos];
				maxpos = i;
			}
		}
		return distr[maxpos];
	};
	__int32 getmin(){ // assumes that maxpos was worked
		if (!count) return 0;
		for (int i =0; i < 64; i ++){
			if (distr[i] < distr[minpos]) {
				maxdif = distr[maxpos] - distr[i];
				minpos = i;
			}
		}
		return distr[maxpos];
	};
#endif
	inline void clear(){
		memset(this, 0, sizeof(_GISTOGRAMM));
	};
	void CalcResults(){
		if (!count) {
			clear();
			return;
		}
		mto = 0;
		int level = 0;
		for (int i=0; i< GESTSIZE; i++){
			if (distr[i] < distr[minpos]) minpos = i;
			if (distr[i] > distr[maxpos]){
				mindif = distr[i] - distr[maxpos];
				maxpos = i;
			}
		}
		maxdif = distr[maxpos] - distr[minpos];
		level = distr[maxpos]/8; // 0.25
		for (int i =0; i < GESTSIZE; i++){
			if (distr[i] > level) mto++;
		}
		if (mindif == 0){
			int secmax = maxpos -1;
			if (secmax <0) {
				secmax = maxpos + 1;
			}
			for (int i = 0; i < GESTSIZE; i++){
				if (distr[i] == distr[maxpos]) continue;
				if (distr[i] > distr[secmax])
					secmax = i;
			}
			mindif = distr[maxpos] - distr[secmax];

		}

	} ;

}GISTOGRAMM, *PGISTOGRAMM;

#endif

__int32 GradientCreate(__int32 * filter, int width, int height, unsigned char * img, __int32 * out32); // filter one of 
__int32 GradientCreateGauss(__int32 * filter, int width, int height, unsigned char * img, __int32 * out32); // filter one of 

void IntSelGoodPoints( INTIMGSTRUCT &imps, unsigned char * img );
void IntTrackPoints(INTIMGSTRUCT &imps);
void IntWindowSelGoodPoints ( 
				   unsigned char * img,					// source bw 8-bits image
				   int ncols, int nrows,				// sizeof image
				   __int32 * gradx, __int32 * grady,	// temp matrixes for gradient must be ncols*nrows size ????
				   unsigned char * featuremap,			// temp bufer for sorting point ncols*nrows
				   __int32 * pointlist,					// temp buffer for value must be 3*nrows*ncols
				   INT_TrackContextRec & tc,			// tracert contect
				   INT_FeatureList	& fl,				// feature list
				   int px, int py,						// center of window
				   int width, int height );				// 1/2 window size
void IntRgnSelGoodPoints( INTIMGSTRUCT &imps, ARRGOODRECT &arr, unsigned char * img );
void IntRgnSelGoodPoints1( INTIMGSTRUCT &imps, ARRGOODRECT &arr, unsigned char * img );

void CountLogicalCenter(INT_FeatureList & fl, POINT & lcp);// count logical center from feature list
void CountLogicalCenter(int *pointlist, POINT & lcp, int cnt);
void Contrast(unsigned char * input, unsigned char * output, int rows, int cols, int level);
//void CountLogicalCenter(int *pointlist, POINT & lcp, int cnt);

void CountDiffImg( INTIMGSTRUCT &imps, INT_FeatureList & cl, unsigned char * img1, unsigned char * img2);

__int32 LaplasPoint(int x, int y, unsigned char * img, int wx, int wy);
__int32 LaplasRect(BOUNDGOODPOINT &lr, unsigned char * img, int wx, int wy);

__int32 DivRect(BOUNDGOODPOINT &lr, unsigned char * img, int wx, int wy);


void Gauss(unsigned char * matr, unsigned char * out , int nrows, int ncols);


int TestLocalPoints(INTIMGSTRUCT &imps, BOUNDGOODPOINT &pold,BOUNDGOODPOINT &pnew,unsigned char * imgnew, unsigned char * imgold);

int getBoundsRects(ARRGOODRECT &arr, INT_FeatureList &list, int porog =16);
///////////////////////////////////////////////////// End INTKLT structures and functions //////////////////////////////
#endif


