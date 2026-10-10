# Runde 36 — Spur J "szenenleon": UNABHAENGIGE ABNAHME 0

Pruefer: Abnahme-Agent 0 (glaubt nur, was er selbst am gebauten Stand gemessen hat).
Baum: `.claude/worktrees/r36_szenenleon`, Zweig `r36/szenenleon`, HEAD b80c88be (Basis master 78a4a748).
Massstab: AUFTRAG.md (Wortlaut + Nachtrag 14:00 + Nachtrag "abends" fuer Spur J Punkt 2, master 836023f5 23:40 —
nach dem Bau-Ende, aber ausdruecklich "gilt fuer Bau, Nachbesserung UND Abnahme"), VERTRAG.md, CLAUDE.md (RE-Gate).

STAND: ABGESCHLOSSEN 2026-10-11.

## Ergebnis: NICHT BESTANDEN

| Punkt | Urteil | Kern |
|---|---|---|
| 1 Container-Schieben 11B0 | erfuellt | Echtweg 11C0->11B0: Leon + Marvin Sub 0x13 synchron (Clip 0x11/0x12/0x11-rueck), +1536/+1517, Container wandert, Schacht frei |
| 2 Erweiterung Werkstatt-Tuer | **teilweise** | Ablauf/Kendo-Clip/Kendo-Ton/Sperre/Elzas Text+Ton/Schlusssatz da; Tuer bleibt im Bild OFFEN (BSS Cut 8/7), Elza sieht sie geschlossen (Cut 0/2); Nachtrag 23:40 (erst schliessen, dann abschliessen) fehlt |
| 3 10F0-Abgang | erfuellt | Echtweg 10D0->10F0: cam=2 bis zum Ende, Parken F2670/F2687 bei 0 geaenderten Pixeln, ein Tuer-zu-Ton danach |
| 4 Irons-Kopf nach dem Tod | erfuellt | Echte Todesszene: Flags 0x00 -> 0x12 beim Armfall F894, Akku konstant; tot A/B 0 px, lebend A/B 170 px am Kopf |

Gates: RE-Gate haelt (9 Stichproben selbst disassembliert/gelesen), Vertrag haelt, gezielte Tests 151/151 gruen.
Maengel M1-M4 (§6): Tuerbild offen, Nachtrag-Schliessen fehlt, falsche Tatsachenbehauptung in der
Entscheidungsfrage, kein Riegel fuer das Tuerbild.

## 0. Bau

- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`
- `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`
- Quellen = HEAD (`git status --short re15_port` leer); exe 23:09:12 juenger als die letzte Quelle
  (szenenleon_r36.c 23:08:43), test_r36_szenenleon.exe 23:09:42 juenger als test_r36_szenenleon.c 23:09:04.
- Messkopie: `re15_port/build/platform/pc/re15_pc_r36j_abn0.exe`, Arbeitsordner je Lauf im Notizblock
  (debug.log entsteht im Arbeitsordner, `freopen("debug.log")` main.c:3207).

Mess-Skript (Notizblock, nicht im Repo): `J_abn0/lauf.sh <ziel> <sek> KEY=VAL...` = Titel-Autovorlauf
(RE15_TITLE_SHOT_AF=2), RE15_WINDOW_SCALE=1, SDL_AUDIODRIVER=dummy, SDL_RENDER_DRIVER=direct3d11 (beschleunigt,
KEIN Software-Renderer), Arbeitsordner = Zielordner. Notizblock = `%TEMP%/claude/c--workspace-git-reAi-v2/
c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/J_abn0/` (Bilder/Logs dort; Befehle unten wiederholbar).

## 1. Punkt 1 — ROOM11B0 Container-Schieben mit Marvin

Wortlaut: "In der Cutscene in ROOM 11B0 mit Leon, Ada und Marvin klappt das mit der Animation und dem gemeinsamen
Wegschieben des Containers mit Marvin noch nicht."

**Messung ueber den ECHTEN Weg (Tuer ROOM11C0 -> ROOM11B0)**, Lauf a1/a2:
`lauf.sh a2_11b0_bilder 420 RE15_DEBUG_JUMP=11c0@250 RE15_SET_FLAG=3:0x43,3:0x82,4:0xf3,4:0x40
RE15_FIRE_AOT=0@60#11c0 RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_STATE_LOG=... RE15_FRAMEDUMP=1600-2760/6:... RE15_EXIT_AT=30#1260`
(ROOM11C0 Door_aot_set Slot 0 @0x01712 `... 00 1b 01` -> ROOM11B0; sub01 @0x011C6/@0x011CE startet sub06 bei
(3,0x83)==0 && (3,0x43)==1 — kein Sprung in die Szene, der Raum wird durch die Tuer betreten.)

- debug.log: `[fire-aot] slot=0 at F60 (Raum 11C0)` -> `[room] PC loaded room11b0.rdt` -> `[scd F1592] Cut_chg(6)` ->
  `[szl-r36] ROOM11B0 Spieler: Plc_dest Modus 0x13 (Schub-Sub @0x80052864)` + `Plc_dest(slot=1 mode=0x13 ...)` (Marvin)
  -> `[szl-r36] ROOM11B0 Spieler: Schub-Sub zu Ende -> Sub 6 (Event-Reach)`.
