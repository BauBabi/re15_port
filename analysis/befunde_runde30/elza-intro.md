# Runde 30 — Thema G: Elza, drei Intro-Fehler (ERMITTLUNG, kein Bau)

Stand 2026-09-28. Gemessen am Arbeitsbaum `master` (e04b9392 = 8d83a025 + Auftragsdatei,
`re15_port/` unveraendert), Bau im eigenen Verzeichnis `re15_port/build_r30_elza-intro`.
An `engine/`, `platform/`, `include/`, `shared_assets/` ist NICHTS geaendert
(`git status --short` dieser vier Baeume: leer).

Alle Adressen in Abschnitt 3 sind in dieser Runde selbst disassembliert
(`re15_disasm.py`, Rohausgabe: `build/r30_elza-intro/disasm_belege.txt`, 1075 Zeilen).
Adressen aus der Vorarbeit (`analysis/befunde_2026-09-27/elza-*.md`) sind nur dort
uebernommen, wo ich das Ziel erneut gelesen habe.

---

## 0. Kurzfassung

Die drei gemeldeten Fehler haben **drei voneinander unabhaengige Ursachen**, alle im Port,
keine in den Daten:

| # | Nutzer sieht | Ursache | Stelle |
|---|---|---|---|
| 1+2 | Lobby-Bild im Vorspann; nach Abbruch "spielt es noch einmal" | Story-Flag (3,193) ist bei Elzas Start 0. ROOM1031 nimmt deshalb den ERSTBESUCHS-Zweig (sub12) und schickt zurueck in die Montage ROOM1241 | `platform/pc/main.c:3839` — der Vorlauf greift nur fuer `0x1170`/`0x1240`, nicht fuer `0x1241` |
| 3a | Leon statt Elza in der Lobby | `scd_vm_init()` nullt `work_vars[0x10]` (angeforderter PL-Index), NACHDEM main.c ihn auf 4 gesetzt hat. Der erste Raumwechsel tauscht PL04 gegen PL00 | `main.c:3344` (setzen) liegt VOR `main.c:3735` (`scd_vm_init`, memset) |
| 3b | Elzas Szene startet nicht, Spieler steht fest | Die Selbst-Tuer-Regel vergleicht ohne Spielervariante: `0x1000 \| raum<<4` = 0x1030 ≠ 0x1031. Kein Neueinstieg, main00 laeuft kein drittes Mal, sub13 startet nie, flag(2,7) bleibt stehen | `engine/src/aot_common.c:645-647` |

Mit drei Ein-Zeilen-Eingriffen an einer KOPIE des Quellbaums (nicht im Repo,
`build/r30_elza-intro/exp_src`, Diff `exp_patches.diff`) laeuft Elzas Einstieg vollstaendig:
Montage → Erzaehler auf Schwarz → Elza (PL04) in der Lobby → ihre Szene → Spielfreigabe.
Leon bleibt dabei bitgleich (15 Bilder, 0 von 691200 Pixeln je Bild abweichend).

⛔ **Berichtigung der Vorarbeit.** `elza-zweig.md` §3.4 beschreibt
`elza_zweig_bilder/elza_lobby_f4000.png` als "Elza in der Lobby (ROOM1031), PL04". Das Bild
zeigt **Leon** (blaue Uniform, Schriftzug "R.P.D." auf dem Ruecken). Die Log-Zeile desselben
Ablaufs steht in jedem meiner Laeufe: `[pld] Spielermodell -> PLD/PL00.PLD (Mesh 17 Teile,
Textur 384x256)`. Die Nutzer-Beobachtung war richtig, die Sichtpruefung der Vorrunde falsch.

---

## 1. Symptom / Auftrag

AUFTRAG.md Abschnitt G, woertlich:

> waehle ich am Anfang Elza aus kommt zum einen im Intro schon kurz ein Bild der Lobby, das
> sollte nicht kommen. Dann, breche ich das intro mit square halten ab, spielt es einfach noch
> einmal. und dann zum anderen, nach dem Intro stehe ich in der Lobby aber mit Leon statt mit
> Elza, und ihre Cutscene spielt nicht ab wie sie soll.

---

## 2. MESSUNG im Port

### 2.1 Werkzeug

| Datei | Zweck |
|---|---|
| `analysis/befunde_runde30/r30_elza_run.sh` | ein Vollstart der gebauten exe (Titel → NEW GAME → Charakterwahl → Spiel), Log + Bildserie |
| `analysis/befunde_runde30/r30_elza_watch.py` | zieht jedes fertige Bild auf einen eindeutigen Namen um (s.u.) |
| `analysis/befunde_runde30/r30_elza_png.py` | PPM → PNG, je Bild Mittelwert und Anteil nicht-schwarzer Pixel |
| `analysis/befunde_runde30/r30_elza_cmp.py` | Pixelvergleich zweier Messordner |
| `analysis/befunde_runde30/r30_elza_exp_run.sh` | dasselbe mit der Experiment-exe |
| `analysis/befunde_runde30/r30_elza_vollstart_riegel.cmake` | der Riegel (Abschnitt 5.4) |
| `re15_port/tests/unit/probe_r30_elza-intro.c` | Engine-Sonde ohne Fenster (Abschnitt 2.6) |

Umgebung jedes Laufs: `RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_PSELECT_AUTO=1 RE15_FADE_LOG=1
RE15_EVT_TRACE=1 RE15_FLAG_TRACE=1 RE15_INPUT_SCRIPT="W2,S1,W900" RE15_INPUT_SCRIPT_START=30`,
fuer Elza zusaetzlich `RE15_PSELECT_AUTO_SWITCH=1`. Bilder ueber `RE15_FRAMEDUMP` (voll
komponierter Frame, 960x720), NICHT ueber `RE15_AUTOSHOT`/`RE15_SOFTWARE_RENDER`.

⛔ **Zwei Messfallen, beide in dieser Runde gelaufen:**

1. `g_engine.frame_count` springt bei JEDEM Raumwechsel auf 0 (`main.c`, Block
   "Frame-Cap des Handoffs relativ zum Eintritt"). Eine Serie `0-12/1` schreibt deshalb in
   jedem Raum-Abschnitt dieselben Dateinamen. Ohne Waechter ueberschreibt der letzte
   Abschnitt alle frueheren — mein erster Lauf `m2` lief so (der Waechter startete nicht,
   weil `MSYS_NO_PATHCONV=1` dem nativen Python `/c/...` unuebersetzt reichte). Alle
   Bildnummern unten sind deshalb **Abschnitt + Bild**, die Dateien heissen
   `k<Ankunftsnummer>_f<Bild>.png`.
2. Der Abbruch der Montage geht nur in einem **Zwei-Bild-Fenster** je Schnitt (Abschnitt 3.4).
   Ein Druck ausserhalb tut nichts. Kontrolllauf `m10`: `RE15_PRESS` Quadrat in Bild 200..212
   → 0 × `Evt_exec sub=3`, die Montage laeuft weiter (`Cut_chg(2)` F363, `(3)` F745).

### 2.2 Elza, Vollstart ohne Abbruch (Lauf `m1`, `m2`)

Log: `build/r30_elza-intro/m1_elza_voll/debug.log` (mit `RE15_SCD_TRACE=1`),
Bilder: `build/r30_elza-intro/m2_elza_voll_bilder/`.

