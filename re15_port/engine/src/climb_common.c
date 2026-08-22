/*
 * RE1.5 Rebuilt — KLETTERN: Sonde FUN_8002d474 + Spieler-Substates 9/10.
 * Siehe include/re15_climb.h fuer die Adress-Herleitung der Entity-Felder.
 *
 * ALLE Konstanten in dieser Datei tragen ihre `@0x…`-Adresse aus der
 * ausgelieferten info/Re1.5/PSX.EXE (direkt disassembliert, kein Decompilat —
 * ausser FUN_8001c6e8, dessen Decompilat RE_15_Quellcode_V2/FUN_8001c6e8.c
 * gegen die Bytes seines Prologs geprueft wurde).
 *
 * NICHT PORTIERT (benannte Reste, KEINE geratenen Ersatzwerte):
 *   - FUN_80045630(0, 7 | 4) @0x80038080 — der Sound beim Ansetzen (Auswahl ueber
 *     `**(0x800acbbc) & 0x1000`, dieselbe Boden-Zelle wie die Schrittgeraeusche).
 *   - FUN_80019700(0x6033000, …) @0x80038794/@0x800387bc — der Landestaub (8 Partikel).
 *
 * NACHGETRAGEN 2026-08-22 (war hier als "nicht portiert" gelistet und WAR die Ursache
 * dafuer, dass der Spieler oben nicht auf der Kiste blieb):
 *   - FUN_8002cfd4(player, obj) — die EINGRENZUNG auf den XZ-Kasten des Objekts, jetzt
 *     `re15_collision_prop_contain` (re15_collision.c). Der Spieler hat im Port keine
 *     Kollisionsbox im RAM, aber die beiden Felder, die FUN_8002cfd4 daraus liest, sind
 *     bekannt: `*(u16*)(player[0x78]+6)` IST der Klemmradius PR (@0x80031d6c liest genau
 *     dieses Feld als a1 von FUN_8003b0a4), und die rotierte Boxmitte player[0x7c] ist
 *     fuer den als Kreis behandelten Spieler {0,0}.
 *     Aufrufer: @0x800382dc (Ende Aufstieg), @0x80038604 (Landung auf einem Objekt) und
 *     — der wichtigste — @0x8002bf04 im Objekt-Pass FUN_8002bd44.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "re15_climb.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_collision.h"
#include "re15_engine.h"     /* g_engine.pad_current — Trace */
#include "re15_player.h"
#include "re15_room.h"       /* g_room_rdt / g_room_rdt_ok / g_current_room_id — Trace */
#include "re15_scd.h"
#include "re15_skeleton.h"

extern scd_vm_t g_scd;

/* ---- Zustandsspiegel des Original-Entitys ------------------------------------------ */
static int      s_sub;        /* 0x800aca59 (+0x05) SUBSTATE: 0 / 9 / 10               */
static int      s_kind;       /* 0x800aca5a (+0x06)                                    */
static int      s_phase;      /* 0x800aca5b (+0x07)                                    */
static int      s_frame;      /* 0x800acae9 (+0x95)                                    */
static int      s_speed;      /* 0x800acae0 (+0x8c)                                    */
static int      s_fall_t;     /* 0x800acaf0                                            */
static int      s_obj    = -1;/* 0x800ac78c — Treffer von FUN_8002d2c0 @0x8002d304     */
static int      s_stand  = -1;/* 0x800ac788 — Standobjekt @0x80031d20                  */
static int      s_on_obj;     /* DAT_800aca3c & 0x4000 @0x80031d24                     */
static int16_t  s_floor_y;    /* 0x800acc0e == player+0x1ba (`lh v0,442` @0x800386ac)  */

int re15_climb_active(void)      { return s_sub != 0; }
int re15_climb_dbg_sub(void)     { return s_sub; }
int re15_climb_dbg_kind(void)    { return s_kind; }
int re15_climb_dbg_phase(void)   { return s_phase; }
int re15_climb_dbg_obj(void)     { return s_obj; }
int re15_climb_dbg_standing(void){ return s_stand; }

