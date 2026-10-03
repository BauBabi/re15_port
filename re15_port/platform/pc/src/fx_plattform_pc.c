/*
 * fx_plattform_pc.c — Runde 34 (Granaten), Spur C (Plattform). Belege je Funktion im Header
 * fx_plattform_pc.h; Dossier analysis/befunde_runde34_granaten/bau_c.md.
 */
#include "fx_plattform_pc.h"
#include "re2_fx.h"
#include <string.h>

/* ===== C1 — ESP-Takt hinter dem Spielschritt ============================================== */

static int s_fx_takt_frei = 0;

void re15_pc_fx_takt_setzen(int frei) { s_fx_takt_frei = frei ? 1 : 0; }
int  re15_pc_fx_takt_frei(void)        { return s_fx_takt_frei; }

int re15_pc_fx_takt(void)
{
    if (!s_fx_takt_frei) return 0;
    s_fx_takt_frei = 0;
    /* RE1.5 @0x8001ce2c `jal 0x80019e20` — hinter Gegner (@0x8001ce04) und Spieler (@0x8001ce0c).
     * Das Pausen-Selbst-Gate (RE15_PAUSE_ACTION, @0x80019e40) traegt re15_esp_fx_tick selbst. */
    re15_esp_fx_tick(re15_esp_room_bank());
    /* RE2 @0x80026980 `jal 0x8001d300` — die Pumpe laeuft hinter der Gegner-Schleife
     * (0x800267c0 .. `bne s2,v0,0x800267c0` @0x80026930); im Port direkt hinter dem RE1.5-Tick. */
    re2fx_tick();
    return 1;
}

/* ===== C2 — ESP-Zeichnen ===================================================================== */

static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }

int re15_pc_esp_sichtbar(const re15_esp_fx_t *f)
{
    if (!f) return 0;
    if (!f->rows_base) return 1;                  /* Altpfad ohne Flags-Modell: wie bisher */
    /* `andi v0,v1,0x1` / `beq` @0x800532fc-300, `andi v0,v1,0x2` / `beq` @0x80053308-0c */
    return (f->flags & 0x03) == 0x03;
}

void re15_pc_esp_weltlage(const re15_esp_fx_t *f, int32_t out[3])
{
    if (f->wpos[0] | f->wpos[1] | f->wpos[2]) {
        /* slot+0x28/2a/2c, s16 (`lh v0,-68/-66/-64(s0)` @0x80053314/24/30; RTPS-Quelle
         * `addiu v0,a1,40` @0x8005350c) — von Spur A je Tick gerechnet (@0x8001a118-2a4). */
        out[0] = f->wpos[0]; out[1] = f->wpos[1]; out[2] = f->wpos[2];
    } else {
        /* Rueckfall bis Spur A wpos schreibt: die bisherige Port-Lage Anker + xlat. */
        out[0] = f->x + f->xlat_x; out[1] = f->y + f->xlat_y; out[2] = f->z + f->xlat_z;
    }
}

/* EFF-Kopf des Platzes (Bank, in der die Id aufgeloest wurde) — NULL, wenn keiner. */
static const uint8_t *esp_eff_kopf(const re15_esp_fx_t *f)
{
    const re15_esp_t *b = f->bank;
    if (!b || !b->raw || f->eff_idx < 0 || f->eff_idx >= b->id_count) return NULL;
    uint32_t hs = b->eff[f->eff_idx].eff_start;
    if ((size_t)hs + 8 > b->raw_size) return NULL;
    return b->raw + hs;
}

uint16_t re15_pc_esp_clut(const re15_esp_fx_t *f)
{
    if (!f) return 0;
    if (f->rows_base && f->clut) return f->clut;  /* gefuehrt (Saat + Routine 10 @0x800176e0-fc) */
    const uint8_t *k = esp_eff_kopf(f);
    if (!k) return 0;
    /* Spawner-Saat FUN_80019700: +0x32 = hdr u16 @+4 + ((sub & 0xff) >> 3) * 0x40 */
    return (uint16_t)(rd16(k + 4) + (uint16_t)((f->sub_index & 0xff) >> 3) * 0x40);
}

