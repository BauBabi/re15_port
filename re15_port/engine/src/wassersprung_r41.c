/*
 * wassersprung_r41.c — Runde 41 Spur A "wasser20b0", Punkt 2: Landungs-Spritzer des Spielers (Sprung von der
 * Kante ins Wasser) und die Wasser-Schritte der NPCs. Belege (Adressen) im Kopf von include/re15_wassersprung_r41.h,
 * Dossier analysis/befunde_runde41/A_wasser20b0.md (Punkt 2).
 */
#include <stdint.h>
#include <string.h>

#include "re15_wassersprung_r41.h"
#include "re15_wasser_r38.h"     /* Codes des Schritts, re15_wasser_r38_anker (Part-Weltmatrix * (0,100,0)) */
#include "re15_actor.h"
#include "re15_anim_select.h"    /* re15_actor_footstep (Bildwort-Bit 0x4000 / 0x1000)                      */
#include "re15_audio.h"          /* re15_audio_footstep                                                     */
#include "re15_enemy.h"          /* re15_enemy_find (NPC-Bank: Skelett + Clips)                             */
#include "re15_esp.h"            /* re15_esp_fx_spawn_rows = FUN_80019700                                   */
#include "re15_rdt.h"            /* re15_rdt_floor_sound = FUN_800437d4                                     */

static re15_wassersprung_r41_mess_t s_mess;

/* Folge-Plaetze der NPC-Schritte (Sub 3, Flags 0x07: Matrix je Takt neu @0x80019f54-a4). */
#define FOLGE_MAX 8
static struct { int platz; int aktor; int part; } s_folge[FOLGE_MAX];
static int s_init = 0;

static void init(void)
{
    if (s_init) return;
    s_init = 1;
    for (int i = 0; i < FOLGE_MAX; i++) { s_folge[i].platz = -1; s_folge[i].aktor = -1; s_folge[i].part = -1; }
}

const re15_wassersprung_r41_mess_t *re15_wassersprung_r41_mess(void) { return &s_mess; }
void re15_wassersprung_r41_reset(void)
{
    memset(&s_mess, 0, sizeof s_mess);
    s_mess.letzter_ton = -1;
    for (int i = 0; i < FOLGE_MAX; i++) { s_folge[i].platz = -1; s_folge[i].aktor = -1; s_folge[i].part = -1; }
    s_init = 1;
}

static int erster_freier_platz(void)
{
    for (int i = 0; i < RE15_ESP_FX_MAX; i++)
        if (!re15_esp_fx_get(i)) return i;
    return -1;
}

/* FUN_80019700(code, gier, Matrix, Versatz) mit bereits aufgeloester Weltlage. */
static int spawn(uint32_t code, int16_t gier, const int32_t w[3])
{
    const re15_esp_t *bank = re15_esp_room_bank();
    const uint8_t  id    = (uint8_t)(code >> 24);
    const uint8_t  sub   = (uint8_t)((code >> 16) & 0xffu);
    const uint16_t skala = (uint16_t)(code & 0xffffu);
    if (!bank || re15_esp_find_id(bank, id) < 0) return 0;
    /* Boden INT32_MAX: Zeilen B = 0 (keine Routine 12) -> keine Port-Bodenklemme (wie wasser_r38). */
    int n = re15_esp_fx_spawn_rows(bank, id, sub, skala, w[0], w[1], w[2], INT32_MAX, gier);
    if (n > 0) s_mess.plaetze += (unsigned)n;
    s_mess.letzter_anker[0] = w[0]; s_mess.letzter_anker[1] = w[1]; s_mess.letzter_anker[2] = w[2];
    return n;
}

/* ---- (a) Landung @0x80038730-d0 ------------------------------------------------------------------- */
int re15_wassersprung_r41_landung(const re15_actor_t *p, int16_t stempel, int32_t t_y)
{
    init();
    if (!p) return 0;
    /* `slt v1,v1,a0` @0x8003874c: Stempel (Spieler+0x88) < y (Spieler+0x38 = Bodenhoehe); `beq v1,zero` @0x80038758 */
    if (!((int32_t)stempel < p->y)) { s_mess.landung_trocken++; return 0; }
    /* a2 = Spieler+0x20 (Entity-Matrix, T = zuletzt gezeichnete Lage), a3 = (0,0,0) @0x8007428c */
    const int32_t w[3] = { p->x, t_y, p->z };
    int n = 0, k = 0;
    for (int s1 = 4; s1 >= 1; s1--) {                                  /* `ori s1,zero,0x4` @0x8003875c, --@0x800387d0 */
        int16_t s0 = (int16_t)((s1 - 2) << 8);                         /* `addiu s0,s0,-2` / `sll s0,s0,8` @0x80038788-90 */
        int16_t g1 = (int16_t)(p->rot_y + s0);                         /* `lh a1,106(v0)` + s0 @0x8003878c-98            */
        int16_t g2 = (int16_t)(g1 + 2048);                             /* `addiu a1,a1,2048` @0x800387c0                  */
        if (k < RE15_WASSERSPRUNG_R41_PLAETZE) s_mess.letzte_gier[k++] = g1;
        if (k < RE15_WASSERSPRUNG_R41_PLAETZE) s_mess.letzte_gier[k++] = g2;
        n += spawn(RE15_WASSERSPRUNG_R41_CODE, g1, w);                 /* jal 0x80019700 @0x80038794 */
        n += spawn(RE15_WASSERSPRUNG_R41_CODE, g2, w);                 /* jal 0x80019700 @0x800387bc */
    }
    s_mess.landungen++;
    return RE15_WASSERSPRUNG_R41_PLAETZE;
}

