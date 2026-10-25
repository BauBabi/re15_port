/*
 * gator_fx_r41_pc.c - Runde 41 Spur G "kanister2080", Nachbesserung 3 (Abnahme 2 M3 / Nachtrag 28 "der Explosionseffekt vom
 * Gaskanister vom Original"): die RE2-Effekte der Kanister-Explosion des Alligators im Port SICHTBAR machen, 1:1 wie RE2.
 *
 * RE2 (Dossier analysis/befunde_runde41/G_kanister2080.md 10.1, alles selbst disassembliert): die R1-Folge des Alligators
 * (EM23_OVL_0000.BIN, R1 P0 @0x801021ec-0x80102398) spawnt ueber die Fontaene FUN_80103eb4 (@0x80102368) Effekte der
 * RAUMBANKEN 0x26 (Blut-Sprueher, Op 69) und 0x08 (gluehende Brocken, Op 41/42 -> Bank 0x09 -> Op 5 -> Spritzer 0x1A) aus der
 * Raum-ESP von ROOM40A0 und der KERNBANK 0 (Blut, CLUT-Zeile 488), ueber den Spritzring FUN_8010338c (@0x80102374) die
 * Kernbank 3 (Zeile 481). Der Port hatte in ROOM2090/2091 nur GLAS1090.ESP (room1090) als Raum-ESP registriert und zeichnete
 * von RE2 TEX.TIM nur die Zeilen 480..484 (re2fx_pc.c, Kommentar N5).
 *
 * Diese Datei (PC-Seite, Engine-Ops in engine/src/re2_fx.c, Block "RUNDE 41 SPUR G"):
 *   1. Beim Betreten von ROOM2090/2091 die Raum-ESP von ROOM40A0 registrieren wie der RE2-Raumlader FUN_8001bba4 (FUN_8001bca0
 *      mit Registry-Basis 8 = re2fx_register_raum), mit den Texturen genau so umgelegt wie FUN_8001bd38 sie in den VRAM legt
 *      (RE2_Quellcode_V2/FUN_8001bd38.c): Bild an x = Spalte * 64 ab Spalte 15 (`iVar10 = 0xf`), y = laufende Hoehe, neue
 *      Spalte bei `0x100 < y + h`; CLUT an (0x120, Zeile) ab Zeile 480 (`y = 0x1e0`); Bankkopf +4 := GetClut(0x120, Zeile),
 *      +6 (Byte) := GetTPage(0, 0, x, y), jedes Zell-v (+n1*8+9, n2 Zellen) += y. Beim Verlassen wieder GLAS1090.ESP (die
 *      Raum-ESP der uebrigen Raeume, glas1120_pc.c), wie RE2 bei jedem Raumwechsel die Raum-ESP des neuen Raums registriert.
 *      Dateien (bytegleich ROOM40A0.RDT): GATOR40A0.ESP = RDT[0x59CC:0x65E8] (effect.esp, Ids 16 19 08 09 26 1A 18),
 *      GATOR40A0_<Id>.TIM = RDT[0x221FC + Offset] (Offset-Tafel rueckwaerts vor RDT[21] = 0x2FCFC).
 *   2. Textur-Platz 54 = 16-Bit-Atlas der Raum-Texturen 512 x (16 x 216): je Raum-CLUT-Zeile 480..495 (x 288) ein Block mit dem
 *      VRAM-Fenster x 896..1023 (TPage 0x0E/0x0F) y 0..215, in dem FUN_8001bd38 die sieben ROOM40A0-Texturen ablegt (Spalte 15:
 *      32+80+72+32, Spalte 14: 88+80+48 Zeilen); Texel vorab ueber die CLUT aufgeloest (dieselbe Regel re15_tim_texel_argb wie der
 *      4-bpp-Pfad von render_pc.c), geladen beim ersten Betreten von 2090/2091.
 *      Textur-Platz 56 = RE2 TEX.TIM Seiten 0x1E/0x1F mit den Kern-CLUT-Zeilen 485..498 (x 272), die re2fx_pc.c auslaesst (N5) -
 *      SPIELWEIT beim ersten Bild geladen (Orchestrator-Entscheid G-E4, Abnahme G 3 M2): RE2 haelt den Kern ab dem Start resident.
 *      Selbst gelesen (info/re2leon/PSX.EXE): main registriert @0x8002b65c-64 die Boot-Task 0x8002b6d4 (`lui a1,0x8003 / addiu
 *      a1,a1,-18732 / jal 0x80031f6c`); sie laedt "TEX TIM" (Name @0x80010a08, Datei 225) nach 0x8011a000 (`jal 0x80012fb8`
 *      @0x8002b8b0-c4) und legt es mit FUN_80076a40 nach VRAM (768,256), CLUT (256,480) 32 x 19 (@0x8002b8c8-dc); CORE00.ESP (Name
 *      @0x800108cc, Datei 144) laedt FUN_8001babc in den festen Puffer 0x801fc400 und registriert ihn (@0x8001bb38-8c), gerufen aus
 *      TITLE.BIN @0x801062dc; der Raumlader FUN_8001bba4 (einziger Aufruf @0x8004a2ec) traegt nur die Raumbanken ein. render_pc.c
 *      RE15_TIM_SLOT_MAX 57 (Orchestrator-Entscheid G-E6).
 *   3. Zeichnen: dieselben Quads wie re2fx_pc_draw (re2fx_quads = FUN_80077924/FUN_80077ed0), nur die Plaetze, die keiner der
 *      beiden vorhandenen Zeichner nimmt: Kern-Zeilen 485..498 in JEDEM Raum fuer jeden Platz (wie RE2; vorher erst nach dem
 *      ersten 2090-Besuch - gemessen Abnahme G 3 Abschnitt 5: Granatwerfer in ROOM2080 "gezeichnet G 0" ohne, "G 6" nach dem
 *      Besuch), Raum-TPage 0x0E/0x0F mit CLUT x 288 solange die Raum-ESP ROOM40A0 registriert ist. Mischmodus wie dort.
 *   4. MESSHAKEN RE15_GATOR_FX_LOG=1 (nur Messzeilen in debug.log, kein Spielverhalten): je Bild mit lebenden RE2-FX-Plaetzen
 *      die Zahl der Plaetze je Bank, der Quads je Zeichner-Klasse und der gezeichneten Quads.
 *   5. Orchestrator-Entscheid G-E1 (Abnahme G 2 M1): Masken-Haken re15_gr2_maske_hook fuer die R1-Kamerawahl (gator_re2_r41.c) -
 *      dieselbe Masken-Kette wie der Zeichner in main.c (RDT-Sektion des Cuts, sonst R15M-Container MASKS/ROOM%04X.MSK mit
 *      re15_masken36_filter und re15_optik_r39_kappen, gezeichnet werden draw_count Rechtecke @0x80039358), Urteil je Rechteck
 *      re15_pri_mask_occludes(depth, vz) (@0x8001ce54).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re2_fx.h"
#include "re2fx_pc.h"         /* re2fx_pc_letzte_quads */
#include "glas1120_pc.h"      /* re15_pc_glas1120_init (Raum-ESP der uebrigen Raeume), re15_pc_glas1120_letzte_quads */
#include "re15_tim.h"
#include "re15_room.h"        /* g_current_room_id */
#include "re15_actor.h"
#include "re15_engine.h"      /* g_engine.frame_count */
#include "re15_gator_re2_r41.h"   /* re15_gr2_r1_laeuft */
#include "asset_root_pc.h"    /* re15_pc_read_re2 */
#include "re15_pri.h"         /* G-E1: Masken-Sektionen, re15_pri_mask_occludes */
#include "re15_entladen.h"    /* G-E1: re15_entladen_msk (R15M-Container), g_re15_entladen_gen */
extern unsigned re15_pri_masken_raum(unsigned room_id);
extern int      re15_masken36_filter(unsigned masken_raum, int cut, re15_pri_cut_t *pri);
extern int      re15_optik_r39_kappen(unsigned masken_raum, int cut, re15_pri_cut_t *pri);

extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
extern int  re15_render_pc_dbg_slot_loaded(int slot);
extern void re15_render_pc_bind_tim_slot(int slot);
extern void re15_render_pc_set_tri_alpha(int a);
extern void re15_render_pc_set_tri_blend(int m);
extern void re15_render_textured_tri(int x0, int y0, int u0, int v0,
                                     int x1, int y1, int u1, int v1,
                                     int x2, int y2, int u2, int v2,
                                     int tpage, int clut, int z,
                                     uint8_t r, uint8_t g, uint8_t b);

#define G41FX_RAUM_SLOT     54      /* render_pc.c RE15_TIM_SLOT_MAX: 53 Kanister, 54 frei, 55 Glas */
#define G41FX_KERN_SLOT     56      /* render_pc.c RE15_TIM_SLOT_MAX 57 (Runde 41 Spur G, Orchestrator-Entscheid G-E6) */
#define G41FX_RAUM_BREITE   512     /* TPage 0x0E + 0x0F = VRAM x 896..1023 = 2 x 256 Texel (4 bpp) */
#define G41FX_RAUM_HOEHE    256     /* VRAM y 0..255 (TPage-Hoehe; ROOM40A0 belegt 0..215) */
#define G41FX_ATLAS_MAX_H   4096    /* GLES-Grenze der Texturhoehe (re2fx_pc.c Kommentar N5) */
#define G41FX_RAUM_CLUT_Y   480     /* `y = 0x1e0` FUN_8001bd38 */
#define G41FX_RAUM_CLUT_X   0x120   /* GetClut(0x120, y) FUN_8001bd38 -> x-Feld 0x12 */
#define G41FX_RAUM_CLUTS    16      /* Zeilen 480..495 (16 Paletten a 16 Farben) */
#define G41FX_SPALTE_START  15      /* `iVar10 = 0xf` FUN_8001bd38: x = 15 * 64 = 960 */
#define G41FX_KERN_CLUT0    485     /* erste Zeile, die re2fx_pc.c nicht laedt (RE2FX_CLUT_ERSTE 480 + 5) */
#define G41FX_KERN_CLUTS    14      /* 485..498: TEX.TIM-CLUT 32 x 19 ab Zeile 480 (`20 00 13 00`) */

