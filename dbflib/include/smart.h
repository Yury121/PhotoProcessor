#pragma once

#include "mt.h"
#include <vector>
#include <string>
#include <map>
#include "domvalue.h"

#define tldom		TLDOM

namespace tldom {
	template <typename T> class allocator {
	protected:
		TLDOM::PTR<IMemory>	Allocator;
	public:
		typedef T		value_type;
		typedef size_t	size_type;
		typedef ptrdiff_t difference_type;


		typedef value_type _FARQ *pointer;
		typedef value_type _FARQ& reference;
		typedef const value_type _FARQ *const_pointer;
		typedef const value_type _FARQ& const_reference;

		template<class _Other>
		struct rebind
		{	// convert an allocator<T> to an allocator <_Other>
			typedef allocator<_Other> other;
		};

		pointer address(reference _Val) const
		{	// return address of mutable _Val
			return (&_Val);
		}

		const_pointer address(const_reference _Val) const
		{	// return address of nonmutable _Val
			return (&_Val);
		}

		allocator()
		{	// construct default allocator (do nothing)
			TLDomGetMemory((void**)&Allocator);
		}

		allocator(const allocator<T>&)
		{	// construct by copying (do nothing)
			TLDomGetMemory((void**)&Allocator);
		}

		template<class _Other>
			allocator(const allocator<_Other>&)
		{	// construct from a related allocator (do nothing)
			TLDomGetMemory((void**)&Allocator);
		}

		template<class _Other>
			allocator<T>& operator=(const allocator<_Other>&)
		{	// assign from a related allocator (do nothing)
			return (*this);
		}

		void deallocate(pointer _Ptr, size_type)
		{	// deallocate object at _Ptr, ignore size
			Allocator->Free(_Ptr);
		}

		pointer allocate(size_type _Count)
		{	// allocate array of _Count elements
			return (T _FARQ *)Allocator->Alloc((long)(_Count * sizeof(T)));
		}

		pointer allocate(size_type _Count, const void _FARQ *)
		{	// allocate array of _Count elements, ignore hint
			return (allocate(_Count));
		}

		void construct(pointer _Ptr, const T& _Val)
		{	// construct object at _Ptr with value _Val
			new ((void _FARQ *)_Ptr) T(_Val);
		}

		void destroy(pointer _Ptr)
		{	// destroy object at _Ptr
			(_Ptr)->~T();
		}

		_SIZT max_size() const
		{	// estimate maximum array size
			_SIZT _Count = (_SIZT)(-1) / sizeof (T);
			return (0 < _Count ? _Count : 1);
		}
	};

#define std_alloc(T) std::allocator<T>
#define tldom_alloc(T) tldom::allocator<T>
	template <typename T,typename A = std::allocator<char> > class buffer
	{
		char*	_buffer;
		A		_allocator;
	public:
		buffer(long nBytes)
		{
			_buffer = (char*)_allocator.allocate(nBytes);
			memset(_buffer,0,nBytes);
		}
		~buffer()
		{
			if(_buffer != NULL)
				_allocator.deallocate(_buffer,0);
			_buffer = NULL;
		}
		operator T () const throw()
		{
			return (T)_buffer;
		}
		T operator->() const throw()
		{
			return (T)_buffer;
		}
		template <typename ARG>
			operator ARG () const throw()
		{
			return (ARG)_buffer;
		}
	};
	template <typename T,typename A = tldom::allocator<T> > class blob
	{
	protected:
		T*		_buffer;
		size_t	_size;
		A		_allocator;
		bool	_attached;
	public:
		blob() 
		{
			_buffer = NULL; _size = 0; _attached = false;
		}
		blob(size_t s) 
		{
			_buffer = NULL;_size = 0;
			_attached = false;
			allocate(s);
		}
		blob(void* ptr, size_t s) 
		{
			_buffer = (T*)ptr; _size = s;	_attached = true;
		}
		~blob() 
		{ 
			deallocate(); 
		}
		size_t sizetype()
		{
			return _size;
		}
		size_t size() 
		{
			return _size*sizeof(T);
		}
		T& operator [] (size_t i)
		{
			return _buffer[i];
		}
		T* ptr()
		{
			return _buffer;
		}
		void attach(void* ptr,size_t s)
		{
			deallocate(); 
			_buffer = (T*)ptr; _size = s;	_attached = true;
		}
		void* detach()
		{
			void* ptr = (void*)_buffer;
			deallocate();
			return ptr;
		}
		template <typename _T> _T typped ()
		{
			return reinterpret_cast<_T>(_buffer);
		}
		template <typename _T,typename _A>
		void copy(blob<_T,_A>& b)
		{
			allocate(b.size(),b.ptr());
		}
		void move(blob<T,A>& b)
		{
			allocate(b.size(),b.ptr());
			b.deallocate();
		}
		void allocate(size_t s,const void* ptr = NULL)
		{
			deallocate();
			if(s == 0)
				return;

			_buffer = _allocator.allocate(s);
			_size = s;
			if(ptr == NULL)
				memset(_buffer,0,_size*sizeof(T));
			else
				memcpy(_buffer,ptr,_size*sizeof(T));
		}
		void deallocate()
		{
			if(_buffer != NULL) {
				if(_attached == false) {
					memset(_buffer,0,_size*sizeof(T));
					_allocator.deallocate(_buffer,_size); 
				}
			}
			_buffer = NULL; 
			_size = 0;
			_attached = false;
		}
		void resize(size_t s) 
		{
			T*		_old = _buffer;
			size_t	_oldsize = _size;

			if(_attached == true)
				return;

			if(_buffer != NULL) { _buffer = NULL;_size = 0;	}
			
			allocate(s);

			if(_old != NULL) 
			{ 
				memcpy(_buffer,_old,(min(_oldsize,s))*sizeof(T)); 
				_allocator.deallocate(_old,size()); 
			}
		}
		void zero()
		{
			if(_buffer != NULL) {
				memset(_buffer,0,_size*sizeof(T));
			}
		}
		void clear()
		{
			deallocate();
		}
	};
	template <typename T> class singleton 
	{
		static void destruct(void)
		{
			AutoMutex	guard(sLocker);
			if(singleton<T>::TypeInstance != NULL){
				singleton<T>::TypeInstance->Shutdown();
				delete (T*)(singleton<T>::TypeInstance);
				singleton<T>::TypeInstance = NULL;
			}
		}
		static T*	TypeInstance;
		static mutex	sLocker;
	public:
		//		Singleton():sLocker
		static T*	instance()
		{
			if(TypeInstance == NULL){

				AutoMutex	guard(sLocker);

				if(TypeInstance == NULL){
					atexit(destruct);
					TypeInstance = new T();
					TypeInstance->Startup();
				}
			}
			return (T*)TypeInstance;
		}
		virtual void Startup() {;}
		virtual void Shutdown() {;}
	};

