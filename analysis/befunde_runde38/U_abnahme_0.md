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

### M1-vorher — derselbe Weg mit der v0.8.25-exe (Gegenprobe: erklaert der Fix den Befund?)

`release/pkg-win/re15_port_v0.8.25/re15_pc.exe` als Kopie im Scratchpad (`re15_pc_v0825_u.exe`), Assets des Pakets
(`RE15_CD_ROOT=<paket>/shared_assets/PSX RE15_RE2_ASSET_ROOT=<paket>/shared_assets/RE2`), identischer Wegplan/Tuerweg,
`RE15_EXIT_AT=700#2000`. Tuer feuert identisch (`DOOR FIRE slot=0 ... spawn=(-20650,-1800,-6200)`). gy.log: beide
Wasserzombies erreichen die Kanalwand x = -22068 (F151 bzw. F401) und pendeln dort bis F700 zwischen RE2 1/1 und 1/14
(z.B. F601 `[4 t=16 st=1/14/2/0 ... @(-22068,0,-10986) b0]`); **0 Bilder auf Band 1**. Das ist der Nutzerbefund;
mit a4809c35 ziehen sie sich auf demselben Weg hoch (M1). Der Fix erklaert den Befund: das RE2-Gehirn hat kein
Hochziehen (eigene Gegenprobe RE2-Overlays, Abschnitt 3 Nr. 8), der neue Dispatch fuehrt die RE1.5-Maschine ein.

### M2 — ECHTER WEG 2070 -> 2000, Leon, `RE15_AI_FLAVOR=re15`, mit Bild

Gleicher Wegplan. wz.log:
```
F146 R2000 slot=4 t=16 drehen st=1/10/1 mo=1 af=0 @(-22068,0,-9931) b0 f1ba=0 rot=4002 kontakt=09
F152 R2000 slot=4 t=16 klettern st=1/10/3 mo=22 af=0 @(-22068,0,-9931) b0 f1ba=0 rot=0 kontakt=00
F222 R2000 slot=4 t=16 boden-1800 st=1/10/3 mo=22 af=70 @(-22068,0,-9931) b0 f1ba=-1800 rot=0 kontakt=00
F310 R2000 slot=4 t=16 oben st=1/0/0 mo=22 af=158 @(-20928,-1800,-9931) b1 f1ba=-1800 rot=0 kontakt=00
F426 R2000 slot=3 t=16 drehen ...  F430 klettern ...  F500 boden-1800 ...
F588 R2000 slot=3 t=16 oben st=1/0/0 mo=22 af=158 @(-20928,-1800,-10474) b1 f1ba=-1800 rot=0 kontakt=00
```
gy.log: 0 Bilder mit einem Typ-0x16 unter dem Wasserboden (y > 0), 0 auf Band 2+ (Vorbefund: Sturz bis y = +1800,
Band 2). Danach greifen beide den stillstehenden Leon an, er stirbt (Lauf endet im Neustart — erwartbar, der
Wegplan wehrt sich nicht). Bild (`m_tuer2000r15v/crop_r15.png`, F140-F320): Haende auf die Kante, Oberkoerper drueber,
Knie hoch, Hocke, Aufrichten auf dem Steg (F310/F320 steht er). Kein Unterschied zur RE2-KI im Bild.

### M3 — ECHTER WEG 2071 -> 2001, Elza (`RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`), Standard-KI