- state.log (Leon = PL, Marvin = Aktor 1 Typ 0x40):
  ```
  F1640 Leon(-1651,-21535) mo=1  | Marvin ss1=6  ss2=2 mo=1  @(852,-21516)
  F1646 Leon(-1651,-21535) mo=17 | Marvin ss1=19 ss2=3 mo=17 @(852,-21516)     <- beide Sub 0x13, Clip 0x11
  F1658 Leon(-1652,-21507) mo=18 | Marvin ss1=19 ss2=5 mo=18 @(851,-21488)     <- beide Clip 0x12 (Schub)
  F1700 Leon(-1643,-20511) mo=18 | Marvin ss1=19 ss2=5 mo=18 @(866,-20511)     <- gleichauf
  F1724 Leon(-1639,-19999) mo=18 | Marvin ss1=19 ss2=5 mo=18 @(873,-19999)
  F1730 Leon(-1639,-19999) mo=17 | Marvin ss1=19 ss2=6 mo=17 @(873,-19999)     <- 0x11 rueckwaerts
  F1742 Leon(-1639,-19999) mo=1  | Marvin ss1=6  ss2=1 mo=1                     <- Sub 6
  ```
  Leon +1536 in z, Marvin +1517, im selben Takt (ss2 3 -> 5 -> 6 synchron).
- Bilder (Kontaktbogen `p1_schub_sheet.png`, Zoom `p1_schub_zoom.png` F1660/F1684/F1708/F1744): F1630/1642 beide
  stehen seitlich am Container; ab F1654 beide mit dem Ruecken zur Kamera, Haende am Container (Marvin an der rechten
  Kante), Schiebehaltung; der Container wandert nach hinten, ab F1738 liegt der Schachtdeckel frei. Keine Durchdringung
  sichtbar.
- Riegel (selbst gefahren): `unit_r36_szenenleon_schub` -> `Leon 0x11=1 0x12=1 rueck=1 Ende B1552 z -21535 -> -19999 |
  Marvin 0x11=1 0x12=1 Ende B1553 | Container z -20918 -> -19549 Flags waehrend 0x0201 Ende 0x0081 | snd0 0 -> 8`.
- RE (selbst disassembliert, info/Re1.5/PSX.EXE): Tabellen 0x80073e30[19] = 0x80076ca0[19] = 0x80052864; Sub
  @0x80052864: +0x94=5 @0x800528bc/c0, anim_set(+0x170,+0x174) @0x8005294c, Rastung `andi 0x3e0` @0x800529a0,
  +0x94=0x11 @0x800529cc, +0x94=0x12 @0x80052a8c, `lui a0,0x207`/`jal 0x80045024` @0x80052acc/d8 bei +0x95==1,
  `andi v0,v0,0x2000` @0x80052b18 -> `jal 0x800369f8`, `jal 0x8004efe4` (0x800b1028, +0x1c3) @0x80052b44 -> +0x6=6,
  anim_set a2=1 @0x80052b98, +0x5=6 @0x80052be0 — alles wie im Code zitiert. Objekt-Pass @0x8002bec0 `andi v0,v0,0x200`
  -> @0x8002bed4 `jal 0x8002cabc` mit a0=player (0x800aca54), a2=1 @0x8002bed8. Member_set Id 6 = Tabelle
  0x80010c8c[6] -> 0x800411c4 `j 0x80041230` / 0x800411c8 `sw a2,0(a0)`.
- Erklaert der Fix den Befund? Ja: vorher kannte der Port Modus 0x13 nicht (Plc_dest -> "POSE/EVENT, kein Walk",
  Marvin re15_npc_sub_idle) und Member_set 6 war ein No-Op — genau die drei Stellen, die jetzt belegt nachgebaut sind.
- Rest-Abweichung: Container +1369 (Riegel) gegen den Raum-Record +1550 (main00 @0x010C4 -20918 / @0x010EA -19368);
  Leon haelt 167 vor der Kontaktlinie an. Ohne Folge (die Szene verlaesst den Raum durch den Schacht, beim
  Wiederbetreten gilt der Record). Nicht weiter geprueft, ob der Ankunftsradius 300 (LAB_80030e5c) die 181 im
  Original genauso erzeugt.

**Urteil Punkt 1: erfuellt.**

## 2. Punkt 2 — Erweiterung: Marvin schliesst die Werkstatt-Tuer ab

Wortlaut (Nachtrag 14:00 massgeblich): Marvin "Hold on" -> rennt zur Innenseite der Werkstatt-Tuer in 11B0 ->
Zuschliess-Animation + Zuschliess-Ton (RE2 Robert Kendo, Spielanfang) -> "Die Tuer soll dann geschlossen bleiben mit
der gleichen geschlossen Meldung wie in Elzas Szenario" -> zurueck zu Leon/Ada -> "I just want to make sure we don't
have any unwanted followers." -> "Wenn man die Tuer anklickt, soll der gleiche Text kommen, wie in Elzas Szenario
dann, wo die Tuer ja geschlossen ist." Parkplatz-Tuer NICHT.

### 2.1 Ablauf (gleicher Echtweg-Lauf a2)
- debug.log: `[szl-r36] ROOM11B0 sub06 @0x017EA Aot_on 5 -> erst die Erweiterung` -> `[msg] room=11b0 id=20` ->
  `[scd F2238] Cut_chg(10)` -> `[scd F2321] Cut_chg(8)` -> `[se] Se_on bank=0x3A id=10 -> RE2-Schlossbank SCHLOSS1010`
  + `[se] Stimme: se=10 layer=0 vag=3 note=66 ... (11025 Hz)` -> `[scd F2507] Cut_chg(10)` -> `[scd F2592] Cut_chg(6)`
  -> `[msg] room=11b0 id=21` -> `[room] PC loaded room1260.rdt`. Kein `[schlossse] ... fehlt`.
