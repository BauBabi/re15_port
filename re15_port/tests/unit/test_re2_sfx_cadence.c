/**
 * @file test_re2_sfx_cadence.c
 * @brief RE2-Flavor: der Frame-Wort-SE-Pfad (`jal 0x801016c8`) — SUB-GATE + DRITTEL-TAKT.
 *
 * Pinnt den Fix aus Cluster 3 / Review-Fund F7 ("die RE2-Frame-Wort-SEs feuern 3x zu haeufig").
 * Alle drei Pins haengen an selbst disassemblierten Bytes (info/re2leon/COMMON/BIN/EMOVL10_S0.BIN,
 * RAW @0x80100000; PSX-EXE-Anteile aus info/Re1.5/PSX.EXE):
 *
 *   Call-Site 1 (EXEC[1] WALK, Fn 0x80101A40) — roh-Byte-verifiziert:
 *     80101cd0: jal  0x8002959c            ; ANIM-ADVANCE (der Produzent von +0x14D)
 *     80101cd8: 9622010e  lhu  v0,270(s1)  ; +0x10E
 *     80101ce0: 30420080  andi v0,v0,0x80
 *     80101ce4: 14400006  bne  v0,zero,0x80101d00
 *     80101cec: 9622021a  lhu  v0,538(s1)  ; +0x21A
 *     80101cf4: 30428000  andi v0,v0,0x8000
 *     80101cf8: 10400019  beq  v0,zero,0x80101d60   ; Gate faellt -> KEIN SE
 *     80101d00: 9223014d  lbu  v1,333(s1)  ; +0x14D = Frame-Byte des laufenden Clips
 *     80101d04-24         lui 0xaaaa/ori 0xaaab/multu/mfhi/srl 1/sll 1/addu/subu  (= v1 % 3)
 *     80101d28: addiu v0,zero,2
 *     80101d2c: bne  v1,v0,0x80101d5c     ; nur Rest 2 erreicht den Call
 *     80101d34: jal  0x801016c8           ; Suchwort 0x0C0405B2, genau 2 Treffer im Overlay
 *   Call-Site 2 (EXEC[2] BUMP, Fn 0x80102260): @0x80102400-04 / @0x80102414-18 /
 *     @0x80102420-4c / jal @0x80102454 — identisch.
 *
 *   Der Emitter selbst (0x801016c8): `lw v0,376(a1)` @0x801016D4 (+0x178 = Frame-Wort-Zeiger),
 *   `lw a0,0(v0)` @0x801016DC, `and 0x08000000` @0x801016E4, `srl 28` @0x801016EC,
 *   `sltiu v0,a0,0x2` @0x801016F0, `jal 0x8005bd6c` @0x801016FC.
 *
 *   Der einzige portierbare Produzent des Sub-Gate-Bits (RE2-INIT):
 *     80100894: lbu v1,8(s2)          ; Entity-Typ
 *     801008a0: addiu v0,zero,17      ; 0x11
 *     801008bc: bne v1,v0,0x801008d8  ; nur Typ 0x11 faellt durch
 *     801008c4: lhu v0,538(s2)
 *     801008d0: ori v0,v0,0x8000
 *     801008d4: sh  v0,538(s2)        ; +0x21A |= 0x8000
 *   Das zweite Gate-Bit +0x10E&0x80 kommt AUSSCHLIESSLICH aus der Gore-/Dismember-Hilfsfunktion
 *   0x80106128 (`lhu +0x10E` @0x8010613C / `ori 0x80` @0x80106144 / `sh +0x10E` @0x80106148,
 *   14 Aufrufer, alle im nicht portierten RE2-Dismember-/Death-Zweig) — im Port also OPEN.
 *
 * PIN A  Produzent: nach dem INIT-Tick traegt genau der Typ-0x11-Zombie das Bit, 0x10/0x16 nicht.
 * PIN B  Sub-Gate:  ohne Bit KEIN einziger Frame-Wort-SE; mit Bit feuert der Pfad.
 * PIN C  Drittel-Takt: jeder Frame-Wort-SE liegt auf einem Frame-Slot mit slot%%3==2, UND die
 *        Gesamtzahl deckt sich exakt mit der unabhaengig aus der RE2-Bank nachgerechneten
 *        Soll-Menge {Slots mit SE-Bit UND slot%%3==2}. Ohne den Takt weicht die Zahl ab, sobald
 *        der Clip einen SE-Frame mit slot%%3!=2 hat (Pin C2 sucht sich genau so einen Clip).
 *
 * Deterministisch: re15_damage_seed_rng(0x0badf00d), fester Raum ROOM1140, feste Bank-Ladefolge.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_room.h"      /* g_current_room_id */
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

