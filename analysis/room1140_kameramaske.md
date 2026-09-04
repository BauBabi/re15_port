# ROOM1140: die Kamera verdeckt den Spieler nicht (Nutzer-Befund 2026-09-04)

Nutzer, `fehler/error2.png`: *„irgendwie ist das Kamera PRI offenbar nicht eingebunden."*

## Was NICHT die Ursache war

* **Nicht** der Commit `6c29d834` („ROOM1140 objektweise neu gezeichnet"). Er wurde von
  `ebf5d995` ueberschrieben, das ist der ausgelieferte Stand. Wer nur `git show 6c29d834`
  liest, sieht den falschen Baum.
* **Nicht** fehlende Maskendaten. Das rechte Stativ HAT eine Maske: 732 freigestellte
  Punkte, 3 Rechtecke.
* **Nicht** Lader oder Renderer. `pri.masks[m].depth` wird unveraendert durchgereicht;
  der Beweis steht im Screenshot selbst — die **Stativbeine** werden ueber Leons Hose
  gezeichnet. Waere der Pfad tot, waere auch das nicht sichtbar.
* **Nicht** aus dem Original abschreibbar: ROOM1140 fuehrt in allen zehn Cut-Sektionen
  `FF FF FF FF` (Dateioffsets 0x51C..0x543) — der Prototyp hat fuer diesen Raum
  ueberhaupt keine `sprite.pri`-Daten. Jede Maske hier ist nachgezeichnet, die Tiefe muss
  also gerechnet werden.

## Die Ursache: falsche TIEFE, nicht fehlende Maske

Eine Maske verdeckt nur, wenn `depth * 64 < vz` der Figur (`re15_pri.h:97-107`). Leon
steht im Screenshot auf dem begehbaren Punkt Welt (8500,−19700) → **vz 9904**, verdeckt
wird er also nur von Tiefe **< 154,75**. Die Cut-2-Sektion trug:

| Rechteck | Ziel | Groesse | Tiefe | wirkt bei vz 9904? |
|---|---|---|---|---|
| Kameragehaeuse | (274,104) | 32×32 | **170** | nein |
| Gehaeuse rechts | (306,104) | 5×32 | **178** | nein |
| Stativbeine | (274,136) | 32×29 | 137 | ja |

Genau das zeigt der Screenshot: die Beine liegen vor Leon, das Gehaeuse verschwindet
hinter ihm.

**Warum die Tiefe falsch war:** das Objekt „Kamerastativ rechts" ist als einziges
ROOM1140-Objekt **ohne `fuss`** in `analysis/esp_masken_2026-09-03/auswahl.json`
eingetragen. Ohne `fuss` nimmt `geom.depth_map_objekt` (`geom.py:313-320`) den
Bodenkontakt **je Bildspalte** aus dem untersten Silhouettenpunkt dieser Spalte. Das
stimmt nur fuer die Spalten, in denen ein Bein steht (x=298 → y=164 → Tiefe 122); in den
Spalten des breiten Gehaeuses ist der unterste Punkt die Gehaeuse-Unterkante
(x=289 → 176, x=301 → 193, x=274 → 280). Die Regel unterstellt, dass JEDE Bildspalte den
Boden beruehrt — bei allem, was oben breiter ist als unten, ist das falsch.

Die Auswahldatei sagt es in ihrer eigenen `_warum`-Zeile voraus: *„Wenn im Spiel jemand
dahinter NICHT verschwindet, ist hier die Zeile nachzutragen."*

## Fix

`"fuss": 164` ergaenzt, dann `python re15_port/tools/maske/raum.py ROOM1140 2`.
164 ist kein geratener Wert: es ist die **unterste freigestellte Zeile des Objekts
selbst** (`pri/1140/02_03.png` reicht in Spalte x=298 bis y=164) — das letzte sichtbare
Beinpixel, bevor die Tischkante es verdeckt.

**Abnahme am Artefakt** (nicht an der Absicht): alle drei Rechtecke tragen jetzt Tiefe
**122** (Schwelle vz > 7808). Ueber die Deckungskarte bei Leons Stehplatz gemessen:

```
VORHER   wirksam 16985 px, unwirksam 617 px   (der ganze Kamerakoerper tot)
NACHHER  wirksam 17602 px, unwirksam   0 px
```

Treuepruefung von `raum.py`: „punktgenau, 17602 px", Rechteckzahl unveraendert 84.

## Die LINKE Kamera: vom Nutzer ABGELEHNT, nicht vergessen

⛔ **ENTSCHEIDUNG DES NUTZERS 2026-09-04, nicht neu aufrollen:** *„Die Linke Kamera ist
egal, genauso wie viele andere Punkte im Raum, da man dort sowieso nicht hinlaufen
kann…"* Es wird also KEINE Maske fuer die linke Kamera gebaut. Der Befund bleibt hier nur
stehen, damit der naechste Durchgang ihn nicht erneut als „Fehler" aufgreift.

⚠️ Meine Messung sagte etwas anderes (2483 begehbare Stehplaetze im Bild hinter ihr,
gezaehlt aus `build/floor_1140.txt`). Der Nutzer kennt den Raum; die Zahl stammt aus dem
Bodendump und sagt nur, welche Bodenpunkte im BILD hinter der Kamera liegen — nicht, ob
der Spieler dort im Spiel tatsaechlich hinkommt. Die Zahl war also kein Gegenbeweis,
sondern ein zu schwaches Mass. Festgehalten, damit klar ist, warum sie hier steht.

## Befund zur linken Kamera (nur zur Kenntnis)

Kein einziges der 84 Cut-2-Rechtecke hat Ziel-X < 97. Die linke Kamera steht bei Bild
x3..42, y90..183 (Bodenkontakt ~y=182 → richtige Tiefe 103) und ist voellig unmaskiert;
**2483 begehbare Stehplaetze** liegen im Bild hinter ihr. Das in `auswahl.json` als
„Kamerastativ links" gefuehrte Objekt ist in Wahrheit ein **Stuhl** (`pri/1140/02_02.png`
zeigt Rueckenlehne, Sitz, Armlehne; liegt bei x97..127) — der Name ist korrigiert, damit
der naechste Durchgang nicht wieder an der falschen Stelle sucht.