void re15_climb_reset(void)
{
    s_sub = 0; s_kind = 0; s_phase = 0; s_frame = 0; s_speed = 0;
    s_fall_t = 0; s_obj = -1; s_stand = -1; s_on_obj = 0; s_floor_y = 0;
}

/* Objekt aktiv = `obj[0x00] & 1` (@0x8002d59c / @0x8002d680 / @0x8002d170). */
static int obj_live(int p)
{
    return p >= 0 && p < 16 && g_scd.props[p].active && (g_scd.props[p].flags & 1u);
}

/* ---- FUN_8002d2c0 @0x8002d2c0 — "steht ein kletterbares Objekt an der Sonde?" ------
 *   @0x8002d2f0  `DAT_800aca3c & 0x4000` gesetzt -> 0 (man steht schon auf einem)
 *   @0x8002d358  `obj[0x00] & 0x101`
 *   @0x8002d364-70 `obj[0x82] == DAT_800acad6`  (Objekt-Band == Spieler-Band)
 *   @0x8002d37c  FUN_8002d1e8(player, obj, 0)   (Reichweite 563 voraus, Radius 900)
 *   @0x8002d394  FUN_8002da4c(probe, obj, -112, obj[0x82])
 *   @0x8002d304  Treffer -> DAT_800ac78c = obj ; Rueckgabe 1                          */
static int climb_probe_object(const re15_actor_t *pl, int32_t px, int32_t pz, int pband)
{
    if (s_on_obj) return 0;                                        /* @0x8002d2f0 */
    for (int p = (int)g_scd.prop_count - 1; p >= 0; p--) {
        if (p >= 16 || !g_scd.props[p].active) continue;
        if ((g_scd.props[p].flags & 0x101u) != 0x101u) continue;    /* @0x8002d358 */
        if ((int)g_scd.props[p].band != pband) continue;            /* @0x8002d370 */
        if (!re15_prop_reach_test(pl->x, pl->z, pl->rot_y, p, 0)) continue; /* @0x8002d37c */
        if (re15_collision_prop_box_hit(p, px, pz, -112,            /* @0x8002d394 */
                                        (int)g_scd.props[p].band)) {
            s_obj = p;                                              /* @0x8002d304 */
            return 1;
        }
    }
    return 0;
}

/* ---- FUN_8002d100 @0x8002d100 — welches Objekt liegt unter dem Aktor? --------------
 * Schleife rueckwaerts ueber den Pool, `obj[0x00] & 1` @0x8002d170,
 * `FUN_8002da4c(&aktor.pos, obj, margin, obj[0x82])` @0x8002d188 — das Band-Gate
 * dort ist damit immer wahr, es ist ein reiner XZ-Boxtest. `lw v1,56(s3)`
 * @0x8002d198 liest die AKTOR-Y (konstant), deshalb gewinnt der ERSTE Treffer
 * (der hoechste Pool-Index). */
static int climb_obj_under_xz(int32_t x, int32_t z, int32_t y, int32_t margin)
{
    int32_t best_y = 0x7fff;                                        /* @0x8002d120 */
    int     best   = -1;
    for (int p = (int)g_scd.prop_count - 1; p >= 0; p--) {
        if (!obj_live(p)) continue;                                 /* @0x8002d170 */
        if (!re15_collision_prop_box_hit(p, x, z, margin,
                                         (int)g_scd.props[p].band)) continue;
        if (!(y < best_y)) continue;                                /* @0x8002d1a0 */
        best = p; best_y = y;                                       /* @0x8002d1ac-b0 */
    }
    return best;
}

static int climb_obj_under(const re15_actor_t *pl, int32_t margin)
{
    return climb_obj_under_xz(pl->x, pl->z, pl->y, margin);
}

