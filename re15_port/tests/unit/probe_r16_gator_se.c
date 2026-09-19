/* probe_r16_gator_se.c - MESS-SONDE Runde 16 (2026-09-19), kein add_test.
 *
 * Nutzer: "der Aligator hat immer noch nicht den Angriffs/Biss Sound".
 *
 * Misst OHNE Audio-Backend (Test-Harnisch) den kompletten SE-Pfad des Ports:
 *   1) ROOM2090: Roster-/Spawn-Reihenfolge der Gegner (SCD main00+sub00) - daraus die
 *      Hook-Registrierungs-Reihenfolge wie main.c pc_enemy_load_ex (Gator -> Bank 17,
 *      Spinne 0x25 -> Bank 11) und der EINE Bank-Latch s_re2se_bank_sel (audio_pc.c:1007).
 *   2) ENEMSE.VBS: die SE-Maps der Baenke 17 und 11 (Eintrag, silent, prog/tone/chan/prio,
 *      VAG-Index+Groesse) - ist der Slot leer?
 *   3) Boss-Lauf bis in den Fress-Finisher: jeder SE-Ruf (Gator-Hook / Spinnen-Hook) mit
 *      Frame, Phase, id, flag2000, dem zu diesem Zeitpunkt gelatchten Bank-Wert und dem
 *      Map-Eintrag dieser id in der GELATCHTEN Bank vs. in der ZUSTAENDIGEN Bank.
 *   4) EMS25/EM23-Frame-Flag-Zensus (0x08000000, id = Wort>>28) je Clip/Frame.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_collision.h"
#include "re15_boss_gator.h"
#include "re15_emd.h"
#include "re15_vab.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

extern void re15_gator_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int));
extern void re15_re2spider_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int), int baby);
extern void scd_register_current_rdt(const re15_rdt_t *rdt);

static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *n = (size_t)sz;
    return b;
}

/* ---- ENEMSE-Bank-Map-Dekoder (Muster load_re2_enemy_se_pc, audio_pc.c:1019-1077) ---- */
typedef struct { uint32_t entry[32]; re2_enemse_se_t se[32]; int map_count; re15_vab_t vab; int ok;
                 uint32_t vagsize[32]; } bankmap_t;
static uint8_t *s_vbs = NULL; static size_t s_vbs_sz = 0;

