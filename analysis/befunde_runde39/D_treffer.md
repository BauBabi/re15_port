# Runde 39 Spur D "treffer" — Dossier

Baum `.claude/worktrees/r39_treffer`, Zweig `r39/treffer`, Basis c71fa144 (master nach Welle 1).

## Wortlaut des Nutzers (AUFTRAG.md)

1. "Zombies die halb am aufstehen sind, sind nicht treffbar. Das ist so im Original Resident Evil 2 auf jeden Fall nicht."
2. "Zombies sind über den Tisch hinweg nicht treffbar. Das darf so nicht sein"
3. "Das Auto Aim darf nicht gelten für Gegner die der Spieler sonst garnicht sehen könnte, zum Beispiel wenn sie um die
   Ecke sind und nicht mal ansatzweise in Leons Blickfeld mit Wand davor."

## Stand

- [x] Schritt 0: Lektuere (AUFTRAG/VERTRAG, Runde-16-Dossiers aufstehen-schuss/trefferhoehe/liegende-und-aufstehen,
      Runde-38 c939ce5d Zielhilfe/Band-Tore, Memory hit-latch-93 / tiefe-aus-kollision / kollisionszellen-sind-waende)
- [x] Schritt 1: RE-Belege Mechanik (RE1.5 + RE2, alles selbst disassembliert/gedumpt, s. "RE-Belege")
- [x] Schritt 2: Messung vorher je Punkt (Spielschritt + echte exe + RE2 live)
- [x] Schritt 3: Umsetzung (Punkt 2/3 Code, Punkt 1 Beleg)
- [x] Schritt 4: Messung nachher, Tests (gezielter ctest s. "Tests"), Abschluss

## RE-Belege (Mechanik)

Werkzeuge: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (RE1.5 PSX.EXE), `re2_disasm.py` (RE2
`info/re2leon/PSX.EXE`, Overlay `--bin EMOVL10_S0.BIN` = byte-identisch zu `EMZ0.BIN`, cmp 53068 B), eigene
Python-Dumps der RDT-Kollisionsbloecke (RE1.5 `shared_assets/PSX/STAGE*/ROOM*.RDT` @0x20, RE2 `info/re2leon/PL0/RDT`).

### B1 — RE1.5: Sichtlinie des Spieler-Schusses = Zellen mit Art-Nibble 3 (2D, keine Hoehe)

Resolver `FUN_80011f50` ruft nach der Kandidatenwahl `jal 0x8001b9b4` (Gewinner-Position, +0x34) — Ergebnis != 0 ->
`return 0` (kein Treffer). `FUN_8001b9b4` @0x8001b9b4-ba50: Delta Ziel - Spieler (`lw v1,52/56/60(a1)` @0x8001b9dc-ba04),
Schleife s0 = 3..0 (`ori s0,zero,0x3` @0x8001ba08, `addiu s0,s0,-1` @0x8001ba38) je Region `jal 0x8003dcc4` mit
`ori a2,zero,0xf00` @0x8001ba1c / `ori a3,zero,0x300` @0x8001ba24, ODER der Ergebnisse (`or s1,s1,v0` @0x8001ba28).
`FUN_8003dcc4` (RE_15_Quellcode_V2/FUN_8003dcc4.c): je Zelle der Region (Zeiger `*(rdt+0x20) + 8 + region*4`), Filter
`(w10 << 16 >> 28) == Spieler+0x82` (Band) UND `(w10 & a2) == a3` = `(w10 & 0xf00) == 0x300`; w10 = `u1 | floor<<8`
(re15_rdt.h `re15_sca_entry_t`: width,density,x,z,type,u0,u1,floor), also **`(floor & 0x0f) == 3`**. Geometrie: beide
Diagonalen der Zelle gegen den Strahl (OuterProduct0-Vorzeichen), alle Koordinaten /0x12. KEIN Hoehentest.
Zweiter Aufrufer derselben Routine mit anderer Art: `FUN_8001bafc` (Schraegen-Test des Resolvers) `FUN_8003dcc4(.., 0x400,
0x400)` = Art-Bit 2. Die Art-Bits sind also ein Zellen-ATTRIBUT der Designer (Bit0+Bit1 = sperrt Sicht/Schuss, Bit2 =
Schraege). Port: `re15_los_ray_blocked` (enemy_ai_common.c:2943, `(c->floor & 0x0f) != 3 -> continue`), Spieler-Schuss
`re15_schuss_r36_sichtlinie_blockiert` (schuss_r36.c, Runde 36 C) — byte-true RE1.5.

Zellen-Zensus (eigener Dump, alle Stages, (shape,u0,u1,Art)): 14826 Zellen Art 3, Art 2 u.a. ROOM1020 [2],[3],[11],[13],
[33] (Moebelbloecke 14230x1824 / 10340x3138 / 10482x3304, band 0) — die Designer haben in 1020 Moebel als NICHT
schuss-sperrend (Art 2) markiert. **ROOM1140 (Briefing, Zombies 0x10/0x11/0x16): alle 19 Zellen Art 3 (floor=0x03)**,
darunter die Tischbloecke [3] 22450x9550 @(-5750,-7800), [6] 12100x6100 @(-4650,-17450), [5] 10250x2850 @(4450,-23600),
[9] 19250x1400 @(-4550,-9200), [11]/[12]/[13]/[10]/[7]/[8] und 5 Kreise 1000x1000 (Stuehle). D.h. im RE1.5-Original
sperrt JEDER Tisch in 1140 den Schuss (2D-Strahl, Art 3) — unfertiger Datenstand der Beta (RE2 vgl. B2).

