/*
 * RE1.5 Rebuilt — Runde 41 Spur AN: Annette-Szene im hinteren Zugwagen ROOM5090 (Leon).
 *
 * Kopf, Herleitung und Abgrenzung: include/re15_annette_r41.h, Dossier analysis/befunde_runde41/AN_annette.md.
 *
 * ⛔ PORT-ERGAENZUNG nach Nutzerentscheid, KEIN Auslieferungscode. Die RE2-Szene (Claire-Disc) ist im Repo nur als Ton
 * vorhanden (Dossier 1.2): belegt sind Annettes Satz und Schrei. Raum, Kameras, Lagen und Bildzahlen der Port-Szene sind
 * PORT-WAHL nach 80 % #145 (ROOM5090 Cut 11, Form in Pose Bild 2 auf ihrer Bild-2-Lage) und nach den Clipdaten der
 * Riesenform (Stufe 3, Hieb links Clip 4 mit Trefferfenster Bild 14-17 aus G4-Clip 4); jede solche Zahl ist so markiert.
 * Kein Asset-Patch: die RDT bleibt byte-true, die Szene ist Bytecode aus ORIGINAL-Opcodes ueber die vorhandene VM.
 */
#include "re15_annette_r41.h"

#include <string.h>
#include <stdlib.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_damage.h"        /* re15_enemy_bone_world_pos, re15_engine_rand8 */
#include "re15_esp.h"           /* re15_esp_fx_spawn_ex, re15_esp_room_bank (Blut wie bb_blut_spawn) */
#include "re15_msg.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_engine.h"

#include "gen/annette_r41_texte.inc"   /* k_an_msg40..42 (tools/r41_annette/texte_bauen.py) */

int re15_r38_birkin_anim(re15_actor_t *e);        /* enemy_ai_common.c: Clip-Vorschub mit der Bank des Typs */
int re15_r38_birkin_fc(const re15_actor_t *e);    /* enemy_ai_common.c: Clip-Laenge */
int re15_bb_bank_da(const re15_actor_t *e);       /* birkin_bahnsteig.c: 0x30 traegt die Riesenform */

/* ---- Lagen und Zeiten (PORT-WAHL, Dossier Abschnitt 3) -------------------------------------------------------------
 * Form waehrend des Satzes: Bild-2-Lage der Formspur (form/FORMBLATT2.md Z. 223 "Pose 2, Bild 2: Lage (776, 0, -23713),
 * Gier 3917") mit dem Drohen-Clip 10, dessen Bild 45-52 Pose Bild 2 ist (STUFE3.md Abschnitt 3) = 80 % #145 in Cut 11.
 * Annette: Start und Ende auf einer Linie 20 Grad von der Form (Abstand 3450 / 3760), so dass sie in Cut 11 rechts frei vor der
 * Form steht (Projektion durch die RID-Kamera Cut 11 @0x1C0, tools/form_kamera.py: Kopf um (500,190) von 640x480) und das Ende in
 * Reichweite des Hiebs liegt: Hand links (Teil 13) im Trefferfenster Bild 14-16 lokal x 3685..3864 (Clip 4, gemessen aus
 * BAHNSTEIG_FORM.EMD); der Kopf der Form reicht lokal bis x 3245 (Clip 26 Bild 0, Knochen 23), der Start liegt davor.
 * Abweichung von #145 (Frau links neben der Form, Abstand rund 1600): links liegt die Suedwand, und kein Clip der Form trifft
 * naeher als rund 3700 (Dossier Abschnitt 3). */
