# Runde 39 Spur A "stimmen" — Dossier

Baum `.claude/worktrees/r39_stimmen`, Zweig `r39/stimmen`, Basis 3e49ff87 (master nach Release v0.8.26).
Start 2026-10-18 02:34; Fortsetzung nach Sitzungslimit ab ~07:10 (Vorgaenger-Commits 71182900, 336e78a8, bc3b3585).
Wortlaut: `AUFTRAG.md`, Zuteilung: `VERTRAG.md` (Spur A, Welle 1).

## Punkte (Wortlaut des Nutzers)
1. "In Room 1050 wird das Voiceover nicht abgespielt von vor dem Retten von Ada - das ich unter main09-main11 abgelegt
   habe. Also die Voiceovers für unsere neue Cutscene"
2. "Die Voiceovers für die letzte Cutscene in ROOM 1150, wenn Irons stirbt sind nicht drin...."
3. "In ROOM 11B0 bei der finalen Cutscene mit Ada und Marvin, fehlt das neue Voiceover von But hold on und I want to make
   sure we don't have any unexpected Visitors. Was dir eigentlich vorliegt."
4. "Bei "Alright, let's get down from here" fehlt aus irgendeinen Grund mal wieder das Voiceover."

## Stand / Fortschritt (neueste zuerst)
- 09:05 ABGESCHLOSSEN: alle vier Punkte gemessen vorher/nachher am echten Weg (§3/§5), Riegel gruen (§6: Unit 2/2,
  Integration 4/4, gezielte Module 93/93), Dossier vollstaendig; letzter Commit fix(r39-stimmen).
- 07:40 Fortsetzung: Vorgaenger-Laeufe ausgewertet (§3 vollstaendig, §5 nachher 1050/1150/11B0), Riegel gebaut
  (unit tabelle 143/143, lader 21/21), Integration (4 Teile, echte exe) gestartet; Ordner-Frage 11B0 geklaert (§4.3),
  Irons-Pegel gemessen (§8), "mal wieder" bisectiert (§3.4).
- 03:05 Vorgaenger: Code bc3b3585 (Tabelle + Haken) gebaut (local_build.sh build OK 03:03) und nachher gemessen
  (Laeufe n1050/n1150/n11b0 im Scratchpad, nicht mehr ins Dossier geschrieben — hier nachgetragen §5).
- 03:05 Messung vorher 1050/1150/11B0 (echte exe) + Zuordnung Datei -> Zeile (Spracherkennung + MFCC-DTW) + RE2-Beleg.
- 02:34 Dossier angelegt, Pflichtlektuere AUFTRAG.md + VERTRAG.md gelesen.

## 0. Gemeinsame Ursache (gemessen, siehe §1-§4)

