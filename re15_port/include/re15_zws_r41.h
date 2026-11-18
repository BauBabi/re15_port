/*
 * RE1.5 Rebuilt — Runde 41, Zwischensequenzen mit den Gorilla-Filmen des Nutzers (ein Modul, eine Tabelle, zwei Filme):
 *   114  Besprechungsraum, Elzas Szenario ROOM1141, John und Elza  -> MOVIE/R114GORI.STR
 *   11C  Parkplatz/Garage,  Leons Szenario ROOM11C0, Leon und Ada   -> MOVIE/R11CGORI.STR
 *   birk6000  Bahnsteig ROOM6000, Leons erstes Treffen mit Annette    -> MOVIE/R409BIRK.STR (RE2-Film "Birkins Mutation",
 *             Runde 42 Strang birkinfilm, include/re15_birkinfilm_r42.h; Blendenart RE15_ZWS_BLENDART_4090, Bild 40 Zeilen tiefer)
 *   birk5011  Monitorraum ROOM5011, Elza zwischen msg 3 und msg 4    -> derselbe Film (Runde 42 birkinfilm Stufe 2)
 * Je Film: Gorilla-Schrei vor der "Was war das?"-Zeile, Tonende + kurze Pause, die Zeile, kurz warten, harter Schnitt in
 * den Film, danach 15 Bilder Einblenden aus Schwarz wie RE2 und der Dialog laeuft weiter. Code: engine/src/zws_r41.c
 * (Tabelle k_filme), platform/pc/src/zws_r41_pc.c (Ton + Filmspieler).
 *
 * ZWEI ARTEN VON HAKEN-STELLEN (re15_zws_stelle_t):
 *   PORT-ZEILE  `2b <id> 00 00` in einem Port-Programm (Message_on-Form ohne Text). Haken in op_message_on HINTER dem
 *               Stimmen-Riegel (scd_vm.c "DEN VORIGEN SATZ AUSREDEN LASSEN"); fertig -> die Zeile ist verbraucht (pc += 4).
 *               Raum 114: die Szene laeuft als Port-Programm ENTRIEGELN39, die Zeilen 61/60 sind dort eingefuegt.
 *   RDT-ANKER   Datei-Offset eines Original-Opcodes im geladenen RDT + seine Bytes (beim Raumladen geprueft,
 *               re15_zws_r41_raum_scan). Haken im Dispatcher VOR dem Opcode (scd_vm.c, Muster rekorder_r38/codepanel_r37):
 *               parken, solange der Schrei/Film laeuft, danach laeuft der Original-Opcode unveraendert. Vor dem Start wartet
 *               der Anker wie die Port-Zeile auf das Ende einer laufenden Sprachzeile (derselbe Riegel, gleiche Notbremse).
 *               Raum 11C: die Szene ist Original-sub02 (Port-Programm SZENE der Runde 39 = Gosub sub02 unveraendert,
 *               parkplatz_r39.c; bei (3,67)=1 Original-Evt_exec sub02 @0x01824) - keine Port-Zeilen, nur Anker.
 *
 * ================================ RAUM 114 (ROOM1141, John und Elza) ================================
 * NUTZER-VORGABE (woertlich, 2026-11-13): "Ich wuerde im Dialog bei John und Elza vor 'What was that?' diesen Schrei-Sound
 * von dem Gorilla einbauen, aus 11C Sekunde 6. Und dann kurz warten, bevor ich das Video abspiele. Nach dem Video geht es
 * normal weiter mit 'Shit! that is what I was talking about!'". Nachtrag Koordinator: gemeint ist "der Auf-die-Brust-
 * klopfen-Sound" -> vorlaeufig Satz 4, an EINER Stelle austauschbar (RE15_ZWS_SCHREI_SATZ); Pause = Tonende + kurz.
 *
 * WO DER DIALOG LIEGT: ROOM1141.RDT sub02, msg 6 "Elza: What was that?!" @0x00DCA `2b 06 00 00`, msg 7 "John: Shit! that
 * is what I was / talking about!" @0x00E14 `2b 07 00 00` (Nachrichten @0x01243 / @0x01262, tools/rdt_msgdump.py).
 * ROOM1140 (Leon) hat diesen Dialog nicht (msg 0..5: Schloss, Gemaelde, Tisch, "The streets are infested ...").
 * Im Port laeuft die Szene als Programm ENTRIEGELN39 (Runde 39 Spur J, include/re15_dining_r39.h; Original-Bytes von
 * sub02 ab @0x00CD4 mit eingefuegten Zeilen, tools/r39_dining/programme_bauen.py -> engine/src/gen/dining_r39_programme.inc).
 * Zwischen msg 6 und msg 7 stehen im Original: Plc_neck Elza @0x00DD2, Sleep 5 @0x00DDC, Plc_neck John @0x00DE4, Sleep 5
 * @0x00DEE, Plc_dest TURN Elza @0x00DF6, Sleep 5 @0x00DFE, Plc_dest TURN John @0x00E06, Gosub sub04 @0x00E0E (wartet, bis
 * John steht), Sleep 86 @0x00E10 `09 0a 56 00` - beide drehen sich zum Fenster, dann Pause.
 *
 * ABLAUF 114 (eingefuegt in ENTRIEGELN39, Message_on-Form `2b <id> 00 00` wie die Port-Warte-Zeile 58 der Runde 39):
 *   ... msg 5, Message_on 58 (Johns Stimme zu msg 5 ausreden lassen, Runde 39)
 *   Message_on 61 = SCHREI: Gorilla-Ton, das Skript parkt bis zum Tonende (Laenge aus der Tonbank, s. u.)
 *   Sleep 20 @0x00DC6 `09 0a 14 00` = Original-Takt vor der Zeile = die "kurze Pause" nach dem Ton
 *   msg 6 "What was that?!" - Drehen zum Fenster, Sleep 86 @0x00E10 (Original) = "kurz warten"
 *   Message_on 60 = FILM: R114GORI.STR (Film des Nutzers), das Skript parkt, bis die Plattform ihn gespielt hat
 *   msg 7 "Shit! that is what I was talking about!" und weiter wie bisher.
 * Der Glasbruch der Runde 39 (Punkt 6, Se_on(0x3E,0x21) x2 vor msg 6) entfaellt an dieser Stelle: der Film zeigt das
 * Zerspringen der Scheiben selbst (Bild 278/284, Ton im Film); zweimal Glas waere ein Widerspruch. Der Satz 0x21 und
 * sein Lader bleiben (re15_dining_r39.h RE15_R39J_SATZ_GLAS).
 *
 * ================================ RAUM 11C (ROOM11C0, Leon und Ada) ================================
 * NUTZER-VORGABE (woertlich, 2026-11-13, zum Film Raum11C_Gorillas_v9_Ton.mp4): "Diese Version des Videos sieht super aus!
 * Auch da brauchen wir vor dem "What!" Von Leon wieder ein Bruellen vom dem Gorilla. Dann kurze Pause, dann das Video,
 * dann geht es weiter mit "Ada, you hide in that patrol car" oder wie der Text da auch ist."
 *
 * WO DER DIALOG LIEGT: ROOM11C0.RDT sub02 @0x0184E..@0x01B02 (Sub-SCD @0x01758, Zeigertabelle 8 Subs), nur Leons
 * Szenario: ROOM11C1 (Elza) hat gar keinen Nachrichtenblock (RDT-Kopf +0x3C = 0). Nachrichten (tools/rdt_msgdump.py):
 *   msg 3 @0x1D59 "Ada: You didn't run into him?"   msg 4 @0x1D80 "Leon: What!?"
 *   msg 5 @0x1D96 "Leon: Ada, you hide inside that patrol / car."   msg 6 @0x1DCB "Ada: O-okay, I'll do that..."
 * sub02 (selbst disassembliert, Laengen scd_vm.c s_opcode_sizes):
 *   @0x01904 `2b 03 00 00` msg 3; @0x01908 Plc_motion 0x10, @0x0190C Sleep 40, @0x01910 Plc_motion 0x10, @0x01914 Plc_flg,
 *   @0x01918 Sleep 40, @0x0191C `3f 00 18 00` Plc_motion 0x18 (Ada),
 *   @0x01920 `09 0a 14 00` Sleep 20                          <- ANKER SCHREI (davor), Sleep 20 = die "kurze Pause"
 *   @0x01924 `2e 01 00 00` Work_set Leon, @0x01928 `2b 04 00 00` msg 4 "Leon: What!?"
 *   @0x0192C..@0x0193C Plc_motion 0x0F / Sleep 40 / Plc_motion 0x0F / Plc_flg / Sleep 50 (Leon),
 *   @0x01940 Plc_neck Leon + Sleep 10, @0x01952 Plc_neck Ada + Sleep 10, @0x01964 Plc_dest TURN Leon + Sleep 10,
 *   @0x01974 Plc_dest TURN Ada, @0x0197C `18 06` Gosub sub06 (wartet, bis Ada steht), @0x0197E `09 0a 0a 00` Sleep 10
 *   - beide drehen sich zum Anhaenger (Punkt (-3617, -17798) = `DF F1` / `7A BA`) = "kurz warten" wie Sleep 86 in 114,
 *   @0x01982 `54 00 01 00 00 00` Sce_bgm_control MAIN Play         <- ANKER FILM (davor)
 *   @0x01988 `29 0c` Cut_chg 12 (Anhaenger von hinten), Gorillas Member_set Y -2500 (@0x0198E/@0x01996), Objekt 0 schwingt
 *   in sieben For-Schleifen 12+6x4 = 36 Takte (@0x0199A..@0x01A28), Gorilla 1 nach (-3617,0,-17798) +0x0C=0x10 (@0x01A2E..
 *   @0x01A3E), Gorilla 2 nach (-1220,-21568) (@0x01A48/@0x01A4C), Sleep 80 @0x01A50, Gorilla 1 +0x0C=0x30 @0x01A58,
 *   @0x01A5C `29 05` Cut_chg 5, @0x01A5E `2b 05 00 00` msg 5 "Leon: Ada, you hide inside that patrol car."
 * Sce_bgm_control @0x01982 = Opcode 0x54, Tabelle @0x800745f8 -> 0x80042998 (packt pc[1..5], `jal 0x80044da4` @0x800429e4);
 * FUN_80044da4 Steuerwert (a0>>24)&0xF = 1 (`andi v1,v0,0xf` @0x80044dcc, Sprungtabelle @0x80010e58[1] -> 0x80044e00):
 * SsSeqSetVol (`jal 0x8005ab5c` @0x80044e24) + SsSeqPlay (`jal 0x80060030` @0x80044e40) = die Kampfmusik beginnt HIER.
 *
 * ABLAUF 11C (drei RDT-Anker, sub02 selbst bleibt bytegleich):
 *   ... msg 3 "You didn't run into him?", Ada Plc_motion 0x18 @0x0191C
 *   ANKER @0x01920: Adas Sprachzeile ausreden lassen, dann SCHREI (derselbe Ton wie 114), parken bis zum Tonende
 *   Sleep 20 @0x01920 (Original) = die "kurze Pause" nach dem Ton (wie Sleep 20 @0x00DC6 in 114)
 *   msg 4 "What!?" @0x01928 - Leon/Ada drehen sich zum Anhaenger, Gosub sub06, Sleep 10 @0x0197E (Original) = "kurz warten"
 *   ANKER @0x01982: FILM R11CGORI.STR (beginnt mit Leon/Ada, die auf den Anhaenger schauen), parken bis gespielt
 *   Sce_bgm_control @0x01982: die Kampfmusik beginnt NACH dem Film (Nutzer 11-13 zur Filmmusik: keine Bossmusik im Film,
 *   "Bruch, wenn das Spiel sie danach neu startet"), dann der Original-Auftritt @0x01988..@0x01A5B UNSICHTBAR (s. u.),
 *   ANKER @0x01A5C `29 05` Cut_chg 5: Ende des Vorspulens, Einblenden aus Schwarz, im selben Takt msg 5 @0x01A5E.
 * AUFTRITT IN KAMERA 12 UNSICHTBAR VORSPULEN (Orchestrator-Entscheid 2026-11-14 zur Nutzer-Vorgabe "dann das Video, dann
 *   geht es weiter mit 'Ada, you hide in that patrol car'"): der Block @0x01988..@0x01A5B (Cut_chg 12 @0x01988; Member_set
 *   Y=-2500 fuer Work 2/1 @0x0198E und 2/2 @0x01996; Work 3/0 @0x0199A = Objekt 0, sieben Speed_set/For-Add_speed/
 *   Add_aspeed-Schleifen @0x0199E..@0x01A28 = Anhaenger wackelt 12+6x4 = 36 Takte; Gorillas auf ihre Plaetze, 0x0C=0x10
 *   @0x01A2E..@0x01A4C; Sleep 80 @0x01A50; 0x0C=0x30 @0x01A58) zeigt dasselbe Ereignis wie der Film noch einmal in
 *   Spielgrafik und soll nicht sichtbar sein. Er laeuft vollstaendig mit allen Logik-Takten (Endzustand wie im Original),
 *   aber ohne Bildausgabe und ohne Wanduhr-Takt: nach dem Film (re15_zws_r41_film_gespielt) meldet re15_zws_r41_vorspulen()
 *   1, die PC-Hauptschleife zeichnet diese Bilder wie ein gehaltenes Apply-Bild (re15_render_pc_bild_halten, Runde 41
 *   Spur E: kein Present, Fade-Takt laeuft) und laesst Bildtakt und Standbild-Wartezeit aus. Der Anker @0x01A5C beendet
 *   das Vorspulen und setzt erst DORT das Einblenden aus Schwarz (dieselben 15 Bilder wie 114, Konstanten
 *   RE15_ZWS_BLENDE_*), das erste gezeigte Bild danach ist Kamera 5 mit msg 5. Bildzaehler und Logik laufen unveraendert
 *   (im Riegel und im Echtlauf gleiche Spielbilder fuer Cut_chg 5 / msg 5 wie ohne Vorspulen). Kein Anker -> Notbremse
 *   nach RE15_ZWS_VORSPUL_NOTBREMSE Bildern (Vorspulen aus, Einblenden). PSX-Fassung: kein Plattform-Haken, nichts zu tun.
 *
 * ================================ GEMEINSAM ================================
 * DER SCHREI: Raum-Tonbank snd1 von ROOM11C0 (Gorilla-Raum; RDT-Kopf +0x14/+0x18/+0x1C = EDT/VH/VB, rdt_common.c), auf der
 * 40-%-Disc bytegleich. Der Gorilla (Typ 0x27, STAGE1.BIN) spielt seine Toene nur ueber FUN_800453d0 (Raum-SE, snd1):
 *   Satz 4: @0x8011793c `lbu v0,7(v1)` / @0x80117944 bne (+0x7 == 0) / @0x8011794c `lbu v1,149(v1)` / @0x80117954 bne
 *           (+0x95 == 0x1d) / @0x8011795c `jal 0x800453d0` / @0x80117960 `ori a0,zero,0x4` = Clip 3 (aufrichten,
 *           Brusttrommeln) Bild 0x1d - ein Ruf/Schrei von 1,23 s (Messung scratchpad ton/NOTIZ.md Abschnitt 2).
 *   Satz 2: @0x80118c60 `ori a0,zero,0x2` / @0x80118c80 `jal 0x800453d0` = Landung nach dem Sprung (0,51 s).
 * Beide Filme nehmen denselben Satz (RE15_ZWS_SCHREI_SATZ, EINE Stelle). Gespielt wird er wie jeder Raum-SE (audio_pc.c
 * re15_audio_se_play_bank -> se_play_layers: Ton-Pegel/-Pan, Tonhoehe note2pitch2). Die Wartezeit = laengste Lage des Satzes
 * in SCD-Takten (re15_zws_r41_satz_takte): Proben nach dem END-Flag-Abbruch (re15_vag_adpcm_decode) / (44100 * pitch /
 * 0x1000) s (audio_pc.c: "0x1000 == 44100 Hz Ausgabe"), aufgerundet auf 30 Takte/s (scd_vm.c op_sleep: "30 Hz SCD VM tick").
 *
 * DIE FILME: shared_assets/PSX/MOVIE/R114GORI.STR / R11CGORI.STR (+ .TXT Herkunft), gebaut von tools/r41_zws114/
 * film_bauen.py --film 114|11C aus Cutscenes/Rooms/<Raum>/Nachbau_Blender/Raum114_Gorillas_v3_Ton.mp4 bzw.
 * Raum11C_Gorillas_v9_Ton.mp4 (unveraendert), Format wie MOVIE/CAPCOM.STR (MDEC BS v2 320x240 + CD-XA 37800 Hz 4 Bit Stereo),
 * 367 bzw. 362 Bilder bei 30/s. Die PC-Plattform spielt sie mit dem Weg des Vorspanns (re15_str.c + re15_xa.c, Ton = Takt,
 * START ueberspringt), zwischen zwei Spielbildern (platform/pc/src/zws_r41_pc.c).
 *
 * UEBERBLENDUNG WIE RE2 (Nutzer 2026-11-13: "Wie ist es in Resident Evil 2 gemacht, wenn die Filmsequenzen starten? So machen
 * wir das auch."). RE2 Leon (info/re2leon/PSX.EXE, ghidra_re2_Leon.txt), selbst disassembliert:
 *   START = harter Schnitt. Movie_on = Opcode 0x6F, Tabelle @0x800a7684 -> LAB_80058d64: `lbu a0,1(v0)` @0x80058d7c,
 *     `jal FUN_80032150` @0x80058d80 (setzt nur DAT_800cfb74 |= 0x200, kein Fade). Der Film laeuft im Bildwechsel FUN_8002b968
 *     (@0x8002b450 im Hauptloop): `andi v0,v0,0x200` @0x8002ba1c, `jal FUN_80032340` @0x8002ba28 - NACH PutDispEnv des
 *     vorigen Bilds und VOR DrawOTag des aktuellen: kein abgeblendetes Spielbild vor dem Film.
 *   ENDE = Einblenden aus Schwarz, vom SKRIPT vorbereitet. Der Filmspieler selbst blendet die In-Game-Filme nicht ab
 *     (FUN_80032984: Kachel nur bei Flag 0x40 schwarz / 0x80 weiss; Filme 4/8/9 tragen 0x0008, Tabelle @0x800a2184 + n*0x20).
 *     Alle Movie_on in RE2 Leon PL0: ROOM1080 sub14 @0x0252A `53 02 02 07 00 f8` Sce_fade_set + @0x02530 `6f 09` (Film mitten in
 *     einer laufenden Szene, danach Plc_motion/Sleep/Plc_neck) -> hier VORBILD; ROOM1080 sub02 @0x01C4C `53 02 02 07 00 fe 6f 08
 *     74 02 ff 7f` (eigenes Ereignis, Einblenden aus 255 in 64 Bildern); ROOM10B0 sub12 `6f 04` ohne Fade.
 *     Sce_fade_set LAB_80057ef0 (Tabelle @0x800a7614 = 0x53): Kanal pc[1] (`lbu s0,1(s2)` @0x80057f18), ABR pc[2] (@0x80057f38,
 *     FUN_8002c1a0 SetDrawMode abr<<5), RGB-Maske pc[3] (@0x80057f3c), Wert = lhu pc[4] (@0x80057f1c) = Schritt je Bild,
 *     Ebene 7 (`li a3,0x7` @0x80057f14); Wert < 0 -> Pegel = Wert + 0x8000 (`ori v0,zero,0x8000` @0x80057f94, `addu` @0x80057f98,
 *     `sh` @0x80057fa0). Takt FUN_8002c378 (`jal` @0x8002b448, zwischen Spieltakt @0x8002b440 und Bildwechsel @0x8002b450):
 *     Helligkeit = Pegel >> 7 (`sra a0,v0,0x17` @0x8002c3b8), Kachel-RGB = Maske & Helligkeit, Pegel += Schritt (@0x8002c404-14),
 *     Halt bei gesetztem Bit 15 (`bltz` @0x8002c3b4) - dieselbe Mechanik wie RE1.5 FUN_80021880 (engine/src/fade_common.c).
 *     Folge ROOM1080 sub14: Pegel 0x7800 -> nach dem Film Helligkeit 240, 224, ..., 16, 0 = 15 Bilder subtraktiv (ABR 2) aus
 *     Schwarz zurueck ins Spiel. Der Port setzt genau das, wenn die Plattform einen Film beendet (re15_zws_r41_film_gespielt);
 *     11C erst am Ende des unsichtbaren Vorspulens (Anker @0x01A5C), dieselben Konstanten.
 *   TON: RE2 schaltet nur die Musik per Skript (sub14 Sce_bgm_control vor/nach dem Film). 114: in ENTRIEGELN39 ist sie schon
 *     aus (Sce_bgm_control MAIN Stop, Port-Zeile +0x80) und startet wie im Original nach msg 7 (@0x00E5C). 11C: vor dem Film
 *     laeuft keine Musik: der Raumlader laedt MAIN16 nur (Tabelleneintrag FF56, MAIN-Byte 0x56 -> Flag 0x56 >> 6 = 1:
 *     `srl v0,s0,6` @0x800442cc, `sb v0` nach 0x800b52ad @0x800442d4; FUN_800444b0 spielt nur bei Flag 0, `lbu v1` 0x800b52ad
 *     @0x800444b8 / `bne v1,zero` @0x800444c8), erst der Original-Opcode @0x01982 startet sie - direkt nach dem Film.
 *     Gemessen im Echtlauf (Mitschnitt): Stille zwischen "What!?" und dem Filmton, Musik ab dem Bild nach dem Film.
 * PSX-Fassung des Ports: keine Plattform-Haken -> Port-Zeilen und Anker laufen ohne Ton/Film durch.
 */
