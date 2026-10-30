# Spur P "alligator2" - UNABHAENGIGE ABNAHME 1

Baum `.claude/worktrees/r41_alligator2`, Zweig r41/alligator2, Stand 4988269c0, Basis f5a5e2f7e. Bau: `local_build.sh configure`, `local_build.sh build` -> `ninja: no work to do`.
Messordner (nicht im Repo): Scratchpad `P1/runs/<lauf>`, Werkzeuge `P1/tools/` (lauf.sh, kampf.sh, pick.sh, pick2.sh, kadaver.sh, fin.sh, rueck.sh, serie.sh, anteil.py, sprung.py, gleit.py, bogen.py, rid40a0.py; eigene Sonden p1_maul.c, p1_scan.c gegen die Engine-Bibliothek dieses Baums), Bildboegen `P1/bogen_*.png`. Alle exe-Laeufe: eigene Kopie `re15_pc_p1a_<lauf>.exe`, Umgebung bereinigt, RE15_WINDOW_SCALE=1.

## Ergebnis
**BESTANDEN.** Die vier Punkte der Aufgabe und die sechs Punkte von AUFTRAG.md Nachtrag 35 sind am exe gemessen erfuellt, Leon und Elza; die Maengel M1..M8 der Abnahme 0 sind behoben; RE-Gate und Vertrag halten. Kein Mangel. Sechs Hinweise fuer Orchestrator und Release Notes, wichtig davon H1 (ein nicht reproduzierter Absturz) und H3 (der tote Alligator ist beim naechsten Betreten wieder da, vorbestehend).

## Urteile
| # | Punkt | Urteil |
|---|---|---|
| 1 | Nachtrag 31.2 Kampfbeginn vor Leon, RE2-Band, Riegel | erfuellt |
| 2 | FG-E7 Option (a), DISABLED weg, beide gruen | erfuellt |
| 3 | Lagespruenge mit dem Kanister im Maul | erfuellt |
| 4 | Riegel Uebergabe Ankunftsszene -> Knall + 8 Zombies | erfuellt |
| N35.1 | Schluessel nach JEDEM Tod erreichbar, Riegel, echter Lauf bis zum Aufheben (Release-Pflicht) | erfuellt |
| N35.2 | Putz 1:1 wie RE2 | erfuellt (Bytes + exe; Bildvergleich mit dem laufenden Original nicht pruefbar, kein Image) |
| N35.3 | Klettern auf den toten Alligator ausschliessen | erfuellt |
| N35.4 | Finisher: Spieler verschwindet wie RE2 | erfuellt |
| N35.5 | Leiche sitzt im Maul, folgt der Kopfbewegung | erfuellt |
| N35.6 | Kanister gerade im Maul, normale Bewegung | erfuellt (Hinweis H4) |

## Punkt 1 - Nachtrag 31.2
Laeufe kL, kE (Rezept des Riegels: `RE15_SET_FLAG=3:245 RE15_DEBUG_JUMP=2080@250 RE15_FIRE_AOT=1@40#2080 RE15_EXIT_AT=650#2090 RE15_FRAMEDUMP=298-650/1:fd_`). debug.log beide: `Kampfbeginn F299: Gator (3033,-21904) Gier 1843 Zone 0, Spielfigur (-8200,-25550) Zone 0, Abstand 11808` (Band 11430..13924). state.log beide: Biss mo=4 ab F436 = +137, Treffer hp 100 -> 50 F443 = +144 (Band 121..158), Leon = Elza.
Eigener Bildanteil (`tools/anteil.py`, numpy, nicht der Auswerter der Spur): Leon rechte Haelfte Median 27,5 %, Spitze 45,6 % (F394), > 50 %: 0; Elza 26,9 %, 45,5 %, 0. Bildbogen `bogen_kE.png` F301-F445: Kopf und Hals ab dem ersten Bild rechts im Becken vor der Figur, kein Bild vor der Linse, Biss an der Leiter.
Riegel: unit_r41_alligator2_kamera(_elza) (Wurzel 4007 >= 3800, Knochen 2684 >= 2500, Gegenprobe alte Lage 1755/610 faellt durch), integration_r41_alligator2_kamera Passed; _kamera_elza im ersten Lauf rot, einzeln 3 x Passed (H2).
Urteil: **erfuellt.**