### B2 — RE2: Sichtlinie = Designer-Flag 0x80 je Zelle + Hoehentest; GLEICHE Routine fuer Schuss UND Zielhilfe

`FUN_80050858(a0 = Punkt A xyz, a1 = Punkt B xyz, a2 = Flagmaske, a3 = Hoehentest)` (RE2_Quellcode_V2/FUN_80050858.c,
selbst disassembliert): SCA-Zeiger `lw t1,32(v0)` @0x80050890 (RDT+0x20, wie RE1.5), Zaehler `lw v0,4(t1)` @0x80050898,
Zellen 16 B ab +16 (`addiu t1,t1,16` @0x800508b0); je Zelle `lhu v1,8(t1)` @0x800508bc / `and v0,v1,fp` @0x800508c4 /
`beq v0,zero,0x800508b0` @0x800508c8 = **ohne Flagtreffer wird die Zelle uebersprungen**; Formtabelle `DAT_800a73b4`
@0x800508d0-e4 (Formen 1-5,13 nie), dann XZ-Kreuzungstest (Rechteckkanten), bei a3 != 0 zusaetzlich YZ/YX-Kreuzung mit
der Zellenhoehe: oben = -((type>>11)*100 + ((type>>6)&0x1f)*1800), unten = -1800*ctz(floormask).
Aufrufer (ghidra_re2_Leon.txt XREF): **Schuss-Applier FUN_800410CC** `addiu a2,zero,128` @0x800412e0 / `addiu a3,zero,1`
@0x800412ec / `jal` @0x800412f0 — VOR der Geometrie, je Kandidat (Decompile: `iVar3 = FUN_80050858(local_30,&local_a0,
0x80,1); if (iVar3 == 0) {...}`); **Zielhilfe FUN_80042144** `jal` @0x800422a8 (Decompile `FUN_80050858(Spieler-Teil
+0x1C1 +0x5c, &local_38 = (Gegner+0x84, Teil-y, Gegner+0x8C), 0x80, 1) == 0` als Kandidatenbedingung; danach Vorrang
stehend (`+0x10E & 0x2000 == 0`) vor liegend, naechster gewinnt). Weitere Aufrufer: Navigator 0x8004a8d8 (Maske 0x2000),
FUN_8003d96c/db38 (0x2080), Kraehe 0x8400.
RE2-Zellenzensus (eigener Dump aller `PL0/RDT/ROOM*.RDT`, 16-B-Satz x,z,w,d,id,type,floormask): ~16 % der Zellen OHNE
Flag 0x80 (z.B. 480 von 3283 Rechtecken, dazu 54 Kreise, 123 Form 7, 118 Form 8), Hoehe fast immer 1800 (type>>11 = 18).
**RE2 loest "ueber Moebel schiessen" also ueber das Designer-Flag je Zelle, nicht ueber die Hoehe.** Beispiele ohne Flag
im RPD 1F: ROOM1030 [34]/[35] 1155x13084, ROOM1050 [19] 3690x3490, ROOM1160 [17] 5340x1160, ROOM1190 [4] 10414x4634.

### B3 — Zielhilfe: RE1.5 OHNE Sichtlinie, RE2 MIT

RE1.5 `FUN_8003703c` @0x8003703c-801371a0 (selbst disassembliert): je aktives Wesen `SquareRoot0(dx^2+dz^2)`
@0x800370f8, `lbu a0,-145(s0)` +0x09 / `andi 0x60` @0x8003710c (versteckt), `lh v0,0(s0)` +0x9A HP `bltz` @0x80037120
(Eimer hp<0), `andi a0,0x80` @0x80037124 (Eimer liegend), sonst naechster (`sltu v0,v1,s5` @0x80037130). **Kein
Aufruf von 0x8001b9b4/0x8003dcc4, keine Sichtlinie.** Port `re15_player_aim_target` (re15_damage.c:1075) = dito.
RE2 `FUN_80042144` prueft je Kandidat die Sichtlinie (B2). Beta -> Retail: RE2 ist das Ziel, der Nutzerpunkt 3 ist
exakt das RE2-Verhalten.

### B4 — RE2: Zielhoehe ist MANUELL (D-Pad), keine automatische Nachfuehrung zum Ziel

