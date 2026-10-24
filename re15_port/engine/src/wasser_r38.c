/*
 * wasser_r38.c — Runde 38 Spur N "wasser": FUN_80045630-Zwilling (Effekt-Zweig + Tonindex) fuer die
 * Spieler-Schritte. Belege und Zeitfolge: include/re15_wasser_r38.h, Dossier
 * analysis/befunde_runde38/N_wasser.md (B1-B10; B9 = DuckStation-GDB-Gegenprobe ROOM1260/2090).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_wasser_r38.h"
#include "re15_wassersprung_r41.h"   /* Runde 41 Spur A: NPC-Folge-Plaetze je Bild */
#include "re15_actor.h"
#include "re15_anim_select.h"   /* re15_actor_anim_select / re15_compute_actor_kf (Rueckfall-Pose) */
#include "re15_audio.h"         /* re15_audio_footstep = Ton-Teil von FUN_80045630                */
#include "re15_climb.h"         /* re15_climb_dbg_standing >= 0 <=> 0x800aca3c & 0x4000          */
#include "re15_emd.h"
#include "re15_engine.h"        /* g_engine.frame_count — nur Mess-Schiene                        */
#include "re15_esp.h"           /* re15_esp_fx_spawn_rows = FUN_80019700 (Start-Flags 3)          */
#include "re15_rdt.h"           /* re15_rdt_floor_sound = FUN_800437d4                            */
#include "re15_room.h"          /* g_current_room_id — nur Mess-Schiene                          */
#include "re15_skeleton.h"

/* ---- Bildanfang-Schnappschuss (B3/B7) --------------------------------------------------------- */
static re15_actor_t s_vor;            /* Spieler am Bildanfang = Zustand des zuletzt gezeichneten Bilds */
static int          s_vor_ok   = 0;
static int16_t      s_stempel  = 0;   /* +0x88, wie der Kommando-Dispatch @0x80031cb4 ihn liest        */
static int          s_objekt   = 0;   /* 0x800aca3c & 0x4000 (gesetzt @0x80031d24 im Vorbild)         */

/* ---- Part-Weltmatrizen des zuletzt gezeichneten Bilds (B6, Fuetterung durch den PC-Zeichner) --- */
static struct { int32_t r[9]; int32_t t[3]; } s_zp[RE15_EMD_MAX_BONES];
static int      s_zp_n     = 0;       /* gefuetterte Parts (0 = keine Fuetterung -> Rueckfall)        */
static uint32_t s_zp_bild  = 0;       /* g_engine.frame_count der Fuetterung                          */

/* ---- Spieler-Zustand des zuletzt gezeichneten Bilds (Lauf 3, dieselbe Fuetterung) ----------------
 * Der Schritt des Originals liegt in ALLEN vier Executoren VOR pos_advance (Echtzeit-Gehen @0x8003255c /
 * @0x800325ec, Echtzeit-Lauf @0x80032868 / @0x8003295c, Plc-Gehen @0x80030c3c / @0x80030c5c, Plc-Lauf
 * @0x80030e90 / @0x80030eb0) und liest Lage x +0x34 / z +0x3c (@0x80045664/68) und das Bildwort *(+0x168)
 * (anim_set des Vorbilds @0x8001f36c) — beides gehoert zum zuletzt gezeichneten Bild. Im Port laeuft die
 * Echtzeit-Bewegung (re15_player_tick) nach dem Bildanfang-Schnappschuss, die Skript-Bewegung
 * (re15_actor_step_all_walkers, Plc_dest) aber VOR re15_game_step (platform/pc/main.c) — dort lag der
 * Schnappschuss einen Laufschritt vor dem gezeichneten Bild (Dossier "Befund Skriptlauf"). */
static re15_actor_t s_gez;
static int          s_gez_ok   = 0;
static int          s_gez_raum = -1;  /* g_current_room_id der Fuetterung (Raumwechsel = ungueltig)   */