	template <long SIZE>
	struct static_buffer 
	{
		unsigned char	Buffer[SIZE];

		static_buffer() {memset(Buffer,0,SIZE);}
		~static_buffer() {memset(Buffer,0,SIZE);}

		template <typename TYPE>
			operator TYPE () {return reinterpret_cast<TYPE>(Buffer);}
			UCHAR operator [] (int i) {return Buffer[i];}
	};
	template <class T>	class vector : public std::vector<T,tldom::allocator<T> > {};
//	template <class K,class T>	class map : public std::map<K,T,std::less<K>,dom::allocator<std::pair<const K,T> > > {};
	class memory : public tldom::blob<byte> {
	protected:
		size_t	m_dwCurrentPointer;
		inline void  needspace(size_t dwNeed)
		{
			if((m_dwCurrentPointer + dwNeed) > _size)
				resize(m_dwCurrentPointer + dwNeed);
		}
	public:
		enum {begin,end,current};
		inline size_t seek(size_t pos,size_t Method = memory::begin)
		{
			if(Method == memory::begin)
				m_dwCurrentPointer = pos;
			if(Method == memory::end) 
				m_dwCurrentPointer = _size + pos;
			if(Method == memory::current) 
				m_dwCurrentPointer += pos;
			return m_dwCurrentPointer;
		}
		memory() : tldom::blob<byte>(){m_dwCurrentPointer = 0;}
		memory(size_t s) : tldom::blob<byte>(s){m_dwCurrentPointer = 0;}
		memory(void* ptr, size_t s) : tldom::blob<byte>(ptr,s){m_dwCurrentPointer = 0;}
		memory::~memory()
		{
			m_dwCurrentPointer = 0;
		}
	public:
		inline memory& operator << (const std::string& str)
		{
			size_t stLength = str.length();
			needspace(stLength + sizeof(size_t));

			memcpy(_buffer + m_dwCurrentPointer,&stLength,sizeof(size_t));

			m_dwCurrentPointer += sizeof(size_t);

			memcpy(_buffer + m_dwCurrentPointer,str.c_str(),stLength);

			m_dwCurrentPointer += stLength;

			return *this;
		}
		inline memory& operator << (const std::wstring& str)
		{
			size_t stLength = str.length();
			needspace((stLength*sizeof(wchar_t)) + sizeof(size_t));

			memcpy(_buffer + m_dwCurrentPointer,&stLength,sizeof(size_t));

			m_dwCurrentPointer += sizeof(size_t);

			memcpy(_buffer + m_dwCurrentPointer,str.c_str(),(stLength*sizeof(wchar_t)));

			m_dwCurrentPointer += (stLength*sizeof(wchar_t));

			return *this;
		}
		inline memory& operator << (const char* strText)
		{
			size_t dwLength = strlen(strText) + 1;

			needspace(dwLength + sizeof(size_t));

			memcpy(_buffer + m_dwCurrentPointer,&dwLength,sizeof(size_t));

			m_dwCurrentPointer += sizeof(size_t);

			memcpy(_buffer + m_dwCurrentPointer,strText,dwLength);

			m_dwCurrentPointer += dwLength;

			return *this;
		}
		inline memory& operator << (const wchar_t* strText)
		{
			size_t dwLength = (wcslen(strText) + 1)/* * sizeof(wchar_t)*/;

			needspace(dwLength * sizeof(wchar_t) + sizeof(size_t));

			memcpy(_buffer + m_dwCurrentPointer,&dwLength,sizeof(size_t));

			m_dwCurrentPointer += sizeof(size_t);

			memcpy(_buffer + m_dwCurrentPointer,strText,dwLength * sizeof(wchar_t));

			m_dwCurrentPointer += dwLength * sizeof(wchar_t);

			return *this;
		}


