// PhotoProcessing tools functions
#include "stdafx.h"
#include "tools.h"
#include "common_build.h"
#include "host_build.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPChannel red;
CPChannel blue;
CPChannel green;
CPChannel gray;

typedef struct _ppExitInfo{
	unsigned int FileSize;
	int ImageWidth;
	int ImageHeight;
	int	ISO;
	float mpixels;
	CString FileName;
	CString Directory;
	CString FileModifyDate;
	CString DateTimeOriginal;
	CString FileType;
	CString ImageDescription;
	CString Make;
	CString Model;
	CString Lens;
	CString LensID;
	CString ExposureProgram;
	CString LightValue;
	CString FNumber;
	CString ExposureTime; 
	CString MaxApertureValue;
	CString ExposureCompensation;
	CString MeteringMode;
	CString ScaleFactor35efl;
	CString CircleOfConfusion;
	CString Lens35efl;
	
}ppExitInfo;

//class CPChannal
CPChannel::CPChannel(){
	arr =0;
	sz.x = sz.y = 0;
	bi = 0;
	lmin = 255;
	lmax = 0;
}
CPChannel::CPChannel(CPoint nsz){
	if (bi) free(bi); bi = 0;
	if (arr) free(arr); arr = 0;
	sz.x = sz.y = 0;
	arr = (uint8*) malloc(nsz.x*nsz.y);
	lmin = 255;
	lmax = 0;
	if (arr){
		sz = nsz;
		bi  =(BITMAPINFO *) malloc(sizeof(BITMAPINFOHEADER )+ 256*sizeof(RGBQUAD));
		if (bi){
			for (int i =0; i< 256; i++){
				bi->bmiColors[i].rgbBlue = bi->bmiColors[i].rgbGreen = bi->bmiColors[i].rgbRed = (BYTE) i;
			}
			bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bi->bmiHeader.biHeight = nsz.y;
			bi->bmiHeader.biWidth = nsz.x;
			bi->bmiHeader.biSizeImage = nsz.x*nsz.y;
			bi->bmiHeader.biClrImportant = 0;
			bi->bmiHeader.biXPelsPerMeter = bi->bmiHeader.biYPelsPerMeter = 0;
			bi->bmiHeader.biBitCount = 8;
			bi->bmiHeader.biCompression = BI_RGB;
			bi->bmiHeader.biPlanes = 1;
			bi->bmiHeader.biClrUsed = 256;
		}
		else{
			free(arr); arr =0;
			sz.x = sz.y = 0;
		}
	}
};//end alloc mem;
CPChannel::CPChannel(int width, int height){
	CPChannel(CPoint::CPoint(width, height));
};
CPChannel::~CPChannel(){
	if (bi) free(bi); bi = 0;
	if (arr) free(arr); arr = 0;
	sz.x = sz.y = 0;
};
int CPChannel::Init(int width, int height){
	CPoint nsz(width, height);
	lmin = 255;
	lmax = 0;
	if (bi) free(bi); bi = 0;
	if (arr) free(arr); arr = 0;
	sz.x = sz.y = 0;
	if (nsz.x*nsz.y)
		arr = (uint8*) malloc((nsz.x+1)*(nsz.y+1));
	if (arr){
		memset(arr,0, (nsz.x+1)*(nsz.y+1));
		sz = nsz;
		bi  =(BITMAPINFO *) malloc(sizeof(BITMAPINFOHEADER )+ 256*sizeof(RGBQUAD));
		if (bi){
			memset(bi,0,sizeof(BITMAPINFOHEADER )+ 256*sizeof(RGBQUAD));
			for (int i =0; i< 256; i++){
				bi->bmiColors[i].rgbBlue = bi->bmiColors[i].rgbGreen = bi->bmiColors[i].rgbRed = (BYTE) i;
			}
			bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bi->bmiHeader.biHeight = nsz.y;
			bi->bmiHeader.biWidth = nsz.x;
			bi->bmiHeader.biSizeImage = nsz.x*nsz.y;
			bi->bmiHeader.biClrImportant = 0;
			bi->bmiHeader.biXPelsPerMeter = bi->bmiHeader.biYPelsPerMeter = 0;
			bi->bmiHeader.biBitCount = 8;
			bi->bmiHeader.biCompression = BI_RGB;
			bi->bmiHeader.biPlanes = 1;
			bi->bmiHeader.biClrUsed = 256;
		}
		else{
			free(arr); arr =0;
			sz.x = sz.y = 0;
		}
	}
	return (sz.x*sz.y);
};
int CPChannel::InitFromImage(int offset, CImage & img){
	int size = 0;
	if (bi) free(bi); bi = 0;
	if (arr) free(arr); arr = 0;
	sz.x = sz.y = 0;
	CBitmap * cBmp1 = CBitmap::FromHandle(HBITMAP(img));
	BITMAP bimg1;
	if (!cBmp1->GetBitmap(&bimg1)) 		
		return 0;
	int width = bimg1.bmWidth;
	int height = bimg1.bmHeight;
	if (bimg1.bmBitsPixel/8 < offset) 
		offset = 0;
	size = bimg1.bmHeight*bimg1.bmWidth;
	uint8 * mem = (uint8 *)bimg1.bmBits;
	int bpp = img.GetBPP()/8;
	if (Init(width, height) > 0){
		for (int i=0; i< size; i++){
			arr[i] =  mem[i*bpp+offset]; 
			if (lmin > arr[i]) lmin = arr[i];
			if (lmax< arr[i]) lmax = arr[i];
		}
		
	}

	return size;
};

