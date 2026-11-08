# Runde 41 - Spur G2 "gatorfix" - UNABHAENGIGE ABNAHME 1 (2026-11-01)

Baum `.claude/worktrees/r41_gatorfix`, Zweig `r41/gatorfix`, HEAD 867c95480 (Basis 789f6e681; Commits eb6e37419, 111b2ac04,
dded2f5f6, 74acc2f83, d8aa99803, e139e9b6e, 867c95480). Pruefer: Abnahme-Agent, glaubt dem Bau nichts. Kein Code geaendert, kein Commit.
Laufordner (nicht im Baum): `<SP>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/G2abn1`
(je Lauf `runs/<name>/` mit video.mp4, debug.log, state.log, gator_boss.log, gator_trag.log, env.txt). Belegbilder: `G2_abnahme_1/`.

## 0. Urteil

**NICHT BESTANDEN.** Punkt 7 (Haengen mit Kanister) ist im freien Spiel nicht behoben, Punkt 2 (Wurf/Fang) ist im Bild nicht lesbar.
Punkte 1, 3, 4, 5, 6, 8 erfuellt (Hinweise unten). Gegenprobe rot, Raumkette Leon/Elza bis Schluessel und Wiedereintritt ok,
keine Nebenwirkung auf die Alligator-Riegel (104/104 gruen).

| Punkt | Urteil | Beleg (kurz) |
|---|---|---|
| 1 Leiche im Maul | erfuellt, Rest beim Zubeissen | Haltephase quer im Maul, nichts unter dem Kinn; im Biss-Moment ~0,3-0,5 s Leiche unter/durch den Kiefer (Leon und Elza gleich) |
| 2 Wurf/Fang/Verschwinden | **nicht lesbar** | Schnauze im ganzen Wurf hinter dem oberen Kino-Balken; Leiche steigt vor dem Hals, sinkt vor dem Hals wieder ab und verschwindet auf halber Halshoehe |
| 3 Montage 5 Zombies zur Leiter | erfuellt | 5 Zombies, alle >= 2476 naeher (Leon/Elza), vorher 8 / 4 gut |
| 4 Marvin/John 3 Zombies | erfuellt | 3 Zombies, Bewegungsrichtung 82-99 Grad gegen Leiter 74-90 Grad, Schuesse F19/F36/F53 (1,8 s), kleinster Rest zur Leiter 9660 (Leon/Elza) |
| 5 Musik | erfuellt | `[bgm] ... FF3C -> MAIN3C [unveraendert, laeuft durch]` in 2080/2081 und zurueck, kein zweites `loaded` |
| 6 Schluessel groesser | erfuellt | Ankunft Ring 14x12 -> 24x21 Fensterpunkte (gleiches Bild, gleiche Zeit); nach dem R1-Tod Ring 43x51 im Schluessel-Schnitt |
| 7 Haengen mit Kanister | **nicht erfuellt** | freies Spiel: Leon 11,7 s Wand-Reiben an der Nordwand, Elza 6,0 s regungslos mit dem Ruecken zur Spielerin, 6,4 s / 5,9 s an Nord-/Suedwand |
| 8 kein Anschwimmen | erfuellt, Kampf kann stocken | 0 x `ANSCHWIMMEN`; Aufnahme Elza F520 (17 s), Leon erst F3650 (122 s), nur weil die Figur am Kanister vorbeilief |

## 1. Bau, Messweg

- `bash re15_port/tools/local_build.sh build` im Spurbaum: `ninja: no work to do.`, `=== LOCAL-BUILD-OK (build)`. exe 2026-11-01 22:02:20,
  md5 bd77cb8726eaaceed4ce568d2ef3fe56; juengste Quelle (gator_re2_r41.c, kanister2080_r41.c) 21:59:45, kanister2080_r41.c.obj 22:02:18; nach
  der exe nur der Doku-Commit 867c95480.
- Vorher-exe: `git archive 789f6e681 re15_port` nach `<SP>/alt` (shared_assets als Verbindung auf den Spurbaum), `local_build.sh configure`,
  `cmake --build build --target re15_pc` (1 min 37 s, kein zweiter Bau gleichzeitig), md5 1772aec70b1cfa040ffd540436e94bea, Messkopie
  `re15_pc_g2abn1_alt.exe` im build-Ordner des Spurbaums.