/* ---- Der Standobjekt-Test aus FUN_80031c44 @0x80031ce8-0x80031d20 als reine Abfrage ----
 * `FUN_8002d100(player, 0x12)` @0x80031ce8 und `obj[0x82] + 1 == DAT_800acad6`
 * @0x80031cfc-0c. Rueckgabe = Objektindex (DAT_800ac788) oder -1.
 *
 * WARUM eine Abfrage an einer frei waehlbaren Position: der Original-Objekt-Pass
 * FUN_8002bd44 laeuft @0x8001ce14 UNMITTELBAR nach FUN_80031c44 @0x8001ce0c und liest
 * dort ein DAT_800ac788, das im selben Frame @0x80031d20 aus der DANN aktuellen
 * Spielerposition gebildet wurde. Der Port ruft `re15_climb_standing_tick()` erst am
 * Frame-ENDE — der gespeicherte Wert waere im Objekt-Pass also einen Frame alt. */
int re15_climb_standing_probe(int32_t x, int32_t z)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int o = climb_obj_under_xz(x, z, pl->y, 0x12);                  /* @0x80031ce8 (a1 = 0x12) */
    if (o < 0) return -1;                                           /* @0x80031cf4 */
    if ((int)g_scd.props[o].band + 1 != re15_collision_debug_band())/* @0x80031cfc-0c */
        return -1;
    return o;                                                       /* @0x80031d20 */
}

/* ---- RE15_CLIMB_TRACE=<datei> — Messschrieb fuer den ECHTEN Spiellauf ---------------
 * Eine Zeile je Gameplay-Frame: Position/Band/Substate/Standobjekt. Diagnose only. */
static void climb_trace(void)
{
    static FILE *f = NULL; static int init = 0; static unsigned tick = 0;
    if (!init) { init = 1;
        const char *p = getenv("RE15_CLIMB_TRACE");
        if (p && *p) f = fopen(p, "w"); }
    if (!f || !g_room_rdt_ok) return;
    tick++;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    fprintf(f, "T%u room=%04X pm=%d pad=%04X pos=(%ld,%ld,%ld) rot=%d band=%d sub=%d kind=0x%02X ph=%d "
               "obj=%d stand=%d mo=%d af=%d rc_b8=%d rc_bnd=%d\n",
            tick, (unsigned)g_current_room_id, (int)g_scd.player_mode,
            (unsigned)g_engine.pad_current,
            (long)pl->x, (long)pl->y, (long)pl->z, (int)pl->rot_y,
            re15_collision_debug_band(), s_sub, s_kind, s_phase, s_obj, s_stand,
            (int)pl->motion, (int)pl->anim_frame,
            (int)re15_collision_room_coll(&g_room_rdt, pl->x, pl->z, 1, 8, 0x100),
            (int)re15_collision_room_coll(&g_room_rdt, pl->x, pl->z, 1,
                                          re15_collision_debug_band(), 0x100));
    fflush(f);
}

/* ---- Per-Frame-Block @0x80031cd8-0x80031d2c ---------------------------------------- */
void re15_climb_standing_tick(void)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_stand  = -1;                                                  /* @0x80031cd8 */
    s_on_obj = 0;                                                   /* @0x80031cdc-e4 */
    int o = climb_obj_under(pl, 0x12);                              /* @0x80031ce8 (a1 = 0x12) */
    if (o < 0) { climb_trace(); return; }                           /* @0x80031cf4 */
    if ((int)g_scd.props[o].band + 1 != re15_collision_debug_band()) /* @0x80031cfc-0c */
        { climb_trace(); return; }
    s_stand  = o;                                                   /* @0x80031d20 */
    s_on_obj = 1;                                                   /* @0x80031d24 (|= 0x4000) */
    climb_trace();
}

