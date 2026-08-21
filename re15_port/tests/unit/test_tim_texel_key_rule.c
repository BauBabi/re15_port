/* test_tim_texel_key_rule.c — PIN: DIE Texel-Transparenz-REGEL (keine Slot-Liste)
 *
 * ===========================================================================================
 * WARUM ES DIESEN TEST GIBT
 * ===========================================================================================
 * Nutzer, 2026-08-21: "Die Flammen haben wieder schwarzen Hintergrund statt transparent zu
 * sein. Das hatten wir schon 40 mal gefuehlt. Da muss es etwas Globales geben, das du immer
 * setzen kannst."  Er hat recht: derselbe Fehler wurde dreimal EINZELN repariert
 *   0a4d4443  ROOM11F0-Cursor        -> Farbschluessel fuer die Prop-Slots nachgetragen
 *   905dcc78  Feuer-Slot 44          -> dort explizit RE15_TIM_KEY_PSX gesetzt
 *   96a4d51c  Blut-Textur (Bit 15)   -> Asset neu extrahiert
 * ...weil die Entscheidung als PRO-SLOT-SCHALTER modelliert war, mit Default "Index 0 opak".
 * Jeder neue Textur-Slot fiel damit auf schwarze Kloetze — zuletzt die Raum-ESP-Slots 36..43,
 * also die Flammen aus dem Report.
 *
 * ===========================================================================================
 * WAS DAS ORIGINAL TUT  (nocash psx-spx, info/Resident_Evil_und_Playstation_Information/
 *                        psx-spx.github.io-master/docs/graphicsprocessingunitgpu.md)
 * ===========================================================================================
 * Z.1128-1131 ("16bit Texture (Direct Color)") und Z.1157-1160 ("Texture Palettes - CLUT")
 * tragen WORTGLEICH dieselbe Tabelle — die Regel gilt also fuer Direktfarbe UND fuer
 * CLUT-aufgeloeste 4bpp/8bpp-Texel:
 *     Color 0000h        = Fully-transparent
 *     Color 0001h..7FFFh = Non-transparent
 *     Color 8000h..FFFFh = Semi-transparent (*)
 *     Bit15 Semi-transparency Flag   /(*) or Non-transparent for opaque commands
 * Z.1167-1176 ("Texture Color Black Limitations"): "On the PSX, texture color 0000h is
 * fully-transparent, that means textures cannot contain Black pixels. However, in some cases,
 * Color 8000h (Black with semi-transparent flag) can be used, depending on the rendering
 * command:  opaque command, eg. GP0(24h) --> 8000h = Non-Transparent Black".
 *
 * DARAUS: der Schluessel haengt am aufgeloesten WERT und an NICHTS SONST — nicht am
 * Paletten-Index, nicht am Slot, nicht am Primitiv. Das ABE-Bit des Zeichenbefehls
 * (RE1.5: FUN_800254a0 `cd = param_4<<1|0x34` POLY_GT3 / FUN_800256b0 `|0x3c` POLY_GT4,
 * Bit 0x02) betrifft ausschliesslich 8000h..FFFFh und ist im Port der Blend-Modus der
 * Zeichen-Queue, nicht der Texel-Dekoder. Eine Liste gibt es im Original NICHT.
 *
 * ===========================================================================================
 * WAS HIER GEPINNT WIRD
 * ===========================================================================================
 *   R1  Die Regel selbst (`re15_tim_texel_argb`, EIN Argument = der aufgeloeste Wert):
 *       0000h transparent, 0001h/7FFFh sichtbar, 8000h OPAKES Schwarz (nicht transparent).
 *   R2  "Neue Textur ohne Eintrag" — eine frei erfundene TIM mit CLUT[0]=0x0000 dekodiert
 *       transparent, ohne dass irgendwo ein Slot registriert waere.
 *   R3  DER FLAMMEN-FALL, byte-genau auf den Asset-Bytes: ROOM1090 Effekt-Id 0x10 (Raum-Bank,
 *       Render-Slot 36+3) — 28103 von 36864 Texeln sind Wert 0x0000. Dazu die NEGATIV-
 *       Kontrolle: unter dem alten Default waeren das 0 transparente und 28103 opak-schwarze
 *       Texel = der gemeldete schwarze Hintergrund.
 *   R4  GAME-WEIT statt Liste: JEDE Effekt-TIM JEDES RDT hat unter der Regel einen
 *       transparenten Anteil > 0, ohne dass eine einzige davon irgendwo eingetragen ist.
 *   R5  REGRESSIONSWACHE MODELLTEXTUREN (der Gegen-Fall "dreieckige Loecher"): ueber die
 *       tatsaechlich vom MD1 gesampelte Flaeche aendert die WERT-Regel bei den Spieler-
 *       modellen exakt NICHTS. Und der Kontrast: der historische INDEX-Schalter haette dort
 *       sichtbare Texel geloescht — DAS war der Loecher-Mechanismus, nicht der Farbschluessel.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_esp.h"

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

static int tim_index_at(const re15_tim_t *t, int x, int y)
{
    const uint8_t *src = (const uint8_t *)t->pixels;
    long i = (long)y * t->width + x;
    if (t->bpp == 8) return src[i];
    return (i & 1) ? (src[i >> 1] >> 4) : (src[i >> 1] & 0xF);
}

/* Ein ganzes Sheet unter der Produktionsregel dekodieren (CLUT-Zeile 0 — die Effekt-TIMs des
 * Spiels haben alle genau eine CLUT). Liefert transparente / opak-schwarze Texel. */
