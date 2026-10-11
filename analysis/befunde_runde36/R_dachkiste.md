# Runde 36 — Spur R "dachkiste" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r36_dachkiste`, Zweig `r36/dachkiste`, Basis r36/integration 99398dc7
(elf integrierte Spuren + G/H/M; Spur A objfall_r36.c integriert). Diff-/Log-Bezuege gegen 99398dc7.

## Auftrag (Orchestrator, aus dem Befund der Spur H, Abschnitte 2.2/2.4 und OFFEN)

Die Kiste obj 1 auf dem DACH von ROOM1090/1091 ist die einzige Stufe zum Tor nach ROOM1100 (Tuer-Record
slot 1 @0x0213A verlangt Band 6 = Kistenoberkante y -10800; aus ROOM1100 fuehrt keine Tuer zurueck). Seit
Spur A (Objekt-Fall FUN_8002c444 in engine/src/objfall_r36.c) kann die Kiste vom Dach fallen — dann waere
das Tor unerreichbar (Softlock). Nutzer-Wortlaut der Spur A: "Das macht ein Fortschritt im Spiel unmoeglich"
— ein vom Port erzeugter Softlock ist ausgeschlossen.

1. An der exe messen, ob Leon (und Elza in ROOM1091) die Dach-Kiste im echten Spiel bis zum Fall-Ausloeser
   schieben kann (echter Weg: aus ROOM10F0 ueber den Tritt, Tuer nach 1090, Kiste in jede moegliche Richtung).
2. Am ORIGINAL messen (DuckStation, Savestate stage_saves/room1090_orig.sav): laesst das Original die Kiste
   bis zur Absprungzelle schieben (Schubpruefung FUN_8003b558 je Schritt @0x8003b634-660, Objekt-Pass
   @0x8002c0fc-c14c), oder blockt es vorher?
3. Blockt das Original -> der Port blockt an derselben Stelle mit demselben Mechanismus (Beleg @0x..., Pin).
   Laesst das Original die Kiste fallen -> byte-true belassen, Softlock als Grenze benennen, Pin auf den
   Original-Weg.
4. Dieselbe Pruefung fuer ROOM3050/3051 obj 1 und ROOM2050/2051 obj 6: haengt dort ein Fortschritt an der Kiste?

## Stand (Arbeitsprotokoll)

* 2026-10-11 Start: Baum sauber auf 99398dc7; AUFTRAG/VERTRAG/H 2.x/A Nachbesserung 3/objfall_r36.c/
  probe_r36_objfall.c/aot_common.c re15_prop_push_tick/re15_collision.c gelesen. Dossier angelegt.
* Bau: `local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)`; Messkopie
  `re15_port/build/platform/pc/re15_pc_r36r.exe`.
* Geometrie ROOM1090 (SCA, eigener Dumper `R_belege/sca_dump.py`, Layout wie rdt_common.c @RDT+0x20): s. Abschnitt
  RE-Belege. Spielstand `stage_saves/room1090_orig.sav` (Hauptbaum, nur gelesen): MZD-Disc HASH-957757946319438E,
  `@0x80026e4c = 08 00 e0 03` (Auslieferungs-EXE, nicht die gepatchte), Leon AUF DEM DACH (-5282,-9000,-1514)
  Blick 672 Band 5, obj 1 (Dach-Kiste) auf dem Record (-5960,-9000,-15482) Band 5 w0 0x101, obj 0 schon auf
  x -9836 (sub07 gefeuert). Idealer Startpunkt fuer die Original-Messung.
* Erkundungslauf exe `R_belege/lauf_r1` (JUMP 1090, Spieler am Ankunftspunkt der Tuer aus 10F0 (-13600,1300) Blick
  3072 = +Z): R dreht den Blick AUFWAERTS (3072 -> 3264/3648/4032 -> 4128 = 32 in 12 Bildern, ~96/Bild, 0.37 s
  ~ 1024), L abwaerts; Gehen ~74 Einheiten/Bild (+~444 Auslauf nach dem Loslassen); Suedwand: der Spieler bleibt
  bei z 3370 stehen = Absprungzelle r1[1] (z 3826 - 450 - Haut) BLK1 — die Karte stimmt an der exe.
* 2026-10-11 ~16:15 Fortsetzung (Vorgaenger vom Sitzungslimit beendet, Stand 085940e6): Dossier, `git log --stat
  99398dc7..HEAD`, Diff gegen 99398dc7 gelesen. Befund: die vom Orchestrator "ungeprueft" committete aot_common.c-
  Aenderung ist der 3-Zeilen-Haken des Messschriebs RE15_PUSH_LOG (dachkiste_r36.c lag UNVERSIONIERT im Baum), kein
  Verhalten; sie war gebaut (obj 13:36), aber nie gemessen. `local_build.sh configure` + `build` -> LOCAL-BUILD-OK,
  Messkopie erneuert. DuckStation `EnableGDBServer = false` geprueft. Aufgeraeumt (Punkt 3). Commit 88c2938e.
* 2026-10-11 ~16:25-16:40 Punkt 1 an der exe gemessen: 5 Sprung-Laeufe Leon (nord/west/westseite/sued/ost), 2 Elza
  (nord/west), 2 echte Wege (Leon 10F0, Elza 10F1) — Ergebnis 1.2/1.3, alle Versuche gesperrt, Zellen wie im Original.
  Messschrieb um TREFFER-ZELLE erweitert und neu gebaut (Laeufe nord/west/westseite damit wiederholt). Commit c770688c.
* 2026-10-11 ~16:45-17:10 Riegel `integration_r36_dachkiste` (leon/elza echter Weg + west) gebaut; erster Lauf scheiterte
  an der Makro-Doppelauswertung der Regex-Argumente (s. Tests), berichtigt; gezielte Unit-Gruppen 18/18 gruen.
* 2026-10-11 ~17:20 `integration_r36_dachkiste` gruen (1/1, 332.73 s). Punkt 4.3 nachgeprueft (Tuer-Zensus STAGE2:
  2070 hat KEINE Tuer nach 2040, der Weg ist 2030 -> 2060 -> 2040; Befund unveraendert). Abschluss-Commit.
  ⛔ BERICHTIGT in Nachbesserung 1: beide Aussagen waren falsch (M1/M2) — s. 4.3 und "Nachbesserung 1".
* 2026-10-11 ~17:35 Abnahme 0 (03a64b28): Punkte 1-3 erfuellt, Punkt 4 teilweise (M1 Zensus unvollstaendig, M2 Weg
  unbelegt, M3 kein Pin). Nachbesserung 1: Tuer-Zensus neu (alle Records, Ifel-Kontext), ROOM2070 = drei Gaenge (SCA),
  exe-Laeufe nb1_2000_2070m / nb1_2070n / nb1_2040treppe / nb1_2030treppe, 3050-Kiste bis zum Fall gefahren (nb1_3050fall_d,
  Vorversuche a-c Kalibrierung), Sonde mit Pfadausgabe + 3050-Pin, Riegel um fall3050/weg2040/treppe2040 erweitert,
  Dossier 4.1/4.3/Nachbesserung 1/Tests/OFFEN/Fuer den Nutzer. Commits c92f4d53 (wip) + Abschluss.
* 2026-10-11 ~18:50 Abnahme 1 (47384115): N1-N5. Nachbesserung 2 (ab ~19:00): Wegplan-Treiber RE15_WEGPLAN gebaut,
  2050-Fall an der exe gefahren (nb2_2050fall_a-c), ROOM2050 im Original per Debug-Menue geladen (room2050_orig.sav) und
  mit orig_2050_push.py gefahren (Diagnosen diag1/diag2, Laeufe a-d; d = vollstaendig), 2020-Durchgang an der exe gefahren
  (nb2_2020durchgang_b-f, Skript-Kontrolle), Sonde/Riegel/Dossier nachgezogen. Abschnitt "Nachbesserung 2".

## Punkt 1 — Messung an der exe (Leon ROOM1090, Elza ROOM1091)

### 1.1 Werkzeug (Fortsetzung 2026-10-11, nach dem Sitzungslimit)

* `RE15_PUSH_LOG=<datei>` — Messschrieb der Schubkette (NEU `engine/src/dachkiste_r36.c` + `include/re15_dachkiste_r36.h`,
  Haken = 3 Zeilen in `aot_common.c re15_prop_push_tick` direkt nach FUN_8003b558(obj,2) @0x8002bfb0/@0x8002bfb8). Je
  Schubversuch eine Zeile: Bild, Objekt, Spielerlage, die von FUN_8002cabc(Spieler,obj,1) @0x8002bfa4 gesetzte Ziel-Lage,
  Ergebnis (`SCHUB-OK` = @0x8002bfc0, `SCHUB-GESPERRT` = @0x8002c0b0 -> Ruecksetzung @0x8002c0b8-d4); bei "gesperrt" je
  getroffener Zelle eine Zeile `TREFFER-ZELLE r<q>[<i>]` (dieselbe Suche wie re15_collision_box_blocked: Quadrant
  FUN_8003b068, Band @0x8003b634-644, u0 & 2 @0x8003b64c-660, halboffenes Rechteck mit r = hx @0x8003b738-794) = das
  Gegenstueck zum Original-Haltepunkt FUN_8003da78 (Aufruf @0x8003b79c je Treffer, a0 = Zelle). Reine Diagnose, kein
  Verhalten, keine Konstante — die Werte sind die, die die Kette ohnehin berechnet.
* Zur Frage des Orchestrators (aot_common.c-Aenderung "ungebaut/ungetestet"): sie ist genau dieser Haken (Ergebnis in
  `gesperrt` halten + Aufruf), nichts Verhaltensrelevantes, also keine Byte-true-Frage. Gebraucht wird er, weil der Zensus
  (4.1) ein MODELL ist (notwendige Bedingung) und Punkt 1 die MESSUNG an der exe verlangt: ohne den Schrieb liesse sich an
  der exe nur die Kistenlage (objfall.txt) vergleichen, nicht der Versuch und seine Sperre. Beim Sitzungslimit war er
  bereits gebaut (Objektdateien 13:36, Zeichenkette RE15_PUSH_LOG in der exe), aber nie gemessen.
* Startskript `R_belege/lauf_exe.sh <lauf> KEY=WERT ...` (Grundumgebung wie tests/integration/test_r36_inventar.cmake:
  Spielstart ueber die Spielerwahl RE15_PSELECT_AUTO, Debug-Menue-Sprung; Elza = RE15_PSELECT_AUTO_SWITCH=1 -> Raum xxx1),
  Messkopie `re15_pc_r36r.exe`, je Lauf debug.log/climb.txt/objfall.txt/push.txt in `R_belege/<lauf>/`.
* Zeitachse `RE15_INPUT_SCRIPT_BASIS=spiel`: das Skript laeuft in JEDEM Raum von dessen Bild 0 neu (g_engine.frame_count
  wird beim Raumwechsel zurueckgesetzt) — so erklaert sich auch der Lauf `lauf_l1` des Vorgaengers (der 10F0-Teil
  wiederholte sich in 1090 und endete an der Saeule r1[2]). Der echte Weg nutzt das: 10F0-Teil + angehaengter 1090-Teil.

### 1.2 Laeufe (alle mit Messkopie des Stands c770688c; Original zum Vergleich aus 2.1)

| Lauf | Figur / Weg | Spieler beim Versuch (x,z,Blick) | Ziel-Lage FUN_8002cabc | FUN_8003b558 | TREFFER-ZELLE | Kiste am Ende |
|---|---|---|---|---|---|---|
| `p1_nord` | Leon, Sprung 1090, Start (-5887,-12500) Blick 1024, U3 / U1.5 / U1.5 | (-5887,-14207) 1024 | (-5960,**-15557**) | GESPERRT x3 (F89/F174/F234) | r3[4] x -7256..4624 z -18560..-16403 typ 1 u0 0xff fl 0x53 | (-5960,-9000,-15482) b5 |
| Original NORD/NORD-2 | | (-5887,-14208) / (-5551,-14204) | (-5960,-15558) / -15554 | gesperrt | 0x80123984 = r3[4], gleiche Bytes | Record |
| `p1_west` | Leon, Sprung, Tasche (-4610,-15155) Blick 2112, U2 / U1.5 / U1.5 | (-4684,-15092) 2112 | (**-6034**,-15482) | GESPERRT x2 (F69/F144) | r3[2] x -9200..-6878 z -18524..265 u0 0xff | Record |
| Original WEST | | (-4685,-15095) 2112 | (-6035,-15482) | gesperrt | r3[2] | Record |
| `p1_westseite` | Leon, Sprung, an der Westwand (-6410,-13919) Blick 1024, U3 / U1.5 / U1.5 | (-6410,-14207) | (-5960,-15557) | GESPERRT x3 | r3[4] | Record |
| Original WESTSEITE | | (-6412,-14208) | (-5960,-15558) | gesperrt | r3[4] | Record |
| `p1_sued` | Leon, Nordtasche (-3853,-15852) Blick 1024, U2, R0.53 (Blick 2560), U3 | kein Versuch (Spieler endet (-6410,-11164)) | — | — | — | Record |
| Original SUED/-DIAGONAL | | kein Versuch (Ende (-6410,-11532)) | — | — | — | Record |
| `p1_ost` | Leon, (-6410,-14132) Blick 0 (Ost), U3 | kein Versuch (laeuft an der Suedflanke entlang bis x 340) | — | — | — | Record |
| Original OST-VERSUCH | | kein Versuch (bis x 518) | — | — | — | Record |
| `e1_nord` | **Elza**, Sprung -> ROOM1091, wie p1_nord | (-5887,-14207) | (-5960,-15557) | GESPERRT x3 (F89/F174/F234) | r3[4] | Record |
| `e1_west` | **Elza**, ROOM1091, wie p1_west | (-4684,-15092) | (-6034,-15482) | GESPERRT x2 | r3[2] | Record |
| `p1_echt` | **Leon, ECHTER WEG** (s. 1.3) | (-6410,-14206) / (-6417,-14206) Blick 1088 | (-5960,**-15556**) | GESPERRT x3 (F1611/F1651/F1711) | r3[4] | Record (F1995) |
| `e1_echt` | **Elza, ECHTER WEG** 10F1 -> 1091 | (-6410,-14206) / (-6417,-14206) | (-5960,-15556) | GESPERRT x3 (F1611/F1651/F1711) | (Lauf mit der Messkopie vor dem Zellen-Schrieb; Zellen im Riegel-Lauf `elza`: r3[4]) | Record |