static int bank_load(int bank, bankmap_t *bm)
{
    re2_enemse_rec_t rec; uint32_t vh_off; int i;
    memset(bm, 0, sizeof *bm);
    if (re2_enemse_toc_entry(bank, &rec) != 0) return -1;
    if (!s_vbs) s_vbs = slurp(RE15_ASSET_RE2_DIR "/ENEMSE.VBS", &s_vbs_sz);
    if (!s_vbs) return -2;
    if (rec.edt_off + rec.edt_size > s_vbs_sz || rec.edt_size < 12) return -3;
    {   const uint8_t *edt = s_vbs + rec.edt_off;
        vh_off = (uint32_t)edt[rec.edt_size-8] | ((uint32_t)edt[rec.edt_size-7]<<8)
               | ((uint32_t)edt[rec.edt_size-6]<<16) | ((uint32_t)edt[rec.edt_size-5]<<24);
        if (vh_off + 0x20u > rec.edt_size) return -4;
        if (re15_vab_parse(edt + vh_off, (size_t)rec.edt_size - vh_off, &bm->vab) != 0) return -5;
        bm->map_count = (int)(vh_off / 4);
        for (i = 0; i < 32 && i < bm->map_count; i++) {
            bm->entry[i] = (uint32_t)edt[i*4] | ((uint32_t)edt[i*4+1]<<8) | ((uint32_t)edt[i*4+2]<<16) | ((uint32_t)edt[i*4+3]<<24);
            re2_enemse_decode_entry(bm->entry[i], &bm->se[i]);
            if (!bm->se[i].silent) {
                int ti = bm->se[i].prog * RE15_VAB_TONES_PER_PROGRAM + bm->se[i].tone;
                int vag = (ti < RE15_VAB_TOTAL_TONES) ? (int)bm->vab.tones[ti].vag_index - 1 : -1;
                bm->vagsize[i] = (vag >= 0 && vag < bm->vab.vag_count) ? bm->vab.samples[vag].size : 0;
            }
        }
    }
    printf("BANK %2d: EDT @0x%X (%u B) VBD @0x%X (%u B) map=%d vags=%d\n", bank, rec.edt_off, rec.edt_size,
           rec.vbd_off, rec.vbd_size, bm->map_count, bm->vab.vag_count);
    for (i = 0; i < 32 && i < bm->map_count; i++) {
        if (bm->se[i].silent) { printf("   id %2d: %08X SILENT\n", i, bm->entry[i]); continue; }
        {   int ti = bm->se[i].prog * RE15_VAB_TONES_PER_PROGRAM + bm->se[i].tone;
            const re15_vab_tone_t *t = &bm->vab.tones[ti];
            /* Phase 2 (Skeptiker-Anmerkung "SE-4-Dauer 1,43 s vs 2,40 s"): die Abspielrate ist
             * KEIN VH-Feld, sondern der SPU-Pitch aus dem Tone (note2pitch2 wie audio_pc.c /
             * SsUtKeyOnV; 0x1000 == 44100 Hz). Dauer = Samples / (pitch*44100/4096). */
            uint16_t pitch = re15_vab_note2pitch2(t->min_note, t->pitch_shift, t->center_note, t->pitch_shift);
            unsigned samples = (bm->vagsize[i] / 16u) * 28u;
            double rate = (double)pitch * 44100.0 / 4096.0;
            printf("   id %2d: %08X prog=%d tone=%d chan=%d prio=%d extra=%d vabovr=%d | vag=%d size=%u B vol=%u pan=%u"
                   " | center=%u min=%u shift=%u pitch=0x%03X = %.0f Hz -> %u Samples = %.2f s\n",
                   i, bm->entry[i], bm->se[i].prog, bm->se[i].tone, bm->se[i].chan, bm->se[i].prio,
                   bm->se[i].extra, bm->se[i].vab_override, (int)t->vag_index - 1, bm->vagsize[i], t->vol, t->pan,
                   t->center_note, t->min_note, t->pitch_shift, (unsigned)pitch, rate, samples,
                   rate > 0 ? samples / rate : 0.0); }
    }
    bm->ok = 1;
    return 0;
}

/* ---- Hook-Spione ---- */
static int s_bank_sel = -1;              /* == s_re2se_bank_sel (audio_pc.c:1007) */
static int s_frame = 0; static const char *s_phase = "";
static bankmap_t s_b17, s_b11;
static int s_n_gator = 0, s_n_spider = 0;
static int s_n_bank = 0, s_n_mismatch = 0, s_n_se4 = 0;   /* Phase 2: Latch == zustaendig je Ruf? */
static void spy_bank(int bank)
{
    if (bank != s_bank_sel) printf("   [bank_fn] Latch %d -> %d\n", s_bank_sel, bank);
    s_bank_sel = bank; s_n_bank++;
}
static void spy_common(const char *src, int intended_bank, int id, int flag2000)
{
    int eid = id + (flag2000 ? 0x10 : 0);
    bankmap_t *lat = (s_bank_sel == 17) ? &s_b17 : (s_bank_sel == 11) ? &s_b11 : NULL;
    bankmap_t *want = (intended_bank == 17) ? &s_b17 : &s_b11;
    int ok = (s_bank_sel == intended_bank);
    if (!ok) s_n_mismatch++;
    if (id == 4 && !flag2000) s_n_se4++;
    printf("   SE F%-5d %-8s %-6s id=%d flag=%d | Latch=Bank %d -> %s | zustaendig Bank %d -> %s | %s\n",
           s_frame, s_phase, src, id, flag2000, s_bank_sel,
           !lat ? "KEINE BANK (stumm)" : (eid >= lat->map_count || lat->se[eid].silent) ? "SILENT/leer" : "spielt",
           intended_bank, (eid >= want->map_count || want->se[eid].silent) ? "SILENT/leer" : "spielt",
           ok ? "Latch==zustaendig" : "MISMATCH");
}
static void spy_gator_se(int id, int flag2000)  { s_n_gator++;  spy_common("GATOR", 17, id, flag2000); }
static void spy_spider_se(int id, int flag2000) { s_n_spider++; spy_common("SPINNE", 11, id, flag2000); }

