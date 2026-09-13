# "Der Oberkoerper sollte abfallen" — was RE2 wirklich tut (Runde 9, 2026-09-13)

Nutzer, mit Marker in ROOM1030: *"da sieht man das die Animation ist, das eigentlich der
Oberkoerper abfallen sollte. tut er aber nicht!"*

Drei Recherchen + sechs Gegenpruefungen (1,5 Mio Token). Rohbericht:
`oberkoerper-ROHBERICHT.json`.

## Die Antwort in drei Saetzen

1. **Der Oberkoerper faellt in RE2 NIE ab.** Part 0 (Brust) bekommt im ganzen
   Zombie-Overlay an keiner einzigen Stelle das Abwurf-Bit 0x40.
2. **RE2 trennt den Zombie am BECKEN.** Beim Schrot-Rumpf-Treffer haengt das Original
   bei Anim-Frame 20 die Huefte (Part 1) samt beider Beinketten aus der Skelettkette aus
   und auf eine ZWEITE Matrix `entity+0x11C` um (`sw v0,288(v1)` @0x80106D00), markiert
   die Parts 1..7 mit `|= 0x9000` (@0x80106DC0-E14) und startet in Phase 2 ueber
   `+0x21A |= 1` (@0x80106AD0) die Zustandsmaschine **FUN_8010B7D4**. Die schiebt die
   Unterhaelfte weg und friert sie in Zustand 4 mit `part[1] |= 0x40` (@0x8010BA9C)
   endgueltig in der Welt ein. Aus Spielersicht: der Rumpf sackt weg, die Beine bleiben
   zurueck.
3. **Genau diesen Aushang hat der Port seit heute** — als "Ragdoll-Bodenanker"
   (versinken2.md, Commit 429663cf). Er schaltet ihn aber in Phase 2 wieder AB
   (`enemy_ai_re2_zombie.c` P2, `re2z_rag_anchor_on = 0`) — also exakt dort, wo das
   Original die Wegschieb-Maschine erst ANschaltet. `FUN_8010B7D4` kennt der Port
   ueberhaupt nicht.

## Was der Wegschleuder-Tod NICHT ist

`FUN_80109610` (3/4 aller Schrot-Kills) wirft nichts ab — exhaustiver Store-Scan aller
39 Stores in 0x80109610..0x801099E0: jede schreibt nach `sp` oder nach `s2` (die Entity).
Der Modellblock-Zeiger aus `lw 408(s2)` taucht NIE als Store-Basis auf, er wird nur als
Matrix-Anker fuer den Effekt-Spawner gelesen. Kein Part-Flag, kein Mesh-Tausch, keine
Wurf-Felder — und keine Delegation (alle neun `jal`-Ziele nachgesehen). Der Port bildet
das korrekt ab; dort ist nichts zu reparieren.

Das restliche 1/4 spielt dieselbe Clip-3/4-Animation und belebt den Zombie mit HP=10 als
KRIECHER wieder (`+0x10E = 0x2001` @0x80106B10).

## Wo es Abwuerfe wirklich gibt

* **Zielen nach UNTEN** (Spalte 0 der Zeile 7): `0x80107438` wirft Parts 3/5/6 ab. Das
  ist die einzige Zerlegung, die die normale Schrotflinte ausloest.
* **Zerreiss-Tod** `0x80108BEC` (Todeszeilen 8/9/17 = Custom Shotgun, Granatwerfer,
  Rakete): Kopf (Part 8) und zwei Gliedmassen (Parts 9/12). Dazu ein Mesh-Tausch fuer
  Part 0 aus dem Reserve-Record 16 (@0x80108FB8-FDC) — der einzige Rumpf-Stumpf im
  ganzen Overlay.

## Zwei Korrekturen an Vorbefunden dieser Sitzung

* Spalte 4/7 der TODES-Zeile 7 ist zweimal `0x80108530`, nicht 0x80105BC0/0x80105438 —
  letztere stehen in der TREFFER-Tabelle, nicht in der Todes-Tabelle.
* Die Klammer ist jetzt in Weltmass aufgeloest: eben zielen ergibt 200..4200 /
  4200..8200 / ab 8200 Einheiten. **Jeder normale Schrot-Kill findet unter 4200 statt** —
  die Port-Klemme auf Klammer 0 liefert fuer die uebliche Spielentfernung also die
  RICHTIGE Zelle. Auch der Zonen-Stempel des Ports ist byte-true. Der Klammer-Befund aus
  `schrot-zerteilen-KURZ.md` bleibt richtig, ist aber fuer den Nahkampf folgenlos.

## Patch-Plan (naechste Runde, NICHT in dieser umgesetzt)

1. `re2z_rag_anchor_on = 0` in Phase 2 entfernen — das Original stellt den Aushang dort
   nicht zurueck, es startet die Maschine.
2. Feld-Set am Aktor: `+0x219` Zustand, `+0x21C/D/E` Clip, `+0x21F` Zaehler, `+0x220`
   Yaw, plus die Matrix `entity+0x11C..+0x138` (ersetzt rag_anchor_x/z durch die
   vollstaendige Matrix).
3. `FUN_8010B7D4` byte-true portieren, pro Frame nach dem Routine-Dispatch (@0x80100550),
   Gate `+0x21A & 1` (@0x8010B830-3C). ⛔ Die RNG-Wuerfe an ihren exakten Stellen
   (@0x8010B8A4, @0x8010B948, @0x8010BB6C, @0x8010BB84) mitnehmen, sonst desyncht der
   RNG gegen das Original.
4. Zeichner: die ELTERN-Matrix von Bone 1..7 ersetzen statt nur die Translation zu
   verschieben (das Original haengt den ZEIGER um). Ab `part_flags[1] & 0x40` die zuletzt
   komponierte Weltmatrix festhalten.
5. Danach, getrennt: `FUN_8010C2A4` (abgetrennte Gliedmasse) und `FUN_8010BCD0`
   (rollender Kopf) — beide ruft das Original pro Frame, beide fehlen im Port.

## ⛔ UNGEPRUEFT (nicht uebernehmen, bevor es belegt ist)

Eine Dimension berichtet als Nebenbefund, die Part-Benennung im Port sei vertauscht
(2-7 = Beine, 9-14 = Arme statt umgekehrt), mit Beleg "CDEMD0.EMS @0x2A800, EMR
parent[] = [-1,0,1,2,3,1,5,6,0,0,9,10,0,12,13]". **Mein eigener Nachschlag an dieser
Adresse liefert etwas anderes**: [-84,61,2,0,8,0,0,0,0,0,0,0,65,0,32] — das ist keine
Elterntabelle. Entweder ist der Offset falsch oder das Feld liegt anders. Die Behauptung
ist deshalb NICHT in den Code uebernommen worden. Naechster Weg: die Hierarchie ueber den
Port-Parser (`re2_ems.c`) aus der geladenen Bank ausgeben lassen, statt roh zu greifen.
Die ZAHLEN im Port (Parts 3/5/6, 9/12, ...) stammen aus dem Original und sind davon
unberuehrt — es ginge nur um Namen in Kommentaren.
