# Runde 37 — Spur J "bissarm" (2026-10-12)

Zweig `r37/bissarm`, Basis master 419380dc. Dossier fortlaufend (Sitzungslimit-Schutz: nach jedem Schritt committet).

## Auftrag (Wortlaut, AUFTRAG.md Nachtrag 6)

> Ja, wie ich gerade gesehen habe fehlt in unseren Port teilweise noch der dranhängende Arm wenn man vom Zombie gebissen wird.

Lesart (AUFTRAG.md, beide pruefen, keine F9-Marke): (1) der Arm des Zombies, der beim Biss/Griff an Leon haengt
(Griff von vorn/hinten, Kriecher am Bein; Leon UND Elza), fehlt in manchen Faellen/Bildern; (2) der angeschossene,
noch "dranhaengende" (baumelnde) Arm eines Zombies fehlt beim Biss. Erst messen, welcher Fall auftritt, dann wie RE2.

## 0. Stand

- [x] 1 Messung vorher (Port, Griff vorn/hinten, ROOM1140/1010, Leon und Elza, RE2-Original per GDB)
- [x] 2 RE-Belege (RE2 Griff EXEC[3], Wandklemme FUN_8003567c, RE1.5-Spieler-Tick, Kriecher-Hirn, Lesart 2)
- [x] 3 Umsetzung (Handkontakt + Opfer-Bezug, Wandklemme ein Durchgang je Bild)
- [x] 4 Messung nachher (Sonde gegen RE2-RAM, Echtlauf-Logs, Bilder)
- [x] 5 Tests (9 eigene Riegel + 67 Nachbar-Riegel gruen)
- [x] 6 OFFEN / 7 Fuer den Nutzer

## 1. Messung vorher (Bau 419380dc, eigene Messkopie `re15_pc_j37mess.exe`, RE2-Standardgehirn, Hybrid-Modell)

Werkzeug: Spielstand per `test_r36_kleidung karte` (Slot 0) -> CONTINUE; `RE15_STATE_LOG`, `RE15_FRAMEDUMP`
(Serien), `RE15_FORCE_CUT` (nur zum Hinschauen, Kamera festnageln), `RE15_ENEMY_DBG`, `RE15_RE2_TRACE`.
Laeufe unter `%TEMP%/.../scratchpad/J37/` (unversioniert; Zahlen hier zitiert).

### 1.1 ROOM1140 (Briefing-Raum, 5 Zombies) — Leon (-1400,-19600) Cut 1
- `enemy_dbg.log`: `RE2 EM016/EM010/EM011: 17 Meshes, 15 Bones, 31 Clips`, gezeichnet je `15 Teile`.
- Griff Slot 5 (Typ 0x11) F58..F165 (state.log `ss1=3`, `gr=1`), danach Slot 2 F166..F271 (Tod F227).
  Leon steht waehrend der Griffe neben dem Greifer (gemessen F79 PL(-791,-19733), Greifer (-297,-19708): 494).
- Bild (Skala 3): Raum zu voll (vier Zombies um Leon), Arm-Zuordnung im Bild nicht eindeutig.