uint16_t re15_pc_esp_tpage(const re15_esp_fx_t *f)
{
    if (!f) return 0;
    if (f->rows_base && f->tpage) return f->tpage; /* gefuehrt (Saat + Routinen 8/10/17 ORen ABR) */
    const uint8_t *k = esp_eff_kopf(f);
    return k ? rd16(k + 6) : 0;                    /* Saat +0x30 = hdr u16 @+6 */
}

void re15_pc_esp_defwh(const re15_esp_fx_t *f, int32_t *w, int32_t *h)
{
    if (f && f->rows_base) {
        /* `lhu v0,4(a1)` @0x800535d0 / `lhu v0,6(a1)` @0x800535e0: die Zeilenkopie slot+0x04/+0x06
         * (vorzeichenlos). Routine 18 (@0x80017c8c) schreibt sie je Tick um (Feuer-Oszillator). */
        *w = (int32_t)(uint16_t)(f->row[0x04] | (f->row[0x05] << 8));
        *h = (int32_t)(uint16_t)(f->row[0x06] | (f->row[0x07] << 8));
    } else {
        *w = 0x1000; *h = 0x1000;                  /* Altpfad ohne Zeile: wie bisher */
    }
}

/* TEX.TIM-Kopf (Datei-Offsets, gelesen): Magic @0x00 = 0x10, Flags @0x04 = 0x08 (4 bpp + CLUT),
 * CLUT-Block @0x08 {len, x 256, y 480, w 32, h 24}, Bild-Block danach {len, x 0, y 0, w 320 hw,
 * h 256}. Die Effektseite x (VRAM 896/960) liegt in Bild-Halbwortspalte x - 896 + 192
 * (tools/tex_tim_effect_slice.py --verify-vram: 32768/32768 Halbworte gleich bei Quellspalte 192). */
#define TEX_VRAM_SEITE_X0   896    /* VRAM-x der ersten Effektseite (tpage 0x1e)            */
#define TEX_QUELL_SPALTE0   192    /* ... = TEX.TIM-Bild-Halbwortspalte 192 (gemessen, s.o.) */

