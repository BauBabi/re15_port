/*
 * RE1.5 Rebuilt — Runde 42 Strang finale, Stufe 1: Finale im hinteren Zugwagen ROOM5090 (Leon) nach dem RE2-Ende.
 *
 * NUTZERWUNSCH 07.11.2026 (woertlich): "Für Die Version 31 möchte ich dann, das du die Form Birkin G5 aus dem Finale wieder
 * entfernst. Die passt nicht... Stattdessen, machen wir ein finale wie beim Golem am Ende von Resident Evil 2. Das jemand Leon den
 * Raketenwerfer zuwirft, und man dann damit episch von mehreren Wiederholungen sieht, wie die Rakete angeflogen kommen und den
 * Gegner zersprengt."
 * NUTZERWUNSCH 17.11.2026 (woertlich): "Beim erschießen des endgegners mit dem raketenwerfer möchte ich das du das game over voiceover
 * von Leon übernimmst und das voiceover von Claire."
 *
 * Bauplan: Strang-Scratchpad FINALE_PLAN.md (S0-S4, S6-S8, S10), Dossiers A (RE2-Finale), B (Schlusszeile), C (Port heute).
 * Vorbild ist der RE2-Super-Tyrant (Typ 0x2B), Overlay build/extracted/re2_ems/CDEMD0_EM2B_ai1.BIN gegen 0x80100000 gelinkt
 * (Datei-Offset = Adresse - 0x80100000), und das Raumskript info/re2leon/PL0/RDT/ROOM7020.RDT (Datei-Offsets @0x0xxxx).
 *
 * Teile dieser Stufe:
 *   Wurzel (S4)    HP 20200, Merker "genug Schaden", Untergrenze 10000, "passend" (Gehzustand, Abstand >= 2801), Uhr 1800 Bilder,
 *                  Wurf-Ausloeser wie ROOM7020 sub11 (die Wurfszene selbst folgt in Stufe 3; hier nur Merker + Messzeile).
 *   Abschuss (S6)  Ausloeser wie die Tyrant-Wurzel @0x80101904-0x80101a00, Zustand 3 Phasen 0..9 @0x801035c0-0x80103e68, das
 *                  Raumprogramm von sub15 (@0x01A22-0x01A70) als Port-Programm aus Original-Opcodes, Schlusszeile "Game over."
 *                  als Message_on (K-11, VOICE-Option gilt).
 *   Ende (S7/S8)   (5,0x1C) + Bank-9-Bit 129 im Kill-Bild (FUN_80018fb0 @0x80103d7c) -> 5090 sub01 -> sub02 (Abspann-Vorspann)
 *                  -> ROOM6040 Diashow -> Set(0,1,0) @0x00590 -> Titel.
 *
 * Messschiene RE15_FINALE_LOG=1 -> finale.log, Zeilen "[finale] T<takt> F<bild> <ereignis>" (T = Spieltakt dieses Moduls).
 */
#ifndef RE15_FINALE_R42_H
#define RE15_FINALE_R42_H

#include <stdint.h>
#include "re15_actor.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Wurzel (S4) ------------------------------------------------------------------------------------------------------
 * HP: `lw v1,-0x48c(v1)` = 0x800cfb74 @0x80101b78-7c; Bit 0x40 gesetzt (`andi v0,v1,0x40 / beqz` @0x80101b84-88; RE2-Spielstart
 * setzt es bedingungslos @0x8002b4ac, Beleg re15_gator_re2_r41.h), Bit 0x20 = EASY/ROOKIE aus (`andi v0,v1,0x20 / beqz`
 * @0x80101b8c-90; der Port faehrt Normal, wie der G4-Zweig 1000 @0x80100678 in re15_birkin_formen.h) -> `lhu v0,0x4c96(v0)`
 * @0x80101bac = 200 (Datei @0x4C96 `c8 00`), `addiu v0,v0,0x4e20` @0x80101bb4, `sh v0,0x156(s0)` @0x80101be0. */
#define RE15_FI_HP_START   (20000 + 200)
#define RE15_FI_HP_GENUG   20000   /* `slti v0,v0,0x4e20` @0x80101ab8 -> +0x1D8 = 1 (`sh v0,0x1d8(s0)` @0x80101ac4)            */
#define RE15_FI_HP_UNTEN   10000   /* `slti v0,v0,0x2710` @0x80101ad0 -> `addiu v0,zero,0x2710 / sh v0,0x156(s0)` @0x80101ad8-dc */
#define RE15_FI_PASSEND_AB  2801   /* Gehzustand @0x80101d74: `lw v0,0x1f0(s0) / sltiu v0,v0,0xaf1` @0x80101d98-a0, sonst
                                    * `sh 1,0x1da(s0)` @0x80101db0; geloescht je Bild `sh zero,0x1da(s0)` @0x80101a10        */
