# Runde 39 — Auftrag des Nutzers (woertlich, 2026-10-17 ~22:00, Antwort im Release-Dialog v0.8.26)

Nutzer-Vorgabe zum Ablauf: "Schließe die vorherigen Punkte ab, Release und dann bauen wir mit all den neuen Punkten die
nächste Version" — also erst v0.8.26 (Runde 38) releasen, DANN diese Liste als naechste Version. "Baue erst einmal fertig,
nutze diese und baue das Stück für Stück, damit nicht zu viel Tokens verbrannt werden, wenn das Limit erreicht ist."
Rekorder: "ich wollte erst Irons_Rekorder_Sound2.wav nehmen statt Irons_Rekorder_Sound1.wav, habe mich dann aber zunächst
doch dagegen entschieden. Es bleibt erst einmal wie es ist."
Dazu offen aus Runde 38: Zweig r38/testtempo (Test-Parallelitaet + Test-Turbo, Workflow wf_8317b9d7-a4e angehalten, Umsetzung
f4412ec1 / a0619152 committet, Messreihen + Pruefung fehlen), NACHLAUF analysis/befunde_runde38/NACHLAUF.md.

## Liste (woertlich)

- In Room 1050 wird das Voiceover nicht abgespielt von vor dem Retten von Ada - das ich unter main09-main11 abgelegt habe. Also die Voiceovers für unsere neue Cutscene
- In Room 1000 möchte ich im Spind, wo vorher der Feuerlöscher drin war jetzt einmal Handfeuerwaffen Munition drin haben. Erst danach den text "Nothing useful", den du jetzt schon hast.
- Memory Cards sind noch nicht Stapelbar, das ist falsch, alles mit einer Zahl dran an amounts was gleich ist muss stapelbar sein.
- Zombies die halb am aufstehen sind, sind nicht treffbar. Das ist so im Original Resident Evil 2 auf jeden Fall nicht.
- Zombies sind über den Tisch hinweg nicht treffbar. Das darf so nicht sein
- Wenn der Spieler verletzt wurde, hat dann kaputte Klamotten und heilt sich aber wieder vollständig - zum Beispiel mit Heilspray - dann muss er auch wieder die ganzen Klamotten haben.
- Das Auto Aim darf nicht gelten für Gegner die der Spieler sonst garnicht sehen könnte, zum Beispiel wenn sie um die Ecke sind und nicht mal ansatzweise in Leons Blickfeld mit Wand davor.
- In Room 10F0 fehlt noch beim öffnen der Schublade der Sound. Den könnte man zum Beispiel aus Resident Evil 2 holen, wenn dort eine Schublade geöffnet wird.
- In Room 1090, wenn die Hunde in der Cutscene rein gesprungen wird, fehlt noch der Sound vom öffnenden Gitter, den könnte man von den öffnenden Gittern der Spinnen aus STAGE2 holen.
- In Room 1090 fehlt noch Sound für den bewegenden Schrank nach dem lösen des Rätsels. Den könnte man aus Resident Evil 2 holen von den bewegenden Regalen in der 2. Etage.
- Ich habe noch mal einen Marker mit F9 gemacht. Leon ist immer noch vom Boden überdeckt an den einen Streifen.
- Der Schlüssel im Raum 10F0 und 10F1 sollte schon kleiner/subtiler sein, wie er dran hängt, nicht so riesig.
- Die Voiceovers für die letzte Cutscene in ROOM 1150, wenn Irons stirbt sind nicht drin....
- In ROOM 11B0 bei der finalen Cutscene mit Ada und Marvin, fehlt das neue Voiceover von But hold on und I want to make sure we don't have any unexpected Visitors. Was dir eigentlich vorliegt.
- Die Gorillas beim Parking Lot können ruhig doppelt so viel Energie haben. Außerdem sollten sie erst die flinch Animation machen, wenn sie von der Energie her 3x mit der Handfeuerwaffe getroffen wurden, nicht bei jeden Schuss.
- Der "Poison" Zustand ist zwar jetzt da, aber er zieht keine Energie ab Stück für Stück, so wie im Originalen Resident Evil 2. Zumindest zeigt er es vom EKG her nicht so an. Da ist noch ein Fehler.
- Bei den Maps des Police Departement passt es noch nicht so ganz. Also, ich habe dir Dateien hinterlegt:
	- map3F_correct.png, map2F_correct.png, map1F_correct.png, mapB2_correct.png. Dort ist immer links, wie es bei uns aussieht und rechts wie es eigentlich aussehen sollte. Keine Doppelwände, Tür immer genau auf Wand platziert, treppen nicht in der Wand platziert usw. Und natürlich muss der Spielermarker da entsprechend passen. Der Room B1 den habe ich nicht gemacht. Da muss du generell noch einmal schauen, da passt nichts so richtig zusammen mit dem wie man sich bewegt im Vergleich zu der Karte. Die Karte von B1 ist noch ziemlicher schrott.