int re15_pc_fx_seite_bauen(const uint8_t *tex, size_t n, uint16_t tpage,
                           re15_pc_fx_seite_t *seite, re15_tim_t *tim)
{
    if (!tex || !seite || !tim || n < 0x20) return -1;
    if (rd16(tex) != 0x0010 || rd16(tex + 2) != 0) return -2;          /* TIM-Magic          */
    if ((tex[4] & 0x0f) != 0x08) return -3;                             /* 4 bpp + CLUT       */
    uint32_t clen = (uint32_t)rd16(tex + 8) | ((uint32_t)rd16(tex + 10) << 16);
    int cx = rd16(tex + 12), cy = rd16(tex + 14), cw = rd16(tex + 16), ch = rd16(tex + 18);
    if (cx > RE15_PC_FX_CLUT_X || cx + cw < RE15_PC_FX_CLUT_X + 16) return -4;
    if (cy > RE15_PC_FX_CLUT_Y0 || cy + ch < RE15_PC_FX_CLUT_Y0 + RE15_PC_FX_CLUT_ZEILEN) return -4;
    size_t ib = 8u + clen;                                              /* Bild-Block         */
    if (ib + 12 > n || 20u + (size_t)cw * ch * 2u > 8u + clen) return -5;
    int iw = rd16(tex + ib + 8), ih = rd16(tex + ib + 10);             /* Halbworte, Zeilen  */
    const uint8_t *pix = tex + ib + 12;
    if (ib + 12 + (size_t)iw * ih * 2u > n || ih < 256) return -5;
    int vx = (tpage & 0x0f) * 64;                                       /* psx-spx TPAGE x    */
    if (!((tpage >> 4) & 1)) return -6;          /* y 0: nicht im TEX-Schnitt (Seiten bei y 256) */
    int col0 = vx - TEX_VRAM_SEITE_X0 + TEX_QUELL_SPALTE0;
    /* nur das GEMESSENE Rechteck (VRAM 896..1023 = Spalten 192..319); alles andere ist nicht belegt */
    if (col0 < TEX_QUELL_SPALTE0 || col0 + 64 > iw) return -6;
    for (int y = 0; y < 256; y++)
        for (int x = 0; x < 64; x++)
            seite->pix[y * 64 + x] = rd16(pix + ((size_t)y * iw + col0 + x) * 2u);
    const uint8_t *cl = tex + 20;
    for (int r = 0; r < RE15_PC_FX_CLUT_ZEILEN; r++)
        for (int i = 0; i < 16; i++)
            seite->clut[r * 16 + i] = rd16(cl + ((size_t)(RE15_PC_FX_CLUT_Y0 - cy + r) * cw
                                                  + (RE15_PC_FX_CLUT_X - cx) + i) * 2u);
    memset(tim, 0, sizeof *tim);
    tim->bpp = 4;
    tim->has_clut = 1;
    tim->clut_x = RE15_PC_FX_CLUT_X;
    tim->clut_y = RE15_PC_FX_CLUT_Y0;
    tim->clut_entries = RE15_PC_FX_CLUT_ZEILEN * 16;
    tim->clut = seite->clut;
    tim->data_x = vx;
    tim->data_y = 256;
    tim->width = 256;
    tim->height = 256;
    tim->pixels = seite->pix;
    return 0;
}

int re15_pc_fx_seite_clut_ok(uint16_t clut)
{
    int x = (clut & 0x3f) * 16, y = (clut >> 6) & 0x1ff;   /* psx-spx CLUT-Wort */
    return x == RE15_PC_FX_CLUT_X && y >= RE15_PC_FX_CLUT_Y0 &&
           y < RE15_PC_FX_CLUT_Y0 + RE15_PC_FX_CLUT_ZEILEN;
}

/* ===== C3 — Licht-Latch-Leser ================================================================ */

#include "re15_skeleton.h"   /* re15_skel_euler_matrix = RotMatrix 0x80068098 (byte-true) */

void re15_pc_licht_latch_rechnen(re15_light_cut_t *cut, int32_t px, int32_t py, int32_t pz,
                                 int16_t rot_y)
{
    /* FUN_8004f008 @0x8001cecc: RotMatrix(0, rot_y, 0) (@0x8004f038 `jal 0x80068098`, SVECTOR
     * {0, a0, 0} @0x8004f01c/34/3c) und ApplyMatrix (@0x8004f048 `jal 0x800661c0`: ctc2 RT,
     * MVMVA 0x4a486012 = sf 1, M = RT, V0, keine Translation; swc2 MAC1..3). Eingang (1200, y?, 0)
     * — y im Scratchpad ist ungesetzt, wirkt aber nicht: bei rx = rz = 0 sind RT12 und RT32 0
     * (m[1] = -(sin(0)*cy)>>12, m[7] = (cz*sin(0))>>12 - ((sin(0)*-sy)>>12 * cx)>>12). */
    int32_t m[9];
    re15_skel_euler_matrix(0, (int)rot_y, 0, m);
    int32_t rx = (m[0] * RE15_PC_LICHT_VOR + m[2] * 0) >> 12;   /* MAC1 */
    int32_t rz = (m[6] * RE15_PC_LICHT_VOR + m[8] * 0) >> 12;   /* MAC3 */
    cut->type_flags[2] = 0;                                              /* @0x8001cef8 */
    if (cut->colors[2][0] < RE15_PC_LICHT_R_MIN) cut->colors[2][0] = RE15_PC_LICHT_R_MIN;   /* @0x8001cf20-34 */
    if (cut->colors[2][1] < RE15_PC_LICHT_G_MIN) cut->colors[2][1] = RE15_PC_LICHT_G_MIN;   /* @0x8001cf5c-70 */
    if (cut->colors[2][2] < RE15_PC_LICHT_B_MIN) cut->colors[2][2] = RE15_PC_LICHT_B_MIN;   /* @0x8001cf98-ac */
    /* lhu + lhu, sh: 16-Bit-Summen (@0x8001cfb8-e4, @0x8001d010-1c, @0x8001d02c-58) */
    cut->positions[2][0] = (int16_t)(uint16_t)((uint16_t)px + (uint16_t)rx);
    cut->positions[2][1] = (int16_t)(uint16_t)((uint16_t)py - RE15_PC_LICHT_HOEHE);
    cut->positions[2][2] = (int16_t)(uint16_t)((uint16_t)pz + (uint16_t)rz);
    cut->brightness[2] = RE15_PC_LICHT_HELL;                             /* @0x8001d080-84 */
}