#define AN_FORM_X          776
#define AN_FORM_Z        (-23713)
#define AN_FORM_GIER       3917
#define AN_START_X         4018
#define AN_START_Z       (-22533)
#define AN_ENDE_X          4309
#define AN_ENDE_Z        (-22427)
#define AN_WEICHEN_SCHRITT   25     /* Einheiten je Bild beim Zurueckweichen */
#define AN_DREH_SCHRITT    0x40     /* Gier je Bild, mit der sich die Form zu ihr dreht */
/* Clips der Riesenform (Stufe 3, BB_C_* in birkin_bahnsteig.c) */
#define AN_FC_DROHEN         10
#define AN_FC_STEHEN          2
#define AN_FC_HIEB            4     /* Hieb links, Teil 13 */
#define AN_HIEB_TREFFER      14     /* erstes Trefferbild, EDD-Bit 16 Bild 14-17 (G4-Clip 4, STUFE3.md Abschnitt 3) */
#define AN_HAND_LINKS        13
#define AN_TON_BIT   0x08000000u    /* Bild-Ton: Bit 27, Id = Wort >> 28 (FUN_80016028, birkin_bahnsteig.c bb_bild_ton) */
#define AN_TON_BANK          24     /* G4-Bank, wie bb_se */
/* Clips der Frau (0x4D-Bank, 24 Clips, Kontaktblatt AN_belege) */
#define AN_FA_STEHEN         22     /* Stehen mit Handgeste beim Sprechen (Clip 0/1 sind Gehen, Clip 2 Klettern; Kontaktblatt) */
#define AN_FA_WEICHEN         4     /* zurueckweichen, Arme gehoben */
#define AN_FA_STURZ           6     /* faellt zu Boden (Bild 12 = am Boden) */
#define AN_FA_STURZ_BODEN    12
#define AN_FLUG_BILDER       12
#define AN_FLUG_TEMPO        15     /* Anfangstempo des Wegschleuderns, Einheiten je Bild, Abnahme 7/8 je Bild */
#define AN_FLUG_HOEHE       300     /* Bogenhoehe */
#define AN_KIPP_BILDER        8
#define AN_KIPP_WINKEL     (-683)   /* 60 Grad nach hinten (Kontaktblatt liegen.png) */
#define AN_KIPP_HEBEN      (-480)   /* Clip 6 Bild 12 um -60 Grad gekippt reicht bis y +482 unter den Ursprung (Gerippe gemessen) */
/* Wagen C, innen (Annette-Spawn z -25300 @0x0122A, Westende x -5683 = SCA [21]/[25], Dossier G5A 1.6) */
#define AN_WAGEN_X_MIN    (-5000)
#define AN_WAGEN_X_MAX     20000
#define AN_WAGEN_Z_MIN    (-25300)
#define AN_WAGEN_Z_MAX    (-22300)

/* ---- Programm ------------------------------------------------------------------------------------------------------
 * Opcode-Formen mit Vorbild (Laengen = s_opcode_sizes, scd_vm.c):
 *   Set `22 bb ii vv`               ROOM5090 sub04 @0x012F2/@0x012F6 (Sperre (2,7), Balken (1,27))
 *   Cut_chg `29 cc`                 ROOM5090 sub04 @0x0130E
 *   Work_set `2e kk ii` + Nop       ROOM5090 sub04 @0x0131C
 *   Plc_dest `40 00 09 20 x z`      ROOM5090 sub04 @0x01320 (Modus 9 = drehen, Ankunftsbit 0x20)
 *   Ankunfts-Poll `11 00 08 00 02 00 12 04 21 05 bb 00`   ROOM5090 sub04 @0x01328..@0x01330
 *   Sleep `09 0a nn nn`             ROOM5090 sub04 @0x0133E
 *   Message_on `2b id 00 00`        ROOM6030 sub02 @0x00DC2 (Maske 0 = Untertitel)
 *   Evt_exec `04 ff 18 ss`          ROOM5090 sub01 @0x01296
 * Zeiten: Satzdauer 118 Bilder = RE2-Aufnahme 3,92 s (Anette/VOICE16.XAS[9]_3.wav, Dossier 1.2); die uebrigen PORT-WAHL. */
#define LE16(v)          (uint8_t)((uint16_t)(v) & 0xff), (uint8_t)(((uint16_t)(v) >> 8) & 0xff)
#define OP_SET(b,i,v)    0x22, (b), (i), (v)
#define OP_CUT(c)        0x29, (c)
#define OP_WORK(k,i)     0x2e, (k), (i), 0x00
#define OP_DREHEN(x,z)   0x40, 0x00, 0x09, 0x20, LE16(x), LE16(z)
#define OP_WARTE(bit)    0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, (bit), 0x00
#define OP_SLEEP(n)      0x09, 0x0a, LE16(n)
#define OP_MSG(id)       0x2b, (id), 0x00, 0x00
#define OP_KAMPF         0x04, 0xff, 0x18, RE15_AN_SUB_KAMPF
#define OP_END           0x01, 0x00