Ziel-FSM (Halten): `andi v0,s2,0x20` @0x80043588 (Pad TIEF) -> `+0x154 = (+0x154 & 0x1fff) | 0x2000` @0x8004359c-a8;
`andi v0,s2,0x10` @0x800435e0 (Pad HOCH) -> `| 0x8000` @0x800435f8; beide erst ab Bild `DAT_800a6d78[(Waffe-1)*3]&0x7f`
des Halte-Clips (`lbu v1,333(s0)` @0x80043578/d0). Die Bits 29-31 des Spieler-word0 (Geometriegruppe des Appliers
`srl s2,v0,29` @0x80047d18) setzt `FUN_80042368` @0x80042368-246c beim Schuss aus der HANDHOEHE: a1 = Pool[Teil 14].ty
(`lw a1,2504(v0)` @0x80043a28, Waffe 2: Teil 11 `lw a1,1988(v0)` @0x80043a10), Schwelle `(Spieler-y + DAT_800a6dcc[Figur]
[Waffe]) * +0x1DE >> 12` + `DAT_800a6dc8[Waffe]` (lo/hi, Dump: Pistole -200/300, h Leon -2427 / Claire -2120),
`slt v0,t1,v0` @0x80042418 -> UP 0x80000000, `slt v0,v0,t1` @0x80042444 -> DOWN 0x20000000, sonst LEVEL 0x40000000.
Es gibt keinen Leser des Zielzeigers +0x1B4, der die Hoehe/Pose nachfuehrt (Vollsuche `0x1b4)` in RE2_Quellcode_V2:
nur Setzer in FUN_80042144/8003d96c/80045c10). => In RE2 muss der Spieler fuer liegende/aufstehende Zombies selbst
TIEF zielen (bekannte RE2-Mechanik).

### B5 — RE2: Teile-Maske waehrend des Aufstehens (Runde 16 trefferhoehe.md, hier nachgelesen)

EXEC[5] P7 @0x80103628-754 (EMOVL10_S0.BIN): Radius +0x9A/+0x9C/+0x90/+0x92 +10 bis 500 @0x80103628-68, Advance
`jal 0x8002959c` a3=256 @0x801036a0, fertig -> P8; sonst `lbu v1,333(s2)` / `addiu v0,zero,55` / `bne v1,v0,0x80103754`
@0x801036d0-d8 -> NUR bei Clip-Bild 55 `lui v1,0xc00 / or / sw v0,0(s2)` @0x801036dc-f0 (Maske Beine+Rumpf zurueck).
Vorher (P0..P7 < Bild 55) Maske = nur Beine (0x04000000, @0x801032B8-EC). Applier-Zeile EBEN bei dy=0: row 3, Satz
`DAT_800a6db4+9+3` = [02 00 00] -> Treffer nur mit Rumpf-Bit -> **EBEN trifft den Aufsteher in RE2 erst ab Bild 55**,
TIEF (row 6 = [01 02 00], Beine zuerst) trifft ihn die ganze Zeit (Nahbox < 4100). Port: `re2z_frame_slot(e) == 55`
(enemy_ai_re2_zombie.c:2508) und `re15_re2_gun_probe` (re15_damage.c:1933) — nachzumessen (Schritt 2).

### B6 — Zielhoehe (D-Pad) ist in RE1.5 UND RE2 NICHT klebrig: Loslassen = MITTE (eigener Disasm, Fortsetzung 12:xx)

Hypothese der Fortsetzung war: "RE2 haelt TIEF nach dem Loslassen, der Port (RE1.5-FSM) faellt sofort auf EBEN zurueck —
deshalb trifft der Nutzer den Aufsteher in RE2, im Port nicht". WIDERLEGT, bevor Code geschrieben wurde:
* RE1.5 Halte-FSM @0x800331e4-0x800332d8 (PSX.EXE): `lw v1,-14488(v1)` = HELD 0x800ac768 / `andi 0x100` (R1) sonst
  `ori v0,zero,0x3` = Senken @0x800331f8-200; `andi 0x10` (HOCH) -> `andi 0x1fff / ori 0x8000 / sh 0x800acaec`
  @0x80033228-38; `andi 0x20` (TIEF) -> `ori 0x2000` @0x80033270-84; **`andi v0,v0,0x30` / `bne v0,zero,0x800332dc`
  @0x8003329c-a0 (weder HOCH noch TIEF gehalten) -> `andi 0x1fff / ori 0x4000 / sh` @0x800332bc-c8 = MITTE**. Port
  player_common.c:1160-1166 (`elev = pad&UP ? 1 : pad&DOWN ? -1 : 0` je Bild) ist dazu byte-true.
* RE2 Halte-Unterzustand B (Funktion @0x8004362c, s1 = a3 = HELD-Pad `addu s1,a3,zero` @0x80043684, R1-Test `andi
  v0,s1,0x100` @0x800436f0 sonst Sub 3 @0x800436fc-704): `andi v0,s1,0x10` (HOCH) -> `ori 0x8000` + `sh v0,340(s0)`
  @0x80043738-4c; `andi v0,s1,0x20` (TIEF) -> `ori 0x2000` @0x8004376c-80; **`andi v0,s1,0x30` @0x80043754 /
  `bne v0,zero,0x800437d0` @0x80043784 / sonst `lhu 340 / andi 0x1fff / ori 0x4000 / sh v0,340(s0)` @0x8004378c-b4 =
  MITTE**. Der erste Unterzustand A (@0x80043540-0x80043604) kennt nur die Uebergaenge nach TIEF/HOCH (+0x6 = 1,
  @0x80043590-a8 / @0x800435e8-600) hinter der Bildschwelle DAT_800a6d78/79[(Waffe-1)*3] (@0x80043574-84 / @0x800435cc-dc).