/* ---- FOLGE-Plaetze (Sub 3, Flags 0x07: Bit 2 = Matrix je Takt neu, @0x80019f54-a4) ------------ */
#define WASSER_FOLGE_MAX 8
static struct { int slot; int part; } s_folge[WASSER_FOLGE_MAX];

static re15_wasser_r38_mess_t s_mess;
static uint32_t s_letztes = 0xFFFFFFFFu;   /* Fusstakt: zuletzt gepruefte Bildnummer (Kantensperre) */
static int s_init = 0;
static void wasser_init(void) { if (!s_init) re15_wasser_r38_reset(); }

static FILE *wasser_log(void)
{
    static int init = 0;
    static FILE *f = NULL;
    if (!init) {
        init = 1;
        const char *p = getenv("RE15_WASSER_LOG");
        if (p && *p) f = fopen(p, "w");
    }
    return f;
}

const re15_wasser_r38_mess_t *re15_wasser_r38_mess(void) { return &s_mess; }
int16_t re15_wasser_r38_stempel(void) { return s_stempel; }   /* Runde 41 Spur A: Stempel des Vorbild-Scans fuer die Landung @0x8003873c */

void re15_wasser_r38_reset(void)
{
    s_init = 1;
    memset(&s_mess, 0, sizeof s_mess);
    memset(&s_vor, 0, sizeof s_vor);
    s_vor_ok = 0; s_stempel = 0; s_objekt = 0;
    s_zp_n = 0; s_zp_bild = 0; s_letztes = 0xFFFFFFFFu;
    memset(&s_gez, 0, sizeof s_gez); s_gez_ok = 0; s_gez_raum = -1;
    for (int i = 0; i < WASSER_FOLGE_MAX; i++) { s_folge[i].slot = -1; s_folge[i].part = -1; }
}

int re15_wasser_r38_a0(int motion, int skriptlauf)
{
    /* Port-Motion 100 = Lauf (W01-Clip 0), 105 = Gehen/Rueckwaerts/Drehen (Clip 5). Der Echtzeit-Lauf
     * (0x80032778) ruft FUN_80045630 mit a0 = 1 (`ori a0,zero,0x1` @0x80032850); der Plc-Lauf
     * 0x80030d28 mit a0 = 0 (`addu a0,zero,zero` @0x80030e18/@0x80030e7c). */
    return (motion == 100 && !skriptlauf) ? 1 : 0;
}

/* Zeichner-Fuetterung (platform/pc/main.c, im Knochen-11-Block): dieselbe Rechnung wie die Hand —
 * R = R_y(Gier) x Pose.rot, T = R_y(Gier) x Pose.trans + Lage (FUN_8001ef54 -> `jal 0x80022da0`
 * @0x8001ef80, a2 = Part+64). Wird in JEDEM Bild gerufen, unabhaengig von der Sichtbarkeit. */
void re15_wasser_r38_zeichner(const re15_skel_pose_t *poses, int n, const int32_t yaw_rot[9],
                              int32_t px, int32_t py, int32_t pz)
{
    wasser_init();
    /* Zustand des gezeichneten Bilds: Bewegung/Bild/Gier des Spielers, Lage = model_pos des Zeichners. */
    s_gez = g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_gez.x = px; s_gez.y = py; s_gez.z = pz;
    s_gez_ok = 1; s_gez_raum = (int)g_current_room_id;
    s_zp_bild = g_engine.frame_count;
    if (!poses || !yaw_rot || n <= 0) { s_zp_n = 0; return; }
    if (n > RE15_EMD_MAX_BONES) n = RE15_EMD_MAX_BONES;
    for (int p = 0; p < n; p++) {
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                int64_t s = 0;
                for (int k = 0; k < 3; k++) s += (int64_t)yaw_rot[r * 3 + k] * (int64_t)poses[p].rot[k * 3 + c];
                s_zp[p].r[r * 3 + c] = (int32_t)(s >> 12);
            }
            int64_t t = 0;
            for (int k = 0; k < 3; k++) t += (int64_t)yaw_rot[r * 3 + k] * (int64_t)poses[p].trans[k];
            s_zp[p].t[r] = (int32_t)(t >> 12) + (r == 0 ? px : (r == 1 ? py : pz));
        }
    }
    s_zp_n = n;
    s_zp_bild = g_engine.frame_count;
}

