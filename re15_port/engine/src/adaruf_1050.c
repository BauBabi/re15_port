/*
 * RE1.5 Rebuilt — Ada-Ruf an der Tuer ROOM1050 -> ROOM10A0, Sperre bis zur Ada-Rettung
 * (Runde 34 Nacht, Spur D).
 *
 * Herleitung, Belege und alle Konstanten: include/re15_adaruf.h und
 * analysis/befunde_runde34_nacht/D_adaruf.md. Kein Asset-Patch: die RDT bleibt byte-true, der Port
 * widmet nach dem Init-Lauf von main00 den Tuer-Slot so um, wie es ein Aot_reset tun wuerde, und
 * spielt die Szene als Bytecode aus ORIGINAL-Opcodes ueber die vorhandene VM.
 */
#include "re15_adaruf.h"

#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_msg.h"
#include "re15_scd.h"
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

/* ---- Die Szene (142 Bytes). Jede Zeile traegt ihr Vorbild im Auslieferungsstand (Datei-Offsets).
 * Opcode-Laengen = s_opcode_sizes (scd_vm.c). Die Form ist die der RE1.5-Szenen, die der Nutzer
 * nennt: ROOM1090 sub02 (Ruf der Frau, Rueckschritt nach dem Lauf zum Feuer), ROOM1090 sub03
 * (Clip 19 vor + zurueck), ROOM1170 sub02 (Clip 17), ROOM1050 sub03 (Plc_dest Modus 9 + Warte-
 * schleife), ROOM1130 sub01 (Tuer -> Text-Platz).
 * Zeitlinie (echte VM, Sonde probe_r34n_d_adaruf, B = Bilder nach dem Quadrat-Druck): Balken ab B1
 * (voll ab B16), Ruf B21..B121, Drehung ab B121, Rueckschritt B123..B133, Leon-Zeilen ab B154/B205,
 * Szenen-Ende B305 (+15 Bilder Balken-Rampe). */
