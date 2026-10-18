# Runde 39 — Spur M "schluessel2" (Dossier, fortlaufend)

Baum `.claude/worktrees/r39_schluessel2` (Zweig `r39/schluessel2`, Basis f70f6ec4). Vorstand: Spur L Punkt 2
(`L_optik.md` Punkt 2, `L_abnahme_0.md` 3.x) — Schluessel 10F0/10F1 auf RE2-Bildgroesse (oberes Quartil `RE2_QUANTIL = 0.75`).
Messwerkzeuge (Scratchpad, nicht im Repo): `r39_m_schluessel2/mess.sh` (Sprung-Weg wie `integration_r39_optik_schluessel`),
`mess_continue.sh` (Spielstand + CONTINUE wie `integration_r38_itemorte` k_lauf), Auswerter `probe_r38_itemorte_kontrast`
(Pixel mit |dR|+|dG|+|dB| > 30 zwischen "mit" und "ohne" (Taken-Bit gesetzt), Huelle, Sum|dL|, AUSSEN).

## Punkt (Wortlaut des Nutzers, AUFTRAG.md Nachtrag 5, 2026-10-18 19:29)
1. "Und die Schlüssel im Communicationsroom sind immer noch zu groß, die sollten maximal halb so groß sein."

"Communicationsroom" = ROOM10F0 (Raumname "COMMUNIC. ROOM", DEBUG.BIN @Datei 0x027C0, `map_hint_common.c:38`), Elza-Zwilling
ROOM10F1. "Die Schluessel" = Parking Lot Key (Leon, Item 0x45, ROOM10F0) und Prison Key (Elza, Item 0x42, ROOM10F1), beide
an der Pinnwand (Lage (1358,-2495,-3400), Zone Slot 8), seit Runde 38 K als skalierte RE2-MD1 (`gen/itemorte_r38_md1.inc`).

## Messung vorher (exe des Baums = Basis f70f6ec4 = master mit Spur L, gebaut 19:35, Messkopie `re15_pc_r39m_vorher.exe`)

Szenen wie Abnahme L0: Leon `RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=10F0@250
RE15_PLAYER_POS=0,4000,1024 RE15_FORCE_CUT=5 RE15_SET_FLAG=9:71,3:94 RE15_EXIT_AT=150#10F0` ("ohne" + `9:82`), Elza
`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_GOTO_ROOM=10F1 RE15_PLAYER_POS=0,4000,1024
RE15_FORCE_CUT=5 RE15_EXIT_AT=150#10F1` ("ohne" `RE15_SET_FLAG=9:83`), Framedump Bild 150, Fenster = Pinnwand-Fenster der
Runde-38-Abnahme x Skala (x 131..145 y 75..87 bei Skala 1). debug.log: Leon `Spieler-Familie PL00`, Elza `PL04` in allen Laeufen.

| Lauf | Pixel | Huelle (Breite x Hoehe) | Sum|dL| | AUSSEN |
|---|---|---|---|---|
| Leon Parking Lot Key, Skala 1 (320x240) | 7 | x 136..140 y 80..81 (5 x 2) | 506 | 0 |
| Leon, Skala 3 (960x720, so spielt der Nutzer) | 69 | x 408..421 y 239..246 (14 x 8) | 3731 | 0 |
| Leon, Skala 4 (1280x960) | 125 | x 544..561 y 318..328 (18 x 11) | 6311 | 0 |
| Elza Prison Key, Skala 1 | 5 | x 137..140 y 80..81 (4 x 2) | 317 | 0 |
| Elza, Skala 3 | 63 | x 408..423 y 239..248 (16 x 10) | 3960 | 0 |
| Elza, Skala 4 | 118 | x 543..564 y 318..331 (22 x 14) | 5524 | 0 |

Bitgleich zu Abnahme L0 (69 / 14 x 8, 63 / 16 x 10) und Dossier L (Skala 4: 125 / 18 x 11, 118 / 22 x 14).
ECHTER Lade-Weg (Spielstand mit `probe_r37_feuerloescher_karte` + `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1`, Bild 100, debug.log
`CONTINUE: resumed in room 10f0` / `10f1`, Elza `PL04`): Skala 3 Leon 69 / x 408..421 y 239..246 / 3731, Elza 63 / x 408..423
y 239..248 / 3960; Skala 4 Leon 125 / 18 x 11 / 6311, Elza 118 / 22 x 14 / 5524 — bitgleich zum Sprung-Weg.

