# Requirements Document

## Introduction

Dieses Dokument definiert die Anforderungen für das vollständige Neuaufsetzen des RE 1.5 (Resident Evil 1.5) Ports als 1:1-Rekonstruktion für PC (SDL2/OpenGL) und PSX (PSn00bSDK). Das Projekt umfasst drei Hauptbereiche: saubere Projektstruktur mit vereinheitlichtem Build-System, Ermittlung der RDT-Raum-Verkettung und den vollständigen Spielfluss, sowie Kiro-Skills zur Unterstützung der Port-Arbeit.

## Glossary

- **Port_Engine**: Die gemeinsame C-Engine, die beide Plattformen (PSX und PC) bedient, mit plattformspezifischen Backends für Rendering, Audio und I/O
- **RDT_Datei**: Room Definition Table — Container-Datei pro Raum mit Header, Adresstabelle und allen raumspezifischen Daten (Kollision, Kamera, Skripte, Modelle, Texturen)
- **SCD_VM**: Script-Bytecode Virtual Machine — Interpreter für die ~90 RE1.5-Opcodes, die Spiellogik, Türen, Events und Interaktionen steuern
- **Stage_Overlay**: Dynamisch geladener Code-Block (STAGE1-6.BIN) ab 0x80100000, enthält raumspezifische Init/Update-Routinen
- **Door_aot_set_4p**: SCD-Opcode 0x68 (40 Bytes) — definiert Tür-Übergänge zwischen Räumen mit Ziel-Stage, Ziel-Room und Position
- **BSS_Datei**: MDEC-Video-Hintergrund — 64KB-Chunks pro Kamera-Winkel, 320x240 VLC-Bitstream
- **Build_System**: CMake-basiertes Build-System mit zwei Targets (PSX via PSn00bSDK-Toolchain, PC via MSVC/GCC + SDL2)
- **Raum_Graph**: Gerichteter Graph aller Raum-Verbindungen, extrahiert aus Door_aot_set_4p-Opcodes in den SCD-Skripten
- **Asset_Pipeline**: Werkzeugkette von Original-CD-Assets über Java-Extraktoren zu den portfertigen Formaten
- **Kiro_Skill**: Wiederverwendbare Kiro-Anweisung, die spezifisches Domänenwissen für RE1.5-Analyse und Port-Arbeit bereitstellt
- **Ghidra_Overlay**: Dekompilierter C-Quellcode der 6 Stage-Overlays — **primäre RE-Quelle** (`RE_15_Quellcode_Overlays/`; RE2-Referenz `RE2_Quellcode_Overlays/`), ~200+ Funktionen pro Stage. EXE-Decompilate (`RE_15_Quellcode_V2/` / `RE2_Quellcode_V2/`) = **Fallback**; Fehlendes wird selbstständig via Ghidra + `stage_saves/` ermittelt

## Requirements

### Requirement 1: Vereinheitlichte Projektstruktur

**User Story:** Als Entwickler möchte ich eine saubere, einheitliche Projektstruktur für den 1:1-Port, damit PC- und PSX-Build aus einer gemeinsamen Codebasis arbeiten und keine Duplikation entsteht.

#### Acceptance Criteria

