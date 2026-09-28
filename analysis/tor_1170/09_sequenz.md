# Tor ROOM1170 - Dossier 09: die Tuersequenz im Port

Stand 2026-09-28. Auftrag des Nutzers: *"bring es so weit das es wie eine Original Tueranimation
eingebaut ist. Auch der Sound soll so sein wie in Resident Evil 2 dieses Gittertor, das du als
Referenz herangezogen hast."*

**Einordnung (Beta -> Retail).** RE1.5 hat die Tuermaschine vollstaendig und ruft sie bei jedem
Tuerwechsel auf (FUN_8001d600 @0x8001d838/48), aber das einzige Skript ist `Evt_end`
(DOOR00.DO2 @0x9A6 `01 00`) - die Sequenz ist unbespielt, die Raumdaten waehlen kein Archiv.
Das System ist also UNFERTIG; Vorbild ist RE2 Retail. Belegt ist jede Stelle in den Dossiers
03/04 (Maschine, Skripte), 08_re_bildtakt / 08_re_blende / 08_re_ton / 08_re_zeichnen
(RE2 statisch gelesen, je Adresse selbst disassembliert). Die Einblendung des Raums NACH der
Tuer bleibt RE1.5 (re15_room_transition_present): dort ist das System vollstaendig.

## 0. Was gebaut ist

| Teil | Datei | Inhalt |
|---|---|---|
| Maschine | `engine/src/door_seq_common.c`, `include/re15_door_seq.h` | RE2-Skriptmaschine (die 39 Tuer-Opcodes, Handler-Adressen im Quelltext), 10 Objekte, Plaetze 10..13, RotMatrix Befehl fuer Befehl nach @0x8008e1f4 mit rcossin_tbl @0x800adeac, Matrizenkette nach FUN_80014234 (IR-Saettigung, 16-Bit-Lage), Blenden-Kanal nach 0x8002c1a0/0x8002c2b0/0x8002c378 |
| Zuordnung + Archiv | `engine/src/door_seq_tor1170.c`, `engine/src/gen/tor_1170_door.inc` | Port-Tabelle Tuer -> Sequenz (ROOM1170 Slot 0/6, ROOM1171 Slot 0/5; Schluessel Rechteck + Band, nicht Slot); Torarchiv im RE2-Aufbau (9 Skripte + MD1 + TIM) |
| Anbindung | `engine/src/aot_common.c`, `engine/src/game_step_common.c` | Anfrage an der Selbst-Tuer; Ablauf vor dem Wiedereintritt (RE2 FUN_80026b7c @0x80026bf8/bfc, RE1.5 @0x8001d838/48) |
| PC-Szene | `platform/pc/src/door_scene_pc.c` | Abdunkeln des stehenden Bildes (32 Bilder), Door_move-Schleife, Door_exit; Takt 59,826 Hz; Zeichnen nach 08_re_zeichnen 4.2 |
| Standbild | `platform/pc/src/render_pc.c` | end_frame bewahrt Hintergrund, Dreiecke und Untertitel des letzten Bildes; `re15_render_pc_standbild_wiederholen()` |
| Ton | `platform/pc/src/audio_pc.c`, `shared_assets/RE2/TORSE.VBS` | DOOR2E-Tonteil UNVERAENDERT als Mini-Bank; Ton 0 (Skript, Bild 100) und Ton 1 (Door_exit) mit dem RE2-Pegelgesetz (@0x80083760) |
| Werkzeuge | `tools/tor/tor_sequenz_bauen.py`, `tools/tor/tuerseq_referenz.py` | Skripte aus DOOR2E, Archiv, TORSE.VBS, rcossin-Tabelle; Bild-fuer-Bild-Referenz |
| Riegel | `tests/unit/unit_door_seq.c`, `tests/unit/probes/tor_1170.cmake` | Maschine gegen den Katalog-Simulator (DOOR2E V0/V1 + Tor V0/V1, je 291 Bilder), RotMatrix, Zuordnung |

