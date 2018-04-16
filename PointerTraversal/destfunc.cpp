#include "stdafx.h"

TYPE_ENUM TE = TE_NONE;

BYTE SJD = 5;
BYTE DJD = 10;
BYTE FindMode = 0;
bool HexMode = false;
WNDPROC DEP = NULL;

BYTE        D1_Byte = NULL;
WORD        D1_Word = NULL;
DWORD       D1_DWord = NULL;
float       D1_Float = NULL;
double      D1_Double = NULL;

BYTE        D2_Byte = NULL;
WORD        D2_Word = NULL;
DWORD       D2_DWord = NULL;
float       D2_Float = NULL;
double      D2_Double = NULL;

PCHAR       astr = NULL;
PWCHAR      wstr = NULL;
PBYTE       ByteArray = NULL;

DWORD       TextCount = 0;
DWORD       ArrayCount = 0;

LRESULT CALLBACK DataEditProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  if (message == WM_CHAR)
  {
    if (TE != TE_NONE)
    {
      switch (TE)
      {
        case TE_BYTE: case TE_WORD: case TE_DWORD:
        if (HexMode)
        {
          if (!((wParam >= 48 && wParam <= 57) || (wParam >= 65 && wParam <= 70) || (wParam >= 97 && wParam <= 102) || wParam == 0x08))
            return 0;
        }
        else
        {
          if (!((wParam >= 48 && wParam <= 57) || wParam == 45 || wParam == 0x08))
            return 0;
        }
        break;
      }
    }
    else
    {
      MessageBox(hDlg, _T("请先选择数据类型!"), _T("警告"), MB_ICONWARNING);
      return 0;
    }
  }
  return CallWindowProc(DEP, hDlg, message, wParam, lParam);
}

int GetEditText(HWND hDlg, int DlgItem, PTCHAR &pstr)
{
  DWORD slen = GetWindowTextLength(GetDlgItem(hDlg, DlgItem));
  if (!slen) return false;

  pstr = (PTCHAR)calloc(slen + 1, sizeof(TCHAR));
  return GetWindowText(GetDlgItem(hDlg, DlgItem), pstr, slen + 1);
}

void SetData2(HWND hDlg, BOOL sbool)
{
  EnableWindow(GetDlgItem(hDlg, IDC_SDATA2), sbool);
  EnableWindow(GetDlgItem(hDlg, IDC_DE2), sbool);
}

PTCHAR GetHexText(PBYTE pByte, DWORD Count)
{
  if (!Count) return NULL;

  PTCHAR ptcs = (PTCHAR)calloc(Count * 2 + 1, sizeof(TCHAR));
  DWORD i;
  DWORD itmp;
  TCHAR tcstmp[4];

  if (!ptcs) return NULL;

  for (i = 0; i < Count; i++)
  {
    itmp = pByte[i];
    _stprintf_s(tcstmp, 4, _T("%02X"), itmp);
    _tcscat_s(ptcs, Count * 2 + 1, tcstmp);
  }
  return ptcs;
}

