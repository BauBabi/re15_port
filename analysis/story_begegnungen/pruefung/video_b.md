# Prüfung Video B: Resident Evil 1.5 (All Footage).mp4

Stand 2026-10-31, Prüfer `video_b`. Gegenstand: `Version_80%/Resident Evil 1.5 (All Footage).mp4`, 14:19.1, 320x240, 30 Bilder/s, 25.774 Bilder. Geprüft gegen `analysis/story_begegnungen/BERICHT.md`, `TEXTE_40.md` und `analysis/video_80prozent/BEFUND.md` Abschnitt 5. Das Repo blieb unverändert.

## 1. Ergebnis

1. **Kein Bild zeigt Leon und Elza zusammen.** Die Widerlegung ist gescheitert. Nur der Auswahlschirm (0:10.5–0:18.8) zeigt beide, in getrennten Bildfeldern als Menü.
2. **Keine Szene zeigt Leon mit John oder Sherry. Keine zeigt Elza mit Ada oder Marvin.**
3. **Begleiter erscheinen in genau zwei Szenen:** Elza, John, Sherry (3:19.5–3:30.5) und Leon, Marvin, Ada (5:04.5–5:14.5). Die zweite fehlt in BERICHT und BEFUND für Video B. Beide stammen nachweislich aus den offiziellen Complete-Disc-Filmen (Abschnitt 3).
4. **BEFUND §5 hat vier falsche Figurenangaben:** 2:14–2:32 ist Leon, nicht Elza. 9:12–9:31 ist Elza, nicht Leon. 11:30–12:20 ist nur Leon. Marvin ist in 13:10–13:56 nicht im Bild. Die Zeile 2:40–2:48 führt keine Figur; im Bild steht Leon.
5. **Neuer Fund zur Reihenfolge, Raum 103:** Elza steht vor dem intakten Gitter (0:59.5–1:03), Leon erlebt den Durchbruch (2:12.8–2:21.7). Die Kamerafolge 12, 11, 5 ist exakt das Ereignis `ROOM1030 sub08`, das es nur in Leons Datei gibt. Das widerlegt zugleich BEFUND §5 Zeile 2:14 am Skript.
6. **Dialoge:** Alle lesbaren Spieltexte stimmen wörtlich mit der Disc überein, bis auf zwei Leon-Zeilen auf dem Dach (0:19.2–0:23.6). Die stehen nicht auf der 40%-Disc.
7. **Schauplätze ohne Disc-Vorlage:** Rippen-Tunnel, Raum mit rotem Rad, 50B, 50E, drei Trailer-Orte. Der von mir zunächst dazu gezählte graue Gang liegt dagegen auf der Disc (20B Cut 11–14).

## 2. Vorgehen

| Schritt | Umfang | Datei |
|---|---|---|
| Vollbilder 2 je Sekunde | 1718 Bilder auf 48 Bögen, alle angesehen | `vb/f2/`, `vb/b_000.jpg`–`b_047.jpg` |
| Schnittliste über alle 25.774 Bilder | 334 Schnitte, 7 Schwarzphasen | `vb_cuts.py`, `vb/cuts.txt`, `vb/black.txt` |
| Kurze Einstellungen unter 0,75 s | 104 Stück. 25 als Einzelbild geprüft (`vb/z/q1.jpg`), der Rest sind Blenden und Blitze in fein gesichteten Abschnitten | `vb_cutlist.py` |
| Untertitelband 2 je Sekunde | 18 Bögen, alle angesehen | `vb_band.py`, `vb/band_00.jpg`–`band_17.jpg` |
| Textsuche je 0,1 s | 2806 Treffer in 66 Bereichen, alle gegen die Bögen geprüft | `vb_text.py`, `vb/text_ranges.txt` |
| Hintergründe | 2 je Sekunde gegen 531 Disc- und 1119 Mod-Bilder | `vb_match.py`, `vb_match2.py`, `vb/seg.txt`, `vb/seg2.txt` |
| Schwierige Orte | Kantenvergleich über 6 Maßstäbe gegen alle 1650 Bilder, 13 Proben. Echte Treffer 0,72–0,83, Nicht-Treffer höchstens 0,61 | `vb_ms.py` |
| Farbprobe auf Elzas Hose | 8592 Bilder (10 je Sekunde) | `vb_elza.py`, `vb/elza_flag.txt` |
| Complete-Disc-Filme gegen B | 4 Dateien aus `documentation/` | `vb_cd.py` |
| Skripte der Disc | `scd_dump_room.py` auf 1030, 1031, 1170, 1171, 2000, 2001, 2030, 2070, 20B0, 20B1 | `vb/scd_*.txt` |
| Feinzooms | rund 50 Bögen | `vb/z/`, `video_b_notizen.md` |

**Zwei eigene Zwischenbefunde habe ich unterwegs selbst widerlegt.** „Grauer Gang fehlt auf der Disc“ war falsch, die Spalte `disc=` in `vb/seg.txt` prüft nur Namensgleichheit. „Mann mit Brille als neue Figur im Trailer“ hielt dem Zoom nicht stand (Abschnitt 7). Beide stehen deshalb hier nicht mehr als Befund.

## 3. Aufbau des Videos

BEFUND nennt B einen Sammelschnitt. Das lässt sich jetzt messen. Die vier Complete-Disc-Filme liegen als 320x240-Dateien unter `documentation/Biohazard 1.5 Complete Disc Video N of 4 (Resident Evil 1.5) (360p).mp4` (Fan-Mitschnitt mit Vorspann „Biohaze.com“, DivX-Logo).