#ifndef RE15_ZWS_R41_H
#define RE15_ZWS_R41_H

#include <stdint.h>
#include <stddef.h>

/* ---- EINZIGE Stelle zum Tauschen des Schreis (beide Filme): Satz der snd1-Bank von ROOM11C0 (4 = Ruf Clip 3, 2 = Landung) */
#define RE15_ZWS_SCHREI_SATZ  4
#define RE15_ZWS_SCHREI_RDT   "STAGE1/ROOM11C0.RDT"
#define RE15_ZWS_SCHREI_BANK  1        /* snd1 = Raum-SE-Bank (FUN_800453d0) */

#define RE15_ZWS_TAKTE_JE_S   30       /* scd_vm.c op_sleep: SCD-VM laeuft mit 30 Hz */
/* Notbremse des Stimmen-Wartens am Anker = RE15_VOICE_NOTBREMSE in scd_vm.c op_message_on (10 s > laengste Aufnahme). */
#define RE15_ZWS_STIMME_NOTBREMSE 300
/* Notbremse des unsichtbaren Vorspulens (Anker nie erreicht): dieselbe Grenze, 300 Bilder = 10 s > Block @0x01988..@0x01A5B
 * (36 Takte Schwingen + Evt_next + Sleep 80 = ca. 117 Takte). */
