/*
 * RE1.5 Rebuilt — Neue Szene beim ERSTEN Betreten von ROOM10F0 (Communication Room): Ada, Leon, Marvin.
 * Danach Kartenhinweis ROOM11C0 -> ROOM1150 und MAIN01 als Hintergrundmusik bis zum Parkplatz.
 *
 * Runde 35, Spur K. NUTZER-VORGABE (woertlich, analysis/befunde_runde35/AUFTRAG.md Z.40-67 und Z.92):
 *   "Wenn man das erste mal den Communication ROOM betritt (ROOM 10F0) soll Ada drin sein - und hinten
 *    rechts an den Monitoren stehen bei CUT2. [...] Zuerst Kamera auf CUT2 - Ada. Dann Kamera auf CUT0 -
 *    Leon. Leon soll den Arm strecken und sagen 'Hey - how did you came in here?' Dann soll Leon auf sie
 *    zulaufen - links von ihr stehen [...] Woman: 'Did you really think there was only one staff card for
 *    the Communication Room?' [...] Dann Soll Marvin durch die Tuer kommen - also Tuer knallen Sound - dann
 *    Marvin Laden - dann zu Cut0 wechseln [...] Marvin und Ada rennen hintereinander Gemeinsam Richtung
 *    Tuer - kurz weiter Cutscene Balken - Cutscene Ende - Karte soll aufgehen - zunaechst kurz eine Weile
 *    ROOM 11C0 auf der Karte markierend anzeigen, also so wie bei ROOM 1150 davor. Danach kurz eine Weile
 *    ROOM 1150 blinkend anzeigen. So lange der Raum nicht besucht ist, blinken beide weiter. [...]
 *    Bis Leon dann den Parking Lot erreicht hat, soll durchweg MAIN01 als Background Musik gespielt werden."
 * Dossier mit allen Belegen und Messungen: analysis/befunde_runde35/K_cut10f0.md.
 *
 * ⛔ PORT-WAHL AUF NUTZERWUNSCH. Im Auslieferungsstand hat ROOM10F0 keine Szene (sub-Tabelle @0x10D4:
 * 2 Subs, sub00 = Kiste/Flag (3,102), sub01 = Member_cmp; main00 @0x00F32 nur Tueren/Texte/Items/Props)
 * und keinen Animationsblock (RDT+0x5C = 0, rbj_zensus.py "kein RBJ"). Gebaut ist die Szene wie der
 * Ada-Ruf (Runde 34 Nacht, adaruf_1050.c): ein PORTSEITIG eingespielter SCD-Bytecode aus ORIGINAL-
 * Opcodes (gen/cut10f0_szene.inc, Generator tools/r35_k/szene_bauen.py mit Vorbild je Opcode-Form),
 * ausgefuehrt von der vorhandenen VM mit der Original-Zeitsemantik — kein Asset-Patch.
 *
 * ABLAUF:
 *   Raumaufbau (scd_room_setup.c, nach dem Init-Lauf von main00): re15_cut10f0_install setzt die
 *   Nachrichten 6..23 ein, laedt die RE2-Tuerbank (DOOR13-Tonteil) und feuert Ereignis 20 — solange
 *   (9,71)=0. scd_event_fire -> re15_cut10f0_ereignis liefert das Programm -> VM-Faden im ersten freien
 *   Ereignis-Slot (wie ROOM1050 sub03, das sub00 beim Raumstart per Evt_exec startet, @0x00C8A).
 *   Das Programm setzt (9,71) als ERSTES Opcode (Einmal-Riegel wie ROOM11B0 sub06 @0x01478).
 *   Ist der Faden zu Ende (re15_cut10f0_tick, game_step_common.c), fordert der Port den Kartenhinweis
 *   an (map_hint_common.c, Eintraege K1 = ROOM11C0, K2 = ROOM1150, Folge per Zeit/START).
 *   Betritt Leon danach ROOM1150, setzt re15_cut10f0_install den Latch (9,72): die zweite Kartenmarke
 *   (ROOM1150) hoert auf zu blinken; die erste (ROOM11C0) haengt an (4,64), dem Flag der Ankunftsszene
 *   des Parkplatzes (ROOM11C0 sub02 @0x0184E).
 *   MAIN01 (Nachbesserung 1, Dossier §9): das Fenster oeffnet, sobald (9,71) UND (9,73) "Irons-
 *   Todesszene gesehen" (Spur L) stehen und keine Szene mehr laeuft — also am Ende der 1150-Montage
 *   ("Bis Leon DANN den Parking Lot erreicht hat", AUFTRAG.md Z.92 hinter der Montage). Der Tick stoesst
 *   die Raummusik dann selbst an (auch nach dem Laden eines Spielstands); die Tabellenweiche
 *   (re15_cut10f0_bgm_eintrag) liefert MAIN01, bis (4,64) steht. Solange die Weiche MAIN01 geliefert
 *   hat, gelten Sce_bgm_control-Befehle der Raumskripte an den MAIN-Slot nicht (sie meinen die
 *   Tabellen-Musik des Raums, re15_cut10f0_bgm_haelt_main) — "durchweg".
 *
 * CHOREOGRAFIE (Fortsetzung, Dossier §8): jede Dialogzeile laeuft im Takt der Original-Dialoge
 * Sleep 40 + 50 + 20 = 110 Bilder (ROOM11B0 sub06 @0x014FA/@0x01502/@0x0150A); Leon dreht sich per
 * Plc_dest Modus 9 (ROOM11C0 sub02 @0x0185E) dem zu, mit dem er spricht — zu Marvin vor "Hey Marvin, glad
 * you made it!", zu Ada vor "Allow me to introduce you. This is..." (Arm Richtung Ada), danach wieder zu
 * Marvin.
 */
