# Runde 38 — Auftrag des Nutzers (woertlich, massgeblich)

Eingang 2026-10-16 (nach v0.8.25, Zwischenversion der Runde 37). Ziel-Version: **v0.8.26**.
Verfahren wie Runde 37 (`analysis/befunde_runde37/AUFTRAG.md`, `VERTRAG.md`, `UEBERGABE.md`): je Spur eigener Arbeitsbaum,
Bau -> unabhaengige Abnahme -> Nachbesserung, Integration, volle Suite einmal seriell, Release.
Nutzer-Bilder dieser Runde: `analysis/befunde_runde38/nutzer/key_location.bmp`, `nutzer/fire_extinguisher.bmp` (rotes
Rechteck = gewuenschter Ort; 320x240, Hintergrund des jeweiligen Kamera-Schnitts). Neue Sprachdateien des Nutzers liegen
unter `synchro/` (Regel `synchro/README.md`), darunter `synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav`.

## Wortlaut (2026-10-16)

> Deine nächsten Aufgaben:
>
> - Der Cursor, der zum Beispiel in ROOM 1150 oder ROOM 11F0 verwendet wird, hat immer noch so einen komischen Schatten. Den hat er im Original nicht...
> - Das Leon schon in Cutscenes angegriffen werden kann, wo er sich noch nicht mal bewegen kann ist mist. So wie bei Leon zum Beispiel bei ROOM 1190 bei den Hunden oder 11D0 bei den Hunden. Es sei den es ist Teil der Cutscene wie bei Elza Room 1170.
> - Ändere in ROOM 11B0 in der neuen Cutscene von Marvin seinen Text von "Hold on..." zu "But, hold on..."
> - Bei Elza fehlt der Zopf - schaue in Resident Evil 2 wie es da gemacht ist mit Clairs Zopf.
> - Bei Elzas Intro fehlt das Voiceover. Es ist das gleiche wie bei Leon.
> - Betreff des Logiklochs mit John in Room 1141, das klingt doch gut. Als Hinweis machen wir: „There's an empty key tag on his belt. / It reads "Dining Room".“ und das "Help" ist einfach Teil der Audio files, genau wie das klopfen. Das soll nicht als Text erscheinen.
> - Außerdem gehe mir die noch offenen Punkte der letzten Session an, also die 2 Punkte die wir mit 0.8.25 nicht mehr geschafft haben.
> - Außerdem weitere findings:
> - Im blackscreen vom preintro Text, scheint die map durch vom Raum bei Elza
> - In Elzas Szenario im ROOM 1040 bei *Gunshot* fehlt der Sound für einen Gunshot.
> - Elza stuck in ihrer Cutscene im ROOM 1170.
> - Nutze beim Rekorder Rätsel beim reinsprechen den Sound Irons_Rekorder_Sound.wav aus dem synchro Ordner, und nachdem er fertig abgespielt ist, machst du den Piep Sound für Erfolgreiche Eingabe und den Tür öffnen Sound. Das zusammen mit der erfolgreich message auf den Panel.
> - Ich hätte den fire extinguisher gerne in room 1030 hängen an der stelle von fire_extinguisher.bmp, anstatt in room 1050.
> - Kendo springt am Ende von ROOM 10B0 nicht die Kante runter, wie im Original, sondern läuft in die Luft weiter
> - Bei Room 1260 Springt Kendo auch nicht die Kanten runter. Machen Marvin und Ada in Leons Szenario übrigens auch immer nicht. Das ist falsch.
> - In Room 1260 und folgende fehlt noch der Wasser Effekt, wenn sich Leon oder Elza im flachen Wasser bewegen.
> - In Room 1140 hat das Rolltor keinen Sound beim hochfahren.
> - Bei Leon - bei der Cutscene nach Irons Tod, wo die Zombies durchbrechen, sollen sich die Zombies jeweils auch Richtung Kamera bewegen, also so, wie es bei den Zombies hinter dem Rolltor schon passiert. Die anderen sollen das auch machen. Das wirkt auch bedrohlicher.
> - Das öffnen des Player Inventories hat keinen Sounds, wenn Resident Evil 2 dort Sound hat, möchte ich auch Sound haben.
> - Ich möchte das du den Parking Lot key, und den Prison Key - je nach Szenario - gut sichtbar an die Location platzierst, die ich im Bild key_location.bmp markiert habe.
> - In ROOM 11B0 macht das Verschieben des COntainers mit Marvin zusammen in der Cutscene noch keinen Verschiebe Sound
> - In ROOM 11B0 macht Marvin während/nach dem verschieben des Containers so eine komische Fall Animation, die er im original nicht macht.
> - Ich möchte das du mir in Elzas Szenario für Roy ein Tagebuch schreibst. Hintergrund könnte FILE16_title_paper.png sein. Muss nicht unbedingt ein Welt Model haben, das taucht einfach auf, wenn man Roy, der am Boden liegt noch einmal anklickt im Room 1011. Das Tagebuch soll kommen statt der Code Message. Und es soll folgendes beinhalten:
> 	- Header: Roys Diary
> 	- Text:
> September 22nd
>
> Umbrella... those bastards...
> It was Umbrella who created these freaks now crawling all over the station.
>
> The S.T.A.R.S. members told us what was going on in the Arklay Mountains, and what did we do? ...We laughed them off.
> Worse, we practically held the door open for those things by letting every bitten survivor inside...
> How could we have been so stupid...
>
> September 28th
>
> Damn it, we’re screwed.
> Now someone has sabotaged the Communications Room and wrecked the radio equipment.
> And of course, the others didn’t have to look far for someone to blame. They went straight for my friend John. He’s the only outsider here, and he only got in because I signed him in on a visitor’s pass...
> They didn’t even hesitate. They threw him into a holding cell downstairs.
>
> There’s no way in hell he did this...
>
> Anyway, they took the cell key from me and locked it up in the Communications Room so I can’t let him out. All I’ve got left is my keycard for the outer door... and its code: 0513.
>
> I just hope he’s okay...
>
>
> Den Code bitte wieder grün hervorheben, wie alle Codes in den Dokumenten
>
> Und danach, auf der Karte einmal den Communications Room markieren und einmal den Prison Room, so wie auch schon bei Leon im Communications Room gemacht.
>
> - Wenn Roy in der Cutscene in Room 1020 auf John zuläuft soll er schon irgendwie ein Zombie stöhnen machen.
> - Wenn John dann in der Cutscene in Room 1020 auf Roy schießt, muss auch ein Schusssound kommen
> - In Stage 2 die Spinnen die fliegen fast alle in der Luft und kommen teilweise nicht aus den Schächten raus wie im Original
> - In Stage 2 die Spinnen verschießen kein Gift wie im Original, was dumm ist - denn das Gift ist das einzige das die Charaktere vergiften kann und damit der einzige Grund für das Gegengift Item
> - In Stage 2 die Minispinnen, die rauskommen, wenn man eine Spinne getötet hat fliegen weitestgehend in der Luft und greifen einen nicht an wie im Original
> - In Stage 3 die kleinen Affen können einen nicht treffen, es passiert nichts
> - In Stage 4 die Spiderman kommen nicht richtig aus den Schächten raus wie im Original, obwohl die Schächte korrekt hoch gehen
> - In Stage 4 die Spiderman führen - mal wieder - dazu das Leon teilweise komplett von der Karte verschwindet, wenn sie einen Angreifen. Sowas darf GARNICHT passieren, bei keinen Gegner.
> - In Elzas Szenario bei ROOM 1151 kommt - wenn man Irons anklickt erst - he already passed - und dann beugt sie sich hin und es kommt "I'll be fine, just worry About yourself for now". Ist natürlich quatsch … sie soll sich nur hinbeugen, und es soll stehen "he already passed"...
> - Die Zombies in Stage2 die sich unten im Wasser befinden und zum Rand des Wassers laufen müssen sich an den Rand hochziehen können, wie im Original Resident Evil 1.5
> - In Room 30E0 soll beim aufnehmen des Flammenwerfers nicht mehr kommen - not available in the preview, sondern einfach der Flammenwerfer aufgenommen werden.
> - Ich möchte eine einfache Möglichkeit - über Skript oder sowas - die Ports für alle Systeme zu bauen - der neuen Version eine neue Versionsnummer - minor - zukommen zu lassen und die alte ins Archiv zu bringen.