## Punkt 2 - FG-E7 Option (a)
`git diff f5a5e2f7e -- re15_port/tests/unit/probes/r41_gatorszene.cmake`: Zeile `DISABLED TRUE` weg. Breiter Lauf: integration_r41_gatorszene_tod_r1 Passed 107,8 s, _tod_r1_elza Passed 161,0 s, _tod, _tod_elza, _tod_explosion, _tod_explosion_elza Passed. integration_r41_alligator2_k3(_elza) Passed: `Wandlage mit Kopf-Klemme, Kamera 0, Tiefe 2721 = 137/10 Bildpunkte, Einstellung F84-F176; bilddiff: 44 Bilder, 44 mit Schluessel-Unterschied, Kasten 10 x 13`.
Werkzeug `schluessel_scan` selbst gefahren: `1688 Lagen, Stufe 0: 1589, 1: 48, 2: 34, 3: 16, 4: 1, unter 10,4: 0, groesster Abstand zur Leiche 6036`. Eigene Sonde p1_scan.c (x -8500..6800, z -26500..-5900, Raster 1000, acht Gieren, zwei Leon-Lagen, auch Lagen in und an den Waenden, kein Ausschluss wie im Riegel): **5376 Faelle, ohne Kamera 0, nicht erreicht 0, unter 10,4: 0**, Stufen 4987/207/142/38/2. Der Riegel-Scan ueberspringt `cut < 0`; die Sonde zeigt, dass der Fall nicht vorkommt. Code gelesen: Schleife ueber Stufen, Abbruch bei >= 104, Rueckfall beste Lage.
Urteil: **erfuellt.**

## Punkt 3 - Lagespruenge
Eigener Auswerter sprung.py (state.log, Wurzel je Bildpaar von "KANISTER angehaengt" bis R1), natuerliche Laeufe:
| Lauf | Figur / Start | Aufnahme / R1 | max | > 340 | > 400 | Weg |
|---|---|---|---|---|---|---|
| pL | Leon (-6000,-17500) | F143 / F322 | 340 | 0 | 0 | 15612 |
| te1 | Elza (0,-7000), Schuesse ab F900 | F170 / F1165 | 340 | 0 | 0 | 71739 |
| c1 | Leon (0,-16000) | F112 / F199 | 306 | 0 | 0 | 3347 |
| c2 | Elza (0,-7000) | F160 / F199 | 306 | 0 | 0 | 2116 |
| c5 | Leon (-4000,-8000) | F872 / keins bis F1400 | 339 | 0 | 0 | 11762 |

Werkzeug `trag_scan` selbst gefahren: `306 Laeufe (270 ohne Aufnahme), 275400 Bilder, Sprung max 338 ... > 340: 0, > 400: 0, Ueberstand max 1366, Bilder Ueberstand > 300: 69` = Dossier. Riegel unit_r41_alligator2_tragen (Gegenprobe harte Klemme max 554), integration_r41_alligator2_tragen Passed (`F114-F931 max 338`), _tragen_elza 3 x Passed einzeln (im ersten Lauf Access violation, H1), unit_r41_kanister2080_*, integration_r41_kanister2080_kampf Passed.
Urteil: **erfuellt.**

## Punkt 4 - Riegel Uebergabe
integration_r41_alligator2_uebergabe, _elza Passed: `Folge DACDKNZ, 8 Spawns, oben 8`, `Szene -> Kampfbeginn -> Leiter -> Knall -> 8 Zombies oben, ohne RE15_SET_FLAG=9:110`. Rezept gelesen: RE15_SET_FLAG nur 3:245, Pruefung auf `[setflag] flag(9,110`, Aktionstaste an der Leiter, Tuer Slot 0, Knall-Stimme direkt hinter der Ausloese-Zeile, Spawns auf y -14400.
Urteil: **erfuellt.**

