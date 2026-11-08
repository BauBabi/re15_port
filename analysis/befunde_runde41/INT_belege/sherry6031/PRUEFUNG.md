# Pruefung Fix "gestreckte Sherry" ROOM6031 / Annette ROOM6030 (unabhaengiger Pruefer)

Geprueft: Zweig r41/integration, Kopf f6204ab0a (Fix 2c1c0eb1a, Belege f6204ab0a) gegen b27d96891.
Nur gelesen und gemessen; einzige Aenderung im Baum ist diese Datei.
Arbeitsdaten des Pruefers: Scratchpad `p6031/` (pruef.py, lauf.sh, gegen/, laeufe/, ctest_unit.txt, ctest_story.txt).

**Ergebnis: bestanden.** Die Ursache ist nachvollzogen, der Fix ist eng, Sherry und Annette sind im Spiel heil und
bewegt, die Riegel greifen (die Gegenprobe ist rot), und beide Testsaetze liefern eine Summenzeile mit 100 %.

## 1. Diff b27d96891..HEAD -- re15_port

Geaendert sind nur `engine/src/enemy_common.c` (+25/-4) und neu `tests/unit/test_r41_npcglieder.c` und
`tests/unit/probes/r41_npcglieder.cmake`. Keine weitere Engine- oder Plattformdatei ist betroffen.

Inhalt von `rbj_resolve_slot`:
- Neue Felder `type` und `bank` im Cache-Eintrag. Ein Treffer im Slot gilt nur, wenn Typ und Bank
  (`re15_enemy_find(type)`) gleich sind. Sonst wird der Eintrag freigegeben und auf dem bisherigen Weg neu gebunden:
  Binder FUN_8001b3f8, Alias, Elliot-Ausnahme und die Regel "Slot ohne Record belegt keinen Platz" bleiben unveraendert.
- Ohne Typwechsel verhaelt sich die Funktion wie vorher: Sie liefert denselben Eintrag, ohne neues Parsen und ohne neue
  Messzeile.
- Die Pruefung `slot <= 0 || slot >= RE15_ACTOR_MAX` steht jetzt vor der Cache-Suche. Das hat keine Auswirkung, weil
  der Cache nur Slots > 0 aufnimmt.
- Ein Raumwechsel leert den Cache weiterhin ueber `re15_rbj_bind_room` (Z. 85). Die Wege
  `re15_dining_r39_rbj_anim/_skel` laufen vor dem Cache und sind unberuehrt.
- Nebeneffekt nur bei einem echten Typ- oder Bankwechsel im selben Slot und Raum. Dann wird mit der Bank des neuen
  Typs gebunden, und genau das verlangt @0x800224a8. Die volle Unit-Suite (1100 Tests) ist gruen, siehe Abschnitt 5.

## 2. Ursache selbst nachgerechnet (eigener Parser, nicht der Port)

`pruef.py` liest die TOC aus der EXE (CDEMD1 @0x80073178, CDEMD0 @0x80072f38, 8 B je Typ ab 0x10), schneidet das EMD aus
dem EMS-Archiv (`re15_port/shared_assets/PSX/EMD`), liest dir[2] (EMR-Kopf, relpos +8, Armature an o_arm) und rechnet
die Ruhelagen in Weltkoordinaten.

Instruktionen. Die Bytes sind in `info/Re1.5/PSX.EXE` und in `Version_40%/PSX.EXE` gleich:

| Adresse | Wort | Bedeutung |
|---|---|---|
| @0x800224a8 | ae420084 | `sw v0,0x84(s2)`: dir[2] des neuen Modells |
| @0x80042328 | 0c0088c0 | `jal 0x80022300` (Sce_em_set) |
| @0x80050d40 | 8c440180 | `lw a0,0x180(v0)` |
| @0x80050d48 | 8c450184 | `lw a1,0x184(v0)` |

Alle vier Modelle haben 15 Knochen, EMR-Kopf (o_arm 100, o_frm 176, 15, 80) und die Eltern -1 0 1 2 3 1 5 6 0 0 9 10 0 12 13.

| Knochen | 0x40 (alter Cache, 6031) | 0x4B Sherry (Netz) | 0x49 John (alter Cache, 6030) | RE2/ANNETTE_4D (Netz) |
|---|---|---|---|---|
| 0 Becken | (0,-1810,0) | (0,-1268,0) | (0,-1810,0) | (0,-1709,0) |
| 8 Kopf | (-128,-709,0), Welt-y -2519 | (-2,-465,0), Welt-y -1733 | (-98,-704,0), Welt-y -2514 | (-21,-623,0), Welt-y -2332 |
| 9 Oberarm L | (-84,-717,-369) | (-37,-412,-219) | (-59,-692,-369) | (-40,-571,-285) |
| 12 Oberarm R | (-84,-722,369) | (-37,-412,221) | (-59,-692,369) | (-40,-571,285) |
| max. Abweichung | 542 gesamt, 311 ohne Becken | | 129 | |

Diese Zahlen stimmen mit BEFUND.md Abschnitt 2 genau ueberein.

RBJ-Records (RDT +0x5C):

| Raum | RBJ-Offset | Records | EMR-Kopf |
|---|---|---|---|
| ROOM6031 | 0x13C8 | rec0 Marker 1, rec1 Marker 2 (Prefix +0xC4), rec2 Marker 3 | `00 00 08 00 0f 00 50 00` |
| ROOM6030 | 0x17BC | rec0, rec1, rec2 ebenso | `00 00 08 00 0f 00 50 00` |