- state.log Marvin: an der Tuer (-20745,-16468) Blick r2119 (West, zur Tuer); F2384..F2402 Clip 15 vorwaerts (af 2->19),
  F2414..F2432 Clip 15 mit Plc_flg 0x80, F2450 Lauf zurueck.
- Bilder `p2_erw_sheet1.png`/`p2_erw_sheet2.png`: "Marvin: Hold on." (F2158..F2230, Geste, dann Lauf an Ada vorbei),
  Werkstatt Cut 10, Tuer Cut 8; Zoom `p2_abschliessen_zoom.png` F2374..F2452: linker Arm zur Tuer vor, zurueck,
  Arm unten — Rueckwaerts-Abspielen wirkt (Bild, nicht nur Zustand). Rueckweg, dann zweizeilig "Marvin: I just want to
  make sure / we don't have any unwanted followers." vor Leon und Ada (F2662..F2734), danach Schacht -> ROOM1260.
- Belege selbst nachgeprueft: RE2 `info/re2leon/PL0/RDT/ROOM1010.RDT` @0x01C18 `3f 00 00 00`, @0x01C1C `09 0a 14 00`,
  @0x01C20 `36 02 0a 01 00 00 d8 c3 f8 f8 c6 c8`, @0x01C30 `3f 00 00 c0`, `5b 0a`, `66 00` (Kendo-Ablauf, Sce_em_set
  @0x01A94 Typ 0x48); RDT+0x08/0x0C/0x10 = 0xA044/0xA104/0xAF24, EDT @+0x28 = `00 00 83 01`;
  `shared_assets/RE2/SCHLOSS1010.EDT/.VH/.VB` bytegleich zu diesen RDT-Bereichen (eigener Vergleich + 
  `schloss1010_extrakt.py --pruefen` rc=0); `rbj_vgl.py`: RE2 1010 Clip 0 == ROOM11B0 RBJ Record 0 UND 1 Clip 15
  (20 Bilder, Bildwoerter + Keyframe-Bytes gleich). ROOM11B1 msg 12 @0x179F = "The door is locked." (rdt_msgdump).
- Riegel `unit_r36_szenenleon_erweiterung`: `Umleitung B1939 msg20 B1939 Tuer B2182 (min 172) Se_on B2211 bank 0x3A satz
  10 (Marvin Clip 15) Zelle zu B2256 Platz 8 B2256 msg21 B2447 (Marvin 1152,-20190) Aot_on B2557 | Cuts: 1 7 8 3 4 5 6
  10 8 10 6`.

### 2.2 Tuer danach (Text + Ton) — beide Seiten
- Ost: `lauf.sh d1_tuer_ost 150 RE15_DEBUG_JUMP=11b0@250 RE15_SET_FLAG=4:0xf3,3:0x43,3:0x82,3:0x83,4:0xcd,9:0x54
  RE15_PLAYER_POS=-20500,-16300,2048 RE15_TUERSE_LOG=... RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=300
  RE15_INPUT_SCRIPT=U1.5,W0.5,A0.2,W3 ...` -> `[szl-r36] ROOM11B0 Werkstatt-Tuer verschlossen (9,84): Ereignis 23`;
  Leon bleibt bei x -20922 stehen; F360 `msg(a=1 fsm=1 id=22)`; tuerse.log `F360 raum=11B1 nachricht=12 art=M weg=AOT
  satz=0(ZU_A) nr=1` (= Elzas Ton).
- West: gleich mit `RE15_PLAYER_POS=-23300,-16300,0` -> Leon bleibt bei x -22638 stehen; F360 msg 22; tuerse.log
  identisch. cam=7.
- Funktion (Sperre + Elzas Text + Elzas Ton von beiden Seiten): erfuellt.

### 2.3 ⛔ MANGEL: die Tuer bleibt im Bild OFFEN — Elza sieht sie GESCHLOSSEN; die Original-Daten haben das Bild
Der Bau-Agent schreibt (Dossier §2.3/OFFEN 1/"Entscheidungsfrage"): der Hintergrund zeige die Tuer offen, "genau so sieht
Elza sie", ein geschlossenes Tuerblatt gebe es "in keinem Original-Datensatz", es braeuchte ein neues 3D-Modell.
**Das ist falsch — gemessen:**
- Hintergruende mit dem Engine-Dekoder dekodiert: `build/tests/unit/probe_bg_dump.exe re15_port/shared_assets/PSX <aus> 1`
  -> `ROOM11B00..14.ppm` (Kontaktbogen `bg_11b_cuts.png`, Zoom `p2_tuer_cut2_vs_cut8.png`):
  **Cut 2 = Tuer GESCHLOSSEN, Cut 8 = Tuer OFFEN (Blatt aufgeschwungen); Cut 0 = GESCHLOSSEN (West-Ansicht),
  Cut 7 = OFFEN.** ROOM11B.BSS enthaelt beide Zustaende.
- Der Schalter ist `Cut_replace` und haengt am Tuerzustand: Leon sub00 @0x01184 `4b 00 07` / @0x01188 `4b 02 08`
  zusammen mit @0x0118C `37 03 0c 00` (Zelle auf); Elza sub00 @0x0110E `4b 00 07` / @0x01112 `4b 02 08` NUR im Zweig
  (3,0x83)==1 (Tuer offen, @0x010FA); im gesperrten Zweig (@0x0112A Text "The door is locked.") gibt es KEIN
  Cut_replace -> Elza sieht Cut 0/2 = geschlossene Tuer.