## Bezug: Logikloch John ROOM1141 (Runde 37, Nachtrag 14 — dort woertlich und mit dem abgestimmten Ablauf)

`analysis/befunde_runde37/AUFTRAG.md` Abschnitt "Nachtrag 14" enthaelt die Nutzer-Nachrichten vom 2026-10-15 und den
abgestimmten Ablauf (Roys Schluessel in 1011, Tuer 1131 -> 1141 verschlossen, John sitzt auf dem Dach fest, Haemmern +
"Help" bis zum geheimen Schalter, panische Musik, Szene ohne "John, wait!"/"sorry", "I cannot leave him alone with those
monsters" solange die Gorillas leben). Der Nutzer hat ihn am 2026-10-16 bestaetigt und festgelegt: Hinweis bei Roys
Leiche = „There's an empty key tag on his belt. / It reads "Dining Room".“; "Help" und Klopfen/Haemmern sind
TON (Audio-Dateien), KEIN Text.

## Bezug: "die 2 Punkte, die wir mit 0.8.25 nicht mehr geschafft haben"

Runde-37-Spuren L (re2zombie) und P (masken36), siehe `analysis/befunde_runde37/UEBERGABE.md` und Release-Notes
v0.8.25 "Folgt am Samstag". Sie laufen als Fortsetzung in ihren Runde-37-Baeumen (`.claude/worktrees/r37_re2zombie`,
`r37_masken36`) mit den Runde-37-Orchestratoren weiter; die Korrektur der vorbestehenden Inversion der RE2-Erholungsleiter
(re2z_hit_main case 3, RE2 `bne` @0x80105b3c) ist jetzt freigegeben.

## Lesarten des Orchestrators (nachpruefbar, keine Wahlfragen)

- "Room 1020"/"ROOM 1040"/"ROOM 1170" in Elza-Punkten = Elzas Raumvariante ROOMxxx1 (1021/1041/1171): Elza-RDT =
  Leon-RDT + 1 (PSX.EXE @0x800397e4/@0x800397ec).
- "neue Versionsnummer - minor": wie alle bisherigen Releases die LETZTE Stelle hochzaehlen (0.8.25 -> 0.8.26, 0.8.99 ->
  0.9.0 wie v0.7.99 -> v0.8.0); das Skript nimmt optional eine explizite Version.

## Nachtrag 1 — 2026-10-16 21:00 (woertlich) -> Spur W

> And additionally: in ROOM 20A0 the shutter that you have to open to push the box down is somehow DIRECTLY in the camera. I cannot see anything... this is not like this in the original....

Lesart: ROOM20A0 (Stage 2, Leon; Elza-Variante ROOM20A1 mitpruefen) — das Rolltor/der Rolladen, der geoeffnet werden muss,
damit man die Kiste hinunterschieben kann, steht/faehrt im Port direkt vor der Kamera und verdeckt das ganze Bild; im
Original nicht. Neue Spur W (VERTRAG §0).