int CPChannel::InitFromBitmap(int offset, Gdiplus::Bitmap * bmp){
	int size = 0;
	if (bi) free(bi); bi = 0;
	if (arr) free(arr); arr = 0;
	sz.x = sz.y = 0;
	HBITMAP hbm;
	bmp->GetHBITMAP(Gdiplus::Color(0,0,0), &hbm);
	CBitmap  * cBmp1 = CBitmap::FromHandle(hbm);
	BITMAP bimg1;
	if (!cBmp1->GetBitmap(&bimg1)) 		
		return 0;
	bpp = bimg1.bmBitsPixel/8;
	if (bpp < offset) 
		offset = 0;
	int width = bimg1.bmWidth;
	int height = bimg1.bmHeight;
	
	size = height*width;
	uint8 * mem = (uint8 *)bimg1.bmBits;
//	int bpp = bimg1.bmBitsPixel/8;
	if (Init(width, height) > 0){
		if (offset >3){ //covert to gray
			int pix = 0;
			for (int i=0; i< size; i++){
				int psz = min(3, bpp);
				pix = 0;
				for (int k=0; k< psz; k++){
					pix += mem[i*bpp+k];
				}
				if (psz)	arr[i] =  pix/psz;
				else arr[i] = 0;
				if (lmin > arr[i]) lmin = arr[i];
				if (lmax< arr[i]) lmax = arr[i];
			};
		}
		else{
			for (int i=0; i< size; i++){
				arr[i] =  mem[i*bpp+offset];  
				if (lmin > arr[i]) lmin = arr[i];
				if (lmax< arr[i]) lmax = arr[i];
			};
		}

	}
	return size;

//	if (cBmp1){
//		delete[] cBmp1;
//	}
};


int CPChannel::InitFromCBitmap(int offset, BITMAP & bimg1 /*CBitmap & cBmp1*/){
	int size = 0;
	if (bi) free(bi); bi = 0;
	if (arr) free(arr); arr = 0;
	sz.x = sz.y = 0;
//	BITMAP bimg1;
//	if (!cBmp1.GetBitmap(&bimg1)) 		
//		return 0;
	bpp = bimg1.bmBitsPixel/8;
	if (bpp < offset) 
		offset = 0;
	int width = bimg1.bmWidth;
	int height = bimg1.bmHeight;
	
	size = height*width;
	uint8 * mem = (uint8 *)bimg1.bmBits;
//	int bpp = bimg1.bmBitsPixel/8;
	if (Init(width, height) > 0){
		if (offset >3){ //covert to gray
			int pix = 0;
			for (int i=0; i< size; i++){
				int psz = min(3, bpp);
				pix = 0;
				for (int k=0; k< psz; k++){
					pix += mem[i*bpp+k];
				}
				if (psz)	arr[i] =  pix/psz;
				else arr[i] = 0;
				if (lmin > arr[i]) lmin = arr[i];
				if (lmax< arr[i]) lmax = arr[i];
			};
		}
		else{
			for (int i=0; i< size; i++){
				arr[i] =  mem[i*bpp+offset];  
				if (lmin > arr[i]) lmin = arr[i];
				if (lmax< arr[i]) lmax = arr[i];
			};
		}

	}
	return size;
};