## Nachtrag 35 Punkt 1 - Schluessel erreichbar
Rezept natuerlicher Kampf (Riegel schluessel), bewusst andere Startlagen als die Riegel; Lauf 1 -> Werkzeug wegplan -> Lauf 2 Eingabe an der exe (tools/pick.sh, pick2.sh):
| Lauf | Figur / Start | Tod | Schluessel | Aufheben (debug.log) | Klettern |
|---|---|---|---|---|---|
| c1 | Leon (0,-16000) | R1, Leiche (-6403,-12734) | (-5700,-7500) | `F586 R2090 ... 34:1` | 0 x sub=9 |
| c2 | Elza (0,-7000) | R1, Leiche (-3465,-9871) | (1400,-8500) | `F594 R2091 ... 34:1` | 0 x sub=9 |
| t1 | Elza (-6000,-17500) | Todesrolle Clip 7 (GB_TEST=3) | (1300,-24400) | `F841 R2091 ... 34:1` | 0 x sub=9 |
| t2 | Leon (0,-16000) | Todesrolle Clip 7 (GB_TEST=3) | (1300,-24400) | `F835 R2090 ... 34:1` | 0 x sub=9 |
| ers2 | Leon, Bits 9:110, 9:111 (Ersatzlage nach dem Laden) | - | (-170,-23629) | `F157 R2090 ... 34:1` | 0 x sub=9 |

Dazu die Riegel Passed: integration_r41_alligator2_schluessel `F688 R2090`, _elza `F706 R2091`, _west_elza `F837 R2091` (Leiche zwischen Figur und Schluessel, Lauf durch den Westkanal). `erreichbar_scan 1000` selbst gefahren: `1688 Todeslagen, 4704256 Stellungen, nicht erreicht 0, Klettern 0`. unit_r41_alligator2_erreichbar: 221 Lagen, 640128 Stellungen, 0/0/0, Gegenprobe Band 0: 1024 von 3600. ROOM2091.RDT ist bytegleich ROOM2090.RDT (Dateivergleich, 0 Unterschiede), die Scans gelten also fuer Elza mit.
Nicht gemessen: Figur steht beim Tod oben auf der Plattform (RE15_PLAYER_POS legt sie nicht dorthin); der Abstieg ist Bestand (unit_r34_plattform), key_begehbar rechnet Band 0 fest.
Urteil: **erfuellt.**