#define RE15_ZWS_VORSPUL_NOTBREMSE RE15_ZWS_STIMME_NOTBREMSE

/* ---- Raum 114 (ROOM1141): Port-Zeilen ohne Text (ROOM1141: 40-42 Runde 38, 50-58 Runde 39, Sprach-Id 59 -> 60/61 frei) */
#define RE15_ZWS114_RAUM         0x1141u
#define RE15_ZWS114_MSG_FILM     60
#define RE15_ZWS114_MSG_SCHREI   61
#define RE15_ZWS114_FILM         "MOVIE/R114GORI.STR"
#define RE15_ZWS114_FILM_BILDER  367      /* Quelle: 367 Bilder bei 30/s (Riegel prueft die Datei) */

/* ---- Raum 11C (ROOM11C0): RDT-Anker in sub02 (Datei-Offsets, Bytes oben im Kopf) ---- */
#define RE15_ZWS11C_RAUM         0x11C0u
#define RE15_ZWS11C_ANKER_SCHREI 0x01920u /* `09 0a 14 00` Sleep 20 vor Work_set Leon + msg 4 "What!?" @0x01928      */
#define RE15_ZWS11C_ANKER_FILM   0x01982u /* `54 00 01 00 00 00` Sce_bgm_control MAIN Play vor Cut_chg 12 @0x01988      */
#define RE15_ZWS11C_ANKER_SICHT  0x01A5Cu /* `29 05` Cut_chg 5 vor msg 5 @0x01A5E: Ende des Vorspulens, Einblenden       */
#define RE15_ZWS11C_MSG_WAS      4        /* @0x01928 `2b 04 00 00` "Leon: What!?"                                     */
#define RE15_ZWS11C_MSG_WEITER   5        /* @0x01A5E `2b 05 00 00` "Leon: Ada, you hide inside that patrol / car."   */
#define RE15_ZWS11C_FILM         "MOVIE/R11CGORI.STR"
#define RE15_ZWS11C_FILM_BILDER  362      /* Quelle: 362 Bilder bei 30/s (Riegel prueft die Datei) */

