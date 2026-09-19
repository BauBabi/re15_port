/* Siehe touch_overlay_pc.h fuer das WARUM, das Layout und die Schalter. */
#include "touch_overlay_pc.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* PSX-Pad-Bits — dasselbe Wort wie input_pc.c / include/re15_player.h (RE15_PAD_BIT_*). */
#define TP_SELECT   0x0001
#define TP_START    0x0008
#define TP_UP       0x0010
#define TP_RIGHT    0x0020
#define TP_DOWN     0x0040
#define TP_LEFT     0x0080
#define TP_L1       0x0400
#define TP_R1       0x0800
#define TP_TRIANGLE 0x1000
#define TP_CIRCLE   0x2000
#define TP_CROSS    0x4000
#define TP_SQUARE   0x8000

/* ------------------------------------------------------------------------------ Zustand */

static int           s_enabled  = -1;      /* -1 = noch nicht aufgeloest */
static SDL_Renderer *s_r        = NULL;
static int           s_marke    = 0;       /* F9-Flanke, wird von take_marke() verbraucht */

#define TP_MAX_FINGERS 10
typedef struct { int active; SDL_FingerID id; SDL_TouchID dev; float x, y; } finger_t;
static finger_t s_fingers[TP_MAX_FINGERS];

/* Knopf-Arten */
enum { K_RECT = 0, K_FACE = 1, K_DPAD = 2, K_MARKE = 3 };

typedef struct {
    int         kind;
    int         x, y, w, h;      /* Rechteck (K_RECT/K_MARKE/K_DPAD-Hitzone) bzw. Umriss */
    int         cx, cy, r;       /* Kreis (K_FACE) */
    uint16_t    bit;
    const char *label;
} btn_t;

#define TP_MAX_BTNS 12
static btn_t s_btn[TP_MAX_BTNS];
static int   s_btn_n   = 0;
static int   s_lay_w   = -1, s_lay_h = -1;
static int   s_unit    = 0;            /* u = H/12 */
static int   s_dpad_cx = 0, s_dpad_cy = 0, s_dpad_arm = 0, s_dpad_dead = 0;

/* ------------------------------------------------------------------------------ Schalter */

static int tp_resolve_enabled(void)
{
    const char *e = getenv("RE15_TOUCH_OVERLAY");
#if defined(__ANDROID__)
    return !(e && e[0] == '0');           /* Android: an, ausser ausdruecklich 0 */
#else
    return (e && e[0] == '1') ? 1 : 0;    /* Desktop: aus, ausser ausdruecklich 1 */
#endif
}

int re15_touch_pc_enabled(void)
{
    if (s_enabled < 0) s_enabled = tp_resolve_enabled();
    return s_enabled;
}

void re15_touch_pc_preinit(void)
{
    if (!re15_touch_pc_enabled()) return;
#if !defined(__ANDROID__)
    /* Desktop-Pruefweg: die Maus erzeugt Finger-Events (ein Finger). Muss VOR SDL_Init
     * gesetzt sein — SDL liest den Hint bei der Maus-Initialisierung. */
    SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "1");
#endif
}

/* ------------------------------------------------------------------------------ Layout */

static void tp_add_rect(int kind, int x, int y, int w, int h, uint16_t bit, const char *label)
{
    if (s_btn_n >= TP_MAX_BTNS) return;
    btn_t *b = &s_btn[s_btn_n++];
    memset(b, 0, sizeof *b);
    b->kind = kind; b->x = x; b->y = y; b->w = w; b->h = h; b->bit = bit; b->label = label;
    b->cx = x + w / 2; b->cy = y + h / 2; b->r = (w < h ? w : h) / 2;
}

static void tp_add_face(int cx, int cy, int r, uint16_t bit, const char *label)
{
    if (s_btn_n >= TP_MAX_BTNS) return;
    btn_t *b = &s_btn[s_btn_n++];
    memset(b, 0, sizeof *b);
    b->kind = K_FACE; b->cx = cx; b->cy = cy; b->r = r; b->bit = bit; b->label = label;
    b->x = cx - r; b->y = cy - r; b->w = 2 * r; b->h = 2 * r;
}

