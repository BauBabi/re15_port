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
start "" re15_pc.exe %*
