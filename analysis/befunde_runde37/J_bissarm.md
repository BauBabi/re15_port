# Runde 37 — Spur J "bissarm" (2026-10-12)

Zweig `r37/bissarm`, Basis master 419380dc. Dossier fortlaufend (Sitzungslimit-Schutz: nach jedem Schritt committet).

## Auftrag (Wortlaut, AUFTRAG.md Nachtrag 6)

> Ja, wie ich gerade gesehen habe fehlt in unseren Port teilweise noch der dranhängende Arm wenn man vom Zombie gebissen wird.

Lesart (AUFTRAG.md, beide pruefen, keine F9-Marke): (1) der Arm des Zombies, der beim Biss/Griff an Leon haengt
(Griff von vorn/hinten, Kriecher am Bein; Leon UND Elza), fehlt in manchen Faellen/Bildern; (2) der angeschossene,
noch "dranhaengende" (baumelnde) Arm eines Zombies fehlt beim Biss. Erst messen, welcher Fall auftritt, dann wie RE2.

## 0. Stand

- [ ] 1 Messung vorher (Port, alle Biss-Varianten, je Bild Teile-Sichtbarkeit)
- [ ] 2 RE-Belege (RE2 Griff/Biss, Teile-Flags)
- [ ] 3 Umsetzung
- [ ] 4 Messung nachher
- [ ] 5 Tests

## 1. Messung vorher (Bau 419380dc, eigene Messkopie `re15_pc_j37mess.exe`, RE2-Standardgehirn + RE2-Modelle)

Werkzeug: Spielstand per `test_r36_kleidung karte` (Slot 0) -> CONTINUE; `RE15_STATE_LOG`, `RE15_FRAMEDUMP`
(Serien), `RE15_FORCE_CUT` (nur zum Hinschauen, Kamera festnageln), `RE15_ENEMY_DBG`, `RE15_RE2_TRACE`.
Laeufe unter `%TEMP%/.../scratchpad/J37/` (unversioniert; Zahlen hier zitiert).

### 1.1 ROOM1140 (Briefing-Raum, 5 Zombies) — Leon (-1400,-19600) Cut 1
- `enemy_dbg.log`: `RE2 EM016/EM010/EM011: 17 Meshes, 15 Bones, 31 Clips`, gezeichnet je `15 Teile`.
- Griff Slot 5 (Typ 0x11) F58..F165 (state.log `ss1=3`, `gr=1`), danach Slot 2 F166..F271 (Tod F227).
  Leons Lage waehrend der Griffe plausibel neben dem Greifer (z.B. F79 PL(-791,-19733), Greifer (-297,-19708)).
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
