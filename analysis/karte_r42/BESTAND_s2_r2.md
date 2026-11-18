# Karte r42 Stufe 2 Runde 2 (STAGE4-6) - der T-Gang 5070/5130 ist wieder zu sehen

Zweig r42/karte. Grundlage: Pruefbericht Stufe 2 Runde 1 (ROT 1).

## 1. Befund (gemessen, nicht modelliert)

- Pruefer: FB_SHOT `messbild_blatt9_s2.png` zeigt fuer 5070/5130 nur den Punkt und drei Tuerstriche auf leerem Blau;
  `integration_map_raum_live` meldet `ROT FEHLT: ROOM5070`, `ROT FEHLT: ROOM5130`.
- Neuer Riegel `unit_r42_karte_sichtbar` (Op-Liste von `re15_inv_screen_build`, alle 210 Zonen mit Original-Rechteck,
  Leon und Elza) auf dem Stand cf95284eb: **genau** 5070/5071/5130/5131 auf Blatt 9 r7 ohne Aktuell-Op
  (0/81 Rasterpunkte, 0/1145 gemalte Texel). Auf aaea5f8c2 dasselbe Feld fuer 5020/5021 (damals auf r7).
  Kein anderer Raum auf irgendeinem Blatt ist betroffen.

## 2. Mechanismus

- Blatt 9 r7 (Rechteck (173,86) 56x80, uv (200,16)) traegt in `s_map_teile` fuenf Teilbereiche, alle zid 84 (Zug 5090),
  dazu fuenf Innenwaende zid 84 (`s_map_walls`). Die Teile decken 1109 der 1145 gemalten Texel; die fehlenden 36 sind
  die vier gemalten Querlinien y 107/116/135/143 (je 9 px) - gemessen mit `scratchpad/karte/s2r2/abdeck.py`.
- `re15_inv_screen.c` zeichnete ein Rechteck mit Teilen NUR ueber die Teile (`re15_map_teil_get` -> `teil_zustand`),
  und `teil_zustand` kennt nur den zid des Teils (84). Fuer 5070 (zid 115) / 5130 (zid 116) ergab jeder Teil
  UNBESUCHT -> ohne Kartenbesitz uebersprungen -> nichts gezeichnet, obwohl `re15_map_rect_state` AKTUELL meldete.
- Herkunft: Die Teilbereiche sind eine PORT-ERGAENZUNG (Nutzer 2026-09-07, fehler/howto4.png). Das Original zeichnet je
  Rechteck zwei SPRTs ohne jeden Zustand (Aufbau FUN_80046fd8, Schleife @0x800472fc-0x800473e0, einzige Verzweigung
  `bne` @0x800473dc = Zaehler). Es gibt also kein Original-Verhalten "Teilbereich", gegen das die Abhilfe laufen koennte.

## 3. Abhilfe (PORT-WAHL, keine Verhaltens-Konstante)

- `re15_map_rect_state_fremd(page, rect)`: Zustand nur aus den Zonen, deren zid KEINE Teile auf diesem Rechteck hat.
- `teil_zustand`: liefert den staerkeren von eigenem Zustand und fremdem Zustand.
- Zeichner: bei Teilen und einer fremden Zone wird zusaetzlich das GANZE Rechteck im fremden Zustand gezeichnet
  (gleiche Gatter wie ohne Teile: Durchgang 0 nur fuer aktuell, unbesucht nur mit Kartenbesitz, RE2 @0x8006E744);
  die Teile malen nur, wo ihr Zustand staerker ist, und werden zuerst eingetragen (= oben, Op-Liste von hinten).
- Blatt 3 r5 (ROOM1110, zid 17) hat keine fremde Zone -> `re15_map_rect_state_fremd` = UNMAPPED -> unveraendert
  (`unit_map_teilbereich` gruen).
- 5090/5091 selbst unveraendert: Teile, Innenwaende, Marken zid 84 nicht angefasst (U-12 in r42/rest).

## 4. Riegel und Gegenprobe

`unit_r42_karte_sichtbar` ([I] Marker auf Aktuell-Op, [J] >= 90 % der gemalten Texel des eigenen Rechtecks aktuell,
[K] im Zug 5090 nach einem Besuch in 5070: 5090-Teil rot und oben, T-Gang darunter ganz gruen).
Etagen-Raeume werden auf dem Band geprueft, dessen Etagenzeile genau dieses Rechteck nennt.
Die Wandlinien der Runde-39-Korrekturen (FILL RE15_KARTE_WAND 0x5AD6, TEX.TIM @0x055C) zaehlen in [J] mit (ROOM1130).

