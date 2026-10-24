# Runde 41 — Spur F "gatorszene" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r41_gatorszene`, Zweig `r41/gatorszene`, Basis `aece7457` (= r41/integration 9f10bdb0
+ AUFTRAG/VERTRAG Stand Nachtrag 16 / FG-E1).

## Punkte (Wortlaut AUFTRAG.md, Zuteilung VERTRAG §0 + Entscheid FG-E1 + Nachtrag 6)

1. "Ich moechte eine neue Cutscene: Wenn man Room 2090 betritt, soll der Charakter einen Zombie am Boden liegen sehen
   mit einen blinkenden Schluessel - Blinkend so - wie bei der anderen Implementierung weiter oben der Feuerloescher.
   Es schwimmt bereits ein Zombie im Wasser, nimm vielleicht diesen. Der Aligator soll dann aus dem Wasser springen
   und den Zombie fressen inklusive dem Schluessel. Wenn der Aligator danach tot ist, soll der Schluessel in der Naehe
   seine Leiche sein und blinken."
   + Nachtrag 6 (Szenenfolge): Ankunft -> Kamera auf Zombie-Leiche mit Schluessel -> oben in 2080 kommen die Zombies
   herein -> erneut Kamera auf den Zombie -> Alligator kommt, frisst Zombie samt Schluessel -> Schrei- und
   Schuettel-Animation Richtung Leon -> Kampfbeginn mit RE2-Kampfmusik (ROOM40A0).
2. "Entferne die Spinnen aus dem ROOM 2090, die nutzen wir nicht mehr."

Ressourcen (VERTRAG 1.1/1.2): Bits 110-113 (110 Szene gesehen, 111 Schluessel liegt, 112 Schluessel genommen,
113 frei), Nachrichten 2090/2091 Ids 42-46.

## Stand (fortlaufend)

- [ ] Pflichtlektuere (AUFTRAG, VERTRAG, UEBERGABE r40/r41, Dossiers B/D)
- [ ] Messung vorher
- [ ] RE-Belege
- [ ] Umsetzung
- [ ] Messung nachher
- [ ] Tests

## 0. Protokoll (neueste unten)

* 2026-10-24 19:4x Start: Baum sauber (`status --porcelain` leer), HEAD aece7457 (= 9f10bdb0 + AUFTRAG/VERTRAG).
  Pflichtlektuere: AUFTRAG.md (ganz, Nachtrag 6 + FG-E1 massgeblich), VERTRAG.md, UEBERGABE.md, Dossier B
  (§3.1, §7, §11.1 B9 untergetaucht, §11.8 Bausteine Schrei/Schuetteln/Musik, §13 FRESSEN), Dossier D §2 (Blink-API).
* Bau des Ausgangsstands (configure + build, 2147 Schritte, EXIT 0) — exe `re15_port/build/platform/pc/re15_pc.exe`.
* Messlaeufe immer mit eigener exe-Kopie `re15_pc_r41f_<lauf>.exe` (Helfer scratchpad `F41/lauf.sh`), RE15_WINDOW_SCALE=1.

## Punkt 2 — Spinnen aus ROOM2090 entfernen

### 2.1 Messung vorher (Stand aece7457, Lauf `v20a0`: RE15_DEBUG_JUMP=20A0@250 + RE15_FIRE_AOT=0@40#20A0 = echter
Tuerweg 20A0 -> 2090, Ankunft (6300,-16250) Cut 2)
- state.log F10: Aktor 1 `t=25 @(-100,-8296)` hp 111, Aktor 2 `t=25 @(-2300,-23896)` hp 110, Aktor 35 `t=23` (Boss).
- F70..F550: beide Spinnen laufen auf Leon zu (Aktor 1 bis (6197,-13482), Aktor 2 bis (6194,-19118)), Angriffsclips 4/7/11.
- Bild F121/F161 (Framedump 1..581/20): die Spinnen sind im Wasser vor Leon zu sehen.
- Weitere Cut-Bilder (je Cut ein Framedump, `F41/mess/cut2090_<c>/bild.ppm`): Cut 0 und Cut 8 zeigen die Spinne neben Leon
  in der Leiter-Nische (Spawn von 2080 aus).

### 2.2 RE-Beleg
- Die Spinnen sind zwei RDT-Saetze in main00 (ROOM2090.RDT und ROOM2091.RDT bytegleich, `xxd -s 0xAB0`):
  `@0x00AB0 44 00 25 00 00 00 00 73 9c ff 00 00 98 df 00 00 00 00 00 00` (Slot 0, Typ 0x25, Kill-Flag 0x73, (-100,0,-8296)),
  `@0x00AC4 44 01 25 00 00 00 00 74 04 f7 00 00 a8 a2 00 00 00 00 00 00` (Slot 1, Typ 0x25, Kill-Flag 0x74, (-2300,0,-23896)).
  Feldlage pc[1] Slot, pc[2] Typ, pc[7] Kill-Flag, pc[8..13] Lage (scd_vm.c op_sce_em_set, Handler 0x800420A0).
