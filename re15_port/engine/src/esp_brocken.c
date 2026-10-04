/*
 * esp_brocken.c — Runde 35 Spur D "redhawk": ESP-Routinen B 36/37 (Landung der Fleisch-Brocken).
 *
 * Belege je Zeile im Header re15_esp_brocken.h und im Dossier analysis/befunde_runde35/D_redhawk.md
 * (R3 = Routine 36 @0x800187c4, R4 = Routine 37 @0x8001885c, R6 = Anim-Stufe @0x8001a38c).
 *
 * VORHER (gemessen, probe_r35_redhawk messung, ROOM11D0, Stand 154a73c1): ein Redhawk-Toetungs-
 * schuss auf den Hund wirft 6 Raum-Id-7-Brocken (Router 0x80104610, FX(2,1|2) je Bild); alle 6
 * lebten nach 900 Bildern noch, B = 36, Zeile 0/2, Anim im Flug-Zyklus 0..4 — der Port kannte
 * Routine B 36/37 nicht (esp_fx_dispatch_b fuehrte nur 12/29), der Anim-Terminator wird ohne den
 * Sprung auf Record 6 nie erreicht.
 */
#include "re15_esp_brocken.h"
#include "re15_collision.h"   /* re15_collision_room_coll = FUN_8001c6e8 */
#include "re15_room.h"        /* g_room_rdt / g_room_rdt_ok */

#include <stdint.h>

static unsigned s_landungen = 0, s_abschluesse = 0;
unsigned re15_esp_brocken_landungen(void)   { return s_landungen; }
unsigned re15_esp_brocken_abschluesse(void) { return s_abschluesse; }

static uint16_t rd16(const uint8_t *r, int o) { return (uint16_t)(r[o] | (r[o + 1] << 8)); }
static void     wr16(uint8_t *r, int o, uint16_t v) { r[o] = (uint8_t)v; r[o + 1] = (uint8_t)(v >> 8); }

int re15_esp_brocken_b(re15_esp_fx_t *f)
{
    if (!f || !f->rows_base) return 0;
    const uint16_t b = rd16(f->row, 0x02);                       /* `lhu v0,2(v0)` @0x8001a2b4 */
    if (b == 36) {
        /* PORT (keine Original-Adresse): die Sammel-Bodenklemme in re15_esp_fx_tick Stufe (f) gilt fuer
         * diesen Platz nicht — den Boden kennt im Original NUR diese Routine (die Tick-Physik
         * @0x8001a2fc-388 ist reines xlat += vel, vel += acc, ohne Klemme). Mit der Klemme kaeme die
         * Weltlage nie unter h, und Routine 36 schluege nie an (genau der gemessene Haenger: vel.y
         * zitterte +-13 auf der Klemme). INT32_MAX = derselbe Wert wie ESP_KEIN_BODEN (re15_esp.c,
         * Granatenplaetze). */
        f->floor_y = INT32_MAX;
        /* P = Weltlage s16 (`lh v0,40/42/44(v1)` @0x800187d8/e4/f0 -> sp+16/20/24 als s32);
         * room_coll(&P, a1 = 0, a2 = 8, a3 = 0x100) (`addu a1,zero,zero` @0x800187dc, `ori a2,zero,0x8`
         * @0x800187e8, `ori a3,zero,0x100` @0x800187f4, `jal 0x8001c6e8` @0x800187f8). */
        const int16_t h = re15_collision_room_coll(g_room_rdt_ok ? &g_room_rdt : NULL,
                                                   (int32_t)f->wpos[0], (int32_t)f->wpos[2], 0, 8, 0x100u);
        if ((int32_t)h < (int32_t)f->wpos[1]) {                  /* sll/sra 16, `lw v1,20(sp)`,
                                                                 * `slt v0,v0,v1` @0x80018804-10 */
            wr16(f->row, 0x02, 37);                              /* `ori v1,zero,0x25` / `sh v1,2(v0)`
                                                                 * @0x80018818/28 */
            f->drift_x = 0; f->drift_y = 0; f->drift_z = 0;      /* `sh zero,16/18/20(v0)`
                                                                 * @0x8001882c/30/38 (Port: drift = slot+0x10) */
            wr16(f->row, 0x10, 0); wr16(f->row, 0x12, 0); wr16(f->row, 0x14, 0);
            s_landungen++;
        } else {
            wr16(f->row, 0x1e, (uint16_t)h);                     /* `sh a0,30(v0)` @0x80018848 */
        }
        return 1;
    }
    if (b == 37) {
        /* `lh v1,42(a0)` Welt-y / `lh v0,30(a0)` h / `slt v0,v0,v1` / `beq` @0x8001886c-7c */
        if ((int32_t)(int16_t)rd16(f->row, 0x1e) < (int32_t)f->wpos[1]) {
            f->flags = f->row[0x0e];                             /* `lbu v0,14(a0)` / `sb v0,108(a0)`
                                                                 * @0x80018884/8c */
            f->frame = (int16_t)f->row[0x16];                    /* `lbu v0,22(v1)` / `sb v0,110(v1)`
                                                                 * @0x8001889c/a4 (Delay-Slot) */
            re15_esp_fx_zeile_weiter(f);                         /* `jal 0x800174e4` @0x800188a0 */
            s_abschluesse++;
        }
        return 1;
    }
    return 0;
}