/* ---- SE-Senke ------------------------------------------------------------------------------ */
static int s_se_fw;                 /* Frame-Wort-SEs (IDs 0/1) im laufenden Fenster */
static int s_se_other;
static void se_cap(int se_id, int flag2000) { (void)flag2000;
    if (se_id < 2) s_se_fw++; else s_se_other++; }
static void bank_cap(int bank) { (void)bank; }

/* ein Spielframe in game_step-Reihenfolge */
static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

/* die ECHTE RE2-Bank (CDEMD0.EMS) — nur sie traegt die SFX-Bits 0x08000000 im Frame-Wort */
static uint8_t *s_ems = NULL; static long s_ems_sz = 0;
static void load_re2_bank(const char *base, uint8_t type)
{
    if (!s_ems) { char p[600]; snprintf(p, sizeof p, "%s/../RE2/CDEMD0.EMS", base);
                  s_ems = slurp(p, &s_ems_sz); }
    if (!s_ems) { fprintf(stderr, "WARN: shared_assets/RE2/CDEMD0.EMS nicht lesbar\n"); return; }
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_ems, (size_t)s_ems_sz, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
}

/* die Bank, aus der der Emitter das Frame-Wort zieht (identische Wahl wie re15_enemy_anim_sfx) */
static const re15_emd_animation_t *emit_bank(const re15_actor_t *e)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok) return NULL;
    return re15_actor_uses_loco_bank(e) ? &b->anim_loco : &b->anim;
}

/* Soll-Zahl: wie oft feuert der ORIGINAL-Pfad, wenn der Clip `clip` genau `ticks` Ticks ab
 * Frame `start` laeuft?
 * MODELL-UMBAU 2026-09-05 (Welle B/F4, verify_zombie_conf.md (e) Korrektur C): der Block
 * traegt jetzt auch den EXTRA-ADVANCE 0x8002A9C8 (@0x80101D54/@0x80102460, vorher bewusst
 * fehlend) — am Residue-2-Tick laeuft NACH der SE-Probe (@0x80101D34/@0x80102454, Probe VOR
 * dem Advance!) ein zweiter Frame-Schritt = 3 Frames je 2 Ticks (1,5-fache Kadenz; das alte
 * Modell nahm 1 Frame/Tick an). Harness-Reihenfolge wie run_all: Brain (Probe+Extra) VOR
 * dem globalen Advance. */
static int expected_hits(const re15_emd_animation_t *A, int clip, int start, int ticks, int with_cadence)
{
    if (!A || clip < 0 || clip >= A->clip_count) return -1;
    const re15_emd_clip_t *c = &A->clips[clip];
    if (c->frame_count <= 0) return -1;
    int n = 0;
    unsigned frame = (unsigned)start;
    for (int t = 0; t < ticks; t++) {
        int slot = (int)(frame % (unsigned)c->frame_count);
        if (!with_cadence || (slot % 3) == 2) {
            uint32_t fw = A->frames[c->first_frame + slot];
            if (with_cadence) {
                if ((fw & 0x08000000u) && (fw >> 28) < 2u) n++;   /* Probe @0x801016c8 */
                frame++;                                          /* Extra-Advance 0x8002A9C8 */
            } else if ((fw & 0x08000000u) && (fw >> 28) < 2u) {
                n++;
            }
        }
        frame++;                                                  /* globaler Advance (959c) */
    }
    return n;
}

