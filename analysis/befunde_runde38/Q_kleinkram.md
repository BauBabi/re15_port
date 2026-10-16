# Runde 38 — Spur Q "kleinkram" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r38_kleinkram` (Zweig `r38/kleinkram`, Basis 1bcbd0ff). Wortlaut: `AUFTRAG.md` (massgeblich),
Zuteilung: `VERTRAG.md` Spur Q (Ressourcen §1.2: 1151/30E0/30E1 nur vorhandene Nachrichten, keine neuen Ids).

## Punkte (Wortlaut des Nutzers)
1. "Das öffnen des Player Inventories hat keinen Sounds, wenn Resident Evil 2 dort Sound hat, möchte ich auch Sound haben."
2. "In Elzas Szenario bei ROOM 1151 kommt - wenn man Irons anklickt erst - he already passed - und dann beugt sie sich hin
   und es kommt "I'll be fine, just worry About yourself for now". Ist natürlich quatsch … sie soll sich nur hinbeugen, und
   es soll stehen "he already passed"..."
3. "In Room 30E0 soll beim aufnehmen des Flammenwerfers nicht mehr kommen - not available in the preview, sondern einfach
   der Flammenwerfer aufgenommen werden."

## Stand
- 2026-10-16: Dossier angelegt, Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE gelesen.
- 2026-10-16: Bau des Basisstands 1bcbd0ff im Baum (local_build.sh configure + build, EXIT 0). Messkopie
  `re15_pc_r38q.exe` neben der exe, Arbeitsordner im Scratchpad (nie im Baum).

## 1. Messung vorher (Stand 1bcbd0ff, echte exe)

### P1 Inventar-Ton
* Lauf a/b: RE15_DEBUG_JUMP=1000@250, Leon (21850,-13400) rot 2048, RE15_AUDIO_CAP_SYNC=cap.raw, RE15_SE_DEBUG=1;
  a mit `RE15_PRESS=start@400,start@500` (Inventar auf/zu, debug.log `[inv] Sicherung im Statusschirm ...` nach F390,
  walk-Zeilen F420..F480 eingefroren), b ohne Druck, beide bis `EXIT_AT=600#1000`.
* Ergebnis: beide Mitschnitte 854 Bilder, **0 abweichende Bilder** (bytegleich) — Oeffnen UND Schliessen sind stumm.
  ROOM1000 ist ab der Sprungstelle still (Spitzenwert 0 ab Mitschnitt-Bild 314), ein SE waere sichtbar. Kein
  `[se] Stimme`-Eintrag im debug.log.
* Ursache im Code: der einzige Oeffnen-Ton steht im DEBUG-Umschalter `re15_menu_toggle` (menu_common.c, d4b41678
  2026-08-31 "Inventar-OEFFNEN: Auswahl-Ton"); der echte Weg START -> `re15_menu_start_poll` -> Stufe 1..2 spielt
  keinen. Tab-Auswahl-Abbruch (START/Kreuz) ist RE1.5-stumm ("No SE", @0x800499d0-a18).

### P2 Elza ROOM1151 Irons
* Lauf: Elza-Spielstand (probe_r36_elza_karte) ROOM1151 bei der Gespraechslage aus sub03 (@0x00ECE Pos_set
  (-20538,-25147), @0x00ED6 Dir_set 1500), CONTINUE, `RE15_PRESS=square@60,...@120,...` = echter Aktionsweg.
* state.log: F60..F120 `msg(a=1 fsm=1 id=20)` ("He has already passed..."), nach dem Wegdruecken ab ~F202
  `msg(a=1 fsm=1 id=2)` (99 Bilder) = "Irons: I'll be fine. Just worry about yourself for now." — genau der
  Nutzerbefund. SCD-Spur: F122 slot=1 @0x0E9E Cmp faellt durch -> @0x0EA4 Ck(3,0x9D) -> @0x0EA8 Set -> @0x0EAC
  Evt_exec sub03 -> @0x0EC2.. (Pos/Dir/Plc_neck/Plc_motion(1,0x0B) = Hinbeugen, Cut 7, Message_on 2).