int CPChannel::Inverse(){
	int size = 0;
	if (arr){
		lmin = 255;
		lmax =0;
		size = sz.x*sz.y;
//		int tmp;
		for (int i=0; i<size; i++){
		//	tmp = 255 - arr[i];
			arr[i]= 255 - arr[i];//(unsigned char) tmp; 
			if (lmin > arr[i]) lmin = arr[i];
			if (lmax< arr[i]) lmax = arr[i];
		}
	}
	return size;
};


#if 0
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
#endif
//#define __VYVSMALL_SCALE_
void CPChannel::Scale(CPChannel& img2)
{
	if (img2.sz.x*img2.sz.y == 0) return;	
	int k =0;
	int size = sz.x * sz.y;
	int nw = img2.sz.x;
	int nh = img2.sz.y;
	int bsz = sz.x;
	//	if (nw < nh){
	//		nw = img2.sz.y;
	//		nh = img2.sz.x;
	//		bsz = sz.y;
	//	}
	int num1 = sz.x;
	int num = nw;
	if (sz.x/nw < sz.y/nh){
		num1 = sz.y;
		num = nh;
	}
	int tt =0; // avg light
	int kk = num1/num;
	int indi, indj, ind;
	ind = 0;
	//	memset(m_imps.img, m_imps.img2[0]/* (unsigned char) avg*/, size);
#ifdef __VYVSMALL_SCALE_
	if (kk >=2){
		for (int i = 0; i< nh; i++){
			k = nw*i;
			if (ind >= size) break;
			for (int j=0; j<nw; j++){
				tt = 0;
				if (ind >= size) break;
				for (int ii =0; ii<kk; ii++){
					if (ind >= size) break;
					for (int jj =0; jj<kk; jj++)  {
						indi = (i*num1 - 1)/num + kk - ii ;
						indj = (j*num1 - 1)/num + kk - jj;
						ind = indi*bsz + indj;
						if (ind < size){
							tt += arr[ind];
						}
						else{
							_RPTW3(_CRT_WARN, _T("string %5d %5d ind =%5d !\n"), i,j, ind);
							break;
						}
					}
				}
				img2.arr[k] = tt/(kk*kk);
				k++;
			}
		}
		_RPTW0(_CRT_WARN, _T("\n loop end!"));
	}
	else{
		for (int i = 0; i< nh; i++){
			if (ind >=size)
				break;
			k = nw*i;
			for (int j=0; j<nw; j++){
				ind = ((i*num1-1)/num+1)*nw + (j*num1-1)/num+1;
				if (ind >=size){
					_RPTW3(_CRT_WARN, _T("== 2 == string  %5d %5d ind =%5d !\n"), i,j, ind);
				}
				img2.arr[k] = arr[ind];
				k++;
			}
		}
	}
#else
	if (kk > 1){
		int ki = sz.x/img2.sz.y;
		int kj = sz.y/img2.sz.x;
		for (int i=0; i< nh; i++){
			k = nw*i;
			for(int j =0; j< nw; j++){
				tt = 0;

				for (int ii =0; ii<kk; ii++){
					indi = i*ki +ii;
					for(int jj = 0; jj<kk; jj++){
						indj = j*kj + jj;
						ind = indi*bsz + indj;
						img2.arr[k] = tt/(kk*kk);
						if (ind < size){
							tt += arr[ind];
						}//
						else{
							_RPTW3(_CRT_WARN, _T("string %5d %5d ind =%5d !\n"), i,j, ind);
							break;
						}

					}
				}
				img2.arr[k] = tt/(kk*kk);
				k++;
			}
		}
	}
	else{
		for (int i = 0; i< nh; i++){
			if (ind >=size)
				break;
			k = nw*i;
			for (int j=0; j<nw; j++){
				ind = ((i*num1-1)/num+1)*nw + (j*num1-1)/num+1;
				if (ind >=size){
					_RPTW3(_CRT_WARN, _T("== 2 == string  %5d %5d ind =%5d !\n"), i,j, ind);
				}
				img2.arr[k] = arr[ind];
				k++;
			}
		}
	}
#endif
	_RPTW3(_CRT_WARN, _T("== 3 == end  %5d %5d ind =%5d !\n"), size, k, ind);
	::Gauss(img2.arr,img2.arr, img2.sz.y, img2.sz.x);
}
void CPChannel::Gauss(){
	if (sz.x*sz.y > 0){
		::Gauss(arr,arr,sz.y,sz.x);
	}
}