## Nachtrag 35 Punkt 2 - Putz
RE2 selbst gelesen (ROOM40A0.RDT, re2_scd_walk.py; EM23_OVL_0000.BIN, PSX.EXE):
- sub01 @0x008C `Ck 21 01 1d 01`, `Ck 21 05 10 00`, `Set 22 05 10 01`, `Evt_exec 04 ff 18 0f`. Tafel @0x800a78c8: [1] = 0x800cfbd8, [5] = 0x800d4874. FUN_80077360 `srlv v0,v0,a1`: Bit 0x1d = Wert 4. `80101de4 ori v0,v0,0x4` / `80101de8 sw v0,0(v1)` (v1 = 0x800cfbd8).
- sub15 @0x0916 `Evt_next`, `Switch 13 1a`, Cut 0-2 -> sub16, 3-6 -> sub18, 7-9 -> sub20, `Set 22 05 10 00` @0x0966. sub16 @0x096C: Gosub, Sleep 1, Gosub, Sleep 4, `Se_on 36 02 0d 01 01`, Gosub, Sleep 3, Gosub, Sleep 4, Gosub.
- sub17 @0x0994: erster Sce_rnd -> (r & 0xfff) - 0x66ce -> `Work_copy 1d 06 10 01` @0x09D8 = Ziel 0x09EC = z; zweiter -> (r & 0x3fff) - 0x4a38 -> `1d 05 08 01` @0x09DC = Ziel 0x09E8 = x; `Sce_espr_on 3a 00 19 00 00 00 00 06 .. e8 ea ..`. Sce_espr_on @0x800565a4-0x80056618 setzt a0 = 0x19000600. Port gr2_putz_takt: r1 -> z, r2 -> x, Bilder 1/2/6/9/13, Satz 13 mit dem dritten: stimmt.
- Sce_rnd FUN_80052ca0 @0x80052cd4-f8 = gr2_putz_rnd. Op 21: Tafel @0x8009d8bc = 0x8001f5a4, Funktion gelesen = op_21. Ringseiten gegen RID ROOM2090 @0x60 geprueft (Cut 0/1 Sued, 2/3 Ost, 4/5 Nord, 6/7 West).
Exe: pN: Stoesse F41, F424, F438; aud (Ton an): `[gator-re2] Raum-SE 13 -> GATOR40A0.VBS Satz 2`, direkt danach `[se] Stimme: se=2 layer=0 vag=2 note=71 ... vol=80`; op_unbekannt 0 in allen Zeilen (pN, te1). GATOR40A0.VBS md5 8afd43aa998b8fcaeb843c5c9c9b4535 = Dossier. Bild `bogen_putz5.png` (F441-F456, Cut 0): zwei Staubsaeulen rund 15-20 Bildpunkte breit fallen die Rueckwand herab, Brocken 1-3 Bildpunkte, bis unter die Wasserkante. Kein Staub um Leon beim Dash mehr, unit_r41_re2gator_putz + integration_r41_re2gator Passed.
RE2-Kameras (RID ROOM40A0, rid40a0.py): Bein 1 liegt fuer Cut 0 fast ganz hinter der Kamera, fuer Cut 2 5000..21000 davor; die Tiefen im Port liegen im selben Bereich, kleine Brocken auf Entfernung sind also die RE2-Form. Einen Bildvergleich mit dem laufenden Original gibt es nicht (kein Image).
Urteil: **erfuellt** (Ort = PORT-WAHL, weil ROOM2090 ein Ring aus vier Seiten ist; im Code so gekennzeichnet).

## Nachtrag 35 Punkt 3 - Klettern
Eigene Laeufe c1_kv2 (Leon), c2_kv2 (Elza): Ziel des Wegplans = Kadaver statt Schluessel, Figur steht 267 bzw. 323 neben der Leiche ((-6138,-12699) / (-3187,-9707)), sieben Aktionstasten: climb.log 2076 / 2286 Zeilen, 0 x sub=9. Ursache war M1: `8002d358 andi v0,v0,0x101`, `8002d364 lbu v1,0(s3)`, `8002d368 lbu v0,130(s0)`, `8002d370 bne v1,v0,0x8002d3a4` (RE1.5 PSX.EXE selbst gelesen). ROOM2090.RDT @0xA6C `2d 02 00 00 02 00 0a 00`, @0xA8E `2d 03 00 00 02 00 0a 00` gelesen: pc[4] = 2. Koerper: R1 P0 `801022f8 ori v0,v0,0x80a`, `801022fc sw v0,0(s1)`, FUN_80034d0c `((*param_1 | uVar4) & 2) == 0`, P3 `8010265c ori v0,v0,0x4000`, `80102664 sh v0,270(s1)` gelesen. unit_r41_alligator2_koerper Passed (lebend 5 von 5 schieben, R1 + Leiche 0 von 63 Bildern, kein Ziel).
Urteil: **erfuellt.**

## Nachtrag 35 Punkt 4 - Finisher
RE2 gelesen: `80101188 jal 0x8001a330` (a1 = 5), `80101190 beq v0,zero,0x801011e8`, `80101194 lui v1,0x8`, `8010119c addiu a0,a0,-1032` = 0x800cfbf8, `801011a8 or v0,v0,v1`, `801011ac sw v0,0(a0)`, `801011b0-c0` Phase + 1.
Exe fL (Leon), Riegel _finisher, _finisher_elza: `vis 639 2090 0`, `Modell aus ab F639, vorher 639 Bilder sichtbar, danach 81 Bilder ohne Spielerfigur`. Bildboegen `bogen_fin.png`, `bogen_finE.png` F612-F700: bis zum Clip-5-Ende Figur quer am Kopf, danach nur der kauende Alligator, kein Rest (auch kein Zopf bei Elza). no_draw klebt nicht: langer Lauf nach Game Over Neustart, `vis 1230 1170 1`.
Urteil: **erfuellt.**

