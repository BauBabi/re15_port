# Implementation Plan: RE 1.5 Port Setup

## Overview

Dieser Implementierungsplan beschreibt die schrittweise Umsetzung der 1:1-Rekonstruktion des RE 1.5 Ports für PC (SDL2/OpenGL) und PSX (PSn00bSDK). Die Aufgaben sind so strukturiert, dass jeder Schritt auf dem vorherigen aufbaut — von der Projektstruktur über die Engine-Kerne bis zur Integration aller Subsysteme.

## Tasks

- [x] 1. Projektstruktur und Build-System aufsetzen
  - [x] 1.1 CMake-Root und Verzeichnisstruktur erstellen
    - Erstelle `re15_port/CMakeLists.txt` mit Root-Projekt-Definition (C + ASM)
    - Erstelle Verzeichnisse: `engine/src/`, `include/`, `platform/psx/src/`, `platform/pc/src/`, `tools/`, `tests/`
    - Erstelle `cmake/psx_toolchain.cmake` mit PSn00bSDK Toolchain-Konfiguration
    - Erstelle `cmake/FindPSn00bSDK.cmake` für SDK-Detektion
    - Implementiere `RE15_BUILD_PSX` / `RE15_BUILD_PC` Optionen mit FATAL_ERROR bei fehlender PSn00bSDK
    - _Requirements: 1.1, 1.6, 1.8_

  - [x] 1.2 Engine-Library mit GLOB-Mechanismus erstellen
    - Erstelle `engine/CMakeLists.txt` mit `file(GLOB ENGINE_SOURCES "src/*.c")`
    - Erstelle `add_library(re15_engine STATIC ${ENGINE_SOURCES})`
    - Setze `target_include_directories(re15_engine PUBLIC ${CMAKE_SOURCE_DIR}/include)`
    - Erstelle Platzhalter-Datei `engine/src/stub.c` damit der Build nicht leer ist
    - _Requirements: 1.2, 1.3, 1.4_

  - [x] 1.3 PC-Target mit SDL2 FetchContent konfigurieren
    - Erstelle `platform/pc/CMakeLists.txt` mit FetchContent für SDL2
    - Erstelle `platform/pc/main.c` mit minimalem Game-Loop-Skeleton (30fps Timer)
    - Linke gegen `re15_engine` und SDL2
    - Implementiere FATAL_ERROR wenn FetchContent fehlschlägt (keine Netzwerkverbindung)
    - _Requirements: 1.7, 1.9_

  - [x] 1.4 PSX-Target mit Toolchain-Datei konfigurieren
    - Erstelle `platform/psx/CMakeLists.txt` für PSn00bSDK-Crosscompile
    - Erstelle `platform/psx/main.c` mit PSX-Boot-Skeleton (GPU-Init, Pad-Init)
    - Erstelle `platform/psx/iso.xml` für ISO-Image-Generierung
    - Linke gegen `re15_engine`
    - _Requirements: 1.6, 1.8_

  - [x] 1.5 Test-Framework einrichten
    - Erstelle `tests/CMakeLists.txt` mit `enable_testing()` und CTest-Integration
    - Erstelle `tests/test_main.c` als Test-Runner mit Exit-Code-Logik
    - Konfiguriere 30-Sekunden-Timeout pro Test via `set_tests_properties(TIMEOUT 30)`
    - Erstelle Verzeichnisse `tests/property/`, `tests/unit/`, `tests/integration/`
    - _Requirements: 12.1, 12.5, 12.7_

  - [x] 1.6 Asset-Pfad-Konfiguration (RE15_ASSETS_PATH)
    - Implementiere CMake-Parameter `RE15_ASSETS_PATH` mit Validierung (Pfad muss existieren)
    - Implementiere FATAL_ERROR mit Beispiel wenn Pfad fehlt oder ungültig
    - Setze `RE15_ASSETS_PATH` als Compile-Time-Define für beide Targets
    - _Requirements: 2.1, 2.6, 2.7_

