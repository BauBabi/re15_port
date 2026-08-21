/* test_prop_texel_key_11f0.c — PIN: der Raum-Prop-Farbschluessel (ROOM11F0-Cursor)
 *
 * NUTZER-BEFUND (2026-08-21, praezisiert): "Bei dem Raetsel, wo man die Schalter druecken
 * muss, da bewegt man einen Cursor. Und der ist im Original der Hintergrund transparent
 * und bei uns schwarz."  Es geht um die FLAECHE DES CURSORS, nicht um das Raumbild.
 *
 * ── WAS DER CURSOR IST ───────────────────────────────────────────────────────────────
 * ROOM11F0.RDT, sub00 @Datei 0x00E54:
 *     2d 00 04 00 00 00 00 01 00 00 ...
 *     Obj_model_set id=0x00 type=4 band=0 pos=(-19554,0,22684) rot=(0,0,0)
 * Modell-Zeigertabelle @RDT+0x30 -> 0x240 (nOmodel = RDT[0x02] = 12), je Prop ein Paar
 * (TIM,MD1) u32 LE:  prop[0] TIM@0x018DAC MD1@0x001928 = der Cursor (eigenes Modell +
 * eigene Textur), prop[1] = die Maschine, prop[2..11] teilen sich Schalter-Modell+Textur.
 * prop[0]-TIM: 8bpp, 128x256, EINE CLUT (256x1 @(0,480)), CLUT[0] = 0x0000.
 *
 * ── WELCHER TRANSPARENZ-MECHANISMUS IM ORIGINAL GREIFT ───────────────────────────────
 * Objekt-Draw-Loop FUN_8002c18c (Objekt-Pool @0x800b3f98, stride 0x94):
 *     - `if (obj[+0x08] == 4) m1.t[1] -= 900`                 (Typ-4-Render-Versatz)
 *     - `bVar2 = (obj[+0x0C] & 2) != 0`                       -> letzter Parameter
 *     - `(obj[+0x0C] & 0x10) == 0` -> FUN_800254a0 (Tri) / FUN_800256b0 (Quad)
 * FUN_800254a0: `param_3->cd = param_4 << 1 | 0x34;`   POLY_GT3, Bit 0x02 = ABE
 * FUN_800256b0: `param_3->cd = param_4 << 1 | 0x3c;`   POLY_GT4, Bit 0x02 = ABE
 * Woher obj[+0x0C] kommt — Obj_model_set-Handler LAB_80040914 (PSX.EXE, disassembliert):
 *     @0x8004093c  lbu a3,1(a2)      pc[1] = obj_id -> Pool-Index (*0x94)
 *     @0x80040940  lbu a0,2(a2)   /  @0x8004095c sb a0,8(a1)    -> obj+0x08 = pc[2] TYPE
 *     @0x80040990  lhu v0,6(a2)   /  @0x800409a4 sw v0,0(a1)    -> obj+0x00 = pc[6..7]|1
 *     @0x800409a8  lh  v0,8(a2)   /  @0x800409b0 sw v0,12(a1)   -> obj+0x0C = pc[8..9]
 * Der Cursor-Record hat pc[8..9] = 00 00, also obj+0x0C = 0 -> ABE-Bit 0, Primitiv-Code
 * bleibt 0x34/0x3C = OPAK.  => SEMI-TRANSPARENZ (ABE) IST ES NICHT.
 * Es bleibt der GPU-FARBSCHLUESSEL. nocash psx-spx, docs/graphicsprocessingunitgpu.md:
 *     "Texture Palettes - CLUT":  `Color 0000h = Fully-transparent`
 *     "Texture Color Black Limitations": "On the PSX, texture color 0000h is
 *      fully-transparent, that means textures cannot contain Black pixels."
 * Die Regel haengt am TEXEL-WERT, nicht am Zeichenbefehl (opaque vs. semi-transparent
 * unterscheidet dort nur die Behandlung von 8000h) — sie gilt also fuer JEDES
 * texturierte Primitiv und damit fuer alle Raum-Props, nicht nur fuer obj 0x00.
 *
 * ── WAS DER PORT TAT ─────────────────────────────────────────────────────────────────
 * render_pc.c lud Prop-TIMs mit `treat_index_0_opaque = (slot != 19 && slot != 20)`;
 * Prop-Slots (RE15_TIM_SLOT_PROP = 4..9 / 26..35) fielen darunter, Index 0 wurde also zu
 * rgb555(CLUT[0]=0x0000) = 0xFF000000 = OPAKES SCHWARZ. Der Farbschluessel wurde im
 * Prop-Pfad ueberhaupt nicht ausgewertet (nur im BG- und im Effekt-Sprite-Pfad).
 *
 * ── WAS DIESER TEST PINNT ────────────────────────────────────────────────────────────
 * Gemessen wird auf den ECHTEN Asset-Bytes, ueber die GENAU SELBE Funktion, die der
 * PC-Renderer beim TIM-Upload benutzt (`re15_tim_texel_argb`, re15_tim.h) — kein Nachbau:
 *   P1  Der Cursor-Record ist byte-genau der oben zitierte (Typ 4, pc[8..9] == 0).
 *       -> ABE aus, also MUSS der Farbschluessel die Transparenz liefern.
 *   P2  Unter RE15_TIM_KEY_PSX wird ein grosser Teil der vom Cursor-Modell TATSAECHLICH
 *       gesampelten Texel transparent (Erwartung byte-genau, s.u.).
 *   P3  NEGATIV = der alte Zustand: unter RE15_TIM_KEY_NONE wird KEIN einziger davon
 *       transparent, und die Index-0-Texel werden zu OPAKEM SCHWARZ (0xFF000000).
 *       Faellt der Fix zurueck, geht P2 rot und P3 beschreibt exakt den Bug.
 *   P4  REGRESSIONSWACHE: bei allen 11 anderen Props dieses Raums aendert der
 *       Farbschluessel NICHTS — 0 transparente Texel unter beiden Modi.
 *   P5  REGRESSIONSWACHE Slots 19/20: RE15_TIM_KEY_INDEX0 verhaelt sich unveraendert
 *       (Index 0 transparent, alles andere opak — unabhaengig vom CLUT-Wert).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_md1.h"
#include "re15_tim.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int g_fail = 0;
#define CHECK(cond, ...) do { \
    if (!(cond)) { g_fail++; printf("  FAIL: " __VA_ARGS__); printf("\n"); } \
} while (0)

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

/* Ein Texel der TIM lesen (4/8 bpp indiziert) -> Palettenindex. */
static int tim_index_at(const re15_tim_t *t, int x, int y)
{
    const uint8_t *src = (const uint8_t *)t->pixels;
    long i = (long)y * t->width + x;
    if (t->bpp == 8) return src[i];
    return (i & 1) ? (src[i >> 1] >> 4) : (src[i >> 1] & 0xF);
}