Der Port spielt eine Sprachzeile ueber `scd_queue_voice(msg-id)` (scd_vm.c op_message_on, Z.1915/1917; auch der
Schreibmaschinen-Zweig) -> Ereignis SCD_AUDIO_VOICE_ON -> audio_pc.c Z.3759 `re15_voice_play(g_current_room_id,
evt.sample_id)` -> `re15_voice_load_clip` sucht `synchro/STAGE<n>/room<RAUM>/main<MSG-ID>.wav` (audio_pc.c Z.2246 ff.;
Regel synchro/README.md). Die Port-Szenen (adaruf_1050.c, irons_tod_1150.c, gen/szl11b0_szene.inc) laufen als
Programme derselben VM, ihr Message_on nimmt denselben Weg (im Log: `[msg] room=... id=22` + `[voice] ...`). Fehlt die
Datei, kehrte der Lader STILL zurueck (keine Logzeile). Die neuen Port-Szenen sprechen mit PORT-Nachrichten-Ids hinter
den Original-Nachrichten (1050: 22-24, 1150: 22-29, 11B0: 20-21) bzw. in 1171 mit Original-Id 3 — der Nutzer hat seine
Aufnahmen aber mit der NAECHSTEN FREIEN DATEINUMMER des Ordners in Szenenreihenfolge abgelegt (1050: nach main08 ->
main09-11; 1150: nach main14 -> main15-21; 11B0: nach main12 -> main13-14; 1171: die drei gesprochenen Zeilen 0/1/3 ->
main00-02). Keine dieser Dateien heisst wie die Message-Id ihrer Zeile -> der Lader findet nichts -> stumm.
Alle vier Dateisaetze kamen mit e2cc7673 (2026-10-16 16:15, Start Runde 38) ins Repo; Runde 38 hat sie nicht
zugeordnet (C_werkstatt11b0.md Z.37: "KEINE main20.wav"; I_heliport1171.md §7: "main02.wav gehoert zu msg 2", "fuer
msg 3 fehlt eine Aufnahme (main03.wav)"; re15_adaruf.h Z.99: "synchro/STAGE1/room1050/main22.wav .. main24.wav").

ZUSAETZLICHER DEFEKT (gemessen §3): in ROOM11B0 sind 13 und 14 ORIGINAL-Nachrichten (Untersuchungstexte, s.u.). Nach der
Dateiregel spielte der Port beim Untersuchen des Containers (sub03 @0x0126E/@0x01298 `2b 0d ff ff` = Message_on 13)
Marvins "But, hold on..." (main13.wav). Ebenso gehoert room1171/main02.wav nicht zu msg 2.

## 1. Zuordnung Datei -> Zeile (Messung, NUTZER-VORGABE = die Dateien selbst)

Werkzeuge (Scratchpad, nicht im Repo): Windows-Spracherkennung `System.Speech` en-US (MS-1033-80-DESK) — freies Diktat
und Zwangswahl unter den Kandidatensaetzen des Raums; MFCC(12)+DTW-Abstand jeder Datei gegen die Roh-Exporte des
Nutzers unter `synchro/unused/STAGE1/room<RAUM>/MiniMax_<Datum>_<Sprecher>.wav` (gleicher Satz: Abstand 1,26-1,85;
anderer Satz: >= 2,2). Kein Roh-Export ist bitgleich mit einer main-Datei (md5, Kreuzkorrelation <= 0,23) — es sind
eigene Takes; der DTW-Abstand ordnet sie trotzdem eindeutig dem gesprochenen SATZ zu.

| Raum | Datei | Dauer | Diktat (woertlich) | Zwangswahl / DTW-Nachbarn | Zeile (Text im Port) |
|---|---|---|---|---|---|
| 1050 | main09 | 4,284 s | "The lack any one place to our year" | "Hello? Anyone? Please, get me out of here!" 0,564; DTW Ada 22:34:51 1,367, 22:35:01 1,574, 22:35:12 1,630, 22:33:12 1,712 (Leon >= 2,36) | msg 22 "Woman: Hello? Anyone? Please, / get me out of here!" |
| 1050 | main10 | 1,869 s | "Another civilian survivor" | 0,935; DTW Leon 22:39:56 1,260, 22:40:14 1,344, 22:40:56 1,428 | msg 23 "Leon: Another civilian survivor." |
| 1050 | main11 | 1,358 s | "I have to help her" | 0,871; DTW Leon 22:42:04 1,507, 22:41:40 1,572, 22:42:42 1,572 | msg 24 "Leon: I have to help her!" |
| 1150 | main15 | 0,720 s | "So are" | DTW Leon 2026-10-14 23:09:46 1,428 (Irons >= 3,40) | msg 22 "Leon: Sir!" |
| 1150 | main16 | 5,747 s | "Sura the communication system can't be fixed \| to use the patrol car get out of your" | (kein Roh-Export) | msg 23 "Leon: Sir, the communication system / can't be fixed! We're going to use the / patrol car to get out of here." |
| 1150 | main17 | 2,183 s | "Our team get you, with many" | "I came to get you, come with me!" 0,679 | msg 24 "Leon: I came to get you, come with me!" |
| 1150 | main18 | 6,049 s | "Leon \| I1 proud to have an officer as dependable as you" | DTW Irons 15:11:18 1,444, 15:15:03 1,459, 15:14:21 1,473 | msg 25 "Irons: Leon... I... I'm proud to have an / officer as dependable as you!" |
| 1150 | main19 | 5,224 s | "But I I'm not going to make that I'm sorry" | — | msg 26 "Irons: But... I... I'm not going to make it... / I'm sorry..." |
| 1150 | main20 | 6,049 s | "Please one last thing to look after yourself on the all those who survived" | 0,892; DTW Irons 15:16:34 1,631, 15:16:47 1,707 | msg 27 "Irons: Please... one last favor... Look / after yourself and the others who / survived." |
| 1150 | main21 | 2,601 s | "Get them all of you Leon" | 0,765; DTW Irons 15:26:20 1,557 | msg 28 "Irons: Get them out of here... Leon..." |
| 1150 | — | | | | msg 29 "Leon: SIR, Sir?!" — KEINE Aufnahme (7 Dateien fuer 8 Zeilen) |
| 11B0 | main13 | 1,707 s | "What: a" | Zwangswahl "Hold on." 0,464; DTW Marvin 2026-10-15 16:45:42 1,686, 16:45:25 1,756, 15:59:15 1,849 (alle drei Diktat "But hold on"); alle 13 alten 11B0-Dateien >= 2,32 | msg 20 "Marvin: But, hold on..." |
| 11B0 | main14 | 3,936 s | "A lawyer jokes want to make sure we don't have an unwanted followers" | "I just want to make sure we don't have any unwanted followers." 0,690 | msg 21 "Marvin: I just want to make sure / we don't have any unwanted followers." |
| 1171 | main00 | 1,707 s | "Although curry OP" | "Elza, hurry up!" 0,320; DTW Kendo 19:01:52 1,510 | msg 0 "John: Elza, hurry up!" |
| 1171 | main01 | 2,902 s | "We needed a cult not" | 0,370; DTW Kendo 19:05:16 1,408, 19:04:08 1,614, 19:04:26 1,734 | msg 1 "John: We need to get this thing / off of here! Help me out!" |
| 1171 | main02 | 2,264 s | "The Whitney lot" | "Alright, let's get down from here!" 0,217 (Zweierwahl gegen "Come on, give me a hand!": 0,226); DTW Kendo 19:11:11 1,424, 19:09:42 1,651 — diese beiden Takes diktieren "All are littered on from here" / "They lied what good are for near" und waehlen "Alright, let's get down from here!" (0,411/0,522); KEIN Take spricht "C'mon, give me a hand!" | msg 3 "John: Alright, let's get down from here!" |

Wortlaut der Original-Nachrichten (RDT+0x3C, Glyphen = msg_common.c, Werkzeug scd_walk_lib.messages):
ROOM1050 msg 0..8 (8 = "Leon: Hey, wait!"); ROOM1150 msg 0..14; ROOM11B0 msg 0..15 mit **13 = "It doesn't seem like
you could move / this on your own."** und **14 = "The shutter is tightly sealed in place."**; ROOM1171 msg 0..4
(2 = "John: C'mon, give me a hand!", 3 = "John: Alright, let's get down from here!").
Message_on-Zensus ROOM1171 (scd_walk_lib.regionen): nur sub02 @0x014BA `2b 00 00 00`, @0x0151E `2b 01 00 00`, @0x01586
`2b 03 00 00` — msg 2 zeigt das Skript nie. ROOM11B0: sub03 @0x0126E/@0x01298 `2b 0d ff ff` (msg 13; Bedingungen davor:
Ck(3,131) @0x0124E, Member_cmp 0x0e @0x01256, Cmp(0,0,3|2) @0x01260/@0x0128A, Sce_key_ck(1,1) @0x0126A/@0x01294 = der
Untersuchungs-Griff am Container), main00 @0x0106C Text-Platz 7 msg 14 (`2c 07 01 31 ... 0e 00 ff ff`).

"I want to make sure we don't have any unexpected Visitors" (Nutzer-Wortlaut) = msg 21 / main14: die Aufnahme spricht
den Text der Szene ("... unwanted followers", Diktat oben), der Nutzer hat aus dem Gedaechtnis zitiert. Text bleibt.

## 2. RE-Beleg der FORM (RE2 = Ziel; RE1.5 hat keine englische Sprachausgabe)

RE2 Retail koppelt Stimme und Text NICHT ueber die Nummer: die Stimme startet ein eigener Befehl `xa_on` mit eigener
16-Bit-Stimmnummer, der Text danach `message_on` mit der Nachrichtennummer.
- `xa_on` (0x59) Handler @0x80058028 (Tabelle re2_scd_oplen.TAB[0x59]): @0x80058040 `lbu a0,1(v0)` (Kanal),
  @0x80058044 `lhu a1,2(v0)` (Stimmnummer = eigener Operand), @0x80058048 `jal 0x800129b4` (XA-Start), @0x80058070
  `addiu v0,v0,4` (PC+4).
- `message_on` (0x2B) Handler @0x80054A8C: @0x80054aa0 `lbu a2,2(v0)` (Nachricht), @0x80054aac `addiu v0,v0,6`,
  @0x80054ac0 `jal 0x8002fe38` (Text) — kein XA-Aufruf.
- Zensus info/re2leon/PL0/RDT (alle Raeume, Walker re2_scd_walk): 309 `xa_on`, 243 Paare `xa_on` -> naechstes
  `message_on`, davon nur 8 mit Stimmnummer == Nachrichtennummer. Beispiele ROOM1010 MAIN sub22 @0x02380
  `59 00 0a 00` (Stimme 10) -> @0x02384 `2b 00 04 00 00 00` (Text 4); ROOM1130 MAIN sub04 @0x01150 `59 00 15 00`
  (Stimme 21) -> @0x01154 `2b 00 11 00 00 00` (Text 17).
=> Eine Tabelle (Raum, Nachricht) -> Aufnahme ist die RE2-Form; die Ordnerregel "Message-Id = Datei"
(synchro/README.md) bleibt der Normalfall, die Tabelle traegt die Faelle, in denen der Nutzer anders nummeriert hat.
Die Zuordnung selbst ist NUTZER-VORGABE (seine Dateien, seine Reihenfolge, gemessen §1).

## 3. Messung vorher (echte exe, Baum 3e49ff87 + Dossier, gebaut 02:42)

Messlauf-Skript: Scratchpad `r39A/lauf.sh` (exe-Kopie `re15_pc_r39a_<tag>.exe` neben der exe, geerbte RE15_* entfernt),
Auswertung `r39A/capwin.py` / `capglob.py` (Mitschnitt RE15_AUDIO_CAP_SYNC, 1470 Takte je Spielbild; Bild f des letzten
Raum-Abschnitts = Takt (Takte - (Endbild+1)) + f; RMS je Fenster + max. normierte Kreuzkorrelation der Nutzer-Datei mit
dem Mitschnitt) sowie das im Repo liegende `probe_r39_stimmen_auswertung fit` (§6) mit denselben Ergebnissen.

### 3.1 P1 ROOM1050 (Lauf v1050, 35 s)
Weg = test_r35_inventar1050 Lauf A: Debug-Sprung ROOM1000 -> Tuer -> ROOM1050 -> zur 10A0-Tuer -> Quadrat.
`RE15_SET_FLAG=3:121 RE15_DEBUG_JUMP=1000@120 RE15_PLAYER_POS=22230,-13400,0,0 RE15_INPUT_SCRIPT=U0.8,W14
RE15_PRESS=square@150 RE15_EXIT_AT=600#1050`.
debug.log: `[adaruf] Ereignis 13: Szene startet`, `[msg] room=1050 id=22/23/24`, KEINE `[voice]`-Zeile.
state.log Message_on-Bilder: msg 22 F171, 23 F303, 24 F413.
Mitschnitt: F171..F300 rms 332, xkorr main09 0,029 | F303..F359 rms 396, xkorr main10 0,063 | F413..F454 rms 550,
xkorr main11 0,060 -> keine der drei Dateien liegt im Ton (fit: korr 0,011/0,007/0,019, Konsistenz 2/8, 1/3, 1/2).

### 3.2 P2 ROOM1150 (Lauf v1150, 56 s)
Weg = test_r35_cut1150 Lauf A: Karte probe_r35_cut1150_karte 1130 nach10f0 ersteszene, CONTINUE in ROOM1130, Aktion an
der Tuer -> ROOM1150 -> Szene. `RE15_EXIT_AT=1250#1150`.
debug.log: `[irons-tod] ROOM1150 Zustand 1: Szene startet (Programm 0)`, `[msg] room=1150 id=22..29`, KEINE `[voice]`.
Message_on-Bilder: 22 F26, 23 F212, 24 F333, 25 F413, 26 F543, 27 F673, 28 F793, 29 F923.
Mitschnitt xkorr (Datei gegen Zeilenfenster): main15 0,062 | main16 0,006 | main17 0,027 | main18 0,028 | main19
0,028 | main20 0,033 | main21 0,034 -> stumm (fit: korr 0,005..0,017, Median 0,00..0,50, Konsistenz 1/11..4/11).

### 3.3 P3 ROOM11B0 (Laeufe v11b0 85 s / v11b0lang bis ROOM1260)
Weg: Karte probe_r36_elza_karte 11c0 -25100 15000 1024 3:0x43 3:0x82 4:0xf3 (Leon, Strom an, 11C0 erledigt, sub04
gesehen), CONTINUE in ROOM11C0, `RE15_FIRE_AOT=0@60#11c0` = Tuer Slot 0 (@0x01712 -> ROOM11B0 Cut 1), Szene sub06 +
Erweiterung bis zum Abstieg (ROOM1260). debug.log: msg 1..12 je `[voice] clip loaded: main01..main12.wav`;
`[msgtext] room=11b0 id=20 "Marvin: But, hold on..."` (F2124..2218) und `id=21 "Marvin: I just want to make sure we
don't have any unwanted followers."` (F2678..2772) OHNE `[voice]`-Zeile.
Mitschnitt (v11b0lang, Abschnitt 11B0): main13 im msg-20-Fenster korr 0,049 (Konsistenz 1/3), main14 im msg-21-Fenster
korr -0,017 (4/7, Median -0,01); global (ganzer Mitschnitt) max. 0,239 / 0,102 = Rauschen. Gegenprobe im selben Lauf:
main12 (msg 12, Regel) korr 0,944 bei F2024 (+2 Bilder nach Message_on) -> das Mass trennt.

### 3.4 P4 ROOM1171 (Lauf v1171, Elza)
Weg = test_r38_heliport1171 weg_re2: Karte probe_r36_elza_karte 1131 2700 15300 1024 3:194 (Figur 4 = Elza, Log
`[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`), CONTINUE in ROOM1131, `RE15_INPUT_SCRIPT=A0.5` (Start 30) +
`RE15_FIRE_AOT=5@60#1171` (Tuer), `RE15_EXIT_AT=60#10b1`. debug.log: `[evt] F61 room=1171 Evt_exec sub=2`; msg 0
F510..751 `[voice] clip loaded: main00.wav` (Mitschnitt korr 0,967 bei F512); msg 1 F752..1062 `clip loaded:
main01.wav` (0,935 bei F754); **msg 3 F1063..1143 `[msg] room=1171 id=3` OHNE `[voice]`-Zeile**, main02 im Fenster
korr 0,004 (global max 0,104 = Rauschen); danach `[aot] DOOR FIRE slot=1` -> ROOM10B1.
"mal wieder" (Bisect-Frage): `git log --name-status -- synchro/STAGE1/room1171` = alle drei Dateien kamen mit e2cc7673
(2026-10-16), eine main03.wav gab es NIE; vor Runde 38 blieb Elzas Dachszene haengen (I_heliport1171.md §1: Mechanismus
seit 74121bb8 2026-07-06 unveraendert in v0.8.23/24/25) -> die Zeile spielte in KEINEM Stand, es gibt keinen Stand, von
dem aus zu bisecten waere. Der Nutzer hat die Zeile geliefert (main02 spricht sie, §1); die Release-Notes v0.8.26
Z.218-220 meldeten trotzdem "main02.wav ('C'mon, give me a hand!') spielt nie ... fuer 'Alright, let's get down from
here!' fehlt noch main03.wav" — Runde 38 Spur I hatte den Inhalt aus der DATEINUMMER geschlossen, ohne die Aufnahme zu
hoeren. "mal wieder" = die zweite Meldung desselben Defekts (Nummer != Message-Id), keine Regression.

## 4. Umsetzung (bc3b3585, Vorgaenger; in dieser Fortsetzung geprueft, nichts geaendert)

### 4.1 Dateien
- NEU `re15_port/include/re15_stimmen_r39.h` (Herleitung, Belege, API), NEU `re15_port/engine/src/stimmen_r39.c`:
  Tabelle `k_tabelle[]` (13 Zeilen: 1050 22/23/24 -> 9/10/11; 1150 22..28 -> 15..21; 11B0 20/21 -> 13/14; 1171 3 -> 2)
  mit je Zeile der Message_on-Stelle (k_ruf +0x1C/+0x50/+0x80 `2b 16|17|18 00 00`; irons_tod OP_MSG(22..29);
  szl11b0 +0x0008 `2b 14 00 00` / +0x010A `2b 15 00 00`; ROOM1171 @0x01586 `2b 03 00 00`) und dem gemessenen Inhalt
  der Datei (§1). `re15_stimmen_r39_stamm(raum, msg, stamm, n)`: 1 = umgeordnete Aufnahme (stamm = "main<datei>"),
  0 = Ordnerregel, -1 = SPERRE (main<msg>.wav gehoert laut Tabelle einer anderen Zeile -> stumm). Jede Entscheidung
  schreibt eine `[voice]`-Zeile; `re15_stimmen_r39_fehlt()` meldet "keine Aufnahme (<Pfad>)" — der Lader schweigt nicht
  mehr (§0).
- Haken `re15_port/platform/pc/src/audio_pc.c` re15_voice_load_clip (4 Zeilen, "Runde 39 Spur A"): nach der Runde-36-L-
  Sonderdatei und vor dem Runde-38-J-Rekorder: `if (re15_stimmen_r39_stamm(room, voice_id, stamm, sizeof stamm) < 0)
  return 0;` (Tabelle setzt den Stamm bzw. sperrt), nach beiden Pfadversuchen `if (!wav)
  re15_stimmen_r39_fehlt(...)`. Alle uebrigen Sonderwege (1011 main16.5, 1241 -> 1240, Rekorder 1100/1101) unberuehrt
  (Riegel §6: 20 "Regel bleibt"-Faelle).
- Kein Asset-Patch, keine Umbenennung der Nutzer-Dateien (VERTRAG §1.2); kein Env-Schalter; PSX-neutral (fprintf nur
  unter RE15_PLATFORM_PC).

### 4.2 Konstanten (jede mit Quelle)
| Konstante | Wert | Quelle |
|---|---|---|
| Nachrichten-Ids 1050 | 22/23/24 | re15_adaruf.h Z.102-104 (VERTRAG r34 §1.3 Spur D), k_ruf +0x1C/+0x50/+0x80 |
| Nachrichten-Ids 1150 | 22..29 | irons_tod_1150.c Z.106-134 OP_MSG (Programm 0 = k_p_szene) |
| Nachrichten-Ids 11B0 | 20/21 | re15_szenenleon_r36.h Z.69-70 (VERTRAG r36 §1.2 J 20..27), szl-Programm +0x0008/+0x010A |
| Nachrichten-Id 1171 | 3 | ROOM1171.RDT sub02 @0x01586 `2b 03 00 00`; Message_on-Zensus 0/1/3 |
| Dateinummern | 9-11, 15-21, 13-14, 2 | NUTZER-VORGABE: die Dateien e2cc7673, Inhalt gemessen §1 |
| Sperre 11B0 13/14 | -1 | ROOM11B0.RDT sub03 @0x0126E/@0x01298 `2b 0d ff ff`, main00 @0x0106C `2c 07 01 31` (Platz 7 msg 14) |
| Sperre 1171 2 | -1 | main02.wav spricht msg 3 (§1); msg 2 nie im Skript |
| Form (Tabelle statt Nummer) | — | RE2 xa_on @0x80058028, @0x80058044 `lhu a1,2(v0)` (§2) |

### 4.3 Ordner-Frage "room11B0 UND room11b0" (Einstiegspunkte des Auftrags)
Gemessen: `ls synchro/STAGE1 | grep -i 11b0` = NUR `room11B0`; `git ls-files synchro/STAGE1 | grep -i 11b0` = nur
`room11B0/main00..14.wav` (15 Dateien, md5 je Datei eindeutig, keine Doppel im ganzen Bestand 1050/1150/11B0/1171);
`fsutil file queryCaseSensitiveInfo synchro/STAGE1` = deaktiviert. Es gibt EINEN Ordner; `ls synchro/STAGE1/room11b0`
oeffnet auf NTFS denselben Ordner (die "zwei Ordner" im Auftrag sind ein Listing-Artefakt). Im Paket/unter Linux liegt
`room11B0` (so, wie git ihn traegt); der Lader sucht `room%04X` zuerst (audio_pc.c Z.2246) -> gefunden.

## 5. Messung nachher (echte exe = Code bc3b3585, Bau 03:03; Laeufe des Vorgaengers n1050/n1150/n11b0 + Integration §6)

Auswertung mit `probe_r39_stimmen_auswertung fit` (Abschnitt aus state.log, Fenster ab Message_on, lag -2..4):
korr = normierte Kreuzkorrelation Datei/Mitschnitt, faktor = Pegel gegenueber der Datei (Lader hebt leise Dateien bis
4x: audio_pc.c "Level-floor" target 14745 = 45 %, cap 4x), konsistenz = Segmente (0,5 s) mit Faktor im Band Median +-50 %.

### 5.1 P1 ROOM1050 (n1050, gleicher Weg wie §3.1)
debug.log: `[voice] ROOM1050 msg 22 <- main09.wav (Runde 39 Spur A, Nutzer-Reihenfolge)` + `clip loaded: main09.wav
(32000 Hz x1 -> 188928 @44100 Hz)`, ebenso 23 <- main10, 24 <- main11; Texte `[msgtext] id=22 "Woman: Hello? Anyone?
Please, get me out of here!"`, 23 "Leon: Another civilian survivor.", 24 "Leon: I have to help her!".
Mitschnitt: main09 ab F172 korr 0,994 faktor 1,97 Konsistenz 8/8 | main10 ab F304 0,981 / 1,20 / 3/3 | main11 ab F414
0,938 / 1,00 / 2/2 (Stimme rms 2909/2004/1474 gegen Rest 332/397/547 = +18,8/+14,1/+8,6 dB).

### 5.2 P2 ROOM1150 (n1150, gleicher Weg wie §3.2, EXIT 1500#1150)
debug.log: `[voice] ROOM1150 msg 22 <- main15.wav` ... `msg 28 <- main21.wav` je mit `clip loaded`; `[voice] ROOM1150
msg 29: keine Aufnahme (synchro/STAGE1/room1150/main29.wav) - Zeile bleibt stumm` (die Zeile, fuer die es keine Datei
gibt, ist jetzt im Log sichtbar). Message_on-Bilder 22 F26, 23 F212, 24 F386, 25 F466, 26 F649, 27 F807, 28 F990, 29 F1120.
Mitschnitt (fit, lag +1): main15 korr 0,999 faktor 1,27 (1/1) | main16 0,864 / 1,00 (11/11) | main17 0,702 / 1,18 (4/4)
| main18 0,294 / 4,00 Median 3,90 (11/11) | main19 0,243 / 3,88 Median 3,99 (10/10) | main20 0,285 / 4,12 Median 4,07
(12/12) | main21 0,927 / 4,00 (5/5). Irons' Zeilen 25-27 liegen unter der BGM MAIN1E (`[bgm] loaded: 7 VAGs, SEQ 888B`
nach msg 22): Stimme rms 513-579 gegen Rest rms 1669-2067 (-10 .. -12 dB) -> die niedrige korr ist die BGM, nicht ein
fehlender Clip (Faktor je 0,5-s-Segment konstant 3,1-4,8 ueber 10-12 Segmente; vorher §3.2 streut er um 0). Pegel
der Dateien: §8.