#define RE15_FI_UHR         1800   /* ROOM7020 sub10 @0x017D4 `09 0a 08 07` = Sleep 0x0708, danach Set(5,0x0F,1) @0x017D8      */

/* ---- Abschuss (S6) ----------------------------------------------------------------------------------------------------- */
#define RE15_FI_BLICK        300   /* `slti v0,v0,0x12c` @0x801019dc: |Peilung - Spielergier| < 300 */
#define RE15_FI_SCHLAF_ZEILE  15   /* ROOM7020 sub15 @0x01A30 `09 0a 0f 00` */
/* Lesefenster der Zeile: Leon 12*16 - 0x1E = 162 Sektoren (STAGE7.BIN @0x150 `01 00 6b 00 0c 00`; Ende @0x80012ae8-0x80012b10),
 * 150 Sektoren/s (Modus 0xC8 @0x8009a415) = 1,08 s = 32,4 Bilder; Wsleeping prueft je Bild (@0x80053ac0-0x80053ad0) -> 33. */
#define RE15_FI_ZEILE_BILDER  33
#define RE15_FI_SCHLAF_W1A    32   /* sub15 @0x01A60 `09 0a 20 00` */
#define RE15_FI_SCHLAF_W1B     5   /* sub15 @0x01A64 `09 0a 05 00` */
#define RE15_FI_RAKETE_WEG     8   /* Phase 1: +0x16A == 8 -> FUN_8001cefc(2,0x0D,0) (`addiu v0,zero,8 / bne` @0x80103754-58) */
#define RE15_FI_W1_ABSTAND 10000   /* FUN_801016cc(e, 0x2710) @0x80103810-14 */
#define RE15_FI_W2_ABSTAND  5000   /* @0x80103914-18 `addiu a1,zero,0x1388` */
#define RE15_FI_W3_ABSTAND  3000   /* @0x80103a18-1c `addiu a1,zero,0xbb8` */
#define RE15_FI_W1_DAUER      40   /* `addiu v1,zero,0x28` @0x801038a4 */
#define RE15_FI_W1_HALT       15   /* `addiu v0,zero,0xf` @0x80103848 -> Bank 2 |= 0x10000000 (2,3) @0x80103888-9c */
#define RE15_FI_W2_DAUER      32   /* `addiu v1,zero,0x20` @0x801039a8 */
#define RE15_FI_W2_HALT        7   /* `addiu v0,zero,7` @0x8010394c */
#define RE15_FI_W3_DAUER      10   /* `addiu v1,zero,0xa` @0x80103a80 */
#define RE15_FI_NACHKNALL     14   /* Phase 8: `slti v0,v0,0xe` @0x80103c8c (alter Zaehler 0..13) */
#define RE15_FI_BROCKEN_BILD  16   /* Phase 8: `addiu v0,zero,0x10 / bne` @0x80103d24-28 */
#define RE15_FI_VSYNC_KNALL    5   /* `addiu v1,zero,5 / sb v1,-998(at)` @0x80103b00-08 */
/* Todesschrei Xa 0x03 direkt aus dem Overlay (`jal 0x800129b4` a0 = 0, a1 = 3 @0x80103bec-f4): STAGE7.BIN @0x156 `05 00 00 00 42 00`
 * = Kanal 5, Block 0, Laenge 66 -> 66*16 - 0x1E = 1026 Sektoren = 6,84 s = 205,2 Bilder; Phase 8 prueft je Bild das XA-Bit
 * (@0x80103d64-74) -> 206. Datei tools/r42_finale/re2_xa_finale.py. */
#define RE15_FI_XA03_DATEI   "R702_XA03_LEON.XAS"
#define RE15_FI_XA03_LESE    1026
#define RE15_FI_XA03_BILDER  206
/* Elza (5091) = Claire-Fassung (Dossier B 6: Claire-Overlay CDEMD1.EMS mit denselben Konstanten, Adressen -0x104): Zeile Xa 0x02 =
 * Claire-STAGE7.BIN @0x150 `06 00 4e 00 18 00` -> 24*16 - 0x1E = 354 Sektoren = 2,36 s = 70,8 -> 71 Bilder; Xa 0x03 = @0x156 Kanal 5
 * Block 0 Laenge 79 -> 79*16 - 0x1E = 1234 Sektoren = 8,23 s = 246,8 -> 247 Bilder. Die Datei kann nur aus der Claire-Disc des Nutzers
 * geschnitten werden (Dossier B 1.3: info/re2clair VOICE11.XAS ist Nullbytes) - bis dahin stumm, die Wartezeit gilt (Plan S9). */
