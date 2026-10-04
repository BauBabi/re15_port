# Runde 35 Spur E (inventar1050) — Unabhaengige Abnahme 0

Stand: Zweig `r35/inventar1050`, HEAD f06ab36f (Basis master 154a73c1), Baum
`.claude/worktrees/r35_inventar1050`. Abnahme 2026-10-03. Alles unten habe ich SELBST am gebauten Stand
gemessen bzw. selbst disassembliert; Aussagen des Bau-Agenten sind nur uebernommen, wo das ausdruecklich
steht.

## 0. Bau und Suite

* `bash re15_port/tools/local_build.sh configure` + `... build`: Preflight `cc1 --version EXIT=0 -> Toolchain OK`,
  `ninja: no work to do.`, `=== LOCAL-BUILD-OK (build)` (die exe vom 22:05 ist der Stand des letzten
  Code-Commits 8e7ee44e; danach nur Test-/Dossier-Commits).
* Suite selbst gefahren (`bash re15_port/tools/local_build.sh test`, nach allen Messlaeufen, ohne parallele
  exe in diesem Baum): `100% tests passed, 0 tests failed out of 487`, `Total Test time (real) = 1197.53 sec`,
  `=== LOCAL-BUILD-OK (test) — Tests 487/487` (EXIT=0, keine Wiederholung noetig, kein roter Fenster-Haken).
* Gezielt: `ctest --test-dir re15_port/build -R "r35_inventar1050|r34n_d_adaruf|unit_room1140_combat"` ->
  `100% tests passed, 0 tests failed out of 18`, darunter
  `r35_inventar1050[a]: Szene, pf=00000007 pm=0 vor START, Statusschirm offen, 210 Gestenbilder zur Tuer, kein Clip 19`,
  `[b]: Startinventar ohne Messer, mg=-1, 9 Stichbilder (W01, Clip 7)`,
  `[c]: alter Stand geladen, Messer entfernt, 9 Stichbilder`, `szene: 0 Fehler`, `raster: 0 Fehler`.

## 1. Messungen je Nutzer-Punkt (eigene Laeufe, eigene exe-Kopie `re15_pc_abnE0.exe`)

Laufskript (Scratchpad, nicht eingecheckt): exe-Kopie neben der Original-exe (Asset-Wurzel), cwd = Laufverzeichnis,
`RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1`, Titel startet selbst (`RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2`).
Bilder per `RE15_FRAMEDUMP` (komponiertes Bild vor dem Present). Spalten Zustandslog: pf = g_re15_pauseflags,
pm = player_mode, mo = Spieler-Clip, ac = Zielclip, mg = Magazin des ausgeruesteten Platzes (-1 = 25c8 0x80).

### P1 — "Nach unserer neuen Ada Cutscene im Room 1050 ... laesst sich innerhalb des selben Raums das Player Inventory nicht mehr oeffnen."

**Lauf A1** (echter Tuerweg ROOM1000 -> ROOM1050, MIT Audio, Quadrat-Hammern waehrend der Szene):
`RE15_SET_FLAG=3:121 RE15_DEBUG_JUMP=1000@120 RE15_PLAYER_POS=22230,-13400,0,0 RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=100 RE15_INPUT_SCRIPT=U0.8,W14 RE15_PRESS=square@150,square@300,square@420,start@560,start@640,square@760,start@900
RE15_INV_DBG=1 RE15_STATE_LOG=state.log RE15_MSG_LOG=1 RE15_FRAMEDUMP=140-560/10:f_ RE15_EXIT_AT=1000#1050`
```
[aot] DOOR FIRE ... target_cut=4 spawn=(14700,0,-13500)            (Tuer 1000 -> 1050 betreten)
[adaruf] Ereignis 13: Szene startet, Spieler (16500,-13500) ...
F151 pf=01000007 pm=2                   Szene (Set(2,7)=1)
F171 msg(a=1 id=22) / F303 id=23 mo=18 / F413 id=24 mo=17
F512 msg(a=0 id=24) pf=01000007         msg 24 schliesst (413+99), Schnappschuss mit Pad-Bit zurueck
F513 pf=00000007 pm=1                   Set(2,7)=0 im Bild danach (413+100) -> Pad-Bit weg
F527 pf=00000007 pm=0
[invdbg] START F560 -> F569 stage=2 open=1 | START F640 -> F653 stage=0 open=0 | START F900 -> F909 open=1
```
Das Quadrat-Hammern in msg 23/24 (F300/F420) aendert nichts (Zustand 5 = Standzeit, kein Tasten-Ende).

