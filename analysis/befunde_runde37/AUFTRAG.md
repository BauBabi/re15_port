# Runde 37 — Auftrag des Nutzers (2026-10-12, Antwort auf die Abschlussmeldung von v0.8.24)

## Wortlaut (unveraendert)

> Room 1100 das Problem ist immer noch da, siehe Zombie auf den F9 Marker. Die Marker Datei liegt da wo immer die DAteien liegen mit F9.  Bei Leon ist die blutlache unpassend wenn roy noch nicht tot ist....
>
> Werkstatt abschluss ist perfekt.
>
> Elza bekommt die Info über den prison key später über roys Tagebuch, das passt schon.
>
> Ja, neuen Texte sind super. Bezug zu ada bei elza kann raus.
>
> Die Fahrstuhl Tür in der Tür Animation ist noch nicht korrekt. DA musst du noch mal schauen ... wenn es keine passende Fahrstuhl Tür gibt, müssen wir die wieder bauen....
>
> Beim Aussteigen aus dem Fahrstuhl fehlt noch die Türsequenz. << stimmt.
>
> Items auf erhöhten Flächen lassen sich vom falschen Stockwerk aufnehmen, z.B. die Super Redhawk in ROOM3050 ohne die Kiste. Das war schon in v0.8.23 so. << muss gefixed werden.
>
> Außerdem hätte ich gerne noch:
> bei den Code panels möchte ich das, so lange die Code noch falsch eingegeben ist unten das blaue Licht leuchtet. Ist er dann richtig eingegeben das grüne Licht. Die Kamera darf noch auf den Panels bleiben wenn der Piep und unlock sound kommt.
>
> Und - bei Leon, bevor dem Ende wo er noch zu Irons muss habe ich eine Lücke gefunden - man kann über die Luke bis nach F1 und kann den "Ich muss zuerst irons holen" Blocker bei der Tür überspringen. Ich schlage vor - du machst den Text zusätzlich noch in ROOM 1050 bei der Tür, wo wir auch die Ada Cutscene triggern, wenn wir Ada noch nicht gerettet haben.

## Lesart je Satz (Orchestrator)

