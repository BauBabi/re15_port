# INT Gator-Nutzerbefunde 2026-11-09 (Runde 41, vor v0.8.30)

Baum `.claude/worktrees/r41_integration`, Zweig `r41/integration`, Basis `0b86e36c6` (= master).
Untersuchungen (scratchpad `gator_nb/{wand,kanister,schluessel}/BERICHT.md`) -> Umsetzung hier.

Nutzer 2026-11-09 (woertlich): "Den Aligator fight habe ich jetzt auch einmal getestet. Ist cool! Es gibt nur 3 winzige
kleinigkeiten die mir aufgefallen sind:
1. Wenn ich direkt an der Kante hochklettere auf die Platform und dann vom Aligator erwischt werden, ist es möglich das er mich
   in die Wand schiebt.
2. Ich musste diverse runden mit dem Aligator drehen, bis er letzendlich den Behälter aufnahm. Eigentlich sollte er den Behälter
   aufnehmen, sobald die Position erreicht ist, wo der Behälter liegt. Wenn das so nicht so einfach ist, lege den Behälter
   zentraler in die Mitte, damit er ihn auf jeden Fall aufnehmen muss, sobald der Spieler den Aligator dort hingelockt hat.
3. Ich bin ausversehen nach dem Tot des Aligators die Leiter hoch und wieder runter. Ich konnte den Schlüssel sehen, aber
   aufnehmen konnte ich ihn nicht mehr."

Der Kampf ist gelobt ("Ist cool!"). Gang, Tempo, Angriffe, Biss, Trefferreaktion und Szenen bleiben, wo der Eingriff nicht
greift, bitgleich (s. je Befund "Wirkt nur, wenn").

## Commits

| Commit | Befund | Kern |
|---|---|---|
| `8d165ee58` | 1 Wand-Schub | Alligator-Treffer beendet das Klettern (Wort-Store PL+4) |
| `80c7394ab` | 2 Kanister | Fenster auch im Biss (RE2) und im Zonen-Dash (Port-Wahl) |
| `6d7a9e121` | 3 Schluessel | Schluessel-Lage nur im Gebiet seiner Kamera (FUN_80014230) |
| `f8c71298a` | 3 Nachtrag | Rueckfall auf die Bildschwelle 10,4 (FG-E7); Riegel weg ohne Boss-Leck; Timeout kletterbiss |

## Befund 1 - Wand-Schub beim Hochklettern (reproduziert)

**Messung:** Nutzer-Sitzung gator_boss.log Z. 3895 `GTR F758 START art 4 sub 3 achse 0 gier 3072->0 ... figur (5506,0,-18568)`,
danach keine GTR-Bildzeile bis F798 (die Kletter-FSM hielt die Figur), `AUSSEN pos=(7854,-18568) ... mo=231`, Ende
`(11948,-1800,-18568)` hinter der Ostwand. Harness mit Kletter-Baenken: bitgleich dieselbe Endlage (Leon und Elza).

**Ursache:** `re15_gator_treffer_r41_einstieg` drehte die Figur und startete die RE2-Reaktion, liess aber den Kletter-Substate
(+0x05 = 0x800aca59) stehen. Der Dispatcher prueft das Klettern vor der Reaktion; in Kletterbild 0x1d ging der Schritt 0x47c
(@0x800381b0 `ori v0,zero,0x47c`, @0x800381bc `jal 0x800245d8`) samt y -1800 (@0x800381e4 `addiu v0,v0,-0x708`) laengs der
Trefferachse - in die Ostwand bzw. die Plattform.

**Original:** der Treffer ersetzt die Spieler-Routine.
- RE2 EM23_OVL_0000.BIN @0x801015b4 `05020224` addiu v0,zero,0x205 / @0x801015c4 `fcfb22ac` sw v0,-0x404(at) (Wort auf PL+4:
  +5 = 2, +6/+7 = 0); Tod-Zweig @0x80103938-48.
- RE1.5 PSX.EXE @0x80012ebc `040022a2` sb v0,4(s1) / @0x80012ed4 `050022a2` sb v0,5(s1) / @0x80012ee0 `060020a2` sb zero,6(s1).

**Fix:** `re15_climb_routine_ersetzt()` (climb_common.c) loescht den Spiegel von +5/+6/+7; `re15_gator_treffer_r41_einstieg`
ruft sie als Erstes (beide Ausgaenge). y, Band, Bodenzaehler, Standobjekt bleiben. Keine neue Konstante.
Substate 10 (offen im Untersuchungsbericht) per Code geklaert: der Biss trifft nur bei +0x93 == 0; Substate 10 setzt +0x93 in
Ph2 vor dem Fall (@0x80038414-1c) und loest es erst bei der Landung (@0x80038748-54) - ein Biss trifft nur oben vor dem Sprung
(Ph0/1) oder nach der Landung (Ph4-6); y und Band sind dort konsistent.

**Wirkt nur, wenn** ein Alligator-Treffer eine kletternde Figur trifft. Sichtbar anders: ein Biss am Kletteransatz wirft die
Figur jetzt ins Wasser zurueck, statt sie trotzdem hinaufzutragen (Original-Regel).

**Riegel** `unit_r41_gatorfix_kletterbiss(_elza)` (Harness MIT PL00/PL00W01-Baenken - ohne sie erreicht das Klettern Bild 0x1d
nie, darum sahen die bisherigen Riegel den Fehler nicht):

| Mass | neu | Gegenprobe 0b86e36c6 (Pruefhaken) |
|---|---|---|
| (a) Lage der Nutzer-Sitzung, Biss beim Klettern t7 | 0 Bilder ausserhalb Deck/Raum, 0 Kletterbilder nach dem Biss | Ende (11948,-1800,-18568) = Nutzer-Sitzung, 549 Bilder ausserhalb, 43 Kletterbilder |
| (b) eingespeist, Kletterbild 0..50 x x 2318/5506/6500 x Gier 3721/2048/1024/3072 | 0 von 612 in/hinter der Wand | 248 von 612 |

