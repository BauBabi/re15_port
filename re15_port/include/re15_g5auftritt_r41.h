/*
 * RE1.5 Rebuilt — G5-Auftritt nach dem Rausschiessen der Riesenform (ROOM5090/5091), Runde 41 Zweig r41/g5auftritt.
 * Dossier: analysis/befunde_runde41/G5_auftritt.md.
 *
 * Nutzer 2026-11-02: "nach dem raus schiessen von Birkin aus dem Zug, sollst du die letzte Stufe GENAUSO machen wie bei
 * Resident Evil 2 [...] Grossaufnahme an seitlich der offenen Stelle des Zuges, und dann kommen da zunaechst die Tentakel
 * vom finalen Birkin G5, und der zieht ihn dann rein."
 *
 * VORLAGE: RE2 ROOM7040 MAIN sub06 ab der Armierung @0x018DE (Member_set(0x17,1) = G5 +0x1D4 Bit 0). Die Szene ist
 * Skript + G5-Overlay; der G5 selbst (enemy_ai_boss_g5.c, Sub 2 [T0..T14]) laeuft unveraendert, dieses Modul fuehrt
 * Kamera, Spieler und Sperre wie sub06 und armiert den G5 im selben Bild wie RE2.
 *
 * SCHNITTSTELLE ZUR FORM-SPUR (r41/form): re15_bf_g5_auftritt_start(slot) GENAU EINMAL rufen, sobald die Riesenform
 * aus dem Zug verschwunden ist (Ende ihres Sturzes; heute: Rueckstoss-Szene Phase 2, birkin_formen.c). `slot` ist der
 * Aktor der Form (Typ 0x30). Ab dem Aufruf gehoert der Slot dem G5 (Typwechsel re15_bf_morph_zu_g5 + Armierung), die
 * Kamera und die Spielersperre dieser Szene. Die Form-Spur darf danach weder Kamera noch Slot noch Spieler anfassen.
 */
#ifndef RE15_G5AUFTRITT_R41_H
#define RE15_G5AUFTRITT_R41_H

#include <stdint.h>
#include "re15_camera.h"

/* Szenen-Schnitte fuer die zwei RE2-Kameras, die es nur in dieser Szene gibt: room7040 RVD (zone.rvd) fuehrt fuer Schnitt 9 und 10
 * NUR die Ankerzone (from 9/10 -> 0), keinen Wechselstreifen; RE1.5 ROOM5090 hat keine Kamera dieser Art (Dossier 1.5). Die
 * Spielkameras der Szene haben dagegen RE1.5-Zwillinge (RE2 4 = Cut 11, RE2 5/6 ~ Cut 12) und laufen im RE1.5-Schnitt.
 * PORT-WAHL der Nummern: ROOM5090/5091 fuehren 16 Schnitte 0..15 (RDT-Kopf Byte 1 = 0x10, RID @0x60); 16/17 sind frei. */
#define RE15_G5A_SCHNITT9         16     /* RE2 room7040 Schnitt 9: Grossaufnahme schraeg auf das Wagenende */
#define RE15_G5A_SCHNITT10        17     /* RE2 room7040 Schnitt 10: niedrig von Osten auf das Wagenende */
#define RE15_G5A_SCHNITT          RE15_G5A_SCHNITT9

/* RE2-Daten je Schnitt n (shared_assets/RE2/, Werkzeug tools/r41_g5auftritt/re2_schnitt9.py, Paket-Gate):
 *   R704_CUTnn.BSS     = info/re2leon/COMMON/BSS/ROOM704.BSS [n*0x10000, (n+1)*0x10000)
 *   R704_CUTnn.PRI     = sprite.pri-Abschnitt aus ROOM7040.RDT (Zeiger RID-Satz +0x1C bis zum Zeiger von n+1), RE2-Form
 *                        (re15_pri_parse_section_re2); Schnitt 9 [0xDD8, 0xF2C) 2 Gruppen / 32 Masken, Schnitt 10 [0xF2C, 0xF30) leer
 *   R704_CUT09_PRI.TIM = Masken-Atlas von Schnitt 9 (SLD-Laenge 0xCF40 aus STAGE7.BIN @0xE6), entpackt mit RE2s Verfahren
 *                        FUN_80010084 (RE1.5s FUN_800c47e8 / sld_common.c liest den RE2-Strom nicht); Schnitt 10 hat keinen
 *                        (STAGE7.BIN @0xE8 = 0x4CA4, chunk[L-4] = 0). */