/* Einblenden nach dem Film = RE2 ROOM1080 sub14 @0x0252A `53 02 02 07 00 f8` (Sce_fade_set, Handler LAB_80057ef0). */
#define RE15_ZWS_BLENDE_KANAL 2        /* pc[1] @0x0252B (`lbu s0,1(s2)` @0x80057f18)                          */
#define RE15_ZWS_BLENDE_ABR   2        /* pc[2] @0x0252C: B - F subtraktiv (@0x80057f38 -> FUN_8002c1a0 abr<<5)  */
#define RE15_ZWS_BLENDE_RGB   7        /* pc[3] @0x0252D: R, G, B (@0x80057f3c -> FUN_8002c1a0 Masken 0xff)     */
#define RE15_ZWS_BLENDE_WERT  (-0x800) /* pc[4..5] @0x0252E `00 f8` = Schritt je Bild (@0x80057f1c lhu)          */
#define RE15_ZWS_BLENDE_EBENE 7        /* `li a3,0x7` @0x80057f14                                               */
/* Startpegel bei Wert < 0: Wert + 0x8000 (@0x80057f94-@0x80057fa0) = 0x7800 -> Helligkeit 240 (@0x8002c3b8 >> 7). */
#define RE15_ZWS_BLENDE_PEGEL ((uint16_t)(RE15_ZWS_BLENDE_WERT + 0x8000))