static const uint8_t k_prog[] = {
    OP_SET(2, 7, 1), OP_SET(1, 27, 1),                    /* Sperre + Balken (sub04 @0x012F2/@0x012F6) */
    OP_CUT(11),                                           /* Cut 11 = 80 % #145 */
    OP_WORK(1, 0), OP_DREHEN(AN_FORM_X, AN_FORM_Z), OP_WARTE(0x20),   /* Leon dreht sich zur Form (sub04 @0x01320) */
    OP_SLEEP(20),
    OP_MSG(RE15_AN_MSG_SATZ),                             /* "Annette: W-William! You're alive!" */
    OP_SLEEP(118),
    OP_SET(5, RE15_AN_SIG_SCHLAG, 1),                     /* Takt: sie weicht, er dreht sich und schlaegt */
    OP_WARTE(RE15_AN_SIG_TREFFER),
    OP_MSG(RE15_AN_MSG_SCHREI),                           /* Schrei */
    OP_WARTE(RE15_AN_SIG_LIEGT),
    OP_SLEEP(20),
    OP_CUT(12),                                           /* Leon im Bild (Kampfkamera, sub04 @0x01342) */
    OP_MSG(RE15_AN_MSG_LEON),                             /* "Leon: Annette!" */
    OP_SLEEP(60),
    OP_KAMPF,                                             /* Kampfstart wie bisher (sub04) */
    OP_END
};

static const struct { uint8_t id; const uint8_t *b; uint8_t n; } k_meldungen[] = {
    { RE15_AN_MSG_SATZ,   k_an_msg40, (uint8_t)sizeof k_an_msg40 },
    { RE15_AN_MSG_SCHREI, k_an_msg41, (uint8_t)sizeof k_an_msg41 },
    { RE15_AN_MSG_LEON,   k_an_msg42, (uint8_t)sizeof k_an_msg42 },
};
#define AN_N_MELDUNGEN ((int)(sizeof k_meldungen / sizeof k_meldungen[0]))

/* ---- Zustand ------------------------------------------------------------------------------------------------------ */
static struct {
    int phase, t;
    int form, frau;               /* Slots, -1 = keiner */
    int32_t vx, vz;               /* Flug, Einheiten*16 je Bild */
    int32_t fx0, fz0;             /* Flugbeginn */
    int blut;                     /* Blutspawns (Testzaehler) */
    int blut_textur;              /* Textur mit Blut geladen */
    unsigned raum;
} s_an = { RE15_AN_AUS, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0 };

static void (*s_modell_fn)(uint8_t) = 0;
static void (*s_se_fn)(int, int) = 0;
static void (*s_bank_fn)(int) = 0;
void re15_annette_r41_modell_hook(void (*fn)(uint8_t type)) { s_modell_fn = fn; }
void re15_annette_r41_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int)) { s_se_fn = se_fn; s_bank_fn = bank_fn; }

void re15_annette_r41_reset(void)
{
    s_an.phase = RE15_AN_AUS; s_an.t = 0; s_an.form = s_an.frau = -1;
    s_an.vx = s_an.vz = 0; s_an.blut = 0; s_an.blut_textur = 0;
}

int re15_annette_r41_phase(void) { return s_an.phase; }
int re15_annette_r41_slot(int form) { return form ? s_an.form : s_an.frau; }
int re15_annette_r41_blut(void) { return s_an.blut; }

const uint8_t *re15_annette_r41_programm(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_prog;
    return k_prog;
}

const uint8_t *re15_annette_r41_meldung(int msg_id, int *out_len)
{
    for (int i = 0; i < AN_N_MELDUNGEN; i++)
        if ((int)k_meldungen[i].id == msg_id) { if (out_len) *out_len = k_meldungen[i].n; return k_meldungen[i].b; }
    if (out_len) *out_len = 0;
    return NULL;
}

