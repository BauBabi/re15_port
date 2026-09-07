# RE1.5 Port — v0.7.6 (Early Preview)

**Der Evidence Room und seine zwei Nebenräume werden jetzt getrennt hervorgehoben** —
so wie du es in `howto4` gezeichnet hast.

---

## Ein Rechteck, drei Zustände

Bisher bestimmte der Port den Zustand (rot = hier stehst du, grün = besucht) **je
Rechteck**. ROOM1110 leuchtete deshalb als Ganzes rot, obwohl seine drei Teile durch
belegte Innenwände getrennt sind und du nur in einem davon stehst.

Eine eigene *Zone* je Teil geht nicht: die bräuchten eigene Rechtecke, und auf Blatt 3
ist keins frei — der Versuch verdrängte ROOM1100 und trieb das Audit von 182 auf 202
Befunde. Die Zone bleibt deshalb ganz, und ihr Rechteck zerfällt in **Teilbereiche** mit
je eigener Weltbox:

| Teil | Bildausschnitt | Weltbox |
|---|---|---|
| Evidence Room | (146,114) 42×31 | x −2909…14950 |
| kleiner Raum oben | (189,122) 24×11 | z −3806…1320 |
| kleiner Raum unten | (189,134) 24×11 | z 1961…7087 |

Als *aktuell* gilt der Teil, in dessen **Weltbox** du stehst — nicht der, dessen
Bildausschnitt der Marker trifft. Die Weltbox ist die Messung, der Ausschnitt nur ihre
Darstellung. Gezeichnet wird je Teil ein Ausschnitt derselben Kachel; die Zeichnung des
Künstlers bleibt unverändert, nur die Einfärbung zerfällt.

Dasselbe fällt für ROOM5090 an, der ebenfalls Selbst-Türen hat: fünf Bereiche statt einem
Block. Insgesamt 8.

## Zwei Sackgassen unterwegs

Beide, weil ich in **Weltkoordinaten** zerlegt habe statt dort, wo du hinschaust:

* **Nur die gezeichneten Innenwände sperren.** ROOM1110s senkrechte Wand endet bei
  z=7100, die Weltbox reicht bis 7600 — durch diese 500 Einheiten hing der ganze Raum
  zusammen. Ergebnis: *ein* Teil statt drei.
* **Alle Wandzellen sperren.** Dann zerhacken die Möbel den Raum in Dutzende Kammern
  (Blatt 12 lieferte 8×8-Schnipsel).

Richtig ist die **gemalte Fläche** des Rechtecks, zerschnitten von genau den Linien, die
der Port dort zeichnet: die sind bereits auf die Fläche begrenzt und laufen von Rand zu
Rand. Die Weltbox jedes Teils folgt danach rückwärts — alle Weltpunkte, die in diesen
Bildbereich projizieren.

## Eine Falle im Zeichner

Der Zwei-Durchgang-Mechanismus (erst der aktuelle Raum, dann der Rest — sonst verschwindet
Rot unter einem grünen Nachbarn) filterte nach dem Zustand des **ganzen** Rechtecks. Ein
Rechteck mit gemischten Teilen wäre nur in einem Durchgang drangekommen und hätte je nach
Standort entweder das Rot oder die grünen Nachbarn verloren. Das Gate sitzt jetzt pro Teil.

## Geprüft

`test_map_teilbereich` stellt den Spieler in jeden der drei Bereiche und verlangt:

```
im Evidence Room       -> Evidence rot,   beide kleinen gruen
im kleinen Raum oben   -> oben rot,       Evidence + unten gruen
im kleinen Raum unten  -> unten rot,      Evidence + oben gruen
```

— **genau einer** aktuell, die anderen **grün** (nicht schwarz), und die Bildausschnitte
überlappen einander nicht.

---

**282/282 Tests**, lokal und im Linux-Container. Alle drei Bereiche zusätzlich direkt im
ausgelieferten Binary nachgewiesen.
