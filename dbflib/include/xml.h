#ifndef XML_INTERFACE
#define XML_INTERFACE

#include "string"
#include "map"
#include "smart.h"

using namespace std;
using namespace dom;

namespace xml {
	enum {dec=10,hex=16,bin=2,base64=6,text};
	typedef dom::blob<byte>	blob;
	template<class _Kty,class _Ty>
	class XmlMultiMap : public std::multimap<_Kty,_Ty,std::less<_Kty>, dom::allocator<std::pair<const _Kty, _Ty> > > {};
	template <class K,class T>	
	class XmlMap : public std::map<K,T,std::less<K>,dom::allocator<std::pair<const K,T> > > {};

	void base64_encode(xml::blob& b,std::string& s);
	void base64_decode(std::string& s,xml::blob& b);

	class value 
	{
	protected:
		friend class document;
		friend class node;
		std::string _value;
	public:
		value(){_value.clear();}
		value(std::string& v);

		void operator = (const xml::value& v);

		void get(int& v,int radix = xml::dec);
		void get(unsigned int& v,int radix = xml::dec);
		void get(char& v,int radix = xml::dec);
		void get(unsigned char& v,int radix = xml::dec);
		void get(long& v,int radix = xml::dec);
		void get(unsigned long& v,int radix = xml::dec);
		void get(bool& v,int radix = xml::text);
		void get(__int64& v,int radix = xml::dec);
		void get(unsigned __int64& v,int radix = xml::dec);
		void get(short& v,int radix = xml::dec);
		void get(unsigned short& v,int radix = xml::dec);
		void get(float& v,int radix = xml::dec);
		void get(double& v,int radix = xml::dec);
		void get(xml::blob& v);
		void get(std::string& v);
		void get(std::wstring& v);
		void get(char* v);
		void get(wchar_t* v);

		void set(const int v,int radix = xml::dec);
		void set(const unsigned int v,int radix = xml::dec);
		void set(const char v,int radix = xml::dec);
		void set(const unsigned char v,int radix = xml::dec);
		void set(const long v,int radix = xml::dec);
		void set(const unsigned long v,int radix = xml::dec);
		void set(const bool v,int radix = xml::text);
		void set(const __int64 v,int radix = xml::dec);
		void set(const unsigned __int64 v,int radix = xml::dec);
		void set(const short v,int radix = xml::dec);
		void set(const unsigned short v,int radix = xml::dec);
		void set(const float v,int radix = xml::dec);
		void set(const double v,int radix = xml::dec);
		void set(const xml::blob& v);
		void set(const std::string& v);
		void set(const std::wstring& v);
		void set(const char* v);
		void set(const wchar_t* v);
	};
	template <typename T>
	class type 
	{
		T _value;
	public:
		type() {;}
		type(const xml::value& v)
		{
			_value = v.get((T&)_value);
		}
		type(const T v)
		{
			_value = v;
		}
		void operator = (const xml::value& v)
		{
			xml::value& vv = (xml::value&)v;
			vv.get((T&)_value);
		}
		void operator = (const T v)
		{
			xml::value& vv = (xml::value&)v;
			_value = vv;
		}
		operator T& () {return (T&)_value;}
		operator xml::value () {xml::value v; v.set((const T&)_value); return v;}
		operator T*() throw()	{return &_value;}
		T& operator*() throw(){return _value;}
		T* operator->() throw(){return &_value;	}
	};
	class node 
	{
	protected:
		friend class document;
		enum {Error,Element,Attribute,Text,Comment};
		std::string					nodeName;
		std::string					nodeValue;

		XmlMultiMap<std::string,node*>		nodeChilds;
		XmlMap<std::string,std::string>		nodeAttributes;
		int						nodeType;
		node*					nodeParent;
		XmlMultiMap<std::string,node*>::iterator					
			nodeIt;
		void store(void* IMsXml,int lvl);
		void store(std::string& szXml,int lvl);
		void load(void* IMsXml);
		node();
		~node();
	public:
		node* parent();
		node* find(const char* szName);
		bool erase(const char* szName);
		size_t count();
		node* first();
		node* next();
		void clear();
		std::string& name();
		
		xml::value get();
		void set(const xml::value& v);
		
		xml::value geta(const char* szAttr);
		void seta(const char* szAttr,const xml::value& a);

		node* add(const char* szName,const char* szValue = NULL);
	};
	class document 
	{
	protected:
		xml::node	nodeRoot;
	public:
		document();
		~document();
		void clear();
		bool load(const char* szFileName);
		bool store(const char* szFileName);
		bool load(const std::string& szXml);
		bool store(std::string& szXml);
		node* root();
		node* get(const char* szNodePath);
		node* add(const char* szNodePath);
	};
};


#endif

