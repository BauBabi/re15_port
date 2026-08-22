/* test_1090_climb_pin.c — PIN: "auf die Kiste klettern" (ROOM1090, Spieler-Substate 9/10).
 *
 * Gepinnt werden AUSSCHLIESSLICH Groessen mit Disassembly-Beleg:
 *
 *  T1  Objekt-FLAGS aus dem Opcode-Strom = pc[6..7] | 1  (LAB_80040914 @0x80040990-a4).
 *      ROOM1090 obj0/obj1 tragen 0x0101 — das Bit 0x100, das FUN_8002d2c0 @0x8002d358
 *      mit `& 0x101` als "kletterbar" abfragt. obj2/obj3 (Typ 0) tragen es NICHT.
 *  T2  Sonde FUN_8002d474: Punkt 800 voraus (DAT_800109c8 = 0x320 @0x8002d49c/@0x8002d500),
 *      Kurs auf 90 Grad gerastet (`(yaw+512) & ~1023` @0x8002d4d4-dc). Vor Kiste 0
 *      -> Substate 9 (@0x8002d604) und KIND 0x81 (@0x8002d614/@0x8002d618).
 *  T3  Substate 9: Phase 0 setzt Clip 5 der W01-Bank (@0x80038030 + anim_set @0x8003809c),
 *      Phase 2 Clip 2 der PL00-Bank (@0x80038134 + anim_set @0x8003826c).
 *  T4  Der Ausloesframe 0x1d (@0x80038194; 0x63 nur fuer Spielertyp 0xf @0x800381a8):
 *      Vorwaertsschub +0x8c = 0x47c (@0x800381b0 + FUN_800245d8 @0x800381bc),
 *      Y -= 1800 (@0x800381e4), BAND += 1 (@0x80038200-08).
 *  T5  Ende bei Clip-Umlauf (anim_set-Rueckgabe @0x8001f624) -> Phase 4 -> Substate 0
 *      (@0x800382e8). Endhoehe == Kistendeckel obj.y - 2*box_hy (die Groesse, die
 *      FUN_8001c6e8 fuer ein Objekt zurueckgibt).
 *  N1  Kein ACTION -> 0.
 *  N2  Zu weit weg: FUN_8002d1e8-Reichweite (563 voraus, Radius 900 @0x8002d224) faellt durch.
 *  N3  Falsches Band: `obj[0x82] != DAT_800acad6` @0x8002d370.
 *  N4  Falscher Objekt-Typ: obj2 hat Bit 0x100 nicht -> `& 0x101` @0x8002d358 faellt durch.
 *  N5  Man steht bereits auf einem Objekt: `DAT_800aca3c & 0x4000` @0x8002d2f0 -> 0
 *      (kein Stapeln), und in der Kistenmitte blockt zusaetzlich @0x8002d6a0 den Absprung.
 *  N6  Vor einer TUER (kein Objekt in Reichweite): die Sonde MUSS 0 liefern, damit
 *      @0x80031fe4 den AOT-/Tuer-Scan FUN_80042bac(player,1,0x10) erreicht.
 *  D1  Absprung: auf der Kiste mit Kurs 2048 -> Substate 10 (@0x8003811c), Landung auf
 *      dem Boden, den FUN_8001c6e8 meldet (@0x80038724-2c `sw v0,56(v1)`).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_climb.h"
#include "re15_collision.h"
#include "re15_skeleton.h"

extern scd_vm_t g_scd;

static int g_fail = 0;
static void chk(const char *what, long got, long want)
{
    if (got != want) { printf("FAIL %-42s got=%ld want=%ld\n", what, got, want); g_fail++; }
    else               printf("ok   %-42s %ld\n", what, got);
}

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

static re15_emd_animation_t s_pl00, s_w01;
static int load_banks(void)
{
    char p[600]; size_t sz = 0;
    snprintf(p, sizeof p, "%s/PLD/PL00.EDD", RE15_ASSET_PSX_DIR);
    uint8_t *a = read_file(p, &sz);
    if (!a || re15_emd_parse_animation(a, sz, &s_pl00) != 0) return 0;
    snprintf(p, sizeof p, "%s/PLD/PL00W01.EDD", RE15_ASSET_PSX_DIR);
    uint8_t *b = read_file(p, &sz);
    if (!b || re15_emd_parse_animation(b, sz, &s_w01) != 0) return 0;
    return 1;
}

static re15_rdt_t   s_rdt;
static re15_actor_t *s_pl;

/* Raum + Spieler in den Auslieferungs-Startzustand zuruecksetzen. */
static void reset_room(uint8_t *raw, size_t sz)
{
    (void)sz;
    re15_actor_init();
    scd_vm_init();
    re15_climb_reset();
    g_current_room_id = 0x1090;
    g_room_change.pending = 0;
    s_pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_pl->active = 1; s_pl->type = 0; s_pl->hp = 100;
    s_pl->x = -10100; s_pl->y = -1800; s_pl->z = 4200; s_pl->rot_y = 0;
    scd_room_reenter(&s_rdt, s_pl->x, s_pl->z, 0);
    re15_collision_set_band(re15_collision_band_from_y(s_pl->y));
    (void)raw;
}

