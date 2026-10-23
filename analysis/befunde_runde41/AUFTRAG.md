# Runde 41 — AUFTRAG (Wortlaut des Nutzers)

Release-Ziel **v0.8.29**. Protokoll: `UEBERGABE.md` (neueste Einträge oben), Spuren: `VERTRAG.md`.
Basis: master nach v0.8.28 (f632a17b) + Masken 5924939d + Doku 0f4d85a2.

## Vorab vereinbart (2026-10-23, Chat vor dem Release v0.8.28)

Nutzer-Design der STAGE2-Kette (Wortlaut):

> Es gibt im ROOM 2030 ebenfalls viele Valves mit Nummern, die Offenbar ebenfalls einem Rätsel entsprechen, das ich
> später einbauen möchte. Meine Idee war zunächst in ROOM 2000 die Leiter zu ROOM 2050 mit herunterlaufenden
> Wasserstrom zu blockieren mit einer Message wie "I cannot climb up the ladder because of the water pressure". Dann
> wollte ich das man das Valve Rätsel in 2030 löst. Damit sollte dann eine Warnung kommen, das das Wasser in einer
> Minute steigt, und man den Raum verlassen soll. Verlässt man den Raum führt das zu ROOM 20B0. später sollte der Raum
> wieder frei werden ohne Wasser. Nach dem Steigen des Wassers dort, sollte die Leiter frei sein, also kein Wasser mehr
> runter laufen. Jetzt haben wir aber auch noch das Rätsel in ROOM 2050 mit den Schaltern das ich auch gerne noch
> kombinieren wollen würde - also das man das Wasserlevel vielleicht im Raum nicht steigen lassen kann, bevor man das
> Rätsel löst. Und die Herunterfahrbare Plattform, bei der ich auch toll finde, wenn die vorher oben wäre und man etwas
> machen muss, damit sie runter fährt...

> Der Umweg 2070 -> 2040 -> 2050 geht nicht, das man um zu 2040 zu gelangen den Aufzug benötigt der Strom braucht.
> Außerdem will ich nicht die Kurbel nutzen um die Bühne zu senken sondern, eher das Pult auf der linken Seite - siehe
> schaltschrank.png. [= ROOM2030 Kamera 2, Pult an der Nordwand]

> Brücken-Pult: Irgendein kleines Rätsel wäre gut.... Wasser in 2030: Über die Ringebene, denn in 20B0 läuft ja das
> Wasser mit großen Druck aus der Tür heraus. Übrigens ist das in unseren Port noch kaputt! Ventilrätsel: einverstanden.

> Zeit abgelaufen und noch im Raum: Tot mit ertrink Effekt. Ventil 13 als Brücken-Rätsel: Einverstanden.

Vereinbarte Reihenfolge: (1) Release v0.8.28 [erledigt], (2) 20B0-Wasser im Original messen und im Port reparieren,
(3) die Kette als Runde 41. Masken STAGE3-6: "danach kannst du deine Masken schließen in der nächsten Version."

## 2026-10-23 16:3x — Auftrag (Wortlaut, nach dem Release v0.8.28)