| Abschnitt | Raum | Bild | Log-Zeile | Bedeutung |
|---|---|---|---|---|
| 1 | 1241 | — | `[flow] pselect done ch=1` / `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)` / `[md1] loaded test.md1: 21 meshes` | Wahl kommt an, PL04 geladen |
| 1 | 1241 | F0 | `[scd] F0 slot=10 @0x055A op=0x22`, `@0x055E op=0x22` | sub02 setzt (3,111) und (3,112) im ersten Bild |
| 1 | 1241 | F161…F1249 | `Cut_chg(1)` F161, `(2)` F363, `(3)` F745, `(4)` F855, `(5)` F947, `(6)` F1057, `(7)` F1149, `(8)` F1249 | Montage, 9 Standbilder |
| 1 | 1241 | F1421 | `[scd] F1421 slot=10 @0x061C op=0x47` → `[aot] DOOR FIRE slot=0 … spawn=(-26214,0,-3861)` | Aot_on(0), Tuer nach Raum 0x03 |
| 2 | **1031** | — | `[room] PC loaded room1031.rdt (219120 bytes)` | |
| 2 | 1031 | — | **`[pld] Spielermodell -> PLD/PL00.PLD (Mesh 17 Teile, Textur 384x256)`** | **Fehler 3a: PL04 → PL00** |
| 2 | 1031 | — | **`[evt] F1421 room=1031 Evt_exec sub=12 …`** / `[bg-log] F1421 load room1031#00 ok` | **Erstbesuchs-Zweig, Hintergrund = Lobby Cut 0** |
| 2 | 1031 | F0…F5 | sechs Bilder, s. Tabelle unten | **Fehler 1: die Lobby ist zu sehen** |
| 2 | 1031 | F6 | `[aot] DOOR FIRE slot=18 … spawn=(-26214,0,-3861)` | sub12: Aot_on(18), Tuer nach Raum 0x24 |
| 3 | **1241** | — | `[room] PC loaded room1241.rdt (163748 bytes)` / `[evt] F6 room=1241 Evt_exec sub=2` | **Fehler 2: die Montage beginnt von vorn** |
| 3 | 1241 | F167…F1427 | `Cut_chg(1)` F167, `(2)` F369, `(3)` F751 … | zweiter voller Durchlauf (+6 Bilder Tuer-Einblendung) |
| 4 | 1031 | — | `[room] PC loaded room1031.rdt` / `[evt] F1427 room=1031 Evt_exec sub=15` / `[scd F1427] Cut_chg(13)` | jetzt der Erzaehler (flag(3,193) steht inzwischen) |
| 4 | 1031 | F445 | `[aot] DOOR FIRE slot=19 … target_cut=6 spawn=(-8200,0,-22500)` / `CUT 13 -> 6` | Selbst-Tuer feuert |
| 4 | 1031 | F445…F2730 | **kein einziges `[evt]` mehr**; `[cine] F600 … z2/7=1 cine=1 pmode=2` bis `F2700`, `[walk] F2730 … pl pos=(-8200,0,-22303) rot=1024` | **Fehler 3b: kein Neueinstieg, keine Szene, Spieler skriptgefuehrt und unbeweglich** |

**Die Lobby im Vorspann, Bild fuer Bild** (Abschnitt 2, `m2`; identische Zahlen im
Abbruch-Lauf `m6`):

| Datei | Bild | Mittel (R,G,B) | nicht-schwarz |
|---|---|---|---|
| `k0013_f000000.png` | F0 | (0.0, 0.0, 0.0) | 0.000 |
| `k0014_f000001.png` | F1 | (0.0, 0.0, 0.0) | 0.000 |
| [`k0015_f000002.png`](../../build/r30_elza-intro/bilder/01a_stand_lobby_f2.png) | F2 | (0.0, 0.1, 0.3) | 0.006 |
| [`k0016_f000003.png`](../../build/r30_elza-intro/bilder/01b_stand_lobby_f3.png) | F3 | (0.2, 0.5, 1.5) | 0.027 |
| [`k0017_f000004.png`](../../build/r30_elza-intro/bilder/01c_stand_lobby_f4.png) | F4 | (0.8, 3.9, 11.7) | 0.271 |
| [`k0018_f000005.png`](../../build/r30_elza-intro/bilder/01d_stand_lobby_f5.png) | F5 | (11.8, 24.9, 39.1) | **0.667** |

Vier sichtbare Bilder (F2..F5 = 133 ms bei 30/s), aufsteigend, weil die Tuer-Einblendung
(`step=-6144`, sechs Stufen) gerade laeuft, waehrend sub12 schon wieder hinausschickt.
F5 zeigt ROOM1031 Cut 0: Drehkreuz, Rollgitter, Scherben.

### 2.3 Elza, Abbruch mit Quadrat (Lauf `m6`)

`RE15_PRESS="square@352,…,square@364"` (13 Bilder, deckt das Fenster F361..F363).

```
[evt] F362 room=1241 Evt_exec sub=3 cond=0xff -> slot 11
[evt] F363 room=1241 Evt_exec sub=3 cond=0xff -> slot 12
[aot] DOOR FIRE slot=0 rect=(0,0,hw=0,hh=0) target_cut=0 spawn=(-26214,0,-3861)
[room] PC loaded room1031.rdt (219120 bytes)
[pld] Spielermodell -> PLD/PL00.PLD (Mesh 17 Teile, Textur 384x256)
[evt] F363 room=1031 Evt_exec sub=12 cond=0xff -> slot 10
[aot] DOOR FIRE slot=18 rect=(0,0,hw=0,hh=0) target_cut=0 spawn=(-26214,0,-3861)
[room] PC loaded room1241.rdt (163748 bytes)
[evt] F6 room=1241 Evt_exec sub=2 cond=0xff -> slot 10
[scd F167] Cut_chg(1)   [scd F369] Cut_chg(2)   [scd F751] Cut_chg(3)   …
```

Der Abbruch selbst funktioniert (sub01 → sub03 → Aot_on(0)). "Spielt noch einmal" ist
derselbe Rueckweg wie in 2.2: ROOM1031 sub12 → Tuer 18 → ROOM1241.

### 2.4 Endzustand im Auslieferungsstand (Lauf `m9`, Abschnitt 4, F750)

[`02_stand_leon_in_lobby_f750.png`](../../build/r30_elza-intro/bilder/02_stand_leon_in_lobby_f750.png):
Leon (PL00) steht vor dem Rollgitter, kein Gegner, keine Szene. Alle 16 Bilder F450..F900
haben denselben Mittelwert (30.1, 49.6, 71.5) — es bewegt sich nichts.

### 2.5 Leon zum Vergleich (Laeufe `m4`, `m7`)

| | Leon | Elza |
|---|---|---|
| Startraum | `STAGE1/ROOM1240.RDT` | `STAGE1/ROOM1241.RDT` |
| nach der Montage | `room1170`, **`Evt_exec sub=11`** (Erzaehler), `Cut_chg(7)` | `room1031`, **`Evt_exec sub=12`** (Erstbesuch) |
| Montage ein zweites Mal? | nein | **ja** |
| Selbst-Tuer | `DOOR FIRE slot=3` F445 → `Evt_exec sub=2`, `sub=4`, `sub=5`, `[enemy] EM47 loaded` | `DOOR FIRE slot=19` F445 → **nichts** |
| Abbruch (`m7`, gleiche `RE15_PRESS`) | `Evt_exec sub=3` F362/F363 → room1170 → sub11 → Helipad | `Evt_exec sub=3` F362/F363 → room1031 → sub12 → **room1241** |

### 2.6 Engine-Sonde (ohne Fenster)

`re15_port/build_r30_elza-intro/tests/unit/probe_r30_elza_intro.exe`, Ausgabe
`build/r30_elza-intro/sonde/probe_r30_elza-intro.log`:

```
=== M4: ueberlebt work_vars[0x10] ein scd_vm_init()? ===
  vor  scd_vm_init: work_vars[0x10] = 4
  nach scd_vm_init: work_vars[0x10] = 0   (character & 0x0F = 4)

=== M1: ROOM1031, flag(3,193)=0 (Port-Zustand nach ROOM1241) ===
  ERGEBNIS M1: Raumwechsel nach 1241 angefordert in Bild 1 (flag(3,193) jetzt 1)

=== M2: ROOM1031, flag(3,193)=1 (Original-Zustand) — Erzaehler + Selbst-Tuer ===
  ERGEBNIS M2: Selbst-Tuer in Bild 440, Neueinstieg=0, Akteure=0, flag(3,207)=1, flag(2,7)=1, pmode=2

=== M3: GEGENPROBE — wie M2, Sonde erzwingt den Neueinstieg ===
[neu]   f441  pos=(-13097,    0,-21855) yaw= 1024 cam= 4 | … f3.207=0 f2.7=1 f1.27=1 … evt=3 akteure=6 reenter=1
[ENDE]  f990  pos=( -9097,    0,-17880) yaw= 3081 cam= 0 | … f3.207=0 f2.7=0 f1.27=0 pmode=1 | evt=2 akteure=6
  ERGEBNIS M3: Neueinstieg=1, Akteure=6, Szene zu Ende in Bild 990 (550 Bilder nach der Tuer)
```

### 2.7 Wo der Fehler NICHT liegt (im Auftrag als Verdacht genannt, gemessen)

| Verdacht | Messung |
|---|---|
| ein Cut der Montage ist ein Lobby-Standbild | nein. `BSS/ROOM1240/BG00..08` und `BSS/ROOM1241/BG00..08` sind paarweise byte-gleich (sha256 je Paar identisch); das Lobby-Bild ist `room1031#00` |
| der Montage-Effekt prueft auf 0x1240 statt auf die Basis | nein. `main.c:4534` und `main.c:5015` vergleichen `RE15_ROOM_BASE(g_current_room_id) == 0x1240` |
| die Pre-Intro-Erkennung (`main.c:4101`) greift fuer 1241 anders | nein. `[flow] game loop start room=1241 preintro=0`, Leon `room=1240 preintro=0` |
| die Flags (3,111)/(3,112) aus ROOM1241 sub02 werden nicht gesetzt | doch, in Bild 0 (s. 2.2). Sie werden ausserdem nur in ROOM1190/1191 gelesen (Zensus 3.3) |
| die Tuer-Zielformel `aot_common.c:572-575` verliert die Variante | nein, 1241 → 1031 und 1031 → 1241 sind gemessen. Der Fehler sitzt 70 Zeilen tiefer im GLEICHER-Raum-Zweig |

