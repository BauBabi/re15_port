# Runde 37 — Spur Q kistenregel (Dossier, fortlaufend)

Baum `.claude/worktrees/r37_kistenregel`, Zweig `r37/kistenregel`, Basis 6f360fb3 (= abgenommener Stand der Spur I
1d338f4e + AUFTRAG/VERTRAG vom master-Stand 3491155e).
Massgeblich: `AUFTRAG.md` "Orchestrator-Auftrag Q" (woertlich):

> Grund: Spur I (I_feuerloescher.md, Nachbesserung 1, offen) mass: legt Leon den Feuerloescher VOR dem Loeschen in die
> Item-Box, kann er das Feuer in ROOM1090 trotzdem loeschen ("Will you use the Fire Extinguisher?" -> Yes, Rettungsszene) —
> ROOM1090 sub00 @0x02306 `21 03 85 00` prueft nur das Aufnahme-Bit (3,0x85), nie das Inventar. [...] Beta -> Retail: in
> RE2 benutzt man ein Item nur, wenn man es TRAEGT. -> Spur Q kistenregel: Zensus ALLER Original-Abfragen (alle RDTs,
> Leon und Elza), die den BESITZ eines Items ueber ein Bit statt ueber das Inventar pruefen [...]; je Fall belegen, ob das
> Item in der Box liegen kann, und wie RE2 dieselbe Art Abfrage macht [...]. Wo die Box den Fall oeffnet: wie RE2 das
> Item im Inventar verlangen (eine Port-Regel an EINER Stelle, je Fall per Tabelle), mit RE2-Text, wenn es fehlt.
> Basis = abgenommener Stand der Spur I.

VERTRAG 1.4 (Spur Q): neue Logik in kistenregel_r37.c (Tabelle je Fall), Haken an der Stelle, wo das Original das Bit
prueft (scd_vm.c Ck-Pfad oder je Raum), feuerloescher_r37.c (Spur I) nur fuer den 1090-Fall; Texte nur mit RE2-Beleg
(Ids nach VERTRAG 1.2 begruenden).

## Stand
(laufend)

## 1. Messung vorher
(laufend)

## 2. RE-Belege
(laufend)

## 3. Zensus
(laufend)

## 4. Umsetzung
(laufend)

## 5. Messung nachher
(laufend)

## 6. Tests
(laufend)

## 7. OFFEN
(laufend)

## 8. Fuer den Nutzer
(laufend)
