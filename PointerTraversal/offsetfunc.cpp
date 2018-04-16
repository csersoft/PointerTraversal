#include "stdafx.h"

//WNDPROC DefEditProc = NULL;
DWORD AddSize = 4;
DWORD AddNumber = 1000;
int BaseAddress = 0;
int OffsetArray[10] = {0};
WORD OffsetCount = 1;
WORD CurOffsetIdx = 1;

/*LRESULT CALLBACK HexEditProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    if (message == WM_CHAR)
        if (!((wParam >= 48 && wParam <= 57) || (wParam >= 65 && wParam <= 70) || (wParam >= 97 && wParam <= 102) || wParam == 0x08))
            return 0;
    return CallWindowProc(DefEditProc,hDlg,message,wParam,lParam);
}*/

void UpdateData(HWND hDlg)
{
    int val;
    WORD i;
    TCHAR sbuff[20];
    
    if (SendDlgItemMessage(hDlg,IDC_ADDSIZE,WM_GETTEXT,20,(LPARAM)sbuff))
        if (_stscanf_s(sbuff,_T("%X"),&val))
            AddSize = val;
    if (SendDlgItemMessage(hDlg,IDC_ADDNUM,WM_GETTEXT,20,(LPARAM)sbuff))
        if (_stscanf_s(sbuff,_T("%u"),&val))
            AddNumber = val;
    if (SendDlgItemMessage(hDlg,IDC_BASEADDRESS,WM_GETTEXT,20,(LPARAM)sbuff))
        if (_stscanf_s(sbuff,_T("%X"),&val))
            BaseAddress = val;

    for (i = 0;i < OffsetCount;i++)
    {
        if (SendDlgItemMessage(hDlg,IDC_EOS1 + i,WM_GETTEXT,20,(LPARAM)sbuff))
            if (_stscanf_s(sbuff,_T("%X"),&val))
                OffsetArray[i] = val;
        if (SendDlgItemMessage(hDlg,IDC_ROS1 + i,BM_GETCHECK,0,0) == BST_CHECKED)
            CurOffsetIdx = i + 1;
    }
}

void InitWindow(HWND hDlg)
{
    TCHAR sbuff[20];

    if (CurOffsetIdx < 1 || CurOffsetIdx > 10) CurOffsetIdx = 1;

    SendDlgItemMessage(hDlg,IDC_ADDSIZE,EM_SETLIMITTEXT,8,0);
    SendDlgItemMessage(hDlg,IDC_ADDNUM,EM_SETLIMITTEXT,8,0);
    SendDlgItemMessage(hDlg,IDC_BASEADDRESS,EM_SETLIMITTEXT,8,0);
    SendDlgItemMessage(hDlg,IDC_EOS1,EM_SETLIMITTEXT,8,0);

    //DefEditProc = (WNDPROC)SetWindowLong(GetDlgItem(hDlg,IDC_ADDSIZE),GWL_WNDPROC,(LONG)HexEditProc);
    //SetWindowLong(GetDlgItem(hDlg,IDC_BASEADDRESS),GWL_WNDPROC,(LONG)HexEditProc);
    //SetWindowLong(GetDlgItem(hDlg,IDC_EOS1),GWL_WNDPROC,(LONG)HexEditProc);

    _stprintf_s(sbuff,20,_T("%X"),AddSize);
    SendDlgItemMessage(hDlg,IDC_ADDSIZE,WM_SETTEXT,0,(LPARAM)sbuff);
    _stprintf_s(sbuff,20,_T("%u"),AddNumber);
    SendDlgItemMessage(hDlg,IDC_ADDNUM,WM_SETTEXT,0,(LPARAM)sbuff);
    _stprintf_s(sbuff,20,_T("%08X"),BaseAddress);
    SendDlgItemMessage(hDlg,IDC_BASEADDRESS,WM_SETTEXT,0,(LPARAM)sbuff);
    _stprintf_s(sbuff,20,_T("%X"),OffsetArray[0]);
    SendDlgItemMessage(hDlg,IDC_EOS1,WM_SETTEXT,0,(LPARAM)sbuff);
}