/* ==== FUN_8002d474 @0x8002d474 — die ACTION-Sonde ==================================== */
int re15_climb_try_start(const re15_rdt_t *rdt, int action_pressed)
{
    if (s_sub) return 1;                    /* Substate 9/10 haben `jr ra`-ENTRYs
                                             * (0x80037fd0 / 0x8003830c) — kein neuer Scan */
    if (!action_pressed) return 0;          /* @0x80031fc4 andi 0x80 / beq */

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int band = re15_collision_debug_band();          /* DAT_800acad6 */
    if (band < 0) return 0;

    /* Sondenpunkt: {800,0} (DAT_800109c8, 8 Byte per lwl/lwr @0x8002d4a0-bc; als
     * s16 gelesen @0x8002d500/@0x8002d50c) um den auf 90 Grad GERASTETEN Kurs
     * gedreht: `(yaw + 512) & ~1023` @0x8002d4d4-dc, dann FUN_8004f008 @0x8002d4e4. */
    const int a = (int)(uint16_t)(((uint16_t)pl->rot_y + 512u) & 0xFC00u) & 0xFFF;
    const int32_t cs = re15_cos_q12(a), sn = re15_sin_q12(a);
    const int32_t fx = pl->x + (int32_t)((800 * cs) >> 12);         /* @0x8002d518-20 */
    const int32_t fz = pl->z - (int32_t)((800 * sn) >> 12);         /* @0x8002d51c-28 */

    /* s0/s6 = FUN_8003b7f0(&probe, 0, band) @0x8002d524 — das Attributwort der Zelle.
     * s5    = FUN_8002d2c0(&probe)          @0x8002d530                              */
    const uint16_t s6 = re15_collision_floor_typeword(rdt, fx, fz, band, 0);
    const int      s5 = climb_probe_object(pl, fx, fz, band);

    if (s5 != 0 || (s6 & 1u)) {
        /* ---- AUFWAERTS @0x8002d550 ------------------------------------------------ */
        for (int p = (int)g_scd.prop_count - 1; p >= 0; p--) {
            if (!obj_live(p)) continue;                             /* @0x8002d59c */
            if (re15_collision_prop_box_hit(p, fx, fz, 200, (band + 1) & 0xff))
                return 0;                                           /* @0x8002d5bc-c4 */
            if (re15_collision_prop_box_hit(p, fx, fz, 200, (band - 2) & 0xff))
                return 0;                                           /* @0x8002d5dc-e4 */
        }
        s_sub = 9;                                                  /* @0x8002d604 */
        /* @0x8002d610 `beq s5,zero,0x8002d61c` (Bytes 12 a0 00 02 geprueft): NUR wenn
         * s5 != 0 wird +0x06 geschrieben, und zwar 0x81 (@0x8002d614/@0x8002d618).
         * Der Block @0x8002d624 (+0x06 = 1) ist im ausgelieferten Binary unerreichbar.
         * PHASE wird in diesem Zweig NICHT genullt — sie ist es bereits, weil der FSM
         * sie beim Verlassen auf 0 setzt (@0x800382f4 / @0x80038824). */
        if (s5) s_kind = 0x81;
        return 1;
    }

    /* ---- ABWAERTS @0x8002d634 ----------------------------------------------------- */
    for (int p = (int)g_scd.prop_count - 1; p >= 0; p--) {
        if (!obj_live(p)) continue;                                 /* @0x8002d680 */
        if (re15_collision_prop_box_hit(p, fx, fz, 450, (band - 1) & 0xff))
            return 0;                                               /* @0x8002d6a0-a8 */
        if (p != s_stand &&                                         /* @0x8002d6b0-b8 */
            re15_collision_prop_box_hit(p, fx, fz, 450, band))
            return 0;                                               /* @0x8002d6c8-d0 */
    }

    int kind = -1;
    if (s_on_obj) {                                                 /* @0x8002d6f4 (&0x4000) */
        /* @0x8002d708 FUN_8002d1e8(player, DAT_800ac788, 1) — Reichweite 700.
         * Treffer -> der Sprung wird NICHT ueber diesen Zweig genommen
         * (`bne v0,zero,0x8002d750` faellt in den SCA-Zweig unten). */
        if (!re15_prop_reach_test(pl->x, pl->z, pl->rot_y, s_stand, 1)) {
            int16_t h = re15_collision_room_coll(rdt, fx, fz, 1, 8, 0x100); /* @0x8002d724 */
            if (!(pl->y < (int32_t)h)) return 0;                    /* @0x8002d738-3c */
            kind = 2;                                               /* @0x8002d740 */
        }
    }
    if (kind < 0) {
        if (!(s6 & 2u)) return 0;                                   /* @0x8002d74c-50 */
        int16_t h = re15_collision_room_coll(rdt, fx, fz, 1, band, 0x100); /* @0x8002d764 */
        if (!(pl->y < (int32_t)h)) return 0;                        /* @0x8002d778-7c */
        kind = (int)(s6 & 0x0eu);                                   /* @0x8002d780 */
    }
    s_kind  = kind;                                                 /* @0x8002d788 */
    s_sub   = 9;                                                    /* @0x8002d794 */
    s_phase = 0;                                                    /* @0x8002d79c */
    return 1;
}