Ein automatischer Schnitt wurde **versucht und verworfen**: Helligkeitsschwellen 62–110,
Zusammenhangskomponente, Dilatation/Erosion. Mit Aufdickung frisst die Maske weit ueber
die Kamera hinaus, ohne sie fehlen Schwenkarm und zwei der drei Beine. Das deckt sich mit
der Werkzeug-Doku (`maske_aus_png.py`): *„Meine automatischen Verfahren scheiterten alle
an derselben Stelle … Ein Mensch mit dem Lasso hat keins dieser Probleme."*
Gebraucht wird eine Lasso-Freistellung der linken Kamera als PNG mit Alpha; die Lage
misst `maske_aus_png.py` selbst, einzutragen ist dann nur `"fuss": 182`.

## ⛔ Keine Messschiene kann diesen Fehler sehen

* `integration_pri_masken` prueft Parsbarkeit, Anzahl und Geometrie innerhalb 320×240 —
  die **Tiefe an keiner Stelle**.
* `verdeckungskarte.py` zaehlt per Konstruktion nur ZUVIEL Verdeckung
  (`verd = ((box>0) & (box*64 < vz)).mean()`, rot ab 0,60). Eine Maske, die gar nicht
  wirkt, ergibt `verd = 0` und gilt als gut — dieselbe Klasse wie Memory
  `reai-v2-proxy-ohne-zielmenge`.

Was fehlt, ist die Gegenprobe: je Objekt pruefen, ob es an den Stehplaetzen unmittelbar
DAHINTER ueberhaupt verdeckt.

## §2 Nutzer-Befunde 2026-09-04 (abends): Stuhl und Wand — im LAUFENDEN SPIEL gemessen