- Bei Elza, direkt in ROOM 1031 in der 1. Cutscene direkt am Anfang hebt sie die Hand aus irgendeinen Grund 2x, also lässt sie einmal fallen zwischendurch und hebt sie nochmal. Im Original hebt sie die Hand nur einmal und senkt sie dann wieder.
- Elza hat im Player Inventory noch das Bild und die Karte von Leon oben drin.
- Bei Elza, nach aufnehmen von Roys Diary muss direkt die Karte kommen, mit den Markern wo man hin muss, noch bevor das Spiel zum bewegen wieder zurückgeben wird nach dem schließen des Tagebuchs.
- Elza kann in ROOM 1090 bei dem Feuerlöscher, bei dem ausgebrannten Fahrzeug hochklettern warum auch immer... das ist falsch...
- Wenn ich einmal von einem Hund gerade angegriffen/gebissen werde, soll ich nicht parallel noch von anderen Hunden angegriffen/gebissen werden können, wenn ich mich nicht wehren kann, so zum Beispiel bei Room 1090.
- Die Zombies in ROOM 1220/1221 und in ROOM 1010 die am Boden liegen kriechen im Original. Das sollen sie bei uns auch.
- In ROOM 1090 ist Leons Positionsmarker der Map außerhalb des aktuellen Raums.

- Ich habe die gelbe Karte bei Elza noch, obwohl ich gesagt habe "discard".
- In Room 10F1 möchte ich, das wenn Elza ihn betritt eine Cutscene läuft wo sie Rechts zum Kommunikationssystem geht und von der Animation her drauf drückt und sagt "Hello, anyone, can you hear me? We Need help!"
- Dann kommt "..."
- Dann "Uhh, this Thing is broken!".
- Suche die dir passenden Animationen dafür heraus.
- Mit Room 1141 bin ich noch überhaupt nicht zufrieden:
	- 1. kommen komische Zombie Geräusche im Hintergrund, die will ich nicht. Es gibt ja keine Zombies.
	- 2. Finde ich es doch besser, wenn John durch die Tür kommt, statt durch das Tor. Es macht keinen Sinn das er dazwischen rein kam. Kannst du nicht während des Tür öffnens die Kamera vorne auf Elza irgendwie machen, dann Tür schließen Sound und dann Kamera auf Tür, wo John bereits da ist?
	- 3. Du hast irgendeinen komischen shout voiceover für John eingebaut. Denn will ich nicht.
	- 4. Ich möchte das du immer im Mix mit einigen Abständen die Voiceovers 1-5 stattdessen abspielst. Dazwischen möchte ich das du immer wieder ein mehrfachen Klopf gegen Tür Sound abspielst, den du aktuell noch garnicht hier drin hast.
	- 5. Ich möchte das Elza darauf reagiert, das sie das das erste mal hört wenn du den Raum betrittst. Sie soll sowas sagen wie "Oh, no, this is Roy! He is in Trouble, I have to help him!".
	- 6. Vor Elzas "What was that?" Möchte ich das du einen Scheiben zerspringen Sound abspielst.
	- 7. Bei Johns Text "It's some Kind of Gorilla...." schaut er ab da nicht mehr vernünftig zu Elza. Das musst du fixen.
	- 8. Nachdem die Monkeys tot sind, rennt John einfach raus. Das macht keinen Sinn. Da muss schon noch etwas kommen wie: "John: Puuuh, that was close! I thought we end up as Gorilla meal….".
	- "John: Elza, follow me, I think I might have found a way out of here!".
	- Alles natürlich mit passenden Animationen.
- Wenn man ROOM 1141 danach verlässt zu room 1171 kommt am anfang noch kurz die Panik Musik. Warum auch immer...
- Wenn John in Room 1171 sagt - "Elza hurry up" macht er so eine komische Kletter Animation statt der ran wink Animation, die er im Original nicht macht. Auch sonst scheinen die Animationen hier noch nicht ganz zu passen.
- Bei "Alright, let's get down from here" fehlt aus irgendeinen Grund mal wieder das Voiceover.