Ergebnis an der exe: **0 angenommene Schuebe, 0 Fall-Ausloeser (objfall.txt ohne `p=1 AUSLOESER`), die Kiste steht in
jedem Lauf am Ende exakt auf dem Record (-5960,-9000,-15482) Band 5** — fuer Leon und fuer Elza. Die Sperrzelle ist
Versuch fuer Versuch dieselbe wie im Original (r3[4] Bruestung fuer jeden Schub nach Norden, r3[2] Westwand fuer den
Schub nach Westen), die Ziel-Lage ist in beiden Spieler +- 1350 auf der Achse des groesseren |d|.

Abweichung 1 Einheit: Spieler-z beim Versuch -14207 (Port) gegen -14208 (Original NORD) bzw. -14206 im echten Weg; die
Ziel-Lage folgt daraus (-15557/-15556 gegen -15558). Das ist die Schrittweite des Spieler-FSM in genau diesem Bild
(Laufmodell / unterschiedliche Anlaufstrecke: das Original lief aus z -13697 an, der Sprung-Lauf aus -12500), nicht die
Schubkette — die Sperre ist in beiden Faellen dieselbe Zelle mit 54..56 Einheiten Ueberdeckung (Ziel-Nordkante -16456..
-16458 gegen Zell-Suedkante -16403). Nicht Gegenstand dieser Spur (s. OFFEN).

### 1.3 Echter Weg (Leon `p1_echt`, Elza `e1_echt`): ROOM10F0/10F1 -> Tritt -> Tuer slot 1 -> ROOM1090/1091 -> Korridor -> Kiste

Skript (BASIS=spiel, START=1): `W10,U2.6,W1,L0.37,W0.3,U0.5,W0.3,R0.37,W0.3,U0.55,W0.3,R0.37,W0.5,U10.5,W1,A0.1,W2.5,U0.6,
W0.3,A0.1,W3` (10F0-Teil des Vorgaengers, `lauf_l1`) + `W10,R0.35,W0.3,U8,W0.5,U1.5,W0.5,U1.5,W2` (1090-Teil).
* 10F0 (Leon): Tritt obj 0 (p=3) nach Osten geschoben bis in die Kerbe — objfall.txt `F759 room=10F0 p=3 KERBE m0b=10
  pos=(-4785,0,216)`; Klettern — climb.txt `T1121 ... pad=8000 pos=(-4899,0,-58) sub=9 kind=0x81 obj=3` -> `T1152
  pos=(-4899,-1800,-1206) band=1 stand=3`; Tuer — debug.log `[aot] DOOR FIRE slot=1 rect=(-4750,-1750,hw=600,hh=500)
  target_cut=0 spawn=(-13600,-9000,1300)`, `[room] PC loaded room1090.rdt`.
  10F1 (Elza, PL04): `F775 KERBE m0b=10 pos=(-4808,0,207)`, `T1121 sub=9 kind=0x81 obj=3` -> `T1152 band=1 stand=3`,
  derselbe DOOR FIRE, `loaded room1091.rdt`.
* 1090/1091: die Wiederholung des 10F0-Teils endet an der Saeule r1[2] ((-5282,1937) Blick 32, wie lauf_l1); R0.35 ->
  Blick 1088 (climb.txt rot=5184 = 1088 + 4096), U8: den Korridor hinauf, mit 64/4096 Drift nach Westen bis an die
  Westwand r3[2] (ab z ~ -13000 gleitend x -6410..-6424 = -6878 + 450 +- Haut); Versuch bei z -14206 (Spieler 74 in die
  Kiste hinein), dann Ausschiebung @0x8002c0d8-e8 auf z -14132 (= -14582 + 450). Drei Versuche, alle gesperrt, Kiste
  Record. Leon und Elza bildgleich (F1611/F1651/F1711, gleiche Zahlen).
* Erster Versuch des echten Wegs (verworfen, nicht archiviert): mit frischem Spielstand startet in Leons ROOM10F0 die
  Szene der Runde 35 Spur K (`[cut10f0] ROOM10F0: Szene gestartet (Ereignis 20, Faden 10, Flag (9,71)=0)`), das Skript
  lief in die Szene hinein (player_mode 2 bis ueber F3300, kein Tuer-Ereignis). Der Erstbesuch enthaelt die Szene;
  gemessen wurde der Zustand "Szene gesehen" ((9,71)=1 per `RE15_SET_FLAG=9:71`, wie Lauf B des r35-Riegels: kein
  `[cut10f0]`). Elza hat in 10F1 keine Szene. S. OFFEN (Szene + Eingaben).

## Punkt 2 — Messung am Original (DuckStation, room1090_orig.sav)

Werkzeug `R_belege/orig_1090_push.py` (Muster analysis/befunde_runde35/H_raeume/re2_mess/re2_gdb_grab.py + Skill
re15-room-capture): DuckStation `-batch -statefile room1090_orig.sav <MZD-cue>`, GDB-Server 127.0.0.1:2345
(settings.ini gesichert, EnableGDBServer/ConfirmPowerOff/LogToFile fuer die Sitzung gesetzt, am Ende aus der Sicherung
zurueckgeschrieben — geprueft `EnableGDBServer = false`), virtueller Pad (vgamepad/ViGEm). Haltepunkte `Z0` auf
@0x8002bfc0 (Schub angenommen), @0x8002c0b0 (Schub gesperrt, vor der Ruecksetzung @0x8002c0b8-d4) und @0x8002c13c
(Fall-Ausloeser); je Halt obj 1 (0x800b3f98+0x94: +0x34/+0x38/+0x3C, +0x04, +0x05, +0x82, +0x8C) und Spieler
(0x800aca54: +0x34/+0x3C, +0x6A, +0x82, +0x05) aus der RAM. Navigation geschlossen geregelt (Blick/Lage lesen, Pad
drehen/laufen).

Werkzeug-Befunde (gemessen, `R_belege/orig_diag*/log.txt`):
* DuckStation antwortet auf ^C mit `S02` (anders als in H_raeume r35 notiert) — Stopp-Antworten werden weggelesen.
* `ConfirmPowerOff = true` haelt das geordnete Schliessen (taskkill ohne /F) im Dialog — fuer die Sitzung false.
* Am Bus hingen ZWEI XInput-Geraete (DuckStation-Log: "Controller 2 inserted -> SDL-0", "Controller 1 -> SDL-1"; ohne
  laufendes Skript zeigt Get-PnpDevice EIN "Xbox 360-Controller fuer Windows" USB\VID_045E&PID_028E) — der eigene
  Pad wurde SDL-1 und kam nie im Spiel an (Padwort 0x800ac768 = 0, Spieler stand). Fuer die Sitzung werden die
  [Pad1]-Bindungen zusaetzlich an SDL-1/SDL-2 gebunden (`pad_bind_alle`); danach: UP -> 0x800ac768 = 0x11, Leon laeuft
  (z -1514 -> -3549 -> -5568 -> -7587 je Sekunde, dabei an der Saeule r1[2] entlang: x bleibt -5282 = -4814-450-18),
  RIGHT 1 s -> Blick 672 -> 3456 (= +2784 = +96/Bild, derselbe Drehsinn wie der Port).
* Die Emulation laeuft nach `c` (Haltepunkt FUN_8002bd44 @0x8002bd44 trifft je Bild, `S05`).

### 2.1 Messung am Original — Ergebnis (`R_belege/orig_lauf5/log.txt` alle Plaene, `orig_lauf6/log.txt` NORD mit Treffer-Zelle)

Leon aus dem Spielstand ((-5282,-1514) Band 5, Blick 672) durch den Korridor x -6878..-4814 nach Norden vor die
Kiste, dann die vier moeglichen Angriffe (jeder Schubversuch = Zaehler obj+0x8C == 9, `SCHUB-GESPERRT` = Halt
@0x8002c0b0 VOR der Ruecksetzung, `TREFFER-ZELLE` = Halt in FUN_8003da78, a0 = die sperrende Zelle):

| Plan | Spieler beim Versuch (x,z,Blick) | Kisten-Ziel-Lage (FUN_8002cabc) | FUN_8003b558 | Treffer-Zelle | Kiste danach |
|---|---|---|---|---|---|
| NORD (von Sueden, UP 4 s) | (-5887,-14208, 1024) | (-5960,**-15558**) = Spieler z - 1350 | gesperrt | **0x80123984 = r3[4] Bruestung** x -7256..4624 z -18560..-16403 u0 0xff fl 0x53 | (-5960,-15482) (Ruecksetzung) |
| NORD-2 (loslassen, erneut) | (-5551,-14204, 1024) | (-5960,-15554) | gesperrt | r3[4] | (-5960,-15482) |
| WEST (von Osten, x -4685, z -15095, Blick 2112) | (-4685,-15095) | (-6035,-15482) = Spieler x - 1350 -> von FUN_8003da78 auf -5960 zurueckgeschoben | gesperrt (lauf5) | r3[2] Westwand (Ziel x < -5978) | (-5960,-15482) |
| WESTSEITE (x -6412 an der Wand, Blick 1024, UP 3 s) | (-6412,-14208) | (-5960,-15558) | gesperrt | r3[4] | (-5960,-15482); Spieler bleibt bei z -14132 |
| OST-VERSUCH (von x -6410, Blick 0, UP 3 s) | laeuft an der Suedflanke entlang bis x 518 | — kein Versuch (Reichweite verfehlt) | — | — | (-5960,-15482) |
| SUED (Nordtasche x -4250 z -15900, UP 2 s, dann Blick 2560) | Spieler z >= -15953 (Bruestung), kein Versuch | — | — | — | (-5960,-15482) |

Ergebnisse: **0 angenommene Schuebe, 0 Fall-Ausloeser**; die Kiste steht am Ende exakt auf dem Record
(-5960,-9000,-15482), obj+0x04 = 0, Band 5. Zwischenbefund zur Zahl -15485 in `SCHUB-GESPERRT`: FUN_8003da78
(Aufruf @0x8003b79c bei jedem Zellentreffer, a0 = Zelle, a1 = Objekt) schiebt das Objekt auf die Zellkante plus 18
(`addiu t0,v0,18` @0x8003dab0 — dieselbe +0x12-Haut wie push_rect): -16403 + 900 + 18 = -15485; die Ruecksetzung
@0x8002c0b8-d4 macht das ohnehin rueckgaengig (s4/s5 = (-5960,-15482) im Halt). Der Schub ist im Original also nicht
"21 Einheiten bis zur Bruestung", sondern von Anfang an ganz gesperrt, weil der erste Schubschritt (Spieler-Schritt
~76 > 21 Luft) die Kiste in die Bruestungszelle setzt.
Spieler-Boxen (gelesen): +0x78 @0x80073e94 = (0,-1530,0,450,1530,450) (FUN_8002cabc nimmt hx = 450 @0x8002cb3c-4c),
+0x7C @0x800b2354 = (0,-1530,0,0,0,0) (Mitte cx/cz = 0 @0x8002cb10/@0x8002cb28). Also Kiste = Spieler +- 1350 wie im Port.

## RE-Belege

### B1 — Geometrie ROOM1090/1091 (SCA @RDT+0x20, 12-B-Zellen; Bandnibble = floor>>4; `R_belege/sca_dump.py`)