#define RE15_FI_ZEILE_BILDER_ELZA 71
#define RE15_FI_XA03_DATEI_ELZA   "R702_XA03_CLAIRE.XAS"
#define RE15_FI_XA03_LESE_ELZA    1234
#define RE15_FI_XA03_BILDER_ELZA  247

/* Port-Signale Programm -> Takt (Bank 5 Wort 0, Raum-Scratch wie RE15_AN_SIG_*): Zensus 5090/5091 (scd_dump_room.py) belegt nur
 * (5,0x1C) und (5,32); Annette 0x10..0x13. Gegenstueck RE2: Member_set(0x18,2) @0x01A5C bzw. (0x18,3) @0x01A6C = Gegner +0x1D6. */
#define RE15_FI_SIG_FREI   0x14u
#define RE15_FI_SIG_W1     0x15u

/* Schlusszeile (K-11): Message_on-Id nach VERTRAG 2.1 (Zensus tools/r42_story/zensus_r42.py, Zeile ROOM5090 Id 2). */
#define RE15_FI_MSG_ZEILE_LEON  2
#define RE15_FI_MSG_ZEILE_ELZA  2   /* ROOM5091 Id 2 (Zensus wie 5090: RDT 0..1) */

/* Phasen (RE2 Zustand 3 +0x06; 0 = aus, 1.. = Phase + 1) */
enum {
    RE15_FI_AUS = 0, RE15_FI_P1 = 2, RE15_FI_P2 = 3, RE15_FI_P4 = 5, RE15_FI_P6 = 7, RE15_FI_P7 = 8, RE15_FI_P8 = 9, RE15_FI_P9 = 10,
    RE15_FI_ENDE = 11
};

/* Raum: 5090/5091 mit der Bahnsteig-Form (re15_bf_bahnsteig_aktiv). */
int  re15_finale_r42_raum(void);
/* Raumaufbau (scd_room_setup.c): Texte einsetzen, Zustand bei Raumwechsel loeschen (Selbst-Tueren behalten ihn). */
void re15_finale_r42_install(uint16_t room_id);
void re15_finale_r42_reset(void);

/* Wurzel: Start-HP beim Eintritt der Form (birkin_bahnsteig.c) und der Takt nach dem Zustand (RE2 @0x80101a10-0x80101adc). */
int  re15_finale_r42_start_hp(int bisher);
void re15_finale_r42_form_takt(int slot, int gehen, int32_t dist);

/* Am Anfang von re15_game_step: Ausloeser und Sequenz (RE2: Gegnerschleife vor dem Spieler, @0x80026570-0x80026620).
 * Rueckgabe 1 = pad/pressed wurden ersetzt. */
int  re15_finale_r42_vor_spielzug(uint16_t *pad, uint16_t *pressed);
/* Die Form gehoert waehrend der Sequenz dem Finale (KI schweigt, wie re15_annette_r41_besitzt). */
int  re15_finale_r42_besitzt(int slot);
/* Gate 2 des RE2-Geschoss-Appliers (@0x80047138-40) fuer die Riesenform: +0x1D3 Bit 0x80 bei gefuehrtem Raketenwerfer
 * (@0x801018d4-0x80101900) und ab Phase 0 (@0x8010361c-24). */
int  re15_finale_r42_kein_ziel(const re15_actor_t *e);

/* Abspann-Freigabe (S7): 1, sobald das Finale beendet ist (Bank 9 Bit RE15_R42_BIT_FINALE_ENDE). */
int  re15_finale_r42_ende(void);

/* Pruef- und Messzugriffe */
int  re15_finale_r42_phase(void);
int  re15_finale_r42_genug(void);
int  re15_finale_r42_passend(void);
int  re15_finale_r42_uhr(void);              /* verbleibende Bilder, -1 = noch nicht gestartet */
int  re15_finale_r42_wurf_faellig(void);
const uint8_t *re15_finale_r42_programm(int elza, int *out_len);
void re15_finale_r42_kampf_start_test(void);  /* nur Riegel: Uhr starten wie das Ende von sub04 */

#ifdef __cplusplus
}
#endif

#endif /* RE15_FINALE_R42_H */