static re15_light_cut_t s_licht_kopie;      /* [0x800ac77c] — die 40-Byte-Kopie (@0x8001cea0) */
static int              s_licht_cut = -1;   /* umgestellter Satz, -1 = keiner                */

int re15_pc_licht_latch_anwenden(re15_light_set_t *ls, int cut, int32_t px, int32_t py, int32_t pz,
                                 int16_t rot_y)
{
    s_licht_cut = -1;
    if (!g_re15_licht_latch) return 0;                               /* @0x8001ce60-68 */
    if (!ls || cut < 0 || cut >= ls->cut_count || cut >= RE15_LIGHT_MAX_CUTS) return 0;
    s_licht_kopie = ls->cuts[cut];                                   /* @0x8001cea0 */
    s_licht_cut = cut;
    re15_pc_licht_latch_rechnen(&ls->cuts[cut], px, py, pz, rot_y);
    return 1;
}

void re15_pc_licht_latch_zurueck(re15_light_set_t *ls)
{
    if (!g_re15_licht_latch) { s_licht_cut = -1; return; }           /* @0x8001d174-7c */
    if (ls && s_licht_cut >= 0 && s_licht_cut < RE15_LIGHT_MAX_CUTS)
        ls->cuts[s_licht_cut] = s_licht_kopie;                       /* @0x8001d1ac */
    s_licht_cut = -1;
    g_re15_licht_latch = 0;                                          /* @0x8001d1b4 */
}

/* ===== C4 — Ton-Weiche und Haken-Bindung ==================================================== */

#include "re15_audio.h"
#include "re15_damage.h"     /* re15_re2_gl_apply (V2b) */
#include <stdio.h>

extern FILE *re15_waffen_log(void);   /* engine/src/player_common.c — Mess-Log RE15_WAFFEN_LOG */

int re15_pc_esp_se_weiche(uint32_t code, int *satz)
{
    unsigned bank = code >> 24;                           /* `srl v1,a0,24` @0x80045028 */
    int s = (int)((code >> 16) & 0xff);                   /* @0x80045078-7c; Byte1 ungelesen */
    re15_se_bank_kind_t k = re15_audio_se_bank_kind(bank);/* Tor < 6 @0x80045094, Tabelle @0x80010e70 */
    if (satz) *satz = s;
    if (k == RE15_SE_BANK_SKIP) return 0;
    if (k == RE15_SE_BANK_SND1) { if (s >= 0x19) return 0; }   /* `sltiu v0,s4,0x19` @0x800450f8 */
    else if (s >= 0x21) return 0;                               /* `sltiu v0,s4,0x21` @0x800450bc.. */
    return (int)k;
}

