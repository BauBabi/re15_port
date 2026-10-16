# Runde 38 — Vertrag der Spuren (verbindlich)

Basis: master nach v0.8.25 (Release 2077bb8e) + Runde-37-Doku (Nachtrag 14) + Auftrag Runde 38. Jede Spur arbeitet in
ihrem eigenen Arbeitsbaum `.claude/worktrees/r38_<thema>` (Zweig `r38/<thema>`). Wortlaut des Nutzers: `AUFTRAG.md`
(massgeblich). Regeln wie Runde 37 (`analysis/befunde_runde37/VERTRAG.md`): STOP-GATE, Beta -> Retail (wo RE1.5 unfertig
ist, gilt RE2; Sound ist RE2, `RE2_SOUND_LISTE.md` zuerst), KEINE volle Suite im Spurbaum (nur `ctest -R`), Dossier
fortlaufend committen, nur eigene Pfade adden. Elza-Punkte immer mit Elza messen (RE15_PSELECT_AUTO=1
RE15_PSELECT_AUTO_SWITCH=1); Elza-Raeume sind die ROOMxxx1-Varianten.

## 0. Spuren

| Spur | Thema / Baum | Punkte (AUFTRAG.md, Reihenfolge des Wortlauts) |
|---|---|---|
| A | cursor / r38_cursor | Cursor in ROOM1150 (Hebetisch) und ROOM11F0 (Panel) ohne den "komischen Schatten" (Original hat keinen) |
| B | szenenschutz / r38_szenenschutz | Leon wird in Szenen, in denen er sich nicht bewegen kann, nicht angegriffen (1190 Hunde, 11D0 Hunde); Ausnahme: Angriff ist Teil der Szene (Elza 1171) |
| C | werkstatt11b0 / r38_werkstatt11b0 | ROOM11B0 Marvin-Szene: Text "But, hold on...", Schiebe-Ton beim gemeinsamen Verschieben, keine Fall-Animation Marvins |
| D | zopf / r38_zopf | Elzas Zopf wie Claires Zopf in RE2 |
| E | elzaintro / r38_elzaintro | Elzas Intro mit demselben Voiceover wie Leon; im Schwarzbild des Pre-Intro-Texts scheint bei Elza der Raum nicht mehr durch |
| F | dining1141 / r38_dining1141 | Logikloch John ROOM1021 -> 1141 (Nachtrag 14 Runde 37 + Festlegungen 2026-10-16) und Rolltor-Ton ROOM1140 (derselbe Gemaelde-Schalter-Rolladen) |
| G | tagebuch / r38_tagebuch | Roys Tagebuch (Elza, ROOM1011, statt der Code-Nachricht; FILE16_title_paper; Code 0513 gruen) + Karte: Communications Room und Prison Room markieren (wie bei Leon) |
| H | elzatoene / r38_elzatoene | Elza: Schuss-Ton ROOM1041 (statt Platzhalter "gunshot"), Roys Zombie-Stoehnen beim Zugehen und Johns Schuss-Ton in ROOM1021 |
| I | heliport1171 / r38_heliport1171 | Elza bleibt in ihrer Szene ROOM1171 stecken |
| J | rekorder / r38_rekorder | Rekorder-Raetsel: Irons_Rekorder_Sound.wav beim Reinsprechen, danach Piep (erfolgreich) + Tuer-Ton + Erfolgsmeldung auf dem Panel |
| K | itemorte / r38_itemorte | Feuerloescher haengt in ROOM1030 an der Stelle aus fire_extinguisher.bmp (statt 1050); Parking Lot Key (Leon) / Prison Key (Elza) gut sichtbar an der Stelle aus key_location.bmp |
| M | npckante / r38_npckante | John (Kendo-Modell) springt am Ende ROOM10B1 und in ROOM1261 die Kanten hinunter; ebenso Marvin und Ada in Leons Szenario |
| N | wasser / r38_wasser | Wasser-Effekt (Bild und Ton), wenn Leon/Elza in ROOM1260 und folgenden Raeumen durch flaches Wasser gehen |
| O | montage / r38_montage | Leon, Szene nach Irons' Tod: alle durchbrechenden Zombies bewegen sich Richtung Kamera wie die hinter dem Rolltor |
| Q | kleinkram / r38_kleinkram | Inventar oeffnen mit RE2-Ton; Elza ROOM1151 Irons: nur Hinbeugen + "He has already passed..."; ROOM30E0 Flammenwerfer ohne "not available in this preview" aufnehmen |
| R | spinnen / r38_spinnen | Stage 2 Spinnen: kommen aus den Schaechten und schweben nicht; spucken Gift (Vergiftung -> Gegengift); Minispinnen laufen am Boden und greifen an |
| S | affen3 / r38_affen3 | Stage 3 kleine Affen treffen den Spieler wie im Original |
| T | spiderman / r38_spiderman | Stage 4 Spiderman kommen richtig aus den Schaechten; der Spieler verschwindet bei KEINEM Gegnerangriff von der Karte (Invariante fuer alle Gegner) |
| U | wasserzombie / r38_wasserzombie | Stage 2 Zombies im Wasser ziehen sich am Rand hoch wie in RE1.5 |
| V | releaseskript / r38_releaseskript | Ein Skript (plus Doppelklick-Datei) baut alle Plattformen, zaehlt die Version hoch, packt und bringt die alte Version ins Archiv |
| W | rolltor20a0 / r38_rolltor20a0 | Nachtrag 1: ROOM20A0 — der Rolladen, den man fuer das Kistenschieben oeffnen muss, steht im Port direkt vor der Kamera (Original nicht); Elza 20A1 mitpruefen |