/* ==== Animations-Hilfen ============================================================= */
/* FUN_8001f314 @0x8001f314 -> FUN_8001f3bc: SAMPLE am aktuellen +0x95, DANN
 * `+0x95 += 1` @0x8001f618 mit `sltu v0,v0,frame_count` @0x8001f624 —
 * Rueckgabe 0 solange < frame_count, 1 beim Umlauf. */
static int anim_step(const re15_emd_animation_t *an, int clip)
{
    int len = 1;
    if (an && clip >= 0 && clip < an->clip_count) {
        len = (int)an->clips[clip].frame_count;
        if (len <= 0) len = 1;
    }
    s_frame++;
    if (s_frame < len) return 0;
    s_frame = 0;
    return 1;
}

static void set_motion(re15_actor_t *p, int motion)
{
    p->motion              = (int16_t)motion;
    p->anim_flags          = 0;
    p->motion_init_delay   = 0;
}

/* FUN_800245d8(0) — der Vorwaerts-Schub um +0x8c entlang der Blickrichtung
 * (actor_locomotion.c dokumentiert dieselbe Funktion fuer den Geh-Pfad). */
static void pos_advance(re15_actor_t *p)
{
    int32_t c = re15_cos_q12((int)p->rot_y), s = re15_sin_q12((int)p->rot_y);
    p->x += (int32_t)(((int64_t)s_speed * c) >> 12);
    p->z -= (int32_t)(((int64_t)s_speed * s) >> 12);
}

