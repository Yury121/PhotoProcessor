// filters files
#include "stdafx.h"
#include <windows.h>

#include "vklt.h"
#include <crtdbg.h>

#define MAXINT64 0xFFFFFFFE00000001
#define MAXINT32 0x00000000FFFFFFFF

//#define max(a, b)  (((a) > (b)) ? (a) : (b)) 
//#define min(a, b)  (((a) < (b)) ? (a) : (b)) 
int _iMaxX = 700;
int _iMinX = 20;
int _iMaxY = 556;
int _iMinY = 20;
int _iBoundLen = MAXBOUNDLEN;



unsigned __int64 intsqrt(unsigned __int64 val){
	if (val <= 1) return val;
	if (val > MAXINT64) return 0xFFFFFFFFFFFFFFFF;
	unsigned __int64 x1 = val;
	unsigned __int64 x2 = 0;
	unsigned __int64 var = 0;

	if (val > MAXINT32){
		x2 = MAXINT32;
	}
	else 
		x2 = (val>>1)+1;
	x1 = 1;//x2>>3;
	var = x2;
	while ((x2 - x1)> 1){
		var = ((x2 + x1)/2);
		if (var*var > val) 
			x2 = var;
		else 
			x1 = var;
	}
	if ((val - x1*x1) > (x2*x2 -val)) 	var = x2;
	else var = x1;
	return var;
}


void DiffFrame(int width, int height,  unsigned char * input1,  unsigned char * input2,  unsigned char * output ){
	int i;
	int len = width*height;
	len = len >>2;
	unsigned __int32 * Buf1 = (unsigned __int32 *) input1;
	unsigned __int32 * Buf2 = (unsigned __int32 *) input2;
	unsigned __int32 * outBuf = (unsigned __int32 *) output;
	for (i = 0; i <len; i++){
		outBuf[i] = Buf1[i]^Buf2[i];
	}


}

// ---------------------------------------------------------------------------------------------
// FilterWinNCH run filter NCH to window and return the medium light of transformed window
// filter	- pointer to used filter
// img		- source image
// outimg	- output img
// mwidth	- image width
// mheight	- image height
// wwidth	- 1/2 window width
// wheight	- 1/2 window height
// x,y		- center of window
// ---------------------------------------------------------------------------------------------
__int32 FilterWinNCH(__int32 * filter, unsigned char * img, unsigned char * outimg, int x, int y, int mwidth, int mheght, int wwidth, int wheight){
	__int32 Light = 0;
	__int32 tmp = 0;
	int last = mwidth*mheght -2;
	int i,j,k, s,m;
	int minx = max(1, x - wwidth);
	int miny = max(1, x- wheight);
	int maxx = min(wwidth-1, x+wwidth);
	int maxy = min(wheight-1, y+wheight);

	for (i = miny; i < maxy; i++){
		k = i*mwidth;
		s = k - mwidth;		// top
		m = k + mwidth;		// bottom
		for (j = minx; j<maxx; j++){
			if (m >= last){
				break;
			}
			if (img[k] >128){
				outimg[k] = 0;
			}
			tmp = filter[0]*img[s-1]	+ filter[3]*img[k-1] + filter[6]*img[m-1];
			tmp += filter[1]*img[s]+ filter[4]*img[k] + filter[7]*img[m];
			tmp += filter[2]*img[s+1]+ filter[5]*img[k+1] + filter[8]*img[m+1];
/*			tmp = filter[0]*(img[s-1]^0xFF)	+ filter[3]*(img[k-1]^0xFF) + filter[6]*(img[m-1]^0xFF);
			tmp = filter[1]*(img[s]^0xFF)+ filter[4]*(img[k]^0xFF) + filter[7]*(img[m]^0xFF);
			tmp = filter[2]*(img[s+1]^0xFF)+ filter[5]*(img[k+1]^0xFF) + filter[8]*(img[m+1]^0xFF);*/
			tmp *= filter[9];
			tmp /= filter[10];
			outimg[k] = (unsigned char) tmp ;
			s++; m++;k++;
			Light += tmp;
		}//end point loop
	}
	m = (maxx-minx)*(maxy-miny);
	if (m) 
		Light = Light/m;
	return Light;
}


// ---------------------------------------------------------------------------------------------
// FilterWinHCH run filter NCH to window and return the medium light of transformed window
// filter	- pointer to used filter
// img		- source image
// outimg	- output img
// mwidth	- image width
// mheight	- image height
// wwidth	- 1/2 window width
// wheight	- 1/2 window height
// x,y		- center of window
// ---------------------------------------------------------------------------------------------

__int32 FilterWinHCH(__int32 * filter, unsigned char * img, unsigned char * outimg, int x, int y, int mwidth, int mheght, int wwidth, int wheight){
	__int32 Light = 0;
	__int32 tmp = 0;
	int i,j,k, s,m;
	int minx = max(1, x - wwidth);
	int miny = max(1, y- wheight);
	int maxx = min(mwidth-1, x+wwidth);
	int maxy = min(mheght-1, y+wheight);

	for (i = miny; i < maxy; i++){
		k = i*mwidth + minx;
		s  = k -mwidth;// + minx;		// top
		m = k + mwidth;// + minx;		// bottom
		for (j = minx; j<maxx; j++){
			tmp = filter[0]*img[s-1]	+ filter[3]*img[k-1] + filter[6]*img[m-1];
			tmp += filter[1]*img[s]+ filter[4]*img[k] + filter[7]*img[m];
			tmp += filter[2]*img[s+1]+ filter[5]*img[k+1] + filter[8]*img[m+1];
			outimg[k] = (unsigned char) (tmp+255)/2;
	//		if ((tmp >256)||(tmp <-255)){
	//			tmp = tmp;
	//		}
			k++; s++; m++;
			Light += tmp;
		}//end point loop
	}
	m = (maxx-minx)*(maxy-miny);
	if (m) 
		Light = Light/m;
	return Light;
}

// ---------------------------------------------------------------------------------------------
// FilterWin32NCH run filter NCH to window and return the medium light of transformed window
// filter	- pointer to used filter
// img		- source image
// outimg	- output img
// mwidth	- image width
// mheight	- image height
// wwidth	- 1/2 window width
// wheight	- 1/2 window height
// x,y		- center of window
// ---------------------------------------------------------------------------------------------
__int32 FilterWin32NCH(__int32 * filter, __int32 * img, __int32 * outimg, int x, int y, int mwidth, int mheght, int wwidth, int wheight){
	__int32 Light = 0;
	__int32 tmp = 0;
	int i,j,k, s,m;
	int minx = max(1, x - wwidth);
	int miny = max(1, x- wheight);
	int maxx = min(mwidth-1, x+wwidth);
	int maxy = min(mheght-1, y+wheight);

	for (i = miny; i < maxy; i++){
		k = i*mwidth + minx;
		s  = k -mwidth;		// top
		m = k + mwidth;		// bottom
		for (j = minx; j<maxx; j++){
			tmp = filter[0]*img[s]	+ filter[3]*img[k] + filter[6]*img[m];
			tmp += filter[1]*img[s+1]+ filter[4]*img[k+1] + filter[7]*img[m+1];
			tmp += filter[2]*img[s+2]+ filter[5]*img[k+2] + filter[8]*img[m+2];
			tmp *= filter[9];
			tmp /= filter[10];
			outimg[k] = tmp;
			k++; s++; m++;
			Light += tmp;
		}//end point loop
	}
	m = (maxx-minx)*(maxy-miny);
	if (m) 
		Light = Light/m;
	return Light;
}


// ---------------------------------------------------------------------------------------------
// FilterWin32HCH run filter NCH to window and return the medium light of transformed window
// filter	- pointer to used filter
// img		- source image
// outimg	- output img
// mwidth	- image width
// mheight	- image height
// wwidth	- 1/2 window width
// wheight	- 1/2 window height
// x,y		- center of window
// ---------------------------------------------------------------------------------------------

__int32 FilterWin32HCH(__int32 * filter, __int32 * img, __int32 * outimg, int x, int y, int mwidth, int mheght, int wwidth, int wheight){
	__int32 Light = 0;
	__int32 tmp = 0;
	int i,j,k, s,m;
	int minx = max(1, x - wwidth);
	int miny = max(1, x- wheight);
	int maxx = min(mwidth-1, x+wwidth);
	int maxy = min(mheght-1, y+wheight);

	for (i = miny; i < maxy; i++){
		k = i*mwidth + minx;
		s  = k -mwidth;		// top
		m = k + mwidth;		// bottom
		for (j = minx; j<maxx; j++){
			tmp = filter[0]*img[s]	+ filter[3]*img[k] + filter[6]*img[m];
			tmp = filter[1]*img[s+1]+ filter[4]*img[k+1] + filter[7]*img[m+1];
			tmp = filter[2]*img[s+2]+ filter[5]*img[k+2] + filter[8]*img[m+2];
			outimg[k] = tmp;
			k++; s++; m++;
			Light += tmp;
		}//end point loop
	}
	m = (maxx-minx)*(maxy-miny);
	if (m) 
		Light = Light/m;
	return Light;
}

// evCalc4 - calc egenvalue
// | a b |
// | c d |
unsigned __int32  evCalc4(unsigned char a, unsigned char b, unsigned char c, unsigned char d,  unsigned __int32 &lam1, unsigned __int32 &lam2){
	unsigned __int32 t1 = (a+d)<<7;
	 lam2 = t1*t1 + ((a*d - b*c)<<16);
	 unsigned __int32 t2 = intsqrt((unsigned __int64) lam2);
	 lam1 = t1 +t2;
	 if (t1 > t2)
		 lam2 = t1 -t2;
	 else 
		 lam2 = t2 -t1;
	 return t2;
}//returned value has dimention 2**8