static uint8_t  *s_esp;            /* umgelegte Kopie von GATOR40A0.ESP (re2fx haelt den Zeiger) */
static int       s_esp_sz;
static int       s_geladen;        /* 0 ungeprueft, 1 ok, -1 fehlt */
static int       s_reg;            /* 1 = Raum-ESP ROOM40A0 ist registriert */
static int       s_raum_ok;        /* Textur-Platz 54 geladen (Atlas der Raum-Texturen) */
static int       s_kern_geladen, s_kern_ok;   /* Platz 56 (Kern-Zeilen 485..498): versucht / geladen */
static int       s_raum_h;         /* belegte Hoehe des VRAM-Fensters nach FUN_8001bd38 (ROOM40A0: 216) = Blockhoehe im Atlas */
static uint8_t   s_raum_pix[G41FX_RAUM_BREITE / 2 * G41FX_RAUM_HOEHE];
static uint16_t  s_raum_clut[G41FX_RAUM_CLUTS * 16];
static uint8_t   s_kern_pix[512 / 2 * 256];
static uint16_t  s_kern_clut[G41FX_KERN_CLUTS * 16];   /* Zeilen 485..498, Eintraege 16..31 (x 272) */
static int       s_letzte_raum, s_letzte_kern;
static int       s_log = -1;

int re15_pc_gator_fx_registriert(void) { return s_reg; }
int re15_pc_gator_fx_letzte_quads(void) { return s_letzte_raum + s_letzte_kern; }

static uint32_t le32(const uint8_t *b) { return (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24); }
static uint16_t le16(const uint8_t *b) { return (uint16_t)(b[0] | (b[1] << 8)); }
static void     wle16(uint8_t *b, uint16_t v) { b[0] = (uint8_t)v; b[1] = (uint8_t)(v >> 8); }

/* 4-bpp-TIM mit CLUT lesen: Zeilen/Breite/CLUT-Hoehe; 0 = ok. */
typedef struct { const uint8_t *pix; int hw, h; const uint8_t *clut; int clut_zeilen; } g41_tim_t;
static int tim_lesen(const uint8_t *t, int sz, g41_tim_t *o)
{
    if (!t || sz < 20 || le32(t) != 0x10u || (le32(t + 4) & 0xFu) != 0x8u) return -1;   /* TIM, 4 bpp + CLUT */
    uint32_t csz = le32(t + 8);
    if (le16(t + 16) != 16) return -2;                                                  /* CLUT 16 breit */
    o->clut = t + 20; o->clut_zeilen = le16(t + 18);
    const uint8_t *img = t + 8 + csz;
    if ((int)(img - t) + 12 > sz) return -3;
    o->hw = le16(img + 8); o->h = le16(img + 10); o->pix = img + 12;
    if ((int)(img - t) + 12 + o->hw * 2 * o->h > sz) return -4;
    return 0;
}

