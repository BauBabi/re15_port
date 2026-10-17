# Runde 38 Spur U "wasserzombie" — Abnahme 0 (unabhaengig)

Datum 2026-10-17. Baum `.claude/worktrees/r38_wasserzombie`, Zweig `r38/wasserzombie`, geprueft HEAD `a4809c35`
(merge-base mit master `cb782bda`). Fortlaufend geschrieben (Sitzungslimit).

## 0. Stand der Pruefung

- [x] AUFTRAG/VERTRAG/Dossier gelesen, Code-Diff gelesen (`git diff master...HEAD`, 11 Dateien; `git diff master`
      zeigt zusaetzlich Orchestrator-Dateien nur, weil master seit der Basis weitergelaufen ist — nicht Teil der Spur)
- [x] Bau: `local_build.sh configure` OK, `local_build.sh build` -> `ninja: no work to do`; exe 2026-10-16 23:39
      neuer als alle Quellen (wasserzombie_r38.c 23:33, main.c 22:59), Arbeitsbaum sauber, exe enthaelt die Strings
      `re2-eintritt` und `RE15_WASSERZOMBIE_LOG` -> die exe ist der Stand a4809c35.
- [x] Riegel der Spur: `unit_r38_wasserzombie` gruen (0.06 s), `integration_r38_wasserzombie` gruen (88.9 s)
- [x] RE-Gate (Abschnitt 3): 6 Stellen selbst disassembliert, alle stimmen
- [ ] Gezielte Gruppen der beruehrten Module
- [ ] Eigene Messung an der exe (Abschnitt 2)
- [ ] Nebenwirkungen (andere u1=1-Raeume, beide Szenarien)
- [ ] Vertrag
- [ ] Urteil

## 1. Nutzer-Punkt (Wortlaut)

> Die Zombies in Stage2 die sich unten im Wasser befinden und zum Rand des Wassers laufen müssen sich an den Rand
> hochziehen können, wie im Original Resident Evil 1.5

## 2. Messung an der exe

Messkopie `re15_port/build/platform/pc/re15_pc_u_abn0.exe` (eigener Name, kein taskkill). Arbeitsordner im Scratchpad
der Abnahme-Sitzung (nicht eingecheckt). Gemeinsame Umgebung: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
SDL_ASSERT=always_ignore RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2`, Logs `RE15_WASSERZOMBIE_LOG=wz.log
RE15_GEGNER_Y_LOG=gy.log`, Steuerung geschlossen geregelt ueber `RE15_WEGPLAN` (engine/src/dachkiste_r36.c).

### M1 — ECHTER WEG: Tuer ROOM2070 -> ROOM2000, Leon, Standard-KI (RE2), mit Bild

Befehl: `RE15_DEBUG_JUMP=2070@250 RE15_PLAYER_POS=-18500,-6400,2048,0`, Wegplan `RAUM 2070 / WARTE 20 / BLICK 2048 /
WARTE 10 / AKTION / RAUM 2000 / WARTE 10 / GEHE -20600 -10600 200 / BLICK 2048 / WARTE 1500`, `RE15_EXIT_AT=1100#2000`
(Bildlauf: `RE15_FRAMEDUMP=130-330/10:fd/f`, `RE15_EXIT_AT=420#2000`; Lauf deterministisch, beide Male dieselben Bilder).

debug.log: `[aot] DOOR FIRE slot=0 rect=(-19450,-6400,hw=500,hh=1500) target_cut=13 spawn=(-20650,-1800,-6200)`,
`[room] PC loaded room2000.rdt` -> beide 0x84-Wasserzombies (Slot 3/4, Typ 0x16, g=84) gespawnt (sub00 laeuft auf dem
Tuerweg). Ohne Annaeherung (Spieler an der Ankunftsstelle, Abstand > 4000) bleiben sie liegen (Vorlauf ohne GEHE:
1300 Bilder 1/8) — passt zum Wecker `+0x1d0 < 0xfa0` (Dossier/0x80103814).

