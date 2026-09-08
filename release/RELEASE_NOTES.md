# RE1.5 Port — v0.7.16 (Early Preview)

**Die grünen Tische — nach demselben Muster wie die Schreibmaschine gelöst.**

---

## Deine zwei Marken

Der Gegner-Block in der F9-Marke funktioniert (danke fürs Nachschauen), und deine beiden
Abzüge zeigen genau, was zu tun war.

**Marke 2, die Schreibmaschine:** sitzt. Sie steht jetzt sauber vor deinem Bein, das Papier
verdeckt es — genau umgekehrt zu v0.7.13, wo die Beine durch die Maschine liefen.

**Marke 1, die grünen Tische:** Tischkante und Gestell wurden über deinen Oberschenkel
gezeichnet. Ursache dieselbe wie bei der Maschine, nur andersherum: die Freistellung trug
den ganzen Klapptisch als **ein** Objekt — eine waagerechte **Platte** und ein senkrechtes
**Gestell** zusammen, unter dem Modell „aufrecht". Das nimmt *einen* Standpunkt (das
vorderste Bein) für beides: pauschal Entfernung 97…106, und damit näher als du an **jedem**
deiner 257 Standorte in diesem Blickwinkel.

Jetzt sind es zwei Gegenstände, getrennt am Hintergrundbild über die grüne Plattenfarbe:

```
                                  vorher     jetzt
Median verdeckt (257 Standorte)   20,8 %     0,5 %
Standorte mit >= 20 % verdeckt      130        63
Marke F191                          481       193
Marke F367 (deine neueste)          513         9
```

Die 193 an F191 sind die Bildspalten 160…164 — dort liegt das Gestell bei Entfernung
107…118 und du bei 115,9, das **vorderste Tischbein steht dort wirklich vor dir**. Ab Spalte
166 liegt das Gestell bei 129…142 und ist frei.

**Warum die Platte nichts verdeckt:** alle drei Modelle geben ihr 202…306, weit hinter dir.
Das ist richtig — eine Tischplatte auf Hüfthöhe, fast von der Kante gesehen, trifft den
Boden erst weit hinten. Sie deckt nur den Flur dahinter ab.

## Das Muster dahinter

Beide Fälle sind dieselbe Krankheit, und RE2 Retail hat sie nicht: **zwei Gegenstände in
einer Freistellung.** Retail gibt jedem Gegenstand eine eigene Gruppe mit ein paar
gestuften Tiefen (Median 5 je Gruppe, gemessen an 4253 Gruppen aus 250 RDTs). Wo im Port
eine Freistellung zwei Dinge enthält, muss ich sie trennen — danach greift für jedes Ding
das Modell, das zu ihm passt:

| | |
|---|---|
| flach liegend (Teppich, Tischplatte) | `flach` — jeder Bildpunkt ist ein Bodenpunkt |
| aufrecht stehend (Schreibmaschine) | `aufrecht` — ein Standpunkt, Profil je Zeile |
| auf Beinen (Tischgestell) | `spalten` — je Spalte der echte Bodenkontakt |
| große Möbel/Wände | `tiefe: kollision` — aus der Raumgeometrie |

Der Blickwinkel hat reichlich Platz (1,3 % Bildfläche), beide Teile bekommen darum
8er-Kacheln — 45 Rechtecke, 2496 von 65536 Atlaspunkten.

282/282 Tests.