/* FUN_8001bd38-Zwilling: Texturen der registrierten Raumbanken in das VRAM-Fenster legen und die Bankkoepfe der ESP-Kopie
 * umlegen. Bankkopf = Bankoffset aus der rueckwaerts gelesenen Offset-Tafel (FUN_8001bca0, wie re2fx_register_raum). */
static int raum_umlegen(void)
{
    const size_t al = ((size_t)s_esp_sz + 3u) & ~(size_t)3u;
    const int32_t ende = (int32_t)al - 4;
    int spalte = G41FX_SPALTE_START, y = 0, zeile = G41FX_RAUM_CLUT_Y, n = 0;
    s_raum_h = 0;
    memset(s_raum_pix, 0, sizeof s_raum_pix);
    memset(s_raum_clut, 0, sizeof s_raum_clut);
    for (n = 0; n < 8; n++) {
        const uint8_t id = s_esp[n];
        if (id == 0xFF) break;                                       /* `if (*pbVar9 == 0xff) return` */
        const uint32_t off = le32(s_esp + ende - 4 * n);
        if ((size_t)off + 12 > (size_t)s_esp_sz) return -10 - n;
        char name[32];
        snprintf(name, sizeof name, "GATOR40A0_%02X.TIM", (unsigned)id);
        int tsz = 0;
        uint8_t *t = re15_pc_read_re2(name, &tsz);
        g41_tim_t ti;
        if (tim_lesen(t, tsz, &ti) != 0 || ti.hw != 64) { free(t); return -20 - n; }   /* 256 Texel breit wie alle 40A0-TIMs */
        if (0x100 < y + ti.h) { spalte--; y = 0; }                   /* `if (0x100 < iVar8 + h) { iVar10--; iVar8 = 0; }` */
        const int x_texel = (spalte - (G41FX_SPALTE_START - 1)) * 256;  /* Spalte 14 -> 0, Spalte 15 -> 256 */
        if (spalte < G41FX_SPALTE_START - 1 || y + ti.h > G41FX_RAUM_HOEHE ||
            zeile + ti.clut_zeilen > G41FX_RAUM_CLUT_Y + G41FX_RAUM_CLUTS) { free(t); return -30 - n; }
        for (int r = 0; r < ti.h; r++)                               /* LoadImage(prect = (spalte*64, y)) */
            memcpy(s_raum_pix + (size_t)(y + r) * (G41FX_RAUM_BREITE / 2) + x_texel / 2, ti.pix + (size_t)r * 128, 128);
        for (int r = 0; r < ti.clut_zeilen; r++)                     /* LoadImage(crect = (0x120, zeile)) */
            for (int k = 0; k < 16; k++)
                s_raum_clut[(zeile - G41FX_RAUM_CLUT_Y + r) * 16 + k] = le16(ti.clut + (size_t)(r * 16 + k) * 2);
        free(t);
        uint8_t *bk = s_esp + off;
        wle16(bk + 4, (uint16_t)((zeile << 6) | (G41FX_RAUM_CLUT_X >> 4)));   /* puVar6[2] = GetClut(0x120, y) */
        bk[6] = (uint8_t)(((y & 0x100) >> 4) | ((spalte * 64) >> 6));        /* (char)puVar6[3] = GetTPage(0, 0, x, y) */
        const unsigned n1 = le16(bk), n2 = le16(bk + 2);
        if ((size_t)off + 8u + n1 * 8u + n2 * 4u > (size_t)s_esp_sz) return -40 - n;
        for (unsigned c = 0; c < n2; c++) bk[n1 * 8u + 9u + c * 4u] = (uint8_t)(bk[n1 * 8u + 9u + c * 4u] + y);   /* v += iVar8 */
        fprintf(stderr, "[gator-fx] Raumbank 0x%02X: TIM %d Zeilen -> TPage 0x%02X y %d, CLUT-Zeilen %d..%d (FUN_8001bd38)\n",
                (unsigned)id, ti.h, (unsigned)bk[6], y, zeile, zeile + ti.clut_zeilen - 1);
        y += ti.h;                                                   /* iVar8 += h */
        zeile += ti.clut_zeilen;                                     /* y += crect->h */
        if (y > s_raum_h) s_raum_h = y;                              /* Nachbesserung 4: Blockhoehe des Atlas */
    }
    return n;
}