Typwort einer Zelle (FUN_8003b7f0 `lhu v0,0(a1)` @0x8003b900 = Zelle+10) = u1 | floor<<8. Zwei Praedikate:
* **Absprung** (Objekt-Pass @0x8002c130 `andi v0,v0,0x2` auf das Typwort) = u1 & 2.
* **Schubsperre Klasse 2** (FUN_8003b558 @0x8003b64c-660: `lhu v0,6(s0)` = Zelle+8, `sll 16 / sra 24` = u0,
  `and v0,s6,v0` mit s6 = a1 & 0xff = 2) = u0 & 2. Spielerwand = u0 & 1 (collision_constrain_impl, Maske 1).

Band-5-Zellen (Dach), die beides sperren (u0 0xff): r2[0]=r3[4] Bruestung x -7256..4624 z -18560..-16403
(@0x01F20/@0x01FD4); r3[2] Westwand x -9200..-6878 z -18524..265 (@0x01FBC); r3[3] x -15464..-6878 z -1234..491
(@0x01FC8); r2[6]=r3[13] Lueftungsblock x -3400..3400 z -17400..-15397 (@0x01F68); r2[1]=r3[6] x -4538..3994
z -5276..-3602; r2[2]=r3[7] Ostwand x 2752..4552 z -17120..-3584; r1[2]=r3[1] Saeule x -4814..-2718 z -5184..4300;
r1[3]=r3[5] x -16616..-14672 z -722..4819; r1[4] x -8191..-2726 z 3432..4425; r1[5] x -16400..-11504 z 3404..4401.
Absprungzellen Band 5 (u0 0x01 = sperren NUR den Spieler, u1 0x0e, Wort 0x530e): r0[4]=r1[14]=r2[7]=r3[16]
x -4100..3100 z -3600..7896 (@0x01E18); r1[1] x -11752..-8058 z 3826..8129 (@0x01E3C). Beide liegen SUEDLICH der
Dachplatte; die Kiste muesste von z -15482 um >= 11432 nach Sueden (Mitte z >= -4050 bzw. >= 3376).

Lage der Kiste obj 1 (Record @0x02190: (-5960,-9000,-15482), Box c(0,-900,0) h(900,900,900), r = hx = 900
@0x8003b68c-690): Westflanke x -6860 gegen die Westwand r3[2] (Ostkante -6878): **18 Einheiten** Luft (Mitte
>= -5978, `sltu` @0x8003b764 halboffen); Nordflanke z -16382 gegen die Bruestung (Suedkante -16403): **21 Einheiten**
(Mitte >= -15503). Oestlich bis zum Lueftungsblock: Mitte < -4300 (1660 Einheiten).

### B2 — Schubkette FUN_8002bd44 (selbst disassembliert, re15_disasm.py dis 0x8002bf14 76 / 0x8002bea8 28)

```
8002bf14: lbu v1,8(s2) / bne v1,v0(=4)          Typ 4
8002bf24: lw v0,-32(s6) / andi 0x1 / beq        UP gehalten (0x800ac768 & 1)
8002bf38: lbu v1,846(s6) / lbu v0,130(s2) / bne  Spielerband 0x800acad6 == obj+0x82
8002bf50: jal 0x8002d1e8 (a0=Spieler, a1=obj, a2=0)  Reichweite: Punkt 563 voraus, |dx|,|dz| <= 900
8002bf60-74: obj+0x8C++ / = 0
8002bf78-84: obj w0 & 0x80 -> 0x8002c0b8 (Ruecksetzung)
8002bf8c-94: obj+0x8C != 9 -> 0x8002c0b8
8002bfa4: jal 0x8002cabc (Spieler, obj, a2=1)  SCHUB: obj = Spieler +- (hx+450) auf der Achse |dx| vs |dz|
8002bfb0: jal 0x8003b558 (obj, a1=2)           Schubpruefung Klasse 2
8002bfb8: bne v0,zero,0x8002c0b0                gesperrt -> 8002c0b0: sb s7(=10),140(s2); 8002c0b8-d4 Ruecksetzung
8002bfc0: sb v0(=8),140(s2)                     angenommen
8002bfc4-dc: 0x800aca3c |= 0x2000               Kontaktbit
8002bfe0-e4: 0x800aca59 != 8 -> 0x8002c0b8      nur im Spieler-Substate 8 bleibt der Schub
8002c0d8-e8: jal 0x8002cabc (Spieler, obj, 0)   DANACH Spieler aus der Kiste schieben
```
Reihenfolge im Bild: Spieler-FSM (geht in die Kiste hinein) -> Schub (Kiste = Spieler +- 1350) -> Ausschiebung.
Darum kann die Kiste nur in die Richtung wandern, in die der Spieler laeuft, und nur, wenn der Spieler auf der
GEGENSEITE steht (|d_axis| = 1350 bei Beruehrung; Achse = die mit dem groesseren |d|, @0x8002cce8-d10).
FUN_8003b558 (dis 0x8003b558 160): @0x8003b634-644 Bandnibble == obj+0x82; @0x8003b64c-660 u0 & a1; @0x8003b668-674
obj w0 & 8 -> uebersprungen; @0x8003b684-690 hx == hz -> r = hx; @0x8003b738-768 (u32)(cx - (x0 - r)) < w + 2r;
@0x8003b770-794 dito z; Treffer -> FUN_8003da78 + s5 |= 1; Rueckgabe s5. Der Port (re15_collision_box_blocked,
re15_collision_push_prop, re15_prop_reach_test, re15_prop_push_tick) ist dieselbe Kette; w0 & 8 fuehrt der Port
nicht (kein Objekt der Auslieferung traegt es: Installer-Flags pc[6..7]|1).

### B3 — Objekt-Pass-Ausloeser (dis 0x8002c0ec 28)
```
8002c0fc: lbu v1,8(s2) / bne v1,4     8002c110: lbu v0,4(s2) / bne (faellt schon)
8002c120: addiu a0,s2,52 / lbu a2,130(s2) / jal 0x8003b7f0 / ori a1,zero,0x1c2
8002c130: andi v0,v0,0x2 / beq -> 0x8002c150       8002c13c: jal 0x8002d3d8 / sb 1,4(s2) / sb zero,5(s2)
```

### B4 — Folgerung (zu MESSEN, nicht anzunehmen)
Eine Bewegung nach Sueden (+Z) verlangt einen Spieler NOERDLICH der Kistenmitte um >= 1350 - v (Beruehrung nach dem
FSM-Schritt), also Spieler-z <= -16832 + v; die Bruestung r2[0] laesst den Spieler (Radius 450) nur bis z >= -15953
(x >= -7706), und westlich davon sperrt r3[2] jeden Spieler mit x < -6428 fuer z < 715. Also gibt es KEINE
Spielerlage fuer einen Schub nach Sueden, solange die Kiste an der Bruestung steht — und sie kann die Bruestung nur
verlassen, wenn sie nach Sueden geschoben wird. Erwartung: Kiste bleibt in x [-5978, -4300), z [-15503, -15482].

## Punkt 3 — Umsetzung

**Das Original blockt (2.1) — und der Port blockt an derselben Stelle mit demselben Mechanismus, OHNE Aenderung am
Verhalten** (1.2/1.3, Versuch fuer Versuch verglichen). Die Kette ist seit Runde 29/30 portiert und war nur nie an
der Dach-Kiste gemessen:

| Original | Port | Beleg |
|---|---|---|
| Schubkette FUN_8002bd44 @0x8002bf14-c0e8 (Typ 4, UP, Band, Reichweite 563/900, Zaehler 9) | `aot_common.c re15_prop_push_tick` | B2 |
| FUN_8002cabc(Spieler,obj,1) @0x8002bfa4: Kiste = Spieler +- (hx+450) auf der Achse des groesseren \|d\| @0x8002cce8-d10 | `re15_collision_push_prop` (re15_collision.c:1037) | B2, Ziel-Lagen in 1.2 identisch bis auf den Spieler-Schritt |
| FUN_8003b558(obj,2) @0x8002bfb0: Band @0x8003b634-644, u0 & 2 @0x8003b64c-660, r = hx @0x8003b68c-690, halboffenes Rechteck @0x8003b738-794 | `re15_collision_box_blocked(rdt, x, z, band, hx, 2u)` (re15_collision.c:1069) | B2; TREFFER-ZELLE in 1.2 = Zelle des Original-Haltepunkts FUN_8003da78 (0x80123984 = r3[4]) |
| gesperrt -> obj+0x8C = 10 @0x8002c0b0, Ruecksetzung @0x8002c0b8-d4 | `s_prop_push_count[p] = 10; continue;` (Lage nie geschrieben) | B2 |
| angenommen -> 8 @0x8002bfc0, Kontaktbit @0x8002bfd4, nur Substate 8 haelt den Schub @0x8002bfe4 | `= 8; s_prop_push_contact = 1; if (!in_sub8) continue;` | B2 |

Kein Fix noetig, keine neue Konstante, kein Verhaltens-Code geaendert. Geaendert/neu (alles Diagnose und Riegel):
* NEU `engine/src/dachkiste_r36.c`, `include/re15_dachkiste_r36.h` — Messschrieb RE15_PUSH_LOG (1.1); seit Nachbesserung 2
  auch der Wegplan-Treiber RE15_WEGPLAN (geschlossen geregelte Mess-Steuerung, Abschnitt "Nachbesserung 2").
* `engine/src/aot_common.c` re15_prop_push_tick: 3 Zeilen Haken ("Runde 36 Spur R: RE15_PUSH_LOG") hinter FUN_8003b558.
* `platform/pc/src/input_pc.c`: 2 Zeilen Haken ("Runde 36 Spur R: RE15_WEGPLAN") hinter der RE15_INPUT_SCRIPT-Ersetzung.
* `tests/unit/probe_r36_dachkiste.c` (Vorgaenger, Zensus mit Spieler, 4.1) + `tests/unit/probes/r36_dachkiste.cmake`
  (+ `integration_r36_dachkiste`), NEU `tests/integration/test_r36_dachkiste.cmake` (Tests).