### 5.3 P3 ROOM11B0 (n11b0, gleicher Weg wie §3.3, EXIT 30#1260)
debug.log: `[voice] ROOM11B0 msg 20 <- main13.wav (Runde 39 Spur A, Nutzer-Reihenfolge)` + `clip loaded: main13.wav
(32000 Hz x1 -> 75264 @44100 Hz)`, `msg 21 <- main14.wav` + `clip loaded: main14.wav (... 173569 ...)`; msg 1..12
weiter `clip loaded: main01..12.wav` (14 Clips); keine Sperrzeile (msg 13/14 kommen in der Szene nicht vor).
Mitschnitt (Abschnitt 11B0): main13 ab F2126 korr 0,905 faktor 1,40 (3/3) | main14 ab F2680 0,957 / 1,44 (7/7) |
Regel-Gegenprobe main01 ab F28 0,924, main12 ab F2026 0,944 (wie vorher).

### 5.4 P4 ROOM1171 (Elza; Lauf der Integration §6.2, Teil 1171, gleicher Weg wie §3.4)
debug.log: `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`; msg 0 `clip loaded: main00.wav`, msg 1 `clip loaded:
main01.wav` (Ordnerregel, wie vorher); **`[msg] room=1171 id=3` -> `[voice] ROOM1171 msg 3 <- main02.wav (Runde 39 Spur
A, Nutzer-Reihenfolge)` + `clip loaded: main02.wav (32000 Hz x1 -> 99841 @44100 Hz)`**; keine Zeile fuer msg 2; danach
`[aot] DOOR FIRE slot=1` -> ROOM10B1 (Abschnitt 10B1, EXIT 60).
Mitschnitt (Abschnitt 1171, 1144 Bilder): main02 ab F1065 (lag 2) korr 0,933 faktor 1,49 Konsistenz 4/4 — der Clip
(68 Bilder) endet bei ~F1133, VOR dem Tuer-AOT (nach F1143): die Zeile spielt ganz; main00 ab F512 0,967, main01 ab
F754 0,935 (wie vorher).