static void tp_layout(int W, int H)
{
    if (W == s_lay_w && H == s_lay_h) return;
    s_lay_w = W; s_lay_h = H;
    s_btn_n = 0;
    int u = H / 12; if (u < 8) u = 8;
    s_unit = u;

    /* D-Pad links unten: Hitzone ein Quadrat, Richtung aus dem Winkel zum Zentrum. */
    s_dpad_cx  = (int)(2.7f * u);
    s_dpad_cy  = H - (int)(3.1f * u);
    s_dpad_arm = (int)(2.2f * u);            /* Armlaenge vom Zentrum */
    s_dpad_dead = (int)(0.45f * u);
    {
        int R = (int)(2.6f * u);
        tp_add_rect(K_DPAD, s_dpad_cx - R, s_dpad_cy - R, 2 * R, 2 * R, 0, NULL);
    }

    /* Face-Buttons rechts unten: Dreieck oben, Kreis rechts, Kreuz unten, Viereck links. */
    {
        int fcx = W - (int)(2.9f * u), fcy = H - (int)(3.1f * u);
        int off = (int)(1.55f * u), r = (int)(0.8f * u);
        tp_add_face(fcx,       fcy - off, r, TP_TRIANGLE, NULL);
        tp_add_face(fcx + off, fcy,       r, TP_CIRCLE,   NULL);
        tp_add_face(fcx,       fcy + off, r, TP_CROSS,    NULL);
        tp_add_face(fcx - off, fcy,       r, TP_SQUARE,   NULL);
    }

    /* Schultern oben links/rechts. */
    tp_add_rect(K_RECT, (int)(0.4f * u),              (int)(0.4f * u), (int)(2.6f * u), (int)(1.15f * u), TP_L1, "L1");
    tp_add_rect(K_RECT, W - (int)(0.4f * u) - (int)(2.6f * u), (int)(0.4f * u), (int)(2.6f * u), (int)(1.15f * u), TP_R1, "R1");

    /* SELECT / START unten Mitte (Breite 3u: "SELECT" = 6 Zeichen a 6*fs bei fs = u/16). */
    {
        int bw = (int)(3.0f * u), bh = (int)(0.9f * u), gap = (int)(0.6f * u);
        int y = H - bh - (int)(0.35f * u);
        tp_add_rect(K_RECT, W / 2 - gap / 2 - bw, y, bw, bh, TP_SELECT, "SELECT");
        tp_add_rect(K_RECT, W / 2 + gap / 2,      y, bw, bh, TP_START,  "START");
    }

    /* F9-MARKE oben Mitte (klein). */
    tp_add_rect(K_MARKE, W / 2 - (int)(1.0f * u), (int)(0.3f * u), (int)(2.0f * u), (int)(0.8f * u), 0, "F9");
}

static void tp_output_size(int *W, int *H)
{
    *W = 0; *H = 0;
    if (s_r) SDL_GetRendererOutputSize(s_r, W, H);
    if (*W <= 0 || *H <= 0) { *W = 1280; *H = 720; }
}

/* ------------------------------------------------------------------------------ Treffer */

static int tp_hit(const btn_t *b, float x, float y)
{
    if (b->kind == K_FACE) {
        float dx = x - (float)b->cx, dy = y - (float)b->cy;
        return (dx * dx + dy * dy) <= (float)(b->r * b->r) * 1.15f;   /* etwas Rand */
    }
    return x >= (float)b->x && x < (float)(b->x + b->w) &&
           y >= (float)b->y && y < (float)(b->y + b->h);
}

/* 8 Sektoren um das D-Pad-Zentrum: 0=RECHTS, 1=OBEN+RECHTS, 2=OBEN, ... (gegen den Uhrzeiger) */
static uint16_t tp_dpad_bits(float x, float y)
{
    float dx = x - (float)s_dpad_cx, dy = (float)s_dpad_cy - y;   /* dy nach oben positiv */
    float r2 = dx * dx + dy * dy;
    if (r2 < (float)(s_dpad_dead * s_dpad_dead)) return 0;
    double ang = atan2((double)dy, (double)dx) * 180.0 / 3.14159265358979323846;
    if (ang < 0) ang += 360.0;
    int sec = (int)floor((ang + 22.5) / 45.0) & 7;
    static const uint16_t tbl[8] = {
        TP_RIGHT, TP_UP | TP_RIGHT, TP_UP, TP_UP | TP_LEFT,
        TP_LEFT,  TP_DOWN | TP_LEFT, TP_DOWN, TP_DOWN | TP_RIGHT
    };
    return tbl[sec];
}