## Nachtrag 35 Punkt 5 - Leiche im Maul
Laeufe szL, szE (F50-F300, Bild fuer Bild gleich), Boegen `bogen_szL2.png`, `bogen_szL3.png`, `bogen_szL4.png`, `bogen_szE.png`: die Leiche liegt quer im Maul, Beine zur einen, Rumpf zur anderen Seite, und folgt dem Kopf durch das ganze Schuetteln. Eigene Sonde p1_maul.c (Harness, Szenenkamera Cut 1): Rumpfmitte <-> Maullage 4..13 in den Stichproben; Clip 5 Bild 102: Gelenk Part 6 (160,82), Maullage (157,52), Spitze x 3000 (155,29), Bild 118: (164,95), (171,72), (178,57) - die Maullage liegt im offenen Maul, die Mitte der Leiche verschwindet dort zwischen den Kiefern, die Enden ragen seitlich heraus. unit_r41_alligator2_leiche(_elza): `Abstand zum Maul beim Anhaengen 1123, angekommen nach 8 Bildern, danach hoechstens 35; Clip-5-Ende Bild 202, weg ab Bild 202`.
Urteil: **erfuellt.**

## Nachtrag 35 Punkt 6 - Kanister
Sitz: RE2 gelesen: `801019c4 addiu a1,zero,1750`, `80101a30 addiu a1,zero,1750` / `80101a38 addiu a2,zero,145`, `80101ad4 addiu a1,zero,-210` / `80101adc addiu a2,zero,55`, `80101048 addiu v0,zero,-23078`, SCD @0x0484 `32 00 e0 a6 40 f8 e4 da` (x -22816), @0x048C `33 00 90 06 00 00 00 fc`. RotZ(-90) legt Modell-y auf Welt +X, unabhaengig vom Winkel um x; KANISTER40A0.MD1 selbst ausgemessen: x -267..267, y -2784..0, z -260..274, Ursprung am Tank-Ende. Rig-z = -262 bei Gier 0xC00 nachgerechnet. Der Tank ragt in RE2 also tatsaechlich nach einer Seite.
Messung: unit_r41_alligator2_kanister_sitz: acht Gieren, Achse * Quer 4073..4083/4096, Lokallage (1750,-210,-262). Exe knL F184: Gier 2113, Bahn vom Betrachter weg, Tank im Bild quer (`bogen_kanL2.png`) = rechtwinklig zum Kopf. `Kanister-Sitz ... Abweichung max 0` ueber 150 angehaengte Bilder.
Bewegung: Clips 0/1/2/3/4 laufen, Weg siehe Punkt 3, kein Sprung, Biss-Zyklen laufen (gleit.py: te1 995 Bilder Clip 0: 230, 1: 206, 2: 150, 3: 150, 4: 258). Mit dem Tank quer ist die Kopf-Klemme an der Wand oefter aktiv (H4).
Urteil: **erfuellt.**