Leon und Elza je 0 Fehler (Laufzeit je ~3 min 20 s).

## Befund 2 - Kanister erst nach vielen Runden (reproduziert)

**Messung (15 exe-Laeufe des Untersuchers, Stand 0b86e36c6):** 27 Passagen am Kanister, 13 ohne Aufnahme: 4x Kanister im Fenster
waehrend des Zonen-Dash, 1x waehrend des Bisses, 7x seitlich vorbei, 1x laengs ausserhalb.

**Ursache:** Fenster nur in CHASE/LURK (enemy_ai_boss_gator.c -> gator_re2_r41.c gr2_kan_takt `if (!im_chase) return`); die
Aufschlagstelle (-6870,-12651) liegt 1849 hinter der Zonenlinie z -14500, wo der Zonen-Dash startet.

**Fix (nur Szenen-Kanister, `re15_gr2_kan_szene_liegt`):**
- Fenster auch in LUNGE - **RE2-belegt**: das Fenster im Sub-1/2-Pre (@0x80100ab8-b1c; @0x80100ae4 `67de4224` addiu -8601,
  @0x80100ae8 `8f01422c` sltiu 0x18f, @0x80100af4 `26020292` lbu +0x226) fragt den Kiefer-Lauf +0x224 nicht; der Biss T1 laeuft
  im selben Pre (@0x80100974 `bf04040c` jal 0x801012fc).
- Fenster auch in RE2_DASH - **PORT-WAHL**, Beleg Nutzer-Wortlaut "sobald die Position erreicht ist, wo der Behälter liegt"
  (RE2 Sub 3 0x80100d30-0x80100ee0 hat kein Fenster).
- Fenstertreffer im Biss/Dash -> CHASE, Clip 0 Bild 0 wie das LUNGE-Ende bzw. das Dash-Ende (Routine 0x101 @0x80100ee4, +0x225 = 0);
  `re15_gr2_dash_abbrechen` ohne yaw += 1024 (@0x80100eb0-c0). Danach die RE2-Stufen unveraendert (T1 bei Stufe 1..6 nicht
  geschaerft, @0x80101360-70). Fenster 8601/0x18f, Vorhalt 8100 (@0x801018a4-c0) unveraendert; keine neue Konstante.
- Pruefhaken `re15_gr2_kan_phasen_alt_setzen(1)` / Messhaken `RE15_G2_KAN_PHASEN_ALT=1`.

Ersatzweg "Behaelter zentraler" nicht noetig: die Liegestelle bleibt (FG-E3).

**Riegel** `integration_r41_gatorfix_kanister_vorbei(_elza)` (neu, `tests/integration/test_r41_gatorfix_kanister.cmake`,
cmake_minimum_required 3.16), Auswerter `test_r41_gatorfix kanpass` (Q12-Rechnung wie gr2_kan_vor): Aufnahme <= erstes
Fensterbild (LURK/CHASE/LUNGE/RE2_DASH) + 1.

| Lauf | Gegenprobe 0b86e36c6 (Laeufe des Untersuchers) | neu Leon | neu Elza |
|---|---|---|---|
| (a) pendel_innen | ROT: Aufnahme F884, Fensterbild F594 (290 zu spaet) | F595 / F594 (Dash) | F627 / F626 (Biss) |
| (b) stehen_ost | ROT: Aufnahme F1020, Fensterbild F252 (768 zu spaet) | F253 / F252 (Dash) | F251 / F250 (Dash) |
| lane_mitte / pendel_mitte / stehen_sued | ROT: 364 / 313 / 21 Bilder zu spaet | - | - |

Erster Elza-Lauf war mit einer Gleitkomma-Rechnung im Auswerter um 2 Bilder rot (Quer 1731 statt Engine >= 1733 an der Grenze);
Auswerter auf die Q12-Rechnung der Engine umgestellt, danach alle Laeufe auf ein Bild genau.

**Restfall (Nutzerregel 2026-11-07):** seitliche Passagen (7 von 27, quer 1968-2997) und Wege, die nie am Kanister vorbeikommen,
bleiben ohne Aufnahme.

## Befund 3 - Schluessel sichtbar, aber nicht aufzunehmen (teilweise reproduziert)

**Messung:** die Aufnahme-Zone geht beim Wiedereintritt NICHT verloren (exe-Laeufe Leon/Elza: `ITEM slot 15 ... -> FREI`,
`34:1`; 200/200 Drucke am Schluessel nach Leiter hoch/runter). Reproduziert ist das Symptom: mit der Leiche des Nutzers
(gator_boss.log `RE2 R1 ende (Leiche) pos=(-5485,-23074)`, Gier 800) lag der Schluessel in Cut 7 bei (-6600,-23400); Cut 7 ist
nur im Westkanal aktiv und zeigt ihn dort unter den Fuessen der Figur, am Schluessel laeuft Cut 0.

**Mechanik** FUN_80014230 (PSX.EXE): @0x80014254 `lbu v1,22(a0)` / @0x8001425c `bne v1,v0` (cam_from == Cut), @0x80014260
`addiu s1,a0,20` (Anker-Satz 0 nie getestet), @0x80014298 `jal 0x80014368`, @0x800142a8 `lbu a0,1(s0)` -> @0x800142ac
`jal 0x800142f4`.

