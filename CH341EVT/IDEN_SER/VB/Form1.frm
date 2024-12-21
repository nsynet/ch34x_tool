VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Search & Monitor CH341Ser Port"
   ClientHeight    =   4485
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   5475
   LinkTopic       =   "Form1"
   ScaleHeight     =   4485
   ScaleWidth      =   5475
   StartUpPosition =   3  '����ȱʡ
   Begin VB.CommandButton notify 
      Caption         =   "devnotify"
      Height          =   240
      Left            =   3585
      TabIndex        =   9
      Top             =   3615
      Visible         =   0   'False
      Width           =   1410
   End
   Begin VB.CommandButton clearshow 
      Caption         =   "�����ʾ"
      Height          =   345
      Left            =   3225
      TabIndex        =   8
      Top             =   3960
      Width           =   2175
   End
   Begin VB.Frame Frame2 
      Caption         =   "��μ���"
      Height          =   1290
      Left            =   3240
      TabIndex        =   5
      Top             =   2175
      Width           =   2130
      Begin VB.CommandButton stopmonitor 
         Caption         =   "ֹͣ���� CH341����"
         Height          =   360
         Left            =   165
         TabIndex        =   7
         Top             =   750
         Width           =   1800
      End
      Begin VB.CommandButton beginmonitor 
         Caption         =   "��ʼ���� CH341����"
         Height          =   360
         Left            =   180
         TabIndex        =   6
         Top             =   300
         Width           =   1800
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "����CH341����"
      Height          =   1290
      Left            =   3240
      TabIndex        =   2
      Top             =   435
      Width           =   2130
      Begin VB.CommandButton PtNameIsCH341 
         Caption         =   "�������Ʋ���"
         Height          =   360
         Left            =   180
         TabIndex        =   4
         Top             =   765
         Width           =   1800
      End
      Begin VB.CommandButton PtHandleIsCH341 
         Caption         =   "���ھ������"
         Height          =   360
         Left            =   180
         TabIndex        =   3
         Top             =   315
         Width           =   1800
      End
   End
   Begin VB.ListBox showinfo 
      Height          =   3840
      ItemData        =   "Form1.frx":0000
      Left            =   90
      List            =   "Form1.frx":0007
      TabIndex        =   1
      Top             =   435
      Width           =   3000
   End
   Begin VB.Label CH341PTver 
      Caption         =   "CH341PT.DLL Ver:??"
      Height          =   180
      Left            =   180
      TabIndex        =   0
      Top             =   180
      Width           =   1620
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub clearshow_Click()
showinfo.Clear
End Sub

Private Sub Form_Load()
CH341PTver.Caption = "CH341PT.DLL Ver:" & Hex(CH341PtGetVersion())
End Sub

Private Sub notify_KeyUp(KeyCode As Integer, Shift As Integer)
   Call CH341SerNotifyFunc(KeyCode)
End Sub

Private Sub PtNameIsCH341_Click()
 Dim porthandle As Long
 Dim i As Byte
 Dim j As Byte
 Dim portname As String
 Dim fullportname As String

showinfo.AddItem "ͨ�������ҵ�CH341����Ϊ:"
For j = 1 To 21
    portname = "COM" + Trim(Str(j)) + Trim(Chr(0)) '�豸��
    fullportname = "\\.\" & portname '�������豸��
    If (CH341PtNameIsCH341(fullportname) <> False) Then
        showinfo.AddItem portname
    End If
Next j
End Sub

Private Sub PtHandleIsCH341_Click()
 '�������Ĵ�����Ϊ"////.//COMxx"
 'ͨ����������������ʶ��CH341����
 Dim porthandle As Long
 Dim i As Byte
 Dim j As Byte
 Dim portname As String
 Dim fullportname As String
 showinfo.AddItem "ͨ������ҵ�CH341����Ϊ:"
 For j = 1 To 21
    portname = "COM" + Trim(Str(j)) + Trim(Chr(0)) '�豸��
    fullportname = "\\.\" & portname '�������豸��
    porthandle = CreateFile(fullportname, GENERIC_READ Or GENERIC_WRITE, FILE_SHARE_READ Or FILE_SHARE_WRITE, vbNullString, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0&)
    If (porthandle <> INVALID_HANDLE_VALUE) Then
        If (CH341PtHandleIsCH341(porthandle) <> False) Then
            showinfo.AddItem portname
       End If
        CloseHandle (porthandle)
    End If
 Next j
End Sub

Private Sub beginmonitor_Click()
If (CH341PtSetDevNotify(vbNullString, AddressOf CH341PT_NOTIFY_ROUTINE) <> False) Then
    showinfo.AddItem "��ʼ����CH341���ڲ��"
Else
    showinfo.AddItem "����CH341���ڼ���ʧ��"
End If
End Sub

Private Sub stopmonitor_Click()
If (CH341PtSetDevNotify(vbNullString, 0&) <> False) Then
    showinfo.AddItem "ֹͣ����CH341���ڲ��"
    ch341sername = ""
Else
    showinfo.AddItem "ֹͣ����CH341���ڲ��ʧ��"
End If
End Sub


    