#ifndef RE15_CUT10F0_H
#define RE15_CUT10F0_H

#include <stdint.h>

/* Der Raum: ROOM10F0 (Leon). ROOM10F1 (Elza) bleibt unberuehrt — Ada/Marvin gehoeren zu Leons Strang
 * (Zensus analysis/nutzer_batch_2026-08-30b/ada-marvin-vorszene.md §2.1: alle 0x40/0x42-Spawns in STAGE1
 * liegen in Leons Raeumen; ROOM11B1 nutzt die Slots fuer Sherry). */
#define RE15_CUT10F0_RAUM            0x10F0

/* "Szene gesehen" — VERTRAG Runde 35 §1.1: Spur K = Bank-9-Bits 71 (72 Reserve). Bank 9 = 0x800b1078
 * (Flag-Tabelle 0x80074664[9]), gespeichert mit g_game.flags; Walker-Zensus 206 RDTs (Runde 34 Nacht,
 * D_adaruf.md §3.2 Verfahren): kein Ck/Set (9,71). Spur L liest dieses Bit fuer ihre 1150-Szene. */
#define RE15_CUT10F0_GESEHEN_BANK    9
#define RE15_CUT10F0_GESEHEN_BIT     71

/* Ereignis-Nummer — VERTRAG §1.3 Vorschlag K = 20. < RE15_RDT_MAX_SUB_SCD (32, Schranke in scd_event_fire),
 * kein ROOM10F0-Sub (sub-Tabelle @0x10D4 off[0] = `04 00` = 2 Eintraege). */
#define RE15_CUT10F0_EREIGNIS        20

/* Portseitige Nachrichten (VERTRAG §1.2: Spur K = 6..30). ROOM10F0-Nachrichtentabelle @0x1168 off[0] =
 * `0c 00` -> 6 Eintraege (Id 0..5); der Raum-Lader re15_msg_load_room_block beschreibt nur diese sechs.
 * Rohbytes: cut_10f0.c (tools/r35_k/texte_bauen.py). Sprachdateien (vom Nutzer, MiniMax):
 * synchro/STAGE1/room10F0/main06.wav .. main23.wav (scd_queue_voice, Raum + Id). */
#define RE15_CUT10F0_MSG_ERSTE       6
#define RE15_CUT10F0_MSG_LETZTE      23

/* Ankunftsbits der Plc_dest-Warteschleifen, Bank 5 (RAUMLOKAL, Raum-SCD-Init FUN_8003ecec loescht Bank 5
 * Wort 0 @0x8003ed74): Leon 0 (ROOM11C0 sub05 @0x01C3E), Ada 1 (ROOM11B0 sub10 @0x018AC), Marvin 2
 * (ROOM11B0 sub11 @0x018BE). */
#define RE15_CUT10F0_BIT_LEON        0
#define RE15_CUT10F0_BIT_ADA         1
#define RE15_CUT10F0_BIT_MARVIN      2

/* NPC-Spawns: Ada Typ 0x42 Skript-Slot 0 (= Aktor 1), Marvin Typ 0x40 Skript-Slot 1 (= Aktor 2) —
 * Typen und Record-Form aus ROOM11B0 main00 @0x01080 `44 00 40 40 00 00 00 ff ...` / @0x01094
 * `44 01 42 40 ...` (grid 0x40 = stehend, Kill-Flag 0xff = kein Gate, @0x80042128). */
