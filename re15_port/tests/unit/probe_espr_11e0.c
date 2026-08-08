/* probe_espr_11e0.c — ROOM11E0 STROM-/Funken-Effekt (WRENCH-Reparatur-Stelle).
 *
 * SOLL (Datei-/Disasm-Belege, alle selbst gedumpt):
 *   sub27 @RDT 0x175C+0x0980 (Datei 0x20DC):
 *     06 00 20 00              Ifel_ck  skip=32
 *     21 04 24 00              Ck       flag(4,36)==0   (36 = 0x24, VOR der Reparatur)
 *     3A 00 11 00 00 00 00 10  Sce_espr_on id=0x11 sub=0 cat=0(absolut) scale16=0x1000
 *     58 D0 A0 F6 C0 E0 00 00      x=-12200 y=-2400 z=-8000 param=0
 *     09 0A 14 00              Sleep 20 Ticks
 *     17 FF FF 00 E4 FF        Goto -28 (zurueck zum Ifel_ck)
 *   Start: main-sub00 @+0x00E2 `04 FF 18 1B` = Evt_exec(FF,Gosub,sub27) — laeuft ab Raum-Init.
 *   Ende:  sub21 (Reparatur-Event) setzt @+0x086A `22 04 24 01` flag(4,36)=1 → Loop endet.
 *
 *   ESP-Bank ROOM11E0 (RDT dir 0x4C/50/54/58 = 0x41F4/0x4768/0x225F4/0x26ED4):
 *     ids {5,7,0x11}; Effekt 0x11 @0x4674: count_a=9, count_b=7, eigene TIM @0x25A74.
 *     Rowblock @0x46E0, sub0 → 1 Stream, 2 Rows @0x46F8:
 *       row0: A=10 f0e=0x0013 f16=0x0020  → Routine 10 @0x800176b0 (selbst disassembliert):
 *             flags:=row[0x0e]=0x13 (Bit4=ABE), TPAGE|=row[0x16]=0x20 (ABR1=ADDITIV),
 *             CLUT+=row[0x1e]<<6, anim_idx:=row[0x26], Row-Advance (jal 0x800174e4).
 *       row1: A=0 (Noop-Halt).
 *     Anim-Records: dur 1,1,1,1,6,1,1,1 → 13 Ticks Lebensdauer, Record 8 = 0/0-Terminator.
 *   Spawner: FUN_80041864 packt a0 = id<<24|sub<<16|scale16(pc[6..7]) (@0x80041934-4c) und
 *   ruft FUN_80019700 — den ROW-Spawner (es gibt KEINEN row-losen Pfad im Original).
 *
 * MESSUNG:
 *   Phase 1 (vor Reparatur): Spawn-Kadenz (~alle 21 Ticks), fx-Felder (id/sub/pos/scale/rows/
 *            flags/tpage nach Routine 10).
 *   Phase 2 (flag(4,36)=1 wie sub21): keine neuen Spawns, Pool laeuft leer.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_esp.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern scd_vm_t g_scd;

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

static uint32_t rd32(const uint8_t *p, size_t o)
{
    return (uint32_t)p[o] | ((uint32_t)p[o+1] << 8) | ((uint32_t)p[o+2] << 16) | ((uint32_t)p[o+3] << 24);
}

static const re15_esp_fx_t *first_fx(void)
{
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (f) return f;
    }
    return NULL;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM11E0.RDT", base);
    uint8_t *dat = read_file(path, &sz);
    if (!dat) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(dat, sz, &rdt) != 0) { fprintf(stderr, "FAIL: RDT parse\n"); return 1; }

    /* ESP-Bank wie pc_load_room_esp (RDT dir 0x4C/0x50/0x54/0x58, byte-true FUN_80019354). */
    static re15_esp_t esp;
    int rc = re15_esp_parse(dat, sz, rd32(dat, 0x4C), rd32(dat, 0x50),
                            rd32(dat, 0x54), rd32(dat, 0x58), &esp);
    if (rc != 0) { fprintf(stderr, "FAIL: ESP parse rc=%d\n", rc); return 1; }
    re15_esp_set_room_bank(&esp);
    re15_esp_fx_reset();
    printf("ESP-Bank: %d ids:", esp.id_count);
    for (int i = 0; i < esp.id_count; i++)
        printf(" 0x%02X(tim=0x%X)", esp.eff[i].effect_id, esp.eff[i].tim_off);
    printf("\n");
    {   /* SOLL-Pin: Effekt 0x11 vorhanden, eigene TIM @0x25A74, Rows sub0 = 1 Stream/2 Rows. */
        int ei = re15_esp_find_id(&esp, 0x11);
        if (ei < 0) { fprintf(stderr, "FAIL: Effekt 0x11 nicht in der Bank\n"); return 1; }
        if (esp.eff[ei].tim_off != 0x25A74) {
            fprintf(stderr, "FAIL: Effekt-0x11-TIM 0x%X != 0x25A74\n", esp.eff[ei].tim_off); return 1; }
        int nrows = 0;
        const uint8_t *rows = re15_esp_row_stream(&esp, ei, 0, 0, &nrows);
        int streams = re15_esp_row_streams(&esp, ei, 0);
        printf("Effekt 0x11: eff_idx=%d streams=%d nrows=%d rowA=%u f0e=0x%02x f16=0x%02x\n",
               ei, streams, nrows,
               rows ? (unsigned)(rows[0] | (rows[1] << 8)) : 0,
               rows ? rows[0x0e] : 0, rows ? rows[0x16] : 0);
        if (streams != 1 || nrows != 2 || !rows ||
            (rows[0] | (rows[1] << 8)) != 10 || rows[0x0e] != 0x13 || rows[0x16] != 0x20) {
            fprintf(stderr, "FAIL: Row-Daten weichen vom Datei-SOLL ab (@0x46F8)\n"); return 1; }
    }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x11E0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 0; pl->y = 0; pl->z = 9000; pl->rot_y = 0;

    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    printf("nach Re-Init: flag(4,36)=%d\n", re15_game_flag_get(4, 36));

    /* ===== Phase 1: 64 Ticks vor der Reparatur ===== */
    int spawn_ticks = 0, first_spawn = -1, max_live = 0;
    re15_esp_fx_t snap; memset(&snap, 0, sizeof snap);
    int have_snap = 0, saw_flags13 = 0, saw_abr1 = 0;
    int prev = 0;
    for (int t = 0; t < 64; t++) {
        scd_vm_tick();
        re15_esp_fx_tick(re15_esp_room_bank());
        int n = re15_esp_fx_count();
        if (n > prev) {
            spawn_ticks++;
            if (first_spawn < 0) first_spawn = t;
            const re15_esp_fx_t *f = first_fx();
            if (f && !have_snap) { snap = *f; have_snap = 1; }
        }
        if (n > max_live) max_live = n;
        const re15_esp_fx_t *f = first_fx();
        if (f && f->flags == 0x13) saw_flags13 = 1;
        if (f && ((f->tpage >> 5) & 3) == 1) saw_abr1 = 1;
        if (t < 4 || n != prev)
            printf("  t=%2d fx=%d%s\n", t, n,
                   f ? (f->rows_base ? " [row-VM]" : " [LEGACY]") : "");
        prev = n;
    }
    printf("Phase 1: erster Spawn t=%d, Spawn-Fenster=%d, max gleichzeitig=%d\n",
           first_spawn, spawn_ticks, max_live);
    if (have_snap)
        printf("  fx: id=0x%02X sub=%u eidx=%d pos=(%ld,%ld,%ld) scale16=0x%04X rows=%s "
               "flags=0x%02X tpage=0x%04X clut=0x%04X\n",
               snap.effect_id, snap.sub_index, snap.eff_idx,
               (long)snap.x, (long)snap.y, (long)snap.z, snap.scale16,
               snap.rows_base ? "JA" : "NEIN(legacy)", snap.flags, snap.tpage, snap.clut);

    int fail = 0;
    /* SOLL-Pins Phase 1 (Belege im Kopf-Kommentar): */
    if (!have_snap)                        { printf("FAIL: kein Effekt-Spawn (sub27 tot?)\n"); fail = 1; }
    if (have_snap && snap.effect_id != 0x11) { printf("FAIL: effect_id 0x%02X != 0x11\n", snap.effect_id); fail = 1; }
    if (have_snap && (snap.x != -12200 || snap.y != -2400 || snap.z != -8000)) {
        printf("FAIL: pos (%ld,%ld,%ld) != (-12200,-2400,-8000) [Datei 0x20E4, cat=0 absolut]\n",
               (long)snap.x, (long)snap.y, (long)snap.z); fail = 1; }
    if (have_snap && snap.scale16 != 0x1000) {
        printf("FAIL: scale16 0x%04X != 0x1000 [pc[6..7] @0x8004194c]\n", snap.scale16); fail = 1; }
    if (have_snap && !snap.rows_base) {
        printf("FAIL: LEGACY-Spawn — FUN_80041864 ruft den ROW-Spawner FUN_80019700\n"); fail = 1; }
    if (!saw_flags13) { printf("FAIL: flags nie 0x13 [Routine 10 @0x800176c0-c8: flags:=row[0x0e]]\n"); fail = 1; }
    if (!saw_abr1)    { printf("FAIL: TPAGE-ABR nie 1 [Routine 10 @0x80017608-20: tpage|=row[0x16]=0x20]\n"); fail = 1; }
    if (spawn_ticks < 2) { printf("FAIL: keine Respawn-Kadenz (Sleep-20-Loop @0x20F4)\n"); fail = 1; }

    /* ===== Phase 2: Reparatur — sub21-Anfang setzt flag(4,36)=1 (@+0x086A `22 04 24 01`) ===== */
    re15_game_flag_set(4, 36, 1);
    int live_after = -1;
    for (int t = 0; t < 64; t++) {
        scd_vm_tick();
        re15_esp_fx_tick(re15_esp_room_bank());
    }
    live_after = re15_esp_fx_count();
    printf("Phase 2 (flag(4,36)=1, 64 Ticks): fx=%d (SOLL 0 — Ifel_ck @0x0980 faellt durch)\n",
           live_after);
    if (live_after != 0) { printf("FAIL: Effekt lebt nach der Reparatur weiter\n"); fail = 1; }

    printf(fail ? "PROBE: FAIL\n" : "PROBE: PASS\n");
    free(dat);
    return fail;
}