* Warum der Runde-36-Riegel das nie sah: integration/unit_r36_elza_irons feuern Slot 2 per RE15_FIRE_AOT /
  re15_aot_fire_slot — das umgeht den Vorwaerts-Stempel des Scans (s. Abschnitt 2).
* Regression? Nein: Mechanismus seit dem Runde-36-Bau 3eaf631d unveraendert (Stempel aot_common.c seit 289823a7
  2026-07-11 bzw. fa0d53a1 2026-07-20, Frame-Ende-Wisch scd_vm.c seit 2026-08-08); der Fehler ist der
  Runde-36-Entwurf selbst ("work_vars[0] wird nie 2" stimmt nicht). Verursachender Commit: 3eaf631d.

### P3 ROOM30E0 Flammenwerfer
* Lauf: RE15_DEBUG_JUMP=30E0@250, Leon (-25000,-26400) rot 2048 vor dem Panel (Slot 3 @0x00A5A), Eingabe
  `A0.1,W0.5,L0.8,W0.5,A0.1,W1,R0.13,W0.5,A0.1,W3` ab Bild 300 (BASIS spiel). Echter Ablauf: Viereck am Panel ->
  sub09 (Cut 2, Modus (5,0..4,6)); LINKS 24 Bilder = sub08 @0x1034 (x -200 je Bild); Viereck -> sub10 (Cut 3);
  RECHTS 4 Bilder = sub07 @0x1028; Viereck -> sub04 -> F410 `@0x0FEE op=0x2B` -> `[msg] room=30e0 id=15`,
  state.log `cam=3 msg(a=1 fsm=1 id=15)`. Framedump F460: Fernansicht des offenen Spinds mit dem Flammenwerfer im
  Gestell, Fadenkreuz darauf, Text "There's a Flamethrow..." (Schreibmaschine).
* ROOM30E1 (Elza) hat KEINEN Flammenwerfer: Zensus aller RDT-Nachrichten — der einzige Waffen-Preview-Text ist
  ROOM30E0 msg 15 @0x14D2 (die uebrigen 16 Preview-Texte sind "Itembox ..."); ROOM30E1 = John-Szene ohne Panel.

## 2. RE-Belege

### 2.1 P1 — RE2 hat einen Oeffnen- und einen Schliessen-Ton (RE2 Retail = Ziel; RE1.5 stumm)
RE2 PSX.EXE (info/re2leon/PSX.EXE, eigener Disasm re2_disasm.py):
* Annahme der Status-Anforderung in der Spielschleife: @0x80026498 `lw v0,0x800ce310` (Tasten-Flanken),
  @0x800264a0 `andi v0,v0,0x4000` (Statustaste), @0x800264c8 `sw v0,0x800cfbd8` (|= 0x8000), @0x800264dc
  `andi v0,v1,0x8000` / @0x800264e8 `sw v0,0(a0)` (|= 0x40, angenommen), @0x80026518 `lbu v1,0x800d5c00`
  (Ziel-Schirm), @0x80026528 `beq v1,v0(=2),0x80026738`, **@0x8002652c `lui a0,0x406` / @0x80026530
  `jal 0x8005ba28` / @0x80026534 `addu a1,zero,zero`** = Se_on(Bank 4 CORE, Satz 6, ohne Position).
* Wege, deren Handler den Schirm selbst anfordern (`sb ..,-3256(at)` = 0x800df348: Item-Aufnahme @0x80051904,
  Box @0x80051de0, Status aus der Zone @0x8005201c, Kartenhinweis @0x800591e8), lesen das Byte @0x800263fc und
  springen @0x80026404 `bne v0,zero,0x80026540` ueber den Block -> dort KEIN Ton. Port: nur der START-Weg.
