# Runde 36 — Spur A: klettern (BLOCKER)

Zweig `r36/klettern`, Basis master 5a9dbb7b (= v0.8.23 3ec5b1f8 + Runde-36-Auftrag). Dossier wird
fortlaufend geschrieben.

## Auftrag (Wortlaut)

> Ganz wichtig - aus irgendeinen Grund kann ich nicht mehr auf die verschiebbaren Leitern/Tritte klettern -
> z.B. ROOM 10F0. Das macht ein Fortschritt im Spiel unmöglich

## Plan

1. REPRODUZIEREN am gebauten Stand (eigener Baum, ROOM10F0, Tritt verschieben + hochklettern).
2. BISECT v0.8.21 (9362dd7e) -> v0.8.22 (87cc8575) -> v0.8.23 (3ec5b1f8), dann die Runde-35-Commits.
3. ORIGINAL-Mechanismus (climb_common.c, FUN_... Adressen) gegen den Port stellen; Fix muss den Befund
   ERKLAEREN.
4. Pin (Mechanik: Tritt verschieben + hochklettern + oben ankommen, ueber den echten Weg) + Suite >= 592.

## Messung vorher

(folgt)

## Bisect-Tabelle

(folgt)

## RE-Belege

(folgt)

## Umsetzung

(folgt)

## Messung nachher

(folgt)

## Tests

(folgt)

## OFFEN

(folgt)

## Fuer den Nutzer

(folgt)
