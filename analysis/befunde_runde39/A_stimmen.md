# Runde 39 Spur A "stimmen" — Dossier (laufend)

Baum `.claude/worktrees/r39_stimmen`, Zweig `r39/stimmen`, Basis 3e49ff87 (master nach Release v0.8.26).
Start 2026-10-18 02:34 (Uhr). Wortlaut: `AUFTRAG.md`, Zuteilung: `VERTRAG.md` (Spur A, Welle 1).

## Punkte (Wortlaut des Nutzers)
1. "In Room 1050 wird das Voiceover nicht abgespielt von vor dem Retten von Ada - das ich unter main09-main11 abgelegt
   habe. Also die Voiceovers für unsere neue Cutscene"
2. "Die Voiceovers für die letzte Cutscene in ROOM 1150, wenn Irons stirbt sind nicht drin...."
3. "In ROOM 11B0 bei der finalen Cutscene mit Ada und Marvin, fehlt das neue Voiceover von But hold on und I want to make
   sure we don't have any unexpected Visitors. Was dir eigentlich vorliegt."
4. "Bei "Alright, let's get down from here" fehlt aus irgendeinen Grund mal wieder das Voiceover."

## Stand / Fortschritt (neueste zuerst)
- 03:05 Messung vorher 1050/1150/11B0 (echte exe) + Zuordnung Datei -> Zeile (Spracherkennung + MFCC-DTW) + RE2-Beleg.
- 02:34 Dossier angelegt, Pflichtlektuere AUFTRAG.md + VERTRAG.md gelesen.

## 0. Gemeinsame Ursache (gemessen, siehe §1-§4)

Der Port spielt eine Sprachzeile ueber `scd_queue_voice(msg-id)` (scd_vm.c op_message_on) -> `re15_voice_play(raum,
msg-id)` -> `re15_voice_load_clip` sucht `synchro/STAGE<n>/room<RAUM>/main<MSG-ID>.wav` (audio_pc.c ~Z.2241). Fehlt die
Datei, kehrt der Lader STILL zurueck (keine Logzeile). Die neuen Port-Szenen sprechen mit PORT-Nachrichten-Ids hinter
den Original-Nachrichten (1050: 22-24, 1150: 22-29, 11B0: 20-21) bzw. in 1171 mit Original-Id 3 — der Nutzer hat seine
Aufnahmen aber mit der NAECHSTEN FREIEN DATEINUMMER des Ordners in Szenenreihenfolge abgelegt (1050: nach main08 ->
main09-11; 1150: nach main14 -> main15-21; 11B0: nach main12 -> main13-14; 1171: die drei gesprochenen Zeilen 0/1/3 ->
main00-02). Keine dieser Dateien heisst wie die Message-Id ihrer Zeile -> der Lader findet nichts -> stumm.
Alle vier Dateisaetze kamen mit e2cc7673 (2026-10-16 16:15, Start Runde 38) ins Repo; Runde 38 hat sie nicht
zugeordnet (C_werkstatt11b0.md Z.37: "KEINE main20.wav"; I_heliport1171.md §7: "main02.wav gehoert zu msg 2", "fuer
msg 3 fehlt eine Aufnahme (main03.wav)").

ZUSAETZLICHER DEFEKT (gemessen §3): in ROOM11B0 sind 13 und 14 ORIGINAL-Nachrichten (Untersuchungstexte, s.u.). Nach der
Dateiregel spielt der Port beim Untersuchen des Containers (sub03 @0x0126E/@0x01298 `2b 0d ff ff` = Message_on 13)
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
Message_on-Zensus ROOM1171 (scd_walk_lib.regionen): nur sub02 @0x014BA `2b 00`, @0x0151E `2b 01`, @0x01586 `2b 03` —
msg 2 zeigt das Skript nie. ROOM11B0: sub03 @0x0126E/@0x01298 `2b 0d ff ff` (msg 13), main00 @0x0106C Text-Platz 7
msg 14 (`2c 07 01 31 ... 0e 00 ff ff`).

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
Auswertung `r39A/capwin.py` (Mitschnitt RE15_AUDIO_CAP_SYNC, Bild f des letzten Raum-Abschnitts = Takt
(Takte - (Endbild+1)) + f; RMS je Fenster + max. normierte Kreuzkorrelation der Nutzer-Datei mit dem Mitschnitt).

### P1 ROOM1050 (Lauf v1050, 35 s)
Weg = test_r35_inventar1050 Lauf A: Debug-Sprung ROOM1000 -> Tuer -> ROOM1050 -> zur 10A0-Tuer -> Quadrat.
`RE15_SET_FLAG=3:121 RE15_DEBUG_JUMP=1000@120 RE15_PLAYER_POS=22230,-13400,0,0 RE15_INPUT_SCRIPT=U0.8,W14
RE15_PRESS=square@150 RE15_EXIT_AT=600#1050`.
debug.log: `[adaruf] Ereignis 13: Szene startet`, `[msg] room=1050 id=22/23/24`, KEINE `[voice]`-Zeile.
state.log Message_on-Bilder: msg 22 F171, 23 F303, 24 F413.
Mitschnitt: F171..F300 rms 332, xkorr main09 0,029 | F303..F359 rms 396, xkorr main10 0,063 | F413..F454 rms 550,
xkorr main11 0,060 -> keine der drei Dateien liegt im Ton.

### P2 ROOM1150 (Lauf v1150, 56 s)
Weg = test_r35_cut1150 Lauf A: Karte probe_r35_cut1150_karte 1130 nach10f0 ersteszene, CONTINUE in ROOM1130, Aktion an
der Tuer -> ROOM1150 -> Szene. `RE15_EXIT_AT=1250#1150`.
debug.log: `[irons-tod] ROOM1150 Zustand 1: Szene startet (Programm 0)`, `[msg] room=1150 id=22..29`, KEINE `[voice]`.
Message_on-Bilder: 22 F26, 23 F212, 24 F333, 25 F413, 26 F543, 27 F673, 28 F793, 29 F923.
Mitschnitt xkorr (Datei gegen Zeilenfenster): main15 0,062 | main16 0,006 | main17 0,027 | main18 0,028 | main19
0,028 | main20 0,033 | main21 0,034 -> stumm.

### P3 ROOM11B0 (Lauf v11b0, 85 s)
Weg: Karte probe_r36_elza_karte 11c0 -25100 15000 1024 3:0x43 3:0x82 4:0xf3 (Leon, Strom an, 11C0 erledigt, sub04
gesehen), CONTINUE in ROOM11C0, `RE15_FIRE_AOT=0@60#11c0` = Tuer Slot 0 (@0x01712 -> ROOM11B0 Cut 1), Szene sub06 +
Erweiterung. debug.log: msg 1..12 je `[voice] clip loaded: main01..main12.wav`; `[msg] room=11b0 id=20` (F2124) und
`id=21` OHNE `[voice]`-Zeile.

## OFFEN (laufend)
- P3 Mitschnitt msg 20/21, P4 Messung vorher (Elza, 1131 -> 1171), Bisect-Frage "mal wieder" (P4).