Erster Lauf ohne Flag: `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, Tuer feuert, ROOM2001 geladen — aber
KEIN Gegner. Ursache (RE15_SCD_TRACE=1, Rohbytes): ROOM2001 sub00 @0x1826 `06 00 2e 00` (If) / @0x182A
`21 03 45 01` (Ck Flag (3,0x45) == 1) umschliesst die beiden Sce_em_set @0x182E/@0x1842; der Trace springt von
@0x182A nach @0x1858. Leon-ROOM2000 dagegen: @0x1794 If Ck(4,0xF4) -> Ada/John (Sce_em_set Typ 0x42/0x40), Else
@0x17C6 `07 00 2e 00` -> die beiden 0x16-Wasserzombies. Elza trifft die 2001-Wasserzombies also erst ab Flag
(3,0x45) — Original-Skript, nicht Spur U.
Zweiter Lauf mit `RE15_SET_FLAG=3:69`: identisch zu M1 —
```
F144 R2001 slot=2 t=16 re2-eintritt ... F150 klettern ... F220 boden-1800 ...
F308 R2001 slot=2 t=16 oben st=1/0/0 mo=22 af=158 @(-20928,-1800,-10290) b1 f1ba=-1800 rot=0 kontakt=00
F340 R2001 slot=1 t=16 re2-eintritt ... F351 klettern ... F401 abbruch st=1/9/0 (Partner-Domino, s. M1)
```
0 Bilder unter dem Wasserboden / auf Band 2+. Bild (`m_elza2001/crop_elza.png`, F150-F310): derselbe Ablauf neben Elza.
ROOM2021 (Elza; die vier Sce_em_set @0x08F6-0x0932 stehen in main00 ohne If/Ck-Klammer davor, Rohbytes) deckt der
Riegel `integration_r38_wasserzombie` Lauf c ab (gruen, s. 0).

### M4 — ROOM2020 ueber den Raumlader (Debug-Sprung), Leon, Standard-KI, mit Bild

`RE15_DEBUG_JUMP=2020@250 RE15_PLAYER_POS=-9500,2600,1024,1`, `RE15_EXIT_AT=700#2020`. (Die Tuer 2050 -> 2020 liess sich
im Messlauf nicht ausloesen: der Spieler wird in 2050 bei F6 auf z = 4718 gesetzt und die Tuer feuerte weder mit Blick
+Z noch -Z — nicht weiter verfolgt, ROOM2000/2001 sind ueber echte Tueren gemessen.) wz.log:
```
F80 R2020 slot=4 t=16 re2-eintritt st=1/10/0 mo=0 af=31 @(-10756,0,1082) b0 f1ba=0 rot=3527 kontakt=c9
F109 R2020 slot=4 t=16 klettern st=1/10/3 mo=22 af=0 @(-10756,0,1082) b0 f1ba=0 rot=3072 kontakt=c0
F179 R2020 slot=4 t=16 boden-1800 st=1/10/3 mo=22 af=70 @(-10756,0,1082) b0 f1ba=-1800 rot=3072 kontakt=c0
F267 R2020 slot=4 t=16 oben st=1/0/0 mo=22 af=158 @(-10756,-1800,2222) b1 f1ba=-1800 rot=3072 kontakt=c0
F347 slot=3 re2-eintritt @(-8650,0,1082) ...   F415 slot=2 re2-eintritt @(-9553,0,1082) ...
```
Kontakt 0xc9 = Richtung 0xC, +8, u1 = 1 (Suedwand z = 1500); Vorschub +1140 in z. Bemerkenswert (gy.log): der
Band-1-Zombie Slot 2 (Spawn Nordsteg (-11250,-1800,-3100)) laeuft auf Leon zu, geht bei F281 ueber die Absturzkante
(u1 = 2) in den Kanal (y -1650 -> 0, Band 1 -> 0), durchquert ihn und zieht sich F421-F581 an der Suedwand hoch
(F581 (-9725,-1800,2101) b1). 0 Bilder unter dem Wasserboden / auf Band 2+.
Bild mit `RE15_FORCE_CUT=6` (Kanalblick, nur zur Sicht; `m_2020c6/sheet_2020c6.png`): F80/F110 Arme auf der Stegkante,
F140-F170 Oberkoerper ueber der Kante, F200-F230 Aufrichten auf dem Steg. Ohne FORCE_CUT bleibt die Kamera nach dem
Sprung auf Cut 0 (Tuer-Ende des Raums) — Folge des Teleports, nicht der Spur.

### M1-Ton — Frame-Wort-SE des Kletter-Clips in der RE2-KI

Lauf wie M1 mit `RE15_AUDIO_CAP_SYNC=cap.raw RE15_SE_DEBUG=1` (statt NOAUDIO), `RE15_EXIT_AT=300#2000`: zwischen
`[walk] F270` und `[walk] F300` steht `[se] Stimme: se=1 layer=0 vag=2 ... -> SE-Stimme 4` — passend zu Clip-Bild 130
(Klettern ab F150 -> F280; Bit 23 = SE 1, Unit-Riegel A). Der Ton des RE1.5-Clips laeuft also auch im RE2-Zweig
(`wz_frame_se`). (Eine zweite se=1-Zeile zwischen F240 und F270 laesst sich ohne Bildnummer im SE-Log nicht zuordnen.)

### M6 — Nebenwirkung: Abbruch des RE2-Hochziehens durch fremden Zustandsschreiber (Bild fuer Bild)

