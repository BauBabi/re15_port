/* probe_11f0_cut_after_puzzle.c — MESSSONDE zum Nutzer-Befund (D)
 * "Wenn ich nach dem Generator-Raetsel direkt weglaufe, wechselt die Kamera / das Bild nicht."
 *
 * Der Generator liegt in ROOM11F0 (probe_gen_11f0). Das Raetsel-Panel schaltet die Kamera
 * hart um und der Ausstieg gibt sie wieder frei:
 *   ROOM11F0.RDT @0x15C0  `29 0a`      = Cut_chg(0x0A)
 *   ROOM11F0.RDT @0x16F0  `2a 00 3c 01`= Cut_old ; (Nop) ; Cut_auto(1)
 * SOLLSEITE (PSX.EXE):
 *   Cut_chg  LAB_800402a0: @0x800402c0 a1 = work_vars[0x0A] (der ALTE Cut, lbu)
 *                          @0x800402d4 `ori DAT_800aca3c,0x100`  = Auto-Cam AUS
 *                          @0x800402e4 `sb a1,DAT_800b3f7b`      = alten Cut merken
 *                          @0x800402f4 `sb 1,DAT_800b5457`       = Kamera-Dirty
 *                          @0x800402ec/fc work_vars[0x0C]/[0x0A]
 *                          @0x80040300 `jal FUN_800142f4`        = Cut sofort anwenden
 *   Cut_old  LAB_8004032c: @0x8004033c a0 = DAT_800b3f7b, @0x80040354 Dirty=1,
 *                          @0x8004035c/64 work_vars[0x0C]/[0x0A] = alter Cut,
 *                          @0x80040368 `jal FUN_800142f4`,
 *                          @0x80040378-84 `and DAT_800aca3c,~0x100` = Auto-Cam WIEDER AN
 *   Cut_auto LAB_800403ac (Flag 1 -> Bit 0x100 loeschen = AN)
 * Danach MUSS der RVD-Zonen-Scan (FUN_80014230) wieder umschalten, sobald der Spieler eine
 * Zone der aktiven Kamera betritt.
 *
 * Diese Sonde misst engine-seitig:
 *   M1 POSITIV-KONTROLLE  — schaltet der Scan im unberuehrten Raum ueberhaupt um?
 *   M2 Raetsel-Pfad       — sub mit Cut_chg(0x0A) laufen lassen, dann sub mit Cut_old+Cut_auto,
 *                           und DANACH exakt dieselbe Strecke nochmal laufen.
 * Wechselt M1 die Kamera und M2 nicht, ist der Fehler reproduziert und liegt in der Engine.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_collision.h"

extern scd_vm_t g_scd;
extern int scd_thread_start(int slot, const uint8_t *pc);

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { printf("FAIL: "); printf(__VA_ARGS__); \
                              printf("\n"); g_fail = 1; } } while (0)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

/* Ein Bild ohne Spieler-FSM: nur SCD-VM + AOT-Scan — genau die beiden Stufen, die
 * Kamera-Zonen und Cut-Opcodes fahren (main.c: scd_vm_tick vor re15_game_step). */
static void tick_frame(re15_actor_t *pl)
{
    scd_vm_tick();
    re15_aot_scan(pl->x, pl->z, (uint8_t)g_scd.cam_id);
}

/* NACHBAR-MESSUNG: ROOM10F0 (der Raum, den der Nutzer benannt hat). Reine Zonen-Ketten-
 * Kontrolle — schaltet der RVD-Scan dort ueberhaupt um? */
static void probe_room(const char *base, const char *name, unsigned room_id,
                       int32_t sx, int32_t sz)
{
    char rp[600]; snprintf(rp, sizeof rp, "%s/STAGE1/%s.RDT", base, name);
    size_t sz2 = 0; uint8_t *raw = read_file(rp, &sz2);
    if (!raw) { printf("[N] SKIP %s\n", name); return; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(raw, sz2, &rdt) < 0) { printf("[N] %s: RDT-Parse fehlgeschlagen\n", name); free(raw); return; }
    re15_actor_init(); scd_vm_init(); re15_aot_init();
    g_current_room_id = room_id; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = sx; pl->y = 0; pl->z = sz; pl->rot_y = 0;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 30; f++) tick_frame(pl);
    int n = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++)
        if (g_aot.slots[i].active && g_aot.slots[i].type == RE15_AOT_TYPE_CAM_SWITCH) n++;
    int seen[64], nseen = 0, sw = 0; seen[nseen++] = (int)g_scd.cam_id;
    for (int step = 0; step < 12; step++) {
        int pick = -1;
        for (int i = RE15_AOT_MAX - 1; i >= 0; i--) {
            const re15_aot_t *a = &g_aot.slots[i];
            if (!a->active || a->type != RE15_AOT_TYPE_CAM_SWITCH) continue;
            if (a->cam_from_filter != g_scd.cam_id) continue;
            int fresh = 1;
            for (int s = 0; s < nseen; s++) if (seen[s] == (int)a->event_id) fresh = 0;
            if (fresh) { pick = i; break; }
        }
        if (pick < 0) break;
        unsigned before = g_scd.cam_id;
        pl->x = g_aot.slots[pick].x; pl->z = g_aot.slots[pick].z;
        for (int f = 0; f < 3; f++) tick_frame(pl);
        if (g_scd.cam_id != before) { sw++; if (nseen < 64) seen[nseen++] = (int)g_scd.cam_id; }
        else break;
    }
    printf("[N] %s: %d RVD-Zonen, Kette %d Umschaltungen / %d Cuts, auto=%d\n",
           name, n, sw, nseen, (int)g_scd.cut_auto_enabled);
    free(raw);
}