void UpdateControl(HWND hDlg)
{
  TCHAR stmp[20];
  DWORD dtmp;

  switch (TE)
  {
    case TE_BYTE: case TE_WORD: case TE_DWORD: case TE_FLOAT: case TE_DOUBLE:
    EnableWindow(GetDlgItem(hDlg, IDC_Accurate), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDC_Greater), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDC_Less), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDC_Range), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDC_HEXVAL), TRUE);
    SendDlgItemMessage(hDlg, IDC_HEXVAL, BM_SETCHECK, HexMode, 0);
    if (TE == TE_BYTE)
    {
      dtmp = D1_Byte;
      _stprintf_s(stmp, 20, _T("%u"), dtmp);
      SendDlgItemMessage(hDlg, IDC_DE1, WM_SETTEXT, 0, (LPARAM)stmp);
      dtmp = D2_Byte;
      _stprintf_s(stmp, 20, _T("%u"), dtmp);
      SendDlgItemMessage(hDlg, IDC_DE2, WM_SETTEXT, 0, (LPARAM)stmp);
    }
    else if (TE == TE_WORD)
    {
      _stprintf_s(stmp, 20, _T("%hu"), D1_Word);
      SendDlgItemMessage(hDlg, IDC_DE1, WM_SETTEXT, 0, (LPARAM)stmp);
      _stprintf_s(stmp, 20, _T("%hu"), D1_Word);
      SendDlgItemMessage(hDlg, IDC_DE2, WM_SETTEXT, 0, (LPARAM)stmp);
    }
    else if (TE == TE_DWORD)
    {
      _stprintf_s(stmp, 20, _T("%u"), D1_DWord);
      SendDlgItemMessage(hDlg, IDC_DE1, WM_SETTEXT, 0, (LPARAM)stmp);
      _stprintf_s(stmp, 20, _T("%u"), D1_DWord);
      SendDlgItemMessage(hDlg, IDC_DE2, WM_SETTEXT, 0, (LPARAM)stmp);
    }
    break;
    case TE_CHAR: case TE_WCHAR: case TE_ARRAY:
    FindMode = 0;
    SetData2(hDlg, FALSE);
    SendDlgItemMessage(hDlg, IDC_Accurate, BM_SETCHECK, BST_CHECKED, 0);
    SendDlgItemMessage(hDlg, IDC_Greater, BM_SETCHECK, BST_UNCHECKED, 0);
    SendDlgItemMessage(hDlg, IDC_Less, BM_SETCHECK, BST_UNCHECKED, 0);
    SendDlgItemMessage(hDlg, IDC_Range, BM_SETCHECK, BST_UNCHECKED, 0);
    EnableWindow(GetDlgItem(hDlg, IDC_Accurate), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDC_Greater), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_Less), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_Range), FALSE);
    if (TE == TE_ARRAY)
    {
      if (ByteArray && ArrayCount)
      {
        PTCHAR ptmp = GetHexText(ByteArray, ArrayCount);
        SendDlgItemMessage(hDlg, IDC_DE1, WM_SETTEXT, 0, (LPARAM)ptmp);
        free(ptmp);
      }
      SendDlgItemMessage(hDlg, IDC_HEXVAL, BM_SETCHECK, BST_INDETERMINATE, 0);
      HexMode = true;
      EnableWindow(GetDlgItem(hDlg, IDC_HEXVAL), FALSE);
    }
    else if (TE == TE_CHAR)
    {
      if (astr && TextCount)
        SendDlgItemMessageA(hDlg, IDC_DE1, WM_SETTEXT, 0, (LPARAM)astr);
      EnableWindow(GetDlgItem(hDlg, IDC_HEXVAL), TRUE);
      SendDlgItemMessage(hDlg, IDC_HEXVAL, BM_SETCHECK, HexMode, 0);
    }
    else if (TE == TE_WCHAR)
    {
      if (wstr && TextCount)
        SendDlgItemMessageW(hDlg, IDC_DE1, WM_SETTEXT, 0, (LPARAM)wstr);
      EnableWindow(GetDlgItem(hDlg, IDC_HEXVAL), TRUE);
      SendDlgItemMessage(hDlg, IDC_HEXVAL, BM_SETCHECK, HexMode, 0);
    }

    break;
    case TE_NONE:
    EnableWindow(GetDlgItem(hDlg, IDC_Accurate), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_Greater), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_Less), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_Range), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_HEXVAL), FALSE);
    break;
  }

  if (SJD < 1 || SJD > 6) SJD = 5;
  if (DJD < 1 || DJD > 14) SJD = 10;

  _stprintf_s(stmp, 20, _T("%u"), SJD);
  SendDlgItemMessage(hDlg, IDC_SJDE, WM_SETTEXT, 0, (LPARAM)stmp);
  _stprintf_s(stmp, 20, _T("%u"), DJD);
  SendDlgItemMessage(hDlg, IDC_DJDE, WM_SETTEXT, 0, (LPARAM)stmp);
}