- Port, Elza ROOM11B1 gesperrt (`lauf.sh c2_11b1_west 120 RE15_GOTO_ROOM=11b1 RE15_SET_FLAG=4:0xf3 ...
  RE15_FRAMEDUMP=200-300/50:...`, cam=0): Tuer im Bild GESCHLOSSEN (`c2_elza_cut0.png`, deckungsgleich mit BSS Cut 0).
- Port, Leon nach Marvins Abschliessen: Erweiterung F2446 Cut 8 = OFFEN (`p2_tuer_cut2_vs_cut8.png` rechts); beim
  Anklicken danach Ost (Cut 8) und West (Cut 7) "The door is lo[cked.]" vor einer sichtbar OFFENEN Tuer
  (`d_tuer_ost_west.png`, links/Mitte; rechts Elzas geschlossene Tuer zum Vergleich).
- Folge: "Die Tuer soll dann geschlossen bleiben ... wie in Elzas Szenario" ist im Bild NICHT erfuellt, obwohl die
  Original-Form (Cut_replace zurueck auf 0/2, z.B. dieselben Saetze `4b 00 07`/`4b 02 08` erneut = Rueck-Tausch, plus
  Cut_chg auf den geschlossenen Cut) ohne neues Modell vorhanden ist. Die Entscheidungsfrage an den Nutzer beruht auf
  einer falschen Tatsachenbehauptung.

### 2.4 ⛔ Nachtrag des Nutzers (master 836023f5, 2026-10-10 23:40, NACH dem Bau-Ende 23:23) — nicht umgesetzt
AUFTRAG.md auf master (massgeblich) hat seit 836023f5 einen Nachtrag, "gilt fuer Bau, Nachbesserung UND Abnahme":
> bevor der tür schluss sound bei Marvin kommt und die abschließ animation bei der neuen Cutscene in 11B0, muss marvin
> zunächst die tür Schließen inkl. animation, sonst gibt es ja nichts abzuschließen.

Verlangte Folge: (1) Lauf zur offenen Tuer, (2) Marvin SCHLIESST die Tuer (Schliess-Animation, sichtbar, wie die Tuer
zugeht) + Tuer-zu-Ton (RE2-Tuerbank), (3) DANACH Abschliess-Animation + Abschliess-Ton (Kendo), (4) Tuer bleibt zu mit
Elzas Text, (5) zurueck + Satz; Abnahme: Ereignisfolge mit Bildnummern Schliessen < Tuer-zu-Ton < Abschliessen <
Abschliess-Ton + Framedump-Serie.
- Gemessen (Lauf a2): an der Tuer laeuft nur Kendos Folge — F2384 Clip 15 vor, `Se_on bank=0x3A id=10` (Abschliess-Ton),
  F2414 Clip 15 rueckwaerts, dann Zelle solid + Text-Platz. **Kein Schliessen (keine Schliess-Animation, kein
  Tuer-zu-Ton, kein Bildwechsel der Tuer)** — die Tuer steht vor, waehrend und nach dem Abschliessen offen im Bild
  (`p2_abschliessen_zoom.png`, `p2_tuer_cut2_vs_cut8.png`). Programm `gen/szl11b0_szene.inc` +0x005C..+0x0080 enthaelt
  genau ein Se_on (0x3A/0x0A), keinen RE2-Tuerbank-Ton.
- Die Original-Daten fuer das sichtbare Schliessen sind vorhanden (2.3): BSS Cut 8 (offen) -> Cut 2 (geschlossen),
  gekoppelt per Cut_replace 2<->8 / 0<->7 (11B0 sub00 @0x01184/@0x01188, 11B1 sub00 @0x0110E/@0x01112).

**Urteil Punkt 2: teilweise** (erfuellt: "Hold on.", Lauf zur Innenseite, Kendo-Clip, Kendo-Ton, Sperre, Elzas Text und
Ton von beiden Seiten, Rueckweg, Schlusssatz, Parkplatz-Tuer unberuehrt; NICHT erfuellt: Tuer im Bild geschlossen
(sie bleibt nach dem Abschliessen und bei jedem spaeteren Besuch offen gemalt, Elzas gesperrte Tuer ist geschlossen
gemalt) und — nach dem Nachtrag 23:40 — das vorausgehende Schliessen mit Animation und Tuer-zu-Ton).

## 3. Punkt 3 — ROOM10F0: Kamera bleibt auf Leon, kein Ploppen, nur Tuer-zu-Ton

Wortlaut: "In der neuen Cutscene in ROOM 10F0 verfolgt die Kamera wie Marvin und Ada den Raum verlassen. Sie
'ploppen' dann einfach weg. ... Bleibe lieber mit der Kamera wenn Marvin und Ada den Raum verlassen auf Leon fixiert.
und mache an der Stelle nur einen 'Tuer schliessen' Sound zum schluss."