* Status-Task 0x800689bc (Start @0x80025b90 `addiu a1,a1,-30276`), Schirm-Tabelle @0x800a936c (Tripel
  Init/Haupt/Ende), Status-Init 0x8006a574: kein `jal 0x8005ba28` (Zensus 0x8006a574..0x8006a76c).
* Schliessen in der Kommandoleiste (Zustand 2 = 0x8006a7f0, Tabelle @0x800a9aac; Zustand 0 = Abblende
  0x80068f08 `jal 0x8002c1a0` (512,6144,7,1)): Abbruch @0x8006a928 `andi 0x2000` -> @0x8006a930 `lui a0,0x405` /
  @0x8006a934 `jal 0x8005ba28` / @0x8006a93c `sb zero,1(s0)`; Statustaste @0x8006a94c `andi 0x4000` ->
  @0x8006a968 `lui a0,0x405` / @0x8006a96c `jal` / @0x8006a978 `sb zero,0x800d5bf1`; EXIT (Cursor 3)
  @0x8006a8cc `beq v1,3` mit Verzoegerungsplatz @0x8006a8d4 `lui a0,0x405` -> @0x8006a910 `sb zero,1(s0)` /
  @0x8006a918 `jal 0x8005ba28`. -> Satz 5 auf allen drei Schliess-Wegen.
* Bank: CORE00.EDH RE1.5 == RE2-Leon bytegleich (analysis/re2_ermittlung_2026-08-30/STRAENGE_ROH.md: 3176 B,
  sha256 7948e41e..; Satz 5 `00 00 63 01`, Satz 6 `00 00 73 01`). Port spielt Bank 4 ueber re15_audio_core_se.
* RE1.5 selbst: Reiterwahl-Abbruch @0x800499d0-a18 ohne Se_on (41 Se_on-Stellen EXE-weit, keine in
  0x8004974c-0x80049a58); RE1.5 hat keinen Oeffnen-Ton -> RE2 gilt (Beta -> Retail).

### 2.2 P2 — warum nach Text 20 doch sub03 lief (RE1.5 PSX.EXE)
* ROOM1151 (Dump scd_dump_room.py): main00 @0x00D92 `2c 02 05 31 .. 1c a8 b4 97 a4 06 78 05 00 00 00 00 00 00`
  Slot 2 sce 5 Flags 0x31 (Vorwaerts-Test + Aktion); sub01 @0x00E9A `06 00 14 00` / @0x00E9E `23 00 00 00 02 00`
  Cmp(var00==2) / @0x00EA4 `21 03 9d 00` Ck(3,0x9D)==0 / @0x00EA8 `22 03 9d 01` / @0x00EAC `04 ff 18 03` sub03;
  sub03 @0x00EC2..@0x00F72 (Set(2,7)/(1,27), Pos (-20538,-25147) @0x00ECE, Dir 1500 @0x00ED6, Sleep 10,
  Plc_neck @0x00EE2, **Plc_motion(1,0x0B) @0x00EEC = Hinbeugen**, Sleep 40, Cut_chg 7 @0x00EF4, Sleep 30,
  **Message_on 2 @0x00EFA `2b 02 00 00`**, Work_set(2,0) @0x00EFE (= Irons), Plc_motion(0,6) @0x00F02/@0x00F0A,
  Plc_motion(0,3) @0x00F12, Plc_flg 4 @0x00F16, Sleeps 50/50/20, Work_set(1,0), Plc_neck x3, Cut_old @0x00F4C,
  Plc_motion(1,0x0B)+Plc_flg 0x80 @0x00F4E/@0x00F52 (Aufrichten), Plc_ret @0x00F68, Evt_end @0x00F72).
* Stempel des Scans FUN_80042bac fuer JEDE sce, vor dem Sprung: Vorwaerts-Test @0x80042ef8 `andi v0,v1,0x20`,
  Treffer @0x80042f34 `addiu v0,s2,-1` / **@0x80042f3c `sh v0,0x800b0fd0` (work_vars[0] = Slot)**, erst danach
  @0x80042f48 `lbu v0,0(s0)` (sce) und der Handler-Sprung @0x80042f74-84. Ein Text-Platz (sce 1) stempelt also
  genauso var00 = 2.
