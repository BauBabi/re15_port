@echo off
REM ============================================================================
REM RE1.5-Port - PSX-Ziel bauen (PSn00bSDK 0.24+)
REM ============================================================================
REM Baut die Engine + das PSX-Backend zu einer PS-EXE und schnuert daraus ein
REM bootfaehiges CD-Image. Die bestehende Basis wird NICHT angefasst - dieses
REM Ziel ist ein eigenstaendiges CMake-Projekt (targets/psx/CMakeLists.txt).
REM
REM Aufruf:   targets\psx\build.cmd
REM Ergebnis: targets\psx\build\re15.exe   (PS-EXE)
REM           targets\psx\build\re15.bin   (CD-Image)
REM           targets\psx\build\re15.cue
REM
REM Voraussetzung: PSn00bSDK 0.24 + mipsel-none-elf-GCC. Beides wird unten
REM automatisch gesucht; abweichende Orte per Umgebungsvariablen setzen:
REM   set PSN00BSDK_ROOT=D:\pfad\zum\sdk\PSn00bSDK-0.24-win32
REM   set PSN00BSDK_GCC=D:\pfad\zur\gcc-mipsel
REM ============================================================================
setlocal EnableDelayedExpansion

if "%PSN00BSDK_ROOT%"=="" (
    for %%D in ("C:\PSn00bSDK\sdk\PSn00bSDK-0.24-win32" "C:\psn00bsdk\sdk\PSn00bSDK-0.24-win32") do (
        if exist "%%~D\lib\libpsn00b\cmake\sdk.cmake" set "PSN00BSDK_ROOT=%%~D"
    )
)
if "%PSN00BSDK_GCC%"=="" (
    for %%D in ("C:\PSn00bSDK\gcc-mipsel" "C:\psn00bsdk\gcc-mipsel") do (
        if exist "%%~D\bin\mipsel-none-elf-gcc.exe" set "PSN00BSDK_GCC=%%~D"
    )
)

if "%PSN00BSDK_ROOT%"=="" (
    echo FEHLER: PSn00bSDK 0.24 nicht gefunden.
    echo         Erwartet: ^<sdk^>\lib\libpsn00b\cmake\sdk.cmake
    echo         Setze PSN00BSDK_ROOT manuell.
    exit /b 1
)
if "%PSN00BSDK_GCC%"=="" (
    echo FEHLER: mipsel-none-elf-GCC nicht gefunden. Setze PSN00BSDK_GCC.
    exit /b 1
)

set "PSN00BSDK_LIBS=%PSN00BSDK_ROOT%\lib\libpsn00b"
set "PATH=%PSN00BSDK_ROOT%\bin;%PSN00BSDK_GCC%\bin;%PATH%"

echo == RE1.5 - PSX-Build ==
echo    SDK : %PSN00BSDK_ROOT%
echo    GCC : %PSN00BSDK_GCC%
echo.

pushd "%~dp0"

cmake -S . -B build -G Ninja ^
      -DCMAKE_TOOLCHAIN_FILE="%PSN00BSDK_LIBS%/cmake/sdk.cmake" ^
      -DPSN00BSDK_TC="%PSN00BSDK_GCC%" ^
      -DCMAKE_BUILD_TYPE=Release || (popd & exit /b 1)

cmake --build build --target re15_iso || (popd & exit /b 1)

echo.
echo == Fertig ==
dir /b build\re15.exe build\re15.bin build\re15.cue 2>nul
echo.
echo RAM-Pruefung (die PSX hat 2 MB; _end muss unter 0x80200000 liegen):
"%PSN00BSDK_GCC%\bin\mipsel-none-elf-nm.exe" build\re15.elf | findstr /R /C:" _end$"
echo    ^^-- liegt der Wert ueber 80200000, passt das Programm NICHT in den
echo        Speicher der Konsole. Stand und Ursachen: targets\psx\README.md
popd
endlocal
