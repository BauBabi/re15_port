/* probe_gen_11f0.c — MESSSONDE ROOM11F0 "Generator-Raum: 10 Schalter-Rätsel".
 *
 * Nutzer-Report: nur 4 Schalter sichtbar (links oben), 6 fehlen (links unten + 5 rechts),
 * Rätsel funktioniert nicht.
 *
 * SOLL (statisch, ROOM11F0.RDT, main=RDT+0x40→0x0CC4, sub=RDT+0x44→0x0D34, byte-gedumpt):
 *   main00 @0x0CC6:  Door_aot_set slot0, Aot_set slot13, Item_aot_set slot14,
 *                    Obj_model_set id=0x01 @0x0D10 pos=(11040,-16200,-15084) rot_y=1656.
 *   sub00  @0x0D5C:  If Ck(4,0xEE)==0:
 *     Aot_set slot1 sce=3 @0x0D64 (Panel, eventId pc[17]=0x10 → sub16)
 *     Aot_set slot2..12 sce=5 flags=0x44 (CENTRE|Objekt-Pool) @0x0D78..0x0E40+0x14
 *     Obj_model_set id=0x00 @0x0E54 (MASCHINE, type=4, pos=(-19554,0,22684), box h=900³)
 *     Obj_model_set id=0x02..0x0B @0x0E76..0x0FA8 (+0x22) = DIE 10 SCHALTER:
 *       links  x=-25975: z=26000/24000/22000/19800/17800 (id 2..6)
 *       rechts x=-18775: z=26000/24000/22000/19800/17800 (id 7..0x0B), alle rot_z=512
 *     4x Sce_em_set 0x87/0x88 @0x0FCA.. (Funken-Effekte)
 *   sub01 (per-frame): Maschine fahren via Sce_key_ck→sub02..05; pro Schalter N:
 *     Ck(5,1+N) && Member_cmp(Maschine member15==2+N) && key 0x40 → Evt_exec sub(6+N)
 *     (N=9 → sub15; member15==12 → sub17=Exit; Win-Pattern Ck(5,0x0D..0x16)=1,0,1,0,…
 *      → Evt_exec sub18 + Set(4,0xEE) @0x012E6..0x012EA).
 *   sub06..15 (Toggle Schalter N): Work_set(3, 2+N); Speed_set(5,±0x40); For 16 {Add_speed;
 *     Evt_next} → rot_z 512↔1536; Latch Set(5,0x0D+N) (z.B. sub06 @0x01322..0x01360).
 *
 * ORIGINAL-Beleg (PSX.EXE): Obj_model_set-Handler LAB_80040914 indexiert den Objekt-Pool
 * DIREKT per obj_id (`a3=pc[1]; a3*0x94 + DAT_800b3f98` @0x8004093c-58) und bindet das
 * Modell pro obj_id aus der RDT+0x30-Tabelle (@0x80040ab4-adc `lw v1,0x30(v0); sll v0,a3,3;
 * lw a1,4(v0)` → FUN_8002b898). Die Objekt-Schleife FUN_800436a8 läuft bis
 * `lbu v0,2(v0)` = RDT-Header nOmodel (@0x80043758/0x800437a4, `sltu s0 < nOmodel`
 * @0x800437ac) — ROOM11F0: nOmodel=12. KEIN 6er-Cap im Original.
 *
 * IST-Fehl-Ursache (PC-Port): platform/pc/main.c pc_load_room_prop_set kappte bei 6
 * (`nprops = prop_count < 6 ? … : 6`) + Draw-Gate `oid < 6` → Props obj_id 0x06..0x0B
 * (unterster linker + 5 rechte Schalter) ohne Modell/Textur = unsichtbar.
 *
 * Diese Sonde misst die ENGINE-Seite (Spawn/AOT/Notch/Toggle/Win) und dass die RDT
 * alle 12 Prop-Modelle liefert (>6 ⇒ jedes Platform-Cap <12 verliert Schalter). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_md1.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern scd_vm_t g_scd;
extern re15_aot_state_t g_aot;

static int g_fail = 0;
#define CHECK(cond, ...) do { \
    if (!(cond)) { g_fail++; printf("  FAIL: " __VA_ARGS__); printf("\n"); } \
} while (0)

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

/* SOLL-Tabelle der 12 Props — Datei-Offsets der Obj_model_set-Records in ROOM11F0.RDT.
 * Spawn-Reihenfolge: main00 zuerst (id 0x01), dann sub00 (id 0x00, 0x02..0x0B). */