static void protokoll(const char *was)
{
#ifdef RE15_PLATFORM_PC
    const re15_actor_t *f = s_an.form >= 0 ? &g_actors[s_an.form] : NULL;
    const re15_actor_t *a = s_an.frau >= 0 ? &g_actors[s_an.frau] : NULL;
    fprintf(stderr, "[annette] F%u %s: Phase %d, Form %s(%d,%d) Gier %d Clip %d/%d, Annette %s(%d,%d,%d) Clip %d/%d, Cut %d\n",
            (unsigned)g_engine.frame_count, was, s_an.phase,
            f ? "" : "-", f ? (int)f->x : 0, f ? (int)f->z : 0, f ? (int)f->rot_y : 0, f ? (int)f->motion : -1,
            f ? (int)f->anim_frame : -1,
            a ? "" : "-", a ? (int)a->x : 0, a ? (int)a->y : 0, a ? (int)a->z : 0, a ? (int)a->motion : -1,
            a ? (int)a->anim_frame : -1, (int)g_scd.work_vars[0x0A]);
#else
    (void)was;
#endif
}

/* ---- Bedingungen ---------------------------------------------------------------------------------------------------- */
static int raum_ist(uint16_t room_id) { return room_id == RE15_AN_RAUM; }

static int annette_steht_da(void)
{
    /* PORT-WAHL nach Nutzerentscheid: im Wagen erst nach dem Tor (3,89); nach dem Kampfstart (3,42) nicht mehr (sie ist dann tot). */
    return re15_game_flag_get(RE15_AN_TOR_BANK, RE15_AN_TOR_BIT) && !re15_game_flag_get(RE15_AN_KAMPF_BANK, RE15_AN_KAMPF_BIT);
}

int re15_annette_r41_spawn_weg(uint16_t room_id, uint8_t type)
{
    if (!raum_ist(room_id) || type != RE15_AN_TYP) return 0;
    const int weg = !annette_steht_da();
#ifdef RE15_PLATFORM_PC
    if (weg) fprintf(stderr, "[annette] ROOM%04X: Frau im Kittel (0x4D, @0x0122A) verworfen: (3,89)=%d (3,42)=%d\n",
                     room_id, re15_game_flag_get(3, 89), re15_game_flag_get(3, 42));
#endif
    return weg;
}

static int slot_vom_typ(uint8_t type)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++)
        if (g_actors[i].active && g_actors[i].type == type) return i;
    return -1;
}

static int16_t blick(const re15_actor_t *e, int32_t tx, int32_t tz)
{
    /* dieselbe Peilung wie re15_enemy_steer_point / birkin_formen.c bf_blick */
    return (int16_t)(((int)re15_atan2_q12(tz - e->z, tx - e->x) - 0x400) & 0x0fff);
}

static void clip(re15_actor_t *e, int c)
{
    e->motion = (int16_t)c; e->anim_frame = 0; e->anim_frac = 7;
    e->anim_flags &= (uint16_t)~0x80u;
}

/* Bild vorruecken, am Ende das letzte Bild halten; Bildton der Form wie bb_bild_ton. Rueckgabe 1 am Clip-Ende. */
static int vor(re15_actor_t *e, int halten)
{
    if (e->type == RE15_AN_FORM_TYP) {
        const re15_enemy_bank_t *b = re15_enemy_find(e->type);
        if (b && b->ok) {
            const uint32_t w = re15_emd_get_frame_entry(&b->anim, (int)e->motion, (int)e->anim_frame);
            if ((w & AN_TON_BIT) && s_se_fn) { if (s_bank_fn) s_bank_fn(AN_TON_BANK); s_se_fn((int)(w >> 28), 0); }
        }
    }
    if (halten && e->anim_frame + 1 >= re15_r38_birkin_fc(e)) return 1;
    return re15_r38_birkin_anim(e);
}

static void blut(const int32_t p[3], int16_t winkel, uint8_t sub)
{
    /* wie bb_blut_spawn (birkin_bahnsteig.c): Effekt 0 = Blut, Skala 0x17d0 + (rng & 0xff) * 8 @0x80107430-34 */
    const uint16_t skala = (uint16_t)(0x17d0 + (re15_engine_rand8() & 0xffu) * 8u);
    s_an.blut++;
    re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0u, sub, skala, p[0], p[1], p[2], winkel);
}

