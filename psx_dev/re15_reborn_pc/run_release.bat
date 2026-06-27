@echo off
setlocal

set "ROOT=%~dp0"
set "EXE=%ROOT%build\re15_reborn_pc.exe"

if not exist "%EXE%" (
  echo [INFO] Release EXE not found. Attempting to build...
  call :build
  if errorlevel 1 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
  )
)

echo [INFO] Starting %EXE%
start "re15_reborn_pc" "%EXE%"
exit /b 0

:build
set "CMAKE=cmake"
where cmake >nul 2>nul
if errorlevel 1 (
  if exist "%USERPROFILE%\tools\cmake\cmake-3.30.5-windows-x86_64\bin\cmake.exe" (
    set "CMAKE=%USERPROFILE%\tools\cmake\cmake-3.30.5-windows-x86_64\bin\cmake.exe"
  ) else (
    echo [ERROR] CMake not found in PATH and portable CMake not found.
    echo         Expected: %USERPROFILE%\tools\cmake\cmake-3.30.5-windows-x86_64\bin\cmake.exe
    exit /b 1
  )
)

"%CMAKE%" -S "%ROOT%" -B "%ROOT%build" -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release || exit /b 1
"%CMAKE%" --build "%ROOT%build" -j4 || exit /b 1
exit /b 0