1. THE Build_System SHALL einen einzigen CMake-Root bereitstellen, der beide Targets (PSX und PC) als Unter-Projekte definiert
2. THE Build_System SHALL gemeinsamen Engine-Code in einem dedizierten Verzeichnis (`engine/`) organisieren, getrennt von plattformspezifischem Code (`platform/psx/`, `platform/pc/`)
3. THE Build_System SHALL gemeinsame Header in einem einzigen Include-Verzeichnis (`include/`) bereitstellen, das von beiden Targets referenziert wird
4. WHEN eine neue `.c`-Quelldatei im Verzeichnis `engine/` abgelegt wird, THE Build_System SHALL diese ohne manuelle Änderung an plattformspezifischen CMakeLists-Dateien in beide Targets einbinden (z.B. via GLOB-Mechanismus)
5. WHEN die Migration abgeschlossen ist, THE Build_System SHALL beide Targets (PSX und PC) erfolgreich kompilieren und linken, wobei die resultierenden Binaries identisches Laufzeitverhalten zu den bisherigen separaten Builds aufweisen (PSX: lauffähiges ISO-Image; PC: lauffähige Executable mit identischer Spiellogik-Ausgabe)
6. THE Build_System SHALL die PSX-Toolchain (PSn00bSDK gcc-mipsel) über eine dedizierte Toolchain-Datei konfigurieren
7. THE Build_System SHALL die PC-Toolchain (MSVC oder GCC + SDL2 via FetchContent) so konfigurieren, dass auf Windows keine zusätzliche manuelle Abhängigkeitsinstallation notwendig ist; auf Linux werden benötigte System-Bibliotheken (libSDL2-dev, libGL, libX11) in der Dokumentation aufgelistet
8. IF das PSX-Target gebaut wird ohne installierte PSn00bSDK, THEN THE Build_System SHALL den Build innerhalb der CMake-Configure-Phase mit einer Fehlermeldung abbrechen, die den Namen der fehlenden Komponente und einen Verweis auf die Installationsanleitung enthält
9. IF das PC-Target gebaut wird und SDL2 via FetchContent nicht heruntergeladen werden kann (z.B. fehlende Netzwerkverbindung), THEN THE Build_System SHALL den Build mit einer Fehlermeldung abbrechen, die die fehlgeschlagene Abhängigkeit (SDL2) benennt

### Requirement 2: Asset-Referenzierung und Verzeichniskonvention

**User Story:** Als Entwickler möchte ich klare Konventionen für die Ablage und Referenzierung der Original-Assets, damit beide Plattformen identische Spieldaten lesen und keine Assets dupliziert werden.

#### Acceptance Criteria

1. THE Port_Engine SHALL alle Original-Assets aus einem einzigen konfigurierbaren Pfad (`ASSETS_ROOT`) lesen, der auf das extrahierte CD-Image zeigt und zur Laufzeit über den zur Compile-Zeit gesetzten CMake-Parameter oder eine Umgebungsvariable `RE15_ASSETS_PATH` aufgelöst wird
2. THE Port_Engine SHALL die originale Verzeichnisstruktur (STAGE1-6/, BIN/, DATA/, DOOR/, EMD/, ITEM/, MOVIE/, PLD/, SOUND/, VOICE/) beibehalten und nicht umstrukturieren
3. THE Port_Engine SHALL RDT-Dateien im Unterverzeichnis `STAGE{N}/` über die Namenskonvention `ROOM{Stage}{RoomHex}{Player}.RDT` adressieren, wobei Stage=1-6, RoomHex=00-27 (hexadezimal), Player=0(Leon)/1(Elza)
4. THE Port_Engine SHALL alle Dateinamen innerhalb von `ASSETS_ROOT` case-insensitiv auflösen, sodass auf case-sensitiven Dateisystemen (Linux/macOS) Großschreibung der Original-CD-Dateien keine Rolle spielt
5. WHEN ein Asset nicht am konfigurierten Pfad gefunden wird, THE Port_Engine SHALL eine Fehlermeldung mit dem vollständigen erwarteten Dateipfad auf stderr ausgeben und die Anwendung mit einem non-zero Exit-Code beenden
6. THE Build_System SHALL einen CMake-Parameter `RE15_ASSETS_PATH` bereitstellen, der den Asset-Root-Pfad für beide Targets setzt
7. IF `RE15_ASSETS_PATH` bei der CMake-Konfiguration nicht gesetzt oder der angegebene Pfad nicht existiert, THEN THE Build_System SHALL die Konfiguration mit einer Fehlermeldung abbrechen, die den erwarteten Parameter und ein Beispiel enthält
8. THE Asset_Pipeline SHALL extrahierte Zwischen-Ergebnisse (TIM→BMP, VAB→WAV, etc.) in einem separaten `build/extracted/`-Verzeichnis ablegen, getrennt von den Originaldaten

### Requirement 3: RDT-Raum-Graph und Spielfluss-Ermittlung

**User Story:** Als Entwickler möchte ich den vollständigen Raum-Graphen aller 6 Stages aus den SCD-Skripten extrahieren, damit ich den Spielfluss rekonstruieren und alle Raum-Übergänge korrekt implementieren kann.

#### Acceptance Criteria

