# Bericht Strang "karte", Stufe 1, Runde 1

## Kurz und in Alltagssprache

- Die Karte der Kanalisation (STAGE2) war kaputt. Fast alle Räume wurden auf das Bild **eines** Raums
  (des großen U-Kanals) gelegt. Ein Raum (2020, wo die Kurbel liegt) hatte gar keine Karte. In einem
  anderen (2050) bewegte sich der Leon-Punkt nicht.
- Das ist jetzt behoben. Jeder Raum der Kanalisation leuchtet auf seinem **eigenen** gemalten Raum auf,
  und der Punkt läuft mit der Figur mit.
- In STAGE3 lagen einige Räume auf falschen oder winzigen Feldern. Dort stand der Punkt in zwei Räumen
  seitlich still. Auch das ist behoben.
- Für alle Räume von STAGE2 bis STAGE6 gibt es jetzt eine Liste, was fehlt (Leon und Elza).
  STAGE4, 5 und 6 sind noch nicht repariert. Das kommt in den nächsten Runden.
- Zwei Stellen in der Kanalisation hat das Original nie gemalt (ein Gang zur Polizeistation und ein
  Teil von Raum 2040). Dort klebt der Punkt noch am Rand. Das ist für Runde 2 vorgemerkt.

Fachwörter: "Blatt" = eine Kartenseite. "Rechteck"/"Kachel" = ein gemalter Raum auf dem Blatt.
"Zeile" = die Umrechnung von der Spielposition auf den Kartenpunkt. "Riegel" = ein automatischer Test,
der den Fehler festhält.

## Was gemessen wurde

1. **Messlatte STAGE1** (was STAGE1 heute kann): Seiten, Grundriss, Türsymbole, Schleier,
   Zeichenreihenfolge, Marker, Seitenwechsel, Sprung an der Tür - mit Zahlen, siehe BESTAND.md §1.
   STAGE1: Marker auf der eigenen Kachel 82 %, Marker läuft mit 85 %, Türsprung Median 4 px.
2. **Bestand STAGE2-6**, jeder Raum, Leon UND Elza (BESTAND.md §5, 130 Zeilen). Werkzeug:
   `re15_port/tests/unit/probe_r42_karte_bestand.c` (Sonde, echter Pfad), Auswertung gegen die Texel
   der Original-Kacheln `DATA/MAP0x.PIX`.
3. **Kartenschirm-Abzüge aus dem echten Spiel** (FB_SHOT, Spielstand per probe_r36_karte):
   Blatt 6 mit Marker in 8 Räumen, Blatt 7 mit Marker in 8 Räumen
   (`analysis/karte_r42/messbild_blatt6_neu.png`, `messbild_blatt7_neu.png`).

## Ergebnis vorher -> nachher (Sonde, Leon+Elza)

| | STAGE1 (Messlatte) | STAGE2 vorher | STAGE2 jetzt | STAGE3 vorher | STAGE3 jetzt |
|---|---|---|---|---|---|
| Räume mit Zone | 74/78 | 18/24 | 22/24 | 30/30 | 30/30 |
| Marker auf eigener Kachel | 82 % | 40 % | 96 % | 76 % | 93 % |
| Marker läuft mit (verschiedene Positionen) | 85 % | 83 % | 85 % | 67 % | 85 % |
| Räume mit stehendem Marker | 10B0 | 2050 | keiner | 3060, 3070, 3080, 30E0 | 3030, 3080, 30E0 (schmal gemalt / offen) |
| Türen mit eigener Marke | 132+8/195 | 32+6/58 | 36+4/58 | 31+20/59 | 43/59 |
| Türsprung Median | 4 px | 29 px | 9 px | 19 px | 8 px |

## Was geändert ist (alles PORT-ERGÄNZUNG, Original-Slots sind meist Stubs {0,0,1,1})

- `re15_port/engine/src/re15_map_zones.h`: Zonenzeilen Blatt 6/7 neu (Kommentarblöcke r42 mit Belegen),
  neue Zonen ROOM2020/2021 (zid 113), ROOM20A0/20A1 (zid 114, PORT-WAHL rect 10), ROOM30D0 Zone 1
  (zid 61 wie 30C0, Varianten-Regel v0.3.96). Türmarken Blatt 6/7 neu (je Durchgang eine Marke auf der
  gemeinsamen Kante, auf_partner 1); die vier Marken von ROOM11A0 bleiben.
- Belege: ausgelieferte Zeilen @0x800768B0+8*Slot (2000 @0x800769E0; 3000/3010/3030/3040/3060/3070/3080/
  3090/30E0 @0x80076A40..@0x80076AB0), Projektion FUN_800473f8 @0x8004741c-0x80047528, Rechtecklisten
  @0x800764D4 (Blatt 6) / @0x80076558 (Blatt 7), Blatt 0xD ohne Kunst (CD-Id @0x80074c66 = 0).
  Hergeleitete Zeilen: `re15_port/tools/karte_r42_zeilen.py` (Kollisions-Bbox mittig in die gemalte
  Fläche, Maßstab höchstens der des Blatts; Spiegel aus den Türen). Gegenprobe außerhalb des Fits:
  Türpaar-Lücke Median 6 px (beide Blätter), Marken 1..6 px neben den gemalten Türsymbolen.
- `re15_port/tests/unit/test_r30_karte_nutzerstand.c`: Klasse-C-Liste um die drei Marken gekürzt, die zu
  den falsch liegenden Räumen gehörten (2030/2080 auf rect 0, 30B0 auf rect 9).

## Riegel und Gegenprobe

- Neu: `unit_r42_karte_st23` (`tests/unit/test_r42_karte_st23.c`): [A] jeder erreichbare Raum hat eine
  Zone, [B] Marker auf den gemalten Texeln (>= 75 %), [C] Marker läuft mit (>= 50 % Positionen),
  [D] jede Tür zeigt ihre Marke - Leon und Elza, 26 Räume x 2. Ausnahmen namentlich mit Grund
  (3030/3080 schmal gemalt, 30E0 Zone 0 offen, 2070 Zone 1/2, 3090 ausgelieferte Zeile, 20A0 r10).
- **Gegenprobe:** auf aaea5f8c2 ROT in allen vier Punkten ([A] 2020/20A0 ohne Zone, [B] 30 Zonen
  daneben, [C] 12 Zonen stehen, [D] 18 Türen ohne Marke), auf dem neuen Stand GRÜN.
- Weitere Teilmengen grün: alle `unit_map_*` (23), `unit_r30_karte_nutzerstand`,
  `integration_map_raum_live`, `integration_map_uebergang`. Der breite Lauf `-R "map|karte|kartenbits"`
  (63 Tests inkl. GUI) lief auf dem Zwischenstand mit 61/63; die zwei roten (unit_map_durchgang,
  unit_r30_karte_nutzerstand) sind danach behoben und einzeln grün.

## Offen

Siehe BESTAND.md §4. Wichtigste: STAGE2 Gang 2070 Zone 2 + Südteil 2040 ungemalt (Schema-Kasten),
ROOM11A0 teilt Blatt 6 rect 2 mit ROOM2050 (Beleg spricht für 2050), 30E0 Zone 0, Treppenmarken Blatt 6/7,
STAGE4-6 komplett (u. a. 5070/5130/6040 ohne Zone, 5040/5120/50A0/5140/5100 auf anderem Rechteck als ihre
ausgelieferte Zeile, stehende Marker 4040/4050/6000/6030). Kein gdigrab-Video gemacht (nur FB_SHOT-Abzüge).