void UpdateDestData(HWND hDlg)
{
  PTCHAR pstr1 = NULL;
  PTCHAR pstr2 = NULL;
  int tslen;
  int itmp;
  int rv;
  TCHAR etmp[4];

  tslen = GetWindowTextLength(GetDlgItem(hDlg, IDC_DE1));
  if (!tslen) return;
  rv = tslen;
  pstr1 = (PTCHAR)calloc(tslen + 1, sizeof(TCHAR));
  GetWindowText(GetDlgItem(hDlg, IDC_DE1), pstr1, tslen + 1);

  if (FindMode == FM_Range)
  {
    tslen = GetWindowTextLength(GetDlgItem(hDlg, IDC_DE2));
    if (tslen)
    {
      pstr2 = (PTCHAR)calloc(tslen + 1, sizeof(TCHAR));
      GetWindowText(GetDlgItem(hDlg, IDC_DE2), pstr2, tslen + 1);
    }
  }

  switch (TE)
  {
    case TE_BYTE:
    if (HexMode)
    {
      if (!_stscanf_s(pstr1, _T("%x"), &itmp))
        break;
    }
    else
    {
      if (!_stscanf_s(pstr1, _T("%d"), &itmp))
        break;
    }

    __asm
    {
      mov eax, itmp
      mov D1_Byte, al
    }

    if (FindMode == FM_Range && pstr2 != NULL)
    {
      if (HexMode)
      {
        if (!_stscanf_s(pstr2, _T("%x"), &itmp))
          break;
      }
      else
      {
        if (!_stscanf_s(pstr2, _T("%d"), &itmp))
          break;
      }

      __asm
      {
        mov eax, itmp
        mov D2_Byte, al
      }
    }
    break;
    case TE_WORD:
    if (HexMode)
    {
      if (!_stscanf_s(pstr1, _T("%hx"), &D1_Word))
        break;
    }
    else
    {
      if (!_stscanf_s(pstr1, _T("%hd"), &D1_Word))
        break;
    }

    if (FindMode == FM_Range && pstr2 != NULL)
    {
      if (HexMode)
      {
        if (!_stscanf_s(pstr2, _T("%hx"), &D2_Word))
          break;
      }
      else
      {
        if (!_stscanf_s(pstr2, _T("%hd"), &D2_Word))
          break;
      }
    }
    break;
    case TE_DWORD:
    if (HexMode)
    {
      if (!_stscanf_s(pstr1, _T("%x"), &D1_DWord))
        break;
    }
    else
    {
      if (!_stscanf_s(pstr1, _T("%d"), &D1_DWord))
        break;
    }

    if (FindMode == FM_Range && pstr2 != NULL)
    {
      if (HexMode)
      {
        if (!_stscanf_s(pstr2, _T("%x"), &D2_DWord))
          break;
      }
      else
      {
        if (!_stscanf_s(pstr1, _T("%d"), &D2_DWord))
          break;
      }
    }
    break;
    case TE_FLOAT:
    if (HexMode)
    {
      if (!_stscanf_s(pstr1, _T("%x"), &D1_Float))
        break;
    }
    else
      D1_Float = (float)_ttof(pstr1);

    if (FindMode == FM_Range && pstr2 != NULL)
    {
      if (HexMode)
      {
        if (!_stscanf_s(pstr2, _T("%x"), &D2_Float))
          break;
      }
      else
        D2_Float = (float)_ttof(pstr2);
    }
    break;
    case TE_DOUBLE:
    if (HexMode)
    {
      if (!_stscanf_s(pstr1, _T("%I64x"), &D1_Double))
        break;
    }
    else
      D1_Double = _ttof(pstr1);

    if (FindMode == FM_Range && pstr2 != NULL)
    {
      if (HexMode)
      {
        if (!_stscanf_s(pstr2, _T("%I64x"), &D2_Double))
          break;
      }
      else
        D2_Double = _ttof(pstr2);
    }
    break;
    case TE_CHAR:
    if (astr) free(astr);

#ifdef _UNICODE
    astr = (PCHAR)calloc(rv + 1, 1);
    if (!astr)
    {
      SetStatusText(_T("申请内存失败!"));
      break;
    }
    wcstombs_s((size_t *)&itmp, astr, rv + 1, pstr1, _TRUNCATE);
#else
    astr = (PCHAR)calloc(rv + 1, 1);
    if (!astr)
    {
      SetStatusText(_T("申请内存失败!"));
      break;
    }
    strcpy_s(astr, rv + 1, pstr1);
#endif
    TextCount = rv;
    break;
    case TE_WCHAR:
    if (wstr) free(wstr);

#ifdef _UNICODE
    wstr = (PWCHAR)calloc(rv + 1, 2);
    if (!wstr)
    {
      SetStatusText(_T("申请内存失败!"));
      break;
    }
    wcscpy_s(wstr, rv + 1, pstr1);
#else
    wstr = (PCHAR)calloc(rv + 1, 1);
    if (!wstr)
    {
      SetStatusText(_T("申请内存失败!"));
      break;
    }
    mbstowcs_s((size_t *)&itmp, wstr, rv + 1, pstr1, _TRUNCATE);
#endif
    TextCount = rv;
    break;
    case TE_ARRAY:
    {
      if (ByteArray) free(ByteArray);
      PTCHAR stmp = (PTCHAR)calloc(rv + 1, sizeof(TCHAR));
      PBYTE btmp = NULL;
      DWORD slen;
      DWORD i;

      if (!stmp) break;
      GetWindowText(GetDlgItem(hDlg, IDC_DE1), stmp, rv + 1);
      slen = _tcslen(stmp);
      if (!slen)
      {
        free(stmp);
        break;
      }

      if (slen % 2)
      {
        free(pstr1);
        pstr1 = (PTCHAR)calloc(slen + 2, sizeof(TCHAR));
        if (!pstr1) break;
        _tcscpy_s(pstr1, slen + 2, _T("0\0"));
        _tcscat_s(pstr1, slen + 2, stmp);
        free(stmp);
      }
      else
        free(stmp);

      ArrayCount = _tcslen(pstr1) / 2;
      if (!ArrayCount) break;
      ByteArray = (PBYTE)calloc(ArrayCount, 1);
      if (!ByteArray)
      {
        SetStatusText(_T("申请内存失败!"));
        break;
      }
      for (i = 0; i < ArrayCount; i++)
      {
        stmp = pstr1 + i * 2;
        if (_stscanf_s(stmp, _T("%2x"), &itmp) == 1)
        {
          btmp = &ByteArray[i];
          __asm
          {
            mov eax, itmp
            mov ebx, btmp
              mov byte ptr[ebx], al
          }
        }
      }
    }
    break;
    default:
    break;
  }

  if (pstr1) free(pstr1);
  if (pstr2) free(pstr2);

  if (GetWindowText(GetDlgItem(hDlg, IDC_SJDE), etmp, 4))
  {
    _stscanf_s(etmp, _T("%u"), &itmp);
    if (itmp < 1 || itmp > 6) itmp = 5;
    SINGLEEPSION = pow(0.1f, itmp);
  }

  if (GetWindowText(GetDlgItem(hDlg, IDC_DJDE), etmp, 4))
  {
    _stscanf_s(etmp, _T("%u"), &itmp);
    if (itmp < 1 || itmp > 14) itmp = 10;
    DOUBLEEPSION = pow(0.1, itmp);
  }
}