**Lauf A2** (danach zur 10A0-Tuer, Quadrat = Sperrtext msg 25, wegdruecken, START):
`... RE15_NOAUDIO=1 RE15_INPUT_SCRIPT=U0.8,W14,U0.5,W0.5,A0.1,W3,A0.1,W3,S0.1,W3 RE15_PRESS=square@150 RE15_EXIT_AT=870#1050`
```
F574 msg(a=1 id=25) pf=FFFF0007   Sperrtext "I have to help th..." (Bild F600)
F668 msg(a=0 id=25) pf=00000007   weggedrueckt
F760 START -> [invdbg] F769 stage=2 open=1   Statusschirm offen (Bild F800)
```
Bild F800 (Scratchpad `abn/a2_sheet.png`): Statusschirm, Item-Liste BROWNING HP 15 + H.GUN BULLETS 50.

**Urteil P1: erfuellt.** Nach der Szene oeffnet START das Inventar im selben Raum, auch nach dem Sperrtext,
mehrfach auf/zu. Der Fix erklaert den Befund: Vorbedingung (msg 24 = `04 00 ... 04 01 01 63`,
adaruf_1050.c Text-Block, + Sleep 100 + Set(2,7)=0) steht im Protokoll; Schliessen jetzt in N+99, Set in N+100.

### P2 — "Kampfmesser aus dem Player Inventar raus ... IMMER der fallback, wenn keine Waffe ausgewaehlt ist"

**Lauf B1** (Neues Spiel, ROOM1000; Pistole im Statusschirm AUSRUESTEN, schiessen, im Statusschirm ABLEGEN, stechen):
`RE15_NOAUDIO=1 RE15_DEBUG_JUMP=1000@120 RE15_PLAYER_POS=22230,-13400,0,0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100
RE15_INPUT_SCRIPT=W1,S0.1,W2,A0.1,W1,A0.1,W1,A0.1,W2,S0.1,W2,M1,MA0.1,M0.6,W1,S0.1,W2,A0.1,W1,A0.1,W1,A0.1,W2,S0.1,W2,M1,MA0.1,M0.6,MA0.1,M0.6,W1
RE15_INV_DBG=1 RE15_STATE_LOG=state.log RE15_FRAMEDUMP=200-800/20:f_ RE15_EXIT_AT=830#1000`
```
[messer] Startinventar Charakter 0: 03 x15 15 x50 | Ausruest-Platz 0x80, Waffe 1
[equip] W-bank -> W01                       (Start: nichts ausgeruestet -> Messer)
Bild F200: Equip Arms = Messer, Item-Liste = Pistole 15 + Munition 50 (KEIN Messer)
nach F332: [equip] W-bank -> W03            (Pistole per Menue ausgeruestet)
F415 pad=8800 ac=7 fx=5 mg=14               Pistolenschuss (15 -> 14)
F602 mg=-1                                  im Menue abgelegt (UNEQUIP, 25c8 = 0x80)
nach F668: [equip] W-bank -> W01            Commit beim Schliessen 0x80 -> Waffe 1
F751 / F772 pad=8800 ac=7 mg=-1             Messerstiche ohne Waffe im Inventar
```
Bilder F300 (Equip Arms Pistole, darunter "Standard Arms" Messer), F620/F640 (Equip Arms wieder Messer,
Item-Liste ohne Messer), F760 (Stich-Pose) — Scratchpad `abn/b1_sheet.png`.
Integrationsriegel Teil c (selbst gefahren): CONTINUE mit Altstand-Karte (Messer Platz 0) ->
`[messer] alter Spielstand: 1 Messer entfernt, Ausruest-Platz 0x80, Waffe 1`, 9 Stichbilder.
Item-Zensus selbst (`re15_port/tools/scd_dump_room.py` ueber alle 240 RDTs unter shared_assets/PSX/STAGE*):
164 Item_aot_set (0x50), Ids {04,05,07,08,0c,0d,13,15,16,17,22..26,30,31,32,36..39,3f,41,44,46,47} — **kein Id 01**.
(Der Bau-Agent nennt 194 Saetze / 206 RDTs; die Zahl weicht ab, der Befund "kein Messer" nicht.)
Kiste: Einlagern der ausgeruesteten Waffe setzt 25c8 := 0x80 (re15_itembox.c unequip_if), der Kisten-Schirm
schliesst ueber menu_common.c close_phase -> `(eq == 0x80) ? 1` -> Messer (Code gelesen, nicht an der exe gefahren).