## Hinweise des Orchestrators (beim Sichern, ohne Wertung)
- Die Hilferufe in ROOM1141 meint Punkt 4: synchro/STAGE1/room1141/help01..05.wav (Nutzer, 2026-10-17 13:17-13:20).
- Neue, nach Namensregel automatisch genutzte Aufnahmen liegen in synchro/STAGE2/room2001 (main02..05), synchro/STAGE3/room3000
  (main02..08), synchro/STAGE1/room1241.
- Karten-Vorlagen: map1F_correct.png, map2F_correct.png, map3F_correct.png, mapB2_correct.png (Repo-Wurzel bzw. nutzer/ - beim
  Start der Runde suchen und committen).

## Nachtrag des Nutzers (woertlich, 2026-10-18 ~07:00, nach dem Wochenlimit-Abbruch der Welle 1)

Zur Rueckfrage ROOM1141 Punkt 5 ("Oh, no, this is Roy! ..."): "Das ist natürlich John, nicht Roy. Roy ist nur unten in
den ROOM 1021 aktiv." -> Elzas Zeile lautet "Oh, no, this is John! He is in Trouble, I have to help him!" (Spur J).

Neuer Punkt (Ende der erneut gesendeten Liste):
- Die Gorillas haben beim springen direkt ihren Schatten unter dem beim.
  (Orchestrator: Spur E - der Schatten haengt beim Sprung mit in der Luft unter dem Koerper statt auf dem Boden zu bleiben.)

Ablauf erneut bestaetigt: "Baue erst einmal fertig, nutze diese und baue das Stück für Stück, damit nicht zu viel
Tokens verbrannt werden, wenn das Limit erreicht ist."

## Nachtrag 2 des Nutzers (woertlich, 2026-10-18 09:48, Antwort auf die Hinweise der Spur A Stimmen)