/* Die vom Modell tatsaechlich gesampelte Texturflaeche ablaufen: pro Face die
 * UV-Bounding-Box (wie der Renderer sie aufspannt), mit dem Seiten-Offset
 * (page & 0x0F) * 128 — identisch zu main.c's `page_off`. Zaehlt, wie viele dieser
 * Texel unter `key_mode` als "nicht zeichnen" (ARGB == 0) dekodieren, und wie viele
 * davon Palettenindex 0 sind. */
typedef struct { long sampled, transparent, idx0, opaque_black; } cover_t;

static void cover_face(const re15_tim_t *tim, const uint16_t *clut_row, int key_mode,
                       const uint8_t *us, const uint8_t *vs, int n, int page,
                       cover_t *acc)
{
    int u0 = us[0], u1 = us[0], v0 = vs[0], v1 = vs[0];
    for (int k = 1; k < n; k++) {
        if (us[k] < u0) u0 = us[k];
        if (us[k] > u1) u1 = us[k];
        if (vs[k] < v0) v0 = vs[k];
        if (vs[k] > v1) v1 = vs[k];
    }
    int po = (page & 0x000F) * 128;
    for (int y = v0; y <= v1; y++) {
        for (int x = u0; x <= u1; x++) {
            int X = x + po;
            if (X >= tim->width || y >= tim->height) continue;
            int idx = tim_index_at(tim, X, y);
            /* key_mode == RE15_TIM_KEY_PSX ist per Definition die Produktionsregel
             * `re15_tim_texel_argb()`; die beiden anderen Modi sind der HISTORISCHE
             * Zustand und leben nur noch als Negativ-Kontrolle (re15_tim.h). */
            uint32_t argb = (key_mode == RE15_TIM_KEY_PSX)
                              ? re15_tim_texel_argb(clut_row[idx])
                              : re15_tim_texel_argb_legacy(clut_row[idx], idx == 0, key_mode);
            acc->sampled++;
            if (idx == 0) acc->idx0++;
            if (argb == 0u) acc->transparent++;
            else if (argb == 0xFF000000u) acc->opaque_black++;
        }
    }
}

