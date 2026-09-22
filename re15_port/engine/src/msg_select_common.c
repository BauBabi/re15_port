/*
 * msg_select_common.c — die EINE Stelle, an der die Ja/Nein-Zahlen stehen.
 *
 * Herleitung samt Instruktionen: include/re15_msg_select.h. Hier nur die Werte, jeder
 * mit seiner Adresse. Wer eine Ja/Nein-Abfrage zeichnet, holt sie HIER — es gibt keine
 * zweite Zahlenreihe im Port.
 */
#include "re15_msg_select.h"

/* ".msg"-Glyphencodes (A-Z ab 0x1D, a-z ab 0x3D): "Yes" und "No". Das Original zieht
 * die beiden Woerter als EINEN ASCII-String @0x800C4954 ("Yes" 01 2e "No" 00, DEBUG.BIN
 * Datei-Offset 0x4954) durch den Debug-Drucker FUN_800279c8; der Port zeichnet sie mit
 * der Spielschrift und braucht sie deshalb als Glyphen. */
const unsigned char re15_msg_select_yes[3] = { 0x35, 0x41, 0x4f };
const unsigned char re15_msg_select_no[2]  = { 0x2a, 0x4b };

/* Box-Ursprung des Prompt-Modus a1 = 0x100 (FUN_80027e68):
 *   @0x80027eec  ori v0,zero,0x22  -> DAT_800b8534 = 34   (X, hier nicht gebraucht)
 *   @0x80027f14  ori v0,zero,0xb4  -> DAT_800b8536 = 180  (Y) */
#define SEL_BOX_Y       0xb4        /* @0x80027f14 */
/* Die Auswahlzeile liegt 0x10 unter dem Box-Ursprung:
 *   @0x80028630 addiu a0,a0,0x10 (Cursor) / @0x80028674 addiu a0,a0,0x10 (Text) */
#define SEL_ROW_DY      0x10        /* @0x80028630 / @0x80028674 */
/* Erste Cursor-Zelle und Abstand der beiden Zellen:
 *   @0x80028650 addiu v0,v0,0xa0        -> 160
 *   @0x8002864c sll   v0,v0,0x1  (=*0x46, Kette @0x8002863c-4c) -> +70 = 230 */
#define SEL_CURSOR_X0   0xa0        /* @0x80028650 */
#define SEL_CURSOR_DX   0x46        /* @0x8002863c-@0x8002864c */
/* Spalte des Options-Textes: @0x80028680 ori a0,a0,0xae -> 174. */
#define SEL_TEXT_X0     0xae        /* @0x80028680 */
/* Blink-Maske: @0x80028600 ori v0,zero,0x18 (danach `sllv v0,v0,s1`, s1 = 0). */
#define SEL_BLINK_MASK  0x18        /* @0x80028600 */

void re15_msg_select_layout(int choice, uint8_t blink, re15_msg_select_t *out)
{
    if (!out) return;
    const int row = SEL_BOX_Y + SEL_ROW_DY;              /* 180 + 16 = 196 */

    out->opt[0].x      = SEL_TEXT_X0;                    /* 174 @0x80028680 */
    out->opt[0].y      = row;
    out->opt[0].glyphs = re15_msg_select_yes;
    out->opt[0].len    = (int)sizeof re15_msg_select_yes;

    out->opt[1].x      = SEL_TEXT_X0 + SEL_CURSOR_DX;    /* 174 + 0x46 = 244 */
    out->opt[1].y      = row;
    out->opt[1].glyphs = re15_msg_select_no;
    out->opt[1].len    = (int)sizeof re15_msg_select_no;

    /* @0x80028638 `andi v1,v1,0x1` — nur Bit 0 der Auswahlzelle zaehlt. */
    out->cursor_x = SEL_CURSOR_X0 + ((choice & 1) ? SEL_CURSOR_DX : 0);  /* 160 / 230 */
    out->cursor_y = row;
    /* @0x80028608 `and` + @0x8002860c `beq ... -> Cursor auslassen`. */
    out->cursor_visible = (blink & SEL_BLINK_MASK) ? 1 : 0;
}

uint8_t re15_msg_select_blink_tick(uint8_t blink, int lr_edge)
{
    /* Reihenfolge ist byte-true und NICHT beliebig: erst nullen (@0x800285d4, nur im
     * Links/Rechts-Zweig), dann dekrementieren (@0x800285f0, immer). Ein Tastendruck
     * setzt den Zaehler damit auf 0xFF — der Cursor steht im SELBEN Bild wieder da. */
    if (lr_edge) blink = 0;               /* @0x800285d4 sb zero,DAT_800b8525 */
    return (uint8_t)(blink - 1);          /* @0x800285f0 addiu v1,v1,-0x1     */
}
