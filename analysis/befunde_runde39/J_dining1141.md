# Runde 39 — Spur J "dining1141" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r39_dining1141` (Zweig `r39/dining1141`, Basis 1038dc15). Baut auf Runde 38 Spur F
(`analysis/befunde_runde38/F_dining1141.md`, `F_abnahme_0.md`, `F_abnahme_1.md`; Modul `engine/src/dining_r38.c`,
`gen/dining_r38_programme.inc`, Werkzeug `tools/r38_dining/programme_bauen.py`).

Punkte (Wortlaut AUFTRAG.md, Runde 39, "Mit Room 1141 bin ich noch überhaupt nicht zufrieden"):
1. Zombie-Geraeusche im Hintergrund weg ("Es gibt ja keine Zombies").
2. John kommt durch die TUER statt durch das Tor; waehrend des Tueroeffnens Kamera vorne auf Elza, Tuer-zu-Ton, dann
   Kamera auf die Tuer, wo John bereits da ist.
3. Johns "shout"-Voiceover (Port-Ton Runde 38 F) weg.
4. Statt dessen help01-05 im Mix mit Abstaenden, dazwischen mehrfaches Klopfen gegen eine Tuer.
5. Elza reagiert beim ersten Hoeren nach dem Betreten: "Oh, no, this is John! He is in Trouble, I have to help him!"
   (Nachtrag 2026-10-18: JOHN, nicht Roy).
6. Glasbruch-Ton vor Elzas "What was that?".
7. John schaut ab "It's some Kind of Gorilla...." nicht mehr zu Elza — fixen.
8. Nach dem Tod der Gorillas: "John: Puuuh, that was close! I thought we end up as Gorilla meal…." und "John: Elza,
   follow me, I think I might have found a way out of here!" mit passenden Animationen, statt einfach rauszurennen.
9. Beim Verlassen 1141 -> 1171 kurz Panik-Musik.

Ressourcen (VERTRAG 1.1/1.2): Bank-9-Bits 94, 95; Nachrichten-Ids 1141 50-59.

## 0. Stand / Fortschritt
- 2026-10-18 14:20 Baum sauber auf 1038dc15, Dossier angelegt.
- 14:27 Basis-Bau `local_build.sh configure` + `build` gruen (LOCAL-BUILD-OK).
- 14:55 Messung vorher (Lauf v0 = echter Tuerweg 1131 -> 1141 -> 1171, Wegplan wie integration_r38_dining1141_d, plus
  400 Bilder in 1171) ausgewertet; Ton-Quellen vermessen (Abschnitt 1, 2).

## 1. Messung vorher

Messordner (nicht committet): scratchpad `j39/l_v0` (Lauf v0, exe-Kopie `re15_pc_j39m_v0.exe`, Elza, `RE15_DEBUG_JUMP=1131@gp`,
Flags nach der 1021-Szene, `RE15_GIVE=0x43:1`, `RE15_R38F_GORILLAS_TOT=600`/`_STARR=1`, Wegplan = integration_r38_dining1141_d
+ `WARTE 400` in 1171; `RE15_SE_DEBUG`, `RE15_BGM_CTL_DEBUG`, `RE15_STATE_LOG`, `RE15_NECK_LOG`, `RE15_AUDIO_CAP_SYNC`).
Werkzeuge: scratchpad `j39/lauf.sh`, `vab_analyse.py`, `re2_scan.py`, `zoom.py`, `bogen.py`.