1. THE Asset_Pipeline SHALL alle Door_aot_set_4p-Opcodes (0x68, 40 Bytes) aus den mainScd- und subScd-Sektionen jeder RDT-Datei über alle 6 Stages extrahieren
2. THE Asset_Pipeline SHALL aus jedem Door_aot_set_4p-Opcode die Felder Quell-Stage, Quell-Room, Ziel-Stage, Ziel-Room und Spawn-Position (X/Y/Z-Koordinaten sowie Blickwinkel) parsen und als strukturierten Datensatz pro Tür ausgeben
3. THE Asset_Pipeline SHALL einen vollständigen Raum_Graph als Adjazenzliste generieren, wobei pro extrahiertem Door_aot_set_4p-Opcode genau eine gerichtete Kante entsteht, mit Format: `Quell-Room → [Ziel-Room, Ziel-Stage, Spawn-X, Spawn-Y, Spawn-Z, Blickwinkel]`
4. THE Asset_Pipeline SHALL den Raum_Graph in einem maschinenlesbaren Format (JSON) und einem menschenlesbaren Format (Markdown-Tabelle) ausgeben
5. IF eine RDT-Datei keine Door_aot_set_4p-Opcodes enthält, THEN THE Asset_Pipeline SHALL diesen Raum als Knoten mit leerer Adjazenzliste und dem Flag "dead_end: true" im Graph aufnehmen
6. THE Asset_Pipeline SHALL die Zuordnung zwischen Ghidra-Overlay-Funktionen (FUN_80xxxxxx) und Raum-Nummern basierend auf dem Dispatch-Muster der Stage-Overlays ermitteln und als JSON-Mapping sowie Markdown-Tabelle ausgeben
7. THE Asset_Pipeline SHALL den Intro-Raum (ROOM1240) als Startknoten des Raum_Graphen kennzeichnen, indem dieser Knoten im JSON-Output das Feld "start_node: true" erhält und in der Markdown-Tabelle explizit als Startknoten ausgewiesen wird
8. IF eine SCD-Sektion einer RDT-Datei nicht lesbar oder fehlerhaft strukturiert ist, THEN THE Asset_Pipeline SHALL eine Warnung mit Dateiname und Fehlerposition ausgeben, die betroffene Datei überspringen und die Extraktion der verbleibenden RDT-Dateien fortsetzen

### Requirement 4: SCD-VM-Interpreter für Raum-Übergänge

**User Story:** Als Entwickler möchte ich einen funktionierenden SCD-VM-Interpreter, der Door_aot_set_4p-Opcodes zur Laufzeit korrekt verarbeitet, damit Türen zwischen Räumen funktionieren.

#### Acceptance Criteria

1. THE SCD_VM SHALL den Opcode Door_aot_set_4p (0x68, 40 Bytes) vollständig interpretieren und folgende Felder extrahieren: AOT-Slot, Floor/Band, Trigger-Rechteck (4 Punkte als s16 LE), Spawn-Position X/Y/Z (s16 LE), Spawn-Rotation (s16 LE), Ziel-Stage (0-basiert) und Ziel-Room, und diese an das AOT-Subsystem übergeben
2. WHEN der Spieler die Aktionstaste drückt, THE Port_Engine SHALL für jede aktive Tür-AOT prüfen, ob die Spielerposition innerhalb des 4-Punkt-Polygons liegt UND der aktuelle Kollisions-Band-Wert des Spielers mit dem Band-Wert der Tür übereinstimmt, und bei Erfüllung beider Bedingungen den Raumwechsel auslösen
3. WHEN ein Raumwechsel ausgelöst wird, THE Port_Engine SHALL den aktuellen Raum entladen (Akteure zurücksetzen, SCD-VM neu initialisieren, Kollisionsdaten freigeben), die Ziel-RDT-Datei laden, den Spieler an der definierten Spawn-Position und -Rotation platzieren und die Kamera auf den Ziel-Cut setzen
4. WHEN der Ziel-Room in einer anderen Stage liegt (dest_stage ≠ aktuelle Stage), THE Port_Engine SHALL zusätzlich das entsprechende Stage_Overlay laden und die raumspezifische BGM-Tabelle für die neue Stage aktivieren
5. IF eine referenzierte Ziel-RDT-Datei nicht geladen werden kann, THEN THE Port_Engine SHALL eine Warnung auf stderr (PC) bzw. printf (PSX) ausgeben, den Raumwechsel abbrechen und den Spieler an seiner aktuellen Position im bisherigen Raum belassen
6. THE SCD_VM SHALL alle Opcodes aus der bestehenden `scd_vm.c`-Implementierung unterstützen, mindestens: Nop (0x00), Evt_end (0x01), Ifel_ck (0x06), Else_ck (0x07), Endif (0x08), Sleep (0x09), For (0x0D), Ck (0x21), Set (0x22), Cut_chg (0x29), Aot_set (0x2C), Door_aot_set_4p (0x68), Obj_model_set (0x2D), Se_on (0x36), Sce_em_set (0x44), Item_aot_set (0x4E/0x50)
7. WHEN ein Tür-Ziel auf denselben Raum verweist, THE Port_Engine SHALL keinen Raum-Neulade durchführen, sondern den Spieler an die Spawn-Position teleportieren und die Kamera umschalten
8. WHEN der Spieler nach einem Raumwechsel an der Spawn-Position platziert wird, THE Port_Engine SHALL alle Tür-AOT-Zonen, deren Trigger-Bereich die neue Spielerposition enthält, als "bereits betreten" markieren, damit kein sofortiges erneutes Auslösen stattfindet