- Jeder Lauf: eigene exe-Kopie `re15_pc_g2abn1_<lauf>.exe`, gdigrab auf das hwnd DIESES Prozesses (aus der eigenen Windows-PID), 30 Bilder/s,
  RE15_WINDOW_SCALE=2 (Fenster 640x480), kein AUTOSHOT, kein SOFTWARE_RENDER, SDL_ASSERT=always_ignore, Ton an. Beendet nur eigene PIDs
  (wieder_leon/wieder_elza nach dem Wiedereintritt per `kill` der eigenen msys-PID). Helfer `<SP>/lauf.sh`.
- Szenenlauf: `RE15_SET_FLAG=3:245 RE15_DEBUG_JUMP=2080@250 RE15_FIRE_AOT=1@40#2080` (Elza: Auswahl-Auto, 2081/2091).
  Kampflaeufe: `RE15_DEBUG_JUMP=2090@250 RE15_SET_FLAG=9:110,3:245 RE15_K2080_GATOR_HP=24990 RE15_R39_MESS_SPIELER_HP=100`, Start (-6000,-17500).

## 2. Punkte 1-6 im echten Fenster

**P1 (Laeufe ank_leon, ank_elza).** Haltephase (Leon t 31,6-32,9 s): Leiche quer im Maul, Kopf links, Beine rechts, nichts unter dem Kinn -
`G2_abnahme_1/p1_leon_biss_und_halt_t31.05_31.4_32.0_32.7.jpg` (unten links/rechts). Rest: im Biss-Moment (t 31,05 / 31,4) liegt die Leiche
unter dem offenen Unterkiefer bzw. ihr Rumpf schneidet durch die Schnauze (oben links/rechts), etwa 0,3-0,5 s; Elza identisch
(`p1p2_elza_biss_halt_wurf_t45.5-51.5.jpg`, Bild 1,2/1,4). Deckt sich mit dem Dossier ("Einschwenken ... bis 24 Scheitel ausserhalb").
Kein Mangel, Hinweis H1.

**P2 (ank_leon 30 Bilder/s, `p2_leon_wurf_fang_t32.97-33.67_30fps.jpg`; Elza gleich).** Log `Folge W F145 wert=57`, `F F163 wert=75`,
`V F165 wert=77` stimmen. Im Bild: mit dem Hochreissen verschwindet die Schnauze hinter dem oberen Kino-Balken (Fenster y 0..48) und bleibt dort
bis nach dem Verschwinden. Die Leiche steigt VOR dem Hals bis knapp unter den Balken, sinkt in den letzten ~6 Bildern vor dem Hals wieder ab
und ist im naechsten Bild weg - auf halber Halshoehe, ohne dass ein Maul zu sehen ist. Ein Spieler sieht "Leiche rutscht am Hals hoch und
runter und ist weg", nicht "hochgeworfen, mit dem Maul gefangen, gefressen". Der Bau hat das selbst gemessen (Dossier P2 "Kamera": Schnauze
hinter dem Balken) und Cut 1 trotzdem behalten. **Mangel M2.**

**P3 (ank_leon/ank_elza, `p3_leon_montage_5zombies_t21.5-27.jpg`).** 5 Zombies, sichtbar nach links (Leiter ausserhalb des Bildes links) gehend.
Auswerter des Baus auf MEINEN state.log: `ganglog montage: 156 Bilder an der Leiter, 5 Zombies, 5 gut, kleinster Weg 2476` (Leon und Elza);
Vorher-exe: `8 Zombies, 4 gut, kleinster Weg -452`.

