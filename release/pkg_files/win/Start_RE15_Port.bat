@echo off
rem ===========================================================================
rem RE1.5 Port — Startskript (Windows x64)
rem ===========================================================================
rem Setzt die Asset-Wurzel explizit auf den mitgelieferten Baum. Ohne
rem RE15_CD_ROOT greift im Port nur die cwd-relative Notkette
rem (platform/pc/main.c, pc_read_shared); die findet shared_assets\PSX, aber
rem NICHT die Geschwister-Texturen unter shared_assets\extracted_fx (dort
rem gesucht als "%RE15_CD_ROOT%\..\extracted_fx\..."). Ohne den Export
rem rendern Blut, Muendungsfeuer, Rauch und Huelsen gar nicht.
rem %~dp0 endet bereits mit einem Backslash.
rem ===========================================================================
cd /d "%~dp0"
set "RE15_ASSET_ROOT=%~dp0shared_assets\PSX"
set "RE15_CD_ROOT=%~dp0shared_assets\PSX"
rem Seit v0.2: OPTIONS->AI=RE2 laedt die RE2-Gegner-Assets von hier.
set "RE15_RE2_ASSET_ROOT=%~dp0shared_assets\RE2"
rem ===========================================================================
rem KEIN "start" hier! Die exe ist eine Konsolen-Anwendung (PE-Subsystem 3).
rem `start` legt fuer sie eine NEUE Konsole an — und genau dabei bleibt der
rem Windows-Loader auf manchen Systemen haengen, NOCH BEVOR main() laeuft:
rem   ntdll!LdrInitializeThunk -> LdrLoadDll -> KernelBase!AttachConsole
rem   -> ntdll!ZwCreateFile   (blockiert; gdb-Stack 2026-08-17)
rem Symptom: Prozess lebt (1 Thread, 0% CPU), zeigt KEIN Fenster und schreibt
rem nicht einmal debug.log. Direkt aufgerufen erbt die exe die Konsole dieser
rem .bat und startet normal.
rem ===========================================================================
re15_pc.exe %*