static uint16_t tp_bits_for_point(float x, float y)
{
    uint16_t bits = 0;
    for (int i = 0; i < s_btn_n; i++) {
        const btn_t *b = &s_btn[i];
        if (!tp_hit(b, x, y)) continue;
        if (b->kind == K_DPAD)       bits |= tp_dpad_bits(x, y);
        else if (b->kind != K_MARKE) bits |= b->bit;
    }
    return bits;
}

uint16_t re15_touch_pc_pad_bits(void)
{
    if (!re15_touch_pc_enabled() || !s_r) return 0;
    int W, H; tp_output_size(&W, &H);
    tp_layout(W, H);
    uint16_t bits = 0;
    for (int i = 0; i < TP_MAX_FINGERS; i++)
        if (s_fingers[i].active) bits |= tp_bits_for_point(s_fingers[i].x, s_fingers[i].y);
    return bits;
}

int re15_touch_pc_take_marke(void)
{
    int m = s_marke; s_marke = 0; return m;
}

/* ------------------------------------------------------------------------------ Events */

static finger_t *tp_find(SDL_TouchID dev, SDL_FingerID id)
{
    for (int i = 0; i < TP_MAX_FINGERS; i++)
        if (s_fingers[i].active && s_fingers[i].id == id && s_fingers[i].dev == dev) return &s_fingers[i];
    return NULL;
}

static void tp_finger_down(SDL_TouchID dev, SDL_FingerID id, float nx, float ny)
{
    int W, H; tp_output_size(&W, &H);
    tp_layout(W, H);
    float x = nx * (float)W, y = ny * (float)H;
    finger_t *f = tp_find(dev, id);
    if (!f) for (int i = 0; i < TP_MAX_FINGERS; i++) if (!s_fingers[i].active) { f = &s_fingers[i]; break; }
    if (!f) return;
    f->active = 1; f->id = id; f->dev = dev; f->x = x; f->y = y;
    /* F9-MARKE ist eine Flanke: nur beim Aufsetzen, nie beim Halten. */
    for (int i = 0; i < s_btn_n; i++)
        if (s_btn[i].kind == K_MARKE && tp_hit(&s_btn[i], x, y)) s_marke = 1;
}

static void tp_finger_move(SDL_TouchID dev, SDL_FingerID id, float nx, float ny)
{
    int W, H; tp_output_size(&W, &H);
    finger_t *f = tp_find(dev, id);
    if (!f) { tp_finger_down(dev, id, nx, ny); return; }
    f->x = nx * (float)W; f->y = ny * (float)H;
}

static void tp_finger_up(SDL_TouchID dev, SDL_FingerID id)
{
    finger_t *f = tp_find(dev, id);
    if (f) f->active = 0;
}

static void tp_release_all(void)
{
    for (int i = 0; i < TP_MAX_FINGERS; i++) s_fingers[i].active = 0;
}

void re15_touch_pc_event(const SDL_Event *e)
{
    if (!e || !re15_touch_pc_enabled()) return;
    switch (e->type) {
    case SDL_FINGERDOWN:   tp_finger_down(e->tfinger.touchId, e->tfinger.fingerId, e->tfinger.x, e->tfinger.y); break;
    case SDL_FINGERMOTION: tp_finger_move(e->tfinger.touchId, e->tfinger.fingerId, e->tfinger.x, e->tfinger.y); break;
    case SDL_FINGERUP:     tp_finger_up(e->tfinger.touchId, e->tfinger.fingerId); break;
    /* Fokus weg / App in den Hintergrund: kein Finger darf "haengen" bleiben. */
    case SDL_APP_WILLENTERBACKGROUND:
    case SDL_APP_DIDENTERBACKGROUND: tp_release_all(); break;
    case SDL_WINDOWEVENT:
        if (e->window.event == SDL_WINDOWEVENT_FOCUS_LOST ||
            e->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) tp_release_all();
        break;
    default: break;
    }
}

/* ------------------------------------------------------------------------------ Mini-Schrift */