/* Fuetterung gilt fuer den Schritt dieses Takts: aus diesem Raum und hoechstens 2 Bilder alt. */
static int gez_gueltig(void)
{
    return s_gez_ok && s_gez_raum == (int)g_current_room_id &&
           (uint32_t)(g_engine.frame_count - s_zp_bild) <= 2u;
}

/* Zustand, den der Schritt sieht: das zuletzt gezeichnete Bild (Zeichner-Fuetterung), ohne Fuetterung
 * (PSX-Lauf, Sonden) der Bildanfang-Schnappschuss. quelle 1 = gezeichnet, 2 = Schnappschuss. */
static const re15_actor_t *schritt_zustand(int *quelle)
{
    if (gez_gueltig()) { *quelle = 1; return &s_gez; }
    *quelle = 2;
    return s_vor_ok ? &s_vor : &g_actors[RE15_ACTOR_SLOT_PLAYER];
}

/* ApplyMatrix(R, {0,100,0}) + T (FUN_80019e20 @0x8001a248-2a4 mit dem Versatz aus @0x800456c4-cc). */
static void anker_aus_matrix(const int32_t r[9], const int32_t t[3], int32_t out[3])
{
    out[0] = t[0] + ((r[1] * RE15_WASSER_R38_VERSATZ_Y) >> 12);
    out[1] = t[1] + ((r[4] * RE15_WASSER_R38_VERSATZ_Y) >> 12);
    out[2] = t[2] + ((r[7] * RE15_WASSER_R38_VERSATZ_Y) >> 12);
}

