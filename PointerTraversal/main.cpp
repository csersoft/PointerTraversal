#include "stdafx.h"

HINSTANCE hInstance = NULL;
HANDLE ProcessHandle = NULL;
HWND MainWindow = NULL;
BOOL IsStop = FALSE;

void BeginTraversal(HWND hDlg);
void StopTraversal(HWND hDlg);

BOOL CheckMessageQueue()
{
    MSG msg;
    while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
    {
        if(msg.message == WM_QUIT)
            return FALSE;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return TRUE;
}

void SetStatusText(PTCHAR st)
{
    SendDlgItemMessage(MainWindow,IDC_STATUS,WM_SETTEXT,0,(LPARAM)st);
}

INT_PTR CALLBACK MainProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;

    switch (message)
    {
    case WM_INITDIALOG:
        {
            LV_COLUMN lvc;

            MainWindow = hDlg;

            ZeroMemory(&lvc,sizeof(LV_COLUMN));
            lvc.mask = LVCF_TEXT | LVCF_WIDTH;

            lvc.pszText = _T("序号");
            lvc.cx = 50;
            ListView_DlgInsertColumn(hDlg,IDC_LIST1,0,&lvc);

            lvc.pszText = _T("完整偏移");
            lvc.cx = 270;
            ListView_DlgInsertColumn(hDlg,IDC_LIST2,0,&lvc);

            lvc.pszText = _T("偏移量");
            lvc.cx = 140;
            ListView_DlgInsertColumn(hDlg,IDC_LIST1,1,&lvc);

            lvc.pszText = _T("目标值");
            lvc.cx = 190;
            ListView_DlgInsertColumn(hDlg,IDC_LIST1,2,&lvc);
            lvc.cx = 120;
            ListView_DlgInsertColumn(hDlg,IDC_LIST2,1,&lvc);

            SendDlgItemMessage(hDlg,IDC_PID,EM_SETLIMITTEXT,8,0);

            ListView_DlgSetExtendedListViewStyle(hDlg,IDC_LIST1,LVS_EX_FULLROWSELECT);
            ListView_DlgSetExtendedListViewStyle(hDlg,IDC_LIST2,LVS_EX_FULLROWSELECT);
        }
        return TRUE;
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        switch (wmId)
        {
        case IDC_SETOFFSET:
            DialogBox(hInstance,MAKEINTRESOURCE(IDD_SETOFFSET),hDlg,OffsetProc);
            break;
        case IDC_SETTYPE:
            DialogBox(hInstance,MAKEINTRESOURCE(IDD_SETTYPE),hDlg,DestProc);
            break;
        case IDC_OPENPROCESS:
            {
                TCHAR stmp[32] = {0};
                DWORD PID;

                if (ProcessHandle) CloseHandle(ProcessHandle);

                if (!GetWindowText(GetDlgItem(hDlg,IDC_PID),stmp,32))
                    break;

                _stscanf_s(stmp,_T("%u"),&PID);

                ProcessHandle = OpenProcess(PROCESS_VM_READ,FALSE,PID);

                if (ProcessHandle == NULL)
                {
                    _stprintf_s(stmp,32,_T("打开进程失败!错误码:0x%X"),GetLastError());
                    SetStatusText(stmp);
                    break;
                }
                SetStatusText(_T("打开进程完毕!"));

                EnableWindow(GetDlgItem(hDlg,IDC_CLOSEPROCESS),TRUE);
                EnableWindow(GetDlgItem(hDlg,IDC_SETOFFSET),TRUE);
                EnableWindow(GetDlgItem(hDlg,IDC_SETTYPE),TRUE);
                EnableWindow(GetDlgItem(hDlg,IDC_BEGINFIND),TRUE);
                EnableWindow(GetDlgItem(hDlg,IDC_LIST1),TRUE);
                EnableWindow(GetDlgItem(hDlg,IDC_LIST2),TRUE);
            }
            break;
        case IDC_CLOSEPROCESS:
            if (ProcessHandle)
            {
                CloseHandle(ProcessHandle);
                ProcessHandle = NULL;
                SetStatusText(_T("关闭进程完毕!"));
            }
            EnableWindow(GetDlgItem(hDlg,IDC_CLOSEPROCESS),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDC_SETOFFSET),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDC_SETTYPE),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDC_BEGINFIND),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDC_STOPFIND),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDC_LIST1),FALSE);
            EnableWindow(GetDlgItem(hDlg,IDC_LIST2),FALSE);
            break;
        case IDC_BEGINFIND:
            BeginTraversal(hDlg);
            break;
        case IDC_STOPFIND:
            IsStop = TRUE;
            break;
        }
        break;
    case WM_CLOSE:
        if (ProcessHandle)
        {
            CloseHandle(ProcessHandle);
            ProcessHandle = NULL;
        }
        EndDialog(hDlg,1);
        break;
    case WM_DESTROY:
        if (ProcessHandle)
        {
            CloseHandle(ProcessHandle);
            ProcessHandle = NULL;
        }
        PostQuitMessage(0);
        break;
    }
    return 0;
}

