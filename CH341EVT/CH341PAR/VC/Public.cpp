// Public.cpp: implementation of the CPublic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ch341par.h"
#include "Public.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ULONG CPublic::mIndex=0;
BOOL CPublic::m_open=FALSE;
HWND CPublic::mainhwnd=NULL;
CPublic::CPublic()
{

}

CPublic::~CPublic()
{

}



UCHAR CPublic::mCharToBcd( UCHAR iChar )
{
	UCHAR	mBCD;
	if ( iChar >= '0' && iChar <= '9' ) mBCD = iChar -'0';
	else if ( iChar >= 'A' && iChar <= 'F' ) mBCD = iChar - 'A' + 0x0a;
	else if ( iChar >= 'a' && iChar <= 'f' ) mBCD = iChar - 'a' + 0x0a;
	else mBCD = 0x00;
	return( mBCD );
}

ULONG CPublic::mStrToBcd( CString str )
{
	char mlen,i=0;
	UCHAR iChar=0,Char[9]="";
	UINT mBCD=0,de=1;
    mlen=strlen(str);
    memcpy(Char,str,mlen);
	for(i=mlen-1;i>=0;i--)
	{	iChar=Char[i];
		if ( iChar >= '0' && iChar <= '9' )
			mBCD = mBCD+(iChar -'0')*de;
		else if ( iChar >= 'A' && iChar <= 'F' ) 
			mBCD =mBCD+ (iChar - 'A' + 0x0a)*de;
		else if ( iChar >= 'a' && iChar <= 'f' )
			mBCD =mBCD+ (iChar - 'a' + 0x0a)*de;
		else return(0);
		de*=16;
	}
	return(mBCD);
}

PUCHAR CPublic::mStrtoVal( PUCHAR str, ULONG strlen )
{
	ULONG i,j;
	ULONG len;
	UCHAR strRev[mMAX_BUFFER_LENGTH];
	if( strlen % 2 != 0 )
	{  	str[strlen] = 0;
	strlen += 1;
	}
	len = strlen / 2;
	for(i=0,j=0;j<len;i++,j++)
	{
		strRev[j] = (UCHAR)((mCharToBcd(str[i])<<4) + mCharToBcd(str[i+1]));
		i++;
	}
	strRev[j]='\0';
	memcpy(str,strRev,len);
	return str;
}