## 6. Tests (Registrierung NUR tests/unit/probes/r39_stimmen.cmake)

### 6.1 Unit (gruen, 07:35)
- `unit_r39_stimmen_tabelle` (tests/unit/test_r39_stimmen.c): 143 Pruefungen — Tabelle 13 Zeilen, jede Datei vorhanden
  (RIFF/WAVE), keine Doppel; Message_on-Stellen aus den LAUFENDEN Programmen (`re15_adaruf_programm` +0x1C/+0x50/+0x80,
  `re15_irons_tod_programm(0)` OP_MSG 22..29 je einmal in Reihenfolge, `re15_szl_programm` +0x0008/+0x010A) und den RDTs
  (ROOM1171 @0x014BA/0x0151E/0x01586, ROOM11B0 @0x0126E/0x01298/0x0106C); Zuordnung je Punkt; Sperre (11B0 13/14,
  1171 2, 1050 9/11, 1150 15/21 -> -1, Stamm unveraendert); Regel bleibt fuer 20 Faelle (11B0 1/12/22, 1050 8/25,
  1150 2/14/29, 1170 0/3, 1171 0/1, 1240 0, 1241 3, 1011 16, 1141 42, 1100 40, 1051 22, 1151 22, 11B1 20).
- `unit_r39_stimmen_lader` (tests/unit/probe_r39_stimmen_lader.c, ECHTER audio_pc.c + RE15_AUDIO_CAP_SYNC ohne SDL-
  Geraet): 21 Pruefungen — je Tabellenzeile `re15_audio_stimme(Raum, Id)` -> Mitschnitt traegt die Nutzer-Datei
  (korr 1,000; Faktor 1,00-1,97 Leon/Ada/Marvin/John, 4,00 Irons = Deckel); 1150 msg 29 stumm; 11B0 msg 13/14 stumm;
  1171 msg 2 stumm (Gegenprobe korr 0,000); Regel 11B0 msg 1, 1171 msg 0, 1170 msg 3 (Leon) korr 1,000.
  `ctest -R "unit_r39_stimmen"`: `100% tests passed, 0 tests failed out of 2`.