		template <typename TYPE> memory& operator << (TYPE value)
		{
			needspace(sizeof(TYPE));
			memcpy(_buffer + m_dwCurrentPointer,&value,sizeof(TYPE));
			m_dwCurrentPointer += sizeof(TYPE);
			return *this;
		}

		inline memory& operator >> (std::string& value)
		{
			size_t stLength;

			if((m_dwCurrentPointer + sizeof(size_t)) > _size)
				return *this;

			memcpy(&stLength,_buffer + m_dwCurrentPointer,sizeof(size_t));

			if((m_dwCurrentPointer + stLength) > _size)
				return *this;

			value.resize(stLength);

			memcpy((void*)value.c_str(),_buffer + m_dwCurrentPointer + sizeof(size_t),stLength);

			m_dwCurrentPointer += sizeof(size_t) + stLength;

			return *this;
		}
		inline memory& operator >> (std::wstring& value) 
		{
			size_t stLength;

			if((m_dwCurrentPointer + sizeof(size_t)) > _size)
				return *this;

			memcpy(&stLength,_buffer + m_dwCurrentPointer,sizeof(size_t));

			if((m_dwCurrentPointer + (stLength*sizeof(wchar_t))) > _size)
				return *this;

			value.resize(stLength);

			memcpy((void*)value.c_str(),_buffer + m_dwCurrentPointer + sizeof(size_t),(stLength*sizeof(wchar_t)));

			m_dwCurrentPointer += sizeof(size_t) + (stLength*sizeof(wchar_t));

			return *this;
		}

