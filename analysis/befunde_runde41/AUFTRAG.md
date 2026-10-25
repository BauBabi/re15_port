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

## 2026-10-23 21:37 — Orchestrator-Entscheide zu den Fragen der Spur D (Bau-Ergebnis 4cbbf4a2; der Nutzer kann widersprechen)

- D1 Puls nur fuer Port-Gegenstaende (Feuerloescher 1030, spaeter der Schluessel 2090 der Spur F) — der Nutzer hat den
  Feuerloescher genannt; die Original-Item-Props der RDTs bleiben wie im Original (Nutzerfrage in der Abschlussmeldung).
- D2 Giftkugel: KEIN One-Save (RE2 FUN_800401d4) — Nutzer-Wortlaut "kann nicht sterben ... aber bereits auf Danger" =
  Danger + Treffer = Tod. Bleibt so.
- D3 VOLUME-Zeile im freien Streifen y 221 unter dem Options-Kasten (keine Original-Beschriftung angeschnitten) —
  angenommen; Nutzer sieht es im Spiel und kann die Lage aendern lassen.

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

## 2026-10-23 23:27 — Nachtrag 5 (Wortlaut; betrifft Spur G, Welle 2)

> Auch beim runterschmeißen des Gaskanisters - es gibt eine Szene in Resident Evil 2, wo Ada den Raketenwerfer runter
> wirft. GENAUSO würde ich das auch haben wollen von der Kamera.

Orchestrator: NUTZER-VORGABE fuer die Kanister-Szene der Spur G. Vorbild = die RE2-Szene, in der Ada den Raketenwerfer
von oben zu Leon hinunterwirft (Endkampf gegen den Tyrant; RE2-Raum per Zensus ermitteln: Item_aot_set des
Raketenwerfers / Ada-NPC + Wurf-Clip im SCD der Leon-Disc info/re2leon/PL0/RDT, Offsets belegen). Kameraschnitte,
Zeitpunkte, NPC-Clips (Werfen) und Flugbahn des Objekts dieser Szene auf 2080 (Marvin/John an der Reling) -> 2090
(Landung im Wasser) abbilden; Zuordnung der RE2-Schnitte auf 2080/2090-Kameras im Dossier mit Bildbelegen.

## 2026-10-24 03:21 — Orchestrator-Hinweise nach Abnahme E 0 (kein Nutzer-Wortlaut; betrifft Spur E und Spur M)

**Spur E, Punkt "Kurbel 2020" (Nachbesserung):** Abnahme 0 (`E_abnahme_0.md`) fand das "Herumfliegen" weder im Port
(vorher/nachher, vier Waffen) noch im Original-Video — ALLE Laeufe starteten aber im STAND bei (2600,-2800) und drueckten
die Aktionstaste aus dem Stillstand. Ungeprueft sind die Wege, auf denen ein Spieler normalerweise an die Kurbel kommt.
Vor jeder Rueckfrage an den Nutzer systematisch messen (Port; bei einem Befund das Original mit demselben Weg):
(a) Aktionstaste waehrend des GEHENS und des RENNENS in die Zone Slot 3 (`Aot_set` @0x009AE, rect (2200,-4150,800,800))
    hinein — was macht der Lauf-/Geh-Zustand waehrend `Message_on` @0x0A5C/@0x0A62 und in `Sleep 20` @0x0A98 zwischen
    `Pos_set` @0x0A8C (3650,-1800,-2925) / Gier `34 04 00 04` @0x0A94 und `Plc_motion` @0x0A9C?
(b) Zugang von allen vier Seiten der Zone und mit verschiedener Gier;
(c) mit gezogener Waffe (Zielhaltung) und direkt nach einem Schuss;
(d) `Work_set kind=1 idx=0` @0x0A88, danach `Work_set kind=3 idx=1` @0x0AA8 + `Speed_set 2f 01 33 00` @0x0AAC und die
    `For 6 { Add_speed }`-Schleifen @0x0AB0-0x0ACC: trifft Add_speed im Port wirklich Objekt 1 und nie den Spieler?
Jede Variante mit RE15_FRAMEDUMP (RE15_WINDOW_SCALE=1) + Bildbogen; ein gefundenes Fliegen am Original nachmessen
(DuckStation-Treiber `abn0_orig_kurbel.py` mit Anlauf statt Stand). Erst wenn alle Varianten original-gleich sind, bleibt
die Rueckfrage an den Nutzer (Weg/Bild).

**Spur M, Masken STAGE2 (Sichtpruefung):** Abnahme E Hinweis H2 (Bilder scratchpad `E41a0/zoom_legs.png` und
`E41a0/zoom_legs_nopri.png`): ROOM2020 Cut 3 — `MASKS/ROOM2020.MSK` (aus 5924939d) zeichnet die Reling ueber Leons Beine
an der Kurbel (Spielerlage nach `Pos_set` @0x0A8C = (3650,-1800,-2925)), obwohl Leon dort VOR der Reling steht; das
Original hat in Cut 3 keine PRI (RDT @0x3FC = ff ff ff ff). Tiefe/Ebene dieser Freistellung belegen und korrigieren
(Nutzer-Dateien unter pri/ nicht aendern — nur Zuordnung/Tiefe im Maskenbau), Vorher/Nachher-Bild genau an dieser Stelle,
und dieselbe Pruefung (Spielfigur an begehbaren Stellen VOR der Freistellung) fuer alle 53 neuen Masken.

## 2026-10-24 03:58 — Orchestrator-Entscheide B6-B8 (Fragen aus Nachbesserung B 1, c59802b5; der Nutzer kann widersprechen)

Massstab: Nutzer-Auftrag "RE2-Alligator-Finisher" + Nachtrag 4 "RE2-artige Kameraschnitte"; Regel Beta -> Retail (wo RE1.5
nichts vorgibt, gilt RE2).
- **B6 Finisher-Kamera = RE2.** Im Alligator-Finisher bleibt der Finisher-Schnitt (FRESSEN P0, Sperre + Cut @0x80100fbc-ff4,
  kein Ende) bis zum Game Over stehen; die RE1.5-Todeskamera (FUN_80015850: Weissblende, kreisende Kamera) wird fuer diesen
  Tod NICHT gezeigt. Die Game-Over-Folge danach bleibt die des Ports. Messung: Finisher-Lauf bis Game Over, Cut und Bild je 20
  Bilder, kein Kamerawechsel nach F658.
- **B7 Intro ohne Wanddurchdringung.** Der Kopf darf im Westkanal nicht ueber die Wand ragen (gemessen F160-F279 Cut 7,
  Kopf x -9574 / Wand x -8900). Form = RE2 (Clip 6 -> Clip 3, Knick yaw += 1024 @0x80100938-48 unveraendert); die
  Startlage/-gier des Intros in ROOM2090 wird so gewaehlt, dass der Knick auf Leons Angriffslinie endet (PORT-WAHL, im Dossier
  mit Rechnung). Messung: Kopf-x je Bild F160-F279 innerhalb der Wand, Bildbogen.
- **B8 Seitliche Sicht bleibt.** ROOM2090 hat keine frontale Zonenkamera in Explosions-/Finisher-Lage; neue Hintergruende
  werden nicht erfunden. Die Massstabsgrenze (groesster RE2-Schnitt) bleibt.
Umsetzung B6/B7 in der naechsten Nachbesserung der Spur B (oder, falls B vorher besteht, als eigener Auftrag vor Spur F).

## 2026-10-24 04:01 — Orchestrator-Entscheide nach Nachbesserung A 1 (7db5a6b2 / 0636c300; der Nutzer kann widersprechen)

- **A-E3 main.c-Haken der Spur A angenommen** (wie B5): Schatten-Bedingung Typ 0x24 und 4. Wert von RE15_PLAYER_POS = Band
  (Tuer-Landungsriegel). Kein Mangel.
- **A-E4 NPC-Schrittgeraeusche mit Lautstaerke und Richtung (Nachtrag 3 "Fussschritt-Sounds ... auch fuer die NPCs"):** die
  Abstands-/Seiten-Regel des Originals (FUN_80045a64, Aufruf @0x80045830) wird an re15_audio_footstep angebunden (Spur A;
  1-5-Zeilen-Haken in audio_pc.c, die Spur D ist integriert -> Konflikt bei der Integration aufloesen). Abnahmepunkt der
  Spur A: ein NPC-Schritt in 2000 ist leiser/seitlich je nach Abstand und Lage zur Kamera wie im Original (Werte je Abstand
  aus FUN_80045a64 mit Adressen, Messzeile je Schritt).