/////////////////////////////////////////////////////////////////////////
void evVisualisation(int width, int height, unsigned char * in, unsigned char * output)
{
	int i,j,k,s, m;
	unsigned __int32 lam1, lam2, lam3, lam4;
	for (i = 1; i<height-1; i++){
		k = i*width;
		s = k -width;
		m = k+width;
		for (j = 1; j < width -1; j++){
			output[k+j] = (unsigned char)((evCalc4(in[s+j-1],in[s+j+1],in[m+j-1],in[m+j+1], lam1, lam2) +
				evCalc4(in[k+j-1],in[k+j+1],in[s+j-1],in[m+j], lam3, lam4))>>9);
		//	output[k+j] = (unsigned char)(lam1 - lam2 +lam3 -lam4)>>10;
		}
	}//end row loop
}





// ---------------------------------------------------------------------------------------------

__int32 FilterGrad2(unsigned char * img, unsigned char * outimg, unsigned char * tmpimg, int x, int y, int mwidth, int mheght, int wwidth, int wheight){
	__int32 Light = 0;
	unsigned __int32 tmp = 0;

	FilterWinHCH((__int32 *) GRAD_FILTERX, img, outimg, x, y, mwidth, mheght, wwidth, wheight);
	FilterWinHCH((__int32 *) GRAD_FILTERY, img, tmpimg, x, y, mwidth, mheght, wwidth, wheight);


	
	int i,j;
	unsigned int k, s,m;
	int minx = max(0, x - wwidth);
	int miny = max(0, x- wheight);
	int maxx = min(mwidth, x+wwidth);
	int maxy = min(mheght, y+wheight);
	s =0; m = 65800;
	for (i = miny; i < maxy; i++){
		k = i*mwidth + minx;
		for (j = minx; j<maxx; j++){
			tmp = ((unsigned __int32 )tmpimg[k])*tmpimg[k]	+ ((unsigned __int32 )outimg[k])*outimg[k];
			if (tmp > s) s = tmp;
			if(tmp < m) m = tmp;
			outimg[k] = tmp>>8;
			k++; 
			Light += tmp;
		}//end point loop
	}
	k = (maxx-minx)*(maxy-miny);
	if (k) {
		Light = Light/k;
	}

	return Light;
}
//==============================================================================

__int32 DinDiap(unsigned char * img, unsigned char * outimg, int x, int y, int mwidth, int mheght, int wwidth, int wheight){
	__int32 Light = 0;
	unsigned __int32 tmp = 0;

	int i,j,k, mmax,mmin;
	int minx = max(0, x - wwidth);
	int miny = max(0, y- wheight);
	int maxx = min(mwidth, x+wwidth);
	int maxy = min(mheght, y+wheight);
	float sigma2 = 0.0;
	float fLight = 0.0;
	mmax =0; mmin = 65800;
	for (i = miny; i < maxy; i++){
		k = i*mwidth + minx;
		for (j = minx; j<maxx; j++){
			if (img[k] > mmax) mmax = img[k];
			if(img[k] < mmin) mmin = img[k];
			k++; 
			Light += img[k];
		}//end point loop
	}
	k = (maxx-minx)*(maxy-miny);
	if (k) {
		fLight = (float) 1.0*Light/k;
		Light = Light/k;
	}
	Light = mmax + mmin - Light;
	
	mmax = max(1, mmax-mmin);
	for (i = miny; i < maxy; i++){
		k = i*mwidth + minx;
		for (j = minx; j<maxx; j++){
			tmp = (img[k] - mmin)*255/mmax;
			sigma2 += ((float) 1.0)*tmp*tmp;
//			if (tmp == 255){
//				outimg[k] = tmp;
//			}
			outimg[k] = tmp;
			k++; 
		}//end point loop
	}
	k = (maxx-minx)*(maxy-miny);
	sigma2 = sigma2/k -  fLight*fLight;
#ifdef _DEBUG
	_RPTW3(_CRT_WARN, L"\r\n DinDiap sigma2 = %f, deriv = %f, sigma_n = %f ", sigma2, fLight, sigma2/(fLight*fLight+0.001));
#endif
	return Light;
}
///////////////////////////////////// INSERT AND SORTING /////////////////////////////////////////////////
inline void sorting(int * arrval, int szarr, int x, int y, int val, int & count, int & vmin){
	int k = 0;	int kk =0;
	if (count < szarr) {//add to any way
		if (arrval[count*3+2] < val){//insert
			//check where add
			for (k = 0; k <count; k++){//loop 1
				kk = k*3;
				if (arrval[kk+2] < val){//insert
					memcpy(&arrval[kk+3], &arrval[kk], sizeof(int)*(count-k)*3);
					arrval[kk++] = x;
					arrval[kk++] = y;
					arrval[kk] = val;
					vmin = arrval[count*3+2];
					count++;
					break;
				}//end insert
			}//end loop 1
			if (k >= count){
				kk = count*3;
				arrval[kk++] = x;
				arrval[kk++]=y;
				arrval[kk] = val;
				count++;
				vmin = val;
			}
		}
		else{//add
			kk = count*3;
			arrval[kk++] = x;
			arrval[kk++]=y;
			arrval[kk] = val;
			count++;
			vmin = val;
		}
	}//end add to any
	else{ // insert
		if (vmin < val){// check if need add
			for(k=0; k< szarr; k++){
				kk = k*3;
				if (arrval[kk+2] < val){
					memcpy(&arrval[kk+3], &arrval[kk], sizeof(int)*(szarr-k)*3);
					arrval[kk++] = x;
					arrval[kk++] = y;
					arrval[kk] = val;
//					vmin = arrval[count*3+2];
					vmin = arrval[(szarr-1)*3+2];
					break;
				}
			}
		}//end if add 

	}//end insert





#if 0
	if (count < szarr) {//add to any way
		if (arr[count] < tmp){//insert
			//check where add
			for (k = 0; k <count; k++){//loop 1
				if (arr[k] < tmp){//insert
					memcpy(&arr[k+1], &arr[k], sizeof(int)*(count-k));
					arr[k] = tmp;
					vmin = arr[count];
					count++;
					break;
				}//end insert
			}//end loop 1
			if (k >= count){
				arr[count] = tmp;
				count++;
				vmin = tmp;
			}
		}
		else{//add
			arr[count] = tmp;
			count++;
			vmin = tmp;
		}
	}//end add to any
	else{ // insert
		if (vmin < tmp){// check if need add
			for(k=0; k< szarr; k++){
				if (arr[k] < tmp){
					memcpy(&arr[k+1], &arr[k], sizeof(int)*(szarr-k));
					arr[k] = tmp;
					vmin = arr[szarr-1];
					break;
				}
			}
		}//end if add 

	}//end insert


#endif
}//end sorting



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//									GradientCreate
//////////////////////////////////////////////////////////////////////////////////////////////////////////
 // conebt new version 
__int32 GradientCreateGauss(__int32 * filter, int width, int height, unsigned char * img, __int32 * out32)
{

	int x,y,k, s,m;
	__int32 Light = 0;
	__int32 tmp = 0;
	__int32 * matr = 0;

	matr = (__int32 *) malloc(sizeof(__int32)*width*height);
	if (matr == 0) 
		return -66000;
	memset(matr, 0, width*height*sizeof(__int32));
	for (y = 2; y< height-2; y++){
		k = y*width +1;
		s = k - width;		// top
		m = k + width;		// bottom
		for (x = 1; x < width-1; x++){
			tmp = filter[0]*img[s-1]+ filter[3]*img[k-1] + filter[6]*img[m-1];
			tmp += filter[1]*img[s]  + filter[4]*img[k]   + filter[7]*img[m];
			tmp += filter[2]*img[s+1]+ filter[5]*img[k+1] + filter[8]*img[m+1];
			matr[k] = tmp;
			k++; s++; m++;
			Light += tmp;
//#ifdef _DEBUG
//			if (tmp < 0){
//				_RPTW1(_CRT_WARN, L"\r\n !!!!!!!!!!!!!!!!!! Gradient more 0x7FF: %x !!!!!!\r\n", tmp);
//			}
//#endif
		}
	}
	memset(out32, 0, width*height*sizeof(__int32));
	for(y = 2; y <height-2; y++){
		m = y*width + 2;
		for (x = 2; x <width-2; x++){
			tmp = 12*matr[m] + 4*(matr[m+1]+matr[m-1]) + matr[m+2] + matr[m-2] + 4*(matr[m+width]+matr[m-width]) + matr[m+2*width] + matr[m-2*width];
			out32[m] =( tmp >>5);
//#ifdef _DEBUG
//			if (abs(out32[m]) > 0x000007FF){
//				_RPTW1(_CRT_WARN, L"\r\n !!!!!!!!!!!!!!!!!! Gradient more 0x000007FF: %x !!!!!!\r\n", out32[m]);
//			}
//#endif
			m++;
		}
	}
	k = (width-4)*(height-4);
	if (k) Light /= k;
	if (matr){
		free(matr); matr = 0;
	}
	return Light;
}

//#if 0 // old version
__int32 GradientCreate(__int32 * filter, int width, int height, unsigned char * img, __int32 * out32)
{
	int x,y,k, s,m;
	__int32 Light = 0;
	__int32 tmp = 0;
	for (y = 1; y< height-1; y++){
		k = y*width +1;
		s = k - width;		// top
		m = k + width;		// bottom
		for (x = 1; x < width-1; x++){
			tmp = filter[0]*img[s-1]+ filter[3]*img[k-1] + filter[6]*img[m-1];
			tmp = filter[1]*img[s]  + filter[4]*img[k]   + filter[7]*img[m];
			tmp = filter[2]*img[s+1]+ filter[5]*img[k+1] + filter[8]*img[m+1];
			out32[k] = tmp;
			k++; s++; m++;
			Light += tmp;
		}
	}
	k = (width-2)*(height-2);
	if (k>0) Light /= k;
	return Light;
}