## 1. Die Skripte des Tors (aus DOOR2E)

Byte fuer Byte DOOR2E (04 Abschnitt 4.2), drei Aenderungen:

1. Der Riegel (DOOR2E Mesh 1, Objekt 1/2, Skripte 5/6) entfaellt - das Tor hat keinen. Sein
   `Evt_exec 04 0b 18 05` (@Datei 0x05070) faellt weg; er kostet kein Bild (Vorschub 4 ohne
   Bildende, @0x800538bc), die Zeitachse bleibt gleich.
2. Der Pfosten an der Angel ist ein eigenes Wurzelobjekt (Objekt 1, Mesh 1) wie der feste
   zweite Fluegel in DOOR15 Variante 2 (04 K30). Er bekommt die Kamerafahrt als eigenes Skript
   auf Platz 11 (DOOR2E: Platz des Riegels), gestartet im selben Bild wie die Fahrt des Fluegels
   (Platz 11 laeuft nach Platz 10 im selben Bild, @0x80014068 / 04 1.4).
3. Die Lage des Pfostens: Angel + 364 in z = 191,51 Raumeinheiten (Cut 12, 01 M9) * k 1,90.

Zeitachse (unit_door_seq gegen den Simulator, 0 Abweichungen): Bild 0 Aufbau + Einblenden
(56 -> 0 in 15 Bildern), Bild 100 Ton 0, Bild 130..209 Schwenk +570 (50,1 Grad), ab Bild 200
Fahrt (x +7140, z +-1600), Bild 260 Ausblenden (0 -> 248 in 32 Bildern), Bild 290 Ende = 291
Bilder; danach 1 Wartebild bis Kanal 0 fertig, dann Schwarz und Ton 1.

Variante 0 = Landeplatz-Seite (Slot 0, Angel rechts, aufdruecken), Variante 1 = Laufsteg-Seite
(Slot 6, Angel links, aufziehen) - so wie das Modell liegt (07 Abschnitt 0).

## 2. Takt

VSync(0) = eine Austastung je Bild = 59,826 Bilder/s (08_re_bildtakt 0; psx-spx
graphicsprocessingunitgpu.md:1264). 291 Bilder = 4,864 s, mit Abdunkeln (32) und Warten (1)
324 Bilder = 5,42 s. Der Port taktet mit einem virtuellen Vcount aus SDL_GetPerformanceCounter
(ein ganzzahliger ms-Deckel trifft die Rate nicht: 16 ms = 62,5 fps).

## 3. Blende

RE2 setzt beim `Sce_fade_set` den Pegel selbst (Schritt < 0 -> Schritt + 0x8000, sonst 0) und
`Sce_fade_adjust` schreibt ihn direkt (08_re_blende 1.1/1.2). Die RE1.5-Kanalmaschine des Ports
tut das nicht (woertlich uebersetzt: 64 statt 15 Einblendbilder) - daher fuehrt die
Tuermaschine ihren Kanal 0 selbst; gezeichnet wird er subtraktiv ueber allem
(`re15_render_pc_title_fade_sub`, ABR 2 = B - F). Vor der Szene dunkelt RE2 das stehende Bild
ab (Kanal 0 Pegel 31, (8,8,8) je Bild, 32 Bilder, 08_re_blende 6): der Port spielt dafuer das
zuletzt gezeigte Bild erneut ab und zieht 8k ab. Die Balken sind waehrend der Tuer gesperrt
(0x800cfb74 & 0x4000).

## 4. Zeichnen

Nach 08_re_zeichnen 4.2: Licht = Tuerlicht (@0x8009a470, Farbmatrix 1600, BK 68) mit der Matrix
des VORIGEN Bildes (Skeptiker-Befund bestaetigt; im ersten Bild Nullmatrix -> nur BK),
NCLIP (gezeichnet bei MAC0 >= 0), NCCT mit Eckennormalen, otz = 341*(sz0+sz1+sz2)>>12,
verworfen bei otz < 64, Sortierung nach OT-Platz (0x80 -> (otz>>7)+511), Projektion H = 290,
Bildmitte (160,120). Schwarzer Hintergrund, keine Raummasken
(`re15_render_pc_clear_scene_overlays`). Tuertextur in TIM-Platz 24.