### Requirement 5: Plattform-Abstraktion (Rendering)

**User Story:** Als Entwickler möchte ich eine saubere Plattform-Abstraktionsschicht für das Rendering, damit Engine-Logik plattformunabhängig bleibt und nur die Backends (PSX-GTE vs. PC-OpenGL/SDL2) sich unterscheiden.

#### Acceptance Criteria

1. THE Port_Engine SHALL ein Rendering-Interface definieren mit Funktionen für: Initialisierung, Frame-Begin, Frame-End, Polygon-Submission (Tri/Quad, texturiert/untexturiert, beleuchtet/unbeleuchtet), Hintergrund-Anzeige und Sprite-Rendering, wobei beide Backends dieselbe Funktionssignatur-Tabelle implementieren
2. THE Port_Engine SHALL auf PSX die GTE-Hardware (RotMatrix, TransMatrix, RTPT, NCCT) für 3D-Transformationen und Beleuchtung nutzen
3. THE Port_Engine SHALL auf PC Software-Mathematik mit 20.12-Fixkomma-Arithmetik (identisch zur PSX-GTE-Präzision) implementieren, sodass transformierte Vertex-Koordinaten auf beiden Plattformen maximal ±1 Pixel Abweichung bei 320x240 Auflösung aufweisen, und die Ausgabe über SDL2/OpenGL rendern
4. THE Port_Engine SHALL MDEC-dekodierte BSS-Hintergründe auf beiden Plattformen in nativer Auflösung (320x240) anzeigen
5. THE Port_Engine SHALL das RE2-Ordering-Table-Konzept (OT) für die Z-Sortierung auf beiden Plattformen implementieren mit einer OT-Tiefe von 4096 Einträgen
6. WHILE ein Frame gerendert wird, THE Port_Engine SHALL auf PSX die GPU-DMA-Submission vor dem VSync-Wait ausführen und beide Plattformen mit einer Ziel-Framerate von 30 FPS (33,33 ms pro Frame) rendern
7. IF die Rendering-Initialisierung fehlschlägt (SDL2-Fenster/OpenGL-Kontext auf PC oder GPU-Initialisierung auf PSX), THEN THE Port_Engine SHALL eine Fehlermeldung mit der Ursache ausgeben und den Start abbrechen ohne undefiniertes Verhalten

### Requirement 6: Plattform-Abstraktion (Audio)

**User Story:** Als Entwickler möchte ich eine saubere Plattform-Abstraktionsschicht für Audio, damit BGM, SFX und Voice auf beiden Plattformen korrekt wiedergegeben werden.

#### Acceptance Criteria

