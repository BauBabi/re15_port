/*
 * RE1.5 Rebuilt — Runde 36 Spur L "elza": das Elza-Szenario (RDT-Variante xxx1).
 *
 * Auftrag (woertlich: analysis/befunde_runde36/AUFTRAG.md, Belege: analysis/befunde_runde36/L_elza.md):
 *   P2 ROOM1151  Irons liegt von Anfang an tot da, Anklicken -> "He has already passed..." (NUTZER-VORGABE;
 *                Original msg 2 = Irons lebt "I'll be fine...").
 *   P3 ROOM1211  Unlock-Ton, wenn Elza die Zellentuer aufschliesst.
 *   P4 ROOM1021  Schuss-Toene, wenn John (Kendo) Roy erschiesst.
 *   P5 ROOM1021  Roys Sterbe-Ton, sobald er erschossen wurde.
 *   P6 ROOM1011  Roys Stoehnen main16.5.wav (Nutzer-Datei) + Untertitel, waehrend Roy bewusstlos wird.
 *   (P1 Fenster/Kraehe ROOM1121: engine/src/fenster_1120.c, Elza-Zweig.)
 *
 * Variante: der Raumlader addiert das Elza-Bit auf den CD-Dateiindex (FUN_800396fc @0x800397e4 `srl a0,a0,31`,
 * @0x800397ec `addu`) — ROOM1151/1211/1021/1011 werden NUR in einem Elza-Spiel geladen. Die Raum-Id IST der
 * Szenario-Schalter.
 *
 * MECHANIK der Toene (P3-P6): ANKER = "Port-Zeile vor einem Original-Opcode". RE1.5 traegt an diesen Stellen
 * keinen Ton (alle 284 Se_on der RDTs liegen auf Bank 2, keiner in den drei Szenen); der Port haengt den Ton so
 * ein, als stuende im Skript direkt vor dem Anker-Opcode eine Se_on-Zeile, mit dem Bildversatz der RE2-Vorlage.
 * Die Anker werden je Raum EINMAL im rohen RDT-Puffer ueber Datei-Offset + Signatur (8 Byte) gesucht
 * (Muster scd_elev_se.c / map_hint_common.c); der VM-Takt meldet jeden ausgefuehrten Opcode.
 */
#ifndef RE15_ELZA_R36_H
#define RE15_ELZA_R36_H

#include <stdint.h>

/* Raeume (Elza-Varianten). */
#define RE15_ELZA_RAUM_1011  0x1011u
#define RE15_ELZA_RAUM_1021  0x1021u
#define RE15_ELZA_RAUM_1151  0x1151u
#define RE15_ELZA_RAUM_1211  0x1211u

/* Port-Ereignis (VERTRAG Runde 36 §1.3: L = 26 in 1151/1121/1021/1101/1211). */
#define RE15_ELZA_EREIGNIS   26

/* ---- P2 Irons ROOM1151 ------------------------------------------------------------------------------------
 * Irons = main00 @0x00E6A `44 00 45 00 ..` (Gegner-Slot 0 = Aktor 1, Typ 0x45, (-21150,-720,-26131)).
 * Totenpose = die Pose, in der Leons Irons nach der Todesszene liegt (Runde 35 Spur L, irons_tod_1150.c:
 * Clip 2 Bild 89 gehalten, Motion-Sub 0 Phase 2 = Halt @0x80050da4). ROOM1151-RBJ rec1 (Irons-Bank, Datei 0x1387C)
 * traegt dieselbe Clip-Tabelle wie ROOM1150 rec1: {36,60,90,35,30,102,34} -> Clip 2 = 90 Bilder, Bild 89 = letztes. */
#define RE15_ELZA_IRONS_TYP       0x45
#define RE15_ELZA_IRONS_CLIP_TOT  2
#define RE15_ELZA_IRONS_BILD_TOT  89
/* Kopf: Plc_neck-Modus 0 (`41 00 ..`, Flags 0x80|0x12 @0x80041ed8-f10) mit Geschwindigkeit 0 (Bytes pc[8..9] ->
 * +0x9e/+0x9f @0x80041f5c-6c): der Slew bewegt um das woertliche Byte (skeleton_common.c, Original @0x800375dc /
 * @0x800377a0), 0 haelt die Achse — der Kopf bleibt auf dem Clip, keine Blickverfolgung. Formbeleg fuer
 * Geschwindigkeit 0 auf einer Achse: ROOM10D0 Plc_neck Modus 2 mit 0x0A00 (Gier steht). */