=> Die Zielhoehe faellt in beiden Originalen beim Loslassen auf EBEN zurueck; der Port weicht hier nicht ab. Punkt 1 ist
   damit NICHT ueber die Spieler-FSM erklaerbar — Messung am Port (Schritt 2) entscheidet, was der Nutzer erlebt.

### B7 — Aufsteher-Treffer FUN_80107A78: der Exit stellt die Teile-Maske her (Port: nicht noetig)

enemy_ai_re2_zombie.c:7338 vermerkt `word0 |= 0x0C000000 @0x80107EA8-B0` als "nicht portiert". Der Exit stellt zugleich
+0x22C mit +0x6+1 wieder her (@0x80107E70/EB8) -> EXEC[5] P8 (`|= 3` @0x80103730-38) bzw. EXEC[8] P5; die Maske wird im Port
im Folgetakt durch P8 gesetzt. Kein Fenster, in dem der Port abweicht (der Zombie steht beim Exit).

## Messung vorher

### Messung vorher — Stand c71fa144 + ungeprueftes ff1649c1, gemessen 12:3x (test_r39_treffer, echter Spielschritt)

Harness: `test_r39_treffer <modus>` (tests/unit/test_r39_treffer.c, RDT + Sce_em_set-Spawns + RE2-Baenke + re15_game_step,
Pistole = RE1.5-Waffe 3, Pad R1/SQUARE; Spielerposition gepinnt, Blick von der Zielhilfe gedreht). Rohdaten im Scratchpad
(m_*.txt), Kernzeilen hier.

**Punkt 1 (RE2-KI, ROOM1140, Fresser-Zombie Slot 2 Typ 0x10 per 0x501 in EXEC[5], Abstand 1498, gap 6):**
```
EBEN: PRE f161 st=1/5/7 mo=0x08 bild=11 maske=1 ; Bild 11,17,23,29,35,41,47,53 -> 8x miss (maske=1, Zeile 3 = [2,0,0])
      f209 bild=59 maske=3 -> HIT (hnd=7 = FUN_80107A78), danach steht er (0x101) und wird normal getroffen
      SUMME: 15 Schuesse, 4 Treffer; in EXEC[5] Bild<55: 0/8 Treffer
TIEF: f161 bild=11 maske=1 -> HIT (Zeile 6 = [1,2,0], Beine) ; SUMME 7 Schuesse, 4 Treffer, Bild<55: 1/1
Fresser-Aufsteher EXEC[8] P4 (Clip 0x15, maske=3): EBEN f15 bild=11 -> HIT (hnd=7); 4/7 (jeder 2. Schuss faellt in die
      Treffer-Reaktion = +0x93-Latch, Original @0x800124f0 / Freigabe @0x80105fa4)
```
=> Der Port verhaelt sich exakt wie das RE2-Modell B5: der STURZ-Aufsteher (EXEC[5], Maske nur Beine bis Clip-Bild 55) ist
mit EBEN nicht treffbar, mit TIEF durchgehend; Fresser-/Liege-Aufsteher (Maske 3) sind mit EBEN treffbar. Die Zielhoehe
faellt beim Loslassen des D-Pads in RE1.5 UND RE2 auf EBEN zurueck (B6). Ob RE2 Retail den Sturz-Aufsteher mit EBEN wirklich
nicht trifft, wird LIVE gemessen (DuckStation-GDB, D_belege/re2_mess/, s. u.).

**Punkt 2 (RE2-KI, ROOM1140, Spieler (1400,-10300) suedlich des Konferenztischs [6], Zombie gepinnt (1400,-19000) noerdlich,
5 Schuesse EBEN, gap 45, Zombie vor jedem Schuss auf 0x101):**
```
A' alter Stand (Moebel-Liste AUS = FUN_8003dcc4 ohne Ausnahme): 0/5 Treffer, Sichtlinie verworfen 5x, sperrende Zelle [6]
A  neuer Stand (Moebel-Liste AN):                               5/5 Treffer, 0 Verwuerfe, Zelle [6] 64x uebersprungen
B  freier Gang (-6700,-12000) -> (-6700,-20000):                 5/5, 0 Verwuerfe
C  Huellenblock [3] (-2500,-10300) -> (-7200,-3000):             0/5, 5 Verwuerfe, Zelle [3] (Wand bleibt Wand)
```
**Punkt 3 (re15_player_aim_target auf echter RDT-Geometrie, Aktoren gesetzt):** ROOM1100 L-Flur, Spieler (-14000,-13000),
A (-18500,-24500) hinter Wand [1]/[2] Abstand 12349, B (-14000,-26000) sichtbar Abstand 13000:
```
alter Stand (Zielhilfe ohne Sichtlinie, FUN_8003703c): Ziel = A (der naechste, hinter der Wand)
neuer Stand: Ziel = B; nur A aktiv -> kein Ziel; ROOM1140: ueber den Tisch Ziel = ja (Liste aus: nein), hinter [3]: kein Ziel
```