- FORM des Verschwindens im Original: ein Sce_em_set-Satz erscheint nicht, wenn das Kill-Flag-Tor ihn verwirft
  (FUN_800420a0 @0x80042120-38; `ori v0,zero,0x8000` / `sw v0,0(s0)` @0x80042230-38 = Eintrag "verworfen").
- NUTZER-VORGABE: "Entferne die Spinnen aus dem ROOM 2090, die nutzen wir nicht mehr." -> Port verwirft genau diese
  zwei Saetze beim Lesen (Muster zombie_abstand_r35.c: Schluessel Raum-Basis + Typ + Original-Lage), kein RDT-Patch,
  kein Kill-Flag gesetzt (0x73/0x74 bleiben frei, kein Spielstand-Unterschied).
- Der in der Aufgabe genannte "TEMPORAERE Spinnen-Freeze" (Suchanker "TEMPORAER") existiert im Stand aece7457 nicht mehr:
  `grep -rn TEMPOR` ueber re15_port ohne Treffer; enemy_ai_common.c:14729 traegt nur noch den Kommentar
  "Spinnen-Freeze 2026-09-10..11 ENTFERNT (Nutzer-Freigabe)". Nichts zu entfernen.

### 2.3 Umsetzung
- NEU `re15_port/engine/src/gatorszene_r41.c` + `re15_port/include/re15_gatorszene_r41.h`:
  `re15_gatorszene_r41_spawn_weg(raum, typ, x, z)` (1 = verwerfen), Riegel-Schalter `re15_gatorszene_r41_spinnen_weg_setzen`
  (nur fuer Mess-Riegel, kein Spielschalter, kein getenv), Zaehler `re15_gatorszene_r41_spinnen_verworfen`.
  Konstanten mit Datei-Offset: Typ 0x25 @0x00AB2/@0x00AC6, Lagen @0x00AB8/@0x00ABC/@0x00ACC/@0x00AD0.
- HAKEN `scd_vm.c` (2 Zeilen, "Runde 41 Spur F"): Include + in op_sce_em_set direkt nach dem Kill-Flag-Tor
  `if (re15_gatorszene_r41_spawn_weg(...)) suppress = 1;` (vor dem Gleichzeitig-Limit, verbraucht keinen Zaehler).
- Riegel MIT Beleg angepasst: `tests/unit/probe_sca_wall_hit.c` Teil C2 (Adult-Spinne an der 2090-Geometrie, setzt
  g_current_room_id = 0x2090): misst die SCA-Klemme der Spinne am ORIGINAL-Satz @0x00AB0 -> Verwerfen fuer die Messung
  aus (`re15_gatorszene_r41_spinnen_weg_setzen(0)`), danach zurueck. `unit_re2_spider_water_2090` setzt keinen Raum
  (g_current_room_id bleibt 0) und ist deshalb unberuehrt; `r31_trefferboxen*` zaehlt Spinnen ueber alle Raeume (> 0 verlangt).
- Gezielt (Stand mit Haken): `ctest -R "^(unit_sca_wall_hit|unit_re2_spider_water_2090|r31_trefferboxen|r31_trefferboxen_versteckt)$"`
  -> `100% tests passed, 0 tests failed out of 4`.

### 2.4 Messung nachher (gebauter Stand mit Haken, exe-Kopien)
- Leon, Lauf `n20a0` (echter Tuerweg 20A0 -> 2090 wie 2.1): debug.log
  `[gatorszene] ROOM2090: Spinne 0x25 bei (-100,-8296) verworfen ...` und `... bei (-2300,-23896) verworfen ...`;
  state.log ab der Ankunft (PL(6300,-16250), Cut 2): 0 Zeilen mit einem Aktor `t=25`, nur der Boss `[35 t=23 ...]`.
- Elza, Lauf `n2091` (RE15_PSELECT_AUTO=1 + _SWITCH=1, Tuerweg 20A1 -> 2091, debug.log `Spieler-Familie PL04`):
  dieselben zwei Zeilen fuer ROOM2091, state.log ab der Ankunft 0 Zeilen `t=25`.
- In 20A0/20A1 bleiben die Schachtspinnen (dort nicht verlangt; `t=25` steht vor dem Tuerwechsel im state.log).