int re15_pc_re2fx_se_weiche(uint32_t code, int *arms_id, int *satz)
{
    int id;
    if (code == RE15_PC_RE2FX_SE_SAEURE)     id = RE15_PC_ARMS_SAEURE;
    else if (code == RE15_PC_RE2FX_SE_BRAND) id = RE15_PC_ARMS_BRAND;
    else return 0;
    if (arms_id) *arms_id = id;
    if (satz) *satz = RE15_PC_ARMS_AUFSCHLAG_SATZ;
    return 1;
}

void re15_pc_esp_se(uint32_t code, const int32_t pos[3])
{
    (void)pos;   /* Lage-Flag Byte0: positionaler Zweig FUN_80045a64 nicht portiert (BAUPLAN O5) */
    int satz = 0;
    int k = re15_pc_esp_se_weiche(code, &satz);
    {   FILE *wl = re15_waffen_log();
        if (wl) fprintf(wl, "    SE  esp code=0x%08x bank=%u satz=%d -> %s\n", (unsigned)code,
                        (unsigned)(code >> 24), satz,
                        k == RE15_SE_BANK_WEAPON ? "ARMS" : k == RE15_SE_BANK_CORE ? "CORE" :
                        k == RE15_SE_BANK_SND0 ? "SND0" : k == RE15_SE_BANK_SND1 ? "SND1" : "verworfen"); }
    switch (k) {
        case RE15_SE_BANK_WEAPON: re15_audio_weapon_se(satz);    break;   /* ARMS der Waffe */
        case RE15_SE_BANK_SND0:   re15_audio_room_se_snd0(satz); break;
        case RE15_SE_BANK_SND1:   re15_audio_room_se(satz);      break;
        case RE15_SE_BANK_CORE:   re15_audio_core_se(satz);      break;
        default: break;
    }
}

void re15_pc_re2fx_se(uint32_t code, const int32_t pos[3])
{
    (void)pos;
    int arms = 0, satz = 0;
    int ok = re15_pc_re2fx_se_weiche(code, &arms, &satz);
    /* Runde 35 Spur B (Werfer-Klasse, re2_fx.c Ops 7/47):
     *   0x01000001  Op 7 Muendungsblitz (`(+0x16 << 16) + 1` @0x8001e1b8, rr = 256) = ARMS Satz 0
     *               der gefuehrten Waffe (RE2 Bank 1) -> re15_audio_weapon_se(0);
     *   0x01110001  Op 47 Sub 12 Explosiv-Aufschlag (@0x80020d40-48) -> RE1.5 ARMS0F Satz 10
     *               (`00003320`, VAG 12800 B = bytegleich RE2 ARMS09 Satz 17, Dossier §2.1);
     *   0x01140001  Op 47 Sub 13 Raketen-Explosion (@0x80020d3c/dc0-c4) -> RE2 ARMS11 Satz 20
     *               (`00003320`, shared_assets/RE2/SOUND/ARMS11). */
    const char *was = ok ? (arms == RE15_PC_ARMS_SAEURE ? "ARMS10 Satz 10" : "ARMS11 Satz 10")
                         : (code == 0x01000001u) ? "ARMS Satz 0 (Waffe)"
                         : (code == 0x01110001u) ? "ARMS0F Satz 10"
                         : (code == 0x01140001u) ? "RE2 ARMS11 Satz 20" : "unbekannt (stumm)";
    {   FILE *wl = re15_waffen_log();
        if (wl) fprintf(wl, "    SE  re2fx code=0x%08x -> %s @(%d,%d,%d)\n", (unsigned)code, was,
                        pos ? (int)pos[0] : 0, pos ? (int)pos[1] : 0, pos ? (int)pos[2] : 0); }   /* Mess-Harness: Lage */
    if (ok) { re15_audio_arms_zusatz_se(arms, satz); return; }
    if (code == 0x01000001u)      re15_audio_weapon_se(0);
    else if (code == 0x01110001u) re15_audio_arms_zusatz_se(0x0F, RE15_PC_ARMS_AUFSCHLAG_SATZ);
    else if (code == 0x01140001u) { extern void re15_audio_re2_arms_se(int, int); re15_audio_re2_arms_se(0x11, 20); }
}