**Messung ueber den ECHTEN Weg (Tuer ROOM10D0 -> ROOM10F0)**, Lauf e2:
`lauf.sh e2_10f0_tuerweg 400 RE15_DEBUG_JUMP=10d0@250 RE15_SET_FLAG=3:0x32 RE15_FIRE_AOT=0@90#10d0 RE15_MSG_LOG=1
RE15_SE_DEBUG=1 RE15_STATE_LOG=... RE15_FRAMEDUMP=2300-2900/2:... RE15_EXIT_AT=2900#10f0`
(ROOM10D0 main00 @0x01174 Door_aot_set Slot 0 -> Raum 0x0F nur bei (3,0x32)=1, sonst @0x0102A Text-Platz; erster
Versuch ohne das Bit blieb deshalb in 10D0 stehen.)
- debug.log: `[fire-aot] slot=0 at F90 (Raum 10D0)` -> `[room] PC loaded room10f0.rdt` -> `[cut10f0] ROOM10F0: Szene
  gestartet` -> letzte Kamera-Anweisung `[scd F911] Cut_chg(2)`; danach KEIN Cut_chg bis zum Ende
  (`[cut10f0] Szene zu Ende` ~F2732).
- state.log (Zeile 23 ab F2477, cam=2 in JEDEM Bild bis F2900): Marvin (8328,-278) F2669 -> geparkt (-30000,-30000)
  F2670; Ada (8315,-246) F2686 -> geparkt F2687.
- Pixel-Differenz aufeinanderfolgender Bilder (Schwelle 60, 2er-Schritt): bis F2664->F2666 wandert die Aenderung an
  den unteren Bildrand (bbox y bis 215), **F2666 -> F2710 durchgehend 0 geaenderte Pixel** — die beiden Park-Bilder
  F2670 und F2687 aendern im Bild nichts. Kontaktbogen `p3_abgang_sheet.png` (F2600..F2696): beide laufen zur Kamera
  und verlassen das Bild unten links, Leon bleibt im Bild.
- Ton: zwei `Se_on bank=14 id=1 -> RE2-Tuerbank` im ganzen Lauf — einer beim Eintreten (Programm +0x019C, Ada/Marvin
  kommen herein, Runde 35), der zweite unmittelbar NACH dem Parken von Ada (Log-Stelle zwischen F2670 und F2700,
  nach Programmfolge Pos_set Ada -> Se_on -> Sleep 45) = ein Tuer-zu-Ton zum Schluss.
- Riegel `unit_r36_szenenleon_abgang10f0` (selbst gefahren): `Zeile 23 B2466 | Cut danach 2, Wechsel 0 | Marvin
  geparkt B2651 (vorher im Bild 0) | Ada geparkt B2676 (vorher im Bild 0) | Tuerton B2676 (1x) | Ende B2721`.
- Code: nur die Generator-Zeilen `cut_chg(0)`/`sleep(10)`/`cut_chg(2)` entfernt (szene_bauen.py), .inc neu erzeugt
  (1342 B), RE15_CUT10F0_OFF_TUER_ZU 0x0514 -> 0x0512 konsistent.

**Urteil Punkt 3: erfuellt.**

## 4. Punkt 4 — Irons tot: kein Kopf-Tracking mehr

Wortlaut: "Sobald Irons gegen Ende von Stage1 tot ist, soll er auch nicht mehr den Kopf hinter Leon hinterher bewegen."

### 4.1 Echter Weg: die Todesszene (Tuer ROOM1130 -> ROOM1150)
`lauf.sh f1_1150_tod 600 RE15_DEBUG_JUMP=1130@250 RE15_SET_FLAG=9:0x47,3:0x5e RE15_FIRE_AOT=2@90#1130 RE15_IT_LOG=1
RE15_NECK_TRACE=.../neck.log RE15_STATE_LOG=... RE15_EXIT_AT=5#1040` (ROOM1130 Door_aot_set Slot 2 @0x008CE -> Raum 0x15)
- debug.log: `[room] PC loaded room1150.rdt` -> `[irons-tod] ROOM1150 Zustand 1: Szene startet` -> `Signal (5,12): Irons'
  Arm faellt (Clip 2 ab Bild 72)` und unmittelbar in der naechsten Logzeile `[szl-r36] ROOM1150 Irons tot: Kopf-FSM +0x1b8 = 0x12`
  -> Tuerknall -> Montage ROOM1130.
- neck.log Irons (Aktor 1): bis F893 `fl=00` (Tracking, acc folgt dem Ziel, z.B. F888 acc=(-335,-233)); **ab F894
  `fl=12`**, Akku gleitet in 5 Bildern auf (0,..) und steht **ab F909 bis F1333 (Montage-Start) konstant auf (0,19)** —
  1 einziger Akku-Wert ueber 425 Bilder, waehrend Leon kniet/aufsteht (Clips 12/10/11, state.log).

### 4.2 Danach, Leon bewegt sich (Gegenprobe lebend)
Vier Laeufe `RE15_DEBUG_JUMP=1150@250 RE15_FORCE_CUT=7 RE15_NECK_TRACE=... RE15_FRAMEDUMP=150:...`:
| Lauf | Flags | Leon | Irons F150 |
|---|---|---|---|
| tot_A | 9:0x49,9:0x47,3:0x5e | (-17500,-25500) | fl=12 acc=(0,19) |
| tot_B | 9:0x49,9:0x47,3:0x5e | (-21000,-22500) | fl=12 acc=(0,19) |
| lebend_A | 3:0x5e | (-17500,-25500) | fl=00 acc=(-111,-12) |
| lebend_B | 3:0x5e | (-21000,-22500) | fl=00 acc=(-714,-58) |
- Bild F150 (Cut 7, Nahaufnahme der Liege): tot_A gegen tot_B **0 Pixel** Unterschied (Schwelle 30); lebend_A gegen
  lebend_B 170 Pixel, bbox (245..273, 52..78) = der Kopf. `p4_irons_lebend_tot_AB.png` (oben lebend, unten tot) — die
  Messung erkennt Tracking, und beim Toten ist keins da.