- [x] 2. Gemeinsame Header und Kern-Interfaces definieren
  - [x] 2.1 Engine-Header und Datentypen erstellen
    - Erstelle `include/re15_types.h` mit Fixkomma-Typen (Q12), Plattform-Defines
    - Erstelle `include/re15_engine.h` mit `re15_game_t` Globalstruktur
    - Erstelle `include/re15_error.h` mit `re15_io_result_t` Enum und Fehlerausgabe-Makros
    - Definiere `RE15_PLATFORM_PC` / `RE15_PLATFORM_PSX` Guards
    - _Requirements: 1.2, 1.3, 5.3_

  - [x] 2.2 Rendering-Interface definieren
    - Erstelle `include/re15_render.h` mit `render_backend_t` Funktionszeiger-Tabelle
    - Definiere `render_tri_t`, `render_quad_t` Strukturen (Vertices, UVs, Farben, Textur-Page)
    - Deklariere `extern const render_backend_t* g_render;`
    - _Requirements: 5.1_

  - [x] 2.3 Audio-Interface definieren
    - Erstelle `include/re15_audio.h` mit `audio_backend_t` Funktionszeiger-Tabelle
    - Definiere Funktionssignaturen für VAB-Load, SEQ-Play, SFX-Trigger, Voice-Play
    - _Requirements: 6.1_

  - [x] 2.4 Input- und I/O-Interface definieren
    - Erstelle `include/re15_input.h` mit `re15_input_state_t` (held/pressed/released als uint16_t)
    - Erstelle `include/re15_io.h` mit `re15_io_backend_t` Funktionszeiger-Tabelle
    - Definiere I/O-Funktionen: open, read, get_size, close
    - _Requirements: 7.1, 7.4_

  - [x] 2.5 RDT- und SCD-Datenstrukturen definieren
    - Erstelle `include/re15_rdt.h` mit `re15_rdt_t` (21 Sektions-Pointer gemäß Adresstabelle)
    - Erstelle `include/re15_scd.h` mit SCD-VM-Strukturen (Thread-Slots, Opcode-Enum, Handler-Signatur)
    - Erstelle `include/re15_aot.h` mit `re15_aot_slot_t` (32 Slots, Union für Door/Item/Generic/CamSwitch)
    - _Requirements: 4.1, 4.6, 10.1_

  - [x] 2.6 Spieler-Datenmodell definieren
    - Erstelle `include/re15_player.h` mit `re15_player_t` Struktur
    - Definiere Konstanten: SPEED_WALK (0x4B), SPEED_RUN (0xC8), ROT_SPEED (0x60)
    - Definiere Motion-States-Enum (Idle, Walk, Run)
    - _Requirements: 11.1, 11.2_

- [x] 3. Checkpoint — Build-System kompiliert beide Targets
  - Ensure all tests pass, ask the user if questions arise.

- [x] 4. Datei-I/O und Asset-Auflösung implementieren
  - [x] 4.1 PC I/O-Backend implementieren
    - Erstelle `platform/pc/src/io_pc.c` mit fopen/fread/fclose Wrapper
    - Implementiere `re15_io_backend_t` Funktionszeiger für PC
    - Fehlerbehandlung: Rückgabe von `re15_io_result_t` Codes bei Fehlern
    - _Requirements: 7.6, 7.8_

  - [x] 4.2 PSX I/O-Backend implementieren
    - Erstelle `platform/psx/src/io_psx.c` mit CdSearchFile/CdRead
    - Implementiere 2048-Byte Sektor-Alignment und 4-Byte DMA-Alignment
    - Fehlerbehandlung: Rückgabe von `re15_io_result_t` Codes
    - _Requirements: 7.5, 7.7, 7.8_

  - [x] 4.3 Case-insensitive Pfadauflösung implementieren
    - Erstelle `engine/src/io_common.c` mit `re15_io_resolve_ci()`
    - Auf POSIX: opendir/readdir mit strcasecmp-Match
    - Auf Windows: FindFirstFile (native Case-Insensitivität)
    - Erstelle `re15_assets_get_root()` — erst CMake-Define, dann Env-Variable
    - _Requirements: 2.4, 2.1_

  - [x] 4.4 Property-Test: Case-insensitive Pfadauflösung
    - **Property 2: Case-insensitive Pfadauflösung**
    - Erstelle `tests/property/prop_case_resolve.c`
    - Generiere zufällige Groß-/Kleinschreibungs-Permutationen eines bekannten Dateinamens
    - Prüfe dass alle Permutationen denselben physischen Pfad liefern
    - **Validates: Requirements 2.4**

  - [x] 4.4b RDT-Dateiname-Konstruktion implementieren
    - Erstelle Funktion `re15_rdt_build_path(stage, room_hex, player, out_path)`
    - Format: `STAGE{N}/ROOM{Stage}{RoomHex}{Player}.RDT` mit 2-stelligem Hex (Großbuchstaben)
    - Implementiere Fehlermeldung mit vollständigem Pfad wenn Asset fehlt
    - _Requirements: 2.3, 2.5_

  - [x] 4.5 Property-Test: RDT-Dateiname-Konstruktion
    - **Property 1: RDT-Dateiname-Konstruktion**
    - Erstelle `tests/property/prop_rdt_filename.c`
    - Generiere alle gültigen Kombinationen Stage(1-6), RoomHex(00-27), Player(0/1)
    - Prüfe exaktes Format-Pattern per Regex-Vergleich
    - **Validates: Requirements 2.3**

- [x] 5. RDT-Parser implementieren
  - [x] 5.1 RDT-Adresstabelle und Sektions-Parser
    - Erstelle `engine/src/rdt_common.c` mit `re15_rdt_parse(buffer, size, &rdt)`
    - Parse 21 Einträge der Adresstabelle (Offset 0x08-0x5C)
    - In-Place-Parsing: Liefere Pointer in den geladenen Puffer (kein Kopieren)
    - Behandle Null-Offset (0x00000000) als nicht vorhandene Sektion (Pointer = NULL)
    - _Requirements: 10.1, 10.2, 10.8_

  - [x] 5.2 Property-Test: RDT Null-Offset-Sektionsbehandlung
    - **Property 15: RDT Null-Offset-Sektionsbehandlung**
    - Erstelle `tests/property/prop_rdt_null_offset.c`
    - Generiere RDT-Buffer mit zufälligen Kombinationen von Null-Offsets
    - Prüfe: Null-Offset → Pointer == NULL, alle anderen Sektionen korrekt geparst
    - **Validates: Requirements 10.8**

  - [x] 5.3 Kollisionsdaten-Parser (SCA)
    - Implementiere SCA-Sektion parsen aus RDT (Typ 1: Rechteck, Typ 3: Kreis, Typ 12/13: Treppen)
    - Extrahiere corner_x, corner_z, width, density pro Zelle
    - _Requirements: 10.2, 11.3_

  - [x] 5.4 Kamera- und Zonen-Parser (RID/RVD)
    - Implementiere RID-Kameradefinitionen parsen (Position, Rotation, FOV pro Cut)
    - Implementiere RVD-Trigger-Zonen parsen (Polygone mit cam_from/cam_to)
    - _Requirements: 10.2, 11.6_

  - [x] 5.5 Unit-Tests: RDT-Parser
    - Erstelle `tests/unit/test_rdt_parser.c`
    - Teste mit bekannter RDT-Datei (ROOM1170): Korrekte Sektions-Offsets
    - Teste Fehlerfall: Ungültiger RDT-Header, Null-Offsets
    - _Requirements: 10.1, 10.8, 12.1_

