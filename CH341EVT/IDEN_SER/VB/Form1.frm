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
   StartUpPosition =   3  '窗口缺省
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
      Caption         =   "清除显示"
      Height          =   345
      Left            =   3225
      TabIndex        =   8
      Top             =   3960
      Width           =   2175
   End
   Begin VB.Frame Frame2 
      Caption         =   "插拔监视"
      Height          =   1290
      Left            =   3240
      TabIndex        =   5
      Top             =   2175
      Width           =   2130
      Begin VB.CommandButton stopmonitor 
         Caption         =   "停止监视 CH341串口"
         Height          =   360
         Left            =   165
         TabIndex        =   7
         Top             =   750
         Width           =   1800
      End
      Begin VB.CommandButton beginmonitor 
         Caption         =   "开始监视 CH341串口"
         Height          =   360
         Left            =   180
         TabIndex        =   6
         Top             =   300
         Width           =   1800
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "查找CH341串口"
      Height          =   1290
      Left            =   3240
      TabIndex        =   2
      Top             =   435
      Width           =   2130
      Begin VB.CommandButton PtNameIsCH341 
         Caption         =   "串口名称查找"
         Height          =   360
         Left            =   180
         TabIndex        =   4
         Top             =   765
         Width           =   1800
      End
      Begin VB.CommandButton PtHandleIsCH341 
         Caption         =   "串口句柄查找"
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

showinfo.AddItem "通过名称找到CH341串口为:"
For j = 1 To 21
    portname = "COM" + Trim(Str(j)) + Trim(Chr(0)) '设备名
    fullportname = "\\.\" & portname '完整的设备名
    If (CH341PtNameIsCH341(fullportname) <> False) Then
        showinfo.AddItem portname
    End If
Next j
End Sub

Private Sub PtHandleIsCH341_Click()
 '在完整的串口名为"////.//COMxx"
 '通过串口特征数据来识别CH341串口
 Dim porthandle As Long
 Dim i As Byte
 Dim j As Byte
 Dim portname As String
 Dim fullportname As String
 showinfo.AddItem "通过句柄找到CH341串口为:"
 For j = 1 To 21
    portname = "COM" + Trim(Str(j)) + Trim(Chr(0)) '设备名
    fullportname = "\\.\" & portname '完整的设备名
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
    showinfo.AddItem "开始监视CH341串口插拔"
Else
    showinfo.AddItem "设置CH341串口监视失败"
End If
End Sub

Private Sub stopmonitor_Click()
If (CH341PtSetDevNotify(vbNullString, 0&) <> False) Then
    showinfo.AddItem "停止监视CH341串口插拔"
    ch341sername = ""
Else
    showinfo.AddItem "停止监视CH341串口插拔失败"
End If
End Sub


    
