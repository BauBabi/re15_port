# RE1.5 Port — v0.7.22 (Early Preview)

**Die harte Regel: begehbare Standfläche heißt keine Maske.**

---

## Warum ausgerechnet diese zwei Winkel — und Dutzende andere nicht

Alle PRIs, die gut aussehen, decken Objekte mit **Kollisionszelle**: du kommst nie näher
als ~250 Einheiten, zwischen dir und dem Objekt bleibt immer ein sauberer Tiefenabstand.

Diese zwei Winkel enthalten die einzigen Objekte **mitten auf begehbarem Boden**. An
deiner Marke F208 standest du bei z=−798 mitten in der Standfläche der Schreibmaschine
(Basis z−958..−768). Dein Körper überstreicht dann selbst ~1000 Einheiten Tiefe — und
*jede* Maskentiefe liegt mittendrin. Drei Marken, drei probierte Tiefen (58…64 / 65…68 /
71), dreimal geschnitten. Das ist unlösbar, kein Feinschliff.

**Capcom wusste das: das Original liefert für genau diese Winkel gar keine Masken.**
Auf der PSX zeichnet Leon immer über Schreibmaschine und Stuhl.

## Was sich ändert

**Entfernt** (Standfläche begehbar — Standort-Schiene über das ganze begehbare Gitter):
* ROOM10E0: Schreibmaschine (2032 Punktbisse), Schrank rechts (1141), Stuhllehne (718),
  und der Teppich-Rest — nachgemessen lagen *alle* seine 518 Punkte vor der
  **Vorderwand-Zelle**, durch die die Kamera schaut; vor den Möbelzellen liegt kein
  einziger. Der Teppich war nie Vordergrund (damit ist auch der Leichen-Konflikt F1585
  endgültig gegenstandslos).
* ROOM10D0: der Stuhl — vier automatische Freistellungen, vier Fehlschläge
  (schwarz auf schwarz); seine Streuner-Punkte waren dein „Leon dahinter transparent".

**Bleibt** (zellengeschützt): Rückwand, Liege, Trennwand, Kachelkante (ihr Fuß steht *in*
der Trennwand-Zelle), Tischplattenecke — und der **Holztisch-Quader**, jetzt auf das
*gemalte* Möbel beschnitten (die Zelle ist ein Polster; ihr rechter Überstand deckte im
Bild den Trolley und blittete ihn über deine Hüfte).

## Nachgerechnet an allen fünf Marken

```
F208  0 Bisse   F315  0   F690  0
F310  Re-Blit nur noch über dem GEMALTEN Stuhl/der Platte (stehen dort echt vor dir)
F3218 295 Beinpunkte über der Platte verdeckt; die 116 offenen liegen ALLE über
      begehbarem Boden rechts der Platte - dort musst du sichtbar sein
```

## Erwartung beim Testen

*Hinter* Schreibmaschine und Bürostuhl zeichnet Leon jetzt **über** das Möbel — exakt wie
auf der echten PSX. Willst du den 10D0-Klappstuhl trotzdem verdeckend haben (über das
Original hinaus): schick mir einmal dein Lasso für den Stuhl, dann bekommt er eine
Hand-Freistellung. Für die Schreibmaschine hilft auch das nicht — begehbare Standfläche.

Tests: **282/282** (im Release-Container), Selbsttest 7/7.