int AddListItemA(HWND hDlg,int DlgItem,int iItem,int iSubItem,PCHAR pText)
{
    LVITEMA lvi;

    if (strlen(pText) == 0) return 0;
    ZeroMemory(&lvi,sizeof(LV_ITEM));
    lvi.mask = LVIF_TEXT;
    lvi.iItem = iItem;
    lvi.iSubItem = iSubItem;
    lvi.pszText = pText;

    if (iSubItem == 0)
        return ListView_DlgInsertItemA(hDlg,DlgItem,&lvi);
    else
        return ListView_DlgSetItemA(hDlg,DlgItem,&lvi);
}

int AddListItemW(HWND hDlg,int DlgItem,int iItem,int iSubItem,PWCHAR pText)
{
    LVITEMW lvi;

    if (wcslen(pText) == 0) return 0;
    ZeroMemory(&lvi,sizeof(LV_ITEM));
    lvi.mask = LVIF_TEXT;
    lvi.iItem = iItem;
    lvi.iSubItem = iSubItem;
    lvi.pszText = pText;

    if (iSubItem == 0)
        return ListView_DlgInsertItemW(hDlg,DlgItem,&lvi);
    else
        return ListView_DlgSetItemW(hDlg,DlgItem,&lvi);
}

int AddListItem(HWND hDlg,int DlgItem,int iItem,int iSubItem,PTCHAR pText)
{
    LV_ITEM lvi;

    if (_tcslen(pText) == 0) return 0;
    ZeroMemory(&lvi,sizeof(LV_ITEM));
    lvi.mask = LVIF_TEXT;
    lvi.iItem = iItem;
    lvi.iSubItem = iSubItem;
    lvi.pszText = pText;

    if (iSubItem == 0)
        return ListView_DlgInsertItem(hDlg,DlgItem,&lvi);
    else
        return ListView_DlgSetItem(hDlg,DlgItem,&lvi);
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInstance,_In_ LPTSTR lpCmdLine,_In_ int nCmdShow)
{
    hInstance = hInst;
    setlocale(LC_ALL,"");
    InitCommonControls();
    
    DialogBox(hInst,MAKEINTRESOURCE(IDD_MAINFORM),0,MainProc);
    if (ProcessHandle) CloseHandle(ProcessHandle);
    return 1;
}

bool GetOffsetText(DWORD AddNum,PTCHAR pText,DWORD MaxCount)
{
    DWORD i;
    DWORD pint;
    PTCHAR stmp = (PTCHAR)calloc(MaxCount,sizeof(TCHAR));

    pText[0] = 0;
    if (!stmp) return false;

    _stprintf_s(pText,MaxCount,_T("[%08X]+%X"),BaseAddress,OffsetArray[0]);
    for (i = 2;i <= OffsetCount;i++)
    {
        if (i == CurOffsetIdx)
            pint = OffsetArray[i - 1] + AddNum * AddSize;
        else
            pint = OffsetArray[i - 1];
        _stprintf_s(stmp,MaxCount,_T("[%s]+%X"),pText,pint);
        _tcscpy_s(pText,MaxCount,stmp);
    }
    free(stmp);
    return true;
}

