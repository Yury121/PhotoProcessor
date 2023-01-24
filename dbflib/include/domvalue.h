#ifndef TLDOM_VALUE
#define TLDOM_VALUE

#include <time.h>
#include <comdef.h>

#define dom		TLDOM

namespace TLDOM
{ 
	enum _VARENUM
	{	vt_null,
		vt_lptstr,
		vt_lpwstr,
		vt_int8,
		vt_int16,
		vt_int32,
		vt_int64,
		vt_boolean,
		vt_float,
		vt_double,
		vt_buffer,
		vt_date_utc,
		vt_date_local,
		vt_uiid,
		vt_ptr,
	};

	typedef unsigned short _VARTYPE;

	typedef struct tagVALUE
	{
		_VARTYPE	Type;

		union {
			char*			lptstr;		// ANSI строка с нулевым окончанием
			unsigned short*	lpwstr;		// Unicode строка с нулевым окончанием
			void*			ptr;
			__int8			i8;
			__int16			i16;
			__int32			i32;
			__int64			i64;
			bool			b;
			float			f;
			double			d;
			struct {
				int		length;
				void*	data;
			}				buffer;
			struct {
				unsigned __int64  day		:5; // день 1-31
				unsigned __int64  wday		:3;	// день недели 0-6 (0-воскресенье)
				unsigned __int64  month		:4;	// месяц
				unsigned __int64  year		:25;// год от 0-2^25
				unsigned __int64  hour		:5;	 // часы 0-23
				unsigned __int64  minute	:6;  // минуты 0-59
				unsigned __int64  second	:6;  // секунды 0-59
				unsigned __int64  milisecond:10; // милисекунды 0-999
			}		date;
			struct {
				unsigned __int32	id1;
				unsigned __int32	id2;
				unsigned __int32	id3;
			}		uiid;
		}the;
	}VALUE, *LPVALUE;

	class value : public VALUE
	{

	public:

		value::value()
		{
			Type = vt_null;
			clear();
		}

		value::~value()
		{
			clear();
		}

		value::value (const value& v)
		{
			Type = vt_null;
			clear();

			TLDOM::PTR<IMemory> _mem;
			TLDomGetMemory((void**)&_mem);

			if(v.Type == vt_lptstr){
				the.lptstr = (char*)_mem->Alloc((long)(strlen(v.the.lptstr) + 1));
				if(the.lptstr != NULL){
					strcpy(the.lptstr,v.the.lptstr);
					Type = vt_lptstr;
				}
			}
			else if(v.Type == vt_lpwstr){
				the.lpwstr = (unsigned short*)_mem->Alloc((long)((wcslen((wchar_t*)v.the.lpwstr) + 1)*2));
				if(the.lpwstr != NULL){
					wcscpy((wchar_t*)the.lpwstr,(wchar_t*)v.the.lpwstr);
					Type = vt_lpwstr;
				}
			}
			else if(v.Type == vt_buffer){
				the.buffer.data = _mem->Alloc(v.the.buffer.length);
				if(the.buffer.data != NULL){
					memcpy(the.buffer.data,v.the.buffer.data,v.the.buffer.length);
					the.buffer.length = v.the.buffer.length;
					Type = vt_buffer;
				}
			}
			else {
				memcpy(&the,&(v.the),sizeof(the));
				Type = v.Type;
			}
		}

