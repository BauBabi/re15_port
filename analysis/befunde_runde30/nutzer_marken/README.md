# Nutzer-Marken Runde 30 (2026-09-27, 22:56-22:57 Uhr)

Quelle: `re15_port/build/platform/pc/` — der Nutzer spielt aus dem Entwicklungs-Bauverzeichnis.
Dort liegen `befund.log` (5,7 MB), die Abzuege `befund_1070_F*_marke*.bmp` und die
Speicherkarte `re15_card.mcr` (22:54:46). Hierher kopiert, damit Arbeitsbaeume sie sehen.

Alle drei Marken stehen in ROOM1070, Cut 2, pos=(15392,0,6583) rot=480 — das Statusmenue
war offen (Reiter MAP), die Welt stand. Zwischen Marke 1 und den Marken 2/3 liegt ein
NEUSTART (neue Log-Koepfe @Zeile 32969/33006): die Marken 2/3 stammen aus einem GELADENEN
Stand derselben Stelle.

| Abzug | Blatt | Befund des Nutzers | gemessen im 960x720-Abzug | in 320x240 |
|---|---|---|---|---|
| `befund_1070_F233_marke1.png` | POLICE STATION ROOF | "die Wand unten blau" | 315 Pixel, x 444..548, y 465..467, Farbe (16,64,176); die drei anderen Kanten (176,176,176) | x 148..182,7  y 155 |
| `befund_1070_F259_marke1.png` | POLICE STATION 2F | "unten eine Tuer eingezeichnet, die es nicht gibt" | gelbe Marke 3x15 px bei x 564..566, y 534..548, frei unter dem Grundriss; sechs weitere Marken sitzen auf Waenden | (188,3 / 180,3) |
| `befund_1070_F310_marke2.png` | POLICE STATION 1F | "ROOM 1000 ist blau eingezeichnet" | Kachel x 624..662, y 270..362, 3627 Pixel, EINE Farbe (16,64,176); aktueller Raum (48,8,48) | x 208..220,7  y 90..120,7 |

(16,64,176) = RGB 1040b0 = RE2s Besucht-Farbe, CLUT-Zeile 501 Eintrag 1 = 0xD902
(analysis/karte_2026-09-27/umsetzung.md). Der Port hat in Runde 27/28 entschieden:
Mechanik RE2, FARBE RE1.5 (Gruen 0x81A4). Beide blauen Stellen tragen also eine Farbe,
die es im Port nicht mehr geben sollte.

Vierter Befund (ohne Marke): "wenn das Spiel gespeichert und dann geladen wird, [sind]
Teile der Karte die ich bereits freigeschaltet habe verloren gegangen". Die Karte
`re15_card_nutzer_2026-09-27.mcr` ist der Stand, mit dem das nachzustellen ist.