void BeginTraversal(HWND hDlg)
{
    DWORD i,j;
    SIZE_T DestSize;

    IsStop = FALSE;
    if (!AddNumber || !AddSize)
    {
        SetStatusText(_T("递增大小与递增次数不能为0!"));
        return;
    }

    switch (TE)
    {
    case TE_BYTE:
        DestSize = 1;
        break;
    case TE_WORD:
        DestSize = 2;
        break;
    case TE_DWORD:
        DestSize = 4;
        break;
    case TE_FLOAT:
        DestSize = 4;
        break;
    case TE_DOUBLE:
        DestSize = 8;
        break;
    case TE_CHAR: case TE_WCHAR:
        if (!TextCount)
        {
            SetStatusText(_T("字符串长度不能为0!"));
            return;
        }
        DestSize = TextCount;
        break;
    case TE_ARRAY:
        if (!ArrayCount)
        {
            SetStatusText(_T("字节数组长度不能为0!"));
            return;
        }
        DestSize = ArrayCount;
        break;
    default:
        SetStatusText(_T("未选择目标类型!"));
        return;
    }

    ListView_DlgDeleteAllItems(hDlg,IDC_LIST1);
    ListView_DlgDeleteAllItems(hDlg,IDC_LIST2);

    SendDlgItemMessage(hDlg,IDC_PROGRESS,PBM_SETRANGE32,0,(LPARAM)AddNumber);
    SendDlgItemMessage(hDlg,IDC_PROGRESS,PBM_SETSTEP,1,0);
    SendDlgItemMessage(hDlg,IDC_PROGRESS,PBM_SETPOS,0,0);
    EnableWindow(GetDlgItem(hDlg,IDC_CLOSEPROCESS),FALSE);
    EnableWindow(GetDlgItem(hDlg,IDC_SETOFFSET),FALSE);
    EnableWindow(GetDlgItem(hDlg,IDC_SETTYPE),FALSE);
    EnableWindow(GetDlgItem(hDlg,IDC_BEGINFIND),FALSE);
    EnableWindow(GetDlgItem(hDlg,IDC_STOPFIND),TRUE);

    if (FindMode < 0 || FindMode > 3) FindMode = 0;

    for (i = 0;i < AddNumber;i++)
    {
        int CurAddr;
        int TmpAddr;
        TypeUnion TU;
        SIZE_T OS;
        int l2idx = 0;
        bool succeed = false;
        TCHAR sbuff[60];

        CheckMessageQueue();
        if (IsStop) break;
        if (!ReadProcessMemory(ProcessHandle,(LPCVOID)BaseAddress,&CurAddr,4,&OS)) break;
        TU.DWord = 0;
        _stprintf_s(sbuff,60,_T("%u"),i + 1);
        AddListItem(hDlg,IDC_LIST1,i,0,sbuff);
        _stprintf_s(sbuff,60,_T("%X + %X = %X"),OffsetArray[CurOffsetIdx - 1],i * AddSize,OffsetArray[CurOffsetIdx - 1] + i * AddSize);
        AddListItem(hDlg,IDC_LIST1,i,1,sbuff);

        for (j = 1;j <= OffsetCount;j++)
        {
            if (j == CurOffsetIdx)
                CurAddr += OffsetArray[j - 1] + i * AddSize;
            else
                CurAddr += OffsetArray[j - 1];

            if (j == OffsetCount)
            {
                if (TE == TE_BYTE || TE == TE_WORD || TE == TE_DWORD || TE == TE_FLOAT || TE == TE_DOUBLE)
                {
                    if (!ReadProcessMemory(ProcessHandle,(LPCVOID)CurAddr,&TU,DestSize,&OS))
                    {
                        AddListItem(hDlg,IDC_LIST1,i,2,_T("*![读取数值失败]"));
                        break;
                    }
                }
                else if (TE == TE_CHAR || TE == TE_WCHAR)
                {
                    TU.pvoid = calloc(DestSize + 2,sizeof(TCHAR));
                    if (!TU.pvoid)
                        break;
                    if (!ReadProcessMemory(ProcessHandle,(LPCVOID)CurAddr,TU.pvoid,DestSize * sizeof(TCHAR),&OS))
                    {
                        AddListItem(hDlg,IDC_LIST1,i,2,_T("*![读取数值失败]"));
                        break;
                    }
                }
                else if (TE == TE_ARRAY)
                {
                    TU.pvoid = malloc(DestSize);
                    if (!TU.pvoid)
                        break;
                    if (!ReadProcessMemory(ProcessHandle,(LPCVOID)CurAddr,TU.pvoid,DestSize,&OS))
                    {
                        AddListItem(hDlg,IDC_LIST1,i,2,_T("*![读取数值失败]"));
                        break;
                    }
                }

                switch (TE)
                {
                case TE_BYTE:
                    _stprintf_s(sbuff,60,_T("%u"),TU.Byte);
                    AddListItem(hDlg,IDC_LIST1,i,2,sbuff);
                    if (FindMode == FM_ExactValue)
                    {
                        if (TU.Byte != D1_Byte)
                            break;
                    }
                    else if (FindMode == FM_Greater)
                    {
                        if (TU.Byte <= D1_Byte)
                            break;
                    }
                    else if (FindMode == FM_Less)
                    {
                        if (TU.Byte >= D1_Byte)
                            break;
                    }
                    else if (FindMode == FM_Range)
                    {
                        if (TU.Byte < D1_Byte || TU.Byte > D2_Byte)
                            break;
                    }
                    else break;
                    GetOffsetText(i,sbuff,60);
                    AddListItem(hDlg,IDC_LIST2,l2idx,0,sbuff);
                    _stprintf_s(sbuff,60,_T("%u"),TU.Byte);
                    AddListItem(hDlg,IDC_LIST2,l2idx,1,sbuff);
                    l2idx++;
                    break;
                case TE_WORD:
                    _stprintf_s(sbuff,60,_T("%hu"),TU.Word);
                    AddListItem(hDlg,IDC_LIST1,i,2,sbuff);
                    if (FindMode == FM_ExactValue)
                    {
                        if (TU.Word != D1_Word)
                            break;
                    }
                    else if (FindMode == FM_Greater)
                    {
                        if (TU.Word <= D1_Word)
                            break;
                    }
                    else if (FindMode == FM_Less)
                    {
                        if (TU.Word >= D1_Word)
                            break;
                    }
                    else if (FindMode == FM_Range)
                    {
                        if (TU.Word < D1_Word || TU.Word > D2_Word)
                            break;
                    }
                    else break;
                    GetOffsetText(i,sbuff,60);
                    AddListItem(hDlg,IDC_LIST2,l2idx,0,sbuff);
                    _stprintf_s(sbuff,60,_T("%hu"),TU.Word);
                    AddListItem(hDlg,IDC_LIST2,l2idx,1,sbuff);
                    l2idx++;
                    break;
                case TE_DWORD:
                    _stprintf_s(sbuff,60,_T("%u"),TU.DWord);
                    AddListItem(hDlg,IDC_LIST1,i,2,sbuff);
                    if (FindMode == FM_ExactValue)
                    {
                        if (TU.DWord != D1_DWord)
                            break;
                    }
                    else if (FindMode == FM_Greater)
                    {
                        if (TU.DWord <= D1_DWord)
                            break;
                    }
                    else if (FindMode == FM_Less)
                    {
                        if (TU.DWord >= D1_DWord)
                            break;
                    }
                    else if (FindMode == FM_Range)
                    {
                        if (TU.DWord < D1_DWord || TU.DWord > D2_DWord)
                            break;
                    }
                    else break;
                    GetOffsetText(i,sbuff,60);
                    AddListItem(hDlg,IDC_LIST2,l2idx,0,sbuff);
                    _stprintf_s(sbuff,60,_T("%u"),TU.DWord);
                    AddListItem(hDlg,IDC_LIST2,l2idx,1,sbuff);
                    l2idx++;
                    break;
                case TE_FLOAT:
                    _stprintf_s(sbuff,60,_T("%f"),TU.Float);
                    AddListItem(hDlg,IDC_LIST1,i,2,sbuff);
                    if (FindMode == FM_ExactValue)
                    {
                        if (!SF_Equal(TU.Float,D1_Float))
                            break;
                    }
                    else if (FindMode == FM_Greater)
                    {
                        if (!SF_Greater(TU.Float,D1_Float))
                            break;
                    }
                    else if (FindMode == FM_Less)
                    {
                        if (!SF_Less(TU.Float,D1_Float))
                            break;
                    }
                    else if (FindMode == FM_Range)
                    {
                        if (SF_Less(TU.Float,D1_Float) || SF_Greater(TU.Float,D2_Float))
                            break;
                    }
                    else break;
                    GetOffsetText(i,sbuff,60);
                    AddListItem(hDlg,IDC_LIST2,l2idx,0,sbuff);
                    _stprintf_s(sbuff,60,_T("%f"),TU.Float);
                    AddListItem(hDlg,IDC_LIST2,l2idx,1,sbuff);
                    l2idx++;
                    break;
                case TE_DOUBLE:
                    _stprintf_s(sbuff,60,_T("%lf"),TU.Double);
                    AddListItem(hDlg,IDC_LIST1,i,2,sbuff);
                    if (FindMode == FM_ExactValue)
                    {
                        if (!DF_Equal(TU.Double,D1_Double))
                            break;
                    }
                    else if (FindMode == FM_Greater)
                    {
                        if (!DF_Greater(TU.Double,D1_Double))
                            break;
                    }
                    else if (FindMode == FM_Less)
                    {
                        if (!DF_Less(TU.Double,D1_Double))
                            break;
                    }
                    else if (FindMode == FM_Range)
                    {
                        if (DF_Less(TU.Double,D1_Double) || DF_Greater(TU.Double,D2_Double))
                            break;
                    }
                    else break;
                    GetOffsetText(i,sbuff,60);
                    AddListItem(hDlg,IDC_LIST2,l2idx,0,sbuff);
                    _stprintf_s(sbuff,60,_T("%lf"),TU.Double);
                    AddListItem(hDlg,IDC_LIST2,l2idx,1,sbuff);
                    l2idx++;
                    break;
                case TE_CHAR:
                    AddListItemA(hDlg,IDC_LIST1,i,2,(PCHAR)TU.pvoid);
                    if (strcmp((const char *)TU.pvoid,astr) == 0)
                    {
                        GetOffsetText(i,sbuff,60);
                        AddListItem(hDlg,IDC_LIST2,l2idx,0,sbuff);
                        AddListItemA(hDlg,IDC_LIST2,l2idx,1,(PCHAR)TU.pvoid);
                        l2idx++;
                    }
                    break;
                case TE_WCHAR:
                    AddListItemW(hDlg,IDC_LIST1,i,2,(PWCHAR)TU.pvoid);
                    if (wcscmp((const wchar_t *)TU.pvoid,wstr) == 0)
                    {
                        GetOffsetText(i,sbuff,60);
                        AddListItem(hDlg,IDC_LIST2,l2idx,0,sbuff);
                        AddListItemW(hDlg,IDC_LIST2,l2idx,1,(PWCHAR)TU.pvoid);
                        l2idx++;
                    }
                    break;
                case TE_ARRAY:
                    if (memcmp(TU.pvoid,ByteArray,ArrayCount) == 0)
                    {
                        GetOffsetText(i,sbuff,60);
                        AddListItem(hDlg,IDC_LIST2,l2idx,0,sbuff);
                        AddListItem(hDlg,IDC_LIST2,l2idx,1,_T("字节数组"));
                        l2idx++;
                    }
                    break;
                default:
                    SetStatusText(_T("未选择目标类型!"));
                    return;
                }
                succeed = true;
            }
            else
            {
                if (!ReadProcessMemory(ProcessHandle,(LPCVOID)CurAddr,&TmpAddr,4,&OS))
                {
                    AddListItem(hDlg,IDC_LIST1,i,2,_T("*![读取数值失败]"));
                    break;
                }
                CurAddr = TmpAddr;
            }
        }
        if (TE == TE_CHAR || TE == TE_WCHAR || TE == TE_ARRAY)
            if (TU.pvoid)
                free(TU.pvoid);
        _stprintf_s(sbuff,60,_T("进度: %u/%u"),i + 1,AddNumber);
        SetStatusText(sbuff);
        SendDlgItemMessage(hDlg,IDC_PROGRESS,PBM_SETPOS,i + 1,0);
    }
    SetStatusText(_T("遍历完毕!"));
    SendDlgItemMessage(hDlg,IDC_PROGRESS,PBM_SETPOS,0,0);
    EnableWindow(GetDlgItem(hDlg,IDC_CLOSEPROCESS),TRUE);
    EnableWindow(GetDlgItem(hDlg,IDC_SETOFFSET),TRUE);
    EnableWindow(GetDlgItem(hDlg,IDC_SETTYPE),TRUE);
    EnableWindow(GetDlgItem(hDlg,IDC_BEGINFIND),TRUE);
    EnableWindow(GetDlgItem(hDlg,IDC_STOPFIND),FALSE);
}

void StopTraversal(HWND hDlg)
{
    IsStop = TRUE;
}