Fortsetzungen aus Runde 37 (eigene Baeume, Runde-37-Orchestratoren, Dokumente unter `analysis/befunde_runde37/`):
**L** re2zombie (`r37_re2zombie`, Nachbesserung 2: Texte + Korrektur der Inversion re2z_hit_main case 3) und
**P** masken36 (`r37_masken36`, Nachbesserung 2: Cut 15 ROOM3050/3051, Kopfoberkante).

## 1. Ressourcen

### 1.1 Bank-9-Bits (gespeichert mit g_game.flags)
Bank 9 traegt im Original die ITEM-TAKEN-Bits; Port-Bits stehen im Riegel `tests/unit/test_r36_kartenbits.c` (k_port[]).
Frei in Original UND Port (2026-10-16 gegrept, selbst nachpruefen): 64, 66, 67, 68, 69, 70, 94, 95, 96, 97.
Zuteilung: **F = 64, 66, 67; G = 68; I = 69** (nur wenn noetig). 70 und 94-97 vergibt der Orchestrator auf Anfrage
(im Dossier begruenden). Jedes neu belegte Bit traegt die Spur in k_port[] ein (kleiner Haken, eine Zeile).

### 1.2 Nachrichten-Ids (Port-Text-Plaetze)
Port-Ids liegen hinter den Original-Nachrichten eines Raums; vorhandene Port-Ids per grep `MSG_ID` (include/, gen/)
ausschliessen. Zuteilung je Raum: **F**: 1011 Ids 45-49, 1021 40-44, 1131 40-44, 1141 40-49; **G**: 1011 40-44;
**I**: 1171 40-44; **J**: 1100/1101 40-44; **K**: 1030/1031 29-31 (Loescher, Form der Runde-37-Spur I) und 10F0/10F1
nur vorhandene Ids; **Q**: 1151/30E0/30E1 nur vorhandene Nachrichten (Ablauf/Weiche aendern), keine neuen.
Sprachdateien (synchro/) folgen der Message-Id (synchro/README.md) — eine Id, zu der der Nutzer schon eine WAV hat,
nie umwidmen.

### 1.3 Item-Ids / Dokumente
F: ein neuer Schluessel ("Dining Room Key") — freie RE1.5-Item-Id per Zensus wie Runde 36 Spur Q (§2.3: Klasse
"Schluessel" ab 0x22, nie platzierte Ids; 0x42/0x45 sind vergeben). G: Roys Tagebuch als Dokument — Dokument-Id per
Zensus der vorhandenen Port-Dokumente (dokumente_r34.c, irons_tisch_1150.c, aot_item_dokument). Beide Zensus im Dossier.

### 1.4 Gemeinsame Dateien (nur kleine Haken, 1-5 Zeilen, Kommentar "Runde 38 Spur X")
scd_room_setup.c (Installer-Zeile am ENDE des Blocks), scd_vm.c (Message_on-/Se_on-/Ereignis-Haken), platform/pc/main.c,
audio_pc.c, render_pc.c, game_step_common.c, enemy_ai_common.c (B, T), enemy_ai_re2_zombie.c (U; teilt die Datei mit
der Runde-37-Spur L — nur Haken, Logik in eigener Datei), enemy_ai_re2_spider.c (R), actor_locomotion.c / NPC-Code (M),
irons_tod_1150.c / zombieaktiv_r37.c (O), re15_inv_screen.c / inventory_common.c (Q), elza_r36.c (H; F/G nur Haken),
schluessel_r36.c + feuerloescher_r37.c (K), tools/r36_j/szene11b0_bauen.py + gen/szl11b0_szene.inc (C),
test_r36_kartenbits.c k_port[] (F, G, I). `release/` gehoert ausschliesslich Spur V; `platform/android/`,
`shared_assets/PSX/*` und `release/RELEASE_NOTES.md`: TABU fuer alle.

### 1.5 Ueberschneidungen (bekannt, bei der Integration aufzuloesen)
- ROOM1011: G (Tagebuch beim zweiten Untersuchen des liegenden Roy) und F (Schluessel an Roys Liegestelle NACH der
  1021-Szene) — verschiedene Phasen; beide Spuren lesen die Phase aus Original-Flags ((4,0xFA)/(3,0x5F)/(4,0xF0)).
- ROOM1021: F (Hinweis an Roys Leiche, msg 9) und H (Toene der Szene sub02) — verschiedene Stellen.
- ROOM1141/1171: F (John auf dem Dach, Vorauslaufen aufs Dach) und I (Elza steckt in 1171 fest) — I misst am
  heutigen Ablauf; F haelt den Uebergang 1141 -> 1171 lauffaehig (Riegel ueber den echten Weg).
- Prison Key / Communications Room: K (Ort des Schluessels) und G (Tagebuch nennt den Communications Room, Karte
  markiert ihn) — Inhalte muessen zusammenpassen; der Prison Key liegt im Communications Room.

## 2. Abnahme-Massstab
Je Nutzer-Punkt: am gebauten Spiel gemessen (Framedump RE15_WINDOW_SCALE=1, Ton-Mitschnitt RE15_AUDIO_CAP_SYNC, Logs),
Original-Beleg (Adresse/Bytes) oder NUTZER-VORGABE/PORT-WAHL mit belegter Form, mindestens ein messender Riegel je Punkt,
Nebenwirkungen auf beide Szenarien geprueft. Spur V: Trockenlauf + Versionslogik-Tests; den echten Release faehrt der
Orchestrator mit dem Skript.