| Stand | [I] | [J] | [K] |
|---|---|---|---|
| aaea5f8c2 | ROT: 5020/5021 (B9 r7 0/81) | ROT: 5020/5021 0/1145 | ROT (5070 ohne Zone) |
| cf95284eb | ROT: 5070/5071/5130/5131 0/81 | ROT: dieselben 0/1145 | ROT (gruener Grund fehlt) |
| neu | gruen | gruen | gruen (rot Op 99 ueber gruen Op 100) |

## 4b. Live-Pin verschaerft

`integration_map_raum_live`: "mindestens 6 von 7 betretenen Raeumen sind sichtbar rot" -> "jeder betretene Raum ist
sichtbar rot" (rot_fehlt == 0). Neu 98/98. Gegenprobe mit der verschaerften Schranke: aaea5f8c2 ROT (ROOM5020, 93/94),
cf95284eb ROT (ROOM5070, ROOM5130, 96/98).

## 4c. Abzuege im echten Spiel (FB_SHOT, `abzuege_s2r2.txt`)

53 Laeufe, 51 Abzuege. Erwarteter Marker nachgerechnet an der TATSAECHLICHEN Lage aus debug.log (letzter [walk]-Eintrag
bis Bild 96) mit der Formel von re15_map_zone_marker (Zeilenpfad FUN_800473f8 @0x8004741c-0x80047528, Port-Tausch/Flip,
Klemmung + 4 px Reserve aus re15_inv_map_marker): **51 von 51 auf den Bildpunkt genau**.
- Neu abgezogen (Leon): 5070 x3, 5130 x3 (Raum jetzt sichtbar, Koerper (48,8,48), Wand (176,176,176) - vorher nur
  Blau (0,16,88)); 4000 x3 (Szene sub05 abgeschaltet mit flag 4:38, `Ck bank=4 bit=38 val=0` @ROOM4000.RDT 0x0129E),
  4010 x3 (flag 4:34, @ROOM4010.RDT 0x006BE), 6000 x3 (flag 3:100, @ROOM6000.RDT 0x00F40), 6030 x3 (flag 4:23,
  @ROOM6030.RDT 0x010D0, sub07).
- Elza (xxx1): 32 von 34 Raeumen. Ohne Abzug 4001 und 4031: die Eintrittsszene (Plc_dest des Szenen-Threads) laeuft
  auch mit flag 4:20 + 3:145 (4001, @ROOM4001.RDT 0x01190/0x01214) bzw. 4:25 (4031, @ROOM4031.RDT 0x005C6) und
  mit Kartenaufruf erst bei Bild 900 weiter; der Kartenschirm oeffnet nicht. Unit-Riegel deckt beide Raeume.
- Schleier: in keinem Abzug eine Besucht-Fuellung ausserhalb des eigenen Raums.
- 6000 c00 (Lage 7412,675) -> Marker (219,78) auf BLAU oberhalb der gemalten Flaeche; c02 (235,85) auf der Wandlinie.
  Dort liegt der Gang zur Tuer 6000 -> 50B0 (Tuer-Record 6000 Slot 0, Welt (2800,2750) -> Karte (210,74);
  Bild `messbild_zeilen_blatt12_s2.png`, "Z000>0B0"), den die Kunst r5 nicht zeigt (ungemalter Block oben links).
  Die Zeile ist in Runde 1 hergeleitet (Tuer-Identitaet: die zwei Westtueren = gemalt (191,103)/(191,133)); ein
  Beleg fuer eine andere Lage fehlt. OFFEN, nicht geaendert.

## 5. Offen (unveraendert aus Runde 1)

- 4050: sechs Seitenkammern ohne gemaltes Feld; eigene Zonen braeuchten 6 Besucht-Bits, 5 frei.
- 6000: Feld r5 passt schlecht zur Kollision (25/48); Tuer 6000<->6030 springt 64 px (ausgelieferte 6030-Zeile
  @0x80076BD8).
- Tuermarken-Luecken 50D0<->50E0 15 px, 50D0<->50F0 16 px, 5080<->6010 15 px.
- Fuenf Drehungen PORT-WAHL mit nur einer Tuer (40B0, 5050, 50E0, 50F0, 5080).
- PORT-WAHL Blatt: 5080 auf 12 statt 9 (`ori v1,zero,0x9` @0x8004bf50), 6020 auf 9 statt 0xC (`ori v1,zero,0xc`
  @0x8004c038).
