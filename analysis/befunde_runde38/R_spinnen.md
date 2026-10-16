# Runde 38 — Spur R "spinnen" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_spinnen` (Zweig `r38/spinnen`, Basis e2cc7673). Angelegt 2026-10-16 16:23.

## Punkte (Wortlaut des Nutzers, AUFTRAG.md Z. 68-70)
1. "In Stage 2 die Spinnen die fliegen fast alle in der Luft und kommen teilweise nicht aus den Schächten raus wie im Original"
2. "In Stage 2 die Spinnen verschießen kein Gift wie im Original, was dumm ist - denn das Gift ist das einzige das die
   Charaktere vergiften kann und damit der einzige Grund für das Gegengift Item"
3. "In Stage 2 die Minispinnen, die rauskommen, wenn man eine Spinne getötet hat fliegen weitestgehend in der Luft und
   greifen einen nicht an wie im Original"

## Arbeitsstand
- 16:23 Dossier angelegt; Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE gelesen.
- 16:55 Messschiene RE15_SPINNEN_LOG gebaut; ROOM2060 in beiden KI-Modi gemessen (1.1/1.2).

## 1. Messung vorher

### 1.0 Werkzeug
Messschiene `RE15_SPINNEN_LOG=<datei>` (neu, engine/src/spinnen_r38.c `re15_spinnen_r38_log_frame`, Haken in
platform/pc/main.c hinter dem RE15_STATE_LOG-Block): je Bild Spieler (x,y,z,hp,Status-Wort +0x98) und je Spinne
(Typ 0x25/0x26) Zustand/Unterzustaende/grid/floor/RE2-Modus +0x222/Lage x,y,z/Drehung/hp. Laeufe ueber die echte
exe (Messkopie `re15_pc_r38sp_mess.exe`), `RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=<raum>@250`.

### 1.1 Zensus der Stage-2-Spinnen (Sce_em_set, `tools/scd_dump_room.py` ueber ALLE ROOM*.RDT)
Typ 0x25 (Adult) NUR in STAGE2: ROOM2030/2031 (7 Saetze, grid 0x00, floor 3 = y -5400 bzw. Satz 4 floor 9 =
y -16200), ROOM2050/2051 (2x grid 0x00 floor 3, y -5400), ROOM2060/2061 (2 Plaetze je if/else: grid 0x41 floor 2
y -3600 IM SCHACHT, sonst grid 0x00 floor 0), ROOM2070/2071 (12 Saetze grid 0x00 floor 0), ROOM2090/2091
(2x grid 0x00 floor 0), ROOM20A0/20A1 (je if/else grid 0x41 floor 3/2 bzw. grid 0x00 floor 0).
Typ 0x26 hat ausser ROOM1090 (7x = FEUER-Emitter, memory reai-v2-spider-ai) KEINEN Satz; STAGE2.BIN registriert 0x26
gar nicht (Registrierung @0x80116eec-0x801170c8: 0x25 -> 0x801109e4 @0x80116f84 `sw v0,11328(at)`, danach
0x40/0x42/0x49/0x4b — kein Store auf 0x80072c44).

ROOM2060 main00 (Datei 0x0DDA-0x0E74): Obj_model_set 0/1 band 2 bei (5775,-4860,5325)/(-5525,-4860,825) = die
SCHACHTGITTER; `Ck(4,0xFB)==0 -> Sce_em_set 44 00 25 41 02 .. (5800,-3600,6400)` sonst `44 00 25 00 00 .. (5200,0,-1000)`;
`Ck(4,0xD5)==0 -> 44 01 25 41 02 .. (-6600,-3600,800)` sonst `(-3800,0,-1400)`. sub01 (pro Bild): `Ck(4,0xFB)==0 &&
Cmp(work[0x0A]==1)` (= angezeigter Cut 1, work_vars[0x0A] = DAT_800b0fe4 @0x8001d948) -> `Evt_exec sub02` + Set(4,0xFB);
dasselbe mit Cut 5 -> sub03 / (4,0xD5). sub02 @0x133E: `Work_set 2 0` (Gegner 0) + `Member_set 0x0C 0x42` (= +0x09
grid := 0x42, actor_common.c Member 12) + `Work_set 3 0` (Objekt 0 = Gitter) + Se_on + Speed/Add_speed-Schleifen =
das Gitter faellt/klappt auf. => Im Original kommt die Schachtspinne heraus, sobald das Gitter faellt und grid
0x41 -> 0x42 wechselt.

### 1.2 Port-Messung ROOM2060 (Debug-Sprung, Spieler-Spawn (7600,0,3950), Cut 0)
- **RE2-KI (Default)**: beide Spinnen (Slot 1/2) starten mit grid 0x41 floor 2 auf **y = -3600** und laufen im
  RE2-Bodenmodus (+0x222 = 0) frei in X/Z umher — **die ganze Zeit auf y -3600, 3600 Einheiten ueber dem Boden
  (Spieler y 0)**. Sie beissen den Spieler aus der Luft: HP 100 -> 80 (F~249) -> 60 (F~449). Bild
  (Framedump F310/F450/F560/F700): die Spinnen haengen ueber Leon unter der Decke neben dem Schachtgitter.
  => Nutzerpunkt 1 ("fliegen in der Luft") REPRODUZIERT. Ursache im Code: enemy_ai_common.c re15_adult_spider_ai_tick
  mappt +0x10E <- grid 0x41 -> RE2-INIT-Sprungtabelle Eintrag 1 (`k == 1` = nichts) -> Modus 0 BODEN auf der
  Spawn-Hoehe; der RE2-Bodenmodus hat keinen Fall/keine Bodenhoehe.
- **RE1.5-KI** (RE15_AI_FLAVOR=re15), Spieler bleibt in Cut 0: beide Spinnen in Zustand 4 (AMBUSH) Unterzustand 1,
  Clip 0x10, unbewegt im Schacht (5800,-3600,6400)/(-6600,-3600,800) = richtig bis zum Ausloeser.
- **RE1.5-KI**, Spieler auf (6000,3000) = Cut 1 (sub02 feuert sofort): Spinne 1 grid 0x42, Zustand 1, bleibt auf
  y -3600 im Schacht stehen (der Port-Zweig `grid != 0x41 -> Zustand 1` hat keinen Abstieg, Kommentar "drop = OPEN").
  => Nutzerpunkt 1 ("kommen nicht aus den Schaechten raus") REPRODUZIERT.

## 2. RE-Belege

## 3. Umsetzung

## 4. Messung nachher

## 5. Tests

## 6. OFFEN

## 7. Fuer den Nutzer