typedef struct { long n, clear, black, idx0; } sheet_t;

static sheet_t decode_sheet(const re15_tim_t *t, int legacy_mode /* -1 = Produktionsregel */)
{
    sheet_t s = {0,0,0,0};
    s.n = (long)t->width * t->height;
    for (long i = 0; i < s.n; i++) {
        int x = (int)(i % t->width), y = (int)(i / t->width);
        int idx = (t->bpp == 16) ? 0 : tim_index_at(t, x, y);
        uint16_t raw = (t->bpp == 16) ? t->pixels[i] : t->clut[idx];
        uint32_t argb = (legacy_mode < 0) ? re15_tim_texel_argb(raw)
                                          : re15_tim_texel_argb_legacy(raw, idx == 0, legacy_mode);
        if (idx == 0) s.idx0++;
        if (argb == 0u) s.clear++;
        else if (argb == 0xFF000000u) s.black++;
    }
    return s;
}

/* Die vom MD1 tatsaechlich gesampelte Texturflaeche (UV-Bounding-Box je Face + Seiten-Offset,
 * wie der Renderer sie aufspannt), mit der ECHTEN CLUT-Wahl pro Face —
 * clut_idx = ((uv->clut >> 6) & 0x1FF) - tim.clut_y   (render_pc.c:2056-2058). */
typedef struct { long sampled, clear_value, clear_index; } cov_t;

static const uint16_t *clut_row_for(const re15_tim_t *t, unsigned clut_word)
{
    int row_width = (t->bpp == 4) ? 16 : 256;
    int n_cluts   = (t->has_clut && row_width > 0) ? (t->clut_entries / row_width) : 1;
    if (n_cluts < 1) n_cluts = 1;
    int idx = (int)((clut_word >> 6) & 0x1FF) - t->clut_y;
    if (idx < 0 || idx >= n_cluts) idx = 0;
    return &t->clut[(long)idx * row_width];
}

static void cover_face(const re15_tim_t *tim, const uint8_t *us, const uint8_t *vs, int n,
                       unsigned page, unsigned clut_word, cov_t *acc)
{
    const uint16_t *cr = clut_row_for(tim, clut_word);
    int u0 = us[0], u1 = us[0], v0 = vs[0], v1 = vs[0];
    for (int k = 1; k < n; k++) {
        if (us[k] < u0) u0 = us[k];
        if (us[k] > u1) u1 = us[k];
        if (vs[k] < v0) v0 = vs[k];
        if (vs[k] > v1) v1 = vs[k];
    }
    int po = (int)(page & 0x000F) * 128;
    for (int y = v0; y <= v1; y++) {
        for (int x = u0; x <= u1; x++) {
            int X = x + po;
            if (X >= tim->width || y >= tim->height) continue;
            int idx = tim_index_at(tim, X, y);
            uint16_t raw = cr[idx];
            acc->sampled++;
            if (re15_tim_texel_argb(raw) == 0u) acc->clear_value++;
            if (re15_tim_texel_argb_legacy(raw, idx == 0, RE15_TIM_KEY_INDEX0) == 0u)
                acc->clear_index++;
        }
    }
}

