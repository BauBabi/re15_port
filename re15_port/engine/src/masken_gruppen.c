/*
 * RE1.5 Rebuilt — Sichtbarkeit der sprite.pri-Masken je Gruppe + SCD-Opcode 0x45 Col_chg_set.
 * Belege (Adressen + Instruktionen): include/re15_masken_gruppen.h.
 * Dossier: analysis/befunde_runde34_nacht/G2_schrift1150.md §3/§5/§9.
 *
 * Datei-Name weicht vom Muster <thema>_<raum>.c ab (VERTRAG §2): die Korrektur ist
 * raumuebergreifend (Opcode 0x45 steht in 10 RDTs, Dossier §7.1).
 */
#include "re15_masken_gruppen.h"

#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "re15_engine.h"      /* g_engine.frame_count — nur fuer die Mess-Zeile */
#include "re15_room.h"        /* g_current_room_id   — nur fuer die Mess-Zeile */
#endif

/* Record-Tabelle DAT_800b2584 (Byte0/Byte1; die Tiefe fuehrt der Port im Maskenparser). */
static uint8_t s_byte0[RE15_MG_MAX];
static uint8_t s_byte1[RE15_MG_MAX];
/* RDT[0]: Zahl der Records, die 0x45 (@0x800396b8) und das Zeichnen (@0x800395c0) sehen.
 * Frisch geladene RDT: Byte 0 == 0 in allen 206 ausgelieferten RDTs (Gegenpruefung §1). */
static int     s_zahl;

#ifdef RE15_PLATFORM_PC
/* MESS-ZEILE (kein Verhalten): RE15_MG_LOG=<datei> haengt je Aufbau und je Opcode 0x45 eine
 * Zeile an. Die GUI-exe hat kein verlaessliches stderr, deshalb eine Datei. */
static void mg_log(const char *fmt, ...)
{
    const char *p = getenv("RE15_MG_LOG");
    if (!p || !*p) return;
    FILE *f = fopen(p, "ab");
    if (!f) return;
    /* wv0A/wv0C = gezeigter/alter Cut (DAT_800b0fe4/DAT_800b0fe8) — belegt in der Mess-Zeile,
     * dass ein Menue sie NICHT umschreibt (Gegenpruefung Auflage 5). */
    fprintf(f, "[maskgrp] F%u Raum %04X wv0A=%d wv0C=%d ", (unsigned)g_engine.frame_count,
            (unsigned)g_current_room_id, (int)g_scd.work_vars[0x0A],
            (int)g_scd.work_vars[0x0C]);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    fputc('\n', f);
    fclose(f);
}
#else
#define mg_log(...) ((void)0)
#endif

static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }

void re15_mg_aufbauen(const re15_rdt_t *rdt, int cut, const char *grund)
{
    (void)grund;   /* nur fuer die Mess-Zeile (PC) */
    if (!rdt || !rdt->raw || !rdt->cuts || cut < 0 || cut >= rdt->cut_count) {
        s_zahl = 0;
        mg_log("Aufbau Cut %d ungueltig -> Zahl 0 Grund %s", cut, grund ? grund : "");
        return;
    }
    const uint8_t *raw = rdt->raw;
    const uint32_t size = (uint32_t)rdt->raw_size;
    const uint32_t off  = rdt->cuts[cut].pri_offset;
    if (size < 8u || off > size - 4u) {
        s_zahl = 0;
        mg_log("Aufbau Cut %d pri_offset=0x%X ausserhalb -> Zahl 0 Grund %s",
               cut, (unsigned)off, grund ? grund : "");
        return;
    }
    const uint8_t *p = raw + off;
    const uint32_t kopf = (uint32_t)p[0] | ((uint32_t)p[1] << 8)
                        | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);   /* @0x80039324 */

    /* NULL-Sektion: @0x80039328-2c Vergleich mit -1, @0x80039338 `sb zero,0(a0)` -> RDT[0] := 0;
     * der Sprung @0x80039334 geht an der Loeschschleife vorbei (Byte0 bleibt stehen). */
    if (kopf == 0xFFFFFFFFu) {
        s_zahl = 0;
        mg_log("Aufbau Cut %d pri_offset=0x%X NULL-Sektion -> Zahl 0 Grund %s",
               cut, (unsigned)off, grund ? grund : "");
        return;
    }

    /* RDT[0] := (kopf >> 16) & 0xFF — @0x80039330 `srl t2,v1,16`, @0x80039358 `sb t2,0(a0)`. */
    s_zahl = (int)((kopf >> 16) & 0xFFu);

    /* Loeschschleife @0x8003936c-84: a3 = RDT[7]; do { a3--; Byte0 := 0; } while (a3 & 0xFF). */
    {
        int n = raw[7] ? (int)raw[7] : 256;
        if (n > RE15_MG_MAX) n = RE15_MG_MAX;
        memset(s_byte0, 0, (size_t)n);
    }

    /* Gruppen (je 8-Byte-Kopf, u16 Anzahl bei +0) — Aussen-Schleife @0x80039544-54 ueber
     * group_count = kopf & 0xFFFF (@0x80039344 andi t8,t8,0xffff), Innen-Schleife @0x8003950c-38
     * ueber die Anzahl der Gruppe; Leer-Gruppen zaehlen den Index mit (@0x800393c0 -> 0x80039540).
     * Record-Index = laufende Maske in Bau-Reihenfolge (@0x80039494 `addiu s5,s5,4`) = dieselbe
     * Reihenfolge wie re15_pri_parse_section (pri_common.c). */
    const int gruppen = (int)(kopf & 0xFFFFu);
    int k = 0;
    for (int g = 0; g < gruppen && g < 256; g++) {
        const uint32_t kpos = off + 4u + (uint32_t)g * 8u;
        if (kpos + 2u > size) break;
        const int anzahl = (int)rd16(raw + kpos);
        for (int m = 0; m < anzahl && k < RE15_MG_MAX; m++, k++) {
            s_byte0[k] |= 1u;                    /* @0x800393d8-e4 lbu / ori 0x1 / sb */
            s_byte1[k] = (uint8_t)(g + 1);       /* @0x800393e8-ec addiu v0,a3,1 / sb */
        }
    }
    mg_log("Aufbau Cut %d pri_offset=0x%X Zahl %d Records %d Grund %s",
           cut, (unsigned)off, s_zahl, k, grund ? grund : "");
}

void re15_mg_setzen(uint8_t g1, uint8_t wert)
{
    int treffer = 0;
    for (int i = 0; i < s_zahl && i < RE15_MG_MAX; i++) {   /* @0x800396e4 sltu a2,a3 */
        if (s_byte1[i] == g1) {                             /* @0x800396d0-d8 */
            s_byte0[i] = wert;                              /* @0x800396e0 sb a1,0(v1) */
            treffer++;
        }
    }
    mg_log("Gruppe %d := %d (%d Treffer, Zahl %d)", (int)g1, (int)wert, treffer, s_zahl);
    (void)treffer;
}

int re15_mg_sichtbar(int i)
{
    if (i < 0 || i >= s_zahl || i >= RE15_MG_MAX) return 1;   /* PORT-KONSTRUKTION, s. Header */
    return (s_byte0[i] & 1u) != 0;                          /* @0x800395f0 andi v0,v0,0x1 */
}

int     re15_mg_zahl(void)    { return s_zahl; }
uint8_t re15_mg_byte0(int i)  { return (i >= 0 && i < RE15_MG_MAX) ? s_byte0[i] : 0; }
uint8_t re15_mg_byte1(int i)  { return (i >= 0 && i < RE15_MG_MAX) ? s_byte1[i] : 0; }

/* SCD-Opcode 0x45 Col_chg_set [op, op1, op2] — Handler @0x800428d4 (Tabelle @0x800745bc):
 * Schluessel op1 + 1 (@0x800428f8, 8 Bit @0x800396cc), Wert op2 (@0x800428f0), pc += 3
 * (@0x80042904), Rueckgabe 1 = weiter im selben Takt (@0x80042900). */
int op_col_chg_set(scd_thread_t *t)
{
    re15_mg_setzen((uint8_t)(t->pc[1] + 1u), t->pc[2]);
    t->pc += 3;
    return 1;
}