#define RE15_CUT10F0_TYP_ADA         0x42
#define RE15_CUT10F0_TYP_MARVIN      0x40
#define RE15_CUT10F0_AKTOR_ADA       1
#define RE15_CUT10F0_AKTOR_MARVIN    2

/* ---- Positionen (Spieleinheiten). SPAWN = der Tuer-Eintritt ROOM10D0 -> ROOM10F0: ROOM10D0 main00
 * @0x01174 Door_aot_set Slot 0, Bytes 14..21 `d0 20 00 00 a2 fe 00 08` = (8400, 0, -350), Gierung 2048,
 * Byte 24 = Cut 0. Alle anderen = PORT-WAHL nach dem Wortlaut, am Bild der echten exe eingemessen
 * (Dossier §4): Ada "hinten rechts an den Monitoren bei CUT2" = Nordost-Ecke vor der Geraetewand
 * (Text-Platz Slot 3 @0x00F86 "There are various devices" z 12200..13000 / Slot 2 @0x00F72 Funkgeraet
 * x 7000..7800), Leon "links von ihr", Marvin "schraeg links zu Leon und Ada". Gierung: 0 = +X,
 * 1024 = -Z, 2048 = -X, 3072 = +Z (actor_locomotion.c: x += cos, z -= sin). */
#define RE15_CUT10F0_SPAWN_X         8400
#define RE15_CUT10F0_SPAWN_Z         (-350)
#define RE15_CUT10F0_SPAWN_DIR       2048
#define RE15_CUT10F0_ADA_X           6000
#define RE15_CUT10F0_ADA_Z           11500
#define RE15_CUT10F0_ADA_DIR         3072
#define RE15_CUT10F0_LEON_X          4800
#define RE15_CUT10F0_LEON_Z          11500
#define RE15_CUT10F0_WP_X            5500
#define RE15_CUT10F0_WP_Z            2500
#define RE15_CUT10F0_MARVIN_X        3800
#define RE15_CUT10F0_MARVIN_Z        9900
#define RE15_CUT10F0_PARK_X          (-30000)   /* ROOM11B0 main00 @0x01080 `d0 8a` */
#define RE15_CUT10F0_PARK_Z          (-30000)

/* Tuerknall = RE2 (VERTRAG §2.2 "Sound ist RE2"): der Door_exit-Satz (se 1) des RE2-Tuerarchivs DOOR13,
 * das der Port fuer GENAU diese Tuer spielt (gen/tuer_zuordnung.inc ROOM10F0 Slot 0 -> Archiv 0x13);
 * Tonteil eingebacken (gen/cut10f0_tuerton.inc, tools/r35_k/tuerton_bauen.py, Tabelle @0x8009A604).
 * Ausgeloest per ORIGINAL-Opcode Se_on (Form ROOM10D0 sub21 @0x01A02) mit einer PORT-BANK-NUMMER:
 * RE1.5 kennt Bank 0..5 (FUN_80045024, DAT_800b21ec[bank]); 0x0E ist unbelegt und wird in
 * audio_pc.c vor der Bank-Weiche auf re15_audio_re2_tuer_se umgeleitet. */
#define RE15_CUT10F0_SE_BANK         0x0E
#define RE15_CUT10F0_SE_TUERKNALL    1

/* Gestenbank: ROOM10F0 hat keinen Animationsblock. Die Gesten (Plc_motion Sub 0 = RBJ-Kanal +0x180,
 * FUN_8001b3f8) kommen fuer die Szene aus dem Block von ROOM11B0 (RDT @0x1CB0, 48168 B): Record 0
 * (Marker Bit 0 = Spieler) = Leons Bibliothek, Clips 15..24 bytegleich mit ROOM1050/1090/1170 (D_adaruf.md
 * §3.4); Record 1 (Marker Bit 1 = Gegner-Index 0) = die NPC-Bibliothek, mit der Marvin dort spricht
 * (sub06 @0x014F6 ff.). Beide NPCs der Szene lesen Record 1 (Alias fuer Aktor 2, enemy_common.c). */
#define RE15_CUT10F0_RBJ_RAUM        0x11B0
#define RE15_CUT10F0_RBJ_NPC_RECORD  1