**P4 (frei_leon/frei_elza, `p4_leon_marvin_3zombies_t19.9-22.0.jpg`, `p4_leon_marvin_folge_t19.3-23.3_6fps.jpg`).** `ROOM2080 SZENE (Marvin
0x40, 3 lebende Zombies` / `ROOM2081 SZENE (John 0x49, 3 lebende Zombies`; Schuesse `F19/F36/F53 ... Treffer` = alle drei in 1,8 s. Aus state.log:
Bewegungsrichtung der Zombies 82/99/83 Grad gegen Richtung zur Leiter 74/83/90 Grad, Weg 131/821/735 bis zum Schuss; `ganglog szene: 3 Zombies,
3 gut, kleinster Rest 9660`. Im Bild steht Marvin in der Tuer und schiesst, die drei gehen an ihm vorbei nach links. Hinweis H2: der erste
Zombie wird schon nach 19 Bildern (104 Einheiten) erschossen, sein "Laufen" sieht man kaum.

**P5 (frei_leon/frei_elza/kill_leon debug.log).** `[bgm] stage=1 room=08 entry=FF3C -> MAIN3C ... [unveraendert, laeuft durch]` beim Ausloesen,
ebenso zurueck in 2090/2091; `[bgm] loaded` fuer MAIN3C genau einmal (kein Neustart). Erst nach dem R1-Tod `entry=042E -> MAIN2E` (Raummusik).

**P6.** Ankunft, gleiche Zeit t 19,0 s, gleiche Kamera, Vorher-exe gegen Spur-exe: Ring 14x12 -> 24x21 Fensterpunkte
(`p6_leon_schluessel_ankunft_vorher_nachher_4x.jpg`). Nach dem R1-Tod (kill_leon, Schluessel-Einstellung Cut 5 F235-F327): Ring 43x51
Fensterpunkte, gut sichtbar (`p6_leon_schluessel_nach_R1-tod_cut5_t41.8.jpg`). Ein Vorher-Bild in derselben Kamera gibt es nicht (Vorher-Lauf
endete in Cut 4 an anderer Lage).

## 3. Punkt 7 - freies Spiel mit Kanister im Maul (Kernpunkt)

Rezept frei_leon/frei_elza: Ausloeseschuss (`RE15_GIVE=3:1`, danach leer), dann je Raum ein Zufallsweg im Tank-Steuer (Saat 1/2, `<SP>/walk.py`:
Rennen 1-3,5 s, Drehen 0,2-0,9 s, Bogen-Rennen, kurze Pausen) ueber 8150 Bilder; nach der Rueckkehr 280 s Spiel. Messzeile `RE15_G2_TRAG_LOG`
(je Bild Wurzel, Takt, Schritt nach den Klemmen). Auswertung `<SP>/haenger.py` + `<SP>/still_state.py`: Stillstand = Wurzel bewegt sich in 30 Bildern
netto < 300; "fern" = Kopf (Wurzel + 4200 voraus) > 2500 vom Spieler, RE2-Dash (Clip 2/3) ausgenommen. Gleiche Eingabe an der Vorher-exe
(alt_frei_*); die Wege laufen auseinander (Biss-Stoesse, andere Aufnahmezeit) - Vergleich, kein A/B im selben Weg.

| Lauf | Trag-Zeit | Stillstand fern vom Spieler | Episoden >= 2 s | laengste |
|---|---|---|---|---|
| frei_leon (Spur) | 158 s | 28 % | 6 | **11,7 s** F7545-F7896 Wurzel (-3952,-10027), Kopf 218 vor der Nordwand |
| frei_elza (Spur) | 262 s | 27 % | 7 | 6,4 s F2078-F2269 Nordwand; 6,0 s F6979-F7158; 5,9 s F3437-F3613 Suedwand |
| alt_frei_leon (vorher) | 275 s | 13 % | 2 | 10,2 s F3953-F4259 (2848,-22595) = Lage der Nutzer-Marke |
| alt_frei_elza (vorher) | 275 s | 7 % | 1 | 2,2 s |
| plat_leon (Spur, Figur steht im Wasser an der Plattform-Westkante) | 85 s | 3 % | 0 | 1,1 s |