| Zeit in B | Herkunft | Wasserzeichen | Beleg |
|---|---|---|---|
| 0:00–0:18.8 | Vorspann, Figurenwahl | BIOFLAMES im Menü | Bild |
| 0:19–1:23 | **Complete Disc Nr. 1** | BIOFLAMES | r = 0,97–1,00, Versatz etwa +11 s |
| 1:23–3:19 | weitere Aufnahmen | BIOFLAMES | – |
| 3:19.5–4:03.5 | **Complete Disc Nr. 3** | BIOFLAMES | r = 0,98, Versatz +3:11.0 |
| 4:05.5–5:03.5 | weitere Aufnahmen | BIOFLAMES | – |
| 5:04.5–5:39.5 | **Complete Disc Nr. 2** | BIOFLAMES | r = 0,93–0,96, Versatz +4:57.5 |
| 5:41–6:58 | weitere Aufnahmen | BIOFLAMES | – |
| 6:59.5–7:47 | Werbeschnitt mit „画面は開発中のものです。“ | BIOFLAMES | – |
| 7:49–9:31 | weitere Aufnahmen | BIOFLAMES | – |
| 9:33–10:07 | BIOHAZARD-2-Trailer | BIOFLAMES | Tafeln |
| 10:07.5–10:43.4 | derselbe Werbeschnitt, kürzer | SURVIVHOR | `vb/dup.txt` |
| 10:43.5–12:22 | weitere Aufnahmen | BIOFLAMES (andere Schrift) | – |
| 12:23–13:08 | **Complete Disc Nr. 4** | SURVIVHOR | r = 0,85–0,99, Versatz etwa +12:11.5 |
| 13:09–14:19 | weitere Aufnahmen, Abspann-Logo | BIOFLAMES | – |

Der Anteil aus Video A ist nicht belegt: Von 279 Bildern aus A erreichen 37 einen Wert ab 0,70 gegen B, die Spitze liegt bei 0,88, und diese Treffer sind Raumähnlichkeit oder Logos, keine durchgehenden Abschnitte (`vb/a_in_b.txt`). Gemeinsame Szenen gibt es, aber keine gemeinsame Aufnahme.

## 4. Abschnittsliste

Lückenlos, 125 Abschnitte, per Skript geprüft (`vb_tabfinal.py`). Raumnummern sind Disc-Nummern, „Cut“ ist die Kamera. Spielfigur: „wohl Leon“ heißt nach Kleidung, nicht sicher.

Sekunden je Spielfigur: Leon 409,6, wohl Leon 17,6, Elza 293,1, offen 43,8, Menü 8,3, kein Spielbild 86,7.