int CPChannel::SelectRect(int x, int y, int width, int height,  CPChannel& img2){
	//this is screen rect coordinats real y must be calculated
	y = max(0, sz.y - y - height); 
	int size = 0;
	if (!( (x<0)||( y<0)||( width <=0 )|| (height<= 0)||(x >= sz.x)||(y >= sz.y)) ){
		if (x +width > sz.x) width = sz.x-width;
		if (y+ height > sz.y) height = sz.y - height;
		if (img2.InitAs(width,height,this) > size){
			unsigned char * sptr = arr+ sz.x*y + x;
			if (img2.arr){
				for (int i = 0; i< height; i++){
					memcpy(&(img2.arr[width*i]), sptr, width);
					sptr += sz.x;
				}//end for
				size = width*height;
			}//end if arr
		}//end if Init
	}
	return size;

}
// histogramm
int CPChannel::FillHistogram(){
	if (!arr) return 0;
	int size = sz.x * sz.y;
	gist.clear();
	for(int i=0; i< size; i++) 
		gist.addvalue(arr[i]);
	return size;
}

#define FULLSCALEALG
int CPChannel::Scale(int num, int num1, CPChannel& img2)
{	// scale num/num1  // num1> num 
#ifdef FULLSCALEALG
	int size = 0;
	unsigned int nw = ((sz.x *num/num1)/4)*4; // new width
	unsigned int nh = sz.y *num/ num1; // new height
	unsigned int k = 0;
	unsigned __int64 tt =0; // avg light
	unsigned int kk = max( num1/num, num/num1);
	int indrs, indks, indre, indke;
	if (nw*nh<= 0) return 0;
#if 0
	if (((nw>>2)<<2)!=nw ){ // we must have even number of row)
		nw =(nw>>2)<<2;
		if (nw > sz.x){
			kk =  nw/sz.x;
			tt = sz.y* sz.x;
			nh = tt/nw;
			num1 = nw;
			num = sz.x;
		}
		else{
			kk = sz.x/nw;
			tt = sz.y*nw;
			nh = tt/sz.x;
			num = nw;
			num1 = sz.x;
		}
		
	}//end check
	tt =0;
#endif
	if (img2.InitAs(nw,nh,this) > size){
		unsigned int tmii, tmji, tmia, tmja, mitr, matr, mitk, matk;
		int tri, tkj;
		indrs =  0;	//
		indre = 0*sz.y;
		indks =  0; 
		indke =  0*sz.x;

		for(unsigned int i= 0; i< nh; i++){
			tmii=i*num1;
			tmia =(i+1)*num1;
			k = i*nw;
			for(unsigned int j=0; j< nw; j++){
				tt = 0;
				tmji = j* num1;
				tmja = (j+1)*num1;
				indrs =(i*num1)/num;// 0;	//
				indre = min((i+1)*num1/num+2, sz.y);//sz.y;/
				indks = (j*num1)/num;//num/num1; 
				indke =  min((j+1)*num1/num+2, sz.x);

				for(unsigned int r= indrs/*0*/; r<indre/*sz.y*/; r++){ // loop for row
					mitr = max(r*num,tmii);
					matr= min((r+1)*num,tmia);
					tri=matr-mitr;
					if (tri>0){
						for (unsigned int kkk =indks/*0*/; kkk <indke /* sz.x*/; kkk ++){
							mitk = max(kkk *num,tmji);
							matk = min((kkk +1)*num,tmja);
							tkj = matk-mitk;
							if(tkj>0)
								tt += tri*arr[r*sz.x+kkk] * tkj;
						}// end loop src column;
					}
				}// end loop src row	
				img2.arr[k/*i*nw + j*/] = (unsigned char) (tt/num1/num1);
				if (img2.arr[k] > img2.lmax) img2.lmax = img2.arr[k];
				if (img2.arr[k] < img2.lmin) img2.lmin = img2.arr[k];
				k++;
			}//loop for result column
		}// loop for result row
		size =k;
	}//end ok
	return size;
#else
	int size = 0;
	int nw = sz.x *num/ num1; // new width
	int nh = sz.y *num/ num1; // new height
	int k = 0;
	int tt =0; // avg light
	int kk = num1/num;
	int indi, indj;
	if (img2.InitAs(nw,nh, this) > size){
		// new img create
		if (kk >2){
			for (int i = 0; i< nh; i++){// loop new row
				k = nw*i;
				for (int j=0; j<nw; j++){// loop new col
					tt = 0;
					for (int ii =0; ii<kk; ii++){//1
						for (int jj =0; jj<kk; jj++)  {//2
							indi = (i*num1 - 1)/num + kk - ii ;
							indj = (j*num1 - 1)/num + kk - jj;
							tt += arr[indi*sz.x + indj ];
						}//2
					}//1
					img2.arr[k] = tt/(kk*kk);
					if (img2.arr[k] > img2.lmax) img2.lmax = img2.arr[k];
					if (img2.arr[k] < img2.lmin) img2.lmin = img2.arr[k];
					k++;
				}//end loop new col
			}//end  loop new row
		}
		else{
			for (int i = 0; i< nh; i++){
				k = nw*i;
				for (int j=0; j<nw; j++){
					img2.arr[k] = arr[((i*num1-1)/num/*+1*/)*sz.x + (j*num1-1)/num+1];
					if (img2.arr[k] > img2.lmax) img2.lmax = img2.arr[k];
					if (img2.arr[k] < img2.lmin) img2.lmin = img2.arr[k];
					k++;
				}
			}
		}
		//img2.Gauss();
		size =k;
	}//end ok
	return size;
#endif

}//end scale num/num1 