**Lauf B3** (Elza ueber die echte Spielerwahl, Weg wie integration_elza_vollstart):
`RE15_NOAUDIO=1 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_INPUT_SCRIPT=W2,S1,W900 RE15_INPUT_SCRIPT_START=30
RE15_STATE_LOG=state.log RE15_EXIT_AT=150`
```
[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)
[messer] Startinventar Charakter 4: | Ausruest-Platz 0x80, Waffe 1      (Tabelle @0x80074bc4 ohne Messer = leer)
[equip] W-bank -> W01                                                   (Messer-Bank)
Zustandslog: 151 von 151 Bildern mg=-1
```

**Urteil P2: erfuellt** (Leon: Start, Ausruesten/Ablegen im Menue, Stich, Altstand an der exe; Elza: Start an der exe,
Stich nur per Riegel, s. H3).

### P3 — "Leon ... redet mit dem Spieler von den Animationen her. Er sollte so mit sich selbst reden, wie in der Cutscene in ROOM 1170."

Lauf A1 (oben), Zustandslog + Bilder F140..F560/10:
```
Gestenbilder mo=18: 110, mo=17: 100, mo=19: 0; Gierung in ALLEN 210 Gestenbildern rot=4095 (= +X, zur Tuer)
msg 23 F303..F402 Clip 18 | msg 24 F413..F512 Clip 17
```
Ausschnitt Leon (Scratchpad `abn/a1_zoom.png`, F280..F520): Leon steht im Profil, Blick zur Tuer (Bild rechts),
F320..F400 Hand zur Brust mit gesenktem Kopf, F420..F430 Arm-Schwung Richtung Tuer, danach Ruhe. Er dreht sich in
keinem Bild zur Kamera. Kamera Cut 4 (14999,-8140) gegen Leon (15800,-13500) Blick (1,0): cos = -801/5420 = -0,148
-> 98,5 Grad (Bau-Agent: 99 Grad) — die Kamera sieht ihn von der Seite/schraeg hinten.
Die Form ist Byte fuer Byte ROOM1170 (selbst aus `re15_port/shared_assets/PSX/STAGE1/ROOM1170.RDT` gelesen):
```
0x175E: 2b 0d 00 00 | 3f 00 12 00 | 41 02 00 00 00 00 2c 01 00 0a | 09 0a 1e 00 | 41 04 03 00 00 00 00 00 64 00
        09 0a 3c 00 | 3f 00 12 00 | 43 00 80 00 | 09 0a 14 00                         (sub14)
0x15EC: 2b 07 00 00 | 3f 00 11 00 | 09 0a 64 00 | 22 02 07 00 | 22 01 1b 00 | 29 03 | 3c 01 | 2e 01 00 00 | 42 00 | 01 00  (sub02)
```
= k_ruf +0x50..+0x7C (sub14-Form) und +0x80..+0x98 (sub02-Form), adaruf_1050.c. Clip 25 (1170 sub02 @0x015E4
`3f 00 19 00`, Hand ans Gesicht) ist raumeigen (D_adaruf.md Z.200-202) und deshalb nicht genommen — im Dossier
unter OFFEN.

**Urteil P3: erfuellt.** Keine Drehung zur Kamera, keine Gespraechsgeste 19, Selbstgespraechs-Gesten aus den
ROOM1170-Szenen (Kopf senken/schuetteln, Hand zur Brust, Arm-Schwung).

## 2. RE-Gate (@0x-Belege, selbst disassembliert)

`re15_disasm.py` (info/Re1.5/PSX.EXE) / `re2_disasm.py` (info/re2leon/PSX.EXE):