int re15_wasser_r38_anker(const void *skel_v, const void *anim_v, const void *aktor_v,
                          int clip_override, uint32_t cur, int part, int32_t out[3])
{
    const re15_emd_skeleton_t  *sk = (const re15_emd_skeleton_t *)skel_v;
    const re15_emd_animation_t *an = (const re15_emd_animation_t *)anim_v;
    const re15_actor_t         *a  = (const re15_actor_t *)aktor_v;
    if (!a) return -1;
    out[0] = a->x; out[1] = a->y; out[2] = a->z;
    if (!sk || !an || an->clip_count <= 0 || part < 0 || part >= sk->bone_count) return -1;
    int kf = re15_compute_actor_kf(an, sk, a, clip_override, cur);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor;
    g_anim_pose_actor = NULL;                       /* ABFRAGE: keinen Crossfade-Zustand veraendern */
    int rv = re15_skel_compute_pose(sk, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return -1;
    int32_t m[3];
    m[0] = poses[part].trans[0] + ((poses[part].rot[1] * RE15_WASSER_R38_VERSATZ_Y) >> 12);
    m[1] = poses[part].trans[1] + ((poses[part].rot[4] * RE15_WASSER_R38_VERSATZ_Y) >> 12);
    m[2] = poses[part].trans[2] + ((poses[part].rot[7] * RE15_WASSER_R38_VERSATZ_Y) >> 12);
    re15_skel_bone_to_world(m, a->rot_y, a->x, a->y, a->z, out);
    return 0;
}

/* Part-Anker fuer den Schritt: zuerst die Zeichner-Matrix des letzten Bilds (exakt Part+0x40), sonst
 * die Pose des Bildanfang-Schnappschusses (gleiche Bankwahl wie der Spieler-Zeichner). quelle 1/2. */
static int part_anker(const re15_game_ctx_t *c, int part, int32_t out[3], int *quelle)
{
    *quelle = 0;
    if (s_zp_n > 0 && part >= 0 && part < s_zp_n && gez_gueltig()) {
        anker_aus_matrix(s_zp[part].r, s_zp[part].t, out);
        *quelle = 1;
        return 0;
    }
    int zq = 0;
    const re15_actor_t *a = schritt_zustand(&zq);
    re15_anim_banks_t b;
    memset(&b, 0, sizeof b);
    b.def_skel  = c ? c->pl00_skel : NULL;  b.def_anim  = c ? c->pl00_anim : NULL;
    b.pl00_skel = b.def_skel;               b.pl00_anim = b.def_anim;
    b.pl00_ok   = (b.pl00_skel && b.pl00_anim) ? 1 : 0;
    b.w01_skel  = c ? c->w_skel : NULL;     b.w01_anim  = c ? c->w_anim : NULL;   /* wact = Zeichner-Bank */
    b.w01_ok    = (b.w01_skel && b.w01_anim) ? 1 : 0;
    re15_anim_view_t v;
    re15_actor_anim_select(a, 1, &b, &v);
    if (re15_wasser_r38_anker(v.skel, v.anim, a, v.clip_override, (uint32_t)a->anim_frame, part, out) != 0)
        return -1;
    *quelle = 2;
    return 0;
}

static void folge_merken(int slot, int part)
{
    int frei = -1;
    for (int i = 0; i < WASSER_FOLGE_MAX; i++) {
        if (s_folge[i].slot == slot) { s_folge[i].part = part; return; }
        if (frei < 0 && s_folge[i].slot < 0) frei = i;
    }
    if (frei < 0) frei = 0;                 /* voll: aeltesten Eintrag ueberschreiben */
    s_folge[frei].slot = slot; s_folge[frei].part = part;
}

/* Erster freier Pool-Platz (re15_esp_fx_spawn_ex vergibt ab 0 den ersten inaktiven — wie der
 * Spawner `sltiu v0,t3,0x60` / `lbu v0,108(t0)` / `beq` @0x8001978c-b0). */
static int erster_freier_platz(void)
{
    for (int i = 0; i < RE15_ESP_FX_MAX; i++)
        if (!re15_esp_fx_get(i)) return i;
    return -1;
}

static int wasser_spawn(const re15_game_ctx_t *c, uint32_t code, int part, int16_t gier, int32_t anker[3],
                        int *quelle)
{
    if (part_anker(c, part, anker, quelle) != 0) { /* out = Aktorlage */ }
    const uint8_t  id    = (uint8_t)(code >> 24);            /* `srl t8,a0,24` @0x80019728   */
    const uint8_t  sub   = (uint8_t)((code >> 16) & 0xffu);  /* `andi t7,v0,0xff` @0x80019730 */
    const uint16_t skala = (uint16_t)(code & 0xffffu);       /* `andi s1,a0,0xffff` @0x80019714 */
    int platz = erster_freier_platz();
    /* Boden INT32_MAX: Zeilen B = 0 (keine Routine 12) -> keine Port-Bodenklemme. */
    int n = re15_esp_fx_spawn_rows(re15_esp_room_bank(), id, sub, skala,
                                   anker[0], anker[1], anker[2], INT32_MAX, gier);
    if (n > 0 && platz >= 0) {
        const re15_esp_fx_t *f = re15_esp_fx_get(platz);
        if (f && f->effect_id == id && f->sub_index == sub) folge_merken(platz, part);
    }
    return n;
}

void re15_wasser_r38_bildanfang(const re15_game_ctx_t *c)
{
    wasser_init();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_vor    = *pl;                                    /* Lage/Gier/Pose des gezeichneten Bilds   */
    s_vor_ok = 1;
    s_stempel = pl->water_y;                           /* Stempel des Vorbild-Scans (@0x8001ce1c) */
    re15_wassersprung_r41_bildanfang();                /* Runde 41 Spur A: Folge-Plaetze der NPC-Schritte (@0x80019f54-a4) */
    pl->water_y = 0;                                   /* `sh zero,-0x3524(at)` @0x80031da4      */
    s_objekt = (re15_climb_dbg_standing() >= 0) ? 1 : 0;

    /* FOLGEN (@0x80019f54 `andi v0,v1,0x4` / @0x80019f60 `lw v0,116(a3)` / Kopie @0x80019f68-a4):
     * Anker der Folge-Plaetze aus der Part-Matrix des zuletzt gezeichneten Bilds. */
    int n = 0;
    for (int i = 0; i < WASSER_FOLGE_MAX; i++) {
        if (s_folge[i].slot < 0) continue;
        re15_esp_fx_t *f = (re15_esp_fx_t *)re15_esp_fx_get(s_folge[i].slot);
        if (!f || f->effect_id != 0x06 || f->sub_index != 0x03 || !(f->flags & 0x04)) {
            if (!f || f->effect_id != 0x06 || f->sub_index != 0x03) { s_folge[i].slot = -1; s_folge[i].part = -1; }
            continue;
        }
        int32_t a[3]; int q = 0;
        if (part_anker(c, s_folge[i].part, a, &q) == 0) { f->x = a[0]; f->y = a[1]; f->z = a[2]; n++; }
    }
    s_mess.folge = n;
}

int re15_wasser_r38_schritt(const re15_game_ctx_t *c, int a0, int fuss)
{
    wasser_init();
    int lq = 0;
    const re15_actor_t *v = schritt_zustand(&lq);
    /* Bodentyp an der Lage VOR der Bewegung dieses Bilds = Lage des gezeichneten Bilds (`lw a0,52(v0)` /
     * `lw a1,60(v0)` / `jal 0x800437d4` @0x80045664-6c; Schritt vor pos_advance in allen vier Executoren). */
    const int boden = (c && c->rdt) ? re15_rdt_floor_sound(c->rdt, v->x, v->z) : 0;
    int s0 = boden;
    int zweig = 0, plaetze = 0, part = -1, quelle = 0;
    int32_t anker[3] = { v->x, v->y, v->z };

    if (boden & RE15_WASSER_R38_BIT_WASSER) {                       /* @0x800456ac-b0 */
        part = fuss;                                                 /* `addiu a2,a2,64` @0x800456c0 */
        plaetze = wasser_spawn(c, RE15_WASSER_R38_CODE_BODEN, part,
                               v->rot_y,                             /* `lh a1,106(a1)` @0x800456d0 */
                               anker, &quelle);
        zweig = 1;
        s_mess.boden++;
    } else if (s_stempel != 0) {                                     /* `lh v0,136(a1)` / beq @0x800456dc-e4 */
        s0 = boden - RE15_WASSER_R38_WATSATZ;                        /* `addiu s0,a0,-3` @0x800456ec */
        part = fuss - 1;                                             /* `addiu a2,a2,-108` @0x800456f8 */
        plaetze = wasser_spawn(c, RE15_WASSER_R38_CODE_STEMPEL, part,
                               (int16_t)RE15_WASSER_R38_GIER_STEMPEL, /* @0x800456e8 */
                               anker, &quelle);
        zweig = 2;
        s_mess.stempel++;
    } else {
        s_mess.trocken++;
    }

    int idx = a0 + (s0 & 0x7f);                                      /* @0x80045718-24 */
    if (s_objekt) idx = RE15_WASSER_R38_OBJEKT_INDEX;                /* Typ 0 < 0x40: @0x80045770-90 */
    re15_audio_footstep(fuss, idx);

    if (zweig) {
        s_mess.anker[0] = anker[0]; s_mess.anker[1] = anker[1]; s_mess.anker[2] = anker[2];
        s_mess.part = part; s_mess.plaetze = plaetze;
    }
    s_mess.edt = idx;
    s_mess.lage[0] = v->x; s_mess.lage[1] = v->y; s_mess.lage[2] = v->z;
    s_mess.lage_quelle = lq;

    FILE *lf = wasser_log();
    if (lf) {
        const re15_actor_t *sv = s_vor_ok ? &s_vor : &g_actors[RE15_ACTOR_SLOT_PLAYER];
        fprintf(lf, "F%u room=%04X schritt fuss=%d a0=%d bild=%d akt=%d lage=(%d,%d,%d) zustand=%s vor=(%d,%d) gier=%d "
                    "boden=%#x stempel=%d objekt=%d "
                    "zweig=%s code=%#010x part=%d anker=(%d,%d,%d) quelle=%s plaetze=%d edt=%#x fx=%d\n",
                (unsigned)g_engine.frame_count, g_current_room_id, fuss, a0,
                (int)v->anim_frame, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].anim_frame,
                (int)v->x, (int)v->y, (int)v->z, lq == 1 ? "gezeichnet" : "schnappschuss", (int)sv->x, (int)sv->z,
                (int)v->rot_y, boden, (int)s_stempel, s_objekt,
                zweig == 1 ? "boden" : (zweig == 2 ? "stempel" : "trocken"),
                zweig == 1 ? RE15_WASSER_R38_CODE_BODEN : (zweig == 2 ? RE15_WASSER_R38_CODE_STEMPEL : 0u),
                part, (int)anker[0], (int)anker[1], (int)anker[2],
                quelle == 1 ? "zeichner" : (quelle == 2 ? "schnappschuss" : "-"),
                plaetze, idx, re15_esp_fx_count());
        fflush(lf);
    }
    return idx;
}

