# Runde 36 — D2: exe-"Haenger" ROOM2060 (Messbericht, 2026-10-12)

Integrationsbaum `.claude/worktrees/r36_integration`, Zweig `r36/integration`, HEAD ba384fac (+ Doku-Commit 7b2e56d9),
exe `re15_port/build/platform/pc/re15_pc.exe` vom 2026-10-12 01:28:36, sha256 `3b835b9458ee6e9a…`. Messkopie
`re15_pc_d2.exe` im selben Ordner (bitgleich, nach den Laeufen geloescht), Arbeitsordner im Sitzungs-Scratchpad
`D2/<lauf>/` (debug.log, climb.txt, objfall.txt, state.txt, finisher.log, wegplan.txt, gdb_bt.txt, gdb_halt.txt,
stillstand.txt, fenster_[abc].png). Kein Code geaendert, nicht gebaut, kein ctest. Startskript `D2/lauf_d2.sh` =
Grundumgebung 1:1 aus `abn1/lauf.sh` der R-Abnahme 1 (RE15_NO_INTRO, RE15_NOAUDIO, RE15_WINDOW_SCALE=1,
SDL_ASSERT=always_ignore, RE15_TITLE_CONFIRM_MS=1500, RE15_PSELECT_AUTO=1, RE15_STAIR_DBG, RE15_CLIMB_TRACE,
RE15_OBJFALL_LOG, RE15_PUSH_LOG, RE15_INPUT_SCRIPT_BASIS=spiel, RE15_INPUT_SCRIPT_START=1), nur exe und Ordner
anders. Waechter `D2/treiber2.sh`: steht climb.txt 25 s still, waehrend der Prozess lebt -> Fensterabzug
(PrintWindow), gdb passiv (`thread apply all bt` + Globals), gdb aktiv (Haltepunkte), erst DANN `taskkill /PID … /F`.

## Ergebnis

**Reproduziert (3 von 3 exakt + 1 Diagnoselauf + Elza/ROOM2061), aber es ist KEIN Haenger der exe.** (10 Laeufe, Abschnitte 1 und 5.) Leon wird in ROOM2060 von den
zwei Adult-Spinnen (Typ 0x25, Slots 1/2) getoetet: HP 100 -> 80 -> 60 -> 40 -> 20 -> 0 -> -20 in sechs Bissen zu je
20 (F166, F270, F424, F486, F543, F626). Ab F627 laeuft der Todes-Clip 7 (113 Bilder, `mo=7 af=0..112` — der
"Stillstand mit af bis 112"), parallel die Game-Over-FSM; in F887 setzt sie `g_gameover_active`, main() verlaesst die
Spielschleife und springt in das TITELMENUE (`goto re_title`, main.c:11561-11565 -> Schleife main.c:3500). Dort
bleibt der Prozess fuer immer stehen, weil im Messlauf nichts mehr eine Taste druecken kann:
1. `RE15_TITLE_CONFIRM_MS` bestaetigt nur EINMAL je Prozess (main.c:3511-3528, `s_tc_done`) — beim Spielstart verbraucht;
2. das Eingabeskript laeuft mit `RE15_INPUT_SCRIPT_BASIS=spiel` auf `g_engine.frame_count` (input_pc.c:397), und das
   zaehlt nur die Spielschleife hoch (main.c:11556) — im Titel steht es auf 888, das Skript liefert also ewig
   Skript-Tick 887 = `U` (0x0010) ohne Flanke;
3. `RE15_EXIT_AT` wird nur in der Spielschleife geprueft (main.c:11342-11353) — Bild 2300 wird nie erreicht.
Alle Messschriebe (debug.log `[walk]`, climb.txt, objfall.txt) schreibt nur die Spielschleife -> "alle Logdateien
eingefroren, Prozess lebt". Das Fenster zeigt das Titelmenue (NEW GAME / LOAD GAME / OPTION), reagiert
(`Responding=True`, `IsHungAppWindow=False`) und verbraucht CPU (Bildschleife).