static const uint8_t k_ruf[RE15_ADARUF_PROG_LEN] = {
    /* +00 */ 0x22, 0x09, 0x41, 0x01,   /* Set(9,65)=1 "Szene gesehen" — Einmal-Riegel als ERSTES Opcode
                                           wie ROOM11B0 sub06 @0x01478 `22 03 83 01`                      */
    /* +04 */ 0x22, 0x02, 0x07, 0x01,   /* Set(2,7)=1  Pad-Sperre    = ROOM1090 sub02 @0x02414            */
    /* +08 */ 0x22, 0x01, 0x1b, 0x01,   /* Set(1,27)=1 Letterbox     = ROOM1090 sub02 @0x02418 (Rampe
                                           FUN_80021a0c: `andi v0,v0,0x10` @0x80021a24, +16 @0x80021a54) */
    /* +0C */ 0x2e, 0x01, 0x00, 0x00,   /* Work_set(1,0)+Nop Spieler = ROOM1090 sub02 @0x0241C            */
    /* +10 */ 0x40, 0x00, 0x06, 0x3f, 0x00, 0x00, 0x00, 0x00,
                                        /* Plc_dest Modus 6 = stehen bleiben (Tabelle 0x80073e30[6] =
                                           0x800517f0: Clip 1 einmal -> Clip-2-Ruhe) = ROOM1090 sub02
                                           @0x0242C                                                        */
    /* +18 */ 0x09, 0x0a, 0x14, 0x00,   /* Sleep 20                  = ROOM1090 sub02 @0x02434            */
    /* +1C */ 0x2b, RE15_ADARUF_MSG_RUF, 0x00, 0x00,
                                        /* Message_on 22 Maske 0 (Untertitel ohne Freeze) — Form ROOM1090
                                           sub02 @0x02438 `2b 00 00 00` (Ruf der Frau, msg 0)             */
    /* +20 */ 0x09, 0x0a, 0x64, 0x00,   /* Sleep 100                 = ROOM1090 sub02 @0x0243C            */
    /* +24 */ 0x40, 0x00, 0x09, 0x20, 0x00, 0x00, 0x00, 0x00,
                                        /* Plc_dest Modus 9 = auf der Stelle zum Blickpunkt drehen
                                           (0x80031360: Clip 5 @0x800313a4, Kegel `ori a2,zero,0x60`
                                           @0x800313d4, Rate 0x60 @0x80031440, kein Vortrieb), Bit 0x20;
                                           Form ROOM1050 sub03 @0x00DC2 `40 00 09 20 7a 3f f6 be`.
                                           Operand +0x28 = (x + 755, z), von der Weiche gesetzt.          */
    /* +2C */ 0x11, 0x00, 0x08, 0x00,   /* Do                        = ROOM1050 sub03 @0x00DCA            */
    /* +30 */ 0x02, 0x00,               /* Evt_next + Nop            = @0x00DCE                           */
    /* +32 */ 0x12, 0x04,               /* Edwhile                   = @0x00DD0                           */
    /* +34 */ 0x21, 0x05, 0x20, 0x00,   /* Ck(5,32)==0 (Ankunftsbit) = @0x00DD2                           */
    /* +38 */ 0x40, 0x00, 0x08, 0x20, 0x00, 0x00, 0x00, 0x00,
                                        /* Plc_dest Modus 8 = Rueckschritt (0x800311f0: 70/Bild @0x80031210,
                                           Ruecken zum Ziel `addiu a2,zero,-48` @0x80031254, Vortrieb
                                           Gierung+0x800 @0x8003125c, Ankunft `slti v0,v0,100` @0x800312fc),
                                           Bit 0x20 — Form ROOM1090 sub02 @0x0247C `40 00 08 20 7e ff 3c f8`.
                                           Operand +0x3C = (x - 755, z), von der Weiche gesetzt.          */
    /* +40 */ 0x11, 0x00, 0x08, 0x00,   /* Do                        = ROOM1090 sub04 @0x026E6 (dort per
                                           Gosub @0x02484; hier eingebettet, ROOM1050 hat kein solches Sub) */
    /* +44 */ 0x02, 0x00,               /* Evt_next + Nop            = sub04 @0x026EA                     */
    /* +46 */ 0x12, 0x04,               /* Edwhile                   = sub04 @0x026EC                     */
    /* +48 */ 0x21, 0x05, 0x20, 0x00,   /* Ck(5,32)==0 (Ankunft)     = sub04 @0x026EE                     */
    /* +4C */ 0x40, 0x00, 0x09, 0x20,
              (uint8_t)(RE15_ADARUF_KAMERA_X & 0xff), (uint8_t)((RE15_ADARUF_KAMERA_X >> 8) & 0xff),
              (uint8_t)(RE15_ADARUF_KAMERA_Z & 0xff), (uint8_t)((RE15_ADARUF_KAMERA_Z >> 8) & 0xff),
                                        /* Plc_dest Modus 9: zur Kamera drehen (Cut 4, re15_adaruf.h
                                           RE15_ADARUF_KAMERA_*). PORT-WAHL, Grund dort: nur frontal
                                           liest sich die Geste so, wie der Nutzer sie beschreibt.
                                           Form wie +0x24 (ROOM1050 sub03 @0x00DC2).                      */
    /* +54 */ 0x11, 0x00, 0x08, 0x00,   /* Do                        = ROOM1050 sub03 @0x00DCA            */
    /* +58 */ 0x02, 0x00,               /* Evt_next + Nop            = @0x00DCE                           */
    /* +5A */ 0x12, 0x04,               /* Edwhile                   = @0x00DD0                           */
    /* +5C */ 0x21, 0x05, 0x20, 0x00,   /* Ck(5,32)==0 (Ankunftsbit) = @0x00DD2                           */
    /* +60 */ 0x09, 0x0a, 0x14, 0x00,   /* Sleep 20                  = ROOM1090 sub02 @0x02486            */
    /* +64 */ 0x2b, RE15_ADARUF_MSG_LEON_A, 0x00, 0x00,
                                        /* Message_on 23 "Leon: Another civilian survivor." — Form
                                           @0x0248A `2b 01 00 00`                                          */
    /* +68 */ 0x3f, 0x00, 0x13, 0x00,   /* Plc_motion(0,19,0) Arm hinaus = ROOM1090 sub03 @0x0265C
                                           (Handler 0x80041b90: `sb a1,148(v0)` @0x80041ba8 = Clip)        */
    /* +6C */ 0x09, 0x0a, 0x19, 0x00,   /* Sleep 25                  = sub03 @0x02660                     */
    /* +70 */ 0x3f, 0x00, 0x13, 0x00,   /* Plc_motion(0,19,0)        = sub03 @0x02664                     */
    /* +74 */ 0x43, 0x00, 0x80, 0x00,   /* Plc_flg(0,0x80,0) rueckwaerts = sub03 @0x02668 (Handler
                                           0x80041fb8, `or v0,v0,a2` @0x80041ffc auf +0x1c4)               */
    /* +78 */ 0x09, 0x0a, 0x1a, 0x00,   /* Sleep 26                  = sub03 @0x0266C                     */
    /* +7C */ 0x2b, RE15_ADARUF_MSG_LEON_B, 0x00, 0x00,
                                        /* Message_on 24 "Leon: I have to help her!" — Form sub03
                                           @0x02670 `2b 06 00 00`; der Stimmen-Riegel (voice_wait) haelt
                                           diese Zeile bis main23.wav zu Ende ist                          */
    /* +80 */ 0x3f, 0x00, 0x11, 0x00,   /* Plc_motion(0,17,0) Arm-Schwung = ROOM1170 sub02 @0x015F0       */
    /* +84 */ 0x09, 0x0a, 0x64, 0x00,   /* Sleep 100                 = ROOM1170 sub02 @0x015F4            */
    /* +88 */ 0x22, 0x02, 0x07, 0x00,   /* Set(2,7)=0                = ROOM1090 sub02 @0x024BE            */
    /* +8C */ 0x22, 0x01, 0x1b, 0x00,   /* Set(1,27)=0               = ROOM1090 sub02 @0x024C2            */
    /* +90 */ 0x2e, 0x01, 0x00, 0x00,   /* Work_set(1,0)+Nop         = ROOM1090 sub02 @0x024C6            */
    /* +94 */ 0x42, 0x00,               /* Plc_ret + Nop             = ROOM1090 sub02 @0x024CA (Handler
                                           0x80041f88: +0x4 = 1, +0x5..+0x7 = 0)                           */
    /* +96 */ 0x46, RE15_ADARUF_SLOT, RE15_ADARUF_SCE_TEXT, RE15_ADARUF_FLAGS,
              RE15_ADARUF_MSG_SPERRE, 0x00, 0xff, 0xff, 0x00, 0x00,
                                        /* Aot_reset(4, sce 1, 0x31, msg 25, 0xffff, 0) — Form ROOM1130
                                           sub01 @0x00A1C `46 03 01 31 01 00 ff ff 00 00`                 */
    /* +A0 */ 0x01, 0x00,               /* Evt_end                                                        */
};

