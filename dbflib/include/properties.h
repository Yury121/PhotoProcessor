#pragma once

#include "dom.h"

typedef unsigned long	PropId;
typedef void*			PropVal;

#define UIID_IProperties		UIID(0xD95E9892,0x74E44b7a,0x93AF29E5)

// Интерфейс получения свойств объекта

struct IProperties : public virtual IBase {
	TLDOM_DECLARE_UIID(UIID_IProperties)

	virtual bool __stdcall Get(const PropId& id,PropVal& val) = 0;
	virtual bool __stdcall Set(const PropId& id,const PropVal& val) = 0;
};