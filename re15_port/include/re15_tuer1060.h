/*
 * RE1.5 Rebuilt — Tuer ROOM1060 -> ROOM1040 (Treppenhaus, Etage 0) gesperrt, solange Leon den
 * Chief nicht geholt hat: "I have to get the Chief first..."
 *
 * Runde 35, Spur L. NUTZER-VORGABE (woertlich, analysis/befunde_runde35/AUFTRAG.md Z. 68):
 *   "Bei Wechsel von ROOM 1060 zu ROOM 1040 soll der Dialog kommen "I have to get the Chief
 *    first...", solange man nicht in ROOM 1150 war"
 * Dossier mit allen Belegen: analysis/befunde_runde35/L_cut1150.md (§2.1, §3.1).
 *
 * ⛔ PORT-WAHL AUF NUTZERWUNSCH. Im Original ist die Tuer immer offen (ROOM1060 main00 installiert
 * sie unbedingt, @0x00D52). Gebaut wie re15_tuer1120.h / re15_adaruf.h: ein Text-Platz (sce 1) auf
 * dem Rechteck der Tuer, solange ein Story-Flag fehlt — der RE1.5-eigene Mechanismus fuer genau
 * diesen Fall (ROOM1170 main00 @0x01320, ROOM1130 sub01 @0x00A1C `46 03 01 31 01 00 ff ff 00 00`).
 * Kein Asset-Patch: der Port widmet den Tuer-Slot nach dem Init-Lauf von main00 um, wie es ein
 * Aot_reset tun wuerde (LAB_80040738 schreibt rec[0]/rec[1] + Nutzlast, @0x8004076c-a8).
 *
 * BEDINGUNG (PORT-WAHL, aus dem Wortlaut "solange man nicht in ROOM 1150 war"):
 *   gesperrt <=> (9,73) = 0  UND  ( (9,71) = 1  ODER  (3,94) = 0 )
 *   (3,94) = erste Irons-Szene gelaufen (ROOM1150 sub08 @0x01110 `22 03 5e 01`) = "ueberhaupt schon
 *            beim Chief gewesen"; davor ist Leon nie in 1150 gewesen -> Sperre (Wortlaut).
 *   (9,71) = "10F0-Szene gesehen" (Spur K, VERTRAG §1.1): ab hier gilt der Plan "I'm going to get
 *            Chief Irons" (AUFTRAG Z. 62) -> Sperre, bis der Chief geholt ist.
 *   (9,73) = "Irons-Todesszene gesehen" (re15_irons_tod.h; die Szene startet beim Betreten von
 *            ROOM1150) = "in ROOM 1150 gewesen" -> frei.
 *   (1,27)/(2,7) sind KEINE Besucht-Latches (Letterbox FUN_80021a0c @0x80021a24 / Pad-Maske
 *   FUN_80030444 @0x800304f4), darum nicht benutzt.
 */
#ifndef RE15_TUER1060_H
#define RE15_TUER1060_H

#include <stdint.h>

/* Der Raum: ROOM1060 (Leon). ROOM1061 (Elza) bleibt offen — Elzas Spiel kennt weder die
 * 10F0-Szene noch die Irons-Szenen ((3,94) wird nur in ROOM1150 sub08 gesetzt, re15_tuer1120.h). */
#define RE15_TUER1060_RAUM        0x1060

/* Der Tuer-Satz: ROOM1060 main00 @Datei 0x00D52
 *   `3b 02 02 31 00 00 e8 67 ec 5e e8 03 98 08 f0 ad 00 00 b2 cc 40 08 00 04 05 00 ...`
 *   = Door_aot_set Slot 2, sce 2, flags 0x31, Etage 0, Rechteck (26600,24300,1000,2200)
 *     -> Stage 0 Raum 0x04 = ROOM1040, Spawn (-21008,0,-13134), Gierung 2112, Cut 5.
 *   (Slots 0/1 @0x00D12/@0x00D32 = dieselbe Flaeche auf Etage 8 -> ROOM1120 bzw. Etage 4 -> ROOM10C0,
 *   bleiben unangetastet.) */
#define RE15_TUER1060_SLOT        2

#define RE15_TUER1060_IRONS_BANK  3     /* erste Irons-Szene: ROOM1150 sub08 @0x01110 `22 03 5e 01` */
#define RE15_TUER1060_IRONS_BIT   94
#define RE15_TUER1060_K_BANK      9     /* Spur K: 10F0-Szene gesehen (VERTRAG §1.1 Bit 71) */
#define RE15_TUER1060_K_BIT       71
#define RE15_TUER1060_TOD_BANK    9     /* Spur L: Irons-Todesszene gesehen (re15_irons_tod.h Bit 73) */
#define RE15_TUER1060_TOD_BIT     73

/* Form des Text-Platzes = die der RE1.5-Vorbilder (re15_tuer1120.h): sce 1, flags 0x31
 * (Tuer-Satz @0x00D52 pc[3]), Pausemaske 0xffff (alle 524 ausgelieferten sce-1-Saetze). */
#define RE15_TUER1060_SCE_TEXT    1
#define RE15_TUER1060_FLAGS       0x31
#define RE15_TUER1060_MASKE       0xffff

/* Nachrichten-Id: ROOM1060-Nachrichtensektion @Datei 0x0E38, off[0] = `02 00` -> 1 Eintrag (Id 0).
 * VERTRAG §1.2: Spur L = 1..5 in ROOM1060. */
#define RE15_TUER1060_MSG_ID      1

/* Beim Raumaufbau (scd_room_setup.c, nach dem Init-Lauf von main00) und auf dem Boot-/CONTINUE-Weg
 * (main.c) rufen. Tut in jedem Raum ausser ROOM1060 nichts; bei erfuellter Freigabe nichts. */
void re15_tuer1060_install(uint16_t room_id);

/* Die Sperrbedingung allein (fuer Riegel und Dossier): 1 = Tuer wird zum Text-Platz. */
int  re15_tuer1060_sperre_aktiv(void);

/* Pruefhaken fuer Riegel: die eingebackene Nachricht (.msg-Rohbytes) und ob die Sperre im
 * aktuellen Raumaufbau gesetzt wurde. */
const uint8_t *re15_tuer1060_meldung(int *out_len);
int  re15_tuer1060_gesperrt(void);

#endif /* RE15_TUER1060_H */