Bekannte Port-Grenze (nicht Tuer-spezifisch): Eckfarben ueber 0x80 kappt der PC-Renderer bei
Textur x 1,0 (08_re_zeichnen 4.6); beim Tor bis zu 7 % dunkler an wenigen Ecken.

## 5. Ton

TORSE.VBS = DOOR2E.DO2[0x0000..0x4AE8) unveraendert (19176 B; Tabelle @0x8009a748
`e8 4a fc d7 0a 00 00 00 4f 9d 00 00`). Aufbau fest: Tonkopf @0, VH @0x10, Nachspann @0xC30,
VB @0xC38 (Tonlader FUN_80014cd0). Ton 0 = Tonkopf-Eintrag 0 `00 00 14 16` (VAG 2, 1,316 s),
Ton 1 = Eintrag 1 `00 00 24 17` (VAG 3, 1,219 s), beide 11025 Hz, SPU-Stimme 22/23, nicht
positional (Lagebyte 0). Pegel nach RE2 (_SsVmKeyOnNow): 10157 / 11198 von 0x3FFF statt der
linearen Port-Formel (die waere 2,08 / 1,65 dB zu laut).

Im Spiel gemessen (`RE15_SE_DEBUG=1`): `se=0 ... pitch=0x400 (11025 Hz) -> SE-Stimme 6 (SPU 22)`,
`se=1 ... -> SE-Stimme 7 (SPU 23)`.

## 6. Abnahme

- `unit_door_seq`: Maschine Bild fuer Bild gegen den Katalog-Simulator, DOOR2E V0/V1 und Tor
  V0/V1 je 291 Bilder, **0 Abweichungen**; RotMatrix an Achsenwinkeln; Zuordnung (Intro-Uebergabe
  Slot 3 und fremde Raeume bleiben aussen vor).
- Echtes Spiel (`RE15_TITLE_SHOT` Auto-Vorlauf, `RE15_DEBUG_JUMP=1170@gp`,
  `RE15_FIRE_AOT=0@600#1170`, `RE15_TUER_SERIE=<dir>`): Tor-Slot 0 startet Variante 0; 32 Bilder
  Abdunkeln des Landeplatzes, 291 Sequenzbilder, 1 Wartebild; danach blendet der Raum in Cut 11
  ein und sub14 startet seinen Dialog. Die Intro-Uebergabe in ROOM1240 (Slot 0, Rechteck 0) blieb
  ohne Sequenz.
- Pruefhaken: `RE15_TUER_TEST=<0|1>` spielt die Sequenz direkt nach dem Start und beendet.

## 7. OFFEN

1. PSX: die Szene laeuft nur auf dem PC; `re15_audio_re2_tor_se` ist dort ein Stub, der Laeufer
   nicht angemeldet (die Anfrage verfaellt, der Tuerwechsel laeuft wie bisher).
2. Kreuz-Raum-Tueren: die Anfrage wird nur an der Selbst-Tuer gesetzt und verbraucht. Soll eine
   Kreuz-Raum-Tuer eine Sequenz bekommen, braucht der Verbrauch vor `re15_room_apply_pending`
   (main.c) denselben Aufruf; ausserdem muss der Schliesston den Raumwechsel ueberleben
   (08_re_ton 5: `re15_audio_load_room_banks` schaltet alle Stimmen ab, RE2 verschont die
   Tuerbank).
3. Wie viele Bilder RE2 zwischen Door_exit und der Raum-Einblendung schwarz haelt, haengt am
   Laden (08_re_blende 4c); der Port laedt synchron und blendet sofort ein.
4. Dynamische Gegenprobe gegen eine RE2-Aufnahme einer DOOR2E-Sequenz steht aus - alles ist
   statisch gelesen und am Simulator gemessen.