- [x] 6. SCD-VM-Interpreter implementieren
  - [x] 6.1 SCD-VM-Kern und Thread-Management
    - Erstelle `engine/src/scd_vm.c` mit VM-Kern-Logik
    - Implementiere 24 Thread-Slots mit PC, Call-Stack, Sleep-Counter
    - Implementiere Dispatch-Loop: Opcode lesen → Handler aufrufen → Return-Code verarbeiten
    - Implementiere Basisopcodes: Nop, Evt_end, Evt_exec, Sleep, Sleeping
    - _Requirements: 4.6, 10.7_

  - [x] 6.2 Kontrollfluss-Opcodes implementieren
    - Implementiere Ifel_ck (0x06), Else_ck (0x07), Endif (0x08)
    - Implementiere For (0x0D), Next (0x0E)
    - Implementiere Ck (0x21), Set (0x22) für Flag-Prüfung und -Setzung
    - Implementiere Work_set (0x2E), Speed_set (0x2F), Add_speed (0x30)
    - _Requirements: 4.6_

  - [x] 6.3 AOT- und Interaktions-Opcodes implementieren
    - Implementiere Cut_chg (0x29) — Kamerawechsel
    - Implementiere Aot_set (0x2C) — Trigger-Zone setzen (20 Bytes)
    - Implementiere Door_aot_set_4p (0x68) — Tür-Übergang (40 Bytes) mit vollständigem Feld-Parsing
    - Implementiere Item_aot_set (0x4E/0x50) — Item-Zone
    - Implementiere Obj_model_set (0x2D), Se_on (0x36), Sce_em_set (0x44)
    - _Requirements: 4.1, 4.6_

  - [x] 6.4 Property-Test: SCD-Opcode PC-Advancement
    - **Property 6: SCD-Opcode PC-Advancement**
    - Erstelle `tests/property/prop_scd_pc_advance.c`
    - Für jeden implementierten Opcode: Generiere gültige Parameter, führe aus
    - Prüfe: PC wurde um exakt die dokumentierte Opcode-Länge vorgerückt
    - **Validates: Requirements 4.6**

  - [x] 6.5 Property-Test: Door_aot_set_4p Parse-Roundtrip
    - **Property 3: Door_aot_set_4p Parse-Roundtrip**
    - Erstelle `tests/property/prop_door_roundtrip.c`
    - Generiere zufällige gültige 40-Byte-Sequenzen
    - Prüfe: Parse → Serialize → identische Bytes
    - **Validates: Requirements 3.1, 3.2, 4.1**

  - [x] 6.6 Unit-Tests: SCD-Opcodes
    - Erstelle `tests/unit/test_scd_opcodes.c`
    - Mindestens 1 Test pro Opcode mit bekannten Eingabe/Ausgabe-Paaren
    - Teste Kontrollfluss (Ifel_ck/Else_ck/Endif Verschachtelung)
    - Teste Sleep/Sleeping Timer-Dekrementierung
    - _Requirements: 4.6, 12.4_

- [x] 7. Checkpoint — SCD-VM und RDT-Parser funktional
  - Ensure all tests pass, ask the user if questions arise.

- [x] 8. Input-System implementieren
  - [x] 8.1 PC Input-Backend (SDL2 Keyboard + Gamepad)
    - Erstelle `platform/pc/src/input_pc.c`
    - Implementiere `re15_input_init()`, `re15_input_tick()`, `re15_input_get()`
    - Mappe SDL2 Keyboard/Gamepad auf PSX-Button-Layout (16-Bit-Bitmaske)
    - Berechne Edge-Detection: pressed = current & ~previous, released = ~current & previous
    - Definiere Standard-Tastenbelegung (WASD + Pfeiltasten + Enter/Space)
    - _Requirements: 7.1, 7.3_

  - [x] 8.2 PSX Input-Backend (PSn00bSDK Pad)
    - Erstelle `platform/psx/src/input_psx.c`
    - Implementiere Pad-Abfrage einmal pro Frame über PSn00bSDK-Pad-Bibliothek
    - Befülle `re15_input_state_t` mit held/pressed/released
    - _Requirements: 7.1, 7.2_

  - [x] 8.3 Property-Test: Input-Edge-Detection
    - **Property 11: Input-Edge-Detection**
    - Erstelle `tests/property/prop_input_edge.c`
    - Generiere zufällige Paare (previous, current) als 16-Bit-Werte
    - Prüfe: pressed == current & ~previous, released == ~current & previous, held == current
    - **Validates: Requirements 7.1**

