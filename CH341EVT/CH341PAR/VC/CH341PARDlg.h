// CH341PARDlg.h : header file
//

#if !defined(AFX_CH341PARDLG_H__792438EF_8C82_4EA0_905B_30A7A46AE668__INCLUDED_)
#define AFX_CH341PARDLG_H__792438EF_8C82_4EA0_905B_30A7A46AE668__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include  "EppPage.h"
#include  "MemPage.h"
#include  "OtherPage.h"
#include  "CH341DLL.h"
#include  "I2CSPage.h"
#include "EeprPage.h"
#include "LEDBTDisp.h"
/////////////////////////////////////////////////////////////////////////////
// CCH341PARDlg dialog
class CCH341PARDlg : public CDialog
{
// Construction
public:

	CCH341PARDlg(CWnd* pParent = NULL);	// standard constructor
	BOOL mClose();
	CEppPage m_epp;
	CMemPage m_mem;
	COtherPage m_other;
	CI2CSPage m_i2cs;
	CLEDBTDisp m_btdis;
	CEeprPage m_eeppromwr;
	void enablebtn(BOOL bEnable); 


// Dialog Data
	//{{AFX_DATA(CCH341PARDlg)
	enum { IDD = IDD_CH341PAR_DIALOG };
	CTabCtrl	m_TabMain;
	CButton	m_ok;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCH341PARDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCH341PARDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSelchangeTabmain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnexit();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//CCH341PARDlg * p_Dlg;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CH341PARDLG_H__792438EF_8C82_4EA0_905B_30A7A46AE668__INCLUDED_)