- `probe_r39_stimmen_auswertung` (Werkzeug, kein Test): fit/rms eines Mitschnitts (§5); gegen die Vorher-Laeufe
  kalibriert (vorher korr <= 0,05, Konsistenz <= 4/11; nachher korr >= 0,70 ohne BGM bzw. Konsistenz >= 10/10 unter BGM).

### 6.2 Integration (echte exe am echten Weg; tests/integration/test_r39_stimmen.cmake, 4 Teile)
Schwellen: `[voice] ROOM.. msg N <- mainNN.wav`-Zeile genau einmal + `clip loaded` + `[msgtext]`-Wortlaut; Mitschnitt je
Zeile korr >= 0,5 (1150: >= 0,12 wegen BGM) UND Pegelfaktor-Median 0,5..4,5 UND Segment-Konsistenz >= 60 %. Keine
geerbten RE15_*-Variablen (werden im Skript geloescht), exe-Kopie `re15_pc_r39a_<teil>_<id>.exe`, eigener Laufordner.
Ergebnis (ctest -R "integration_r39_stimmen", echte exe, 07:55-08:20; Werte aus den STATUS-Zeilen der Laeufe):
- `integration_r39_stimmen_1050` Passed 35,07 s: `main09.wav@F171 korr 0.994 median 1.97 kons 8/8; main10.wav@F303 korr
  0.981 median 1.20 kons 3/3; main11.wav@F413 korr 0.938 median 1.00 kons 2/2`; Zuordnungszeilen 22/23/24 je genau einmal,
  `[msgtext]` 22/23/24 im Wortlaut.