| von | bis | Ort | Spielfigur | Figuren im Bild | Text oder Bemerkung |
|---|---|---|---|---|---|
| 0:00.0 | 0:09.5 | Vorspann, kein Spielbild | – | keine | Katzenauge mit BIOHAZARD 2 / PROTOTYPE / AKA / RESIDENT EVIL 1.5 |
| 0:09.5 | 0:10.5 | Feuer-Wischblende | – | keine |  |
| 0:10.5 | 0:18.8 | Spielerwahl | Menü | Leon, Elza (Menü, je eigene Bildhälfte) | PLEASE SELECT MAIN CAST / Leon,S,Kennedy / Elza Walker / PROFILE 178 Cm 21 A / 169 Cm 19 0 52.4 Kg; Einblendung BIOHAZARD 1.5, WWW.BIOFLAMES.COM |
| 0:18.8 | 0:24.0 | 117 Cut 4, Dach | Leon | Leon | UT おい, Wort unsicher, ！ bis 0:21.6; UT …クソッ なんてこった！ ab 0:21.9; beide nicht auf der Disc |
| 0:24.0 | 0:30.5 | 117 Cut 3, 2, 1 | Leon | Leon |  |
| 0:30.5 | 0:36.5 | 118 Cut 2, 1 Gang | Leon | Leon, 2 Zombies |  |
| 0:36.5 | 0:41.0 | 11E Cut 13 | Leon | Leon, 2 Zombies, Blutlache |  |
| 0:41.0 | 0:48.5 | 107 Cut 6 Büro | Elza | Elza, 3 Zombies |  |
| 0:48.5 | 0:50.0 | 107 Cut 2 | Elza | Elza |  |
| 0:50.0 | 0:54.0 | 104 Cut 1 | Elza | Elza, kriechendes Wesen | Rolltor halb offen, dann zu, grosse Blutlache |
| 0:54.0 | 0:59.5 | 119 Cut 6 Schießstand | Leon | Leon | UT スイッチがある おしますか？ ▸Yes No (119 #0) |
| 0:59.5 | 1:03.0 | 103 Cut 6 | Elza | Elza, Zombies hinter dem Gitter | Gitter intakt |
| 1:03.0 | 1:08.5 | 120 Cut 2 Leichenhalle | Leon | Leon mit Flammenwerfer |  |
| 1:08.5 | 1:16.0 | 117 Cut 3 | Elza | Elza, Krähen |  |
| 1:16.0 | 1:20.6 | 10B Cut 1, 2, 8 | Leon | Leon, klein (blau, rotbraunes Haar bei 1:20) | Schrift RACCOON POLICE gehört zum Hintergrund |
| 1:20.6 | 1:22.5 | Weißblende | – | keine |  |
| 1:22.5 | 1:31.3 | 117 Cut 0, 1, 0 | Leon | Leon, Krähen |  |
| 1:31.3 | 1:33.5 | Schwarz | – | keine |  |
| 1:33.5 | 1:44.4 | 120 Cut 2, 1, 0 | Leon | Leon, RPD-Zombies |  |
| 1:44.4 | 1:46.4 | Schwarz | – | keine |  |
| 1:46.4 | 1:56.4 | 11E Cut 5 | Elza | Elza, Zombies |  |
| 1:56.4 | 1:58.9 | Schwarz | – | keine |  |
| 1:58.9 | 2:05.5 | 307 Cut 3, 12, 4 | Elza | Elza |  |
| 2:05.5 | 2:08.0 | 307 Cut 8 | – | Birkin hinter dem Tor |  |
| 2:08.0 | 2:12.8 | Rotblende, Logo | – | keine | BIOHAZARD 2 |
| 2:12.8 | 2:21.7 | 103 Cut 12, 11 | – | Zombies brechen durch das Gitter | Spielfigur nicht im Bild. Kamerafolge wie ROOM1030 sub08 |
| 2:21.7 | 2:33.0 | 103 Cut 5, 11, 1, 9, 1 | Leon | Leon, Zombie | Kopf platzt 2:25.6 |
| 2:33.0 | 2:39.5 | 107 Cut 4, 7 | Leon | Leon, Zombie |  |
| 2:39.5 | 2:48.5 | 104 Cut 4, 5 | Leon | Leon, Zombie | Kopf platzt 2:45.7 |
| 2:48.5 | 2:57.5 | 115 Cut 1, 2, 3, 5, 3 Chefbüro | Elza | Elza |  |
| 2:57.5 | 3:06.5 | 119 Cut 1, 0, 3, 5 | Elza | Elza, Hunde |  |
| 3:06.5 | 3:11.0 | 104 Cut 6, 3 | Elza | Elza, Leiche |  |
| 3:11.0 | 3:12.5 | 307 Cut 10 | – | Birkin nah |  |
| 3:12.5 | 3:18.0 | 120 Cut 2 | Elza | Elza, RPD-Zombies |  |
| 3:18.0 | 3:19.5 | Weißblende | – | keine |  |
| 3:19.5 | 3:30.5 | 306 Cut 5 Hinterhof | Elza | Elza, Sherry, John | UT …エルザ / 私 ヒミツの入り口を知ってるの; ヒミツの入り口？; ここよ この中から入れるの; シェリー！ 待って！ (306 #3-6) |
| 3:30.5 | 3:35.5 | 300 Cut 2 | Elza | Elza, Zombies |  |
| 3:35.5 | 3:46.0 | 301 Cut 4, 3, 2, 0 | Elza | Elza, Zombies |  |
| 3:46.0 | 3:51.5 | 306 Cut 3 | Elza | Elza, Zombie |  |
| 3:51.5 | 3:54.0 | 307 Cut 9 Konsole | Elza | Elza |  |
| 3:54.0 | 4:03.5 | 307 Cut 8, 10 | – | Birkin | Tor bricht |
| 4:03.5 | 4:05.5 | Weißblende, Schwarz | – | keine |  |
| 4:05.5 | 5:03.5 | Rippen-Tunnel, 3 Kameras | Leon | Leon, Zombies in Khaki | Nicht auf der Disc; Geometrie wie Fan-Raum 202 der Mod |
| 5:03.5 | 5:04.5 | Schwarz | – | keine |  |
| 5:04.5 | 5:14.5 | 200 oder 20B, Cut 2, Kanal | Leon | Leon, Marvin (hellblaues Hemd), Ada (weiss) | Ada im Wasser zur Leiter, Marvin folgt. Kein Text |
| 5:14.5 | 5:19.0 | Maschinenraum mit rotem Rad | Leon | Leon, Spinne | Nicht auf der Disc |
| 5:19.0 | 5:24.5 | Tunnel mit oranger Tür | Leon | Leon rechts hinten, Zombies in Khaki | Nicht auf der Disc |
| 5:24.5 | 5:30.5 | 200 Cut 13 | Leon | Leon, Zombies |  |
| 5:30.5 | 5:40.0 | 20B Cut 11-14, dann Cut 7, grauer Gang | Leon | Leon, Spinne, weiße Wolke hinter ihm | Disc-Bilder; in der Mod als 207 Cut 0-3 geführt |
| 5:40.0 | 5:41.0 | Schwarz | – | keine |  |
| 5:41.0 | 6:01.0 | 207 Cut 1, 3-7 (Disc-Moosgang) | Elza | Elza, Spinnen | Disc-Treffer 0,84-0,93, in BEFUND nicht zugeordnet; keine ROOM2071 auf der Disc |
| 6:01.0 | 6:36.0 | 104 Cut 6, 2 | Leon | Leon, Zombies |  |
| 6:36.0 | 6:46.0 | 104 Cut 1 | Leon | Leon | Rolltor offen |
| 6:46.0 | 6:55.5 | 104 Cut 4 | Leon | Leon | UT ボタンがある / 押しますか？ ▸Yes No (104 #0) |
| 6:55.5 | 6:58.0 | 104 Cut 1 | Leon | Leon, Zombie | Rolltor zu |
| 6:58.0 | 6:59.5 | Weißblende | – | keine |  |
| 6:59.5 | 7:03.5 | 11E Cut 4, 3, 2 | Leon | Leon | Einblendung 画面は開発中のものです。 |
| 7:03.5 | 7:05.2 | 118 Cut 5 | wohl Leon | Spielfigur von hinten, 2 Zombies | Blau, kein Rot |
| 7:05.2 | 7:06.9 | 20B Cut 13, grauer Gang | Elza | Elza, Spinne | Disc-Bild, in der Mod 207 Cut 2 |
| 7:06.9 | 7:08.0 | Tunnel | Leon | Leon klein, Klauenzombie | Nicht auf der Disc |
| 7:08.0 | 7:10.5 | 504 Cut 1, 2 | Elza | Elza, Zombies |  |
| 7:10.5 | 7:14.0 | 307 Cut 8, 4, 10 | Elza | Birkin, Elza (Cut 4) |  |
| 7:14.0 | 7:16.0 | Rotblende | – | keine |  |
| 7:16.0 | 7:20.0 | 107 Cut 2 | offen | 3 dunkle Figuren, eine fällt | Keine Elza; Figur links unklar |
| 7:20.0 | 7:26.5 | 300 Cut 1 | Elza | Elza, Zombies, Leichen |  |
| 7:26.5 | 7:31.0 | Tunnel | Leon | Leon, Zombies | Nicht auf der Disc |
| 7:31.0 | 7:39.0 | 506 Cut 6, 5 | Elza | Elza, Zombie |  |
| 7:39.0 | 7:43.0 | 20B Cut 14, grauer Gang | Leon | Leon, Spinne | Disc-Bild, in der Mod 207 Cut 3 |
| 7:43.0 | 7:47.3 | 514 Cut 6, 7 | Elza | Elza, brennender Zombie |  |
| 7:47.3 | 7:49.0 | Negativ-Blende | – | keine |  |
| 7:49.0 | 8:03.7 | 103 Cut 11, 1, 11 | offen | Zombies, Explosionen | Gitter zerstört, Spielfigur nicht erkennbar |
| 8:03.7 | 8:05.0 | Orangeblende | – | keine |  |
| 8:05.0 | 8:44.0 | 107 Cut 3, 7 | Elza | Elza, Zombies | Granaten, Zombies zerfallen in Ober- und Unterkoerper |
| 8:44.0 | 8:45.3 | Orangeblende | – | keine |  |
| 8:45.3 | 9:10.3 | 119 Cut 4, 5, 3 | Leon | Leon, Hunde |  |
| 9:10.3 | 9:11.7 | Orangeblende | – | keine |  |
| 9:11.7 | 9:31.5 | 512 Cut 2, 3, 6, 3, 4 rot | Elza | Elza, brennender Zombie | BEFUND nennt Leon |
| 9:31.5 | 9:33.2 | Negativ-Blende | – | keine |  |
| 9:33.2 | 9:36.0 | Trailer-Tafel | – | keine | あの惨劇から / 2か月後 |
| 9:36.0 | 9:38.0 | Trailer: 103 Cut 9, 0 | offen | 4 Silhouetten in Uniform | Keine Zuordnung |
| 9:38.0 | 9:40.4 | Trailer: 103 Cut 5 Wandkarte | offen | Uniformierte in hellen Hemden von hinten, Blut, mehrere Figuren rechts | Keine Zuordnung |
| 9:40.4 | 9:42.5 | Trailer-Tafel | – | keine | ラクーンシティ 全土に |
| 9:42.5 | 9:44.8 | Trailer: 115 Cut 0 | wohl Leon | Leon-Modell (rotbraunes Haar, Weste) schießt, Zombies | Trefferwolke am Zombie im Vordergrund |
| 9:44.8 | 9:49.0 | Trailer-Tafel, Rotblende | – | keine | 真の悪夢が 始まった |
| 9:49.0 | 9:51.0 | Trailer: grüner Gang mit Gitter | wohl Leon | Figur in Weste und Jeans, Zombies | Raum nicht zugeordnet |
| 9:51.0 | 9:53.5 | Trailer: blauer Gang | wohl Leon | Figur mit dunkler Weste, Jeans, nach Kleidung Leon-Modell; Zombie | Raum nicht zugeordnet |
| 9:53.5 | 9:56.0 | Trailer: 115 Cut 2 | wohl Leon | Figur hinter dem Schreibtisch schießt, nach Kleidung Leon-Modell; 2 Zombies |  |
| 9:56.0 | 9:58.0 | Trailer: 104 Cut 4 | offen | Figur wird gebissen, Zombie | Keine Zuordnung |
| 9:58.0 | 9:58.5 | Trailer: 104 Cut 1 | Leon | Leon-Modell, Zombie |  |
| 9:58.5 | 10:00.0 | Trailer: 115 Cut 3 | wohl Leon | Leon-Modell links (Jeans) schießt, Zombies |  |
| 10:00.0 | 10:01.6 | Trailer: 104 Cut 5 | Leon | Leon-Modell, Zombie |  |
| 10:01.6 | 10:02.8 | Trailer: gelbe Wand, Gitterboden | Leon | Leon-Modell, grosse Kreatur mit grüner Stelle | Raum nicht zugeordnet |
| 10:02.8 | 10:05.8 | Trailer: Moosgang | offen | Kleine Figur | Tafel 恐怖再び; Raum nicht zugeordnet |
| 10:05.8 | 10:07.5 | Trailer: violetter Grund | Leon | Leon-Modell mit langer Waffe |  |
| 10:07.5 | 10:09.0 | 118 Cut 5 (Survivhor) | wohl Leon | wie 7:03.5 | Wiederholung |
| 10:09.0 | 10:10.7 | 20B Cut 13 | Elza | Elza, Spinne | Wiederholung von 7:05 |
| 10:10.7 | 10:12.0 | Tunnel | Leon | Leon klein, Klauenzombie | Wiederholung, länger |
| 10:12.0 | 10:14.5 | 504 Cut 1, 2 | Elza | Elza, Zombies | Wiederholung |
| 10:14.5 | 10:18.5 | 107 Cut 2 | offen | 3 dunkle Figuren | Wiederholung von 7:16 |
| 10:18.5 | 10:25.0 | 300 Cut 1 | Elza | Elza, Zombies | Wiederholung von 7:20 |
| 10:25.0 | 10:33.0 | 506 Cut 6, 5 | Elza | Elza, Zombie | Wiederholung von 7:31 |
| 10:33.0 | 10:37.5 | 20B Cut 14 | Leon | Leon, Spinne | Wiederholung von 7:39 |
| 10:37.5 | 10:43.5 | 514 Cut 6, 7 | Elza | Elza, brennender Zombie | Wiederholung, länger |
| 10:43.5 | 11:25.5 | 506 Cut 1, 2, 3, 4, 7, 5, 6, 5 | Elza | Elza, Zombie | Rotlicht ab 10:54; 11:12.5-11:25 wie 7:31-7:39 |
| 11:25.5 | 11:26.0 | Schwarz | – | keine |  |
| 11:26.0 | 12:21.5 | 107 Cut 1, 2, 1, 5, 4, 7, 3, 7, 3, 2 | Leon | Leon, RPD-Zombies | BEFUND nennt auch Elza; keine Elza im Bild |
| 12:21.5 | 12:23.0 | Negativ-Blende, Rosa | – | keine |  |
| 12:23.0 | 12:25.0 | 50C Cut 4, 3 | Leon | Leon |  |
| 12:25.0 | 12:29.0 | 50B Cut 2 Maschinenhalle | Leon | Leon | Nicht auf der Disc |
| 12:29.0 | 12:38.0 | 50E Cut 8, 5 | Leon | Leon, braune Kreatur | Nicht auf der Disc |
| 12:38.0 | 12:47.0 | 408 Cut 1, 2 | Elza | Elza, dunkelrotes Wesen | Disc: nur Rohbau-Bild, keine RDT |
| 12:47.0 | 12:52.5 | 504 Cut 1, 2 | Elza | Elza, Zombies |  |
| 12:52.5 | 12:59.5 | 514 Cut 4, 5, 6 | Leon | Leon, brennende Zombies | In BEFUND ohne Figur |
| 12:59.5 | 13:03.0 | 512 Cut 6 | Elza | Elza, brennender Zombie | BEFUND nennt 5140 |
| 13:03.0 | 13:07.3 | 50A Cut 12 Hangar C-2 | – | Birkin, Spider-Man | Disc: Rohfassung des Bildes |
| 13:07.3 | 13:09.0 | Weißblende | – | keine |  |
| 13:09.0 | 13:28.5 | 307 Cut 1, 2, 3, 12, 4, 9, 4 | Leon | Leon, Konsole |  |
| 13:28.5 | 13:49.3 | 307 Cut 3, 4 | Leon | Leon, Birkin | BEFUND nennt Marvin; nicht im Bild |
| 13:49.3 | 13:56.9 | 307 Cut 5 | Leon | Leon, Birkin |  |
| 13:56.9 | 13:58.5 | Weißblende | – | keine |  |
| 13:58.5 | 14:10.2 | 103 Cut 7, 8 blau gefaerbt | offen | Zombiepulk um eine Figur | In BEFUND nicht zugeordnet |
| 14:10.2 | 14:11.4 | Weiß | – | keine |  |
| 14:11.4 | 14:15.0 | Todes-Einschnitte, Lichtkegel | wohl Leon | Liegende Figur, 2 Zombies | Figur nicht sicher erkennbar, keine rote Hose |
| 14:15.0 | 14:16.5 | Orange | – | keine |  |
| 14:16.5 | 14:19.1 | Logo | – | keine | BIOHAZARD 2 mit Katzenauge |

## 5. Szenen mit zwei oder mehr benannten Figuren

| Zeit | Figuren | Ort | Quelle | Bemerkung |
|---|---|---|---|---|
| 0:10.5–0:18.8 | Leon, Elza | Menü | Bioflames | Zwei getrennte Bildfelder, je PROFILE. Keine Szene |
| **3:19.5–3:30.5** | **Elza, John, Sherry** | 306 Cut 5 | Complete Disc Nr. 3 | Sherry kriecht ins Lüftungsgitter, 4 Untertitel |
| **5:04.5–5:14.5** | **Leon, Marvin, Ada** | 200 oder 20B Cut 2 | Complete Disc Nr. 2 | Siehe unten |
| 7:10.5–7:14 | Elza, Birkin | 307 Cut 8, 4, 10 | Bioflames | Beide bei 7:11.2 im selben Bild |
| 12:29–12:38 | Leon, braune Kreatur | 50E Cut 8, 5 | Complete Disc Nr. 4 | Laut BEFUND G10 Birkin, im Bild nicht benannt |
| 13:03–13:07.3 | Birkin, Spider-Man | 50A Cut 12 | Complete Disc Nr. 4 | Keine Spielfigur |
| 13:28.5–13:56.9 | Leon, Birkin | 307 Cut 3, 4, 5 | Bioflames | Marvin nicht im Bild |
| 10:01.6–10:02.8 | Leon-Modell, große Kreatur | nicht zugeordnet | Trailer | Kreatur nicht benennbar |

**Die Szene Leon, Marvin, Ada.** Sauberste Quelle ist Complete Disc Nr. 2, dort ab 0:07.6 (`vb/belege/leon_marvin_ada_clip2.jpg`). Leon steht rechts auf dem Sims und bewegt sich nicht. Daneben steht ein Mann in hellblauem Hemd und dunkler Hose, das ist das 80%-Modell Marvins. Eine Figur in Weiß watet durch den Kanal und steigt links auf den Sims zur Leiter, das ist Ada. Danach folgt Marvin durchs Wasser (B 5:09–5:12.5) und steht zuletzt ebenfalls links. Kein Text. Das passt zur Disc-Zeile 11B #20: „Marvin, ich gehe zuerst runter. Danach lass die Frau runter!“

## 6. Alle Untertitel und Einblendungen

| Zeit | Text | Disc | Bemerkung |
|---|---|---|---|
| 0:00–0:09.5 | BIOHAZARD 2 / PROTOTYPE / AKA / RESIDENT EVIL 1.5 | – | Vorspann der Webseite |
| 0:10.5–0:18.8 | PLEASE SELECT MAIN CAST, Leon,S,Kennedy, Elza Walker, PROFILE 178 Cm 21 A, 169 Cm 19 0 52.4 Kg, BIOHAZARD 1.5, WWW.BIOFLAMES.COM | `DATA/SELECT.TIM` | Die Disc-Zeilen „From police department“ und „From women's college“ sind nicht sichtbar. Das Logo verdeckt Leons Gewicht |
| 0:19.2–0:21.6 | おい + Wort unsicher + ！, Lesevorschlag おい 危険だぞ！ | **fehlt** | Leon, Dach 117 Cut 4. BEFUND las 待て, das passt nicht zur Zeichenzahl |
| 0:21.9–0:23.6 | …クソッ なんてこった！ (クソッ unsicher) | **fehlt** | Leon, allein am Geländer, kein Gegenüber im Bild |
| 0:54.3–0:55.8 | スイッチがある おしますか？ ▸Yes No | 119 #0, gleich | Mit Kana, BEFUND 5.4 schreibt 押 |
| 1:16–1:18 | RACCOON POLICE | Hintergrund 10B | Kein Text |
| 2:09–2:12.5 | BIOHAZARD 2 | – | Logo |
| 3:19.9–3:21.5 | …エルザ / 私 ヒミツの入り口を知ってるの | 306 #3, gleich | Sherry |
| 3:22–3:23.5 | ヒミツの入り口？ | 306 #4, gleich | Elza |
| 3:24–3:28.5 | ここよ この中から入れるの | 306 #5, gleich | Sherry |
| 3:29.5–3:30.2 | シェリー！ 待って！ | 306 #6, gleich | Elza |
| 6:47.8–6:48.8 | ボタンがある / 押しますか？ ▸Yes No | 104 #0, gleich | Leon |
| 6:59.5–7:01.1 | 画面は開発中のものです。 | – | Hinweis des Werbeschnitts |
| 9:33.2–9:36 | あの惨劇から / 2か月後 | – | Trailer |
| 9:40.4–9:42.5 | ラクーンシティ 全土に | – | Trailer |
| 9:44.8–9:48.3 | 真の悪夢が 始まった | – | Trailer |
| 10:03–10:05.8 | 恐怖再び | – | Trailer |
| 14:16.5–14:19 | BIOHAZARD 2 | – | Logo |
| fast durchgehend | WWW.BIOFLAMES.COM oder WWW.SURVIVHOR.COM | – | Wasserzeichen, Abschnitt 3 |

Mehr Text gibt es in B nicht: Band-Bögen, 66 Textbereiche und Streifen `vb/z/t1.jpg`, `t2.jpg` ergeben sonst nur Mündungsfeuer, Papier, Blut und Bodenkanten. Die Tonspur habe ich nicht ausgewertet.

## 7. Widerlegungsversuche

| Kandidat | Zeit | Ergebnis |
|---|---|---|
| Uniformierte neben Elza, Leichenhalle | 3:12.5–3:18 | RPD-Zombies, dasselbe Modell, das Leon bei 1:33.8–1:39 erschießt. Kein rotbraunes Haar, keine Weste |
| Figuren neben Elza, Hof | 7:20–7:26.5, 10:18.5–10:25 | Graublaue Zombies, greifen sie an |
| Figuren neben Elza, Büro | 0:41–0:48.5 | Braune Zombies |
| Drei dunkle Figuren im Büro | 7:16–7:20, 10:14.5–10:18.5 | Eine wird getroffen und fällt. Keine rote Hose, also keine Elza. Schütze nicht sicher |
| Zweite blaue Figur neben Leon | 13:48–13:49 | Birkins Jeansbeine |
| Weiße Figur neben Leon, Halle | 12:26.5–12:28.5 | Feststehend in allen Bildern, gehört zum Hintergrund |
| „Mann mit Brille“ im Chefbüro | 9:42.6–9:44.5 | Rotbraunes Haar, Weste, zielt. Der rote Fleck ist ein Treffer am Zombie davor. Nach Kleidung Leon-Modell, keine neue Figur belegt |
| Figur hinter dem Schreibtisch | 9:54–9:55.6 | Helle Ärmel, Weste, Jeans, schießt. Kein Rot, kein weißes Brustteil |
| Mehrere Figuren im Chefbüro | 9:58.6–9:59.9 | Links Leon-Modell, sonst Zombies |
| Blauer Pulk | 13:58.5–14:10 | Personen nicht unterscheidbar, Restunsicherheit |
| Todes-Einschnitt | 14:11.4–14:15 | Eine liegende Figur, zwei Zombies. Blauer Oberkörper, wohl Leon |
| Rote Flächen außerhalb der Elza-Abschnitte | gesamt | Nur Logos, Tafeln, Menü, Flammenwerfer, rotes Rad, Bodenlicht, Feuer, Blut, rote Lampe. In 11:26–12:21 kein einziger Treffer |

Unter den Zombies steht bei Elza nie die Weste mit Rückenstern und rotbraunem Haar, bei Leon nie die rote Hose. **Urteil: nicht widerlegt.** Grenzen: 320x240, Dunkelheit, 43,8 s ohne erkennbare Spielfigur.

## 8. BEFUND.md Abschnitt 5, Zeile für Zeile

| Zeile | Urteil | Befund |
|---|---|---|
| Vorspann „Sammelschnitt … V1 und V2“ | teils | Complete-Disc-Teil gemessen. Anteil aus V2 nicht belegt |
| Vorspann „am Ende (9:34–10:06)“ | ungenau | Der Trailer liegt mitten im Video, danach kommen 4 Minuten |
| 0:10–0:18 Spielerwahl | richtig | – |
| **2:14–2:32 „Elza“, Cut 6/11/5/1/9** | **falsch** | Leon mit Weste, rotbraunem Haar und Jeans (2:24, 2:27.5, 2:29.2). Cut 12 statt 6. Dasselbe Ereignis gibt es nur in `ROOM1030` |
| 2:40–2:48 | Figur fehlt | Die Zeile nennt niemanden, auch nicht Elza. Im Bild Leon |
| 2:50–2:57 Elza, Chefbüro | richtig | Name „Irons“ steht nicht auf der Disc |
| 2:58–3:05 Elza gegen Hunde | richtig | – |
| 3:32–3:45 Elza am Lkw | richtig | – |
| **4:06–5:12 „2000/2070, Zombie-Gruppen“** | **falsch** | 4:05.5–5:03.5 Rippen-Tunnel ohne Disc-Bild. 5:04.5–5:14.5 ist keine Zombie-Szene, sondern Leon, Marvin, Ada |
| 5:42–6:00 „?110/11B, nicht sicher“ | geklärt | Disc-Raum 207 Cut 1, 3–7, Werte 0,84–0,93 |
| 6:02–6:57 Leon, 1040 | richtig | – |
| 7:00 Einblendung | richtig | Beginnt 6:59.5 in Cut 4 |
| 7:44, 10:38–10:42 | ergänzt | Elza in 514 Cut 6, 7 |
| 12:52–13:02 „5140 Cut 4–7“ | ungenau | 12:52.5–12:59.5 Leon in 514. 12:59.5–13:03 Elza in 512 Cut 6 |
| 8:06–8:42 Elza mit Granaten | richtig | – |
| 8:46–9:05 Leon gegen Hunde | richtig | – |
| **9:12–9:31 „Leon und Zombie im Feuer“** | **falsch** | Elza unter Rotfilter. Gleiche Kamera wie 13:00 ohne Filter, gleiche Haarform, gleiche dunkle Knieflecken (`vb/belege/feuer_elza.jpg`) |
| **11:30–12:20 „Leon und Elza (getrennt!)“** | **falsch** | Nur Leon, 11:26–12:21. Elza in 107 steht bei 0:41–0:50 und 8:05–8:44 |
| 13:10–13:56 „Leon (mit Marvin)“ | **falsch** | Marvin ist nicht im Bild. Er stand nur im Mod-Skript |
| 14:00–14:14 „nicht zugeordnet“ | geklärt | Raum 103 Cut 7, 8, blau gefärbt. Disc-Bild gleich. Opfer wohl Leon, nicht sicher |
| 5.2 Schadensmodell, Zeiten | richtig | Nahschuss Leon, Granate Elza |
| 5.3 „neben Elza … Büro 0:44–0:47, 7:16, 10:16“ | **falsch** | Bei 0:44 braune Zombies. Bei 7:16 und 10:16 steht Elza nicht im Bild |
| 5.3 „In keinem der drei Videos …“ | richtig für B | Abschnitt 7 |
| 5.4 0:20 „1170 Msg 5/6“ | ungenau | Mod-Text. Die Disc hat dort nichts |
| 5.4 0:54 押しますか | ungenau | Im Bild おしますか, wie Disc 119 #0 |
| G4 weiße Partikelwolken | Teil geklärt | Im Kanal sind es Spritzer an den Füßen (B 5:05–5:09). Im Gang eine Wolke hinter Leon, die in Cut 7 aus der Tür bricht (5:34–5:40). Art offen |
| H3 Rolltor in 104 | geklärt | Tor in Bewegung: offen 6:36–6:44, Knopf 6:47.8, zu ab 6:55.5. Bei Elza läuft es 0:50–0:53 |

## 9. BERICHT.md

| Stelle | Aussage | Urteil | Beleg |
|---|---|---|---|
| §1, §3 | Leon und Elza nirgends sicher in einem Bild | bestätigt für B | Abschnitt 7 |
| §10 | Kein Video zeigt John oder Sherry neben Leon | bestätigt für B | Abschnitt 7 |
| §3 | Elza – Marvin | in B nicht zu sehen | Nur A 0:46.8 |
| §3 | Leon – Ada, Marvin „mehrfach“ | in B genau einmal | 5:04.5–5:14.5 |
| §3 | Leon – Frau im Kittel | in B nicht zu sehen | Kein Bild |
| §5 Dach | Leon „Kein Text“ | ungenau | Stimmt für die Disc (Textblock 1170 nur John, kein `Message_on` in `ROOM1170`). Im 80%-Stand spricht Leon zweimal |
| §5 Kanal | Ohne Text | ergänzen | B 5:04.5: Leon, Marvin, Ada. `ROOM2000`, `ROOM2001`, `ROOM20B0` setzen keinen NPC |
| §8 | Marvin 80% hellblaues Hemd, eigenes Modell | bestätigt | Complete Disc Nr. 2 ab 0:08 |
| §1, §4 | Reihenfolge Elza vor Leon | gestützt, neu | Raum 103, Abschnitt 10.4 |
| §10 | B 0:12, B 3:21 | bestätigt | Abschnitt 6 |
| §10 | B 7:21–12:21 „Nur Zombies“ | bestätigt, gilt für das ganze Video | Abschnitt 7 |
| §10 | Tabelle Video B | lückenhaft | Es fehlen 0:19 Dachtexte, 5:04.5 und die Raum-103-Folge |
| §10 | Ende in beiden Videos nicht zu sehen | bestätigt für B | Kein Bild aus 603, 604 oder vom Bahnsteig |
| §12 | Video B alle 3 Sekunden | überholt | Jetzt 2 je Sekunde plus Schnittliste |
| §2 | Bioflames/Complete-Disc-Material | bestätigt und messbar | Abschnitt 3 |

## 10. Unterschiede zum 40%-Stand

### 10.1 Dialoge

Nur eine Abweichung: Die zwei Leon-Zeilen auf dem Dach (0:19.2–0:23.6) stehen nicht auf der Disc. `ROOM1170` hat kein `Message_on`, der Textblock trägt nur Johns vier Zeilen. Beide Untertitel sind in Complete Disc Nr. 1 enthalten, also Capcom. Die Mod übersetzt an dieser Stelle „Hey! Hey wait! Down here!“ und „Oh, that's just freaking great...“. Der erste Satz passt nur teilweise zu meiner unsicheren Lesung, der zweite passt zu なんてこった. Wen Leon anruft, zeigt das Bild nicht.

Alle übrigen Spieltexte (119 #0, 306 #3–6, 104 #0) sind wörtlich gleich.

### 10.2 Figuren und Gegner

| Punkt | Video B | Disc |
|---|---|---|
| Marvin im Kanal | 5:04.5, hellblaues Hemd | Kein NPC in 2000, 2001, 20B0. Typ 0x40 ist Platzhalter |
| Ada im Kanal | 5:04.5, weiß | Kein NPC in diesen Räumen |
| Spinnen im grauen Gang | Leon 5:30.5, 7:39. Elza 7:05 | `ROOM20B0` und `ROOM20B1` ohne Spinne, `ROOM20B1` nur John `@0x14BC` |
| Elza im Moosgang | 5:41–6:01 | Nur `ROOM2070`, keine `ROOM2071`. Spinnen `@0x2346`, `@0x235A` |
| Elza in Raum 408 | 12:38–12:47 | Nur `ROOM408.BSS`, keine RDT |
| Spinne im Raum mit rotem Rad | 5:14.5–5:19 | Raum fehlt |
| Birkin gegen Spider-Man | 13:03–13:07 | `ROOM50A0` und `ROOM50A1` setzen keinen einzigen Gegner |

Keine Frau im Kittel, kein Roy, kein Polizeichef im Bild.

### 10.3 Schauplätze

**Ohne Disc-Vorlage, B ist hier Quelle:**

| Ort | Zeit | Disc | Mod |
|---|---|---|---|
| Rippen-Tunnel, 3 Kameras | 4:05.5–5:03.5, 7:06.9–7:08, 7:26.5–7:31, 10:10.7–10:12 | Kein Treffer (höchstens 0,56 auf 200/20B, Ähnlichkeit der Bauart) | Fan-Raum 202 „Zombie Tunnel“ by Juvenal (README Z. 62) bildet die Geometrie nach, andere Pixel (`vb/belege/tunnel_gegen_fan202.jpg`) |
| Tunnel mit oranger Tür | 5:19–5:24.5 | Kein Treffer (0,42) | Fan-Raum 202 Cut 5 ähnlich, ohne Tür |
| Raum mit rotem Rad | 5:14.5–5:19 | Kein Treffer (0,42) | Fan-Raum 206 anderer Blick, BEFUND H2 |
| 50B Maschinenhalle | 12:25–12:29 | Keine Datei | 50B Cut 2, Wert 0,72 |
| 50E | 12:29–12:38 | Keine Datei | 50E Cut 8, 5, Wert 0,75 |
| Grüner Gang mit Gittertor | 9:49–9:51, 10:02.8–10:05.8 | Kein Treffer (0,44) | Kein Treffer |
| Blauer Gang | 9:51–9:53.5 | Kein sicherer Treffer (20104 mit 0,56, Sichtvergleich nicht überzeugend) | – |
| Halle mit gelber Wand | 10:01.6–10:02.8 | Kein Treffer (0,38) | Kein Treffer |

Ob die Mod-Bilder von 50B und 50E von Capcom oder von Fans stammen, kann ich nicht entscheiden.

**Auf der Disc nur als Rohfassung, im Video fertig:** 118 (`D 11801` weiß und grau ohne Textur), 408 (Graubau), 50A Cut 12 (Tor, Säule anders). Beleg `vb/belege/raeume_408_50A.jpg`.

**Auf der Disc vorhanden, in BEFUND falsch benannt:** Die Mod hat Bilder umnummeriert. Disc 20B Cut 11–14 ist in der Mod 207 Cut 0–3 (Differenz 0,9–3,6), Disc 203 Cut 0–4 ist Mod 207 Cut 4–8, Disc 201 ist Mod 11A, Disc 604 ist Mod 603 Cut 6–12. Die Moosräume der Disc (207) fehlen in der Mod ganz, passend zu README Z. 228 „Removed old green sewers rooms“. BEFUND §3 und §5 rechnen in Mod-Nummern: „2070 Cut 0–3“ heißt auf der Disc 20B Cut 11–14. Beleg `vb/grau.txt`, `vb/belege/grauer_gang_20B.jpg`.

**Tür in 20B:** `ROOM20B0 sub02 @0x14EA` setzt Cut 7 (`@0x1500`) und lässt die Spielfigur laufen, `sub03` und `sub04` bewegen die Objekte 4 und 3 (`@0x149E`, `@0x14C0`). Bei B 5:38.6–5:39.8 bricht in Cut 7 die weiße Wolke an der Tür hervor. Das passt zusammen, die Wolke selbst steht nicht im Skript. Die Mod nennt das Ereignis „water flood“.

### 10.4 Neu: Raum 103 als vierte Weltspur

| | Elza | Leon |
|---|---|---|
| Video | 0:59.5–1:03, Cut 6, **Gitter intakt**, Zombies dahinter | 2:12.8 Cut 12, 2:17.3 Cut 11, 2:21.7 Cut 5, **Gitter durchbrochen** |
| Disc | `ROOM1031 @0x1EAE`: nur `Ck(3,134)`, kein Ereignis. Setzer von (3,134) auf der Disc: keiner | `ROOM1030 sub01 @0x204A` startet `sub08 @0x2632`: `Cut_replace` 0→9, 3→10, 4→11, 6→12 (`@0x2646`–`@0x264F`), dann `Cut_chg` 12, 11, 5 (`@0x266E`, `@0x268C`, `@0x2692`) |
| Folgetext | – | 11B #11 „Das Rolltor wurde durchbrochen. Sie kommen einer nach dem anderen rein.“ passt dazu, nennt Raum 103 aber nicht. `Set(4,6)` `@0x205A` ist keine Klammer: `ROOM11B0 main00 @0xE0E` setzt (4,6) selbst |

Die Bildpaare sind `D 10306` gegen `D 10312` und `D 10304` gegen `D 10311` (`vb/belege/gitter_folge.jpg`). Damit ist diese Stelle von zwei Seiten Leon: Figur im Bild und Skript nur in `ROOM1030`.

Das passt zur Richtung des Berichts, ist aber schwächer als dessen drei harte Stellen, denn hier erzwingt Leons Datei keinen Vorzustand. Das Ereignis läuft in Leons Szenario selbst ab. Ein zweites Mal zeigt B das zerstörte Gitter bei 7:49–8:03.7 ohne erkennbare Spielfigur. Die orange Wirkung gleicht Elzas Granaten (8:10.5), das ist aber kein Beleg für sie. Wäre es Elza, müsste (3,134) im 80%-Stand gesetzt werden.

### 10.5 Gemeinsame Räume

B zeigt beide Figuren getrennt in denselben zehn Räumen: 103, 104, 107, 117, 119, 11E, 120, 20B, 307, 514. In 307 erscheint Birkin bei beiden (Elza 3:54, 7:10.5, Leon 13:28.5). In 514 begegnen beide dem gleichen brennenden Zombie im weißen Hemd (Elza 7:43, Leon 12:52.5).

## 11. Grenzen

- Figuren sind Sichtbefund auf 320x240, teils mehrfach umkopiert und gefiltert. Sicher sind die Zuordnungen, die Haar, Weste oder rote Hose zeigen.
- 43,8 s bleiben ohne erkennbare Spielfigur, vor allem 7:16–7:20, 7:49–8:03.7, 13:58.5–14:10 und Teile des Trailers.
- Die zwei Dachzeilen sind nur teilweise lesbar.
- Tonspur nicht ausgewertet.
- Die Farbprobe prüft Elzas rote Hose. Unter Blau- oder Rotfilter sagt sie wenig, dort entscheidet die Sicht.
- Die Abschnittsgrenzen stammen aus Schnitten und 2-Bilder-Sicht. Vier Grenzen liegen unter der Messschwelle der Schnittsuche (0:09.5, 2:05.5, 3:11, 7:14), Genauigkeit dort etwa 1 s.
- Bei `ROOM1030 sub08` ist die Reihenfolge belegt, nicht die Dauer. Die Sleep-Summen (225 zu 200) und die Videodauer (4,50 zu 4,37 s) habe ich nicht abgeglichen.

## 12. Dateien

Alle unter `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/pruefung/`:

- `video_b.md` dieses Dossier, `video_b_notizen.md` Laufnotizen
- `vb/belege/` Belegbilder: `leon_marvin_ada_clip2.jpg`, `gitter_folge.jpg`, `gitter_elza_leon.jpg`, `feuer_elza.jpg`, `grauer_gang_20B.jpg`, `raeume_408_50A.jpg`, `tunnel_gegen_fan202.jpg`
- `vb/tabelle.md` Abschnittsliste, `vb/cuts.txt`, `vb/black.txt`, `vb/seg.txt`, `vb/seg2.txt`, `vb/dup.txt`, `vb/a_in_b.txt`, `vb/elza_flag.txt`, `vb/grau.txt`
- `vb/b_000.jpg`–`b_047.jpg` Kontaktbögen, `vb/band_00.jpg`–`band_17.jpg` Untertitelband, `vb/z/` Zooms
- `vb_*.py` Werkzeuge