## Gates
**RE-Gate.** Stichprobe selbst disassembliert/gelesen, alle stimmen: EM23_OVL_0000.BIN @0x801022ec-fc, @0x8010265c-64, @0x80101190-ac, @0x801019c4, @0x80101a30-38, @0x80101a4c-54, @0x80101ad4-dc, @0x80101048, @0x80100eb0-c0, @0x80102784-88, @0x80101dd8-f8, Stufendauern @0x80101970/@0x801019dc; RE2 PSX.EXE FUN_80034d0c, FUN_80027160 (Skala Bit 0x800), FUN_80052ca0, FUN_80077360, FUN_8001f5a4, @0x8009d8bc, @0x800a78c8, @0x800565a4-0x80056618; RE1.5 PSX.EXE @0x8002d350-70; ROOM2090.RDT @0xA6C, @0xA8E, RID @0x60; ROOM40A0.RDT SCD 0x008C, 0x0484, 0x048C, 0x0916-0x09F0. Port-Werte ohne Adresse sind als PORT-WAHL / NUTZER-VORGABE mit Messung gekennzeichnet (GSZ_LEICHE_*, GSZ_GATOR_GIER_STD, GSZ_KEY_AUF_R*, GSZ_BILD_RAND, GSZ_SUCH_STUFEN, GSZ_SPIELER_*, GSZ_LEICHE_MITTE_Y, k_putz_seite, Saat von gr2_putz_rnd). Einzige Luecke ohne Etikett: Suchfenster +-40 Raster im Rueckfall `cut < 0`; der Fall kommt in 5376 Lagen nicht vor. `git diff f5a5e2f7e HEAD -- re15_port | grep -i "deferred|tunable|interim|for now|faithful|plausibel|TODO|FIXME"`: 0 Treffer in hinzugefuegten Zeilen; getenv im Diff: 0; re15_gr2_kk_hart_setzen ist Pruefhaken ohne Spielweg. Der Fix erklaert jeden Befund (M1 Band-Gate, M2 Push-Gate, Sitz der Leiche, Sitz des Kanisters: Ursachen im Dossier getrennt gemessen, hier nachgelesen). **Haelt.**

**Vertrag.** Gegen die Basis f5a5e2f7e (master ist waehrend der Abnahme weitergelaufen, `git diff master` zeigt sonst fremde Commits): 36 Dateien, nur analysis/, re15_port/engine, include, platform/pc (eine Zeile), tests, tools, shared_assets/RE2. Kein release/, platform/android/, shared_assets/PSX/, pri/, synchro/, keine Test-CMakeLists. Keine neuen Bank-9-Bits, keine Nachrichten-Ids, keine Eintraege in k_port[]. GATOR40A0.VBS steht schon in make_package.sh. Neue Tests: eigene Laufordner, exe-Kopie mit Praefix, `cmake_minimum_required`, libm nur `if(NOT WIN32)`. Nachgezogene Pruefungen in test_r41_gatorszene.c und test_r41_re2gator.c gelesen: begruendet (Lage, Doppel-Naehe, K = X moeglich, Putz neu); der Schuss F955 statt F931 in tod_r1 haelt die Vorbedingung des Riegels, die K3-Wandlage deckt integration_r41_alligator2_k3. enemy_ai_boss_gator.c +35 Zeilen ist mehr als "1-5 Zeilen Haken", wie in Abnahme 0: Mechanismus liegt dort, markiert. **Haelt.**

**Tests.** Eigene Laeufe:
1. `ctest -R "r41_alligator2" -j4 --timeout 1300 -V` unter Last: 22 von 24 gruen, rot _kamera_elza und _tragen_elza. Einzeln `--repeat until-fail:3`: beide 3 x Passed (`100% tests passed, 0 tests failed out of 2`). Gruende H1, H2.
2. Breite Gruppe (Scratch regex.txt: Spur-Nachbarn gatorszene, re2gator, kanister2080, Kollision, Item, Tueren, Kamera, re2fx, Werfer, Wasser, Spinnen, Body-Push, Birkin-Szenen) `-j4 --timeout 1000`: **`99% tests passed, 1 tests failed out of 190`**, 1376 s. Rot nur `529 - integration_r35_granate`: wand/zombie/gator ok, `r35_granate [birkin]: Slot 2: Typ 30 grid 10 im Bild 399 (soll 36 / 13`, dieselbe Zeile wie in Abnahme 0 und im Dossier auf der Basis. Der Diff beruehrt kein Birkin-Modul.
Keine volle Suite im Spurbaum.

## Maengel
Keine.