**Fix (PORT-WAHL, Beleg Nutzer-Wortlaut), zwei Durchgaenge (f8c71298a):** Durchgang 1 sucht nur im Gebiet; erreicht dort keine Kamera die
Bildschwelle 10,4 (Nutzerentscheid FG-E7 Option (a)), sucht Durchgang 2 wie 0b86e36c6. Gemessen (unit_r41_alligator2_schluessel, Raster 3000):
82 von 208 Todeslagen im Gebiet, 126 Rueckfall, 0 unter 10,4. Nur-Gebiet (6d7a9e121) haette 126 von 208 unter 10,4 gelegt. Gebiet eines Cuts = von seinen Eintritts-Vierecken (cam_to == Cut) aus begehbar im
Wasser erreichbar, ohne ein eigenes Austritts-Viereck zu betreten (Raster GSZ_RASTER, key_begehbar, Punkttest FUN_80014368);
key_suche sucht nur noch dort. Stufen, 10,4-Bildpunkte-Schwelle, Rueckfall unveraendert. Pruefhaken
`re15_gatorszene_r41_key_gebiet_alt_setzen(1)`.

**Riegel** `unit_r41_gatorszene_tod_gebiet(_elza)`: Tod -> Einstellung -> Leiter hoch (2080) und runter (2090, Cut 0) -> zu Fuss
mit dem RVD-Wechsel des Ports zum Schluessel -> angezeigter Cut == Schluessel-Kamera, Aktionstaste nimmt ihn auf.

| Leiche | neu | Gegenprobe 0b86e36c6 |
|---|---|---|
| Nutzer (-5485,-23074) | Schluessel (-600,-26500) Cut 0, angezeigt Cut 0, 11,6 Bildpunkte, Aufnahme | Schluessel (-6600,-23400) Kamera 7, angezeigt Cut 0, 3,8 Bildpunkte - ROT |
| todesrolle (-5416,-22578) | (-700,-26500) Cut 0 = 0 (Durchgang 1) | - |
| r1 (3413,-25001) | Rueckfall: (1900,-23300) Kamera 2, am Schluessel Cut 1 (nur dokumentiert) | - |
| Lauf A (-5500,-8900) | Rueckfall: (-8400,-9200) Kamera 6, am Schluessel Cut 5 (nur dokumentiert) | - |

Pflicht im Riegel: Nutzer-Leiche im Durchgang 1 und Kamera-Gleichheit dort; Aufnahme per Aktionstaste in allen Lagen.

Bestehende unit_r41_gatorszene_tod / tod_explosion / schluessel / leiche gruen.

## Testlaeufe

Bau: `RE15_BUILD_DIR=.../r41_integration/re15_port/build bash re15_port/tools/local_build.sh configure|build` -> `=== LOCAL-BUILD-OK`.
Die Maschine lief unter Fremdlast (dieselbe alte Sonde sonde_wand.exe: 0,6 s -> 4,4 s).

1. `ctest -R "gator|kanister|alligator|2090|2080|re2gator" -j 2 --timeout 1800` (Stand 4d68d2549, vor f8c71298a), Summenzeile woertlich:
   `99% tests passed, 2 tests failed out of 150` - rot: `unit_r41_alligator2_schluessel` (126 von 208 unter 10,4) und `unit_r41_alligator2_weg`
   (Lage 4/5 -1 Bilder). Ursachen und Fix in f8c71298a (s. Befund 3 und Commit-Text): Rueckfall auf die Schwelle; weg = Boss-Zustands-Leck des Riegels
   (auch mit der Lagewahl 0b86e36c6 und Folge Lage 2 -> 4 rot), Messhaken re15_gator_boss_zustand_frisch. Danach einzeln: schluessel 0 Fehler, weg 5/5.
2. Nach f8c71298a die betroffenen exe-Riegel `ctest -R "integration_r41_(gatorszene_tod|alligator2|restpunkte_gator)" -j 2 --timeout 1800`:
   `100% tests passed, 0 tests failed out of 21`.
3. `ctest -j 6 --timeout 600 -R "^(unit_|prop_|probe_)" -E "^(unit_r38_releaseskript_|unit_r30_android_|unit_r35_android_|unit_r39_titelversion_cmake|unit_r39_titelversion_ablauf$)"`
   (Stand f8c71298a), Summenzeile woertlich: `99% tests passed, 5 tests failed out of 1104` - alle fuenf `(Timeout)` unter Last:
   unit_gator_sweep, unit_r37_masken36_ausschluss, unit_r41_alligator2_erreichbar, unit_r41_birkin_wand_g5, unit_r41_birkin_wand_griff.
   Einzeln nachgefahren (`-j 1`): `100% tests passed, 0 tests failed out of 5` (107 / 478 / 79 / 41 / 198 s).
4. Neue Riegel einzeln: kletterbiss / kletterbiss_elza je 0 Fehler (allein 3 min 16 s, unter Last 16 min 22 s -> TIMEOUT 1800);
   In der Gruppe (1.): kanister_vorbei 364 s / kanister_vorbei_elza 402 s Passed, kletterbiss 154 s / _elza 155 s Passed, tod_gebiet / _elza Passed;
   in der Unit-Suite (3., Stand f8c71298a): kletterbiss 1088 s / _elza 1091 s Passed, tod_gebiet / _elza Passed.
   Gegenproben: kletterbiss mit Pruefhaken rot (Ende (11948,-1800,-18568), 248/612); kanpass auf den 0b86e36c6-Laeufen rot (290 / 768 / 364 / 313 / 21 Bilder);
   tod_gebiet mit Pruefhaken rot (Cut 7 gegen 0).

## Offen

- **Befund 3, Zielkonflikt (Nutzerentscheid noetig):** "Schluessel im Gebiet seiner Kamera" gegen "mindestens 10,4 Bildpunkte" (FG-E7 Option (a)).
  Heute gewinnt die Schwelle: in 126 von 208 Raster-Todeslagen faellt die Wahl auf den Stand 0b86e36c6 zurueck, dort kann die Einstellung weiter eine
  Kamera zeigen, die am Schluessel nicht laeuft (Riegel-Lagen r1 und Lauf A). Gebiet zuerst wuerde dort Schluessel unter 10,4 legen.