- **A-E5 RBJ-Binder (A OFFEN 9, Beleg A_belege/nb1d/rbj_a3_zensus.txt) gehoert Spur J.** Statisch bindet das Original
  (FUN_8001b3f8: a3 kumulativ ueber die Records, spaetere Records ueberschreiben, Spieler = letzter Record mit Bit 0) in 12
  Raeumen anders als der Port (rbj_resolve_slot / re15_emd_parse_rbj): 2000, 3091, 30E0, 4000, 4001, 4010, 5011, 6030,
  6031 (Spieler orig rec2, Port rec0; Gegner-Slot 1 orig rec2, Port ungebunden), 3060 (Gegner orig {0:1, 2:2}, Port
  {0:1, 1:2}), 1040/1041 (Spieler orig rec-1). Das ist sehr wahrscheinlich die Ursache der Nutzer-Punkte "3060 Marvins
  Animation" (Spur J) und "3091-Szene mit staendig falschen Animationen" (Spur K).
  * **Spur J** baut den Binder byte-true als ERSTEN Schritt (Disasm FUN_8001b3f8 selbst lesen, Adressen im Kopf), misst ihn
    zur Laufzeit in allen 12 Raeumen (welcher Record spielt je Slot, vorher/nachher) und committet ihn SOFORT als eigenen
    Commit mit `rbj-binder` in der ersten Zeile der Nachricht. Danach die eigenen Punkte (3060/3061).
  * **Spur K** aendert den Binder NICHT selbst. Sie beginnt mit den Punkten, die ihn nicht brauchen (30C1 Sherry Modell +
    Waffe, Analyse der 3091-Szene); sobald J den Commit hat (`git -C <ROOT>/.claude/worktrees/r41_stage3szenen_a log --oneline
    --grep rbj-binder`), uebernimmt K ihn UNVERAENDERT per `git cherry-pick <hash>` (bei der Integration loest git den
    gleichen Patch konfliktfrei) und misst 3091 darauf. Abweichungen am Binder meldet K im Dossier an J (nicht selbst aendern).
- Engine-weite Restpunkte der Spur A (OFFEN 1 ESP-Sprites im ersten Bild nach dem Kamerawechsel, OFFEN 2 RVD-Schaltzeitpunkt,
  OFFEN 3 Gegner-INIT-Takt, OFFEN 4 allgemeine Szenensperre AUTO/FLAG_CHG) bleiben fuer die naechste Runde (UEBERGABE).

## 2026-10-24 04:52 — Orchestrator-Auftraege nach Nachbesserung E 1 (kein Nutzer-Wortlaut; Spur E)

- **E-N1 Kamera-Flackern ROOM2020 (Nebenbefund der Nachbesserung E 1, Lauf g2):** steht der Spieler bei (9000,-3200), wechselt
  der Port JEDES Bild zwischen Cut 0 und Cut 2 (F1..F299); im Original-Video steht dieselbe Lage ruhig auf Cut 0. Das ist ein
  sichtbarer Port-Fehler in einem Raum der Spur E und wird von Spur E behoben: Mechanismus der Zonen-Umschaltung im Original
  belegen (RVD-Pruefung, Reihenfolge/Hysterese, Adressen), Ursache im Port finden, beheben, vorher/nachher an genau dieser
  Lage messen (Cut je Bild), dazu ein Riegel. Abnahmepunkt der Spur E.
- **E-N2 Geh-Stopp waehrend der Frage (Punkt Kurbel):** "Mechanismus nicht gefunden = NICHT fertig". Der Port wechselt beim
  Anlauf im Gehen zwischen Message_on 0 und 1 ein Bild lang auf Stehen 200, das Original-RAM zeigt waehrend der Frage noch
  Gehen. Den Geh-Stopp (Kommando 1, 0x80031de8) disassemblieren, das Original nach dem Ja im Geh-Fall messen (DuckStation,
  Ja-Eingabe zuverlaessig machen) und den Port angleichen oder mit Messung + Adresse als gleich belegen.

## 2026-10-24 05:25 — Orchestrator-Entscheide C-E1..C-E3 (Abnahme C 0; der Nutzer kann widersprechen)