Lauf wie M1, `RE15_FRAMEDUMP=396-404/1:fd/f`, `RE15_EXIT_AT=406#2000` (deterministisch: dieselben Bilder wie M1).
gy.log Slot 3 (zweiter Kletterer) — woertlich gekuerzt:
```
F399 [3 t=16 st=1/10/3/0 mo=22 af=48 @(-22068,0,-9970) b0 f1ba=0]
F400 [3 t=16 st=1/9/0/0  mo=22 af=49 @(-22068,0,-9970) b0 f1ba=0]   <- 0x901 von Slot 4 (Griff P8) NACH Slot 3s Tick
F401 [3 t=16 st=1/9/1/0  mo=13 af=23 @(-22468,0,-9970) b0 f1ba=0]   (wz.log: F401 abbruch st=1/9/0 mo=22 af=9)
```
Bild (`m_domino/crop_domino2.png`): F398/F399 haengt der Zombie mit dem Oberkoerper auf der Stegkante; **F400 ist er bis
auf ein Bein unten links verschwunden**; F401 steht er im Taumel-Clip 13 im Kanal. Ursache im Code: die Bank-Abfrage
`re15_wasserzombie_bank` (Renderer main.c, Uhr `re15_actor_clip_len`, Schuss-Anker) gilt nur, solange `wz_marke_gilt`
(Zustand 1 UND Sub 0x0A). Schreibt ein anderer Aktor im selben Bild nach dem Tick des Kletterers ein Zustandswort
(Partner-Domino 0x901 aus RE2-Griff-P8, Slot 4 > Slot 3), zeichnet der Renderer den Zombie in diesem Bild mit
RE1.5-Clipnummer 22 / Bild 49 auf der **RE2-Bank** (EM016, 31 Clips) — ein falscher Clip auf dem falschen Rig. Auch
der Bildzaehler wird im naechsten Bild VOR dem Abbruch auf der RE2-Clip-22-Laenge weitergezaehlt (af 49 -> 9).
Die Spur erzeugt diesen Ein-Bild-Sprung (vorher gab es kein RE1.5-Posing auf dem RE2-Zombie); der Riegel F prueft
nach dem Abbruch nur y/+0x1ba/Band, nicht Clip/Bank. -> Mangel 1.

### M5 — Nebenwirkung: andere Raeume mit u1&1-Zellen (Mechanismus ist raumunabhaengig)

Eigener SCA-Zensus (RDT u32 @0x20, Kopf 24 B, Zellen 12 B `w,d,x,z,type,u0,u1,floor`, Band = floor >> 4; Skript
Scratchpad `re/sca_u1.py`) fuer die im Dossier genannten Raeume:
- ROOM1090: eine Zelle [3] x -15243..10285 z -6284..8458 floor 0x03 = **Band 0**; alle vier Zombie-Spawns
  (Sce_em_set @0x023a0-0x023dc) stehen auf **Band 1** (pc[4] = 1, y = -1800). Die Klemme FUN_8003b0a4, die +0x90 setzt, sieht
  nur Zellen des eigenen Bands (selbst disassembliert: `lhu a0,0(s2)` mit s2 = Zelle+10 / `sll 16` / `srl 28` /
  `lbu v1,130(ent)` / `bne -> naechste Zelle` @0x8003b224-23c) -> kein Hochziehen im normalen Spiel.
- ROOM10B0: eine Zelle [6] floor 0x01 = Band 0; alle vier Spawns @0x01726-0x01762 auf Band 1 -> ebenso.
- ROOM3010: Zellen [4] und [17] auf Band 0, Spawns 1-5 auf Band 0 (Deskriptoren 0x84/0x85/0x87/0x88). Spawn-Block
  @0x210E `06 00 0c 00 / 21 04 0a 00` = nur bei Flag (4,0x0A) = 1. Lauf `RE15_SET_FLAG=4:10 RE15_DEBUG_JUMP=3010@250
  RE15_PLAYER_POS=-23700,-9100,0,1`, 900 Bilder Standard-KI: alle 7 Gegner gespawnt, kein Hochziehen ausgeloest
  (wz.log leer), keine Auffaelligkeit in gy.log (Zombie 7 faellt regulaer von Band 1 auf Band 0).
- ROOM5020: Zellen auf Band 0/1/3; ein Typ-0x18-Spawn auf Band 0 — nicht gelaufen (Stage 5, Spawn-Bedingung nicht
  geprueft). Restrisiko gering: derselbe Mechanismus, dieselbe Bank (Unit-Riegel A prueft 0x18).
Ergebnis: Stage 1 (1090/10B0) ist im normalen Spiel nicht betroffen; 3010 unauffaellig.

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
aufrecht) — im Kopf als "PORT-ABBILDUNG" beschrieben. Suchwoerter `deferred|tunable|interim|for now|faithful|plausib|
TODO|approx|geschaetzt|vermut|ungefaehr` in den `+`-Zeilen des Code-Diffs: **0 Treffer**. Einziger neuer getenv:
`RE15_WASSERZOMBIE_LOG` (Messschiene, oeffnet nur eine Logdatei, kein Verhalten) — kein Env-Schalter als Abschluss.