/* ==== SUBSTATE 9 = LAB_80037fd8 (HOCHKLETTERN) ====================================== */
static void climb_sub9(const re15_rdt_t *rdt, re15_actor_t *p,
                       const re15_emd_animation_t *pl00, const re15_emd_animation_t *w01)
{
    (void)rdt;
    if (s_phase > 4) { s_phase = 0; return; }   /* `sltiu v0,v1,0x5` @0x80037fe8 */

    switch (s_phase) {
    case 0:                                     /* @0x80038014 — FAELLT DURCH nach case 1 */
        s_speed  = 0x4b;                        /* @0x80038014-1c  +0x8c = 75 */
        s_phase  = 1;                           /* @0x80038020-28 */
        set_motion(p, RE15_PLAYER_MOTION_CLIMB_TURN);   /* @0x8003802c-34 +0x94 = 5 (W01) */
        s_frame  = 0;                           /* @0x80038040       +0x95 = 0 */
        p->anim_frac = 7;                       /* @0x80038048       +0x8f = 7 */
        /* fallthrough */
    case 1: {                                   /* @0x8003804c */
        /* @0x80038080 FUN_80045630(0, 7 - 3*((**0x800acbbc & 0x1000)>>12)) = OPEN (Sound) */
        p->anim_frame = (uint16_t)s_frame;      /* SAMPLE, dann Vorschub (@0x8001f35c) */
        (void)anim_step(w01, 5);                /* @0x8003809c anim_set(W01-Bank, clip 5) */

        /* Kurs-Einschwingen @0x800380a4-d4: Schrittweite (yaw>>2)&0xff, Richtung ueber
         * Bit 0x200; Ausstieg wenn (yaw & 0x3e0) == 0, dann `yaw &= 0xff00`
         * (@0x800380e8-f4) — das Ergebnis ist damit ein Vielfaches von 0x400 = 90 Grad. */
        uint16_t y = (uint16_t)((uint16_t)p->rot_y & 0x0FFFu);
        uint16_t st = (uint16_t)((y >> 2) & 0xff);
        y = (uint16_t)((y & 0x200u) ? (y + st) : (y - st));
        p->rot_y = (int16_t)(y & 0x0FFFu);
        if (y & 0x3e0u) return;                 /* @0x800380ec */
        p->rot_y = (int16_t)((y & 0xff00u) & 0x0FFFu);   /* @0x800380f4 */
        s_phase = 2;                            /* @0x80038108 */
        if (s_kind & 2) {                       /* @0x8003810c-10 */
            s_sub   = 10;                       /* @0x8003811c */
            s_phase = 0;                        /* @0x800382f4 (j 0x800382f0) */
        }
        return;
    }
    case 2:                                     /* @0x80038128 — FAELLT DURCH nach case 3 */
        s_phase = 3;                            /* @0x80038128-30 */
        set_motion(p, RE15_PLAYER_MOTION_CLIMB_UP);    /* @0x80038134-3c +0x94 = 2 (PL00) */
        s_frame = 0;                            /* @0x80038148       +0x95 = 0 */
        p->anim_frac = 7;                       /* @0x80038150       +0x8f = 7 */
        /* fallthrough */
    case 3: {                                   /* @0x80038154 */
        if (s_frame == 0x11) p->hit_react |= 1;                  /* @0x80038164-80 */
        /* Ausloese-Frame: Spielertyp (+0x08) == 0xf -> 0x63, sonst 0x1d (@0x80038184-ac) */
        const int trg = ((int)p->type == 0x0f) ? 0x63 : 0x1d;
        if (s_frame == trg) {
            s_speed = 0x47c;                                     /* @0x800381b0-b8 */
            pos_advance(p);                                      /* @0x800381bc */
            const int band = re15_collision_debug_band();        /* @0x800381c8 */
            const int32_t px = p->x, pz = p->z;                  /* @0x800381d8-e0 */
            p->y -= 1800;                                        /* @0x800381e4-ec */
            re15_collision_set_band(band + 1);                   /* @0x80038200-08 */
            p->pos_s_x = (uint16_t)px;                           /* @0x80038210 */
            p->pos_s_z = (uint16_t)pz;                           /* @0x80038218 */
            s_floor_y  = (int16_t)(s_floor_y - 1800);            /* @0x8003821c-24 */
            p->pos_s_y = (uint16_t)p->y;                         /* @0x8003822c */
        }
        if (s_frame == 0x25) p->hit_react &= (uint8_t)0xfe;      /* @0x80038238-58 */
        p->anim_frame = (uint16_t)s_frame;                       /* SAMPLE vor dem Vorschub */
        s_phase += anim_step(pl00, 2);                           /* @0x8003826c-88 */
        return;
    }
    case 4:                                     /* @0x80038294 */
        if ((s_kind & 0x80) && s_obj >= 0 && s_obj < 16) {       /* @0x800382a4 */
            p->pos_s_x = (uint16_t)(int32_t)g_scd.props[s_obj].x; /* @0x800382bc-c8 */
            p->pos_s_z = (uint16_t)(int32_t)g_scd.props[s_obj].z; /* @0x800382cc-d8 */
            /* @0x800382dc FUN_8002cfd4(player, obj) — die Eingrenzung auf die
             * Objektoberflaeche. Sie laeuft NACH dem posS-Snap auf die Objektmitte,
             * die Vorposition ist also garantiert "drin" -> jede Achse, die JETZT
             * ausserhalb liegt, wird auf die Kante gezogen. */
            re15_collision_prop_contain(s_obj, &p->x, &p->z);
        }
        s_sub   = 0;                            /* @0x800382e8 */
        s_kind  = 0;                            /* @0x800382ec */
        s_phase = 0;                            /* @0x800382f4 */
        return;
    default:
        return;
    }
}

