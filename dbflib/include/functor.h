#pragma once

#define ARG_LIST_0 
#define ARG_LIST_1 Param1
#define ARG_LIST_2 ARG_LIST_1,Param2
#define ARG_LIST_3 ARG_LIST_2,Param3
#define ARG_LIST_4 ARG_LIST_3,Param4
#define ARG_LIST_5 ARG_LIST_4,Param5


#define TYPE_LIST_0 
#define TYPE_LIST_1 void* Param1
#define TYPE_LIST_2 TYPE_LIST_1,void* Param2
#define TYPE_LIST_3 TYPE_LIST_2,void* Param3
#define TYPE_LIST_4 TYPE_LIST_3,void* Param4
#define TYPE_LIST_5 TYPE_LIST_4,void* Param5

#define TMPL_LIST_0
#define TMPL_LIST_1 typename P1
#define TMPL_LIST_2 TMPL_LIST_1,typename P2
#define TMPL_LIST_3 TMPL_LIST_2,typename P3
#define TMPL_LIST_4 TMPL_LIST_3,typename P4
#define TMPL_LIST_5 TMPL_LIST_4,typename P5

#define TMPL_LIST_I_0
#define TMPL_LIST_I_1 P1 Param1
#define TMPL_LIST_I_2 TMPL_LIST_I_1,P2 Param2
#define TMPL_LIST_I_3 TMPL_LIST_I_2,P3 Param3
#define TMPL_LIST_I_4 TMPL_LIST_I_3,P4 Param4
#define TMPL_LIST_I_5 TMPL_LIST_I_4,P5 Param5

#include "dom.h"

class Functor {
public:
	void operator delete (void* address)
	{
		TLDOM::PTR<IMemory>	Allocator;	
		TLDomGetMemory((void**)&Allocator);
		if(Allocator != NULL)
			return Allocator->Free(address);
		return ::operator delete (address);
	}
	void operator delete [] (void* address)
	{
		TLDOM::PTR<IMemory>	Allocator;	
		TLDomGetMemory((void**)&Allocator);
		if(Allocator != NULL)
			return Allocator->Free(address);
		return ::operator delete [](address);
	}
	virtual void operator ()(TYPE_LIST_0) = 0;
	virtual void operator ()(TYPE_LIST_1) = 0;
	virtual void operator ()(TYPE_LIST_2) = 0;
	virtual void operator ()(TYPE_LIST_3) = 0;
	virtual void operator ()(TYPE_LIST_4) = 0;
	virtual void operator ()(TYPE_LIST_5) = 0;
};

#define FN_TYPE(args) typedef void (__stdcall OWNER::*Function##args) (TYPE_LIST_##args);\
	Function##args	fnMethod##args;
#define FN_CONSTRUCT(args) \
	template<TMPL_LIST_##args>\
	Function (OWNER* Owner,void (__stdcall OWNER::*Fn) (TMPL_LIST_I_##args)) {\
	fnThis		= Owner;\
	fnMethod##args	= (Function##args)Fn;\
	}
#define FN_CALL(args) \
	virtual void operator ()(TYPE_LIST_##args){\
		(fnThis->*fnMethod##args)(ARG_LIST_##args);\
	}

template<class OWNER>
class Function : public Functor{
	OWNER*		fnThis;
	FN_TYPE(0);
	FN_TYPE(1);
	FN_TYPE(2);
	FN_TYPE(3);
	FN_TYPE(4);
	FN_TYPE(5);
public:
	virtual ~Function(){fnThis = NULL;}
	void* operator new (size_t bytes)
	{
		TLDOM::PTR<IMemory>	Allocator;	DomGetMemory((void**)&Allocator);
		if(Allocator != NULL)
			return Allocator->Alloc((long)bytes);
		return ::operator new (bytes);
	}
	Function(OWNER* Owner,void (__stdcall OWNER::*Fn)())
	{
		fnThis		= Owner;
		fnMethod0	= Fn;
	}
	virtual void operator () (TYPE_LIST_3)
	{
		(fnThis->*fnMethod3)(ARG_LIST_3);
	}
	FN_CALL(0) FN_CALL(1) FN_CALL(2) /*FN_CALL(3)*/ FN_CALL(4) FN_CALL(5)
	FN_CONSTRUCT(1)	FN_CONSTRUCT(2)	FN_CONSTRUCT(3)	FN_CONSTRUCT(4)	FN_CONSTRUCT(5)	
};