Die Stelle der Nutzer-Marke (Ostwand, Rampen-Hop) ist weg - die Hoerer verschieben sich an andere Waende:
- **frei_leon F7545-F7896 (11,7 s):** Figur in der NW-Ecke (-8432,-5868); der Gator paddelt Clip 1 mit Gier ~2980-3145 (nach Norden) gegen die
  Nordwand: Takt 155/Bild, nach den Klemmen 50/Bild, netto < 300 je Sekunde; gator_boss.log je 26 Bilder `KOPF-KLEMME an ... Ueberstand (0,-185)`
  und alle ~14 Bilder `PUTZ Stoss` (F7547..F7750 lueckenlos). Das ist das alte Festdruecken (Ziel und Kopf-Klemme widersprechen sich), nur an der
  Nordwand. Bild `p7_leon_haenger_F7545-F7896_nordwand_1fps.jpg`: Gator liegt mit dem Kopf in der Ecke, Leon steht dahinter, ~13 s keine Bewegung.
- **frei_elza F6979-F7158 (6,0 s):** Wurzel (-487,-22339) steht exakt still (Takt (0,0) je Bild), Gier pendelt 3285/3381, Kopf + Kanister zeigen
  nach Norden in die Plattform-Suedkante, Elza steht 4040 hinter ihm an der Suedwand (Blickfehler 168 Grad), `PUTZ Stoss` alle ~14 Bilder.
  Bild `p7_elza_haenger_F6979-F7158_ruecken_zum_spieler_1fps.jpg`.
- frei_elza F2078-F2269 / F904-F1043 (Nordwand, Figur NW-Ecke) und F3437-F3613 (Suedwand, Figur SO-Ecke): dasselbe Muster wie Leon.
- Karten `p7_karte_frei_leon.jpg`, `p7_karte_frei_elza.jpg` (rote Kreuze = Stillstand) gegen `p7_karte_VORHER_frei_*.jpg`.

Steht die Figur frei im Wasser (plat_leon), kommt er an und beisst; die Haenger entstehen, sobald die Figur an einer Wand/Ecke steht, die der
4200 lange Kopf mit dem Kanister nicht erreicht. Warum der G2-Haken dort nicht greift (gelesen, nicht repariert): im Leon-Haenger meldet
gator_boss.log `TICK ph=2 ... ziel=(-3876..-4172, -7954..-8605)` bei Wurzel (-3933..-4142, -9362..-10076) - das schon verschobene Steuerziel liegt
noch 1300-2000 noerdlich der Linie, an der die Kopf-Klemme die Wurzel haelt (`KOPF-KLEMME an ... Ueberstand (0,-185)` je Paddel-Zyklus). Ziel und
Klemme widersprechen sich also weiter. Der Riegel unit_r41_gatorfix_klemm prueft vier feste Lagen (Marke + West/Sued/Nord) mit Ueberstand 0 am
Ziel; im Spiel kommt der Gator schraeg an (Blickfehler ~50 Grad zur Figur in der Ecke, Kanister quer) - diesen Fall deckt er nicht.

**Plattform.** Die Figur kam in keinem Lauf auf die Plattform (kein GUARD ph=5 in gator_trag.log; Versuch plat_leon: Westkante, `A` -> kein
Klettern, climb.log kind 0). Den gemeldeten Fall "wartet am Rampen-Hop" konnte ich darum nicht im Fenster sehen. Gegen RE2 (EM23_OVL_0000.BIN,
info/re2leon/PSX.EXE, selbst disassembliert): **RE2 hat keinen Warte- oder Belagerungszustand.** Sub 1/Sub 2 bewegen den Koerper nur ueber den
Wurzelversatz der Clips (`FUN_8001a330 -> FUN_80015e7c -> FUN_800152c8`, Versatz @0x800152e4-334), Clip 0 = 5990 in 163 Bildern, Clip 1 =
5798 in 26 Bildern, beide vorwaerts; nur das Intro P0 wartet (`lh v0,468(s0) / beq` @0x80100854-68); mit dem Kanister (Stufe 7 @0x80101b58-8c)
schwimmt er ohne Zaehler weiter, bis zur Zonenlinie `slti -7322` @0x801009e0 -> LUNGE -> Sub 7 (67 Bilder, `sltiu 0x43` @0x8010128c) -> FRESSEN.
Eine Hoehenabfrage des Spielers gibt es nur im Biss (@0x80103e1c). Das Warten ist also nicht RE2. Die Belagerung ist Nutzer-Design
(enemy_ai_boss_gator.c:1675 "solange Leon OBEN steht -> BELAGERN"), und dasselbe Nutzer-Design verlangt ausdruecklich
"PATROUILLE statt Erstarren (Nutzer "bleibt haengen": stand Leon oben ausser Reichweite ... er stand regungslos)" (enemy_ai_boss_gator.c:2083).
Ein Gator, der mit Kanister am Rampen-Hop steht, ist nach dem Mass des Nutzers ein Rest des Haengens. Nebenbefund des RE2-Agenten: Spur-B-Belege
@0x80100ca4-b0 / @0x80100cb4-c4 liegen in der toten Funktion 0x80100c54 (kein Tabelleneintrag, kein jal) - ohne Folge fuer G2.