1. THE Port_Engine SHALL ein Audio-Interface definieren mit Funktionen für: VAB-Bank laden, SEQ-Playback (BGM) starten/stoppen, SFX-Trigger (Se_on), Voice-Playback und Lautstärke-Steuerung (0-127 linear)
2. THE Port_Engine SHALL auf PSX die SPU-Hardware über libsnd/libspu für VAB/SEQ-Wiedergabe nutzen
3. THE Port_Engine SHALL auf PC eine SDL2-Audio-Abstraktion bereitstellen, die VAB-Samples mittels PSX-ADPCM-Dekodierung (Filter-Koeffizienten aus VAG-Header) zu PCM konvertiert und über SDL-Audio-Callbacks mit 44100 Hz Samplerate wiedergibt
4. THE Port_Engine SHALL die 47 BGM-Dateien (MAIN*.BGM als SEQ+VAB) auf beiden Plattformen abspielen, wobei BGM-Tracks im Loop abspielen bis explizit gestoppt oder durch einen anderen BGM-Track ersetzt
5. THE Port_Engine SHALL SFX über den SCD-Opcode Se_on (0x36) mit korrekten Parametern (Bank 0-15, ID 0-127, Volume 0-127, Pan 0-127 wobei 64=Mitte) auslösen
6. IF eine VAB-Bank nicht geladen werden kann (Datei fehlt oder Header-Magic ungültig), THEN THE Port_Engine SHALL den Fehler auf stderr protokollieren und ohne Audio für diese Bank fortfahren ohne Absturz

### Requirement 7: Plattform-Abstraktion (Eingabe und Datei-I/O)

**User Story:** Als Entwickler möchte ich einheitliche Abstraktionen für Eingabe und Datei-I/O, damit Spielsteuerung und Asset-Laden plattformunabhängig funktionieren.

#### Acceptance Criteria

1. THE Port_Engine SHALL ein Input-Interface definieren, das digitale Buttons (D-Pad, Cross, Circle, Square, Triangle, L1/R1/L2/R2, Start, Select) als 16-Bit-Bitmaske bereitstellt und pro Frame drei Zustände liefert: aktuell gedrückt (held), in diesem Frame neu gedrückt (pressed-edge) und in diesem Frame losgelassen (released-edge)
2. WHERE Platform=PSX, THE Port_Engine SHALL die PSn00bSDK-Pad-Bibliothek für Controller-Input nutzen und das Pad-Ergebnis pro Frame einmal abfragen
3. WHERE Platform=PC, THE Port_Engine SHALL SDL2-Keyboard- und Gamepad-Eingabe auf das PSX-Button-Layout mappen, wobei eine Standard-Tastenbelegung vordefiniert ist
4. THE Port_Engine SHALL ein Datei-I/O-Interface definieren mit synchronen Funktionen für: Datei öffnen (gibt Handle oder Fehlerindikator zurück), vollständig lesen (gibt gelesene Byte-Anzahl zurück), Dateigröße ermitteln (gibt Größe in Bytes zurück) und schließen
5. WHERE Platform=PSX, THE Port_Engine SHALL den CD-Dateisystem-Zugriff (CdSearchFile + CdRead) implementieren
6. WHERE Platform=PC, THE Port_Engine SHALL den Dateisystem-Zugriff über Standard-C-I/O (fopen/fread/fclose) implementieren
7. WHERE Platform=PSX, WHEN eine Datei gelesen wird, THE Port_Engine SHALL Sektor-Alignment (2048 Bytes) beachten und Puffer an einer 4-Byte-Grenze ausrichten für DMA-Kompatibilität
8. IF eine Datei-I/O-Operation fehlschlägt (Datei nicht gefunden, Lesefehler oder unzureichender Puffer), THEN THE Port_Engine SHALL einen Fehlercode an den Aufrufer zurückgeben und den Fehlerzustand protokollieren ohne Absturz

### Requirement 8: Ghidra-Overlay-Mapping und Funktionskatalog

**User Story:** Als Entwickler möchte ich eine systematische Zuordnung der dekompilinerten Ghidra-Funktionen zu semantischen Namen und Raum-Nummern, damit ich den Overlay-Code für die Port-Arbeit nutzen kann.

#### Acceptance Criteria

