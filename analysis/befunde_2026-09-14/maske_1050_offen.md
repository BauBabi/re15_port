# ROOM1050 Cut 6 — „Leon ist teilweise transparent": offen, mit korrigierter Messgrundlage

Nutzer 2026-09-14: *„Leon ist teilweise transparent bei den PRI Overlays - siehe Marker."*

## 1. Zwei Zahlen aus den Vorrunden waren falsch — beide berichtigt

**(a) Die zitierte Figurspanne „8464..9972" gehört zu einem ANDEREN RAUM.**
`analysis/nutzer_marken_2026-09-13/befund_abend.log` Zeile 1419 steht unter **R1070 C6**.
Die einzige F9-Marke für R1050 C6 ist **MARKE 9** (Zeilen 836-867):

    F258  R1050 C6  pos=(23232,0,-21128)  kasten=x70..94,y100..141
    Gezeichnete Dreiecke: Kamera-z 8983..10151  (Maskentiefe 140..158)

Damit ist auch die daraus abgeleitete Schranke „depth <= 132" falsch. Richtig ist:
Vollverdeckung verlangt **Tiefe <= 139**, gar keine Verdeckung **Tiefe >= 159**.

**(b) Noch davor war „9120/9352/9584" als Spielertiefe angesetzt.** Das sind FUSS/HÜFTE/KOPF
einer Pose, analytisch aus der Aktorlage gerechnet (`main.c:5027-5034`) — eine Messhilfe,
nicht der Sortierschlüssel. Der Zeichner vergleicht gegen den PRO-POLYGON-z
(`render_pc.c:877/907` gegen `s_textri_depth`, gesetzt `:2145-2154`).

## 2. ⛔ Die Messschiene selbst deckt nur 14,2 % ab

`re15_render_pc_tri_z_reset()` (`main.c:7023`) … `re15_render_pc_tri_z_freeze()`
(`main.c:7799`) umschließen **ausschließlich die Spieler-Knochenschleife**. NPC- und
Gegner-Dreiecke (`main.c:8682/8761/8767`, `:8979/9039/9045`) liegen HINTER dem freeze.

Gemessen in ROOM1140 (unabhängig aus dem Dreieckspuffer nachgerechnet):

    Puffer 2426 Dreiecke (lit 2426 / unlit 0 / verworfen 0)
    Fenster[0,344)  n=344   z  9747..11105   <- das meldet die Schiene
    AUSSERHALB      n=2082  z  5036..10423
    ABDECKUNG: 344 von 2426 = 14,2 %

Der nächste nicht erfasste Punkt (z 5036 = Maskentiefe 78) liegt **4711 Einheiten VOR**
dem gemeldeten Minimum. Kontrolle ROOM1130 (kein Gegner): 334/334 = 100 %.
**Die Schiene meldet also nicht das nächste gezeichnete Dreieck, sobald ein Gegner im
Raum ist.** Jede Maskendiagnose, die auf ihr beruht, ist entsprechend zu eng.

## 3. Der Mechanismus des Durchscheinens — an der echten Marke abgelesen

Die Kacheln des Kaffeeautomaten tragen spaltenweise gestaffelte Tiefen (MARKE 9):

    x=35 : 301 301 268 241 216 198      <- AUSREISSER (Schräge als Bodenkontakt gelesen)
    x=45 : 134 134 134 134 134 131 131 131
    x=55 : 136 135 135 135 135 135 135 134
    x=65 : 141 141 141 141 141 141 141 139
    x=75 : 145 147 147 147 147 148 147
    x=85 : 153 153

Leons Dreiecke liegen bei Maskentiefe **140..158**. Die Kacheln bei 141/147/148/153 liegen
also MITTEN in seiner Tiefenspanne: sie decken die hinteren Dreiecke und lassen die
vorderen durch — das ist das Schachbrett, das der Nutzer als Transparenz sieht.
Ursache im Rezept: der Automat steht GEKIPPT (Unterkante von (52,146) nach (85,132)), und
die Spaltenregel (`geom.py:1208-1214`) liest die Schräge je Spalte als Bodenkontakt.
Sein Eintrag in `analysis/esp_masken_2026-09-03/auswahl.json` (Zeilen 1609-1616) trägt
überhaupt keinen Tiefenschlüssel.

## 4. Warum hier NICHT gefixt wurde

Der naheliegende Schlüssel `"aufrecht": "spalten"` wurde gemessen und **macht es nicht
gut**: Tiefe 149..170 im Messkasten, 293 von 538 Punkten weiterhin nur teilweise gedeckt
(vorher 538). Das sind 45 % Verbesserung und **keine Abnahme** — solange eine Zeile
TEILWEISE meldet, scheint Leon dort durch.

Der Grund liegt tiefer: die Freistellung `06_01` ist aus dem Zeilenprofil zurückgerechnet
rund **4300 Welteinheiten hoch** — Pinnwand und die Wand darüber stecken in EINER Region.
Der nächste Schritt ist deshalb keine weitere Zahl, sondern **die Region zu trennen** bzw.
die echte Oberkante des Geräts im Bild zu messen.

## 5. Gleiche Fehlerklasse im Bestand (nachgezählt, bestätigt)

**56 von 153** Gegenständen in `auswahl.json` tragen keinen Tiefenschlüssel, in 11 Räumen:
ROOM1000 (24), ROOM11F0 (6), ROOM1050 (5), ROOM1220 (5), ROOM1010 (4), ROOM1180 (4),
ROOM1230 (4), ROOM1160 (1), ROOM11B0 (1), ROOM11D0 (1), ROOM2090 (1).
Davon **12 beweisbar falsch** (Spalten außerhalb der SCA-Hülle). Drei Gegenstände in
ROOM1000 Cut 3 (`auswahl.json:1346-1367`) erzeugen heute **gar keine Maske** — kein
Bodentreffer, `anwenden.py:483-484` verwirft jede Kachel.

## 6. Was zuerst zu tun ist

1. **Die Messschiene weiten** (Punkt 2) — ohne sie misst jede Folgearbeit 14 % der Realität.
2. Die Region `06_01` trennen (Punkt 4), erst danach ein Profil setzen.
3. Dann die 12 belegten Fälle und die drei maskenlosen aus Punkt 5.