/* 5x7-Bitmuster, MSB = linke Spalte. Reihenfolge: Leerzeichen, 0-9, A-Z, dann '%' '.' '/' '-' ':' '(' ')'. */
static const char *const s_font[] = {
    "00000","00000","00000","00000","00000","00000","00000",   /* ' ' */
    "01110","10001","10011","10101","11001","10001","01110",   /* 0 */
    "00100","01100","00100","00100","00100","00100","01110",   /* 1 */
    "01110","10001","00001","00010","00100","01000","11111",   /* 2 */
    "11111","00010","00100","00010","00001","10001","01110",   /* 3 */
    "00010","00110","01010","10010","11111","00010","00010",   /* 4 */
    "11111","10000","11110","00001","00001","10001","01110",   /* 5 */
    "00110","01000","10000","11110","10001","10001","01110",   /* 6 */
    "11111","00001","00010","00100","01000","01000","01000",   /* 7 */
    "01110","10001","10001","01110","10001","10001","01110",   /* 8 */
    "01110","10001","10001","01111","00001","00010","01100",   /* 9 */
    "01110","10001","10001","11111","10001","10001","10001",   /* A */
    "11110","10001","10001","11110","10001","10001","11110",   /* B */
    "01110","10001","10000","10000","10000","10001","01110",   /* C */
    "11100","10010","10001","10001","10001","10010","11100",   /* D */
    "11111","10000","10000","11110","10000","10000","11111",   /* E */
    "11111","10000","10000","11110","10000","10000","10000",   /* F */
    "01110","10001","10000","10111","10001","10001","01111",   /* G */
    "10001","10001","10001","11111","10001","10001","10001",   /* H */
    "01110","00100","00100","00100","00100","00100","01110",   /* I */
    "00111","00010","00010","00010","00010","10010","01100",   /* J */
    "10001","10010","10100","11000","10100","10010","10001",   /* K */
    "10000","10000","10000","10000","10000","10000","11111",   /* L */
    "10001","11011","10101","10101","10001","10001","10001",   /* M */
    "10001","10001","11001","10101","10011","10001","10001",   /* N */
    "01110","10001","10001","10001","10001","10001","01110",   /* O */
    "11110","10001","10001","11110","10000","10000","10000",   /* P */
    "01110","10001","10001","10001","10101","10010","01101",   /* Q */
    "11110","10001","10001","11110","10100","10010","10001",   /* R */
    "01111","10000","10000","01110","00001","00001","11110",   /* S */
    "11111","00100","00100","00100","00100","00100","00100",   /* T */
    "10001","10001","10001","10001","10001","10001","01110",   /* U */
    "10001","10001","10001","10001","10001","01010","00100",   /* V */
    "10001","10001","10001","10101","10101","10101","01010",   /* W */
    "10001","10001","01010","00100","01010","10001","10001",   /* X */
    "10001","10001","10001","01010","00100","00100","00100",   /* Y */
    "11111","00001","00010","00100","01000","10000","11111",   /* Z */
    "11001","11010","00010","00100","01000","01011","10011",   /* % */
    "00000","00000","00000","00000","00000","01100","01100",   /* . */
    "00001","00010","00010","00100","01000","01000","10000",   /* / */
    "00000","00000","00000","11111","00000","00000","00000",   /* - */
    "00000","01100","01100","00000","01100","01100","00000",   /* : */
    "00010","00100","01000","01000","01000","00100","00010",   /* ( */
    "01000","00100","00010","00010","00010","00100","01000",   /* ) */
};

static int tp_glyph_index(char c)
{
    if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
    if (c == ' ') return 0;
    if (c >= '0' && c <= '9') return 1 + (c - '0');
    if (c >= 'A' && c <= 'Z') return 11 + (c - 'A');
    switch (c) {
        case '%': return 37; case '.': return 38; case '/': return 39; case '-': return 40;
        case ':': return 41; case '(': return 42; case ')': return 43;
        default:  return 0;
    }
}

int re15_touch_pc_text(SDL_Renderer *r, int x, int y, int scale, const char *s,
                       uint8_t cr, uint8_t cg, uint8_t cb, uint8_t ca)
{
    if (!r || !s) return 0;
    if (scale < 1) scale = 1;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, cr, cg, cb, ca);
    int penx = x;
    for (; *s; s++) {
        int g = tp_glyph_index(*s);
        const char *const *rows = &s_font[g * 7];
        for (int ry = 0; ry < 7; ry++)
            for (int rx = 0; rx < 5; rx++)
                if (rows[ry][rx] == '1') {
                    SDL_Rect px = { penx + rx * scale, y + ry * scale, scale, scale };
                    SDL_RenderFillRect(r, &px);
                }
        penx += 6 * scale;
    }
    return penx - x;
}