Die "Klemme bei x -2328" der R-Abnahme 1 ist ebenfalls keine: dort endet das Skript-Segment `U4.47` (Tick 468..601
= F469..F602); Leon steht ab F603 (climb.txt T854 `pad=0000`), dreht mit `R0.37` auf 3104 und wird in F626 im Stehen
gebissen.

## 1. Exakte Reproduktion an der Integrations-exe (3 Wiederholungen)

Kommandozeile = R3b aus R_abnahme_1.md (Nachtrag) bzw. dem Abnahme-Transkript (`timeout -k 5 700 bash lauf.sh r3b_2060 …`,
gestartet 18:46:45), voll ausgeschrieben:
`RE15_DEBUG_JUMP=2060@250 RE15_PLAYER_POS=7600,3950,2048,0 RE15_INPUT_SCRIPT=W2,U1.3,W0.3,L0.37,W0.3,U4.8,W0.3,R0.37,W0.3,U4.6,W0.3,R0.37,W0.3,U4.47,W0.3,R0.37,W0.3,U0.79,W0.3,R0.33,W0.3,U17,W1.5,R0.37,W0.3,U0.7,W0.3,L0.37,W0.3,U1.4,W0.3,L0.37,W0.3,U6.3,W1.5,U0.7,W0.3,A0.1,W2.5,R0.37,W0.3,A0.1,W3 RE15_EXIT_AT=2300`
(debug.log: `-> 1834 ticks @ 30 fps, start frame 1`).

| Lauf | Start | climb.txt | letzte Zeile (= letztes Spielbild) | Stillstand erkannt | danach |
|---|---|---|---|---|---|
| w1_exakt | 01:40:45 | 1138 Zeilen, zuletzt geschrieben 01:41:39.99 | `T1138 room=2060 pad=0010 pos=(-2328,0,-7336) rot=3104 mo=7 af=112 … t4pos=(-3143,2075)` | 01:42:06 (25 s) | gdb, Abbruch 01:42:16, run.log `exit=1` (taskkill) |
| w2_exakt | 01:42:48 | 1138, 01:43:43.10 | identisch | 01:44:08 | Abbruch 01:44:19 |
| w3_exakt | 01:44:45 | 1138, 01:45:39.91 | identisch | 01:46:06 | Abbruch 01:46:15 |
| d1_diag (+RE15_STATE_LOG, RE15_FINISHER_LOG) | 01:47:00 | 1138, 01:47:55.04 | identisch | 01:48:20 | Abbruch 01:48:29 |

* w1/w2/w3/d1 climb.txt sind **bitgleich** (`cmp`); debug.log endet jeweils mit `[walk] F870 cut=4 mo=7 af=112 frz=0 |
  pl pos=(-2328,0,-7336) rot=3104` — genau wie im R3b-Lauf (`abn1/r3b_2060/debug.log`, letzte Zeile ebenso).
* Gegen R3b (R-Baum Stand Nachbesserung 1 = 01248da5, Messkopie `re15_pc_r36abn1.exe`, sha256 `13ffe6a5…`, 2026-10-11 18:46): alle 1138 climb-Zeilen gleich, wenn man das Feld
  `t4pos` weglaesst (`diff` nach `sed 's/ t4pos=([^)]*)//'`: 0 Zeilen). Einziger Unterschied: Tritt obj 2 steht im
  Integrationsstand ab Raumstart auf (-3143,2075) statt (-3125,2075) = die Ruhe-Ausschiebung der Spur R
  (Nachbesserung 3, LAB_8002c0b8 / FUN_8003da78 @0x8003dab8, +18 aus r0[9]); auf den Weg des Spielers wirkt das nicht.
* Bild-Zuordnung: climb `T` = Spielbild `F` + 251 (debug `[walk] F600 … (-2178,0,-7336)` = climb `T851`). T1138 = F887.

## 2. Was der "haengende" Prozess tut (gdb, Fenster, Globals)

