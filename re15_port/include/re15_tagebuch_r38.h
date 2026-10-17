/*
 * RE1.5 Rebuilt — Runde 38 Spur G "tagebuch": ROYS DIARY (Elza, ROOM1011) statt der Code-Nachricht, danach die
 * Karte mit Communications Room und Prison Room. Dossier: analysis/befunde_runde38/G_tagebuch.md.
 *
 * NUTZER-VORGABE (analysis/befunde_runde38/AUFTRAG.md, woertlich):
 *   "Ich möchte das du mir in Elzas Szenario für Roy ein Tagebuch schreibst. Hintergrund könnte
 *    FILE16_title_paper.png sein. Muss nicht unbedingt ein Welt Model haben, das taucht einfach auf, wenn man Roy,
 *    der am Boden liegt noch einmal anklickt im Room 1011. Das Tagebuch soll kommen statt der Code Message."
 *   "Den Code bitte wieder grün hervorheben, wie alle Codes in den Dokumenten"
 *   "Und danach, auf der Karte einmal den Communications Room markieren und einmal den Prison Room, so wie auch
 *    schon bei Leon im Communications Room gemacht."
 *
 * WAS DAS ORIGINAL VORGIBT (ROOM1011.RDT, selbst gelesen; tools/scd_dump_room.py):
 *   sub00 @0x00A2C  Ck(4,0xFA)==1 -> @0x00A3E Sce_em_set Roy (Typ 0x47) + @0x00A52 Aot_set Slot 5 sce 3 -> sub06
 *   sub02 @0x00DC0  Ende des Dialogs: Aot_reset Slot 5 -> sub06 (`46 05 03 31 ff 00 18 06 00 00`)
 *   sub06           msg 19/20 + Prison key (Runde 36 Q leitet Ereignis 6 auf 7 um, re15_schluessel_r36_umleiten)
 *   sub07 @0x00E4E  Aot_reset(5,0) | @0x00E58 Work_set(1,0) | @0x00E5C Plc_motion(1,11) (Elza beugt sich) |
 *                   @0x00E60 Sleep 30 | @0x00E64 `2b 12 ff ff` Message_on 18 | @0x00E68 Evt_next |
 *                   @0x00E6A Plc_motion(1,11) + @0x00E6E Plc_flg 0x80 (zurueck) | @0x00E72 Sleep 30 |
 *                   @0x00E76 Plc_ret | @0x00E78 Aot_reset Slot 5 -> sub07 | @0x00E82 `01 00` Evt_end
 *   msg 18 @0x1231  "He has passed out from his injuries.<02>He is holding a slip.<02>The numbers <61><05 01>0513
 *                   <05 00>" are printed on the slip." — DIE Code-Nachricht (0513 gruen: Steuerbyte 0x05 Arg 1).
 * Der Port ersetzt GENAU dieses Message_on (Raum 0x1011, Nachricht 18) durch das Dokument; Elzas Beugen davor und
 * das Aufrichten danach bleiben die des Originals (sub07).
 *
 * DOKUMENT = RE2-DOKUMENT-AUFNAHME (RE1.5 hat kein Dokument-System, re15_files.h). RE2 info/re2leon/PSX.EXE,
 * Aufnahme FUN_80071ba0 (selbst disassembliert):
 *   80071bbc  sltiu v0,a3,0x68 / 80071bc0 beq v0,zero,0x80071d00   Id >= 0x68 -> Dokument-Zweig
 *   80071d00  jal 0x800692dc / 80071d04 addiu a0,a3,-104            AN DIE FILE-LISTE HAENGEN (vor dem Lesen)
 *   80071d30  sb zero,23555(at)                                     Seite 0 (Titelseite)
 *   80071dc0  addiu v0,zero,4 / 80071dc4 sb v0,1(s1)                Modus 4 = Leser
 * FUN_800692dc @0x800692dc-0x80069314 sucht nur den ersten Platz == 0xFF (`bne v0,a2` @0x800692f4) — KEINE
 * Doppelpruefung; RE2 bietet ein genommenes Dokument nie ein zweites Mal an (Zone aus @0x80072b40, Flag
 * @0x80072b8c). Der Port nimmt dafuer den vorhandenen Weg re15_menu_request_doc (Runde 30).
 *
 * ERNEUTES UNTERSUCHEN — PORT-WAHL: die Code-Nachricht kam bei JEDEM Untersuchen (sub07 legt Slot 5 auf sich selbst
 * zurueck, @0x00E78). "statt der Code Message" -> jedes Untersuchen oeffnet das Tagebuch. Das erste haengt es an die
 * FILE-Liste ("The Roys Diary has been filed.", Bit 68); danach steht es schon in der Liste und wird NUR GELESEN
 * (kein zweites Anhaengen, keine Meldung, kein Hinweis) — RE2s Liste fuehrt kein Dokument doppelt.
 *
 * KARTE — RE2-ERGAENZUNG wie Leons Hinweise (map_hint_common.c; RE2 Opcode 0x84 @0x800591C4: Modus 4 `sb v0,
 * [0x800D5C00]` @0x800591DC, Anforderung @0x800591E8, Hinweis-Nr = Skriptbyte 1 @0x80059208/@0x80059210 — EIN Ziel
 * je Opcode). Zwei Ziele nacheinander = die Kette der Runde 35 Spur K (Leon, Szene im Communications Room ROOM10F0:
 * 11C0 -> 1150, zeitgesteuert, START schaltet weiter). Angefordert wird sie, wenn das Ereignis des ERSTEN Lesens
 * an seinem Evt_end angekommen ist (Leons Hinweis: am Evt_end der Szene ROOM1150 sub08 @0x012EC).
 */
