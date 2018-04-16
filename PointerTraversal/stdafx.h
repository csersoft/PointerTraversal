#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>
#include <commctrl.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>
#include <locale.h>
#include <tchar.h>

#include "resource.h"

#define CN_LABEL            _T("Static")
#define CN_EDIT             _T("Edit")
#define CN_BUTTON           _T("Button")
#define RB_TEXT             _T("遍历当前偏移处的指针")

#define CS_DEFLABEL         (WS_CHILD | WS_VISIBLE | SS_RIGHT)
#define CS_DEFEDIT          (WS_CHILD | WS_VISIBLE | ES_CENTER | ES_UPPERCASE)
#define CS_DEFBUTTON        (WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_NOTIFY)

#define FM_ExactValue       0
#define FM_Greater          1
#define FM_Less             2
#define FM_Range            3

#ifdef _INC_COMMCTRL

#define ListView_DlgSetExtendedListViewStyle(hwnd, dlgitem, dw)\
    (DWORD)::SendDlgItemMessage((hwnd),(int)(dlgitem),LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dw)

#define ListView_DlgInsertColumn(hwnd, dlgitem, iCol, pcol) \
    (int)::SendDlgItemMessage((hwnd),(int)(dlgitem),LVM_INSERTCOLUMN, (WPARAM)(int)(iCol), (LPARAM)(const LV_COLUMN *)(pcol))

#define ListView_DlgDeleteAllItems(hwnd, dlgitem) \
    (BOOL)::SendDlgItemMessage((hwnd),(int)(dlgitem),LVM_DELETEALLITEMS, 0, 0L)

#define ListView_DlgInsertItemA(hwnd, dlgitem, pitem)   \
    (int)::SendDlgItemMessageA((hwnd),(int)(dlgitem),LVM_INSERTITEMA, 0, (LPARAM)(const LVITEMA *)(pitem))

#define ListView_DlgInsertItemW(hwnd, dlgitem, pitem)   \
    (int)::SendDlgItemMessageW((hwnd),(int)(dlgitem),LVM_INSERTITEMW, 0, (LPARAM)(const LVITEMW *)(pitem))

#define ListView_DlgInsertItem(hwnd, dlgitem, pitem)   \
    (int)::SendDlgItemMessage((hwnd),(int)(dlgitem),LVM_INSERTITEM, 0, (LPARAM)(const LV_ITEM *)(pitem))

#define ListView_DlgSetItemA(hwnd, dlgitem,pitem) \
    (BOOL)::SendDlgItemMessageA((hwnd),(int)(dlgitem),LVM_SETITEMA, 0, (LPARAM)(const LVITEMA *)(pitem))

#define ListView_DlgSetItemW(hwnd, dlgitem,pitem) \
    (BOOL)::SendDlgItemMessageW((hwnd),(int)(dlgitem),LVM_SETITEMW, 0, (LPARAM)(const LVITEMW *)(pitem))

#define ListView_DlgSetItem(hwnd, dlgitem,pitem) \
    (BOOL)::SendDlgItemMessage((hwnd),(int)(dlgitem),LVM_SETITEM, 0, (LPARAM)(const LV_ITEM *)(pitem))

#define ListView_DlgGetItemCount(hwnd, dlgitem) \
    (int)::SendDlgItemMessage((hwnd),(int)(dlgitem),LVM_GETITEMCOUNT, 0, 0L)

#endif

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(linker,"/manifestdependency:\"type='win32'   name='Microsoft.Windows.Common-Controls'   version='6.0.0.0'   processorArchitecture='x86'   publicKeyToken='6595b64144ccf1df'   language='*'\"")

typedef enum _TYPE_ENUM
{
    TE_NONE = -1,
    TE_BYTE,
    TE_WORD,
    TE_DWORD,
    TE_FLOAT,
    TE_DOUBLE,
    TE_CHAR,
    TE_WCHAR,
    TE_ARRAY
} TYPE_ENUM;

typedef union _TypeUnion
{
    BYTE            Byte;
    WORD            Word;
    DWORD           DWord;
    float           Float;
    double          Double;
    PVOID           pvoid;
} TypeUnion;

extern HINSTANCE        hInstance;
extern HANDLE           ProcessHandle;
extern float            SINGLEEPSION;
extern double           DOUBLEEPSION;
extern DWORD            AddSize;
extern DWORD            AddNumber;
extern int              BaseAddress;
extern int              OffsetArray[10];
extern WORD             OffsetCount;
extern WORD             CurOffsetIdx;
extern TYPE_ENUM        TE;
extern BYTE             SJD;
extern BYTE             DJD;
extern BYTE             FindMode;

extern BYTE             D1_Byte;
extern WORD             D1_Word;
extern DWORD            D1_DWord;
extern float            D1_Float;
extern double           D1_Double;

extern BYTE             D2_Byte;
extern WORD             D2_Word;
extern DWORD            D2_DWord;
extern float            D2_Float;
extern double           D2_Double;

extern PCHAR            astr;
extern PWCHAR           wstr;
extern PBYTE            ByteArray;

extern DWORD            TextCount;
extern DWORD            ArrayCount;

extern void SetStatusText(PTCHAR st);
extern void HexFilter(HWND hEdit);
extern INT_PTR CALLBACK DestProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
extern INT_PTR CALLBACK OffsetProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
extern int AddListItem(HWND hDlg,int DlgItem,int iItem,int iSubItem,PTCHAR pText);

extern bool SF_Equal(float a, float b);
extern bool SF_Greater(float a, float b);
extern bool SF_Less(float a, float b);
extern bool DF_Equal(double a, double b);
extern bool DF_Greater(double a, double b);
extern bool DF_Less(double a, double b);