Passiv (`gdb -p <PID> -batch`, `thread apply all bt 40`, w1): 12 Threads, alle Nebenthreads in Wartefunktionen
(D3D-Hintergrund, Windows.Gaming.Input, CoreMessaging, Debugger-Break). Hauptthread:
```
#17 D3D_RenderPresent ()   #18 SDL_RenderPresent_REAL ()   #19 SDL_RenderPresent ()
#20 re15_render_end_frame ()   #21 main ()            (Ruecksprung 0x14000dd58 im Image)
```
Der Ruecksprung liegt direkt hinter `call re15_title_fadein_level` / `call re15_render_pc_title_fade_sub` /
`call re15_render_end_frame` (objdump 0x14000dd47..dd53) = Bildende der TITELSCHLEIFE. In w3 stand der Hauptthread in
`re15_gameflow_mode ()` <- `main ()` @0x14000de55 = `cmp $0x1,%eax ; je 0x14000d604` = die Schleifenbedingung
`while (re15_gameflow_mode() == RE15_MODE_TITLE)` (main.c:3500; 0x14000d604 ruft bei 0x14000d609 `re15_input_tick`).

Globals (Symboltabelle der exe, Layout re15_engine.h:31-37; die ersten vier Werte in allen fuenf Stillstand-Laeufen w1,
w2, w3, d1, v4a gleich, g_engine erst ab treiber2.sh gelesen = w3, d1, v4a):
```
g_gameflow        = 0x00000001   (RE15_MODE_TITLE, re15_gameflow.h:16-28)
g_gameover_active = 0x00000001   (gesetzt game_step_common.c:797-798, sub 6 ctr >= 0x6d)
s_script_len      = 0x0000072a   (1834 Skript-Ticks)      s_script_spiel = 1 (Basis = g_engine.frame_count)
g_engine.frame_count = 0x00000378 (888)   pad_current/previous/pressed/released = 0x0010 0x0010 0x0000 0x0000
s_input_ticks     = 0x197c -> 0x19cf -> 0x19d0 -> 0x19d1 (w1; zaehlt je gerendertem Bild weiter)
```
Aktiv (Haltepunkte `re15_render_pc_title_menu`, `re15_climb_standing_tick`, `re15_game_step`, dreimal `continue`):
in allen fuenf Stillstand-Laeufen 3 von 3 Treffern auf `re15_render_pc_title_menu` (Ruecksprung `main` 0x14000d837 = hinter
`call re15_render_pc_title_menu` @0x14000d832 = main.c:3558), **0 Treffer** auf den Spiel-Tick. Zwischen den Treffern
steigt s_input_ticks um je 1, frame_count bleibt 888 (w3, d1, v4a).

Fenster (PrintWindow, `D2/w*_exakt/fenster_[abc].png`): Titelmenue "BIOHAZARD 2 — NEW GAME / LOAD GAME / OPTION",
`Responding=True`, `IsHungAppWindow=False`; CPU-Zeit steigt (w1 12.11 s -> 13.06 s -> 13.41 s ueber ~10 s) — eine
laufende Bildschleife, kein blockierter und kein drehender Prozess.

**Endlosschleife = main.c:3500 (Titelmenue), Bedingung:** die Schleife verlaesst man nur mit einer Bestaetigung
(Flanke einer Bestaetigungstaste Kreuz/Quadrat/Dreieck/Kreis/Start in `pp = g_engine.pad_pressed`, Maske
main.c:3569-3570, bzw. das einmalige RE15_TITLE_CONFIRM_MS-Kreuz main.c:3526-3528).
`pad_pressed` ist 0, weil `bits = s_script[t]` mit `t = g_engine.frame_count - s_script_start = 888 - 1 = 887`
(input_pc.c:397, 441) jedes Bild denselben Wert 0x0010 liefert (U17 = Tick 683..1192) und `frame_count` nur am Ende
der Spielschleife waechst (main.c:11556). `s_tc_done` ist seit dem ersten Titel 1. RE15_EXIT_AT (main.c:11342-11353)
liegt in der Spielschleife `while (running)` (main.c:5046 .. 11557) und wird im Titel nie geprueft.

## 3. Ursache des Spielendes: Leon stirbt an den zwei Adult-Spinnen (Diagnoselauf d1_diag)

