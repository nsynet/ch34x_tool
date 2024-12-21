// MemPage.cpp : implementation file
//

#include "stdafx.h"
#include "Total.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMemPage property page

IMPLEMENT_DYNCREATE(CMemPage, CPropertyPage)

CMemPage::CMemPage() : CPropertyPage(CMemPage::IDD)
{
	//{{AFX_DATA_INIT(CMemPage)
	m_memdata0 = _T("");
	m_memdata1 = _T("");
	m_memlen0 = _T("");
	m_memlen1 = _T("");
	//}}AFX_DATA_INIT
}

CMemPage::~CMemPage()
{
}

void CMemPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMemPage)
	DDX_Text(pDX, IDC_EDIT_MEMDATA0, m_memdata0);
	DDX_Text(pDX, IDC_EDIT_MEMDATA1, m_memdata1);
	DDX_Text(pDX, IDC_EDIT_MEMLEN0, m_memlen0);
	DDX_Text(pDX, IDC_EDIT_MEMLEN1, m_memlen1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMemPage, CPropertyPage)
	//{{AFX_MSG_MAP(CMemPage)
	ON_BN_CLICKED(IDC_BUTTON_MEMREAD0, OnButtonMemread0)
	ON_BN_CLICKED(IDC_BUTTON_MEMWRITE0, OnButtonMemwrite0)
	ON_BN_CLICKED(IDC_BUTTON_MEMREAD1, OnButtonMemread1)
	ON_BN_CLICKED(IDC_BUTTON_MEMWRITE1, OnButtonMemwrite1)
	ON_BN_CLICKED(IDC_BTNMEMADD0CL, OnBtnmemadd0cl)
	ON_BN_CLICKED(IDC_BTNI2CMEMADD1CL, OnBtni2cmemadd1cl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMemPage message handlers

void CMemPage::OnButtonMemread0()   //MEM方式读地址0
{
	UCHAR mBuf[mMAX_BUFFER_LENGTH]="";
	ULONG mLen=0;
	UpdateData(TRUE);
	mLen = CPublic::mStrToBcd(m_memlen0);
	if(mLen <= 0)
	{
		MessageBox("请输入读数据长度","CH341",MB_OK|MB_ICONSTOP);
		return;
	}
	if(mLen>=0x1000)
	{
		MessageBox("请输入小于0x1000的数据长度","CH341",MB_OK|MB_ICONSTOP);
		return;
	}
	if(CPublic::m_open)
	{
		try
		{
			if(!CH341MemReadAddr0(CPublic::mIndex, &mBuf[0], &mLen ))
			{   m_memlen0.Format("%x",  0);
				MessageBox("MEM方式读地址0失败!","CH341",MB_OK|MB_ICONSTOP);
			}
			else
			{
				CHAR buffer[mMAX_BUFFER_LENGTH * 3]="";
				ULONG i,j=0;
				for(i=0; i<mLen;i++)
				{
					sprintf(&buffer[j],"%2x ",mBuf[i]);    //两位十六进制数加一个空格
					if (mBuf[i]<16 )                      //一位十六进制字符前面加0
						buffer[j]=48;    				
					if (buffer[j]>=97 && buffer[j]<=122) //小写字母转为大写字母
                        buffer[j]=buffer[j]-32;
					if (buffer[j+1]>=97 && buffer[j+1]<=122) //小写字母转为大写字母
						buffer[j+1]=buffer[j+1]-32;
					j += 3;	
				}
				buffer[j] = '\0';
				m_memdata0 = _T(buffer);
				m_memlen0.Format("%x",  mLen);
				
			}
		}
		catch( CException * )
		{
			TRACE("Error: %d",GetLastError());
		}
	}
	else
	{
		MessageBox("设备未打开！","CH341",MB_OK|MB_ICONSTOP);
	}
	UpdateData(FALSE);
}

void CMemPage::OnButtonMemwrite0()		//MEM方式写地址0
{
	UCHAR mBuf[mMAX_BUFFER_LENGTH*2]="";
	UCHAR buffer[mMAX_BUFFER_LENGTH]="";
	UCHAR bufferFilter[mMAX_BUFFER_LENGTH*2]="";
	ULONG mLen=0,j=0;
	UpdateData(TRUE);
	mLen =CPublic::mStrToBcd(m_memlen0);
	
	if(mLen <= 0 || strlen(m_memdata0) == 0)
	{
		MessageBox("请输入写数据和长度","CH341",MB_OK|MB_ICONSTOP);
		return;
	}
	memcpy(mBuf,m_memdata0,strlen(m_memdata0));
	for(int i=0;i<strlen(m_memdata0);i++)
	{
		if(mBuf[i]!=' ')
		bufferFilter[j++]=mBuf[i];
	}
	if(mLen > j/2)
		mLen = j/2;
	if(mLen>=0x1000)
	{
		MessageBox("请输入小于0x1000的数据长度","CH341",MB_OK|MB_ICONSTOP);
		return;
	}
	memcpy(mBuf,bufferFilter,mLen*2);
	memcpy(buffer, CPublic::mStrtoVal(&bufferFilter[0],mLen*2),mLen);
	if(CPublic::m_open)
	{
		try
		{
			if(!CH341MemWriteAddr0( CPublic::mIndex, &buffer[0], &mLen ) )
			{   m_memlen0.Format("%x", 0);
				MessageBox("MEM方式写地址0失败！","CH341",MB_OK|MB_ICONSTOP);
			}
			else
			{
				m_memlen0.Format("%x",  mLen);	
			}
		}
		catch( CException * )
		{
			TRACE("Error: %d",GetLastError());
		}
	}
	else
	{
		MessageBox("设备未打开！","CH341",MB_OK|MB_ICONSTOP);
	}
	UpdateData(FALSE);
}

void CMemPage::OnButtonMemread1()		//MEM方式读地址1
{
	UCHAR mBuf[mMAX_BUFFER_LENGTH]="";
	ULONG mLen=0;
	UpdateData(TRUE);
	mLen =CPublic::mStrToBcd(m_memlen1);
	
	if(mLen <= 0)
	{
		MessageBox("请输入读数据长度","CH341",MB_OK|MB_ICONSTOP);
		return;
	}
	if(mLen>=0x1000)
	{
		MessageBox("请输入小于0x1000的数据长度","CH341",MB_OK|MB_ICONSTOP);
		return;
	}

	if(CPublic::m_open)
	{
		if(!CH341MemReadAddr1(CPublic::mIndex, &mBuf[0], &mLen ))
		{   m_memlen1.Format("%x", 0);
			MessageBox("MEM方式读地址1失败！","CH341",MB_OK|MB_ICONSTOP);
		}
		else
		{
			CHAR buffer[mMAX_BUFFER_LENGTH * 3];
			ULONG i,j=0;
			for(i=0; i<mLen;i++)
			{
				sprintf(&buffer[j],"%2x ",mBuf[i]);    //两位十六进制数加一个空格
				if (mBuf[i]<16 )                     //一位十六进制字符前面加0
					buffer[j]=48;    			
				if (buffer[j]>=97 && buffer[j]<=122) //小写字母转为大写字母
					buffer[j]=buffer[j]-32;
				if (buffer[j+1]>=97 && buffer[j+1]<=122) //小写字母转为大写字母
					buffer[j+1]=buffer[j+1]-32;
				j += 3;	
			}
			buffer[j] = '\0';
			m_memdata1 = _T(buffer);
			m_memlen1.Format("%x",  mLen);
			
		}
	}
	else
	{
		MessageBox("设备未打开！","CH341",MB_OK|MB_ICONSTOP);
	}UpdateData(FALSE);
}

void CMemPage::OnButtonMemwrite1()		//MEM方式写地址1
{
	UCHAR mBuf[mMAX_BUFFER_LENGTH*2]="";
	UCHAR bufferFilter[mMAX_BUFFER_LENGTH*2]="";
	UCHAR buffer[mMAX_BUFFER_LENGTH]="";
	ULONG mLen=0,j=0;
	UpdateData(TRUE);
	mLen =CPublic::mStrToBcd(m_memlen1);	
	if(mLen <= 0 || strlen(m_memdata1) == 0)
	{
		MessageBox("请输入写数据和长度","CH341",MB_OK|MB_ICONSTOP);
		return;
	}	
	memcpy(mBuf,m_memdata1,strlen(m_memdata1));
	for(int i=0;i<strlen(m_memdata1);i++)
	{
		if(mBuf[i]!=' ')
			bufferFilter[j++]=mBuf[i];
	}
	if(mLen > j/2)
		mLen = j/2;
	if(mLen>=0x1000)
	{
		MessageBox("请输入小于0x1000的数据长度","CH341",MB_OK|MB_ICONSTOP);
		return;
	}
	memcpy(mBuf,bufferFilter,mLen*2);
	memcpy(buffer, CPublic::mStrtoVal(&bufferFilter[0],mLen*2),mLen);
	if(CPublic::m_open)
	{
		if(!CH341MemWriteAddr1( CPublic::mIndex, &buffer[0], &mLen ) )
		{   m_memlen1.Format("%x",  0);
			MessageBox("MEM方式写地址1失败！","CH341",MB_OK|MB_ICONSTOP);
		}
		else
		{
			m_memlen1.Format("%x",  mLen);
			
		}
	}
	else
	{
		MessageBox("设备未打开！","CH341",MB_OK|MB_ICONSTOP);
	}	UpdateData(FALSE);
}

BOOL CMemPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	this->enablebtn(CPublic::m_open );  //初始化按钮

	return TRUE; 
}

void CMemPage::OnBtnmemadd0cl() 
{
	// TODO: Add your control notification handler code here
	::SetDlgItemText(GetSafeHwnd(),IDC_EDIT_MEMDATA0,"");
}



void CMemPage::OnBtni2cmemadd1cl() 
{
	// TODO: Add your control notification handler code here
	::SetDlgItemText(GetSafeHwnd(),IDC_EDIT_MEMDATA1,"");
}

void CMemPage::enablebtn(BOOL bEnable)
{
	::EnableWindow(::GetDlgItem(this->GetSafeHwnd(), IDC_BUTTON_MEMREAD1), bEnable);
	::EnableWindow(::GetDlgItem(this->GetSafeHwnd(), IDC_BUTTON_MEMWRITE1), bEnable);
	::EnableWindow(::GetDlgItem(this->GetSafeHwnd(), IDC_BUTTON_MEMREAD0), bEnable);
	::EnableWindow(::GetDlgItem(this->GetSafeHwnd(), IDC_BUTTON_MEMWRITE0), bEnable);
}