## Hinweise (kein Mangel, fuer Orchestrator und Release Notes)
- **H1 Absturz, einmal, nicht reproduziert.** integration_r41_alligator2_tragen_elza im Lauf 1: `exe exit=Access violation`, letzte Zeilen state.log + debug.log F56 (23:46:22), Renderer direct3d. Dasselbe Rezept danach 21 x sauber: 3 x ctest, te1 bis F2400, 3 x gdb bis F200, 2 x gdb bis F400 (davon einmal mit 35 x Fenster maximieren/wiederherstellen), 12 x Serie bis F110 unter Last. Windows-Ereignisprotokoll: kein Eintrag zu re15 (Abfrage Application ab 23:30), kein WER-Bericht, also weder Modul noch Offset. Zeitgleich im System: googledrivefs Unmount 23:46:04-49, DTLite.exe abgestuerzt 23:46:43, Disc-Soft-Dienst-Fehler ab 23:46:05, Kernel-Power 105 um 23:50:02. Ursache damit NICHT belegt, ein Bezug zu Spur P aber auch nicht: Putz, Op 21 und Staub liefen in allen 21 Wiederholungen ueber dieselben Bilder. In der vollen Suite beobachten; tritt er wieder auf, ist er ein Blocker.
- **H2 Riegel kamera_elza** faellt, wenn das Fenster von aussen die Groesse aendert: `bildanteil: 26 Bilder ...` statt 149, im eigenen Lauf kL dasselbe F298-F330 (`[framedump] 1920x1009`, um 23:50). Kein Spielfehler; der Auswerter wirft solche Bilder weg und meldet FEHLER.
- **H3 Der tote Alligator ist beim naechsten Betreten wieder da.** Lauf rN: natuerlicher R1-Tod F360, Leiter nach 2080, zurueck: `[enemy] ROOM2090: Alligator (0x23) in Wasserzone gesetzt`, F40 `st=1 ... @(-7200,-9200) hp=25300`, Schluessel unveraendert an (-5700,-1725,-7500), `Kanister verbraucht (R1-Tod) -> Bit 116 = 0`: volle Energie, kein zweiter Kanister, also nicht mehr toetbar. Der Schluessel bleibt erreichbar (ers2, Aufnahme mit lauerndem Alligator), kein Softlock. Vorbestehend: platform/pc/main.c "em_flag_id 0xFF = kein Persist-Gate (Boss erscheint je Betreten)", nicht im Diff, nicht in den Punkten dieser Spur. RE2 anders: SCD-init ROOM40A0 @0x0054 `Ck 21 04 33 00` vor `Sce_em_set 44 00 00 23` @0x00A0, sub02 @0x00AE `Set 22 04 33 01`: nach dem Beginn des Kampfs kommt der Gator nie wieder. Zur Entscheidung an den Nutzer.
- **H4 Tank quer an der Wand.** Mit dem Tank quer ragt ein Ende bis 2600 zur Seite, die Kopf-Klemme ist oefter aktiv: te1 288 von 995 Bildern, dort Clip 1 74 je Bild vorwaerts statt 143 ohne Kanister, Clip 4 rueckwaerts 7026 statt 0; knL 18 Bilder seitlich > 100 (max 314). Kein Sprung (Punkt 3), aber sichtbares Abdruecken von der Wand. Die Ueberlaenge ist 1,5 x RE2 (Dossier OFFEN 9).
- **H5 Dossier-Zahlen:** OFFEN 3 nennt noch Ueberstand max 2282 / 0,43 % (neu 1366 / 69 Bilder), Punkt 2 noch `Stufe 0: 1595 ... 4: 0`, groesster Abstand 5271 (neu 1589/48/34/16/1, 6036). Der Kommentar vor schluessel_scan ebenso.
- **H6 Integration:** master lief waehrend der Abnahme weiter (zuletzt 6e3aa3289, 25+ Commits, darin aa463a744 RE15_GATOR_ORIGINAL in enemy_ai_boss_gator.c, gatorszene_r41.c). `git merge-tree --write-tree HEAD master`: Konflikt nur in AUFTRAG.md (Nachtrag 35 auf beiden Seiten), Code mischt automatisch; Riegel danach neu fahren.

## Nicht geprueft
PSX-Bau, RE2-Original am Emulator (kein Image), volle Suite, voller Weg Leiter -> Szene -> Treffer -> Kanister -> R1 in einem Lauf (zwei Versuche v1/v2: Schuesse ab F300 trafen vor dem ersten Biss nicht, danach haelt der HP-Messhaken die Figur im Treffer-Zustand; die Teilstuecke decken uebergabe und kanister2080_kampf).