* `analysis/befunde_runde36/R_belege/lauf_exe.sh` (Startskript), Laufordner `p1_*`, `e1_*`.
* Aufgeraeumt: das vom Orchestrator doppelt verschachtelt committete `R_belege/analysis/.../lauf_d2` (byteidentisch mit
  `lauf_d1`) und 9x `settings.ini.bak` (DuckStation-Sicherungen mit lokalem Nutzerpfad; der Beleg "zurueckgeschrieben,
  EnableGDBServer = false" steht in jedem `orig_lauf*/log.txt`; im Hauptbaum geprueft: `EnableGDBServer = false`).

## Punkt 4 — ROOM3050/3051 obj 1, ROOM2050/2051 obj 6

### 4.1 Zensus MIT Spieler (Sonde `tests/unit/probe_r36_dachkiste.c`, `R_belege/zensus_diag.txt`)

Die Freiraum-Zensus der Spur A flutet nur die Kiste. Diese Sonde fuehrt den Spieler mit: Zustand (Kistenzelle,
Spielerseite), Schub nur von der Gegenseite (Kiste = Spieler +- (hx+450), @0x8002cd30-44/@0x8002cd98-ac), Spieler-
Wandklemme Radius 450 Maske 1 (grosszuegig 432), Schubstellung axial 1350..1463 (Reichweite 900 + 563 voraus,
@0x8002d204/@0x8002d224), seitlich bis +-1238, Spieler-Freiraum je Kistenlage exakt komprimiert geflutet. Start =
Tuer-Ankunft des Raums. Ergebnis (Auslieferung, obere Lage):

| Raum / Objekt | Band | Start Spieler | erreichbare Kistenzellen | Ausloeser | Kistenlage (Mitte) |
|---|---|---|---|---|---|
| 1090 / 1091 obj 1 | 5 | (-13600,1300) aus 10F0/10F1 | 12 | **0** | x [-5978,-5959) z [-15503,-15481) — Seiten W und N ohne Schubstellung |
| 3050 / 3051 obj 1 | 4 | (-3596,1523) aus 30E0 | 13373 | **8** (erste (-23875,-18500), Spieler NOERDLICH) | x [-24100,-1800) z [-22350,1123) |
| 2050 / 2051 obj 6 | 3 | (9100,4100) aus 2000/20B0 | 3898 | **1** ((7550,-12000), Spieler SUEDLICH) | x [-6650,9000) z [-11700,-5800) |
| 2050 obj 6 nach sub02 (Zelle 0x33 u0 0, 0x32 u0 0xff) | 3 | (9100,4100) und (-6800,-5650) | 3898 | 1 (dito) | dito |

(Zahlen seit Nachbesserung 1: die Sonde fuehrt den Seitenwechsel auf derselben Kistenzelle als eigenen Zustand und gibt
den Schub-PFAD zur ersten Ausloeser-Lage aus; vorher 9 / 12729 (7) / 3575 (1) — gleiche Aussage. Pfad 3050: Spieler
oestlich, Schub nach Westen von x -18300 bis -23875; Seitenwechsel nach Norden; Schub nach Sueden von z -20800 bis -18500
= Absprungzelle. Pfad 2050: N/W/N/W-Schuebe bis (7550,-7150), dann von Sueden nach Norden bis (7550,-12000).)

### 4.2 ROOM3050/3051 obj 1 — der Fall ist der VORGESEHENE Weg, kein Fortschritt haengt an der oberen Lage

* Records (ROOM3050.RDT): obj 1 oben @0x01F1A Band 4 (-18000,-7200,-20800); unter `Ck (3,93)=1` @0x01F40 Band 0
  (-24000,0,-22200) = die GEFALLENE Lage; Latch sub01 @0x02184-2196: `Ck (3,93)=0 / Work_set 3,1 / Member_cmp 15 == 7 /
  Set (3,93)=1` — Kerbe = Aot slot 7 @0x01F06 (sce 5, floor 0, Rechteck x -25000..-22950, z -23200..-21150, unter
  `Ck (3,93)=0` @0x01F02). Absprungzelle Band 4: r*[46] x -25000..-22950 z -18050..-16800 Wort 0x4002 (u0 0x01 = sperrt
  nur den Spieler).
* Was haengt an der Kiste: `Item_aot_set slot 6 @0x01EC6` floor 1 (= Oberkante der gefallenen Kiste, y -1800) Rechteck
  x -24800..-23000, z -23700..-22900, Item 0x07 = **SUPER REDHAWK** (s_item_names[7]), Menge 6 — nur vom Dach der
  gefallenen Kiste erreichbar. Beide Tueren brauchen die Kiste NICHT: slot 0 @0x01E86 floor 0 -> ROOM3040, slot 1
  @0x01EA6 floor 4 (Kistenband selbst, keine Oberkante) -> ROOM30E0.
* Folge: der Fall (Spur A) oeffnet hier den Weg zur Magnum, er sperrt nichts. Der Zensus belegt, dass der Spieler die
  Kiste wirklich bis zur Absprungzelle bringt (von Norden; Fallrichtung FUN_8002d3d8 = vom Spieler weg = +Z).

### 4.3 ROOM2050/2051 obj 6 — Tuer nach ROOM2040 braucht die Kistenoberkante; ROOM2040 (Halle) ist ohne die Kiste erreichbar (BERICHTIGT, Nachbesserung 1)

* Records (ROOM2050.RDT main00): obj 6 @0x01D0E Band 3 (-1600,-5400,-9348) rot 2048, unbedingt; `Door_aot_set slot 2
  @0x01C8A floor 4` (Rechteck x -8200..-7200, z -6200..-5000) -> ROOM2040 (21700,-5400,-5400) Band 3 — floor 4 = Band 3 + 1 =
  Oberkante der Kiste obj 6 (dieselbe Mechanik wie in 1090). Die Tuer-Flaeche liegt WESTLICH der Band-3-Wand r*[51]
  (x -7550..-6150, z -4500..850), die sub02 @0x02294-22A4 per `Sca_id_set r*[0x33] u0 = 0` erst oeffnet (Raetsel: drei
  Kisten obj 1/2/3 (Band 0) in die Kerben slot 3/4/5 -> (5,0..2) -> sub01 @0x0201A `Aot_reset 6 -> sub02`).
* Absprung: Band-3-Zelle r*[49] x 6350..8750 z -13950..-12150 Wort 0x3202; darunter r*[23] Band 2 x 4550..6350 Wort 0x2102
  und r*[77]/[78] Band 1 Wort 0x1002 — eine KASKADE von Absprungzellen bis Band 0: die Kiste soll dort hinunter. An der
  Kante: Aot slot 7 @0x01E3C/@0x01EC6 (floor 3, x 6800..8300, z -12000..-11200; sce 0 bzw. sce 1 Nachricht nach (3,104))
  und slot 6 @0x01E28 (floor 3, x 4700..5500, z -11550..-10750, sce 3 -> sub04: Nachricht, (3,104), obj 4 Member 1 (y)
  -16200 -> -1400, Kameras). sub02 sperrt nach dem Raetsel r*[0x32] (x 6650..8550, z -12150..-11650) mit u0 0xff.
* Was an der Kiste haengt: NUR die Tuer slot 2 (2050 -> 2040 Halle). Alle anderen Tueren von 2050 (slot 0 @0x01C4A floor 3
  -> 2000, slot 1 @0x01C6A floor 3 -> 2020) liegen auf dem Spielerband 3.
* **Tuer-Zensus STAGE2 (BERICHTIGT, `R_belege/tueren_zensus.py` -> `stage2_tueren.txt`, alle Door_aot_set aus main UND subs
  mit Bedingungskontext; Auszug `stage2_tueren_2040.txt`):** nach ROOM2040 fuehren ROOM2050/2051 slot 2 @0x01C8A (floor 4,
  Kiste; Ankunft Halle Band 3 (21700,-5400,-5400)), ROOM2060/2061 slot 1 @0x00DBA (floor 1; Ankunft Halle Band 3
  (-12850,-5400,1600)) und **ROOM2070/2071 slot 5 @0x00B1C** `3b 05 02 31 00 00 88 13 18 b1 e8 03 d0 07 da b2 d8 dc 76 b2
  00 00 01 04 0c 05 ...` (floor 0, UNBEDINGT — nach dem Ifel/Else-Block @0x00ABA-0B1A, vor dem Switch @0x00B3C; Ankunft
  GALERIE Band 5 (-19750,-9000,-19850), cut 12); Gegenstueck ROOM2040 slot 4 @0x0131C (floor 5 -> 2070 cut 9). Der
  Vorgaenger-Eintrag "2070 slot 5" war richtig; die Aussage des Stands f487909d ("2070 hat KEINE Tuer nach 2040") war
  falsch, weil der alte Zensus nur die vorderen Records des main00 nahm (Abnahme 0, M1).
* **ROOM2070 besteht aus DREI getrennt ummauerten Gaengen** (SCA Band 0, `sca_dump.py ROOM2070.RDT 0`): Mittelgang
  z -8200..-4600 (Waende r0[0..3]: slot 0 @0x00A3A -> 2000, slot 1 @0x00A5A -> 2030), Suedgang z 4650..8100 (r0[4..7]:
  slot 2 -> 2030, slot 3 -> 2000), Nordgang z -21000..-17450 (r0[8..11]: slot 4 @0x00AD6/@0x00AFA -> ROOM11A0 (STAGE1;
  sce 0 + Ereignis slot 7 solange (3,151)=0, gesetzt in 2070 sub02 @0x00CE4), slot 5 @0x00B1C -> 2040). Slot 5 ist also
  NUR aus dem Nordgang erreichbar — Ankunft aus ROOM11A0 slot 0 @0x00F5A (-18650,0,-19300) — und NICHT ueber ROOM2000
  slot 3 @0x0167E, dessen Ankunft (-18650,0,-6450) im Mittelgang liegt. An der exe gemessen: `R_belege/nb1_2000_2070m`
  (JUMP 2000, Spieler (-20800,-6200) Band 1 Blick 0: `DOOR FIRE slot=3 rect=(-19800,-6200,hw=500,hh=2000) target_cut=3
  spawn=(-18650,0,-6450)`, `loaded room2070.rdt`, Spieler danach bei (-7112,0,-7732) Band 0 im Mittelgang) und
  `R_belege/nb1_2070n` (JUMP 2070, Spieler (-18650,-19300) Band 0 Blick 0 = 11A0-Ankunft: nach 300 Bildern Osten
  `DOOR FIRE slot=5 rect=(5500,-19200,hw=500,hh=1000) target_cut=12 spawn=(-19750,-9000,-19850)`, `loaded room2040.rdt`,
  Spieler in 2040 1200 Bilder lang Band 5). Der in Abnahme 0 vorgeschlagene Weg "2000 slot 3 -> 2070 -> slot 5 -> 2040"
  ist deshalb KEIN Laufweg (M1-Nachbesserung: die Records stimmen, die Mauer trennt sie).
* **Band-Wechsel in den Raeumen (Abnahme 0, M2):**
  - ROOM2040: Treppe `Aot_set sce 13` slot 5 @0x0133C (floor 3) / slot 6 @0x01350 (floor 5), gleiches Rechteck x -2350..-350
    z -18250..-13950 = die beiden Enden EINER Treppe (Port stair_common.c, re15_aot.h: sce 12/13). An der exe gemessen
    `R_belege/nb1_2040treppe` (JUMP 2040, Spieler (-1350,-19200) Band 5 Blick 3072, Viereck in der Zone): stair_dbg.log
    `[STAIRREG] slot=5 type=13 ... centre=(-1350,-16100) half=(1000,2150)`, `[stair] START dir=DOWN cur_band=5
    target_band=3`, climb.txt danach `pos=(-1359,-5400,-13782) ... band=3` und zu Fuss weiter bis (-1359,-5400,-10068)
    Band 3 = die Halle. Galerie (Band 5, Ankunft aus 2070) und Halle (Band 3, Ankunft der Kisten-Tuer aus 2050 und der
    Tuer aus 2060) sind verbunden.
  - ROOM2060: Tritt obj 2 (3,172): 0 -> (-3125,0,2075) @0x00EA6, 1 -> (475,0,-7375) @0x00ECC — an der Tuer slot 1 @0x00DBA
    (floor 1, x -200..1100 z -8850..-7850): Band 0 -> 1 durch Klettern. Von Spur A an der exe gemessen (A_klettern.md
    Z. 231: `ROOM2060 flag=1 obj=2 ... oben=(525,-1800,-7473) band=1(soll 1) -> OBEN`; Sonde probe_r36_klettern_zensus
    beide Lagen) — nicht wiederholt.
  - ROOM2030: EINE Treppe slot 8 @0x01B8E `2c 08 0c 31 80 00 5e e8 30 df 74 0e 78 05 01 00 02 00 00 00` = sce 12, Band-Byte
    0x80 (Wildcard, @0x80042cac-cd4, Port seit v0.7.79), Rechteck x -6050..-2350 z -8400..-7000, count 2 (Byte 16). An der
    exe gemessen `R_belege/nb1_2030treppe` (Spieler (-1800,-7700) Band 3 Blick 2048 = Hallenseite): `[stair] START
    dir=UP cur_band=3 target_band=5`, climb.txt danach `pos=(-6155,-9000,-7693) ... band=5`. **Band 5, nicht 9** — die
    Tueren slot 1/2/3 von 2030 liegen auf floor 9 (y -16200), die Treppe hebt per Record um 2 Baender (y -5400 -> -9000).
    Der im Stand f487909d genannte Ersatzweg "2070 (Mittelgang) -> 2030 (Band 3) -> Treppe -> slot 3 -> 2060" ist damit
    an der Treppe von 2030 NICHT belegt (Abnahme 0, M2 — bestaetigt) und wird gestrichen; Band 9 in 2030 erreicht man nur
    durch Tueren (2020 slot 1 nfloor 9, 2060 slot 0 nfloor 9, 2080 slot 0 nfloor 9).
* **Weg nach ROOM2040 (Halle) ohne die Kiste obj 6** — Tuer-Graph auf (Raum, Band) mit den gemessenen Band-Wechseln
  (`tueren_zensus.py --graph ROOM2000/B1 ROOM2040/B3 --ohne ROOM2050:2 ROOM2051:2`, 2070 nach Gaengen geteilt):
  `ROOM2000 slot 4 @0x0169E (floor 1) -> 2050 Band 3` -> `ROOM2050 slot 1 @0x01C6A (floor 3) -> 2020 Band 1` ->
  `ROOM2020 slot 1 @0x0088A (floor 1) -> 2030 Band 9` (bis (3,219)=1 steht an der Stelle das Ereignis slot 4 sce 3
  @0x00852 -> sub03 @0x00B1C-B3A: `Aot_reset 4 / Set (3,219)=1 / Message_on 2 / Aot_reset 1 (sce 2)` — die Tuer oeffnet
  sich beim ERSTEN Benutzen von innen, kein Schluessel) -> `ROOM2030 slot 3 @0x01B4C (floor 9) -> 2060 Band 0` -> Tritt
  obj 2 (Spur A) -> `ROOM2060 slot 1 @0x00DBA (floor 1) -> 2040 Band 3 (Halle)`. Jede Tuer liegt auf dem Ankunftsband der
  vorigen; der einzige Band-Wechsel im Raum ist der Tritt in 2060 (gemessen, Spur A). ⛔ BERICHTIGT (Nachbesserung 2, N1):
  der frueher hier genannte "zweite Zugang" ROOM11A0 slot 0 @0x00F5A -> 2070 Nordgang -> slot 5 -> 2040 ist ZIRKULAER —
  11A0 slot 0 verlangt (3,151)=1, und (3,151) wird in STAGE1-3 nur in ROOM2070/2071 sub02 @0x00CE4 gesetzt (Ereignis Aot
  slot 7 sce 3 @0x00AC2 IM Nordgang), der Nordgang wird zum ersten Mal nur aus ROOM2040 slot 4 @0x0131C (Galerie) betreten.
  Er steht also erst NACH dem ersten Galerie-Besuch offen und taugt nicht als Weg nach 2040 fuer einen Spieler, dessen
  Kiste vorher gefallen ist. Die beiden an der exe gemessenen Glieder (2070 Nordgang -> slot 5 -> 2040 Band 5, Treppe
  Band 5 -> 3) bleiben als Verbindungen richtig, sind aber kein Ersatzweg. Mit der Kiste: 2000 -> 2050 -> Klettern obj 6
  -> slot 2 -> 2040 Band 3 (3 Kanten).
  Der Graph nimmt an, dass zwei Tueren desselben Bands eines Raums verbunden sind (fuer 2070 widerlegt und von Hand
  geteilt; fuer die uebrigen Raeume des Wegs nicht einzeln gelaufen — OFFEN).
* Zensus: obj 6 erreicht mit dem Spieler die Absprungzelle (von Sueden geschoben, Fallrichtung -Z = ueber die Kante
  nach Norden), vor und nach sub02. Ob die Kaskade im Original die Kiste bis Band 0 bringt und ob sie unten gebraucht
  wird (obj 8 @0x01D4C ist die geparkte Band-0-Kiste unter `Ck (5,7)=1`, (5,7) wird in 2050 nirgends gesetzt), ist
  Teil des STAGE2-Raetsels und hier nicht Auftrag; byte-true belassen. In der Sonde bleibt 2050/2051 deshalb DIAGNOSE
  (soll = -1, Begruendung im Code: ein Pin hielte eine am Original ungemessene Aussage fest; die Nutzer-Frage ist ueber
  den Tuer-Graphen beantwortet).

## Nachbesserung 1 (Abnahme 0: M1, M2, M3)

### M1 — 4.3 "NACHGEPRUEFT" war falsch, Zensusdatei unvollstaendig
* Ursache: der Zensus des Stands f487909d (`stage2_tueren_2040.txt`) nahm je RDT nur die vorderen Door_aot_set des
  main00 und brach am ersten Ifel ab; die Records ROOM2070/2071 @0x00AD6 (slot 4 sce 0, `Ck(3,151)=0`), @0x00AFA (slot 4
  sce 2, sonst) und @0x00B1C (slot 5 -> 2040, unbedingt) hinter dem Block fehlten, daraus die falsche Aussage "2070 hat
  keine Tuer nach 2040".
* Messung vorher: `stage2_tueren_2040.txt` (f487909d) listet fuer 2070/2071 nur slots 0..3.
* Beleg: `python tools/scd_dump_room.py ROOM2070.RDT` main00 @0x00B1C (Bytes oben), @0x00ABA `Ifel_ck 06 00 3c 00`
  (Falsch-Ziel 0x00AFA), @0x00AF6 `Else_ck 07 00 26 00` (Ziel 0x00B1C) — slot 5 steht hinter beiden; ROOM2071 identisch.
* Aenderung: neuer Zensus `R_belege/tueren_zensus.py` (alle Records aus main und allen subs, Ifel/Else/Case-Kontext,
  Nutzlast decodiert: floor, Rechteck, Ziel, cut, nfloor, Spawn), Ausgaben `stage1_tueren.txt` (505 Zeilen),
  `stage2_tueren.txt` (162), `stage3_tueren.txt` (152), Auszug `stage2_tueren_2040.txt` neu geschrieben (14 Records
  2070/2071 statt 8). Dossier 4.3 berichtigt (oben).
* Messung nachher: `grep 'ROOM2070 main00 @0x00B1C' stage2_tueren_2040.txt` -> `slot 5 sce 2 floor 0 ... -> ROOM2040
  cut 12 nfloor 5 spawn (-19750,-9000,-19850) ... unbedingt`. Zusaetzlich gemessen, dass der von der Abnahme
  vorgeschlagene Laufweg 2000 slot 3 -> 2070 -> slot 5 an der Mauer scheitert (nb1_2000_2070m / nb1_2070n, 4.3).

### M2 — Ersatzweg 2030 -> 2060 -> 2040 war nicht belegt
* Ursache: der Stand f487909d argumentierte nur mit Raum-Tueren, ohne die Baender (2030 Ankunft 3, Tuer slot 3 floor 9;
  2060 Ankunft 0, Tuer slot 1 floor 1).
* Messung vorher: keine (nur Records).
* Beleg: Treppen-Records 2040 slot 5/6 @0x0133C/@0x01350 (sce 13, floor 3/5, gleiches Rechteck), 2030 slot 8 @0x01B8E
  (sce 12, Band 0x80, count 2); Tritt 2060 obj 2 @0x00EA6/@0x00ECC (Spur A, A_klettern.md Z. 231); 2020 sub03 @0x00B1C-B3A
  (Set (3,219)=1 oeffnet 2020 slot 1 @0x0088A).
* Aenderung: der Satz "2030 -> 2060 -> 2040" ist gestrichen; 4.3 nennt den band-konsistenten Weg 2050 -> 2020 -> 2030
  (Band 9) -> 2060 (Tritt) -> 2040 Halle und den Galerie-Zugang 11A0 -> 2070 Nordgang -> 2040 -> Treppe -> Halle.
* Messung nachher (exe, `R_belege/nb1_*`): 2070 Nordgang -> slot 5 -> 2040 Band 5 (nb1_2070n); 2040 Treppe Band 5 -> 3
  (nb1_2040treppe); 2030 Treppe Band 3 -> 5, NICHT 9 (nb1_2030treppe) — der alte Weg ist damit widerlegt, nicht belegt.
  Riegel `integration_r36_dachkiste` Laeufe `weg2040` und `treppe2040` halten die beiden gemessenen Glieder.

### M3 — kein Pin fuer Punkt 4
* Ursache: `probe_r36_dachkiste.c` fuehrte 3050/3051 und 2050/2051 mit soll = -1; der Fall war an der exe nicht gefahren.
* Messung vorher: Zensus 3050 "7 Ausloeser, erste Lage (-23875,-18500)" ohne Pin; kein exe-Lauf.
* Beleg (Mechanik, Spur A + eigene Messung): Objekt-Pass @0x8002c120-134 (`FUN_8003b7f0(&obj+0x34, 0x1c2, obj+0x82) & 2`),
  Absprungzelle r*[46] @0x00AD4 x -25000..-22950 z -18050..-16800 Wort 0x4002 (u0 0x01 sperrt nur den Spieler), Westwand
  r*[31] x -27000..-25000 (Schubsperre FUN_8003b558 Klasse 2), Fall FUN_8002c444 (objfall_r36.c), Kerbe slot 7 @0x01F06
  + Latch sub01 @0x02184-2196 (`Member_cmp 15 == 7` = obj+0x0B Kerben-Index) + Fall-Lage @0x01F40 + Super Redhawk slot 6
  @0x01EC6 floor 1.
* Aenderung: 3050/3051 soll = 1 PLUS Pruefung, dass die erste Ausloeser-Lage in der um 450 (@0x8002c128) erweiterten
  Absprungzelle r*[46] liegt; 2050/2051 bleiben begruendete Diagnose (Kommentar im Code, 4.3). Sonde gibt den Schub-Pfad
  aus (Vorgaenger-Verfolgung). Neuer Riegel-Lauf `fall3050` in `test_r36_dachkiste.cmake` (echte exe).
* Messung nachher (exe, `R_belege/nb1_3050fall_d`; Vorversuche a-c = Skript-Kalibrierung: Mauer r*[41] bei z -18850..
  -17850 umgangen, Schubtempo ~20 Einheiten/Bild gemessen, Umweg noerdlich der Kiste bis z -22717): Leon aus der
  30E0-Ankunft (-3596,1523) Band 4 den Gang nach Norden (x -6482) bis z -20708, nach Westen gegen die Kiste: `F618 ...
  SCHUB-OK spieler=(-16724,-20276) rot=2080 lage=(-18000,-20800) ziel=(-18074,-20800)`, 297 Schuebe West bis
  `lage=(-24058,-20800)`, dann `F914 SCHUB-GESPERRT ... ziel=(-24108,-20800)` + `TREFFER-ZELLE r3[31]: x -27000..-25000`
  (Westwand, wie 1090: Sperre statt Durchdringung); Umweg Nord (z -22717) / West (x -24532) / Blick 3096; 129 Schuebe
  Sued ab `F1261 ... spieler=(-24454,-22076) rot=3128 lage=(-24058,-20800) ziel=(-24058,-20726)`; `F1383 room=3050 p=1
  AUSLOESER wort=0x4002 band=4 pos=(-24058,-7200,-18488) spieler=(-24425,-19838) richt=(0,1)` (-18488 = Zellkante -18050
  - 450 + Schritt), Fallzustand `zust 0->1 ... 1->2 ... 2->3`, `F1396 BODEN FUN_8001c6e8=0 band 4 -> 0`, `F1405 LANDUNG y=0
  band=0 pos=(-24058,-15601)`, Endlage `p1 (-24058,0,-15561) b0 r(-1029,0,0)` (umgekippt, Band 0). 425 + 129 SCHUB-OK,
  1 GESPERRT (Wand), 1 AUSLOESER. Die zweite Stufe des Raetsels (Band-0-Schub in Kerbe 7 -> (3,93) -> Redhawk) ist
  Record-Beleg (Latch wie 5020, dort von Spur A gemessen), hier nicht gefahren (OFFEN).

## Nachbesserung 2 (Abnahme 1: N1, N2, N3, N4, N5)

Stand beim Start: 47384115 (Abnahme 1). Gebaut (`local_build.sh build` -> LOCAL-BUILD-OK), Messkopie
`re15_pc_r36r_nb2.exe`, Startskript `R_belege/lauf_nb2.sh <lauf> <wegplan> KEY=WERT`.

### Werkzeug: Wegplan-Treiber RE15_WEGPLAN (engine/src/dachkiste_r36.c, Haken input_pc.c)
* Ursache fuer das Werkzeug: die Abnahme konnte den 2050-Fall mit dem offenen Skript RE15_INPUT_SCRIPT nicht fahren
  (R_abnahme_1.md R4c/R4d: Anlauf-/Auslauf-Animation mo=8 mit Wurzelbewegung, Drehungen in mo=17 geschluckt — die
  berechneten Stellungen werden nach mehreren Drehungen nicht getroffen). Das Original-Messskript ist dagegen
  geschlossen geregelt (orig_1090_push.py: Lage/Blick aus der RAM lesen, Pad stellen). Der Port bekommt dasselbe:
  `RE15_WEGPLAN=<datei>` liest je Bild Spielerlage, Blick (+0x6A) und Objektlage und erzeugt die Pad-Bits
  (Befehle OBJ/RAUM/WARTE/GEGNERWEG/ZELLE/GEHE/BLICK/SCHIEB/AKTION/ENDE, Kopf von dachkiste_r36.c). Haken: 2 Zeilen in
  `platform/pc/src/input_pc.c` hinter der RE15_INPUT_SCRIPT-Ersetzung ("Runde 36 Spur R"). Reine Mess-Steuerung wie
  RE15_INPUT_SCRIPT, ohne Env ein frueher Ruecksprung; kein Spiel-Code liest sie; kein Verhalten.
* Gemessene Eigenheiten, die der Treiber beruecksichtigt (Laeufe nb2_2050fall_a/b): `OBJ n` ist die Obj_model_set-
  Nummer, der Pool-Slot ist die Installationsreihenfolge (ROOM2050 obj 6 = Slot 0 — in Lauf a wurde Slot 6 = obj 5
  verfolgt, der Schub lief trotzdem bis zur Sperre an r*[32] z -4900 bei Kistenmitte -5841 = -4900 - 900 - 41);
  Gehen ~74/Bild nach 12 Bildern Anlauf, Auslauf ~444 nach dem Loslassen -> Loslass-Abstand 40 + 35 x min(Bilder,12),
  Vorbeilaufen (Abstand waechst) -> loslassen; Drehen 96/Bild, R erhoeht rot_y; Schub: die Kiste bewegt sich nur in
  Teilen des Schub-Clips (mo=18, 25 Bilder): Sued-Schub 119 Bilder / 2236 Einheiten (55 Bilder Schritt 0, Schritte
  9..50), Ost-Schub 591 Bilder / 5513 Einheiten (432 Bilder Schritt 0, Schritte 3..49) — groesster Schritt 50 < 100 =
  Breite des Fensters [7550,7650), die Kiste landet also sicher darin, wenn der Treiber beim ersten x >= 7550 loslaesst.
* GEGNERWEG: ROOM2050 hat zwei Adult Spider (Sce_em_set @0x01CE6/@0x01CFA, Typ 0x25, (4300,-6450)/(-5000,-6082)), die Leon
  nach ~700 Bildern toeten (A_abnahme_3.md, Lauf r2050_a). Fuer die Messung der Schubkette (liest nur Spieler, Objekt
  und SCA) werden sie beim Raumstart freigegeben (re15_actor_free, active = 0). Gegenstueck am Original s. N3.

### N1 — "zweiter Zugang" ueber ROOM11A0 ist zirkulaer
* Ursache: 4.3 und "Fuer den Nutzer" nannten 11A0 slot 0 @0x00F5A -> 2070 Nordgang -> slot 5 -> 2040 als Weg ohne
  Kiste; 11A0 slot 0 steht unter (3,151)=1, gesetzt nur in ROOM2070/2071 sub02 @0x00CE4 (Ereignis im Nordgang), und der
  Nordgang wird zuerst nur aus 2040 slot 4 @0x0131C (Galerie) betreten (Abnahme 1, selbst nachgelesen: Flag-Scan ueber
  STAGE1-3 mit scd_dump_room.py findet `22 03 97 01` nur in 2070/2071).
* Aenderung: 4.3 und "Fuer den Nutzer" berichtigt (Weg als zirkulaer gekennzeichnet, kein Ersatzweg). Keine Messung
  noetig, keine Code-Aenderung.

### N2 — ROOM2020: die Luecke r*[15] ist per SCD geschlossen (kein Port-Defekt), der Durchgang ist ein Raetsel
* Messung vorher (Abnahme 1, R1/R1b): Spieler bleibt bei x 218 = Ostkante -250 von r0[15] + 450 + 18 stehen; in die
  Luecke gesetzt wird er beim Laden auf z -4318 herausgeschoben; die RDT traegt fuer r0[15] @0x004F8 u0 0x00.
* Beleg (Mechanismus): die RDT-Bytes sind nicht der Laufzeitwert. ROOM2020 sub00 (Raumstart) setzt bei (3,114)=0
  `Sca_id_set 37 00 0f ff / 37 01 0f ff / ... / 37 04 0f ff` @0x0098E-099E und bei (3,114)=1, (3,113)=0 dasselbe
  @0x00A06-0A16 — Zelle 0x0f = r*[15] in allen fuenf Regionen bekommt u0 = 0xff. Original-Handler des Opcodes 0x37
  = LAB_8004175c (selbst disassembliert): `lbu v1,1(a2)` Region, `lw v0,32(v0)` Tabelle hinter DAT_800ac778,
  `lw v1,0(v1)` = Zellenfeld der Region (Index Region+1 @0x80041774-7c), `lbu a1,2(a2)` Index x 12 (@0x80041780-88),
  `lbu a1,3(a2)` Wert, **`sb a1,9(v0)` @0x8004179c** = Byte +9 der 12-Byte-Zelle = u0 (Layout wie sca_dump.py: w,d,x,z,
  typ,u0,u1,floor). Danach sperrt die Zelle den Spieler ueber FUN_8003b0a4 (Maske 1 & u0, Port
  collision_constrain_impl re15_collision.c:727) — beides byte-true, der Port tut dasselbe (`op_sca_id_set` scd_vm.c:
  `e->u0 = t->pc[3]`). An der exe zur Laufzeit gemessen (Lauf `nb2_2020durchgang`, Befehl ZELLE): r0[15] u0 = 0xff
  beim Raumstart (s. Messung nachher).
* Wann oeffnet sich die Luecke: sub02 @0x00A5C (Ereignis Aot slot 3 sce 3 @0x009AE, Rechteck x 2200..3000 z -4150..
  -3350 floor 1 = im Suedstreifen, nur bei (3,114)=1 und (3,113)=0) -> Nachrichten 0/1, (12,31)=0 (Dialog bestaetigt;
  (12,31) ist in allen Raeumen das Dialog-Ergebnis hinter Message_on, kein Gegenstand), Set (3,113)=1, Szene (Cut 3/4,
  obj 1 faehrt mit Speed_set/Add_speed weg, obj 2 kommt an die Stelle (-1452,1684,-584)), dann `37 0x 0f 00` @0x00ADE-
  0AEE (u0 = 0) + `39 0x 0f 10` (floor 0x10). (3,114) wird nur in ROOM2010/2011 sub02 @0x008CA gesetzt (Ereignis Aot
  slot 4 sce 3 @0x00880, x 2700..3700 z -1550..-550 floor 0, Nachricht, (12,31)=0, obj 3 faehrt weg) — ROOM2010 haengt
  unbedingt an ROOM2000 slot 2 @0x0165E (und an 20B0/20B1 slot 2). Der Durchgang in 2020 ist also ein Raetsel aus zwei
  Aktionen ohne Gegenstand und ohne Kiste: Hinweis in 2010 lesen, dann in 2020 am Suedstreifen handeln.
* Aenderung: kein Code (der Port ist hier byte-true). Dossier 4.3 / "Fuer den Nutzer" berichtigt; Messung des
  Durchgangs an der exe mit dem Wegplan-Treiber (Lauf `nb2_2020durchgang`, unten).
* Messung nachher (exe, Wegplan-Treiber, `R_belege/nb2_2020durchgang_f/` (wegplan.txt, debug.log); Vorlaeufe b-e =
  Kalibrierung: Examine-Blick NORD (Punkt 620 voraus (2698,-4002) im exakten Rechteck, Blick Sued lag mit -2762
  draussen), drei Druecke fuer sub02 (Message_on 0 / Evt_next / Message_on 1 / Evt_next / Ck (12,31) — der Skript-
  Kontrolllauf `nb2_2020skript` mit RE15_INPUT_SCRIPT A0.1 x3 zeigte dieselbe Folge), Tuer-Blick +Z (Rechteck z 2800..
  3800 an der +z-Kante des Streifens)). Spieler per RE15_PLAYER_POS an der 2050-Ankunft (11350,-3150) Band 1, (3,114)=1
  per RE15_SET_FLAG (= Hinweis in ROOM2010 gelesen): `ZELLE r0[15] LAUFZEIT: ... u0 0xff ... fl 0x13` beim Raumstart
  (= Sca_id_set @0x0098E-099E bzw. @0x00A06-0A16); GEHE (2600,-3450), BLICK 1024, AKTION x3 -> `[msg] room=2020 id=0`,
  `id=1`, dann die Szene `[scd F423] Cut_chg(3)`, `F551 Cut_chg(4)`, `F621 Cut_chg(3)`, Letterbox zu F635, obj 1 nach
  (-1452,1684,14316) geparkt; danach `ZELLE r0[15] LAUFZEIT: ... u0 0x00 u1 0x00 fl 0x10` (= @0x00ADE-0AEE und
  Sca_floor_set @0x00AF2-0B02); GEHE (-1300,-3400) -> (-1291,2290) DURCH die Luecke in den Nordstreifen -> (-13708,2523)
  -> (-14222,2832) an der Tuer; BLICK 3072, AKTION: `[msg] id=2` (sub03 @0x00B1C: Aot_reset 4, Set (3,219)=1, Message_on 2,
  Aot_reset 1 sce 2), AKTION, AKTION -> `[aot] DOOR FIRE slot=1 rect=(-14200,3300,hw=1000,hh=500) target_cut=13
  spawn=(-450,-16200,-9700)` -> `[room] PC loaded room2030.rdt` (Band 9) -> `EXIT_AT: Bild 300 in Raum 2030`. Der Ersatzweg
  2050 -> 2020 -> 2030 ist damit an der exe gangbar (das Glied 2030 Band 9 -> slot 3 -> 2060 hat die Abnahme 1 selbst
  gemessen (R2), 2060 Tritt -> slot 1 -> 2040 die Spur A). Am Original ist der 2020-Durchgang nicht gemessen (OFFEN).

### N3 — ROOM2050/2051 obj 6: an der exe GEMESSEN, die Kiste erreicht die Absprungzelle und faellt
* Geometrie (sca_dump.py ROOM2050.RDT 3, Band 3, in allen 5 Regionen gleich): Kiste Record (-1600,-5400,-9348) @0x01D0E
  Box h(900,900,900); Spalte zwischen r*[38] (x <= -2600, z -14000..-8100), r*[46] (x -2600..-1500 z -12500..-11300),
  r*[39] (z <= -12500) und r*[41]/[43] (x >= 1250 bzw. 2450); nach Sueden frei bis Kistenmitte z -5800 (r*[32] z -4900);
  nach Osten nur fuer Kistenmitte z in [-7150, -5800] (r*[43] x 2450..6650 z -8750..-8050: halboffen @0x8003b770-794,
  -7150 + 9650 = 2500 ist nicht < 2500); am Ostende das Fenster x in [7550, 7650) zwischen r*[44] (x 5100..6650) und
  r*[40] (x 8550..9900); von dort nach Norden frei (r*[50] u0 0x00 zur Laufzeit, s. u.) bis in die Absprungzelle r*[49]
  @0x00B5C x 6350..8750 z -13950..-12150 (Wort 0x3202, u0 0x01 sperrt nur den Spieler), Objekt-Pass ab Kistenmitte
  z <= -11700 (+450 @0x8002c12c). Spielerstellungen: noerdlich der Kiste (-1000,-10800) (r*[46] aufgeblasen x <= -1032
  greift nicht); Tasche westlich x <= -2950, z (-6632,-5368) (r*[45] z -8100..-7100 und r*[32] je + 450 + 18); suedlich
  der Kiste bei z -5600 (r*[30]/[32] aufgeblasen z >= -5368).
* Laufzeit-Zellen (Treiber ZELLE, nb2_2050fall_c): r0[50] (0x32) x 6650..8550 z -12150..-11650 **u0 0x00** (sub00
  @0x01E94-1EA4 `37 0x 32 00` bei (3,109)=0, (3,104)=0 — Auslieferungszustand beim ersten Betreten; nach dem Hebel-
  Ereignis sub04 @0x02346 bzw. nach dem Raetsel sub02 @0x022A8 wird sie 0xff), r0[49] u0 0x01 u1 0x02 fl 0x32, r0[51]
  (0x33) u0 0xff.
* Messung (exe, `R_belege/nb2_2050fall_c/wegplan.txt`, push.txt, objfall.txt; Spieler per RE15_PLAYER_POS (-1000,-10800)
  Blick 3072 Band 3, Spinnen freigegeben): SCHIEB Sued F67-F196 -> Kistenmitte z -7037 (120 SCHUB-OK); Umweg Ost/Sued/
  West in die Tasche (-3108,-6351) (vier GEHE, alle angekommen); SCHIEB Ost F449-F1225 -> x 7571 (592 SCHUB-OK, 0
  gesperrt); Umweg nach Sueden (7583,-5597); SCHIEB Nord ab F1384: `F1624 room=2050 p=0 AUSLOESER wort=0x3202 band=3
  pos=(7571,-5400,-11706) spieler=(7380,-10356) richt=(0,-1)`, `F1625 zust 0->1 n=1`, Kippen rot x -2040 -> -1767 ...,
  Endlage `p0 (7571,-3600,-13681) b2 r(-1038,0,2048)` = auf Band 2 (y -3600, die erste Stufe der Kaskade r*[23] 0x2102)
  gelandet, nicht mehr auf Band 3. **Der Port laesst die 2050-Kiste ueber die Kante fallen; danach fehlt die Stufe fuer
  die Tuer slot 2 @0x01C8A (floor 4) nach ROOM2040.**
* **Original (DuckStation-GDB, `R_belege/orig_2050_d/log.txt`): die Kiste laesst sich bis in die Absprungzelle schieben
  und faellt — Port = Original.** Derselbe Weg wie an der exe, geschlossen geregelt: Leon von der Tuer-Ankunft (9100,4100)
  durch den Ostkorridor (x 8818..9432) und entlang z -6300 nach Westen, hinter die Kiste (-1027,-10771); SUED Runde 0:
  117x SCHUB-OK (@0x8002bfc0), Kistenmitte z -9348 -> -7031 (Bedingung); Umweg in die Tasche (-3050,-6491); OST Runde 0:
  432x SCHUB-OK, 0 gesperrt, x -1600 -> 7562 (im Fenster [7550,7650)); Umweg nach Sueden (7579,-5651); NORD Runde 0:
  230x SCHUB-OK, dann `FALL-AUSLOESER obj6=(7562,-5400,-11732) spieler=(7175,-10382)` = Halt @0x8002c13c (Objekt-Pass
  FUN_8003b7f0(.,0x1c2,3) & 2 -> FUN_8002d3d8), 0 TREFFER-ZELLE; danach faellt=1, y -5400 -> -4534 -> -4054 -> -3600,
  Band 3 -> 2, Endlage obj6 (7562,-3600,-13088) faellt=0 Band 2. Summe 779 angenommene Schuebe, 0 gesperrt, 1 Fall-
  Ausloeser, HP 100 (Spinnen ueber Aktiv-Bit + g_active_count abgeschaltet, s. u.). Port zum Vergleich: AUSLOESER bei
  (7571,-5400,-11706), Landung (7571,-3600,-13681) Band 2 — dieselbe Zelle, dieselbe Kante, dieselbe Landehoehe; die
  Endlage in z weicht um 593 ab (Fall-/Kippbahn FUN_8002c444, Spur A — hier nur notiert, OFFEN).
  Werkzeug `R_belege/orig_2050_push.py`, Spielstand
  `R_belege/room2050_orig.sav` per Debug-Menue JUMP 205 SEPTIC POOL (re15_quickload.py --triangle 1 --right 5, kurze
  Tipper 0,04 s; mit 0,08 s lief die Liste per Auto-Repeat ans Ende = JUMP 102), Auslieferungs-EXE @0x80026e4c 0800e003,
  Leon an der Tuer-Ankunft (9100,-5400,4100) Band 3, obj 6 = Pool-Index 6 (-1600,-5400,-9348)).
  Werkzeug-Befunde: (a) der virtuelle Pad kommt erst ~0,5-1 s nach dem ERSTEN Druck an (orig_2050_diag1: Padwort 0 bei
  0,5 s, 0x2002 ab 1,0 s, Blick +1440 je 0,5 s = 96/Bild) -> Drehtest 1,2 s halten; (b) Spinnen abschalten: die Entity-
  Schleife FUN_8001a50c prueft @0x8001a55c-568 `lw v0,0(entity) / andi 1 / beq zero -> 0x8001a5a0` (Aktiv-Bit) und ruft
  den Handler @0x8001a580-588 UNGEPRUEFT (`lw v0,0(v0) / jalr v0` — den Dispatch-Eintrag 0x80072c40 zu nullen waere ein
  jalr 0); sie zaehlt aber nur AKTIVE Entities (`addiu s0,s0,1` im Delay-Slot des jalr @0x8001a58c) und laeuft, bis
  s0 == g_active_count (`lbu s1,-13746(s1)` = 0x800aca4e @0x8001a528, Vergleich @0x8001a5b0-b8). Nur das Aktiv-Bit zu
  loeschen (w0 0x0801 -> 0x0800) laesst die Schleife ueber den Pool hinauslaufen — gemessen orig_2050_diag2_ent: Padwort
  0x2002 kommt an, Leon dreht nicht; mit g_active_count 2 -> 0 (orig_2050_diag2_ent2) dreht er (Blick 480 ... 6240).
  Das ist das Gegenstueck zu re15_actor_free (active = 0; der Port filtert den Pool ohne Zaehler).