Nutzer: *„error1 - when I get closer to the chair, it is not in front of me anymore."*
und *„error2, even though i made a pri for this whole wall, my arm clips through."*

### ⛔ Zuerst habe ich es FALSCH gemessen

Aus den Screenshots hatte ich abgeleitet, das Tiefenmodell (`tools/maske/geom.py`,
`vz_at_floor`) sei um Faktor ~1,47 zu fern. **Das war mein Fehler**: ich nahm den
untersten *sichtbaren* Pixel der Figur als Fusspunkt — der ist in beiden Faellen
verdeckt (vom Tisch bzw. von der Wand). Deshalb wurde nichts danach gedreht, sondern
eine Sonde gebaut: `RE15_PRI_LOG=<Datei>` (render_pc.c) schreibt die Spielerposition und
die Tiefen der Dreiecke, gegen die `re15_pri_mask_camera_z(depth)` wirklich vergleicht.

Gegenprobe, dass die Nachrechnung stimmt (`view = R*welt/4096 + t`):

| Ort | berechnet Kopf/Fuss | Sonde meldete |
|---|---|---|
| ROOM1130 Cut 3, Spieler (−3329,0,3332) | 6373 / 6835 | Dreiecke 5724…6920 |
| ROOM1140 Cut 0, Spieler (−7125,0,−14807) | 8559 / 9028 | Dreiecke 5040…10369 (enthaelt auch andere Objekte) |

### Der Stuhl (ROOM1140 Cut 0) — behoben

Verdeckt wird der Spieler GANZ erst bei Tiefe < 133. Der Stuhl trug **136** — er
verfehlte es um DREI und verdeckte nur noch die Beine; beim Naeherkommen fiel er aus.

Ursache: `"fuss": 140` nahm den untersten Punkt der Freistellung als BODENkontakt. Der
liegt aber auf der **Tischplatte** — der Stuhl steht dahinter, seine Fuesse sind
verdeckt. Mit `"ebene": -700` (wie die Konferenztisch-Eintraege) ergeben sich **113/117**
= Schwelle 7232/7488, beide unter 8559. Alle vier Rechtecke verdecken jetzt vollstaendig.

### Die Wand (ROOM1130 Cut 3) — KEIN Fehler, gemessen und begruendet

An seiner Position ist er bei 6373…6835. Die Wandtiefen je Bildspalte:

```
x = 85..95 : 122..832  = Kamera-Z  7808..53248  -> HINTER ihm
x = 97..115:  75.. 94  = Kamera-Z  4800.. 6016  -> vor ihm
```

Sein Arm liegt bei x=85…95, also ueber dem Wandstueck, das **hinter ihm** liegt (973 bis
46413 Einheiten). Dass es dort nicht verdeckt, ist richtig.

⛔ **Die naheliegende Erklaerung „zu grobe Kachelung" wurde GEMESSEN UND VERWORFEN.**
43 der 79 Kacheln liegen ueber einem Tiefensprung (die Kachel bei x=88 spannt intern
122…414 und bekommt den Median 190). Es liegt nahe, feiner zu unterteilen. Nachgerechnet
mit der feinstmoeglichen Aufloesung (Tiefe je BILDPUNKT statt je Kachel):

```
Wand-Freistellung 3450 Punkte
   verdeckt bei Aufloesung je Bildpunkt : 2142
   verdeckt mit der 8-px-Kachelung      : 2447
   Differenz: -305 Punkte, davon 0 die NEU verdecken wuerden
```

Die grobe Kachelung verdeckt heute also **mehr** als die Geometrie hergibt, nicht
weniger. Eine feinere Unterteilung wuerde den Befund **verschlimmern**. Sie wird deshalb
nicht gebaut.

Bleibt als Erklaerung fuer den optischen Eindruck: der Arm steht neben dem Pfeiler ueber
der zurueckweichenden Wand. Wenn er dort in die Wand einzudringen SCHEINT, ist das die
Figur/Kollision, nicht die Maske — eine Maske kann ihn dort nicht verdecken, ohne ihn
weiter unten im Flur faelschlich zu verschlucken.
