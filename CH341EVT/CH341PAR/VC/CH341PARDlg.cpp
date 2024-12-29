// CH341PARDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Total.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



void CALLBACK CH341_NOTIFY_ROUTINE (ULONG	iEventStatus );  // �豸�¼�֪ͨ�ص�����

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	AfxMessageBox(_T("Version :2.0  https://github.com/nsynet/ch34x_tool "));
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCH341PARDlg dialog

CCH341PARDlg::CCH341PARDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCH341PARDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCH341PARDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
//	p_Dlg = this;
}

void CCH341PARDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCH341PARDlg)
	DDX_Control(pDX, IDC_TABMAIN, m_TabMain);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCH341PARDlg, CDialog)
	//{{AFX_MSG_MAP(CCH341PARDlg)
	ON_WM_DESTROY()
	ON_WM_KEYUP()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABMAIN, OnSelchangeTabmain)
	ON_BN_CLICKED(IDC_BTNEXIT, OnBtnexit)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCH341PARDlg message handlers

BOOL CCH341PARDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	ULONG nRet;

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	CPublic::mIndex = 0;  //�򿪵��豸���
	CPublic::m_open = FALSE;  //�豸�򿪱�־

	if(CH341OpenDevice(CPublic::mIndex) == INVALID_HANDLE_VALUE)
	{
		CPublic::m_open = FALSE;
		this->SetWindowText("CH341EVT **�豸�Ѱγ�");
	}
	else
	{
		CPublic::m_open = true;
		this->SetWindowText("CH341EVT **�豸�Ѳ���");
		nRet = CH341GetVerIC(0);
	}
	m_TabMain.InsertItem(0,"EPP����");//����Tab�ؼ�ҳ��
	m_TabMain.InsertItem(1,"MEM����");
	m_TabMain.InsertItem(2,"EEPROM����");
	m_TabMain.InsertItem(3,"I2C stream");
	m_TabMain.InsertItem(4,"EEPROM��д");
	m_TabMain.InsertItem(5,"����I/O");
	m_epp.Create(IDD_DLGEPP,GetDlgItem(IDC_TABMAIN));
	m_mem.Create(IDD_DLGMEM,GetDlgItem(IDC_TABMAIN));
	m_other.Create(IDD_DLGOTHER,GetDlgItem(IDC_TABMAIN));
	m_i2cs.Create(IDD_DLGI2C2,GetDlgItem(IDC_TABMAIN));
	m_btdis.Create(IDD_DLGBtDisp,GetDlgItem(IDC_TABMAIN));
	m_eeppromwr.Create(IDD_EEPROMRW,GetDlgItem(IDC_TABMAIN));
	m_epp.SetParent(GetDlgItem(IDC_TABMAIN));
	m_mem.SetParent(GetDlgItem(IDC_TABMAIN));
	m_other.SetParent(GetDlgItem(IDC_TABMAIN));
	m_i2cs.SetParent(GetDlgItem(IDC_TABMAIN));
	m_btdis.SetParent(GetDlgItem(IDC_TABMAIN));
	m_eeppromwr.SetParent(GetDlgItem(IDC_TABMAIN));
	CRect rs;
	GetClientRect(&rs);
	rs.top += 20 ;
	m_epp.MoveWindow(&rs);
	m_mem.MoveWindow(&rs);
	m_other.MoveWindow(&rs);
	m_i2cs.MoveWindow(&rs);
	m_btdis.MoveWindow(&rs);
	m_eeppromwr.MoveWindow(&rs);
	m_epp.ShowWindow(TRUE);
	m_mem.ShowWindow(FALSE);
	m_other.ShowWindow(FALSE);
	m_i2cs.ShowWindow(FALSE);
	m_eeppromwr.ShowWindow(FALSE);
	m_btdis.ShowWindow(FALSE);
	m_TabMain.SetCurSel(0);
	CH341SetParaMode(CPublic::mIndex,0);
	CPublic::mainhwnd=GetSafeHwnd(); 
	CH341SetDeviceNotify(CPublic::mIndex,NULL,CH341_NOTIFY_ROUTINE);   //ע�����֪ͨ
	return TRUE;  // return TRUE  unless you set the focus to a control
}