* Wisch work_vars := -1 nur im VM-Lauf: FUN_8003ebf4 (@0x8003ebfc..ec14), gerufen am Ende von FUN_8003f0a0; der
  Bild-Treiber @0x8003f038 ueberspringt den ganzen VM-Lauf bei `g_pauseflags & 0x02000000` (@0x8003f040 `lw`,
  @0x8003f044 `lui v1,0x200`, @0x8003f04c `bne v0,zero,0x8003f090`). Text 20 mit Maske 0xffff haelt die VM an ->
  var00 = 2 ueberlebt bis zum Schliessen -> sub01 -> sub03 (Hinbeugen + msg 2). Genau der Nutzerbefund und die
  Messung (F122 Cmp faellt durch).
* (3,0x9D) Zensus 240 RDTs: nur ROOM1150/1151 sub01 (Ck @0x0EC6/@0x0EA4, Set @0x0ECA/@0x0EA8).

### 2.3 P3 — Aufnahme wie jede Waffe (RE1.5 PSX.EXE + RDTs)
* Item-Handler sce 9 @0x80043328: @0x8004332c `lbu v0,0x80072d3b` / @0x80043334 `bne` (nur aus Zustand 0),
  @0x8004334c `sb 1,0x80072d3b`, @0x8004335c `sb v0,0x800afbb6` (Typ), @0x80043364 `sw a0,0x800aca30` (Satz)
  -> Aufnahme-FSM FUN_8001db28 (Port re15_item_modal_start/tick: "Will you take the ...", Yes/No, voll).
* Flammenwerfer = Item 0x0E: Waffen-Record @0x80074e50 `64 00 00 00 a8 4c 07 80 02 01 00 00` (Magazin 100,
  Munitions-Satz 0x80074ca8), Bild ITPS.ITP @0x2A000 (gerendert: Flammenwerfer), Name "FLAMETHROWER", 2-Zellen-
  Waffe (Modal @0x8001df40 `addiu -14; sltiu 6`).
* Menge: Zensus aller Item_aot_set (0x50) mit Waffen-Typ in 240 RDTs — 13 Saetze / 7 Typen, ALLE Menge ==
  Magazin des Records (0x04 15, 0x05 15, 0x07 6, 0x08 7, 0x0c 100, 0x0d 12, 0x13 100). -> 0x0E Menge 100.
* ROOM30E0 sub04: @0x00FE0 `46 03 00 ..` Aot_reset(3,0), @0x00FEA Set(2,7), **@0x00FEE `2b 0f ff ff`
  Message_on 15** (Text @0x14D2), @0x00FF2 Evt_next, @0x00FF4 Cut_chg 2, Sleep 30, **@0x00FFA `22 03 9c 01`
  Set(3,0x9C)**, Set(2,7,0), Cut_chg 1, Cut_auto. main00 @0x00A52 `06 00 64 00` / @0x00A56 `21 03 9c 00`
  Ck(3,0x9C)==0 -> Panel Slot 3 @0x00A5A (`ff 00 18 09 00 00` -> sub09), Cursor-Platz 4/5, Objekt 0.
  (3,0x9C) Zensus: nur ROOM30E0 (Ck @0x0A56, Set @0x0FFA) = das "genommen"-Bit dieses Flammenwerfers.
* Nicht genommen ("No" bzw. voll): Modal Zustand 7 `bltz` @0x8001e054 / `andi 0x1` @0x8001e068 -> Zustand 8,
  nichts eingefuegt, Welt-Item bleibt (Item-Zone bleibt scharf). Gegenstueck hier: Panel bleibt erreichbar.