/* ================================ ZWEITE BLENDENART: RE2 ROOM4090 (Runde 42 Strang birkinfilm) ================================
 * Tabellenspalte `blende` (re15_zws_film_t): RE15_ZWS_BLENDART_1080 (0, Standard) = harter Schnitt in den Film, danach 15 Bilder
 * Einblenden (oben, ROOM1080 sub14); RE15_ZWS_BLENDART_4090 = genau der Uebergang des Films R409 "Birkins Mutation" in RE2 Leon
 * ROOM4090 sub02 (selbst disassembliert; Opcode-Tabelle @0x800a74c8):
 *   @0x027F4 `53 02 02 07 00 02` Sce_fade_set Kanal 2, ABR 2, RGB 7, Wert +0x0200: FUN_8002c1a0 @0x80057f50 (Schritt +0x200),
 *            FUN_8002c2b0(Kanal, 0) @0x80057f64 (`sh a1,0(a0)` @0x8002c2d4), Wert > 0 -> Pegel 0 (`sh zero` @0x80057fa4).
 *   @0x027FA Sce_bgm_control (SUB anhalten), @0x02800 Sleep 1, @0x02804 Sce_bgm_control (MAIN anhalten), @0x0280A Sleep 62,
 *   @0x0280E Cut_chg 10, @0x02810 Evt_next. Sleep (0x09 -> 0x800539dc) setzt den Zaehler, Sleeping (0x0a -> 0x80053a24) zaehlt
 *   je Bild herunter und gibt bei 0 mit `addiu pc,3` + return 2 ab (@0x80053a6c/@0x80053a88), Evt_next (0x02 -> 0x80053860)
 *   gibt ab: Abblende in Bild F0, Film in Bild F64 -> F0..F63 = 64 Bilder Helligkeit 0, 4, ..., 252 (Takt FUN_8002c378:
 *   Helligkeit = Pegel >> 7 @0x8002c3b8, danach Pegel += Schritt @0x8002c404-@0x8002c414, Halt bei Bit 15 @0x8002c3b4).
 *   @0x02812 `53 02 02 07 00 fe` Wert -0x0200 -> Pegel = Wert + 0x8000 = 0x7E00 (@0x80057f98) -> Helligkeit 252 im Bild F64,
 *   @0x02818 `6f 0b` Movie_on -> der Film laeuft im Bildwechsel von F64 (harter Schnitt aus fast Schwarz),
 *   @0x0281C `74 02 ff 7f` Sce_fade_adjust (0x74 -> 0x80057fd8: FUN_8002c2b0(Kanal 2, 0x7FFF) = Pegel 0x7FFF) im Bild nach dem
 *   Film -> Helligkeit 255, 251, ..., 3 (64 Bilder), dann Bit 15 -> Halt; @0x02820 Sleep 63 (Skript wartet die Einblende ab).
 * Im Port: die Film-Zeile setzt beim ersten Takt die Abblende, parkt bis Takt 64, setzt dann Pegel 0x7E00/Schritt -0x200 und fordert
 * den Film an; die Plattform meldet re15_zws_r41_film_gespielt -> Pegel 0x7FFF (= @0x0281C). Unterschied zu RE2: das Bild von
 * Takt 64 (Helligkeit 252) zeigt der Port VOR dem Film (die Plattform spielt den Film zu Beginn des naechsten Bilds), RE2 danach.
 * Waehrend des Films tickt der Port-Blendkanal mit (re15_render_end_frame je Filmbild), RE2 nicht (der Film laeuft innerhalb von
 * FUN_8002b968); deshalb setzt film_gespielt den Pegel wie @0x0281C neu. */