void RedrawForm(HWND hDlg)
{
    WORD i;
    HWND CH;
    HFONT hFont;
    HWND wndtmp;
    TCHAR sbuff[20];

    if (OffsetCount < 1 || OffsetCount > 10) OffsetCount = 1;
    if (CurOffsetIdx < 1 || CurOffsetIdx > 10) CurOffsetIdx = 1;

    if (OffsetCount == 1)
        EnableWindow(GetDlgItem(hDlg,IDC_DELOFFSET),FALSE);
    else
        EnableWindow(GetDlgItem(hDlg,IDC_DELOFFSET),TRUE);

    if (OffsetCount == 10)
        EnableWindow(GetDlgItem(hDlg,IDC_ADDOFFSET),FALSE);
    else
        EnableWindow(GetDlgItem(hDlg,IDC_ADDOFFSET),TRUE);

    for (i = OffsetCount + 1;i <= 10;i++)   //销毁多余控件
    {
        CH = GetDlgItem(hDlg,IDC_LOS1 + i - 1);
        if (CH) DestroyWindow(CH);

        CH = GetDlgItem(hDlg,IDC_EOS1 + i - 1);
        if (CH) DestroyWindow(CH);

        CH = GetDlgItem(hDlg,IDC_ROS1 + i - 1);
        if (CH)
        {
            if (SendMessage(CH,BM_GETCHECK,0,0) == BST_CHECKED)
                SendDlgItemMessage(hDlg,IDC_ROS1 + OffsetCount - 1,BM_SETCHECK,BST_CHECKED,0);
            DestroyWindow(CH);
        }
    }

    SetWindowPos(hDlg,0,0,0,390,152 + (OffsetCount - 1) * 24,SWP_NOMOVE | SWP_NOZORDER);
    hFont = (HFONT)SendMessage(hDlg,WM_GETFONT,0,0);

    for (i = 2;i <= OffsetCount;i++)    //创建新控件
    {
        CH = GetDlgItem(hDlg,IDC_LOS1 + i - 1);
        if (!CH)
        {
            _stprintf_s(sbuff,20,_T("%u级偏移  (Hex):"),i);
            wndtmp = CreateWindowEx(WS_EX_TRANSPARENT,CN_LABEL,sbuff,CS_DEFLABEL,11,98 + (i - 1) * 24,90,18,hDlg,0,hInstance,0);
            SetWindowLong(wndtmp,GWL_ID,IDC_LOS1 + i - 1);
            SendMessage(wndtmp,WM_SETFONT,(WPARAM)hFont,TRUE);

            _stprintf_s(sbuff,20,_T("%X"),OffsetArray[i - 1]);
            wndtmp = CreateWindowEx(WS_EX_CLIENTEDGE,CN_EDIT,sbuff,CS_DEFEDIT,107,96 + (i - 1) * 24,80,20,hDlg,0,hInstance,0);
            SetWindowLong(wndtmp,GWL_ID,IDC_EOS1 + i - 1);
            SendMessage(wndtmp,EM_SETLIMITTEXT,8,0);
            SendMessage(wndtmp,WM_SETFONT,(WPARAM)hFont,TRUE);

            wndtmp = CreateWindowEx(WS_EX_TRANSPARENT,CN_BUTTON,RB_TEXT,CS_DEFBUTTON,204,97 + (i - 1) * 24,160,20,hDlg,0,hInstance,0);
            SetWindowLong(wndtmp,GWL_ID,IDC_ROS1 + i - 1);
            SendMessage(wndtmp,WM_SETFONT,(WPARAM)hFont,TRUE);
        }
    }
}

INT_PTR CALLBACK OffsetProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    int wmId, wmEvent;

    switch (message)
    {
    case WM_INITDIALOG:
        InitWindow(hDlg);
        RedrawForm(hDlg);
        SendDlgItemMessage(hDlg,IDC_ROS1 + CurOffsetIdx - 1,BM_SETCHECK,1,0);
        return TRUE;
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDC_ADDOFFSET:
            if (OffsetCount != 10) OffsetCount++;
            RedrawForm(hDlg);
            break;
        case IDC_DELOFFSET:
            if (OffsetCount != 1) OffsetCount--;
            RedrawForm(hDlg);
            break;
        default:
            if (wmId == IDC_BASEADDRESS || wmId == IDC_ADDSIZE || (wmId >= IDC_EOS1 && wmId <= IDC_EOS1 + 9))
                if (wmEvent == EN_UPDATE)
                    HexFilter((HWND)lParam);
            break;
        }

        break;
    case WM_CLOSE:
        UpdateData(hDlg);
        EndDialog(hDlg,1);
        break;
    }
    return 0;
}