#define RE15_G5A_DATEI_FORM       "R704_CUT%02d%s"

/* Zeitachse in KI-Bildern ab der Armierung (Bild 0 = sub06 @0x018DE), s. Dossier 1.2. Sleep n = n Bilder; Wsleeping (0x0C,
 * @0x80053ac0-d0) wartet, bis das XA-Bit 0x20 in 0x800cfbd8 faellt. XA-Dauer = (Laenge*16 - 0x1E) Sektoren (Ende-Sektor
 * @0x80012af4-b10) bei 150 Sektoren/s (Laufwerksmodus 0xC8 @0x8009a415) * 30 Bilder/s; Laengen aus STAGE7.BIN @0x144 (Zeiger
 * 0x800ce544 = 0x8011a144): Xa 0x0B @0x186 = (Kanal 4, 0, 84) -> 262,8 -> 263; Xa 0x0C @0x18C = (1, 0, 107) -> 336,4 -> 337. */
#define RE15_G5A_BILD_SCHNITT10   437    /* 1 @0x018E6 + 11 @0x018F0 + 60 @0x0190A + 60 @0x0191C + 263 Xa @0x01920 + 42 @0x01926 -> @0x0193A Cut_chg(10) */
#define RE15_G5A_BILD_SCHNITT5    807    /* + 1 @0x01944 + 337 Xa @0x01946 + 32 @0x0194C -> @0x01950 Cut_chg(5) */
#define RE15_G5A_BILD_ENDE        1162   /* + 355 @0x0195E -> @0x01962 Cut_chg(6) .. @0x01987 Set(1,0x1B,0): Kampf */

/* Ton der Szene (Abnahme 1 M5). RE2-XA: Xa_on (0x59, Handler @0x80058028 -> FUN_800129b4) mit Wsleep/Wsleeping @0x80053ac0-d0.
 *   Xa 0x0B @0x01920 `59 00 0b 00`: 1 @0x018E6 + 11 @0x018F0 + 60 @0x0190A + 60 @0x0191C = Bild 132
 *   Xa 0x0C @0x01946 `59 00 0c 00`: Cut_chg(10) @0x0193A (Bild 437) + Evt_next @0x01944 = Bild 438
 * Datei VOICE07.XAS (Stage-Tabelle @0x80010818[7] = 0x34A -> Satz @0x800988a4 + 0x34A*8: LBA 105782, 4447744 Byte = Disc
 * PL0/VOICE/VOICE07.XAS;1), Kanal/Block/Laenge aus STAGE7.BIN @0x186 `04 00 00 00 54 00` und @0x18C `01 00 00 00 6b 00`.
 * Lesefenster = Ende - Start = (Block+Laenge)*16 - 0x1E (FUN_800129b4 @0x80012af4-b10): 84*16-30 = 1314, 107*16-30 = 1682
 * (Laufzeit: Savestates der Abnahme-1-Aufnahme s06 Start/Ende 105786/107100, s12 105783/107465).
 * Gemessen in der RE2-Aufnahme (re2_g5_run1.mp4, Armierung 63,80 s): Xa 0x0B setzt bei 68,239 s ein (+4,44 s; Bild 132 = 4,40 s),
 * Xa 0x0C bei 79,6245 s, im selben Bild wie Schnitt 10 (79,626 s); beide laufen ohne Drift in Echtzeit. */
#define RE15_G5A_BILD_XA0B        132
#define RE15_G5A_BILD_XA0C        438
#define RE15_G5A_XA0B_DATEI       "R704_XA0B.XAS"   /* Kanal 4, Sektoren 4..1316 von VOICE07.XAS (tools/r41_g5auftritt/re2_xa_schnitt.py) */
#define RE15_G5A_XA0C_DATEI       "R704_XA0C.XAS"   /* Kanal 1, Sektoren 1..1681 */
#define RE15_G5A_XA0B_LESE        1314
#define RE15_G5A_XA0C_LESE        1682