> Gehe die von vorhin besprochenen noch offenen Punkte an, aslo: Runde 41 startet mit dem Wasserschwall in 20B0 im
> Original. Danach folgen die Ventil- und Brückenkette in STAGE2 und das Schließen der STAGE3–6-Masken, wie mit Ihnen
> vereinbart.
>
> Außerdem:
>
> - Mir fehlt in den Optionen eine Lautstärke Option bei der ich alles lauter oder leiser machen kann.
> - Leon hat eine komische herumfliegende Animation im ROOM 2020 am Anfang wenn er die Kurbel nutzt.
> - In STAGE2 fehlt noch der Wassereffekt vom rein springen von der Kante in das Wasser. Auch die NPCs teilen die
>   gleichen Effekte wenn sie durch das Wasser gehen.
> - Die NPCs z.B. Ada und Marvin können ineinander rein laufen, das ist falsch, die sind beide fest.
> - In ROOM 2050 lassen sich die Kisten in einander schieben, das ist falsch
> - Die Spinnen Animation beim herausklettern vom Gitter und runter klettern an der Wand stimmen noch nicht ganz.
> - Die Map B1 ist jetzt beinahe perfekt, aber bevor man in den vorderen Teil der Waffen Armory - room 1090 - geht -
>   also die vordere Tür - macht der Spieler auf der Map einen Sprung zum hinteren Teil, obwohl man garnicht da ist.
> - Der Spieler kann nicht sterben, wenn er noch mal mit Gift von den Spinnen angeschossen wird, aber bereits auf
>   Danger ist...
> - Die Tür der Werkstatt in ROOM 11B1 bei Elza, wenn sie über ROOM 2000 rein kommt und die Cutscene abspielt wo sie
>   Elza sucht, ist immer noch offen. Ich wollte das sie dauerhaft geschlossen bleibt in Elzas Szenario.
> - Kannst du den Feuerlöscher in room 1030 einen leichten Blink Effekt geben, das man sieht, das der aufnehmbar ist?
>   Ich glaube so etwas existiert bereits in Resident Evil 2, übernehme den.
> - Ich möchte eine neue Cutscene: Wenn man Room 2090 betritt, soll der Charakter einen Zombie am Boden liegen sehen
>   mit einen blinkenden Schlüssel - Blinkend so - wie bei der anderen Implementierung weiter oben der Feuerlöscher.
>   Es schwimmt bereits ein Zombie im Wasser, nimm vielleicht diesen. Der Aligator soll dann aus dem Wasser springen
>   und den Zombie fressen inklusive dem Schlüssel. Wenn der Aligator danach tot ist, soll der Schlüssel in der Nähe
>   seine Leiche sein und blinken. Entferne die Spinnen aus dem ROOM 2090, die nutzen wir nicht mehr.
> - Der Aligator in ROOM 2090 hat bereits einen finisher in Resident Evil 2. Wir hätten garkeinen eigenen bauen
>   müssen... Ich will das du den Finisher und fehlende Animationen aus Resident Evil 2 extrahierst!
> - Generell: Du kannst fast alles vom Aligator Fight von Resident Evil 2 übernehmen. Ich möchte das der Aligator beim
>   hinterher laufen das gleiche Verhalten bei Animation und schreien macht. An gleichen Stellen beschleunigt usw. Ich
>   möchte das auch die Wände ab und an wackeln wie im Original und Putz runter fällt. Das einzige was wir natürlich
>   nach wie vor brauchen, ist das der Aligator weiterhin über die Platform hinten klettern kann.
> Dann will ich folgendes:
> 	- Nach dem Start der Cutscene, wo der Aligator den Zombie mit dem Schlüssel frisst, soll oben im ROOM 2080 im
> 	  Hauptbereich Erst ein Sound kommen der schließendem Metall Tür. Dann sollen oben im Hauptbereich 8 Zombies
> 	  stehen die gespawned sind.
> 	- Ich möchte das im ROOM 2080 am Rand der Reling der Gaskanister steht, der in Resident Evil 2 dafür sorgt, das
> 	  man den Aligator besiegt. Siehe Gas_Can.bmp.
> 	- Hat man den Aligator oft genug angeschossen, soll eine Cutscene Triggern. Es gibt erneut die Geräusche eine
> 	  schließenden Tür. Marvin (Bei Leons Szenario), John (Bei Elzas Szenario) ist im ROOM 2080. Tötet alle 8 Zombies
> 	  mit Kopfschuss - ein Schuss - Kopf platzt - tot. Rennt zu den Gas Kanister nimmt ihn auf in beide Hände und sagt
> 	  so etwas wie - "Hey, Leon, try with this!" Dann schmeißt er den Gaskanister die Reling runter. Der liegt dann in
> 	  2090 an der Stelle im Wasser. Wenn der Aligator dran vorbei kommt, soll er ihn aufnehmen, GENAUSO wie im
> 	  original Resident Evil 2. Sobald er den Kanister im Maul hat, soll man auf ihn schießen können, und die Sterbe
> 	  Animation mit geplatzen Kiefer etc. vom Aligator soll genauso kommen wie im Original.
> - Ich habe diverse neue PRIs für STAGE2 erstellt. die sollen alle sauber eingebaut werden!
> - In ROOM 2000 und ROOM 2001 ist die Animation/Aktion der Zombies durchgehend falsch. Sind sind am Boden und fressen,
>   anstatt aufzustehen wenn man ihnen näher kommt. gleiches trifft zu bei den Zombies in STAGE3 in ROOM 3000, 3010
>   etc. es ist durchgehend falsch. Auch die Animation von Marvin, am Ende seine Abschnitten in STAGE3, bevor zurück
>   gewechselt wird zu Leon ist falsch in ROOM 3060. In Elzas Szenario, in ROOM 3061 in der cutscene klappt das rein
>   krabbeln von Sherry in den Schacht nicht. Am Ende von dieser Cutscene hat Elza auch erneut eine falsche Animation.
>   Ab 30C1 wird es richtig wild... Sherrys Charakter Modell ist völlig falsch und kaputt. Außerdem sollte sie keine
>   Waffe haben. Die Cutscene im ROOM 3091 hat STÄNDIG falsche Animationen....
> - Brich all diese Aufgaben in immer wenige parallele Tasks runter, denn es ist so viel, das schaffst du mit der
>   aktuellen Token Limitierung sowieso nicht.