/* Die RAM-Kopie, die die VM ausfuehrt: die Weiche setzt die zwei Ziel-Operanden aus der Spieler-
 * position im Druckbild ein. Warum relativ zum Standort: das Original stellt Leon vor dem
 * Rueckschritt per Member_set auf Ort UND Richtung (ROOM1090 sub02 @0x0246C/@0x02470/@0x02474,
 * versteckt hinter Cut_chg 15 @0x02468); ohne Schnitt waere ein solcher Sprung sichtbar. Die Tuer
 * liegt auf der Ostwand (Rechteck x 16700..17700 @0x00B5A), also: erst zur Wand drehen (+X), dann
 * 755 nach -X zurueck — z bleibt, keine Kamerazone wird gekreuzt (RVD-Umschaltbaender sind reine
 * z-Baender, ROOM1050 RVD @0x001B0, Saetze @0x002A0/@0x002B4/@0x002DC). */
static uint8_t s_prog[RE15_ADARUF_PROG_LEN];

/* ---- Nachrichten (.msg-Rohbytes). ⛔ NEUE TEXTE (Nutzervorgabe), keine Original-Saetze. Belegt ist
 * die FORM und jede Glyphe (Tabelle = Umkehrung von msg_common.c re15_msg_glyph):
 *   Dialog:  Kopf `04 00 05 cc <Sprecher> 16 05 00 00`, Ende `04 01 01 63` — ROOM1090 msg 0 @0x0275C
 *            (`04 00 05 02 33 4b 49 3d 4a 16 05 00 00` = "Woman:" Farbe 02) und msg 1 @0x0279C
 *            (`04 00 05 01 28 41 4b 4a 16 05 00 00` = "Leon:" Farbe 01); `01 63` = 99 Bilder Standzeit.
 *   Text:    Kopf `04 02`, Ende `01 00` — ROOM1130 msg 1 @0x0B46 (wie tuer1120_1130.c).
 *   Umbruch: 0x08 (FUN_80028868, msg_common.c re15_msg_layout).
 * Wortstuecke im Auslieferungsstand (tools/r34n_d/texte_bauen.py, Dossier §5.5):
 *   "Woman:" ROOM1090 @0x2760 · "Hel" ROOM1021 @0x2319 · "lo" ROOM1011 @0x11CD · "? A" ROOM30E1 @0x1099
 *   · "nyone" ROOM1031 @0x2FBC · "Please," ROOM1011 @0x11E1 · "get me out of here!" ROOM1090 @0x2784
 *   · "Leon:" ROOM1050 @0x103B · "An" ROOM1010 @0x0A90 · "other " ROOM1011 @0x1164 · "ci" ROOM1011
 *   @0x0FFE · "vi" ROOM1011 @0x1156 · "li" ROOM1011 @0x126A · "an su" ROOM1240 @0x0673 · "rvivor"
 *   ROOM1011 @0x1155 · "." (0x57) ROOM1000 @0x0D33 · "I have to " ROOM1170 @0x1C4B · "help her"
 *   ROOM11B0 @0x1BBA · "!" ROOM1011 @0x0EF0 · "the " ROOM1011 @0x0F13 · "S" ROOM1010 @0x0A54 ·
 *   "urvivor" ROOM1011 @0x1154 · " first" ROOM11B0 @0x1BAE.
 * Sprecher "Woman:" statt "Ada:" = Nutzer-Nachtrag 2026-09-30 und RE1.5-Konvention vor der Rettung
 * (ROOM1090 msg 0 @0x0275C, msg 2 @0x027BD, msg 4 @0x0281F). "Survivor" gross = Nutzer-Schreibung. */
