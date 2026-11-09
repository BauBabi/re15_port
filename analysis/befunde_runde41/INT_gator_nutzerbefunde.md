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

**Fix (PORT-WAHL, Beleg Nutzer-Wortlaut):** Gebiet eines Cuts = von seinen Eintritts-Vierecken (cam_to == Cut) aus begehbar im
Wasser erreichbar, ohne ein eigenes Austritts-Viereck zu betreten (Raster GSZ_RASTER, key_begehbar, Punkttest FUN_80014368);
key_suche sucht nur noch dort. Stufen, 10,4-Bildpunkte-Schwelle, Rueckfall unveraendert. Pruefhaken
`re15_gatorszene_r41_key_gebiet_alt_setzen(1)`.

**Riegel** `unit_r41_gatorszene_tod_gebiet(_elza)`: Tod -> Einstellung -> Leiter hoch (2080) und runter (2090, Cut 0) -> zu Fuss
mit dem RVD-Wechsel des Ports zum Schluessel -> angezeigter Cut == Schluessel-Kamera, Aktionstaste nimmt ihn auf.

| Leiche | neu | Gegenprobe 0b86e36c6 |
|---|---|---|
| Nutzer (-5485,-23074) | Schluessel (-600,-26500) Cut 0, angezeigt Cut 0, 11,6 Bildpunkte, Aufnahme | Schluessel (-6600,-23400) Kamera 7, angezeigt Cut 0, 3,8 Bildpunkte - ROT |
| todesrolle (-5416,-22578) | (-700,-26500) Cut 0 = 0 | - |
| r1 (3413,-25001) | (-600,-21900) Cut 0 = 0 | - |
| Lauf A (-5500,-8900) | (-1100,-5900) Cut 5 = 5 (Weg 0 -> 7 -> 6 -> 5) | - |

Bestehende unit_r41_gatorszene_tod / tod_explosion / schluessel / leiche gruen.

## Testlaeufe

Stand des Zwischen-Commits: Riegel der drei Befunde einzeln gruen (s. o.); ctest-Gruppe `gator|kanister|alligator|2090|2080|re2gator` (-j 2, 150 Tests, davon 50 exe-Laeufe) laeuft, bisher 5/5 gruen. Endstand folgt.

## Offen

(folgt mit dem Endstand)