- **Befund 3, Lage:** im Durchgang 1 liegt der Schluessel bis zur doppelten Naehe von der Leiche (Nutzer-Leiche: (-600,-26500), 5,9k entfernt an der
  Suedwand, Cut 0 11,6 Bildpunkte; vorher (-6600,-23400), 1,1k entfernt). Das haelt die bestehende Regel (Stufen bis doppelte Naehe), ist aber sichtbar
  weiter weg - im Spieltest abnehmen.
- **Befund 1:** ein Biss am Kletteransatz wirft die Figur jetzt ins Wasser zurueck (Original-Regel), sichtbare Verhaltensaenderung. Nebenbefund ausserhalb
  der Spur: auch der RE1.5-Schadenspfad re15_player_take_damage beendet ein Klettern im Port nicht (@0x80012ed4 wuerde es tun) - nicht angefasst.
- **Befund 1, exe-Riegel** `test_r41_gatorfix_kletterbiss.cmake` (Plan des Untersuchers) nicht gebaut; der Harness-Riegel faehrt den echten Engine-Weg
  mit den Kletter-Baenken und trifft die Nutzer-Sitzung bitgleich.
- **Befund 2:** Dash-Abbruch ohne yaw += 1024 - moegliche kurze Gier-/Kopfkante im Bild nicht per gdigrab abgenommen (Cut 2/3). Restfall seitliche
  Passagen (7 von 27) bleibt (Nutzerregel 2026-11-07).
- **Befund 3, exe-Riegel** `integration_r41_restpunkte_gator_wiederkehr` (Plan des Untersuchers) nicht gebaut; der Harness-Riegel faehrt Tod, Leiter,
  Rueckkehr und den RVD-Wechsel des Ports, die Zonenkamera wird dabei explizit eingeschaltet (cut_auto_enabled = 1 wie im freien Spiel).
- Kein Hauptbaum-/master-Merge, kein Paket (nicht Teil dieses Auftrags).

## Nachbesserung (Abnahme 0, 6b221f0d6: B1 und B2)

