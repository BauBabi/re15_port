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
* NEU `engine/src/dachkiste_r36.c`, `include/re15_dachkiste_r36.h` — Messschrieb RE15_PUSH_LOG (1.1).
* `engine/src/aot_common.c` re15_prop_push_tick: 3 Zeilen Haken ("Runde 36 Spur R: RE15_PUSH_LOG") hinter FUN_8003b558.
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
| 1090 / 1091 obj 1 | 5 | (-13600,1300) aus 10F0/10F1 | 9 | **0** | x [-5978,-5959) z [-15503,-15481) — Seiten W und N ohne Schubstellung |
| 3050 / 3051 obj 1 | 4 | (-3596,1523) aus 30E0 | 12729 | **7** (erste (-23875,-18500), Spieler NOERDLICH) | x [-24100,-1800) z [-22350,1123) |
| 2050 / 2051 obj 6 | 3 | (9100,4100) aus 2000/20B0 | 3575 | **1** ((7550,-12000), Spieler SUEDLICH) | x [-6650,9000) z [-11700,-5800) |
| 2050 obj 6 nach sub02 (Zelle 0x33 u0 0, 0x32 u0 0xff) | 3 | (9100,4100) und (-6800,-5650) | 3575 | 1 (dito) | dito |

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

### 4.3 ROOM2050/2051 obj 6 — Tuer nach ROOM2040 braucht die Kistenoberkante, aber ROOM2040 hat drei weitere Tueren

* Records (ROOM2050.RDT main00): obj 6 @0x01D0E Band 3 (-1600,-5400,-9348) rot 2048, unbedingt; `Door_aot_set slot 2
  @0x01C8A floor 4` (Rechteck x -8200..-7200, z -6200..-5000) -> ROOM2040 (21700,-5400,-5400) — floor 4 = Band 3 + 1 =
  Oberkante der Kiste obj 6 (dieselbe Mechanik wie in 1090). Die Tuer-Flaeche liegt WESTLICH der Band-3-Wand r*[51]
  (x -7550..-6150, z -4500..850), die sub02 @0x02294-22A4 per `Sca_id_set r*[0x33] u0 = 0` erst oeffnet (Raetsel: drei
  Kisten obj 1/2/3 (Band 0) in die Kerben slot 3/4/5 -> (5,0..2) -> sub01 @0x0201A `Aot_reset 6 -> sub02`).
* Absprung: Band-3-Zelle r*[49] x 6350..8750 z -13950..-12150 Wort 0x3202; darunter r*[23] Band 2 x 4550..6350 Wort 0x2102
  und r*[77]/[78] Band 1 Wort 0x1002 — eine KASKADE von Absprungzellen bis Band 0: die Kiste soll dort hinunter. An der
  Kante: Aot slot 7 @0x01E3C/@0x01EC6 (floor 3, x 6800..8300, z -12000..-11200; sce 0 bzw. sce 1 Nachricht nach (3,104))
  und slot 6 @0x01E28 (floor 3, x 4700..5500, z -11550..-10750, sce 3 -> sub04: Nachricht, (3,104), obj 4 Member 1 (y)
  -16200 -> -1400, Kameras). sub02 sperrt nach dem Raetsel r*[0x32] (x 6650..8550, z -12150..-11650) mit u0 0xff.
* Tueren nach ROOM2040 (Zensus aller STAGE2-RDT, Tuer-Nutzlast Byte 23 == 0x04): ROOM2050/2051 slot 2 (floor 4),
  ROOM2060/2061 slot 1 (floor 1), ROOM2070/2071 slot 5 (floor 0), ROOM2040/2041 selbst slot 2/3 (floor 5, Selbsttueren).
  Die Kiste obj 6 ist also NICHT der einzige Weg nach ROOM2040.
* Zensus: obj 6 erreicht mit dem Spieler die Absprungzelle (von Sueden geschoben, Fallrichtung -Z = ueber die Kante
  nach Norden), vor und nach sub02. Ob die Kaskade im Original die Kiste bis Band 0 bringt und ob sie unten gebraucht
  wird (obj 8 @0x01D4C ist die geparkte Band-0-Kiste unter `Ck (5,7)=1`, (5,7) wird in 2050 nirgends gesetzt), ist
  Teil des STAGE2-Raetsels und hier nicht Auftrag; byte-true belassen (Original-Mechanik, Tuer anderweitig erreichbar).

## Tests