### 1.2 ROOM1010 (zwei Zombies) — Leon (3700,0,2300) Cut 4, 0,3 s rueckwaerts -> Griff durch Slot 1 (Typ 0x10)
state.log (Auszug):
```
F57 PL(3392,3279,rot=0,hp=100)    gr=0 [1 t=10 st=1 ss1=1 ...  @(3998,3875,r1541)]
F58 PL(0,0,rot=3589,hp=100)       gr=1 [1 t=10 st=1 ss1=3 ss2=1 mo=11 ... @(3998,3875,r1541)]
F89 PL(0,0,rot=3589,hp=80)        gr=1 [1 ... ss1=3 ss2=3 mo=12 af=26 d=5585 @(3524,3437,r1541)]
F146 PL(0,0,rot=3589,hp=60)       gr=1 [1 ... ss1=3 ss2=7 ...]
F147 PL(3514,3178,rot=3589,hp=60) gr=1 [1 ... ss1=3 ss2=8 ...]
```
**Leon steht den GANZEN Griff (F58..F146) auf (0,0)** — der Greifer beisst an seinem Platz ins Leere
(Kamera 6: Zombie allein in Biss-Pose ab F66), Leon ringt in Kamera 5/7 allein vor dem Schreibtisch.
Ursache (Code, `enemy_ai_common.c` re15_victim_place): die Port-Wandklemme (5f9ec726, 2026-09-13, "gilt jetzt
fuer JEDEN Greifer") faellt auf `s_victim_ok_x/z` zurueck; der Startwert wird nur beim Gitterarm-Griff
(enemy_ai_common.c ~Z.12901) und beim Gorilla (affen_11c0.c:86) gesetzt — NIE beim RE2-Zombie-Griff
(`re15_re2z_victim_begin`). Startwert ist damit der Rest eines frueheren Griffs bzw. (0,0).

### 1.3 Einzel-Greifer ROOM1140 (Kill-Flags der Em-Status-Zone 7 = Sce_em_set pc[7] 211..214, Gate @0x80042120-38)
Spielstand per neuem Werkzeug `test_r37_bissarm karte` (Waffe 3 ausgeruestet, Flags gesetzt) -> nur Satz 4 (Typ 0x11,
(200,-19600)) spawnt. Griff von VORN (Leon (-1000,-19600) Blick 0, Kamera Cut 1 hinter Leon) und von HINTEN (Blick 2048,
Kamera vor Leon). Bilder: `J_belege/vergleich_vorn_hybrid_ik.png` (obere Zeile = heute), `J_belege/vergleich_hinten_hybrid_ik_p1_re2.png`
(oberste Zeile = heute).
- Alle 15 Teile des Greifers werden in jedem Griffbild gezeichnet (`re15_re2z_gore_resolve` 15/15, `test_r37_bissarm griff`).
  Kein Teil "fehlt" im Sinne von nicht gezeichnet.
- **Vorn (Kamera hinter Leon):** der Unterarm des Zombies liegt QUER/TIEF ueber Leons Ruecken, nicht ueber der Schulter.
- **Hinten (Kamera vor Leon):** auf Leons Brust/Schulter haengt in vielen Bildern KEIN Zombie-Arm (F96, F112); der Arm liegt
  hinter Leons Schulter verdeckt. Das ist der "teilweise fehlende dranhaengende Arm".
- Elza (PL04.EMR, Sonde R37J_ELZA): Schultern ~100 tiefer als Leon (-2277 statt -2379), Hand 11 liegt 47..90 von ihrer Schulter.

### 1.4 RE2-ORIGINAL gemessen (DuckStation-GDB, `J_belege/re2_mess/re2_biss_gdb.py`, Muster Runde 35 Spur H)
Spielstand `%LOCALAPPDATA%/DuckStation/savestates/SLUS-00748_4.sav` (Stage 0 Raum 20, EIN Zombie Typ 0x10 bei (-26809,-19947)),
Disc re2leon.cue = dieselbe EXE wie info/re2leon/PSX.EXE (Code 61440/61440, re2_ss.py). EMOVL10_S0.BIN ist dort RELOZIERT bei
0x8010D000 geladen (12474/13266 Worte gleich, Rest = relozierte Zeiger; P3-Bytes `21202002 21284002` @0x8010F838 == Datei+0x2838).
Haltepunkt = Biss-Loop P3 (Port-Adresse @0x80102838, im Spielstand 0x8010F838), je Halt PL+16 Parts + Zombie+17 Parts
(`J_belege/re2_vorn_frames.bin`, `re2_hinten_frames.bin`, Texte `*_frames.txt`), Bild = VRAM beim Schliessen (ConfirmPowerOff
fuer den Lauf false, settings.ini + Resume-Spielstand danach zurueckgeschrieben).
- Bilder: `J_belege/re2_original_vorn_biss.png` (Kamera hinter Leon: der nackte Zombie-Unterarm haengt UEBER Leons linker
  Schulter den Ruecken hinunter) und `re2_original_hinten_biss.png` (Kamera vor Leon: Zombie-Arm UEBER Leons rechter
  Schulter auf der Brust; Leon haelt dabei seine Waffe).
- Zahlen (re2gt / re2_ana): Zombie-Teile 0..14 Flag 1 (gezeichnet), 15/16 = 0. Hand 11 liegt 45..180 von Leons Part 12
  (Schultergelenk), Y -2423 bei Leons Schulter -2379 (Vorn, Halt 16) = die Hand liegt UEBER der Schulter.
- Volumenmass (Unterarm/Hand-Vertices in Leons Rumpf/Kopf/Oberarm-Huelle aus PL00.MD1): Unterarm 10 0..4/26, Hand 11 0..2/42.

### 1.5 Warum der Port anders aussieht — zwei Mechanismen, beide gemessen
**(A) Hybrid-Proportionen.** Der RE2-Flavor zeichnet seit 2026-08-21 bedingungslos den Hybrid (RE2-Gehirn/-Clips,
RE1.5-Mesh/TIM, `re2_hybrid_apply`: Hierarchie RE2, BIND-LAENGEN RE1.5 + root_y_fix). Die RE1.5-Zombies sind ~10 % kleiner:
Bind-Kette (Sonde `test_r37_bissarm skel`, Byte-Tabellen PSX/EMD/CDEMD0.EMS vs RE2/CDEMD0.EMS):

| | Wurzel | Schulter | Oberarm | Unterarm | Kopf | Beinkette |
|---|---|---|---|---|---|---|
| RE1.5 EM010 | -1751 | (-50,-550,-399) | 517 | 419 | -667 | 228+664+783=1675 |
| RE2 EM010 | -2067 | (-54,-605,-421) | 569 | 459 | -783 | 252+730+861=1843 |
| Leon PL00 | -1804 | (-59,-692,-369) | 422 | 408 | -704 | |

Mit RE2-Rig (Sonde R37J_REIN_RE2) liegt die Hand im Port BITGLEICH zum RE2-RAM (Vorn F78: Hand 11 Y -2423, Leon-Schulter -2379,
Abstand 50 — RE2 Halt 16: 50). Mit dem Hybrid liegt dieselbe Hand ~200 TIEFER (F78: Y -2225, Abstand zur Schulter 168;
dHand 202..234 ueber alle Griffbilder, Sonde R37J_IK) — sie kommt UNTER statt UEBER Leons Schulter, der Unterarm verschwindet
hinter/in Leon. Gegenproben: RE1.5-Flavor (eigene RE1.5-Griff-Clips zu RE1.5-Modellen) zeigt die Arme UEBER den Schultern
(`J_belege/re15_flavor_hinten_biss.png`); reines RE2-Modell im Port (Experiment) ebenso. Nur die Mischung ist falsch.

**(B) Port-Wandklemme der Opfer-Platzierung** (`enemy_ai_common.c` re15_victim_place, seit 5f9ec726 fuer JEDEN Greifer):
(1) Startwert `s_victim_ok_x/z` wird beim RE2-Zombie-Griff NIE gesetzt (nur Gitterarm/Gorilla rufen
`re15_victim_anchor_calibrate`) -> ROOM1010: Leon steht den ganzen Griff auf (0,0) (Sonde: Leon (-125..,..) d=5250 zum Zombie).
(2) Der Rueckfall "nicht idempotent -> letzter guter Standpunkt" haelt Leon fest, sobald ein Bild 2 Einheiten Rundungsrest
hat (Sonde R37J_KLEMME, ROOM1010 F31: n1 (3215,3978) c1 (3217,3976) -> Rueckfall), danach bis 394 Einheiten neben der
RE2-Formation (F267: Ist (3557,3156) Soll (3754,2959)) — der Zombie-Arm haengt dann in der Luft bzw. in Leon.

**(C) Ausgeschlossen (gemessen):** Teile-Maske (alle 15 Teile gezeichnet, 1.3); Region-Cull des ganzen Greifers
(main.c `npc_region_culled` = Original FUN_8001e8c8 @0x8001e970-7c, Punkt-im-Viereck DAT_800ac790, aussen nur
zusammensetzen @0x8001e9b4): Messwerkzeug `test_r37_bissarm region` ueber alle Griffbilder der Laeufe ROOM1140
(Leon und Elza, je 103), ROOM1010 (103, Cut 6) und ROOM1030 (265) — 0 Bilder mit Leon innen und Greifer aussen.

## 2. RE-Belege

### 2.1 Griff = RE2 EXEC[3] @0x801025EC (Phasen-Tabelle @0x8010001C)
- Aufrechter Zombie: Zustand 1, +0x5 == 3 (Ausfuehr-Tabelle @0x8010C8CC[3], Entscheid @0x8010C88C); Kriecher: Zustand-1-Wurzel @0x8010114C
  `andi 0x3f` @0x8010115C in die Tabelle @0x8010C854 (ungerade -> Kriecher-Hirn @0x80101210, gerade -> aufrecht
  @0x8010118C), Kriecher-EXEC @0x8010C918[1] = **dieselbe** Funktion 0x801025EC. Kriech-Variante im Griff selbst:
  `lhu v0,270(s1)` @0x8010266C, `andi s5,v0,0x1` @0x80102674 (+0x10E Bit 0), Typ 0x11/0x17 `addiu s5,s5,2` @0x80102690.
- Zombie-Clips aufrecht: 11 Zupacken (P0/P1), 12 Biss-Schleife (P2 `addiu v1,1` @0x80102830 = Griff+1), 13 Abwurf.
  Biss-Schleife P3 @0x80102838; im Spielstand SLUS-00748_4.sav liegt EMOVL10_S0 reloziert bei 0x8010D000, P3 =
  0x8010F838 (Bytes `21202002 21284002` == Datei+0x2838).
- RE2-RAM (Fixture `J_belege/re2_vorn_frames.bin` / `re2_hinten_frames.bin`, Part-Matrix +0x5C je Halt): Hand 11
  liegt 45..180 von Leons Schultergelenk 12, Y -2423 bei Schulter -2379 (vorn Halt 16) = UEBER der Schulter.
  Der Port zeichnet mit RE2-Bind dieselbe Hand **bitgleich** (Sonde `R37J_REIN_RE2`, 1.5) — die RE2-Rig-Pose ist
  damit der Beleg fuer den Kontaktpunkt.

### 2.2 Wandklemme im Griff (RE2-Hauptschleife, selbst disassembliert aus info/re2leon/PSX.EXE)
```
8002660c  lhu  v0,0x3b1e(s3)        ; Spieler+0x10E (s0 = s3+0x3a10)
80026614  andi v0,v0,0x8000
80026618  bne  v0,zero,0x80026688   ; im Griff RAM +0x10E = 0x000F -> faellt durch
80026620  jal  0x8003bfac           ; Spieler-Tick (Opfer-Platzierung 0x80015CB8)
80026628  jal  0x800355c4           ; Schub
80026634  jal  0x8003567c           ; Wandklemme, a1 = 0x8000 (Delay-Slot @0x80026638)
FUN_8003567c:
8003568c  lw   v1,0(s0)
80035694  andi v0,v1,0x8 / 80035698 bne v0,zero,0x800356ec   ; RAM word0 0x40001001 -> Bit 3 = 0
800356a0  lw   v0,484(s0) / 800356a8 bne v0,zero,0x800356ec  ; RAM +0x1E4 = 0
800356c0  addiu a0,s0,132           ; +0x84 Position
800356c8  lbu  a2,262(s0) / 800356d4 sllv a2,v0,a2           ; Band-Maske 1<<+0x106
800356cc  lhu  a1,144(s0)           ; +0x90 Radius (450)
800356d0  jal  0x8004c1bc           ; EIN Durchgang
800356ec  lw   v0,132(s0) / 800356f0 lw v1,140(s0)
800356f4  sh   v0,280(s0) / 800356f8 sh v1,282(s0)          ; alter Standpunkt +0x118/+0x11A fuer das NAECHSTE Bild
```
RE1.5 genauso: Spieler-Tick nach dem Kommando-Handler (`jalr v0` @0x80031cb4): `lw v0,-13764(v0)` = 0x800aca3c
@0x80031d44, `andi 0x4000` @0x80031d4c, `jal 0x8002b498` @0x80031d58 (Schub), `jal 0x8003b0a4` @0x80031d70
(`ori a2,zero,0x1` @0x80031d74) — ebenfalls ein Durchgang je Bild, kein Rueckfall.

### 2.3 Elza: die Originale passen den Griff je Figur an
- RE1.5 liefert zwei Gegnerarchive: `EMD/CDEMD0.EMS` (Leon) und `CDEMD1.EMS` (Elza). Eintragsweiser Vergleich
  (Werkzeug `ems_cmp.py`, Ordnung re15_ems.c s_ems_order): EM10/EM11/EM12/EM13/EM16 (Index 0..4) unterscheiden
  sich GENAU in dir[3]/dir[4] (Aktionsbank, 43 Clips inkl. Griff 0..5) und dir[5]/dir[6] (Opferbank); dir[0..2],
  dir[7] (MD1), dir[8] (TIM) byte-gleich. EM18 nur Opferbank.
- RE1.5-Griff gemessen (Sonde `re15_kontakt`, RE1.5-Gehirn, Biss-Schleife +0x6 == 3 @0x801027dc, Hand 10/13 gegen
  Opfer-Schulter 12/9, Mittel ueber 101 Bilder, Y negativ = oben):

| Bank / Opfer | vorn Hand10-S12 | vorn Hand13-S9 | hinten Y |
|---|---|---|---|
| CDEMD0 / Leon (Original Leon) | (-64,-127,-2) | (-63,-165,-56) | -138 / -133 |
| CDEMD1 / Elza (Original Elza) | (-31,-65,-181) | (40,-20,22) | 0 / -30 |
| CDEMD0 / Elza (heutiger RE1.5-Port) | (-16,-229,-77) | (-22,-284,20) | -255 / -255 |

  Die Elza-Bank legt die Zombie-Hand ~165 tiefer — passend zu Elzas tieferen Schultern. RE2 hat dasselbe
  (Claire-Archiv), im Repo liegt aber nur `info/re2leon`.
- Port vor dem Elza-Schritt (Hand auf das absolute RE2-Leon-Ziel): Elzas Kontakt-Hand 117/121 ZU HOCH ueber ihrer
  Schulter; ohne Handkontakt (v0.8.24-Stand) 83/79 zu tief.

### 2.4 Kriecher (Bein-Griff)
Sonde `kriecher` (ROOM1030, zwei Kriecher wie probe_re2z_crawl_attack, 172 Griffbilder Clip 14/15): Hybrid-Hand
mittel 542, RE2-Rig-Hand 531 vom naechsten Bein-Gelenk Leons; |RE2-Rig - Hybrid| mittel 95. In BEIDEN Rigs liegt
keine Hand an Leon (der Kriecher beisst ins Bein) — kein Kontaktpunkt, kein Handlungsbedarf.

### 2.5 Lesart (2) "angeschossener, baumelnder Arm"
RE2 hat keinen baumelnden Arm. Abgeschossener Unterarm = fliegendes Teil, FUN_80028DAC (info/re2leon/PSX.EXE):
```
80028dc0  lhu  v1,160(s0)          ; Lebensdauer +0xA0
80028dcc  sltiu v0,v0,0x1d         ; < 29 ?
80028dd8  j 0x80028de4 / 80028ddc sw zero,0(s0)   ; nach 29 Bildern Flagwort 0 = weg
80028df0  lhu  v0,154(s0) / 80028df4 lhu v1,158(s0) / 80028e00 sh v0,154(s0)   ; +0x9A (vy) += +0x9E
80028e08  lw   v1,96(s0) / 80028e14 sw v1,96(s0)   ; Part +0x60 (t.y) += +0x9A
```
Der Port hat das schon (enemy_ai_re2_zombie.c re2z_arm_throw, Wurf @0x80107514-88: +0x9A = -10 @0x8010755C,
+0x9E = 0 @0x80107560 -> steigt 10 je Bild und ist nach 29 Bildern weg). Nichts zu bauen.

## 3. Umsetzung

Neue Dateien: `re15_port/include/re15_bissarm_r37.h`, `re15_port/engine/src/bissarm_r37.c`. Haken (je 1 Zeile,
Kommentar "Runde 37 Spur J"):
- `platform/pc/main.c` Spieler-Zeichner nach `re15_skel_compute_pose`: `re15_bissarm_r37_opfer_merken(...)` (nur im
  Opfer-Zustand); NPC-Zeichner nach dem Region-Cull: `re15_bissarm_r37_handkontakt(...)`.
- `engine/src/re2_ems.c` re2_hybrid_apply: `re15_bissarm_r37_re2_bind_merken(kind, &eb->skel)` vor dem Umbau.
- `engine/src/enemy_ai_common.c`: Startwert `re15_victim_anchor_calibrate(player->x, player->z)` beim Zupacken
  (RE1.5-Latch und re15_re2z_victim_begin; RE2 +0x118/+0x11A `sh` @0x800356f4-f8) und in re15_victim_place
  `|| re15_bissarm_r37_ein_durchgang(vg_ty)` (Zombie-Familie: EIN Durchgang, kein Rueckfall; `jal 0x8004c1bc`
  @0x800356d0, RE1.5 @0x80031d70).

**(A) Handkontakt** (PORT-KONSTRUKT wie root_y_fix; Invariante = RE2-Kontaktpunkt): waehrend die Griff-Funktion
0x801025EC laeuft (Zustand 1, +0x5 == 3) wird je Arm die Pose desselben Keyframes zweimal ohne Ueberblendung
gerechnet (Hybrid-Rig und RE2-Rig mit RE2-Bind, ohne Wurzelkorrektur); Ziel = gezeichnete Hand + (RE2-Hand -
Hybrid-Hand), Ellbogen-Seite = RE2-Ellbogen; Zwei-Gelenk-Kette mit den RE1.5-Armlaengen (Oberarm/Unterarm aus der
Hybrid-Bindtabelle), Drehung der Glieder per Rodrigues. Keine neue Zahl: alle Laengen/Lagen kommen aus den
Bindtabellen (RE2/CDEMD0.EMS, PSX/EMD/CDEMD0.EMS) und den RE2-Keyframes. Die Ueberblendung (+0x8F) bleibt.
Kriecher (+0x5 == 1) und Fress-Zustand (EXEC[6]) bleiben unberuehrt (2.4).

**(A2) Opfer-Bezug** (Elza): `re15_bissarm_r37_opfer_merken` rechnet die Schultern 9/12 des gezeichneten Opfers und
dieselbe Opfer-Pose mit Leons Knochen (`PLD/PL00.PLD`, CD-Datei 0x3C = Tabelle @0x80073f70 Eintrag 0, Teil 1 EMR
ueber re15_pld_part, gelesen mit re15_asset_read_file). Ziel += nach 1/d^2 gewichtete Schulter-Differenz (Welt ->
Rahmen des Greifers, Umkehrung von re15_skel_bone_to_world), nur im stehenden Griff (+0x10E Bit 0 = 0 @0x80102674).
Fuer Leon ist die Differenz bitgenau 0 (gleiche Knochen) — Leon aendert sich dadurch nicht. Ein dritter Anker
(Kopf 8) wurde gemessen und verworfen: er zog die Kontakt-Hand von der Schulter weg (vorn -157 statt -148).

**(B) Wandklemme**: s. Haken oben. ROOM1140 (freie Flaeche) unveraendert, ROOM1010 Leon am Greifer.

**Mess-Schiene** `RE15_BISSARM_LOG=<datei>`: je gezeichnetem Griffbild `hand11/ziel11/rest11`, `hand14/ziel14/rest14`
(Welt) und `bezug11/bezug14` (angelegter Opfer-Bezug). Kein Verhalten.

## 4. Messung nachher

### 4.1 Sonde gegen das RE2-Original (unit_r37_bissarm_re2_*)
Vektor Zombie-Hand - Leons Schultergelenk im Greiferrahmen, Port-Bild k gegen RE2-Halt k+1, 29 Biss-Bilder:

| | ohne Handkontakt | mit Handkontakt | max (Bilder 0..3) |
|---|---|---|---|
| vorn Hand 11 / 14 | 222 / 223 | 9 / 3 | 83 / 27 |
| hinten Hand 11 / 14 | 222 / 223 | 8 / 2 | 83 / 27 |

Ab Biss-Bild 4 jedes Bild <= 6 (Rundung). Bilder 0..3 und im Echtlauf 31..37 des Biss-Clips: die RE2-Hand liegt
jenseits der RE1.5-Armlaenge (Rest bis 133), der Arm ist dort gestreckt zum Ziel gerichtet.

### 4.2 Elza (unit_r37_bissarm_elza_*), Kontakt-Hand je Biss-Bild ab 4, Hoehe ueber IHRER Schulter gegen RE2/Leon
| | v0.8.24 | absolutes RE2-Ziel | mit Opfer-Bezug |
|---|---|---|---|
| vorn | 83 zu tief | 117 zu hoch | 0 (max 2), Abstand 4 (max 11) |
| hinten | 79 zu tief | 121 zu hoch | 2 (max 8), Abstand 6 (max 14) |

### 4.3 Echtlauf (exe, integration_r37_bissarm_echtlauf; Gegenprobe mit dem Bau 419380dc)
| Lauf | nachher | vorher |
|---|---|---|
| A ROOM1140 Leon, Griff von hinten | 103 Griffbilder gezeichnet mit Handkontakt, 50 Biss-Bilder beide Haende auf dem RE2-Ziel, Opfer-Bezug 0, Halten max 1128 | kein bissarm.log (Haken fehlt) |
| B ROOM1010 Cut 6 | 103 / 54, Halten max 917, 0 Bilder auf (0,0) | Leon 89 Bilder auf (0,0), 81 Halte-Bilder > 2000 (bis 5571), Sprung 4737 |
| C ROOM1140 Elza | 103 / 58, Opfer-Bezug in 103 Zeilen, |Y| mittel 116 | — |

Wandklemme in der Sonde (unit_r37_bissarm_klemme_*): ROOM1010 vorher 16 bzw. 80/81 Fehlbilder, nachher 0; ROOM1140 0/0.

### 4.4 Bilder (`J_belege/`)
- `vergleich_leon_hinten_vorher_nachher.png`, `vergleich_leon_vorn_vorher_nachher.png`: obere Zeile v0.8.24, untere
  nachher — der Zombie-Unterarm liegt jetzt ueber Leons Schulter (hinten F104: vorher kein Arm auf der Schulter).
- `vergleich_elza_vorher_absolut_bezug.png`: Zeilen v0.8.24 / absolutes RE2-Ziel / mit Opfer-Bezug (F104: Unterarm
  auf Elzas Schulter statt darueber).
- `room1010_nachher_cut6.png`: Leon und Greifer gemeinsam im Bild (vorher `room1010_vorher_leon_weg.png`).
- RE2-Original: `re2_original_vorn_biss.png`, `re2_original_hinten_biss.png`.

## 5. Tests

Eigene Riegel (`tests/unit/probes/r37_bissarm.cmake`, `tests/integration/test_r37_bissarm_echtlauf.cmake`):
unit_r37_bissarm_re2_vorn/_hinten, _elza_vorn/_hinten, _klemme_1010a/_1010b/_1140_vorn/_1140_hinten,
integration_r37_bissarm_echtlauf.

`ctest -R "r37_bissarm"`: `100% tests passed, 0 tests failed out of 9`

Nachbarn (Kleidung/Schuss Runde 36, Hybrid/Rig, Gore, Griffe, Kriecher, Gitterarme 1210, Affen, Zombie-KI):
`ctest -R "r37_bissarm|r36_kleidung|r36_schuss|re2_hybrid_rig|rig_root_fix|re2_gore|re2z_import|re2z_gore_se|re2_victim_collapse|dog_grab_anchor|re2z_grab_core_se|re2z_devour_anchor|re2z_pushoff_crawl|re2z_finisher_bite|re2_crawler|crawl_toggle|re2z_crawl_hurt_reentry|re2z_crawler_spawn_wait|1030_crawl_live|1210_|r35_raeume|r35_affen|r36_zombieki|re2_zombie_ai|re2_zombie_abc|zombie_pose|re2_zombie_pose_bank"`:
`100% tests passed, 0 tests failed out of 76` (421 s, seriell). Keine volle Suite (Spurbaum).

## 6. OFFEN

1. **ROOM1220 duenne Wand (Kriecher-Griff).** Zelle 7 (x -21825..-21550, 275 dick): die Kriecher-Formation stellt Leon
   ~2000 vom Greifer; Bild F13 springt er in EINEM Durchgang von (-21086,-5496) nach (-22318,-5344) durch die Wand
   (vorher wie nachher gleich, Sonde `pruef_klemme 1220 -21000 -5500 0 vorn`). Ob RE2s FUN_8004c1bc einen Schritt
   > Wanddicke ebenso durchlaesst, ist nicht belegt. Naechster Weg: FUN_8004c1bc (Zellentest Punkt oder Strecke)
   disassemblieren und gegen collision_constrain_impl stellen. Deshalb nicht im Riegel.
2. **RE1.5-Flavor Elza laedt die Leon-Bank.** Der Port liest nur `EMD/CDEMD0.EMS` (main.c `pc_read_shared` ~Z.773);
   das Original hat fuer Elza `CDEMD1.EMS` mit eigener Aktions-/Opferbank (2.3; mit der Leon-Bank liegt die
   Hand bei Elza 229..284 ueber ihrer Schulter statt 20..65). Naechster Weg: die Datei-Id-Auswahl der EMS im
   RE1.5-Lader finden (wie die PLD-Tabelle @0x80073f70 je DAT_800aca5c) und Elza im RE1.5-Flavor CDEMD1 geben.
3. **RE2-Claire-Daten fehlen.** Der Opfer-Bezug fuer Elza traegt den RE2-Leon-Kontakt auf ihre Schultern ueber
   (Port-Konstrukt, 3). Mit einer RE2-Claire-Disc (CDEMD1) liesse sich das gegen das Original messen.
4. **Reichweite.** Biss-Bilder 0..5 und 31..37: die RE2-Hand liegt bis 133 jenseits der RE1.5-Armlaenge; der Arm
   ist dort gestreckt (gemessen im Echtlauf A). Die Kopfhoehe des Hybrid-Zombies liegt ~200 unter RE2 (Mund an
   Leons Schulter statt am Hals). Eine RE2-Proportion im Griff waere eine Darstellungsentscheidung (nicht gebaut).
5. **Waffe im Griff.** Im RE2-Original haelt Leon im Griff seine Waffe sichtbar (re2_original_hinten_biss.png), im
   Port ist sie im Opfer-Zustand weg — beobachtet, nicht Teil dieser Spur.

## 7. Fuer den Nutzer

Der Arm fehlte nicht wirklich — er steckte in Leon. Unsere Zombies haben die Griff-Animationen aus RE2, aber die
etwas kleineren RE1.5-Koerper. Dadurch landete die Hand, die im Original UEBER Leons Schulter haengt, rund 20 cm
tiefer — unter der Schulter, und der Unterarm verschwand in Leons Ruecken oder Brust. Jetzt legt der Port die Hand
in jedem Griffbild auf genau die Stelle, an der sie im RE2-Original liegt (gemessen am laufenden RE2), der Arm
beugt sich mit seinen eigenen Laengen dorthin. Bei Elza liegt die Hand auf IHRER Schulter, nicht darueber.
Zweitens: in manchen Raeumen (z.B. dem Raum mit dem Schreibtisch, ROOM1010) stand Leon waehrend des ganzen Griffs
an einer falschen Stelle — der Zombie biss ins Leere. Das ist ebenfalls behoben (wie im Original: eine
Wandpruefung je Bild ab dem Platz, an dem Leon beim Zupacken stand).
Selbst pruefen: `RE15_BISSARM_LOG=bissarm.log` schreibt je Griffbild, wo die Hand liegt und wo sie im Original
laege. Keine neuen Spieldateien, nichts am Paket-Gate.