- [x] 9. Spieler-Entität und Tank-Controls implementieren
  - [x] 9.1 Tank-Controls Bewegungslogik
    - Erstelle `engine/src/player_common.c` mit `re15_player_update(player, input)`
    - Implementiere Vorwärts/Rückwärts entlang Blickrichtung (sin_q12/cos_q12)
    - Implementiere Links/Rechts Rotation mit ROT_SPEED pro Frame
    - Implementiere Yaw-Wrap (& 0x0FFF für Bereich 0-4095)
    - Implementiere Geschwindigkeitsstufen: Walk (0x4B), Run (0xC8) bei gehaltener Run-Taste
    - _Requirements: 11.1, 11.2_

  - [x] 9.2 Fixkomma-Mathematik (Q12) implementieren
    - Erstelle `engine/src/math_common.c` mit sin_q12/cos_q12 Lookup-Tabellen
    - Implementiere RotMatrix, TransMatrix Äquivalente für PC (20.12 Software)
    - Stelle sicher: Ergebnisse weichen max. ±1 Pixel von GTE-Referenz bei 320×240 ab
    - _Requirements: 5.3, 11.2_

  - [x] 9.3 Property-Test: Tank-Controls Zustandsupdate
    - **Property 12: Tank-Controls Zustandsupdate**
    - Erstelle `tests/property/prop_tank_controls.c`
    - Generiere zufällige Spielerzustände und Input-Kombinationen
    - Prüfe: (a) Bewegung entlang Blickrichtung, (b) Rotation ±ROT_SPEED, (c) Yaw in [0,4095], (d) Animationszustand korrekt
    - **Validates: Requirements 11.2, 11.4**

  - [x] 9.4 Property-Test: Fixkomma-Rendering-Präzision
    - **Property 8: Fixkomma-Rendering-Präzision**
    - Erstelle `tests/property/prop_fixpoint_precision.c`
    - Generiere zufällige 3D-Vertices im Bereich ±32767
    - Transformiere mit PC-Software-Mathe und vergleiche gegen GTE-Referenzimplementierung
    - Prüfe: Max ±1 Pixel Abweichung bei 320×240
    - **Validates: Requirements 5.3**

- [x] 10. Kollisionssystem implementieren
  - [x] 10.1 SCA-Kollisionsprüfung
    - Erstelle `engine/src/collision.c` mit `re15_collision_check(player, sca_data)`
    - Implementiere Zylinder-vs-Rechteck (Typ 1) Durchdringungsprüfung
    - Implementiere Zylinder-vs-Kreis (Typ 3) Prüfung
    - Bei Durchdringung: Position entlang der nächsten Kanten-Normale zurücksetzen
    - Implementiere Treppen (Typ 12/13): Y-Versatz ±0x708 pro Band
    - _Requirements: 11.3_

  - [x] 10.2 Property-Test: SCA-Kollisionserkennung
    - **Property 13: SCA-Kollisionserkennung**
    - Erstelle `tests/property/prop_sca_collision.c`
    - Generiere zufällige Spielerpositionen und SCA-Zellen
    - Prüfe: Durchdringung korrekt erkannt, Rücksetz-Richtung orthogonal zur durchdrungenen Kante
    - **Validates: Requirements 11.3**

- [x] 11. AOT-Trigger-System und Kamerazonen implementieren
  - [x] 11.1 AOT-Slot-Verwaltung und Point-in-4P-Polygon
    - Erstelle `engine/src/aot_common.c` mit 32 AOT-Slots
    - Implementiere `re15_aot_check(player_x, player_z, floor_band)` — iteriert aktive Slots
    - Implementiere Point-in-4-Punkt-Polygon-Test (Cross-Product aller 4 Kanten)
    - Implementiere "entered"-Flag: Setze bei Spawn innerhalb AOT-Zone
    - _Requirements: 4.2, 4.8, 11.5_

  - [x] 11.2 Property-Test: Point-in-4P-Polygon Korrektheit
    - **Property 5: Point-in-4P-Polygon Korrektheit**
    - Erstelle `tests/property/prop_point_in_poly.c`
    - Generiere zufällige konvexe 4-Punkt-Polygone und Testpunkte
    - Prüfe: Containment genau dann true wenn Cross-Product-Test konsistent positiv
    - **Validates: Requirements 4.2, 11.5, 11.6**

  - [x] 11.3 RVD-Kamerazone-Dispatch implementieren
    - Implementiere Kamerazone-Prüfung in `engine/src/camera_common.c`
    - Wenn Spielerposition Zonengrenze überschreitet → aktive Kamera-ID umschalten
    - Lade zugehörigen BSS-Hintergrund-Chunk bei Kamerawechsel
    - _Requirements: 11.6, 10.5_

  - [x] 11.4 Property-Test: Zone-Containment → Kamera/Aktion-Dispatch
    - **Property 14: Zone-Containment → Kamera/Aktion-Dispatch**
    - Erstelle `tests/property/prop_zone_dispatch.c`
    - Generiere Spielerpositionen innerhalb definierter Zonen
    - Prüfe: Aktive Kamera == cam_to der enthaltenden Zone; AOT-Typ → korrekte Aktion
    - **Validates: Requirements 11.5, 11.6**