		value::value (const _variant_t& vv)
		{
			Type = vt_null;
			clear();

			TLDOM::PTR<IMemory> _mem;
			TLDomGetMemory((void**)&_mem);

			switch (vv.vt)
			{
			case VT_EMPTY:
			case VT_NULL:
				break;
			case VT_BOOL:
				the.b = (vv.boolVal?true:false);
				Type = vt_boolean;
			break;
			case VT_I1:
			case VT_UI1:
				the.i8 = vv.cVal;
				Type = vt_int8;
				break;
			case VT_I2:
			case VT_UI2:
				the.i16 = vv.iVal;
				Type = vt_int16;
				break;
			case VT_I4:
			case VT_UI4:
			case VT_INT:
			case VT_UINT:
			case VT_HRESULT:
				the.i32 = vv.lVal;
				Type = vt_int32;
				break;
			case VT_R4:
				the.f = vv.fltVal;
				Type = vt_float;
				break;
			case VT_R8:
				the.d = vv.dblVal;
				Type = vt_double;
				break;
			case VT_DATE:
			case VT_FILETIME:
			case VT_DECIMAL:
			case VT_I8:
			case VT_UI8:
				the.i64 = vv.llVal;
				Type = vt_int64;
			break;
			case VT_BSTR:
			case VT_LPWSTR:
				{
					the.lpwstr = (unsigned short*)_mem->Alloc((long)((wcslen((wchar_t*)vv.bstrVal) + 1)*2));
					if(the.lpwstr != NULL){
						wcscpy((wchar_t*)the.lpwstr,(wchar_t*)vv.bstrVal);
						Type = vt_lpwstr;
					}
				}
				break;
			case VT_LPSTR:
				{
					the.lptstr = (char*)_mem->Alloc((long)(strlen(vv.pcVal) + 1));
					if(the.lptstr != NULL){
						strcpy(the.lptstr,vv.pcVal);
						Type = vt_lptstr;
					}
				}
				break;

			case VT_BLOB:
			case VT_VOID:
			case VT_PTR:
			case VT_INT_PTR:
			case VT_UINT_PTR:
				the.ptr = vv.byref;
				Type = vt_ptr;
			default:
				break;
			}
		}

		value::value (char* lptstr)
		{
			Type = vt_null;
			clear();

			TLDOM::PTR<IMemory> _mem;
			TLDomGetMemory((void**)&_mem);

			the.lptstr = (char*)_mem->Alloc((long)(strlen(lptstr) + 1));
			if(the.lptstr != NULL){
				strcpy(the.lptstr, lptstr);
				Type = vt_lptstr;
			}
		}

		operator char*() const
		{
			return (this->the.lptstr?this->the.lptstr:"");
		}

		value::value (wchar_t* lpwstr)
		{
			Type = vt_null;
			clear();

			TLDOM::PTR<IMemory> _mem;
			TLDomGetMemory((void**)&_mem);
			the.lpwstr = (unsigned short*)_mem->Alloc((long)((wcslen((wchar_t*)lpwstr) + 1)*2));
			if(the.lpwstr != NULL){
				wcscpy((wchar_t*)the.lpwstr,(wchar_t*)lpwstr);
				Type = vt_lpwstr;
			}
		}

		value& operator=(const value& v)
		{
			Type = vt_null;
			clear();

			TLDOM::PTR<IMemory> _mem;
			TLDomGetMemory((void**)&_mem);

			if(v.Type == vt_lptstr){
				the.lptstr = (char*)_mem->Alloc((long)(strlen(v.the.lptstr) + 1));
				if(the.lptstr != NULL){
					strcpy(the.lptstr,v.the.lptstr);
					Type = vt_lptstr;
				}
			}
			else if(v.Type == vt_lpwstr){
				the.lpwstr = (unsigned short*)_mem->Alloc((long)((wcslen((wchar_t*)v.the.lpwstr) + 1)*2));
				if(the.lpwstr != NULL){
					wcscpy((wchar_t*)the.lpwstr,(wchar_t*)v.the.lpwstr);
					Type = vt_lpwstr;
				}
			}
			else if(v.Type == vt_buffer){
				the.buffer.data = _mem->Alloc(v.the.buffer.length);
				if(the.buffer.data != NULL){
					memcpy(the.buffer.data,v.the.buffer.data,v.the.buffer.length);
					the.buffer.length = v.the.buffer.length;
					Type = vt_buffer;
				}
			}
			else {
				memcpy(&the,&(v.the),sizeof(the));
				Type = v.Type;
			}

			return (*this);
		}