/* ------------------------------------------------------------------------------ Zeichnen */

static void tp_fill_circle(SDL_Renderer *r, int cx, int cy, int rad, SDL_Color c)
{
    enum { N = 28 };
    SDL_Vertex v[N * 3];
    for (int i = 0; i < N; i++) {
        double a0 = (double)i * 2.0 * 3.14159265358979323846 / N;
        double a1 = (double)(i + 1) * 2.0 * 3.14159265358979323846 / N;
        v[i*3+0].position.x = (float)cx;                              v[i*3+0].position.y = (float)cy;
        v[i*3+1].position.x = (float)cx + (float)(rad * cos(a0));     v[i*3+1].position.y = (float)cy + (float)(rad * sin(a0));
        v[i*3+2].position.x = (float)cx + (float)(rad * cos(a1));     v[i*3+2].position.y = (float)cy + (float)(rad * sin(a1));
        for (int k = 0; k < 3; k++) { v[i*3+k].color = c; v[i*3+k].tex_coord.x = v[i*3+k].tex_coord.y = 0.f; }
    }
    SDL_RenderGeometry(r, NULL, v, N * 3, NULL, 0);
}

static void tp_ring(SDL_Renderer *r, int cx, int cy, int rad, int thick, SDL_Color c)
{
    enum { N = 28 };
    SDL_Vertex v[N * 6];
    double ro = rad, ri = rad - thick; if (ri < 1) ri = 1;
    for (int i = 0; i < N; i++) {
        double a0 = (double)i * 2.0 * 3.14159265358979323846 / N;
        double a1 = (double)(i + 1) * 2.0 * 3.14159265358979323846 / N;
        SDL_FPoint o0 = { (float)(cx + ro * cos(a0)), (float)(cy + ro * sin(a0)) };
        SDL_FPoint o1 = { (float)(cx + ro * cos(a1)), (float)(cy + ro * sin(a1)) };
        SDL_FPoint i0 = { (float)(cx + ri * cos(a0)), (float)(cy + ri * sin(a0)) };
        SDL_FPoint i1 = { (float)(cx + ri * cos(a1)), (float)(cy + ri * sin(a1)) };
        SDL_FPoint p[6] = { o0, o1, i0, i0, o1, i1 };
        for (int k = 0; k < 6; k++) { v[i*6+k].position = p[k]; v[i*6+k].color = c; v[i*6+k].tex_coord.x = v[i*6+k].tex_coord.y = 0.f; }
    }
    SDL_RenderGeometry(r, NULL, v, N * 6, NULL, 0);
}

/* dicke Linie als Quad */
static void tp_thick_line(SDL_Renderer *r, float x0, float y0, float x1, float y1, float t, SDL_Color c)
{
    float dx = x1 - x0, dy = y1 - y0, len = sqrtf(dx * dx + dy * dy);
    if (len < 0.001f) return;
    float nx = -dy / len * t * 0.5f, ny = dx / len * t * 0.5f;
    SDL_Vertex v[6];
    SDL_FPoint p[6] = { {x0+nx,y0+ny}, {x1+nx,y1+ny}, {x0-nx,y0-ny}, {x0-nx,y0-ny}, {x1+nx,y1+ny}, {x1-nx,y1-ny} };
    for (int k = 0; k < 6; k++) { v[k].position = p[k]; v[k].color = c; v[k].tex_coord.x = v[k].tex_coord.y = 0.f; }
    SDL_RenderGeometry(r, NULL, v, 6, NULL, 0);
}

static void tp_fill_rect(SDL_Renderer *r, int x, int y, int w, int h, SDL_Color c)
{
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_Rect rc = { x, y, w, h };
    SDL_RenderFillRect(r, &rc);
}

static void tp_rect_outline(SDL_Renderer *r, int x, int y, int w, int h, int t, SDL_Color c)
{
    tp_fill_rect(r, x, y, w, t, c);
    tp_fill_rect(r, x, y + h - t, w, t, c);
    tp_fill_rect(r, x, y, t, h, c);
    tp_fill_rect(r, x + w - t, y, t, h, c);
}

