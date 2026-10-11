# v0.8.24 - 2026-10-11

Ihr Auftrag der Runde 36 ist gebaut. Jeder Punkt wurde von einem zweiten Agenten unabhaengig am gebauten
Spiel gegen Ihren Wortlaut nachgemessen; wo er nicht bestand, wurde nachgebessert und erneut abgenommen.
@@SUITE@@

## Was Sie merken

**Klettern.** Auf die verschiebbaren Tritte und Leitern kommt man wieder hinauf (ROOM10F0, 11A0, 2040, 2060,
20A0, 5020 und die Elza-Raeume). Sie waren im Port seit dem byte-genauen Klettern (August) nie kletterbar:
die Abfrage liess nur Objekte mit einem Bit zu, das diese Tritte nicht tragen; das Original nimmt jedes
Objekt (PSX.EXE @0x8002d358). Dazu kippen geschobene Kisten jetzt wie im Original ueber Kanten und fallen
herunter (ROOM5020/5021: die Kiste faellt auf den Boden, man kann auf sie klettern, der Raum merkt es sich).

**Leons Kleidung** zeigt bei Caution/Danger wieder Blut und Risse. Seit die RE2-KI Standard ist (August),
setzte kein Gegner mehr Wunden. Jetzt wieder: drei ueberlebte Zombie-Griffe auf dieselbe Seite,
Hundebisse (so viele Wundpunkte je verlorenem HP wie in RE1.5), Kraehen. Elzas Flecken sitzen jetzt auf
ihrer eigenen Kleidung.

**Schuesse.** Keine Waffe trifft mehr durch Tische, Baenke, Saeulen oder ueber den Konferenztisch, wenn
man davor steht (wie im Original). Fressende Zombies sind unverwundbar, bis sie aufstehen (RE2); sie wachen
auf, wenn man naeher als etwa 4 m kommt. Das Blut sitzt an der Trefferstelle (Brust, Knie, Hals beim
Kriecher). Ihre Frage zur Super Redhawk: ja, RE2 laesst Kraehen beim Magnum-Treffer genauso zerplatzen
(13 Teile, danach weg).

**Granaten.** Die normale Handgranate (auch Granatwerfer-Sprengrunde und Rakete) setzt Zombies nicht
mehr in Brand - sie zerplatzen nur. Feuer gibt es nur bei der Brandgranate. Jeder Druck auf den Abzug
wirft jetzt eine Granate (vorher ging sie verloren, wenn R1 zu frueh losgelassen wurde). Verbraucht wird
sie erst, wenn sie wirklich fliegt: wird Leon in der Wurfbewegung gepackt, bleibt sie im Inventar.

**20 Gegner je Raum** wie im Original (vorher 15): in ROOM1030 und ROOM1040 erscheinen die letzten fuenf
Zombies jetzt.

**Inventar.** Speichern verbraucht eine Memory Card (die letzte verschwindet; Abbrechen behaelt sie, wie
in RE2). Die letzte Hand-, Saeure- oder Brandgranate verschwindet nach dem Wurf aus dem Inventar. Das
Debug-Menue springt mit Elza in ihre Raeume (z.B. ROOM1071, ROOM1151).

**Leon, ROOM11B0.** Marvin schiebt den Container mit Leon zusammen weg. Danach: "Hold on." - Marvin laeuft
zur Werkstatt-Tuer, macht sie zu (Tuer-zu-Ton), schliesst sie ab wie Robert Kendo im Waffenladen (RE2-Klick)
und kommt zurueck: "I just want to make sure we don't have any unwanted followers." Die Tuer bleibt von
beiden Seiten zu ("The door is locked."). Die Parkplatz-Tuer schliesst er nicht ab.
**ROOM10F0.** Die Kamera bleibt auf Leon, wenn Marvin und Ada gehen; zum Schluss nur der Tuer-zu-Ton.
**Irons** bewegt nach seinem Tod den Kopf nicht mehr hinter Leon her.

**Irons' Tod / Montage.** Der Tuerknall ist jetzt ein echter Knall aus RE2 (ROOM106 "Zombies breaking in",
ein harter Schlag gegen die Tuer). Reihenfolge je Schritt: erst der Knall, dann sind die Zombies da, dann
erst die Kamera auf sie. In ROOM11C0 sprechen Ada und Marvin miteinander (zugewandt, Blick zum Gebaeude bei
"Oh, no, Leon!").