Weitere eigene Pruefungen zum Gate:
- Die Gegenprobe "kein Zombie-Zustand setzt +0x1ba zurueck" (Grundlage des RE1.5-Abbruch-Restpunkts im Dossier):
  die drei uebrigen EXE-Schreiber @0x80043300 (Funktion ~0x800431cc, nur ueber Tabelle @0x8007469c-b8),
  @0x80052750 (~0x80052508) und @0x80052d98 (~0x80052c14, `jal 0x8001c6e8` room_coll davor) haben keine jal-Aufrufer
  und stehen nur in den EXE-Tabellen @0x80073e70-90 / @0x80076ce0-d00 (eigener Wort-Scan) — nicht in den
  STAGE1/STAGE2-Zombie-Tabellen. Die Aussage haelt, soweit statisch pruefbar; dynamisch bleibt sie offen (Dossier 6).
- Die Klemme FUN_8003b0a4 filtert Zellen nach Band (@0x8003b224-23c, s. M5) — Grundlage dafuer, dass Band-1-Zombies in
  1090/10B0 nicht auf Band-0-Kletterzellen reagieren.

## 4. Gezielte Tests (keine volle Suite)

- `ctest -R "^unit_r38_wasserzombie$" -V`: gruen; Ausgabe u.a. `[C RE1.5 2020] ... oben=F258 landung(slot 4)=(-11536,-1800,2222)
  ... danach max_y=-1800 band 1..1`, `[D RE2 2020] ... re15bank=1 len=159 ... st=1/0 mo=0`, `[D-Gegenprobe ohne Bank]
  eintritt=F-1 y=0`, `[F] Abbruch: st=1 y=0 f1ba=0 band=0`, `[E RE1.5 2000]` / `[E RE2 2000]` je oben (-20928,-1800,z) b1.
- `ctest -R "^integration_r38_wasserzombie$"`: `100% tests passed, 0 tests failed out of 1` (88.94 s).
- Gruppe der beruehrten Module (Regex des Bau-Agenten + `unit_re2_hybrid_rig|unit_r36_pri_schatten|unit_enemy_.*|
  .*schwerkraft.*|.*gravity.*`, -j 4): `100% tests passed, 0 tests failed out of 103` (20.6 s).
- Integrationsgruppe seriell (`integration_r36_schuss|r36_elza_schuss|r37_beretta|r37_zombieaktiv|r36_pri_schatten|
  r36_gegnerzahl|r36_klettern|r36_szenenleon`): (laufend)

## 5. Vertrag

- Pfade (`git diff master...HEAD --name-only`, 11 Dateien): nur Dossier, neues Modul + Kopf, Haken in
  enemy_ai_common.c / enemy_ai_re2_zombie.c / schuss_r36.c / platform/pc/main.c, Riegel unter tests/unit (Sonde
  `tests/unit/probes/r38_wasserzombie.cmake`, keine Test-CMakeLists) und tests/integration. **Kein** release/,
  platform/android/, shared_assets/PSX/.
- Ressourcen: keine Bank-9-Bits, keine Nachrichten-Ids, keine Items/Dokumente, keine Sprachdateien (VERTRAG 1.1-1.3
  nicht beruehrt).
- Gemeinsame Dateien: alle Haken klein und mit "Runde 38 Spur U" gekennzeichnet (enemy_ai_common.c: Include, ein
  ersetzter Aufruf, 2 Zeilen in re15_actor_clip_len, eine Hinweiszeile; enemy_ai_re2_zombie.c: Include + 1 Zeile vor
  dem switch; schuss_r36.c: Include + 2 Zeilen; main.c: Include + 1 + 1 + 5 Zeilen). VERTRAG 1.4 nennt
  enemy_ai_common.c fuer B/T und schuss_r36.c gar nicht — die Tabelle ist sichtbar nicht abschliessend (R, S, M haben
  enemy_ai_common.c ebenfalls beruehrt); Form der Haken ist vertragsgemaess.
- Probe-Merge (nur Objekte, kein Arbeitsbaum): `git merge-tree --write-tree r38/integration a4809c35` -> Konflikte NUR
  in den Include-Zeilen von enemy_ai_common.c (Spur I/T an derselben Stelle) und main.c (Spur E/I/T) — beide Seiten
  behalten; enemy_ai_re2_zombie.c (Spur L) automatisch.
- Riegel je Punkt: unit_r38_wasserzombie (A-F) + integration_r38_wasserzombie (a-d) — messend (Log-Folge, Lage, Band,
  +0x1ba, Invariante "nie unter dem Wasserboden / nie Band 2").
- Code-Hygiene (kein Verhaltensfehler): die alte `static` Funktion `re15_enemy_ai_live_edge_fall` bleibt unbenutzt
  im Code (enemy_ai_common.c:3380, -Wunused-function), der Kommentar direkt ueber dem neuen Aufruf (enemy_ai_common.c
  ~4774) beschreibt weiter "EDGE-FALL ... tumbles over the contact edge ... down one floor" — widerspricht der neuen
  Einordnung; `include/re15_wasserzombie_r38.h:84` erzeugt `-Wcomment` ("*sk/*an" in einem Kommentar) in jeder
  einbindenden Datei.