void re15_pc_r34_haken_binden(void)
{
    re15_esp_se_hook        = re15_pc_esp_se;      /* V1c: Routinen 29/31 -> FUN_80045024-Analogon */
    re15_esp_aufschlag_hook = re2fx_aufschlag;     /* V1d: Zuender-7-Bild 0x0A/0x0B -> Op 49/48 (E8) */
    re2fx_se_hook           = re15_pc_re2fx_se;    /* V3: Op 48/49-SEs -> ARMS11/ARMS10 Satz 10 (E9) */
    re2fx_applier           = re15_re2_gl_apply;   /* V2b/V3: Op 40 -> FUN_800470C0-Zwilling (Spur B) */
}

/* ===== Zeichenkamera des Effektpasses: entfallen (Integration W3) — main.c ruft
 * re2fx_pc_set_ansicht mit den Werten von pc_draw_effects (fx_plattform_pc.h). ================= */

/* ===== C8 — Harness RE15_FORCE_AUFSCHLAG ===================================================== */

#include <stdlib.h>

int re15_pc_force_aufschlag_eintrag(const char *spec, unsigned bild, int32_t px, int32_t py,
                                    int32_t pz, int16_t rot_y, int *re2_art, int32_t q[3])
{
    if (!spec || !*spec) return 0;
    const char *p = spec;
    while (*p) {
        char *e = NULL;
        long art = strtol(p, &e, 10);
        if (!e || *e != '@') return 0;
        long b = strtol(e + 1, &e, 10);
        if (b >= 0 && (unsigned long)b == bild && (art == 1 || art == 2)) {
            int32_t m[9];
            re15_skel_euler_matrix(0, (int)rot_y, 0, m);        /* RotMatrix 0x80068098 */
            if (re2_art) *re2_art = (int)art;
            if (q) {
                q[0] = px + ((m[0] * RE15_PC_FORCE_AUFSCHLAG_ABSTAND) >> 12);
                q[1] = py;
                q[2] = pz + ((m[6] * RE15_PC_FORCE_AUFSCHLAG_ABSTAND) >> 12);
            }
            return 1;
        }
        if (!e || !*e) break;
        p = (*e == ',') ? e + 1 : e;
        if (p == e) break;   /* unbekanntes Trennzeichen */
    }
    return 0;
}

/* ===== Integration W6 — Harness RE15_FORCE_EXPLOSION="<art>@<bild>:<slot>[,...]" ============== */

int re15_pc_force_explosion_eintrag(const char *spec, unsigned bild, int *art, int *slot)
{
    if (!spec || !*spec) return 0;
    const char *p = spec;
    while (*p) {
        char *e = NULL;
        long a = strtol(p, &e, 10);
        if (!e || *e != '@') return 0;
        long b = strtol(e + 1, &e, 10);
        if (!e || *e != ':') return 0;
        long s = strtol(e + 1, &e, 10);
        if (b >= 0 && (unsigned long)b == bild && a >= 2 && a <= 4 && s >= 1 && s < RE15_ACTOR_MAX) {
            if (art)  *art  = (int)a;
            if (slot) *slot = (int)s;
            return 1;
        }
        if (!e || !*e) break;
        p = (*e == ',') ? e + 1 : e;
        if (p == e) break;   /* unbekanntes Trennzeichen */
    }
    return 0;
}