---

## 3. ORIGINAL-MECHANISMUS

### 3.1 Der Einstieg setzt den Raum, aber KEIN Story-Flag

`FUN_8001d22c` (PSX.EXE), Zweig Neues Spiel:

```
8001d284  lw   v0,-13768(v0)     ; 0x800aca38
8001d288  lui  v1,0x2
8001d290  beq  v0,zero,0x8001d49c ; Bit 0x20000 aus -> LADEN
8001d29c  lw   v0,-13764(v0)     ; 0x800aca3c
8001d2a4  bltz v0,0x8001d324     ; Bit 31 = ELZA
8001d2a8  ori  v0,zero,0x17      ; LEON: Raum 0x17
8001d2b0  sh   v0,4066(at)       ; 0x800b0fe2
8001d324  ori  v0,zero,0x3       ; ELZA: Raum 0x03
8001d32c  sh   v0,4066(at)       ; 0x800b0fe2
8001d330  addiu v0,zero,-8888    ; X  -> 0x800aca94 / playerX
8001d348  addiu v0,zero,-12989   ; Z  -> 0x800aca98 / playerZ
8001d360  addiu v0,zero,-2960    ; Winkel -> 0x800acabe
8001d368  sh   zero,4064(at)     ; Stage 0 -> 0x800b0fe0
```

Gemeinsamer Schwanz:

```
8001d51c  lbu  a0,-13732(a0)     ; a0 = 0x800aca5c (Charakter-Byte)
8001d520  lui  v1,0x8000
8001d538  sw   zero,-13760(at)   ; g_pauseflags = 0
8001d53c  and  v0,v0,v1          ; 0x800aca3c &= 0x80000000 (nur das Elza-Bit bleibt)
8001d558  sh   a0,4080(at)       ; 0x800b0ff0 = angeforderter PL-Index
8001d5a4  jal  0x800314b0        ; Spielermodell laden
8001d5ac  jal  0x800396fc        ; Raum laden
8001d5c8  sb   v0,21591(at)      ; 0x800b5457 = 1
8001d5cc  jal  0x80021634        ; (a0=0,a1=0) Schwarz-Clear AUS
```

**Kein Schreibzugriff auf die Story-Baenke.** Eigener Voll-Scan ueber PSX.EXE und alle acht
Overlays (`r30_elza_flagxref.py 800b0ff8 800b1017`): **0 Treffer** fuer fest verdrahtete
Zugriffe auf Bank 3. Gegenprobe desselben Scanners auf `0x800b0ff0`: 3 Treffer, genau die
bekannten (`@0x8001d558`, `@0x80039768`, `@0x8003977c`). Das EXE-Abbild traegt Bank 3 als
Nullen: `read 0x800b0ff8 8 --w 4` = `[0,0,0,0,0,0,0,0]`.
(Ausgabe: `build/r30_elza-intro/flagxref_bank3.txt`.)

Charakter-Schreiber in TITLE.BIN, erneut gelesen: `801024a4 bne a1,zero,0x801024cc`,
`801024c0 sb zero` (Leon 0), `801024cc ori v0,zero,0x4` + `801024d4 sb` (Elza 4),
`801024c8 and` / `801024e4 or` / `801024ec sw` auf `0x800aca3c`.

### 3.2 Flag-Adressen

Zeigertabelle `0x80074664` (14 Eintraege): Bank 2 → `0x800aca40` (= g_pauseflags),
Bank 3 → `0x800b0ff8`, Bank 4 → `0x800b1018`, Bank 5 → `0x800b1028`.

```
; Ck (0x21), Tabelleneintrag @0x8007452c -> 0x8003fcf4
8003fcfc  lbu  a2,1(v0)          ; Bank
8003fd00  lhu  a1,2(v0)          ; Bit | Wert<<8
8003fd10  srl  v1,a1,3  / 8003fd14 andi v1,v1,0x1c   ; Wortversatz
8003fd18  andi a0,a1,0x1f        ; Bit im Wort
8003fd38  lui  v0,0x8000 / 8003fd40 srlv v0,v0,a0    ; Maske 0x80000000 >> Bit
; Set (0x22), @0x80074530 -> 0x8003fdd0 : @0x8003fe6c `or` setzt, @0x8003fe54-58 `nor`+`and` loescht
```

| Flag | Wort | Maske |
|---|---|---|
| (3,193) | `0x800b1010` | `0x40000000` |
| (3,125) | `0x800b1004` | `0x00000004` |
| (3,207) | `0x800b1010` | `0x00010000` |
| (2,7) | `0x800aca40` | `0x01000000` |

### 3.3 ROOM1031: eine Drei-Zustands-Maschine ueber (3,193) und (3,125)

`ROOM1031.RDT` (219120 B), opcode-exakter Walk (`scd_dump_room.py`), Dump:
`build/r30_elza-intro/scd/ROOM1031.scd.txt`.

```
main00
@0x0204A  06 00 2c 00            Ifel_ck
@0x0204E  21 03 c1 00            Ck(3,193,0)                     ; ERSTBESUCH
@0x02052  3b 12 02 31 01 00 00 00 00 00 00 00 00 00 9a 99 00 00 eb f0 00 00 00 24 …
                                 Door_aot_set Slot 18, Rechteck 0x0,
                                 next_pos (-26214,0,-3861), Stage 0, RAUM 0x24, Cut 0
@0x02072  04 ff 18 0c            Evt_exec sub12
@0x02076  07 00 34 00            Else_ck
@0x0207A  06 00 2a 00            Ifel_ck
@0x0207E  21 03 7d 00            Ck(3,125,0)                     ; ERZAEHLER
@0x02082  3b 13 02 31 01 00 00 00 00 00 00 00 00 00 f8 df 00 00 1c a8 00 04 00 03 06 …
                                 Door_aot_set Slot 19, Rechteck 0x0,
                                 next_pos (-8200,0,-22500), Yaw 0x0400, Stage 0,
                                 RAUM 0x03 (= ROOM1031 SELBST), Cut 6
@0x020A2  04 ff 18 0f            Evt_exec sub15
@0x01E50  06 00 c0 01 / @0x01E54 21 03 7d 01   Ck(3,125,1)       ; SPIELBETRIEB
@0x01E58  24 12 06 00            Save(0x12, 6)
@0x01E5C… 20 x Sce_em_set (Typ 0x16), @0x01FEC Obj_model_set, @0x0200E Evt_exec sub11
@0x020AA  06 00 0a 00 / @0x020AE 21 03 cf 01   Ck(3,207,1)
@0x020B2  04 ff 18 0d            Evt_exec sub13                  ; ELZAS SZENE

sub12 @0x02976   22 03 c1 01   Set(3,193,1)
      @0x0297A   09 0a 01 00   Sleep 1
      @0x0297E   47 12         Aot_on(18)        -> ROOM1241
sub15 @0x02A68   22 03 7d 01   Set(3,125,1)
      @0x02A6C   22 03 cf 01   Set(3,207,1)
      @0x02A70   22 02 07 01   Set(2,7,1)
      @0x02A74   29 0d         Cut_chg 13
      @0x02A7A…  Message_on 0x14 / 0x15 / 0x16 / 0x17, je Sleep 100
      @0x02A9E   47 13         Aot_on(19)        -> ROOM1031 selbst
sub13 @0x02982   22 03 cf 00   Set(3,207,0)
      @0x02986   22 02 07 01 / @0x0298A 22 01 1b 01   Fenster AUF
      @0x0298E   29 04         Cut_chg 4
      @0x02994…  Member_set X=-13097, Z=-21855, Yaw=1024
      @0x029A4…  Plc_motion / Plc_neck / Plc_dest, Message_on 0x12 (@0x02A08), 0x13 (@0x02A24)
      @0x02A4A   22 02 07 00 / @0x02A4E 22 01 1b 00   Fenster ZU
      @0x02A56   42            Plc_ret
```

**Das ist Zeile fuer Zeile Leons Aufbau in ROOM1170** (`ROOM1170.scd.txt`):
`@0x01298 Ck(3,193,0)` → `@0x0129C Door_aot_set Slot 2 … Raum 0x24` → `Evt_exec sub03`
(`@0x0160C Set(3,193,1)`, `@0x01614 Aot_on 2`); sonst `@0x012C8 Ck(3,125,0)` →
`@0x012CC Door_aot_set Slot 3 … Raum 0x17` → `Evt_exec sub11` (`@0x0169A Set(3,125,1)`,
`@0x016D0 Aot_on 3`).

