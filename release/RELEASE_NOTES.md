# RE1.5 Port — v0.7.9 (Early Preview)

**Die Tiefe einer Vordergrundmaske kommt jetzt aus der Raumgeometrie**, nicht mehr aus der
Kontur der Freistellung. Und der erste Fehler, den das behebt: die Holzbank in ROOM10C0.

---

## Die Holzbank verdeckt jetzt an ihrem Ende

Dein Befund: *„ein weiterer Marker der unten überdecken muss fehlt … unter meinen Füßen.
Ich stehe am Ende der Bank, aber sie überdeckt mich nicht."*

Gemessen an deiner F9-Marke: du stehst bei Welt(−4468, −5782), Tiefe 156,7. In deinem
Körperkasten trugen die Masken **174..187** — alle zu fern, **null** davon wirksam.

**Ursache:** Die Silhouettenregel nimmt je Bildspalte den untersten sichtbaren Punkt als
Bodenkontakt. Am rechten Bankende ist das aber die **Seitenkante** der Bank, nicht ihr Fuß
— der Sehstrahl durch diesen Punkt landet weit dahinter:

```
Spalte 64 -> Tiefe 150      Spalte 80 -> 174      Spalte 88 -> 187
```

**Fix:** die Tiefe aus den Kollisionsdaten. Die Bank steht dort als Zelle
`x −4000..550, z −6250..−5250`, im Bild x 2..104, Tiefe **81,5..150,5** — also näher als
du. (Und dein x=−4468 liegt knapp außerhalb der Zelle, die bei x=−4000 endet: genau das
„am Ende der Bank".)

| im Körperkasten | vorher | jetzt |
|---|---|---|
| verdeckende Maskenpunkte | 0 | **520** |
| zu fern („frei") | 672 | **0** |

## Der Mechanismus dahinter

`geom.kollisionstiefe()` schießt einen Sehstrahl pro Bildpunkt gegen die Wandzellen des
Raums:

```
Kameraort = R^-1 * (-t)            Richtung = R^-1 * (sx-160, sy-120, H)
Schnitt mit den vier senkrechten Seitenflaechen jeder Zelle, naechster Treffer gewinnt.
```

Angeschlossen über `"tiefe": "kollision"` in der Auswahldatei. Selbstprüfung: der
Treffpunkt Welt(−3950,−1813,5658) projiziert exakt auf Bild (85,140) zurück, und eine
Spielerposition auf (122,6/201,9) gegen (123/201) aus dem Log, Tiefe 78,1 gegen 78.

Das ersetzt eine Ableitung, die von der gezeichneten Unterkante abhing, durch eine aus der
Geometrie — und braucht keine sorgfältig gezeichnete Silhouette mehr.

---

## Zwei Dinge, die ich richtigstellen muss

**1.** In ROOM1130 Cut 3 bringt derselbe Mechanismus **nichts** (Gesamtmetrik 22,4 % →
22,5 %). Ich hatte dir vorher 27 % gemeldet — die kamen aus einem Vorabtest mit einer von
mir *gewählten* Wandhöhe von 3000. Aus deiner Freistellung gemessen ist die Wand mindestens
**4469** hoch; damit bleiben 39 %. Die 27 % waren ein Artefakt meiner Annahme.

**2.** Der Fall dort ist gar kein Tiefenfehler. Du stehst bei Welt x=−4418, die Marmorwand
bei x=−3950 — **468 Einheiten**, bei einem Spielerradius von **450**
(`DAT_80073e94[6]`). Die Kamera ist so gedreht, dass Osten im Bild links liegt; dein
ausgestreckter Arm zeigt also direkt auf die Wand und ragt hinein, weil die Kollision nur
den Körperradius abhält. Die Maske schneidet ihn korrekt ab.

---

**282/282 Tests**, lokal und im Linux-Container.