### RE2-Retail LIVE (DuckStation-GDB + vgamepad, Spielstand SLUS-00748_4.sav = Stage 0 Raum 20, ein Zombie id 0x10)

Werkzeuge: `D_belege/re2_mess/re2_gdb_aufsteher.py` (Haltepunkte Applier FUN_800410CC @0x800410CC + Rueckkehr @0x80043B04 /
@0x80048A0C, Herzschlag 0,3 s; je Probe Zombie w0/w4/+0x14C/+0x156/+0x223/+0x21A/+0x1D3/+0x10E, Leon +0x154/+0x4/+0x14C),
`re2_gdb_tastensonde.py`, `re2_stops_auswerten.py`; Rohdaten `daten/<lauf>/stops.txt`, `log.txt`.
Befunde der Vorlaeufe (eben_1..5, tasten_1):
* In diesem Spielstand/dieser Belegung feuert KREUZ (vgamepad A), nicht Viereck (`daten/tasten_1/log.txt`: bei A sofort
  `feuer_T3` 0x80047c6c + Applier; Leons Waffe +0x10E = 0x000f = SMG id 15, Tabelle T3 @0x800a7068 [A 0x8004326c, B'
  0x800479d8, Feuer 0x80047c6c]; die Handfeuerwaffen-Tabelle T_gun @0x800a7048 = [A, B 0x8004362c, Feuer 0x80043908]).
* Zielwahl der Zielhilfe im Halte-Handler A: `jal 0x80042144` mit **a1 = 3000** (`addiu a1,zero,3000` @0x800434e8) und nur
  EINMAL je Zielen (+0x16B-Latch `lb v0,363(s0)` @0x800434d0 / `sb 1,363` @0x800434f0); 30000 nur in den Raeumen
  0x800d481c == 0xA0003/0x40006 (@0x80043300-2c). eben_4: Zielen bei 7000 Abstand -> keine Aufschaltung, Leon feuerte starr.
* Die geladene Overlay-Kopie @0x80100000 ist NICHT EMOVL10_S0.BIN (= EMZ0.BIN): Bytes @0x80103568/@0x80103628/@0x80102BE8
  live != Datei (tasten_1/log.txt). Die S0-Adressen der Phasen-Handler (B5) gelten fuer dieses Overlay nicht; die Messung
  stuetzt sich deshalb nur auf EXE-Haltepunkte + RAM.
* **Teile-Maske live (w0 bits 26-28): stehend 3; EXEC[5] P1 anfangs 1 (w0=0x04001001), ab P1 Bild ~30 und in P2/P3
  (liegend, Clip 0x16)/P6/P7 (Clip 9) bis Bild 54: 0 (w0=0x00000001); ab P7 Bild 55: 3 (w0=0x0c00....)** — d.h. in RE2
  Retail (dieses Overlay) hat der gestuerzte Zombie waehrend Liegen und Aufstehen bis Clip-Bild 55 GAR KEINE Trefferteile
  (weder Beine noch Rumpf), das S0-Modell "nur Beine" (B5) ist dort nicht das, was der Applier liest.
* eben_4/5: 1130 bzw. 1064 SMG-Schuesse, HP 1500 unveraendert in ALLEN Zustaenden (auch stehend bei 1100-1400 Abstand,
  Maske 3, Zielen bei 2383 im Radius) -> die SMG-Kette traf nie; Messkette ohne bestaetigten Treffer ungueltig. Naechster
  Lauf: Schrotflinte (Slot 3, id 7) per Status-Bildschirm ausruesten (R2_EQUIP=shotgun), Munition per RAM 250.

### Echte exe (re15_pc_r39d_mess.exe = Bau 12:5x dieses Baums, Muster test_r30_cut_blitz: RE15_DEBUG_JUMP=<raum>@240,
RE15_TITLE_SHOT_AF=2, RE15_PLAYER_POS, RE15_EQUIP=3, RE15_INPUT_SCRIPT ab Bild 330, RE15_STATE_LOG + RE15_TREFFER_LOG;
Werkzeuge D_belege/exe_mess/exe_lauf.sh + exe_auswerten.py, Logs *_treffer.log / *_auswertung.txt dort)

**Punkt 2 — ROOM1250 (Kantine), Spieler (-12000,-1000) oestlich der Tischzelle [7] (x -16150..-14150, z -5700..2650),
sieben stehende Zombies (grid 0) bei x -18200 westlich davon, 6 Schuesse Pistole EBEN im Abstand 1 s:**
```
alter Stand (RE15_TREFFER_ALT=1): 0 Treffer, treffer.log 6x "sperre ... zelle=79" (= Partition 3, Zelle [7] = Tisch)
neuer Stand:                      6 Treffer (Slot 5 hp 250->234->219->204->189->174->159), je Kandidat "frei ... moebel_uebersprungen=4"
```
**Punkt 3 — ROOM1180 (Flur), Spieler (3800,6000) im Ostgang, nur R1 (Zielen):**
```
alter Stand: "ziel slot=1 typ=10 @(-582,12718)" = der naechste Zombie HINTER der Wandzelle [6] (x 976..2190, z -4188..12299)
neuer Stand: "ziel slot=2 typ=10 @(5105,15408)" = der sichtbare im Ostgang; Slot 1 und 3 je Bild "zielhilfe_verdeckt
             zelle=51/47" (= [6] bzw. [2], Huellenwaende)
```