/* 4-bpp-Index des Texels (x, y) in einem 512 x 256-Puffer (256 B je Zeile): gerades x = unteres Halbbyte (PSX-GPU; render_pc.c
 * re15_render_pc_upload_tim_slot liest genauso). */
static unsigned nib(const uint8_t *pix, int x, int y)
{
    const uint8_t b = pix[(size_t)y * 256u + (unsigned)x / 2u];
    return (x & 1) ? (unsigned)(b >> 4) : (unsigned)(b & 0xFu);
}

/* Kern-Zeilen 485..498 von RE2 TEX.TIM (Lage wie re2fx_pc_lade_tex: Bild (768,256), Seiten 0x1E/0x1F = Bild-hw 128..255,
 * CLUT (256,480) 32 x 19, Spalte x 272 = Eintraege 16..31) in Platz 56 - einmal, beim ersten Bild (G-E4, s. Kopf Punkt 2). */
static void kern_laden(void)
{
    if (s_kern_geladen) return;
    s_kern_geladen = 1;
    int xsz = 0;
    uint8_t *x = re15_pc_read_re2("TEX.TIM", &xsz);
    if (x && xsz > 20 && le32(x) == 0x10u && (le32(x + 4) & 0xFu) == 0x8u && le16(x + 16) == 32 && le16(x + 18) == 19) {
        const uint32_t csz = le32(x + 8);
        const uint8_t *img = x + 8 + csz;
        if ((size_t)(img - x) + 12u + 256u * 256u * 2u <= (size_t)xsz && le16(img + 8) == 256 && le16(img + 10) == 256) {
            for (int r = 0; r < G41FX_KERN_CLUTS; r++)
                for (int k = 0; k < 16; k++)
                    s_kern_clut[r * 16 + k] = le16(x + 20 + (size_t)((G41FX_KERN_CLUT0 - 480 + r) * 32 + 16 + k) * 2);
            for (int yy = 0; yy < 256; yy++) memcpy(s_kern_pix + (size_t)yy * 256, img + 12 + (size_t)yy * 512 + 256, 256);
            re15_tim_t t;
            memset(&t, 0, sizeof t);
            t.bpp = 4; t.has_clut = 1;
            t.clut_x = 272; t.clut_y = G41FX_KERN_CLUT0; t.clut_entries = G41FX_KERN_CLUTS * 16; t.clut = s_kern_clut;
            t.data_x = 896; t.data_y = 256; t.width = 512; t.height = 256;
            t.pixels = (const uint16_t *)(const void *)s_kern_pix;
            re15_render_pc_upload_tim_slot(&t, G41FX_KERN_SLOT);
            s_kern_ok = re15_render_pc_dbg_slot_loaded(G41FX_KERN_SLOT);
        }
    }
    free(x);
    fprintf(stderr, "[gator-fx] F%u Kern-Zeilen %d..%d (RE2 TEX.TIM) Slot %d %s - spielweit ab dem Start (G-E4)\n",
            (unsigned)g_engine.frame_count, G41FX_KERN_CLUT0, G41FX_KERN_CLUT0 + G41FX_KERN_CLUTS - 1, G41FX_KERN_SLOT,
            s_kern_ok ? "ok" : "FEHLT");
}