1. THE Asset_Pipeline SHALL in jedem der 6 Stage-Overlay-Dateien (STAGE1_overlay.c bis STAGE6_overlay.c) die Dispatch-Muster identifizieren, indem sie Jump-Tables (indirekte Sprünge über Adressen indiziert durch Actor-Byte-Offsets) erkennt, und für jede erkannte Jump-Table die Zuordnung Room-ID → Funktions-Adresse als strukturierte Liste dokumentieren
2. THE Asset_Pipeline SHALL für jeden Stage-Ordner (STAGE1_full/ bis STAGE6_full/) eine Mapping-Tabelle generieren, die pro Funktion folgende Felder enthält: Funktions-Adresse (FUN_80xxxxxx), Stage-Nummer (1-6), Room-ID (Hex-Wert aus Jump-Table-Index), semantischer Name (aus RE15_FUN_CATALOG.md falls vorhanden, sonst leer), und Zuordnungs-Konfidenz (high/medium/low)
3. THE Asset_Pipeline SHALL beim Generieren des Mappings alle Einträge aus RE15_FUN_CATALOG.md per Adress-Abgleich übernehmen und Funktionen, die im Overlay vorhanden aber nicht im Katalog gelistet sind, als neue Einträge mit Konfidenz "low" kennzeichnen
4. WHEN eine Overlay-Funktion über den Jump-Table-Index eindeutig einem Room-ID zugeordnet ist, THE Asset_Pipeline SHALL den Funktionsnamen im Format `room{Stage}{RoomHex}_init` (für die erste Funktion des Raums) bzw. `room{Stage}{RoomHex}_update` (für nachfolgende Funktionen desselben Raums) vorschlagen
5. THE Asset_Pipeline SHALL das Mapping als JSON-Datei mit einem Array von Einträgen (jeweils mit den Feldern: address, stage, room_id, proposed_name, catalog_name, confidence) und als Markdown-Tabelle mit denselben Spalten ausgeben
6. IF eine Overlay-Funktion keiner Room-ID über einen erkannten Jump-Table zugeordnet werden kann, THEN THE Asset_Pipeline SHALL diese Funktion im Mapping mit room_id "unresolved" aufführen und die Anzahl unaufgelöster Funktionen pro Stage in einer Zusammenfassung am Ende der Markdown-Ausgabe ausweisen

### Requirement 9: Kiro-Skills für die Port-Arbeit

**User Story:** Als Entwickler möchte ich spezialisierte Kiro-Skills, die mich bei der täglichen Port-Arbeit unterstützen, damit wiederkehrende Analyse- und Verständnisaufgaben effizient erledigt werden.

#### Acceptance Criteria

1. THE Kiro_Skill SHALL einen RDT-Analyse-Skill bereitstellen, der eine gegebene RDT-Datei parst und für jede erkannte Sektion (Header-Felder, Kollision, Kameras, SCD-Offsets, Modelle, Texturen) eine strukturierte Zusammenfassung mit Sektionsname, Offset, Größe und Anzahl der Einträge ausgibt
2. IF eine RDT-Datei ungültig ist oder eine Sektion nicht geparst werden kann, THEN THE Kiro_Skill SHALL die betroffene Sektion als fehlerhaft kennzeichnen mit dem Offset an dem das Parsing fehlschlug und die übrigen Sektionen trotzdem verarbeiten
3. THE Kiro_Skill SHALL einen SCD-Disassembly-Skill bereitstellen, der SCD-Bytecode aus einer RDT-Datei in zeilenweisen Pseudo-Code übersetzt mit jeweils Offset, Opcode-Name, numerischen Parametern und einer einzeiligen Beschreibung jedes Parameters gemäß der Opcode-Tabelle in SCDScriptDisassembler.java
4. IF ein SCD-Opcode unbekannt ist (nicht in der Opcode-Tabelle enthalten), THEN THE Kiro_Skill SHALL den Opcode als unbekannt mit seinem Hex-Wert und den rohen Bytes bis zum nächsten erkannten Opcode ausgeben
5. THE Kiro_Skill SHALL einen Ghidra-Mapping-Skill bereitstellen, der zu einer gegebenen FUN_80xxxxxx-Adresse das zugehörige Subsystem, den Funktionszweck und das Confidence-Level aus RE15_FUN_CATALOG.md zurückgibt
6. IF eine Adresse nicht in RE15_FUN_CATALOG.md gefunden wird, THEN THE Kiro_Skill SHALL eine Meldung ausgeben dass die Adresse nicht katalogisiert ist und die nächsten 3 katalogisierten Adressen im selben 0x1000-Bereich als mögliche Nachbarn auflisten
7. THE Kiro_Skill SHALL einen Build-Workflow-Skill bereitstellen, der die Build-Befehle für PSX- und PC-Targets aus der Projekt-Build-Konfiguration generiert und bei einem fehlgeschlagenen Build den Fehler-Output parst und eine Diagnose mit betroffener Datei, Zeilennummer und Lösungsvorschlag ausgibt
8. THE Kiro_Skill SHALL einen Asset-Pipeline-Skill bereitstellen, der für einen gegebenen Asset-Typ (RDT, BSS, EMD, TIM, VAB) die Extraktionsschritte, benötigten Parser-Klassen und das Ausgabeformat als geordnete Liste dokumentiert
9. WHEN ein Skill aktiviert wird, THE Kiro_Skill SHALL die zum angefragten Skill gehörenden Abschnitte aus RE15_KNOWLEDGE.md und RE15_FUN_CATALOG.md als Kontext laden, wobei die Zuordnung Skill-zu-Abschnitt statisch pro Skill definiert ist