- Zusaetzlich Leon laufend (`g1_irons_tot_lauf`, Input-Skript, Leon x -17950..-17016 / z -26284..-25500): 380 Bilder
  `fl=12 acc=(0,19)` konstant.
- Riegel `unit_r36_szenenleon_irons`: lebend A -615 / B -491, tot (0,-153) an beiden Standpunkten.
- Elza ROOM1151: Spur L (Baum r36_elza, elza_r36.c `re15_elza_r36_install`) setzt dieselbe gehaltene Todespose
  (state 4, Sub 0, Phase 2, Clip 2, Bild 89) + `neck_flags 0x80|0x12`, Geschwindigkeit 0; J's Regel greift dort
  ebenfalls (setzt 0x12, Bit 0x80 faellt weg — `re15_npc_neck_spawn_init` laeuft nur bei neck_bone==0, das Bit wird
  sonst nur fuer Ziel==eigener Slot gelesen, skeleton_common.c:517 -> folgenlos). Im Integrationsbaum nachzumessen.

**Urteil Punkt 4: erfuellt.**

## 5. Gates

### 5.1 RE-Gate (Stichproben selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`)
| zitiert | gelesen | Urteil |
|---|---|---|
| 0x80073e30[19] / 0x80076ca0[19] -> 0x80052864 | `table`: [19] 0x80073e7c -> 0x80052864, 0x80076cec -> 0x80052864 | stimmt |
| Sub 0x13 Phasen (@0x800528bc .. @0x80052be0) | `dis 0x80052864 230` (Auszug Punkt 1) | stimmt Zeile fuer Zeile |
| Member_set Id 6 = `sw a2,0(a0)` @0x800411c8 | Tabelle 0x80010c8c[6] -> 0x800411c4 `j 0x80041230` + Delay-Slot 0x800411c8 `sw a2,0(a0)` | stimmt |
| Bit 0x200 @0x8002bec0 -> FUN_8002cabc(Spieler,obj,1) @0x8002bed4 | 0x8002beb8 `lw v0,0(s2)`, 0x8002bec0 `andi v0,v0,0x200`, 0x8002bed0 `addiu a0,a0,-13740` (player), 0x8002bed4 `jal 0x8002cabc`, 0x8002bed8 `ori a2,zero,0x1` | stimmt |
| +0x1b8 = 0x12 @0x80042614/18 | 0x8004260c `andi v0,v0,0x40` / 0x80042610 `beq` / 0x80042614 `ori v0,zero,0x12` / 0x80042618 `sb v0,440(s0)` | stimmt |
| Plc_neck Modus 0 caseD @0x80041ed8 | `lbu v0,440(v1)` / `j 0x80041f14` / `ori v0,v0,0x12` (= ODER, der Port setzt `=` — fuer den Toten aus Flags 0 gleichwertig) | stimmt |
| Kopf-FSM 0x02 @0x80037528 / 0x10 @0x80037554 | `andi v0,v0,0x2` @0x80037528, `andi v0,v0,0x10` @0x80037554 -> `sh zero,20(sp)` | stimmt |
| STAGE1 @0x8011d278 `jal 0x80037358` | `dis 0x8011d268` (STAGE1.BIN): 0x8011d278 `jal 0x80037358` | stimmt |
| RE2 ROOM1010 @0x01C18..@0x01C44, EDT @0x28 | Datei-Bytes selbst gelesen (Punkt 2.1) | stimmt |