int main(void)
{
    const char *base = RE15_ASSET_PSX_DIR;
    char path[600];
    long sz = 0;
    printf("== RE2-Frame-Wort-SE: Sub-Gate + Drittel-Takt ==\n");

    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", base);
    uint8_t *data = slurp(path, &sz);
    if (!data) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, (size_t)sz, &rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->y = 0; pl->z = 0;
    if (rdt.main_scd) scd_thread_start(0, rdt.main_scd);
    scd_thread_start(1, rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();
    load_re2_bank(base, 0x10);
    load_re2_bank(base, 0x11);
    load_re2_bank(base, 0x16);

    /* Der Spieler weit weg, damit die INIT-Ticks nicht sofort in Angriffszustaende kippen. */
    pl->x = 30000; pl->z = 30000;

    /* ------------------------------------------------------------------ PIN A: der Produzent */
    /* Vor dem ersten KI-Tick steht jeder Zombie in state 0; erst der Tick fuehrt re2z_init aus
     * (RE2-Zustandstabelle @0x8010C830, INIT-Handler @0x8010065C). Deshalb wird hier GENAU EIN
     * Frame gefahren, bevor das Bit geprueft wird. */
    int n_before = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active &&
            (g_actors[s].type == 0x10 || g_actors[s].type == 0x11 || g_actors[s].type == 0x16))
            n_before++;
    CHECK(n_before > 0, "PIN A: kein RE2-Zombie gespawnt");
    printf("  [A] %d Zombies gespawnt; state vor dem INIT-Tick = %d\n",
           n_before, (int)g_actors[1].state);
    frame();                                    /* INIT-Tick: state 0 -> re2z_init */

    int slot11 = -1, slot_other = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        if (e->type == 0x11 && slot11 < 0) slot11 = s;
        if ((e->type == 0x10 || e->type == 0x16) && slot_other < 0) slot_other = s;
    }
    CHECK(slot11 > 0, "PIN A: ROOM1140 muss einen Typ-0x11-Zombie liefern");
    CHECK(slot_other > 0, "PIN A: ROOM1140 muss einen Typ-0x10/0x16-Zombie liefern");
    if (slot11 < 0 || slot_other < 0) { printf("%d Pin(s) FEHLGESCHLAGEN\n", fails ? fails : 1);
                                        return 1; }
    re15_actor_t *z11 = &g_actors[slot11];
    re15_actor_t *zot = &g_actors[slot_other];
    printf("  [A] slot %d type=0x%02X +0x21A=0x%04X hp=%d | slot %d type=0x%02X +0x21A=0x%04X hp=%d\n",
           slot11, z11->type, (unsigned)z11->re2z_flags21a, (int)z11->hp,
           slot_other, zot->type, (unsigned)zot->re2z_flags21a, (int)zot->hp);
    CHECK((z11->re2z_flags21a & 0x8000u) != 0,
          "PIN A: Typ 0x11 muss nach dem INIT +0x21A|0x8000 tragen (`bne v1,17` @0x801008BC -> "
          "`ori 0x8000` @0x801008D0 / `sh 538` @0x801008D4)");
    CHECK((zot->re2z_flags21a & 0x8000u) == 0,
          "PIN A: Typ 0x%02X darf das Bit NICHT tragen (der bne @0x801008BC springt darueber); "
          "der zweite Produzent @0x801008D8-0x80100950 haengt am RE2-Global DAT_800cfb74&0x40 "
          "und ist im Port als OPEN gefuehrt", zot->type);

    /* --------------------------------------------------------------- PIN B: das Sub-Gate ---- */
    /* Beide Zombies in WALK (EXEC[1] = state 1 / sub 1, `sw 0x101` @0x80101E5C u.a.). sub_state_2
     * bleibt 0, damit die P0-Zeile des Executors (@0x80101A74-8C) den echten Walk-Clip setzt. */
    const int WIN = 300;
    re15_re2z_audio_hook(se_cap, bank_cap);

    /* B1: der 0x10/0x16-Zombie ALLEIN (der 0x11er wird stillgelegt) -> KEIN Frame-Wort-SE. */
    zot->re2z_f10e &= (uint16_t)~0x0080u;        /* kein Gore-Latch (@0x80106144) */
    zot->re2z_flags21a &= (uint16_t)~0x8000u;
    re15_ai_set_state_word(zot, 0x101); zot->sub_state_2 = 0; zot->anim_frame = 0;
    z11->active = 0;
    s_se_fw = s_se_other = 0;
    for (int t = 0; t < WIN; t++) frame();
    int fw_nogate = s_se_fw;
    printf("  [B] ohne Sub-Gate-Bit (slot %d, Clip %d): %d Frame-Wort-SEs in %d Frames (Soll 0)\n",
           slot_other, (int)zot->motion, fw_nogate, WIN);
    CHECK(fw_nogate == 0,
          "PIN B: ohne (+0x10E&0x80 || +0x21A&0x8000) darf KEIN Frame-Wort-SE feuern "
          "(@0x80101ce0-f8 / @0x80102400-18); gemessen %d", fw_nogate);

    /* B2: derselbe Zombie MIT gesetztem Bit -> der Pfad feuert. */
    zot->re2z_flags21a |= 0x8000u;               /* @0x801008D0-D4 */
    re15_ai_set_state_word(zot, 0x101); zot->sub_state_2 = 0; zot->anim_frame = 0;
    s_se_fw = s_se_other = 0;
    for (int t = 0; t < WIN; t++) frame();
    int fw_gate = s_se_fw;
    z11->active = 1;
    printf("  [B] mit  Sub-Gate-Bit (slot %d, Clip %d): %d Frame-Wort-SEs in %d Frames (Soll > 0)\n",
           slot_other, (int)zot->motion, fw_gate, WIN);
    CHECK(fw_gate > 0,
          "PIN B: mit gesetztem Sub-Gate-Bit MUSS der Frame-Wort-Pfad feuern "
          "(`jal 0x801016c8` @0x80101d34); gemessen 0");

    /* --------------------------------------------------------- PIN C: der Drittel-Takt ------ */
    /* C1: Tick-genau — jeder SE liegt auf einem Frame-Slot mit slot%3 == 2. */
    zot->active = 0;
    re15_ai_set_state_word(z11, 0x101); z11->sub_state_2 = 0; z11->anim_frame = 0;
    frame();                                     /* P0: Executor setzt den Walk-Clip */
    int walkclip = (int)z11->motion;
    const re15_emd_animation_t *A = emit_bank(z11);
    CHECK(A != NULL, "PIN C: RE2-Bank fuer Typ 0x11 fehlt");
    if (!A) { printf("%d Pin(s) FEHLGESCHLAGEN\n", fails ? fails : 1); return 1; }
    int fcount = (walkclip >= 0 && walkclip < A->clip_count) ? A->clips[walkclip].frame_count : 0;
    CHECK(fcount > 0, "PIN C: Walk-Clip %d hat keine Frames", walkclip);
    if (fcount <= 0) { printf("%d Pin(s) FEHLGESCHLAGEN\n", fails ? fails : 1); return 1; }
    int start_slot = (int)(z11->anim_frame % (uint32_t)fcount);

    int off_beat = 0, on_beat = 0, se_total = 0;
    for (int t = 0; t < WIN; t++) {
        int slot = (int)(z11->anim_frame % (uint32_t)fcount);
        s_se_fw = 0;
        frame();
        if (s_se_fw > 0) { se_total += s_se_fw; if ((slot % 3) == 2) on_beat++; else off_beat++; }
    }
    printf("  [C1] Clip %d (fc=%d, start slot %d): %d Frame-Wort-SEs, davon %d auf slot%%3==2, "
           "%d daneben\n", walkclip, fcount, start_slot, se_total, on_beat, off_beat);
    CHECK(off_beat == 0,
          "PIN C1: jeder Frame-Wort-SE muss auf einem Slot mit slot%%3==2 liegen "
          "(`lbu +0x14D` @0x80101d00, %%3-Magic @0x80101d04-24, `bne !=2` @0x80101d2c); "
          "%d Ausreisser", off_beat);
    int soll = expected_hits(A, walkclip, start_slot, WIN, 1);
    CHECK(se_total == soll,
          "PIN C1: Soll aus der RE2-Bank nachgerechnet = %d Frame-Wort-SEs, gemessen %d",
          soll, se_total);

    /* C2: DER DISKRIMINIERENDE FALL. Beim Standard-Walk-Clip liegen die SE-Frames zufaellig
     * beide auf slot%3==2 (EM010 Pair-1-Clip 0: Frames 20 und 62), der Takt ist dort also
     * unsichtbar. Deshalb wird hier ein Clip DERSELBEN Bank gesucht, dessen SE-Menge MIT und
     * OHNE Takt verschieden gross ist, und genau der wird gefahren. sub_state_2 bleibt auf 1,
     * damit die P0-Zeile den Messclip nicht ueberschreibt (nur P0 @0x80101A74-8C setzt +0x14C). */
    int cand = -1, cand_mit = 0, cand_ohne = 0;
    for (int cl = 0; cl < A->clip_count; cl++) {
        int m = expected_hits(A, cl, 0, WIN, 1);
        int o = expected_hits(A, cl, 0, WIN, 0);
        if (m > 0 && o > m) { cand = cl; cand_mit = m; cand_ohne = o; break; }
    }
    if (cand < 0) {
        printf("  [C2] kein Clip der Typ-0x11-Bank trennt Takt/kein-Takt — Pin entfaellt "
               "(Diagnose, kein FAIL)\n");
    } else {
        re15_ai_set_state_word(z11, 0x101);
        z11->sub_state_2 = 1;                    /* P0 uebersprungen -> Messclip bleibt stehen */
        /* Interferenzfrei messen (2026-09-05): Spieler ausser Leiter-Reichweite, sonst
         * committen D/E/Grab neue Clips (re2z_clip -> anim_frame=0) und der Reset
         * verschiebt den Takt gegen das expected_hits-Modell (gemessen: 7 statt 8 bei
         * cmis>0). Der Pin misst den DRITTEL-TAKT, nicht die Leiter. */
        pl->x = z11->x + 30000; pl->z = z11->z + 30000; pl->motion = 0;
        z11->motion = (int16_t)cand; z11->anim_frame = 0;
        z11->anim_frac = 0; z11->anim_blend_rate = 0x100;  /* kein Blend-Halte-Tick am Start —
                                                            * das Modell rechnet 1 Frame/Tick
                                                            * (+ Extra-Advance) ohne frac-Decay */
        z11->anim_freeze = 0; z11->anim_flags |= 0x04u;    /* Loop, wie der Walk-Clip */
        s_se_fw = s_se_other = 0;
        int cmis = 0;
        for (int t = 0; t < WIN; t++) {
            if ((int)z11->motion != cand) { z11->motion = (int16_t)cand; cmis++; }
            z11->anim_freeze = 0;
            int pre = (int)z11->anim_frame, se0 = s_se_fw;
            frame();
            if (getenv("C2_DBG")) {
                int fc2 = A->clips[cand].frame_count;
                int want = ((pre % fc2) % 3 == 2) ? 2 : 1;
                if ((int)z11->anim_frame - pre != want || (s_se_fw - se0) != 0)
                    fprintf(stderr, "  t%-3d pre=%d slot=%d post=%d (want +%d) se+=%d\n",
                            t, pre, pre % fc2, (int)z11->anim_frame, want, s_se_fw - se0);
            }
        }
        printf("  [C2] Clip %d: gemessen %d — Soll mit Takt %d, ohne Takt %d (Clip-Resets %d)\n",
               cand, s_se_fw, cand_mit, cand_ohne, cmis);
        CHECK(s_se_fw == cand_mit,
              "PIN C2: Drittel-Takt fehlt oder ist falsch — Clip %d muss %d Frame-Wort-SEs "
              "liefern (ohne den Takt waeren es %d); gemessen %d "
              "(@0x80101d00-2c / @0x80102420-4c)", cand, cand_mit, cand_ohne, s_se_fw);
    }

    re15_re2z_audio_hook(NULL, NULL);
    free(data);
    if (fails) { printf("\n%d Pin(s) FEHLGESCHLAGEN\n", fails); return 1; }
    printf("\nRE2-SFX-KADENZ: alle Pins gruen\n");
    return 0;
}
