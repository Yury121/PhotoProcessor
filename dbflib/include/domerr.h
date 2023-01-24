#ifndef DOM_ERROR_CODES
#define DOM_ERROR_CODES

typedef long	RESULT;
#define MAKE_RESULT(sev,fac,code) \
	((RESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )

#define E_OK					MAKE_HRESULT(0,0,0)
#define E_FAILED				MAKE_HRESULT(0,0,1)

//	Dynamic Object Model internal Errors
#define	E_INVALID_INTERFACE		MAKE_RESULT(1,0,1)
#define E_NOT_SUPPORTED			MAKE_RESULT(1,0,2)
#define E_NOT_IMPL				MAKE_RESULT(1,0,3)
#define E_MT_FAILED				MAKE_RESULT(1,0,4)

#endif //DOM_ERROR_CODES