wz.log (woertlich):
```
F144 R2000 slot=4 t=16 re2-eintritt st=1/10/0 mo=27 af=18 @(-22068,0,-10290) b0 f1ba=0 rot=87 kontakt=09
F144 R2000 slot=4 t=16 drehen st=1/10/1 mo=1 af=0 @(-22068,0,-10290) b0 f1ba=0 rot=87 kontakt=09
F150 R2000 slot=4 t=16 klettern st=1/10/3 mo=22 af=0 @(-22068,0,-10290) b0 f1ba=0 rot=0 kontakt=00
F220 R2000 slot=4 t=16 boden-1800 st=1/10/3 mo=22 af=70 @(-22068,0,-10290) b0 f1ba=-1800 rot=0 kontakt=00
F308 R2000 slot=4 t=16 oben st=1/0/0 mo=22 af=158 @(-20928,-1800,-10290) b1 f1ba=-1800 rot=0 kontakt=00
F308 R2000 slot=4 t=16 re2-uebergabe st=1/0/1 mo=0 af=3 @(-20928,-1800,-10290) b1 f1ba=-1800 rot=0 kontakt=00
F340 R2000 slot=3 t=16 re2-eintritt st=1/10/0 mo=0 af=63 @(-22068,0,-9970) b0 f1ba=0 rot=165 kontakt=09
F351 R2000 slot=3 t=16 klettern ...   F401 R2000 slot=3 t=16 abbruch st=1/9/0 mo=22 af=9 ...
F724 slot=4 re2-eintritt @(-22068,0,-9608) b0 ... F902 slot=4 oben @(-20926,-1800,-9605) b1 f1ba=-1800
F791 slot=3 re2-eintritt @(-22068,0,-10407) b0 ... F956 slot=3 oben @(-20928,-1800,-10407) b1 f1ba=-1800
```
Kontakt 0x09 = Richtung 0, +8, u1 = 1 (Kanalwand x = -21650 von Band 0 aus); Vorschub genau +1140 in x
(-22068 -> -20928, @0x80103474); +0x1ba -1800 genau bei Clip-Bild 70 (0x46).

Bild (`fd/f000140..f000320`, 320x240, Ausschnitt 3x vergroessert, Scratchpad `m_tuer2000v/crop_a.png`): F140 Zombie im
Kanal an der Wand (RE2-Gang), F150-F180 Haende auf dem Steg, Oberkoerper ueber der Kante, Beine haengen im Kanal,
F200-F220 Knie kommt hoch, F240-F260 kniet auf der Stegkante, F280-F300 richtet sich neben Leon auf, F310 steht auf
dem Steg. RE1.5-Pose auf dem Hybrid-Netz ohne zerrissene Geometrie. **Das ist ein Hochziehen an den Rand.**

Was danach geschieht (gy.log, vorbestehende RE2-KI, NICHT Teil von Spur U, aber fuer den Nutzer sichtbar):
F312-F399 greift der gelandete Zombie Leon (RE2 1/3/x); im Griff-Ende P8 (F399/F400) schreibt er dem Partner 0x901
(Partner-Domino, Dossier 6) -> der zweite Zombie bricht sein Hochziehen bei Clip-Bild ~49 ab (`abbruch`, +0x1ba bleibt 0,
weil vor Bild 0x46) und rutscht im RE2-Taumel-Executor 9 (Clip 13) in 12 Bildern ~2700 Einheiten rueckwaerts durchs
Wasser (x -22068 -> -24792); der Greifer selbst wird im Griff-Ende ~2000 Einheiten zurueckgestossen, faellt bei F405
ueber die Absturzkante (u1=2) und landet bei F416 im Wasser in 1/5 (Sturz). Beide setzen danach erneut an und kommen
hoch (F902/F956). Beides ist die RE2-Griffmechanik (Domino `sw 0x901` @0x80102E90-98, Rueckstoss EXEC[9]) — sie
traf vorher ebenso einen an der Wand stehenden Zombie; der Ablauf ist im Dossier unter OFFEN gefuehrt.

## 3. RE-Gate — selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, Hauptbaum, nur lesend)

Binaerdateien: `info/Re1.5/PSX/BIN/STAGE2.BIN` (roh @0x80100000), `info/Re1.5/PSX/BIN/STAGE1.BIN`, `info/Re1.5/PSX.EXE`.