- Diff nach Rate-Woertern durchsucht (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|vermut|approx`):
  0 Treffer im Code. `getenv` im Diff nur Log-Schalter (RE15_SE_DEBUG in audio_pc.c, R36J_DBG im Riegel) — kein
  Verhaltensschalter. Dossier §1.2 enthaelt ein Wort "Plausibilitaet" (Gegenrechnung Sleep 85 + Bilder nach der
  RE des Bank-5-Bits @0x80052b44, kein Beleg-Ersatz) — Hinweis, kein Mangel.
- Konstanten ohne Adresse: Wegpunkte/HEIM = als PORT-WAHL markiert (Generator-Kopf); Texte = NUTZER-VORGABE;
  Port-Bank 0x3A = Port-interne Nummer (als solche beschrieben); Sleep 35 statt Kendos Plc_cnt/Plc_stop = als
  PORT-WAHL mit RE2-Adressen begruendet. Commit-Message b80c88be traegt die Adressen.
- RE-Gate: **haelt** — mit einer Ausnahme in der Begruendung (nicht im Code): die Behauptung "kein geschlossenes
  Tuerbild im Original" ist falsch (Punkt 2.3).

### 5.2 Vertrag (VERTRAG.md)
- Bank 9: nur Bit 84 (J) gesetzt (`22 09 54 01`, Test `9:0x54`), 85 unbenutzt. Nachrichten ROOM11B0 20/21/22 (J: 20..27).
  Ereignisse 22/23 nur in ROOM11B0 (`re15_szl_ereignis` prueft Raum). Port-Bank 0x3A: keine Kollision (in allen
  r36-Baeumen nur 0x0E = Spur K r35 als weitere Port-Bank).
- Pfade (`git diff master --name-only`): kein release/, platform/android/, shared_assets/PSX/, keine
  tests/*/CMakeLists.txt. Neue Assets `shared_assets/RE2/SCHLOSS1010.EDT/.VH/.VB` (bytegleich RE2, im Dossier fuer das
  Paket-Gate genannt). `tools/r35_k/szene_bauen.py` = Generator der J-eigenen 10F0-Szene (VERTRAG: J = cut_10f0.c).
- Gemeinsame Dateien als kleine Haken: scd_room_setup.c (+2, Installer-Zeile am Block-Ende mit "Runde 36 Spur J"),
  scd_vm.c (+9 an 4 Stellen), game_step_common.c (+6), enemy_ai_common.c (+7), main.c (+4), anim_select_common.c (1),
  aot_common.c (+2), lock_se_common.c (+2). **Hinweis:** audio_pc.c +52 (SCHLOSS1010-Lader nach GLAS1090-Muster) ist
  neue Logik in einer gemeinsam genutzten Datei (se_play_layers ist dort static — begruendbar), Merge-Risiko mit den
  Ton-Spuren G/K/L.
- Probe-Merge (`git merge-tree --write-tree r36/integration r36/szenenleon`, nur lesend): 2 Konflikte, beide trivial
  (Nachbarzeilen, beide behalten): scd_room_setup.c Include + Installer-Zeile neben Spur A (`re15_objfall_*`),
  enemy_ai_common.c Include neben Spur B (`re15_kleidung_r36.h`). audio_pc.c/scd_vm.c/main.c/game_step_common.c
  automatisch.
- Tests je Punkt vorhanden und messend (6 Unit-Teile + integration_r36_szenenleon). Es gibt KEINEN Test fuer das Bild
  der Werkstatt-Tuer (Cut 2/8) und keinen fuer den Nachtrag (Schliessen vor dem Abschliessen).

### 5.3 Gezielte Tests (selbst gefahren, kein volles Paket — VERTRAG/Auftrag)
`ctest --test-dir re15_port/build -R "<regex>" --timeout 900 --output-on-failure`, Regex = die Liste des Bau-Agenten
plus integration_r35_cut10f0, unit_r22_10f0, unit_anim_, unit_irons_, unit_se_, unit_r33_tuer, unit_door_seq,
integration_door_traversal, integration_keydoor, prop_door, unit_r30_irons, integration_r30_irons, unit_r30_elza,
unit_r35_zgirl, unit_r35_inhalt, unit_sicherung_1150, unit_room1150, integration_r34n_g_schrift1150,
integration_elza_vollstart, unit_elza:
```
100% tests passed, 0 tests failed out of 151        (Total Test time (real) = 1762.51 sec)
integration_elza_vollstart 100 s, integration_r30_irons_tisch_laden/bild/licht 42/70/66 s,
integration_r34n_g_schrift1150 143 s, integration_r35_cut10f0 305 s, integration_r35_cut1150 824 s (unter Last,
Schranke 900), integration_r36_szenenleon 126 s
```
Unit-Teile einzeln (Ausgaben im Wortlaut in §1/§2/§3/§4 zitiert): programm/schub/erweiterung/tuer/abgang10f0/irons PASS.

### 5.4 Nebenwirkungen (selbst gemessen)
- **ROOM1171 (Elza)** benutzt ebenfalls Plc_dest Modus 0x13 (sub02 @0x01566/@0x01572, Spielerin + Typ 0x49) — der
  neue Sub 0x13 laeuft dort jetzt mit. Lauf `h2_1171`: `RE15_GOTO_ROOM=1171 RE15_SET_FLAG=3:0xc2
  RE15_SET_FLAG_AT=5:28@200` (ohne den Schalter steht die Szene im Lauf `h1_1171` — GOTO-Sprung, Leon-Modell, ohne
  Eingabe — 1500 Bilder lang im Warten @0x013F4 auf (5,28); wer das Bit im Original setzt, ist nicht untersucht,
  ausserhalb Spur J): `[szl-r36] ROOM1171 Spieler: Plc_dest Modus 0x13` ->
  Spielerin + Typ 0x49 Clip 0x11/0x12 synchron (+1315 in x) -> `Schub-Sub zu Ende -> Sub 6` (sub03 @0x015EE Set(5,20))
  -> msg 3 -> `[room] PC loaded room10b1.rdt`. Kein Haenger.
- **Member_set Id 6** jetzt wirksam: ausser ROOM11B0 nutzen nur ROOM20A0/20A1 sub08 @0x01EDA/@0x01F3E (Objekt 0,
  Werte 0x11 / 0x01) — der Port liest von props.flags nur 0x1/0x2/0x100/0x200 -> folgenlos.
- **Objekt-Bit 0x200** beim Aufbau: Zensus aller RDTs (Obj_model_set pc[6..7]): nur ROOM11E0/11E1 @0x016E6 (0x03FC,
  Objekt 0 Typ 0, kein Kisten-Typ) — der Port uebernimmt dort jetzt FUN_8002cabc(Spieler,obj,1) wie das Original
  (@0x8002bec0 liegt im Original hinter Typ-Handler, Aktor- und Objekt-Ausschiebung; der Port-Haken steht vor dem
  Typ-4-Block, den das Original fuer 0x200 ebenfalls ueberspringt). FUN_8002cabc selbst disassembliert: Abbruch nur bei
  Aktor-Wort&0x40 (@0x8002cad8) bzw. Objekt-Wort&0x2 (@0x8002caec) — bei 0x03FD nicht gesetzt, das Original schiebt also
  ebenso; der Port-Haken prueft 0x2 nicht (fuer die vorhandenen Daten folgenlos). Nicht gesondert gemessen.
- ROOM11B1 (Elza) unberuehrt (Riegel tuer: Platz 8 = msg 12, kein Port-Programm; eigener Lauf c2: Cut 0, Tuer zu).
- Parkplatz-Tuer 11B0 Slot 1 (-26730,-29940) -> ROOM11C0: kein Haken beruehrt sie.

## 6. Maengel (nummeriert, nachpruefbar)

**M1 — Werkstatt-Tuer bleibt im Bild OFFEN (Punkt 2, Wortlaut "Die Tuer soll dann geschlossen bleiben ... wie in
Elzas Szenario").** Nach Marvins Abschliessen (Erweiterung F2446, Cut 8) und bei jedem spaeteren Besuch mit (9,84)=1
(Ost Cut 8, West Cut 7) zeigt der Hintergrund die Tuer offen, waehrend "The door is locked." erscheint. Elzas
gesperrte Tuer (11B1, (3,0x83)==0) ist geschlossen gemalt (Cut 0/2). Ursache: Leon sub00 @0x01184 `4b 00 07` /
@0x01188 `4b 02 08` tauscht beim Strom auf die OFFEN-Bilder 7/8 (zusammen mit @0x0118C Zelle auf); Ereignis 22/23
schliessen nur die Zelle (`37 03 0c ff`/`39 03 0c 03`) und legen den Text-Platz, tauschen die Cuts aber nicht zurueck.
Original-Form vorhanden: Cut_replace ist ein TAUSCH (LAB_80040414 @0x8004044c-a8, Port op_cut_replace) — dieselben
Saetze erneut = zurueck auf 0/2 (11B1 nutzt sie nur im Offen-Zweig @0x0110E/@0x01112). Nachpruefen:
`probe_bg_dump.exe <PSX> <aus> 1` -> ROOM11B02 (zu) / ROOM11B08 (offen) / ROOM11B00 (zu) / ROOM11B07 (offen); Laeufe
d1/d2 (§2.2) mit `RE15_FRAMEDUMP` -> Tuer muss geschlossen gemalt sein.

**M2 — Nachtrag 23:40 nicht umgesetzt: kein Schliessen vor dem Abschliessen (Punkt 2).** Es fehlen Schliess-Animation,
sichtbares Zugehen der Tuer und Tuer-zu-Ton (RE2-Tuerbank) VOR Kendos Abschliess-Clip/-Ton; verlangte Abnahme:
Bildnummern Schliessen < Tuer-zu-Ton < Abschliessen < Abschliess-Ton + Framedump-Serie. Heute an der Tuer nur
+0x005C..+0x0080 (Clip 15, Sleep 20, Se_on 0x3A/0x0A, Clip 15 rueckwaerts, Sleep 35, Zelle zu). Das sichtbare Zugehen
liefert dieselbe Original-Mechanik wie M1 (Cut 8 offen -> Cut 2 zu). (Der Nachtrag kam nach dem Bau-Ende — kein
Vorwurf an den Bau-Agenten, aber der Wortlaut gilt fuer diese Abnahme.)

**M3 — Falsche Tatsachenbehauptung im Dossier / Entscheidungsfrage an den Nutzer.** J_szenenleon.md §2.3, OFFEN 1,
"Fuer den Nutzer/Entscheidungsfrage" und die Abschlussmeldung behaupten: ROOM11B.BSS zeige die Tuer offen, "genau so
sieht Elza sie", ein geschlossenes Tuerblatt gebe es "in keinem Original-Datensatz", es braeuchte ein neues 3D-Modell.
Gemessen falsch (Cut 0/2 geschlossen, Elza sieht im gesperrten Zustand Cut 0/2; Port-Lauf c2 cam=0 Tuer zu). Die
Frage darf so nicht an den Nutzer gehen; OFFEN 1 ist durch M1/M2 zu ersetzen.

**M4 — Kein Riegel fuer das Tuerbild.** unit_r36_szenenleon_tuer/erweiterung und integration_r36_szenenleon pruefen
Zelle, Text-Platz und Ton, nicht den gezeigten Cut. M1 ging deshalb gruen durch. Verlangt: nach dem Abschliessen und
beim Raumaufbau mit (9,84)=1 sind die Zonen auf 0/2 zurueckgetauscht (Kamera an der Tuer zeigt Cut 2 bzw. 0), und
fuer M2 die Ereignisfolge mit Bildnummern.

Hinweise (kein Mangel):
- H1 audio_pc.c +52 Zeilen neue Logik in gemeinsam genutzter Datei (Merge-Risiko mit G/K/L).
- H2 Probe-Merge in r36/integration: 2 triviale Konflikte (scd_room_setup.c, enemy_ai_common.c) — beide Zeilen behalten.
- H3 ROOM1171 (Elza, ausserhalb Spur J): die Szene sub02 wartet @0x013F4 auf (5,28); im Lauf h1_1171 (GOTO, ohne
  Eingabe) kam das Bit in 1500 Bildern nicht — Mechanismus nicht untersucht; mit erzwungenem Bit laeuft der neue Sub 0x13
  dort sauber durch (§5.4). Fuer den Orchestrator.
- H4 Dossier §1.2 "Plausibilitaet" (Gegenrechnung nach erfolgter RE, kein Beleg-Ersatz).
- H5 Paket-Gate: SCHLOSS1010.EDT/.VH/.VB muessen in release/make_package.sh (nur Orchestrator) — im Dossier genannt.
