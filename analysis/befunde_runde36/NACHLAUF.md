# Runde 36 — Nachlauf-Liste des Orchestrators

Funde der Abnahmen, die ausserhalb des Wortlauts der jeweiligen Spur liegen, aber nach den Regeln
("Original oder nicht", 100%-Mandat, Beta -> Retail) gebaut oder dem Nutzer gemeldet werden muessen.
Fortlaufend gepflegt; Quelle je Eintrag = Abnahmebericht im Spurbaum.

| # | Quelle | Fund | Stand |
|---|---|---|---|
| 1 | A_abnahme_2.md Mangel 2 | Objekt-Fall Typ 4 fehlt im Port: geschobene Kisten kippen/fallen im Original ueber Kanten (Objekt-Pass @0x8002c0fc-c14c -> Typ-Tabelle @0x80073c70 [4] = FUN_8002c40c -> FUN_8002c444, schreibt obj+0x82). ROOM5020/5021 Lage (4,203)=1 und vermutlich 20A0/20A1 (3,204)=1 entstehen im Port nie; Latch 5020 @0x012F4-302 / 20A0 @0x01AD4-e2 | Folgelauf wf_fc40f428-68b (Nachbesserung 3, Entscheidung: portieren) |
| 2 | B_abnahme_1.md Hinweis 6 | RE2-Hund-Claim PL+0x1D3 \|= 0x80 (@0x801051f8/0x80105200, Pruefung @0x80104e5c-68) fehlt -> mehrere Hunde beissen gleichzeitig (drei Bisse in fuenf Bildern, F582/F585/F587) | offen — Beta->Retail: portieren |
| 3 | B_abnahme_1.md Hinweis 3 | Unter dem RE2-Gehirn spawnt jeder ueberlebte Griff jetzt zusaetzlich das RE1.5-Release-Blut (FUN_80019700 a0=0x1500 @0x8010a574) | dem Nutzer ankuendigen (RELEASE_NOTES) |
| 4 | B_abnahme_1.md Hinweis 4 | ROOM1190 Kampfbild: erstes Decal (Panel 4 = Westen-Rueckseite) erst sichtbar, wenn Leon der Kamera den Ruecken zeigt | dem Nutzer ankuendigen |
| 5 | B Dossier OFFEN | STAGE3-5 (0,0x32)-Stempel (FUN_80112784 / FUN_8010de24 / FUN_8010dfa4), Birkin-Stempel STAGE5, RE2-Kraehe ohne Dive/Strike-Zwilling | offen (vorbestehend) |
| 6 | F_abnahme_0.md Hinweis | Lebender Gorilla hat jetzt den grossen Schatten aus dem disassemblierten INIT (Record 1700/1800 statt 500x600) | dem Nutzer zum Spieltest nennen |
| 7 | D_abnahme_1.md Hinweis 6 | Granaten-Menge erreicht 0 jetzt erst im Spawnbild — Spur I ("Granaten bei 0 entfernen") muss dort entfernen | bei der Integration von Spur I pruefen |
| 8 | C Dossier "Fuer den Nutzer" | Liegender Spawn-Zombie (EXEC[7]) ist weiter treffbar; Dossier stellt das als Frage — Regel "keine Wahlfragen": RE2-Verhalten belegen und bauen | offen |
| 9 | A_abnahme_0.md H1 | ROOM2060 Startlage: die Spinne schiebt Leon nach dem Aufstieg vom Tritt, er schwebt auf y=-1800 (Original-Reihenfolge Push vor Standtest @0x80031cbc/@0x80031ce8; Erreichbarkeit im Original unbelegt) | offen |
| 10 | A_abnahme_0.md H5 | Debug-Sprung mit Elza nach 3091 landet in 3090 | Spur I (Debug-Menue Elza-Raeume) |