- [x] 12. Checkpoint — Spieler bewegt sich mit Kollision und Kamerawechsel
  - Ensure all tests pass, ask the user if questions arise.

- [x] 13. Raum-Lade-System implementieren
  - [x] 13.1 Room-Manager mit Lade-/Entlade-Logik
    - Erstelle `engine/src/room_load.c` mit `re15_room_load(stage, room_id, player_id)`
    - Implementiere vollständiges Entladen: Speicher freigeben, AOT-Slots zurücksetzen, SCD-VM reinitalisieren
    - Implementiere Lade-Sequenz: I/O → RDT-Parse → SCD-Init → BSS-Laden → Audio-Bank-Laden
    - Implementiere Null-Offset-Behandlung (Sektion nicht vorhanden → Subsystem ohne Daten initialisieren)
    - _Requirements: 10.1, 10.2, 10.3, 10.7, 10.8_

  - [x] 13.2 BSS-Hintergrund-Laden
    - Implementiere BSS-Datei-Laden und 64KB-Chunk-Selektion nach Kamera-ID
    - Chunk-Offset: `camera_id * 65536`
    - Fehlerfall: BSS fehlt oder Kamera-ID > verfügbare Chunks → schwarzer Hintergrund + Warnung
    - _Requirements: 10.5, 10.9_

  - [x] 13.3 Property-Test: BSS-Chunk-Selektion nach Kamera-ID
    - **Property 16: BSS-Chunk-Selektion nach Kamera-ID**
    - Erstelle `tests/property/prop_bss_chunk.c`
    - Generiere BSS-Dateien mit N Chunks und zufällige Kamera-IDs
    - Prüfe: Geladener Chunk == Bytes an Offset `c * 65536` bis `(c+1)*65536-1`
    - **Validates: Requirements 10.5**

  - [x] 13.4 Stage-Wechsel und Overlay-Laden
    - Implementiere Stage-Change-Logik: Overlay laden + VAB-Bank wechseln
    - Implementiere raumspezifische BGM-Tabelle aktivieren bei neuem Stage
    - _Requirements: 4.4, 10.4_

  - [x] 13.5 PSX-Speicher-Management (Streaming-Fallback)
    - Implementiere Speicher-Budget-Prüfung (max 1,5 MB pro Raum auf PSX)
    - Bei Überschreitung: Nur aktueller Raum + ein vorgeladener Nachbar-Raum im Speicher
    - _Requirements: 10.6_

  - [x] 13.6 Unit-Tests: Raum-Laden (Headless)
    - Erstelle `tests/unit/test_room_load.c`
    - Teste: Raum laden, Sektionen korrekt verfügbar, Entladen gibt Speicher frei
    - Teste: Fehlende RDT-Datei → Fehlerbehandlung
    - _Requirements: 10.1, 12.2_

- [x] 14. Tür-Übergänge und Raumwechsel integrieren
  - [x] 14.1 Tür-Trigger-Logik zusammenführen
    - Verbinde AOT-System mit SCD-VM: Door_aot_set_4p registriert AOT-Slots
    - Implementiere Aktionstaste-Check: Wenn Spieler in Tür-Polygon + Band-Match → Raumwechsel auslösen
    - Implementiere Spawn-Position/Rotation setzen nach Raumwechsel
    - _Requirements: 4.2, 4.3_

  - [x] 14.2 Same-Room-Teleport und Spawn-Entered-Markierung
    - Wenn dest_room == current_room: Kein Neulade, nur Teleport + Kamera-Umschaltung
    - Nach Raumwechsel: Alle Tür-AOTs markieren deren Zone die Spawn-Position enthält ("entered" Flag)
    - Implementiere Fehlerfallbehandlung: Ziel-RDT nicht ladbar → Warnung, Spieler bleibt
    - _Requirements: 4.5, 4.7, 4.8_

  - [x] 14.3 Property-Test: Spawn-Position Tür-Entered-Invariante
    - **Property 7: Spawn-Position Tür-Entered-Invariante**
    - Erstelle `tests/property/prop_spawn_entered.c`
    - Generiere Spawn-Positionen und aktive Tür-AOT-Zonen
    - Prüfe: Jede Tür-AOT deren Polygon die Spawn-Position enthält hat entered==true
    - **Validates: Requirements 4.8**

  - [x] 14.4 Integration-Tests: Raumwechsel
    - Erstelle `tests/integration/test_room_transition.c`
    - Teste: Gleiche-Stage Raumwechsel (Entladen/Laden/Spawn korrekt)
    - Teste: Stage-Wechsel (Overlay + VAB-Bank wechseln)
    - Teste: Same-Room-Teleport
    - _Requirements: 4.2, 4.3, 4.4, 4.7_