static void meldungen_einsetzen(void)
{
    for (int i = 0; i < AN_N_MELDUNGEN; i++) {
        re15_msg_install_text(k_meldungen[i].id, k_meldungen[i].b, k_meldungen[i].n);
        int d = re15_msg_compute_duration(k_meldungen[i].b, k_meldungen[i].n, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(k_meldungen[i].id, d);
    }
}

void re15_annette_r41_install(uint16_t room_id)
{
    if (s_an.raum != room_id) { re15_annette_r41_reset(); s_an.raum = room_id; }
    if (!raum_ist(room_id)) return;
    meldungen_einsetzen();
}

static int32_t klemme(int32_t v, int32_t lo, int32_t hi) { return v < lo ? lo : v > hi ? hi : v; }

/* Start der Szene: Figuren auf ihre Lagen (PORT-WAHL, s. oben). */
static void figuren_stellen(void)
{
    re15_actor_t *f = &g_actors[s_an.form], *a = &g_actors[s_an.frau];
    f->x = AN_FORM_X; f->y = 0; f->z = AN_FORM_Z; f->rot_y = AN_FORM_GIER;
    f->no_draw = 0;
    clip(f, AN_FC_DROHEN);
    a->x = AN_START_X; a->y = 0; a->z = AN_START_Z; a->rot_x = a->rot_z = 0;
    a->rot_y = blick(a, f->x, f->z);
    clip(a, AN_FA_STEHEN);
}

const uint8_t *re15_annette_r41_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *target)
{
    if (!raum_ist(room_id) || sub_id != RE15_AN_SUB_KAMPF) return target;
    if (s_an.phase == RE15_AN_LEICHE) {                  /* das eigene Programm ruft den Kampfstart: Form frei */
        s_an.phase = RE15_AN_FREI;
        protokoll("Kampfstart (sub04), Form frei");
        return target;
    }
    if (s_an.phase != RE15_AN_AUS || !annette_steht_da()) return target;
    s_an.form = slot_vom_typ(RE15_AN_FORM_TYP);
    s_an.frau = slot_vom_typ(RE15_AN_TYP);
    if (s_an.form < 0 || s_an.frau < 0 || !re15_bb_bank_da(&g_actors[s_an.form])) {
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[annette] keine Szene: Form-Slot %d (Riesenform %d), Annette-Slot %d\n", s_an.form,
                s_an.form >= 0 ? re15_bb_bank_da(&g_actors[s_an.form]) : 0, s_an.frau);
#endif
        s_an.form = s_an.frau = -1;
        return target;
    }
    re15_game_flag_set(5, RE15_AN_SIG_SCHLAG, 0);
    re15_game_flag_set(5, RE15_AN_SIG_TREFFER, 0);
    re15_game_flag_set(5, RE15_AN_SIG_LIEGT, 0);
    s_an.phase = RE15_AN_DROHEN; s_an.t = 0; s_an.blut = 0;
    figuren_stellen();
    protokoll("Szenenstart statt sub04");
    return k_prog;
}

int re15_annette_r41_besitzt(int slot)
{
    if (s_an.phase == RE15_AN_AUS || slot < 0) return 0;
    if (slot == s_an.frau) return 1;                     /* lebend in der Szene, danach als Leiche */
    return slot == s_an.form && s_an.phase < RE15_AN_FREI;
}

int re15_annette_r41_volle_drehung(const void *actor)
{
    return s_an.frau >= 0 && actor == (const void *)&g_actors[s_an.frau] && s_an.phase >= RE15_AN_KIPPEN;
}

const char *re15_annette_r41_emd_datei(uint8_t type)
{
    if (type != RE15_AN_TYP) return NULL;                /* 0x4D steht nur in 5090/6000/6030 (Leon) */
    return s_an.blut_textur ? "ANNETTE_4D_BLUT.EMD" : "ANNETTE_4D.EMD";
}