**Flag-Zensus ueber alle 206 begehbaren RDT** (`r30_elza_flag_zensus.py`, Ausgabe
`build/r30_elza-intro/flag_zensus.txt`):

| Flag | Fundstellen | wo |
|---|---|---|
| (3,193) | 4 | ROOM1031 main00 Ck + sub12 Set; ROOM1170 main00 Ck + sub03 Set |
| (3,125) | 8 | nur ROOM1031 und ROOM1170 |
| (3,207) | 3 | nur ROOM1031 (main00 Ck, sub15 Set, sub13 Loeschen) |
| (3,111) | 7 | ROOM1241 sub02 Set; gelesen nur in ROOM1190/1191 |
| (3,112) | 5 | ROOM1241 sub02 Set; gelesen nur in ROOM1190/1191 |

(3,193) hat spielweit keinen weiteren Leser. Ein Vorlauf fuer Elza kann also nichts anderes
treffen als genau diese Weiche.

### 3.4 ROOM1241: Tuerziel, Abbruch, Tastenfenster

```
main00 @0x0051A  3b 00 02 31 … 9a 99 00 00 eb f0 00 00 00 03 …   Door_aot_set Slot 0 -> Raum 0x03
       (ROOM1240 an derselben Stelle: … 00 17 …, einziger Unterschied @0x0531)
sub01  @0x0054A  06 00 0a 00     Ifel_ck
       @0x0054E  51 01 40 00     Sce_key_ck(1, 0x0040)
       @0x00552  04 ff 18 03     Evt_exec sub03
sub03  @0x00620  09 0a 01 00     Sleep 1
       @0x00624  47 00           Aot_on(0)
sub02  @0x0055A  22 03 6f 01  Set(3,111,1)   @0x0055E  22 03 70 01  Set(3,112,1)
       @0x00570  22 02 07 00  Set(2,7,0) / @0x00574 Sleep 2 / @0x00578 Set(2,7,1)   <- FENSTER
       dieselbe Dreiergruppe @0x0058A, @0x005AC, @0x005CC, @0x005EC, @0x0060C
       @0x0061C  47 00           Aot_on(0)
```

Warum "Quadrat HALTEN" und nicht druecken:

```
; Sce_key_ck (0x51), @0x800745ec -> 0x80042920
80042928  lbu  a1,1(v0)          ; Parameter
8004292c  lhu  v1,2(v0)          ; Maske
8004293c  lw   v0,-14488(v0)     ; 0x800ac768 = VIRTUELLES Halte-Wort
80042944  and  v1,v1,v0
80042948  bne  v1,zero,0x80042954 / 8004294c addu v0,a1,zero / 80042950 xori v0,a1,0x1

; FUN_80030444 baut das Wort und MASKIERT es waehrend einer Szene
800304b8  lw   v0,0(a3)          ; Zeiger aus 0x80073e1c[Preset]; [0] -> 0x80073dbc
800304c4  lhu  v0,0(v1)          ; Roh-Maske des virtuellen Bits
800304d4  sllv v0,t1,a0          ; 1 << Bit
800304f4  lw   v0,-13760(v0)     ; g_pauseflags
800304f8  lui  v1,0x100          ; 0x01000000 = flag(2,7)
80030500  beq  v0,zero,0x80030520
80030514  andi v0,v0,0xf000
8003051c  sw   v0,-14488(at)     ; 0x800ac768 &= 0xf000
```

Tabelle `0x80073dbc` (16 x u16): `[0x1000,0x2000,0x4000,0x8000,0x1000,0x4000,0x0080,0x0080,
0x0008,0x0040,0x0008,0x0004,0x8000,0x2000,0x0080,0x0040]` — virtuelles Bit 6 (Maske 0x0040)
liest Roh-Bit `0x0080` = Quadrat (Preset 0; Preset 1 = `0x80073ddc` traegt dort `0x0020`).
Solange flag(2,7) steht, ist Bit 6 weggeblendet; die
Montage oeffnet je Schnitt genau zwei Bilder lang. Der Port bildet das bereits nach
(`game_step_common.c:1106-1109`), gemessen: Fenster F159..F161, F361..F363, F743..F745,
F945..F947, F1147..F1149, F1329..F1331.

### 3.5 Jede Tuer laedt den Raum neu — auch die Selbst-Tuer

```
; Aot_on (0x47), @0x800745c4 -> 0x800407bc
800407d4  lbu  v0,1(v0)  … 800407ec lw v1,0(at)   ; Record aus 0x800ac9b0[Slot]
8004080c  lbu  v0,0(v1)  … 80040824 lw v0,0(at)   ; Handler aus 0x8007469c[sce]
8004082c  jalr v0
; sce-2-Handler, 0x8007469c[2] -> 0x800430bc
800430c4  sw   a0,-13912(at)     ; 0x800ac9a8 = Tuer-Payload
800430d4  sb   v0,21337(at)      ; 0x800b5359 = 1 (Uebergang anfordern)
800430dc  lui  v1,0xff00 / 800430e0 or / 800430e4 sw   ; g_pauseflags |= 0xff000000

; FUN_8001d600, Tuer-Zweig
8001d618  bne  v0,zero,0x8001d82c ; Payload vorhanden -> Tuer
8001d87c  lh   v0,0(a0)          ; X      8001d89c lh v0,2(a0) ; Y     8001d8bc lh v0,4(a0) ; Z
8001d8dc  lhu  v0,6(a0)          ; Winkel
8001d930  lbu  v1,10(a0)         ; Cut    -> 0x800afbb5 / 0x800b0fe4
8001d94c  lbu  v0,9(a0)          ; RAUM   -> 8001d95c sh v0,4066(at)  0x800b0fe2
8001d960  lbu  v0,8(a0)          ; STAGE
8001d968  beq  v1,v0,0x8001d988  ; verglichen wird NUR die Stage
8001d980  jal  0x80039a30        ; (nur bei Stage-Wechsel)
8001d988  jal  0x800396fc        ; RAUMLADER — unbedingt
```

Es gibt keinen Vergleich Zielraum gegen aktuellen Raum. Der Raumlader endet mit der
SCD-Raum-Init:

```
; FUN_800396fc
80039760  lbu  a0,0(s0)          ; 0x800aca5c
80039768  lh   v1,4080(v1)       ; 0x800b0ff0 angeforderter PL-Index
8003976c  andi v0,a0,0xf
80039770  beq  v0,v1,0x80039790  ; gleich -> kein Modellwechsel
80039774  andi v0,a0,0xf0 / 8003977c lbu v1,4080(v1) / 80039784 or v0,v0,v1
80039788  jal  0x800314b0        ; Spielermodell NEU LADEN
8003978c  sb   v0,0(s0)
800397e0  lhu  v0,0(v1)          ; Basis-Dateiindex
800397e4  srl  a0,a0,31          ; Elza-Bit
800397ec  addu a0,a0,v0          ; Index = Basis + Elza
80039a00  jal  0x8003ef6c        ; SCD-Raum-Init

; FUN_8003ef6c
8003efa0  lw   v0,64(v0)         ; RDT+0x40 = main/init
8003efb0  jal  0x8003ee3c        ; (0,0) Slot 0
8003efc4  lw   v0,68(v0)         ; RDT+0x44 = sub
8003efd4  jal  0x8003ee3c        ; (1,0) Slot 1 = sub00
8003f018  jal  0x8003f0a0        ; Dispatcher laeuft SOFORT

; FUN_8003ecec (Raum-Reset, von 0x8003ef84) raeumt punktuell:
8003ed60 sh 0xff,0x800b0ff4 / 8003ed74 sw zero,0x800b1028 (Bank 5, Wort 0) /
8003ed7c sh zero,0x800b0ff2 / 8003ed84 sh zero,0x800aca50 / 8003ed94 sb zero,0x800b281e
; 0x800b0ff0 und Bank 3 (0x800b0ff8..) sind NICHT dabei.
```

### 3.6 Die Reihenfolge des Originals — am Savestate gemessen (Leon)

`r30_elza_ss_sweep.py` ueber alle 84 lesbaren `stage_saves/*.sav`
(Ausgabe `build/r30_elza-intro/ss_sweep.txt`). Alle fuenf Savestates, die IN der Montage
stehen (Raum 0x24), tragen **Vorraum 0x17 und flag(3,193) = 1, flag(3,125) = 0**:

| Savestate | Raum | Cut | Vorraum `0x800b0fe6` | (3,193) | (3,125) | `@0x80026e4c` |
|---|---|---|---|---|---|---|
| `montage1240_orig.sav` | 0x24 | 2 | 0x17 | 1 | 0 | `0800e003` (Auslieferung) |
| `orig_intro_late.sav` | 0x24 | 2 | 0x17 | 1 | 0 | `0800e003` |
| `fx_R60.sav` | 0x24 | 5 | 0x17 | 1 | 0 | `0800e003` |
| `narrator_orig.sav` | 0x24 | 7 | 0x17 | 1 | 0 | `0800e003` |
| `mzd_debugmenu.sav` | 0x24 | 1 | 0x17 | 1 | 0 | `0800e003` |