/* Kartenhinweis danach (map_hint_common.c s_hints, Eintraege K1/K2): ZIEL 1 = ROOM11C0 "PARKING LOT"
 * (Zonen-Hauptzeile Blatt 0 / Rechteck 4), danach ZIEL 2 = ROOM1150 (Blatt 4 / Rechteck 2). "kurz eine
 * Weile" = PORT-WAHL: drei volle Blinkperioden des RE2-Hinweiszaehlers (Periode 78 Schritte je VBlank,
 * @0x8006F20C-0x8006F284, re15_map_hint_periode) = 234 Schritte ~ 3,9 s; START/Abbruch springt sofort
 * weiter bzw. schliesst (Maske 0x6000 @0x8006F884). */
#define RE15_CUT10F0_ZIEL1_RAUM      0x11C0
#define RE15_CUT10F0_ZIEL2_RAUM      0x1150
#define RE15_CUT10F0_HINWEIS_PERIODEN 3

/* "ROOM1150 NACH der Szene betreten" — der Besucht-Latch des ZWEITEN Ziels. ROOM1150 ist im echten Spiel
 * vor der 10F0-Szene laengst besucht (erste Irons-Szene, Flag (3,94) gesetzt @0x01110 in ROOM1150 sub08;
 * erst ihr Hinweis schickt nach ROOM10F0, map_hint_common.c Eintrag 0) — das Besucht-Bit der ZONE stuende
 * also schon und die Kachel blinkte nie (gemessen, Dossier §8.1). Der Nutzer-Satz "So lange der Raum nicht
 * besucht ist, blinken beide weiter" meint den Besuch NACH der Szene: gesetzt beim Raumaufbau von ROOM1150
 * mit (9,71)=1 (re15_cut10f0_install). VERTRAG Runde 35 §1.1: Bit 72 = Reserve der Spur K (Zensus alle
 * 240 RDTs + Port: kein Ck/Set (9,72), Dossier §8.1). */
#define RE15_CUT10F0_ZIEL2_BESUCHT_BANK 9
#define RE15_CUT10F0_ZIEL2_BESUCHT_BIT  72

/* "Parkplatz erreicht" — ORIGINAL-Flag (4,64), nur GELESEN (VERTRAG §1.1 "Vorhandene Original-Flags duerfen
 * gelesen werden"): die Ankunftsszene des Parkplatzes setzt es als ERSTES Opcode, ROOM11C0 sub02 @0x0184E
 * `22 04 40 01`; gestartet wird sie von sub01 in jedem Spielbild, solange es 0 ist (@0x01820 `21 04 40 00`,
 * @0x01824 `04 0a 18 02`); sub00 @0x0176C waehlt damit den Spawn. Zensus 240 RDTs: sonst kein Ck/Set (4,64),
 * kein Port-Nutzer. Es beendet das Blinken der Kachel ROOM11C0 (map_hint_common.c Eintrag K1) und das
 * MAIN01-Fenster. Nachbesserung 1: vorher das Besucht-Bit der ZONE — das setzt aber JEDER Raumaufbau
 * (scd_room_setup.c: re15_map_zone_update nach main00/sub00), also auch der Schnitt der 1150-Montage nach
 * ROOM11C0 Cut 13 (Spur L), bei dem Leon gar nicht dort ist (K_abnahme_0.md §9). */
#define RE15_CUT10F0_ZIEL1_ERREICHT_BANK 4
#define RE15_CUT10F0_ZIEL1_ERREICHT_BIT  64

/* MAIN01 bis zum Parkplatz: Tabellen-Eintrag wie UNK_80074828 (FUN_800443ec: low = MAIN, high = SUB,
 * 0xff = kein SUB; MAIN01 = Slot 1, ROOM1030 traegt ihn @0x8007482e als 0x4041 mit Manuell-Start-Flag
 * 0x40 — hier OHNE Flag, damit er von selbst laeuft (FUN_800444b0 spielt nur Flag == 0, @0x800444c8)).
 * Gilt fuer STAGE1, in ROOM11C0 selbst (Raum-Byte 0x1C) nie.
 * BEGINN (Nachbesserung 1, Mangel 2): (9,71) UND (9,73) — Bit 73 = "Irons-Todesszene gesehen", gesetzt von
 * Spur L (VERTRAG §1.1, hier nur GELESEN) — und keine laufende Szene (re15_cine_active: (1,27) || (2,7));
 * der Nutzer-Satz steht HINTER der 1150-Montage (AUFTRAG.md Z.92). ENDE: (4,64) (oben). */