static void laden(void)
{
    if (s_geladen) return;
    s_geladen = -1;
    s_esp = re15_pc_read_re2("GATOR40A0.ESP", &s_esp_sz);
    if (!s_esp || s_esp_sz < 12) {
        fprintf(stderr, "[gator-fx] shared_assets/RE2/GATOR40A0.ESP fehlt -> keine Explosion der RE2-Raumbanken\n");
        return;
    }
    const int nb = raum_umlegen();
    if (nb <= 0) { fprintf(stderr, "[gator-fx] Raum-Texturen GATOR40A0_*.TIM fehlen/unlesbar (rc %d)\n", nb); return; }
    /* Atlas (Nachbesserung 4): 16 Raum-Bloecke (CLUT-Zeile 480 + k) der belegten Fensterhoehe s_raum_h. Texel = CLUT[Index]
     * (16 Bit), dieselbe Wertregel wie der 4-bpp-Pfad von render_pc.c. */
    const int h = G41FX_RAUM_CLUTS * s_raum_h;
    if (s_raum_h <= 0 || s_raum_h > G41FX_RAUM_HOEHE || h > G41FX_ATLAS_MAX_H) {
        fprintf(stderr, "[gator-fx] Atlas unmoeglich: Fensterhoehe %d -> %d Zeilen (Grenze %d)\n", s_raum_h, h, G41FX_ATLAS_MAX_H);
        return;
    }
    uint16_t *at = (uint16_t *)calloc((size_t)G41FX_RAUM_BREITE * (size_t)h, sizeof *at);
    if (!at) return;
    for (int c = 0; c < G41FX_RAUM_CLUTS; c++)
        for (int yy = 0; yy < s_raum_h; yy++)
            for (int xx = 0; xx < G41FX_RAUM_BREITE; xx++)
                at[(size_t)(c * s_raum_h + yy) * G41FX_RAUM_BREITE + (size_t)xx] = s_raum_clut[c * 16 + (int)nib(s_raum_pix, xx, yy)];
    re15_tim_t t;
    memset(&t, 0, sizeof t);
    t.bpp = 16; t.has_clut = 0;
    t.width = G41FX_RAUM_BREITE; t.height = h;
    t.pixels = at;
    re15_render_pc_upload_tim_slot(&t, G41FX_RAUM_SLOT);
    free(at);
    s_raum_ok = re15_render_pc_dbg_slot_loaded(G41FX_RAUM_SLOT);
    s_geladen = 1;
    fprintf(stderr, "[gator-fx] GATOR40A0.ESP %d B (%d Raumbanken umgelegt), Raum-Atlas Slot %d %s: %d x %d (16 Bloecke a %d Zeilen)\n",
            s_esp_sz, nb, G41FX_RAUM_SLOT, s_raum_ok ? "ok" : "FEHLT", G41FX_RAUM_BREITE, h, s_raum_h);
}

/* RE2-Raumlader FUN_8001bba4: Raum-ESP des NEUEN Raums registrieren (vorherige Raumbanken austragen, @0x8001bbc8-c0c). */
static void raum_takt(void)
{
    const unsigned r = (unsigned)g_current_room_id & 0xFFFFu;
    const int soll = (r == 0x2090u || r == 0x2091u);
    if (soll && !s_reg) {
        laden();
        if (s_geladen <= 0) return;
        const int rc = re2fx_register_raum(s_esp, (size_t)s_esp_sz);
        s_reg = (rc == 0);
        fprintf(stderr, "[gator-fx] F%u ROOM%04X: Raum-ESP ROOM40A0 -> re2fx_register_raum rc=%d (Banken 0x26 %d, 0x08 %d, 0x09 %d, 0x1A %d)\n",
                (unsigned)g_engine.frame_count, r, rc, re2fx_bank_registriert(0x26), re2fx_bank_registriert(0x08),
                re2fx_bank_registriert(0x09), re2fx_bank_registriert(0x1A));
    } else if (!soll && s_reg) {
        s_reg = 0;
        re15_pc_glas1120_init();                                     /* GLAS1090.ESP wieder (Raum-ESP der uebrigen Raeume) */
        fprintf(stderr, "[gator-fx] F%u ROOM%04X: Raum-ESP ROOM40A0 ausgetragen, GLAS1090.ESP wieder registriert\n",
                (unsigned)g_engine.frame_count, r);
    }
}