Das Original kommt also aus dem Startraum in die Montage und hat (3,193) zu diesem
Zeitpunkt schon gesetzt. Fuer Elza gibt es **keinen** Savestate (0 von 84 mit
`0x800aca5c != 0` oder Elza-Bit); ihr Ablauf ist statisch belegt (3.1, 3.3), nicht dynamisch.

### 3.7 Der Erzaehler-Cut 13 ist schwarz, ohne Sonderfall

| Groesse | Wert |
|---|---|
| `BSS/ROOM1031/BG13.BSS` | sha256 `44cafcd7ff20b961…` = `ROOM1170/BG07.BSS` = `ROOM1240/BG00.BSS` (byte-gleich) |
| Kamera ROOM1031 Cut 13 @Datei 0x200 | pos (-12834,-3114,-9774) tgt (-7794,-2196,-22446) |
| Kamera ROOM1170 Cut 7 @0x140, ROOM124x Cut 0 @0x60 | dieselben sechs Werte |
| Kamera ROOM1031 Cut 0 @0x60 | dieselben sechs Werte |

Die "Leer-Kamera" der Montage ist die Lobby-Kamera 0. Der Spieler steht waehrend des
Erzaehlers auf dem Montage-Payload (-26214,0,-3861); relativ zur Kamera
(-13380, +5913), Blickrichtung (+5040, -12672), Skalarprodukt < 0 → hinter der Kamera.
Gemessen am Experiment (Lauf `x2`, Bilder `k0013`..`k0025` = F0..F12 nach dem Uebergang):
13 von 13 Bildern mit `nicht-schwarz = 0.000`; ab F6 hebt nur der Erzaehlertext den
Mittelwert auf 0.8. Der Sonderfall `main.c:5886` (Raum 0x1170 + Cut 7) wird dafuer nicht
gebraucht.

---

## 4. URSACHE — warum der Port die Symptome zeigt

### 4.1 Fehler 1 und 2: der Vorlauf von (3,193) fehlt fuer Elza

Der Port startet nicht im Startraum, sondern in der Montage, und stellt dafuer den Zustand
her, den das Original an dieser Stelle schon hat. `platform/pc/main.c:3839`:

```c
if (boot_room == 0x1170 || boot_room == 0x1240) {
    re15_game_flag_set(3, 193, 1);
```

Fuer Elza ist `boot_room == 0x1241`, die Bedingung ist falsch, (3,193) bleibt 0. ROOM1031
main00 nimmt `Ck(3,193,0)` @0x0204E als wahr, sub12 setzt das Flag nach und feuert
Aot_on(18) zurueck in die Montage. Dazwischen liegen 6 Bilder Lobby (Fehler 1), danach
laeuft die Montage erneut (Fehler 2).

Runde 35 hat den Vorlauf **bewusst** auf den geraden Ids gelassen (`elza-zweig.md` §2.6:
"Ihn auf die Basis-Id zu oeffnen haette Elza einen Leon-Flag untergeschoben"). Das war
eine Annahme ueber die Daten, keine Messung: (3,193) ist kein Leon-Flag, sondern die
gemeinsame Vorspann-Weiche beider Startraeume (3.3, Zensus: 4 Fundstellen).

### 4.2 Fehler 3a: die Reihenfolge in main.c

```
main.c:3343  s_player_model_idx    = g_gameflow.character & 0x0F;      -> 4
main.c:3344  g_scd.work_vars[0x10] = (int16_t)(g_gameflow.character & 0x0F);   -> 4
main.c:3735  scd_vm_init();        -> memset(&g_scd, 0, …)   (scd_vm.c:469)  -> 0
```

Beim ersten Raumwechsel liest `pc_sync_player_model` (`main.c:1253`)
`want = work_vars[0x10] = 0`, vergleicht gegen `s_player_model_idx = 4` und laedt
`PLD/PL00.PLD`. Das ist genau der Rueckfall, den der Kommentar an `main.c:3332-3342`
verhindern sollte. Der Lade-Weg ist NICHT betroffen: `re15_savedata_restore` setzt
`work_vars[0x10]` NACH `scd_vm_init` (`re15_savedata.c:229`), deshalb ist
`unit_elza_zweig` Teil E2 gruen.

### 4.3 Fehler 3b: die Selbst-Tuer-Regel kennt keine Variante

`engine/src/aot_common.c:645-647`:

```c
if (d->dest_room != 0 &&
    (0x1000u | ((unsigned)d->dest_room << 4)) == g_current_room_id)
    g_scd_pending_scenario = (int)d->target_cut;
```

Fuer ROOM1031: `0x1000 | (0x03 << 4)` = `0x1030` ≠ `0x1031`. `g_scd_pending_scenario`
bleibt -1, `re15_game_step` steigt nicht neu ein, main00 laeuft nicht zum dritten Mal.
Folgen: kein `Sce_em_set`, kein `Evt_exec sub13`, und weil sub15 `Set(2,7,1)` @0x02A70
gesetzt und niemand es geloescht hat, zieht main.c `player_mode` jedes Bild auf 2.

Zensus aller Selbst-Tueren (`r30_elza_selbsttuer_zensus.py`, Ausgabe
`build/r30_elza-intro/selbsttuer_zensus.txt`): **67** `Door_aot_set` zeigen auf den eigenen
Raum. Die Port-Regel greift fuer **11** (alle Stage 1, gerade Id), fuer **56** nicht —
32 davon Variante 1, 24 Variante 0 ausserhalb Stage 1 oder in Raum 0.

---

## 5. UMSETZUNGSPLAN fuer den Bau-Agenten

Einordnung nach `reai-v2-beta-zu-retail`: RE1.5 hat dieses System **vollstaendig** (Daten
und EXE), es gilt RE1.5. Kein RE2-Beleg noetig.

### 5.1 Schritt P1 — Vorlauf von (3,193) auf die Basis-Id

| | |
|---|---|
| Datei | `re15_port/platform/pc/main.c:3839` |
| Aenderung | `boot_room == 0x1240` → `RE15_ROOM_BASE(boot_room) == 0x1240`; `boot_room == 0x1170` bleibt |
| Beleg | ROOM1031.RDT sub12 @0x02976 `22 03 c1 01` = Set(3,193,1), ausgefuehrt VOR der Montage (main00 @0x0204E Ck / @0x02072 Evt_exec). Gegenstueck Leon: ROOM1170.RDT sub03 @0x0160C. Savestate: 5 von 5 Montage-Staenden mit (3,193)=1 |
| Kommentar dort | den Satz "ROOM1170-specific" berichtigen: (3,193) wird von ROOM1170 UND ROOM1031 gelesen, sonst nirgends |
| Nicht anfassen | die drei uebrigen `0x1170`-Sonderfaelle (`main.c:3878`, `:5886`, `:7137`) — Elzas Kette beruehrt sie nicht; der Erzaehler-Cut 13 ist ohne sie schwarz (3.7) |

### 5.2 Schritt P2 — angeforderten PL-Index nach `scd_vm_init` setzen

| | |
|---|---|
| Datei | `re15_port/platform/pc/main.c:3735` |
| Aenderung | unmittelbar nach `scd_vm_init();` `g_scd.work_vars[0x10] = (int16_t)(g_gameflow.character & 0x0F);` — Zeile 3344 kann entfallen, 3343 und 3345 bleiben (Statics, vom memset unberuehrt) |
| Beleg | `lbu a0,-13732(a0)` @0x8001d51c → `sh a0,4080(at)` @0x8001d558; Leser `lh v1,4080(v1)` @0x80039768, `beq v0,v1` @0x80039770, `jal 0x800314b0` @0x80039788 |
| Leon | `character & 0x0F` = 0, der memset liefert heute schon 0 → keine Aenderung |

### 5.3 Schritt P3 — Selbst-Tuer mit Variante

| | |
|---|---|
| Datei | `re15_port/engine/src/aot_common.c:645-647` |
| Aenderung | `(0x1000u \| ((unsigned)d->dest_room << 4) \| (g_current_room_id & 0x000Fu)) == g_current_room_id` |
| Beleg | Variante = `srl a0,a0,31` @0x800397e4 + `addu a0,a0,v0` @0x800397ec; Neueinstieg = `jal 0x800396fc` @0x8001d988 unbedingt, `jal 0x8003ef6c` @0x80039a00 |
| Wirkung | 10 Selbst-Tueren kommen dazu: ROOM1031 Slot 19, ROOM1111 Slots 1-4, ROOM1171 Slots 0 und 5, ROOM1191 Slot 15, ROOM11A1 Slots 2 und 3. Leons 11 bleiben, wie sie sind (ROOM1090 Slot 3, ROOM1110 Slots 1-4, ROOM1170 Slots 0/3/6, ROOM1190 Slot 15, ROOM11A0 Slots 2/3) |
| Leon | `g_current_room_id & 0xF` = 0 → Ausdruck unveraendert |

