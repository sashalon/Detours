//////////////////////////////////////////////////////////////////////////////
//
//  Detours Test Program (_win32.cpp of traceapi.dll)
//
//  Microsoft Research Detours Package
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//

///////////////////////////////////////////////////////////////// Trampolines.
//

BOOL (__stdcall * Real_SystemParametersInfoA)(UINT a0,
                                              UINT a1,
                                              PVOID a2,
                                              UINT a3)
    = SystemParametersInfoA;

BOOL (__stdcall * Real_SystemParametersInfoW)(UINT a0,
                                              UINT a1,
                                              PVOID a2,
                                              UINT a3)
    = SystemParametersInfoW;

UINT_PTR(__stdcall *Real_SHAppBarMessage)(
	DWORD       dwMessage,
	PAPPBARDATA pData)
	= SHAppBarMessage;

///////////////////////////////////////////////////////////////////// Detours.
//
BOOL bProcessDetailsDumped = FALSE;
BOOL __stdcall Mine_SystemParametersInfoA(UINT a0,
	UINT a1,
	PVOID a2,
	UINT a3)
{
	if (a0 == 0x2f)
	{
		if (!bProcessDetailsDumped)
		{
			bProcessDetailsDumped = TRUE;
			ProcessEnumerate();
		}
		RECT* rc = (RECT*)a2;
		int x, y, w, h;
		x = y = w = h = -1;
		if (rc != NULL)
		{
			__try {

				x = rc->left;
				y = rc->top;
				w = rc->right;
				h = rc->bottom;
			}
			__finally {};
		}
		DbgPrintFile(0, "SystemParametersInfoA(%x, %p, %p -> (%d,%d)-(%d,%d) ,%p)\n", a0, a1, a2, x, y, w, h, a3);
	}

	BOOL rv = 0;
	__try {
		rv = Real_SystemParametersInfoA(a0, a1, a2, a3);
	}
	__finally {
		if(a0 == 0x2f)
			DbgPrintFile(0, "SystemParametersInfoA(,,,) -> %x\n", rv);
	};
	return rv;
}

BOOL __stdcall Mine_SystemParametersInfoW(UINT a0,
	UINT a1,
	PVOID a2,
	UINT a3)
{
	if (a0 == 0x2f)
	{
		if (!bProcessDetailsDumped)
		{
			bProcessDetailsDumped = TRUE;
			ProcessEnumerate();
		}
		RECT* rc = (RECT*)a2;
		int x, y, w, h;
		x = y = w = h = -1;
		if (rc != NULL)
		{
			__try {

				x = rc->left;
				y = rc->top;
				w = rc->right;
				h = rc->bottom;
			}
			__finally {};
		}
		DbgPrintFile(0, "SystemParametersInfoW(%x, %p, %p -> (%d,%d)-(%d,%d) ,%p)\n", a0, a1, a2, x, y, w, h, a3);
	}

	BOOL rv = 0;
	__try {
		rv = Real_SystemParametersInfoW(a0, a1, a2, a3);
	}
	__finally {
		if (a0 == 0x2f)
			DbgPrintFile(0, "SystemParametersInfoW(,,,) -> %x\n", rv);
	};
	return rv;
}

UINT_PTR __stdcall Mine_SHAppBarMessage(
	DWORD       dwMessage,
	PAPPBARDATA pData)
{
	if (!bProcessDetailsDumped)
	{
		bProcessDetailsDumped = TRUE;
		ProcessEnumerate();
	}
	APPBARDATA abm;
	abm.cbSize = 0;
	abm.hWnd = NULL;
	abm.uCallbackMessage = 0;
	abm.uEdge = 0;;
	abm.rc.left = abm.rc.right = abm.rc.top = abm.rc.bottom = -1;
	abm.lParam = 0;
	
	if (pData != NULL)
	{
		abm.cbSize = pData->cbSize;
		abm.hWnd = pData->hWnd;
		abm.uCallbackMessage = pData->uCallbackMessage;
		abm.uEdge = pData->uEdge;
		abm.rc.left = pData->rc.left;
		abm.rc.right = pData->rc.right;
		abm.rc.top = pData->rc.top;
		abm.rc.bottom = pData->rc.bottom;
		abm.lParam = pData->lParam;
	}
	DbgPrintFile(0, "--> SHAppBarMessage(%x, cbSize=%d, hWnd=%x, cbMsg=%x, uEdge=%d, (%d,%d)-(%d,%d), lParam=%x\n", 
		dwMessage, abm.cbSize, abm.hWnd, abm.uCallbackMessage, abm.uEdge, abm.rc.left, abm.rc.top, abm.rc.right, abm.rc.bottom, abm.lParam);

	UINT_PTR rv = 0;
	__try {
		rv = Real_SHAppBarMessage(dwMessage, pData);
		if (pData != NULL)
		{
			abm.cbSize = pData->cbSize;
			abm.hWnd = pData->hWnd;
			abm.uCallbackMessage = pData->uCallbackMessage;
			abm.uEdge = pData->uEdge;
			abm.rc.left = pData->rc.left;
			abm.rc.right = pData->rc.right;
			abm.rc.top = pData->rc.top;
			abm.rc.bottom = pData->rc.bottom;
			abm.lParam = pData->lParam;
		}
		DbgPrintFile(0, "(%x)<--SHAppBarMessage(%x, cbSize=%d, hWnd=%x, cbMsg=%x, uEdge=%d, (%d,%d)-(%d,%d), lParam=%x\n", rv,
			dwMessage, abm.cbSize, abm.hWnd, abm.uCallbackMessage, abm.uEdge, abm.rc.left, abm.rc.top, abm.rc.right, abm.rc.bottom, abm.lParam);
	}
	__finally {
	};
	return rv;
}

