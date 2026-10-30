# Runde 41 — Spur N "nachtrag30" (Dossier, fortlaufend)

Baum `.claude/worktrees/r41_nachtrag30`, Zweig `r41/nachtrag30`, Basis cd48e3c7 (= Merge master 21aad0c3).
Auftrag: AUFTRAG.md Nachtrag 30 / 30b / 30c (Wortlaut massgeblich). Zuteilung VERTRAG 1.2 "N": Bits 100-103/106/107/125
(+126-133 nach Zensus), Ids 2000/20B0 40, 2030 40-55, 2010/2011 40-44, 2050 40-44; Lautstaerke-Modul der Spur D.

## 0. Nutzer-Frage (Nachtrag 30c, Wortlaut) und Antwort

> Aber ich raffe es immer noch nicht... wie kommst du auf 9, 2, 1, 8? Wir haben 6 valves: 1 - schwarz, 2 - grün. 3 - rot,
> 7 schwarz, 8 grün, 9 rot und dann das Plakat. Ich wollte sagen das es laut Plakat "Grün" und "Schwarz" vertauscht hat,
> also 9 und 2. Wie kommst du auf die Lösung?

Waehrend dieser Spur kamen Nachtrag 30d (17:23, Plakat NEU 1. GREEN 01 - 2. RED 02 - 3. BLACK 09 - 4. RED 08, Loesung
09 -> 02 -> 01 -> 08; ersetzt 30c) und 30e (Wasser in 2010 bis zur Flut) hinzu (vom Orchestrator in diesen Zweig committet,
4bd6c7e2; master danach gemergt 89dfbe48). Antwort/Rechenweg fuer den Nutzer: §3.

## Punkte (Stand nach 30d/30e)

1. Schrank 2010 klappt auf -> Sicherungskasten wie 2060 (fuse_box.bmp), Mechanismus wie 2060.
2. Panel-Raetsel 2030 (Druck 80) — Entwuerfe zur Freigabe (gebaut wird erst nach Freigabe).
3. Notiztext ("green and red" + Nummern "black"/"green") + Plakat NEU + Loesung 09 -> 02 -> 01 -> 08 (Nachtrag 30d).
4. RE2-Ton der Plattformfahrt 2030.
5. RE2-Alarmton zusaetzlich durchgehend (Warntext 46 bis Flut/Verlassen).
6. Lautstaerke: ALLE Toene (Tuersequenzen, Wassersprung ...).
7. (30e) 2010: Wasser die Leiter herunter und ueber den Weg zum Becken in der Mitte, bis zur Flut in 2030.

## 2. Panel-Raetsel 2030 — Entwuerfe zur Freigabe (Nachtrag 30/30b; gebaut wird erst nach der Freigabe)

Ziel aller Entwuerfe: die Druckanzeige zeigt 80, danach die bestehende Frage Id 43 "The pressure gauge reads 80. / Lower the
bridge?". Bilder (320x240, doppelt): `N_belege/panel_entwurf_a.png`, `_b.png`, `_c.png`; Kamera und Ausschnitt:
`N_belege/panel_ausschnitt.png`.

**Kamera (gemessen).** Das Nutzerbild ist Cut 2: schaltschrank.png = Cut 2 um 10 px versetzt (Schablonenvergleich 0,999 bei
Massstab 1,0; naechster Cut 0,25). `ROOM2030.RDT` Cut 2 @0x0A0 (3883,-8101,-7091)->(-11568,-5157,-9778) zeigt das ganze Panel
schraeg, der linke Rand faellt aus dem Bild. Cut 1 @0x080 (-3858,-8101,-3691)->(11593,-5157,-12615) zeigt es fast von vorn, der
untere Teil liegt hinter dem Gelaender. Beide fov 26684 -> H 208. Aus 15 Punkten in beiden Cuts trianguliert (Strahlenabstand
<= 42): Tafelebene z -10550, Scheiben ca. 35 davor, Tafel ca. x -920..670, y -9030..-6390. Beide Cuts sind darauf entzerrt; die
Quelle hat rund 3 px je 100 Einheiten.

**Was wirklich auf dem Panel ist:** fuenf Roehrenelemente (Paar, einzeln, Paar: grauer Kopf, helles Schauglas oder Griff, Loch im
Fuss); eine runde Anzeige; vier gleiche runde Scheiben in einer Reihe (weiss, schwarzer Balken ueber den Durchmesser); Schild oben
rechts (rotes und helles Feld); rotes Warnschild; weisses Schild im Metallrahmen; blauer Kasten; fuenf runde Tasten (eine einzeln,
vier in einer Reihe unter den Scheiben, darunter je ein schwarzes Feld). Der Beleg-Bogen benennt alle.

**Vorbilder, nicht kopiert:** 2050 = vier Hebel ein/aus, feste Werte 30/20/40/10, Summe 80. 11F0 = zehn Schalter mit + und -,
Summe 80. Beide Original-Nahansichten (2050 Cut 15, 11F0 Cut 10) tragen denselben blauen Stahlrahmen (Zeilen 0..15 und 224..239
bitgleich).