- "er Port kann die Stimme ruhig lauter anheben das es passt." (Irons' Zeilen ROOM1150 main18-21, -47 dBFS)
- "Du hast recht, das SIR, sir! habe ich vergessen und jetzt mit main22 ergänzt." (synchro/STAGE1/room1150/main22.wav,
  Datei vom 2026-10-18 09:45 = Zeile msg 29 "Leon: SIR, Sir?!")
- "Ändere es zu unwanted followers im Text bei 11B0."
- "Zu 1171, passt."

Orchestrator (Nachbesserung 1 der Spur A): (1) Port-Anhebung von Irons' Dateien ueber den 4x-Deckel hinaus =
NUTZER-VORGABE; "dass es passt" ist zu MESSEN (Bezug: Leons Zeilen derselben Szene / BGM MAIN1E), keine geratene Zahl.
(2) Tabelle: ROOM1150 msg 29 <- main22.wav (Nutzer legt weiter mit der naechsten freien Nummer ab). (3) ROOM11B0 msg 21
lautet schon "I just want to make sure / we don't have any unwanted followers." (tools/r36_j/szene11b0_bauen.py:286,
gen/szl11b0_szene.inc:134) - Text und Aufnahme stimmen ueberein, nichts zu aendern; dem Nutzer gemeldet. (4) erledigt.

## Nachtrag 3 des Nutzers (woertlich, 2026-10-18 14:25, Antworten auf die Entscheidungsfragen der Wellen 1/2)

- "Zu 1. ok, wenn Resident Evil 2 das so macht, lassen wir das so." (Spur D: Sturz-Aufsteher bis Clip-Bild 55 nur an den
  Beinen treffbar = RE2 byte-true -> bleibt)
- "Zu 2. ja, wir machen das Gorilla Verhalten generell so." (Spur E: HP x2 + Flinch erst nach 3 Handfeuerwaffen-Treffern
  fuer ALLE Gorillas Typ 0x27, nicht nur ROOM11C0 -> Nachbesserung 2 der Spur E)
- "Zu 3. Ja, dann belasse es so." (Spur F: EKG unter Gift = RE1.5-Welle in RE2-POISON-Farbe -> bleibt)
- "Zu 4. 15 Schuss - die Halbierung." (Spur C: Spind ROOM1000 = 15 Schuss -> bleibt)
- "Zu 5. Ja, alles was ein Zahlenammount hat, soll stapelbar sein, auch in der Kiste." (Spur C: Granaten stapeln; die Kiste
  nutzt dieselbe Regel re15_inventar_r39_stapelt, re15_itembox.c:102 -> erfuellt, nichts zu tun)

## Nachtrag 4 des Nutzers (woertlich, 2026-10-18 19:25, ROOM1141 - fuer Spur J)

- Bei ROOM 1141 soll Elza das Rolltor öffnen und dann noch ganz normal laufen können.
- Erst wenn sie danach die Tür entriegelt soll die Cutscene losgehen mit John. Am besten rennt sie dann bis zur Szene mit den anderen Kamera, die nicht mehr Richtung Tür zeigt, dreht sich dann zur Tür und hebt die Waffe. Dann kommt alles das, was jetzt schon drin ist. Tür knallen, sicht auf John usw.
- John sollte auch ab und an mal auf die Gorillas schießen in ROOM 1141 und dann ihre Aufmerksamkeit erhalten, sonst ist es komisch
- John sollte am Ende der Cutscene in ROOM 1141, bevor sie den Raum verlassen nicht die Waffe auf Elza richten, sondern das heranwinken/Folge Animation abspielen stattdessen....
- (Nachschub 19:25) "Oder falls Elza keine Waffe ausgerüstet hat eben das Messer...." (zu Punkt 2: hebt die Waffe - ohne ausgeruestete Waffe das Messer)
- (Nachschub 19:25) "Ich meine mit ausgerüstet - keine Waffe im Inventar hat" -> Regel fuer Punkt 2: hat Elza eine Schusswaffe im INVENTAR, hebt sie diese (die ausgeruestete; ist keine ausgeruestet, eine aus dem Inventar), auch wenn sie nicht ausgeruestet ist; nur ohne jede Schusswaffe im Inventar hebt sie das Messer.
- (Nachschub 19:26) "Falls sie irgendeine Waffe im Inventar hat, hebt sie halt diese."

## Nachtrag 5 des Nutzers (woertlich, 2026-10-18 19:29)

- Und die Schlüssel im Communicationsroom sind immer noch zu groß, die sollten maximal halb so groß sein.
  (Orchestrator: Spur L Punkt 2 hatte die Schluessel 10F0/10F1 auf RE2-Bildgroesse gebracht - Leon 69 px, Elza 63 px bei
  960x720, Hebel RE2_QUANTIL in tools/r38_itemorte/itemorte_export.py. NUTZER-VORGABE jetzt: hoechstens halb so gross wie
  dieser Stand (Faktor 0,5 linear) -> neue Spur M "schluessel2".)

## Nachtrag 6 des Nutzers (woertlich, 19:46)
- Antwort auf die Rueckfragen der Spur I: "1. ok, zu 2. nein, korrekt soll es geschrieben werden." -> Funkszene: "We need help!",
  "Uhh, this thing is broken!" (umgesetzt). Orchestrator: dieselbe Regel fuer neue Nutzer-Zeilen der Spur J ("He is in
  trouble", "gorilla meal") - korrekte Gross-/Kleinschreibung.

## Nachtrag 7 des Nutzers (woertlich, 2026-10-18 20:28, fuer Spur J)
- "Nein, die Animation beim follow me in room 1141 von John ist schon wieder falsch... die soll so aussehen wie in room 1171, die du jetzt korrigiert hast..."
  Orchestrator (Belege aus Spur K, Zweig r39/heliport1171, Dossier K_heliport1171.md Abschnitte 1/2, Commits 18940e95 + ab223800):
  das korrigierte Heranwinken in 1171 ("Elza, hurry up!") ist Original Plc_motion(0,2,0) @0x014CE = RBJ-Record 1 (Marker 0x2 =
  Gegner 0 = John) Clip 2, 96 Bilder, aus ROOM1171.RDT (RBJ @RDT+0x5C -> @0x188C, 0x409C B; rec1 EDD @0x3E34, 3 Clips {20,30,96});
  Lader FUN_8001b3f8 (RBJ = Raumkopf+0x5C, lw a2,92(v0) @0x8001b404). Der Port zeigte statt dessen Bank 0 Clip 2 = Kletter-Clip
  (RE15_NPK_KANTE_CLIP_HOCH), weil der RBJ-Kanal-Cache (RE15_RBJ_CACHE_MAX 2, enemy_common.c) von den Kraehen belegt war (Fix K
  18940e95: Slot ohne Record belegt keinen Platz). FUER 1141: Johns "follow me"-Heranwinken (Ende der Szene, Nutzer-Punkt 8 /
  Nachtrag 4 Punkt 4) = GENAU dieser Clip (ROOM1171 RBJ rec1 Clip 2), in 1141 aus ROOM1171 geliehen (Muster Spur I: RBJ-Leihe
  "[rbj] ... von ROOM1031 geliehen", platform/pc/src/cut10f0_pc.c); den Cache-Fix aus K beachten (bei der Integration zusammen).
  Riegel: Clip-Folge Johns bei "follow me" = 1171-Clip (Bildzahl 96, Quelle ROOM1171), kein Waffe-auf-Elza-Clip.

## Nachtrag 8 des Nutzers (woertlich, 2026-10-18 22:05)
- "So, ich habe jetzt die voiceovers ergänzt die fehlen in room 1141: Oh, no, this is Roy! He is in Trouble, I have to help him! ist main 12, Puuuh, that was close! I thought we end up as Gorilla meal…. ist main13 und Elza, follow me, I think I might have found a way out of here! ist main14."
- "Außerdem habe ich die voiceovers für ROOM 10F1 ergänzt, mit main06 und main08."
  Orchestrator (selbst gemessen 22:10-22:16, Windows-Spracherkennung System.Speech en-US: Diktat + Zwangswahl unter den
  Raum-Saetzen; Skript scratchpad/stimmen_n8/erkenne.ps1). Dateien unveraendert uebernommen (32 kHz, 16 bit, mono):
  room1141/main12 4,97 s (21:43), main13 5,83 s (21:55), main14 4,28 s (21:59); room10F1/main06 4,46 s (21:23),
  main08 2,82 s (21:40); Roh-Takes synchro/unused/STAGE1/room1141 + room10F1.
  | Datei | Diktat | Zwangswahl | Zeile im Port |
  |---|---|---|---|
  | 1141/main12 | "All know this is rolling he's in trouble I have to help in" | "Oh, no, this is ROY! ..." 0,823 (die John-Fassung nicht unter den Alternativen) | ROOM1141 msg 50 (Spur J, tools/r39_dining/programme_bauen.py:524 "Elza: Oh, no, this is John! / He is in trouble, I have to help him!") |
  | 1141/main13 | "Path that was coals are that we end up as goal of nearly" | - | ROOM1141 msg 51 (:527 "John: Puuuh, that was close! I thought / we end up as gorilla meal....") |
  | 1141/main14 | "Goals at fall league and armada fouled away all of you" | "Elza, follow me, I think I might have found a way out of here!" 0,664 | ROOM1141 msg 52 (:530) |
  | 10F1/main06 | "Carlyle anyone can hear me we need help" | "Hello, anyone, can you hear me? We need help!" 0,772 | ROOM10F1 msg 40 (Spur I) |
  | 10F1/main08 | "At this thing is broken" | - | ROOM10F1 msg 42 (Spur I; msg 41 "..." bleibt ohne Stimme) |
  - main12 SAGT "Roy", der Text sagt "John" (Nutzer-Entscheid Nachtrag 1, 1141 = John). Der Text bleibt John; der Nutzer ist
    informiert. Eine neue Aufnahme unter DEMSELBEN Dateinamen braucht keine Code-Aenderung.
  - KOLLISION: ROOM1141.RDT (msg_sec 0x10E4, re15_port/tools/rdt_msgdump.py) hat Original-Zeilen id 12 @0x13B6 "You've opened
    the lock.", id 13 @0x13D1 "A beautiful painting.There's a switch below. / Will you push it?", id 14 @0x1417 "It's a big
    table." Nach der Ordnerregel (synchro/README.md: main<MSG-ID>) spielten main12/13/14 dort - beim Entriegeln, am Gemaelde,
    am Tisch. Sie muessen stumm bleiben. Die Tabelle engine/src/stimmen_r39.c (Spur A) sperrt das von selbst: eine Zeile
    (Raum, msg, datei) macht die gleichnamige msg <datei> stumm (re15_stimmen_r39_stamm, zweite Schleife, "[voice] ... stumm").
    ROOM10F1 hat Original-Ids 0..5 (I_funk10f1.md: msg_sec @0x1168, 6 Stueck) - keine Kollision.
  - Zuteilung: neue Spur N "stimmen3" (10F1 msg 40/42 <- main06/main08 + 1141-Zeilen 50/51/52 <- main12/13/14, die 12/13/14
    sperren; Riegel; Messung am echten Weg). Spur J (Nachbesserung 2): dieselben drei 1141-Zeilen in stimmen_r39.c eintragen
    (bei der Integration gilt die Fassung der Spur N) und die Wiedergabe von msg 50/51/52 im neuen 1141-Ablauf messen.