Nur die zwei blockierenden Befunde der Abnahme 0. Commits `16f7b402b` (B2) und `bf345ea49` (B1). Laufordner (nicht im Baum):
`<SP>/nb1` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/gator_nb/nb1`
(runs/s1_*, runs/s2_leon, runs/s2_elza, plan/, plan_elza/, scan/, ctest_unit.log, ctest_exe.log, mdis.py).

### B2 - Kanister-Fenster im Biss: der Biss laeuft zu Ende (`16f7b402b`)

RE2 EM23_OVL_0000.BIN, selbst disassembliert (mdis.py):
- Wurzel je Takt: @0x80100320 `f105040c` jal 0x801017c4 (T2, Kanister-Stufen), danach unbedingt @0x80100328 `e204040c` jal 0x80101388 (Kiefer-Maschine).
- Kiefer-Maschine: @0x80101398 `24020382` lb v1,0x224(s0), Sprungtafel @0x80100004 = 0x80101564, 0x801013c4, 0x801013d4, 0x80101418, 0x80101488,
  0x801014f8. Im ganzen Bereich 0x80101388-0x80101578 kein Zugriff auf +0x226. Stufe 5: @0x80101558 `240200a2` sb zero,0x224(s0), @0x8010155c `5e05040c`
  jal 0x80101578 (Treffer; dort @0x801015b4 `05020224` / @0x801015c4 `fcfb22ac`).
- +0x226 sperrt nur den NEUEN Biss (T1): @0x8010133c `24028280` lb v0,0x224(a0) / @0x80101344 bnez, @0x80101360 `26028390` lbu v1,0x226(a0) / @0x80101368 beqz /
  @0x80101370 bne v1,7 -> @0x8010137c `240282a0` sb v0,0x224(a0).

Fix: der Fenstertreffer in LUNGE oeffnet nur das Fenster, die Phase LUNGE laeuft unveraendert weiter (Steuern, Vortrieb, Trefferfenster, Ende). Die Kanister-Stufen 2..7
laufen in `gr2_kan_takt` ohnehin phasenunabhaengig. Der Dash-Abbruch (PORT-WAHL, Nutzer-Wortlaut) bleibt. Keine neue Konstante. Pruefhaken
`re15_gator_kan_biss_abbruch_alt_setzen(1)` = Stand 80c7394ab.

| Riegel | neu | Gegenprobe 80c7394ab |
|---|---|---|
| `unit_r41_gatorfix_kanbiss` / `_elza` (neu; Harness, echter Kampf, 3 Lagen, Kanister im LUNGE-Bild 1/3/5 vor die Wurzel) | 9/9 Fenstertreffer im Biss, Biss 44/44 Bilder gleich wie ohne Kanister, Treffer r7 = r7, danach Stufe 7 (r39-r43) | 9/9 abweichend (39-43 Bilder), kein Treffer - rot |
| `kanpass` (exe-Riegel `integration_r41_gatorfix_kanister_vorbei(_elza)`), neu: faellt die Aufnahme in einen Biss, muss er im Bild danach weiterlaufen | Elza a: `Aufnahme im Biss (af 9) -> Bild danach Phase 3 af 10: Biss laeuft weiter`, `BISSLAUF F617-F661 art 4 treffer 1` | alter Lauf lauf_8e143b27/a: `Phase 2 af 1: BISS ABGEBROCHEN` -> ROT; dort `BISSLAUF F617-F628 treffer 0` |

Die uebrigen Kanister-Laeufe des Riegels bleiben (Leon a F595/F594 Dash, b F253/F252 Dash; Elza b F251/F250 Dash).

### B1 - Schluessel immer im Gebiet seiner Kamera (`bf345ea49`)

Mechanik des angezeigten Cuts wie im Befund 3 belegt (FUN_80014230 @0x80014254 / @0x8001425c / @0x80014260 / @0x80014298 / @0x800142a8 / @0x800142ac).

Gemessen (Werkzeug `test_r41_alligator2 schluessel_scan 3000`, 208 Lagen, `<SP>/nb1/scan`):

| Variante | im Gebiet | Bild >= 10,4 | groesster Abstand zur Leiche | kleinstes Bild |
|---|---|---|---|---|
| Stand f8c71298a (Rueckfall ohne Gebiet) | 82 | 208 | 6036 | 10,5 |
| (b) im Gebiet weiten bis zur Schwelle | 208 | 188 (20 erreichen sie in KEINEM Gebiet) | **18084** (andere Raumseite) | 6,3 |
| **(a) im Gebiet, beste Lage bis zur doppelten Naehe (gewaehlt)** | **208** | 82 | **6036** | **6,3** (126 Lagen 6,3 bis 10,3) |

Gewaehlt (a): beide Nutzer-Vorgaben gelten - Befund 3 (Bild und Ort stimmen ueberein) und "soll der Schlüssel in der Nähe seine Leiche sein" (AUFTRAG.md, Spur F).
(b) verletzt die Naehe grob. (c) (Rueckfall-Lage, Einstellung auf den Cut am Schluessel) zeigt dort ein kleineres Bild als die beste Lage im Gebiet. Die Regel
"reicht das nicht, gilt die beste gefundene Lage" ist die bestehende FG-E7-Regel, jetzt nur im Gebiet. Keine neue Zahl. Messhaken `key_durchgang`: 0 = Schwelle
im Gebiet erreicht, 1 = im Gebiet darunter, 2 = ohne Gebiet (nur Pruefhaken). Pruefhaken `re15_gatorszene_r41_key_rueckfall_alt_setzen(1)` = Stand f8c71298a.

**Natuerlicher Weg** (Rezept der Abnahme 0, `schl.sh`): Kampf bis zum R1-Tod, Leiter hoch, zurueck, zu Fuss zum Schluessel, Leiter hoch und zurueck.

| Lauf | Leiche | Schluessel (Einstellung) | Cut auf dem Weg / am Schluessel | Aufnahme | nach 2. Leitergang |
|---|---|---|---|---|---|
| s1_leon / s1_elza | (-7328,-8628) | (-1100,-5900) Kamera 5, Durchgang 1, 4910 von der Leiche, Tiefe 3661 = 10,2 Bildpunkte | - | - | - |
| s1_leon_west / s1_elza_west | (-7038,-8628) | (-1100,-5900) Kamera 5, Durchgang 1, 4579 von der Leiche | - | - | - |
| s2_leon | (-7328,-8628) | (-1100,-5900) Kamera 5 | 0 -> 7 (F77) -> 6 (F95) -> 5 (F110); **cam=5** F110-F159 | F158 `ITEM slot 15 ... -> FREI`, `34:1` | keine Schluessel-Zeile, `34:1` |
| s2_elza | (-7328,-8628) | (-1100,-5900) Kamera 5 | 0 -> 7 -> 6 -> 5; **cam=5** bis F170 | F162 `-> FREI`, `34:1` | keine Schluessel-Zeile, `34:1` |

Vorher (Abnahme 0, Stand f8c71298a) bei denselben Leichen: Kamera 4 bzw. 6 in der Einstellung, am Schluessel cam=5. s2_elza brauchte einen eigenen Weg
(`<SP>/nb1/plan_elza/skript_elza.txt`: Gerade 0,5 statt 0,367 s). Elza laeuft langsamer und blieb sonst am Maschinenblock haengen. Das betrifft den Messweg,
nicht die Lage.

| Riegel | neu | Gegenprobe |
|---|---|---|
| `unit_r41_alligator2_schluessel` | 208 Lagen, ohne Gebiet 0, ohne Kamera 0; Durchgang 0: 82, nie unter 10,4; Abstand hoechstens 6036 (doppelte Naehe); Westkanal-Fall jetzt Durchgang 1 (Bild 9,4) | f8c71298a: 126 Lagen ohne Gebiet - rot |
| `unit_r41_gatorszene_tod_gebiet` / `_elza` | Kamera-Gleichheit jetzt in JEDER Lage Pflicht; neu die Leichen der Abnahme 0: abn0_s2 und abn0_s1_west -> Schluessel (-1100,-5900) Kamera 5, angezeigt Cut 5, Aufnahme ja; r1 und lauf_a ebenso gleich | f8c71298a: abn0_s2 und abn0_s1_west Kamera 4 / Cut 5 (2 von 2 rot); 0b86e36c6: Nutzer-Leiche Kamera 7 / Cut 0 rot |

### Testlaeufe der Nachbesserung

Bau `RE15_BUILD_DIR=.../r41_integration/re15_port/build bash re15_port/tools/local_build.sh build` -> `=== LOCAL-BUILD-OK (build)`.
1. `ctest -R "gator|kanister|alligator|2090|2080|re2gator" -E "^integration_" -j 4 --timeout 1800`: `100% tests passed, 0 tests failed out of 102` (367,84 s).
2. `ctest -R "integration_r41_(gatorszene|alligator2|restpunkte_gator|gatorfix_kanister)" -j 3 --timeout 2400`: `100% tests passed, 0 tests failed out of 27`
   (2113,03 s). Darin die exe-Schluessel-Riegel alligator2_schluessel(_elza, _west_elza): jetzt Schluessel (-1100,-5900) Kamera 5 Durchgang 1, Aufnahme
   F597/F597/F598. k3(_elza) und gatorszene_tod(_r1, _explosion)(_elza): Cut 0, Durchgang 0 (>= 10,4), bestanden wie zuvor.
3. Einzeln: kanbiss / kanbiss_elza je 0 Fehler; alligator2_schluessel 0 Fehler (139 s); tod_gebiet 0 Fehler.

### Offen nach der Nachbesserung

- **B1, Bildgroesse:** In 126 von 208 Raster-Lagen und in allen vier natuerlichen Laeufen dieses Rezepts liegt der Schluessel jetzt unter der Bildschwelle 10,4
  (Orchestrator-Entscheid FG-E7). Er liegt aber am Ort und im Bild seiner Kamera (natuerlich 10,2 Bildpunkte, kleinstes 6,3), bis zur doppelten Naehe von der
  Leiche (natuerlich 4579-4910, vorher 1021-1214). Will der Nutzer den Schluessel naeher an der Leiche, waere der naechste Weg die Lage der Stufe 0 im Gebiet
  (kleineres Bild). Das ist ein Nutzerentscheid.
- Restfall K1 (seitliche Kanister-Passagen) und H1/H3 der Abnahme 0 bleiben offen (nicht blockierend).

## Nachtrag K1/H3 (nach Abnahme 1, mit K2)

Offene Punkte aus Abnahme 1 (b29263046): K1 Kanister-Restfall, K2 Nutzer-Antwort zur Schluessel-Lage, H3 Dash-Abbruch. Commits `e5cd784dc` (K1),
`ae9e17c9e` (K2), `3061fe397` (H3). Laufordner (nicht im Baum): `<SP>/k1n` =
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/gator_nb/k1n`
(runs/, sicht/, weg_0b86/, weg_neu/, wurfscan_*.txt, vorbei.py, klassen.py, ctest_k12.log, ctest_gruppe.log).