**Gemeinsam:** Rahmen pixelgleich aus 2050 Cut 15, EXIT-Feld pixelgleich aus 11F0 Cut 10 (x 210..238, y 175..204), Cursor aus einem
echten Spielbild 2050. Lage, Farbverlauf und Wand stammen aus dem entzerrten Echtbild; Roehren, Scheiben und Schilder sind an den
echten Stellen neu gezeichnet, weil die Quelle dafuer zu grob ist. Ausschnitt = oberer Teil der Tafel, unten sichtbar die
Oberkanten von weissem Schild und blauem Kasten. Schild oben rechts neu beschriftet: BRIDGE / PRESSURE 80. Bedienung wie 2050:
Steuerkreuz bewegt den Cursor, SQUARE betaetigt, CROSS oder EXIT verlaesst das Panel. Eingang "Will you operate the panel?"
(Frageform ROOM2050 msg 0). Alle Raetselwerte und Texte sind PORT-WAHL. Beim Bau entfaellt Ventil 13 (Slot 13, Ereignis 25, Texte
41/42/47), Bit 101 bedeutet dann "Druck 80 erreicht". Die Zeit unter einer Minute ist aus der Zugzahl geschaetzt, nicht am
Spieler gemessen. Pruefung der Regeln: Durchsuchen aller Zustaende (Werte je Entwurf).

### Entwurf A "Druckspeicher" (`panel_entwurf_a.png`, Stand 50/20/30, Quelle C gewaehlt)

* Aufbau: drei Tanks auf einer Skala, A = linkes Paar (fasst 100), B = einzelne Roehre (50), C = rechtes Paar (30). Das Fenster im
  Metall ist so lang, wie der Tank fasst. Die Anzeige liest Tank A, rote Marke bei 80.
* Bedienung: Cursor auf einen Tank, SQUARE waehlt die Quelle (gruene Lampe), Cursor auf einen zweiten Tank, SQUARE fuellt um.
* Regel: es fliesst, bis die Quelle leer oder das Ziel voll ist.
* Loesung: Start 100/0/0; A nach B (50/50/0), B nach C (50/20/30), C nach A (80/20/0).
* Weg dorthin: die Nadel steht ueber der roten 80, es muessen 20 weg; beim Probieren bleiben in B genau 20 zurueck, 50 + 30 = 80.
* Fehlversuch: gibt es nicht, jeder Stand fuehrt weiter; EXIT setzt auf den Start zurueck. Leere Quelle oder volles Ziel: Fehlerton
  wie 2050 und ein Satz.
* Texte: "The pressure gauge reads 100." / "Pick a tank to drain, / then a tank to fill." / "The tank is empty." / "The tank is full."
* Geprueft: 16 Zustaende, keine Sackgasse, genau eine kuerzeste Loesung (3 Zuege), von jedem Stand hoechstens 4 Zuege bis 80.

### Entwurf B "Stellventile" (`panel_entwurf_b.png`, Stand 2-0-1-2, Anzeige 20)

* Aufbau: die vier Scheiben sind Drehventile in einem aufgedruckten Leitungsbild von PUMP zur Anzeige. Offen = Balken in Linie mit
  der Leitung (senkrecht), Lampe darueber leuchtet. Je offenes Ventil 20 auf der Anzeige.
* Bedienung: Cursor auf eine Scheibe, SQUARE dreht 60 Grad.
* Regel: jede Scheibe nimmt ihre rechte Nachbarin mit, die letzte dreht allein. Der Balken steht nach drei Klicks wieder senkrecht.
* Loesung: Start 2-0-1-2 (Rasten, 0 = senkrecht): Ventil 1 einmal, Ventil 2 zweimal, Ventil 4 einmal; Reihenfolge egal.
* Weg dorthin: schon der erste Klick zeigt die Kopplung; von links nach rechts klappt es immer.
* Fehlversuch: gibt es nicht, weiterdrehen; EXIT setzt auf den Start zurueck.
* Texte: "The pressure gauge reads 20." / "A valve is open when it stands / in line with the pipe." / "The valves are linked."
* Geprueft: 81 Zustaende, jeder loesbar, aus diesem Start 4 Klicks (Anzeige 20, 20, 20, 60, 80 in der genannten Folge), aus dem
  schlechtesten Start 8.

### Entwurf C "Pumpe und Ablass" (`panel_entwurf_c.png`, Stand 60, Hitze 2)

* Aufbau: linkes Paar = Griff PUMP, rechtes Paar = Griff BLEED, einzelne Roehre = OIL TEMP in drei Stufen (oberste rot), Anzeige =
  Druck. Rotes Schild: DO NOT OVERHEAT / RELIEF VALVE.
* Bedienung: Cursor auf einen Griff, SQUARE zieht ihn einmal.
* Regel: PUMP +30 Druck, +1 Hitze. BLEED -10 Druck, -1 Hitze (nicht unter 0). Hitze 3 oder Druck ueber 100: das Ueberdruckventil
  bricht auf, alles zurueck auf 0.