/* G-E1: Masken des Cuts `cut` wie main.c (Abschnitt "NACHGEZEICHNETE MASKEN"), zwischengespeichert je (Raum, Cut, Entlade-Stand). */
static re15_pri_cut_t s_mk;
static unsigned s_mk_raum = 0xFFFFFFFFu, s_mk_gen = 0xFFFFFFFFu;
static int      s_mk_cut = -1, s_mk_n;
static int maske_verdeckt(int cut, int sx, int sy, int32_t vz)
{
    if (!g_room_rdt_ok || !g_room_rdt.cuts || cut < 0 || cut >= g_room_rdt.cut_count) return 0;
    if (g_current_room_id != s_mk_raum || cut != s_mk_cut || g_re15_entladen_gen != s_mk_gen) {
        s_mk_raum = g_current_room_id; s_mk_cut = cut; s_mk_gen = g_re15_entladen_gen;
        memset(&s_mk, 0, sizeof s_mk);
        int n = g_room_rdt.raw ? re15_pri_parse_section(g_room_rdt.raw, (size_t)g_room_rdt.raw_size, g_room_rdt.cuts[cut].pri_offset, &s_mk) : 0;
        if (n == 0) {
            int msz = 0;
            const uint8_t *msk = re15_entladen_msk(g_current_room_id, &msz);
            if (msk) {
                const uint32_t mo = re15_pri_msk_section_offset(msk, (size_t)msz, cut);
                if (mo) n = re15_pri_parse_section(msk, (size_t)msz, mo, &s_mk);
                if (n > 0) n = re15_masken36_filter(re15_pri_masken_raum(g_current_room_id), cut, &s_mk);
                if (n > 0) n = re15_optik_r39_kappen(re15_pri_masken_raum(g_current_room_id), cut, &s_mk);
            }
        }
        s_mk_n = (n > 0) ? s_mk.draw_count : 0;
        if (s_mk_n > RE15_PRI_MAX_MASKS_PER_CUT) s_mk_n = RE15_PRI_MAX_MASKS_PER_CUT;
    }
    for (int m = 0; m < s_mk_n; m++) {
        const re15_pri_mask_t *k = &s_mk.masks[m];
        const int dx = (int16_t)k->dstX, dy = (int16_t)k->dstY;
        if (sx < dx || sx >= dx + (int)k->width || sy < dy || sy >= dy + (int)k->height) continue;
        if (re15_pri_mask_occludes((int)k->depth, (long)vz)) return 1;
    }
    return 0;
}

static int r1_laeuft(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x23 && re15_gr2_r1_laeuft(s)) return 1;
    return 0;
}