BOOL CCH341PARDlg::mClose()  //�ر�CH341�豸
{
	CH341CloseDevice(CPublic::mIndex);
	CPublic::m_open = FALSE;
	return TRUE;
}

void CCH341PARDlg::OnDestroy() 
{
	mClose();
	CH341SetDeviceNotify(CPublic::mIndex,NULL,NULL);  //ע������֪ͨ
	CDialog::OnDestroy();	
}

void CALLBACK CH341_NOTIFY_ROUTINE (  // �豸�¼�֪ͨ�ص�����
	ULONG	iEventStatus)
{	//�����жϳ����в��ܶ�ϵͳ��Դ��������,���Բ���¼��������ͨ��PostMessage()�ŵ� CCH341PARDlg::OnKeyUp()������
	::PostMessage(CPublic::mainhwnd,WM_KEYUP,iEventStatus,0);	
}

void CCH341PARDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) //����¼�����
{	
	ULONG iEventStatus;
	iEventStatus=nChar;  //����¼�
	
	if(iEventStatus==CH341_DEVICE_ARRIVAL){ // �豸�����¼�,�Ѿ�����
		if(CH341OpenDevice(CPublic::mIndex) == INVALID_HANDLE_VALUE)	{
			MessageBox("���豸ʧ��!","CH341PAR",MB_OK|MB_ICONSTOP);
			CPublic::m_open = FALSE;
			this->SetWindowText("CH341EVT **�豸�Ѱγ�");
		}		
		else
		{
			CPublic::m_open = TRUE; //�򿪳ɹ�
			this->SetWindowText("CH341EVT **�豸�Ѳ���");
			int current=m_TabMain.GetCurSel();
			if(current==0)
			{
				CH341SetParaMode(CPublic::mIndex,0);
			}
			else if(current==1||current==5)
			{
				CH341SetParaMode(CPublic::mIndex,2);
			}
		}
	}
	else if(iEventStatus==CH341_DEVICE_REMOVE){ // �豸�γ��¼�,�Ѿ��γ�
		CH341CloseDevice(CPublic::mIndex);
		CPublic::m_open = FALSE;	
	}
	enablebtn(CPublic::m_open); //�豸��,��ť����,�豸û��,��ť����
}

//���ð�ť�Ƿ����
 void CCH341PARDlg::enablebtn(BOOL bEnable)  //bEnable=true :able ;=false:enable
 {
	 ::EnableWindow(::GetDlgItem(m_mem.GetSafeHwnd(), IDC_BUTTON_MEMREAD1), bEnable);
	 ::EnableWindow(::GetDlgItem(m_mem.GetSafeHwnd(), IDC_BUTTON_MEMWRITE1), bEnable);
	 ::EnableWindow(::GetDlgItem(m_mem.GetSafeHwnd(), IDC_BUTTON_MEMREAD0), bEnable);
	 ::EnableWindow(::GetDlgItem(m_mem.GetSafeHwnd(), IDC_BUTTON_MEMWRITE0), bEnable);
	 
	 ::EnableWindow(::GetDlgItem(m_epp.GetSafeHwnd(), IDC_BUTTON_EPPREAD0), bEnable);
	 ::EnableWindow(::GetDlgItem(m_epp.GetSafeHwnd(), IDC_BUTTON_EPPWRITE0), bEnable);
	 ::EnableWindow(::GetDlgItem(m_epp.GetSafeHwnd(), IDC_BUTTON_EPPREAD1), bEnable);
	 ::EnableWindow(::GetDlgItem(m_epp.GetSafeHwnd(), IDC_BUTTON_EPPWRITE1), bEnable);
	 
	 ::EnableWindow(::GetDlgItem(m_i2cs.GetSafeHwnd(), IDC_SEND), bEnable);
	 
	 ::EnableWindow(::GetDlgItem(m_btdis.GetSafeHwnd(), IDC_BtRefresh), bEnable);
	 
	 ::EnableWindow(::GetDlgItem(m_eeppromwr.GetSafeHwnd(), IDC_WrDate), bEnable);
	 ::EnableWindow(::GetDlgItem(m_eeppromwr.GetSafeHwnd(), IDC_RdDate), bEnable);
	 
	 ::EnableWindow(::GetDlgItem(m_other.GetSafeHwnd(), IDC_BUTTON_I2CREAD), bEnable);
	 ::EnableWindow(::GetDlgItem(m_other.GetSafeHwnd(), IDC_BUTTON_I2CWRITE), bEnable);	
	 
/*	 if(m_btdis.formshow==TRUE && bEnable)  //����I/O�����ѳ�ʼ��,ˢ��LED,�����뿪��״̬
	 {
		 m_btdis.OnCheck8(); 
		 m_btdis.OnBtRefresh();
	 }
	 
*/	 if(bEnable)  //���������ʾ
		 this->SetWindowText("CH341EVT **�豸�Ѳ���");
	 else
		this->SetWindowText("CH341EVT **�豸�Ѱγ�");
 }