typedef struct { uint8_t id; int32_t x, y, z; int16_t rz; const char *src; } soll_prop_t;
static const soll_prop_t SOLL[12] = {
    { 0x01,  11040, -16200, -15084,   0, "main00 @0x0D10" },
    { 0x00, -19554,      0,  22684,   0, "sub00  @0x0E54 (Maschine)" },
    { 0x02, -25975,     10,  26000, 512, "sub00  @0x0E76 (Schalter L1)" },
    { 0x03, -25975,     10,  24000, 512, "sub00  @0x0E98 (Schalter L2)" },
    { 0x04, -25975,     10,  22000, 512, "sub00  @0x0EBA (Schalter L3)" },
    { 0x05, -25975,     10,  19800, 512, "sub00  @0x0EDC (Schalter L4)" },
    { 0x06, -25975,     10,  17800, 512, "sub00  @0x0EFE (Schalter L5, 'links unten')" },
    { 0x07, -18775,     10,  26000, 512, "sub00  @0x0F20 (Schalter R1)" },
    { 0x08, -18775,     10,  24000, 512, "sub00  @0x0F42 (Schalter R2)" },
    { 0x09, -18775,     10,  22000, 512, "sub00  @0x0F64 (Schalter R3)" },
    { 0x0A, -18775,     10,  19800, 512, "sub00  @0x0F86 (Schalter R4)" },
    { 0x0B, -18775,     10,  17800, 512, "sub00  @0x0FA8 (Schalter R5)" },
};

/* Index im Prop-Pool per obj_id (Original: Pool DIREKT per id indexiert,
 * LAB_80040914 @0x8004093c-58; der Port appended sequenziell + sucht per obj_id). */