## 4. Punkt 8 - Aufnahme nur im Vorbeikommen

- Kein Anschwimmen: frei_leon/frei_elza 0 Zeilen `ANSCHWIMMEN|HALS zum Kanister`; Vorher-exe `F50 ANSCHWIMMEN an ... Abstand 10267` / `F43 ...`,
  Aufnahme F154/F143.
- Spur: Elza `KANISTER Stufe 1 ... F520` (17 s). Leon erst `F3650` (122 s): bis dahin hielt der Zufallsweg die Figur im Sued-/Ostbecken, kleinster
  Abstand Gator-Kanister 13900-17700; aufgenommen erst, als die Figur selbst bis 656 an den Kanister kam (F3600 Figur (-5419,-10084)).
- Folge: Die Aufnahme haengt allein am Weg der Figur. Bleibt der Spieler weg vom Kanister (Leiter, Suedbecken), nimmt der Gator ihn nie; Beschuss
  toetet nicht (GB_HP = RE2-Sockel, "Beschuss toetet nicht"), der Kampf stockt, bis der Spieler ihn zum Kanister fuehrt. Das ist genau der Wortlaut
  des Nutzers, aber ohne Rueckfall. **Entscheidung E1** (kein Mangel der Spur).

## 5. Gegenprobe (Riegel ohne den Bau)

Riegel-Skript `test_r41_gatorfix.cmake` mit der Vorher-exe und dem Auswerter der Spur (`<SP>/gp/`):
- `gang_montage`: rc=1, `ganglog montage: ... 8 Zombies, 4 gut, kleinster Weg -616` -> rot.
- `gang_szene`: rc=1, `ganglog szene: 0 Bilder an der Leiter, 0 Zombies`, letzter Schuss F186 -> rot.
Pruefhaken der Unit-Gegenproben gelesen: `re15_gr2_kk_ziel_alt_setzen` kehrt am Funktionsanfang zurueck (= Ziel wie vorher),
`re15_k2080_anschwimmen_alt_setzen` schaltet `anschwimm_lage` und das RE2-Fenster (`len/ab` unveraendert) zurueck - beide geben den alten Weg
wieder. Die Unit-Riegel selbst konnte ich nicht gegen die alte Engine bauen (sie rufen die neuen Haken).

## 6. Raumkette Leon und Elza

- Szene -> Kampf -> Ausloeser -> Marvin/John-Szene -> Rueckkehr -> Aufnahme -> R1-Tod -> Schluessel-Einstellung (kill_leon, wieder_leon,
  wieder_elza; `RE15_K2080_RUECK_KANISTER=1`): `Kanister verbraucht (R1-Tod) -> Bit 116 = 0`, `Water Key (Item 0x34 ...) bei der Leiche
  (1400,-1764,-8500)`, Schluessel-Einstellung Cut 5 F235-F327.
- Wiedereintritt (wieder_leon/wieder_elza): Leiter 2090 -> 2080 (`DOOR FIRE slot=0`), Leiter zurueck (`DOOR FIRE slot=1 ... spawn=(-8200,0,-25550)`):
  im neuen 2090/2091-Besuch kein Gator (kein Slot 35 in 328 Bildern, keine `gator da`-Zeile), keine Szene, Schluessel liegt weiter, Musik
  `MAIN2E [unveraendert, laeuft durch]`, Tor (Slot 1) gesperrt bis zum Schluessel. Elza 6 Wiedereintritte, in keinem ein Gator (0 Slot-35-Eintraege ab dem ersten Wiedereintritt).