d1_diag = exakt w1 + `RE15_STATE_LOG=state.txt` (main.c:7763-7809, je Spielbild Spieler-HP/Zustand und alle Aktoren)
+ `RE15_FINISHER_LOG=1` (finisher.log, game_step_common.c:721-733). Gleicher Stillstand, climb.txt bitgleich zu w1.
ROOM2060 hat zwei Aktoren Typ 0x25 (Adult-Spinne): Slot 1 Start (6112,5580), Slot 2 Start (-6600,800).
Spieler-HP (state.txt `PL(...,hp=)`, nur Aenderungen):

| Bild | HP | Spieler | beissende Spinne (Zustand im Trefferbild, Lage, ai_dist) |
|---|---|---|---|
| F0 | 100 | (7600,3950) | — |
| F166 | 80 | (4789,1138) | Slot 2 `st=1 ss1=7 ss2=6`, (3839,1714), d=1408 |
| F270 | 60 | (5057,-5430) | Slot 1 `ss1=7 ss2=6`, (4759,-4317), d=1448 |
| F424 | 40 | (5448,-7832) | Slot 1 `ss1=7 ss2=6`, (5215,-7942), d=554 |
| F486 | 20 | (4072,-7336) | Slot 1 `ss1=7 ss2=6`, (4434,-7269), d=668 |
| F543 | 0 | (947,-7336) | Slot 2 `ss1=7 ss2=6`, (1087,-6159), d=1390 |
| F626 | **-20** | (-2328,-7336) rot 3104, steht (mo=200) | Slot 2 `ss1=7 ss2=6`, (-2250,-7196), d=459 |

Danach: F627 `mo=7` (cmd-3-Tod, game_step_common.c:223 `pl->motion = 7` @0x80036778/80, Clip 7 = 113 Bilder aus
PL00.EDD), F740 `pst=7` (cmd 7 nach dem Clip-Wrap, game_step_common.c:262-267 @0x80036814), die Spinnen beissen die
Leiche weiter (hp -40 F775, -60 F884). finisher.log: `FSM-START sub=0` -> `BLACKBG sub=1 ctr=27` -> `YOUDIED sub=3`
(die Zeilen tragen keine Bildnummer). Die Game-Over-FSM (game_step_common.c:735-808) braucht 50 + 27 + 1 +
(19+3+16+3+16) + 16 + 110 = 261 Ticks bis `g_gameover_active = 1` (sub 6, `ctr >= 0x6d`, Z. 797-798); mit dem ersten
FSM-Tick in F627 (erstes Bild nach hp < 0; das FSM-START-Bild selbst schreibt finisher.log nicht mit) ist das F887 —
GEMESSEN ist: **F887** = letzte Zeile von state.txt und climb.txt (T1138), danach kein Spielbild mehr. Im selben Bild stellt main.c:7845-7872 `mode = TITLE` und
`running = 0`; frame_count++ (main.c:11556) -> 888 = der in gdb gelesene, ab da eingefrorene Wert.

Schaden je Biss 20: in den Trefferbildern steht die Spinne in `ss1=7 ss2=6`; der Port-Schadensweg der Spinne unter
dem Default-Flavor RE2 (enemy_ai_re2_zombie.c:105 `s_flavor = RE15_AI_FLAVOR_RE2`; Weiche enemy_ai_common.c:10987)
ist `re2s_attack` -> `re15_re2_player_damage_mode(pl, RE2SPIDER_BITE_DMG, 0)` mit `RE2SPIDER_BITE_DMG 20`
(enemy_ai_re2_spider.c:340/349, Kommentar `addiu a0,zero,20` @0x80105A78 RE2-Overlay). Ob Biss-Takt und -Reichweite
dem Original entsprechen, ist NICHT Gegenstand von D2 und hier nicht gemessen.

Dieselbe Signatur hatte die R-Abnahme 1 schon zweimal frueher, unbemerkt (Scratchpad `abn1/`):
* `r3_2060` (Start 18:27, EXIT_AT 2400): climb.txt `T982 … mo=7 af=1` (Tod) -> letzte Zeile `T1241 … pos=(7900,0,1318)
  mo=7 af=112`, run.log leer (kein `exit=`) — 1241 - 982 = 259 = derselbe Abstand Tod -> Titel wie w1 (T879 -> T1138).