int main(void)
{
    const char *base = RE15_ASSET_PSX_DIR;
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM11F0.RDT", base);
    size_t sz = 0;
    uint8_t *raw = read_file(rp, &sz);
    if (!raw) { printf("SKIP: %s fehlt\n", rp); return 77; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(raw, sz, &rdt) < 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    /* --- M0: welcher sub traegt Cut_chg(0x0A) (@0x15C0) bzw. Cut_old+Cut_auto (@0x16F0)? --- */
    int sub_chg = -1, sub_old = -1;
    for (int i = 0; i < RE15_RDT_MAX_SUB_SCD; i++) {
        if (!rdt.sub_scd[i]) continue;
        size_t off = (size_t)(rdt.sub_scd[i] - raw);
        if (off <= 0x15C0 && (sub_chg < 0 || off > (size_t)(rdt.sub_scd[sub_chg] - raw))) sub_chg = i;
        if (off <= 0x16F0 && (sub_old < 0 || off > (size_t)(rdt.sub_scd[sub_old] - raw))) sub_old = i;
    }
    printf("[M0] Cut_chg(0x0A) @0x15C0 liegt in sub%02d (Start 0x%04X)\n",
           sub_chg, sub_chg >= 0 ? (unsigned)(rdt.sub_scd[sub_chg] - raw) : 0u);
    printf("[M0] Cut_old+Cut_auto @0x16F0 liegt in sub%02d (Start 0x%04X)\n",
           sub_old, sub_old >= 0 ? (unsigned)(rdt.sub_scd[sub_old] - raw) : 0u);
    CHECK(sub_chg >= 0 && sub_old >= 0, "die beiden subs sind nicht auffindbar");
    if (sub_chg < 0 || sub_old < 0) return 1;

    re15_actor_init();
    scd_vm_init();
    re15_aot_init();
    g_current_room_id = 0x11F0;
    g_room_change.pending = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -19554; pl->y = 0; pl->z = 22684; pl->rot_y = 0;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 30; f++) tick_frame(pl);

    /* --- M0b: die RVD-Zonen des Raums (aus der geladenen AOT-Tabelle) --- */
    int n_cam = 0;
    printf("[M0b] RVD-Zonen (slot: cam_from -> cam_to, Rechteck):\n");
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active || a->type != RE15_AOT_TYPE_CAM_SWITCH) continue;
        n_cam++;
        if (n_cam <= 24)
            printf("      %2d: %3u -> %-3u  c=(%6ld,%6ld) h=(%5d,%5d) quad=%d\n",
                   i, (unsigned)a->cam_from_filter, (unsigned)a->event_id,
                   (long)a->x, (long)a->z, a->half_w, a->half_h, a->has_quad);
    }
    printf("[M0b] %d RVD-Zonen, cut_auto_enabled=%d, cam_id=%u, work_vars[0x0A]=%d\n",
           n_cam, (int)g_scd.cut_auto_enabled, (unsigned)g_scd.cam_id,
           (int)g_scd.work_vars[0x0A]);
    CHECK(n_cam > 0, "ROOM11F0 hat gar keine RVD-Kamera-Zonen — Messung sinnlos");
    if (n_cam == 0) return 1;

    /* --- Die Strecke: die ZONEN-KETTE. Vom aktuellen Cut aus die erste Zone mit
     * cam_from == cam_id suchen, in ihren Mittelpunkt setzen, ticken — der Scan MUSS
     * dann auf deren cam_to schalten. Das ist die Zustandsmaschine, die der Nutzer
     * beim Weglaufen durchlaeuft, und zugleich die Positiv-Kontrolle. */
    #define WALK(tag, out_switches, out_cuts) do {                                    \
        int seen[64]; int nseen = 0; int sw = 0;                                       \
        seen[nseen++] = (int)g_scd.cam_id;                                             \
        for (int step = 0; step < 12; step++) {                                        \
            int pick = -1;                                                             \
            for (int i = RE15_AOT_MAX - 1; i >= 0; i--) {                              \
                const re15_aot_t *a = &g_aot.slots[i];                                 \
                if (!a->active || a->type != RE15_AOT_TYPE_CAM_SWITCH) continue;       \
                if (a->cam_from_filter != g_scd.cam_id) continue;                      \
                int fresh = 1;                                                         \
                for (int s = 0; s < nseen; s++) if (seen[s] == (int)a->event_id) fresh = 0; \
                if (fresh) { pick = i; break; }                                         \
            }                                                                           \
            if (pick < 0) break;                                                        \
            unsigned before = g_scd.cam_id;                                             \
            pl->x = g_aot.slots[pick].x; pl->z = g_aot.slots[pick].z;                    \
            for (int f = 0; f < 3; f++) tick_frame(pl);                                 \
            if (g_scd.cam_id != before) { sw++;                                          \
                if (nseen < 64) seen[nseen++] = (int)g_scd.cam_id; }                     \
            else break;                                                                  \
        }                                                                                \
        printf("[%s] %d Umschaltungen, %d Cuts der Kette, Endstand cam=%u "              \
               "auto=%d wv0A=%d\n", tag, sw, nseen, (unsigned)g_scd.cam_id,              \
               (int)g_scd.cut_auto_enabled, (int)g_scd.work_vars[0x0A]);                 \
        (out_switches) = sw; (out_cuts) = nseen;                                         \
    } while (0)

    int sw1 = 0, nc1 = 0;
    WALK("M1 POSITIV-KONTROLLE", sw1, nc1);
    CHECK(sw1 > 0, "schon OHNE Raetsel schaltet der RVD-Scan nie um — anderer Defekt");

    /* Zurueck auf den Ausgangs-Cut, damit M2 dieselbe Kette laeuft wie M1. */
    pl->x = -19554; pl->z = 22684;
    for (int f = 0; f < 3; f++) tick_frame(pl);

    /* --- M2: der Raetsel-Pfad. Der VM werden die ECHTEN Raum-Bytes vorgesetzt, aber ab
     * dem Cut-Opcode: sub16 beginnt mit Message_on + Warten auf die Spieler-Bestaetigung
     * (@0x15A2 `2b 00 ff ff`), das ist Dialog-Praeambel und nicht Teil des Kamera-
     * Mechanismus. Gestartet wird bei @0x15C0 (`29 0a` = Cut_chg(0x0A)) bzw. @0x16F0
     * (`2a 00 3c 01` = Cut_old ; Cut_auto(1)) — beide Opcodes laufen durch die echte VM,
     * nichts wird von Hand nachgebildet. --- */
    CHECK(raw[0x15C0] == 0x29 && raw[0x15C1] == 0x0A,
          "ROOM11F0 @0x15C0 ist nicht Cut_chg(0x0A) (%02x %02x)", raw[0x15C0], raw[0x15C1]);
    CHECK(raw[0x16F0] == 0x2A && raw[0x16F2] == 0x3C && raw[0x16F3] == 0x01,
          "ROOM11F0 @0x16F0 ist nicht Cut_old+Cut_auto(1) (%02x %02x %02x %02x)",
          raw[0x16F0], raw[0x16F1], raw[0x16F2], raw[0x16F3]);
    scd_thread_start(3, raw + 0x15C0);
    for (int f = 0; f < 60; f++) tick_frame(pl);
    printf("[M2a] nach Cut_chg @0x15C0: cam=%u auto=%d wv0A=%d wv0C=%d\n",
           (unsigned)g_scd.cam_id, (int)g_scd.cut_auto_enabled,
           (int)g_scd.work_vars[0x0A], (int)g_scd.work_vars[0x0C]);
    CHECK(g_scd.cam_id == 0x0A, "Cut_chg(0x0A) hat cam_id nicht auf 10 gesetzt (ist %u)",
          (unsigned)g_scd.cam_id);
    CHECK(g_scd.cut_auto_enabled == 0,
          "Cut_chg hat den Auto-Scan nicht abgeschaltet (@0x800402d4 setzt Bit 0x100)");

    scd_thread_start(4, raw + 0x16F0);
    for (int f = 0; f < 60; f++) tick_frame(pl);
    printf("[M2b] nach Cut_old+Cut_auto @0x16F0: cam=%u auto=%d wv0A=%d wv0C=%d\n",
           (unsigned)g_scd.cam_id, (int)g_scd.cut_auto_enabled,
           (int)g_scd.work_vars[0x0A], (int)g_scd.work_vars[0x0C]);
    CHECK(g_scd.cut_auto_enabled == 1,
          "Cut_auto(1)/Cut_old hat den Auto-Scan NICHT wieder scharf geschaltet "
          "(@0x80040378-84 loescht Bit 0x100) — genau der Nutzer-Befund");

    int sw2 = 0, nc2 = 0;
    WALK("M2 NACH DEM RAETSEL", sw2, nc2);
    CHECK(sw2 > 0, "nach dem Raetsel schaltet der RVD-Scan NIE mehr um (Nutzer-Befund D)");
    CHECK(nc2 == nc1, "nach dem Raetsel erreicht der Scan nur %d statt %d verschiedene Cuts",
          nc2, nc1);

    free(raw);
    probe_room(base, "ROOM10F0", 0x10F0, 0, 0);
    printf(g_fail ? "SONDE 11F0-cut-after-puzzle: ABWEICHUNG\n"
                  : "SONDE 11F0-cut-after-puzzle: OK\n");
    return g_fail;
}