- Aufheben des Water Key: integration_r41_alligator2_schluessel / _elza / _schluessel_west_elza (Abschnitt 7).

## 7. Nebenwirkungen - Riegel der Alligator-Spuren

PowerShell, `C:\msys64\mingw64\bin` vorn im PATH, SDL_ASSERT=always_ignore:
`ctest --test-dir re15_port/build -R "r41_gatorszene|r41_kanister2080|r41_alligator2|r41_re2gator|r41_gatorfix" -j3 --timeout 1500 --output-on-failure`
(Log `<SP>/ctest_alle.log`):

**`100% tests passed, 0 tests failed out of 104`**, `Total Test time (real) = 1351.56 sec` (Start 23:29:20, gcc aus C:/msys64/mingw64/bin; die ersten Minuten neben eigenen Fensterlaeufen).
Darin alle Riegel der Spuren F (gatorszene), G (kanister2080, auch `integration_r41_kanister2080_kampf` Passed 1086.96 s), P (alligator2, auch
`_schluessel`, `_schluessel_elza`, `_schluessel_west_elza` = Water Key aufgehoben), B (re2gator) und G2 (gatorfix, gang_* 4 x Passed). Die
Riegel liefen NACH dem letzten Engine-Commit e139e9b6e; der Bau hatte nach diesem Commit nur den kampf-Riegel neu gefahren.

## 8. Maengel

- **M1 (Punkt 7, blockierend):** Mit dem Kanister im Maul haengt der Gator im freien Spiel weiter, sobald die Figur an einer Wand oder in einer Ecke
  steht: Leon 11,7 s Wand-Reiben an der Nordwand (Kopf-Klemme + PUTZ alle 14 Bilder), Elza 6,0 s regungslos mit dem Ruecken zur Spielerin vor der
  Plattform-Suedkante, 6,4 s / 5,9 s an Nord- und Suedwand. Fern-Stillstand 27-28 % der Trag-Zeit (vorher 7-13 % mit derselben Eingabe). Der Fix
  greift an der Marken-Lage, nicht im Raum.
- **M2 (Punkt 2):** Fang und Verschwinden nicht im Bild: die Schnauze liegt im ganzen Wurf hinter dem oberen Kino-Balken, die Leiche verschwindet
  auf halber Halshoehe vor dem Hals. Wurf/Fang/Fressen ist so fuer den Spieler nicht lesbar.
- **M3 (Punkt 7, Plattform):** Der vom Bau gemeldete Fall "wartet mit Kanister am Rampen-Hop" ist weder RE2 (kein Wartezustand) noch das Nutzer-
  Design der Belagerung ("PATROUILLE statt Erstarren", enemy_ai_boss_gator.c:2083); nicht im Fenster nachgewiesen, weil die Figur die Plattform
  im Messlauf nicht erreichte.

## 9. Hinweise / Entscheidungen

- H1 Biss-Moment: 0,3-0,5 s Leiche unter dem offenen Unterkiefer bzw. durch die Schnauze (Leon und Elza), danach sauber.
- H2 Marvin-Szene: Zombie 1 stirbt nach 19 Bildern (104 Einheiten Weg).
- E1 Punkt 8: Aufnahme nur im Vorbeikommen ohne Rueckfall - der Spieler kann den Kampf unbegrenzt stocken lassen (Leon-Lauf 122 s bis zur
  Aufnahme). Entscheidung Nutzer/Orchestrator.
- H3 Die Laeufe mit `RE15_DEBUG_JUMP=2090` zeigen die Zombie-Leiche der (uebersprungenen) Ankunftsszene im Becken - Folge des Sprungs, kein G2-Befund.

## 10. Nicht geprueft

PSX-Bau, Klettern auf die Plattform im echten Fenster (kein Weg gefunden), Hoeren (nur Audio-Log), volle Suite.