### N4 — Beleg-Datei zensus_diag.txt neu geschrieben
* `R_belege/zensus_diag.txt` = Ausgabe von `probe_r36_dachkiste.exe diag` (Stand dieser Nachbesserung): 1090/1091 12
  Zellen / 0 Ausloeser, 3050/3051 13373 / 8 (Start (-3596,1523) = 30E0 slot 6 @0x00ABA), 2050/2051 3898 / 1 mit Pfad —
  stimmt mit Tabelle 4.1 ueberein.

### N5 — ROOM3050 am Original: unter OFFEN gefuehrt
* Datenlage eindeutig (Super Redhawk slot 6 @0x01EC6 floor 1 nur auf der gefallenen Kiste, Latch sub01 @0x02184-2196);
  offen bleibt, ob das Original denselben Schubpfad (West bis r*[31], Sued in r*[46]) zulaesst. Messweg: re15_quickload.py
  --triangle 2 (JUMP 300 ...), Raum 0x305 steppen, orig_2050_push.py-Muster mit K = Pool-Index von obj 1. Eintrag OFFEN.

## Tests

Alle in `tests/unit/probes/r36_dachkiste.cmake` (GLOB), kein Eingriff in tests/*/CMakeLists.txt.

* `unit_r36_dachkiste_zensus` (`probe_r36_dachkiste pin`, 4.1): Zensus MIT Spieler ueber die Praedikate der Schubkette —
  ROOM1090/1091 obj 1 erreicht KEINE Absprungzelle, Kistenmitte bleibt in z [-15503, -15482] (notwendige Bedingung,
  Spielerfreiheit ueberschaetzt). **Seit Nachbesserung 1 (M3):** ROOM3050/3051 obj 1 soll = 1 — der Zensus MUSS einen
  Ausloeser finden und die erste Lage muss in der um 450 (@0x8002c128) erweiterten Absprungzelle r*[46] @0x00AD4 liegen
  (x -25450..-22500, z -18500..-16350; gefunden (-23875,-18500)). **Seit Nachbesserung 2 (N3):** ROOM2050/2051 obj 6
  soll = 1 (Auslieferung und nach sub02) — der Zensus MUSS einen Ausloeser finden und die erste Lage muss in der um 450
  erweiterten Absprungzelle r*[49] @0x00B5C liegen (x 5900..9200, z -14400..-11700; gefunden (7550,-12000)); der Fall
  ist an der exe gemessen (nb2_2050fall_c) und am Original (orig_2050_*). Die Sonde gibt den Schub-Pfad zur ersten
  Ausloeser-Lage aus.
* `integration_r36_dachkiste` (`tests/integration/test_r36_dachkiste.cmake`, echte exe, Messkopie
  `re15_pc_r36r_haken_<id>.exe`, Grundumgebung wie test_r36_inventar.cmake, ein Wiederholungsversuch bei Fenster-Flattern):
  - `leon`: ECHTER WEG ROOM10F0 ((9,71)=1) -> Tritt -> Klettern -> Tuer slot 1 -> ROOM1090 -> Korridor -> 3 Schuebe. MUSS:
    `loaded room10f0.rdt`, kein `[cut10f0] ... Szene gestartet`, `[aot] DOOR FIRE slot=1 rect=(-4750,-1750,hw=600,hh=500)
    target_cut=0 spawn=(-13600,-9000,1300)`, `loaded room1090.rdt`, >= 1x `room=1090 p=1 obj=1 SCHUB-GESPERRT ...
    ziel=(-5960,-155xx) ... band=5` + `TREFFER-ZELLE ... x -7256..4624 z -18560..-16403`, KEIN `SCHUB-OK` fuer obj 1,
    KEIN `p=1 AUSLOESER`/`zust`/`BODEN`/`LANDUNG`, letzte Lage `p1 (-5960,-9000,-15482) b5`.
  - `elza`: derselbe Weg mit RE15_PSELECT_AUTO_SWITCH=1 (`Spieler-Familie PL04`, ROOM10F1 -> ROOM1091), dieselben MUSS.
  - `west`: Leon, Sprung 1090, Tasche (-4610,-15155) Blick 2112: `SCHUB-GESPERRT ... ziel=(-60xx,-15482)` +
    `TREFFER-ZELLE ... x -9200..-6878 z -18524..265`, kein SCHUB-OK, kein Fall, Record.
  - **`fall3050` (NEU, Nachbesserung 1 / M3, Punkt 4):** Leon aus der 30E0-Ankunft (-3596,1523) Band 4, Skript wie
    `R_belege/nb1_3050fall_d`: MUSS >= 100x `room=3050 p=1 obj=1 SCHUB-OK`, `p=1 AUSLOESER wort=0x4002 band=4
    pos=(-24xxx,-7200,-18xxx)`, `p=1 LANDUNG y=0 band=0`, letzte Lage `p1 (x,0,z) b0`. Der vorgesehene Fall der 3050-Kiste
    laeuft im Port (Objekt-Pass @0x8002c120-134 -> FUN_8002c444).
  - **`weg2040` / `treppe2040` (NEU, Nachbesserung 1 / M1+M2, Punkt 4):** Nordgang 2070 (11A0-Ankunft (-18650,-19300)
    Band 0) nach Osten -> MUSS `[aot] DOOR FIRE slot=5 rect=(5500,-19200,hw=500,hh=1000) target_cut=12
    spawn=(-19750,-9000,-19850)` + `loaded room2040.rdt`; Galerie (-1350,-19200) Band 5, Viereck in der Treppenzone ->
    MUSS stair_dbg.log `[stair] START dir=DOWN cur_band=5 target_band=3` + climb.txt `room=2040 ... pos=(x,-5400,z) ...
    band=3`. ROOM2040 ist ohne die Kiste in 2050 erreichbar und seine Galerie mit der Halle verbunden.
  - **`fall2050` (NEU, Nachbesserung 2 / N3, Punkt 4):** Leon per RE15_PLAYER_POS (-1000,-10800) Blick 3072 Band 3, Wegplan
    (vom Riegel in den Laufordner geschrieben, RE15_WEGPLAN): GEGNERWEG, ZELLE r0[50]/r0[49], SCHIEB Sued bis Kistenmitte
    z >= -7050, Umweg in die Tasche, SCHIEB Ost bis x >= 7550, Umweg nach Sueden, SCHIEB Nord. MUSS: wegplan_log.txt
    `ZELLE r0[50] LAUFZEIT: ... u0 0x00` (sub00 @0x01E94), `SCHIEB: Bedingung erfuellt (obj.z = -70xx)`, Ost-Ende
    `obj.x = 75xx..764x` (Fenster [7550,7650)), push.txt >= 300x `room=2050 p=0 obj=6 SCHUB-OK`, objfall.txt `p=0 AUSLOESER
    wort=0x3202 band=3 pos=(75xx/76xx,-5400,-116xx/-117xx)` + `p=0 zust 0->1`, letzte Lage p0 NICHT mehr `b3`.
  - **`weg2020` (NEU, Nachbesserung 2 / N2, Punkt 4):** Leon an der 2050-Ankunft (11350,-3150) Band 1, (3,114)=1 per
    RE15_SET_FLAG, Wegplan (RE15_WEGPLAN): ZELLE r0[15], GEHE (2600,-3450), BLICK 1024, AKTION x3, ZELLE r0[15], GEHE durch
    die Luecke in den Nordstreifen bis (-14200,3150), BLICK 3072, AKTION x4. MUSS: `ZELLE r0[15] ... u0 0xff ... fl 0x13`
    beim Start, `[msg] room=2020 id=0`, `Cut_chg(3)`, danach `ZELLE r0[15] ... u0 0x00 u1 0x00 fl 0x10`, `[msg] room=2020
    id=2`, `[aot] DOOR FIRE slot=1 rect=(-14200,3300,hw=1000,hh=500) target_cut=13 spawn=(-450,-16200,-9700)`, `loaded
    room2030.rdt`.
  Er diskriminiert: ein Port, der den 1090-Schub annimmt oder die 1090-Kiste fallen laesst, oder in 3050 den Fall NICHT
  ausloest, oder die 2050-Kiste nicht bis in die Absprungzelle kommen laesst (z. B. eine zu enge Schubsperre an r*[43]/
  r*[44]/r*[40] oder eine Laufzeit-Sperre an r*[50]), oder die 2020-Luecke nicht per Sca_id_set schliesst/oeffnet, oder
  2070 slot 5 / die 2040-Treppe verliert, faellt durch.
  ⛔ Makro-Falle beim Bau des Riegels (gemessen): Makro-Argumente werden im Makro ein zweites Mal ausgewertet, ein `\\(`
  im Aufruf wurde zur Regex-Gruppe `(` und `\\[x\\]` zum Klammerausdruck — der erste Lauf fand den gesperrten Schub
  "nicht", obwohl push.txt ihn enthielt. Regex-Argumente deshalb ohne Backslash (`[(]`, `[.]`), Klartext per string(FIND).
* Gezielte Laeufe (KEINE volle Suite im Spurbaum), Stand Nachbesserung 2 (81d02dd3+, Bau LOCAL-BUILD-OK):
  - `ctest --test-dir re15_port/build -R "unit_r36_dachkiste_zensus|unit_r36_objfall|unit_r36_klettern|unit_1090_|prop_sca_collision|prop_point_in_poly|prop_zone_dispatch|unit_r22_10f0_figur|unit_re2_room1090_ab|unit_plc_back_yaw_1090|probe_1090_|unit_map_1090|unit_r35_cut10f0|unit_r36_szenenleon_abgang10f0" --timeout 900`:
    `100% tests passed, 0 tests failed out of 31` (Total Test time (real) = 14.89 sec; mit den 2050-Pins der Sonde).
  - `ctest --test-dir re15_port/build -R "integration_r36_dachkiste" --timeout 1800 --output-on-failure`:
    INTEGRATION_ERGEBNIS_NB2 (acht Laeufe leon/elza/west/fall3050/weg2040/treppe2040/fall2050/weg2020).
* Gezielte Laeufe, Stand Nachbesserung 1 (zum Vergleich):
  - `ctest --test-dir re15_port/build -R "unit_r36_dachkiste_zensus|unit_r36_objfall|unit_r36_klettern|unit_1090_|prop_sca_collision|prop_point_in_poly|prop_zone_dispatch|unit_r22_10f0_figur|unit_re2_room1090_ab|unit_plc_back_yaw_1090|probe_1090_|unit_map_1090|unit_r35_cut10f0|unit_r36_szenenleon_abgang10f0" --timeout 900`:
    `100% tests passed, 0 tests failed out of 31` (Total Test time (real) = 17.36 sec; unit_r36_dachkiste_zensus 8.80 s
    einzeln, mit den 3050-Pins).
  - `ctest --test-dir re15_port/build -R "integration_r36_dachkiste" --timeout 1200 --output-on-failure`:
    `100% tests passed, 0 tests failed out of 1` (Passed 574.09 sec, Laufordner re15_port/build/r36_dachkiste_wd/lauf_f39bdbcb; sechs Laeufe leon/elza/west/fall3050/weg2040/treppe2040).
  - Stand f487909d (Abnahme 0, zum Vergleich): Unit-Gruppe 18/18 bzw. 31/31, Riegel 1/1 (332.73 s / 301.13 s).

## OFFEN

* **Spieler-Schritt 1 Einheit** (1.2): Spieler-z beim Schubversuch -14207 (Port) gegen -14208 (Original NORD) bei
  verschiedener Anlaufstrecke; die Schubkette und die Sperre sind davon unberuehrt (54..56 Einheiten Ueberdeckung mit
  r3[4]). Ein bildgleicher Vergleich des Laufmodells (gleicher Startpunkt, gleiche Eingabe, Bild fuer Bild) ist nicht
  Auftrag dieser Spur; die Messschiene dafuer (RE15_PUSH_LOG + orig_1090_push.py-Haltepunkte) liegt bereit.
* **Szene ROOM10F0 vom Debug-Sprung-Spawn** (1.3, Lauf `R_belege/p1_szene`, nur Diagnose): mit frischem Spielstand
  und Sprung nach 10F0 (Spawn (-7125,0,4950) = DEBUG.BIN-Tabelle) startet die Szene der Runde 35 Spur K (`[cut10f0]
  Szene gestartet`, `[msg] id=6`), Leon laeuft per Plc_dest los (climb.txt T551-T581 mo=105, pm=2) und bleibt bei
  (1824,0,1568) stehen (mo=105 bis F3600, keine Nachricht 7, kein Szenenende) — die Szene ist auf den Tuer-Spawn aus
  ROOM10D0 gebaut (r35-Riegel: Nachricht 7 bei F396, Ende ~F2730) und kommt vom Sprung-Spawn nicht ans Ziel. Im Spiel
  ist 10F0 beim ersten Besuch nur durch diese Tuer erreichbar (1090 liegt dahinter), also kein Spielweg; Hinweis fuer
  Spur K / das Mess-Harness (RE15_DEBUG_JUMP in einen Raum mit ausstehender Szene). Die Messung dieser Spur fuhr daher
  (9,71)=1 (Szene gesehen).
* Elzas Lauf `e1_echt` entstand mit der Messkopie VOR dem Zellen-Schrieb (push.txt ohne TREFFER-ZELLE); der Riegel-Lauf
  `elza` liefert die Zellen (r3[4]) — s. Tests.
* **ROOM2050/2051 obj 6 am Original** (4.3, nicht Auftrag, byte-true belassen): ob die Absprung-Kaskade r*[49]/[23]/
  [77]/[78] die Kiste im Original bis Band 0 bringt und ob sie unten gebraucht wird (obj 8 @0x01D4C unter (5,7)) —
  kein Spielstand in 2050, STAGE2-Raetsel unfertig. Der Zensus (Modell) findet den Ausloeser; kein Pin darauf.
* **ROOM2030-Treppe endet auf Band 5, die Tueren liegen auf floor 9** (4.3, Nachbesserung 1 gemessen): Record slot 8
  @0x01B8E count 2 (Byte 16) hebt von Band 3 (y -5400) auf Band 5 (y -9000); slots 1/2/3 verlangen Band 9 (y -16200,
  Ankuenfte aus 2020/2060/2080 nfloor 9). Ob das Original dieselbe Treppe genauso faehrt (count-Byte @0x800435b8 sce 12)
  und wie der Beta-Raum 2030 Band 5 und 9 verbindet, ist hier nicht gemessen (kein Spielstand in 2030) — Hinweis fuer
  die STAGE2-Spur; fuer die Frage dieser Spur reicht, dass der Weg ueber 2030 Band 3 NICHT belegt ist und gestrichen wurde.
* **Ersatzweg nach 2040 nicht Ende-zu-Ende gelaufen** (4.3): die Kette 2050 slot 1 -> 2020 (sub03 (3,219)) -> 2030 Band 9
  -> slot 3 -> 2060 (Tritt, Spur A) -> slot 1 -> 2040 Halle ist Record- und Band-konsistent; gemessen sind die Glieder
  2060-Tritt (Spur A), 2070-Nordgang -> 2040 und 2040-Treppe (hier). Nicht gelaufen: 2020 Ereignis sub03, 2030 Band-9-
  Etage von der 2020-Ankunft bis slot 3, die Verbindung Galerie-Ankunft (-19750,-19850) -> Treppenzone (x -1350) in 2040
  (Galerie-Waende r*[29..35] lassen den Streifen z -21400..-18250 frei; `nb1_2070n` lief 1200 Bilder auf Band 5 ohne
  Klemme). Die Annahme "zwei Tueren desselben Bands eines Raums sind verbunden" gilt fuer 2070 NICHT (drei Gaenge) und
  ist fuer 2020/2030/2060 nicht einzeln geprueft.
* **ROOM3050 Stufe 2** (4.2/M3): nach dem gemessenen Fall (Band 0, Endlage (-24058,0,-15561)) der Band-0-Schub in die
  Kerbe slot 7 @0x01F06 (-25000..-22950, -23200..-21150) -> Latch (3,93) -> Fall-Lage @0x01F40 -> Super Redhawk slot 6:
  Record-Beleg, derselbe Latch-Mechanismus wie 5020 (Spur A, exe gemessen); der Abstieg auf Band 0 in 3050 (Ereignis-
  Leiter sub00 Aot slot 2..5 @0x02044-2080 unter (3,82)) ist hier nicht gefahren.
* **Fall-/Kippbahn der 2050-Kiste nach dem Ausloeser** (N3): Port und Original loesen in derselben Zelle aus (Port
  (7571,-11706), Original (7562,-11732)) und landen auf Band 2 / y -3600; die Endlage in z weicht um 593 ab (Port -13681,
  Original -13088) — Fallmechanik FUN_8002c444 (Spur A, objfall_r36.c), hier nur notiert, nicht Auftrag dieser Spur.
* **Werkzeug-Beobachtung, nicht geprueft:** im Treiber-Lauf nb2_2020durchgang_c zuendete der Examine-Druck 2 Bilder nach
  dem Drehende nicht sichtbar (ohne RE15_MSG_LOG), mit 30 Bildern Wartezeit (Lauf d) ja; ob dieselbe Druckfolge (drei
  Druecke) auch in Lauf c lief, ist nicht belegt — der Riegel wartet 30 Bilder.
* **ROOM3050 am Original nicht gemessen (N5, Nachbesserung 2):** die Datenlage macht den Fall zum vorgesehenen Weg
  (Super Redhawk slot 6 @0x01EC6 floor 1 nur auf der gefallenen Kiste, Latch sub01 @0x02184-2196), offen bleibt, ob das
  Original denselben Schubpfad (West bis r*[31], Sued in r*[46]) zulaesst. Messweg liegt bereit: Spielstand per
  `re15_quickload.py --triangle 2 --right 5 --postload 20` (Stage 3 beginnt bei JUMP 300; Tipper 0,04 s), dann
  `orig_2050_push.py`-Muster mit K = Pool-Index von obj 1 und den Stationen West/Sued aus nb1_3050fall_d.
* **Ersatzweg nach ROOM2040 ueber ROOM2020 (N2):** der Durchgang im Suedstreifen ist per SCD geschlossen und oeffnet sich
  nur nach dem Hinweis in ROOM2010 (sub02 @0x008CA (3,114)) und dem Ereignis in 2020 (Aot slot 3 sce 3 @0x009AE -> sub02
  @0x00ADE). An der exe gemessen ist der Durchgang nach dem Ereignis (Lauf nb2_2020durchgang, s. Nachbesserung 2 N2);
  am Original NICHT gemessen (kein Spielstand mit (3,114)=1 in 2020; Messweg: JUMP 202, Flag per GDB setzen, Ereignis am
  Rechteck ausloesen). Die Glieder 2030 Band 9 -> slot 3 -> 2060 (Abnahme 1 R2) und 2060 Tritt -> slot 1 -> 2040 (Spur A)
  sind an der exe gemessen; der exe-Haenger in 2060 (R_abnahme_1.md Nachtrag) ist nicht Teil dieser Spur.

## Fuer den Nutzer

**Softlock moeglich? NEIN (ROOM1090/1091: die Dach-Kiste bewegt sich nicht). Port = Original? JA — auch fuer die
Kisten in ROOM3050 (Fall vorgesehen) und ROOM2050 (Fall im Original gemessen).**

* Die Kiste auf dem Dach von ROOM1090 (Leon) / ROOM1091 (Elza) laesst sich **weder im Original noch im Port bewegen**.
  Am Original (DuckStation, Auslieferungs-EXE, Haltepunkte in der Schubkette) und an der exe (Leon und Elza, Sprung-
  Laeufe in alle vier Richtungen UND der echte Weg aus ROOM10F0/10F1 ueber den Tritt und die Tuer) gemessen: jeder
  Schub nach Norden setzt die Kiste in die Bruestungsmauer, jeder Schub nach Westen in die Westwand, und das Spiel
  nimmt den Schub dann nicht an (Kiste springt auf ihren Platz zurueck). Von Norden oder Westen kommt man nicht an sie
  heran (Mauer/Wand), von Osten und Sueden gibt es keine Stellung, aus der ein Schub zaehlt. 0 angenommene Schuebe,
  0 Fall-Ausloeser, Kiste am Ende immer exakt auf ihrem Platz (-5960,-9000,-15482).
* Darum kann sie **nicht vom Dach fallen**; der Weg auf die Kiste und ueber die Bruestung durch das Tor nach ROOM1100
  bleibt immer offen. Die Fallmechanik der Spur A ist davon unberuehrt — sie greift dort, wo das Spiel es vorsieht:
  - ROOM3050/3051 (Punkt 4): der Fall der Kiste obj 1 ist der vorgesehene Weg — nur vom Dach der gefallenen Kiste
    erreicht man die **Super Redhawk** (Item slot 6, Floor 1). Beide Tueren des Raums brauchen die Kiste nicht. **An der
    exe gefahren (Nachbesserung 1):** Leon schiebt die Kiste vom Gang aus nach Westen bis an die Wand und von Norden
    ueber die Kante — sie faellt, kippt und landet unten (Band 0). Der Fall ist als Riegel festgehalten (`fall3050`).
  - ROOM2050/2051 (Punkt 4): die Kiste obj 6 ist die Stufe zur Tuer nach ROOM2040 (Halle) und kann ueber die Kante
    fallen — **im Port UND im Original gemessen (Nachbesserung 2: exe nb2_2050fall_c, Original DuckStation-GDB
    orig_2050_d, derselbe Weg Sued/Ost/Nord, Fall-Ausloeser in derselben Zelle, Landung Band 2)**. Das ist also kein
    Port-Softlock, sondern Originalverhalten (byte-true belassen). Faellt sie, ist diese Tuer
    weg, aber ROOM2040 bleibt erreichbar: ueber ROOM2020 -> ROOM2030 -> ROOM2060 (Tritt) -> Halle (Nachbesserung 2: der
    Durchgang in ROOM2020 ist ein Raetsel — Hinweis in ROOM2010, dann Aktion in 2020 —, s. "Nachbesserung 2" N2); der
    Weg ueber ROOM11A0 -> ROOM2070 (Nordgang) -> Galerie -> Treppe -> Halle steht dagegen erst NACH dem ersten
    Galerie-Besuch offen (zirkulaer, N1) und zaehlt nicht; die Tuer 2070 -> 2040 und die Treppe 2040 sind an der exe
    gemessen, der Tritt in 2060 von Spur A. **Berichtigung gegenueber dem ersten Stand:** ROOM2070 HAT die Tuer nach 2040 (slot 5),
    sie liegt aber in einem eigenen, von den anderen Gaengen abgemauerten Teil (Nordgang) — von ROOM2000 aus kommt man
    nicht dorthin; und der frueher genannte Weg ueber die Treppe in ROOM2030 traegt nicht (sie endet auf Band 5, die
    Tueren liegen auf Band 9).
* Keine neuen Assets fuer das Paket-Gate, keine Sprachdateien, keine Bedienhinweise, keine Entscheidungsfrage. Der
  Port erhaelt nur Messschriebe (RE15_PUSH_LOG, Pfadausgabe der Zensus-Sonde, Wegplan-Treiber RE15_WEGPLAN) und Riegel;
  kein Verhaltens-Code wurde geaendert (Diff gegen 99398dc7 in re15_port/: aot_common.c 3-Zeilen-Haken, input_pc.c
  2-Zeilen-Haken, dachkiste_r36.c/.h Messschrieb + Treiber, Tests).
* Hinweis zum Spielweg (kein Port-Thema): in ROOM2050 kann die Kiste obj 6 — wie im Original — ueber die Kante
  geschoben werden; danach fuehrt der Weg nach ROOM2040 durch ROOM2020 (Hinweis in ROOM2010 lesen, dann in 2020 an der
  Nordkante des Suedstreifens handeln), 2030 (Band 9), 2060 (Tritt) in die Halle. Das ist der Beta-Spielweg, nichts
  wurde veraendert.
* DuckStation: `settings.ini` nach den Original-Messungen aus der Sicherung zurueckgeschrieben (EnableGDBServer = false,
  ConfirmPowerOff = true, [Pad1] nur SDL-0 wie vorgefunden).