### Requirement 10: Raum-Lade-System (Multi-Room-Fähigkeit)

**User Story:** Als Entwickler möchte ich ein robustes Raum-Lade-System, das beliebige RDT-Dateien zur Laufzeit laden und initialisieren kann, damit der vollständige Spielfluss über alle 6 Stages möglich wird.

#### Acceptance Criteria

1. THE Port_Engine SHALL eine RDT-Datei vollständig in den Arbeitsspeicher laden und alle Sektionen gemäß der Adresstabelle (Offset 0x08-0x5C, 21 Einträge) parsen
2. THE Port_Engine SHALL aus der geladenen RDT die Kollisionsdaten, Kameradefinitionen, SCD-Skripte, Modell-Referenzen und Textur-Daten extrahieren und den jeweiligen Engine-Subsystemen als initialisierte Datenstrukturen bereitstellen
3. WHEN ein Raumwechsel stattfindet, THE Port_Engine SHALL den vorherigen Raum-Speicher vollständig freigeben, bevor der neue Raum geladen wird, sodass keine Speicherfragmente des alten Raums verbleiben
4. WHEN eine neue Stage betreten wird, THE Port_Engine SHALL die zugehörige VAB-Sound-Bank (EDH/VB aus SOUND/) laden und für SFX-Wiedergabe bereithalten
5. WHEN ein Raum geladen wird, THE Port_Engine SHALL die zugehörige BSS-Datei laden und den korrekten 64KB-Chunk basierend auf der aktiven Kamera-ID (Index 0 bis Anzahl Kameras minus 1 laut RDT-Kamerasektion) anzeigen
6. IF der belegte Arbeitsspeicher auf PSX 1.5 MB überschreitet (bei RAM-Limit von 2 MB), THEN THE Port_Engine SHALL die Lade-Strategie auf Streaming umstellen und nur den aktuellen Raum plus maximal einen vorgeladenen Nachbar-Raum im Speicher halten
7. WHEN ein Raum vollständig geladen ist, THE Port_Engine SHALL das mainScd (Haupt-Skript) und subScd (Sub-Skripte) des Raums in der SCD_VM initialisieren und die erste Ausführung triggern
8. IF eine RDT-Sektion einen Offset von 0x00000000 in der Adresstabelle enthält, THEN THE Port_Engine SHALL diese Sektion als nicht vorhanden behandeln und das zugehörige Subsystem ohne diese Daten initialisieren
9. IF die BSS-Datei zum aktuellen Raum nicht gefunden wird oder die aktive Kamera-ID die Anzahl verfügbarer Chunks überschreitet, THEN THE Port_Engine SHALL einen schwarzen Hintergrund anzeigen und eine Warnung protokollieren ohne den Ladevorgang abzubrechen

### Requirement 11: Spieler-Entität und Basis-Steuerung

**User Story:** Als Entwickler möchte ich eine funktionale Spieler-Entität mit Tank-Controls, damit die grundlegende Bewegung und Interaktion mit der Spielwelt funktioniert.

#### Acceptance Criteria