#define RE15_ELZA_IRONS_NECK_FLAGS (0x80 | 0x12)
#define RE15_ELZA_IRONS_NECK_SPEED 0x0000
/* Anklick-Platz: AOT-Slot 2 (main00 @0x00D92 `2c 02 05 31 ..`, sce 5 = Ansprechen -> work_vars[0] = 2 -> sub01
 * @0x00E9E -> sub03 msg 2). Port: Aot_reset-Semantik auf sce 1 (Text), Flags 0x31 (= die des Platzes), Nutzlast
 * (msg, 0xffff) wie alle 524 ausgelieferten sce-1-Saetze (Pausemaske 0xffff, scd_vm.c re15_scd_show_message). */
#define RE15_ELZA_IRONS_SLOT      2
#define RE15_ELZA_IRONS_SCE_TEXT  1
#define RE15_ELZA_IRONS_FLAGS     0x31
#define RE15_ELZA_IRONS_MASKE     0xFFFFu
#define RE15_ELZA_IRONS_MSG_ID    20        /* VERTRAG Runde 36 §1.2: ROOM1151 L 20..23 */

/* ---- P6 ROOM1011: Roys Stoehnen ---------------------------------------------------------------------------
 * Nachrichten-Id 24 = RDT-Zahl von ROOM1011 (24 Nachrichten 0..23; VERTRAG-Regel "ab RDT-Zahl"). Text = die Bytes
 * von ROOM1011 msg 10 "Roy: groans" (@0x010C9) woertlich. Stimme: synchro/STAGE1/room1011/main16.5.wav. */
#define RE15_ELZA_STOEHNEN_MSG_ID 24
#define RE15_ELZA_STOEHNEN_DATEI  "main16.5"

/* ---- Toene (Plattform-Haken re15_elza_se_hook; Satz der Mini-Bank shared_assets/RE2/ELZASE.VBS) ---------- */
#define RE15_ELZA_TON_AUF     0     /* RE2 ROOM1140 Satz 0x25 (= RE2_ELZA_SE_AUF)    */
#define RE15_ELZA_TON_SCHUSS  1     /* RE2 ROOM40A0 Satz 0x08 (= RE2_ELZA_SE_SCHUSS) */
/* Roys Sterbe-Ton = RE1.5-Raum-SE 5 der snd1-Bank (FUN_80106c18 @0x80106da8-ac), ueber re15_audio_room_se. */
#define RE15_ELZA_ROY_SE      5

/* Aktionen der Anker (Pruefhaken). */
#define RE15_ELZA_AKT_AUF      1
#define RE15_ELZA_AKT_SCHUSS   2
#define RE15_ELZA_AKT_ROY_TOD  3
#define RE15_ELZA_AKT_STOEHNEN 4
#define RE15_ELZA_AKT_N        5

/* HAKEN scd_vm.c scd_register_current_rdt: Anker des Raums im rohen RDT suchen (Signaturpruefung). */
void re15_elza_r36_raum_scan(const unsigned char *raw, int raw_size, unsigned room_id);
/* HAKEN scd_vm.c Opcode-Schleife: vor jedem ausgefuehrten Opcode (Wachposten g_re15_elza_anker_n). */
extern int g_re15_elza_anker_n;
void re15_elza_r36_pc(const unsigned char *pc);
/* HAKEN scd_vm.c Bildtakt (neben re15_irons_tod_tick): verzoegerte Anker-Aktionen. */
void re15_elza_r36_tick(void);
/* HAKEN scd_room_setup.c (Ende des Installer-Blocks) + main.c Boot-/CONTINUE-Weg: P2 Irons 1151. */
void re15_elza_r36_install(uint16_t room_id);
/* HAKEN audio_pc.c re15_voice_load_clip: Dateiname (ohne .wav) fuer (Raum, Nachricht) oder NULL. */
const char *re15_elza_stimme_datei(unsigned room_id, int msg_id);
/* Plattform-Haken fuer die RE2-Toene (PC: elza_se_pc.c). NULL = stumm. */
extern void (*re15_elza_se_hook)(int ton);

/* Pruefhaken. */
int            re15_elza_r36_zaehler(int aktion);
int            re15_elza_r36_letzter_takt(int aktion);     /* VM-Takt (g_scd.tick_count) der letzten Ausfuehrung */
unsigned       re15_elza_r36_anker_off(int i);             /* RDT-Datei-Offset des i-ten armierten Ankers */
const uint8_t *re15_elza_r36_meldung(int welche, int *out_len);   /* 0 Irons, 1 Stoehnen */
void           re15_elza_r36_reset(void);

#endif /* RE15_ELZA_R36_H */
