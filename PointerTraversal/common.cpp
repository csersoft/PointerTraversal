#include "stdafx.h"

bool CheckHexText(HWND hEdit)
{
    PTCHAR stmp = NULL;
    DWORD tlen = GetWindowTextLength(hEdit);
    DWORD i;

    if (!tlen) return true;
    stmp = (PTCHAR)calloc(tlen + 1,sizeof(TCHAR));
    if (!stmp) return false;
    SendMessage(hEdit,WM_GETTEXT,tlen + 1,(LPARAM)stmp);

    for (i = 0;i <= tlen;i++)
    {
        if (stmp[i] == 0)
            break;

        if (!((stmp[i] >= 48 && stmp[i] <= 57) || (stmp[i] >= 65 && stmp[i] <= 70) || (stmp[i] >= 97 && stmp[i] <= 102)))
        {
            free(stmp);
            return false;
        }
    }
    return true;
}

void HexFilter(HWND hEdit)
{
    PTCHAR stmp = NULL;
    PTCHAR ntmp = NULL;
    DWORD i;
    DWORD nos = 0;
    DWORD tlen = GetWindowTextLength(hEdit);

    if (!tlen) return;
    if (CheckHexText(hEdit)) return;
    stmp = (PTCHAR)calloc(tlen + 1,sizeof(TCHAR));
    ntmp = (PTCHAR)calloc(tlen + 1,sizeof(TCHAR));
    if (!stmp || !ntmp)
    {
        if (stmp) free(stmp);
        if (ntmp) free(ntmp);
        return;
    }
    SendMessage(hEdit,WM_GETTEXT,tlen + 1,(LPARAM)stmp);

    for (i = 0;i <= tlen;i++)
    {
        if (stmp[i] == 0)
            break;

        if (!((stmp[i] >= 48 && stmp[i] <= 57) || (stmp[i] >= 65 && stmp[i] <= 70) || (stmp[i] >= 97 && stmp[i] <= 102)))
            continue;
        ntmp[nos++] = stmp[i];
    }
    free(stmp);
    SendMessage(hEdit,WM_SETTEXT,0,(LPARAM)ntmp);
    free(ntmp);
    SendMessage(hEdit,EM_SETSEL,nos,nos);
}