void re15_touch_pc_draw(SDL_Renderer *r)
{
    if (!r || !re15_touch_pc_enabled()) return;
    int W, H;
    SDL_GetRendererOutputSize(r, &W, &H);
    if (W <= 0 || H <= 0) return;
    tp_layout(W, H);
    uint16_t held = re15_touch_pc_pad_bits();

    /* In Fensterkoordinaten zeichnen: logische 320x240-Skalierung kurz abschalten, damit die
     * Knoepfe auch in den schwarzen Letterbox-Streifen liegen duerfen. */
    int lw = 0, lh = 0;
    SDL_RenderGetLogicalSize(r, &lw, &lh);
    SDL_RenderSetLogicalSize(r, 0, 0);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

    const int   u = s_unit;
    const int   t = (u / 9 > 2) ? u / 9 : 2;                 /* Strichstaerke */
    const int   fs = (u / 16 > 1) ? u / 16 : 1;               /* Schrift-Skalierung (Glyph 5x7 -> 6fs Vorschub) */
    const SDL_Color base_idle = { 255, 255, 255,  42 };
    const SDL_Color base_down = { 255, 255, 255, 120 };
    const SDL_Color line_idle = { 255, 255, 255, 130 };
    const SDL_Color line_down = { 255, 255, 255, 230 };

    for (int i = 0; i < s_btn_n; i++) {
        const btn_t *b = &s_btn[i];
        switch (b->kind) {
        case K_DPAD: {
            int cx = s_dpad_cx, cy = s_dpad_cy, arm = s_dpad_arm, half = (int)(0.62f * u);
            /* vier Arme (Kreuzform) */
            struct { int x, y, w, h; uint16_t bit; } arms[4] = {
                { cx - half, cy - arm,  2 * half, arm - half, TP_UP    },
                { cx - half, cy + half, 2 * half, arm - half, TP_DOWN  },
                { cx - arm,  cy - half, arm - half, 2 * half, TP_LEFT  },
                { cx + half, cy - half, arm - half, 2 * half, TP_RIGHT },
            };
            tp_fill_rect(r, cx - half, cy - half, 2 * half, 2 * half, base_idle);
            for (int a = 0; a < 4; a++) {
                int down = (held & arms[a].bit) ? 1 : 0;
                tp_fill_rect(r, arms[a].x, arms[a].y, arms[a].w, arms[a].h, down ? base_down : base_idle);
                tp_rect_outline(r, arms[a].x, arms[a].y, arms[a].w, arms[a].h, t, down ? line_down : line_idle);
            }
            /* Pfeilspitzen */
            {
                SDL_Color c = line_idle; float s = (float)half * 0.55f;
                float ux = (float)cx, uy = (float)cy;
                tp_thick_line(r, ux - s, uy - arm + s * 1.6f, ux, uy - arm + s * 0.6f, (float)t, (held & TP_UP)    ? line_down : c);
                tp_thick_line(r, ux + s, uy - arm + s * 1.6f, ux, uy - arm + s * 0.6f, (float)t, (held & TP_UP)    ? line_down : c);
                tp_thick_line(r, ux - s, uy + arm - s * 1.6f, ux, uy + arm - s * 0.6f, (float)t, (held & TP_DOWN)  ? line_down : c);
                tp_thick_line(r, ux + s, uy + arm - s * 1.6f, ux, uy + arm - s * 0.6f, (float)t, (held & TP_DOWN)  ? line_down : c);
                tp_thick_line(r, ux - arm + s * 1.6f, uy - s, ux - arm + s * 0.6f, uy, (float)t, (held & TP_LEFT)  ? line_down : c);
                tp_thick_line(r, ux - arm + s * 1.6f, uy + s, ux - arm + s * 0.6f, uy, (float)t, (held & TP_LEFT)  ? line_down : c);
                tp_thick_line(r, ux + arm - s * 1.6f, uy - s, ux + arm - s * 0.6f, uy, (float)t, (held & TP_RIGHT) ? line_down : c);
                tp_thick_line(r, ux + arm - s * 1.6f, uy + s, ux + arm - s * 0.6f, uy, (float)t, (held & TP_RIGHT) ? line_down : c);
            }
            break;
        }
        case K_FACE: {
            int down = (held & b->bit) ? 1 : 0;
            tp_fill_circle(r, b->cx, b->cy, b->r, down ? base_down : base_idle);
            tp_ring(r, b->cx, b->cy, b->r, t, down ? line_down : line_idle);
            float g = (float)b->r * 0.45f, cx = (float)b->cx, cy = (float)b->cy;
            SDL_Color gc;
            if (b->bit == TP_CROSS) {            /* Kreuz: hellblau */
                gc.r = 120; gc.g = 170; gc.b = 255; gc.a = (Uint8)(down ? 255 : 190);
                tp_thick_line(r, cx - g, cy - g, cx + g, cy + g, (float)t, gc);
                tp_thick_line(r, cx - g, cy + g, cx + g, cy - g, (float)t, gc);
            } else if (b->bit == TP_CIRCLE) {    /* Kreis: rot */
                gc.r = 255; gc.g = 100; gc.b = 100; gc.a = (Uint8)(down ? 255 : 190);
                tp_ring(r, b->cx, b->cy, (int)g, t, gc);
            } else if (b->bit == TP_SQUARE) {    /* Viereck: rosa */
                gc.r = 255; gc.g = 140; gc.b = 220; gc.a = (Uint8)(down ? 255 : 190);
                tp_rect_outline(r, (int)(cx - g), (int)(cy - g), (int)(2 * g), (int)(2 * g), t, gc);
            } else {                             /* Dreieck: gruen */
                gc.r = 120; gc.g = 230; gc.b = 140; gc.a = (Uint8)(down ? 255 : 190);
                tp_thick_line(r, cx, cy - g, cx + g, cy + g * 0.8f, (float)t, gc);
                tp_thick_line(r, cx + g, cy + g * 0.8f, cx - g, cy + g * 0.8f, (float)t, gc);
                tp_thick_line(r, cx - g, cy + g * 0.8f, cx, cy - g, (float)t, gc);
            }
            break;
        }
        case K_RECT:
        case K_MARKE: {
            int down = (b->kind == K_RECT) ? ((held & b->bit) ? 1 : 0) : 0;
            SDL_Color fill = (b->kind == K_MARKE) ? (SDL_Color){ 255, 200, 60, 60 } : (down ? base_down : base_idle);
            tp_fill_rect(r, b->x, b->y, b->w, b->h, fill);
            tp_rect_outline(r, b->x, b->y, b->w, b->h, t, down ? line_down : line_idle);
            if (b->label) {
                int tw = (int)strlen(b->label) * 6 * fs - fs;
                re15_touch_pc_text(r, b->cx - tw / 2, b->cy - 7 * fs / 2, fs, b->label,
                                   255, 255, 255, (Uint8)(down ? 240 : 170));
            }
            break;
        }
        default: break;
        }
    }

    SDL_RenderSetLogicalSize(r, lw, lh);
}