int CPChannel::Scale(int nwidth){
	int size = 0;
	if (sz.x*sz.y){
		CPChannel tst;
		Scale(nwidth, sz.x, tst);
		size = this->Init(tst.sz.x, tst.sz.y);
		memcpy(arr, tst.arr, size);
	}
	return size;
}

int CPChannel::ScaleVarios(int nWidth, int nHeight, CPChannel& img2) {

	int size = 0;
	if (arr == nullptr)
		return 0;

	unsigned int num = 1;
	unsigned int num1 = 1;
	unsigned int kw = 1;
	unsigned int kh = 1;
	unsigned int nw = (nWidth / 4) * 4; // new width
	unsigned int nh = nHeight; // new height

	if (sz.x > sz.y) {
		num = nw;
		num1 = sz.x;
	}
	else {
		num = nh;
		num1 = sz.y;
	}

	unsigned int k = 0;
	unsigned __int64 tt = 0; // avg light
	//	unsigned int kk = max(num1 / num, num / num1);
	int indrs, indks, indre, indke;
	if ((nw * nh <= 0) || (sz.x * sz.y <= 0))
		return 0;
	if (sz.x < nw) kw = nw / sz.x;
	else kw = sz.x / nw;
	if (sz.y < nh) kh = nh / sz.y;
	else kh = sz.y / nh;

	if (img2.InitAs(nw, nh, this) > size) {
		unsigned int tmii, tmji, tmia, tmja, mitr, matr, mitk, matk;
		int tri, tkj;
		indrs = 0;	//
		indre = 0 * sz.y;
		indks = 0;
		indke = 0 * sz.x;

		for (unsigned int i = 0; i < nh; i++) {
			tmii = i * num1;
			tmia = (i + 1) * num1;
			k = i * nw;
			for (unsigned int j = 0; j < nw; j++) {
				tt = 0;
				tmji = j * num1;
				tmja = (j + 1) * num1;
				indrs = (i * num1) / num;// 0;	//
				indre = min((i + 1) * num1 / num + 2, sz.y);//sz.y;/
				indks = (j * num1) / num;//num/num1; 
				indke = min((j + 1) * num1 / num + 2, sz.x);

				for (unsigned int r = indrs/*0*/; r < indre/*sz.y*/; r++) { // loop for row
					mitr = max(r * num, tmii);
					matr = min((r + 1) * num, tmia);
					tri = matr - mitr;
					if (tri > 0) {
						for (unsigned int kkk = indks/*0*/; kkk < indke /* sz.x*/; kkk++) {
							mitk = max(kkk * num, tmji);
							matk = min((kkk + 1) * num, tmja);
							tkj = matk - mitk;
							if (tkj > 0)
								tt += tri * arr[r * sz.x + kkk] * tkj;
						}// end loop src column;
					}
				}// end loop src row	
				img2.arr[k/*i*nw + j*/] = (unsigned char)(tt / num1 / num1);
				if (img2.arr[k] > img2.lmax) img2.lmax = img2.arr[k];
				if (img2.arr[k] < img2.lmin) img2.lmin = img2.arr[k];
				k++;
			}//loop for result column
		}// loop for result row
		size = k;
	}//end ok

	return size;

}//end scale varios