///////////////////////////////////////////////////////////////////////////////////////////////
__int32 GradientWindowCreate(__int32 * filter, int width, int height, RECT br, unsigned char * img, __int32 * out32)
{
	int x,y,k, s,m;
	__int32 Light = 0;
	__int32 tmp = 0;
	for (y = br.top; y< br.bottom; y++){
		k = y*width +br.left;
		s = k - width;		// top
		m = k + width;		// bottom
		for (x = br.left; x < br.right; x++){
			tmp = filter[0]*img[s-1]	+ filter[3]*img[k-1] + filter[6]*img[m-1];
			tmp = +filter[1]*img[s]+ filter[4]*img[k] + filter[7]*img[m];
			tmp = +filter[2]*img[s+1]+ filter[5]*img[k+1] + filter[8]*img[m+1];
			out32[k] = tmp;
			k++; s++; m++;
			Light += tmp;
		}
	}
	k = (br.right-br.left)*(br.bottom - br.top);
	if (k) Light /= k;
	return Light;
}
 //#endif
///

__int32 GradientWindowCreateGauss(__int32 * filter, int width, int height, RECT br, unsigned char * img, __int32 * out32)
{
	int x,y,k, s,m;
	int i, j = 1;
	__int32 Light = 0;
	__int32 tmp = 0;
	k = sizeof(__int32)*(br.bottom - br.top+4)*(br.right-br.left+4);
	__int32 * matr = (__int32 *)malloc(k);
	memset((unsigned char *)matr, 0, k);
	i = 2*(br.right-br.left+4);
	for (y = br.top; y< br.bottom; y++){
		k = y*width +br.left;
		s = k - width;		// top
		m = k + width;		// bottom
		j=2;
		for (x = br.left; x < br.right; x++){
			tmp = filter[0]*img[s-1]	+ filter[3]*img[k-1] + filter[6]*img[m-1];
			tmp += filter[1]*img[s]+ filter[4]*img[k] + filter[7]*img[m];
			tmp += filter[2]*img[s+1]+ filter[5]*img[k+1] + filter[8]*img[m+1];
			matr[i+j] = tmp;
			k++; s++; m++;
			j++;
			Light += tmp;
		}
		i += (br.right-br.left+4);
	}
	k = (br.right-br.left)*(br.bottom - br.top);
	if (k) Light /= k;
	s = br.right - br.left +4;

	for(y = 0; y <br.bottom-br.top; y++){
		m = (y+2)*s + 2;
		k = (y+br.top)*width + br.left;
		for (x = br.left; x <br.right; x++){
			tmp = 12*matr[m] + 4*(matr[m+1]+matr[m-1]) + matr[m+2] + matr[m-2] + 4*(matr[m+s]+matr[m-s]) + matr[m+2*s] + matr[m-2*s];
			out32[k] =( tmp >>5);
//#ifdef _DEBUG
//			if (abs(out32[m]) > 0x000007FF){
//				_RPTW1(_CRT_WARN, L"\r\n !!!!!!!!!!!!!!!!!! Gradient more 0x000007FF: %x !!!!!!\r\n", out32[m]);
//			}
//#endif
			k++;
			m++;
		}
	}
	free(matr);
#if _DEBUG
#ifdef _PRINTGRAD
	_RPTW0(_CRT_WARN, L"\r\n !! Gradient matrix small\r\n");
	for	 (int k = 310; k<330; k++){
		for (int m = 340; m <390; m++){
			_RPTW1(_CRT_WARN, L"%5d!", out32[k*width+m]);
		}
		_RPTW0(_CRT_WARN, L"\r\n");
	}
#endif
#endif
	return Light;
}
/////////////////////////////////////////// x, y is 24.8 coords ////////////////////////////////
static __int32 _interpolate(__int32 x, __int32 y, IntImage img){ // coord in format 25.7; x,y > 0
	__int32 xt = (int) x>>7;  /* coordinates of top-left corner */
	__int32 yt = (int) y>>7;
	__int32 ax = x&0x0000007F;;
	__int32 ay = y&0x0000007F;;
	__int32 *ptr = img.data + (img.cols*yt) + xt;
#if 0 //ndef _DNDEBUG
	if (xt<0 || yt<0 || xt>=img->ncols-1 || yt>=img->nrows-1) {
		fprintf(stderr, "(xt,yt)=(%d,%d)  imgsize=(%d,%d)\n(x,y)=(%f,%f)  (ax,ay)=(%f,%f)\n", xt, yt, img->ncols, img->nrows, x, y, ax, ay);
		fflush(stderr);
	}
#endif

//	assert (xt >= 0 && yt >= 0 && xt <= img.cols - 2 && yt <= img.rows - 2);
	
	return ( ( (128-ax)*(128-ay)* *ptr + ax*(128-ay)* *(ptr+1) + (128-ax)*ay* *(ptr+(img.cols)) + ax*ay* *(ptr+(img.cols)+1))>>14);
}