Werkzeug-Stand (`itemorte_export.py` ohne `--schreiben`, Basis): Park s 2,4100, gezeichnete Huelle 4,461 x 2,985 px (320er
Bild), Prison s 2,5623, 5,770 x 2,868 px; `--schreiben` erzeugt die committeten `gen/itemorte_r38_*.inc` bytegleich (nur
Zeilenenden; `git diff` leer) — der Generator liest seine eigene Ausgabe nicht (Fixpunkt).

**Grenze "maximal halb so gross" in den Messgroessen (Skala 3):** Parking Lot Key Huelle <= 7 x 4, Prison Key <= 8 x 5
(Skala 4: <= 9 x 5 bzw. 11 x 7); Flaeche (Pixelzahl) dann ~1/4 (~17 bzw. ~16 Pixel bei Skala 3).

## Belege (Form der Nutzer-Vorgabe, Werkzeug-Kette)

* **Zahl = NUTZER-VORGABE, kein RE-Wert:** Faktor 0,5 linear (`NUTZER_FAKTOR_M = 0.5` in
  `tools/r38_itemorte/itemorte_export.py`, `RE15_R39M_FAKTOR100 50` in `gen/itemorte_r38_lage.inc`) auf den Stand Spur L
  (RE2-Quartil-Groesse). "maximal halb so gross" ist eine OBERGRENZE: 0,5 linear haelt sie fuer beide Lesarten (Laenge 0,5;
  Flaeche 0,25 <= 0,5). Genommen wird der GROESSTE Massstab, der sie einhaelt — "halb so gross", nicht kleiner, weil der
  Schluessel sichtbar und aufnehmbar bleiben muss (Runde 38 "gut sichtbar", Runde 39 L "kleiner/subtiler").
* **Form: Bildgroesse linear im Massstab.** Der Massstab steckt nur in den Ecken des gelieferten MD1 (@0x44, Runde 38 K;
  `schluessel_r36.c:359` liefert `re15_itemorte_r38_schluessel_md1`, kein Laufzeit-Massstab — `re15_itemorte_r38_schluessel_massstab`
  lesen nur Riegel). Die Projektion ist die GTE-RTPS `MAC0=(((H*20000h/SZ3)+1)/2)*IR1+OFX, SX2=MAC0/10000h`
  (psx-spx `geometrytransformationenginegte.md:418`): bei fester Tiefe SZ3 linear in IR1, also in s. H kommt aus dem
  Kamera-Satz, selbst disassembliert (`re15_disasm.py dis 0x80021e50`, RE1.5 PSX.EXE):
  ```
  80021e68: lhu a0,2(v0)        ; Kamera-Satz (RDT+0x24 + 32*cut) +2 = fov
  80021e6c: jal 0x80066c30      ; SetGeomScreen
  80021e70: srl a0,a0,7         ; (Verzoegerungsplatz) H = fov >> 7
  80066c30: 00 d0 c4 48  gte_ldH a0  (ghidra1_V2.txt "Possible REG13.OBJ/SetGeomScreen")
  ```
  ebenso @0x80046120 `lhu a0,2(v0)` / @0x80046124 `jal 0x80066c30` / @0x80046128 `srl a0,a0,7`. Im Port
  `camera_common.c` `fov_to_screen_dist` (fov >> 7). Die Pinnwand-Ebene (z = -3400, Schluessel flach darauf) liegt im
  Nutzer-Cut 5 bei vz ~13000 fast auf einer Tiefe; den Rest (Tiefenunterschied ueber die Schluesselflaeche, gerundete
  Ecken) rechnet das Werkzeug am GEZEICHNETEN Netz genau (Bisektion, s.u.).
* **Lage bleibt:** das Werkzeug zentriert das MD1 auf die Mitte des sichtbaren Texel-Rechtecks (mx, mz) und skaliert um diese
  Mitte; `pos = round(mitte)` (Sehstrahl der Nutzer-Marke x Wand z -3400) haengt nicht von s ab -> Lage (1358,-2495,-3400),
  Drehung (3072,0,2048) bzw. (1024,0,0), flaches Licht 0x0010 und Aufhebe-Zone (798,-3600) 1000x1000 / Slot 8 unveraendert.