### K1 - Behaelter in die Mitte (`e5cd784dc`)

Nutzer 2026-11-09 Punkt 2 (woertlich): "Wenn das so nicht so einfach ist, lege den Behälter zentraler in die Mitte, damit er ihn auf jeden Fall aufnehmen
muss, sobald der Spieler den Aligator dort hingelockt hat." PORT-WAHL, Beleg ist dieser Wortlaut.

**Messung Kanalmitte (Kollision):** ROOM2090.RDT SCA @0x6DC, Westwand Kasten [0] @0x6F4 x -10900..-8900 (Innenkante -8900), Plattform-Block
Kasten [4] @0x724 x -1700..1850 z -20000..-12450. Kanalmitte x = -5300 (Werkzeug `test_r41_kanister2080 wurfscan`, liest die SCA selbst).

**Lage:** Der Kanister liegt, wo er aufschlaegt (FG-E3). Die Wurfrichtung dreht nur die RE2-Bahn (v (25,20,10) / a (5,20,0) @0x088E-08A2). Weil die
gedrehten Vektoren ganze Zahlen sind, liegen die Landestellen auf einem Gitter (Nennbahn, `<SP>/k1n/wurfscan_nord2.txt`, `wurfscan_sued2.txt`):

| Richtung | Gier vorher -> jetzt | Nennlandung vorher -> jetzt | Abstand zur Kanalmitte | Fall-Bilder in Cut 12 |
|---|---|---|---|---|
| Nord | 3072 -> 3342 (Bereich 3341..3344) | (-6808,-12658) -> (-5022,-13323) | 1508 -> 278 (naechste Gitterstelle; Nachbar 3320..3340 = (-5725), 425) | 13 -> 12 von 39 |
| Sued | 1024 -> 888 (Bereich 886..891) | (-6048,-21588) -> (-5193,-21512) | 107 | 18 -> 18 von 39 |

Nord liegt 873 im Westkanal (Plattform-Nordkante z -12450), Sued an der Suedmuendung des Kanals. Laufzeit-Landung (Haende von Marvin/John):
Leon (-5084,-13316), Elza (-5056,-13306). Fall/Wurf, Kamera und Szenen bleiben dieselben Ablaeufe (Cut 12 von unten, Cut 7 in 2080); Spur P
(Kanister im Maul) und Spur kanister2080 bleiben unveraendert gruen (Gruppe unten: tragen, kanister2080*, re2gator). Aufnahmefenster
(8601/0x18f @0x80100ae4-e8), Vorhalt, Stufen und die Nord/Sued-Wahl sind unveraendert. Pruefhaken `re15_k2080_wurf_alt_setzen(1)`.

**Laeufe (exe, `<SP>/k1n/runs`, Rezepte der Abnahme 1):** Vorbeischwimmen = die Schnauze (Wurzel + 5400) zieht mit |quer| < 3000 am liegenden
Kanister vorbei, vor der Aufnahme (Werkzeug `vorbei.py`, im Riegel `test_r41_gatorfix kanvorbei`).

| Lauf | Aufnahme | Vorbeischwimmen davor |
|---|---|---|
| pendel_west / pendel_mitte / pendel_innen (Leon) | F809 / F131 / F553 | 0 / 0 / 0 |
| elza_pendel_west / _mitte / _innen | F484 / F131 / F608 | 0 / 0 / 0 |
| lane_mitte / elza_lane_mitte | F131 / F131 | 0 / 0 |
| Gegenprobe alte Lage (Messhaken RE15_K2080_WURF_GIER=3072): pendel_mitte, k_pendel_mitte, k_lane_mitte, k_elza_pendel_mitte | F180 / F180 / F196 / F174 | je 1 bei F141 (quer 2102..2126) - **rot** |

Der K1-Fall der Abnahme 1 (k_alt_pendel_west F803-F805 "quer 1755") war beim Nachmessen kein Vorbeischwimmen: dort lag der Kanister 6409..6580
voraus (laengs ausserhalb des Fensters), der Alligator drehte zum Kanister ein und nahm ihn F812 auf. Neu: pendel_west nimmt ihn F809 auf.

**Riegel:** `integration_r41_gatorfix_kanister_mitte(_elza)` (neu): Pendel West/Mitte/innen, Aufnahme und 0 Vorbeischwimmen; Gegenprobe (Leon)
pendel_mitte mit der alten Lage muss rot sein - gemessen `Vorbeischwimmen davor 1 ... F141 Phase 2 laengs 5161 quer 2109 -> ROT`.
`unit_r41_kanister2080_landung`: Landung = mittigste Gitterstelle (Gier +-400), Nord im Westkanal, Gegenprobe alte Gier 1508 von der Mitte.
`integration_r41_gatorfix_kanister_vorbei(_elza)`: Lauf b steht jetzt 2070 oestlich der neuen Liegestelle (-2952,-13323).