		operator unsigned short*() const
		{
			return this->the.lpwstr;
		}

		value::value(bool value)
		{
			Type = vt_null;
			clear();
			the.b = value; Type = vt_boolean;
		}

		operator bool() const
		{
			return this->the.b;
		}

		value::value(void* value)
		{
			Type = vt_null;
			clear();
			the.ptr = value; Type = vt_ptr;
		}

		operator void*() const
		{
			return this->the.ptr;
		}

		value::value (__int32 value)
		{
			Type = vt_null;
			clear();
			the.i32 = value; Type = vt_int32;
		}

		operator __int32() const
		{
			return this->the.i32;
		}

		value::value (short value)
		{
			Type = vt_null;
			clear();
			the.i16 = value; Type = vt_int16;
		}

		operator short() const
		{
			return this->the.i16;
		}

		value::value (long value)
		{
			Type = vt_null;
			clear();
			the.i32 = value; Type = vt_int32;
		}

		operator long() const
		{
			return this->the.i32;
		}

		value::value (__int64 value)
		{
			Type = vt_null;
			clear();
			the.i64 = value; Type = vt_int64;
		}

		operator __int64() const
		{
			return this->the.i64;
		}

		value::value (float value)
		{
			Type = vt_null;
			clear();
			the.f = value; Type = vt_float;
		}

		operator float() const
		{
			return this->the.f;
		}

		value::value (double value)
		{
			Type = vt_null;
			clear();
			the.d = value; Type = vt_double;
		}

		operator double() const
		{
			return this->the.d;
		}

		value::value (void* data,int length)
		{
			Type = vt_null;
			clear();

			TLDOM::PTR<IMemory> _mem;
			TLDomGetMemory((void**)&_mem);
			the.buffer.data = _mem->Alloc(length);
			if(the.buffer.data != NULL){
				memcpy(the.buffer.data,data,length);
				the.buffer.length = length;
				Type = vt_buffer;
			}
		}

		value::value(long id1,long id2,long id3)
		{
			Type = vt_null;
			clear();
			the.uiid.id1 = id1;
			the.uiid.id2 = id2;
			the.uiid.id3 = id3;
			Type = vt_uiid;
		}

		value::value (short date_type,
			short day,
			short month,
			long year,
			short hour,
			short minute,
			short second,
			short msecond,
			short dayofweek)
		{
			Type = vt_null;
			clear();

			the.date.day			= day;
			the.date.wday			= dayofweek;
			the.date.month		= month;
			the.date.year			= year;
			the.date.hour			= hour;
			the.date.minute		= minute;
			the.date.second		= second;
			the.date.milisecond	= msecond;
			Type = date_type;
		}

		void value::currenttime(short date_type)
		{
			tm			*t;
			__time64_t	long_time;

			_time64( &long_time );
			t = (date_type == vt_date_utc)?_gmtime64(&long_time):_localtime64( &long_time ); 
			if(t != NULL){
				the.date.day			= t->tm_mday;
				the.date.wday			= t->tm_wday ;
				the.date.month		= t->tm_mon + 1;
				the.date.year			= t->tm_year + 1900;
				the.date.hour			= t->tm_hour;
				the.date.minute		= t->tm_min;
				the.date.second		= t->tm_sec;
				the.date.milisecond	= 0;
				Type = date_type;
			}
		}

			bool value::empty()
		{
			return (Type == vt_null);
		}

		void value::clear()
		{
			TLDOM::PTR<IMemory> _mem;
			TLDomGetMemory((void**)&_mem);

			if((Type == vt_lptstr || Type == vt_lpwstr) && the.lptstr != NULL && the.lpwstr != NULL)
				_mem->Free(the.lptstr);
			if(Type == vt_buffer && the.buffer.data != NULL)
				_mem->Free(the.buffer.data);
			memset(&the,0,sizeof(the));
			Type = vt_null;
		}

		short value::type()
		{
			return Type;
		}
	};
}
#endif // TLDOM_VALUE