static const uint8_t k_msg22[] = {   /* "Woman: Hello? Anyone? Please, / get me out of here!" */
    0x04, 0x00, 0x05, 0x02, 0x33, 0x4b, 0x49, 0x3d, 0x4a, 0x16, 0x05, 0x00, 0x00,
    /* H    e     l     l     o     ?    _     A     n     y     o     n     e     ?    _          */
    0x24, 0x41, 0x48, 0x48, 0x4b, 0x1b, 0x00, 0x1d, 0x4a, 0x55, 0x4b, 0x4a, 0x41, 0x1b, 0x00,
    /* P    l     e     a     s     e     ,                                                        */
    0x2c, 0x48, 0x41, 0x3d, 0x4f, 0x41, 0x18,
    0x08,  /* Umbruch nach "Please," — Vorbild ROOM1090 msg 0 bricht vor "get me out of here!?" */
    /* g    e     t    _     m     e    _     o     u     t    _     o     f    _     h     e     r     e     ! */
    0x43, 0x41, 0x50, 0x00, 0x49, 0x41, 0x00, 0x4b, 0x51, 0x50, 0x00, 0x4b, 0x42, 0x00, 0x44, 0x41, 0x4e,
    0x41, 0x1a,
    0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg23[] = {   /* "Leon: Another civilian survivor." */
    0x04, 0x00, 0x05, 0x01, 0x28, 0x41, 0x4b, 0x4a, 0x16, 0x05, 0x00, 0x00,
    /* A    n     o     t     h     e     r    _                                                   */
    0x1d, 0x4a, 0x4b, 0x50, 0x44, 0x41, 0x4e, 0x00,
    /* c    i     v     i     l     i     a     n    _                                             */
    0x3f, 0x45, 0x52, 0x45, 0x48, 0x45, 0x3d, 0x4a, 0x00,
    /* s    u     r     v     i     v     o     r     .                                            */
    0x4f, 0x51, 0x4e, 0x52, 0x45, 0x52, 0x4b, 0x4e, 0x57,
    0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg24[] = {   /* "Leon: I have to help her!" */
    0x04, 0x00, 0x05, 0x01, 0x28, 0x41, 0x4b, 0x4a, 0x16, 0x05, 0x00, 0x00,
    /* I   _     h     a     v     e    _     t     o    _     h     e     l     p    _     h     e     r     ! */
    0x25, 0x00, 0x44, 0x3d, 0x52, 0x41, 0x00, 0x50, 0x4b, 0x00, 0x44, 0x41, 0x48, 0x4c, 0x00, 0x44, 0x41, 0x4e,
    0x1a,
    0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg25[] = {   /* "I have to help the Survivor first!" (eine Zeile) */
    0x04, 0x02,
    /* I   _     h     a     v     e    _     t     o    _     h     e     l     p    _              */
    0x25, 0x00, 0x44, 0x3d, 0x52, 0x41, 0x00, 0x50, 0x4b, 0x00, 0x44, 0x41, 0x48, 0x4c, 0x00,
    /* t    h     e    _     S     u     r     v     i     v     o     r    _     f     i     r     s     t     ! */
    0x50, 0x44, 0x41, 0x00, 0x2f, 0x51, 0x4e, 0x52, 0x45, 0x52, 0x4b, 0x4e, 0x00, 0x42, 0x45, 0x4e, 0x4f, 0x50,
    0x1a,
    0x01, 0x00,
};

static const struct { uint8_t id; const uint8_t *b; uint8_t n; } k_meldungen[] = {
    { RE15_ADARUF_MSG_RUF,    k_msg22, (uint8_t)sizeof k_msg22 },
    { RE15_ADARUF_MSG_LEON_A, k_msg23, (uint8_t)sizeof k_msg23 },
    { RE15_ADARUF_MSG_LEON_B, k_msg24, (uint8_t)sizeof k_msg24 },
    { RE15_ADARUF_MSG_SPERRE, k_msg25, (uint8_t)sizeof k_msg25 },
};
#define ADARUF_N_MELDUNGEN ((int)(sizeof k_meldungen / sizeof k_meldungen[0]))

static uint8_t s_zustand = RE15_ADARUF_AUS;

const uint8_t *re15_adaruf_programm(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_ruf;
    return k_ruf;
}

const uint8_t *re15_adaruf_laufprogramm(void) { return s_prog; }

const uint8_t *re15_adaruf_meldung(int msg_id, int *out_len)
{
    for (int i = 0; i < ADARUF_N_MELDUNGEN; i++)
        if ((int)k_meldungen[i].id == msg_id) {
            if (out_len) *out_len = (int)k_meldungen[i].n;
            return k_meldungen[i].b;
        }
    if (out_len) *out_len = 0;
    return NULL;
}

int re15_adaruf_zustand(void) { return s_zustand; }

static void meldungen_einsetzen(void)
{
    for (int i = 0; i < ADARUF_N_MELDUNGEN; i++) {
        re15_msg_install_text(k_meldungen[i].id, k_meldungen[i].b, k_meldungen[i].n);
        int d = re15_msg_compute_duration(k_meldungen[i].b, k_meldungen[i].n, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(k_meldungen[i].id, d);
    }
}

void re15_adaruf_install(uint16_t room_id)
{
    s_zustand = RE15_ADARUF_AUS;
    if (room_id != RE15_ADARUF_RAUM) return;
    if (re15_game_flag_get(RE15_ADARUF_FREI_BANK, RE15_ADARUF_FREI_BIT)) return;   /* Ada gerettet */

    /* Nur den echten Tuer-Satz umwidmen: main00 @0x00B5A hat Slot 4 als Tuer angelegt. Steht dort
     * etwas anderes (kuenftige Skript-Aenderung, Test mit leerem Raum), bleibt alles, wie es ist. */
    const re15_aot_t *a = &g_aot.slots[RE15_ADARUF_SLOT];
    if (!a->active || a->type != RE15_AOT_TYPE_DOOR) return;

    meldungen_einsetzen();
    /* Aot_reset-Semantik (LAB_80040738): rec[0] = sce, rec[1] = flags, Nutzlast; Rechteck und Band
     * bleiben die der Tuer. Das Band steht im Original in rec[2] und wird von Aot_reset nicht
     * geschrieben (@0x8004076c-78 fasst nur rec[0]/rec[1] an); der Port haelt das Tuer-Band in
     * door_params (op_door_aot_set: pc[4] = 0x00 @0x00B5E), die anderen Platz-Typen lesen a->band —
     * also hier uebertragen (wie re15_tuer1120_install). */
    g_aot.slots[RE15_ADARUF_SLOT].band = g_aot.door_params[RE15_ADARUF_SLOT].band;
    if (!re15_game_flag_get(RE15_ADARUF_GESEHEN_BANK, RE15_ADARUF_GESEHEN_BIT)) {
        re15_aot_retype(RE15_ADARUF_SLOT, RE15_ADARUF_SCE_EREIGNIS, RE15_ADARUF_FLAGS,
                        RE15_ADARUF_P0_EREIGNIS, RE15_ADARUF_P1_EREIGNIS, 0);
        s_zustand = RE15_ADARUF_SZENE;
    } else {
        re15_aot_retype(RE15_ADARUF_SLOT, RE15_ADARUF_SCE_TEXT, RE15_ADARUF_FLAGS,
                        RE15_ADARUF_MSG_SPERRE, RE15_ADARUF_MASKE_TEXT, 0);
        s_zustand = RE15_ADARUF_SPERRE;
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[adaruf] ROOM%04X Slot %d -> %s (Flags (%d,%d)=0, (%d,%d)=%d)\n",
            (unsigned)room_id, RE15_ADARUF_SLOT,
            s_zustand == RE15_ADARUF_SZENE ? "Ereignis-Platz sce 3 / Ereignis 13 (Szene)"
                                           : "Text-Platz sce 1 / msg 25 (Sperre)",
            RE15_ADARUF_FREI_BANK, RE15_ADARUF_FREI_BIT, RE15_ADARUF_GESEHEN_BANK,
            RE15_ADARUF_GESEHEN_BIT,
            re15_game_flag_get(RE15_ADARUF_GESEHEN_BANK, RE15_ADARUF_GESEHEN_BIT));
#endif
}

/* Laeuft das Programm schon in einem Faden? (Gegenpruefung Auflage 5: ein zweiter Ausloeser darf
 * weder einen zweiten Faden starten noch die Ziele des laufenden umschreiben.) */
static int programm_laeuft(void)
{
    for (int s = 0; s < SCD_THREAD_COUNT; s++) {
        const scd_thread_t *t = &g_scd.threads[s];
        if (t->active && t->pc >= s_prog && t->pc < s_prog + sizeof s_prog) return 1;
    }
    return 0;
}

static void s16_setzen(uint8_t *p, int32_t v)
{
    p[0] = (uint8_t)(v & 0xff);
    p[1] = (uint8_t)((v >> 8) & 0xff);
}

const uint8_t *re15_adaruf_ereignis(uint16_t room_id, uint8_t event_id)
{
    if (room_id != RE15_ADARUF_RAUM || event_id != RE15_ADARUF_EREIGNIS) return NULL;
    if (re15_game_flag_get(RE15_ADARUF_GESEHEN_BANK, RE15_ADARUF_GESEHEN_BIT)) return NULL;
    if (re15_game_flag_get(RE15_ADARUF_FREI_BANK, RE15_ADARUF_FREI_BIT)) return NULL;
    if (programm_laeuft()) return NULL;

    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memcpy(s_prog, k_ruf, sizeof s_prog);
    s16_setzen(&s_prog[RE15_ADARUF_OFF_DREH + 0], pl->x + RE15_ADARUF_SCHRITT);
    s16_setzen(&s_prog[RE15_ADARUF_OFF_DREH + 2], pl->z);
    s16_setzen(&s_prog[RE15_ADARUF_OFF_ZIEL + 0], pl->x - RE15_ADARUF_SCHRITT);
    s16_setzen(&s_prog[RE15_ADARUF_OFF_ZIEL + 2], pl->z);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[adaruf] Ereignis %d: Szene startet, Spieler (%d,%d) Gierung %d -> Blickpunkt "
                    "(%d,%d), Rueckschritt-Ziel (%d,%d)\n",
            (int)event_id, (int)pl->x, (int)pl->z, (int)pl->rot_y,
            (int)(pl->x + RE15_ADARUF_SCHRITT), (int)pl->z,
            (int)(pl->x - RE15_ADARUF_SCHRITT), (int)pl->z);
#endif
    return s_prog;
}