⛔ **Bewusst NICHT Teil dieses Schritts:** die Verallgemeinerung auf Stage ≥ 2 und Raum 0
(46 weitere Selbst-Tueren: 24 in Leons, 22 in Elzas Raeumen — u.a. ROOM5090/5091 Slots
0,2,3,4 = der Endkampf-Raum, ROOM4050/4051 je 12). Byte-true waere sie (3.5), aber sie
aendert Leons Verhalten in 24 Tueren und gehoert in eine eigene Runde mit eigener Messung.

### 5.4 Riegel

**(i) Integration, echte exe** — `analysis/befunde_runde30/r30_elza_vollstart_riegel.cmake`
nach `re15_port/tests/integration/test_elza_vollstart.cmake` uebernehmen und in
`tests/unit/probes/r30_elza-intro.cmake` anmelden:

```cmake
if(TARGET re15_pc)
    add_test(NAME integration_elza_vollstart
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/elza_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_elza_vollstart.cmake)
    set_tests_properties(integration_elza_vollstart PROPERTIES TIMEOUT 240)
endif()
```

Er prueft am `debug.log`:

| | Bedingung | Auslieferungsstand (`m6`) | Experiment (`riegel_exp`) |
|---|---|---|---|
| (a) | erster Hintergrund in ROOM1031 = Cut 13 | **#00** | #13 |
| (b) | ROOM1241 nach dem Start 0 x neu geladen | **1 x** | 0 x |
| (b) | kein `Evt_exec sub=12`, aber `sub=15` in ROOM1031 | **sub12=1** | sub12=0, sub15=1 |
| (c) | keine Zeile `Spielermodell -> PLD/PL00` | **vorhanden** | keine |
| (c) | nach `DOOR FIRE slot=19` ein `Evt_exec sub=13` | **fehlt** | vorhanden |
| (c) | danach `letterbox closed -> gameplay` | **fehlt** | vorhanden |
| | Ergebnis | 6 Fehler, rc=1 | `elza_vollstart OK`, rc=0 |

Laufzeit 100 s (gemessen: Start ~25 s + 363 + 445 + 570 Bilder). Das Spiel hat keinen
Ausstieg nach Gesamtbildern; der Lauf endet ueber die Zeitschranke, geprueft wird allein
das Protokoll. `RE15_MIN_TESTS` in `tools/local_build.sh` mit anheben.

⛔ CMake-Falle, in diesem Skript selbst gelaufen: ein Muster `[pld]` liest CMakes Regex als
Zeichenklasse. Der erste Wurf traf `[window] windowed 960x720` und verfehlte die
`[pld]`-Zeile. Das Skript benutzt deshalb nur klammerfreie Muster. Dieselbe Schwaeche
steckt in `tests/integration/test_boot_bg_pin.cmake` (Filter auf `[bg-log]`); dort faellt sie
nicht auf, weil die Folgepruefungen eigene Woerter suchen.

**(ii) Engine, ohne Fenster** — aus `probe_r30_elza-intro.c` Teil M2 einen Haken machen:
nach `Aot_on(19)` (gemessen Bild 440) muss `g_scd_self_reenter_fired == 1` sein und
flag(3,207) sowie flag(2,7) innerhalb von 600 Bildern auf 0 fallen (gemessen in der
Gegenprobe M3: 550 Bilder). Teil M4 gehoert NICHT in einen Haken — dass `scd_vm_init` nullt,
ist richtig; falsch ist die Reihenfolge in main.c, und die sieht nur Riegel (i).

### 5.5 Wie gemessen wird, dass es stimmt

1. Riegel (i) gruen.
2. Bildserie `0-12/1` mit Waechter: im Abschnitt nach dem Uebergang 1241 → 1031 muessen alle
   13 Bilder `nicht-schwarz = 0.000` haben (Experiment `x2`: erfuellt).
3. Leon vorher gegen nachher, Serie `200-1600/200`, 15 Bilder: 0 abweichende Pixel
   (Experiment `x3` gegen Auslieferungsstand `m8`: erfuellt,
   `build/r30_elza-intro/leon_stand_gegen_exp.txt`).

### 5.6 Das Experiment (Beleg, dass der Plan traegt)

Quellkopie `build/r30_elza-intro/exp_src` (nur `engine/`, `include/`, `platform/pc`,
`platform/psx`), drei Eingriffe laut `build/r30_elza-intro/exp_patches.diff`, Bau in
`build/r30_elza-intro/exp_build`. Lauf `x1` (Elza, Abbruch im Fenster F361..F363):

```
[r30-exp] work_vars[0x10] nach scd_vm_init = 4
[evt] F363 room=1031 Evt_exec sub=15 …          [scd F363] Cut_chg(13)
[aot] DOOR FIRE slot=19 … target_cut=6 spawn=(-8200,0,-22500)
[evt] F445 room=1031 Evt_exec sub=11 …   sub=13 …   sub=2 …
[scd F445] Cut_chg(4)   [scd F521] Cut_chg(6)   [scd F840] Cut_chg(0)
[scd F1013] letterbox closed -> gameplay (player_mode=0)
```

Keine `[pld]`-Zeile, kein zweites `PC loaded room1241`.

| Bild | zu sehen |
|---|---|
| [`03_exp_erzaehler_f90.png`](../../build/r30_elza-intro/bilder/03_exp_erzaehler_f90.png) | Schwarz, Text "We barricaded ourselves inside the police station..." |
| [`04_exp_elza_szene_f480.png`](../../build/r30_elza-intro/bilder/04_exp_elza_szene_f480.png) | Cut 4, Elza (rot-weisser Anzug, blond) am Rollgitter |
| [`05_exp_elza_gitter_f600.png`](../../build/r30_elza-intro/bilder/05_exp_elza_gitter_f600.png) | Cut 6, Elza vor dem Gitter, dahinter Gegner |
| [`06_exp_elza_text_f930.png`](../../build/r30_elza-intro/bilder/06_exp_elza_text_f930.png) | Cut 0, "Elza: Hey, is anyone here?!" |
| [`07_exp_spiel_f2400.png`](../../build/r30_elza-intro/bilder/07_exp_spiel_f2400.png) | Spielbetrieb, Elza in der Lobby |
| [`08_exp_uebergang_f5_schwarz.png`](../../build/r30_elza-intro/bilder/08_exp_uebergang_f5_schwarz.png) | Bild 5 nach dem Uebergang: schwarz (Auslieferungsstand an derselben Stelle: Lobby, 66,7 %) |

---

## 6. Risiken / offene Fragen

1. **P3 oeffnet neun weitere Tueren in vier Elza-Raeumen fuer den Neueinstieg** (ROOM1111,
   1171, 1191, 11A1). Das ist die Regel, die fuer Leons Raeume heute schon gilt, aber keiner
   dieser vier Raeume ist mit Elza gefahren.
2. **Der Riegel (i) dauert 100 s und faehrt ein Fenster.** Die vier bestehenden GUI-Haken
   flattern unter parallelen Agenten (Memory `reai-v2-gui-tests-flattern…`); ein fuenfter
   erbt das.
3. **Spieler steht nach der Selbst-Tuer auf z = -22303 statt -22500.** Payload @0x02082+18
   ist `1c a8` = -22500; der Port schiebt ihn im selben Bild um 197 Einheiten. Fuer die
   Szene folgenlos (sub13 setzt die Position selbst, @0x02994), als Abweichung notiert.
4. **Sechs Akteure statt zwanzig.** main00 traegt 20 `Sce_em_set` (Typ 0x16), im Port sind
   nach dem Neueinstieg 6 aktiv. `Save(0x12,6)` @0x01E58 und die sechs `Case`-Zweige in
   sub00 (@0x02126…) sprechen dafuer, dass 6 gewollt ist — nachgewiesen habe ich es nicht.
5. **Die Erzaehler-Texte 0x14..0x17 laufen in ROOM1031 als Schreibmaschine**, Leons
   Gegenstuecke in ROOM1170 als Volltext (`re15_room_full_text`, Liste `{0x1170, 0x1240}`).
   ROOM1031 in die Liste zu nehmen traefe alle Lobby-Texte. Welche Darstellung das Original
   hier zeigt, ist offen.
6. **PSX-Ziel und Android.** Android baut `platform/pc/main.c` mit
   (`platform/android/jni/CMakeLists.txt:45`), bekommt P1 und P2 also mit. Das PSX-Ziel hat
   keine Charakterwahl und ist nicht betrachtet.