## Umsetzung

### Punkt 2 + 3 (Sichtlinie des Schusses, Zielhilfe) — Dateien, Konstanten, Form

* `engine/src/treffer_r39.c` + `include/re15_treffer_r39.h` (neu, Vorgaenger ff1649c1, hier geprueft und ergaenzt):
  - `re15_treffer_r39_los_blockiert(von, nach)` = FUN_8001b9b4-Zwilling (Regionen 3..0 `ori s0,zero,0x3` @0x8001ba08 /
    `addiu s0,s0,-1` @0x8001ba38, je Region FUN_8003dcc4(.., 0xf00, 0x300) @0x8001ba1c-24: Band == Ursprung+0x82, Art-Nibble 3,
    Kreuzungstest beider Diagonalen /0x12 = `re15_r36_los_zelle_kreuzt` aus enemy_ai_common.c), ABER Moebel-Zellen werden
    uebersprungen — die FORM von RE2 FUN_80050858 (`lhu v1,8(t1)` @0x800508bc / `and v0,v1,fp` @0x800508c4 / `beq
    v0,zero,0x800508b0` @0x800508c8: Zelle ohne Flagtreffer = naechste Zelle), die der RE2-Schuss-Applier mit Maske 0x80 ruft
    (`addiu a2,zero,128` @0x800412e0 / `addiu a3,zero,1` @0x800412ec / `jal` @0x800412f0).
  - `re15_treffer_r39_zelle_moebel(room, zelle)`: Zuordnung ueber Raum + (x,z,w,d) gegen `gen/treffer_r39_moebel.inc` (218
    Eintraege, 11 Leon-Raeume + Elza-Varianten, Generator tools/r39_treffer_moebel_gen.py, Inhalt aus den Hintergrundbildern
    je Cut: D_belege/zellen/ROOM*_inseln_*.png — Tische, Pulte, Betten, Stuehle, Theken, Baenke, Kisten/Kartons, Rohre am
    Boden, Eimer, Topfpflanzen; Regale/Maschinen/Saeulen/Waende bleiben sperrend). Jeder Eintrag ist eine Art-3-INSEL-Zelle
    (nicht an der Huelle), Riegel unit_r39_treffer_moebel; die Runde-36-Moebel mit belegter Oberkante (k_r36_moebel,
    zombieki_r36.c: 1010 Tisch #5 + Stuhlkreise, 1220 Betten) sind Teilmenge (Riegel).
    PORT-WAHL (als solche gekennzeichnet): RE1.5s Designer haben das Attribut in ROOM1020 fuer Moebel benutzt (Art 2) und
    sonst pauschal Art 3 gesetzt (unfertiger Beta-Datenstand); RE2 setzt bei ~16 % der Zellen das Flag nicht. Die Liste ist
    die RE2-Praxis auf den RE1.5-Daten — welche Zelle ein Moebel IST, steht in keiner Zahl, nur im Bild.
  - `re15_treffer_r39_zielhilfe_verdeckt(pl, e)`: die RE2-Kandidatenbedingung der Zielhilfe (FUN_80042144 `jal 0x80050858`
    @0x800422a8, `bne v0,zero,0x800422f4` @0x800422b0 = verdeckter Kandidat wird uebersprungen) mit demselben Strahl.
  - Sonden: `re15_treffer_r39_moebel_aus` / `_zielhilfe_aus` (alter Stand im selben Lauf), Mess-Sonde RE15_TREFFER_ALT=1
    (nur Umgebungsvariable: beides aus = alter Stand, fuer die exe-Vorher/Nachher-Messung), Messschiene RE15_TREFFER_LOG=<datei>
    (Zeilen `sperre`/`frei`/`zielhilfe_verdeckt`/`ziel slot=`), Zaehler fuer die Riegel.
* Haken (gemeinsame Dateien, je 1-5 Zeilen): `schuss_r36.c re15_schuss_r36_sichtlinie_blockiert` ruft den Zwilling mit
  Moebel-Ausnahme statt der Regionen-Schleife; `re15_damage.c re15_player_aim_target`: `if (re15_treffer_r39_zielhilfe_verdeckt(pl, e))
  continue;` vor der Abstandswahl (RE1.5 FUN_8003703c hatte keine Sichtlinie, B3) + Logzeile am Ende.
* Fortsetzung (diese Sitzung): `extern`-Deklaration durch `re15_zombieki_r36.h` ersetzt; Zielwahl-Log; RE15_TREFFER_ALT.

### Punkt 1 (halb aufstehende Zombies) — KEINE Code-Aenderung, Beleg + Riegel

Gemessen (Spielschritt, RE2-KI, ROOM1140): nur der STURZ-Aufsteher (EXEC[5], Zombie nach Flinch-Sturz 0x501) ist mit EBEN vor
Clip-Bild 55 nicht treffbar; mit TIEF (D-Pad unten gehalten) ab dem ersten Bild; Fresser-Aufsteher (EXEC[8]) und Liege-Spawns
(Maske 3) sind mit EBEN treffbar. RE2 Retail dazu:
* EXE-Applier FUN_800410CC: `lw v0,0(s2)` @0x800413c4 / `srl v0,v0,26` @0x800413cc / `andi s6,v0,0x7` @0x800413d4 /
  `beq s6,zero,0x80041774` @0x800413d8 — ohne Trefferteile ist der Zombie kein Kandidat; EBEN waehlt Zeile 3 = [2,0,0]
  (Rumpf), TIEF Zeile 6 = [1,2,0] (Beine zuerst) (Maskentabelle DAT_800a6db4, @0x8004141c-4e4).
* Overlay EMZ0/EMOVL10_S0 (statisch, B5): Sturz P0 setzt "nur Beine" (@0x801032b8-fc), Rumpf erst bei Clip-Bild 55
  (@0x801036d0-f0) bzw. P8 (@0x80103730-38) -> EBEN trifft erst ab Bild 55, TIEF die Beine.
* RE2 LIVE (DuckStation-GDB, SLUS-00748_4, anderes Overlay als S0): Maske in EXEC[5] P2..P7 bis Bild 54 = 0, ab Bild 55 = 3
  -> dort trifft bis Bild 55 GAR NICHTS (auch TIEF nicht). Der Port ist also nicht restriktiver als RE2; die Nutzer-
  Aussage "in RE2 ist das auf jeden Fall nicht so" bestaetigt sich fuer den Sturz-Aufsteher nicht.
* Zielhoehe faellt beim Loslassen in RE1.5 und RE2 auf EBEN zurueck (B6) — keine Port-Abweichung.
NUTZER-VORGABE greift nur, soweit RE2 nicht belegt ist (AUFTRAG) — RE2 ist belegt; Riegel `unit_r39_treffer_aufsteher`
pinnt das Verhalten (a/b/c). Offen bleibt die Variante "Maske 0 statt 1" des Live-Overlays (OFFEN).

## Messung nachher

### Spielschritt (test_r39_treffer, dieselben Szenen wie "Messung vorher")

* Punkt 2 `tisch`: A ueber den Tisch 5/5 (vorher 0/5), B frei 5/5, C Wand [3] 0/5 mit 5 Verwuerfen (unveraendert) — PASS.
* Punkt 3 `zielhilfe`: ROOM1100 Ziel = sichtbarer B (vorher A hinter der Wand), nur A -> kein Ziel, Gegenprobe alter Stand = A;
  ROOM1140 ueber den Tisch Ziel ja (Liste aus: nein), hinter [3] kein Ziel — PASS.
* Punkt 1 `aufsteher`: (a) Sturz-Aufsteher EBEN 0 Treffer vor Bild 55, erster Treffer ab Bild 55 (gemessen 59); (b) TIEF trifft
  vor Bild 55 (Bild 11); (c) Fresser-Aufsteher EBEN trifft (Bild 11) — PASS (= unveraenderter Port-Stand, s. Umsetzung Punkt 1).

### Echte exe: s. "Echte exe" oben (1250: 0/6 -> 6/6; 1180: Ziel hinter der Wand -> sichtbarer). ROOM1140-Dauerfeuer am
Nutzerweg (lauf_aufsteher1140/_b): der Fresser (Slot 2, 50 HP) wird nach dem Aufstehen mit EBEN getroffen (4 Treffer bis zum
Tod); im zweiten Lauf (Spieler (-3500,-20600) mitten unter den Fressern, D_belege/exe_mess/aufsteher1140b_phasen.txt) stuerzen
Slot 3 (80 HP) und Slot 4 (250 HP) je ZWEIMAL (1/5/1 -> 1/5/3 liegend -> 1/5/6 -> 1/5/7 Clip 9 -> 1/5/8 -> 1/1/1) unter
EBEN-Dauerfeuer: waehrend 1/5/3 .. 1/5/7 KEIN Treffer; erster Treffer Slot 3 bei F369 = Clip-Bild 59 (st=2/3/1 = Aufsteher-
Treffer FUN_80107A78, hp 80->64) - identisch mit dem Spielschritt (Bild 59) und dem RE2-Tor (Bild 55).

## Tests

Neue Tests (tests/unit/probes/r39_treffer.cmake, Quelle tests/unit/test_r39_treffer.c, je Lauf eigener Prozess, keine
Umgebungsvariablen des Aufrufers, absolute Asset-Pfade, kein Fenster):
* `unit_r39_treffer_tisch` (Punkt 2, Spielschritt ROOM1140: A 5/5, A' alter Stand 0/5 + 5 Verwuerfe Zelle [6], B 5/5, C Wand 0/5)
* `unit_r39_treffer_zielhilfe` (Punkt 3: ROOM1100 L-Flur sichtbarer vor verdecktem Naeherem, nur verdeckter -> kein Ziel,
  alter Stand = verdeckter; ROOM1140 ueber den Tisch ja / Liste aus nein / hinter Huelle nein)
* `unit_r39_treffer_moebel` (Liste gegen RDT-Bytes: Art 3, Geometrie, keine Huelle, Elza-Paar, Obermenge k_r36_moebel)
* `unit_r39_treffer_aufsteher` (Punkt 1: EBEN 0 Treffer vor Bild 55 + erster ab 55, TIEF vorher, Fresser EBEN)
Messmodi ohne add_test: `spawns <raum>`, `mess_aufsteher eben|tief [gap]`, `mess_fresser eben|tief [gap]`.

Gezielter Lauf (keine volle Suite):

## OFFEN

* RE2-Live: die Treffer-Kette mit der SMG (einzige geladene Schusswaffe des Spielstands _4) traf in keinem Zustand (eben_4/5/7:
  Leon aufgeschaltet (+0x1B4 = Zombie), Blick 3349 bei Soll ~3372, Abstand 1100-4400, Maske 3 stehend) - Ursache offen
  (SMG-Fenster/Boxen der Tabelle T3?). Der Beleg "Maske 0 in EXEC[5] P2-P7<55" ist direkte RAM-Beobachtung + EXE-Tor
  @0x800413d8, aber ohne bestaetigten Vergleichstreffer. Naechster Weg: Handfeuerwaffe ins Inventar/ausruesten (Status-
  Bildschirm-Sequenz blind gescheitert: Menue blieb offen, mode=2) oder einen Spielstand mit Handfeuerwaffe + Zombie.
* Das live geladene Zombie-Overlay (Stage 0 Raum 20) ist nicht EMOVL10_S0.BIN/EMZ0.BIN (Bytes @0x80103568/628/0x80102BE8
  verschieden; Dateisuche ueber die ersten 0x200 Bytes fand keinen Treffer) - welche Datei es ist und ob dort P2 die Maske
  loescht (S0: P0 "nur Beine", kein Loescher gefunden; live: Maske 0 ab P1 Bild ~30), ist nicht geklaert. Fuer den Port
  (S0-Modell "nur Beine" -> TIEF trifft) KEINE Aenderung: sie wuerde den Nutzerpunkt verschaerfen.
* Moebel-Liste: Zuordnung aus Hintergrundbildern (PORT-WAHL, Form RE2-Flag); Raeume ohne Zombies/ohne Inselzellen sind nicht
  gelistet; ROOM1140 [13] "Leiste am Tuergang" ist die Tuerschwelle (hinter ihr steht nie ein Gegner, harmlos).
* vgamepad-Laeufe (RE2-Messung) erscheinen parallelen Port-exes als "Xbox 360 Controller" - Skript-Laeufe ersetzen die
  Pad-Bits, interaktive Titel-/Menue-Haken koennten Eingaben sehen; waehrend der Integration keine RE2-GDB-Laeufe fahren.

## Fuer den Nutzer

* Keine neuen Assets, keine Sprachdateien, keine Aenderung an shared_assets/PSX. Neue Datendatei im Quellbaum:
  engine/src/gen/treffer_r39_moebel.inc (Moebel-Liste, Generator tools/r39_treffer_moebel_gen.py).
* Ueber den Tisch schiessen: geht jetzt in allen gelisteten Raeumen (1010, 1020, 1070, 10D0, 10E0, 1110, 1140, 11F0, 1220,
  1250, 30D0 + Elza-Varianten) fuer Tische, Pulte, Betten, Stuehle, Theken, Baenke, Kisten, Rohre, Eimer; Regale, Saeulen,
  Maschinen und Waende sperren weiter (wie RE2: Zellen ohne Flag 0x80 sind durchlaessig).
* Zielhilfe: nur noch Gegner mit freier Sichtlinie (RE2 FUN_80042144 @0x800422a8-b0); ein Gegner hinter einer Wand/um die
  Ecke wird nicht angevisiert, auch wenn er naeher ist; ohne sichtbaren Gegner dreht Leon sich nicht.
* Halb aufstehende Zombies (Punkt 1): Gemessen trifft EBEN einen vom Sturz aufstehenden Zombie erst ab Clip-Bild 55 des
  Aufsteh-Clips (letztes Drittel), TIEF (D-Pad unten halten) die Beine ab dem ersten Bild; Zombies, die vom Fressen oder aus
  der Liege aufstehen, sind mit EBEN jederzeit treffbar. RE2 Retail live gemessen (DuckStation, Spielstand _4): der gestuerzte
  Zombie traegt bis Bild 55 GAR KEINE Trefferteile (w0 bits 26-28 = 0) - dort trifft weder EBEN noch TIEF; ab Bild 55 Beine+
  Rumpf. Der Port ist damit nicht strenger als RE2 (TIEF trifft sogar frueher). Deshalb hier keine Code-Aenderung; die
  Zielhoehe faellt in RE1.5 und RE2 beim Loslassen des D-Pads auf EBEN zurueck (@0x800332bc-c8 / @0x8004378c-b4).
  Bedienhinweis: fuer einen gestuerzten Zombie D-Pad UNTEN gedrueckt halten, solange R1 gehalten wird.