1. **Ausloeser @0x80101f00-44 (STAGE2.BIN, ENGAGE-Decide 0x80101eec)** — Befehl `dis 0x80101eec 40 --bin STAGE2.BIN`:
   `80101f00 lbu a0,144(a1)` / `80101f08 andi v0,a0,0x3` / `80101f0c beq v0,zero` / `80101f10 andi v0,a0,0xf0` /
   `80101f14 lh v1,106(a1)` / `80101f18 sll v0,v0,4` / `80101f1c subu` / `80101f20 addiu v0,v0,512` /
   `80101f24 andi 0xfff` / `80101f28 slti v0,v0,1024` / `80101f30 andi v0,a0,0x1` / `80101f34 addiu v0,v0,9` /
   `80101f38 sll v0,v0,8` / `80101f3c ori v0,v0,0x1` / `80101f44 sw v0,4(a1)`. Stimmt mit
   `re15_wasserzombie_kante_vorn` ueberein (u1&3, Kegel +0x200/<0x400, u1 Bit 0 -> 0xA01).
   Erreichbarkeit: Modus-0-Dispatcher 0x80101520 liest DECIDE @0x80117944 / ANIMATE @0x80117994, Modus 2 (0x80101618)
   DECIDE @0x80117a64 / ANIMATE @0x80117994; `table 0x80117944`/`0x80117a64` -> [2] = 0x80101eec (beide);
   Modus-Tabelle @0x80117910 aus `lbu v0,9 / andi 0xf` @0x801014e0-0x80101504; Modus 4 (0x80101710) ->
   @0x80117acc/@0x80117ad0, Ausgang `sb zero,9` @0x801039d0 + `sw v0,4` @0x801039e0 (zurueck in Modus 0).
2. **Hochziehen ANIMATE[0x0A] 0x8010325c** — `table 0x80117994 24 --bin STAGE2.BIN`: [10] = 0x8010325c.
   `dis 0x8010325c 136`: Phase 0 `sb v1(=1),6` / `sb v1,148` @0x801032c0 (v1 = 1 aus dem Delay-Slot
   @0x80103288) / `sb zero,149` / `sb 7,143` @0x801032e0; Phase 1 `jal 0x8001f314` (a2 = 0, a3 = 0x200 @0x801032fc),
   `jal 0x8001aa68` mit a0 = (+0x90&0xf0)<<4, a1 = 0x10 @0x80103310, `addu v1,v0,v1` / `sh v1,106` im Delay-Slot
   @0x80103340 (immer), d == 0 -> +0x6 = 2; Phase 2 `ori v0,zero,0x16` @0x80103368, `ori v0,zero,0x474` @0x80103398 /
   `sh v0,140` @0x8010339c, +0x6a = (+0x90&0xf0)<<4 @0x801033bc; Phase 3 `ori v0,zero,0x46` @0x801033d0,
   `sh 0x474,176` @0x801033e0, `lhu v0,442 / addiu v0,v0,-1800 / sh v0,442` @0x801033f0-fc, `jal 0x8001f314`
   @0x80103414 (a3 = 0x200 @0x80103418), `beq v0,zero` @0x8010341c mit Delay-Slot `addu a0,zero,zero`
   @0x80103420 (a0 = 0 in beiden Zweigen), `lbu 130 / addiu 1 / sb 130` @0x80103430-3c,
   `lw 56 / addiu -1800 / sw 56` @0x8010344c-58, `ori v0,zero,0x1 / sw v0,4` @0x80103464-68,
   `jal 0x800245d8` @0x80103474 mit Delay-Slot `sh zero,176` @0x80103478. Stimmt Zeile fuer Zeile mit
   `re15_wasserzombie_hochziehen` und allen Kopf-Konstanten (RE15_WZ_*).
   STAGE1-Zwilling: `table 0x8011f890 --bin STAGE1.BIN` [10] = 0x801033c8, dieselben Konstanten
   (0x16 @0x801034d4, 0x474 @0x80103504, 0x46 @0x8010353c, -1800 auf +0x1ba @0x80103564, +0x82+1 @0x801035a8,
   -1800 auf +0x38 @0x801035c0, `sw 4` @0x801035d4, `jal 0x800245d8` @0x801035e0) -> der Ersatz des alten
   Port-"EDGE-FALL" gilt auch fuer Stage 1 zu Recht.
3. **FUN_8001aa68 (PSX.EXE)** — `dis 0x8001aa68 24`: `subu a0,a0,v0 / addu a0,a1,a0 / andi 0xfff` @0x8001aa7c-84,
   `slt v0,a0,2*a1 / bne -> 0` @0x8001aa90-98, `sltiu v0,a0,0x801` @0x8001aaa0 -> +a1, sonst -a1. Schreibt +0x6a nicht.
   = `wz_8001aa68`.
