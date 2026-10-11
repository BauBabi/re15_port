# Runde 36 — Spur I "inventar" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r36_inventar`, Zweig `r36/inventar`, Basis master 78a4a748.
Zuteilung (VERTRAG.md §0/§1.4): I = inventory_common.c + re15_savepoint.c + Debug-Menue; keine Bits,
keine Nachrichten-IDs, keine Ereignis-Nummern.

## Auftrag (woertlich, AUFTRAG.md Z. 28/29)

> - Bei den Debug Menu, bei den man zwischen die Räume springt, schickst du, wenn man mit Elza spielt Elza
>   in Leons Räume. Wenn man mit Elza spielt, sind die Räume mit "1" hinten korrekt, zum Beispiel ROOM 1071.
> - Mit Nutzung von Memory Cards beim Speichern soll sich die Anzahl reduzieren. Sind Memory Cards oder
>   Handgranaten 0 sollen sie aus dem Player inventory verschwinden

Punkte:
* **I1** Memory Card: Speichern verbraucht 1 (Anzahl -1).
* **I2** Memory Card bei 0 -> Slot verschwindet aus dem Inventar.
* **I3** Handgranaten bei 0 (letzter Wurf) -> Slot verschwindet aus dem Inventar.
* **I4** Debug-Menue Raumsprung: bei Elza die xxx1-Raeume (z.B. ROOM1071), nicht Leons.

## Stand

(laufend)

## Messung vorher

## RE-Belege

## Umsetzung

## Messung nachher

## Tests

## OFFEN

## Fuer den Nutzer