int re15_pc_force_explosion(int art, const re15_actor_t *ziel)
{
    if (!ziel || art < 2 || art > 4) return -1;
    /* Wie Routine 31 im Zuender-7-Bild (re15_esp.c): P = (x, y - 500, z) der liegenden Granate
     * (`lh v0,42(v1)` @0x800185a0 / `addiu v0,v0,-500` @0x800185a8), FUN_80012d60(500, &P, Art) (`ori a0,zero,0x1f4`
     * @0x80018598, `jal 0x80012d60` @0x800185b8) — hier liegt die "Granate" am Ziel. */
    re15_attack_box_t box;
    box.x = ziel->x; box.y = ziel->y - 500; box.z = ziel->z;
    box.radius = 500;
    int n = re15_resolve_attack(&box, (uint8_t)art, -1);
    if (art != 2) {
        /* E8: Art 3/4 -> Aufschlag Op 49 (Saeure, re2_art 2) / Op 48 (Brand, re2_art 1) an der Lage */
        int32_t q[3] = { ziel->x, ziel->y, ziel->z };
        re2fx_aufschlag(5 - art, q, (int16_t)ziel->rot_y);
    }
    return n;
}

/* ===== C7 — RE2-Part-Farbwort fuer RE2-KI-Aktoren ausserhalb der Zombie-Gore-Bruecke ========= */

#include "re15_ai_flavor.h"
#include "re2_ems.h"         /* re2_hybrid_perm — RE2-Part -> Bone der RE1.5-Bank (Integration W6) */

int re15_pc_re2_part_tint(const re15_actor_t *e, int n, int re2_rig, uint32_t *out_tint, int out_n)
{
    if (!e || !out_tint || out_n <= 0) return 0;
    for (int i = 0; i < out_n; i++) out_tint[i] = 0x00808080u;      /* neutral (FUN_80028368.c:55) */
    if (!re15_ai_re2_for_type(e->type)) return 0;                    /* nur RE2-KI-Typen */
    /* Integration W6: ALLE Part-Woerter, die Spur B fuehrt (re2z_part_tint[20], re15_actor.h) —
     * Hund-Tod faerbt 17 Parts (EMD0G_MOD0: `sw a0,112(v0)` @0x801047f4, `sltiu v0,s0,0x11`
     * @0x801047f8), Spinnen-Tod 20 (EMS25 FUN_8010609C: `sw a1,112(v0)` @0x801060b0,
     * `sltiu v0,a2,0x14` @0x801060b4) — selbst nachgelesen. Vorher endete die Schleife bei 16. */
    const int parts = (int)(sizeof e->re2z_part_tint / sizeof e->re2z_part_tint[0]);
    /* Part -> Bone der GEZEICHNETEN Bank (Gegenpruefung C H3): traegt sie das RE2-Skelett (reines
     * RE2-EMD oder Hybrid "RE2-Rig + RE1.5-Geometrie", re2_hybrid_apply), ist Bone-Slot i == Part i.
     * Ist es das RE1.5-Skelett (RE2-Archiv fehlte -> pc_enemy_load_ex-Rueckfall), liegt Part p auf
     * Bone perm[p] der Hybrid-Tabelle re2_hybrid_perm — dieselbe Abbildung, die re2z_part_to_bone /
     * re2z_bone_to_part im Import-Modus nehmen (re2z_perm_for); -1 (Hundepfoten-Slots 7/10) = kein
     * Bone -> die Tinte faellt weg. Ohne Tabelle fuer den Typ: keine Tinte. */
    const int8_t *perm = NULL;
    int pn = parts;
    if (!re2_rig) {
        pn = re2_hybrid_perm((int)e->type, &perm);
        if (pn <= 0 || !perm) return 0;
    }
    int nicht_neutral = 0;
    for (int p = 0; p < parts && p < pn; p++) {
        uint32_t t = e->re2z_part_tint[p];
        if (t == 0) continue;                                        /* nie geseedet -> neutral */
        int b = re2_rig ? p : (int)perm[p];
        if (b < 0 || b >= n || b >= out_n) continue;
        out_tint[b] = t;                                             /* Part +0x70 (@0x80027900) */
        if ((t & 0x00ffffffu) != 0x00808080u) nicht_neutral = 1;
    }
    return nicht_neutral;
}