/* ---- (b) NPC-Schritt = FUN_80045630(a0, fuss) fuer Typ >= 0x40 ---------------------------------- */
static void folge_merken(int platz, int aktor, int part)
{
    init();
    int frei = -1;
    for (int i = 0; i < FOLGE_MAX; i++) {
        if (s_folge[i].platz == platz) { s_folge[i].aktor = aktor; s_folge[i].part = part; return; }
        if (frei < 0 && s_folge[i].platz < 0) frei = i;
    }
    if (frei < 0) frei = 0;
    s_folge[frei].platz = platz; s_folge[frei].aktor = aktor; s_folge[frei].part = part;
}

static int npc_anker(const re15_actor_t *e, int part, int32_t out[3])
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (b && b->ok)
        return re15_wasser_r38_anker(&b->skel, &b->anim, e, -1, e->anim_frame, part, out);
    out[0] = e->x; out[1] = e->y; out[2] = e->z;
    return -1;
}

int re15_wassersprung_r41_npc_schritt(re15_actor_t *e, int a0, int fuss, const void *rdt_v)
{
    init();
    if (!e || e->type < 0x40) return -1;
    const re15_rdt_t *rdt = (const re15_rdt_t *)rdt_v;
    /* Bodentyp an der Lage +0x34/+0x3c (`jal 0x800437d4` @0x8004566c) */
    const int boden = rdt ? re15_rdt_floor_sound(rdt, e->x, e->z) : 0;
    int s0 = boden, part = -1;
    uint32_t code = 0; int16_t gier = 0;
    if (boden & RE15_WASSER_R38_BIT_WASSER) {                     /* `andi v0,s0,0x80` @0x800456ac          */
        code = RE15_WASSER_R38_CODE_BODEN; part = fuss; gier = e->rot_y;   /* @0x800456b8-d0                 */
        s_mess.npc_boden++;
    } else if (e->water_y != 0) {                                 /* `lh v0,136(a1)` / beq @0x800456dc-e4   */
        s0 = boden - RE15_WASSER_R38_WATSATZ;                     /* `addiu s0,a0,-3` @0x800456ec           */
        code = RE15_WASSER_R38_CODE_STEMPEL; part = fuss - 1; gier = (int16_t)RE15_WASSER_R38_GIER_STEMPEL;
        s_mess.npc_stempel++;
    }
    if (code) {
        int32_t w[3];
        (void)npc_anker(e, part, w);
        int platz = erster_freier_platz();
        int n = spawn(code, gier, w);
        if (n > 0 && platz >= 0 && code == RE15_WASSER_R38_CODE_STEMPEL)
            folge_merken(platz, (int)(e - g_actors), part);
    }
    /* Tonindex: a0 + (Boden & 0x7f) @0x80045718-24, NPC-Zweig -8 @0x8004575c (kein Objekt-Override) */
    int idx = a0 + (s0 & 0x7f) - RE15_WASSERSPRUNG_R41_NPC_TON;
    if (idx >= 0) re15_audio_footstep(fuss, idx);
    s_mess.npc_schritte++;
    s_mess.letzter_ton = idx;
    return idx;
}

int re15_wassersprung_r41_npc_takt(re15_actor_t *e, int a0)
{
    if (!e || e->type < 0x40) return 0;
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok) return 0;
    /* Bildwort des gezeichneten Bilds (*(+0x168), `andi 0x4000` @0x80051288; Fuss 7 - 3*((Wort>>12)&1)) */
    int fuss = re15_actor_footstep(&b->anim, e, -1, e->anim_frame);
    if (!fuss) return 0;
    extern re15_rdt_t g_room_rdt; extern int g_room_rdt_ok;
    (void)re15_wassersprung_r41_npc_schritt(e, a0, fuss, g_room_rdt_ok ? &g_room_rdt : NULL);
    return 1;
}

void re15_wassersprung_r41_bildanfang(void)
{
    init();
    for (int i = 0; i < FOLGE_MAX; i++) {
        if (s_folge[i].platz < 0) continue;
        re15_esp_fx_t *f = (re15_esp_fx_t *)re15_esp_fx_get(s_folge[i].platz);
        if (!f || f->effect_id != 0x06 || f->sub_index != 0x03 || !(f->flags & 0x04)) {
            if (!f || f->effect_id != 0x06 || f->sub_index != 0x03) { s_folge[i].platz = -1; s_folge[i].aktor = -1; }
            continue;
        }
        if (s_folge[i].aktor <= 0 || s_folge[i].aktor >= RE15_ACTOR_MAX) continue;
        const re15_actor_t *e = &g_actors[s_folge[i].aktor];
        if (!e->active) { s_folge[i].platz = -1; continue; }
        int32_t a[3];
        if (npc_anker(e, s_folge[i].part, a) == 0) { f->x = a[0]; f->y = a[1]; f->z = a[2]; }
    }
}