#define RE15_CUT10F0_BGM_EINTRAG     0xFF01
#define RE15_CUT10F0_BGM_RAUM_ENDE   0x1C
#define RE15_CUT10F0_BGM_START_BANK  9
#define RE15_CUT10F0_BGM_START_BIT   73

/* Zustand (Pruefhaken). */
#define RE15_CUT10F0_AUS             0    /* nicht ROOM10F0 oder Szene schon gesehen           */
#define RE15_CUT10F0_LAEUFT          1    /* Faden gestartet                                   */
#define RE15_CUT10F0_FERTIG          2    /* Faden zu Ende: Hinweis angefordert                */

/* HAKEN scd_room_setup.c (nach dem Init-Lauf von main00, Ende des Installer-Blocks) und platform/pc/main.c
 * (Boot-/CONTINUE-Weg). Laeuft fuer JEDEN Raum: ROOM10F0 -> Szene starten; ROOM1150 mit (9,71)=1 -> Latch
 * (9,72) setzen; sonst nichts. */
void re15_cut10f0_install(uint16_t room_id);
/* HAKEN scd_vm.c scd_event_fire: das Port-Programm fuer (ROOM10F0, Ereignis 20), sonst NULL. */
const uint8_t *re15_cut10f0_ereignis(uint16_t room_id, uint8_t event_id);
/* HAKEN game_step_common.c (einmal je Spielbild): Szenen-Ende -> Kartenhinweis; MAIN01-Fenster oeffnen/
 * schliessen und die Raummusik anstossen, wenn die letzte Auskunft an die Audio-Schicht nicht mehr gilt. */
void re15_cut10f0_tick(void);
/* HAKEN platform/pc/main.c (Raumaufbau ohne eigenen Animationsblock): Raum, dessen RDT-Block zu leihen
 * ist (0x11B0), sonst 0. Die Leihe selbst: platform/pc/src/cut10f0_pc.c re15_cut10f0_pc_rbj_leihen —
 * Zeiger auf den geliehenen Block (resident, nie free) oder NULL, *size gesetzt. */
unsigned re15_cut10f0_rbj_quelle(unsigned room_id);
uint8_t *re15_cut10f0_pc_rbj_leihen(unsigned room_id, int *size);
/* HAKEN enemy_common.c rbj_resolve_slot: Record fuer einen Aktor-Slot ohne Marker-Bit (Aktor 2 -> 1), sonst -1. */
int re15_cut10f0_rbj_record_alias(int slot);
/* HAKEN audio_pc.c ss_bgm_entry: erzwungener Tabellen-Eintrag (0xFF01 = MAIN01) oder -1. Merkt sich die
 * Auskunft (die Audio-Schicht waehlt danach). */
int re15_cut10f0_bgm_eintrag(int stage, int room);
/* HAKEN audio_pc.c (SCD_AUDIO_SEQ_CTL, Slot 0): 1 = die Weiche hat der Audio-Schicht MAIN01 geliefert —
 * Sce_bgm_control des Raumskripts an den MAIN-Slot gilt der Tabellen-Musik und wird nicht angewandt. */
int re15_cut10f0_bgm_haelt_main(void);
/* HAKEN menu_common.c map_mode (offener Hinweis-Schirm, je Menue-Schritt): zeitgesteuerte Hinweiskette.
 * weiter = START/Abbruch gedrueckt. 0 = nichts; 1 = *hint_nr ist jetzt der Folge-Hinweis, der Schirm zeigt
 * ihn schon; 2 = Zeit um (letztes Ziel) -> der Aufrufer schliesst wie bei START. */
int re15_cut10f0_hinweis_kette(int *hint_nr, int weiter);
/* HAKEN menu_common.c menu_task_step: zweites Kartenziel der normalen Karte (g_inv_screen.ziel2_*);
 * karte_mit_ziel = 0 loescht es. */
void re15_cut10f0_ziel2_setzen(int karte_mit_ziel);

/* Pruefhaken (kein Spielverhalten). */
int            re15_cut10f0_zustand(void);
int            re15_cut10f0_bgm_fenster(void);             /* 1 = MAIN01-Fenster offen */
const uint8_t *re15_cut10f0_programm(int *out_len);
const uint8_t *re15_cut10f0_meldung(int msg_id, int *out_len);
int            re15_cut10f0_msg_offset(int msg_id);        /* Offset des Message_on im Programm, -1 */
const uint8_t *re15_cut10f0_tuerton(int *out_len);

#endif /* RE15_CUT10F0_H */