static void frameflag_census(const char *name, re15_enemy_bank_t *eb)
{
    int c, f, n = 0;
    printf("FRAME-FLAG-SEs %s (Bit 0x08000000, id=Wort>>28):", name);
    for (c = 0; c < eb->anim.clip_count; c++)
        for (f = 0; f < eb->anim.clips[c].frame_count; f++) {
            uint32_t w = eb->anim.frames[eb->anim.clips[c].first_frame + f];
            if (w & 0x08000000u) { printf(" c%d/f%d:SE%u", c, f, w >> 28); n++; }
        }
    printf("  (%d)\n", n);
}

int main(void)
{
#ifdef _WIN32
    _putenv("RE15_GB_TEST=1"); _putenv("RE15_GB_STUMM=1");
#else
    setenv("RE15_GB_TEST", "1", 1); setenv("RE15_GB_STUMM", "1", 1);
#endif
    size_t n = 0, es = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &n);
    uint8_t *ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &es);
    re15_enemy_bank_t *eb23, *eb25;
    int s, f, gslot = -1, first_spider = -1, order_pos = 0;
    if (!buf || !ems) { printf("FAIL assets\n"); return 1; }

    printf("=== 2) ENEMSE-Maps ===\n");
    bank_load(17, &s_b17);
    bank_load(11, &s_b11);

    /* --- 1) Raum ueber die SCD spawnen lassen (Roster-Reihenfolge = main.c-Ladereihenfolge) --- */
    printf("\n=== 1) ROOM2090 SCD-Spawn (main00 + sub00, 120 Ticks) ===\n");
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL RDT\n"); return 1; }
    g_room_rdt_ok = 1;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    scd_register_current_rdt(&g_room_rdt);
    if (g_room_rdt.main_scd) scd_thread_start(0, g_room_rdt.main_scd);
    if (g_room_rdt.sub_scd[0]) scd_thread_start(1, g_room_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    g_current_room_id = 0x2090;
    for (f = 0; f < 120; f++) scd_vm_tick();
    for (s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *a = &g_actors[s];
        if (!a->active) continue;
        printf("   slot %2d: type=0x%02X pos=(%d,%d,%d) grid=0x%02X state=%d  -> Ladeposition %d\n",
               s, a->type, a->x, a->y, a->z, a->grid_id, a->state, order_pos++);
        if (a->type == 0x23u && gslot < 0) gslot = s;
        if ((a->type == 0x25u || a->type == 0x26u) && first_spider < 0) first_spider = s;
    }
    printf("   Gator-Slot=%d erste Spinne=%d -> %s registriert ZULETZT (main.c pc_enemy_load_ex je Slot aufsteigend)\n",
           gslot, first_spider,
           (gslot < 0) ? "kein Gator" : (first_spider < 0) ? "nur Gator" : (first_spider > gslot) ? "SPINNE (Bank 11)" : "GATOR (Bank 17)");

    /* Baenke laden (RE2 EM23 + EMS25) */
    eb23 = re15_enemy_alloc(0x23u);
    if (!eb23 || re2_ems_load_bank(ems, es, 0x23, eb23, NULL) != 0) { printf("FAIL Bank 0x23\n"); return 1; }
    eb23->buf = NULL; eb23->ok = 1;
    eb25 = re15_enemy_alloc(0x25u);
    if (!eb25 || re2_ems_load_bank(ems, es, 0x25, eb25, NULL) != 0) { printf("FAIL Bank 0x25\n"); return 1; }
    eb25->buf = NULL; eb25->ok = 1;

    printf("\n=== 4) Frame-Flag-Zensus ===\n");
    frameflag_census("EM23 (Gator)", eb23);
    frameflag_census("EMS25 (Spinne)", eb25);

    /* Hook-Registrierung in Roster-Reihenfolge nachstellen */
    printf("\n=== 3) Hook-Registrierung in Ladereihenfolge + SE-Protokoll ===\n");
    for (s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *a = &g_actors[s];
        static int done23 = 0, done25 = 0;
        if (!a->active) continue;
        if (a->type == 0x23u && !done23) { done23 = 1; printf("   Hook GATOR (Slot %d):", s); re15_gator_audio_hook(spy_gator_se, spy_bank); }
        if ((a->type == 0x25u || a->type == 0x26u) && !done25) { done25 = 1; printf("   Hook SPINNE (Slot %d):", s);
            re15_re2spider_audio_hook(spy_spider_se, spy_bank, a->type == 0x26u); }
    }
    if (gslot < 0) {
        /* main.c:6044-6079: der Gator ist KEIN RDT-Record; main.c setzt ihn in den LETZTEN
         * freien Slot (rueckwaerts gesucht) und die Roster-Schleife :6082 laedt danach je Slot
         * AUFSTEIGEND -> Spinnen-Hook (Bank 11) zuerst, Gator-Hook (Bank 17) ZULETZT. */
        gslot = RE15_ACTOR_MAX - 1;
        printf("   Kein Gator im RDT-Spawn -> main.c-Port-Ergaenzung: Slot %d (letzter freier), Hook GATOR danach:", gslot);
        re15_gator_audio_hook(spy_gator_se, spy_bank);
    }
    printf("   => Latch nach dem Laden: Bank %d (= die Bank, aus der ALLE RE2-SEs des Raums spielen)\n", s_bank_sel);

    /* --- Boss-Lauf: Spieler nah, Lunge/Biss/Finisher erzwingen (probe_gator_maul-Rahmen) --- */
    {
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        re15_actor_t *e = &g_actors[gslot];
        int fress_f0 = -1, p3 = 0;
        memset(e, 0, sizeof *e);
        e->active = 1; e->type = 0x23u; e->x = -6000; e->y = 0; e->z = -22000;
        e->grid_id = 0; e->state = 0; e->em_flag_id = 0xFF;
        re15_enemy_apply_hitbox(e, 0x23u);
        /* Spinnen weit weg parken (keine Kreuzwirkung, Skill re15-room-probe) */
        for (s = 1; s < RE15_ACTOR_MAX; s++) if (s != gslot && g_actors[s].active) { g_actors[s].x = 30000; g_actors[s].z = 30000; }
        pl->active = 1; pl->type = 0; pl->hp = 30; pl->x = -4000; pl->z = -22000; pl->y = 0; pl->floor = 0;
        re15_player_cmd_reset(); re15_damage_seed_rng(0x2545f491u);
        re15_gator_boss_tick(gslot);
        e->x = -6000; e->z = -22000; e->y = -1200;
        for (f = 0; f < 3000; f++) {
            int32_t ox, oz;
            s_frame = f;
            if (pl->hp >= 0) { pl->x = -4000; pl->z = -22000; pl->y = 0; pl->state = 0; pl->motion = 0; pl->hit_react = 0; }
            ox = e->x; oz = e->z;
            s_phase = re15_gator_fressen_hold() ? "FRESSEN" : "kampf";
            re15_gator_boss_tick(gslot);
            if ((e->x != ox || e->z != oz) && !re15_gator_boss_skip_clamp(e)) {
                int32_t nx = e->x, nz = e->z;
                re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz, e->hit_radius_min, e->y, 4u);
                e->x = nx; e->z = nz;
            }
            if (re15_gator_fressen_hold() && fress_f0 < 0) { fress_f0 = f; printf("   -- Fress-Finisher ab F%d (motion=%d)\n", f, (int)e->motion); }
            if (e->motion == 11 && ++p3 > 130) break;
        }
        printf("   Gator-SE-Rufe gesamt: %d, Spinnen-SE-Rufe: %d, Fress-Start F%d\n", s_n_gator, s_n_spider, fress_f0);
        printf("   PHASE 2: bank_fn-Rufe=%d, SE-Rufe mit Latch==zustaendig: %d/%d (MISMATCH %d), "
               "direkte SE-4-Rufe (Lunge-Start @0x80100d64/d84): %d\n",
               s_n_bank, s_n_gator + s_n_spider - s_n_mismatch, s_n_gator + s_n_spider, s_n_mismatch, s_n_se4);
    }

    /* ---- 5) Kieferkurve der RE2-Lunge-Clips 2/3 (Stufe B2 - MESSUNG, kein Umbau) ----
     * Bone 7 = Unterkiefer (Memory reai-v2-gator-bosskampf); Winkel je Frame aus dem EMR-
     * Keyframe (12-bit gepackt, 4096 = 360 Grad). "Maul offen ab ~f54" (Runde 4/6) wird hier
     * durch die Daten ersetzt: Frame des ersten Ausschlags > 64 (1/64 Umdrehung) und Peak. */
    {   static const int clips[] = { 2, 3, 4 }; unsigned ci;
        printf("\n=== 5) Kiefer-Kurve (Bone 7 Rot-X/Y/Z, Q12-Winkel) je Frame, Clips 2/3/4 ===\n");
        for (ci = 0; ci < sizeof clips / sizeof clips[0]; ci++) {
            int c = clips[ci], fr, first = -1, peakf = -1; int peak = 0;
            const re15_emd_clip_t *cl = &eb23->anim.clips[c];
            int16_t ax0 = 0, ay0 = 0, az0 = 0;
            {   int kf0 = cl->first_frame < 0 ? 0 : (int)(eb23->anim.frames[cl->first_frame] & 0xFFFu);
                re15_emd_get_keyframe_angles(&eb23->skel, kf0, 7, &ax0, &ay0, &az0); }
            printf("   Clip %d (%d F): f0 Kiefer=(%d,%d,%d)", c, cl->frame_count, ax0, ay0, az0);
            for (fr = 0; fr < cl->frame_count; fr++) {
                int fi = cl->first_frame + fr, fend = cl->first_frame + cl->frame_count - 1, kf, d;
                int16_t ax = 0, ay = 0, az = 0;
                while ((eb23->anim.frames[fi] & 0x8000u) && fi < fend) fi++;
                kf = (int)(eb23->anim.frames[fi] & 0xFFFu);
                re15_emd_get_keyframe_angles(&eb23->skel, kf, 7, &ax, &ay, &az);
                d = ((int)az - (int)az0 + 0x800) & 0xFFF; d -= 0x800; if (d < 0) d = -d;
                {   int dy = ((int)ay - (int)ay0 + 0x800) & 0xFFF; dy -= 0x800; if (dy < 0) dy = -dy; if (dy > d) d = dy; }
                {   int dx = ((int)ax - (int)ax0 + 0x800) & 0xFFF; dx -= 0x800; if (dx < 0) dx = -dx; if (dx > d) d = dx; }
                if (first < 0 && d > 64) first = fr;
                if (d > peak) { peak = d; peakf = fr; }
                if ((fr % 6) == 0) printf("%s f%d:(%d,%d,%d)", (fr % 60) == 0 ? "\n      " : " ", fr, ax, ay, az);
            }
            printf("\n      => erster Ausschlag > 64 bei f%d, Peak %d bei f%d\n", first, peak, peakf);
        }
    }
    return 0;
}