int CalcEvclid(int m_uM, int m_uN )
{
	bool IsReverce = false;
	__int64 a, a_, c, b, b_, d, q,r, t;
	a = b_ =0;
	a_ = b = 1;
	int m_Nod = 0;	int m_A = 0; int m_B = 0;
	c = m_uM; d = m_uN;
	if ((m_uM == 0)||(m_uN == 0)) {
		return 1;
	}
	if (d > c){
		IsReverce = true;
		c ^= d;	d = c^d; c ^=d;
	}
	if (d > 0){ 
		do {
			q = c/d; 	r = c%d;
			if (r){
				c = d; d = r; t =a_;
				a_ = a; a = t -q*a; t = b_;
				b_ = b; b = t- q*b;
			}
		}
		while (r !=0);
		m_Nod = (int) d;
		if (IsReverce){
			a ^= b;	b ^= a; a ^= b;
		}
		m_A = (int) a; m_B = (int) b;
		//	m_Nod = a*m_uN + b*m_uN;
	}
	return m_Nod;
}

int CPChannel::DinDiap(){
//	if( lmax-lmin == 255)		return 255;
	int tmp = sz.x*sz.y;
	if (tmp <= 0) return 0;
	lmin = 255; lmax =0;
	for (int i = 0; i <tmp; i++){
		if (arr[i] > lmax) lmax = arr[i];
		if (arr[i]< lmin) lmin = arr[i];
	}
	int k = 0;
	if (lmax > 0){
		for(int i = 0; i< tmp; i++){
			arr[i] = (arr[i] - lmin)*255/lmax;
		}
		lmax = 255;
		lmin = 0;
	}
	return lmax-lmin;
};

/*==========================================================*/
#if 0 // copy from filters
void Contrast(unsigned char * input, unsigned char * output, int rows, int cols, int level)
{// change contrast to level
//	level = level&0x000000FF;
	int szSize = rows*cols;
	int Light = 0;
	for (int i=0; i< szSize; i++){
		Light = Light + input[i];
	}
	Light /= szSize;
	for (int i =0; i< szSize; i++){
		output[i] = (unsigned char) (Light - (Light - input[i])*level/255);
	}
}
#endif
int CPChannel::Gamma(unsigned char * GAMMA){
	if (!arr) return 0;
	int size = sz.x*sz.y;
	for (int s=0; s< size; s++){
		arr[s] = GAMMA[arr[s]];
	}
	return size;
};

int CPChannel::SelectGoodPoints(INT_FeatureList &fl)
{
	int cnt = 0;
	if (arr == 0) 
		return -1; //error image
	if (sz.x*sz.y ==0) 
		return -1; //error image
	int nrows = sz.y;
	INTIMGSTRUCT gimps;
	if (fl.nFeatures <= 0) 
		return 0;
	fl.Clear();
	if (gimps.Initilise(sz.x, sz.y, fl.nFeatures+2))
	{
		_iMaxX = sz.x - _iMinX;
		_iMaxY = sz.y - _iMinY;
		
		IntWindowSelGoodPoints(arr,    			// source bw 8-bits image
			sz.x, sz.y, 				// sizeof image
			gimps.gradx.data, gimps.grady.data,	// temp matrixes for gradient must be ncols*nrows size ????
			gimps.featuremap,			// temp bufer for sorting point ncols*nrows
			gimps.pointlist,					// temp buffer for value must be 3*nrows*ncols
			gimps.tc,			// tracert contect
			gimps.fl,				// feature list
			(_iMaxX+_iMinX)/2, (_iMaxY+_iMinY)/2,//gimps.ncols/2, gimps.nrows/2,						// center of window
			(_iMaxX-_iMinX)/2, (_iMaxY-_iMinY)/2);//gimps.ncols/3, gimps.nrows/3 );				// 1/2 window size
		if (gimps.fl.feature[0].val > 0){			
			for (int i =0; i< gimps.fl.nFeatures; i++){
				if (gimps.fl.feature[i].val > 0){
					fl.feature[cnt] = gimps.fl.feature[i];
					cnt++;
				}
			}
		}
		else{
			if (gimps.fl.feature[0].val < 0) cnt = -1;
		}
		gimps.Release();
	}
	return cnt;
};//end select good points