void CCH341PARDlg::OnSelchangeTabmain(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int current=m_TabMain.GetCurSel();
	switch(current)
	{
		case 0:
			{
				m_epp.ShowWindow(TRUE);
				m_mem.ShowWindow(FALSE);
				m_other.ShowWindow(FALSE);
				m_i2cs.ShowWindow(FALSE);
				m_eeppromwr.ShowWindow(FALSE);
				m_btdis.ShowWindow(FALSE);
				if(CPublic::m_open)
				CH341SetParaMode(CPublic::mIndex,0);
			}
			break;
		case 1:
			{
				m_epp.ShowWindow(FALSE);
				m_mem.ShowWindow(TRUE);
				m_other.ShowWindow(FALSE);
				m_i2cs.ShowWindow(FALSE);
				m_eeppromwr.ShowWindow(FALSE);
				m_btdis.ShowWindow(FALSE);
				if(CPublic::m_open)
				CH341SetParaMode(CPublic::mIndex,2);
			}
			break;
		case 2:
			{
				m_epp.ShowWindow(FALSE);
				m_mem.ShowWindow(FALSE);
				m_other.ShowWindow(TRUE);
				m_i2cs.ShowWindow(FALSE);
				m_eeppromwr.ShowWindow(FALSE);
				m_btdis.ShowWindow(FALSE);
			}
			break;
		case 3:
			{
				m_epp.ShowWindow(FALSE);
				m_mem.ShowWindow(FALSE);
				m_other.ShowWindow(FALSE);
				m_i2cs.ShowWindow(TRUE);
				m_eeppromwr.ShowWindow(FALSE);
				m_btdis.ShowWindow(FALSE);
			}
			break;
		case 4:
			{
				m_epp.ShowWindow(FALSE);
				m_mem.ShowWindow(FALSE);
				m_other.ShowWindow(FALSE);
				m_i2cs.ShowWindow(FALSE);
				m_eeppromwr.ShowWindow(TRUE);
				m_btdis.ShowWindow(FALSE);
			}
			break;
		case 5:
			{
				if(CPublic::m_open)
				CH341SetParaMode(CPublic::mIndex,2);
				m_epp.ShowWindow(FALSE);
				m_mem.ShowWindow(FALSE);
				m_other.ShowWindow(FALSE);
				m_i2cs.ShowWindow(FALSE);
				m_eeppromwr.ShowWindow(FALSE);
				m_btdis.ShowWindow(TRUE);
				m_btdis.OnSetActive();	
			}
			break;
	}
}

void CCH341PARDlg::OnBtnexit() 
{
	mClose();
	CH341SetDeviceNotify(CPublic::mIndex,NULL,NULL);  //ע������֪ͨ
	this->OnOK();
}
