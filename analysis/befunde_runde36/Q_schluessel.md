# Runde 36 — Spur Q "schluessel" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r36_schluessel`, Zweig `r36/schluessel`, Basis master 78a4a748.

## Auftrag (woertlich, AUFTRAG.md Z. 45 + Z. 49)

> - Ich möchte bei Leon das die Tür zum Parking Lot ROOM 11C0 verschlossen bleibt, bis ein Parking Lot
>   Schlüssel im Communication ROOM 10F0 gefunden wurde. Wähle dir einen Schlüssel aus den Resident Evil 2
>   Items aus, der am ehesten den optischen Eindruck eines Parking Lot Schlüssels macht.
> - Ich möchte bei Elza das der prison key nicht in Room 1011 ist, wie aktuell, sondern er soll da sein, wo
>   dann auch der Parking Lot Schlüssel in Leons Szenario ist. Auch hier - suche die ein gutes Model aus
>   Resident Evil 2 aus für einen prison key.

Ressourcen (VERTRAG.md): Bank-9-Bits 82 (Parking-Lot-Schluessel genommen, Leon 10F0), 83 (Prison Key
genommen, Elza 10F1); Nachrichten-IDs 28..29 in 11B0/11B1, 28..31 in 10F0/10F1 (nur falls noetig);
Ereignis 25 (10F0/10F1/11B0). Muster: inhalt_r35.c (Item-AOT) -> neue Datei schluessel_r36.c,
Tuer-Sperre nach tuer1120_1130.c.

Story-Hintergrund (Nachtrag 14:00, nur Dossier): Kendo/John sitzt wegen des sabotierten Funkraums in
Untersuchungshaft; die Beamten entzogen Roy (Kendos Freund) die Befugnis, ihn zu befreien — deshalb liegt
der Gefaengnisschluessel nicht bei Roy, sondern im Communications Room. KEIN leerer Haken/Text fuer Leon.

## 0. Status

- [ ] Bestandsaufnahme (Item-Tabelle, Prison Key in 1011, Tuer 11B0->11C0, RE2-Schluessel-Modelle)
- [ ] RE-Belege (Item-AOT/Key-Door-Mechanismus)
- [ ] Umsetzung
- [ ] Messung nachher / Tests

## 1. Messung vorher

(folgt)

## 2. RE-Belege

(folgt)

## 3. Umsetzung

(folgt)

## 4. Messung nachher

(folgt)

## 5. Tests

(folgt)

## 6. OFFEN

(folgt)

## 7. Fuer den Nutzer

(folgt)