/* FUSSTAKT — Schritt-Erkennung wie das Original (Lauf 2, gemessen + disassembliert, Dossier B9):
 * der Geh-/Lauf-Executor des Bilds N liest das Bildwort *(+0x168) (`lw v0,-13380(v0)` = 0x800acbbc,
 * `lw v1,0(v0)`, `andi v0,v1,0x4000` @0x8003252c-3c), das der anim_set des VORBILDS gesetzt hat
 * (`sw a2,360(t0)` @0x8001f36c auf das Bild +0x95 VOR dessen Vorschub `addiu`/`sb v0,149` @0x8001f618/1c)
 * — also das Bild, das N-1 gezeichnet hat, und dessen Pose tragen auch die Part-Matrizen (+0x40).
 * DuckStation-GDB ROOM1260: Schritt Part 7 bei +0x95 = 8 mit Bildwort[7] = 0x0000606d, Part 4 bei
 * +0x95 = 23 mit Bildwort[22] = 0x0000507c. Der Port las vorher das Bild NACH dem Vorschub dieses
 * Takts (re15_player_tick @player_common.c anim_frame++) = einen Takt zu frueh, mit der Pose des
 * Vorbilds (Anker ~50 Einheiten neben dem Original). Hier: Bild + Pose des gezeichneten Bilds (Zeichner-
 * Fuetterung; ohne sie der Bildanfang-Schnappschuss, Lauf 3), Executor-Bedingung (Gehen 105 / Laufen 100)
 * aus dem laufenden Takt. */
void re15_wasser_r38_fusstakt(const re15_game_ctx_t *c)
{
    wasser_init();
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int lq = 0;
    const re15_actor_t *v  = schritt_zustand(&lq);
    if (c && c->rdt_ok && c->w01_anim && (pl->motion == 105 || pl->motion == 100) &&
        (v->motion == 105 || v->motion == 100)) {
        uint32_t bild = (uint32_t)v->anim_frame;
        if (bild != s_letztes) {                          /* je Bild hoechstens ein Schritt (60-fps-Halbtakt) */
            s_letztes = bild;
            int clip = (v->motion == 105) ? 5 : 0;         /* Gehen = W01 Clip 5, Laufen = Clip 0 */
            int fuss = re15_actor_footstep(c->w01_anim, v, clip, bild);
            if (fuss) (void)re15_wasser_r38_schritt(c, re15_wasser_r38_a0(pl->motion, pl->walk_active), fuss);
        }
    } else {
        s_letztes = 0xFFFFFFFFu;                           /* kein Geh-/Lauf-Executor in diesem Takt */
    }
}