Alle in `tests/unit/probes/r36_dachkiste.cmake` (GLOB), kein Eingriff in tests/*/CMakeLists.txt.

* `unit_r36_dachkiste_zensus` (`probe_r36_dachkiste pin`, Vorgaenger, 4.1): Zensus MIT Spieler ueber die Praedikate der
  Schubkette — ROOM1090/1091 obj 1 erreicht KEINE Absprungzelle, Kistenmitte bleibt in z [-15503, -15482] (notwendige
  Bedingung, Spielerfreiheit ueberschaetzt); ROOM3050/3051 obj 1 und ROOM2050/2051 obj 6 erreichen eine (Punkt 4).
* `integration_r36_dachkiste` (NEU, `tests/integration/test_r36_dachkiste.cmake`, echte exe, Messkopie
  `re15_pc_r36r_haken_<id>.exe`, Grundumgebung wie test_r36_inventar.cmake, ein Wiederholungsversuch bei Fenster-Flattern):
  - `leon`: ECHTER WEG ROOM10F0 ((9,71)=1) -> Tritt -> Klettern -> Tuer slot 1 -> ROOM1090 -> Korridor -> 3 Schuebe. MUSS:
    `loaded room10f0.rdt`, kein `[cut10f0] ... Szene gestartet`, `[aot] DOOR FIRE slot=1 rect=(-4750,-1750,hw=600,hh=500)
    target_cut=0 spawn=(-13600,-9000,1300)`, `loaded room1090.rdt`, >= 1x `room=1090 p=1 obj=1 SCHUB-GESPERRT ...
    ziel=(-5960,-155xx) ... band=5` + `TREFFER-ZELLE ... x -7256..4624 z -18560..-16403`, KEIN `SCHUB-OK` fuer obj 1,
    KEIN `p=1 AUSLOESER`/`zust`/`BODEN`/`LANDUNG`, letzte Lage `p1 (-5960,-9000,-15482) b5`.
  - `elza`: derselbe Weg mit RE15_PSELECT_AUTO_SWITCH=1 (`Spieler-Familie PL04`, ROOM10F1 -> ROOM1091), dieselben MUSS.
  - `west`: Leon, Sprung 1090, Tasche (-4610,-15155) Blick 2112: `SCHUB-GESPERRT ... ziel=(-60xx,-15482)` +
    `TREFFER-ZELLE ... x -9200..-6878 z -18524..265`, kein SCHUB-OK, kein Fall, Record.
  Er diskriminiert: ein Port, der den Schub annimmt oder die Kiste fallen laesst, faellt durch.
  ⛔ Makro-Falle beim Bau des Riegels (gemessen): Makro-Argumente werden im Makro ein zweites Mal ausgewertet, ein `\\(`
  im Aufruf wurde zur Regex-Gruppe `(` und `\\[x\\]` zum Klammerausdruck — der erste Lauf fand den gesperrten Schub
  "nicht", obwohl push.txt ihn enthielt. Regex-Argumente deshalb ohne Backslash (`[(]`, `[.]`), Klartext per string(FIND).
* Gezielte Laeufe (KEINE volle Suite im Spurbaum):
  - `ctest --test-dir re15_port/build -R "unit_r36_dachkiste_zensus|unit_r36_objfall|unit_r36_klettern|unit_1090_|prop_sca_collision|prop_point_in_poly|prop_zone_dispatch|unit_r22_10f0_figur" --timeout 900`:
    `100% tests passed, 0 tests failed out of 18` (unit_r36_dachkiste_zensus 8.60 s, unit_r36_objfall, unit_r36_klettern_*,
    unit_1090_room/push/fire/push_anim/climb/stay_on_crate/flame_out/nodraw/npc_prop_push, prop_sca_collision ...).
  - `ctest --test-dir re15_port/build -R "integration_r36_dachkiste" --timeout 1200`: (Summenzeile s.u., Abschluss)

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
* Nicht Auftrag, byte-true belassen (4.3): ob die Absprung-Kaskade in ROOM2050/2051 die Kiste obj 6 im Original bis
  Band 0 bringt und ob sie unten gebraucht wird (STAGE2-Raetsel; ROOM2040 hat drei weitere Tueren).

## Fuer den Nutzer

**Softlock moeglich? NEIN. Port = Original? JA.**

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
    erreicht man die **Super Redhawk** (Item slot 6, Floor 1). Beide Tueren des Raums brauchen die Kiste nicht.
  - ROOM2050/2051 (Punkt 4): die Kiste obj 6 kann ueber die Kante fallen; die Tuer nach ROOM2040 auf ihrer Oberkante
    waere dann weg, aber ROOM2040 hat drei weitere Tueren (2060/2061, 2070/2071, 2040 selbst) — kein Fortschritt haengt
    an dieser Kiste.
* Keine neuen Assets fuer das Paket-Gate, keine Sprachdateien, keine Bedienhinweise, keine Entscheidungsfrage. Der
  Port erhaelt nur einen Messschrieb (RE15_PUSH_LOG) und einen Riegel, der genau diesen Mechanismus festhaelt.