#define RE15_ZWS_BLENDART_1080   0
#define RE15_ZWS_BLENDART_4090   1
#define RE15_ZWS4090_AB_WERT     0x0200   /* @0x027F8 `00 02` = Schritt je Bild                                      */
#define RE15_ZWS4090_AB_PEGEL    0x0000   /* Wert > 0 -> `sh zero` @0x80057fa4                                       */
#define RE15_ZWS4090_AB_BILDER   64       /* F0..F63: Sleep 1 @0x02800 + Sleep 62 @0x0280A + Evt_next @0x02810         */
#define RE15_ZWS4090_AUF_WERT    (-0x0200) /* @0x02816 `00 fe`                                                        */
#define RE15_ZWS4090_AUF_PEGEL   ((uint16_t)(RE15_ZWS4090_AUF_WERT + 0x8000))   /* 0x7E00, @0x80057f98                 */
#define RE15_ZWS4090_NACH_PEGEL  0x7FFF   /* @0x0281E `ff 7f` -> FUN_8002c2b0 `sh a1` @0x8002c2d4                       */

/* Haken-Stelle: Port-Zeile (msg >= 0) oder RDT-Anker (msg < 0: Datei-Offset + erwartete Bytes). */
typedef struct {
    int16_t  msg;          /* Port-Zeile `2b <msg> 00 00`, sonst -1 */
    uint32_t off;          /* RDT-Anker: Datei-Offset des Original-Opcodes, VOR dem eingehaengt wird */
    uint8_t  bytes[6];     /* erwartete Bytes am Anker (beim Raumladen geprueft) */
    uint8_t  n;
} re15_zws_stelle_t;