static cover_t cover_prop(const re15_rdt_t *rdt, int op, int key_mode, int *ok)
{
    cover_t c = {0, 0, 0, 0};
    *ok = 0;
    re15_tim_t tim;
    re15_md1_t md1;
    if (!rdt->prop_tim[op] || !rdt->prop_md1[op]) return c;
    if (re15_tim_parse(rdt->prop_tim[op], rdt->prop_tim_size[op], &tim) != 0) return c;
    if (re15_md1_parse(rdt->prop_md1[op], rdt->prop_md1_size[op], &md1) != 0) return c;
    if (!tim.has_clut || (tim.bpp != 4 && tim.bpp != 8)) return c;
    const uint16_t *clut_row = tim.clut;   /* ROOM11F0: genau eine CLUT je Prop-TIM */
    for (int m = 0; m < md1.mesh_count; m++) {
        const re15_md1_mesh_t *me = &md1.meshes[m];
        for (int i = 0; i < me->triangle_count; i++) {
            const re15_md1_tri_uv_t *uv = &me->triangle_uvs[i];
            uint8_t us[3] = { uv->u0, uv->u1, uv->u2 };
            uint8_t vs[3] = { uv->v0, uv->v1, uv->v2 };
            cover_face(&tim, clut_row, key_mode, us, vs, 3, (int)uv->page, &c);
        }
        for (int i = 0; i < me->quad_count; i++) {
            const re15_md1_quad_uv_t *uv = &me->quad_uvs[i];
            uint8_t us[4] = { uv->u0, uv->u1, uv->u2, uv->u3 };
            uint8_t vs[4] = { uv->v0, uv->v1, uv->v2, uv->v3 };
            cover_face(&tim, clut_row, key_mode, us, vs, 4, (int)uv->page, &c);
        }
    }
    *ok = 1;
    return c;
}