| # | Wortlaut (kurz) | Lesart | Spur |
|---|---|---|---|
| 1 | ROOM1100 Problem immer noch da, Zombie auf dem F9-Marker | PRI/Verdeckung in ROOM1100 Cut 1 noch falsch. Marker = Hauptbaum `re15_port/build/platform/pc/befund_1100_F892_marke1.bmp` (2026-10-12 10:07; befund.log MARKE 1 Zeilen ~15850ff: Leon (-14914,0,-25645) rot 4488, Zombie 2 liegend (-14269,0,-25091), 105 Masken des Winkels mit Tiefen) und `befund_1100_F240_marke1.bmp` (2026-10-05: nur der Kopf eines liegenden Zombies ueber der rechten Wand sichtbar). ⛔ Spur O (Runde 36) hatte die Datei nicht gefunden, weil sie nicht im exe-Ordner gesucht hat. | A pri1100 |
| 2 | Blutlache bei Leon unpassend, solange Roy nicht tot ist | Story-Vorschlag R2 (Blutlache an Roys Platz in 1010) wird NICHT gebaut. | — (abgelehnt) |
| 3 | Werkstatt-Abschluss perfekt | 11B0: Marvin schliesst nur die Werkstatt-Tuer ab — bleibt. | — |
| 4 | Elza: Prison-Key-Info spaeter ueber Roys Tagebuch | kein Hinweis-Text noetig (Nutzer schreibt das Tagebuch). | — |
| 5 | Neue Texte super; Ada-Bezug bei Elza kann raus | Texte der Runde 36 freigegeben. ROOM50B1 msg 0 (Elza) "It's Ada. She's dead.It looks like one of those / gorillas killed her." ohne Ada-Bezug umschreiben. | F |
| 6 | Fahrstuhl-Tuer in der Tuer-Animation noch nicht korrekt; sonst neu bauen | ROOM1080: DOOR27 (RE2-Labor-Fahrstuhl) passt nicht. In RE2 eine passende Fahrstuhl-Tuer suchen; gibt es keine, eine eigene bauen (Port-Archiv wie Runde 33/36, Optik des gemalten RE1.5-Fahrstuhls, Mechanik eines Fahrstuhls). | C fahrstuhl |
| 7 | Beim Aussteigen fehlt die Tuersequenz — stimmt | Ausstieg aus der Kabine (ROOM1080/4020 -> Etage) bekommt eine Tuersequenz (RE2: ROOM6010 spielt DOOR27 V0) — mit der Tuer aus #6. | C fahrstuhl |
| 8 | Items auf erhoehten Flaechen vom falschen Stockwerk aufnehmbar — muss gefixed werden | NACHLAUF Runde 36 #73/#74: Band-Gate fuer Item-AOTs (@0x80042cac-ccc) + Member_set 0x12 setzt das Kollisionsband (@0x80041228) — nur gemeinsam. | D itemband |
| 9 | Code-Panels: blaues Licht unten solange falsch, gruen wenn richtig; Kamera bleibt beim Piep + Unlock auf dem Panel | NEU (Port-Wunsch, RE2 als Vorbild pruefen). | E codepanel |
| 10 | Leon vor dem Ende (zu Irons): Luecke ueber die Luke nach F1 umgeht die "Chief first"-Sperre; Text zusaetzlich an der ROOM1050-Tuer, wo die Ada-Szene triggert | Sperre "I have to get the Chief first..." (re15_tuer1060.h, Bedingung (9,73)=0 und ((9,71)=1 oder (3,94)=0)) zusaetzlich an ROOM1050 Slot 4 (-> 10A0, re15_adaruf.h), solange Ada noch nicht gerettet ist bleibt die Ada-Logik. NUTZER-VORGABE. | F sperre1050 |

Nicht beantwortete Fragen aus v0.8.24 ("Zu entscheiden": Tuerknall 106/205, Groesse Parking Lot Key, Tischmasken 10F0,
Lageplaene 1140/Groesse/1030, Treppenhaus-Blaettern, weitere Tueren, ROOM4000-Tonfolge, Story-Vorschlag V6 Montage-Folgen
bei Elza, Karte 1090/1190, Untertitel main16.5) bleiben wie gebaut; V6 wird nicht gebaut, solange der Nutzer nicht zustimmt.

## Nachtrag 2026-10-12 (waehrend des Aufbaus der Runde, woertlich)

> Weitere findings - bei der Beretta M93R muss - wenn sie schon mehrfach schießt, das auch gelten das die Gegner mehrfach getroffen werden. Werden sie nur ein einziges mal getroffen bei 3 Schuss, ist es nur Schussverschwendung.

Lesart: Der 3-Schuss-Feuerstoss der Beretta M93R (Waffe 5; Glock 18 = Waffe 6 hat denselben Burst-Handler 0x800338A8)
verbraucht 3 Schuss, trifft den Gegner aber nur einmal. Jeder Schuss des Feuerstosses soll treffen (Schaden + Treffer-
reaktion/Blut je Schuss). Zuerst messen, dann am Original belegen, ob RE1.5 je Schuss trifft (dann Port = Original
nachziehen) oder nur einmal (dann NUTZER-VORGABE). -> Spur G beretta.

## Bestaetigung 2026-10-12 (woertlich)

> ja, perfekt.

Antwort auf die Zusammenfassung der Lesart (Spuren A-G wie oben; Sperre ROOM1050: vor Adas Rettung bleibt die Ada-Szene,
danach der Chief-Text nur in der Endphase — nach der 10F0-Szene und bevor Irons geholt ist; Fruehspiel ungesperrt;
Werkstatt bleibt, neue Texte uebernommen, keine Blutlache, kein Elza-Hinweis, V6 nur auf ausdruecklichen Wunsch).