		inline memory& operator << (TLDOM::value& val)
		{
			(*this) << (unsigned int)val.Type;

			if(val.type() == vt_lptstr){
				(*this) << (const char*)val.the.lptstr;
			}
			else if(val.type() == vt_lpwstr){
				(*this) << (const wchar_t*)val.the.lpwstr;
			}
			else if(val.type() == vt_buffer){
				(*this) << val.the.buffer.length;
				put(val.the.buffer.data,val.the.buffer.length);
			}
			else {
				put(&val.the,sizeof(val.the));
			}
			return *this;
		}
		inline memory& operator >> (value& val)
		{
			unsigned int _Type;	(*this) >> _Type;

			if(_Type == vt_lptstr){
				std::string	_data;
				(*this) >> _data;
				val = tldom::value((char*)_data.c_str());
			}
			else if(_Type == vt_lpwstr){
				std::wstring	_data;
				(*this) >> _data;
				val = tldom::value((wchar_t*)_data.c_str());
			}
			else if(_Type == vt_buffer){
				unsigned long size;	(*this) >> size;

				val = tldom::value(cur_ptr(),size);
			}
			else {
				get(&val.the,sizeof(val.the));
			}
			return *this;
		}

		inline memory& operator >> (char* value)
		{
			size_t	dwSize = 0;

			if((m_dwCurrentPointer + sizeof(size_t)) > _size)
				return *this;

			memcpy(&dwSize,_buffer + m_dwCurrentPointer,sizeof(size_t));

			if((m_dwCurrentPointer + dwSize) > _size)
				return *this;

			memcpy(value,_buffer + m_dwCurrentPointer + sizeof(size_t),dwSize);

			m_dwCurrentPointer += sizeof(size_t) + dwSize;

			return *this;
		}

		template <typename TYPE> memory& operator >> (TYPE& value)
		{
			if((m_dwCurrentPointer + sizeof(TYPE)) > _size)
				return *this;

			memcpy(&value,_buffer + m_dwCurrentPointer,sizeof(TYPE));

			m_dwCurrentPointer += sizeof(TYPE);

			return *this;
		}

		inline void*  cur_ptr()
		{
			return (void*)(_buffer+m_dwCurrentPointer);
		}

		inline memory&  put(void* pBuffer, size_t dwLength)
		{
			needspace(dwLength);
			memcpy(_buffer + m_dwCurrentPointer,pBuffer,dwLength);
			m_dwCurrentPointer += dwLength;	
			return *this;
		}

		inline memory&  get(void* pBuffer, size_t dwLength)
		{
			if((m_dwCurrentPointer + dwLength) > _size)
				return *this;

			memcpy(pBuffer,_buffer + m_dwCurrentPointer,dwLength);

			m_dwCurrentPointer += dwLength;

			return *this;
		}
	};
	class chars : public std::basic_string<char, std::char_traits<char>, tldom::allocator<char> > {
	public:
		chars (const chars& p) { assign(p); }
		chars () { ; }
		chars (const char* p) { assign(p);}
		void format(const char* format,...)
		{
			va_list args;
			int len;
			char * buffer;
			try
			{
				va_start( args, format );
				len = _vscprintf( format, args ) + 1;
				buffer = new char [len*sizeof(char)];
				vsprintf( buffer, format, args );
				assign(buffer);
				delete[] buffer;
			}
			catch (...)
			{
				;
			}
		}
	};


}

#define SINGLETON(CLASS)	CLASS* tldom::singleton<CLASS>::TypeInstance = NULL; mutex dom::singleton<CLASS>::sLocker;