Der EMR-Kopf bedeutet o_arm 0, o_frm 8, 15 Knochen, 80 B. Die Records tragen also keine Hierarchie, und die Aussage
des Fixers ist bestaetigt.

Der Ablauf im Port ist bestaetigt. Im eigenen Lauf s6031 bindet ROOM6031 den Raum genau einmal
(`[rbj-binder] ROOM6031 Spieler rec2 ...`). Danach folgen `Aktor 1 Typ 0x40 -> rec1 gebunden`, dann
`Sce_em_set type=0x4B slot=0` ohne neue Raumbindung, also ohne Raumladen, und dann (nur mit dem Fix)
`Aktor 1 Typ 0x4B Marker-Slot 1 -> rec1 gebunden`. Fuer ROOM6030 gilt dasselbe mit 0x49 -> 0x4D.

## 3. Eigene exe-Laeufe (eigene Messkopie `re15_pc_p6031_<name>.exe`, beschleunigter Renderer, OHNE RE15_SOFTWARE_RENDER)

Aufbau wie in den Laeufen des Fixers: Karte `probe_r36_story_karte`, Eingabe per `RE15_INPUT_SCRIPT`, `RE15_FIRE_AOT`,
`RE15_FRAMEDUMP` mit Abstand 10 bzw. 50 Bilder.

| Lauf | Aufbau | Bilder | Befund |
|---|---|---|---|
| s6031 (HEAD) | Karte `vor6031 wagen`, `A0.3,W30`, `RE15_FIRE_AOT=1@200#6031`, Ende 340 | F200-F340/10 | Sherry zusammenhaengend, gleicher Untertitel wie die Marke ("I know how to start this! ..."). Kopf auf dem Rumpf, Arme an den Schultern, Hand zum Mund (F250-F300), Arm unten (F240/F310). |
| s6031_alt (Gegenprobe) | dieselbe exe, gelinkt mit enemy_common.c aus b27d96891 | F250/F260/F280 | Das Bild der Marke ist reproduziert: Kopf hoch und versetzt, graue Oberarme seitlich abgeloest. |
| a6030 (HEAD) | Karte `vor6030`, `A0.3,W8,U3,W40`, `RE15_FIRE_AOT=1@3200#6030`, Ende 3700 | F3250-F3700/50 | Annette zusammenhaengend durch "Is everyone aboard?", "Everyone we could find...", "Sherry...? My daughter...", "Can you start this thing?". Gesten wechseln (Arm ausgestreckt F3300/F3500, unten F3400). |

Die Animation laeuft (state.log, Aktor 1):
- 6031, F230-F340: Clips 1 und 5, 30 verschiedene Bildnummern (af).
- 6030, F3250-F3700: Clips 13, 14 und 16, 30 verschiedene Bildnummern.

Nichts ist eingefroren. Damit ist der offene Punkt "gdigrab/beschleunigter Renderer" des Fixers fuer den
Readback-Pfad abgedeckt: Die Bilder kommen aus dem beschleunigten Renderer per RE15_FRAMEDUMP. Eine gdigrab-Aufnahme
des Fensters wurde nicht gemacht.

Bildbogen im Scratchpad: `p6031/s6031_a.png`, `s6031_b.png`, `a6030_a.png`, `a6030_b.png`, `vorher_nachher_pruefer.png`.

## 4. Riegel und Gegenprobe

- `ctest -R "^unit_r41_npcglieder_"`: 2/2 gruen.
- Gegenprobe des Pruefers: `test_r41_npcglieder.c.obj` gegen `enemy_common.c` aus b27d96891 gelinkt (eigenes Objekt
  vor `libre15_engine.a`).
  - sherry6031: rc 1, "groesste relpos-Abweichung 311, Abstand Kind-Eltern 346".
  - annette6030: rc 1, "Abweichung 129, Abstand 147".
  - Beide sind rot, wie gemeldet.
- Der Riegel prueft das richtige Mass: |relpos| je Kind-Eltern-Paar gegen das gezeichnete Modell, mit Toleranz 0. Er
  prueft ausserdem, dass die Keyframes weiter im RBJ-Block liegen, damit der Kanal nicht stillschweigend auf die Bank
  zurueckfaellt.

## 5. Testsaetze (Summenzeilen, selbst gelaufen am Kopf f6204ab0a)

- `ctest -R "^(unit_|prop_|probe_)" -E "^(unit_r38_releaseskript_|unit_r30_android_|unit_r35_android_|unit_r39_titelversion_cmake)" -j 6 --timeout 600`
  -> **100% tests passed, 0 tests failed out of 1100**, Total Test time (real) = 264.43 sec.
  - `unit_r41_birkin_wand_griff` ist dabei gruen (208 s). Im abgebrochenen Lauf des Fixers war er rot; das passt
    zu Last durch parallele Laeufe, siehe Memory "GUI-Tests flattern".
- `ctest -R "annette|story|torweg|kartenbits|g5auftritt|r41_story80|zug" -j 2 --timeout 1800`
  -> **100% tests passed, 0 tests failed out of 31**, Total Test time (real) = 1647.51 sec. Das sind 30 wie in Welle 2
  Lauf B plus der neue Riegel `unit_r41_npcglieder_annette6030`.

## 6. Offene Punkte

- Wie der Fixer schreibt: Der Initialisierer des Part-Pools (+0x188 aus +0x84) ist nicht einzeln disassembliert. Die
  Belege @0x800224a8 und die RBJ-EMR ohne Hierarchie reichen aber fuer die Aussage "die Knochenlagen kommen vom Modell
  im Slot".
- Nach dem master-ff muss die exe im Hauptbaum neu gebaut werden, damit der Nutzer den Fix sieht.