#ifndef RE15_TAGEBUCH_R38_H
#define RE15_TAGEBUCH_R38_H

#include <stdint.h>

#include "re15_scd.h"              /* scd_thread_t */
#include "re15_schluessel_r36.h"   /* RE15_R36Q_PRISON_BIT — Prison Key genommen (Ziel 1 erreicht) */

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Ort: Elzas Verhoerraum (Roy existiert nur dort: Sce_em_set Typ 0x47 @0x009F6/@0x00A3E, Runde 37 K §2.2b) */
#define RE15_TAGEBUCH_R38_RAUM        0x1011
/* Die Code-Nachricht: Message_on 18 (`2b 12 ff ff` sub07 @0x00E64; Text @0x1231). */
#define RE15_TAGEBUCH_R38_MSG         18
/* Evt_end des Ereignisses, gemessen vom Message_on aus: @0x00E82 - @0x00E64 = 0x1E (`01 00`). */
#define RE15_TAGEBUCH_R38_EVT_END     0x1E

/* ---- Dokument (VERTRAG 1.3: Nummer per Zensus der Port-Dokumente; G_belege/zensus.txt) ---------------------
 * Nr 5 = naechste nach Irons Diary (0) und den vier Dokumenten der Runde 34 Nacht (1..4) -> Item-Id 0x48 + 5 = 0x4D
 * (RE1.5 erste Dokument-Id u8 @0x800c7370; RE2 `addiu a0,a3,-104` @0x80071d04). 0x4D in keiner Item-Zone der 240
 * RDTs (Zensus (a)). Bildsatz 30 = erster freier hinter FILE29 (PORT-WAHL wie Runde 34: "die naechsten freien
 * Saetze"); max_page 5 = gemessen am Satz (FILE30_p05 vorhanden, p06 nicht; tools/r38_g/tagebuch_satz.py);
 * H 176 = TIM-Kopf der Vorlage FILE16 = RE2-Record @0x800AA184 `06 00 50 00` (y_off 0x50, H = 256 - 80). */