* Loesung: PUMP, PUMP, BLEED, PUMP oder PUMP, BLEED, PUMP, PUMP. Dreimal PUMP geht nicht, die dritte Pumpe ueberhitzt.
* Weg dorthin: die Werte stehen an den Griffen, die Hitze steigt sichtbar; wer ueberhitzt, sieht die Regel.
* Fehlversuch: Zischen, Nadel und Hitze fallen auf 0, Satz, neuer Versuch.
* Texte: "The pressure gauge reads zero." / "The oil overheated. / The relief valve blew!" / "The pressure is too high. / The relief
  valve blew!"
* Geprueft: 23 Zustaende, keine Sackgasse, genau die zwei genannten kuerzesten Folgen (4 Zuege), von jedem Stand hoechstens 5 Zuege.

### Empfehlung

**Entwurf A.** Er nutzt das Auffaelligste des Panels (Roehren neben der Anzeige), hat nichts zum Ein- und Ausschalten und ist darum
am weitesten von 2050 entfernt, und die Loesung hat einen echten Aha-Moment ohne Rechenarbeit. Kein Fehlversuch, keine Sackgasse.
Einziger Punkt zum Entscheiden: die Nadel startet bei 100 und muss herunter. B ist 2050 am naechsten (vier Elemente, je 20). C ist
die Wahl, wenn ein Fehlversuch mit Folgen gewuenscht ist.

## 6. Lautstaerke — Messung vorher (Stand 89dfbe48 = master + nichts geaendert; exe 17:23)

Nutzer-Einstellung im Hauptbaum (nur gelesen): `re15_port/build/platform/pc/re15_options.cfg` = `volume=20` / `sound=mono`
(geschrieben 16:48, drei Minuten vor Nachtrag 30).

* Weg der Toene (Code gelesen): EIN SDL-Geraet (audio_pc.c re15_audio_init, `SDL_OpenAudioDevice` mit `audio_callback`); in
  audio_callback laufen nacheinander s_active (alle SE: Raum/Kern/Waffen/RE2-Tuerbank `re15_audio_re2_tuer_se` der
  Tuersequenz door_scene_pc.c, Schritte `re15_audio_footstep` = auch Wasser-Schritt/Landung), s_xa (Stimmen), s_fmv_audio (FMV),
  re15_ss_render_bgm (MAIN/SUB/SUB2 inkl. RE2-Alarm SUB_25), re15_amb_mix (Rotor) — danach Mono-Faltung und als LETZTES die
  Endstufe der Spur D (`(out*q)>>15`). Kein zweiter Ausgabeweg (grep SDL_OpenAudio/QueueAudio/PlaySound/waveOut: nur diese Stelle).
  Die Tuersequenz (door_scene_pc.c) pumpt denselben Mischer (`re15_audio_se_pumpe` / `re15_audio_tick`).
* Messung A (deterministisch, RE15_AUDIO_CAP_SYNC = dieselbe Mischkette ohne Geraet), Lauf 20A0 -> Tuer Slot 0 (Tuersequenz
  `DOOR26`, `[tuer] Bild 60 Se_on Tonkopf-Eintrag 0`, Schliesston) -> 2090 mit Sprung ins Becken (climb.log `sub=10 kind=0x02
  ph=4` = Landung), volume=100 gegen volume=50: **2 499 000 von 2 499 000 Samples exakt `v50 == (v100*0x4000)>>15`**, 785 Takte mit
  Ton, Verhaeltnis je Takt 0,5000 (Tuer-Takte 185..245 mit Spitzen bis 16681 -> 8340).
* Messung B (echter SDL-Rueckruf, RE15_AUDIO_CAP, Nutzer-Einstellung MONO) volume=100 gegen volume=20, gleicher Lauf:
  Spitze 17274 -> 3674 (0,213), Median-RMS 1468 -> 298 (0,203), 95-%-Spitze 9322 -> 1740 (0,187) — Echtzeit, darum nicht
  bitgleich. Werkzeug scratchpad `n30/laut/vergleich.py`, Laeufe `n30/laut/{land100,land50,rt100,rt20}`.
* Pruefung "fremde Prozesse": alle 707 Agenten-Laeufe seit 08:00 (debug.log im scratchpad) und 262 Test-Laeufe 16:00-17:10 in den
  Spurbaeumen liefen mit RE15_NOAUDIO oder CAP_SYNC (stumm), ausser einem Lauf 10:53 (int_kette) und meinen zwei Echtzeit-Laeufen
  17:32/17:33 (Messung B, waren hoerbar). Die Toene beim Nutzer kamen also aus seinem Spiel.
* Zwischenstand: der Mischweg skaliert JEDE Quelle, auch Tuersequenz und Wassersprung. Die Ursache der Beobachtung liegt woanders ->
  weiter (§6 unten).

(Abschnitte je Punkt: Messung vorher / RE-Belege / Umsetzung / Messung nachher / Tests / OFFEN.)