## Hinweise des Orchestrators (beim Sichern, ohne Wertung)
- Die "diversen neuen PRIs für STAGE2" (75 Freistellungen 11A0 + 2010/2020/2030/2040/2050/2080/20A0) sind als 53
  Kameramasken bereits in master (5924939d, Lage/Ebene in auswahl.json be59e679). Offen ist nur die Sichtprüfung im
  Spiel (Spur M) — gibt es seitdem weitere Dateien unter pri/, meldet `git status pri/`.
- Gas_Can.bmp (Repo-Wurzel, 320x240): rote Marke an der Reling vor dem "SEPTIC POOL"-Schild, oberer Hauptbereich.
- Die 20B0-Flut ist im Port kaputt: EM024 (Typ 0x24, Partikel-Emitter) Unterzweige grid 0/1 (0x8010f130 / 0x8010f3fc
  -> 0x80019700) stehen in enemy_ai_common.c als OFFEN.
- Elzas Variante der Zeile "Hey, Leon, try with this!" heisst sinngemaess "Hey, Elza, try with this!" (John).

## 2026-10-23 17:2x — Nachtrag 1 (Wortlaut, waehrend Welle 1; betrifft Spur A / jede DuckStation-Aufnahme)

> Er sagt immer failed media capture - invalid type. Du musst in duckstation backend auf FFmpeg ändern und container
> auf mp4.

Umgesetzt vom Orchestrator: `%LOCALAPPDATA%/DuckStation/settings.ini` `[MediaCapture]` `Backend = FFmpeg`,
`Container = mp4` (vorher MediaFoundation + avi -> "failed media capture - invalid type"). Gilt ab dem naechsten
DuckStation-Start; Sicherung `settings.ini.vor_ffmpeg_2026-10-23`. Agenten: settings.ini NICHT auf MediaFoundation/avi
zuruecksetzen; Aufnahmen landen als .mp4 (Frames per ffmpeg extrahieren).

## 2026-10-23 17:37 — Nachtrag 2 (Wortlaut; betrifft Spur A und jede Original-Messung mit Bild)

> Du scheinst immernoch kein Video aufgenommen zu haben, wie du es wohl vorhin geplant hast. Warum?