bool CPChannel::GradientCreate(__int32 * filter, bool IsGauss){
	bool bRet = false;
	int size = sz.x*sz.y;
	int pmin = 0x7fffffff;
	int pmax = 0;
	int out32 = 0;
	if (size <= 0) 
		return false;
	if (arr == 0) return false;
	__int32* data =  (__int32*) malloc(sizeof(__int32)*size);
	if (data != 0){
		memset((unsigned char * ) data, 0x00, sizeof(__int32)*size );
		if (IsGauss){
			if (::GradientCreateGauss(filter, sz.x, sz.y, arr, data)!= -66000)
				bRet = true;
		}
		else{
			if (::GradientCreate(filter, sz.x, sz.y, arr, data)!= -66000)	bRet = true;
		}
		if (bRet)
		{
			for (int i =0; i<size; i++){
				if (pmin > data[i]) pmin = data[i];
				if (pmax < data[i]) pmax = data[i];
			}
			out32 = pmax - pmin;
			if ( out32 > 0){
				for(int i=0; i < size; i++){
					//arr[i] = (unsigned char) ((data[i]-pmin)*255/out32);
					arr[i] = (unsigned char) ((data[i])*255/pmax);
				}
			}
			
		}
		free(data);
	}
	return bRet;
}



// sobel
void IMG_sobel_3x3_8_cn
(
    const unsigned char *restrict in,   /* Input image data   */
    unsigned char       *restrict out,  /* Output image data  */
    short cols, short rows              /* Image dimensions   */
)
{    
    int H, O, V, i;
    int i00, i01, i02;
    int i10,      i12;
    int i20, i21, i22;
    int w = cols;

#ifndef NOASSUME
    _nassert(cols > 3);
    _nassert(cols % 2 == 0);
    _nassert(rows > 3);
    _nassert(cols * (rows - 2) - 2 >= 8);
#endif

    /* -------------------------------------------------------------------- */
    /*  Iterate over entire image as a single, continuous raster line.      */
    /* -------------------------------------------------------------------- */
#ifndef NOASSUME
//    #pragma MUST_ITERATE(8,,2);
#endif
    for (i = 0; i < cols*(rows-2) - 2; i++)
    {
        /* ---------------------------------------------------------------- */
        /*  Read in the required 3x3 region from the input.                 */
        /* ---------------------------------------------------------------- */
        i00=in[i    ]; i01=in[i    +1]; i02=in[i    +2];
        i10=in[i+  w];                  i12=in[i+  w+2];
        i20=in[i+2*w]; i21=in[i+2*w+1]; i22=in[i+2*w+2];

        /* ---------------------------------------------------------------- */
        /*  Apply horizontal and vertical filter masks.  The final filter   */
        /*  output is the sum of the absolute values of these filters.      */
        /* ---------------------------------------------------------------- */

        H = -   i00 - 2*i01 -   i02 +
            +   i20 + 2*i21 +   i22;

        V = -   i00         +   i02 
            - 2*i10         + 2*i12 
            -   i20         +   i22;    

        O = abs(H) + abs(V);

        /* ---------------------------------------------------------------- */
        /*  Clamp to 8-bit range.  The output is always positive due to     */
        /*  the absolute value, so we only need to check for overflow.      */
        /* ---------------------------------------------------------------- */
        if (O > 255) O = 255;

        /* ---------------------------------------------------------------- */
        /*  Store it.                                                       */
        /* ---------------------------------------------------------------- */
        out[i + 1] = O;
    }
}


void testSobel(unsigned char * inptr, unsigned char * outptr, int rows, int column)
{
	IMG_sobel_3x3_8_cn(inptr, outptr, (short)column, (short)rows);

}
int CPChannel::Sobel(){
	int size = 0;
	
	if (arr){
		size = sz.x*sz.y;
		unsigned char * parr = (unsigned char *) malloc(size);		
		lmin = 255;
		lmax =0;
		if (parr){
			testSobel(arr, parr, sz.y, (sz.x/2)*2);
/*			for (int i=0; i<size; i++){
				arr[i] ^= parr[i];
				if (lmax < arr[i])  lmax = arr[i];
				if (lmin > arr[i])  lmin = arr[i];
			
		}

//	*/		memcpy(arr, parr,  (sz.x/2)*2*sz.y);
			free(parr);
		}
	
	}
	return size;
};