* `r2_2030` (EXIT_AT 700): Ende im Raum 2060 waehrend des Todes-Clips (`T1223 mo=7 af=1` -> `T1243 mo=7 af=21`,
  `[flow] EXIT_AT: Bild 700 in Raum 2060 erreicht`) — das EXIT_AT kam vor dem Titel.

## 4. "Bleibt bei x -2328 stehen" und "bewegt sich nicht mehr" — keine Klemme

climb.txt w1 (identisch R3b): T849..T853 `pad=0010` Schritt 75/Bild nach Westen bis `pos=(-2328,0,-7336)`; T854 (F603)
`pad=0000 mo=200` — das Skript-Segment `U4.47` (134 Ticks, Tick 468..601, debug `[input-script] Tick 468 -> F469
Tasten 0x0010`) ist zu Ende, Leon bleibt ohne Auslauf stehen. T863..T873 `pad=0020` Drehung 2048 -> 3104 (96/Bild,
`R0.37`), T874..T882 `pad=0000` (W0.3, Tick 622..630), darin F626 Biss -> F627 Todes-Clip (climb T879 = F628 `mo=7 af=1`). Das `U0.79`-Segment danach (Tick 631..654)
trifft schon den toten Leon (`mo=7`). Es gibt an dieser Stelle weder eine Wand noch eine Klemme; die Vermutung
"derselbe Mechanismus wie in 2020 (N2)" und der Hinweis an Spur A (Klettern/Kollision) in R_abnahme_1.md sind damit
gegenstandslos.

## 5. Varianten um die Stelle

Alle mit derselben Grundumgebung, Messkopie und Waechter; "GEGNERWEG" = `RE15_WEGPLAN` mit `RAUM 2060|2061` + `GEGNERWEG`
+ `ENDE` (dachkiste_r36.c:368-380, `re15_actor_free` fuer alle Aktoren `type != 0`; im Lauf F31: `Gegner Slot 1 Typ 0x25
(6112,5580) freigegeben`, `Gegner Slot 2 Typ 0x25 (-6600,800) freigegeben`), danach steuert wieder das Skript (dessen
erste 60 Ticks `W2` = Pad 0 sind, die 30 RAUM-Bilder ueberdecken also nichts).

| Lauf | Figur / Raum | Start | Eingabe | Gegner | Ergebnis |
|---|---|---|---|---|---|
| v1_ohne_gegner | Leon, 2060 | (7600,3950) Blick 2048 | exaktes R3b-Skript | GEGNERWEG | **kein Stillstand**: HP durchgehend 100, climb 2551 Zeilen, `[flow] EXIT_AT: Bild 2300 in Raum 2060 erreicht -> exit`, run.log `exit=0` (102 s). Ohne Biss-Unterbrechungen weicht der Weg ab T540 von w1 ab (Nordgang bei z -6706 nach Westen, Westgasse nach Sueden, dann Tritt-Schub mo=18) |
| v2a_lage_ohne | Leon, 2060 | **(-2328,-7336) Blick 3104** (= Haengerlage) | `W2,U4,W1,L0.74,W0.3,XU3,W1,R0.37,W0.3,XU3,W1,R0.37,W0.3,XU3,W2` (gehen Sued, rennen Nord/West/Sued) | GEGNERWEG | **kein Stillstand**, `EXIT_AT: Bild 800` exit=0. Gehen nach Sueden: Kante z -6018 (T330), Leon gleitet an ihr nach Osten bis x -1968; Rennen Nord: Nordwand z -7832, gleitet bis x -1158; Rennen West: bis x -5132 (Westwand); Rennen Sued: bis z 3982 |
| v2b_lage_mit | Leon, 2060 | wie v2a | wie v2a | **aktiv** | **kein Stillstand**: Bisse F153/F425/F605/F722 (HP 100 -> 20), Leon ueberlebt, `EXIT_AT: Bild 1500` exit=0, kein `mo=7` |
| v3_tritt_ecke | Leon, 2060 | (7600,3950) | Wegplan `wp_tritt_ecke.txt` (+ `RE15_INPUT_SCRIPT=W200`, damit nie das echte Pad gelesen wird): Tritt obj 2 Nord bis z -7402 (F1141), Ost bis x 337 (F1507), `FLAG (3,172) = 1` (F1568), dann Haengerlage (-2292,-7349) Blick 3066, Sued in die Ecke, zurueck, Ost an den Tritt | GEGNERWEG | **kein Stillstand**: Sueden blockiert an z -6018 (`KLEMME beim Gehen nach (-2328,-4500): Rest 1518`, F1694 — Wand, das Spiel laeuft weiter, climb T1929..T1945 `pad=0010 mo=105` an derselben Lage), Ost endet vor dem Tritt bei (-738,-7345), `EXIT_AT: Bild 2600 in Raum 2060` exit=0 (113 s) |
| v4a_elza_mit | **Elza (PL04), ROOM2061** (`RE15_PSELECT_AUTO_SWITCH=1`, debug `JUMP -> 206 … (ROOM2061)`, `loaded room2061.rdt`) | (7600,3950) | exaktes R3b-Skript | aktiv | **derselbe Stillstand**: HP-Verlauf bildgleich zu Leon (F166 … F626 -> -20), Tod T879, climb endet T1138 `room=2061 … pos=(-2328,0,-7336) rot=3104 mo=7 af=118` (Elzas Clip 7 hat 119 Bilder); gdb: `g_gameflow=1`, `g_gameover_active=1`, `frame_count=0x378`, Pad `0x0010 0x0010 0x0000 0x0000`, 3/3 Treffer `re15_render_pc_title_menu`; climb ohne room/t4pos/Clip-Bild = w1 (0 Diff-Zeilen) |
| v4b_elza_ohne | Elza, 2061 | (7600,3950) | exaktes R3b-Skript | GEGNERWEG | **kein Stillstand**: HP 100, `EXIT_AT: Bild 2300 in Raum 2061` exit=0 (105 s) |

