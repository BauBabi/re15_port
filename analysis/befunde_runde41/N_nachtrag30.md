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