## 3. Umsetzung
| Datei | Inhalt |
|---|---|
| include/re15_kleinkram_r38.h (NEU) | Belege + Konstanten: Inventar-Ton 6/5, 1151-Anker, 30E0 Typ 0x0E Menge 100, Panel-Satz |
| engine/src/kleinkram_r38.c (NEU) | Inventar-Ton (Zaehler + Messzeile); Anker-Tabelle (Raum, Datei-Offset, 8-Byte-Signatur) 1151 @0x00EFA/F02/F0A/F12/F16/F72, 30E0 @0x00FEE/@0x00FFA; Opcode-Haken |
| engine/src/menu_common.c (3 Haken) | START-Annahme -> Satz 6; Reiterwahl-Abbruch/START -> Satz 5; EXIT -> Satz 5 (statt 6) |
| engine/src/scd_vm.c (3 Zeilen) | include, Raum-Scan in scd_register_current_rdt, Opcode-Haken neben dem Codepanel-Haken |
| engine/src/elza_r36.c (1 Bedingung) | Runde-36-Umbau von Slot 2 beim Raumstart nur noch, wenn (3,0x9D) schon steht |

Keine neuen Nachrichten-Ids (VERTRAG §1.2: 1151 Text 20 aus Runde 36), keine Bank-9-Bits, keine Assets.

## 4. Messung nachher (echte exe, Messkopie re15_pc_r38q.exe, Stand c6389db5 + Messzeilen-Fix)
* P1 (gleiche Laeufe a/b wie vorher): debug.log `[r38q] F400 Inventar-Ton Satz 6 (Oeffnen)`, `[se] Stimme: se=6
  layer=0 vag=3 note=64`, `[r38q] F500 Inventar-Ton Satz 5 (Abbruch)`, `[se] Stimme: se=5 ... vag=2 note=63`.
  Mitschnitt a gegen b: 14 abweichende Bilder in genau zwei Laeufen [654..660] und [754..760] (= Raum-Bild 400 und
  500, Versatz 254), Spitzen 5488/5464/4563/2694/5496/3907/1772 bzw. 1232..4863, davor/danach 0.
* P2 (gleicher Spielstand-Lauf, Drucke 60/200/450/500, Framedump 180): F0..F140 kein Text, Elza `mo=11`
  (Hinbeugen); `[r38q] F141 ROOM1151 sub03 @0x00EFA Message_on 2 -> Text 20`; state.log 60 Bilder
  `msg(a=1 fsm=1 id=20)` ab F141 (bis Druck F200); Gesten @0x00F02/@0x00F0A/@0x00F12/@0x00F16 entfallen;
  `F401 sub03 Ende: Slot 2 -> Text-Platz 20, (3,0x9D)=1`; zweites Anklicken F450 -> 51 Bilder Text 20, keine Szene;
  **msg 2 in keinem Bild**; Irons 561/561 Bilder `[1 t=45 st=4 ss1=0 ss2=2 ss3=0 g=00 mo=2 af=89` (tot).
  Framedump F180 (Cut 7): Irons tot auf der Liege, Elza zu ihm gebeugt, Untertitel "He has already passe..".
* P3 Ja (Eingabe wie vorher + Viereck ~F497): `[r38q] F410 ... Message_on 15 -> Aufnahme-Modal Item 0x0E x100`,
  state.log kein `id=15`; Framedump F500: "Will you take the Flamethrower? > Yes  No" mit dem ITPS-Bild ueber der
  Spind-Ansicht; `F503 Flammenwerfer genommen: Zellen 0 -> 2`; danach Original weiter (Cut 2 30 Bilder, Cut 1).
* P3 Nein (RECHTS ~F527, Viereck ~F537): `F562 Flammenwerfer NICHT genommen (No/voll): Panel Slot 3 wieder scharf,
  Zellen 0`, `F593 sub04 @0x00FFA Set(3,0x9C) entfaellt`; erneutes Viereck am Panel -> sub09 startet wieder
  (`thread start ... first_op=0x29`, Cut 2) — der Flammenwerfer bleibt erreichbar.