/* Gemessen 2026-08-21 auf shared_assets/PSX/STAGE1/ROOM11F0.RDT. */
#define CURSOR_SAMPLED      109042L
#define CURSOR_IDX0         101215L

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    size_t sz = 0;

    snprintf(path, sizeof path, "%s/STAGE1/ROOM11F0.RDT", base);
    uint8_t *dat = read_file(path, &sz);
    if (!dat) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(dat, sz, &rdt) != 0) { fprintf(stderr, "FAIL: RDT-parse\n"); free(dat); return 1; }

    /* ===== P1: der Cursor-Record ist der zitierte — Typ 4 und ABE-Feld 0 ============ */
    printf("===== P1: Obj_model_set-Record fuer Objekt 0x00 (@Datei 0x00E54) =====\n");
    {
        const uint8_t *pc = dat + 0x00E54;
        printf("   raw = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
               pc[0], pc[1], pc[2], pc[3], pc[4], pc[5], pc[6], pc[7], pc[8], pc[9]);
        CHECK(pc[0] == 0x2D, "kein Obj_model_set an 0x00E54 (op=0x%02X)", pc[0]);
        CHECK(pc[1] == 0x00, "obj_id=%u != 0 (LAB_80040914 @0x8004093c `lbu a3,1(a2)`)", pc[1]);
        CHECK(pc[2] == 0x04, "type=%u != 4 (obj+0x08, @0x80040940/@0x8004095c)", pc[2]);
        /* obj+0x0C = pc[8..9] (@0x800409a8 `lh v0,8(a2)` / @0x800409b0 `sw v0,12(a1)`).
         * FUN_8002c18c liest davon Bit 0x02 als ABE und Bit 0x10 als Draw-Variante. */
        int obj0c = (int)(int16_t)((uint16_t)pc[8] | ((uint16_t)pc[9] << 8));
        printf("   obj+0x0C = pc[8..9] = %d  -> ABE(&2)=%d  altpfad(&0x10)=%d\n",
               obj0c, (obj0c & 2) ? 1 : 0, (obj0c & 0x10) ? 1 : 0);
        CHECK(obj0c == 0, "obj+0x0C = %d != 0 — dann waere die ABE-Frage neu zu klaeren", obj0c);
        CHECK((obj0c & 2) == 0,
              "ABE-Bit gesetzt: der Primitiv-Code waere 0x36/0x3e, nicht 0x34/0x3c");
    }

    /* ===== P2/P3: Cursor-Textur unter Farbschluessel vs. Alt-Verhalten ============== */
    printf("\n===== P2/P3: prop[0] (Cursor) — gesampelte Texel =====\n");
    {
        int ok_psx = 0, ok_none = 0;
        cover_t psx  = cover_prop(&rdt, 0, RE15_TIM_KEY_PSX,  &ok_psx);
        cover_t none = cover_prop(&rdt, 0, RE15_TIM_KEY_NONE, &ok_none);
        CHECK(ok_psx && ok_none, "prop[0] TIM/MD1 nicht parsebar");
        printf("   gesampelt=%ld  idx0=%ld (%.1f%%)\n",
               psx.sampled, psx.idx0,
               psx.sampled ? 100.0 * (double)psx.idx0 / (double)psx.sampled : 0.0);
        printf("   KEY_PSX : transparent=%ld  opak-schwarz=%ld\n",
               psx.transparent, psx.opaque_black);
        printf("   KEY_NONE: transparent=%ld  opak-schwarz=%ld   (= der alte Zustand)\n",
               none.transparent, none.opaque_black);

        CHECK(psx.sampled == CURSOR_SAMPLED,
              "gesampelte Texel %ld != %ld — die Assets/UVs haben sich geaendert",
              psx.sampled, CURSOR_SAMPLED);
        CHECK(psx.idx0 == CURSOR_IDX0,
              "Index-0-Texel %ld != %ld", psx.idx0, CURSOR_IDX0);

        /* P2 — DER PIN: unter dem Farbschluessel ist der Grossteil der Cursor-Flaeche
         * durchsichtig, das Raumbild scheint also durch. */
        CHECK(psx.transparent >= CURSOR_IDX0,
              "nur %ld transparente Texel (SOLL >= %ld = alle Index-0-Texel, CLUT[0]=0x0000)",
              psx.transparent, CURSOR_IDX0);
        CHECK(psx.sampled > 0 && psx.transparent * 10 > psx.sampled * 9,
              "transparenter Anteil %.1f%% < 90%% — der Cursor waere weiter zugemalt",
              psx.sampled ? 100.0 * (double)psx.transparent / (double)psx.sampled : 0.0);

        /* P3 — NEGATIV: genau das tat der Port vorher. Wird der Fix zurueckgebaut
         * (Prop-Upload wieder mit RE15_TIM_KEY_NONE), ist DAS das Ergebnis. */
        CHECK(none.transparent == 0,
              "KEY_NONE liefert %ld transparente Texel — der Negativ-Fall stimmt nicht mehr",
              none.transparent);
        CHECK(none.opaque_black >= CURSOR_IDX0,
              "KEY_NONE malt nur %ld Texel opak schwarz (SOLL >= %ld)",
              none.opaque_black, CURSOR_IDX0);
    }

    /* ===== P4: Regressionswache — die 11 anderen Props aendern sich NICHT =========== */
    printf("\n===== P4: Regressionswache, prop[1..%d] =====\n", rdt.prop_count - 1);
    {
        int checked = 0;
        for (int op = 1; op < rdt.prop_count && op < RE15_RDT_MAX_PROPS; op++) {
            int ok_psx = 0, ok_none = 0;
            cover_t psx  = cover_prop(&rdt, op, RE15_TIM_KEY_PSX,  &ok_psx);
            cover_t none = cover_prop(&rdt, op, RE15_TIM_KEY_NONE, &ok_none);
            if (!ok_psx || !ok_none) continue;
            checked++;
            printf("   prop[%2d] gesampelt=%6ld idx0=%ld transparent(PSX)=%ld\n",
                   op, psx.sampled, psx.idx0, psx.transparent);
            CHECK(psx.transparent == 0,
                  "prop[%d]: %ld Texel wuerden neu transparent — Regression",
                  op, psx.transparent);
            CHECK(psx.transparent == none.transparent && psx.opaque_black == none.opaque_black,
                  "prop[%d]: Farbschluessel aendert das Bild (PSX %ld/%ld vs NONE %ld/%ld)",
                  op, psx.transparent, psx.opaque_black, none.transparent, none.opaque_black);
        }
        CHECK(checked == 11, "nur %d der 11 uebrigen Props geprueft", checked);
    }

    /* ===== P5: die drei historischen Modi bleiben als Negativ-Kontrolle definiert ==== */
    printf("\n===== P5: Produktionsregel vs. die zwei historischen Modi =====\n");
    {
        /* Produktionsregel (re15_tim_texel_argb, EINZIGES Argument = der aufgeloeste Wert) */
        CHECK(re15_tim_texel_argb(0x0000) == 0u,
              "die Regel muss den Texel-WERT 0x0000 keyen");
        CHECK(re15_tim_texel_argb(0x0001) == re15_tim_rgb555_to_argb8888(0x0001),
              "die Regel darf 0x0001 NICHT keyen (psx-spx: nur 0000h)");
        CHECK(re15_tim_texel_argb(0x8000) == 0xFF000000u,
              "0x8000 = STP-Schwarz ist bei opaken Befehlen NICHT-transparentes Schwarz");
        /* historische Modi — sie duerfen NUR noch in Tests vorkommen */
        CHECK(re15_tim_texel_argb_legacy(0x0000, 1, RE15_TIM_KEY_INDEX0) == 0u,
              "KEY_INDEX0: Index 0 transparent");
        CHECK(re15_tim_texel_argb_legacy(0x7FFF, 1, RE15_TIM_KEY_INDEX0) == 0u,
              "KEY_INDEX0 entscheidet am INDEX, nicht am CLUT-Wert — DAS riss die Loecher");
        CHECK(re15_tim_texel_argb_legacy(0x0000, 0, RE15_TIM_KEY_INDEX0) == 0xFF000000u,
              "KEY_INDEX0 keyt Wert 0x0000 bei Index != 0 nicht (Alt-Verhalten)");
        CHECK(re15_tim_texel_argb_legacy(0x0000, 1, RE15_TIM_KEY_NONE) == 0xFF000000u,
              "KEY_NONE laesst Index 0 opak schwarz — DAS war der schwarze Hintergrund");
        CHECK(re15_tim_texel_argb_legacy(0x0000, 0, RE15_TIM_KEY_PSX) == re15_tim_texel_argb(0x0000),
              "KEY_PSX muss deckungsgleich mit der Produktionsregel sein");
    }

    printf("\n===== BEFUND: %s (%d Fehler) =====\n", g_fail ? "DIVERGENT" : "OK", g_fail);
    free(dat);
    return g_fail ? 1 : 0;
}