**Schluessel.** Leon: die Tuer ROOM11B0 -> 11C0 (Parkplatz) ist verschlossen ("The door is marked
"Parking Lot."") bis zum Parking Lot Key; er liegt im Funkraum ROOM10F0 auf der Konsole links neben der Tuer
von 10D0 (RE2-Modell "P. Room Key"). Elza: der Prison Key liegt an derselben Stelle in ROOM10F1 (RE2-Modell
"Cabin Key"), nicht mehr bei Roy; nach der Befreiung fragt das Spiel, ob Sie ihn wegwerfen wollen.

**Elza.** ROOM1121: das Fenster ist von Anfang an kaputt, eine Kraehe mehr. ROOM1151: Irons liegt tot da
("He has already passed..."). ROOM1211: Aufschliess-Ton beim Befreien. ROOM1021: zwei Schuesse und Roys
Todeslaut. ROOM1011: Ihre Aufnahme main16.5 (Roys Stoehnen) laeuft, wenn Roy zusammenbricht - die Szene
liegt in ROOM1011, nicht 1101.

**Story (Leon und Elza begegnen sich nie).** Roy liegt in Leons ROOM1020 erst, wenn der Strom an ist.
Leon findet in ROOM1210 die offene Zelle ("The cell door is open... / Whoever was locked up here is gone."),
Elza in ROOM1091 den ausgebrannten Schuppen ("It's a burnt-out shed. / Someone must have put out the fire.").
Elf Textzeilen, die eine Begegnung andeuteten, sind umgeschrieben (Tabelle unten).

**Karte.** Im Treppenhaus ROOM10A0 sind auf B1 die Treppen eingezeichnet. In ROOM11D0 steht der Marker nach der
Tuer am Eingang, nicht in der Mitte. Der Raum mit Hunde-Luke und Weste ist ROOM1190 (Schiessstand B1), nicht
ROOM1090: Halle und Westen-Kammer sind zwei getrennte Kaesten, jeder leuchtet nur, wenn man drin ist. Die Tuer
von ROOM11C0 liegt waagerecht auf der Wand. Die kleinen Raeume von ROOM1110 und die Fahrstuhlkabine gelten erst
als besucht, wenn man drin war (Fahrstuhl 1F -> 3F: nur 1F und 3F). Dafuer musste ROOM11A0 (Leiter zur
Kanalisation) auf das Blatt "DRAINS B2" umziehen - auf B1 gibt es fuer ihn kein Rechteck. Alte Spielstaende zeigen
diese Teilbereiche erst wieder, wenn man sie erneut betritt.
@@WEITERE@@

## Zur Freigabe: umgeschriebene Texte

| Raum | alt | neu |
|---|---|---|
| 30C1 (Elza) | Elza: Leon?! How is he? / We got separated back at the RPD. | Elza: Leon? I don't know him. / I never ran into him at the RPD. |
| 30C1 (Elza) | Marvin: He's all right and he's helping / me and Ada escape the city. | Marvin: He's a rookie, and he's helping / me and Ada escape the city. |
| 30C1 (Elza) | Elza: Will you tell him that me and John / and Sherry are here and we're all right? | Elza: Will you tell them that me and John / and Sherry are here and we're all right? |
| 6031 (Elza) | Elza: Where's Leon? | Elza: Sherry! Is anyone else here? |
| 6031 (Elza) | Sherry: He's fighting off monsters. / He wants you to start the train. | Sherry: A policeman! He's fighting off / monsters. He wants us to start the train. |
| 6030 (Leon) | Leon: Where's Elza? | Leon: Is anyone else with you? |
| 6030 (Leon) | Sherry: She's out there fighting / the monsters! | Sherry: Elza is out there fighting / the monsters! |
| 6030 (Leon) | Leon: Keeping the monsters at bay. / Can you start this thing? | Leon: I've never met her. / Can you start this thing? |
| 50B0 (Leon) | Ada: Your other friends are down there / with her too. | Ada: Some other survivors are / down there with her too. |
| 6000 (Leon) | Annette: You must be that cop. / Your friends are already here. | Annette: You must be that cop. / Your friend is already here. |
| 6000 (Leon) | Annette: Come on. Let's join your / friends and get out of here. | Annette: Come on. Let's join the others / and get out of here. |

Jede Zeile ist einzeln aenderbar (re15_port/tools/r36_p/texte_bauen.py). ROOM3060 msg 1-8 (Elza/Sherry/John im
Leon-RDT) blieb unveraendert: der Raum zeigt sie nie, Elza hat ein eigenes ROOM3061.

## Sprachdateien zum Aufnehmen

Ohne Datei laufen die Zeilen stumm mit Untertitel.
* synchro/STAGE1/room11b0/ main20.wav ("Hold on.") und main21.wav ("I just want to make sure / we don't have
  any unwanted followers.") - Marvin.
* synchro/STAGE1/room1210/main20.wav (Leon, offene Zelle) und synchro/STAGE1/room1091/main20.wav (Elza, Schuppen).

## Zu entscheiden

* ROOM1011: main16.5 ist ein einzelnes Stoehnen; der Untertitel lautet "Roy: groans" (wie Roys Original-Zeile).
  Welcher Wortlaut soll dort stehen?
* Die elf umgeschriebenen Textzeilen (Tabelle oben) und die zwei neuen Spuren-Texte. Ausserdem: ROOM50B1
  "It's Ada." - Elza kennt Ada nicht; so lassen oder z.B. "A woman... Could this be Marvin's 'Ada'?"
* Tuerknall: Alternative waere RE2 ROOM205 "Zombies smashing through wood" (Holz splittert) statt ROOM106.
* Texte an der Parkplatz-Tuer und zu den Schluesseln sind eigene Formulierungen in RE2-Form - bei Wunsch
  anderen Wortlaut nennen.
* Karte: Sie schrieben "ROOM 1090" fuer den Raum mit Hunde-Luke und Weste - gebaut ist es fuer ROOM1190
  (Schiessstand B1), ROOM1090 ist der Hinterhof. War 1190 gemeint?
* Karte: Wer das Treppenhaus ROOM1060 nur im Erdgeschoss betritt, kann schon zu 2F/3F blaettern (leer). Fuer den
  Fahrstuhl ist das abgestellt - soll es fuer Treppenhaeuser genauso gelten?
@@ENTSCHEIDEN@@

## Grenzen
* Gorillas ROOM11C0 (Spur F): @@F_GRENZE@@
* RE2-Hunde: der Claim, mit dem in RE2 nur ein Hund zugleich beisst (PL+0x1D3 |= 0x80, @0x801051f8/0x80105200,
  Pruefung @0x80104e5c-68), fehlt im Port - mehrere Hunde koennen in wenigen Bildern nacheinander zubeissen.
* Kleidung: die Wundstempel der Stages 3-5 (FUN_80112784 / FUN_8010de24 / FUN_8010dfa4) und Birkins eigene
  Stempel (STAGE5) sind weiter nicht portiert; die RE2-Kraehe kennt nur den Griff, nicht Dive/Strike.
* Fressende RE2-Zombies tropfen beim Fressen noch kein Blut (@0x80103C8C); die Feuer-Emitter der Brandgranate
  tragen einen Welt- statt Anker-Versatz (0,200,0). Der Kopfabriss des Kriechers wirft Fleischbrocken (Id 8) -
  in Raeumen ohne Raum-Id 5 nur das Hals-Blut.
* Der liegende Spawn-Zombie (EXEC[7]) ist weiter treffbar, bevor er aufsteht (RE2 sperrt ihn).
* Kisten: der Spieler faellt nicht von Objekten mit Bit 0x100 herunter (FUN_8002dc48, Substate 0xd) - er steht
  an der Kante. In ROOM2060 schiebt die Spinne Leon nach dem Aufstieg vom Tritt (y -1800), Original ungemessen.
* 20 Gegner je Raum: in ROOM20B0/20B1 schreibt das Original die Slots 20..34 hinter sein Feld; der Port
  verwirft sie. Das PSX-Ziel braucht 5 x sizeof(re15_actor_t) mehr RAM, ungemessen (kein PSn00bSDK).
* Elza ROOM1021: Roy sackt beim Schuss nicht sichtbar zusammen (Original noch nicht gemessen).
* Karte ROOM11D0: der Zwinger-Gang noerdlich der Tuerwand hat in der Kachel keinen Platz, der Marker klemmt
  dort an der Oberkante (Platzhalter-Kachel des Prototyps).
@@GRENZEN_REST@@