- `integration_r39_stimmen_1150` Passed 56,36 s: `main15.wav@F26 korr 0.999 median 1.27 kons 1/1; main16.wav@F212 korr
  0.864 median 1.00 kons 11/11; main17.wav@F386 korr 0.702 median 1.21 kons 4/4; main18.wav@F466 korr 0.294 median 3.90
  kons 11/11; main19.wav@F649 korr 0.243 median 3.99 kons 10/10; main20.wav@F807 korr 0.285 median 4.07 kons 12/12;
  main21.wav@F990 korr 0.927 median 4.01 kons 5/5`; msg 29 "keine Aufnahme" genau einmal; Texte 22/25/28/29.
- `integration_r39_stimmen_11b0` Passed 102,39 s: `main13.wav@F2124 korr 0.905 median 1.40 kons 3/3; main14.wav@F2678 korr
  0.957 median 1.44 kons 7/7; main01.wav@F26 korr 0.924 median 1.00 kons 8/8; main12.wav@F2024 korr 0.944 median 1.13
  kons 5/5`; keine Sperrzeile; Texte 20/21.
- `integration_r39_stimmen_1171` Passed 58,17 s (Elza PL04): `main02.wav@F1063 korr 0.933 median 1.50 kons 4/4;
  main00.wav@F510 korr 0.967 median 1.16 kons 3/3; main01.wav@F752 korr 0.935 median 1.30 kons 5/5`; `[msg] room=1171
  id=3` vorhanden, keine Anforderung von msg 2.
(Erster Lauf des Skripts scheiterte an sich selbst: CMake zerlegt ";" in Listenelementen -> Trenner "|"; Original-
Nachrichten loggen kein `[msgtext]` -> fuer 1171 zaehlt `[msg] id=3`, der Wortlaut steht im Unit-Riegel.)

### 6.3 Gezielte Tests der beruehrten Module (statt voller Suite, VERTRAG)
`ctest --test-dir re15_port/build -R "r39_stimmen|synchro_stimme|voice_ausreden|audio_sperren|r30_tuer_stimme|
r34n_f_leiche_stimme|r36_elza_|r38_elzaintro|r38_rekorder|r38_werkstatt11b0|r38_heliport1171|r36_szenenleon|
r35_cut1150|r36_szene1150|r34n_d_adaruf|r35_inventar1050|r27_ada_cutscene_1050|ada_eintritt_1050|r38_kleinkram_irons"
--timeout 1500` (93 Tests, davon 21 Integration; Lauf 08:25-09:02, einzeln/sequenziell, keine geerbten RE15_*):
**`100% tests passed, 0 tests failed out of 93`** (Total Test time (real) = 2219.39 sec). Kein Flattern, keine
Wiederholung noetig. Keine volle Suite im Spurbaum (VERTRAG); die faehrt der Orchestrator in der Integration.

## 7. OFFEN
- (keins fuer die vier Punkte; Nutzer-Fragen §8)

## 8. Fuer den Nutzer
- Neue Assets: keine (keine Aenderung unter synchro/ oder shared_assets/; Paket-Gate unveraendert). Sprachdateien: keine
  neuen; die vorhandenen room1050/main09-11, room1150/main15-21, room11B0/main13-14, room1171/main00-02 spielen jetzt
  an ihren Zeilen (Tabelle im Code, Dateien unveraendert).
- ROOM1150: fuer die achte Zeile der Sterbeszene, msg 29 "Leon: SIR, Sir?!", liegt KEINE Aufnahme vor (7 Dateien fuer 8
  Zeilen); eine Datei `synchro/STAGE1/room1150/main29.wav` wuerde nach der Ordnerregel sofort spielen.