---

## 7. Was ich ausdruecklich NICHT belegen konnte

| | Aussage | warum nicht |
|---|---|---|
| N1 | Ob im ORIGINAL beim Erstbesuch des Startraums (vor der Montage) Bilder der Lobby bzw. des Helipads sichtbar sind, und wie viele | Kein Savestate aus diesem Fenster (die fuenf Montage-Staende liegen dahinter), kein Emulatorlauf in dieser Runde. Statisch steht nur: `FUN_8001d22c` gibt das Bild frei (`jal 0x80021634` mit 0,0 @0x8001d5cc, `0x800b5457 = 1` @0x8001d5c8), sub12 braucht `Sleep 1` vor `Aot_on`. Dass der Port hier NICHTS zeigt, ist die bestehende Port-Entscheidung fuer Leon (`integration_boot_bg_pin`) und der ausdrueckliche Nutzerwunsch fuer Elza — kein gemessenes Original-Verhalten |
| N2 | Elzas Ablauf im Original dynamisch | 0 von 84 Savestates mit Elza. Belegt ist er statisch aus EXE und RDT |
| N3 | Wer die Story-Baenke beim Neuen Spiel nullt | Der Scan findet keinen fest verdrahteten Schreiber; ein Block-Loeschen ueber Zeiger faende er nicht. Belegt ist nur das Null-Abbild in der EXE |
| N4 | Ob die Gegner hinter dem Gitter im Port richtig AUSSEHEN | Sie erscheinen orange-braun; das Log meldet `[tim] WARNUNG slot 11: … 29.5% der Texel dekodieren zu OPAKEM Schwarz`. Kein Vergleichsbild des Originals |
| N5 | Punkt 6.4 (6 von 20) und Punkt 6.5 (Textdarstellung) | s. dort |
| N6 | Dass ein Nutzer-Lauf OHNE `RE15_PSELECT_AUTO`/`RE15_PRESS` dieselben Bildnummern trifft | Die Charakterwahl und der Tastendruck sind ueber Messhaken gefahren, nicht ueber echte Eingabe. Die Ursachen haengen nicht an der Bildnummer, die Zahlen in 2.2 schon |

---

## 8. Artefakte

| Pfad | Inhalt |
|---|---|
| `analysis/befunde_runde30/elza-intro.md` | dieses Dossier |
| `analysis/befunde_runde30/r30_elza_*.py`, `r30_elza_*.sh`, `r30_elza_vollstart_riegel.cmake` | Werkzeuge |
| `re15_port/tests/unit/probe_r30_elza-intro.c`, `probes/r30_elza-intro.cmake` | Engine-Sonde (kein `add_test`) |
| `build/r30_elza-intro/m1…m10_*/debug.log` | Laeufe Auslieferungsstand |
| `build/r30_elza-intro/x1…x3_*/` | Laeufe Experiment |
| `build/r30_elza-intro/bilder/` | die zwoelf verlinkten Abzuege |
| `build/r30_elza-intro/exp_patches.diff` | die drei Eingriffe des Experiments |
| `build/r30_elza-intro/disasm_belege.txt` | Rohausgabe aller Disassemblierungen |
| `build/r30_elza-intro/scd/ROOM{1030,1031,1170,1171,1240,1241}.scd.txt` | SCD-Dumps |
| `build/r30_elza-intro/{flag_zensus,flagxref_bank3,selbsttuer_zensus,ss_sweep}.txt` | Zensus-Ausgaben |
| `build/r30_elza-intro/riegel_*.log` | Riegel gegen Auslieferungsstand und Experiment |

---

## 9. UMSETZUNG (Bau-Agent Runde 30, Zweig `r30/elza-intro`, 2026-09-28)

Gebaut im Arbeitsbaum `.claude/worktrees/r30_elza` auf master d98e9639 (Engine/Plattform
seit 8d83a025 unveraendert — `git diff --stat 8d83a025 d98e9639 -- re15_port` beruehrt nur
Sonden und `tools/`). Bauverzeichnis `re15_port/build`, Messordner
`build/r30_elza-bau/` (unversioniert).

### 9.1 Vor dem ersten Edit selbst nachgeprueft

| Beleg | Binaerdatei | Ergebnis |
|---|---|---|
| `lbu a0,-13732(a0)` @0x8001d51c, `sh a0,4080(at)` @0x8001d558 | `info/Re1.5/PSX.EXE` (`re15_disasm.py dis 0x8001d518 18`) | wie im Dossier |
| `lh v1,4080(v1)` @0x80039768, `beq v0,v1` @0x80039770, `jal 0x800314b0` @0x80039788 | PSX.EXE | wie im Dossier |
| `lw a0,-13764(a0)` (0x800aca3c) @0x800397b8, `lhu v0,0(v1)` @0x800397e0, `srl a0,a0,31` @0x800397e4, `addu a0,a0,v0` @0x800397ec | PSX.EXE | wie im Dossier; die Quelle von a0 (0x800aca3c) zusaetzlich gelesen |
| `beq v1,v0,0x8001d988` @0x8001d968, `jal 0x800396fc` @0x8001d988 (Sprungziel = Raumlader, nicht nur Aufrufstelle) | PSX.EXE | wie im Dossier |
| `jal 0x8003ef6c` @0x80039a00 | PSX.EXE | wie im Dossier |
| ROOM1031.RDT @0x0204E `21 03 c1 00`, @0x02052..0x02069 Door 18 -> `.. 00 24`, @0x02072 `04 ff 18 0c`, @0x02076 `07 00 34 00`, @0x0207E `21 03 7d 00`, @0x02082 `3b 13` .. @0x02098 `00 03 06`, @0x020A2 `04 ff 18 0f`, @0x020AE `21 03 cf 01`, @0x020B2 `04 ff 18 0d`, @0x02976 `22 03 c1 01 09 0a 01 00 47 12`, @0x02982 `22 03 cf 00`, @0x02A4A `22 02 07 00 22 01 1b 00`, @0x02A68 `22 03 7d 01`, @0x02A9E `47 13` | `shared_assets/PSX/STAGE1/ROOM1031.RDT` (byte-gleich mit `info/Re1.5/PSX/STAGE1/ROOM1031.RDT`) | wie im Dossier |
| ROOM1170.RDT @0x01298 `21 03 c1 00`, @0x0160C `22 03 c1 01` | `shared_assets/PSX/STAGE1/ROOM1170.RDT` | wie im Dossier |

Ausgangszustand mit der vorhandenen Sonde (`build/r30_elza-bau/sonde/vorher.log`): M4 vor 4 /
nach 0; M1 Wechsel nach 1241 in Bild 1; M2 Selbst-Tuer Bild 440, Neueinstieg=0, Akteure=0,
flag(3,207)=1, flag(2,7)=1, pmode=2; M3 Szenenende Bild 990. Identisch mit §2.6.

### 9.2 Gebaut

| Schritt | Datei | Commit |
|---|---|---|
| P1 | `platform/pc/main.c` Vorlauf: `boot_room == 0x1170 \|\| RE15_ROOM_BASE(boot_room) == 0x1240`; Kommentar "ROOM1170-specific" berichtigt (4 Fundstellen mit Adressen) | ea5e0173 |
| P2 | `platform/pc/main.c`: `g_scd.work_vars[0x10] = character & 0x0F` unmittelbar nach `scd_vm_init()`; die alte Zeile vor dem memset entfernt, `s_player_model_idx` und `re15_vest_model_mark` bleiben | b39203d0 |
| P3 | `engine/src/aot_common.c`: Selbst-Tuer-Vergleich `(0x1000 \| raum<<4 \| (g_current_room_id & 0xF)) == g_current_room_id` | ce18f540 |
| Riegel (ii) | `tests/unit/test_r30_elza_selbsttuer.c`, ctest `unit_r30_elza_selbsttuer` (TIMEOUT 60) | 78cc4412 |
| Riegel (i) | `tests/integration/test_elza_vollstart.cmake` (aus der Vorlage), ctest `integration_elza_vollstart` (TIMEOUT 240) | 78cc4412 |
| Sonde | `probe_r30_elza-intro.c`: Tuer-Erkennung nimmt auch den Neueinstieg (s. 9.4) | 78cc4412 |

Beide Riegel sind in `tests/unit/probes/r30_elza-intro.cmake` angemeldet;
`tests/unit/CMakeLists.txt` und `RE15_MIN_TESTS` sind NICHT angefasst (Auftrag).
Keine Asset-Datei geaendert. Die Verallgemeinerung von P3 auf Stage >= 2 / Raum 0 ist
ausdruecklich NICHT gebaut.

### 9.3 Abnahme — Soll/Ist