Zusammen mit Abschnitt 1: **Stillstand genau dann, wenn die Figur stirbt** (5 von 5 Laeufen mit Tod: w1, w2, w3, d1,
v4a; 0 von 5 ohne Tod: v1, v2a, v2b, v3, v4b — darunter alle Lauf-/Geh-Varianten an der Haengerstelle, mit
Tritt auf dem Record und mit Tritt in der Kerbe). Zeitpunkt jeweils 259 climb-Zeilen nach der ersten `mo=7 af=1`-Zeile.
In allen Laeufen meldet debug.log `[pad] Controller: "Xbox 360 Controller" (Slot 0)` — ein angesteckter Controller wie
im R3b-Lauf; mit aktivem Skript ersetzt input_pc.c:440-446 die Pad-Bits vollstaendig (gdb: `pad_current = 0x0010` =
Skriptwert), er wirkt also weder im Spiel noch im Titel.

## 6. Ist der damalige Stand (R-Baum vor Nachbesserung 3) noch relevant?

Nein, fuer diesen Befund nicht. R3b lief am 2026-10-11 18:46 auf der R-exe vom Stand Nachbesserung 1 (01248da5, Kopie
`re15_pc_r36abn1.exe`, sha256 `13ffe6a5…`); die Integrations-exe (R Nachbesserung 3 + alle Spuren) liefert dieselben
1138 climb-Zeilen bis auf `t4pos` (Ruhe-Ausschiebung des Tritts beim Raumstart, -3125 -> -3143) und denselben
Stillstand in T1138. Der Mechanismus (Tod durch die Spinnen -> Game-Over-FSM -> Titelschleife ohne Eingabe) beruehrt
weder den Ruhe-Zweig (LAB_8002c0b8) noch den Hoehentest (FUN_8002cabc) der Spur R; die Nachbesserung 3 hat ihn weder
verursacht noch behoben. Ohne Gegner (v1, v2a, v3, v4b) laeuft dieselbe Stelle auf der Integrations-exe ohne Stillstand.

## 7. Vorschlag

1. **Kein Spiel-/Kollisionsfix.** Es gibt keinen Haenger und keine Klemme; Tod -> Titel ist der FE-5.3-Pfad
   (main.c:7845-7872, 11558-11565). NACHLAUF.md Punkt 51 kann mit Verweis auf diesen Bericht geschlossen werden; der
   Nachtrag in R_abnahme_1.md ("Haenger der exe", "Klemme bei x -2328", Hinweis an Spur A) ist entsprechend zu lesen.