- ROOM1150 Pegel: Irons' neue Zeilen (main18-21, rms 0,0040-0,0046 Vollaussteuerung) sind ~22 dB leiser als Leons
  (main15-17: 0,055-0,061) — wie schon seine alten Zeilen (main05/07/09/12/13: 0,0054-0,0062). Der Port hebt leise
  Aufnahmen hoechstens 4x (+12 dB) an (audio_pc.c "Level-floor", seit 2026-08); in der Sterbeszene liegt Irons damit
  ~10 dB unter der BGM MAIN1E (gemessen §5.2). Die Zeilen sind drin, aber leise. ENTSCHEIDUNGSFRAGE: Irons lauter
  exportieren (dann greift die Anhebung nicht mehr), oder soll der Port die Anhebung fuer Irons' Dateien ueber 4x
  hinaus erlauben (Port-Wahl, Zahl waere zu bestimmen)?
- ROOM11B0: msg 21 spricht "I just want to make sure we don't have any unwanted followers." (so der Szenentext und die
  Aufnahme main14) — nicht "unexpected visitors" (Gedaechtniszitat).
- ROOM1171: "Alright, let's get down from here!" ist Ihre main02.wav (die Release-Notes v0.8.26 hatten sie faelschlich
  als "C'mon, give me a hand!" gefuehrt); "C'mon, give me a hand!" (msg 2) zeigt das Original-Skript nie — keine
  Aufnahme noetig.
- Bedienung: keine Aenderung. Jede Sprachzeile ohne Datei steht jetzt im debug.log (`[voice] ROOM.... msg N: keine
  Aufnahme (<Pfad>)`), Umordnungen als `[voice] ROOM.... msg N <- mainNN.wav (Runde 39 Spur A, Nutzer-Reihenfolge)`.

## 9. Nachbesserung 1 (Abnahme 0 bestanden bb285672; Nutzer-Nachtrag 2, AUFTRAG.md 2026-10-18 09:48)

Fortsetzung ab 09:50 im selben Baum (HEAD 275a322c = Merge master 32ab1f97 mit main22.wav + Nachtrag 2).
Drei Maengel/Auftraege der Abnahme (A_abnahme_0.md §6.2 + Orchestrator):
- M1 NUTZER-VORGABE "er Port kann die Stimme ruhig lauter anheben das es passt." -> Port-Anhebung ueber den 4x-Deckel
  hinaus; FORM aus dem Bestand gemessen (§9.1-§9.4).
- M2 NUTZER-VORGABE "das SIR, sir! habe ich ... mit main22 ergänzt." -> ROOM1150 msg 29 <- main22.wav (§9.5).
- M3 "Ändere es zu unwanted followers im Text bei 11B0." -> Text ist schon so; mit [msgtext]-Zeile belegt (§9.6).

### 9.0 Stand / Fortschritt (neueste zuerst)
- 10:35 Konstanten korrigiert (T 1163 -> 985: Riegel am Bestand kippte room2000/main02 um ein LSB; C 21759, R 2040),
  Dossier §9.1/§9.2/§9.6 geschrieben; Integration (4 Teile) laeuft mit dem T=1163-Bau (Irons-Zeilen davon unberuehrt).
- 10:25 Code gebaut (5a402341 + Kommentar-Fix), Unit: tabelle 14 Zeilen gruen, lader gruen (Irons 7,4-12,6x), pegel ROT
  (room2000/main02 an der Schwelle, s. 10:35).
- 09:56 Zensus des Bestands gestartet, Spracherkennung main22 gestartet, Dossier-Abschnitt angelegt.

### 9.1 M1 — Zensus des Bestands: die FORM der Anhebung wird gemessen, nicht geraten