/* ==== SUBSTATE 10 = LAB_80038314 (HERUNTERSPRINGEN) ================================= */
static void climb_sub10(const re15_rdt_t *rdt, re15_actor_t *p,
                        const re15_emd_animation_t *pl00)
{
    if (s_phase > 6) { s_phase = 0; return; }   /* `sltiu v0,v1,0x7` @0x80038330 */

    switch (s_phase) {
    case 0:                                     /* @0x8003835c — FAELLT DURCH nach case 1 */
        s_phase = 1;                            /* @0x8003835c-64 */
        set_motion(p, RE15_PLAYER_MOTION_CLIMB_D0);   /* @0x80038368-70 +0x94 = 3 */
        s_frame = 0;                            /* @0x8003837c */
        p->anim_frac = 7;                       /* @0x80038384 */
        /* fallthrough */
    case 1:                                     /* @0x80038388 */
        p->anim_frame = (uint16_t)s_frame;
        (void)anim_step(pl00, 3);               /* @0x8003839c */
        /* Der Frame wird hier NACH dem Vorschub geprueft (@0x800383a4-b0). */
        if (s_frame == 0x0d) s_phase = 2;       /* @0x800383bc */
        return;

    case 2: {                                   /* @0x800383c8 — FAELLT DURCH nach case 3 */
        s_phase = 3;                            /* @0x800383cc-d4 */
        set_motion(p, RE15_PLAYER_MOTION_CLIMB_D1);   /* @0x800383d8-e8 +0x94 = 4 */
        s_frame = 0;                            /* @0x800383f0 */
        p->anim_frac = 0;                       /* @0x800383f8  +0x8f = 0 (NICHT 7) */
        s_fall_t = 0;                           /* @0x80038400 */
        p->hit_react |= 1;                      /* @0x80038414-1c */
        s_speed = 0x320;                        /* @0x80038408-10  +0x8c = 800 */
        pos_advance(p);                         /* @0x80038418 */
        const int band0 = re15_collision_debug_band();           /* @0x80038438 */
        p->y += 500;                            /* @0x8003843c-44 */
        int16_t h = re15_collision_room_coll(rdt, p->x, p->z, 0, band0, 0x100); /* @0x80038448 */
        p->pos_s_x = (uint16_t)p->x;            /* @0x80038464/@0x80038480 */
        s_floor_y  = h;                         /* @0x80038494 */
        p->pos_s_z = (uint16_t)p->z;            /* @0x80038498 */
        p->pos_s_y = (uint16_t)p->y;            /* @0x800384a0 */
        /* Band = -(h / 1800): `mult 0x91a2b3c5 / mfhi + t0 / sra 10 / subu`
         * @0x80038460-bc — die Magic-Division durch 1800 mit Vorzeichen. */
        re15_collision_set_band(-((int)h / 1800));               /* @0x800384bc */
        {   /* @0x800384c4 FUN_8003b0a4(&pos, radius, 1) = die SCA-Klemme */
            int32_t nx = p->x, nz = p->z;
            re15_collision_constrain(rdt, p->x, p->z, &nx, &nz);
            p->x = nx; p->z = nz;
        }
        /* @0x800384e0-50 Objekt-Pass: FUN_8002cabc(player, obj, 1) je aktivem Objekt. */
        for (int q = 0; q < (int)g_scd.prop_count && q < 16; q++) {
            if (!obj_live(q)) continue;                          /* @0x80038514 */
            int32_t ox = g_scd.props[q].x, oz = g_scd.props[q].z;
            if (re15_collision_push_prop(q, p->x, p->z, &ox, &oz)) {
                g_scd.props[q].x = ox; g_scd.props[q].z = oz;    /* @0x8002cd44/@0x8002cdac */
            }
        }
        /* @0x80038560 FUN_8002d100(player, 450) — landet er direkt auf einem Objekt? */
        const int lo = climb_obj_under(p, 0x1c2);
        if (lo >= 0) {
            const int32_t top = (int32_t)g_scd.props[lo].y - 1800;   /* @0x80038580 */
            if (!((int32_t)s_floor_y < top) && !(top < p->y)) {      /* @0x80038584-a0 */
                s_floor_y = (int16_t)top;                            /* @0x800385c8 */
                re15_collision_set_band(-(top / 1800));              /* @0x800385e0 */
                p->pos_s_x = (uint16_t)(int32_t)g_scd.props[lo].x;   /* @0x800385f0 */
                p->pos_s_z = (uint16_t)(int32_t)g_scd.props[lo].z;   /* @0x80038600 */
                re15_collision_prop_contain(lo, &p->x, &p->z);       /* @0x80038604 */
            }
        }
        p->pos_s_x = (uint16_t)p->x;            /* @0x80038628 */
        p->pos_s_z = (uint16_t)p->z;            /* @0x80038630 */
        p->pos_s_y = (uint16_t)p->y;            /* @0x80038638 */
        /* fallthrough */
    }
    case 3: {                                   /* @0x8003863c */
        /* Y += 90 + 50*fall_t, fall_t++ (@0x80038660-9c). */
        const int t = s_fall_t;
        s_fall_t = t + 1;                       /* @0x8003867c */
        p->y += 90 + 50 * (int)(int16_t)t;      /* @0x8003866c/@0x80038680-94 */
        p->anim_frame = (uint16_t)s_frame;
        (void)anim_step(pl00, 4);               /* @0x80038698 */
        if ((int32_t)s_floor_y - 800 < p->y) s_phase = 4;        /* @0x800386ac-c8 */
        return;
    }
    case 4:                                     /* @0x800386d4 — FAELLT DURCH nach case 5 */
        s_phase = 5;                            /* @0x800386e8 */
        /* +0x94 = 5, aber 6 wenn (KIND & 0xc) >= 5 (@0x800386f4-708). */
        set_motion(p, ((s_kind & 0x0c) < 5) ? RE15_PLAYER_MOTION_CLIMB_LAND5
                                            : RE15_PLAYER_MOTION_CLIMB_LAND6);
        s_frame = 0;                            /* @0x80038718 */
        p->y = (int32_t)s_floor_y;              /* @0x80038724-2c */
        p->hit_react &= (uint8_t)0xfe;          /* @0x80038748-54 */
        /* @0x80038758-cc der Landestaub (FUN_80019700 x8) = OPEN (siehe Dateikopf) */
        /* fallthrough */
    case 5:                                     /* @0x800387d4 */
        p->anim_frame = (uint16_t)s_frame;
        s_phase += anim_step(pl00,
                             (p->motion == RE15_PLAYER_MOTION_CLIMB_LAND6) ? 6 : 5);
        return;                                 /* @0x800387e8-804 */
    case 6:                                     /* @0x80038810 */
        s_sub   = 0;                            /* @0x80038814 */
        s_kind  = 0;                            /* @0x8003881c */
        s_phase = 0;                            /* @0x80038824 */
        return;
    default:
        return;
    }
}

void re15_climb_tick(const re15_rdt_t *rdt,
                     const re15_emd_skeleton_t  *pl00_skel,
                     const re15_emd_animation_t *pl00_anim,
                     const re15_emd_animation_t *w01_anim)
{
    (void)pl00_skel;
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Der Dispatcher @0x80031ed0-f14 ruft ENTRY (0x80037fd0/0x8003830c = beide
     * `jr ra`) und danach UPDATE aus PTR_LAB_80073ff0[substate]. */
    if      (s_sub ==  9) climb_sub9 (rdt, p, pl00_anim, w01_anim);
    else if (s_sub == 10) climb_sub10(rdt, p, pl00_anim);
}