2. **Harness (nur Testhaken / Logzeile, kein Spielverhalten):**
   a. Beim Uebergang Spiel -> Titel eine Logzeile ins debug.log, z. B. `[flow] GAME OVER -> TITEL (Bild %u, Raum %04x)`
      an der Stelle main.c:7845 (`if (g_gameover_active)`). Dann sagt ein "eingefrorenes" debug.log selbst, was passiert ist.
   b. Messlaeufe mit `RE15_EXIT_AT` sollten auch dann enden, wenn die Spielschleife durch den Tod verlassen wird:
      z. B. vor `goto re_title` (main.c:11561-11565) bei gesetztem RE15_EXIT_AT `[flow] EXIT_AT: Spielende (Tod)` +
      `re15_testhaken_ende()` (main.c:2898) mit eigenem Rueckgabewert. So wird aus dem unendlichen Prozess ein
      erkennbares Ende. (Grund, warum es heute haengen bleibt: Skript mit Basis `spiel` friert im Titel ein,
      input_pc.c:70-73/397; TITLE_CONFIRM_MS ist einmalig, main.c:3511-3528; EXIT_AT nur in der Spielschleife.)
   c. Messlaeufe in Raeumen mit Gegnern: entweder `GEGNERWEG` (RE15_WEGPLAN, dachkiste_r36.c:368-380 — so fahren die
      Riegel `tritt2060`/`echt2050` der Spur R) oder `RE15_STATE_LOG` mitschreiben und die Spieler-HP pruefen. Erkennungs-
      merkmal ohne Zusatz-Log: climb.txt endet mit `mo=7 af=112` (Leon) bzw. `af=118` (Elza) genau 259 Zeilen nach der
      ersten `mo=7 af=1`-Zeile.
   d. Waechter: ein `timeout … bash lauf.sh` zeigt diesen Zustand nicht an (r3_2060: run.log leer, kein `exit=`, die
      Laufschale wurde beendet, bevor die exe von selbst endete) — Stillstand an climb.txt erkennen (wie `D2/treiber2.sh`)
      und die exe per PID beenden (`taskkill //PID <pid> //F`).
3. Offen und NICHT Teil von D2: ob Takt/Reichweite der Spinnenbisse (20 HP, RE2-Brain) in ROOM2060 dem Ziel
   entsprechen — hier nur gemessen, dass sie Leon/Elza auf dem R3b-Weg in F626 toeten und auf dem v2b-Weg bis F1500 auf
   20 HP bringen.

## 8. Messdateien

Scratchpad `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/D2/`:
`lauf_d2.sh`, `treiber2.sh` (Waechter; `treiber.sh` = Fassung 1 fuer w1/w2), `gdb_passiv.gdb`, `gdb_halt.gdb`,
`fenster.ps1`, Wegplaene `wp_gegnerweg.txt`, `wp_gegnerweg_2061.txt`, `wp_tritt_ecke.txt`; je Lauf ein Ordner
`w1_exakt`, `w2_exakt`, `w3_exakt`, `d1_diag`, `v1_ohne_gegner`, `v2a_lage_ohne`, `v2b_lage_mit`, `v3_tritt_ecke`,
`v4a_elza_mit`, `v4b_elza_ohne` (debug.log, climb.txt, objfall.txt, push.txt, run.log, ggf. state.txt, finisher.log,
wegplan.txt; bei Stillstand stillstand.txt, gdb_bt.txt, gdb_halt.txt, fenster_a/b/c.png). R-Abnahme-1-Vergleich:
`../abn1/{r3b_2060,r3_2060,r2_2030}/`. Unversioniert. Messkopie `re15_pc_d2.exe` nach den Laeufen geloescht; kein
re15_pc_d2-Prozess mehr aktiv.
Nebenwirkung im Baum: jede exe schreibt ohne Schalter `befund.log` neben sich (main.c:6185-6260, `fopen(pfad, "ab")`),
die Messkopie also in `re15_port/build/platform/pc/befund.log` (dieselbe Datei wie die parallel laufenden Testkopien).