Werkzeug (Scratchpad, nicht im Repo): `r39A/zensus.py` (numpy) ueber ALLE 216 Dateien `synchro/STAGE<n>/room<RAUM>/
main<NN>.wav` (ohne `synchro/unused/`): Rate/Kanaele/Bits, Dauer, Spitze |x|max, Effektivwert rms (ganze Datei),
aktiver rms (20-ms-Fenster > -30 dB unter dem lautesten Fenster), Rauschteppich (10. Perzentil der 20-ms-Fenster),
Crest = Spitze/rms, und die Stufe 1 des Laders nachgerechnet (audio_pc.c re15_voice_load_clip "Level-floor": Ziel
14745 = 45 % Spitze, Deckel 4x; eingefuehrt mit aa346af1 2026-06-28, Kommentar dort: "cap the gain so near-silent
clips don't blow up their noise floor"). `r39A/ableiten.py` leitet daraus die Konstanten ab. Tabelle: `r39A/zensus.csv`.

Befund — der Bestand ist ZWEIKLASSIG (Werte in Vollaussteuerung FS):
| Klasse | Dateien | rms | Spitze | Crest | Stufe 1 | rms nach Stufe 1 |
|---|---|---|---|---|---|---|
| auf Pegel | 206 (alle Sprecher ausser Irons, alle Raeume) | 0,034-0,092 | 0,21-0,66 | 4,5-13,4 | 1,00-2,11x (nie am Deckel) | 0,0355-0,0995 |
| leise | 10 = ALLE Irons-Dateien room1150 main02/05/07/09/12/13/18/19/20/21 | 0,0040-0,0062 | 0,053-0,096 | 12,5-20,6 | alle 4,00x (Deckel) | 0,0159-0,0247 |
Zwischen den Klassen liegt eine Luecke: 0,0247 (Irons main05 nach Stufe 1) .. 0,0355 (room2000/main02). Keine Datei der
normalen Klasse beruehrt den 4x-Deckel; genau die zehn Irons-Dateien bleiben an ihm haengen.

Irons' neue Zeilen (Sterbeszene) im Einzelnen (32000 Hz mono 16 bit):
| Datei | Dauer | Spitze | rms | Crest | |x| 99,9 % | Stufe 1 -> rms | F0 |
|---|---|---|---|---|---|---|---|
| main18 | 6,049 s | 0,0528 (bei 2,96 s) | 0,00398 | 13,3 | 0,0273 | 4,00x -> 0,0159 | 89 Hz |
| main19 | 5,224 s | 0,0746 (bei 4,25 s) | 0,00415 | 18,0 | 0,0377 | 4,00x -> 0,0166 | 89 Hz |
| main20 | 6,049 s | 0,0898 (bei 2,97 s) | 0,00436 | 20,6 | 0,0385 | 4,00x -> 0,0175 | 115 Hz |
| main21 | 2,601 s | 0,0574 (bei 0,81 s) | 0,00459 | 12,5 | 0,0358 | 4,00x -> 0,0184 | 121 Hz |
Leons Zeilen derselben Szene: main15 rms 0,0612 Spitze 0,356 (Stufe 1 1,26x), main16 0,0560 / 0,549 (1,00x), main17
0,0553 / 0,406 (1,11x), main22 0,0607 / 0,418 (1,07x). Die Spitze der Irons-Dateien ist ein EINZELNER Transient: der
99,9-%-Wert von |x| liegt bei 0,027-0,039, die Spitze 2-3x darueber — darum entscheidet bei ihnen die Spitzenbegrenzung,
nicht das Lautheitsziel (§9.3).

Rauschteppich (10. Perzentil der 20-ms-Fenster): Irons' Dateien 0,00046-0,00061 FS — dasselbe ABSOLUTE Niveau wie Leons
Dateien (0,0003-0,0013); Irons' Sprache liegt also 22 dB tiefer im selben Rauschen (SNR ~28 dB statt ~47 dB). Nach der
Anhebung (6,9-12,6x) liegt sein Teppich bei 0,0034-0,0077 FS (-49 .. -42 dBFS); in der Sterbeszene 16-20 dB unter der
BGM (rms 0,05-0,08 FS, §9.2). Das ist der gemessene Preis der NUTZER-VORGABE; der alte Deckel-Grund ("noise floor") ist
damit beziffert, nicht ignoriert.

Herleitung der drei Konstanten (alle aus dem Bestand; Werte der 16-bit-Skala, wie der Lader sie ganzzahlig rechnet —
Riegel unit_r39_stimmen_pegel druckt sie mit):
- T (Schwelle "zu leise") = 985 (0,0301 FS) = MITTE DER LUECKE zwischen den Klassen: 808 (room1150/main05 nach Stufe 1,
  lauteste Datei der leisen Klasse) .. 1162 (room2000/main02, leiseste Datei auf Spitzenpegel); zu beiden 18 % Abstand.
  Erste Herleitung war T = 1163 = der Randwert selbst (Zensus-Gleitkomma 0,03550 x 32768 = 1163,3): der Riegel am
  Bestand zeigte sofort, dass die ganzzahlige Rechnung des Laders fuer room2000/main02 1162 liefert — ein LSB unter der
  Schwelle, die Datei (auf Pegel!) waere mit 1,47x angehoben worden. Eine Schwelle gehoert in die Luecke, nicht auf
  ihren Rand (das Resampling auf Geraeterate verschiebt rms um < 1 %, die Luecke traegt 18 %).
- R (Ziel) = 2040 (0,0623 FS) = Median des rms der 206 Dateien auf Spitzenpegel (Quartile 0,0518 / 0,0624 / 0,0719;
  Leons 1150-Zeilen 0,056-0,077 liegen mittendrin) — "Mitte des Bestands", dieselbe Idee wie das Ziel der Stufe 1
  ("~45% full-scale, matches the loud clips"). Am heutigen Bestand entscheidet R nicht (alle zehn Dateien laufen vorher
  in C); R traegt kuenftige leise Dateien mit normalem Crest.
- C (Spitzen-Deckel) = 21759 (0,664 FS) = Spitze der lautesten gelieferten Datei room1240/main05.wav (48 kHz 24 bit
  stereo, Leons Vorspann): kein angehobener Clip wird in der Spitze lauter als das, was der Port heute schon ungeklemmt
  mit BGM mischt. Der Mischer (audio_pc.c :482-486) summiert die Stimme mit cd_vol 0x3FFF>>14 = 1,0 (RE2 CD_initvol) und
  klemmt erst die SUMME auf int16 — darum ist "Spitze <= 32767 je Datei" KEIN Schutz vor Uebersteuerung, C muss Platz
  fuer die BGM lassen (§9.2).
Vorschau der Regel (ableiten.py) = die zehn Irons-Dateien, alle Spitzen-begrenzt: main02 9,3x, main05 6,9x, main07
7,7x, main09 7,0x, main12 7,5x, main13 9,1x, main18 12,6x, main19 8,9x, main20 7,4x, main21 11,6x (rms danach
0,032-0,053 FS).

### 9.2 M1 — BGM MAIN1E in der Sterbeszene (Mitschnitt lauf_1150_cd62ee6a, VOR der Aenderung; `r39A/bgm_spitze.py`)
Abschnitt ROOM1150 (1251 Bilder): max |x| 18246 = 0,557 FS, KEIN Abtastwert am Anschlag, rms 1946. Luecken zwischen den
Zeilen (nur BGM/SE, kein Clip aktiv): F452-465 Spitze 0,105 FS rms 0,024 | F630-648 0,135 / 0,038 | F790-806 0,276 /
0,079 | F970-989 0,201 / 0,064 | F1100-1119 0,481 / 0,123 (Schwell vor msg 29). Zeilenfenster mit Irons (4x-Stand):
F466-559 Spitze 0,290, F649-739 0,423, F807-899 0,412. Mit C = 0,664 FS bleibt die Summe Stimme + BGM in den
Irons-Fenstern unter 0,664 + 0,276 = 0,94 FS — die Integration zaehlt die geklemmten Werte je Zeilenfenster (muss 0 sein).

### 9.6 M3 — ROOM11B0 msg 21: "unwanted followers" steht schon so im Text (nichts geaendert)
Nutzer: "Ändere es zu unwanted followers im Text bei 11B0." Der Szenentext ist bereits so: gen/szl11b0_szene.inc:134
`/* msg 21 Marvin: I just want to make sure / we don't have any unwanted followers. */` (Quelle tools/r36_j/
szene11b0_bauen.py:286 — die Datei liegt nicht im Baum, nur ihr Erzeugnis). Beleg aus einem Lauf der echten exe
(lauf_11b0_b1551704, debug.log Z.564/566, RE15_MSG_LOG):
`[msgtext] room=11b0 id=21 "Marvin: I just want to make sure we don't have any unwanted followers."` gefolgt von
`[voice] ROOM11B0 msg 21 <- main14.wav (Runde 39 Spur A, Nutzer-Reihenfolge)`; Z.501/503: `[msgtext] room=11b0 id=20
"Marvin: But, hold on..."` + `msg 20 <- main13.wav`. Die Aufnahme main14 spricht denselben Wortlaut (Dossier §1,
Abnahme §3: Kandidat "unexpected visitors" angeboten und nicht gewaehlt). Riegel: integration_r39_stimmen_11b0 prueft
den Wortlaut von msg 20/21 im Log (unveraendert gruen).