- **C-E1 (Mangel M2, Abhaengigkeit von Spur A):** Der AUTO-AOT-Haken bleibt Code der Spur A (aot_common.c +
  re15_wasser20b0_r41_auto_aot); Spur C uebernimmt ihn NICHT (auch nicht per cherry-pick — er haengt an A's Modul). Spur C
  baut stattdessen den Ketten-Riegel `integration_r41_ventilkette_kette` (Ventile -> Ost-Tuer zu Fuss -> 20B0-Szene bis
  Szenen-Ende/letterbox zu und (3,235)=1 -> Leiter -> 2050) und registriert ihn in probes/r41_ventilkette.cmake NUR, wenn
  A's Modul im Baum liegt (`if(EXISTS ${...}/engine/src/wasser20b0_r41.c)`), sonst mit einer Statuszeile "uebersprungen: Spur A
  fehlt". Der Orchestrator misst die ganze Kette im Integrationsbaum A+C; M2 gilt fuer die Abnahme der Spur C als erfuellt,
  wenn der Riegel existiert, im C-Baum sauber uebersprungen wird und sein Ablauf im Dossier beschrieben ist.
- **C-E2 (Mangel M5, geflutetes 2030):** Bis der Rechner 2080 entleert hat (Bit 107), sind die Zugaenge ins geflutete 2030
  (20B0 Ost-Tuer, 2070 Slots 1/2, jede weitere Tuer in den Ring) gesperrt: Aktionstaste -> Port-Text, kein Raumwechsel.
  Id-Zuteilung (VERTRAG 1.2 ergaenzt): 20B0/20B1 Id 41, 2070/2071 Id 40 (Wortlaut englisch wie das Spiel, Vorschlag bis zur
  Freigabe durch den Nutzer). Nach Bit 107 wieder offen. Riegel: Sperre vor 107, offen nach 107, je Zugang.
- **C-E3 (Mangel M6, Vertrag):** Die Haken in platform/pc/main.c (4 Stellen) und game_step_common.c (1 Zeile) sowie die
  Querbezugs-Flags in test_r36_dachkiste.cmake / test_r37_itemband.cmake sind angenommen (wie B5), weil sie klein,
  markiert und durch die neue Sperre noetig sind. Kein Mangel mehr.

## 2026-10-24 07:49 — Orchestrator-Entscheid C-E2b (Revision von C-E2 nach Abnahme C 1, Mangel N1; der Nutzer kann widersprechen)

Befund der Abnahme C 1: Die in C-E2 vorausgesetzte Route "2050 -> 2020 -> 2030 oben -> 2080" existiert nicht (Nord- und
Suedgalerie von 2030 sind durch Wand 66 getrennt, SCA @0x880; die 2080-Tuer liegt auf der Suedgalerie und ist nur per
Lift B vom gefluteten Ring erreichbar). Nach der Flut steht der Ring unter Wasser (bis -16200) und ist nicht begehbar —
daran aendert sich nichts (Tod durch Ertrinken setzt Wasser ueber Kopfhoehe voraus).
Entscheid: Der Weg zum Rechner 2080 fuehrt nach der Flut ueber das Alligator-Becken: ... -> 20A0 -> Sprung ins Wasser
-> 2090 -> Tuer 2090 Slot 0 -> 2080 (passt zur Geschichte: Marvin/John wirft den Kanister von 2080 ueber das Gelaender
in das 2090-Becken). Spur C (Nachbesserung 2):
1. Tuergraph aller STAGE2-Raeume fuer den Zustand "nach der Flut" (Bits 103+100, ohne 107; Strom/Generator und
   Aufzug 2040 wie im Normalspiel zu diesem Zeitpunkt) aus den Door_aot_set-Zielen + Sperrbedingungen (SCD-Offsets)
   aufstellen und den Weg 20B0/2050 -> 2080 zu Fuss messen (Leon UND Elza), Riegel dazu. C-E2 (Ring-Sperre) bleibt.
2. Steht auf diesem Weg nur eine PORT-Sperre (ein Bit/Haken der Runde 41) im Weg, diese so anpassen, dass der Weg
   frei ist (mit Beleg). Steht eine ORIGINAL-Sperre im Weg oder gibt es den Weg nicht: NICHTS erfinden — den
   blockierenden Punkt (Tuer, Bit, SCD-Offset, Bild) im Dossier benennen und als OFFEN fuer den Orchestrator melden.
3. Dossier §7 und §0b (M5) richtigstellen.

## 2026-10-24 08:06 — Orchestrator-Entscheid B9 (Abnahme B 2, Mangel M1; der Nutzer kann widersprechen)

M1 der Abnahme B 2: die Intro-Startlage teleportiert den SICHTBAR lauernden Gator ueber ~16000 Einheiten. Form = RE2:
vor dem Intro ist der Gator NICHT sichtbar (RE2: Sce_em_set mit y = 3000 @RDT 0x25A0, also unter der Wasserlinie;
Pos_set erst im Intro @SCD 0x0736); er bricht aus dem Wasser. Fuer ROOM2090/2091 heisst das: bis zum Intro-Ausloeser kein
sichtbarer Gator (untergetaucht/unsichtbar wie RE2, Lage frei), dann Pos_set auf die Intro-Startlage nach B7. Passt zur
neuen Szene der Spur F ("der Alligator springt aus dem Wasser"). Riegel: im natuerlichen Ablauf (Lauf N der Abnahme)
kein Bild mit sichtbarem Gator vor dem Durchbruch und kein sichtbarer Lagesprung. Weitere Maengel M2/M3 der Abnahme B 2
wie dort gefordert (RE2-Form, Adressen dort).

## 2026-10-24 09:00 — Orchestrator-Entscheid C-E2c (Frage (j) der Nachbesserung C 2; der Nutzer kann widersprechen)

Befund der Nachbesserung C 2 (Tuergraph C_belege/nb2_tuergraph_nach_flut.txt, zu Fuss gemessen): nach der Flut gibt es
KEINEN Weg zum Rechner 2080. Die 2080-Tuer von 2030 liegt auf der Suedgalerie (nur per Lift B vom gefluteten Ring), und der
Umweg 2040 -> 2070 -> 11A0 -> 20A0 -> 2090 -> 2080 endet an der ORIGINAL-Sperre ROOM11A0/11A1 Slot 4 (Ck(3,206)=0
@0x00FEA -> msg 2 "It's locked from the other side." @0x00FEE; (3,206)=1 setzt nur 20A0/20A1 sub10 @0x01FF0). Die
Reihenfolge der Kette ist durch den Nutzer-Auftrag erzwungen (Leiter 2000 bis zur Flut gesperrt -> 2050/2020/2060/Strom erst
NACH der Flut; beide 2030-Lifte haengen am Generator 2060), also kann der Ring vor der Flut nicht der Weg sein.
Entscheid j1: Mit dem Ende der Flut (Bits 103 + 100) wird (3,206) = 1 gesetzt — die 11A0/11A1-Tuer nach 20A0 ist danach
offen (PORT-WAHL als Folge der Port-Flut; kein neuer Text, die Tuer oeffnet wie im Original nach sub10). Weg nach der Flut:
20B0 -> Leiter -> 2050 -> 2020 -> 2030 Nordgalerie -> 2060 -> 2040 -> 2070 Nordgang -> 11A0 -> 20A0 -> Sprung ins Wasser ->
2090 -> Tuer Slot 0 -> 2080 -> Rechner (Bit 107) -> 2030 entleert. Passt zur Geschichte der Spuren F/G (2090-Szene mit dem
Alligator, dann 2080 mit Marvin/John, der Kanister fliegt zurueck ins 2090-Becken). j2 (Waten im gefluteten Ring) und j3
(Flut erst nach 2080) verworfen: j2 widerspricht dem Ertrinken (Pegel 1800 ueber dem Ring), j3 widerspricht der vom Nutzer
verlangten Leiter-Sperre bis zur Flut.
Spur C setzt j1 um (Riegel: (3,206) vor der Flut 0, danach 1; zu Fuss gemessener Weg 11A0 -> 20A0 -> 2090 -> 2080 fuer Leon
UND Elza, Alligator dabei per Testhaken ruhig/entfernt), Dossier §7 mit dem Weg fuer den Nutzer.

## 2026-10-24 09:17 — Nachtrag 6 (Wortlaut; widerruft Entscheid C-E2c, betrifft Spur C und die Alligator-Spuren B/F/G)

> Nein, du darfst keine zusätzliche Tür öffnen um die Kette zu lösen. Dann passt die Rätselkette so nicht, und wir müssen
> sie anders machen.
>
> Betreff Aligator darfst du alles machen was du willst. Du darfst auch die Startposition des Aligators und die Position der
> schwimmenden Zombie Leiche, die sichtbar den Schlüssel hat ändern, wenn das Inszenatorisch besser aussieht. Wichtig ist -
> so stelle ich mir das vor - Leon kommt im Raum an, sieht die Zombie Leiche mit dem Schlüssel in der Cutscene. Dann kommen
> oben in 2080 die Zombies rein. Dann erneut Kamera auf Zombie mit Schlüssel. Aligator kommt frisst den Zombie samt
> Schlüssel. Macht danach Richtung Leon eine schrei und Schüttel Animation. Danach geht der Kampf los, mit der Kampfmusik
> von Resident Evil 2.

Folgen (Orchestrator):
- **C-E2c ist WIDERRUFEN** (keine zusaetzliche Tuer; die Original-Sperre 11A0/11A1 Slot 4 bleibt). Die Kette nach der Flut
  wird neu entworfen — Vorschlag an den Nutzer, Entscheid folgt als Nachtrag; bis dahin keine Umsetzung von j1.
- **Alligator (B/F/G):** freie Hand bei Startlage des Alligators und Lage der schwimmenden Zombie-Leiche mit dem Schluessel
  (Inszenierung). Szenenfolge der 2090-Ankunft (Spur F, mit Spur G): Leon kommt an -> Kamera auf die Zombie-Leiche mit dem
  blinkenden Schluessel -> oben in 2080 kommen die Zombies herein (die 8 Zombies der Spur G) -> erneut Kamera auf den Zombie
  mit dem Schluessel -> der Alligator kommt und frisst den Zombie samt Schluessel -> Schrei- und Schuettel-Animation in
  Richtung Leon -> Kampfbeginn mit der Kampfmusik aus Resident Evil 2 (RE2-Alligator-BGM aus ROOM40A0 belegen).

## 2026-10-24 09:22 — Nachtrag 7 (Wortlaut; Antwort auf die Frage "Wie soll 2030 nach der Flut wieder trocken werden?"; Spur C)

Frage des Orchestrators (gekuerzt): 2080 erreicht man nur ueber die 2030-Suedgalerie (Lift B vom Ring; beide Lifte brauchen
den Generator 2060) oder aus 2090; wegen der Leiter-Sperre kommen 2050/2020/2060 und damit der Strom erst nach der Flut.
Angeboten: "Pumpe in 2050" oder "2030 laeuft durch 20B0 leer". Antwort des Nutzers:

> Nach der Aktivierung des Stroms ist 2030 leer gelaufen.

Folgen (Orchestrator, Umsetzung Spur C):
- Ausloeser der Entleerung = Strom aktiviert (Generator 2060, Original-Ergebnisbit (3,0x7E) aus 2060; Setzer und Leser mit
  SCD-Offsets belegen). Ist die Flut gelaufen (Bits 103 + 100) und der Strom an, ist 2030 beim naechsten Betreten trocken
  (Zustand wie bisher nach Bit 107; Bit 107 wird mit dem Strom gesetzt, oder der Zustand haengt direkt an (3,0x7E) — Wahl mit
  Beleg im Dossier). Kein neuer Text noetig.
- Der Rechner in 2080 macht nur noch seine ORIGINAL-Aufgabe (Wasser in 2080 senken, Tuer nach 2090 frei, (3,245)); die
  Port-Erweiterung "2080_rechner_erw" und die Texte 2080 Id 40-41 entfallen (VERTRAG 1.2 entsprechend).
- Ring-Sperre C-E2 (20B0 Ost-Tuer, 2070 Slots 1/2) gilt nur, solange 2030 geflutet ist (Flut gelaufen UND Strom aus).
- Weg nach der Flut (zu Fuss messen, Leon UND Elza, Riegel): 20B0 -> Leiter -> 2050 -> (Kisten-Raetsel) -> 2020 -> Kurbel
  (3,219) -> 2030 Nordgalerie -> 2060 Generator -> Strom -> 2030 trocken -> Lift -> Ring -> Lift B -> Suedgalerie -> 2080
  -> Rechner -> 2090. Keine zusaetzliche Tuer, keine Original-Sperre umgehen.

## 2026-10-24 09:24 — Nachtrag 8 (Wortlaut; ergaenzt Nachtrag 7, Spur C)

> der Wasser pegel in 2030 kann dann noch auf reling höhe sein. also quasi nur noch der untere bereich geflutet nach strom
> aktivierungüber generator.

Folgen (Orchestrator, Umsetzung Spur C):
- Nach der Stromaktivierung (Generator 2060) sinkt das Wasser in 2030 NICHT ganz ab, sondern bleibt auf Relinghoehe stehen:
  nur der untere Bereich (Schachtgrund, Steg/Bruecke auf Band 1, Tank mit Ventil 13) bleibt geflutet, der Ring ist trocken
  und begehbar (Lift A/B, Ring-Tueren 20B0-Ost und 2070 Slots 1/2 wieder offen).
- "Relinghoehe" aus der Geometrie belegen (Gelaender-Zellen der SCA mit Offsets, z. B. Steg-Gelaender Zellen 3/4 und die
  Ring-Reling; Hoehe so, dass der Ring trocken bleibt und der untere Bereich unter Wasser steht) und mit Bildern aus den
  Kameras des Rings und der Draufsicht zeigen (vorher/nachher).
- Der geflutete untere Bereich ist nicht begehbar (kein Weg unter Wasser; z. B. die Treppe zum Tank gesperrt wie die
  Steg-Sperre vor dem Pult) — Umsetzung mit Beleg, Riegel. Gegenstaende im unteren Bereich (z. B. Schrotpatronen am Steg)
  sind nach der Flut nicht mehr erreichbar: im Dossier fuer den Nutzer nennen.

## 2026-10-24 09:26 — Nachtrag 9 (Wortlaut; Reihenfolge in 2030, Spur C)

> und die brücke in 2030 muss man entsprechd vorher runtergelassen haben, damit man das valve rätsel vorher lösen kann.

Folgen (Orchestrator, Umsetzung Spur C):
- Reihenfolge in 2030: Bruecke herunterlassen (Pult + Ventil 13, Bit 102) -> erst DANN ist das Ventilraetsel loesbar -> Flut.
- Wo liegen die nummerierten Ventile im Bild wirklich? Aus den Hintergruenden/Kameras von 2030 belegen (Bildbogen mit den
  Ventil-Schildern und ihrer Lage in der Kollision). Liegen sie im unteren Bereich (ueber den Steg/die Bruecke erreichbar),
  gehoert der Ausloeser des Ventilraetsels dorthin (dann ergibt sich die Reihenfolge aus dem Weg); liegen sie am Ring,
  sperrt Bit 102 das Ventilraetsel (Aktionstaste vorher: vorhandene/Port-Nachricht aus den Ids 2030 40-49, Wortlaut als
  Vorschlag fuer den Nutzer). Riegel: Ventilraetsel vor Bit 102 nicht ausloesbar, danach ja; Weg zu Fuss (Leon UND Elza).
- Nach der Flut liegen die Ventile ggf. im gefluteten unteren Bereich (Nachtrag 8) — das ist gewollt.

## 2026-10-24 09:55 — Nachtrag 10 (Nutzer-Entscheid per Auswahl; Sicherung vor der Flut, Spur C)

Frage des Orchestrators (Hinweis H-a der Abnahme C 2): Der Generator 2060 braucht die Sicherung ((3,144), 2060 sub19
@0x0169E; ohne sie msg 3 "One of the fuses is missing..."); die Sicherung liegt in 2030 im UNTEREN Bereich (Slot 9 @0x01BAA,
Steg-Suedlandeplatz, Band 1), der nach der Flut geflutet bleibt -> wer sie vor der Flut nicht aufhebt, bekommt nie Strom.
Angeboten: "Flut erst mit Sicherung" / "Sicherung wird nach 20B0 gespuelt" / "Sicherung woanders hinlegen".
**Gewaehlt vom Nutzer: "Flut erst mit Sicherung (Empfohlen)"** — Beschreibung der Option: "Die Ventile lassen sich erst
drehen, wenn die Sicherung aufgehoben ist (vorher ein kurzer Text am Ventil). Die Sicherung bleibt am Original-Platz; man
kommt nach Nachtrag 9 ohnehin ueber die Bruecke an ihr vorbei. Kein Softlock moeglich."

Folgen (Umsetzung Spur C): Bedingung des Ventilraetsels = Bruecke unten (Bit 102, Nachtrag 9) UND Sicherung aufgehoben
(Item-Taken-Bit der Sicherung aus dem RDT-Zensus belegen — nicht "im Inventar", denn sie kann schon in 2060 eingesetzt
sein). Vorher an den Ventilen ein kurzer Port-Text (Id aus 2030 40-49, Wortlaut als Vorschlag). Riegel: ohne Sicherung kein
Ventilraetsel, mit Sicherung ja; ganzer Ablauf zu Fuss (Leon UND Elza).

## 2026-10-24 10:50 — Orchestrator-Entscheide E-E1..E-E3 (Abnahme E 2; der Nutzer kann widersprechen)

- **E-E1 (Mangel 3, Vertrag):** Die spielweit wirkenden Haken der Spur E (Bildreihenfolge Spiel-Task @0x80020de4 vor
  Nachrichten-Takt @0x80020f3c/@0x80010044; Message_on kehrt zurueck @0x80040520, Freeze ueber @0x8003f040; Apply-Bild nicht
  praesentiert @0x80021558/@0x80021560) sind angenommen, weil sie das Original nachbilden — in scd_vm.c, main.c, render_pc.c,
  msg_common.c, player_common.c, re15_inv_screen.c je klein und markiert. Bedingung: Bei der Integration laufen alle
  Nachrichten-/Szenen-/Tuer-Riegel (msg, scd, cut, door, szene, karte) mit; Konflikte mit C/F/G (op_message_on) und D
  (render_pc.c, main.c) werden dort beidseitig geloest.
- **E-E2 (Maengel 1/2, Renn-Stopp):** Der Renn-Stopp des Originals (0x80032604 @0x8003272c/@0x80032730-760, 0x80032778
  @0x800328f8-95c) wird SPIELWEIT eingebaut (Stand 635be823 als Ausgang). unit_r36_zombieki_affen_block (Runde 36 Spur F,
  Seed 0x67be2dfd) ist neu einzumessen: erst belegen, WAS die Sonde misst und warum der Renn-Stopp den Wert aendert; die neue
  Schranke nur aus einer Messung mit Beleg, nicht "gruen gemacht". Der Riegel integration_r41_kleinkram_b_renn wird auf die
  Original-Lage nach dem Auslaufen umgestellt (Original o_renn2 n93-n99: 6 Bilder, Kurbeln bei (3650,-3369)).
- **E-E3 (Mangel 4, Softlock):** Im gehaltenen Apply-Bild laeuft der Blenden-Takt weiter (Original: @0x80020f44 vor dem
  Present @0x80020f4c) — re15_fade_tick auch im gehaltenen Bild; Riegel: Raumeintritt ROOM2020 auf der Kante (9000,-3200)
  blendet ein (pf-Rampe wie VORHER bis F10), Spieler steuerbar.

## 2026-10-24 12:42 — Nachtrag 11 (Nutzer-Entscheide per Auswahl, 11:4x; Spur M)

1. Frage: "ROOM20A0 Kamera 12, Datei 12_02, ist ein Bodenstreifen vor der Ziegelwand. Wer darauf stand, verlor im Spiel die
   Fuesse; sie wird deshalb vorerst nicht gebaut (deine PNG bleibt unveraendert, 12_01 bleibt aktiv). Was soll damit
   passieren?" — **Antwort: "Weglassen ist ok"** (12_02 bleibt aus dem Bau; Mangel 2 der Abnahme M 0 ist damit erledigt).
2. Frage: "Soll ich die alten _port.png in pri/ durch die berichtigten ersetzen?" — **Antwort: "Ja, ersetzen"**.
   Erledigt vom Orchestrator auf master 919cc1ed: 204 von 214 pri/STAGE3-6/*_port.png aus M_bilder/uebersicht/ ersetzt.
   Ohne berichtigtes Gegenstueck (Spur M liefert sie nach, Ersatz bei der Integration): pri/STAGE3/3040+3041/01,
   3050+3051/06+15, 30E0+30E1/06+07.

## 2026-10-24 12:43 — Orchestrator-Entscheide I-E1/I-E2 (Abnahme I 0; der Nutzer kann widersprechen)

- **I-E1 (V1, Vertrag):** Die Haken der Spur I in enemy_ai_common.c (Include + 2 Zeilen, RE1.5-Grid-Dispatcher
  0x80117910[4..6]) und player_common.c (1 Zeile, Pin-Freigabe der geteilten Phasenmaschine) sind angenommen (wie B5);
  bei der Integration mit A (enemy_ai_common.c) gegenlesen.
- **I-E2 (Hinweis H2 der Abnahme I 0):** Laden eines Spielstands IN ROOM2000/2001 (CONTINUE) zeigt keine Zombies (300 Bilder
  ohne Zombie-Aktor) — vorbestehend, aber genau die Raeume des Nutzer-Punkts der Spur I. Spur I behebt es (Ursache im
  Boot-/Lade-Pfad belegen, Riegel: Laden in 2000 und 2001 -> Zombies liegen wie beim Tuerweg).

## 2026-10-24 13:11 — Nachtrag 12 (Wortlaut; Antworten auf die Fragen (k) und (l) der Spur C)

Fragen (gekuerzt): (k) "Umgesetzt ist die Unterkante: Wasser bis zur Oberkante der unteren Reling (y -3600). Der Ring ist
trocken, Steg und Landeplaetze stehen 1800 unter Wasser. Oder meintest du direkt buendig an die Ring-Reling (y -5400)?"
(l) Zwei neue Texte zur Freigabe: "I cannot go down the stairs / because of the water." und "Numbered valves. If I turn
them, / this whole level will be flooded. | I should look around here first."

> (k) Wasserstand nach dem Strom:: unterhalb der Absperrung, sodass es nicht drüber "rein" laufen würde.
> zu (l) Zwei neue Texte zur Freigabe: Für den Moment ok, aber „Numbered valves. If I turn them, / this whole level will be
> flooded. | I should look around here first." würde ich später noch ändern wollen.

Folgen (Orchestrator, Spur C):
- (k) Der Ruhepegel nach dem Strom steht so hoch wie moeglich, aber UNTERHALB der Absperrung am inneren Ringrand, so dass
  kein Wasser auf den Ring laeuft (nicht buendig mit dem Ring). Die Absperrung (Bruestung/Gelaender am Ringrand, Hoehe und
  Art) aus der Geometrie belegen (SCA-Zellen mit Offsets + Hintergrundbild); ist sie ein durchsichtiges Gelaender, gilt die
  Ringboden-Kante (y -5400) als Grenze und der Pegel liegt knapp darunter. y -3600 bleibt nur, wenn die Geometrie zeigt, dass
  es keinen hoeheren Pegel unterhalb der Absperrung gibt. Bildbeleg vorher/nachher aus den Ring-Kameras.
- (l) Beide Texte sind vorerst freigegeben (2030 Id 48 Treppe, Id 49 Ventile). Den Ventil-Text (Id 49) will der Nutzer
  spaeter aendern — in den Release Notes unter "Zu entscheiden" fuehren.

## 2026-10-24 13:19 — Nachtrag 13 (Wortlaut; das echte Ventilraetsel in 2030, Spur C — Nutzer-Entscheid "Jetzt in v0.8.29")

> Der Text: „Numbered valves. If I turn them, / this whole level will be flooded. | I should look around here first." ist
> deshalb dämlich, weil sich die Frage stellt - woher weiß den der Spieler das, das dann alles flutet?
>
> Wenn denn würde ich beim Anklicken der Anlage oder bei einer Leiche oder so ein Dokument machen, wo so etwas drin steht wie:
>
> These amateurs—not only did they completely mess up the water ventilation, so that the water level not only rises up to
> the railing but also goes beyond it until it has completely drained out—but they also made a complete mess of the valve
> instructions.... They mixed up black, green, and red, and swapped the numbers for black! What are they? Colorblind? How
> is anyone supposed to work with something like this....
>
> und dann würde ich ein Plakat machen mit der Anleitung in welcher Reihenfolge mit welchen Zahlen man die valves drehen muss,
> nur eben genau falsch entsprechend dem Dokument.

Frage des Orchestrators: "Soll Spur C das jetzt noch fuer v0.8.29 bauen, oder kommt es in die naechste Runde?" —
**Antwort: "Jetzt in v0.8.29 (Empfohlen)"** (Option: Dokument mit dem Nutzer-Text bei einer Leiche am Steg-Suedlandeplatz,
wo die Sicherung liegt, oder beim Untersuchen der Anlage; Plakat als Nahaufnahme mit der falschen Reihenfolge; echtes
Dreh-Raetsel statt Platzhalter; Plakat-Inhalt und Loesung dem Nutzer zur Freigabe vorlegen).

Folgen (Orchestrator, Umsetzung Spur C):
1. Text 2030 Id 49 ersetzen: kein Wissen, das der Spieler nicht haben kann — neutral, z. B. nur "I should look around
   here first." (Gate Nachtrag 9/10 bleibt: Bruecke unten UND Sicherung aufgehoben).
2. Dokument: Inhalt = der Nutzer-Text oben WOERTLICH (nur Umbruch/Seiten fuer den Leser; Titel als Vorschlag). Fundort:
   Leiche am Steg-Suedlandeplatz neben der Sicherung (bevorzugt, weil der Spieler dort ohnehin vorbeikommt) oder beim
   Untersuchen der Ventil-Anlage — Wahl mit Begruendung. Dokument-System des Ports (re15_files, Ids ab 0x48), Item-/Taken-Bit
   nach RDT-Zensus (Memory reai-v2-bank9-taken-bits; Bit 107 ist seit Nachtrag 7 frei und gehoert C).
3. Plakat: Nahaufnahme (System der Runde 40 "nahaufnahmen") an einer Wand im unteren Bereich bei den Ventilen, mit der
   Anleitung (Reihenfolge Farbe + Nummer) — genau so falsch, wie das Dokument es beschreibt: Farben schwarz/gruen/rot
   vertauscht, die Nummern der schwarzen Ventile getauscht. Bild als Port-Asset (im Dossier und fuer das Paket-Gate nennen).
4. Raetsel: die echten Ventile (Nummern/Farben aus den 2030-Hintergruenden belegen, C_belege/nb3_ventile_lage_sheet.png)
   in der richtigen Reihenfolge drehen; falsche Reihenfolge -> zurueckgesetzt (Form wie die Original-Raetsel 2060/11F0 mit
   Beleg); die Loesung muss aus Plakat + Dokument + den sichtbaren Ventilen eindeutig ableitbar sein. Richtige Loesung ->
   Warnung -> Flut wie bisher.
5. Fuer den Nutzer (Dossier §7): Plakat-Bild, Dokument-Text, Loesung und Ableitung als Vorschlag zur Freigabe.
6. Nachrichten-Ids: 2030 Ids 40-49 sind C's; werden mehr gebraucht, 2030 Ids 50-55 (VERTRAG 1.2 ergaenzt).

## 2026-10-24 17:32 — Orchestrator-Entscheid E-E4 (Abnahme E 3, Mangel 2; der Nutzer kann widersprechen)

unit_r36_zombieki_affen_block darf nicht durch eine nachtraeglich gewaehlte Schranke gruen werden. Die Sonde misst den
Haenger (blockierte Gorilla-Bilder an einer Klemme je Seed) wieder wirksam; die zwei bekannten Haenger-Stellen (u. a. Klemme
(11129,9982), Seed 0x9b5bf362: 4405/6000 Bilder) werden als BENANNTE Ausnahmen mit Beleg gefuehrt. Ist die Ursache im
Port-Code (zombieki_r36.c, Runde-36-PORT-WAHL) belegbar und klein zu beheben, behebt Spur E sie in dieser Nachbesserung
(mit Riegel); sonst Folgeauftrag in UEBERGABE §3b. Mangel 1 (Renn-Stopp nach Treppe, Griff-Freigabe, Plc_ret: Original
loescht +5/+6 @0x80038ea8/@0x80038eb0 bzw. @0x80041f90-9c) behebt Spur E wie gefordert.

## 2026-10-24 17:36 — Nachtrag 14 (Nutzer-Antwort per Auswahl; Kurbel ROOM2020, Spur E)

Frage (gekuerzt): "Was meintest du mit 'herumfliegend'?" — Optionen: das Einfrieren (behoben) / den Sprung an die Kurbel
(Original macht ihn genauso, davor friert das Bild ~1/4 s ein) / etwas anderes. **Antwort des Nutzers:**

> Ja, direkt am Anfang springt Leon so komisch zu der Kurbel. So halb fliegend.

Folgen (Orchestrator, Umsetzung Spur E):
- Gemeint ist der Sprung an die Kurbel (Cut_chg 3 @0x0A7E, Pos_set @0x0A8C, Gier @0x0A94, Sleep 20, Plc_motion @0x0A9C).
  Im Original ist er nicht als "Fliegen" zu sehen, weil das Bild nach Cut_chg stehen bleibt, bis der Hintergrund von CD
  geladen ist (Apply-Zweig @0x80021558/@0x80021560, Lader FUN_80013c50; im Original-Video ~8 Bilder). Im Port dauert dieses
  Standbild 1 Bild -> der Uebergang (alte Lage/Pose -> Kurbel) wird sichtbar.
- Ziel: Port Bild fuer Bild wie das Original-Video vom "Yes" bis Plc_motion (Leon erscheint erst an der Kurbel, kein
  sichtbares Hinueberfliegen, keine sichtbare Ueberblendung von der alten Pose). Die Standbild-Dauer des Originals an
  genau dieser Stelle aus dem Original-Video bzw. DuckStation-RAM MESSEN (nicht schaetzen) und als gemessenen Wert mit
  Quelle (Video/Savestate, Bildnummern) einsetzen; pruefen, ob die Laenge vom Hintergrund-Laden abhaengt und ob sie fuer
  alle Cut_chg mit Hintergrundwechsel gilt (dann spielweit, mit den Riegeln der Spur E-E1). Riegel + Bildvergleich
  Port/Original (Gehen, Rennen, Stehen).

## 2026-10-24 17:38 — Orchestrator-Entscheid H-E1 (Abnahme H 0: beide Nutzer-Punkte erfuellt, nur Vertrag V1/V2)

- **V1 angenommen** (wie B5/I-E1): Include + Aufruf `re15_kollision_r41_npc_schwanz(slot)` in enemy_ai_common.c
  (re15_npc_ai_tick vor re15_npc_wall_clamp; Original-Reihenfolge aec4/b544 vor der Wandklemme @0x8011cc64).
- **V2 angenommen** als belegter Querbezug ("neue Regel widerlegt alten Riegel"): test_r39_dining1141_kampf.cmake
  `WARTE 60` -> `WARTE 240` ist durch die festen NPCs noetig (alt: vor-exe gruen / nach-exe rot nur ueber die Zufallsdeckung
  Salve/Griff; neu: nach-exe gruen) — Messung steht in H_abnahme_0.md. Die Nachbesserung H 1 dokumentiert beides im Dossier;
  sonst ist nichts zu tun. Die naechste Abnahme der Spur H bewertet V1/V2 nicht mehr als Mangel.

## 2026-10-24 19:01 — Nachtrag 15 (Nutzer-Entscheid per Auswahl: Ventilraetsel freigegeben, Spur C)

Vorgelegt (Nachbesserung C 4): echte Ventile Nordwand 03 rot / 02 gruen / 01 schwarz, Suedwand 09 rot / 08 gruen / 07
schwarz; Plakat (absichtlich falsch) "1. GREEN 09 - 2. RED 01 - 3. BLACK 02 - 4. RED 07"; Dokument "Technician's Note"
(Nutzer-Text woertlich) neben der Sicherung; Loesung 09 -> 07 -> 02 -> 01; falsche Folge -> "The valves turned back to
their original position."; Frage am Rad "Valve 09, red wheel. Will you turn it?". **Antwort: "Freigeben"** — Plakat,
Loesung, Titel und Texte bleiben so (Fragen (m) und (n) der Spur C damit beantwortet).

## 2026-10-24 19:03 — Orchestrator-Entscheid FG-E1 (Aufteilung der Spuren F und G nach Nachtrag 6; der Nutzer kann widersprechen)

F und G starten parallel auf dem integrierten Stand mit Spur B (r41/integration nach dem Merge von B). Damit sie sich nicht
gegenseitig ueberschreiben:
- **Spur F (gatorszene) besitzt die ganze 2090-Ankunftsszene** nach Nachtrag 6: Ankunft -> Kamera auf die Zombie-Leiche mit
  dem blinkenden Schluessel (Blinken wie der Feuerloescher, Spur D re15_blink) -> Einstellung "oben in 2080 kommen die Zombies
  herein" -> erneut die Leiche -> der Alligator kommt (aus dem Wasser, B9: vorher unsichtbar) und frisst den Zombie samt
  Schluessel -> Schrei- und Schuettel-Animation zu Leon (RE2-Clips aus Spur B) -> Kampfbeginn mit der RE2-Kampfmusik (aus
  ROOM40A0 belegen, Ton nach shared_assets/RE2/). Dazu: Spinnen aus 2090 entfernen; nach dem Tod liegt der blinkende
  Schluessel bei der Leiche. Die Zombies der 2080-Einstellung baut F als SZENEN-Figuren (nur fuer diese Einstellung); ist
  der obere 2080-Bereich aus 2090 nicht sichtbar, ist die Einstellung ein kurzer Schnitt mit eigener Kamera — Form und
  Belege im Dossier. F setzt Bit 110 (Szene gesehen) wie im VERTRAG.
- **Spur G (kanister2080) besitzt ROOM2080/2081**: nach Bit 110 beim Betreten Metalltuer-Ton + 8 Zombies im oberen
  Hauptbereich (dieselbe Zahl/Modelle wie F's Einstellung), Gaskanister an der Reling (Gas_Can.bmp, Modell aus Spur B
  shared_assets/RE2/KANISTER40A0.*), die Marvin/John-Szene nach genug Treffern auf den Alligator (Kopfschuss je Zombie, Kanister
  aufnehmen, "Hey, Leon/Elza, try with this!" — Stimmen synchro/STAGE2/room2080/main42.wav bzw. room2081/main42.wav, Wurf
  ueber die Reling mit Kamera wie Adas Raketenwerfer-Wurf in RE2, Nachtrag 5), dann Kanister im 2090-Wasser ueber Spur B's
  API (re15_gr2_kanister_setzen) -> Aufnahme/Explosion/Kiefer-Tod (Spur B, fertig).
- Gemeinsame Beruehrungspunkte nur ueber Bits/APIs (F: Bits 110-113, Text-Ids 2090/2091 42-46; G: Bits 114-119, Ids 2080/2081
  42-47), keine Aenderung im Modul der anderen Spur; Treffer-Zaehler fuer G's Ausloeser liest G aus B's API.

## 2026-10-24 19:31 — Nachtrag 16 (Wortlaut + Auswahl-Antwort; Ventil-Sicherungskasten in ROOM2010, Spur C — ersetzt Nachtrag 10)

> Das einzige was ich noch reichlich schlecht finde ist das der Charakter so etwas sagen muss wie - "Ich muss mich vorher
> noch umschauen" bis er die SIcherung genommen hat. So etwas gibt es im ganzen Spiel nicht und ist komisch.... Kannst du
> evtl. noch einen zweiten Sicherungskasten irgendwo einbauen - außerhalb des Raumes, wo man die Sicherung zuerst einsetzen
> muss. Macht man das geht die Kamera auf die Ventile und man hört ein unlock geräusch, das man sie dann drehen kann. Und
> dann kann man die Sicherung aus dem Kasten wieder raus nehmen und in den 2. Sicherungskasten packen? Irgendwie sowas? Das
> mach insgesamt viel mehr sinn....

Frage des Orchestrators: Wo soll der Kasten hin (angeboten: 2070 vor der Ring-Tuer / 2030 am Ring)? Antwort:

> Nein, weder 2070 noch 2030, denn man muss ja, nachdem man das Rätsel gelöst hat, und der Raum geflutet wurde die
> Sicherung heraus nehmen können, um danach damit zum anderen Generator zu gehen. Ist sie raus, sollen die valves wieder
> blockiert werden. Nein, ich glaube im ROOM 2010 wäre das vielleicht am günstigsten?

Folgen (Orchestrator, Umsetzung Spur C):
1. Neuer Ventil-Sicherungskasten in ROOM2010/2011 (Lage an einer passenden Wand aus den 2010-Hintergruenden belegen;
   Kasten-Modell/Bild wie der Original-Kasten in 2060, falls vorhanden — Beleg). 2010 ist vor UND nach der Flut ueber
   2000/20B0 Slot 2 erreichbar -> kein Softlock.
2. Sicherung einsetzen (Frage im Original-Stil, Sicherung verlaesst das Inventar) -> "Kamera auf die Ventile" + Unlock-
   Geraeusch (RE2/RE1.5-Ton mit Beleg): als kurze Einstellung der 2030-Ventil-Kamera (z. B. Hintergrund der Ventil-Kamera als
   Standbild wie die Nahaufnahmen der Runde 40, oder kurzer Raumwechsel hin und zurueck — Form mit Beleg/Begruendung im
   Dossier) -> Ventile in 2030 sind frei, solange die Sicherung im Kasten steckt.
3. Sicherung herausnehmen (jederzeit, auch nach der Flut) -> zurueck ins Inventar, Ventile wieder blockiert. Danach geht
   sie wie im Original in den Generator-Kasten 2060 ((3,144) sub19 @0x0169E).
4. Die Ventile ohne Sicherung im 2010-Kasten: neutraler Text OHNE erfundenes Wissen und OHNE "look around" (z. B. "The
   valve won't turn." / "It's locked." — Vorschlag). Der Gate-Text Id 49 und die Bedingung "Sicherung aufgehoben" aus
   Nachtrag 10 entfallen; Nachtrag 9 (Bruecke vorher unten) bleibt (die Ventile liegen im unteren Bereich).
5. Nachrichten-Ids: 2010/2011 Ids 40-44 fuer C (VERTRAG 1.2). Bits: aus C's Zuteilung (z. B. 107 ist mit dem Dokument
   belegt -> naechste freie nach Zensus, im VERTRAG nachtragen).
6. Riegel: ohne Kasten-Sicherung kein Ventil; mit ja; nach der Flut Herausnehmen moeglich und Generator 2060 erreichbar
   (ganzer Weg zu Fuss, Leon UND Elza).

## 2026-10-24 19:44 — Nachtrag 17 (Wortlaut; Ton beim Hereinkommen der Zombies in 2080, Spuren G und F)

Bezug (Orchestrator-Meldung): "Metalltuer-Ton und 8 Zombies" (Spur G, 2080).

> Muss natürlich ein knallen sein, als ob die Zombies rein brechen, so wie bei STAGE1 von Elza/Leon am Ende in der Cutscene
> nach Irons.

Folgen (Orchestrator): Kein Schliess-Ton, sondern der KNALL des Hereinbrechens, wie in der STAGE1-Endszene nach Irons
(Leon und Elza). Spur G (und Spur F fuer die Einstellung "oben in 2080 kommen die Zombies herein" in der 2090-Szene) belegt
die genaue Original-Stelle (Raum, SCD-Offset des Ton-Befehls / Szenen-Ereignis, Ton-Id bzw. VAG/VAB-Satz) und verwendet
denselben Ton (gleiche Datei/Stimme, Ton-Datei des Ports im Dossier fuer den Nutzer nennen). Beide nutzen denselben Ton.

## 2026-10-24 19:53 — Nachtrag 18 (Wortlaut; vier Punkte zur Ventilkette, Spur C)

> 4 Dinge:
>
> - Das Poster mit der Anleitung der Valves die gedreht werden müssen entspricht nicht dem Stil des restlichen Spieles wie
>   die Dinge dort aussehen. Das sieht er aus wie aus Monkey Island..... Wenn schon muss du den Stil des Rest des Spieles
>   damit treffen
> - in ROOM 2050 bei den neuen Rätsel mit den hebeln finde ich das mit den Lampen unten gut. ABER, bei den ausgewählten
>   hebel, muss dann auch der Heben runter gehen wenn er ausgewählt wurde.
> - Wenn an den valves gedreht wird in ROOM 2030, dann muss auch die valve dreh animation kommen. Die kannst du aus Resident
>   Evil 2 übernehmen.
> - Bei der Minute in ROOM 2030 bis der Raum flutet, muss es einen typischen Timer geben wie aus den Rest des Spiels, der dann
>   abläuft. Außerdem brauche ich den Text noch mal, damit ich den voiceover dafür aufnehmen kann. Also - Alarm, so wie am
>   Spiel Ende immer und dann das Voiceover beim Text.

Folgen (Orchestrator, Umsetzung Spur C in der naechsten Nachbesserung):
1. Plakat im Stil des Spiels: Vorbild sind die echten Nahaufnahmen/Dokumente des Spiels (RE1.5-Nahaufnahmen der Runde 40,
   RE2-Aushaenge/Dokumente aus info/re2leon bzw. extracted_re2_dokumente) — Papier/Textur, Schrift (Schreibmaschine/Druck wie
   die Spiel-Dokumente), Farben, Abnutzung; keine Comic-/Zeichentrick-Optik. Vorher/Nachher-Bild + Vergleich mit 2 echten
   Spiel-Nahaufnahmen im Dossier. Inhalt (Reihenfolge/Farben/Nummern) bleibt wie freigegeben (Nachtrag 15).
2. ROOM2050 Schaltpult: die Lampen bleiben; der ausgewaehlte/umgelegte Hebel muss sichtbar nach unten gehen (Hebel-Stellung
   je Zustand, Form wie die Original-Hebel 11F0/2060 mit Beleg; Grafik aus dem Hintergrund/den Original-Modellen ableiten).
3. ROOM2030 Ventile: beim Drehen spielt die Ventil-Dreh-Animation aus Resident Evil 2 (RE2-Spieler-Clip des Ventilrads,
   Raum/RBJ/EDD mit Offsets belegen, wie Spur B die RE2-Clips uebernommen hat; ggf. das Rad selbst mitdrehen).
4. ROOM2030 Countdown: die 60 s laufen als typischer Spiel-Timer ab (Anzeige wie der Countdown am Spielende — Original-
   Timer-UI belegen), dazu der Alarm wie am Spielende (Original-Ton belegen), und beim Warntext die Sprachausgabe des Nutzers.
   Datei-Platz fuer die Aufnahme des Nutzers: synchro/STAGE2/room2030/main46.wav (Elza: room2031/main46.wav; fehlt die Datei,
   kein Ton — der Port darf sie nicht erzeugen). Wortlaut des Textes Id 46 wird dem Nutzer genannt.

## 2026-10-24 19:55 — Nachtrag 19 (Wortlaut; Warntext 2030 Id 46, Spur C)

> Ändere den Text zu: WARNING: The water will rise in one minute. Please leave the room immediately!

Folgen (Orchestrator): Text 2030/2031 Id 46 = "WARNING: The water will rise in one minute. Please leave the room
immediately!" (Wortlaut exakt; nur der Zeilenumbruch nach der Breite des Nachrichtenfensters). Die Sprachaufnahme des Nutzers
kommt nach synchro/STAGE2/room2030/main46.wav bzw. room2031/main46.wav (Nachtrag 18 Punkt 4).

## 2026-10-24 20:34 — Orchestrator-Entscheid E-E5 (Fragen der Nachbesserung E 4; der Nutzer kann widersprechen)

- Der 2-Zeilen-Haken der Spur E in engine/src/game_step_common.c (vor der Zweig-Kette des Spieler-Dispatchers; Renn-Stopp
  endet, wenn ein Durchlauf keinen Kommando-1-Schritt macht — Original +5/+6 @0x8003272c/@0x800328fc, Ueberschreiber
  @0x80043544/@0x80043610/@0x80038ea8/@0x80041f90/@0x80041bb0/@0x80102640/@0x8011c38c) ist angenommen (Erweiterung von E-E1/E-E2).
- Die dritte benannte Ausnahme der Gorilla-Sonde (WESTBLOCK x 3800..6200 / z 9900..12500) ist bestaetigt, weil dort auch die
  Original-Regel haengt (AUS 5880 in 15 Seeds) — kein Rueckschritt der Spur E. Die drei Gorilla-Haenger (TASCHE, SUB7,
  WESTBLOCK; zombieki_r36.c, Runde-36-PORT-WAHL) sind Folgeauftrag (UEBERGABE §3b).

## 2026-10-24 21:41 — Nachtrag 20 (Wortlaut; Lage der Zombies in 2080, Spuren G und F)

> In 2080, nachdem die Zombies durch die Tür gebrochen sind, sollen sie oberhalb von der Treppe laufen, nicht unten.

Folgen (Orchestrator): Die 8 Zombies (Spur G, ROOM2080/2081) brechen durch die Tuer herein und laufen auf der OBEREN Ebene
(oberhalb der Treppe), nicht unten. Ebene/Band und Tuer aus der Geometrie belegen (SCA-Zellen/Baender, Tuer-Satz), Wege der
Zombies auf der oberen Ebene halten (kein Abstieg ueber die Treppe), Bildbeleg aus den 2080-Kameras. Spur F zeigt in der
2090-Einstellung "oben in 2080 kommen die Zombies herein" dieselbe Lage.

## 2026-10-24 22:25 — Nachtrag 21 (Wortlaut; Zugang zum Alligator und Zweck des Schluessels, Spuren F und G)

> Nur für den fall das du es falsch hast - man kommt über die Leiter zum Aligator, nicht über das Tor. Das Tor soll
> verschlossen sein, und dann mit den Schlüssel den man vom Aligator bekommt geöffnet werden!

Folgen (Orchestrator):
- Zugang zu ROOM2090/2091 im Spielablauf: ueber die LEITER aus 2080 (ROOM2080 Slot 1 @0x00BC2 -> 2090 (-8200,0,-25550), frei
  nach dem Original-Rechner (3,245)); die 2090-Ankunftsszene (Spur F) beginnt bei dieser Ankunft.
- Das TOR zwischen 2090 und 20A0 (2090 Slot 1 @0x009B6 <-> 20A0 Slot 0 @0x01746, Rolltor-Bereich 20A0) ist VERSCHLOSSEN
  und oeffnet sich erst mit dem Schluessel, den man vom Alligator bekommt (Spur F: Schluessel nach dem Tod blinkend bei der
  Leiche; Tuer-Sperre/Schluessel-Item in Original-Form mit Beleg, Item-/Taken-Bit nach Zensus, Bits aus F's Zuteilung
  110-113). Damit ist die offene Frage "welche Tuer oeffnet der 2090-Schluessel" beantwortet. Weg danach: 2090 -> Tor -> 20A0
  -> (Original sub10 (3,206)) -> 11A0 -> STAGE3.
- Spur G: die Kanister-Szene setzt voraus, dass der Spieler ueber die Leiter in 2090 ist (nicht ueber 20A0).

## 2026-10-24 23:10 — Orchestrator-Entscheid J-E1 (Abnahme J 0: alle Nutzer-Punkte erfuellt, nur Vertrag V1 + Doku M2)

- **V1 angenommen** (wie B5/I-E1/H-E1): Haken der Spur J in scd_vm.c (register_opcodes, s_op_table[0x48]/[0x49], 3 Zeilen)
  und platform/pc/main.c (pc_prop_world, 4 Zeilen), markiert "Runde 41 Spur J". Bei der Integration in register_opcodes mit
  C/E (op_message_on) und den uebrigen Haken beidseitig mergen.
- M2 (Dossier/Kopf von re15_superobj_r41.h zu ROOM4031: Gegner 0 beim ersten Besuch ist John, Typ 0x49) berichtigt die
  Nachbesserung J 1; sonst ist nichts zu tun.

## 2026-10-25 00:01 — Nachtrag 22 (Nutzer-Entscheide per Auswahl zu den Fragen der Spur G)

1. "Wann ist der Alligator 'oft genug angeschossen'?" — **"RE2-Schwelle (300 Schaden)"** (wie gebaut: RE2-Schwaeche
   @0x80101ebc-dc; Pistole 43 / Schrotflinte 11 / Magnum 7 / Granatwerfer 5 Treffer).
2. "Soll der Alligator nach der Kanister-Szene sofort wieder auftauchen?" — **"Erst beim Annaehern"** (wie gebaut).
3. "Soll beim Hereinkommen von Marvin/John ebenfalls der Knall kommen?" — **"Stahltuer-Ton lassen"** (DOOR06 wie gebaut).
Damit sind alle Entscheidungsfragen der Spur G beantwortet. Sprachaufnahmen fuer den Nutzer: synchro/STAGE2/room2080/main42.wav
("Hey, Leon, try with this!", Marvin) und synchro/STAGE2/room2081/main42.wav ("Hey, Elza, try with this!", John).

## 2026-10-25 00:22 — Nachtrag 23 (Wortlaut; Ablauf des Alligator-Kampfes mit der Kanister-Szene, Spuren G/F/B)

> Aber die Kanister Szene kommt erst nachdem der Aligator genug getroffen wurde. Aligator Kampf -> Mehrere Treffer ->
> Kanister Szene -> Kampf geht weiter, Aligator nimmt irgendwann in dem weiteren Rundlauf den Kanister auf -> Schuss -> Ende
> Kiefer kaputt.

Folgen (Orchestrator): Massgeblicher Ablauf (ergaenzt Nachtrag 22 Punkt 2 "Erst beim Annaehern" = der Alligator setzt nach
der Szene seinen normalen RE2-Ablauf fort, nicht sofort angreifend): Kampf -> Treffer bis zur RE2-Schwelle (300 Schaden) ->
Kanister-Szene (Marvin/John, Wurf ins 2090-Becken) -> Kampf geht weiter -> der Alligator nimmt den Kanister IRGENDWANN in
seinem weiteren Rundlauf auf (Aufnahme-Mechanik der Spur B, RE2-Form) -> Schuss auf den Kanister im Maul -> Explosion ->
Tod mit geplatztem Kiefer. Abnahmepunkt der Spur G: diese ganze Folge im ECHTEN Ablauf an der exe (Leon UND Elza), ohne
Testhaken fuer den Kanister — insbesondere: der Kanister liegt nach dem Wurf erreichbar auf dem Rundweg des Alligators
und wird im normalen Kampf tatsaechlich aufgenommen (Messung: Bild der Aufnahme, Zeit seit dem Wurf).

## 2026-10-25 00:23 — Nachtrag 24 (Wortlaut; Animation vor der Kanister-Aufnahme, Spur G) + Orchestrator-Entscheid FG-E2

> Ich sah die Animation vor dem aufnehmen des Kanisters ist noch falsch. Da hat er mal wieder eine dämliche Kletter
> Animation

Folgen (Orchestrator):
- Vor der Kanister-Aufnahme darf der Alligator KEINE Kletter-Animation (CROSS/GUARD-Kletterclip des Nutzer-Designs) zeigen.
  Form = RE2: er schwimmt zum Kanister (Sub 1 Schwimmen, Clip 0, Blend 32 @0x80100b04-18 / @0x80100c04, Stufen der
  T2-Aufnahme @0x801017c4-0x80101d40) und nimmt ihn auf. Ursache im Ablauf finden (liegt der geworfene Kanister so, dass der
  Weg ueber die Plattform/Rampe fuehrt? greift CROSS waehrend des Aufnahme-Fensters?) und belegt beheben; Bildbogen der
  Aufnahme vorher/nachher im echten Ablauf (Nachtrag 23), Leon UND Elza.
- **FG-E2:** Spur G darf dafuer gezielt in Spur B's Modulen (gator_re2_r41.c, enemy_ai_boss_gator.c) aendern — nur die
  Aufnahme-/Anschwimm-Logik, markiert "Runde 41 Spur G (Nachtrag 24)"; Spur F aendert dort nur die Szene/den Auftritt.
  Bei der Integration beidseitig mergen; die B-Riegel (unit_r41_re2gator_*, integration_r41_re2gator) bleiben gruen.

## 2026-10-25 00:24 — Nachtrag 24b (Wortlaut; BERICHTIGT Nachtrag 24 — es geht um Marvin/John, nicht um den Alligator)

> nein, nicht der aligator

Folgen (Orchestrator, ersetzt die Folgen von Nachtrag 24; FG-E2 ist WIDERRUFEN, keine Aenderung in Spur B's Modulen):
- Die "daemliche Kletter-Animation" vor dem Aufnehmen des Kanisters spielt **Marvin (Leon) bzw. John (Elza) in ROOM2080/2081**
  in der Kanister-Szene der Spur G (Punkt 3: "Rennt zu dem Gas Kanister, nimmt ihn auf in beide Haende"). Spur G belegt, welcher
  Clip dort laeuft (Bank/EDD/RBJ, Clip-Nummer, Ursache: z. B. falscher Clip-Index, Kanten-/Kletter-Sub des NPC-Gangs an der
  Reling, Stufe), und ersetzt ihn durch passende Animationen: Hinlaufen (Lauf-Clip des NPC) und Aufnehmen mit beiden Haenden
  (Original-/RE2-Clip mit Beleg — z. B. eine Aufheb-/Trage-Animation aus RE1.5-NPC- oder RE2-Banken; Herkunft im Dossier).
  Bildbogen der Szene vorher/nachher, Leon UND Elza.

Nachsatz des Nutzers zu 24b (Wortlaut): "marvin" — gesehen bei Marvin (Leons Szenario); John (Elza) gleich pruefen.

## 2026-10-25 00:56 — Orchestrator-Entscheide FG-E3/FG-E4 (Abnahme G 0, Maengel M2/M3; der Nutzer kann widersprechen)

- **FG-E3 (M2, Nachtrag 23):** Nach der Kanister-Szene setzt der Kampf mit DEMSELBEN Alligator fort (HP, Intro gesehen,
  Lage/Zustand bleiben; kein erneuter Durchbruch), und der Kanister landet so, dass der Alligator ihn im normalen Rundlauf
  aufnimmt (B's Aufnahme-Fenster). Spur G darf dafuer kleine, markierte Haken ("Runde 41 Spur G, FG-E3") in Spur B's Modulen
  setzen (gator_re2_r41.c / enemy_ai_boss_gator.c: Zustand ueber die Szene erhalten, Landepunkt/Fenster), Merkbits aus G's
  Zuteilung 114-119. B's Riegel bleiben gruen. Beleg im echten Ablauf ohne Testhaken (Leon UND Elza): Wurf -> Kampf -> Aufnahme
  -> Schuss -> Kiefer.
- **FG-E4 (M3, Nachtrag 5):** Eine Kamera "genau wie Adas Raketenwerfer-Wurf" (Werferin von unten, ~44 Grad) gibt es in
  ROOM2080/2090 nicht — der Port hat nur die vorgerenderten Hintergruende der Original-Kameras, eine neue Blickrichtung
  braeuchte einen neuen Hintergrund. Gewaehlt wird die naechstliegende vorhandene Kamera (laut Abnahme 2090 Cut 12, knapp
  daneben) mit der RE2-Schnittfolge des Wurfs (ROOM7020 sub12/13: Bild, Dauer, Schnitte belegen); PORT-WAHL im Dossier mit
  Bildvergleich RE2 <-> Port. Der Nutzer wird darauf hingewiesen.

## 2026-10-25 02:47 — Nachtrag 25 (Nutzer-Antworten per Auswahl zu den Fragen (o)/(p)/(q) der Spur C) + Orchestrator-Entscheid C-E4

(o) "Soll waehrend der 60 Sekunden RE2s Selbstzerstoerungs-Alarm (SUB_25, RE2 ROOM6040) laufen?" — **"Ja, RE2-Alarm SUB_25"**.
(p) "Countdown-Anzeige: gebaut MM:SS:HH oben rechts ... Passt das?" — **Antwort des Nutzers (Wortlaut): "Auch da gibt es
    mindestens ein Alarm in Resident Evil 2."** -> RE2 HAT eine Countdown-Anzeige (Selbstzerstoerungs-Zeit am Spielende):
    Spur C findet sie in RE2 (ghidra_re2_Leon.txt / RE2_Quellcode_V2: Zeit-Anzeige, Ziffern-Textur, Lage, Format, Ausloeser,
    z. B. die Laborsequenz um ROOM6040 / den Zug) und baut den 2030-Countdown in GENAU dieser Form nach (Form/Lage/Ziffern/
    Farbe mit Beleg), statt der Port-Wahl MM:SS:HH.
(q) "A fuse box for the valve controls." — **"Freigeben"**.

**C-E4:** Fuer den Alarm darf Spur C einen kleinen, markierten Haken im Musik-/BGM-Lader platform/pc/src/audio_pc.c setzen
("Runde 41 Spur C, C-E4"; Spur D ist integriert, beidseitig mergen). SUB_25 aus info/re2leon/COMMON/SOUND/SUB_25.BGM bytegleich
nach shared_assets/RE2/SOUND/ (Paket-Gate nennen). Alarm vom Warntext bis zur Flut bzw. bis zum Verlassen des Raums.

## 2026-10-25 02:51 — Orchestrator-Entscheid F-E1 (Abnahme F 0, Mangel M4: alte Riegel durch Nutzer-Vorgaben widerlegt)

Die von Spur F rot gemachten Riegel pruefen Zustaende, die der Nutzer ausdruecklich geaendert hat ("neue Regel widerlegt alten
Riegel", Memory reai-v2-gezielte-integration-querbezug). Spur F zieht sie mit Beleg nach (Kommentar mit Nachtrag/Wortlaut) und
faehrt sie gruen — kein Lockern ohne Grund:
- unit_r36_tueren_schott ("ROOM2090: keine begehbare Tuer nach ROOM20A0"): Tor 2090<->20A0 verschlossen bis zum Alligator-
  Schluessel (Nachtrag 21) -> Erwartung: verschlossen vor (9,113), begehbar danach.
- unit_r38_spinnen ("ROOM2090 Cut 0: 0 Spinnen"): Spinnen in 2090/2091 entfernt (Nutzer-Auftrag) -> 2090/2091 aus der
  Spinnen-Erwartung, uebrige Raeume unveraendert.
- unit_r30_tuer_verschlossen (Tabellenzahl 54 statt 52): Tabelle um die zwei neuen Tor-Saetze ergaenzen UND M3 (doppelter Ton)
  beheben -> genau 1 Ton.
- integration_r41_re2gator (Spur B, "RE2-Intro ..."): die 2090-Ankunftsszene ersetzt das RE2-Intro (Nachtrag 6) -> der Riegel
  prueft den Auftritt ueber F's Szene (Clip-Folge/Schnitte), B's uebrige Pruefungen bleiben unveraendert gruen.