/* Hieb ausrichten: die Gier waehlen, bei der die linke Hand im Trefferbild ihr am naechsten kommt (gemessen am Gerippe). */
static void hieb_ausrichten(re15_actor_t *f, const re15_actor_t *a)
{
    const int16_t basis = blick(f, a->x, a->z);
    const int16_t m0 = f->motion; const int32_t b0 = f->anim_frame;
    int16_t beste = basis; int64_t best = -1;
    f->motion = AN_FC_HIEB; f->anim_frame = AN_HIEB_TREFFER + 1;
    for (int d = -0x100; d <= 0x100; d += 0x10) {
        int32_t h[3];
        f->rot_y = (int16_t)((basis + d) & 0x0fff);
        re15_enemy_bone_world_pos(f, AN_HAND_LINKS, h);
        const int64_t dx = h[0] - a->x, dz = h[2] - a->z, q = dx * dx + dz * dz;
        if (best < 0 || q < best) { best = q; beste = f->rot_y; }
    }
    f->rot_y = beste; f->motion = m0; f->anim_frame = b0;
}

void re15_annette_r41_tick(void)
{
    if (s_an.phase == RE15_AN_AUS) return;
    if (!raum_ist((uint16_t)g_current_room_id)) { re15_annette_r41_reset(); return; }
    if (s_an.form < 0 || s_an.frau < 0) return;
    re15_actor_t *f = &g_actors[s_an.form], *a = &g_actors[s_an.frau];
    if (!f->active || !a->active) { s_an.phase = RE15_AN_FREI; return; }
    s_an.t++;
    switch (s_an.phase) {
    case RE15_AN_DROHEN:
        if (f->motion == AN_FC_DROHEN) { if (vor(f, 1)) clip(f, AN_FC_STEHEN); }
        else vor(f, 0);
        vor(a, 0);
        a->rot_y = blick(a, f->x, f->z);
        if (re15_game_flag_get(5, RE15_AN_SIG_SCHLAG)) {
            re15_game_flag_set(5, RE15_AN_SIG_SCHLAG, 0);
            s_an.phase = RE15_AN_WEICHEN; s_an.t = 0;
            if (f->motion != AN_FC_STEHEN) clip(f, AN_FC_STEHEN);
            clip(a, AN_FA_WEICHEN);
            protokoll("sie weicht, er dreht sich");
        }
        break;
    case RE15_AN_WEICHEN: {
        vor(f, 0); vor(a, 0);
        const int32_t dx = AN_ENDE_X - a->x, dz = AN_ENDE_Z - a->z;
        const int32_t rest = abs(dx) + abs(dz);
        if (rest > AN_WEICHEN_SCHRITT) {
            a->x += (int32_t)((int64_t)dx * AN_WEICHEN_SCHRITT / rest);
            a->z += (int32_t)((int64_t)dz * AN_WEICHEN_SCHRITT / rest);
        } else { a->x = AN_ENDE_X; a->z = AN_ENDE_Z; }
        a->rot_y = blick(a, f->x, f->z);                 /* rueckwaerts, das Gesicht zu ihm */
        const int16_t ziel = blick(f, a->x, a->z);
        int d = ((int)ziel - (int)f->rot_y) & 0x0fff; if (d >= 0x800) d -= 0x1000;
        if (d > AN_DREH_SCHRITT) d = AN_DREH_SCHRITT; else if (d < -AN_DREH_SCHRITT) d = -AN_DREH_SCHRITT;
        f->rot_y = (int16_t)((f->rot_y + d) & 0x0fff);
        if (rest <= AN_WEICHEN_SCHRITT && abs(d) < AN_DREH_SCHRITT) {     /* angekommen und ausgerichtet */
            hieb_ausrichten(f, a);
            clip(f, AN_FC_HIEB);
            s_an.phase = RE15_AN_HIEB; s_an.t = 0;
            protokoll("Hieb");
        }
        break;
    }
    case RE15_AN_HIEB:
        vor(a, 0);
        if ((int)f->anim_frame == AN_HIEB_TREFFER) {
            int32_t h[3], p[3];
            const int16_t w = blick(f, a->x, a->z);
            re15_enemy_bone_world_pos(f, AN_HAND_LINKS, h);
            p[0] = a->x; p[1] = -1500; p[2] = a->z;      /* Brusthoehe der Frau (Modellhoehe 2785, Ansichtsblatt) */
            blut(p, w, 1u); blut(p, w, 0u); blut(h, w, 0u); blut(h, w, 0u);
            /* Wegschleudern: weg von der Form, dazu die Richtung, in die die Hand zieht (Hand -> Frau) */
            int32_t ux = a->x - f->x, uz = a->z - f->z;
            int32_t hx = a->x - h[0], hz = a->z - h[2];
            int32_t lx = ux * 2 + hx, lz = uz * 2 + hz;
            int32_t l = abs(lx) + abs(lz); if (l < 1) l = 1;
            s_an.vx = (int32_t)((int64_t)lx * AN_FLUG_TEMPO * 16 / l);
            s_an.vz = (int32_t)((int64_t)lz * AN_FLUG_TEMPO * 16 / l);
            s_an.fx0 = a->x; s_an.fz0 = a->z;
            clip(a, AN_FA_STURZ);
            a->rot_y = (int16_t)((a->rot_y + 0x800) & 0x0fff);   /* der Hieb reisst sie herum: sie faellt rueckwaerts zur Form hin,
                                                                  * also in die Tiefe von Cut 11 statt aus dem unteren Bildrand */
            a->hp = -1;                                  /* NPC-Familie: unverwundbar, kein Treffer-Kandidat (re15_damage.c) */
            s_an.blut_textur = 1;
            if (s_modell_fn) s_modell_fn(RE15_AN_TYP);   /* Kittel mit Blut (ANNETTE_4D_BLUT.EMD = RE2 EM049-Textur) */
            re15_game_flag_set(5, RE15_AN_SIG_TREFFER, 1);
            s_an.phase = RE15_AN_FLUG; s_an.t = 0;
            protokoll("Treffer");
        }
        vor(f, 1);
        break;
    case RE15_AN_FLUG: {
        if (vor(f, 1)) clip(f, AN_FC_STEHEN);
        if (a->anim_frame < AN_FA_STURZ_BODEN) vor(a, 1);
        a->x = klemme(a->x + s_an.vx / 16, AN_WAGEN_X_MIN, AN_WAGEN_X_MAX);
        a->z = klemme(a->z + s_an.vz / 16, AN_WAGEN_Z_MIN, AN_WAGEN_Z_MAX);
        s_an.vx = s_an.vx * 7 / 8; s_an.vz = s_an.vz * 7 / 8;
        /* Bogen: y = -H * 4 t (n - t) / n^2 */
        const int t = s_an.t, n = AN_FLUG_BILDER;
        a->y = -(int32_t)((int64_t)AN_FLUG_HOEHE * 4 * t * (n - t) / (n * n));
        if (t >= n) {
            int32_t p[3] = { a->x, -200, a->z };
            const int16_t w = blick(f, a->x, a->z);
            a->y = 0;
            blut(p, w, 0u); blut(p, w, 1u);
            a->anim_frame = AN_FA_STURZ_BODEN;
            s_an.phase = RE15_AN_KIPPEN; s_an.t = 0;
            protokoll("Aufschlag");
        }
        break;
    }
    case RE15_AN_KIPPEN:
        if (f->motion == AN_FC_HIEB) { if (vor(f, 1)) clip(f, AN_FC_STEHEN); } else vor(f, 0);
        a->anim_frame = AN_FA_STURZ_BODEN;
        a->rot_z = (int16_t)(AN_KIPP_WINKEL * s_an.t / AN_KIPP_BILDER);
        a->y = AN_KIPP_HEBEN * s_an.t / AN_KIPP_BILDER;
        if (s_an.t >= AN_KIPP_BILDER) {
            re15_game_flag_set(5, RE15_AN_SIG_LIEGT, 1);
            s_an.phase = RE15_AN_LEICHE; s_an.t = 0;
            protokoll("liegt");
        }
        break;
    case RE15_AN_LEICHE:
        if (f->motion == AN_FC_HIEB) { if (vor(f, 1)) clip(f, AN_FC_STEHEN); } else vor(f, 0);
        f->rot_y = blick(f, a->x, a->z);
        /* fallthrough: Leiche halten */
    case RE15_AN_FREI:
    default:
        a->anim_frame = AN_FA_STURZ_BODEN;
        a->rot_z = AN_KIPP_WINKEL;
        a->y = AN_KIPP_HEBEN;
        break;
    }
}