1. THE Port_Engine SHALL eine Spieler-Entität mit Position (X/Y/Z als Fixkomma-Werte), Rotation (Yaw im Bereich 0–4095), Geschwindigkeit (0 bis maximal 2 Stufen: Gehen und Rennen) und Animationszustand verwalten
2. THE Port_Engine SHALL Tank-Controls implementieren: Vorwärts/Rückwärts bewegt den Spieler entlang seiner Blickrichtung mit einer Gehgeschwindigkeit und einer Renngeschwindigkeit (ausgelöst durch gehaltene Rennen-Taste), Links/Rechts dreht die Blickrichtung mit einer festen Rotationsgeschwindigkeit pro Frame
3. THE Port_Engine SHALL den Spieler als zylindrischen Kollisionskörper (definiert durch Position und Radius) gegen die Kollisionsdaten (SCA-Dreiecke) des aktuellen Raums prüfen und bei erkannter Durchdringung die Position entlang der Kollisionsnormale zurücksetzen
4. THE Port_Engine SHALL die Spieler-Animation (PLD-Modell) basierend auf dem Bewegungszustand abspielen: Idle-Animation bei Geschwindigkeit 0, Geh-Animation bei Gehgeschwindigkeit, Renn-Animation bei Renngeschwindigkeit, wobei die Animation mit der Engine-Framerate (30 fps) fortschreitet
5. WHEN der Spieler eine AOT-Zone betritt (Tür, Item, Event), THE Port_Engine SHALL die entsprechende Aktion gemäß AOT-Typ auslösen (Door_aot_set_4p löst Raumwechsel aus, Item_aot_set löst Item-Aufnahme aus, Aot_set löst Event-Callback aus)
6. WHEN die Spieler-Position eine RVD-Kamerazone-Grenze überschreitet, THE Port_Engine SHALL die aktive Kamera auf diejenige Kamera-ID umschalten, deren Zone die aktuelle Spielerposition enthält, und den zugehörigen BSS-Hintergrund-Chunk laden
7. IF die PLD-Modelldatei des Spielers beim Laden nicht gefunden wird, THEN THE Port_Engine SHALL eine Fehlermeldung mit dem erwarteten Dateipfad ausgeben und den Ladevorgang abbrechen

### Requirement 12: Test- und Validierungsstrategie

**User Story:** Als Entwickler möchte ich eine klare Test- und Validierungsstrategie, damit ich Fortschritt und Korrektheit des Ports objektiv messen kann.

#### Acceptance Criteria

1. THE Build_System SHALL ein Test-Target bereitstellen, das Unit-Tests für Engine-Kernfunktionen (RDT-Parser, SCD-VM-Opcodes, Kollisionsprüfung, Fixkomma-Mathematik) ausführt und bei Erfolg Exit-Code 0, bei mindestens einem Fehlschlag Exit-Code ungleich 0 zurückgibt
2. THE Port_Engine SHALL einen Headless-Modus auf PC bereitstellen, der Raum-Laden und SCD-Ausführung ohne Rendering durchführt und das Ergebnis (Erfolg oder Fehler mit betroffener Sektion) auf stdout ausgibt sowie über den Exit-Code signalisiert
3. THE Port_Engine SHALL einen Vergleichsmodus bereitstellen, der Spielzustände (Spielerposition, Flags, Inventar) gegen referenzierte Savestates aus `stage_saves/` validiert, wobei Positionswerte mit einer Toleranz von maximal 1 Fixkomma-Einheit als übereinstimmend gelten und bei Abweichung die abweichenden Felder mit Soll- und Ist-Wert ausgegeben werden
4. WHEN ein SCD-Opcode implementiert wird, THE Build_System SHALL mindestens einen automatisierten Test bereitstellen, der die korrekte Ausführung des Opcodes gegen bekannte Eingabe/Ausgabe-Paare prüft
5. THE Build_System SHALL alle Tests auf dem PC-Target ohne GPU-Hardware und ohne PSX-Devkit ausführbar bereitstellen, sodass eine CI/CD-Umgebung mit Standard-CPU die Tests vollständig durchlaufen kann
6. THE Port_Engine SHALL Debug-Overlays bereitstellen (Kollisionsboxen, AOT-Zonen, Kamerazonen, SCD-Trace), die zur Laufzeit einzeln über Tastenkürzel ein- und ausgeschaltet werden können
7. IF ein einzelner Test länger als 30 Sekunden ohne Ergebnis läuft, THEN THE Build_System SHALL diesen Test als fehlgeschlagen abbrechen und den Timeout im Testergebnis dokumentieren
