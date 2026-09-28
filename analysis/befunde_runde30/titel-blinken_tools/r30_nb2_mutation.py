#!/usr/bin/env python3
"""r30_nb2_mutation.py - Runde 30 / Thema D, zweite Nachbesserung: GEGENPROBEN.

Setzt in einem Wegwerf-Baum (git worktree, NICHT der Arbeitsbaum) genau eine Mutation und
laesst alles andere auf dem Stand des Zweigs. Danach baut der Aufrufer re15_pc und faehrt
tests/integration/test_r30_titel_puls.cmake gegen die gebaute exe.

Aufruf: python r30_nb2_mutation.py <baum> <mutation>
  basis                     keine Aenderung (Kontrolle)
  render_alt                render_pc.c = master d98e9639, unveraendert (Mangel M1 des
                            Gegenpruefers woertlich; die neue Ruecklese fehlt dort)
  render_alt_schiene        render_pc.c = master d98e9639 + NUR die Messschiene nachgeruestet:
                            dieselbe Ruecklese wie im neuen Stand; "gezeichnet" = der Pulswert,
                            mit dem der alte Zeichner die Farbmodulation rechnet (s_tmoji_pulse_val)
  zeichner_eigener_schritt  neues render_pc.c, aber re15_render_pc_title_menu holt den Pulswert
                            nicht aus der Engine, sondern schreitet ihn je Zeichenaufruf selbst
                            fort (so wie render_pc.c bis d98e9639)
  modulation_alt            neues render_pc.c, aber die aktive Zeile mit der alten Abbildung
                            200 + (wert - 0x80) * 55 / 0x3e (Faktor 0,784 ... 1,000) statt
                            texel * wert >> 7 mit Saettigung 31
  main_je_bild              main.c: ein Pulsschritt je Bild (re15_title_pulse_step()) statt der
                            faelligen Durchgaenge (Mutation M2 des Gegenpruefers)
"""
import subprocess, sys

BASIS_ALT = "d98e9639"
RPC = "re15_port/platform/pc/src/render_pc.c"
MAIN = "re15_port/platform/pc/main.c"
CR = chr(13); LF = chr(10)

def lesen(pfad):
    s = open(pfad, "rb").read().decode("utf-8")
    return s.replace(CR + LF, LF), (CR + LF) in s

def schreiben(pfad, s, crlf):
    if crlf: s = s.replace(LF, CR + LF)
    open(pfad, "wb").write(s.encode("utf-8"))

def ersetze(s, alt, neu, name):
    n = s.count(alt)
    if n != 1:
        sys.exit("FEHLER %s: %d Treffer statt 1 fuer: %r" % (name, n, alt[:160]))
    return s.replace(alt, neu)

def git(baum, *a):
    return subprocess.run(["git", "-C", baum] + list(a), check=True, capture_output=True).stdout

RUECKLESE = r'''
/* === NACHGERUESTETE MESSSCHIENE (r30_nb2_mutation.py, identisch mit dem neuen Stand) === */
static int      s_trow_on = -1;
static int      s_trow_valid = 0, s_trow_drawn = -1, s_trow_row = -1;
static uint32_t s_trow_hash = 0, s_trow_sum = 0;
static void title_row_readback(int row, int y0)
{
    if (s_trow_on < 0) { const char *e = getenv("RE15_TITLE_PULSE_LOG"); s_trow_on = (e && *e) ? 1 : 0; }
    if (!s_trow_on) return;
    s_trow_valid = 0;
    float sx = 1.0f, sy = 1.0f;
    SDL_Rect vp;
    SDL_RenderGetScale(s_renderer, &sx, &sy);
    SDL_RenderGetViewport(s_renderer, &vp);
    if (sx <= 0.0f || sy <= 0.0f) return;
    const int LW = 256, LH = 17, LX = 0x20;
    int px0 = (int) ((float) (vp.x + LX) * sx), py0 = (int) ((float) (vp.y + y0) * sy);
    int px1 = (int) ((float) (vp.x + LX + LW) * sx + 0.999f), py1 = (int) ((float) (vp.y + y0 + LH) * sy + 0.999f);
    int pw = px1 - px0, ph = py1 - py0;
    if (pw <= 0 || ph <= 0) return;
    uint32_t *buf = (uint32_t *) malloc((size_t) pw * ph * 4);
    if (!buf) return;
    SDL_Rect r = { px0, py0, pw, ph };
    if (SDL_RenderReadPixels(s_renderer, &r, SDL_PIXELFORMAT_RGB888, buf, pw * 4) == 0) {
        uint32_t h = 2166136261u, sum = 0;
        for (int ly = 0; ly < LH; ly++) {
            for (int lx = 0; lx < LW; lx++) {
                int px = (int) (((float) (vp.x + LX + lx) + 0.5f) * sx) - px0;
                int py = (int) (((float) (vp.y + y0 + ly) + 0.5f) * sy) - py0;
                px = (px < 0) ? 0 : (px >= pw) ? pw - 1 : px;
                py = (py < 0) ? 0 : (py >= ph) ? ph - 1 : py;
                uint32_t p = buf[(size_t) py * pw + px];
                uint8_t c[3] = { (uint8_t) ((p >> 19) & 31), (uint8_t) ((p >> 11) & 31), (uint8_t) ((p >> 3) & 31) };
                for (int k = 0; k < 3; k++) { h ^= c[k]; h *= 16777619u; sum += c[k]; }
            }
        }
        s_trow_hash = h; s_trow_sum = sum; s_trow_row = row; s_trow_valid = 1;
    }
    free(buf);
}
int re15_render_pc_title_row_probe(int *drawn, int *row, uint32_t *hash, uint32_t *sum)
{
    int ok = s_trow_valid;
    if (drawn) *drawn = ok ? s_trow_drawn : -1;
    if (row)   *row   = ok ? s_trow_row   : -1;
    if (hash)  *hash  = ok ? s_trow_hash  : 0;
    if (sum)   *sum   = ok ? s_trow_sum   : 0;
    s_trow_valid = 0;
    return ok;
}
/* === Ende nachgeruestete Messschiene === */

void re15_render_end_frame(void)
{'''

