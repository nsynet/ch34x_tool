// Public.h: interface for the CPublic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUBLIC_H__3F1ECEAF_8FBF_4A6F_8A9C_A8E43AD0F9AA__INCLUDED_)
#define AFX_PUBLIC_H__3F1ECEAF_8FBF_4A6F_8A9C_A8E43AD0F9AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Total.h"

class CPublic  
{
public:
	CPublic();
	virtual ~CPublic();
public:
	static ULONG mIndex;
	static BOOL m_open;
	static HWND mainhwnd;
	static UCHAR mCharToBcd(UCHAR iChar);
	static ULONG mStrToBcd(CString str);
	static PUCHAR mStrtoVal(PUCHAR str, ULONG strlen);

};


#endif // !defined(AFX_PUBLIC_H__3F1ECEAF_8FBF_4A6F_8A9C_A8E43AD0F9AA__INCLUDED_)