#define RE15_TAGEBUCH_R38_DOK         5
#define RE15_TAGEBUCH_R38_BILDSATZ    30
#define RE15_TAGEBUCH_R38_MAX_PAGE    5
#define RE15_TAGEBUCH_R38_SEITE_H     176
/* Bank-9-Bit (VERTRAG 1.1: G = 68; Zensus (b): kein Set/Ck, keine Item-Zone in 240 RDTs). Gesetzt beim Ablegen
 * (menu_common.c doc_msg_tick = RE2 `jal 0x8007730c` @0x80072b8c); zugleich "Hinweis gezeigt" der Kartenkette. */
#define RE15_TAGEBUCH_R38_BANK        9
#define RE15_TAGEBUCH_R38_BIT         68

/* ---- Kartenkette (Eintraege in map_hint_common.c s_hints[]) ----------------------------------------------
 * Ziel 1 COMMUNICATIONS ROOM = ROOM10F1 (Elzas Variante; DEBUG.BIN Raumname "COMMUNIC. ROOM", Satz 15 der
 *   Stage-0-Liste = 0x10F0), Zone 0 -> Blatt 3 (2F) Rechteck 9 (re15_map_zones.h Hauptzeile).
 *   ERREICHT = Prison Key genommen (9, RE15_R36Q_PRISON_BIT): das Tagebuch schickt Elza wegen des Zellen-
 *   schluessels dorthin ("locked it up in the Communications Room"); der Raum kann VOR dem Lesen schon besucht
 *   sein (Roy erst spaeter untersucht) — dieselbe Lage wie ROOM1150 in Runde 35 K, die deshalb ein eigenes
 *   Erreicht-Flag statt des Besucht-Bits nimmt.
 * Ziel 2 PRISON ROOM = ROOM1211 "DETENTI. AREA" (Satz 33 = 0x1210): Johns Zelle (sub00 @0x01E36 Sce_em_set Typ 0x49
 *   bei (-26225,0,-25100), Text msg 10 "It's locked. It seems that there is someone inside..."), Zone 0 -> Blatt 1
 *   (B2) Rechteck 3.
 *   ERREICHT = John befreit (4,0xFC): ROOM1211 sub02 @0x01F76 `22 04 fc 01` (Befreiungsszene). */
#define RE15_TAGEBUCH_R38_ZIEL1_RAUM          0x10F1
#define RE15_TAGEBUCH_R38_ZIEL1_ERREICHT_BANK 9
#define RE15_TAGEBUCH_R38_ZIEL1_ERREICHT_BIT  RE15_R36Q_PRISON_BIT
#define RE15_TAGEBUCH_R38_ZIEL2_RAUM          0x1211
#define RE15_TAGEBUCH_R38_ZIEL2_ERREICHT_BANK 4
#define RE15_TAGEBUCH_R38_ZIEL2_ERREICHT_BIT  0xFC

/* Haken in op_message_on (scd_vm.c). 0 = nicht zustaendig; 2 = parken (PC bleibt, der Leser laeuft oder wird
 * angefordert); 1 = fertig, der Aufrufer schiebt den PC um 4 weiter (Message_on uebersprungen). */
int  re15_tagebuch_r38_message_on(const scd_thread_t *t);

/* Haken in game_step_common.c VOR der Annahme der Hinweis-Anforderung: fordert die Kartenkette an, sobald das
 * Ereignis des ersten Lesens sein Evt_end erreicht hat. */
void re15_tagebuch_r38_tick(void);

/* Haken in menu_common.c (Dokument-Aufnahme): Listenplatz von `doc`, wenn es schon in der FILE-Liste steht,
 * sonst -1 (dann haengt der Aufrufer es an). */
int  re15_tagebuch_r38_listenplatz(int doc);

/* Fuer Riegel/Protokoll: 1 = Kartenkette wartet auf das Evt_end. */
int  re15_tagebuch_r38_hinweis_faellig(void);

#ifdef __cplusplus
}
#endif

#endif /* RE15_TAGEBUCH_R38_H */
