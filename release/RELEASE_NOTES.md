# RE1.5 Port — v0.7.23 (Early Preview)

**Du hattest recht: alles Nicht-Transparente deckt. Punkt.**

---

## Warum dein einfacher Satz die richtige Lösung war

Die Vorderwand-Kollision klemmt dich bei z=−982 (deine 1695 geloggten Positionen enden
exakt dort). Die Vorderkante der Schreibmaschine liegt bei z=−958. **Du kannst nie vor
der Maschine stehen** — nur daneben oder dahinter. Für solche Objekte ist „deckt immer"
schlicht die korrekte Semantik.

Mein Fehler war die Leserichtung deiner Marken: ich habe „Leon blitzt durch die
Schreibmaschine" als *„Maske deckt zu viel"* gelesen und die Tiefe immer weiter
weggenommen (58…64 → 65…68 → 71 → ganz gelöscht). Alle drei Tiefen lagen **mitten in
deinem Körper-Tiefenband** (gezeichnete Dreiecke 3466…4545) — das erzeugte genau das
Geflacker, das du gemeldet hast: halb bedeckt, halb durchgestreckt. Die richtige
Richtung war **näher**, nicht ferner.

## Was jetzt gilt (ROOM10E0, Winkel 7)

Schreibmaschine, Schrank rechts, Stuhllehne und Kachelkante: **feste Tiefe 50** — unter
dem kleinsten je gezeichneten Spieler-Dreieck aller deiner F9-Marken (3466 = Tiefe 54,
also 266 Einheiten Abstand). Ergebnis: das Objekt steht immer vollständig und ruhig vor
der Figur. An deinen drei Maschinen-Marken (F376/F315/F208) simuliert und angesehen:
Maschine samt Papier und Walze komplett vor Leon, keine Fransen.

Die zellengestützten Objekte (Rückwand, Liege, Trennwand, Tisch-Quader in 10D0) behalten
ihre echten Entfernungen — vor die kann man laufen, dort wäre „deckt immer" falsch.

## ROOM10D0

Unverändert zu v0.7.22: der Tisch-Quader deckt deine Beine hinter der Platte; der
Klappstuhl wartet auf dein Lasso (vier automatische Freistellungen haben schwarz auf
schwarz jedes Mal danebengegriffen — deine Hand trifft es in drei Minuten).

Tests: **282/282** (im Release-Container).