static __int32 _interpolate8(__int32 x, _int32 y, int cols, unsigned char * img){ // coord in format 25.7; x,y > 0
	__int32 xt = (int) x>>7;  /* coordinates of top-left corner */
	__int32 yt = (int) y>>7;
	__int32 ax = x&0x0000007F;;
	__int32 ay = y&0x0000007F;;
	__int32 value = 0;
	unsigned char *ptr = img + (cols*yt) + xt;
#if 0 //ndef _DNDEBUG
	if (xt<0 || yt<0 || xt>=img->ncols-1 || yt>=img->nrows-1) {
		fprintf(stderr, "(xt,yt)=(%d,%d)  imgsize=(%d,%d)\n(x,y)=(%f,%f)  (ax,ay)=(%f,%f)\n", xt, yt, img->ncols, img->nrows, x, y, ax, ay);
		fflush(stderr);
	}
#endif

	value =  ( (128-ax)*(128-ay)* *ptr + ax*(128-ay)* *(ptr+1) + (128-ax)*ay* *(ptr+cols) + ax*ay* *(ptr+cols+1))>>14;
//	assert (xt >= 0 && yt >= 0 && xt <= img.cols - 2 && yt <= img.rows - 2);
	
	return (value);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//    select Good features   INTIMGSTRUCT
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void IntSelGoodPoints( INTIMGSTRUCT &imps, unsigned char * img )
{
	if (!(imps.iMask&0xFFE00000)) return;
	//1. Create gradient matrix;
	if (GradientCreate( (__int32 *) GRAD_FILTERX1, imps.ncols, imps.nrows, img, imps.gradx.data)== -66000){
		return;
	}
	if (GradientCreate( (__int32 *) GRAD_FILTERY1, imps.ncols, imps.nrows, img, imps.grady.data)== -66000){
		return;
	}
	// 2. smooth gradient matrixes !!??
	// 3. for every point calculate min egenvalue and store to pointlist
	int window_hw, window_hh;
//	int *pointlist;
	int npoints = 0;

	__int32 gx, gy;
	__int32 gxx, gxy, gyy;
	int xx, yy;
	__int32 *ptr;
	__int32 val;
//	__int32 sum;
	//__int64 det;
	__int64 dsk;
	unsigned int limit = 0x6FFFFFFF;
	int borderx = imps.tc.borderx;	/* Must not touch cols */
	int bordery = imps.tc.bordery;	/* lost by convolution */
	int x, y;
	int i;
	window_hw = imps.tc.window_height/2;
	window_hh = imps.tc.window_width/2;
	if (borderx < window_hw)  borderx = window_hw;
	if (bordery < window_hh)  bordery = window_hh;

//#define MAXPOINTNUM 64
	int arrval[MAXPOINTNUM*3+3];// = (int *) malloc(sizeof(int)*3*20);
	int vmin = 0x0;
	int count = 0;
	int vmax =1;
	memset( arrval, 0,sizeof(arrval));

	ptr = imps.pointlist;
	imps.tc.nSkippedPixels = 3;
	for (y = bordery ; y < imps.nrows - bordery ; y += imps.tc.nSkippedPixels ){
		for (x = borderx ; x < imps.ncols - borderx ; x += imps.tc.nSkippedPixels + 1)  {
			/* Sum the gradients in the surrounding window */
			gxx = 0;  gxy = 0;  gyy = 0;
			for (yy = y-window_hh ; yy <= y+window_hh ; yy++){
				for (xx = x-window_hw ; xx <= x+window_hw ; xx++)  {
					gx = *(imps.gradx.data + imps.ncols*yy+xx);
					gy = *(imps.grady.data + imps.ncols*yy+xx);
					gxx += gx * gx;
					gxy += gx * gy;
					gyy += gy * gy;
				}
			}
			/* Store the trackability of the pixel as the minimum of the two eigenvalues */
			*ptr++ = x;
			*ptr++ = y;
			//val = _minEigenvalue(gxx, gxy, gyy);
			dsk = ((__int64)gxx*gyy)- (gxy/2)*(gxy/2);
			//dsk = abs(((__int64)gxx*gyy)- (gxy/2)*(gxy/2));
			val = (int) (dsk >> 30);
#if 0
			//val = _minEigenvalue(gxx, gxy, gyy);
			sum = (gxx + gyy)<<6; //25.7 / 2
			dsk = ((__int64)(gxx -gyy)*(gxx-gyy));
			dsk = dsk<<12; //(25.7 + 25.7)^2 /4
			det = gxy;
			det = det*det;
			det = det<<14;
			val = sum - intsqrt(dsk+det); //25.7
#endif
			if (val < 0){
				val = -val;
			}
			if (val)
				sorting(arrval, MAXPOINTNUM, x,y, val, count, vmin);

//			val = val>>7;
	//		if (val > limit) val = limit;
			*ptr++ = (int) val;
			npoints++;
		}
	}
// check control points

	// sorting points
//	_sortPointList(imps.pointlist, npoints);
	//select elements to featurelist

	memset(imps.featuremap,0, imps.ncols*imps.nrows);

	if (0/*!overwriteAllFeatures*/){
		for (i = 0 ; i < imps.fl.nFeatures; i++){
			if (imps.fl.feature[i].val >= 0)  {
				x   =  imps.fl.feature[i].x;
				y   =  imps.fl.feature[i].y;
	//			_fillFeaturemap(x, y, imps.featuremap, mindist, imps.ncols, imps.nrows);
				for (yy = y - mindist ; yy <= y + mindist ; yy++){
					for (xx = x - mindist ; xx <= x + mindist ; xx++){
						if (xx >= 0 && xx < imps.ncols && yy >= 0 && yy < imps.nrows)
							imps.featuremap[yy*imps.ncols+xx] = 1;			
					}
				}//end featuremap loop
			}
		}
	}
	ptr = imps.pointlist;
	ptr = arrval;
	npoints =count;
	i = 0;
	while (1)  {    /* If we can't add all the points, then fill in the rest
						of the featurelist with -1's */
		if (ptr >= arrval/*imps.pointlist*/ + 3*npoints)  {
			while (i < imps.fl.nFeatures)  {	
				if (imps.fl.feature[i].val < 0)	{
					imps.fl.feature[i].x   = -1;
					imps.fl.feature[i].y   = -1;
					imps.fl.feature[i].val = KLT_NOT_FOUND;
//					imps.fl.feature[i].aff_img = 0;
//					imps.fl.feature[i].aff_img_gradx = 0;
//					imps.fl.feature[i].aff_img_grady = 0;
// test					imps.fl.feature[i].aff_x = -1;
// test					imps.fl.feature[i].aff_y = -1;
// test					imps.fl.feature[i].aff_Axx = 1;
// test					imps.fl.feature[i].aff_Ayx = 0;
// test					imps.fl.feature[i].aff_Axy = 0;
// test					imps.fl.feature[i].aff_Ayy = 1;
				}
				i++;
			}
			break;
		}
		x   = *ptr++;
		y   = *ptr++;
		val = *ptr++;
		/* Ensure that feature is in-bounds */
//		assert(x >= 0);
//		assert(x < ncols);
//		assert(y >= 0);
//		assert(y < nrows);
//		while (!overwriteAllFeatures && indx < featurelist->nFeatures && featurelist->feature[indx]->val >= 0)
//			indx++;
		if (i >= imps.fl.nFeatures)  
			break;
		/* If no neighbor has been selected, and if the minimum
		eigenvalue is large enough, then add feature to the current list */
		if (!imps.featuremap[y*imps.ncols+x] && val >= min_eigenvalue)  {
			imps.fl.feature[i].x   =  x;
			imps.fl.feature[i].y   = y;
			imps.fl.feature[i].val = val;
//			imps.fl.feature[i].aff_img = NULL;
//			imps.fl.feature[i].aff_img_gradx = NULL;
//			imps.fl.feature[i].aff_img_grady = NULL;
// test			imps.fl.feature[i].aff_x = 0;
// test			imps.fl.feature[i].aff_y = 0;
// test			imps.fl.feature[i].aff_Axx = 1;
// test			imps.fl.feature[i].aff_Ayx = 0;
// test			imps.fl.feature[i].aff_Axy = 0;
// test			imps.fl.feature[i].aff_Ayy = 1;
			i++;
/* Fill in surrounding region of feature map, but make sure that pixels are in-bounds */
			
//new sorting			for (yy = y - mindist ; yy <= y + mindist ; yy++){
//new sorting				for (xx = x - mindist ; xx <= x + mindist ; xx++){
//new sorting					if (xx >= 0 && xx < imps.ncols && yy >= 0 && yy < imps.nrows)
//new sorting						imps.featuremap[yy*imps.ncols+xx] = 1;			
//new sorting				}
//new sorting			}//end featuremap loop
		}//end if
	}

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void CalcDifImg(int width, int height, unsigned char * input1, unsigned char * input2, __int32 * outImg){
	int i, j, k;
	k = 0;
	for (i = 0; i< height; i++){
		for(j = 0; j<width; j++){
			outImg[k] = ((__int32) 128)*input2[k] - ((__int32) 128)*input1[k];
//			outImg[k] ^= input1[k];
			//outImg[k] = (outImg[k]<<8);
			k++;
		}
	}
}//end CalcDifImg

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// void IntTrackPoints(INTIMGSTRUCT &imps)
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void IntTrackPoints(INTIMGSTRUCT &imps){
	// select feature from tracert list
	// step 1 create gradient matrix for selected point
	int i,/* j,*/ k, m;
	__int32 gx, gy;
	__int32 gxx, gxy, gyy, diffx, diffy;
	int xx, yy;
//	__int32 *ptr;
	__int32 val;
	__int64 sum;
	__int64 det, dsk;
	__int32 px, py, pxe, pye, im, px0, py0;//, pxe0, pye0;
	int window_hh = imps.tc.window_height;
	int window_hw = imps.tc.window_width;
	
	if (!(imps.iMask&0xFFE00000)) return;
	//1. Create gradient matrix;
//	DinDiap( imps.img, imps.img, imps.ncols/2, imps.nrows/2, imps.ncols, imps.nrows, imps.ncols, imps.nrows);
//	DinDiap( imps.img2, imps.img2, imps.ncols/2, imps.nrows/2, imps.ncols, imps.nrows, imps.ncols, imps.nrows);
	GradientCreate( (__int32 *) GRAD_FILTERX, imps.ncols, imps.nrows, imps.img, imps.gradx.data);
	GradientCreate( (__int32 *) GRAD_FILTERY, imps.ncols, imps.nrows, imps.img, imps.grady.data);
//	GradientCreate( (__int32 *) GRAD_FILTERX, imps.ncols, imps.nrows, imps.img2, imps.tmpimg.data);
//	GradientCreate( (__int32 *) GRAD_FILTERY, imps.ncols, imps.nrows, imps.img2, imps.cstmpimg.data);


//	CalcDifImg( imps.ncols, imps.nrows, imps.img2, imps.img, imps.floatimg.data);
	// loop for fl
	for (i=0; i<imps.fl.nFeatures; i++){
		// calculate matrix and difference for all point
		if (imps.fl.feature[i].val < 0) continue; //point can be losed
		
		// calculate matrixes

		gxx = 0;  gxy = 0;  gyy = 0; diffx =0; diffy = 0;
		px0 = px = imps.fl.feature[i].x; py0 = py = imps.fl.feature[i].y;
		
		pxe = px<<7; pye = py<<7;

		m = 3;
		val = imps.fl.feature[i].val;
		// loop for egenvalue
		while ((val > 10)&&m) {
			gxx = 0;  gxy = 0;  gyy = 0; diffx =0; diffy = 0;

			for (yy = py -window_hh ; yy <= py + window_hh ; yy++){
				k = imps.ncols*yy + px - window_hw; 
				for (xx = px - window_hw ; xx <= px + window_hw ; xx++)  {
//				gx = (imps.gradx.data[k] - imps.tmpimg.data[k]);//*(imps.gradx.data + imps.ncols*yy+xx);
					//				gy = (imps.grady.data[k] - imps.cstmpimg.data[k]);//*(imps.grady.data + imps.ncols*yy+xx);
					pxe = (pxe&0x0000007F)+(xx<<7); pye =  (pye&0x0000007F)+(yy<<7);
	
//					gx = imps.gradx.data[k];//*(imps.gradx.data + imps.ncols*yy+xx);
//					gy = imps.grady.data[k];//*(imps.grady.data + imps.ncols*yy+xx);

					gx =_interpolate(pxe,pye,imps.gradx);
					gy =_interpolate(pxe,pye,imps.grady);
					gxx += gx * gx;
					gxy += gx * gy;
					gyy += gy * gy;
				
//					im = imps.floatimg.data[k];
//					im = ((__int32) 128)*imps.img2[k] - 128*_interpolate8(pxe,pye,imps.ncols, imps.img) ;
					im = ((__int32) 128)*_interpolate8(pxe,pye,imps.ncols, imps.img) - ((__int32) 128)*_interpolate8(pxe,pye,imps.ncols, imps.img2);//imps.img[k] ;
					diffx += gx*im;
					//				diffx += gx*imps.floatimg.data[k];
					diffy += gy*im;
					//				diffy += gy*imps.floatimg.data[k];
					k++;
				}//end row loop
			}//end loop
			
			// calculate egen value
			//val = _minEigenvalue(gxx, gxy, gyy);


			dsk = ((__int64)gxx*gyy)- (gxy/2)*(gxy/2);
			//dsk = abs(((__int64)gxx*gyy)- (gxy/2)*(gxy/2));
			val = (int) (dsk >> 30);

//			sum = ((__int64)gxx + gyy)<<6; //25.7 / 2
//			dsk = ((__int64)(gxx - gyy))*(gxx -gyy);
//			dsk = dsk<<12; //(25.7 + 25.7)^2 /4
//			det = gxy;
//			det = det*det;
//			det = det<<14;
//			val = sum - intsqrt(dsk+det); //25.7
			if (val < 0){	val = -val;	}
//			val = val>>9;
			if (val){// track point ok!
				// solv equation
				imps.fl.feature[i].val = val;
				det = ((__int64)gxx *gyy) - ((__int64)gxy*gxy);
				if (det == 0) {
					if ( m >= 3)
						imps.fl.feature[i].val = KLT_SMALL_DET;
					break;
				}

				pxe = (pxe&0x0000007F)+(px<<7); pye =  (pye&0x0000007F)+(py<<7);
				sum =  ((__int64)diffx)*gyy - ((__int64) diffy)*gxy; // 25.7
				//imps.fl.feature[i].aff_x = sum/det;
				pxe +=  (int) (sum/det);
				sum = ((__int64)gxx)*diffy - ((__int64) diffx)*gxy;
//				imps.fl.feature[i].aff_y = sum/det; //25.7
				pye += (int) (sum/det);
				
				px = pxe >> 7;
				py = pye >> 7;
			}
			else{
				//  point was be lost
				imps.fl.feature[i].val = -1; 
			}
			m--;
		}//end loop
		imps.fl.feature[i].x = px; //( (imps.fl.feature[i].x<<7) + imps.fl.feature[i].aff_x)>>7;
		imps.fl.feature[i].y = py;//( (imps.fl.feature[i].y<<7) + imps.fl.feature[i].aff_y)>>7;
	}//end loop for feature list





}




void IntWindowSelGoodPoints ( 
				   unsigned char * img,					// source bw 8-bits image
				   int ncols, int nrows,				// sizeof image
				   __int32 * gradx, __int32 * grady,	// temp matrixes for gradient must be ncols*nrows size ????
				   unsigned char * featuremap,			// temp bufer for sorting point ncols*nrows
				   __int32 * pointlist,					// temp buffer for value must be 3*nrows*ncols
				   INT_TrackContextRec & tc,			// tracert contect
				   INT_FeatureList	& fl,				// feature list
				   int px, int py,						// center of window
				   int width, int height				// 1/2 window size
)
{
	//1. Create gradient matrix;
	fl.Clear();
	if (GradientCreateGauss( (__int32 *) GRAD_FILTERX1, ncols, nrows, img, gradx) == -66000) {
		fl.feature[0].val = -100;
		return;
	}
	if (GradientCreateGauss( (__int32 *) GRAD_FILTERY1, ncols, nrows, img, grady) == -66000){
		fl.feature[0].val = -100;
		return;
	}
	// 2. smooth gradient matrixes !!??
	// 3. for every point calculate min egenvalue and store to pointlist
	int window_hw, window_hh;
//	int *pointlist;
	int npoints = 0;

	__int32 gx, gy;
	__int32 gxx, gxy, gyy;
	int xx, yy;
	__int32 *ptr;
	__int32 val;
//	__int32 sum;
	__int64  dsk;
	int limit = 0x6FFFFFFF;
	int borderx = tc.borderx;	/* Must not touch cols */
	int bordery = tc.bordery;	/* lost by convolution */
	int x, y;
	int i;
	int wws, wwe, hhs, hhe; // adaptive image rectangle width and height
	window_hw = tc.window_height/2;
	window_hh = tc.window_width/2;
	if (borderx < window_hw)  borderx = window_hw;
	if (bordery < window_hh)  bordery = window_hh;

	wws = max(borderx, px-width+borderx);
	wwe = min(ncols-borderx, px+width-borderx);
	hhs = max(bordery, py-height+bordery);
	hhe = min(nrows - bordery, py+height-bordery);
	fl.Clear();

	ptr = pointlist;
 //   memset(ptr,0, 
//#define MAXPOINTNUM 64
	int arrval[MAXPOINTNUM*3+3]={};// = (int *) malloc(sizeof(int)*3*20);
	int vmin = 0x0;
	int count = 0;
	int vmax =1;
//	memset(arrval,0,sizeof(arrval));


	tc.nSkippedPixels = 3;
	for (y = hhs ; y < hhe ; y += tc.nSkippedPixels ){
		for (x = wws ; x < wwe ; x += tc.nSkippedPixels + 1)  {
			/* Sum the gradients in the surrounding window */
			gxx = 0;  gxy = 0;  gyy = 0;
			if ((x > 300)&& (y> 150)){//==
				x=x;
			}
			for (yy = y-window_hh ; yy <= y+window_hh ; yy++){
				for (xx = x-window_hw ; xx <= x+window_hw ; xx++)  {
					gx = *(gradx + ncols*yy+xx);
					gy = *(grady + ncols*yy+xx);
					gxx += gx * gx;
					gxy += gx * gy;
					gyy += gy * gy;
				}
			}
			/* Store the trackability of the pixel as the minimum of the two eigenvalues */
			*ptr++ = x;
			*ptr++ = y;
			//other way select max 
			dsk = ((__int64)gxx*gyy)- (gxy/2)*(gxy/2);
			//dsk = abs(((__int64)gxx*gyy)- (gxy/2)*(gxy/2));
			val = (int) (dsk >> 30);
#if 0
			//val = _minEigenvalue(gxx, gxy, gyy);
			sum = (gxx + gyy)<<6; //25.7 / 2
			dsk = ((__int64)(gxx -gyy)*(gxx-gyy));
			dsk = dsk<<12; //(25.7 + 25.7)^2 /4
			det = gxy;
			det = det*det;
			det = det<<14;
			val = sum - intsqrt(dsk+det); //25.7
#endif
			if (val < 0){
				val = -val;
			}
//			val = val>>7;
			if (val > limit) 
				val = limit;
			*ptr++ = (int) val;
			if (val)
				sorting(arrval, MAXPOINTNUM, x,y, val, count, vmin);

			npoints++;
		}
	}
// check control points

	// sorting points
//	_sortPointList(pointlist, npoints);
	//select elements to featurelist
	memset(featuremap,0, ncols*nrows);

	if (0/*!overwriteAllFeatures*/){
		for (i = 0 ; i < fl.nFeatures; i++){
			if (fl.feature[i].val >= 0)  {
				x   =  fl.feature[i].x;
				y   =  fl.feature[i].y;
	//			_fillFeaturemap(x, y, imps.featuremap, mindist, imps.ncols, imps.nrows);
				for (yy = y - mindist ; yy <= y + mindist ; yy++){
					for (xx = x - mindist ; xx <= x + mindist ; xx++){
						if (xx >= 0 && xx < ncols && yy >= 0 && yy < nrows)
							featuremap[yy*ncols+xx] = 1;			
					}
				}//end featuremap loop
			}
		}
	}
	ptr = pointlist;
	ptr = arrval;
	memset(pointlist, 0, npoints*3*sizeof(int));
	npoints = count;
	memcpy(pointlist,arrval, sizeof(arrval));

	i = 0;
	while (1)  {    /* If we can't add all the points, then fill in the rest
						of the featurelist with -1's */
		if (ptr >= arrval/*pointlist*/ + 3*npoints)  {
			while (i < fl.nFeatures)  {	
				if (fl.feature[i].val < 0)	{
					fl.feature[i].x   = 0;
					fl.feature[i].y   = -1;
					fl.feature[i].val = KLT_NOT_FOUND;
// test					fl.feature[i].aff_x = -1;
// test					fl.feature[i].aff_y = -1;
// test					fl.feature[i].aff_Axx = 1;
// test					fl.feature[i].aff_Ayx = 0;
// test					fl.feature[i].aff_Axy = 0;
// test					fl.feature[i].aff_Ayy = 1;
				}
				i++;
			}
			break;
		}
		x   = *ptr++;
		y   = *ptr++;
		val = *ptr++;
		/* Ensure that feature is in-bounds */
//		assert(x >= 0);
//		assert(x < ncols);
//		assert(y >= 0);
//		assert(y < nrows);
//		while (!overwriteAllFeatures && indx < featurelist->nFeatures && featurelist->feature[indx]->val >= 0)
//			indx++;
		if (i >= fl.nFeatures)  
			break;
		/* If no neighbor has been selected, and if the minimum
		eigenvalue is large enough, then add feature to the current list */
//new sorting		if (!featuremap[y*ncols+x] && val >= min_eigenvalue)  {
			fl.feature[i].x   =  x;
			fl.feature[i].y   = y;
			fl.feature[i].val = val;
// test			fl.feature[i].aff_x = 0;
// test			fl.feature[i].aff_y = 0;
// test			fl.feature[i].aff_Axx = 1;
// test			fl.feature[i].aff_Ayx = 0;
// test			fl.feature[i].aff_Axy = 0;
// test			fl.feature[i].aff_Ayy = 1;
			i++;
/* Fill in surrounding region of feature map, but make sure that pixels are in-bounds */
			
//new sorting			for (yy = y - mindist ; yy <= y + mindist ; yy++){
//new sorting				for (xx = x - mindist ; xx <= x + mindist ; xx++){
//new sorting					if (xx >= 0 && xx < ncols && yy >= 0 && yy < nrows)
//new sorting						featuremap[yy*ncols+xx] = 1;			
//new sorting				}
//new sorting			}//end featuremap loop
//new sorting		}//end if
	}
}
////////////////////////////////////////////////////////////////////
//                  Select from region
///////////////////////////////////////////////////////////////////
void IntRgnSelGoodPoints( INTIMGSTRUCT &imps, ARRGOODRECT &arr, unsigned char * img )
{
	if (!(imps.iMask&0xFFE00000)) return;
	//1. Create gradient matrix;
	GradientCreate( (__int32 *) GRAD_FILTERX, imps.ncols, imps.nrows, img, imps.gradx.data);
	GradientCreate( (__int32 *) GRAD_FILTERY, imps.ncols, imps.nrows, img, imps.grady.data);
	// 2. smooth gradient matrixes !!??
	// 3. for every point calculate min egenvalue and store to pointlist
	int window_hw, window_hh;
//	int *pointlist;
	int npoints = 0;
	int MaxSize = imps.ncols*imps.nrows/8;
	__int32 gx, gy;
	__int32 gxx, gxy, gyy;
	int xx, yy;
	__int32 *ptr;
	__int32 val;
//	__int32 sum;
	__int64 /*det,*/ dsk;
	int limit = 0x1FFFFFFF;
	int borderx = imps.tc.borderx;	/* Must not touch cols */
	int bordery = imps.tc.bordery;	/* lost by convolution */
	int x, y;
	int i, ind, k;
	window_hw = imps.tc.window_height/2;
	window_hh = imps.tc.window_width/2;
	if (borderx < window_hw)  borderx = window_hw;
	if (bordery < window_hh)  bordery = window_hh;
	i =0;
	ptr = imps.pointlist;
	imps.fl.Clear();

//#define MAXPOINTNUM 64
	int arrval[MAXPOINTNUM*3+3];// = (int *) malloc(sizeof(int)*3*20);
	int arrval_sel[MAXPOINTNUM*3+3];
	int vmin = 1;
	int count = 0;
	int count_sel = 0;
	int vmin_sel =1;
//	int kk;
//	int k;
	

	memset( arrval, 0,sizeof(arrval));
	memset( arrval_sel, 0,sizeof(arrval));

	imps.tc.nSkippedPixels = 3;

	for (ind = 0; ind < arr.num; ind++){
		if (npoints >= MaxSize) break;
		if (arr.br[ind].count){
			npoints = 0; // all privileg to first
			for (y = arr.br[ind].rect.top ; y < arr.br[ind].rect.bottom; y += imps.tc.nSkippedPixels ){
				if (npoints >= MaxSize) break;
				for (x = arr.br[ind].rect.left ; x < arr.br[ind].rect.right ; x += imps.tc.nSkippedPixels + 1)  {
					if (npoints >= MaxSize) break;
					/* Sum the gradients in the surrounding window */
					gxx = 0;  gxy = 0;  gyy = 0;
					if (y <3){
						break;
					}
					for (yy = y-window_hh ; yy <= y+window_hh ; yy++){
						for (xx = x-window_hw ; xx <= x+window_hw ; xx++)  {
							gx = *(imps.gradx.data + imps.ncols*yy+xx);
							gy = *(imps.grady.data + imps.ncols*yy+xx);
							gxx += gx * gx;
							gxy += gx * gy;
							gyy += gy * gy;
						}
					}
					/* Store the trackability of the pixel as the minimum of the two eigenvalues */
					*ptr++ = x;
					*ptr++ = y;
					dsk = ((__int64)gxx*gyy)- (gxy/2)*(gxy/2);
					//dsk = abs(((__int64)gxx*gyy)- (gxy/2)*(gxy/2));
					val = (int) (dsk >> 30);
#if 0
					//val = _minEigenvalue(gxx, gxy, gyy);
					sum = (gxx + gyy)<<6; //25.7 / 2
					dsk = ((__int64)(gxx -gyy)*(gxx-gyy));
					dsk = dsk<<12; //(25.7 + 25.7)^2 /4
					det = gxy;
					det = det*det;
					det = det<<14;
					val = sum - intsqrt(dsk+det); //25.7
#endif
					if (val < 0){
						val = -val;
					}

					if (val >1) {
						if (arr.br[ind].IsSelect)
							sorting(arrval_sel, MAXPOINTNUM, x,y, val, count_sel, vmin_sel);
						else
							sorting(arrval, MAXPOINTNUM, x,y, val, count, vmin);
					}
					

					//					val = val>>7;
					//					if (val > limit) val = limit;
					*ptr++ = (int) val;
					npoints++;
				}

			}
			if (npoints == 0) continue;



			// sorting points
			//		_sortPointList(imps.pointlist, npoints);
			//select elements to featurelist
			memset(imps.featuremap,0, imps.ncols*imps.nrows);

			ptr = imps.pointlist;
			k = 0;

		}//end check count

	}//end arr loop

// check control points
	if (count_sel){
		if (vmin_sel > vmin){
			k =0;
			do {
				sorting(arrval, MAXPOINTNUM, arrval_sel[k*3],arrval_sel[k*3+1], arrval_sel[k*3+2], count, vmin);
				k++;
				if (k >= count_sel) break;
			}while (vmin_sel > vmin);
		}
		else{
			count_sel = max(1, count_sel/4);
			if (count < count_sel){
				memcpy(&arrval[count*3],&arrval_sel[0], sizeof(int)*count_sel*3);
				count += count_sel;
			}
			else{
				memcpy(&arrval[count_sel*3], &arrval[0], (sizeof(int)*3*(count-count_sel)));
				memcpy(&arrval[0], &arrval_sel[0], sizeof(int)*count_sel*3);
			}
		}
	}

			ptr = imps.pointlist;
			ptr = arrval;
			npoints =count;
			i = 0;
			while (1)  {    /* If we can't add all the points, then fill in the rest
							of the featurelist with -1's */
				if (k++ > 8) break;
				if (ptr >= arrval/*imps.pointlist*/ + 3*npoints)  {
					while (i < imps.fl.nFeatures)  {	
						if (imps.fl.feature[i].val < 0)	{
							imps.fl.feature[i].x   = -1;
							imps.fl.feature[i].y   = -1;
							imps.fl.feature[i].val = KLT_NOT_FOUND;
						}
						i++;
					}
					break;
				}
				x   = *ptr++;
				y   = *ptr++;
				val = *ptr++;
				/* Ensure that feature is in-bounds */
				//		assert(x >= 0);
				//		assert(x < ncols);
				//		assert(y >= 0);
				//		assert(y < nrows);
				if (i >= imps.fl.nFeatures)  
					break;
				/* If no neighbor has been selected, and if the minimum
				eigenvalue is large enough, then add feature to the current list */
//new sorting				if (!imps.featuremap[y*imps.ncols+x] && val >= min_eigenvalue)  {
					imps.fl.feature[i].x   =  x;
					imps.fl.feature[i].y   = y;
					imps.fl.feature[i].val = val;
					i++;
					/* Fill in surrounding region of feature map, but make sure that pixels are in-bounds */

					//new sorting				for (yy = y - mindist ; yy <= y + mindist ; yy++){
					//new sorting					for (xx = x - mindist ; xx <= x + mindist ; xx++){
					//new sorting						if (xx >= 0 && xx < imps.ncols && yy >= 0 && yy < imps.nrows)
					//new sorting							imps.featuremap[yy*imps.ncols+xx] = 1;			
					//new sorting					}
					//new sorting				}//end featuremap loop
//new sorting				}//end if
			}

			memcpy(imps.pointlist,arrval, sizeof(int)*3*count);
}

///////////////////////////////////////////////////////
void IntRgnSelGoodPoints1( INTIMGSTRUCT &imps, ARRGOODRECT &arr, unsigned char * img )
{
	if (!(imps.iMask&0xFFE00000)) return;
	//1. Create gradient matrix;
//	GradientCreate( (__int32 *) GRAD_FILTERX, imps.ncols, imps.nrows, img, imps.gradx.data);
//	GradientCreate( (__int32 *) GRAD_FILTERY, imps.ncols, imps.nrows, img, imps.grady.data);
	// 2. smooth gradient matrixes !!??
	// 3. for every point calculate min egenvalue and store to pointlist
	int window_hw, window_hh;
//	int *pointlist;
	int npoints = 0;
	int MaxSize = imps.ncols*imps.nrows/8;
	__int32 gx, gy;
	__int32 gxx, gxy, gyy;
	int xx, yy;
	__int32 *ptr;
	__int32 val;
//	__int32 sum;
	__int64 /*det,*/ dsk;
	int limit = 0x1FFFFFFF;
	int borderx = imps.tc.borderx;	/* Must not touch cols */
	int bordery = imps.tc.bordery;	/* lost by convolution */
	int x, y;
	int i, ind, k;
	window_hw = imps.tc.window_height/2;
	window_hh = imps.tc.window_width/2;
	if (borderx < window_hw)  borderx = window_hw;
	if (bordery < window_hh)  bordery = window_hh;
	i =0;
	ptr = imps.pointlist;
	imps.fl.Clear();

//#define MAXPOINTNUM 64
	int arrval[MAXPOINTNUM*3+3];// = (int *) malloc(sizeof(int)*3*20);
	int arrval_sel[MAXPOINTNUM*3+3];
	int vmin = 1;
	int count = 0;
	int count_sel = 0;
	int vmin_sel =1;
//	int kk;
//	int k;
	RECT rcur; // curent boundary rect
	
	

	memset( arrval, 0,sizeof(arrval));
	memset( arrval_sel, 0,sizeof(arrval));

	imps.tc.nSkippedPixels = 3;
	npoints = 0; // all privileg to first

	for (ind = 0; ind < arr.num; ind++){
		if (npoints >= MaxSize) break;
		if (arr.br[ind].count){
			rcur.bottom = min(arr.br[ind].rect.bottom+window_hh+1, imps.nrows);
			rcur.left = max(arr.br[ind].rect.left-window_hh-1, 0);
			rcur.right = min(arr.br[ind].rect.right+window_hh+1, imps.ncols);
			rcur.top = max(arr.br[ind].rect.top-window_hh-1, 0); 
			GradientWindowCreate( (__int32 *) GRAD_FILTERX1, imps.ncols, imps.nrows, rcur, img, imps.gradx.data);
			GradientWindowCreate( (__int32 *) GRAD_FILTERY1, imps.ncols, imps.nrows, rcur, img, imps.grady.data);

			for (y = arr.br[ind].rect.top ; y < arr.br[ind].rect.bottom; y += imps.tc.nSkippedPixels){
				if (npoints >= MaxSize) break;
#if _DEBUG
	_RPTW0(_CRT_WARN, L"\r\n");
#endif
				for (x = arr.br[ind].rect.left ; x < arr.br[ind].rect.right ; x += imps.tc.nSkippedPixels + 1)  {
					if (npoints >= MaxSize) break;
					/* Sum the gradients in the surrounding window */
					gxx = 0;  gxy = 0;  gyy = 0;
					if (y <3){
						break;
					}

					for (yy = y-window_hh ; yy <= y+window_hh ; yy++){
						for (xx = x-window_hw ; xx <= x+window_hw ; xx++)  {
							gx = imps.gradx.data[imps.ncols*yy+xx];//*(imps.gradx.data + imps.ncols*yy+xx);
							gy = imps.grady.data[imps.ncols*yy+xx];//*(imps.grady.data + imps.ncols*yy+xx);
							gxx += gx * gx;
							gxy += gx * gy;
							gyy += gy * gy;
						}
					}
					/* Store the trackability of the pixel as the minimum of the two eigenvalues */
					*ptr++ = x;
					*ptr++ = y;
					dsk = ((__int64)gxx*gyy)- (gxy/2)*(gxy/2);
					//dsk = abs(((__int64)gxx*gyy)- gxy*gxy);
					val = (int) (dsk >> 30);
#if 0
					//val = _minEigenvalue(gxx, gxy, gyy);
					sum = (gxx + gyy)<<6; //25.7 / 2
					dsk = ((__int64)(gxx -gyy)*(gxx-gyy));
					dsk = dsk<<12; //(25.7 + 25.7)^2 /4
					det = gxy;
					det = det*det;
					det = det<<14;
					val = sum - intsqrt(dsk+det); //25.7
#endif
					if (val < 0){
						val = -val;
					}

					if (val >1) {
						if (arr.br[ind].IsSelect)
							sorting(arrval_sel, MAXPOINTNUM, x,y, val, count_sel, vmin_sel);
						else
							sorting(arrval, MAXPOINTNUM, x,y, val, count, vmin);
					}
					

					//					val = val>>7;
					//					if (val > limit) val = limit;
					*ptr++ = (int) val;
					npoints++;
				}

			}
			if (npoints == 0) continue;



			// sorting points
			//		_sortPointList(imps.pointlist, npoints);
			//select elements to featurelist
			memset(imps.featuremap,0, imps.ncols*imps.nrows);

			ptr = imps.pointlist;
			k = 0;

		}//end check count

	}//end arr loop
#if _DEBUG
	_RPTW0(_CRT_WARN, L"\r\n");
#endif

// check control points
	if (count_sel){
		if (vmin_sel > vmin){
			k =0;
			do {
				sorting(arrval, MAXPOINTNUM, arrval_sel[k*3],arrval_sel[k*3+1], arrval_sel[k*3+2], count, vmin);
				k++;
				if (k >= count_sel) break;
			}while (vmin_sel > vmin);
		}
		else{
			count_sel = max(1, count_sel/4);
			if (count < count_sel){
				memcpy(&arrval[count*3],&arrval_sel[0], sizeof(int)*count_sel*3);
				count += count_sel;
			}
			else{
				memcpy(&arrval[count_sel*3], &arrval[0], (sizeof(int)*3*(count-count_sel)));
				memcpy(&arrval[0], &arrval_sel[0], sizeof(int)*count_sel*3);
			}
		}
	}

			ptr = imps.pointlist;
			ptr = arrval;
			npoints =count;
			i = 0;
			while (1)  {    /* If we can't add all the points, then fill in the rest
							of the featurelist with -1's */
				if (k++ > 8) break;
				if (ptr >= arrval/*imps.pointlist*/ + 3*npoints)  {
					while (i < imps.fl.nFeatures)  {	
						if (imps.fl.feature[i].val < 0)	{
							imps.fl.feature[i].x   = -1;
							imps.fl.feature[i].y   = -1;
							imps.fl.feature[i].val = KLT_NOT_FOUND;
						}
						i++;
					}
					break;
				}
				x   = *ptr++;
				y   = *ptr++;
				val = *ptr++;
				/* Ensure that feature is in-bounds */
				//		assert(x >= 0);
				//		assert(x < ncols);
				//		assert(y >= 0);
				//		assert(y < nrows);
				if (i >= imps.fl.nFeatures)  
					break;
				/* If no neighbor has been selected, and if the minimum
				eigenvalue is large enough, then add feature to the current list */
//new sorting				if (!imps.featuremap[y*imps.ncols+x] && val >= min_eigenvalue)  {
					imps.fl.feature[i].x   =  x;
					imps.fl.feature[i].y   = y;
					imps.fl.feature[i].val = val;
					i++;
					/* Fill in surrounding region of feature map, but make sure that pixels are in-bounds */

					//new sorting				for (yy = y - mindist ; yy <= y + mindist ; yy++){
					//new sorting					for (xx = x - mindist ; xx <= x + mindist ; xx++){
					//new sorting						if (xx >= 0 && xx < imps.ncols && yy >= 0 && yy < imps.nrows)
					//new sorting							imps.featuremap[yy*imps.ncols+xx] = 1;			
					//new sorting					}
					//new sorting				}//end featuremap loop
//new sorting				}//end if
			}

			memcpy(imps.pointlist,arrval, sizeof(int)*3*count);
}

/////////////////////////////////////////////////////////
void CountLogicalCenter(INT_FeatureList & fl, POINT & lcp){
	// calculate center
	__int64 x = 0;
	__int64 y = 0;
	__int64 sqr = 0;
	__int64 sumsqr = 0;
	for (int i = 0; i< fl.nFeatures; i++){
		if (fl.feature[i].val > 0){
			sqr = fl.feature[i].val*fl.feature[i].val;
			sumsqr += sqr;
			x += sqr*fl.feature[i].x;
			y += sqr*fl.feature[i].y;
		}
		else
			break;
	}
	if (sumsqr){
		x = x/sumsqr;
		y =y/sumsqr;
	}
	lcp.x = (int) x;
	lcp.y = (int) y;
}

void CountLogicalCenter(int *pointlist, POINT & lcp, int cnt){
	// calculate center
	__int64 x = 0;
	__int64 y = 0;
	__int64 sqr = 0;
	__int64 sumsqr = 0;
	for (int i = 0; i< cnt; i++){
		if (pointlist[i*3+2] > 0){
			sqr =  ((pointlist[i*3+2])&0x0007FFFF);//*pointlist[i*3+2];
	//		sqr *= sqr;
	//		sqr = sqr>>5;
	//		if (sqr > 0) sqr = 1;
	//		else sqr = 0;
			sumsqr += sqr;
			x += sqr*pointlist[i*3];
			y += sqr*pointlist[i*3+1];
		}
	}
	if ((x<0)||(y<0)){
		x=y=0;
	}
	if (sumsqr){
		x = x/sumsqr;
		y =y/sumsqr;
	}
	else {x=y=0;}
	if ((x<0)||(y<0)){
		x=y=0;
	}
	lcp.x = (LONG) x;
	lcp.y = (LONG) y;
}


////////////////////////////
void Contrast(unsigned char * input, unsigned char * output, int rows, int cols, int level)
{// change contrast to level
//	level = level&0x000000FF;
	int szSize = rows*cols;
	int Light = 0;
	int mmax = 1;
	int mmin = 255;
	for (int i=0; i< szSize; i++){
		Light = Light + input[i];
		if (mmin > input[i]) mmin =input[i];
		if (mmax < input[i]) mmax = input[i];
	}

	Light /= szSize;
	
	int k1 = mmax - mmin;
	if ((k1==0)||level==0) {
		memset(output, (unsigned char) Light, szSize);
	}
	else{
		for (int i =0; i< szSize; i++){
			if (input[i]>0) 
				mmin = (input[i]-Light)*level/128 +Light;
			mmin = (input[i]-Light)*level/64 +Light;
			mmax=mmin;
			if (mmin<0) mmax=0;
			if(mmin > 255) mmax =255;
			output[i] = (unsigned char) mmax ;
			//output[i] = (unsigned char) (Light - (Light - input[i])*level/255);
			//output[i] = (unsigned char) (255* ( level*(szSize*(input[i]-Light)+k1) - 255 )/szSize +Light )/k1 ;
		}
	}
}

void CountDiffImg( INTIMGSTRUCT &imps, INT_FeatureList & cl, unsigned char * img1, unsigned char * img2){
	__int64 gxx = 0, gyy = 0, gxy =0, gxt=0, gyt =0, dsk =0, det = 0;
	int dt =0, dx = 0, dy=0 ;
	int gx,gy;
	int s, k;
//	_RPTW1(_CRT_WARN, L"\r\n\t DELTA for %d points: ", cl.nFeatures);
	for (int i = 0; i < cl.nFeatures; i++) {
		if (imps.fl.feature[i].val < 1){
			break;
		}
		if (cl.feature[i].val <1) {
			break;
		}
		for (int yy = -3;/*= imps.fl.feature[i].y-3*/ yy <=3/* imps.fl.feature[i].y+3*/; yy++){
			s = (yy+imps.fl.feature[i].y)*imps.ncols + imps.fl.feature[i].x -3;
			k = (yy+ cl.feature[i].y)*imps.ncols + cl.feature[i].x -3;
			for (int xx = imps.fl.feature[i].x-3; xx <= imps.fl.feature[i].x+3; xx++)  {
				gx = *(imps.gradx.data + s);
				gy = *(imps.grady.data + s);
				dt =  img1[s] ^ img2[k];
				dsk += dt;//*dt;

				gxx +=  gx * gx;
				gxy += gx * gy;
				gyy += gy * gy;
				//			dt =  ((int) img1[s] - img2[s]);
				gxt += gx * dt;
				gyt += gy * dt;
				s++;k++;
			}
		}
		dx = imps.fl.feature[i].x - cl.feature[i].x;
		dy = imps.fl.feature[i].y - cl.feature[i].y;
		det = gxx*dx + gxy*dy  - gxt;
		imps.fl.feature[i].aff_x = (int) gxx;
		det = gyy*dx + gyy*dy  - gyt;
		imps.fl.feature[i].aff_y = (int) gyy;
		det = (dsk/cl.feature[i].val)>>16;
//		_RPTW1(_CRT_WARN, L" %d,", det);//, imps.fl.feature[i].aff_x, imps.fl.feature[i].aff_y);
		if (det) imps.fl.feature[i].Clear();
//		if (imps.fl.feature[i].val){
//			_RPTW2(_CRT_WARN, L" (%d,%d);", imps.fl.feature[i].aff_x, imps.fl.feature[i].aff_y);
//		}

	}//end loop for feture list
//	_RPTW0(_CRT_WARN, L"\r\n\t END DELTA\r\n");
}

//=========================================
__int32 LaplasPoint(int x, int y, unsigned char * img, int wx, int wy){
	__int32 Lap = 0;
	int k = wy*y + x;
	int m = wy*(y-1)+x;
	int s = wy*(y+1)+x;
	if ((x > 2)&&(y>2)&&(x <wx -2)&&(y < wy-2)){
		Lap = 4*img[k]-img[k-1]-img[k+1] - img[m] - img[s];
	}
	return Lap;

}

__int32 LaplasRect(BOUNDGOODPOINT &lr, unsigned char * img, int wx, int wy){
	__int32 Lap = 0;
	__int32 L =0;
	int k,m,s, p=0;

	for (int y = lr.rect.top; y <= lr.rect.bottom; y++){ 
		for (int x = lr.rect.left; x <= lr.rect.right; x++){
			k = wy*y + x;
			m = wy*(y-1)+x;
			s = wy*(y+1)+x;
			if ((x > 2)&&(y>2)&&(x <wx -2)&&(y < wy-2)){
				p++;
				L += img[k];
				Lap += 5*img[k]-img[k-1]-img[k+1] - img[m] - img[s];
			}
		}
	}
	if (L){Lap = Lap*p/L;}
	return Lap;

}
// divergetion
__int32 DivRect(BOUNDGOODPOINT &lr, unsigned char * img, int wx, int wy){
	__int32 Lap = 0;
	__int32 L =0;
	int k,m,s, p=0;

	for (int y = lr.rect.top; y <= lr.rect.bottom; y++){ 
		for (int x = lr.rect.left; x <= lr.rect.right; x++){
			k = wy*y + x;
			m = wy*(y-1)+x;
			s = wy*(y+1)+x;
			if ((x > 2)&&(y>2)&&(x <wx -2)&&(y < wy-2)){
				p++;
				L += img[k];
				Lap += img[k+1]-img[k-1] +img[m] - img[s];
			}
		}
	}
//	if (L){Lap = Lap*p/L;}
	return Lap/p;
}


void Gauss(unsigned char * matr, unsigned char * out , int nrows, int ncols){
	int tmp =0;
	int x,y, m;
	for (y = 2; y <nrows -2; y++){
		m = ncols*y +2;
		for (x = 2; x < ncols-2; x++){
			tmp = ((int ) 12)*matr[m] + 4*(matr[m+1]+matr[m-1]) + matr[m+2] + matr[m-2] + 4*(matr[m+ncols]+matr[m-ncols]) + matr[m+2*ncols] + matr[m-2*ncols];
			out[m] = (tmp >>5);
			m++;
		}
	}
}

////////////////////////////////////////////////////////////////////////

int TestLocalPoints(INTIMGSTRUCT &imps, BOUNDGOODPOINT &pold,BOUNDGOODPOINT &pnew,unsigned char * imgnew, unsigned char * imgold){
	int diff = 0;
	int xx,x;
	int mm = min(pold.minrad, pnew.minrad)/2;
	for (int i= -mm; i<= mm; i++){
		x = imps.ncols*(pnew.y +i) + pnew.x -mm;  //new
		xx = imps.ncols*(pold.y +i) + pold.x -mm; //old
		for (int k=-mm; k<=mm; k++){
			diff += imps.gradx.data[x]*(pold.x - pnew.x) + imps.grady.data[x]*(pold.y - pnew.y) +imgnew[x] - imgold[xx];
			xx++; x++;
		}
	}

		//imps.gradx.data[pnew.y*imps.ncols + pnew.x]*(pnew.x - pold.x) + imps.grady.data[pnew.y*imps.ncols + pnew.x]*(pnew.y - pold.y) -imgnew[pnew.y*imps.ncols + pnew.x] + imgold[pold.y*imps.ncols + pold.x];
	return (abs(diff)/mm/mm);

}

/////////////////////////////////////////////////////////////////////////////////////////////
inline int metrika(int x, int px, int y, int py, int porog){
	if (abs(x-px)>= porog) return 0;
	if (abs(y-py)>= porog) return 0;
	return (porog -abs(x-px))+(porog - abs(y-py)); 
}
inline int metrika2(BOUNDGOODPOINT & br1, BOUNDGOODPOINT & br2){
	int porog = br1.minrad +br2.minrad;
	int x = (min(br1.rect.left,br2.rect.left)+max(br1.rect.right,br2.rect.right)+1)/2;
	int y = (min(br1.rect.top, br2.rect.top)+max(br1.rect.bottom, br2.rect.bottom)+1)/2;
	return metrika(br1.x,x,br1.y,y,porog)*metrika(br2.x,x,br2.y,y,porog);
}

int getBoundsRects(ARRGOODRECT &arr, INT_FeatureList &list, int porog){
	int cnt = 0;
	int szmas = list.nFeatures*list.nFeatures;
	BOUNDGOODPOINT br;
	int * mas = (int *) malloc(szmas*sizeof(int));
	if (!mas) 
		return 0; // error!!!
	br.Clear();

	memset(mas,0, szmas*sizeof(int));

	arr.Clear(); // clear arr loops
	int * p =0;	

	for (int i= 0; i< list.nFeatures; i++){
		if (list.feature[i].val == 0) 
			continue; // point not exit or in bound
		p = &mas[i*list.nFeatures]; // row adress
		*p= 256;
		br.x = list.feature[i].x;
		br.y = list.feature[i].y;
		br.val = list.feature[i].val;
		br.rect.left = br.rect.right = list.feature[i].x;
		br.rect.top = br.rect.bottom = list.feature[i].y;
		br.minrad = porog/2;
		br.count = 1;
		for (int j=i+1; j<list.nFeatures; j++){
			if (br.val == 0) break;
			if (list.feature[j].val == 0) continue;
			p[j]=  metrika(br.x,list.feature[j].x, br.y,list.feature[j].y, porog);
			if (p[j] == 0) continue; // very long get next point;
			//fill rect information
			br.x = (br.x + list.feature[j].x+1)/2;
			br.y = (br.y + list.feature[j].y+1)/2;
			br.rect.left  = max(_iMinX, min(br.rect.left, list.feature[j].x));
			br.rect.right = min(_iMaxX, max(br.rect.right, list.feature[j].x));
			br.rect.top = max(_iMinY,min(br.rect.top, list.feature[j].y));
			br.rect.bottom = min(_iMaxY,max(br.rect.bottom, list.feature[j].y));
			br.minrad = max(br.minrad, (br.rect.right - br.rect.left+1)/2);
			br.minrad = max(br.minrad, (br.rect.bottom - br.rect.top+1)/2);
			br.count++;
			// ??????????????????????????????? clear val
			list.feature[j].val = 0;
		}//end loop j;
		// br - resulting rectangle; 
		for (int k =0; k < arr.num; k++){
			if (br.val == 0) break; // point  already  in bound
			if (arr.br[k].val == 0){
				// add new rect ?
				br.x = (br.rect.left+br.rect.right+1)/2;
				br.y = (br.rect.top + br.rect.bottom+1)/2;
				arr.br[k] = br;
				cnt++;
				break;
			}
			else{
				//mas[i] = metrika(arr.br[k].x, br.x, arr.br[k].y, br.y,  max(porog,max(arr.br[k].minrad, br.minrad)));
				mas[i] = metrika2(arr.br[k],br);//(arr.br[k].x, br.x, arr.br[k].y, br.y,  max(arr.br[k].minrad, br.minrad));
				if (mas[i] > 0){
					arr.br[k].x = (arr.br[k].x + br.x +1 )/2;
					arr.br[k].y = (arr.br[k].y + br.y +1 )/2;
					arr.br[k].rect.left = min(arr.br[k].rect.left, br.rect.left );
					arr.br[k].rect.right = max(arr.br[k].rect.right, br.rect.right );
					arr.br[k].rect.top = min(arr.br[k].rect.top, br.rect.top );
					arr.br[k].rect.bottom = max(arr.br[k].rect.bottom, br.rect.bottom );
					arr.br[k].minrad = max(10, max( (arr.br[k].rect.right-arr.br[k].rect.left+1)/2,  (arr.br[k].rect.bottom-arr.br[k].rect.top+1)/2  ));
					arr.br[k].count += br.count;	  
					arr.br[k].x = (arr.br[k].rect.left + arr.br[k].rect.right+1)/2;
					arr.br[k].y = (arr.br[k].rect.top + arr.br[k].rect.bottom+1)/2;

					break;
				}
			}// end check point
				
		}// loop for arr;


	}//end loop for points
	//centring
	for(int k =0; k<arr.num; k++){
		if (arr.br[k].val){
	//		arr.br[k].x = (arr.br[k].rect.left + arr.br[k].rect.right+1)/2;
	//		arr.br[k].y = (arr.br[k].rect.top + arr.br[k].rect.bottom+1)/2;
			arr.br[k].rect.left = max(_iMinX, arr.br[k].x - arr.br[k].minrad);
			arr.br[k].rect.right = min(_iMaxX, arr.br[k].x + arr.br[k].minrad);
			arr.br[k].rect.top = max(_iMinY, arr.br[k].y - arr.br[k].minrad);
			arr.br[k].rect.bottom = min(_iMaxY, arr.br[k].y + arr.br[k].minrad);
		}
		else
			break;
	}


	if (mas) free(mas);
	return cnt;
}
/////////////////////////////////////////////////////////////////////////////////////////////