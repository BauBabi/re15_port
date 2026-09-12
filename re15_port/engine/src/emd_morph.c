/* ============================================================================================
 * dir[0]-VERTEX-MORPH — byte-true Port von FUN_8004BF90 @0x8004bf90
 * (RE2-Leon PSX.EXE, Datei 0x3C790..0x3C994; Dossier
 *  analysis/befunde_runde7_2026-09-13/g5-morph.md, dort instruktionsweise belegt).
 * --------------------------------------------------------------------------------------------
 * Das Original ruft die Funktion genau einmal je Frame (G5-Blob: `jal 0x8004bf90`
 * @0x80103bf8, a0=entity, a1=&part2, a2=*(part2+0x08) = MD1-Objekt,
 * a3=*(part2+0x88) = Gewichts-Controller) und arbeitet in zwei Schritten:
 *
 *   1. BASIS-RESTORE: Segment 0 (Block + data_off) wird verbatim in den Vertexpuffer
 *      zurueckkopiert (@0x8004bfc4-c0a4, 5-fach abgerollt; das pad-Halbwort bleibt
 *      unberuehrt). Ohne diesen Schritt wuerde die Kette unten akkumulieren.
 *   2. SEGMENT-KETTE: fuer i = 1..seg_count wird mit dem Gewicht w[i-1] gegen die
 *      i-te Ziel-Pose interpoliert - GTE `mtc2 (4095-w),IR0` / GPF (0x4b98003d) /
 *      `mtc2 w,IR0` / GPL (0x4ba8003e), also
 *          v = ((4095 - w) * v + w * ziel) >> 12
 *      arithmetisch geschoben und auf s16 gesaettigt (GTE-IR, lm=0). Die Kette rechnet
 *      AUF DEM ERGEBNIS des vorigen Segments - das ist KEIN gewichteter Mittelwert.
 *      `w == 0` ueberspringt das Segment komplett (`beq a0,zero` @0x8004c0d0) - das ist
 *      der Budget-Trick des Originals und bleibt hier stehen.
 *
 * Normalen werden NICHT gemorpht (das Original laesst sie stehen, g5-morph.md 1.3).
 * Die MD1-Basis wird nie beschrieben: sie aliast das residente CDEMD0.EMS.
 * ========================================================================================== */
#include <stdint.h>

#include "re15_md1.h"
#include "re15_enemy.h"

static int16_t morph_sat16(int32_t v)
{
    /* GTE-IR-Saettigung (lm = 0): [-32768, 32767]. */
    if (v >  32767) return  32767;
    if (v < -32768) return -32768;
    return (int16_t)v;
}

void re15_emd_morph_apply(const re15_morph_ctrl_t *c, const int16_t *w,
                          re15_md1_vertex_t *out)
{
    const int16_t *s0;
    int v, i, n;
    if (!c || !c->seg0 || !out || !w) return;
    n  = (int)c->nverts;
    s0 = (const int16_t *)c->seg0;

    for (v = 0; v < n; v++) {                      /* Schritt 1 @0x8004bfc4-c0a4 */
        out[v].x = s0[v * 3 + 0];
        out[v].y = s0[v * 3 + 1];
        out[v].z = s0[v * 3 + 2];
    }

    for (i = 1; i <= (int)c->seg_count; i++) {     /* Schritt 2, t3 = 1..seg_count */
        int32_t wi = (int32_t)w[i - 1];            /* lhu + MTC2 -> 16 Bit */
        int32_t a;
        const int16_t *t;
        if ((uint16_t)wi == 0u) continue;          /* beq a0,zero @0x8004c0d0 */
        a = (int32_t)(int16_t)(4095 - wi);         /* subu t8,a0 + MTC2 @0x8004c0d4 */
        t = (const int16_t *)(c->seg0 + (size_t)i * (size_t)c->seg_size);
        for (v = 0; v < n; v++) {
            out[v].x = morph_sat16((a * (int32_t)out[v].x + wi * (int32_t)t[v*3+0]) >> 12);
            out[v].y = morph_sat16((a * (int32_t)out[v].y + wi * (int32_t)t[v*3+1]) >> 12);
            out[v].z = morph_sat16((a * (int32_t)out[v].z + wi * (int32_t)t[v*3+2]) >> 12);
        }
    }
}
