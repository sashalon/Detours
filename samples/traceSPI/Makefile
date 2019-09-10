##############################################################################
##
##  Utility to trace Win32 APIs.
##
##  Microsoft Research Detours Package
##
##  Copyright (c) Microsoft Corporation.  All rights reserved.
##

!include ..\common.mak

LIBS=$(LIBS) kernel32.lib gdi32.lib user32.lib shell32.lib advapi32.lib ole32.lib ws2_32.lib

##############################################################################

all: dirs \
    $(BIND)\trcspi$(DETOURS_BITS).dll \
!IF $(DETOURS_SOURCE_BROWSING)==1
    $(OBJD)\trcspi$(DETOURS_BITS).bsc \
!ENDIF
    option

##############################################################################

clean:
    -del *~ test.txt 2>nul
    -del $(BIND)\trcspi*.* 2>nul
    -rmdir /q /s $(OBJD) 2>nul

realclean: clean
    -rmdir /q /s $(OBJDS) 2>nul

dirs:
    @if not exist $(BIND) mkdir $(BIND) && echo.   Created $(BIND)
    @if not exist $(OBJD) mkdir $(OBJD) && echo.   Created $(OBJD)

$(OBJD)\trcspi.obj : trcspi.cpp _win32.cpp

$(OBJD)\trcspi.res : trcspi.rc

$(BIND)\trcspi$(DETOURS_BITS).dll: $(OBJD)\trcspi.obj $(OBJD)\trcspi.res $(DEPS)
    cl /LD $(CFLAGS) /Fe$@ /Fd$(@R).pdb \
        $(OBJD)\trcspi.obj $(OBJD)\trcspi.res \
        /link $(LINKFLAGS) /release /subsystem:console \
        /export:DetourFinishHelperProcess,@1,NONAME \
        $(LIBS)

$(OBJD)\trcspi$(DETOURS_BITS).bsc : $(OBJD)\trcspi.obj
    bscmake /v /n /o $@ $(OBJD)\trcspi.sbr

############################################### Install non-bit-size binaries.

!IF "$(DETOURS_OPTION_PROCESSOR)" != ""

$(OPTD)\trcspi$(DETOURS_OPTION_BITS).dll:
$(OPTD)\trcspi$(DETOURS_OPTION_BITS).pdb:

$(BIND)\trcspi$(DETOURS_OPTION_BITS).dll : $(OPTD)\trcspi$(DETOURS_OPTION_BITS).dll
    @if exist $? copy /y $? $(BIND) >nul && echo $@ copied from $(DETOURS_OPTION_PROCESSOR).
$(BIND)\trcspi$(DETOURS_OPTION_BITS).pdb : $(OPTD)\trcspi$(DETOURS_OPTION_BITS).pdb
    @if exist $? copy /y $? $(BIND) >nul && echo $@ copied from $(DETOURS_OPTION_PROCESSOR).

option: \
    $(BIND)\trcspi$(DETOURS_OPTION_BITS).dll \
    $(BIND)\trcspi$(DETOURS_OPTION_BITS).pdb \

!ELSE

option:

!ENDIF

##############################################################################

test: all
    @echo -------- Logging output to test.txt ------------
    start $(BIND)\syelogd.exe /o test.txt
    $(BIND)\sleep5.exe 1
    @echo -------- Should load trcspi$(DETOURS_BITS).dll dynamically using withdll.exe ------------
    $(BIND)\withdll -d:$(BIND)\trcspi$(DETOURS_BITS).dll $(BIND)\sleepold.exe
    @echo -------- Log from syelog -------------
    type test.txt

debug: all
    @echo -------- Logging output to test.txt ------------
    start $(BIND)\syelogd.exe /o test.txt
    $(BIND)\sleep5.exe 1
    @echo -------- Should load trcspi$(DETOURS_BITS).dll dynamically using withdll.exe ------------
    windbg -o -g -G $(BIND)\withdll -d:$(BIND)\trcspi$(DETOURS_BITS).dll $(BIND)\sleepold.exe
    @echo -------- Log from syelog -------------
    type test.txt

calc: all
    @echo -------- Logging output to test.txt ------------
    start $(BIND)\syelogd.exe /q /o test.txt
    $(BIND)\sleep5.exe 1
    @echo -------- Should load trcspi$(DETOURS_BITS).dll dynamically using withdll.exe ------------
    $(BIND)\withdll -d:$(BIND)\trcspi$(DETOURS_BITS).dll $(SYSTEMROOT)\System32\calc.exe
    @echo -------- Log from syelog -------------
    wc test.txt

explorer: all
    @echo -------- Logging output to test.txt ------------
    start $(BIND)\syelogd.exe /q /o test.txt
    $(BIND)\sleep5.exe 1
    @echo -------- Should load trcspi$(DETOURS_BITS).dll dynamically using withdll.exe ------------
    $(BIND)\withdll -d:$(BIND)\trcspi$(DETOURS_BITS).dll $(SYSTEMROOT)\explorer.exe
    @echo -------- Log from syelog -------------
    wc test.txt

wordpad: all
    @echo -------- Logging output to test.txt ------------
    start $(BIND)\syelogd.exe /q /o test.txt
    $(BIND)\sleep5.exe 1
    @echo -------- Should load trcspi$(DETOURS_BITS).dll dynamically using withdll.exe ------------
    $(BIND)\withdll -d:$(BIND)\trcspi$(DETOURS_BITS).dll $(PROGRAMFILES)\Windows NT\Accessories\wordpad.exe
    @echo -------- Log from syelog -------------
    wc test.txt

notepad: all
    @echo -------- Logging output to test.txt ------------
    start $(BIND)\syelogd.exe /q /o test.txt
    $(BIND)\sleep5.exe 1
    @echo -------- Should load trcspi$(DETOURS_BITS).dll dynamically using withdll.exe ------------
    $(BIND)\withdll -d:$(BIND)\trcspi$(DETOURS_BITS).dll $(SYSTEMROOT)\notepad.exe
    @echo -------- Log from syelog -------------
    wc test.txt

ie: all
    @echo -------- Logging output to test.txt ------------
    start $(BIND)\syelogd.exe /q /o test.txt
    $(BIND)\sleep5.exe 1
    @echo -------- Should load trcspi$(DETOURS_BITS).dll dynamically using withdll.exe ------------
    windbg -g -G -o $(BIND)\withdll -d:$(BIND)\trcspi$(DETOURS_BITS).dll "$(PROGRAMFILES)\Internet Explorer\iexplore.exe"
    @echo -------- Log from syelog -------------
    wc test.txt

ws: all
    $(BIND)\withdll -d:$(BIND)\trcspi$(DETOURS_BITS).dll $(BIND)\WebServiceTester.exe

################################################################# End of File.