bool CheckEditText(HWND hEdit)
{
  PTCHAR stmp = NULL;
  DWORD tlen = GetWindowTextLength(hEdit);
  DWORD i;

  if (!tlen) return true;
  stmp = (PTCHAR)calloc(tlen + 1, sizeof(TCHAR));
  if (!stmp) return false;
  SendMessage(hEdit, WM_GETTEXT, tlen + 1, (LPARAM)stmp);

  for (i = 0; i <= tlen; i++)
  {
    if (stmp[i] == 0)
      break;

    if (HexMode)
    {
      if (!((stmp[i] >= 48 && stmp[i] <= 57) || (stmp[i] >= 65 && stmp[i] <= 70) || (stmp[i] >= 97 && stmp[i] <= 102)))
      {
        free(stmp);
        return false;
      }
    }
    else
    {
      switch (TE)
      {
        case TE_BYTE: case TE_WORD: case TE_DWORD:
        if (!((stmp[i] >= 48 && stmp[i] <= 57) || stmp[i] == 45))
        {
          free(stmp);
          return false;
        }
        case TE_FLOAT: case TE_DOUBLE:
        if (!((stmp[i] >= 48 && stmp[i] <= 57) || stmp[i] == 45 || stmp[i] == 46 || stmp[i] == 69 || stmp[i] == 101))
        {
          free(stmp);
          return false;
        }
        case TE_ARRAY:
        if (!((stmp[i] >= 48 && stmp[i] <= 57) || (stmp[i] >= 65 && stmp[i] <= 70) || (stmp[i] >= 97 && stmp[i] <= 102)))
        {
          free(stmp);
          return false;
        }
        default:
        free(stmp);
        return true;
      }
    }
  }
  return true;
}

