# Runde 36 Spur N — Karten-Items (RE2-Lageplan als aufsammelbare Etagenkarte)

Baum `.claude/worktrees/r36_kartenitems`, Zweig `r36/kartenitems`, Basis master 78a4a748.
Dossier wird fortlaufend geschrieben (Sitzungslimit).

## Auftrag (woertlich, AUFTRAG.md Z. 73-77)

> - Extrahiere mir Karte01 oder Karte02 von deinen Karten hier: "RE2-Weltmodelle: Öffnen Sie
>   extracted_re2_items/uebersicht.html im Browser. Oben stehen die Schlüsselkarten und die drei
>   Lageplan-Karten zum Aufsammeln.". Hänge sie mir beispielhaft an die Stelle von map3F.bmp, und schalte
>   damit die Map für Etage 3F frei, wie das Kartensystem in Resident Evil 2 auch funktioniert. Auch mit
>   den Text.
> - Bei Room 10D0 möchte ich das du die map für 2F da platzierst wo steht map2F.bmp.
> - Bei Room 1030 möchte ich das du die map für 1F da platzierst wo steht map1F.bmp
> - Map B2 soll im Room 1200 sein bei mapB2.bmp.
> - Map B1 soll im Room 11D0 sein bei mapB1.bmp

Zuteilung (VERTRAG.md): Bank-9-Bits 87 = 3F, 88 = 2F, 89 = 1F, 90 = B2, 91 = B1; Ereignis 28;
Nachrichten-IDs je Raum 2 ab 28 (1150: r35 nahm 20..31 -> pruefen / Item-Modal).

## Stand der Arbeit

- [ ] Messung vorher (Port: Karte ohne Karten-Item, Etagen-Freischaltung heute)
- [ ] RE-Belege RE2: Aufnahme-Skript (room20B0/2130/3060/4040/5040), Bank 33/35, Kartenzeichner
- [ ] Modell karte01/karte02 -> Port-Prop
- [ ] Weltpositionen je Raum (Rueckprojektion der Marke)
- [ ] Freischaltung + Kartenzeichnung
- [ ] Text beim Aufnehmen
- [ ] Tests / Messung nachher