- [x] 15. Rendering-Backends implementieren
  - [x] 15.1 PC Rendering-Backend (SDL2/OpenGL)
    - Erstelle `platform/pc/src/render_pc.c` mit `render_backend_t` Implementierung
    - Implementiere init: SDL2-Fenster (320×240 skaliert) + OpenGL-Kontext
    - Implementiere submit_tri/submit_quad über SDL_RenderGeometry
    - Implementiere show_background: BSS-Chunk als Textur rendern
    - Implementiere OT (4096 Einträge) für Z-Sortierung
    - Implementiere 30fps Frame-Timing via SDL_Delay
    - Fehlerfall: SDL-Init/OpenGL-Fehler → Fehlermeldung + Abbruch
    - _Requirements: 5.1, 5.3, 5.4, 5.5, 5.6, 5.7_

  - [x] 15.2 PSX Rendering-Backend (GTE/GPU)
    - Erstelle `platform/psx/src/render_psx.c` mit `render_backend_t` Implementierung
    - Implementiere GTE-Transformationen: RotMatrix, TransMatrix, RTPT, NCCT
    - Implementiere GPU-DMA-Submission vor VSync-Wait
    - Implementiere OT (4096 Einträge)
    - Implementiere MDEC-Decoder für BSS-Hintergründe
    - _Requirements: 5.1, 5.2, 5.4, 5.5, 5.6_

  - [x] 15.3 Property-Test: OT Z-Sortierung
    - **Property 9: OT Z-Sortierung**
    - Erstelle `tests/property/prop_ot_sort.c`
    - Generiere N Primitive mit zufälligen Z-Werten [0, 4095]
    - Füge in OT ein, traversiere, prüfe: Reihenfolge ist höchster Z zuerst (Painter's Algorithm)
    - **Validates: Requirements 5.5**

- [x] 16. Audio-Backends implementieren
  - [x] 16.1 PC Audio-Backend (SDL2 + ADPCM-Decoder)
    - Erstelle `platform/pc/src/audio_pc.c` mit `audio_backend_t` Implementierung
    - Implementiere PSX-ADPCM-Dekodierung (4-bit → 16-bit PCM, Filter-Koeffizienten)
    - Implementiere SDL2 Audio-Callback bei 44100 Hz
    - Implementiere 16-Kanal-Mixer mit linearem Volume (0-127)
    - Implementiere VAB-Laden: VH-Header parsen, VB-Samples dekodieren
    - Implementiere SEQ→MIDI→Sample-Playback (Loop bis explizit gestoppt)
    - Implementiere SFX via Se_on Opcode (Bank 0-15, ID 0-127, Vol 0-127, Pan 0-127)
    - Fehlerfall: VAB nicht ladbar → Warnung, ohne Audio für diese Bank fortfahren
    - _Requirements: 6.1, 6.3, 6.4, 6.5, 6.6_

  - [x] 16.2 PSX Audio-Backend (SPU/libsnd)
    - Erstelle `platform/psx/src/audio_psx.c` mit `audio_backend_t` Implementierung
    - Implementiere SPU-Hardware-Zugriff über PSn00bSDK (SpuSetVoiceAttr, SpuSetKey)
    - Implementiere VAB-Upload in SPU-RAM
    - Implementiere SEQ-Playback über Sequencer
    - _Requirements: 6.1, 6.2, 6.4, 6.5_

  - [x] 16.3 Property-Test: ADPCM-Dekodierung
    - **Property 10: ADPCM-Dekodierung**
    - Erstelle `tests/property/prop_adpcm_decode.c`
    - Generiere gültige 16-Byte ADPCM-Blöcke (Shift 0-12, Filter 0-4, 28 Nibbles)
    - Prüfe: Exakt 28 PCM-Samples, PSX-SPU-konform (Shift, Filter f0/f1, Clamping [-32768, 32767])
    - **Validates: Requirements 6.3**

  - [x] 16.4 Unit-Tests: VAB-Parsing
    - Erstelle `tests/unit/test_audio_vab.c`
    - Teste: Gültiger VAB-Header → korrekte Sample-Anzahl
    - Teste: Ungültiger Magic → Fehlercode ohne Absturz
    - _Requirements: 6.6, 12.1_

- [x] 17. Checkpoint — Komplette Engine-Loop mit Rendering + Audio
  - Ensure all tests pass, ask the user if questions arise.

- [x] 18. Raum-Graph-Extraktion (Asset-Pipeline-Tool)
  - [x] 18.1 Door_aot_set_4p-Extraktor implementieren
    - Erstelle `tools/room_graph/extract_doors.c` (oder Java-Integration falls bestehend)
    - Parse mainScd und subScd aller RDT-Dateien über 6 Stages
    - Extrahiere Opcode 0x68 (40 Bytes): AOT-Slot, Floor/Band, Trigger, Spawn, Dest-Stage/Room
    - Bei unlesbarer SCD-Sektion: Warnung + Datei überspringen, Rest weiterverarbeiten
    - _Requirements: 3.1, 3.2, 3.8_

  - [x] 18.2 Raum-Graph generieren (JSON + Markdown)
    - Erstelle Adjazenzliste als gerichteten Graph
    - Pro Door-Opcode: eine gerichtete Kante mit Ziel-Room, Stage, Spawn-Position, Blickwinkel
    - Kennzeichne ROOM1240 als `start_node: true`
    - Räume ohne Door-Opcodes: `dead_end: true`
    - Ausgabe: `room_graph.json` + `room_graph.md` (Markdown-Tabelle)
    - _Requirements: 3.3, 3.4, 3.5, 3.7_

  - [x] 18.3 Property-Test: JSON-Roundtrip (Raum-Graph)
    - **Property 4: Strukturdaten JSON-Roundtrip**
    - Erstelle `tests/property/prop_json_roundtrip.c`
    - Generiere zufällige Raum-Graph-Datensätze
    - Prüfe: Serialize → Deserialize → semantisch identisch
    - **Validates: Requirements 3.4, 8.5**

- [x] 19. Ghidra-Overlay-Mapping-Tool implementieren
  - [x] 19.1 Dispatch-Pattern-Erkennung in Stage-Overlays
    - Erstelle `tools/overlay_mapper/overlay_mapper.c`
    - Parse STAGE1_overlay.c bis STAGE6_overlay.c
    - Erkenne Jump-Tables (indirekte Sprünge, indiziert durch Room-Index)
    - Extrahiere Zuordnung Room-ID → Funktions-Adresse pro Jump-Table
    - _Requirements: 8.1_

  - [x] 19.2 Mapping-Tabelle generieren (JSON + Markdown)
    - Generiere pro Funktion: Adresse, Stage, Room-ID, vorgeschlagener Name, Katalog-Name, Konfidenz
    - Übernehme Einträge aus RE15_FUN_CATALOG.md per Adress-Abgleich
    - Nicht katalogisierte Funktionen: Konfidenz "low"
    - Namensvorschlag: `room{Stage}{RoomHex}_init` / `room{Stage}{RoomHex}_update`
    - Unaufgelöste Funktionen: room_id "unresolved" + Zusammenfassung pro Stage
    - Ausgabe: `overlay_mapping.json` + `overlay_mapping.md`
    - _Requirements: 8.2, 8.3, 8.4, 8.5, 8.6_

- [x] 20. Kiro-Skills erstellen
  - [x] 20.1 RDT-Analyse-Skill
    - Erstelle `.kiro/skills/rdt-analysis.md`
    - Skill parst RDT-Datei und gibt pro Sektion: Name, Offset, Größe, Einträge-Anzahl aus
    - Definiere Kontext-Laden aus RE15_KNOWLEDGE.md §1.1
    - Fehlerfall: Ungültige Sektion → als fehlerhaft kennzeichnen, Rest weiterverarbeiten
    - _Requirements: 9.1, 9.2, 9.9_

  - [x] 20.2 SCD-Disassembly-Skill
    - Erstelle `.kiro/skills/scd-disassembly.md`
    - Skill übersetzt SCD-Bytecode in Pseudo-Code (Offset, Opcode-Name, Parameter, Beschreibung)
    - Definiere Kontext-Laden aus RE15_KNOWLEDGE.md §1.2 + SCDScriptDisassembler.java
    - Unbekannte Opcodes: Hex-Wert + rohe Bytes ausgeben
    - _Requirements: 9.3, 9.4, 9.9_

  - [x] 20.3 Ghidra-Mapping-Skill
    - Erstelle `.kiro/skills/ghidra-mapping.md`
    - Skill gibt zu FUN_80xxxxxx: Subsystem, Zweck, Confidence zurück
    - Definiere Kontext-Laden aus RE15_FUN_CATALOG.md (gesamter Katalog)
    - Nicht katalogisierte Adressen: Meldung + nächste 3 Nachbarn im 0x1000-Bereich
    - _Requirements: 9.5, 9.6, 9.9_

  - [x] 20.4 Build-Workflow-Skill
    - Erstelle `.kiro/skills/build-workflow.md`
    - Skill generiert Build-Befehle für PSX und PC aus CMake-Konfiguration
    - Bei fehlgeschlagenem Build: Fehler-Output parsen → Datei, Zeile, Lösungsvorschlag
    - _Requirements: 9.7_

  - [x] 20.5 Asset-Pipeline-Skill
    - Erstelle `.kiro/skills/asset-pipeline.md`
    - Skill dokumentiert pro Asset-Typ (RDT, BSS, EMD, TIM, VAB): Extraktionsschritte, Parser-Klassen, Ausgabeformat
    - Definiere Kontext-Laden aus RE15_KNOWLEDGE.md §1.1-§1.8
    - _Requirements: 9.8, 9.9_

- [x] 21. Test- und Validierungssystem implementieren
  - [x] 21.1 Headless-Modus (PC)
    - Implementiere CLI-Flag `--headless` in `platform/pc/main.c`
    - Raum laden + SCD-Ausführung (N Ticks) ohne Rendering
    - Ergebnis auf stdout (JSON: player_pos, flags, errors)
    - Exit-Code: 0 = OK, non-zero = Fehler
    - _Requirements: 12.2_

  - [x] 21.2 Vergleichsmodus (Savestate-Validierung)
    - Implementiere CLI-Flag `--compare <savestate_path>`
    - Vergleiche Spielzustände: Position (Toleranz ±1 Fixkomma), Flags (exakt), Inventar (exakt)
    - Bei Abweichung: Ausgabe der abweichenden Felder mit Soll/Ist-Wert
    - _Requirements: 12.3_

  - [x] 21.3 Property-Test: Savestate-Vergleichstoleranz
    - **Property 17: Savestate-Vergleichstoleranz**
    - Erstelle `tests/property/prop_savestate_cmp.c`
    - Generiere Paare von Positionswerten mit Delta 0, 1, 2, ...
    - Prüfe: |p1-p2| <= 1 → "übereinstimmend", |p1-p2| > 1 → "abweichend" mit Soll/Ist
    - **Validates: Requirements 12.3**

  - [x] 21.4 Debug-Overlays implementieren
    - Implementiere F1-F5 Tastenkürzel für Debug-Overlays (nur PC-Build)
    - F1: Kollisionsboxen (SCA als Drahtgitter)
    - F2: AOT-Zonen (Tür/Item/Event farbcodiert)
    - F3: Kamerazonen (RVD-Polygone mit Kamera-ID)
    - F4: SCD-Trace (letzte 10 Opcodes)
    - F5: Speicher-Belegung (RAM-Balken)
    - _Requirements: 12.6_

- [x] 22. Integration und End-to-End-Verdrahtung
  - [x] 22.1 Game-Loop zusammenführen (PC)
    - Verdrahte alle Subsysteme in `platform/pc/main.c`:
      - Input → Player-Update → Kollision → AOT-Check → Kamera → SCD-Tick → Render
    - Implementiere 30fps Loop mit korrektem Timing
    - Initialisiere alle Backends (Render, Audio, Input, I/O)
    - Lade initialen Raum (ROOM1240 als Startknoten)
    - _Requirements: 1.5, 5.6_

  - [x] 22.2 Game-Loop zusammenführen (PSX)
    - Verdrahte alle Subsysteme in `platform/psx/main.c`:
      - Pad-Input → Player → Kollision → AOT → Kamera → SCD → GTE/GPU-Render
    - Implementiere VSync-synchronisierten 30fps Loop
    - Initialisiere PSX-Hardware (GPU, SPU, CD, Pad)
    - _Requirements: 1.5, 5.6_

  - [x] 22.3 Integration-Tests: Cross-Stage und Headless
    - Erstelle `tests/integration/test_cross_stage.c`
    - Teste: Vollständiger Stage-Wechsel (Stage 1 → Stage 2)
    - Erstelle `tests/integration/test_headless_run.c`
    - Teste: Headless-Modus lädt 5+ Räume sequentiell ohne Fehler
    - _Requirements: 4.4, 12.2, 12.5_

- [x] 23. Abschluss-Checkpoint — Vollständiger Build beider Targets
  - Ensure all tests pass, ask the user if questions arise.

## Notes

- Tasks mit `*` sind optional und können für ein schnelleres MVP übersprungen werden
- Jede Task referenziert spezifische Requirements für Nachverfolgbarkeit
- Checkpoints stellen inkrementelle Validierung sicher
- Property-Tests validieren universelle Korrektheitseigenschaften aus dem Design-Dokument
- Unit-Tests validieren spezifische Beispiele und Fehlerfälle
- Die Implementierung nutzt C als Sprache für beide Plattformen (PSX + PC)
- Test-Bibliothek für Property-Tests: [theft](https://github.com/silentbicycle/theft) (C, MIT)
- Alle Tests laufen auf PC ohne GPU/PSX-Devkit (CI/CD-tauglich)

## Task Dependency Graph

```json
{
  "waves": [
    { "id": 0, "tasks": ["1.1"] },
    { "id": 1, "tasks": ["1.2", "1.6"] },
    { "id": 2, "tasks": ["1.3", "1.4", "1.5"] },
    { "id": 3, "tasks": ["2.1"] },
    { "id": 4, "tasks": ["2.2", "2.3", "2.4", "2.5", "2.6"] },
    { "id": 5, "tasks": ["4.1", "4.2", "4.3", "4.4b"] },
    { "id": 6, "tasks": ["4.4", "4.5", "5.1"] },
    { "id": 7, "tasks": ["5.2", "5.3", "5.4"] },
    { "id": 8, "tasks": ["5.5", "6.1"] },
    { "id": 9, "tasks": ["6.2", "6.3"] },
    { "id": 10, "tasks": ["6.4", "6.5", "6.6"] },
    { "id": 11, "tasks": ["8.1", "8.2", "9.1", "9.2"] },
    { "id": 12, "tasks": ["8.3", "9.3", "9.4", "10.1"] },
    { "id": 13, "tasks": ["10.2", "11.1"] },
    { "id": 14, "tasks": ["11.2", "11.3"] },
    { "id": 15, "tasks": ["11.4", "13.1"] },
    { "id": 16, "tasks": ["13.2", "13.4", "13.5"] },
    { "id": 17, "tasks": ["13.3", "13.6", "14.1"] },
    { "id": 18, "tasks": ["14.2", "14.3"] },
    { "id": 19, "tasks": ["14.4", "15.1", "15.2"] },
    { "id": 20, "tasks": ["15.3", "16.1", "16.2"] },
    { "id": 21, "tasks": ["16.3", "16.4"] },
    { "id": 22, "tasks": ["18.1", "19.1"] },
    { "id": 23, "tasks": ["18.2", "19.2"] },
    { "id": 24, "tasks": ["18.3", "20.1", "20.2", "20.3", "20.4", "20.5"] },
    { "id": 25, "tasks": ["21.1", "21.2", "21.4"] },
    { "id": 26, "tasks": ["21.3", "22.1", "22.2"] },
    { "id": 27, "tasks": ["22.3"] }
  ]
}
```