/* Ein Film = eine Tabellenzeile (engine/src/zws_r41.c k_filme). */
typedef struct {
    const char        *kennung;      /* Log-Praefix "[<kennung>]": "zws114" / "zws11c" */
    uint16_t           raum;         /* g_current_room_id */
    re15_zws_stelle_t  schrei;       /* vor der "Was war das?"-Zeile */
    re15_zws_stelle_t  film;         /* nach dem Warten, vor der Fortsetzung */
    re15_zws_stelle_t  sichtbar;     /* n > 0: nach dem Film unsichtbar vorspulen bis zu diesem RDT-Anker, dort Einblenden */
    int                schrei_satz;  /* ROOM11C0 snd1 Satz */
    const char        *film_datei;   /* relativ zur Asset-Wurzel */
    int                film_bilder;
    int                blende;       /* RE15_ZWS_BLENDART_1080 (0) / _4090 (Runde 42 birkinfilm) */
    int                bild_y;       /* Bildlage: Zeilen Rand oben, wenn der Film niedriger als 240 ist (R409: 40) */
} re15_zws_film_t;

/* Tabelle (Riegel/Plattform). */
int                    re15_zws_r41_anzahl(void);
const re15_zws_film_t *re15_zws_r41_film_nr(int i);
const re15_zws_film_t *re15_zws_r41_film_fuer_raum(unsigned raum);
const re15_zws_film_t *re15_zws_r41_aktiv(void);   /* Film des laufenden Schreis/Films, sonst NULL */

