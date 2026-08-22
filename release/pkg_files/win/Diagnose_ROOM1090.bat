@echo off
REM ============================================================================
REM  Diagnose-Start fuer den ROOM1090-Cutscene-Fehler ("Leon verschwindet")
REM
REM  Einfach doppelklicken und ganz normal spielen, bis der Fehler auftritt.
REM  Danach das Spiel beenden und die Datei  debug_room1090.log  schicken.
REM
REM  Das Spiel laeuft dabei voellig normal - es wird nur zusaetzlich
REM  mitgeschrieben, was in der Szene passiert. Nichts wird veraendert.
REM ============================================================================

cd /d "%~dp0"

REM Positions-/Kamera-/Sichtbarkeits-Spur je Bild
set RE15_VIS_TRACE=1

REM Protokolliert, wie oft die Cutscene gestartet wird
set RE15_EVT_TRACE=1

echo.
echo   Diagnose-Modus laeuft.
echo.
echo   1. Ganz normal bis in den Hof spielen (ROOM1090).
echo   2. Die Szene ausloesen und den Fehler auftreten lassen.
echo   3. Spiel beenden (Fenster schliessen).
echo   4. Die Datei  debug_room1090.log  aus diesem Ordner schicken.
echo.

re15_pc.exe

REM Das Spiel schreibt immer nach debug.log (fester Name, wird bei jedem Start
REM neu angelegt). Nach dem Beenden unter sprechendem Namen sichern, damit ein
REM spaeterer Start sie nicht ueberschreibt.
if exist debug.log (
    copy /y debug.log debug_room1090.log >nul
    echo.
    echo   Fertig: debug_room1090.log liegt in diesem Ordner.
) else (
    echo.
    echo   ACHTUNG: keine debug.log gefunden - lief das Spiel ueberhaupt?
)

echo.
pause