/* ------------------------------------------------------------------------------ Selbsttest */

static void tp_selftest(void)
{
    int W, H; tp_output_size(&W, &H);
    tp_layout(W, H);
    int ok = 0, fail = 0;
    /* Jeder Knopf einzeln: Finger genau auf die Mitte -> exakt sein Bit. */
    for (int i = 0; i < s_btn_n; i++) {
        const btn_t *b = &s_btn[i];
        if (b->kind == K_DPAD) continue;
        tp_release_all();
        s_marke = 0;
        tp_finger_down(1, 100 + i, (float)b->cx / (float)W, (float)b->cy / (float)H);
        uint16_t got = re15_touch_pc_pad_bits();
        int marke = s_marke;
        int good = (b->kind == K_MARKE) ? (got == 0 && marke == 1) : (got == b->bit && marke == 0);
        fprintf(stderr, "[touch] selftest %-6s @(%d,%d) bits=%04X marke=%d %s\n",
                b->label ? b->label : (b->bit == TP_CROSS ? "KREUZ" : b->bit == TP_CIRCLE ? "KREIS" :
                                       b->bit == TP_SQUARE ? "VIER" : "DREI"),
                b->cx, b->cy, got, marke, good ? "ok" : "FAIL");
        if (good) ok++; else fail++;
    }
    /* D-Pad: acht Richtungen, Finger jeweils 1.5u vom Zentrum. */
    {
        static const struct { float dx, dy; uint16_t want; const char *nm; } dirs[8] = {
            {  1.f,  0.f, TP_RIGHT,           "RECHTS" }, {  1.f, -1.f, TP_UP | TP_RIGHT,   "OBEN+RECHTS" },
            {  0.f, -1.f, TP_UP,              "OBEN"   }, { -1.f, -1.f, TP_UP | TP_LEFT,    "OBEN+LINKS"  },
            { -1.f,  0.f, TP_LEFT,            "LINKS"  }, { -1.f,  1.f, TP_DOWN | TP_LEFT,  "UNTEN+LINKS" },
            {  0.f,  1.f, TP_DOWN,            "UNTEN"  }, {  1.f,  1.f, TP_DOWN | TP_RIGHT, "UNTEN+RECHTS"},
        };
        for (int d = 0; d < 8; d++) {
            tp_release_all();
            float x = (float)s_dpad_cx + dirs[d].dx * 1.5f * (float)s_unit;
            float y = (float)s_dpad_cy + dirs[d].dy * 1.5f * (float)s_unit;
            tp_finger_down(1, 200 + d, x / (float)W, y / (float)H);
            uint16_t got = re15_touch_pc_pad_bits();
            int good = (got == dirs[d].want);
            fprintf(stderr, "[touch] selftest DPAD %-12s bits=%04X want=%04X %s\n", dirs[d].nm, got, dirs[d].want, good ? "ok" : "FAIL");
            if (good) ok++; else fail++;
        }
        /* Totzone: Finger exakt im Zentrum -> keine Richtung. */
        tp_release_all();
        tp_finger_down(1, 299, (float)s_dpad_cx / (float)W, (float)s_dpad_cy / (float)H);
        uint16_t got = re15_touch_pc_pad_bits();
        fprintf(stderr, "[touch] selftest DPAD ZENTRUM bits=%04X want=0000 %s\n", got, got == 0 ? "ok" : "FAIL");
        if (got == 0) ok++; else fail++;
    }
    /* Zwei Finger gleichzeitig: OBEN + KREUZ = Rennen. */
    {
        tp_release_all();
        const btn_t *cross = NULL;
        for (int i = 0; i < s_btn_n; i++) if (s_btn[i].kind == K_FACE && s_btn[i].bit == TP_CROSS) cross = &s_btn[i];
        tp_finger_down(1, 301, (float)s_dpad_cx / (float)W, ((float)s_dpad_cy - 1.5f * (float)s_unit) / (float)H);
        if (cross) tp_finger_down(1, 302, (float)cross->cx / (float)W, (float)cross->cy / (float)H);
        uint16_t got = re15_touch_pc_pad_bits();
        int good = (got == (TP_UP | TP_CROSS));
        fprintf(stderr, "[touch] selftest ZWEI FINGER (OBEN+KREUZ) bits=%04X want=%04X %s\n", got, TP_UP | TP_CROSS, good ? "ok" : "FAIL");
        if (good) ok++; else fail++;
        /* Loslassen loescht. */
        tp_finger_up(1, 301); tp_finger_up(1, 302);
        got = re15_touch_pc_pad_bits();
        fprintf(stderr, "[touch] selftest LOSLASSEN bits=%04X want=0000 %s\n", got, got == 0 ? "ok" : "FAIL");
        if (got == 0) ok++; else fail++;
    }
    tp_release_all(); s_marke = 0;
    fprintf(stderr, "[touch] SELFTEST RESULT ok=%d fail=%d (Ausgabe %dx%d, u=%d)\n", ok, fail, W, H, s_unit);
}

void re15_touch_pc_init(SDL_Renderer *r)
{
    s_r = r;
    if (!re15_touch_pc_enabled()) return;
    memset(s_fingers, 0, sizeof s_fingers);
    s_lay_w = s_lay_h = -1;
    {
        int W, H; tp_output_size(&W, &H);
        tp_layout(W, H);
        fprintf(stderr, "[touch] Overlay-Controller AN (%dx%d, u=%d, %d Knoepfe)\n", W, H, s_unit, s_btn_n);
    }
    { const char *st = getenv("RE15_TOUCH_SELFTEST"); if (st && st[0] == '1') tp_selftest(); }
}