void TextFilter(HWND hEdit)
{
  PTCHAR stmp = NULL;
  PTCHAR ntmp = NULL;
  DWORD i;
  DWORD nos = 0;
  DWORD tlen = GetWindowTextLength(hEdit);

  if (!tlen) return;
  if (CheckEditText(hEdit)) return;
  if (TE == TE_CHAR || TE == TE_WCHAR) return;
  stmp = (PTCHAR)calloc(tlen + 1, sizeof(TCHAR));
  ntmp = (PTCHAR)calloc(tlen + 1, sizeof(TCHAR));
  if (!stmp || !ntmp) return;

  SendMessage(hEdit, WM_GETTEXT, tlen + 1, (LPARAM)stmp);

  for (i = 0; i <= tlen; i++)
  {
    if (stmp[i] == 0)
      break;

    if (HexMode)
    {
      if (!((stmp[i] >= 48 && stmp[i] <= 57) || (stmp[i] >= 65 && stmp[i] <= 70) || (stmp[i] >= 97 && stmp[i] <= 102)))
        continue;
    }
    else
    {
      switch (TE)
      {
        case TE_BYTE: case TE_WORD: case TE_DWORD:
        if (!((stmp[i] >= 48 && stmp[i] <= 57) || stmp[i] == 45))
          continue;
        break;
        case TE_FLOAT: case TE_DOUBLE:
        if (!((stmp[i] >= 48 && stmp[i] <= 57) || stmp[i] == 45 || stmp[i] == 46 || stmp[i] == 69 || stmp[i] == 101))
          continue;
        break;
        case TE_ARRAY:
        if (!((stmp[i] >= 48 && stmp[i] <= 57) || (stmp[i] >= 65 && stmp[i] <= 70) || (stmp[i] >= 97 && stmp[i] <= 102)))
          continue;
        break;
        default:
        free(stmp);
        free(ntmp);
        return;
      }
    }
    ntmp[nos++] = stmp[i];
  }
  free(stmp);
  SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)ntmp);
  free(ntmp);
  SendMessage(hEdit, EM_SETSEL, nos, nos);
}