static cov_t cover_md1(const re15_md1_t *md1, const re15_tim_t *tim)
{
    cov_t c = {0,0,0};
    for (int m = 0; m < md1->mesh_count; m++) {
        const re15_md1_mesh_t *me = &md1->meshes[m];
        for (int i = 0; i < me->triangle_count; i++) {
            const re15_md1_tri_uv_t *uv = &me->triangle_uvs[i];
            uint8_t us[3] = { uv->u0, uv->u1, uv->u2 };
            uint8_t vs[3] = { uv->v0, uv->v1, uv->v2 };
            cover_face(tim, us, vs, 3, uv->page, uv->clut, &c);
        }
        for (int i = 0; i < me->quad_count; i++) {
            const re15_md1_quad_uv_t *uv = &me->quad_uvs[i];
            uint8_t us[4] = { uv->u0, uv->u1, uv->u2, uv->u3 };
            uint8_t vs[4] = { uv->v0, uv->v1, uv->v2, uv->v3 };
            cover_face(tim, us, vs, 4, uv->page, uv->clut, &c);
        }
    }
    return c;
}

/* --- Messwerte 2026-08-21 auf re15_port/shared_assets/PSX -------------------------------- */
#define FLAME_TEXELS        36864L   /* ROOM1090 Effekt-Id 0x10, 4bpp 256x144               */
#define FLAME_VALUE0        28103L   /* davon Wert 0x0000 = auf der PSX gar nicht gezeichnet */
#define LEON_SAMPLED       117197L
#define ELZA_SAMPLED       133212L

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    size_t sz = 0;

    /* ===== R1: DIE REGEL ============================================================== */
    printf("===== R1: die Regel (psx-spx-Tabelle, Z.1128-1131 / Z.1157-1160) =====\n");
    {
        CHECK(re15_tim_texel_argb(0x0000) == 0u,
              "0000h muss 'fully-transparent' sein");
        CHECK(re15_tim_texel_argb(0x0001) == re15_tim_rgb555_to_argb8888(0x0001),
              "0001h ist 'Non-transparent' — darf NICHT gekeyt werden");
        CHECK(re15_tim_texel_argb(0x7FFF) == re15_tim_rgb555_to_argb8888(0x7FFF),
              "7FFFh ist 'Non-transparent'");
        /* 8000h = Schwarz MIT STP-Flag. Bei opaken Zeichenbefehlen (RE1.5 setzt 0x34/0x3c,
         * ABE-Bit 0x02 aus) ist das NICHT-transparentes Schwarz — der Farbschluessel greift
         * hier ausdruecklich nicht. */
        CHECK(re15_tim_texel_argb(0x8000) == 0xFF000000u,
              "8000h muss opakes Schwarz bleiben (psx-spx: 'Non-Transparent Black')");
        CHECK(re15_tim_texel_argb(0xFFFF) != 0u,
              "FFFFh ist nicht transparent");
        printf("   0000h->%08X  0001h->%08X  7FFFh->%08X  8000h->%08X\n",
               re15_tim_texel_argb(0x0000), re15_tim_texel_argb(0x0001),
               re15_tim_texel_argb(0x7FFF), re15_tim_texel_argb(0x8000));
    }

    /* ===== R2: eine NEUE, nirgends registrierte Textur ist automatisch richtig ======== */
    printf("\n===== R2: frei erfundene TIM, kein Slot-Eintrag, kein Modus =====\n");
    {
        /* So wuerde ein neues Effekt-Sheet aussehen: CLUT[0] = 0x0000 (der uebliche
         * "Hintergrund"), CLUT[1..] = Farben. Ohne jede Registrierung muss Index 0
         * transparent und alles andere sichtbar dekodieren. */
        uint16_t clut[16] = { 0x0000, 0x001F, 0x03E0, 0x7C00, 0x7FFF, 0x8000,
                              0x0421, 0x1234, 0, 0, 0, 0, 0, 0, 0, 0 };
        long clear = 0, drawn = 0;
        for (int idx = 0; idx < 8; idx++) {
            if (re15_tim_texel_argb(clut[idx]) == 0u) clear++; else drawn++;
        }
        CHECK(clear == 1 && drawn == 7,
              "neue TIM: %ld transparent / %ld sichtbar (SOLL 1/7 — nur CLUT[0]=0x0000)",
              clear, drawn);
        CHECK(re15_tim_texel_argb(clut[5]) == 0xFF000000u,
              "CLUT[5]=0x8000 muss als opakes Schwarz durchkommen");
        printf("   ok: genau der 0x0000-Eintrag faellt weg, ohne jeden Zusatzeintrag\n");
    }

    /* ===== R3: DER FLAMMEN-FALL, byte-genau ========================================== */
    printf("\n===== R3: ROOM1090 Effekt-Id 0x10 (die Flammen aus dem Report) =====\n");
    {
        snprintf(path, sizeof path, "%s/STAGE1/ROOM1090.RDT", base);
        uint8_t *b = read_file(path, &sz);
        CHECK(b != NULL, "ROOM1090.RDT nicht lesbar (%s)", path);
        if (b) {
            #define U32LE(o) ((uint32_t)b[o] | ((uint32_t)b[(o)+1]<<8) | \
                              ((uint32_t)b[(o)+2]<<16) | ((uint32_t)b[(o)+3]<<24))
            uint32_t idh = U32LE(0x4C), pe = U32LE(0x50), tb = U32LE(0x54), te = U32LE(0x58);
            #undef U32LE
            static re15_esp_t esp;
            CHECK(re15_esp_parse(b, sz, idh, pe, tb, te, &esp) == 0, "ESP-Parse ROOM1090");
            int flame_ei = -1;
            for (int ei = 0; ei < esp.id_count; ei++)
                if (esp.eff[ei].effect_id == 0x10) flame_ei = ei;
            CHECK(flame_ei == 3,
                  "Effekt-Id 0x10 steht auf Index %d (erwartet 3 -> Render-Slot 36+3 = 39)",
                  flame_ei);
            if (flame_ei >= 0) {
                re15_tim_t tim;
                CHECK(re15_tim_parse(b + esp.eff[flame_ei].tim_off,
                                     sz - esp.eff[flame_ei].tim_off, &tim) == 0,
                      "Flammen-TIM nicht parsebar");
                sheet_t now  = decode_sheet(&tim, -1);                     /* Produktionsregel */
                sheet_t was  = decode_sheet(&tim, RE15_TIM_KEY_NONE);      /* alter Default    */
                printf("   Sheet %dx%d %dbpp = %ld Texel\n",
                       tim.width, tim.height, tim.bpp, now.n);
                printf("   REGEL    : transparent=%ld (%.1f%%)  opak-schwarz=%ld\n",
                       now.clear, 100.0*(double)now.clear/(double)now.n, now.black);
                printf("   ALT/KEY_NONE: transparent=%ld  opak-schwarz=%ld  (= der Bug)\n",
                       was.clear, was.black);
                CHECK(now.n == FLAME_TEXELS, "Sheet hat %ld statt %ld Texel", now.n, FLAME_TEXELS);
                CHECK(now.clear == FLAME_VALUE0,
                      "%ld transparente Texel statt %ld", now.clear, FLAME_VALUE0);
                CHECK(now.black == 0,
                      "%ld Texel bleiben opak schwarz — der Hintergrund waere weiter zu",
                      now.black);
                /* NEGATIV: genau das tat der Port vor dem Fix. */
                CHECK(was.clear == 0 && was.black == FLAME_VALUE0,
                      "Negativ-Kontrolle stimmt nicht mehr (alt: %ld transparent / %ld schwarz)",
                      was.clear, was.black);
            }
            free(b);
        }
    }

    /* ===== R4: GAME-WEIT — jede Effekt-TIM, ohne einen einzigen Listeneintrag ========= */
    printf("\n===== R4: alle Effekt-Sheets aller RDTs unter derselben Regel =====\n");
    {
        long rooms = 0, sheets = 0, sheets_ok = 0, worst_black = 0;
        char worst[64] = "-";
        for (int st = 1; st <= 6; st++) {
            for (int r = 0; r < 0x100; r++) {
                snprintf(path, sizeof path, "%s/STAGE%d/ROOM%d%02X0.RDT", base, st, st, r);
                uint8_t *b = read_file(path, &sz);
                if (!b) continue;
                rooms++;
                #define U32LE(o) ((uint32_t)b[o] | ((uint32_t)b[(o)+1]<<8) | \
                                  ((uint32_t)b[(o)+2]<<16) | ((uint32_t)b[(o)+3]<<24))
                uint32_t idh = U32LE(0x4C), pe = U32LE(0x50), tb = U32LE(0x54), te = U32LE(0x58);
                #undef U32LE
                static re15_esp_t esp;
                if (re15_esp_parse(b, sz, idh, pe, tb, te, &esp) == 0) {
                    for (int ei = 0; ei < esp.id_count; ei++) {
                        if (!esp.eff[ei].tim_off || (uint32_t)sz <= esp.eff[ei].tim_off) continue;
                        re15_tim_t tim;
                        if (re15_tim_parse(b + esp.eff[ei].tim_off, sz - esp.eff[ei].tim_off,
                                           &tim) != 0) continue;
                        sheets++;
                        sheet_t s = decode_sheet(&tim, -1);
                        if (s.clear > 0) sheets_ok++;
                        if (s.black > worst_black) {
                            worst_black = s.black;
                            snprintf(worst, sizeof worst, "ROOM%d%02X0 eff[%d] id=0x%02X",
                                     st, r, ei, esp.eff[ei].effect_id);
                        }
                    }
                }
                free(b);
            }
        }
        printf("   %ld RDTs, %ld Effekt-Sheets, %ld davon mit transparenten Texeln\n",
               rooms, sheets, sheets_ok);
        printf("   groesster opak-schwarzer Anteil: %ld Texel (%s)\n", worst_black, worst);
        CHECK(rooms >= 100, "nur %ld RDTs gefunden — Asset-Baum unvollstaendig?", rooms);
        CHECK(sheets > 0 && sheets == sheets_ok,
              "%ld von %ld Effekt-Sheets haben KEIN transparentes Texel — dort waere wieder "
              "ein schwarzer Klotz", sheets - sheets_ok, sheets);
    }

    /* ===== R5: REGRESSIONSWACHE — Modelltexturen bekommen KEINE Loecher =============== */
    printf("\n===== R5: Gegen-Fall Modelltexturen (die 'dreieckigen Loecher') =====\n");
    {
        static const struct { const char *md1, *tim, *tag; long sampled; } mods[] = {
            { "PLD/PL00.MD1", "PLD/PL00.TIM", "Leon PL00", LEON_SAMPLED },
            { "PLD/PL04.MD1", "PLD/PL04.TIM", "Elza PL04", ELZA_SAMPLED },
        };
        for (size_t i = 0; i < sizeof(mods)/sizeof(mods[0]); i++) {
            size_t msz = 0, tsz = 0;
            snprintf(path, sizeof path, "%s/%s", base, mods[i].md1);
            uint8_t *mb = read_file(path, &msz);
            snprintf(path, sizeof path, "%s/%s", base, mods[i].tim);
            uint8_t *tb = read_file(path, &tsz);
            CHECK(mb && tb, "%s: Asset fehlt", mods[i].tag);
            if (!mb || !tb) { free(mb); free(tb); continue; }
            static re15_md1_t md1; re15_tim_t tim;
            if (re15_md1_parse(mb, msz, &md1) == 0 && re15_tim_parse(tb, tsz, &tim) == 0) {
                cov_t c = cover_md1(&md1, &tim);
                printf("   %-10s gesampelt=%7ld  WERT-Regel loescht %ld  |  "
                       "INDEX-Schalter haette %ld geloescht\n",
                       mods[i].tag, c.sampled, c.clear_value, c.clear_index);
                CHECK(c.sampled == mods[i].sampled,
                      "%s: %ld gesampelte Texel statt %ld — Assets/UVs geaendert",
                      mods[i].tag, c.sampled, mods[i].sampled);
                /* DER PIN: die byte-true WERT-Regel loescht an der Spielerhaut NICHTS. */
                CHECK(c.clear_value == 0,
                      "%s: die Regel wuerde %ld gesampelte Texel loeschen — LOECHER",
                      mods[i].tag, c.clear_value);
            }
            free(mb); free(tb);
        }
        /* Der Kontrast, auf einer Textur, die Index 0 als ECHTE FARBE benutzt: ELLIOT.TIM.
         * Unter der Wert-Regel bleibt alles stehen, unter dem historischen INDEX-Schalter
         * verschwinden sichtbare Texel — das war der Loecher-Mechanismus. */
        size_t msz = 0, tsz = 0;
        snprintf(path, sizeof path, "%s/PLD/ELLIOT.MD1", base);
        uint8_t *mb = read_file(path, &msz);
        snprintf(path, sizeof path, "%s/PLD/ELLIOT.TIM", base);
        uint8_t *tb = read_file(path, &tsz);
        if (mb && tb) {
            static re15_md1_t md1; re15_tim_t tim;
            if (re15_md1_parse(mb, msz, &md1) == 0 && re15_tim_parse(tb, tsz, &tim) == 0) {
                cov_t c = cover_md1(&md1, &tim);
                printf("   %-10s gesampelt=%7ld  WERT-Regel loescht %ld  |  "
                       "INDEX-Schalter haette %ld geloescht\n",
                       "Elliot", c.sampled, c.clear_value, c.clear_index);
                CHECK(c.clear_value == 0,
                      "Elliot: die Regel loescht %ld Texel", c.clear_value);
                CHECK(c.clear_index > 0,
                      "Elliot: der INDEX-Schalter muesste hier sichtbare Texel loeschen — "
                      "sonst stimmt die Erklaerung der 'Loecher' nicht mehr");
            }
        }
        free(mb); free(tb);
    }

    printf("\n===== BEFUND: %s (%d Fehler) =====\n", g_fail ? "DIVERGENT" : "OK", g_fail);
    return g_fail ? 1 : 0;
}