### K2 - Schluessel naeher an der Leiche (`ae9e17c9e`)

Nutzer 2026-11-09 (woertlich): "na er soll näher bei der Leiche liegen, immerhin hat der Aligator ja den Schlüssel samt Zombie gefressen." PORT-WAHL.

**Regel:** Von allen Punkten, die die bestehenden Bedingungen erfuellen (Wasserquadrat, fuer die Figur begehbar = aufnehmbar, im Gebiet UND im Bild der
Zonenkamera nach FUN_80014230, nicht von Leiche/Figur verdeckt, Naehe-Ring bis zur doppelten Naehe), gilt jetzt der dem naechsten Knochen der Leiche
naechste (vorher: das groesste Bild bis zur Bildschwelle). Die Einstellung zeigt weiter die Kamera, in deren Gebiet er liegt (Befund 3). Die
Bildschwelle FG-E7 entscheidet nichts mehr. Bildgroesse und Durchgang werden nur gemessen. Keine neue Zahl. Pruefhaken
`re15_gatorszene_r41_key_naehe_alt_setzen(1)` = Stand b29263046.

| Messung | jetzt | Stand b29263046 |
|---|---|---|
| Raster 3000, 208 Todeslagen: Abstand zum naechsten Knochen max / Mittel | **1348 / 823** | 6036 / 3808 |
| je Lage | 208 naeher, 0 weiter | - |
| im Gebiet seiner Kamera / ohne Kamera | 208 / 0 | 208 / 0 |
| Bildgroesse (Quadrat) min / Mittel; ueber 10,4 | 2,9 / 4,9 Bildpunkte; 3 von 208 | 6,3 / 9,8; 82 von 208 |
| Nutzer-Leiche (-5485,-23074) | (-2900,-26400) Cut 0, 786, 7,0 Bildpunkte am Schluessel | 3061 |
| natuerliche Kampfenden Abnahme 0 (-7328,-8628) / (-7038,-8628) | (-5400,-5900) / (-5000,-6000) Cut 5, 735 / 746, 5,0 / 5,2 Bildpunkte | 4910 / 4579 |
| todesrolle / r1 / lauf_a | 819 / 812 / 812 (Cut 0 / 1 / 5; 5,9 / 4,7 / 5,1 Bildpunkte) | 3788 / 2442 / 4283 |

In allen sechs Leichen des Riegels tod_gebiet (Leon und Elza): Tod -> Einstellung -> Leiter hoch (2080) und runter (2090) -> zu Fuss mit dem
RVD-Wechsel zum Schluessel: angezeigter Cut = Kamera der Einstellung, Aktionstaste nimmt ihn auf. Vor bf345ea49 lag er natuerlich 1021-1214 vom
naechsten Knochen; jetzt 735-819 in diesen Lagen, im Raster hoechstens 1348 (dort liegt kein naeherer zulaessiger Punkt). **Bildgroesse:** der
Schluessel ist jetzt klein (Mittel 4,9, kleinstes 2,9 Bildpunkte; an der Nutzer-Leiche 7,0) - nur gemessen, wie beauftragt.

**Riegel:** `unit_r41_alligator2_schluessel` (je Lage nie weiter als b29263046, jede Lage im Gebiet; Gegenprobe rot), `unit_r41_gatorszene_tod_gebiet(_elza)`
(<= 1214, Kamera-Gleichheit, Aufnahme nach dem Leitergang; Gegenproben b29263046, f8c71298a, 0b86e36c6 rot). Die exe-Riegel `test_r41_alligator2.cmake`
(k3) und `test_r41_gatorszene.cmake` (tod*) lesen beide Log-Formen ("an der Leiche" / "groesstes Bild"); die Schwelle 10,4 und der bilddiff-Kasten
10 x 9 sind dort nur noch Messung (Sichtbarkeit in jedem Bild der Einstellung und Puls bleiben Pflicht). `unit_r41_alligator2_weg`: 5 von 5 Lagen
zu Fuss aufgehoben. Das Wiedererscheinen nach dem Leitergang (genau einmal) haengt nicht an der Lagewahl; geaendert ist nur die Lage.

### H3 - Dash-Abbruch (`3061fe397`)

RE2 EM23_OVL_0000.BIN Sub 3 (0x80100d30-0x80100ee8, selbst disassembliert mit Capstone, md5 0293e099bbeae989776264eb14c81bc9):
- `80100ea8 0f000012 beqz s0,0x80100ee8` (s0 = Clip-3-Endflag von FUN_8001a330), `80100eb0 76002296 lhu v0,0x76(s1)`, `80100eb8 250220a2 sb zero,0x225(s1)`,
  `80100ebc 00044224 addiu v0,v0,0x400`, `80100ec0 760022a6 sh v0,0x76(s1)`, `80100ecc 01010224 addiu v0,zero,0x101` / `80100ee4 040022ae sw v0,4(s1)`.
- Sub 3 hat keinen anderen Ausgang (Phase +6: 0 -> Clip 2 Blend 0x10 @0x80100dac-db4, 1 -> Clip 3 @0x80100e20-e28; kein Fenster, kein Abbruch).
- **Ergebnis:** yaw += 1024 gehoert nur zum normalen Ende. Der Abbruch (PORT-WAHL Punkt 2) bleibt ohne Drehung - nicht geaendert.

**gdigrab des eigenen Fensters** (hwnd der eigenen exe-Kopie, 30 Bilder/s; Abbruch der alten Lage im Dash, Clip 2 Bild 39 -> Clip 0, F253, Rueckschnitt
Cut 6 -> Zonenkamera 5): `<SP>/k1n/sicht/dash_abbruch_vergleich.png` (oben vorher, unten nachher), Einzelbilder `sicht/dash_alt_cut6`, `sicht/dash_neu_cut6`.
Die Blickrichtung bleibt; die POSE sprang in einem Bild vom Anlauf (Kopf hoch, Maul offen) ins flache Schwimmen (mittlerer Bildunterschied 2,1 gegen
sonst 0,6-1,1). Eine Drehung um 1024 haette den Koerper zusaetzlich um 90 Grad gedreht.

