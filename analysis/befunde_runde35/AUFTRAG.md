# Runde 35 (2026-10-03) — Auftrag des Nutzers, woertlich

Stand vor der Runde: master 19f32749 (v0.8.21, Suite 478/478). Vorherige Runde: 34 (Granaten r34g,
Nacht r34n, Android r34a) — Dossiers `analysis/befunde_runde34_*`.

> Mehrere Dinge:
> - Granaten sollen nicht durch die Wand fliegen
> - Was sollen Zombie Mädchen sein? Wenn es das gibt, muss es natürlich mit portiert werden
> - Die Tests bei Birkin und Alligator sollst du natürlich auch hinzufügen
> - Eine normale Handgranate beim Zombie etc. darf auch gerne mehr Brutalität rein - mehr Zerplatzen, abplatzende Beine, Arme, sowas...:
> - Die Range der Granaten Explosion ist viel zu niedrig, es ist fast unmöglich Gegner damit zu treffen.
> - Die Granate hat den falschen Explosionssound
> - Einige Waffen, wie die Granatwerfer oder der Raketenwerfer gehen noch nicht
> - Andere Waffen wie der Flammenwerfer oder die Colt Python gehen noch nicht richtig.
> - Nach unserer neuen Ada Cutscene im Room 1050, mit der man die andere Tür nicht betreten darf, bevor man Ada gerettet hat, lässt sich innerhalb des selben Raums das Player Inventory nicht mehr öffnen.
> - Einige Doppeltüren von den Türsequenzen die wir erstellt haben, haben unsymetrischen aufbaue, zum Beispiel unsymmetrische Türgriffe. Das ist so im allgemeinen nicht.
> - Wenn ich mit der Super Redhawk auf die Hunde schieße bleiben die Fleisch Effekte die sich rauslösen permanent da in loop.
> - Bei unseren selbsterstellten Dokumenten möchte ich, das der Code darin mit diesen grün hervorgehoben wird.
> - Im ROOM 1010 sind die stehenden Zombies  zu nah an der Tür. Das ist unfair, da man so keine Chance hat, den Zombies auszuweichen. Bitte setze sie ein Stück weiter zurück. In ROOM 1220 teilweise genauso. Die Zombies in den Zellen müssen zumindest so weit weg sein, das man eine Chance hat aus dem Raum wieder raus zu drehen.
> - Außerdem möchte ich das du in ROOM 1010 eine Memory Card hinzufügst im Regal - siehe add_card.bmp
> - Dann möchte ich, das du das Kampfmesser aus den Player Inventar raus nimmst. Das Kampfmesser soll IMMER der fallback sein, wenn keine Waffe ausgewählt ist. Dafür muss es nicht noch extra im Player Inventory liegen.
> - In ROOM 1190 haben die Hunde einen Schatten während sie durch die Luke springen in der Luft. Das ist quatsch und muss während des Springens raus.
> - Außerdem will ich in ROOM 1190 bei der Zielscheibe ganz links und bei der 3. Zielscheibe von links, das dort der Text ergänzt wird: "This target has a surprisingly large number of bullet holes.". Die anderen beiden Zielscheiben sollen den Text haben "This target does not have many bullet holes".
> - Nachdem ich gestorben bin und new game mache habe ich teilweise noch PRIs von meinen Spielstand davor über die angezeigt werden …. Wenn man tot ist, aber auch wenn man den Raum wechselt sollen sämtliche Assets von den Räumen davor entladen sein.
> - Beim Elevator ROOM 1080 bewegt sich auf der Map der Player Cursor nicht.
> - Bei ROOM 1200 nach dem aufnehmen des Minidisc players steht der Zombie von der Trage auf und läuft durch die Luft, statt wie im Original danach auf die Spieler Ebene runter zu kommen.
> - In ROOM 11C0 nach der Ada Cutscene verschwindet Ada nicht, wenn sie sich verstecken soll, und sie muss dann wieder raus kommen, wenn die Monkeys besiegt sind.
> - In ROOM 11F0 taucht nicht auf der Karte auf, wenn man drin ist.
> - In ROOM 1230 bekomme ich die Map von ROOM 11E0.
> - In ROOM 1210 ist der Korridor falsch und so gut wie alle Türen fehlen
> - In ROOM 1200 taucht nicht auf der Karte auf, wenn man drin ist.
> - Die Zombie Arme in ROOM 1210, wenn sie einen greifen bewegen sich nicht synchron zu Leon beim schütteln, dadurch clipped er.
> - In ROOM 11C0 kommt der Monkey noch nicht an der Korrekten Position aus dem Auto
> - Die Monkeys in ROOM 11C0 haben einen komisch beweglichen Teil am Oberkörper, der so nicht im Original existiert
> - Die Monkeys in ROOM 11C0 sind nicht so wie im Original im Original ist die KI zielstrebiger und aggresiver…. Da stimmt noch irgendwas bei der Übernahme nicht
> - Die Monkeys haben die Brust schlagen Animation offenbar noch nicht, die sie im Original manchmal ausführen.
> - Ich möchte das die Monkeys erst springen, wenn sie 3x getroffen wurden, nicht nach jeden Schuss.
> - Ich möchte das du in ROOM 1090 Schrotflinten Munition in die Welt packst, auf den Außenlüfter. Stelle - siehe Shotgun.bmp.
> - Ich will das du mir die Karten Modelle in der Welt zum Einsammeln, also die World items, aus Resident Evil 2 extrahierst und irgendwo ablegst wo ich es sehen kann
> - Ich möchte eine weitere Cutscene haben:
> 	- Wenn man das erste mal den Communication ROOM betritt (ROOM 10F0) soll Ada drin sein - und hinten rechts an den Monitoren stehen bei CUT2. Die Kamera muss immer wechseln zwischen den cuts, solange leon noch nicht da steht bei ihr
> 	- Zuerst Kamera auf CUT2 - Ada
> 	- Dann Kamera auf CUT0 - Leon.
> 	- Leon soll den Arm strecken und sagen "Hey - how did you came in here?"
> 	- Dann soll Leon auf sie zulaufen - links von ihr stehen, damit beide gut im Zentrum stehen für den Dialog.
> 	- Woman: "Did you really think there was only one staff card for the Communication Room?" - Animation soll diese um 180 grad gedrehte Arm Gestik sein
> 	- Woman: Anyway... the communication system is completely destroyed. We won't reach anyone with it anymore… - Dabei soll sie die Kopf Schüttel Animation machen mit leicht gesenkten Kopf.
> 	- Dann Soll Marvin durch die Tür kommen - also Tür knallen Sound - dann Marvin Laden - dann zu Cut0 wechseln zu Marvin
> 	- Marvin schaut dann Richtung Cut2 zu Leon: "Leon! You already made it!" - Arm streck Animation
> 	- Marvin läuft ebenfalls Richtung Cut 2 und steht schräg links zu Leon und Ada, das man alle 3 noch sehen kann.
> 	- Leon: Hey Marvin, glad you made it! wieder arm strecken
> 	- Leon: Allow me to introduce you. This is… ->  arm strecken Richtung Ada
> 	- Ada: … Ada, Ada Wong
> 	- Leon: Ada Wong.
> 	- Marvin: "Hello, glad to meet another Survivor! I'm Marvin."
> 	- Leon: Anyway... looks like we can't contact anyone with this thing anymore. (kopfschüttel Animation Kopf leicht gebeugt.
> 	- Marvin: "Ohh… what do we do then?... "
> 	- Leon: …
> 	- etwas pause
> 	- Leon: I know! The patrol car! We can use it to get out of here!
> 	- Marvin: Yeah, you're right! That could be our way out!
> 	- Leon: Okay, Marvin, you go with Ada to the parking lot and wait there. I'm going to get Chief Irons, and I'll be right behind you!
> 	- Marvin: Alright! Sounds like a plan. Take care Leon!
> 	- Marvin und Ada rennen hintereinander Gemeinsam Richtung Tür
> 	- kurz weiter Cutscene Balken
> 	- Cutscene Ende - Karte soll aufgehen - zunächst kurz eine Weile ROOM 11C0 auf der Karte markierend anzeigen, also so wie bei ROOM 1150 davor. Danach kurz eine Weile ROOM 1150 blinkend anzeigen. So lange der Raum nicht besucht ist, blinken beide weiter.
> 	- Sei bei den Animationen ein wenig flexibel, das es gut passt, wie bei vergleichbaren Dialogen auch.
> 	- Bei Wechsel von ROOM 1060 zu ROOM 1040 soll der Dialog kommen "I have to get the Chief first...", solange man nicht in ROOM 1150 war
> 	- Betritt man ROOM 1150 kommt eine weitere Cutscene:
> 	- Leon: Sir! (Arm Streck Animation)
> 	- Leon rennt zu Irons zur Liege
> 	- Leon: Sir, the communication system can't be fixed! We're going to use the patrol car to get out of here. I came to get you, come with me!
> 	- Irons: Leon... I... I'm proud to have an officer as dependable as you!
> 	- Irons: But... I... I'm not going to make it... I'm sorry...
> 	- Irons: Please... one last favor... Look after yourself and the others who survived. Be a hero... Leon... (Hier soll er wieder den Arm ausgestreckt haben für eine Weile - wie in der 1. Cutscene - und dann soll er den Arm aprupt runter fallen lassen und sich garnicht mehr bewegen - tot.
> 	- Leon: SIR, Sir?!
> 	- Leon bleibt kniend - schüttelt mit leicht geneigten Kopf den Kopf langsam und steht dann langsam wieder auf.
> 	- Kurze Pause
> 	- Dann gibt es einen Knall - Jetzt kommt es drauf an - Wenn man in ROOM 1140 alle Zombies getötet hat, passiert da nichts, falls dort noch Zombies leben, soll es ein lautes Tür knallen geben, und die Anzahl an Zombies die noch leben in ROOM 1130 spawnen bei Cut0. Nach dem Laden soll die Kamera da hin gehen.
> 	- Dann soll es einen weiteren lauten Knall geben - so wie bei der Cutscene von ROOM 1030 - Dann soll - in ROOM 1040 - wenn das Rolltor noch unten ist das Rolltor hoch gehen und 5 Zombies durch kommen. Wenn es bereits offen ist, kommen nur 5 Zombies. Nach Laden der Zombies - Kamera auf Cut1
> 	- Dann - wenn in Room 1070 die Zombies nicht getötet wurden, soll es ein Türknall geben und die Zombies in ROOM 1030 spawnen in Cut7.
> 	- Und dann noch ein Knall und die Zombies kriechen noch einmal durch das Tor in Cut6.
> 	- Dann Wechsel zu Room 11C0 Parking Lot Cut 13, wo Ada in der späteren cutscene schjon steht. Da kommt eine neue Cutscene hin mit Ada und Marvin. Beide drehen sich dann um Richtung Gebäude
> 		- Ada: What was this noise? Did you hear that?
> 		- Marvin: Yes!....
> 		- Marvin: Oh, no, Leon!
> 		- Marvin dreht sich zu Ada
> 		- Marvin: I have to help him, sorry! (Arm streck Animation)
> 		- Marvin rennt aus dem CUT raus Richtung Gebäude
> 		- Ada: Marvin!... (Arm Streck Animation)
> 		- Ende der Cutscene.
> 	- Bis Leon dann den Parking Lot erreicht hat, soll durchweg MAIN01 als Background Musik gespielt werden.
> 	- Wenn Leon dann in ROOM 1120 Cut1 Richtung dem Fenster hinten zuläuft, sollen die Scheiben zerbrechen - so wie in Resident Evil 2 - und eine Krähe "rein fliegen". Die Glas Zersplitter Effekt musst du aus Resident Evil 2 extrahieren, sowie der Knall Sound. Es wäre super, wenn du dann auch im Background das Hintere Fenster etwas "beschädigen" könntest.
> 	- Ich bin auch noch nicht ganz zufrieden mit unserer neuen Cutscene in ROOM 1050. Da diese die "3. Wand" quasi durchbricht, und Leon so mit dem Spieler redet von den Animationen her. Er sollte er so mit sich selbst reden, wie in der Cutscene in ROOM 1170.
> - Das wäre es dann.... Man besiegt die Monkeys - und kommt dann von Stage1 zu Stage2 und alles ist gut.
>
> - Es gibt noch offene Punkte aus der anderen Session:
> Offen, alle niedrig:
>
> - Die Fortschrittsanzeige beim Entpacken wird auf sehr breiten Displays seitlich abgeschnitten.
> - Wenn ein Update eine Datei durch einen Ordner gleichen Namens ersetzt oder umgekehrt, bricht das Entpacken sauber ab; erst der zweite Start stellt den Stand her.
> - Künftige Änderungen am Prüfskript müssen dessen Urteilslogik selbst sorgfältig mittesten.
>
> Die kannst du dann noch angehen.

## Beigelegte Marken (Repo-Wurzel, unversioniert, Kopien in `analysis/befunde_runde35/marken/`)

* `add_card.bmp` — ROOM1010 (Verhoerraum, Kamerawinkel mit Schreibtisch/Monitoren): rote Marke bei
  Bildpunkt (272,157) von 320x240 = das Regal am RECHTEN Bildrand. Dort soll die Memory Card liegen.
* `Shotgun.bmp` — ROOM1090 (Hinterhof, Aussenwand mit zwei Klimageraeten): rote Marke bei (107,133)
  = OBEN auf dem RECHTEN der beiden Aussenluefter (Klimageraet). Dort soll Schrotflinten-Munition liegen.
