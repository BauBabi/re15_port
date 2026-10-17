@echo off
rem =============================================================================
rem RE1.5 Port - Release fuer alle Systeme per Doppelklick (Runde 38, Spur V)
rem =============================================================================
rem Startet release\release_alles.sh in Git-Bash - nie in C:\Windows\System32\bash.exe (WSL; dort
rem fehlen docker.exe, cygpath und die Windows-Pfade, re15_port/tools/local_build.sh-Kopf).
rem
rem   Doppelklick (ohne Argumente): erst der Trockenlauf (Version, Pruefungen, Schritte), dann die Wahl
rem     [1] bauen + packen + archivieren, [2] dazu am Ende Commit + Tag + Push, [3] nur Commit + Tag + Push
rem     (schon gebaut und archiviert; stehen Commit und Tag schon, weil der Push scheiterte, wird nur noch
rem     gepusht), [4] nichts. Das Fenster bleibt am Ende offen.
rem   Aus einer Konsole MIT Argumenten: 1:1 an release_alles.sh, ohne Rueckfrage und ohne Pause, z.B.
rem     release\release_alles.cmd --dry-run
rem     release\release_alles.cmd --version v0.8.26 --tag
rem     release\release_alles.cmd --ab linux
rem Rueckgabe = die von release_alles.sh (0 = fertig, 1 = Abbruch, 2 = Aufruffehler).
rem Anleitung: release\README.md
rem =============================================================================
setlocal EnableExtensions
set "SKRIPT=%~dp0release_alles.sh"
set "SKRIPT=%SKRIPT:\=/%"
set "RC=0"

set "GITBASH="
for %%B in ("%ProgramW6432%\Git\bin\bash.exe" "%ProgramFiles%\Git\bin\bash.exe" "%LocalAppData%\Programs\Git\bin\bash.exe") do (
    if not defined GITBASH if exist "%%~B" set "GITBASH=%%~B"
)
if defined GITBASH goto gefunden
for /f "tokens=2,*" %%A in ('reg query "HKLM\SOFTWARE\GitForWindows" /v InstallPath 2^>nul ^| find "InstallPath"') do (
    if exist "%%B\bin\bash.exe" set "GITBASH=%%B\bin\bash.exe"
)
if defined GITBASH goto gefunden
for /f "delims=" %%G in ('where git.exe 2^>nul') do (
    if not defined GITBASH if exist "%%~dpG..\bin\bash.exe" set "GITBASH=%%~dpG..\bin\bash.exe"
)
if defined GITBASH goto gefunden
echo Git-Bash nicht gefunden (Git for Windows, erwartet z.B. C:\Program Files\Git\bin\bash.exe).
set "RC=2"
goto ende_pause

:gefunden
if "%~1"=="" goto doppelklick
"%GITBASH%" "%SKRIPT%" %*
exit /b %ERRORLEVEL%

:doppelklick
echo Git-Bash: %GITBASH%
echo.
"%GITBASH%" "%SKRIPT%" --dry-run
if errorlevel 1 (
    echo.
    echo Der Trockenlauf meldet Hindernisse - Zeilen [XX] oben. Bauen geht so nicht; Wahl 3 kann trotzdem passen.
)
echo.
echo   [1] Release bauen, packen, archivieren - ohne Commit/Tag/Push. Dauert Stunden.
echo   [2] wie 1, am Ende Release-Commit + Tag + Push. Die Release-Notes muessen dann stehen.
echo   [3] nur Release-Commit + Tag + Push - die Version ist schon gebaut und archiviert.
echo       Stehen Commit und Tag schon (der Push scheiterte), wird nur noch gepusht.
echo   [4] nichts tun
choice /C 1234 /N /M "Auswahl [1-4]: "
if errorlevel 4 goto ende_pause
if errorlevel 3 goto nur_tag
if errorlevel 2 goto mit_tag
"%GITBASH%" "%SKRIPT%"
set "RC=%ERRORLEVEL%"
goto ende_pause
:mit_tag
"%GITBASH%" "%SKRIPT%" --tag
set "RC=%ERRORLEVEL%"
goto ende_pause
:nur_tag
"%GITBASH%" "%SKRIPT%" --ab abschluss --tag
set "RC=%ERRORLEVEL%"

:ende_pause
echo.
pause
exit /b %RC%