void InitForm(HWND hDlg)
{
  SendDlgItemMessage(hDlg, IDC_CB_TYPE, CB_ADDSTRING, 0, (LPARAM)_T("  Byte    1字节,整数"));
  SendDlgItemMessage(hDlg, IDC_CB_TYPE, CB_ADDSTRING, 0, (LPARAM)_T("  Word    2字节,整数"));
  SendDlgItemMessage(hDlg, IDC_CB_TYPE, CB_ADDSTRING, 0, (LPARAM)_T("  DWord    4字节,整数"));
  SendDlgItemMessage(hDlg, IDC_CB_TYPE, CB_ADDSTRING, 0, (LPARAM)_T("  Float    4字节,单精度浮点"));
  SendDlgItemMessage(hDlg, IDC_CB_TYPE, CB_ADDSTRING, 0, (LPARAM)_T("  Double    8字节,双精度浮点"));
  SendDlgItemMessage(hDlg, IDC_CB_TYPE, CB_ADDSTRING, 0, (LPARAM)_T("  char[]    ASCII字符串"));
  SendDlgItemMessage(hDlg, IDC_CB_TYPE, CB_ADDSTRING, 0, (LPARAM)_T("  WCHAR[]    UTF-16字符串"));
  SendDlgItemMessage(hDlg, IDC_CB_TYPE, CB_ADDSTRING, 0, (LPARAM)_T("  byte[]    字节数组"));
  SendDlgItemMessage(hDlg, IDC_Accurate, BM_SETCHECK, BST_CHECKED, 0);
  SendDlgItemMessage(hDlg, IDC_SJDE, EM_SETLIMITTEXT, 1, 0);
  SendDlgItemMessage(hDlg, IDC_DJDE, EM_SETLIMITTEXT, 2, 0);
  SendDlgItemMessage(hDlg, IDC_CB_TYPE, CB_SETCURSEL, TE, 0);
  DEP = (WNDPROC)SetWindowLong(GetDlgItem(hDlg, IDC_DE1), GWL_WNDPROC, (LONG)DataEditProc);
  SetWindowLong(GetDlgItem(hDlg, IDC_DE2), GWL_WNDPROC, (LONG)DataEditProc);
  UpdateControl(hDlg);
}

INT_PTR CALLBACK DestProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;

  switch (message)
  {
    case WM_INITDIALOG:
    InitForm(hDlg);
    return TRUE;
    case WM_COMMAND:
    wmId = LOWORD(wParam);
    wmEvent = HIWORD(wParam);
    switch (wmId)
    {
      case IDC_Accurate:
      if (wmEvent == BN_CLICKED)
      {
        FindMode = FM_ExactValue;
        SetData2(hDlg, FALSE);
      }
      break;
      case IDC_Greater:
      if (wmEvent == BN_CLICKED)
      {
        FindMode = FM_Greater;
        SetData2(hDlg, FALSE);
      }
      break;
      case IDC_Less:
      if (wmEvent == BN_CLICKED)
      {
        FindMode = FM_Less;
        SetData2(hDlg, FALSE);
      }
      break;
      case IDC_Range:
      if (wmEvent == BN_CLICKED)
      {
        FindMode = FM_Range;
        SetData2(hDlg, TRUE);
      }
      break;
      case IDC_CB_TYPE:
      if (wmEvent == CBN_SELCHANGE)
      {
        TE = (TYPE_ENUM)SendDlgItemMessage(hDlg, IDC_CB_TYPE, CB_GETCURSEL, 0, 0);
        UpdateControl(hDlg);
      }
      break;
      case IDC_HEXVAL:
      if (wmEvent == BN_CLICKED)
      {
        if (SendDlgItemMessage(hDlg, IDC_HEXVAL, BM_GETCHECK, 0, 0) == BST_CHECKED)
          HexMode = true;
        else
          HexMode = false;
      }
      break;
      case IDC_DE1: case IDC_DE2:
      if (wmEvent == EN_UPDATE)
        TextFilter((HWND)lParam);
      break;
    }
    break;
    case WM_CLOSE:
    UpdateDestData(hDlg);
    EndDialog(hDlg, 1);
    break;
  }
  return 0;
}