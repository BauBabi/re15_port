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

## 2026-10-23 21:37 — Orchestrator-Entscheide zu den Fragen der Spuren B und C (Dossier-Stand beim Sitzungslimit; der Nutzer kann widersprechen)

Spur B (re2gator, §7):
- B1 Tempo: RE2-Werte bleiben (Schwimmen 24,5/Bild in der Biss-Sperre, Paddeln ~149/Bild, Zonen-Dashes) — Nutzer-
  Wortlaut "fast alles vom Aligator Fight von Resident Evil 2 uebernehmen ... an gleichen Stellen beschleunigt". Kein
  Deckel; Nutzer testet das Tempo im Becken und entscheidet dann.
- B2 Beschuss toetet nicht (RE2-Sockel 25300), Tod nur ueber den Kanister (Spur G), Grossreaktion 25 % je Treffer mit
  2-3 s Sperre wie RE2 — angenommen (deckt sich mit dem Nutzer-Design: Treffer-Schwelle -> Szene -> Kanister -> Tod).
- B3 Putz nur an den RE2-Stellen (Dash-Start), nicht bei jedem starken Schuetteln — angenommen ("ab und an").
- Fortsetzung: Baum ist auf 86661849 gesichert (UNGEPRUEFT); gezielte Tests zu Ende fahren, Dossier §4/§5 abschliessen,
  Abschluss-Commit feat(r41-re2gator).

Spur C (ventilkette, §7):
- C-a Hebelgewichte 30/20/40/10 (Loesung 1 + 3 + klein) — angenommen. C-b Bruecke oben = Ringebene y -5400 — angenommen.
- C-c Flut gilt auch beim Verlassen ueber die oberen Tueren — angenommen (Nutzer: "verlaesst man den Raum").
- C-d Lift-Rufzonen nach der Flut mit dem Original-Satz 1 — angenommen. C-e Ertrinken = Tod + blaue Blende + Spritzer,
  ohne eigenen Ton (RE2 hat keinen) — angenommen.
- Port-Texte 2000/2030/2050/2080 (Dossier §7) gelten bis zur Freigabe des Nutzers als Vorschlag; Wortlaut-Vorgaben
  des Nutzers unveraendert.

## 2026-10-23 22:32 — Orchestrator-Entscheid B4 (Fortsetzungs-Ergebnis der Spur B; der Nutzer kann widersprechen)

- B4 Kamera: Intro/Grossreaktion/Finisher/Kanister-Tod des Alligators laufen mit der Port-Todeskamera bzw. den 2090-
  Kameras, NICHT mit den RE2-Schnitten aus ROOM40A0 (Cut 0/1/5/6/13 gehoeren zum RE2-Korridor, nicht zum Becken).
  Spur G darf fuer die Kanister-Szene eigene Kamera-Programme (2080/2090-Kameras) anlegen. Nutzerfrage in der
  Abschlussmeldung: RE2-artige Kameraschnitte fuer die Explosion gewuenscht?
- Hinweis an Spur G (aus B §6): Zielhilfe FUN_8003703c = naechster Gegner rundum — solange die Spinnen im Becken sind,
  trifft man den Gator nur, wenn er naeher ist als die Spinnen; Spur F entfernt die Spinnen (Nutzer-Auftrag).

## 2026-10-23 22:37 — Nachtrag 4 (Wortlaut; ersetzt Entscheid B4, betrifft Spur B jetzt und Spur G in Welle 2)

> Ja, ich möchte RE2-artige Kameraschnitte

Orchestrator: Antwort auf die Frage "RE2-artige Kameraschnitte fuer die Explosion gewuenscht?" -> NUTZER-VORGABE. Spur B
baut das RE2-Kameraskript des Alligator-Kampfes nach: die Kameraschnitte von ROOM40A0 (Intro/Durchbruch, Grossreaktion
R1, Finisher, Kanister-Aufnahme, Schuss -> Explosion/Kiefer-Tod — Cut-Folge, Zeitpunkte und Dauer je Schnitt aus dem
RE2-SCD/Overlay mit Offsets belegen) und bildet sie auf die Kameras von ROOM2090/2091 ab (je RE2-Schnitt die 2090-Kamera
mit der entsprechenden Blickbeziehung: Gator frontal / seitlich / Nahaufnahme; Zuordnung im Dossier mit Bildbelegen,
PORT-WAHL der Zuordnung, Form = RE2). Pins: Schnittfolge + Bildnummern im Log je Phase. Spur G nutzt dieselben Programme
fuer die Kanister-Szene (Wurf aus 2080 -> Landung 2090 -> Aufnahme -> Explosion). Faellt die laufende Abnahme B ohne
diesen Punkt "bestanden" aus, folgt eine gesonderte Nachbesserung B mit diesem Mangel.

## 2026-10-23 23:06 — Orchestrator-Entscheid B5 (Abnahme B 0, Mangel M3) + Hinweis fuer Abnahme B 1

- B5 Gemeinsame Dateien: die kleinen, markierten Haken der Spur B in platform/pc/main.c (3), enemy_ai_re2_zombie.c (4
  Zeilen re15_re2z_gore_active fuer Typ 0x23) und re2_fx.c (+40 Zeilen Op 11/73) sind ANGENOMMEN (Vertrag §1.3 um B
  erweitert); Konflikte mit D/I/G loest die Integration. Kein Mangel mehr fuer Abnahme 1.
- Abnahme B 1 prueft zusaetzlich Nachtrag 4 (RE2-artige Kameraschnitte fuer Durchbruch/R1/Finisher/Kanister/Explosion,
  Zuordnung ROOM40A0-Schnitte -> 2090-Kameras mit Belegen). Fehlt das, ist die Spur NICHT bestanden.
