/*
 * RE1.5 Rebuilt — Runde 41 Spur E "kleinkram_b", Nachbesserung 2 (master-AUFTRAG E-N1): das Apply-Bild eines
 * Kamerawechsels wird nicht gezeichnet und nicht praesentiert — im Fenster bleibt das zuletzt gezeigte Bild stehen.
 *
 * ORIGINAL (selbst disassembliert, info/Re1.5/PSX.EXE): Present FUN_8002137c — ist der Cut schmutzig (@0x800214f4-
 *   @0x80021514 Vergleich angezeigt/angefordert, @0x80021530/@0x80021538 DAT_800b5457), ruft @0x80021558 den Apply
 *   FUN_80021bbc und springt @0x80021560 `j 0x800215fc` ueber die drei DrawOTag (@0x800215bc/@0x800215d0/@0x800215e4)
 *   UND den Pufferwechsel (@0x800215f4 `xori v0,v0,0x1` / @0x800215f8 `sb v0,DAT_800aca34`). Die mit der alten Kamera
 *   gebaute OT dieses Durchlaufs wird nie gezeigt, der Bildschirm zeigt den vorigen Puffer weiter.
 * GEMESSEN (DuckStation-GDB, Dossier "Nachbesserung 2"): ROOM2020 auf der RVD-Kante x = 9000 39 von 40 Durchlaeufen
 *   Apply (@0x80021558) ohne ein DrawOTag (@0x800215bc), angeforderter Cut 0/2 im Wechsel — das Bild steht.
 * PORT: die Entscheidung trifft die Engine (re15_kamerahalt_r41_haelt, engine/src/kamerahalt_r41.c); hier nur die
 *   Ausfuehrung: Warteschlangen dieses Bildes verwerfen (Dreiecke/Schatten, Masken-Riegel fuer ein Bild —
 *   re15_render_pc_clear_scene_overlays; den Rest setzt das naechste re15_render_begin_frame zurueck) und KEIN
 *   SDL_RenderPresent. Das Standbild der Tuerszene (s_sb_*) wird nur in re15_render_end_frame gefuehrt und bleibt
 *   damit das zuletzt GEZEIGTE Bild.
 *   Messabzug: ein Ruecklese-Auftrag (re15_render_pc_request_readback: RE15_FRAMEDUMP, RE15_R38F_SICHT, ...) liest den
 *   Backbuffer im Normalpfad VOR dem Present; ein gehaltenes Bild hat keinen neuen Backbuffer. Darum wird ab dem ersten
 *   Auftrag (oder mit gesetztem RE15_FRAMEDUMP) jedes gezeigte Bild hier zusaetzlich gemerkt und ein Auftrag fuer ein
 *   gehaltenes Bild mit diesem Bild bedient (Zeile "[framedump] ... (gehalten)"). Ohne Rueckleser: kein Ruecklesen,
 *   keine Kosten.
 * NACHBESSERUNG 3 (Abnahme 2 Mangel 4, Orchestrator-Entscheid E-E3): der BLENDEN-TAKT laeuft auch im gehaltenen Bild.
 *   ORIGINAL (selbst disassembliert): Hauptschleife @0x80020f34 `jal 0x80021a0c` (Letterbox), @0x80020f3c `jal
 *   0x80010000`, @0x80020f44 `jal 0x80021880` (Blenden-Takt), @0x80020f4c `jal 0x8002137c` (Present) — der Takt liegt
 *   VOR dem Present und damit auch im Apply-Durchlauf; FUN_80021880 fuehrt den Pegel jedes aktiven Kanals weiter
 *   (@0x8002191c `lhu v0,0(s0)` / @0x80021920 `lhu v1,-6(s1)` / @0x80021928 `addu` / @0x8002192c `sh v0,0(s0)`), die
 *   Kachel dieses Durchlaufs landet in der OT, die der Apply-Zweig nie zeichnet (@0x80021560).
 *   PORT: re15_fade_tick() (= FUN_80021880) steckt in re15_render_end_frame(); das gehaltene Bild ruft end_frame nicht
 *   und liess den Takt aus -> jede Rampe verlor je gehaltenem Bild einen Schritt, beim Raumeintritt auf einer
 *   wechselnden RVD-Kante endete die Einblendung nie (Freeze pf=FF000007, Softlock; Abnahme 2 §3.6). Jetzt: Takt hier,
 *   nichts gezeichnet.
 */
#include <SDL.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "re15_fade.h"   /* re15_fade_tick = FUN_80021880 (Nachbesserung 3) */

extern SDL_Renderer *re15_render_pc_renderer(void);
extern void re15_render_pc_clear_scene_overlays(void);
extern int  re15_render_pc_abzug_nehmen(char *pfad, int n);

static uint32_t *s_abzug;          /* zuletzt gezeigtes Bild, RGB888 (nur mit RE15_FRAMEDUMP) */
static int       s_w, s_h;
static int       s_an = -1;
static unsigned  s_gehalten;

void re15_bildhalt_r41_merken(int auftrag)
{
    if (s_an < 0) { const char *e = getenv("RE15_FRAMEDUMP"); s_an = (e && *e) ? 1 : 0; }
    if (auftrag) s_an = 1;                   /* ab dem ersten Ruecklese-Auftrag jedes gezeigte Bild merken */
    if (!s_an) return;
    SDL_Renderer *r = re15_render_pc_renderer();
    if (!r) return;
    int w = 0, h = 0;
    SDL_GetRendererOutputSize(r, &w, &h);
    if (w <= 0 || h <= 0) return;
    if (w != s_w || h != s_h || !s_abzug) {
        free(s_abzug);
        s_abzug = (uint32_t *)malloc((size_t)w * (size_t)h * 4u);
        s_w = s_abzug ? w : 0;
        s_h = s_abzug ? h : 0;
        if (!s_abzug) return;
    }
    if (SDL_RenderReadPixels(r, NULL, SDL_PIXELFORMAT_RGB888, s_abzug, w * 4) != 0) {
        s_w = s_h = 0;   /* kein gueltiges Bild gemerkt */
    }
}

void re15_render_pc_bild_halten(void)
{
    s_gehalten++;
    re15_fade_tick();   /* @0x80020f44 vor @0x80020f4c: Pegel += Schritt (@0x8002191c-@0x8002192c), Kachel ungezeichnet */
    re15_render_pc_clear_scene_overlays();   /* Dreiecke, Schatten verwerfen; Masken-Riegel gilt bis begin_frame */
    char pfad[512];
    if (!re15_render_pc_abzug_nehmen(pfad, (int)sizeof pfad)) return;
    if (!s_abzug || s_w <= 0 || s_h <= 0) {
        fprintf(stderr, "[framedump] gehalten, aber kein gezeigtes Bild gemerkt -> %s fehlt\n", pfad);
        return;
    }
    FILE *pf = fopen(pfad, "wb");
    if (!pf) return;
    fprintf(pf, "P6\n%d %d\n255\n", s_w, s_h);
    for (int i = 0; i < s_w * s_h; i++) {
        const uint32_t px = s_abzug[i];
        const unsigned char rgb[3] = { (unsigned char)(px >> 16), (unsigned char)(px >> 8), (unsigned char)px };
        fwrite(rgb, 1, 3, pf);
    }
    fclose(pf);
    fprintf(stderr, "[framedump] %dx%d -> %s (gehalten)\n", s_w, s_h, pfad);
}

unsigned re15_bildhalt_r41_zahl(void) { return s_gehalten; }