/* Zustand des Films (Riegel/Plattform). */
enum { RE15_ZWS_FILM_AUS = 0, RE15_ZWS_FILM_OFFEN = 1, RE15_ZWS_FILM_GESPIELT = 2,
       RE15_ZWS_FILM_ABBLENDEN = 3 };   /* 3: Blendenart 4090, die Film-Zeile parkt RE15_ZWS4090_AB_BILDER Takte vor dem Film */

/* HAKEN op_message_on (scd_vm.c, nach dem Stimmen-Riegel): 0 = nicht diese Zeile, 1 = verbraucht (pc += 4), 2 = parken. */
int re15_zws_r41_message_on(const uint8_t *pc);
/* HAKEN Dispatcher (scd_vm.c, vor dem Opcode, nur bei g_re15_zws_anker_n != 0): 0 = Opcode laeuft, 2 = parken. */
extern int g_re15_zws_anker_n;
int re15_zws_r41_pc(const uint8_t *pc);
/* Raumladen (scd_register_current_rdt): RDT-Anker des Raums gegen die Bytes pruefen und armieren. */
void re15_zws_r41_raum_scan(const uint8_t *raw, long raw_size, unsigned room_id);
/* Installer (re15_dining_r39_install, jeder Raum): Schrei-/Film-Zustand zuruecksetzen. */
void re15_zws_r41_install(uint16_t room_id);

/* Plattform-Dienste (platform/pc/src/zws_r41_pc.c). NULL = nicht vorhanden -> Zeile/Anker laeuft sofort durch.
 *   schrei_hook(satz): Satz spielen, Rueckgabe = Laenge in SCD-Takten (< 0 = nicht gespielt).
 *   film_spieler: != 0, wenn die Plattform den Film zwischen zwei Spielbildern spielt (re15_zws_r41_film_offen /
 *                 re15_zws_r41_film_gespielt). */
extern int (*re15_zws_r41_schrei_hook)(int satz);
extern int re15_zws_r41_film_spieler;
int  re15_zws_r41_film_offen(void);      /* 1 = Film angefordert, Skript parkt (Datei: re15_zws_r41_aktiv()->film_datei) */
void re15_zws_r41_film_gespielt(void);   /* Plattform: Film zu Ende (oder uebersprungen) -> RE2-Einblenden */
void re15_zws_r41_film_fehlt(void);      /* Plattform: Film nicht abspielbar -> weiter ohne Einblenden */
int  re15_zws_r41_film_zustand(void);
/* 1 = nach dem Film wird der Original-Auftritt unsichtbar vorgespult: die Plattform praesentiert diese Bilder nicht und
 * laesst den Bildtakt aus. re15_zws_r41_vorspul_bild(): je uebersprungenem Bild einmal (Zaehler + Notbremse). */
int  re15_zws_r41_vorspulen(void);
void re15_zws_r41_vorspul_bild(void);
extern unsigned g_re15_zws_vorspul_bilder;   /* Bilder des letzten Vorspulens */

/* Laenge des Satzes `satz` einer Raumbank in SCD-Takten (laengste Lage, aufgerundet), < 0 = Satz leer/Bank defekt. */
int re15_zws_r41_satz_takte(const uint8_t *edt, const uint8_t *vh, int vh_size, const uint8_t *vb, int vb_size, int satz);

/* Pruefhaken (Riegel test_r41_zws114 / test_r41_zws11c). */
int re15_zws_r41_schrei_rest(void);    /* > 0: Skript parkt noch so viele VM-Takte auf dem Schrei */
extern unsigned g_re15_zws_schreie;    /* gestartete Schreie */
extern unsigned g_re15_zws_filme;      /* angeforderte Filme */
extern int      g_re15_zws_schrei_takte;   /* Wartezeit des letzten Schreis laut Hook */
extern unsigned g_re15_zws_stimme_warten;  /* Takte, die ein Anker zuletzt auf eine Sprachzeile gewartet hat */

#endif /* RE15_ZWS_R41_H */
