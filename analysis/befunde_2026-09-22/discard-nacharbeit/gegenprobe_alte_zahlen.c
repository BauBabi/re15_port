/* Gegenprobe: die ALTEN, geratenen Zahlen durch die Pruefungen von TEIL D schicken. */
#include <stdio.h>
#include <stdint.h>
typedef struct { int x, y; const unsigned char *g; int len; } opt_t;
typedef struct { opt_t opt[2]; int cursor_x, cursor_y, cursor_visible; } sel_t;
static const unsigned char yes_g[3] = {0x35,0x41,0x4f}, no_g[2] = {0x2a,0x4b};

/* Der ALTE Stand von platform/pc/main.c (Aufnahme-Prompt UND Wegwerf-Abfrage):
 *   re15_render_pc_msg_text(190, 202, yes_g, 3);
 *   re15_render_pc_msg_text(234, 202, no_g,  2);
 *   re15_render_pc_cursor(pchoice ? 224 : 180, 203);
 * Feste Zahlen, KEIN Blink-Gatter. */
static void alt_layout(int choice, uint8_t blink, sel_t *o)
{
    (void)blink;
    o->opt[0].x = 190; o->opt[0].y = 202; o->opt[0].g = yes_g; o->opt[0].len = 3;
    o->opt[1].x = 234; o->opt[1].y = 202; o->opt[1].g = no_g;  o->opt[1].len = 2;
    o->cursor_x = choice ? 224 : 180; o->cursor_y = 203; o->cursor_visible = 1;
}

int main(void)
{
    int f = 0; sel_t ja, nein;
    alt_layout(0, 0xff, &ja); alt_layout(1, 0xff, &nein);
    if (ja.opt[0].x  != 174) { printf("ROT  Yes-Spalte %d statt 174 (@0x80028680)\n", ja.opt[0].x); f++; }
    if (ja.opt[1].x  != 244) { printf("ROT  No-Spalte %d statt 244 (@0x8002864c)\n", ja.opt[1].x); f++; }
    if (ja.opt[0].y  != 196) { printf("ROT  Zeile %d statt 196 (@0x80028674)\n", ja.opt[0].y); f++; }
    if (ja.cursor_x  != 160) { printf("ROT  Cursor bei Yes %d statt 160 (@0x80028650)\n", ja.cursor_x); f++; }
    if (nein.cursor_x!= 230) { printf("ROT  Cursor bei No %d statt 230\n", nein.cursor_x); f++; }
    if (ja.cursor_y  != 196) { printf("ROT  Cursor-Zeile %d statt 196 (@0x80028630)\n", ja.cursor_y); f++; }
    int sicht = 0;
    for (int z = 0; z < 256; z++) { sel_t s; alt_layout(0, (uint8_t)z, &s); if (s.cursor_visible) sicht++; }
    if (sicht != 192) { printf("ROT  Blink-Gatter: %d von 256 sichtbar statt 192 (@0x80028600)\n", sicht); f++; }
    printf("Gegenprobe am ALTEN Stand: %d Pruefungen ROT\n", f);
    return f ? 1 : 0;
}