### 1.1 Punkt 1 (Zombie-Geraeusche) und Punkt 4 (kein Klopfen) — was in 1141 klingt
debug.log v0 ab `loaded room1141.rdt`: die Klopf-Schleife (Programm HAEMMERN, Runde 38 F) spielt je Runde
`Se_on bank=0x3C id=10` (F1, F245, F485, F725), `bank=0x3C id=11` zweimal (F35/F65, F275/F305, ...) und `bank=0x3D id=0`
= main42.wav (F155, F395, F635). Port-Bank 0x3C = KLOPF1060 = RE2 ROOM1060 snd0 ("ROOM106.SAP - Cabin sounds - Zombies
breaking in", RE2_SOUND_LISTE.md). Vermessen (dekodiert mit der Abspielrate des Tones, `[se] Stimme`-Zeilen bestaetigen sie):
| Satz | Tone/VAG | Rate | Dauer | Befund |
|---|---|---|---|---|
| 0x0A `00 00 85 20` | Tone 8+9 (1 Zusatzlage) / VAG 5 | 11025 Hz | 1,39 s | breitbandiges Rauschen, kein Grundton (Autokorrelation 0,11-0,25, Schwerpunkt ~2200 Hz) = Krachen/Splittern, KEIN Klopfen |
| 0x0B `00 00 ad 00` | Tone 10 / VAG 6 | 8193 Hz | 2,73 s | durchgehender Grundton 85-110 Hz in JEDEM 40-ms-Rahmen 0,0-2,6 s, Formantbaender 0,5/1/2/3 kHz im Spektrogramm = STIMME (Zombie-Stoehnen) |
Satz 0x0B laeuft zweimal je Schleifenrunde: das sind die "komischen Zombie Geraeusche im Hintergrund". Ein Klopfen gegen
eine Tuer gibt es im Ist-Stand nicht (0x0A ist ein Krachen) — der Nutzer hat recht ("den du aktuell noch garnicht hier drin hast").
Bild: scratchpad `j39/zoom1.png` (Zeilen 1060/0A, 1060/0B).

### 1.2 Punkt 3 (Johns "shout") 
`Se_on bank=0x3D id=0 -> Runde 38 Spur F (Stimme main42.wav)` je Runde, `[voice] clip loaded: main42.wav` (0,95 s, Schnitt
"Help me out." aus room1171/main01.wav, Runde 38 F 2.5) = der "komische shout voiceover".

### 1.3 Punkt 7 (John schaut ab msg 9 nicht zu Elza) — gemessen am Zustands-Log v0
Johns Blick (rot) gegen die Richtung John -> Elza (Port-Yaw: pos_advance @0x800245d8 x += cos, z -= sin):
| Nachricht | Elza | John | rot John | Soll | Fehler |
|---|---|---|---|---|---|
| 40, 3, 4, 5 | (-8069,-10258) | (-5840,-10127) | 2011 | 2010 | 0 Grad |
| 6, 7, 8 (Fensterblick) | dito | dito | 13 | 2010 | -175 Grad (gewollt: Blick zu den Fenstern) |
| **9, 10, 11** | dito | dito | **3341** | 2010 | **+117 Grad** |
| 11 (Ende) | dito | dito | 3917 | 2010 | +168 Grad |
Ab msg 9 steht John 117 Grad neben Elza — Befund bestaetigt.

### 1.4 Punkt 9 (Panik-Musik beim Wechsel 1141 -> 1171)
debug.log v0: `[bgm] stage=0 room=14 entry=FF56 -> MAIN16(flag 1)`; Sce_bgm_control slot 0 op=1 capTick 3674 (Klopf-Schleife),
op=2 capTick 4939 (John frei), op=1 capTick 5792 (Gorilla-Einbruch @0x00E5C), op=2 capTick 6920 (Gorillas tot, sub01 @0x00C06);
1171: `[bgm] stage=0 room=17 entry=5572 -> MAIN32(flag 1) SUB(flags 1/0)`, `Sce_bgm_control slot=0 op=1 capTick=8139` (main00
@0x011D6). Mitschnitt-RMS (30-Takt-Fenster): 6880 1553, 6910 881, 6940 3,6, danach 0 (MAIN16 aus) bis zur Tuer, ab 8110 MAIN32
(RMS 300-1300). In DIESEM Weg spielt beim Eintritt in 1171 also MAIN32 (der Titel des Raums), nicht MAIN16 — Abschnitt 4.9 klaert,
was der Nutzer als "kurz Panik Musik" hoert.
## 2. RE-Belege

## 3. Umsetzung

## 4. Messung nachher

## 5. Tests

## 6. OFFEN

## 7. Fuer den Nutzer