static int find_prop(uint8_t obj_id)
{
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].active && g_scd.props[i].obj_id == obj_id)
            return i;
    return -1;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM11F0.RDT", base);
    uint8_t *dat = read_file(path, &sz);
    if (!dat) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(dat, sz, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    /* ===== M1: RDT liefert ALLE 12 Prop-Modelle (nOmodel @RDT+0x02 = 12) ===== */
    printf("===== M1: RDT-Modell-Tabelle (RDT+0x30, nOmodel=%u) =====\n", rdt.nOmodel);
    CHECK(rdt.nOmodel == 12, "nOmodel=%u != 12", rdt.nOmodel);
    CHECK(rdt.prop_count == 12, "rdt.prop_count=%d != 12", rdt.prop_count);
    int md1_ok = 0;
    for (int k = 0; k < rdt.prop_count; k++) {
        re15_md1_t m;
        int ok = (rdt.prop_md1[k] &&
                  re15_md1_parse(rdt.prop_md1[k], (size_t)rdt.prop_md1_size[k], &m) == 0);
        if (ok) md1_ok++;
        printf("   prop-model[%2d]: md1=%c(%6d B, %d meshes) tim=%c(%6d B)\n", k,
               ok ? '+' : '-', rdt.prop_md1_size[k], ok ? m.mesh_count : -1,
               rdt.prop_tim[k] ? '+' : '-', rdt.prop_tim_size[k]);
    }
    CHECK(md1_ok == 12, "nur %d/12 MD1 parsen", md1_ok);
    printf("   >>> 12 Modelle > 6: jedes Platform-Prop-Cap < 12 verliert die Schalter 0x06..0x0B\n");

    /* ===== M2: Raum-Init — alle 12 Props gespawnt, Positionen == RDT-Bytes ===== */
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x11F0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    /* Spieler ins Raum-Zentrum abseits aller AOTs (Panel-AOT slot1 liegt bei
     * x[-2200..-1400] z[-15850..-14450], Datei @0x0D64). */
    pl->x = -8000; pl->y = 0; pl->z = 0; pl->rot_y = 0;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);

    printf("\n===== M2: g_scd.props nach main00+sub00 (SOLL: 12, flag(4,0xEE)=0-Zweig) =====\n");
    printf("   prop_count=%u  flag(4,0xEE)=%d\n", g_scd.prop_count, re15_game_flag_get(4, 0xEE));
    CHECK(g_scd.prop_count == 12, "prop_count=%u != 12", g_scd.prop_count);
    for (int i = 0; i < 12; i++) {
        const soll_prop_t *s = &SOLL[i];
        int pi = find_prop(s->id);
        if (pi < 0) { CHECK(0, "Prop id=0x%02X (%s) NICHT gespawnt", s->id, s->src); continue; }
        int ok = (g_scd.props[pi].x == s->x && g_scd.props[pi].y == s->y &&
                  g_scd.props[pi].z == s->z && g_scd.props[pi].rot_z == s->rz);
        printf("   id=0x%02X IST=(%6ld,%6ld,%6ld) rz=%4d | SOLL=(%6ld,%6ld,%6ld) rz=%4d %s [%s]\n",
               s->id, (long)g_scd.props[pi].x, (long)g_scd.props[pi].y,
               (long)g_scd.props[pi].z, (int)g_scd.props[pi].rot_z,
               (long)s->x, (long)s->y, (long)s->z, (int)s->rz,
               ok ? "OK" : "**DIVERGENZ**", s->src);
        CHECK(ok, "Prop id=0x%02X Position/Rotation != RDT-Bytes", s->id);
    }

    /* ===== M3: AOT-Slots (Panel slot1 sce3, Schalter-Zonen 2..12 sce5/0x44) ===== */
    printf("\n===== M3: AOT-Slots =====\n");
    for (int s2 = 0; s2 < RE15_AOT_MAX; s2++) {
        const re15_aot_t *a = &g_aot.slots[s2];
        if (!a->active) continue;
        if (a->type == RE15_AOT_TYPE_CAM_SWITCH) continue;
        printf("   aot=%2d type=%u ev=%u band=0x%02x sceflg=0x%02x c=(%6ld,%6ld) half=(%5ld,%5ld)\n",
               s2, a->type, a->event_id, a->band, a->sce_flags,
               (long)a->x, (long)a->z, (long)a->half_w, (long)a->half_h);
    }
    for (int s2 = 2; s2 <= 12; s2++)
        CHECK(g_aot.slots[s2].active, "Schalter-AOT slot %d fehlt", s2);

    /* ===== M4: Objekt-Notch — Maschine (id 0) in jede Schalter-Zone parken =====
     * Original: FUN_800436a8 Objekt-Pass cleart obj+0x0B (@0x80043788) und stempelt per
     * FUN_80042bac(pool=4, AUTO) den AOT-Index; sub01 vergleicht member15==2+N. */
    printf("\n===== M4: Maschinen-Notch (member_0b) pro Schalter-Zone =====\n");
    int mi = find_prop(0x00);
    CHECK(mi >= 0, "Maschine (id 0) fehlt");
    if (mi >= 0) {
        /* Home-Position (-19554,22684): liegt LT. DATEI-BYTES im Zone-9-Rect —
         * Aot_set slot 9 @0x0E04: x=0xB30C=-19700, z=0x5302=21250, w=d=0x0802=2050
         * -> x[-19700..-17650] z[21250..23300]; |dx|=879, |dz|=409 <= 1025.
         * Das Original stempelt dort ebenfalls 9 (Rect-Test FUN_80042bac, Pool-Pass
         * FUN_800436a8) — Home-Notch SOLL = 9, NICHT 0. */
        re15_object_notch_update();
        printf("   Maschine@Home (-19554,22684): notch=%u (SOLL 9, Zone-9-Rect @0x0E04)\n",
               g_scd.props[mi].member_0b);
        CHECK(g_scd.props[mi].member_0b == 9, "Home-Notch=%u != 9", g_scd.props[mi].member_0b);
        for (int s3 = 2; s3 <= 12; s3++) {
            const re15_aot_t *a = &g_aot.slots[s3];
            g_scd.props[mi].x = a->x; g_scd.props[mi].z = a->z;
            re15_object_notch_update();
            printf("   Maschine@Zone %2d (%6ld,%6ld): notch=%u (SOLL %d)\n",
                   s3, (long)a->x, (long)a->z, g_scd.props[mi].member_0b, s3);
            CHECK(g_scd.props[mi].member_0b == s3, "Zone %d: notch=%u", s3,
                  g_scd.props[mi].member_0b);
        }
        g_scd.props[mi].x = -19554; g_scd.props[mi].z = 22684;   /* Home @0x0E54 */
        re15_object_notch_update();
    }

    /* ===== M5: Schalter-Toggle sub06 (Datei @0x01322): rot_z 512->1536, Latch 5/0x0D =====
     * SOLL-Bytes: Set(5,1,0); Work_set(3,2); If Ck(5,0x0D)==0: Speed_set(5,+0x40);
     * For 16 {Add_speed; Evt_next}; Set(5,0x0D,1) Else: Speed_set(5,-0x40); ... Set(5,0x0D,0). */
    printf("\n===== M5: Toggle-Sub06 auf Schalter id=0x02 =====\n");
    int si = find_prop(0x02);
    if (si >= 0) {
        printf("   vor  Toggle: rot_z=%d flag(5,0x0D)=%d\n", (int)g_scd.props[si].rot_z,
               re15_game_flag_get(5, 0x0D));
        int th = scd_event_fire(6);
        CHECK(th >= 0, "scd_event_fire(6) fand keinen freien Thread");
        for (int t = 0; t < 40; t++) scd_vm_tick();
        printf("   nach Toggle: rot_z=%d flag(5,0x0D)=%d (SOLL 1536 / 1)\n",
               (int)g_scd.props[si].rot_z, re15_game_flag_get(5, 0x0D));
        CHECK(g_scd.props[si].rot_z == 1536, "rot_z=%d != 1536 (512 + 16*0x40)",
              (int)g_scd.props[si].rot_z);
        CHECK(re15_game_flag_get(5, 0x0D) == 1, "Latch flag(5,0x0D) nicht gesetzt");
        /* Rueck-Toggle (Else-Zweig @0x01348): -0x40 x16 -> 512, Latch clear. */
        th = scd_event_fire(6);
        CHECK(th >= 0, "2. scd_event_fire(6) fand keinen freien Thread");
        for (int t = 0; t < 40; t++) scd_vm_tick();
        printf("   Rueck-Toggle: rot_z=%d flag(5,0x0D)=%d (SOLL 512 / 0)\n",
               (int)g_scd.props[si].rot_z, re15_game_flag_get(5, 0x0D));
        CHECK(g_scd.props[si].rot_z == 512, "Rueck-Toggle rot_z=%d != 512",
              (int)g_scd.props[si].rot_z);
        CHECK(re15_game_flag_get(5, 0x0D) == 0, "Latch flag(5,0x0D) nicht geloescht");
    } else CHECK(0, "Schalter id=0x02 fehlt");

    /* ===== M6: Win-Erkennung sub01 @0x012B6: Pattern 1,0,1,0,1,0,1,0,1,0 =====
     * -> Evt_exec sub18 (@0x012E6) + Set(4,0xEE) (@0x012EA); sub18 setzt sofort
     * Set(4,0xF3) (@0x016F6). */
    printf("\n===== M6: Win-Pattern -> sub18 =====\n");
    {
        static const uint8_t pat[10] = { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };
        for (int n = 0; n < 10; n++) re15_game_flag_set(5, (uint8_t)(0x0D + n), pat[n]);
        for (int t = 0; t < 8; t++) scd_vm_tick();
        printf("   flag(4,0xEE)=%d flag(4,0xF3)=%d (SOLL 1/1)\n",
               re15_game_flag_get(4, 0xEE), re15_game_flag_get(4, 0xF3));
        CHECK(re15_game_flag_get(4, 0xEE) == 1, "Win nicht gefeuert: flag(4,0xEE)=0");
        CHECK(re15_game_flag_get(4, 0xF3) == 1, "sub18 lief nicht: flag(4,0xF3)=0");
    }

    printf("\n===== BEFUND: %s (%d Fehler) =====\n", g_fail ? "DIVERGENT" : "OK", g_fail);
    free(dat);
    return g_fail ? 1 : 0;
}
