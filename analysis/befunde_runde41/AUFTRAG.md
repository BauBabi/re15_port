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