Stand laut Dossier A §1.3: Media-Capture lieferte nur 0,9 s (55 Bilder, videos/...2026-10-23-17-21-39.mp4), danach
Savestate-Reihe + RAM/ESP-Pool statt Video; Fruehphase (Bild 0..130, Osttuer, Cut 11) noch nicht gemessen.
VERBINDLICH (Nutzer-Massstab, Memory reai-v2-duckstation-config-update: "Ist das Video vom Original identisch deinem
Port, dann ist es richtig umgesetzt, sonst nicht"): Spur A nimmt die GANZE 20B0-Flutszene im Original als Video auf
(settings.ini steht seit 17:25 auf Backend = FFmpeg, Container = mp4; Hotkey ToggleMediaCapture per vgamepad-Button),
extrahiert die Bilder (ffmpeg, ab Szenenstart bis Leon frei ist), legt Video + Bildreihe unter
analysis/befunde_runde41/A_belege/ ab (Pfade im Dossier) und vergleicht Bild fuer Bild mit dem Port-Framedump
(RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1): Zeitpunkt, Ort, Zahl und Groesse der Partikel je Bild. Die Abnahme prueft das
Video, nicht nur die RAM-Reihe.

## 2026-10-23 18:5x — Orchestrator-Entscheide zu den Fragen der Spur A (Bau-Ergebnis 18:54; der Nutzer kann widersprechen)

- E1 NPC-Wasserschritte: Der Nutzer-Wortlaut "Auch die NPCs teilen die gleichen Effekte wenn sie durch das Wasser gehen"
  ist eine Vorgabe. Befund der Spur: die ausgelieferten NPC-Geh-Clips 5 (0x40/0x42/0x45) tragen keine Fussaufsetzer
  (Bildwort-Bit 0x4000 fehlt), im Original spritzt also kein NPC. ENTSCHEID: NPC-Schritte BAUEN als PORT-WAHL mit der
  vorgeschlagenen Form (Aufsetzer-Bilder des Geh-Clips 0 derselben EDD auf den 20-Bild-Clip 5 abbilden, Beleg der
  Bildnummern im Dossier), Spritzer + Schritt-Ton wie beim Spieler; Pin je NPC-Typ.
- E2 RE15_ACTOR_MAX 21 -> 36: angenommen, sofern das Original in 20B0 wirklich 35 Emitter fuehrt (DAT_800aca4e=35 am
  Savestate belegt; die Abnahme prueft das gegen den Runde-36-Befund "20 Plaetze, FUN_80037250 sltiu 0x14" und erklaert
  den Widerspruch mit Adresse). Querschnitt: alle Aktor-Schleifen/Schattenliste; die volle Suite vor dem Release ist
  das Gate; PSX-Budget (+28 KB BSS) im Dossier OFFEN lassen.
- Video-Vergleich (Port-Wolke 2-5x duenner/dunkler, erster Schub spaeter): bleibt MANGEL bis Bild-fuer-Bild gleich
  (Helligkeit/Mischmodus der Effekt-0x0f-Sprites gegen VRAM/CLUT des Originals messen, nicht schaetzen).

## 2026-10-23 19:12 — Nachtrag 3 (Wortlaut; bestaetigt und erweitert Entscheid E1, Spur A)

> Zu deiner Aussage - "NPC-Wasserschritte" - ja alles: Effekte wenn sie sich im Wasser bewegen fußschritt sounds wenn
> sie laufen. All das muss auch für die NPCs gelten.

Orchestrator: Damit ist E1 NUTZER-VORGABE, nicht nur Port-Wahl: NPCs (alle Typen 0x40..0x4b, in Szenen UND beim
freien Lauf) bekommen dieselben Schritt-Effekte wie der Spieler — Wasser-Spritzer/Wellen beim Bewegen im Wasser UND
Fussschritt-Toene beim Laufen (Boden-Zweig mit Ton -8 @0x8004575c, Wasser-Zweig) — ueber die Original-Routine
FUN_80045630, deren Aufsetzer-Bits (0x4000) in den ausgelieferten NPC-Geh-Clips 5 fehlen: Aufsetzer-Bilder aus dem
Geh-Clip 0/1 derselben EDD auf Clip 5 abbilden (Bildnummern im Dossier belegen), fuer jeden NPC-Typ, beide Szenarien.
Pin je Typ: Schritt-Ton und Wasser-Effekt beim Lauf durch Wasser (z.B. Ada/Marvin in STAGE2) und Schritt-Ton auf dem
Boden (z.B. 11C0/10F0).