def main():
    baum, mut = sys.argv[1], sys.argv[2]
    git(baum, "checkout", "HEAD", "--", RPC, MAIN)          # jede Mutation vom Zweig-Stand aus
    if mut == "basis":
        pass
    elif mut in ("render_alt", "render_alt_schiene"):
        alt = git(baum, "show", "%s:%s" % (BASIS_ALT, RPC)).decode("utf-8")
        crlf = (CR + LF) in alt
        s = alt.replace(CR + LF, LF)
        if mut == "render_alt_schiene":
            s = ersetze(s, "\nvoid re15_render_end_frame(void)\n{", RUECKLESE, mut)
            s = ersetze(s, """            SDL_SetTextureColorMod(tex, (Uint8) mod, (Uint8) mod, (Uint8) mod);
""", """            SDL_SetTextureColorMod(tex, (Uint8) mod, (Uint8) mod, (Uint8) mod);
            if (active) s_trow_drawn = s_tmoji_pulse_val;   /* MESSSCHIENE: Wert der Modulation */
""", mut)
            s = ersetze(s, """            SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_ADD); SDL_RenderCopy(s_renderer, tex, NULL, &glow);   /* abr1 */
        }
    }
""", """            SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_ADD); SDL_RenderCopy(s_renderer, tex, NULL, &glow);   /* abr1 */
        }
        if (s_tmoji_cursor >= 0 && s_tmoji_cursor < 3)
            title_row_readback(s_tmoji_cursor, ITEM_Y[s_tmoji_cursor]);   /* MESSSCHIENE */
    }
""", mut)
        schreiben("%s/%s" % (baum, RPC), s, crlf)
    elif mut == "zeichner_eigener_schritt":
        s, crlf = lesen("%s/%s" % (baum, RPC))
        s = ersetze(s, """        int val = re15_title_pulse_value();
""", """        /* MUTATION: eigener Pulsschritt je Zeichenaufruf (wie render_pc.c bis d98e9639) */
        static int s_mut_ctr = 0, s_mut_val = 0x80;
        if (s_mut_ctr < 0x1f) s_mut_val += 2; else s_mut_val -= 2;
        if (++s_mut_ctr >= 0x3c) { s_mut_ctr = 0; s_mut_val = 0x80; }
        int val = s_mut_val;
""", mut)
        schreiben("%s/%s" % (baum, RPC), s, crlf)
    elif mut == "modulation_alt":
        s, crlf = lesen("%s/%s" % (baum, RPC))
        s = ersetze(s, """static SDL_Texture *tmoji_strip(const re15_tim_t *t, int v, int h, int clut_base, int farbe)
{""", """static int s_mut_alt = 0;   /* MUTATION: alte Abbildung fuer die aktive Zeile */
static SDL_Texture *tmoji_strip(const re15_tim_t *t, int v, int h, int clut_base, int farbe)
{""", mut)
        s = ersetze(s, """                ch[k] = (ch[k] * (uint32_t) farbe) >> 7;      /* texel * farbe / 128 */
                if (ch[k] > 31u) ch[k] = 31u;                 /* Saettigung 31       */
""", """                if (s_mut_alt) ch[k] = ch[k] * (uint32_t) (200 + (farbe - 0x80) * 55 / 0x3e) / 255u;
                else { ch[k] = (ch[k] * (uint32_t) farbe) >> 7; if (ch[k] > 31u) ch[k] = 31u; }
""", mut)
        s = ersetze(s, """                s_tmoji_act[cursor][lv] = tmoji_strip(tmoji, s_tmoji_src[cursor].v,
                                                      s_tmoji_src[cursor].h, 0, val);
""", """            { s_mut_alt = 1;
                s_tmoji_act[cursor][lv] = tmoji_strip(tmoji, s_tmoji_src[cursor].v,
                                                      s_tmoji_src[cursor].h, 0, val);
              s_mut_alt = 0; }
""", mut)
        schreiben("%s/%s" % (baum, RPC), s, crlf)
    elif mut == "main_je_bild":
        s, crlf = lesen("%s/%s" % (baum, MAIN))
        s = ersetze(s, """                  re15_title_pulse_advance(due);
""", """                  re15_title_pulse_step();   /* MUTATION: ein Schritt je Bild */
""", mut)
        schreiben("%s/%s" % (baum, MAIN), s, crlf)
    else:
        sys.exit("unbekannte Mutation %s" % mut)
    d = git(baum, "diff", "--stat").decode()
    print("Mutation %s gesetzt.%s" % (mut, ("\n" + d) if d else " (keine Aenderung)"))

if __name__ == "__main__":
    main()