/* VORLAUF (Abnahme 1 M7): re15_bf_g5_auftritt_start beginnt mit RE2s Schnitt 9 VOR der Armierung (sub06 @0x0187C..@0x018D6: leere Tuer,
 * Leon laeuft von -3800 nach 1400 herein, schaut zur Tuer, Sleep 11); die Armierung (Bild 0) folgt danach. Bis dahin ist der Slot
 * schon Typ 0x36, aber unarmiert und geparkt (RE2 Sce_em_set @0x011AC). */

/* SCHNITTSTELLE (s. Kopf). */
void re15_bf_g5_auftritt_start(int slot);

void re15_g5a_tick(void);      /* je KI-Tick aus re15_bf_raumtick (vor der Gegner-Schleife) */
void re15_g5a_reset(void);     /* Raumwechsel (re15_bf_reset) */
int  re15_g5a_laeuft(void);    /* 1 = Szene aktiv */
int  re15_g5a_bild(void);      /* Bild seit der Armierung, -1 = aus */
int  re15_g5a_vorlauf(void);   /* Vorlaufbild seit dem Start (RE2 Schnitt 9 vor der Armierung, Abnahme 1 M7), -1 = kein Vorlauf */

/* RID-Satz eines Szenen-Schnitts (16/17) in ROOM5090/5091, sonst NULL (jeder andere Raum, jeder andere Index). */
const re15_camera_cut_t *re15_g5a_schnitt(int cut);
/* RE2-Schnittnummer (9/10) eines Szenen-Schnitts in ROOM5090/5091, sonst -1 (Dateinamen RE15_G5A_DATEI_FORM). */
int  re15_g5a_re2_schnitt(int cut);
/* Lichtsatz fuer einen Schnitt: 16/17 -> derselbe Index (RE2 room7040 Lichtsatz 9/10, von re15_g5a_licht_einsetzen auf 16/17
 * gelegt), sonst `cut`. */
int  re15_g5a_licht_schnitt(int cut);
/* Nach jedem Lichtsatz-Parse des Raums: in ROOM5090/5091 RE2 room7040 Lichtsatz 9 und 10 (ROOM7040.RDT @0x668/@0x690) auf die
 * Indizes 16/17 legen und cut_count auf 18 heben; jeder andere Raum: nichts. */
void re15_g5a_licht_einsetzen(void *licht_satz /* re15_light_set_t* */);

/* M4 (Nachbesserung 5): Tunnellampen = RE2 ROOM7040 Raumobjekte 3/4 (sub11/sub12, Modell R704_LAMPE.MD1/.TIM), Beben (sub04).
 * re15_g5a_lampe: i 0 = Objekt 3 (sub11), 1 = Objekt 4 (sub12); Weltlage und Drehung (Pos_set/Dir_set des Skripts, sonst der Parkplatz
 * (-32000,0,-32000) des Obj_model_set); Rueckgabe 1 = vom Skript gesetzt. re15_g5a_lampe_zone: RE2 FUN_8002c820 mit der Ankerzone des
 * Schnitts (16 = RE2 9, 17 = RE2 10), 1 = das Objekt wird gezeichnet. re15_g5a_beben: Sce_shake_on pc[2] dieses Bildes (das Bild rutscht
 * um -Wert Zeilen nach unten), 0 = kein Beben. re15_g5a_beben_ton_hook: Se_on @0x016AC (Plattform bindet die Mini-Bank R704_BEBEN.VBS). */
int  re15_g5a_lampe(int i, int32_t pos[3], int16_t dir[3]);
int  re15_g5a_lampe_zone(int cut, int32_t x, int32_t z);
int  re15_g5a_beben(void);
extern void (*re15_g5a_beben_ton_hook)(void);
/* NB6: Zahl der in dieser Szene gespawnten Brocken (RE2 sub05, 8 x Sce_espr_on Effekt 0x19; Messung). */
int  re15_g5a_brocken_gespawnt(void);

#endif /* RE15_G5AUFTRITT_R41_H */
