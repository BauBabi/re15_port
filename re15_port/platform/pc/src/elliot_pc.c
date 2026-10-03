/*
 * elliot_pc.c — Runde 35 Spur I, Nachbesserung 1 (Abnahme 0, M2): Elliot (NPC-Typ 0x47) ist ein
 * RAUM-Modell und faellt an jeder Grenze (raum / spielstart / spielende).
 *
 * ORIGINAL (RE1.5 PSX.EXE, Belege analysis/befunde_runde35/I_entladen.md "Nachbesserung 1"):
 *   Sce_em_set (Opcode 0x44, Handler @0x800420a0) legt das Modell JEDES gesetzten Typs in die
 *   Raum-Arena: `lw s1,-0x3884(s1)` @0x800422c4 = Arena-Kopf *0x800ac77c, `sw s1,0x7c(s0)`
 *   @0x800422dc, `addiu s1,s1,12` @0x800422f4, `jal 0x80022300` @0x80042328 (FUN_80022300 liest
 *   das EMD des Typs aus CDEMD0/1.EMS — Datei 0x26/0x27 — nach a3 = Arena und laedt sein TIM ins
 *   VRAM), danach `sw s1,-0x3884(at)` @0x80042554 (Kopf hinter das Modell). Der Raumlader setzt
 *   diesen Kopf bei JEDEM Raumladen auf die Basis zurueck (@0x80039738) -> das Modell ist weg.
 *   Anders der Spieler: FUN_800314b0 laedt sein PLD in den FESTEN Puffer 0x801bd814
 *   (`lui a1,0x801b / ori a1,a1,0xd814` @0x800314c8/cc), nicht in die Arena.
 *
 * PORT: das Aussehen bleibt die Port-Wahl der Phase 4.5.13-R23 (PL05 = PLD/ELLIOT.{MD1,EDD,EMR,TIM},
 * vorher fest beim Boot geladen); die LEBENSDAUER folgt jetzt dem Original:
 *   laden   beim Spawn eines Typ-0x47-Aktors (main.c, Roster-Vorladen nach scd_vm_tick — die
 *           Stelle, an der der Port jedes Sce_em_set-Modell nachlaedt; == @0x80042328),
 *           inkl. Dialog-Gesten-Overlay aus der gebundenen Raum-RBJ (re15_rbj_room; dieselbe
 *           Rechnung wie re15_apply_room_cinematic Schritt 2) und TIM in Slot 1;
 *   entladen an jeder Grenze (entladen_pc.c alles_entladen; Slot 1 faellt dort mit den Raum-Slots).
 * main.c behaelt seine Strukturen (die Zeichen-/Anim-Wege lesen sie per Adresse) und meldet sie
 * hier an; dieses Modul fuellt und leert sie.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_entladen.h"
#include "re15_md1.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re15_enemy.h"
#include "re15_room.h"
#include "asset_root_pc.h"

extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
extern void re15_npc_set_elliot_anim(const re15_emd_animation_t *a);

/* Angemeldete Ablage (main.c) */
static re15_md1_t           *s_md1 = NULL;
static int                  *s_md1_ok = NULL, *s_skel_ok = NULL;
static re15_emd_skeleton_t  *s_skel = NULL, *s_base_skel = NULL;
static re15_emd_animation_t *s_anim = NULL, *s_base_anim = NULL;

/* Eigene Puffer (MD1/EMR/EDD bleiben resident: die geparsten Strukturen zeigen hinein). */
static uint8_t *s_md1_buf = NULL, *s_edd_buf = NULL, *s_emr_buf = NULL;
static int      s_geladen = 0;
static unsigned s_gen = 0;

/* Overlay-Zwischenspeicher (re15_emd_animation_t traegt ~7 KB Bilder — nicht auf den Stack). */
static re15_emd_skeleton_t  s_ov_skel;
static re15_emd_animation_t s_ov_anim;

void re15_elliot_pc_anmelden(void *md1, int *md1_ok, void *skel, void *anim, int *skel_ok,
                             void *base_skel, void *base_anim)
{
    s_md1 = (re15_md1_t *)md1;               s_md1_ok = md1_ok;
    s_skel = (re15_emd_skeleton_t *)skel;    s_anim = (re15_emd_animation_t *)anim;
    s_skel_ok = skel_ok;
    s_base_skel = (re15_emd_skeleton_t *)base_skel;
    s_base_anim = (re15_emd_animation_t *)base_anim;
}