////////////////////////////////////////////////////////////// AttachDetours.
//
static PCHAR DetRealName(PCHAR psz)
{
    PCHAR pszBeg = psz;
    // Move to end of name.
    while (*psz) {
        psz++;
    }
    // Move back through A-Za-z0-9 names.
    while (psz > pszBeg &&
           ((psz[-1] >= 'A' && psz[-1] <= 'Z') ||
            (psz[-1] >= 'a' && psz[-1] <= 'z') ||
            (psz[-1] >= '0' && psz[-1] <= '9'))) {
        psz--;
    }
    return psz;
}

static VOID Dump(PBYTE pbBytes, LONG nBytes, PBYTE pbTarget)
{
    CHAR szBuffer[256];
    PCHAR pszBuffer = szBuffer;

    for (LONG n = 0; n < nBytes; n += 12) {
        pszBuffer += StringCchPrintfA(pszBuffer, sizeof(szBuffer), "  %p: ", pbBytes + n);
        for (LONG m = n; m < n + 12; m++) {
            if (m >= nBytes) {
                pszBuffer += StringCchPrintfA(pszBuffer, sizeof(szBuffer), "   ");
            }
            else {
                pszBuffer += StringCchPrintfA(pszBuffer, sizeof(szBuffer), "%02x ", pbBytes[m]);
            }
        }
        if (n == 0) {
            pszBuffer += StringCchPrintfA(pszBuffer, sizeof(szBuffer), "[%p]", pbTarget);
        }
        pszBuffer += StringCchPrintfA(pszBuffer, sizeof(szBuffer), "\n");
    }

    Syelog(SYELOG_SEVERITY_INFORMATION, "%s", szBuffer);
}

static VOID Decode(PBYTE pbCode, LONG nInst)
{
    PBYTE pbSrc = pbCode;
    PBYTE pbEnd;
    PBYTE pbTarget;
    for (LONG n = 0; n < nInst; n++) {
        pbTarget = NULL;
        pbEnd = (PBYTE)DetourCopyInstruction(NULL, NULL, (PVOID)pbSrc, (PVOID*)&pbTarget, NULL);
        Dump(pbSrc, (int)(pbEnd - pbSrc), pbTarget);
        pbSrc = pbEnd;

        if (pbTarget != NULL) {
            break;
        }
    }
}

VOID DetAttach(PVOID *ppvReal, PVOID pvMine, PCHAR psz)
{
    PVOID pvReal = NULL;
    if (ppvReal == NULL) {
        ppvReal = &pvReal;
    }

    LONG l = DetourAttach(ppvReal, pvMine);
    if (l != 0) {
        Syelog(SYELOG_SEVERITY_NOTICE,
               "Attach failed: `%s': error %d\n", DetRealName(psz), l);

        Decode((PBYTE)*ppvReal, 3);
    }
}

VOID DetDetach(PVOID *ppvReal, PVOID pvMine, PCHAR psz)
{
    LONG l = DetourDetach(ppvReal, pvMine);
    if (l != 0) {
#if 0
        Syelog(SYELOG_SEVERITY_NOTICE,
               "Detach failed: `%s': error %d\n", DetRealName(psz), l);
#else
        (void)psz;
#endif
    }
}

#define ATTACH(x)       DetAttach(&(PVOID&)Real_##x,Mine_##x,#x)
#define DETACH(x)       DetDetach(&(PVOID&)Real_##x,Mine_##x,#x)

LONG AttachDetours(VOID)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    // For this many APIs, we'll ignore one or two can't be detoured.
    DetourSetIgnoreTooSmall(TRUE);

    ATTACH(SystemParametersInfoA);
    ATTACH(SystemParametersInfoW);
    ATTACH(SHAppBarMessage);

    PVOID *ppbFailedPointer = NULL;
    LONG error = DetourTransactionCommitEx(&ppbFailedPointer);
    if (error != 0) {
        printf("traceapi.dll: Attach transaction failed to commit. Error %d (%p/%p)",
               error, ppbFailedPointer, *ppbFailedPointer);
        return error;
    }
    return 0;
}

LONG DetachDetours(VOID)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    // For this many APIs, we'll ignore one or two can't be detoured.
    DetourSetIgnoreTooSmall(TRUE);

    DETACH(SystemParametersInfoA);
    DETACH(SystemParametersInfoW);
    DETACH(SHAppBarMessage);

    if (DetourTransactionCommit() != 0) {
        PVOID *ppbFailedPointer = NULL;
        LONG error = DetourTransactionCommitEx(&ppbFailedPointer);

        printf("traceapi.dll: Detach transaction failed to commit. Error %d (%p/%p)",
               error, ppbFailedPointer, *ppbFailedPointer);
        return error;
    }
    return 0;
}
//
///////////////////////////////////////////////////////////////// End of File.
