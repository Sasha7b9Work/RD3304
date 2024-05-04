@echo off

if "%1" EQU "" goto HINT

if %1==g6 goto RUN_G6
if %1==editor goto RUN_EDITOR

goto HINT

:RUN_EDITOR
    cd ../../generated/Win32/Programmer/Debug
    start Editor.exe
    cd ../../../../scripts/vs_win
    goto EXIT


:RUN_G6
    cd ../../generated/Win32/Panel/Debug
    start G6-49.exe
    cd ../../../../scripts/vs_win
    goto EXIT


:HINT
    echo.
    echo Using:
    echo        run.bat [g6^|editor]
    echo.
    goto EXIT

:EXIT