void re15_pc_gator_fx_draw(const re15_camera_view_t *cam, int cx, int cy, int camf,
                           int has_region, const int16_t rxs[4], const int16_t rzs[4])
{
    static re2fx_quad_t q[384];                                     /* 0x3C00 B Paketpuffer = 384 POLY_FT4 (re2fx_pc.c) */
    s_letzte_raum = s_letzte_kern = 0;
    re15_gr2_maske_hook = maske_verdeckt;
    kern_laden();                                                   /* G-E4: Kern-Zeilen ab dem ersten Bild, jeder Raum */                           /* G-E1 (gator_re2_r41.c gr2_kam_r1_start) */
    raum_takt();
    if (s_log < 0) { const char *e = getenv("RE15_GATOR_FX_LOG"); s_log = (e && *e && *e != '0') ? 1 : 0; }
    if (!cam) return;
    const int n = re2fx_quads(cam, cx, cy, camf, has_region, rxs, rzs, q, (int)(sizeof q / sizeof q[0]));
    int k480 = 0, k485 = 0, rr = 0, gl = 0, rest = 0, aus_fenster = 0, gebunden = -1;
    for (int i = n - 1; i >= 0; i--) {                              /* rueckwaerts wie re2fx_pc_draw (OT vorn) */
        const re2fx_quad_t *e = &q[i];
        const unsigned seite = e->tpage & 0x1Fu, cxf = e->clut & 0x3Fu;
        const int zeile = (e->clut >> 6) & 0x1FF;
        const uint8_t *pl = re2fx_platz(e->platz);
        const int bank = pl ? (int)pl[0x1C] : -1;
        int slot = -1, uo = 0, vo = 0;
        if ((seite == 0x1E || seite == 0x1F) && cxf == 0x11u) {
            if (zeile >= 480 && zeile < G41FX_KERN_CLUT0) { k480++; continue; }      /* re2fx_pc_draw */
            if (zeile >= G41FX_KERN_CLUT0 && zeile < G41FX_KERN_CLUT0 + G41FX_KERN_CLUTS) {
                k485++; slot = G41FX_KERN_SLOT; uo = (seite == 0x1F) ? 256 : 0;      /* CLUT-Zeile waehlt den Slab */
            }
        } else if (s_reg && (seite == 0x0E || seite == 0x0F) && cxf == (G41FX_RAUM_CLUT_X >> 4) &&
                   zeile >= G41FX_RAUM_CLUT_Y && zeile < G41FX_RAUM_CLUT_Y + G41FX_RAUM_CLUTS) {
            rr++;
            /* Atlas-Block der CLUT-Zeile; Zellen ausserhalb des belegten Fensters (v > s_raum_h) gaebe es nur bei einer
             * fehlerhaften ESP (FUN_8001bd38 legt jede Zelle in ihre TIM) - gezaehlt, nicht gezeichnet. */
            if (e->v0 > s_raum_h || e->v1 > s_raum_h) { aus_fenster++; continue; }
            slot = G41FX_RAUM_SLOT; uo = (seite == 0x0F) ? 256 : 0; vo = (zeile - G41FX_RAUM_CLUT_Y) * s_raum_h;
        } else if (bank >= 0x10 && bank <= 0x14) { gl++; continue; }                 /* re15_pc_glas1120_draw */
        if (slot < 0) { rest++; continue; }
        if ((slot == G41FX_KERN_SLOT && !s_kern_ok) || (slot == G41FX_RAUM_SLOT && !s_raum_ok)) continue;
        if (gebunden != slot) { re15_render_pc_bind_tim_slot(slot); gebunden = slot; }
        const int abe = (e->code & 2) != 0, abr = (e->tpage >> 5) & 3;     /* Mischmodus wie re2fx_pc_draw */
        if (abe) {
            switch (abr) {
            case 1:  re15_render_pc_set_tri_blend(1); re15_render_pc_set_tri_alpha(255); break;   /* B + F */
            case 2:  re15_render_pc_set_tri_blend(2); re15_render_pc_set_tri_alpha(255); break;   /* B - F */
            case 3:  re15_render_pc_set_tri_blend(1); re15_render_pc_set_tri_alpha(64);  break;   /* B + F/4 */
            default: re15_render_pc_set_tri_alpha(128); break;                                    /* 0.5B + 0.5F */
            }
        }
        const int u0 = e->u0 + uo, u1 = e->u1 + uo, v0 = e->v0 + vo, v1 = e->v1 + vo;   /* Raum-Atlas: Block-Versatz vo */
        re15_render_textured_tri(e->x0, e->y0, u0, v0,  e->x1, e->y0, u1, v0,
                                 e->x0, e->y1, u0, v1,  e->tpage, e->clut, e->vz, 255, 255, 255);
        re15_render_textured_tri(e->x1, e->y0, u1, v0,  e->x1, e->y1, u1, v1,
                                 e->x0, e->y1, u0, v1,  e->tpage, e->clut, e->vz, 255, 255, 255);
        if (abe) { re15_render_pc_set_tri_blend(0); re15_render_pc_set_tri_alpha(255); }
        if (slot == G41FX_KERN_SLOT) s_letzte_kern++; else s_letzte_raum++;
    }
    if (s_log != 1) return;
    /* MESSZEILE (RE15_GATOR_FX_LOG): Plaetze je Bank, Quads je Zeichner-Klasse, gezeichnet. */
    int bk[6] = {0}, sonst = 0, leb = 0, sicht = 0;
    for (int i = 0; i < RE2FX_PLAETZE; i++) {
        const uint8_t *pl = re2fx_platz(i);
        const unsigned st = pl ? (unsigned)(pl[0x18] | (pl[0x19] << 8)) : 0u;
        if (!st) continue;
        leb++;
        if ((st & 0xA000u) == 0xA000u) sicht++;
        switch (pl[0x1C]) {
        case 0x00: bk[0]++; break; case 0x03: bk[1]++; break; case 0x08: bk[2]++; break;
        case 0x09: bk[3]++; break; case 0x1A: bk[4]++; break; case 0x26: bk[5]++; break; default: sonst++; break;
        }
    }
    if (!leb) return;
    fprintf(stderr, "[gator-fx] F%u ROOM%04X R1 %d reg40A0 %d Plaetze %d (b00 %d b03 %d b08 %d b09 %d b1a %d b26 %d sonst %d) sichtbar %d "
                    "Quads %d: kern480 %d kern485 %d raum %d glas %d rest %d | gezeichnet G %d (kern485 %d raum %d) re2fx_pc %d | op_unbekannt %u op69 %u op41 %u"
                    " | aus_fenster %d\n",
            (unsigned)g_engine.frame_count, (unsigned)g_current_room_id & 0xFFFFu, r1_laeuft(), s_reg, leb, bk[0], bk[1], bk[2], bk[3],
            bk[4], bk[5], sonst, sicht, n, k480, k485, rr, gl, rest, s_letzte_raum + s_letzte_kern, s_letzte_kern, s_letzte_raum,
            re2fx_pc_letzte_quads(), re2fx_op_unbekannt(), re2fx_op_zaehler(69), re2fx_op_zaehler(41), aus_fenster);
}