unsigned int CPChannel::Integrator(){
	int size = 0;
	unsigned int sum = 0, sw = 0, sh =0;
	if (arr){
		size = sz.x*sz.y;
		if (size > 16000000) return sum;
		unsigned int * parr = (unsigned int *) malloc(size*sizeof(unsigned int));
		if (parr){
			int row = 0;
			memset((unsigned char *) &parr[0], 0, size*sizeof(unsigned int));
			parr[0] = arr[0];
			for (int i =1; i<sz.x; i++){
				parr[i] = parr[i-1]+ arr[i];
				arr[i] = parr[i]/(i+1);
			}
			for (int i = 1; i< sz.y; i++){
				row = i*sz.x;
				parr[row] = parr[row-sz.x]+ arr[row]; 
				arr[row] = parr[row]/(i+1);
				for (int j = 1; j < sz.x; j++){
					parr[j+row] = parr[j-1+row] + parr[j+row-sz.x] - parr[j-1+row-sz.x] + arr[j+row];
					arr[j+row] = parr[j+row]/((i+1)*(j+1));
				}// end column loop
			}//end row loop
			sum = parr[size-1];
			free(parr);
		}

#if 0
		if (parr){
			int row = 0;
			memset((unsigned char *) &parr[0], 0, size*sizeof(unsigned int));
			parr[0] = arr[0];
			for (int i =1; i<sz.x; i++){
				parr[i] = parr[i-1]+ arr[i];
			}
			for (int i = 1; i< sz.y; i++){
				row = i*sz.x;
				parr[row] = parr[row-sz.x]+ arr[row]; 
				for (int j = 1; j < sz.x; j++){
					parr[j+row] = parr[j-1+row] + parr[j+row-sz.x] - parr[j-1+row-sz.x] + arr[j+row];
				}// end column loop
			}//end row loop
			for (int i=0; i<size; i++) arr[i] = parr[i]/size;
			sum = parr[size-1];
			free(parr);
		}
#endif

	}//end check

	return sum;
}

bool CheckFileExist(CString fname, DWORD dwAttrib){
	// dwAtrib is combinatio this flags:
	//		FILE_ATTRIBUTE_DIRECTORY
	//		FILE_ATTRIBUTE_READONLY
	//		0 - any file
	DWORD dwShareMode = 0;
	DWORD dwAccess = GENERIC_WRITE;
	if (dwAttrib&FILE_ATTRIBUTE_DIRECTORY) {
		dwAttrib = dwAttrib|FILE_FLAG_BACKUP_SEMANTICS|FILE_ATTRIBUTE_READONLY ;
		dwAccess = GENERIC_ALL;
	}
	if (dwAttrib&FILE_ATTRIBUTE_READONLY){
		dwShareMode = dwShareMode|FILE_SHARE_READ;
		dwAccess = GENERIC_READ;
	}
	HANDLE hFile = CreateFile(fname, dwAccess,dwShareMode, 0, OPEN_EXISTING, dwAttrib,0);
	if (hFile != INVALID_HANDLE_VALUE){
		CloseHandle(hFile);
		return true;
	}
	dwAccess = GetLastError();
	return false;
}

CString GetDirectory(HWND hWnd, CString sPrompt)
{
	
		CString tar_path,s;//,tmp;
		BOOL IsSelect=FALSE;
		// select working path
		BROWSEINFO bi;
		bi.hwndOwner = hWnd;
		bi.pidlRoot = NULL;
//		LPITEMIDLIST pict;
//		HRESULT ret = SHGetFolderLocation(hWnd, CSIDL_MYPICTURES, 0, 0,&pict);
//		if (ret==S_OK){
//			bi.pidlRoot = pict;
//		}
		bi.pszDisplayName = s.GetBuffer(_MAX_PATHW);
		if (sPrompt.IsEmpty()){
			sPrompt = _T("Select folder for scan"); 
			//tmp.LoadString(IDS_SELUSB);
			//sPrompt.LoadString(IDS_SELUSB);
			bi.lpszTitle = sPrompt.GetBuffer();//_T("Укажите флэш диск");//qwerty
		}
//		else
		bi.lpszTitle = sPrompt.GetBuffer();
		bi.ulFlags = BIF_RETURNONLYFSDIRS| BIF_USENEWUI| BIF_NONEWFOLDERBUTTON|BIF_VALIDATE;//|BIF_DONTGOBELOWDOMAIN|BIF_RETURNFSANCESTORS|BIF_NOTRANSLATETARGETS;//BIF_EDITBOX  ;
		bi.lpfn = NULL;
		bi.lParam = NULL;
      //  EnableWindow(false);
		LPITEMIDLIST item = SHBrowseForFolder( &bi);
//		EnableWindow(TRUE);
		s.ReleaseBuffer();
		IsSelect = SHGetPathFromIDList(item, tar_path.GetBuffer(_MAX_PATHW));
		tar_path.ReleaseBuffer();
		if (IsSelect) {
			//SetCurrentDirectory(LPCTSTR(tar_path));
			return tar_path;
		}//end  selection active
		else return tar_path= _T("");
		
}