| Behauptung | Selbst gelesen | Stimmt |
|---|---|---|
| P1 @0x800283b8 `j 0x80028424` nach der Spanne, @0x8002842c `bne v0,zero,0x8002826c` | `800283b4 sb v0,-31452(at)` / `800283b8 j 0x80028424` / `80028424 lbu v0,0(s0)` / `8002842c bne v0,zero,0x8002826c` | ja |
| P1 druckbar = (b-0x0c)&0xff < 0xec @0x80028274-80 | `80028274 addiu v0,v0,-12` / `80028278 andi 0xff` / `8002827c sltiu v0,v0,0xec` / `80028280 bne v0,zero,0x80028434` | ja |
| P1 `01 NN` -> Zustand 6 + Standzeit im selben Aufruf | Tabelle @0x8001096c [0] -> 0x800282bc; `800282c8 bne v0,zero,0x80028728` / `80028728 ori v0,zero,0x6` / `80028730 sb v0,-31455(at)` / `80028744 sb v0,-31451(at)` | ja |
| P2 Commit 0x80 -> Waffe 1 @0x80046654-88 | `80046658 lbu v1,9672(v1)` / `8004665c ori v0,zero,0x80` / `80046660 bne v1,v0` / `80046668 j 0x80046680` / `8004666c ori v0,zero,0x1` / `80046688 sb v0,-13731(at)` | ja |
| P2 Starttabellen @0x80074bb8 / @0x80074bc4 / @0x80074bd0 | `80074bb8: 01 03 15 00 00 00 .. 01 00 00 00 00 00 .. 00 0f 32 00 00 00` | ja |
| P2 Weiche `sltiu v0,v0,0x4` @0x80045e28 auf 0x800aca5c; 25c9 := 0x80 @0x80045fe0; 25c8 := 0 @0x80045fec | `80045e20 lbu v0,-13732(v0)` / `80045e28 sltiu v0,v0,0x4` / `80045fe0 sb v0,9673(at)` (v0 = 0x80) / `80045fec sb zero,9672(at)` | ja |
| P2 RE2 Reserve Platz >= 0 @0x8006a294/@0x8006a2a0 | `8006a278 jal 0x800696cc` (Suche, nicht gefunden `80069708 addiu v0,zero,-1`) / `8006a294 nor v0,zero,a0` / `8006a2a0 srl v0,v0,31` | ja |
| P2 RE2 breite Waffe nicht schieben bei 0x80 @0x8006999c | `80069978 lbu a0,8(s4)` / `80069998 addiu v0,zero,128` / `8006999c beq v1,v0,0x80069aa4` / `800699a0 addiu v0,a0,2` | ja |
| P2 Statusschirm 0x80 = Messer-Kachel hell @0x800495e8-618 | `800495e8 bne v1,v0` / `800495f0 ori v0,zero,0x80` / `800495f8 sb v0,9677(at)` (25cd) / `80049614 sb v0,...` (0x3e) | ja |
| P3 ROOM1170 sub14/sub02, ROOM1090 sub02 @0x02486 `09 0a 14 00` | Rohbytes oben | ja |

Suche im Diff nach `deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|vorlaeufig|geschaetzt`: kein
Treffer. Neue `getenv("RE15_...")`: keine. Nutzer-Vorgaben gekennzeichnet (`NUTZER-VORGABE: Messer nicht hinein`
messer_rueckfall.c, `NUTZER-VORGABE (AUFTRAG.md Z.94)` re15_adaruf.h / adaruf_1050.c). Commit-Messages tragen die
Adressen (148eb284, c17305bd, 232099e6, f06ab36f gelesen). **Gate haelt.**

## 3. Vertrag / Pfade / Tests

* `git diff master --name-only` (ausser analysis/): engine/src/{adaruf_1050,game_step_common,inventory_common,
  messer_rueckfall (neu),msg_common,re15_itembox,re15_savedata}.c, include/{re15_adaruf,re15_inventory,re15_messer (neu)}.h,
  platform/pc/main.c, tests/integration/{test_r34_granaten (+4 Zeilen),test_r35_inventar1050 (neu)}.cmake,
  tests/unit/probes/r35_inventar1050.cmake (neu), tests/unit/{test_r34n_d_adaruf,test_r35_inventar1050 (neu),
  test_room1140_combat}.c, tools/r35_e/messlauf.sh. **Keine** release/, platform/android/, shared_assets/PSX/,
  tests/unit/CMakeLists.txt, tests/integration/CMakeLists.txt. Gemeinsame Dateien nur mit kleinen Haken
  (main.c 1 -> 2 Zeilen, msg_common.c +5, game_step_common.c 1 Zeile). **Pfad-Gate haelt.**
* Bank-9-Bits: kein neues (k_ruf `22 09 41 01` = Bit 65 aus Runde 34). Nachrichten-IDs: keine neuen (22..25 vorhanden,
  28..31 unbenutzt). Ereignisse: keines neu (13 vorhanden). **Vertrag haelt.**
* Tests je Punkt vorhanden und messend: P1 `unit_r34n_d_adaruf_szene/_raster` (Pad-Bit + START), P2
  `unit_r35_inventar1050_*` (8), P3 `unit_r34n_d_adaruf_szene` (Gierung zur Tuer, cos max <= 0, Clip 18/17, Neck 2/4,
  kein Clip 19), alle drei an der echten exe in `integration_r35_inventar1050` (a/b/c). Alle gruen (selbst gefahren).
  Die vom Bau-Agenten genannte Mutationsprobe (Fix aus -> Riegel rot) habe ich NICHT wiederholt (kein Code-Eingriff
  in der Abnahme); der Riegel prueft aber genau die Vorbedingung (Pad-Bit nach dem Szenenende, START-Annahme).

## 4. Hinweise (nicht abnahmeentscheidend, nachpruefbar)