**Fix (RE2-belegt):** RE2 betritt nach dem Dash-Ende Sub 1 (Routine 0x101), Sub 1 setzt Clip 0 mit Blend 0x20: `80100c38 20000624 addiu a2,zero,0x20`,
`80100c3c cc68000c jal 0x8001a330`. Der Abbruch setzt jetzt Clip 0 mit Blend 32 (anim_frac 32, Rate 4096/33, Form der Lauerpause). Nachher: der Kopf
senkt sich ueber mehrere Bilder (Bildunterschied 1,3-1,5 je Bild, kein Sprung). gator_trag.log vorher/nachher bitgleich (nur die Darstellung aendert
sich). Messhaken `RE15_G2_DASH_BLEND_ALT=1`. Das normale Dash-Ende (Clip 3 -> Clip 0) bleibt unveraendert (gelobter Kampf, s. Offen).

**Riegel alligator2_weg gegen 0b86e36c6 nachgefahren** (Messhaken des Riegels R41_WEG_LECK=1 = ohne frischen Boss-Zustand, R41_WEG_LAGEWAHL=0b86 =
Lagewahl 0b86e36c6, R41_WEG_FOLGE):

| Lauf | Ergebnis |
|---|---|
| master-exe 0b86e36c6 (`re15_port/build/tests/unit/test_r41_alligator2.exe`, md5 d77f2ac915a9b395edd7a41dd8836bdd), Folge 12345 | 5 von 5 |
| Baum, Lagewahl 0b86e36c6, Leck, Folge 12345 | 5 von 5 (dieselben Schluessel wie master) |
| Baum, Lagewahl 0b86e36c6, **Leck, Folge 2 -> 4** | **1 von 2 - rot** (Lage 4: -1 Bilder) |
| Baum, Lagewahl 0b86e36c6, frisch, Folge 2 -> 4 | 2 von 2 |
| Baum (K2), Leck, Folge 12345 | 3 von 5 - rot (Lagen 4/5) |
| Baum (K2), frisch, Folge 12345 (ctest) | 5 von 5 |

Die Behauptung aus f8c71298a stimmt: auch mit der Lagewahl von 0b86e36c6 ist der Riegel ohne frischen Boss-Zustand bei der Folge 2 -> 4 rot. Die
Riegelaenderung behebt ein Harness-Leck, sie verdeckt keinen Fehler der Lagewahl.

### Testlaeufe Nachtrag

Bau `RE15_BUILD_DIR=.../r41_integration/re15_port/build bash re15_port/tools/local_build.sh configure|build` -> `=== LOCAL-BUILD-OK`. Unter Fremdlast (Spur form).
1. Teilmenge K1/K2 (Stand vor H3): `ctest -R "^(unit_r41_gatorszene_tod_gebiet(_elza)?|unit_r41_alligator2_weg|unit_r41_kanister2080_.*|integration_r41_gatorfix_kanister_(mitte|vorbei)(_elza)?)$" -j 4`:
   `100% tests passed, 0 tests failed out of 27`. Dazu einzeln unit_r41_alligator2_schluessel Passed (184 s).
2. Gator-Riegelgruppe nach allen drei Commits: `ctest -R "gator|kanister|alligator|2090|2080|re2gator" -j 4 --timeout 2400` (154 Riegel) lief nur bis 17 von 154,
   dann endete der Fixer am Wochenlimit (kein Testfehler): 16 Passed, 1 Failed. Rot war `integration_r41_kanister2080_kampf` (268,28 s): der Riegel
   erwartete noch die alten Wurfrichtungen 3072/1024. In `05c8851e6` an 3342/888 angepasst (nur der Riegel, kein Spielcode) und einzeln nachgefahren:
   `1/1 Test #1273: integration_r41_kanister2080_kampf ... Passed 1151.66 sec`. Der vollstaendige Gruppenlauf nach dem letzten Commit steht noch aus;
   er ist Aufgabe der unabhaengigen Abnahme 2.

### Offen nach dem Nachtrag

- **K1 Geometrie:** Das Fenster ist 2 x 1733 = 3466 breit, die erreichbare Wurzelbahn im Kanal 4268 (Klemme 1466 an Westwand und Plattform). Schwimmt der
  Alligator dicht an einer Wand ganz geradeaus durch den Kanal, liegt der Kanister in der Mitte 2134 neben ihm. In den 8 Laeufen kam das nicht vor
  (0 Vorbeischwimmen); ausgeschlossen ist es geometrisch nicht. Das Fenster selbst bleibt RE2 (Auftrag).
- **K2 Bildgroesse:** der Schluessel ist jetzt meist 3-7 Bildpunkte gross (vorher bis 10+). Im Spieltest abnehmen, ob er so noch gut zu sehen ist.
- **H3:** Auch das normale Dash-Ende wechselt im Port ohne Blend 32 auf Clip 0 (RE2 Sub 1 @0x80100c38 blendet). Nicht geaendert (gelobter Kampf);
  im gdigrab nicht vermessen.
- Der Harness-Teil K von `unit_r41_gatorfix_nb4` legt den Kanister weiter an die alte Liegestelle (-6870,-12651) als feste Testlage (Mechanik-Riegel);
  nicht umgestellt.
- Kein exe-Lauf der ganzen natuerlichen Kette Tod -> Leiter -> Fussweg -> Aufnahme -> Leiter mit der neuen Lage; die Kette ist im Harness-Riegel
  tod_gebiet (6 Leichen, Leon/Elza) und die Aufnahme am echten Weg im exe-Riegel alligator2_schluessel(_elza, _west_elza) abgedeckt.
- Kein master-Merge, kein Paket.
