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

/* SCHNITTSTELLE (s. Kopf). */
void re15_bf_g5_auftritt_start(int slot);

void re15_g5a_tick(void);      /* je KI-Tick aus re15_bf_raumtick (vor der Gegner-Schleife) */
void re15_g5a_reset(void);     /* Raumwechsel (re15_bf_reset) */
int  re15_g5a_laeuft(void);    /* 1 = Szene aktiv */
int  re15_g5a_bild(void);      /* Bild seit der Armierung, -1 = aus */

/* RID-Satz eines Szenen-Schnitts (16/17) in ROOM5090/5091, sonst NULL (jeder andere Raum, jeder andere Index). */
const re15_camera_cut_t *re15_g5a_schnitt(int cut);
/* RE2-Schnittnummer (9/10) eines Szenen-Schnitts in ROOM5090/5091, sonst -1 (Dateinamen RE15_G5A_DATEI_FORM). */
int  re15_g5a_re2_schnitt(int cut);
/* Lichtsatz fuer einen Schnitt: 16/17 -> 11 (PORT-WAHL, s. g5auftritt_r41.c), sonst `cut`. */
int  re15_g5a_licht_schnitt(int cut);

#endif /* RE15_G5AUFTRITT_R41_H */