int re15_elliot_pc_belegt(unsigned *gen)
{
    if (gen) *gen = s_gen;
    return s_geladen;
}

void re15_elliot_pc_entladen(void)
{
    if (!s_geladen && !s_md1_buf && !s_edd_buf && !s_emr_buf) return;
    re15_npc_set_elliot_anim(NULL);          /* Executor: kein 0x47-Gestenbank mehr */
    if (s_md1)       memset(s_md1, 0, sizeof *s_md1);
    if (s_skel)      memset(s_skel, 0, sizeof *s_skel);
    if (s_anim)      memset(s_anim, 0, sizeof *s_anim);
    if (s_base_skel) memset(s_base_skel, 0, sizeof *s_base_skel);
    if (s_base_anim) memset(s_base_anim, 0, sizeof *s_base_anim);
    if (s_md1_ok)  *s_md1_ok = 0;
    if (s_skel_ok) *s_skel_ok = 0;
    free(s_md1_buf); free(s_edd_buf); free(s_emr_buf);
    s_md1_buf = s_edd_buf = s_emr_buf = NULL;
    s_geladen = 0;
    /* TIM-Slot 1 faellt mit den Raum-Slots (render_pc.c re15_render_pc_tim_slot_raum). */
}

int re15_elliot_pc_sicherstellen(void)
{
    if (s_geladen) return 1;
    if (!s_md1 || !s_md1_ok || !s_skel || !s_anim || !s_skel_ok || !s_base_skel || !s_base_anim)
        return 0;                            /* main.c hat (noch) nicht angemeldet */

    /* Dieselben Dateien und Schritte wie der fruehere Boot-Lader in main.c. */
    int md1_n = 0, edd_n = 0, emr_n = 0, tim_n = 0;
    s_md1_buf = re15_pc_read_any("PLD/ELLIOT.MD1", &md1_n);
    *s_md1_ok = (s_md1_buf && re15_md1_parse(s_md1_buf, md1_n, s_md1) == 0);
    s_edd_buf = re15_pc_read_any("PLD/ELLIOT.EDD", &edd_n);
    s_emr_buf = re15_pc_read_any("PLD/ELLIOT.EMR", &emr_n);
    *s_skel_ok = 0;
    if (s_edd_buf && s_emr_buf &&
        re15_emd_parse_animation(s_edd_buf, (size_t)edd_n, s_anim) == 0 &&
        re15_emd_parse_skeleton (s_emr_buf, (size_t)emr_n, s_skel) == 0) {
        *s_skel_ok = 1;
        fprintf(stderr, "[elliot] PL05 loaded: %d meshes, %d bones, %d clips (Raum %04X, Spawn 0x47)\n",
                s_md1->mesh_count, s_skel->bone_count, s_anim->clip_count, g_current_room_id);
    }
    /* Basis (vor dem Overlay) — der Raumwechsel-Weg in main.c setzt darauf zurueck. */
    *s_base_skel = *s_skel;
    *s_base_anim = *s_anim;
    /* Dialog-Gesten der Raum-RBJ (re15_apply_room_cinematic Schritt 2, enemy_common.c). */
    size_t rbj_n = 0;
    const uint8_t *rbj = re15_rbj_room(&rbj_n);
    if (*s_skel_ok && rbj && rbj_n > 0 &&
        re15_emd_parse_rbj(rbj, rbj_n, s_base_skel, &s_ov_skel, &s_ov_anim) == 0) {
        *s_skel = s_ov_skel;
        *s_anim = s_ov_anim;
        fprintf(stderr, "[elliot] Raum-RBJ-Overlay: %d clips\n", s_anim->clip_count);
    }
    re15_npc_set_elliot_anim(s_anim);

    uint8_t *tim_buf = re15_pc_read_any("PLD/ELLIOT.TIM", &tim_n);
    re15_tim_t tim;
    if (tim_buf && re15_tim_parse(tim_buf, tim_n, &tim) == 0) {
        re15_render_pc_upload_tim_slot(&tim, 1);
        fprintf(stderr, "[tim] elliot TIM in slot 1: %dx%d\n", tim.width, tim.height);
    } else {
        fprintf(stderr, "[tim] elliot TIM FAILED to load — NPC type 0x47 will use Leon's TIM\n");
    }
    free(tim_buf);   /* der Upload kopiert die Texel in die Textur */

    s_geladen = 1;
    s_gen = g_re15_entladen_gen;
    return *s_md1_ok && *s_skel_ok;
}