/* Genau dorthin stellen, wo der Objekt-Push-Out den Anlauf stoppt:
 * x = kiste.x - box_hx - Spielerradius(450), z = kiste.z, Blick auf +X. */
static void stand_at_crate_front(void)
{
    s_pl->x = (int32_t)g_scd.props[0].x - (int32_t)g_scd.props[0].box_hx - 450;
    s_pl->z = (int32_t)g_scd.props[0].z;
    s_pl->y = -1800;
    s_pl->rot_y = 0;
    re15_collision_set_band(1);
    re15_climb_standing_tick();
}

int main(void)
{
    size_t sz = 0;
    uint8_t *raw = read_file(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1090.RDT", &sz);
    if (!raw) { printf("SKIP: ROOM1090.RDT fehlt\n"); return 77; }
    if (re15_rdt_parse(raw, sz, &s_rdt) < 0) { printf("SKIP: RDT-Parse\n"); return 77; }
    if (!load_banks()) { printf("SKIP: PL00/PL00W01-Baenke fehlen\n"); return 77; }
    reset_room(raw, sz);

    /* ---- T1: Objekt-FLAGS ---------------------------------------------------------- */
    chk("T1 obj0.flags (pc[6..7]|1)", (long)g_scd.props[0].flags, 0x0101);
    chk("T1 obj1.flags", (long)g_scd.props[1].flags, 0x0101);
    chk("T1 obj0.band", (long)g_scd.props[0].band, 1);
    chk("T1 obj2 kein Kletterbit",
        (long)((g_scd.props[2].flags & 0x101u) == 0x101u), 0);

    /* ---- N1: kein ACTION ----------------------------------------------------------- */
    stand_at_crate_front();
    chk("N1 ACTION nicht gedrueckt", re15_climb_try_start(&s_rdt, 0), 0);

    /* ---- N2: zu weit weg (Reichweite 900 um den 563er-Punkt) ----------------------- */
    reset_room(raw, sz);
    s_pl->x = (int32_t)g_scd.props[0].x - 3000;
    s_pl->z = (int32_t)g_scd.props[0].z;
    s_pl->rot_y = 0; re15_collision_set_band(1); re15_climb_standing_tick();
    chk("N2 ausser Reichweite", re15_climb_try_start(&s_rdt, 1), 0);

    /* ---- N3: falsches Band --------------------------------------------------------- */
    reset_room(raw, sz);
    stand_at_crate_front();
    re15_collision_set_band(3);                    /* Kiste ist Band 1 */
    chk("N3 falsches Band", re15_climb_try_start(&s_rdt, 1), 0);

    /* ---- N4: falscher Objekt-Typ (Kletterbit fehlt) -------------------------------- */
    reset_room(raw, sz);
    stand_at_crate_front();
    {
        uint16_t save = g_scd.props[0].flags;
        g_scd.props[0].flags = (uint16_t)(save & ~0x100u);   /* nur Bit 0x100 wegnehmen */
        chk("N4 Objekt ohne Bit 0x100", re15_climb_try_start(&s_rdt, 1), 0);
        g_scd.props[0].flags = save;
    }

    /* ---- N6: vor einer Tuer (Aktion muss durchfallen) ------------------------------ */
    reset_room(raw, sz);
    {
        /* Der erste Tuer-/AOT-Slot des Raums; Blick in alle vier Himmelsrichtungen. */
        const re15_aot_t *d = &g_aot.slots[0];
        int any = 0;
        const int yaws[4] = {0, 1024, 2048, 3072};
        for (int k = 0; k < 4; k++) {
            s_pl->x = d->x; s_pl->z = d->z; s_pl->y = -1800; s_pl->rot_y = (int16_t)yaws[k];
            re15_collision_set_band(1); re15_climb_standing_tick();
            if (re15_climb_try_start(&s_rdt, 1)) any = 1;
            re15_climb_reset();
        }
        chk("N6 Tuer-Slot0: Sonde faellt durch", any, 0);
    }

    /* ---- N7: waehrend des KISTEN-SCHIEBENS (Substate 8) darf ACTION NICHTS ausloesen --
     * FUN_8002d474 hat game-weit GENAU VIER Aufrufer (ghidra1_V2.txt:120408 `XREF[4]`):
     *   80031fd0 / 800323ec / 800326bc / 80032a3c = die Sub-ENTRYs 0/1/2/3
     *   (@0x80073fb0[0..3] = 0x80031f38 / 0x800322e8 / 0x80032604 / 0x80032974).
     * Der Sub-ENTRY 8 (Schieben) ist LAB_8003579c (@0x80073fb0[8]) und enthaelt in seinen
     * 32 Instruktionen KEIN einziges `jal` (@0x8003579c-0x8003580c) — die Sonde kann also
     * beim Schieben gar nicht laufen. Rueckgabe 1 = "Aktion verbraucht" (derselbe Zuschnitt
     * wie fuer Substate 9/10), damit der Aufrufer auch den Tuer-/Examine-Scan entwaffnet.
     *
     * GEMESSEN OHNE DAS GATE (echter Spiellauf): SQUARE+UP an der Kiste startete Substate 9,
     * waehrend `re15_player_push_substate()` gesetzt blieb — der Bank-Zweig fuer Substate 8
     * in anim_select_common.c nahm daraufhin die Kletter-Sentinels als DIREKTE PL00-Clips
     * (230%24 = 14, 231%24 = 15, 24 Bilder geloopt) statt W01-Clip 5 / PL00-Clip 2.
     * Das ist der Nutzer-Befund "nach dem Verschieben eine ganz andere, falsche Animation". */
    reset_room(raw, sz);
    stand_at_crate_front();
    {
        /* Objekt-Pass mit gehaltenem UP fahren, bis obj[+0x8C] die 9 erreicht und das
         * Kontaktbit 0x2000 steht (@0x8002bf60-70 / @0x8002bfd4). */
        for (int f = 0; f < 20 && !re15_prop_push_contact(); f++)
            re15_prop_push_tick(&s_rdt, RE15_PAD_BIT_UP);
        chk("N7 Kontaktbit 0x2000 gesetzt (@0x8002bfd4)", re15_prop_push_contact(), 1);
        /* Erst der Spieler-Tick betritt Substate 8 (@0x800323cc -> `sw 0x801` @0x8003247c). */
        re15_player_tick(NULL, RE15_PAD_BIT_UP);
        chk("N7 Substate 8 aktiv (DAT_800aca59 == 8)", re15_player_push_substate(), 1);
        chk("N7 ACTION verbraucht, KEIN Scan", re15_climb_try_start(&s_rdt, 1), 1);
        chk("N7 KEIN Klettern gestartet", re15_climb_dbg_sub(), 0);
        chk("N7 Schiebe-Phase unveraendert", (long)(re15_player_push_phase() >= 0), 1);
        re15_player_push_reset();
        re15_prop_push_reset();
    }

    /* ---- T2..T5: der Aufstieg ------------------------------------------------------ */
    reset_room(raw, sz);
    stand_at_crate_front();
    const int32_t x0 = s_pl->x;
    chk("T2 try_start vor der Kiste", re15_climb_try_start(&s_rdt, 1), 1);
    chk("T2 Substate", re15_climb_dbg_sub(), 9);
    chk("T2 KIND", re15_climb_dbg_kind(), 0x81);
    chk("T2 getroffenes Objekt (DAT_800ac78c)", re15_climb_dbg_obj(), 0);

    /* N5a: waehrend Substate 9 darf kein zweiter Scan starten (ENTRY = `jr ra`). */
    chk("N5a kein Neustart im Substate 9", re15_climb_try_start(&s_rdt, 1), 1);

    int f_trigger = -1, f_end = -1, motion_turn = -1, motion_up = -1;
    int band_before = re15_collision_debug_band();
    int32_t y_before = s_pl->y;
    int frac_at[200]; for (int i = 0; i < 200; i++) frac_at[i] = -1;
    for (int f = 0; f < 200 && re15_climb_active(); f++) {
        re15_climb_tick(&s_rdt, NULL, &s_pl00, &s_w01);
        re15_climb_standing_tick();
        frac_at[f] = (int)s_pl->anim_frac;      /* der Wert, mit dem DIESES Bild posiert wird */
        if (f == 0) motion_turn = (int)s_pl->motion;
        if (motion_up < 0 && re15_climb_dbg_phase() == 3) motion_up = (int)s_pl->motion;
        if (f_trigger < 0 && s_pl->y != y_before) f_trigger = f;
        if (!re15_climb_active()) f_end = f;
    }
    chk("T3 Phase 0/1 Motion (W01 clip 5)", motion_turn, RE15_PLAYER_MOTION_CLIMB_TURN);
    chk("T3 Phase 2/3 Motion (PL00 clip 2)", motion_up, RE15_PLAYER_MOTION_CLIMB_UP);
    chk("T4 Vorwaertsschub 0x47c", (long)(s_pl->x - x0), 0x47c);
    chk("T4 Y -= 1800", (long)(s_pl->y - y_before), -1800);
    chk("T4 Band += 1", re15_collision_debug_band(), band_before + 1);
    chk("T5 Substate zurueck auf 0", re15_climb_dbg_sub(), 0);
    chk("T5 Endhoehe == Kistendeckel", (long)s_pl->y,
        (long)((int32_t)g_scd.props[0].y - 2 * (int32_t)g_scd.props[0].box_hy));
    chk("T5 steht jetzt auf Objekt 0 (DAT_800ac788)", re15_climb_dbg_standing(), 0);
    /* Clip 2 hat 50 Frames -> Phase 3 laeuft 50 Ticks, davor 1 Tick Phase 0/1,
     * danach 1 Tick Phase 4. Der Ausloeser sitzt auf Frame 0x1d. */
    chk("T4 Ausloesetick (1 Tick Phase0/1 + Frame 0x1d)", f_trigger, 1 + 0x1d);
    chk("T5 Gesamtdauer (1 + 50 + 1 Ticks)", f_end, 1 + 50 + 1 - 1);

    /* ---- T6: FRAC-Crossfade +0x8f (0x800acae3) MUSS abklingen ----------------------
     * FUN_8001f3bc dekrementiert +0x8f bei JEDEM anim_set:
     *   8001f408 lbu s0,143(v1) / 8001f41c bne a3,zero,0x8001f45c  (0 -> kein Blend, kein Decay)
     *   8001f5a8 lbu v0,143(v1) / 8001f5b0 addiu v0,v0,-1 / 8001f5b4 sb v0,143(v1)
     * Substate 9 ruft anim_set in JEDEM Tick (@0x8003809c Phase 1, @0x8003826c Phase 3) und
     * seedet 7 in Phase 0 (@0x80038048) bzw. Phase 2 (@0x80038150).
     *
     * WARUM DAS EIN PIN IST (gemessen 2026-08-22, echter Spiellauf ROOM1090): ohne den Decay
     * stand `frac` 55 Bilder lang auf 7 = 87,5 % Vorframe-Anteil im Renderer
     * (skeleton_common.c:208), INKLUSIVE der Wurzel-Translation. Am Ausloesframe 0x1d springt
     * die Welt-Y um -1800 (@0x800381e4) und die Clip-Wurzel-py von -2614 auf -846
     * (PL00.EDD Clip 2, Slot 28 -> 29) — byte-true heben sich beide auf. Mit eingefrorenem
     * Crossfade folgte die Wurzel nur 12,5 %/Bild: Becken-Y 1357 Einheiten zu hoch, dann
     * 11 Bilder Absinken = der Nutzer-Befund "am Ende faellt er von oben auf die Kiste".
     * DESHALB ist `frac == 0 am Ausloesetick` die eigentliche Regressionsschranke. */
    chk("T6 frac Tick0 (Phase0 seedet 7 @0x80038048)",  frac_at[0], 7);
    chk("T6 frac Tick1 (Phase2 seedet 7 @0x80038150)",  frac_at[1], 7);
    chk("T6 frac Tick2 (Decay @0x8001f5b0)",            frac_at[2], 6);
    chk("T6 frac Tick8 (nach 7 Decays = 0)",            frac_at[8], 0);
    chk("T6 frac am Ausloesetick 0x1d == 0 (kein Blend der -1800-Stufe)",
        frac_at[f_trigger], 0);

    /* ---- N5b: auf der Kiste stehend darf FUN_8002d2c0 nichts mehr finden ----------- */
    chk("N5b Mitte: kein Absprung (eigene Kiste blockt)",
        re15_climb_try_start(&s_rdt, 1), 0);

    /* ---- D1: Absprung nach hinten -------------------------------------------------- */
    s_pl->x = (int32_t)g_scd.props[0].x - 850;
    s_pl->z = (int32_t)g_scd.props[0].z;
    s_pl->rot_y = 2048;                                  /* zurueck Richtung -X */
    re15_climb_standing_tick();
    chk("D1 try_start am Rand", re15_climb_try_start(&s_rdt, 1), 1);
    chk("D1 KIND == 2 (@0x8002d740)", re15_climb_dbg_kind(), 2);
    int saw10 = 0, frac_land = -1, frac_fall_max = -1;
    for (int f = 0; f < 400 && re15_climb_active(); f++) {
        re15_climb_tick(&s_rdt, NULL, &s_pl00, &s_w01);
        re15_climb_standing_tick();
        if (re15_climb_dbg_sub() == 10) saw10 = 1;
        if (s_pl->motion == RE15_PLAYER_MOTION_CLIMB_D1 &&
            (int)s_pl->anim_frac > frac_fall_max) frac_fall_max = (int)s_pl->anim_frac;
        if (frac_land < 0 && (s_pl->motion == RE15_PLAYER_MOTION_CLIMB_LAND5 ||
                              s_pl->motion == RE15_PLAYER_MOTION_CLIMB_LAND6))
            frac_land = (int)s_pl->anim_frac;
    }
    chk("D1 Substate 10 erreicht (@0x8003811c)", saw10, 1);
    /* Substate 10 setzt +0x8f in Phase 2 auf 0 (@0x800383f8 `sb zero,-13597(at)`) — der
     * FALL darf also NIE blenden, sonst haengt die Wurzel der Fallkurve Y += 90+50*t
     * (@0x8003866c-94) genauso nach wie beim Aufstieg. */
    chk("D2 frac im Fall (Clip 4) bleibt 0 (@0x800383f8)", frac_fall_max, 0);
    /* ...und Phase 4 schreibt vor der Landung noch einmal ausdruecklich 0
     * (@0x80038720 `sb zero,-13597(at)`, direkt neben +0x95 = 0 @0x80038718). */
    chk("D2 frac bei der Landung == 0 (@0x80038720)", frac_land, 0);
    chk("D1 Landehoehe == FUN_8001c6e8-Boden", (long)s_pl->y,
        (long)re15_collision_room_coll(&s_rdt, s_pl->x, s_pl->z, 1, 8, 0x100));
    chk("D1 Band nach der Landung", re15_collision_debug_band(), 1);
    chk("D1 Substate zurueck auf 0", re15_climb_dbg_sub(), 0);

    free(raw);
    printf(g_fail ? "\nFEHLER: %d\n" : "\nALLE PINS OK (%d Fehler)\n", g_fail);
    return g_fail ? 1 : 0;
}
