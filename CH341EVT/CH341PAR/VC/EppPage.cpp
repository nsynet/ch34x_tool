// EppPage.cpp : implementation file
//

#include "stdafx.h"
#include "Total.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEppPage property page

IMPLEMENT_DYNCREATE(CEppPage, CPropertyPage)

CEppPage::CEppPage() : CPropertyPage(CEppPage::IDD)
{
	//{{AFX_DATA_INIT(CEppPage)
	m_eppdata0 = _T("");
	m_eppdata1 = _T("");
	m_epplen0 = _T("");
	m_epplen1 = _T("");
	//}}AFX_DATA_INIT
}

CEppPage::~CEppPage()
{
}

void CEppPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEppPage)
	DDX_Control(pDX, IDC_EDIT_EPPDATA0, m_eppdata0control);
	DDX_Text(pDX, IDC_EDIT_EPPDATA0, m_eppdata0);
	DDX_Text(pDX, IDC_EDIT_EPPDATA1, m_eppdata1);
	DDX_Text(pDX, IDC_EDIT_EPPLEN0, m_epplen0);
	DDX_Text(pDX, IDC_EDIT_EPPLEN1, m_epplen1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEppPage, CPropertyPage)
	//{{AFX_MSG_MAP(CEppPage)
	ON_BN_CLICKED(IDC_BUTTON_EPPREAD0, OnButtonEppread0)
	ON_BN_CLICKED(IDC_BUTTON_EPPWRITE0, OnButtonEppwrite0)
	ON_BN_CLICKED(IDC_BUTTON_EPPREAD1, OnButtonEppread1)
	ON_BN_CLICKED(IDC_BUTTON_EPPWRITE1, OnButtonEppwrite1)
	ON_BN_CLICKED(IDC_BTNEPPDATACL, OnBtneppdatacl)
	ON_BN_CLICKED(IDC_BTNADDCL, OnBtnaddcl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEppPage message handlers

void CEppPage::OnButtonEppread0()	//EPP方式读数据
{
	UCHAR mBuf[mMAX_BUFFER_LENGTH]="";
	ULONG mLen=0;
	UpdateData(TRUE);
	mLen = CPublic::mStrToBcd(m_epplen0);
	if(mLen <= 0)
	{
		MessageBox("请输入读取长度","CH341",MB_OK|MB_ICONSTOP);
		return;
	}
	if(mLen>=0x1000)
	{
		MessageBox("请输入小于0x1000的数值","CH341",MB_OK|MB_ICONSTOP);
		return;
	}
	if(CPublic::m_open)
	{
		try
		{
			if(!CH341EppReadData(CPublic::mIndex,  &mBuf[0],&mLen))
			{   m_epplen0.Format("%x", 0);
				MessageBox("EPP方式读数据失败！ ","CH341",MB_OK|MB_ICONSTOP);
			}
			else
			{
				CHAR buffer[mMAX_BUFFER_LENGTH * 3];
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
				m_epplen0.Format("%x", mLen);
				m_eppdata0 = buffer;			
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
	UpdateData(false);
}

void CEppPage::OnButtonEppwrite0()		//EPP方式写数据
{
	UCHAR mBuf[mMAX_BUFFER_LENGTH*2]="";
	UCHAR bufferFilter[mMAX_BUFFER_LENGTH*2]="";
	UCHAR buffer[mMAX_BUFFER_LENGTH]="";
	ULONG mLen=0,j=0;
	UpdateData(TRUE);
	mLen =CPublic::mStrToBcd(m_epplen0);	
	if(mLen<=0 || strlen(m_eppdata0) == 0)
	{
		MessageBox("请输入写数据和长度！","CH341",MB_OK|MB_ICONSTOP);
		return;
	}
	memcpy(mBuf,m_eppdata0,strlen(m_eppdata0));
	for(int i=0;i<strlen(m_eppdata0);i++)
	{
		if(mBuf[i]!=' ')
			bufferFilter[j++]=mBuf[i];
	}
	if(mLen > j/2)  //在输入长度和数据长度中取小值
		mLen = j/2;
	if(mLen>=0x1000)
	{
		MessageBox("请输入小于0x1000的数值","CH341",MB_OK|MB_ICONSTOP);
		return;
	}
	memcpy(mBuf,bufferFilter,mLen*2);
	memcpy(buffer, CPublic::mStrtoVal(&bufferFilter[0],mLen*2),mLen);    
	if(CPublic::m_open)
	{
		if(!CH341EppWriteData( CPublic::mIndex, &buffer[0], &mLen ) )
		{   
			m_epplen0.Format("%x", 0);
			MessageBox("EPP写数据块0失败！","CH341",MB_OK|MB_ICONSTOP);
		}
		else
		{
			m_epplen0.Format("%x", mLen);    //返回写入的数据个数
			
		}
	}
	else
	{
		MessageBox("设备未打开！","CH341",MB_OK|MB_ICONSTOP);
	}UpdateData(FALSE);
}

void CEppPage::OnButtonEppread1()		//EPP方式读地址
{
	UCHAR mBuf[mMAX_BUFFER_LENGTH]="";
	ULONG mLen=0;
	UpdateData(TRUE);
	mLen =CPublic::mStrToBcd( m_epplen1);
	if(mLen<=0)
	{
		MessageBox("请输入读数据长度","CH341",MB_OK|MB_ICONSTOP);
		return;
	}
	if(mLen>=0x1000)
	{
		MessageBox("请输入小于0x1000的数值","CH341",MB_OK|MB_ICONSTOP);
		return;
	}
	if(CPublic::m_open)
	{

		if(!CH341EppReadAddr(CPublic::mIndex, &mBuf[0], &mLen ))
		{   m_epplen1.Format("%x", 0);
			MessageBox("EPP方式读地址失败！ ","CH341",MB_OK|MB_ICONSTOP);
		}
		else
		{
			CHAR buffer[mMAX_BUFFER_LENGTH * 3];
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
			m_eppdata1 =buffer;
			m_epplen1.Format("%x", mLen);
			
		}
	}
	else
	{
		MessageBox("设备未打开！","CH341",MB_OK|MB_ICONSTOP);
	}UpdateData(FALSE);
}

void CEppPage::OnButtonEppwrite1()		//EPP方式写地址
{
	UCHAR mBuf[mMAX_BUFFER_LENGTH*2]="";
	UCHAR bufferFilter[mMAX_BUFFER_LENGTH*2]="";
	UCHAR buffer[mMAX_BUFFER_LENGTH]="";
	ULONG mLen=0,j=0;
	UpdateData(TRUE);
	mLen= CPublic::mStrToBcd(m_epplen1);	
	if(mLen<=0 || strlen(m_eppdata1) == 0)
	{
		MessageBox("请输入写数据和长度！","CH341",MB_OK|MB_ICONSTOP);
		return;
	}

	memcpy(mBuf,m_eppdata1,strlen(m_eppdata1));
	for(int i=0;i<strlen(m_eppdata1);i++)//过滤输入字符串中的空格
	{
		if(mBuf[i]!=' ')
			bufferFilter[j++]=mBuf[i];
	}
	if(mLen > j/2)  //在输入长度和数据长度中取小值
		mLen = j/2;
	if(mLen>=0x1000)
	{
		MessageBox("请输入小于0x1000的数值","CH341",MB_OK|MB_ICONSTOP);
		return;
	}
    memcpy(mBuf,bufferFilter,mLen*2);   
	memcpy(buffer, CPublic::mStrtoVal(&bufferFilter[0],mLen*2),mLen);
	if(CPublic::m_open)
	{
		if(!CH341EppWriteAddr( CPublic::mIndex, &buffer[0], &mLen ) )
		{  
			m_epplen1.Format("%x", 0);
			MessageBox("EPP方式写地址失败！","CH341",MB_OK|MB_ICONSTOP);
		}
		else
		{
			m_epplen1.Format("%x", mLen);	
		}
	}
	else
	{
		MessageBox("设备未打开！","CH341",MB_OK|MB_ICONSTOP);
	}UpdateData(FALSE);
}


BOOL CEppPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	this->enablebtn(CPublic::m_open ); //初始化按钮

	return TRUE;  
}

void CEppPage::OnBtneppdatacl() 
{
	// TODO: Add your control notification handler code here
	::SetDlgItemText(GetSafeHwnd(),IDC_EDIT_EPPDATA0,"");
}

void CEppPage::OnBtnaddcl() 
{
	// TODO: Add your control notification handler code here
	::SetDlgItemText(GetSafeHwnd(),IDC_EDIT_EPPDATA1,"");
}



void CEppPage::enablebtn(BOOL bEnable)
{
	::EnableWindow(::GetDlgItem(this->GetSafeHwnd(), IDC_BUTTON_EPPREAD0), bEnable);
	::EnableWindow(::GetDlgItem(this->GetSafeHwnd(), IDC_BUTTON_EPPWRITE0), bEnable);
	::EnableWindow(::GetDlgItem(this->GetSafeHwnd(), IDC_BUTTON_EPPREAD1), bEnable);
	::EnableWindow(::GetDlgItem(this->GetSafeHwnd(), IDC_BUTTON_EPPWRITE1), bEnable);
}
