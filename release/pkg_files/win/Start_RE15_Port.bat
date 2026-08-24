@echo off
rem ===========================================================================
rem RE1.5 Port — Startskript (Windows x64)
rem ===========================================================================
rem SEIT v0.3.20 IST DIESE .BAT NICHT MEHR NOETIG.
rem
rem Die exe sucht ihre Assets jetzt NEBEN SICH: sie ermittelt ihr eigenes
rem Verzeichnis (GetModuleFileName) und leitet daraus die Wurzeln ab —
rem   <exe-Ordner>\shared_assets\PSX          (CD-Baum)
rem   <exe-Ordner>\shared_assets\extracted_fx (Effekt-Texturen)
rem   <exe-Ordner>\shared_assets\RE2          (RE2-KI-Assets)
rem   <exe-Ordner>\synchro                    (Sprachausgabe)
rem — unabhaengig davon, aus welchem Arbeitsverzeichnis sie gestartet wird.
rem re15_pc.exe darf also direkt doppelgeklickt werden.
rem
rem VORGESCHICHTE (Nutzer-Report 0.3.19, "Es fehlen die Optionen in config,
rem die subtitles usw."): davor kannte kein Lade-Pfad im Port sein eigenes
rem exe-Verzeichnis. Ohne die Exporte unten fiel er auf einen einkompilierten
rem Pfad zurueck, der im Docker-Cross-Build auf den Container zeigt
rem ("/src/re15_port/shared_assets/PSX") und beim Nutzer nicht existiert:
rem DATA\TEX.TIM — die Spielschrift — wurde nicht gefunden, damit fehlten
rem CONFIG-Labels, Untertitel, Dialoge und Item-Namen; dazu stumme Musik und
rem fehlende Effekt-Texturen.
rem
rem Die .bat bleibt als bequemer Doppelklick-Starter liegen. Die Exporte
rem setzen dieselben Pfade, die die exe ohnehin selbst findet — sie schaden
rem nicht und dienen zugleich als Notueberschreibung fuer verschobene Baeume.
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