Alle Laeufe mit der ECHTEN exe, Bilder ueber `RE15_FRAMEDUMP` (voll komponierter Frame,
960x720) mit dem Waechter `r30_elza_watch.py`, Auswertung `r30_elza_png.py` /
`r30_elza_cmp.py`. Umgebung wie §2.1; Abbruch ueber `RE15_PRESS` square@352..364.
"vorher" = exe aus d98e9639 ohne Edit (`build/r30_elza-bau/exe_vorher`), "nachher" = exe
mit P1+P2+P3 (`exe_nachher`).

| Messung | Soll | vorher | nachher |
|---|---|---|---|
| Sonde M4 `work_vars[0x10]` nach `scd_vm_init` (misst die Engine-Funktion, nicht main.c) | — | 0 | 0 (unveraendert richtig: der memset ist korrekt, P2 liegt in main.c) |
| Sonde M2: Selbst-Tuer / Neueinstieg / Akteure / (3,207) / (2,7) | Neueinstieg 1 | Bild 440 / 0 / 0 / 1 / 1 | Bild 440 / 1 / 6 / 0 / 1 (Szene laeuft) |
| Riegel (ii) `unit_r30_elza_selbsttuer` | rc 0 | rc 1, `FAIL(H3)` (Gegenprobe: aot_common.c auf den Stand vor P3 zurueckgesetzt, nur dieses Ziel gebaut) | rc 0; Tuer Bild 440, Neueinstieg 1, Szenenende Bild 989 (549 nach der Tuer, Schranke 600) |
| Riegel (i) auf dem Protokoll des Laufs (`NUR_PRUEFEN=1`) | `elza_vollstart OK` | 6 Fehler, rc 1 (erster BG #00, 1241 1x neu, sub12=1, PL00-Tausch, sub13=0, Szenenende=0) — identisch mit `riegel_stand_m6.log` | OK, rc 0 (erster BG #13, 1241 0x, sub12=0, sub15=1, Tuer19=1, sub13=1, Szenenende=1, kein PL00) |
| Riegel (i) ueber ctest (`integration_elza_vollstart`, eigener Lauf der exe) | Passed | — | Passed, 112.7 s (`ctest -R`, einzeln): MESSWERTE wie Zeile davor, `elza_vollstart OK` |
| Bildserie `0-12/1`, Abschnitt nach 1241 -> 1031, nicht-schwarz F0..F12 | 0.000 in allen 13 | F0 0.000, F1 0.000, F2 0.006, F3 0.027, F4 0.271, F5 0.667 — danach nur 6 Bilder, dann wieder ROOM1241 | 0.000 in allen 13 (F6..F12 Mittel 0.8 = Erzaehlertext) |
| `PC loaded room1241` nach dem Start (Abbruch mit Quadrat) | 0 | 1 | 0 (zwei Laeufe: `n_elza_skip_dicht`, `n_elza_szene`) |
| Spielermodell in ROOM1031 | PL04, keine `[pld] Spielermodell -> PLD/PL00`-Zeile | `[pld] Spielermodell -> PLD/PL00.PLD` (Zeile 479) | keine `[pld]`-Zeile; einzige Modellzeile `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`; Bild F480/F960/F1440 zeigt Elza (rot-weisser Anzug, blond) |
| ROOM1031 nach dem Abbruch | sub15 -> Tuer 19 -> sub13 -> Spielfreigabe | sub12 F363 -> Tuer 18 -> ROOM1241 | `Evt_exec sub=15` F363, `Cut_chg(13)`; `DOOR FIRE slot=19` F445; `sub=11` + `sub=13` F445; `Cut_chg(4)` F445, `(6)` F521, `(0)` F840; `letterbox closed -> gameplay` F1013 |
| Leon vorher gegen nachher, Serie `200-1600/200` | 15 Bilder, 0 abweichende Pixel | — | 15 Bilder, 0 von je 691200 Pixeln abweichend (`leon_vorher_gegen_nachher.txt`); Ereignisfolge (`[evt]`, `DOOR FIRE`, Raumladen) beider Laeufe zeilengleich |
| Suite (ctest, `--timeout 240`) | alles gruen | 360 (master) | **362/362 gruen**, RC=0, 308 s (`ctest --test-dir re15_port/build --timeout 240`, Rueckgabewert ohne Pipe gelesen); darin `integration_elza_vollstart` Passed 108.3 s, `unit_r30_elza_selbsttuer` Passed, `unit_elza_zweig` Passed — kein GUI-Haken musste wiederholt werden |

Abzuege (`build/r30_elza-bau/abzuege/`): `01_vorher_1031_f5_lobby.png` (Lobby Cut 0 im
Vorspann), `02_nachher_1031_f5_schwarz.png`, `03_nachher_1031_f12_erzaehler.png`,
`04_nachher_1031_f480_elza_szene_cut4.png`, `05_nachher_1031_f960_elza_szene_cut0.png`,
`06_nachher_1031_f1440_elza_spielbetrieb.png`.

### 9.4 Abweichungen vom Plan

1. **Sonde nachgezogen.** Nach P3 meldete `probe_r30_elza_intro` M2/M3 "Selbst-Tuer NICHT
   erreicht": der Port steigt jetzt im Tuer-Bild neu ein, sub13 setzt den Spieler im selben
   Bild per Member_set (@0x02994) auf X = -13097 — die Erkennung ueber X = -8200 griff nicht
   mehr. Die Sonde (und der Riegel (ii)) werten deshalb auch `g_scd_self_reenter_fired` als
   Tuer-Bild. Gemessen nachher: Tuer Bild 440, Neueinstieg 1, Akteure 6.
2. **Szenenende 549 statt 550 Bilder nach der Tuer.** Die Gegenprobe M3 des Dossiers stellte
   den Neueinstieg ein Bild SPAETER her (Sonde setzte `g_scd_pending_scenario` nach dem
   Tuer-Bild); der gebaute Port tut es im Tuer-Bild selbst (Szenenende Bild 989).
3. **Riegel (ii) prueft zusaetzlich H0 (Belegbytes aus der ausgelieferten ROOM1031.RDT)** und
   H1 (kein Raumwechsel waehrend des Laufs) — beide aus dem Dossier abgeleitet, keine neuen
   Konstanten ausser den zitierten Datei-Offsets.
4. **Die Dossier-Bilder `m8_leon_voll_bilder` sind halbiert gespeichert** (480x360,
   `r30_elza_png.py --halb`) und taugen nicht als Pixelvergleich. Der Leon-Vergleich laeuft
   deshalb gegen einen EIGENEN Vorher-Lauf mit der unveraenderten exe desselben Baumes
   (`build/r30_elza-bau/exe_vorher`, gebaut aus d98e9639 vor dem ersten Edit).

### 9.5 Was ich NICHT gemessen habe

1. **Das Original selbst.** Kein Elza-Savestate, kein Emulatorlauf in diesem Bau (§7 N1/N2
   gelten unveraendert). Belegt ist der Mechanismus statisch (EXE + RDT, 9.1), gemessen ist
   nur der Port.
2. **Die neun weiteren Selbst-Tueren, die P3 in Elza-Raeumen scharf schaltet** (ROOM1111
   Slots 1-4, ROOM1171 Slots 0/5, ROOM1191 Slot 15, ROOM11A1 Slots 2/3) — keine davon mit
   Elza gefahren (§6.1). Gemessen ist nur ROOM1031 Slot 19.
3. **Nutzerlauf ohne Messhaken.** Charakterwahl ueber `RE15_PSELECT_AUTO(_SWITCH)`, Abbruch ueber
   `RE15_PRESS` (13 Bilder square, deckt das Fenster F361..F363). Ein echtes "Quadrat halten"
   mit dem Pad ist nicht gefahren (§7 N6).
4. **Sichtpruefung nur ueber Framedump**, nicht ueber gdigrab am Fensterhandle. Der Framedump
   ist der voll komponierte Frame (kein `RE15_AUTOSHOT`/`RE15_SOFTWARE_RENDER`).
5. **Android und PSX** nicht gebaut. Android kompiliert `platform/pc/main.c` mit und bekommt
   P1/P2; die PSX-`main.c` setzt (3,193) ohnehin unbedingt und hat keine Charakterwahl.
6. **Offene Punkte des Dossiers, nicht Teil des Plans und nicht angefasst:** Spieler nach der
   Selbst-Tuer auf z = -22303 statt -22500 (§6.3), 6 statt 20 Akteure (§6.4 — nachher
   gemessen 6, wie im Experiment), Erzaehler-Text als Schreibmaschine statt Volltext (§6.5),
   Aussehen der Gegner hinter dem Gitter (§7 N4). Im Spielbetrieb-Bild F1440 steht unten links
   ein angeschnittenes Objekt nah an der Kamera — dasselbe zeigt das Experiment-Bild
   `07_exp_spiel_f2400.png` des Dossiers; nicht untersucht.