* **H1 — Dossier-Aussage zu @0x80028434 ist falsch.** E_inventar1050.md "Umsetzung P1": "Druckbare Glyphen nach der
  Spanne warten wie bisher das Tempo ab (@0x80028434 `bne s2,zero` mit verbrauchtem Budget)". Disasm:
  `80028434 bne s2,zero,0x80028250` mit Verzoegerungsplatz `80028438 addiu s0,s0,1` — der Zeiger s0 (Fenster-Ende,
  `8002874c sw s0,-31444(at)` = 0x800b852c) rueckt AUCH bei nicht genommenem Sprung vor; erst danach
  `8002843c lbu v0,-31452(v0)` / `j 0x80028740` (Tempo). Das Original zeigt also die erste druckbare Glyphe nach der
  Spanne im SELBEN Bild und wartet erst dann das Tempo; der Port (msg_common.c Zweig `04 00`, `break`) wartet vorher.
  Vorbestehende Abweichung (eine Glyphe um `message_scroll` Bilder spaeter), beruehrt P1 nicht (msg 22..24 enden mit
  `01 63`, Steuerbyte). Satz im Dossier korrigieren; Angleichung waere eine eigene Aenderung mit diesem Beleg.
* **H2 — veralteter Kommentar** re15_itembox.c:138-143 ("equip index += 2 UNCONDITIONALLY ... no 0x80 guard ...
  byte-true as disassembled") widerspricht der neuen Zeile :154 (`!= 0x80`, RE2 @0x8006999c).
* **H3 — Elza nur der Start an der exe gemessen** (Lauf B3: leeres Inventar, 0x80, Waffe 1, W01). Ein Elza-Stich bzw.
  ein Elza-Durchlauf ist nicht gefahren (der Titel-Autostart `RE15_TITLE_SHOT_AF` waehlt immer Leon —
  main.c:3689 `re15_gameflow_new_game(0)`; mein erster Versuch B2 damit landete bei `character=0`). Der Bau-Agent
  fuehrt den Elza-Durchlauf unter OFFEN.
* **H4 — Item-Debug (SELECT+R1) ruestet die Debug-Waffe nicht mehr automatisch aus**, wenn Zelle 0 nicht der
  ausgeruestete Platz ist (vorher war Zelle 0 immer das ausgeruestete Messer). Folge: test_r34_granaten.cmake Lauf
  "debug" +`RE15_EQUIP=3`; moegliche Zusammenfuehrungs-Kollision mit Spur A (granate) in dieser Datei.
* **H5 — Nachrichten-FSM-Korrektur wirkt global**: jede Zeile `04 00 ... 04 NN 01 NN` schliesst ein Bild frueher
  (= Original, Belege oben). Suite gruen; andere Szenen-Zeitlinien sind nicht einzeln nachgemessen (Dossier OFFEN).
* **H6 — Clip 25** (ROOM1170 sub02 @0x015E4, Hand ans Gesicht) ist raumeigen und in ROOM1050 nicht verfuegbar; ersetzt
  durch die sub14-Form. Wer genau diese Geste will, braucht ein fremdes RBJ-Clip in ROOM1050 (Dossier OFFEN).

## 5. Ergebnis

| Punkt | Urteil |
|---|---|
| P1 Inventar nach der 1050-Szene | erfuellt |
| P2 Messer raus, immer Rueckfall | erfuellt |
| P3 Selbstgespraech wie ROOM1170 | erfuellt |
| Suite | 487/487 gruen (selbst gefahren) |
| @0x-Gate | haelt |
| Pfad-/Vertrags-Gate | haelt |
| Tests je Punkt | vorhanden, messend, gruen |

**Abnahme: BESTANDEN.** Maengel: keine abnahmeentscheidenden. Die Hinweise H1 (falscher Dossier-Satz zu
@0x80028434) und H2 (veralteter Kommentar re15_itembox.c:138-143) sollten vor dem Zusammenfuehren korrigiert werden;
H4 (test_r34_granaten.cmake) ist beim Zusammenfuehren mit Spur A zu beachten.

Belegbilder dieser Abnahme liegen nur im Scratchpad der Abnahme-Sitzung (`abn/a1_zoom.png`, `abn/a2_sheet.png`,
`abn/b1_sheet.png`), nicht im Repo; die Laeufe sind mit den obigen Schaltern jederzeit wiederholbar. Das eingecheckte
Bild des Bau-Agenten `E_belege/p3_leon_gesten_zoom.png` (angesehen) zeigt dasselbe wie mein Lauf A1: Profil zur Tuer,
F320..F380 Hand zur Brust, F430 Arm-Schwung, keine Drehung zur Kamera.