4. **FUN_800245d8 (PSX.EXE)** — `dis 0x800245d8 48` + `dis 0x80024694 30`: (+0x8c,0,0) rotiert mit
   RotMatrixY(+0x6a + a0) (`jal 0x800659d0` @0x80024660), `lw/addu/sw 52` @0x800246d8-e4 (X += IR1),
   `lw/addu/sw 60` @0x800246ec-f8 (Z += IR3). Port: x += cos*s>>12, z -= sin*s>>12 (Unit-Riegel: 0xC00 -> z+1140,
   0x000 -> x+1140; gemessen, s.u.).
5. **Schatten** — `dis 0x801004c0 20 --bin STAGE2.BIN`: `lh a1,442(v0)` @0x801004fc, `jal 0x8001b064` @0x80100500,
   a0 = +0xb0 @0x80100504. Kontaktbyte-Produzent PSX.EXE `sra 4 / addiu 8 / andi 3 / addu / sb v0,144` @0x8003b4c0-dc.
6. **+0xb0 ohne Leser (Gegenprobe, eigene Suche)** — Scan aller Lasten UND Zeiger-Ableitungen `addiu rt,rs,0xb0..0xb3`
   (rs != sp) in PSX.EXE und STAGE2.BIN: Zeiger nur an FUN_8001b064 (Schatten zeichnen, @0x80100504/@0x80031dd0)
   und FUN_8001af5c (Schatten-Init, @0x80100ad4/@0x80042898; 0x8006exxx sind BIOS-Stubs `addiu t2,zero,0xb0`).
   `dis 0x8001b064 110` + Rest: liest den Block nur bei +8/+10/+12/+14 und schreibt Primitive ab +16;
   `dis 0x8001af5c 66`: schreibt +8..+14/+16..+56, liest +0 nicht. Block+0 (= Aktor +0xb0) hat also keinen Leser —
   das Weglassen der zwei +0xb0-Stores ist belegt.
7. **Typ-Roster** — eigener Scan der `sw` auf 0x80072bac+Typ*4 in STAGE2.BIN: @0x80116f04-44 Typ 0x10/0x11/0x12/0x16/
   0x18/0x1c-0x1f -> 0x801002b8; 0x13 -> 0x8010a75c (eigene Wurzel). Zustandstabelle @0x801178b8 [1] = 0x801010b8.
   = `re15_wasserzombie_typ`.
8. **RE2-Gegenprobe** — eigener Wort-Scan (addi/addiu/ori mit 0x0708/0xf8f8) in info/re2leon/COMMON/BIN: EMZ0.BIN,
   EMZ0_d1.BIN, EMOVL10_S0/S1.BIN 0 Treffer; EMD0G_MOD0.BIN @0x4e8c und EMOVL21_S0.BIN @0x14a4 je 1 (Gegenprobe).
   RE2 Retail hat kein Zombie-Hochziehen -> Ziel RE1.5 (deckt sich mit dem Wortlaut "wie im Original Resident Evil 1.5").

Konstanten im Diff (alle mit Adresse): RE15_WZ_SUB 0x0A (@0x80101f30-44), RE15_WZ_CLIP_DREH 1 (@0x80103288/@0x801032c0),
RE15_WZ_CLIP_KLETTER 0x16 (@0x80103368), RE15_WZ_FRAC 7 (@0x801032dc-e0/@0x80103388-8c), RE15_WZ_RATE 0x200
(@0x801032fc/@0x80103418), RE15_WZ_DREHSCHRITT 0x10 (@0x80103310), RE15_WZ_TEMPO 0x474 (@0x80103398),
RE15_WZ_BODENBILD 0x46 (@0x801033d0), RE15_WZ_BAND 1800 (@0x801033f8/@0x80103454). Gekennzeichnet als PORT-WAHL:
Rig-Wechsel ohne Ueberblendung, +0x1ba-Ruecksetzen beim RE2-Abbruch; als PORT-EMULATION: LOOP-Bit in Phase 0/1.
Port-Abbildung ohne Original-Gegenstueck (RE2-Gehirn hat kein Hochziehen): Eintritt aus dem RE2-Gang (Zustand 1/Sub 1,
aufrecht) — im Kopf als "PORT-ABBILDUNG" beschrieben. Suchwoerter `deferred|tunable|interim|for now|faithful|plausibel|
TODO` im Diff: (laufend)
