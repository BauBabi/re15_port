/*
 * item_discard_common.c — "You don't need this key any more. Discard it?"
 *
 * Die vollstaendige Herleitung (Wortlaut @0x800C508B, RE2-Regel LAB_80051718,
 * Benutzungsstellen-Zensus, Sackgassen-Beweis) steht im Kopf von
 * include/re15_item_discard.h. Hier nur noch, was der Code TUT, mit den Adressen
 * an den einzelnen Zeilen.
 */
#include "re15_item_discard.h"
#include "re15_inventory.h"    /* g_inv, re15_inv_find_item, re15_inv_remove_slot        */
#include "re15_scd.h"          /* g_scd.message_active / message_fsm_active              */
#include "re15_item_prompt.h"  /* re15_item_prompt_walk — Glyphenzahl des Skripts        */

#include "gen/discard_sites.inc"

/* Port-Prompt-Schluessel 8 -> BSS-Skript [6] (item_prompt_common.c:60). */
#define RE15_DISCARD_PROMPT_KEY 8

/* Schreibmaschinen-Kadenz: identisch zum Aufnahme-Prompt, byte-true FUN_80028134
 * (@0x800281b0-c4 `DAT_800b8524 = 2 << s1`, s1 = 0 -> 1 Glyphe je 2 Bilder). Der
 * Schnellvorlauf ist derselbe: HELD virtuell 0x4000 -> Zaehler -= 4 (@0x80028228),
 * 2 Glyphen je Ablauf (@0x8002822c `sltiu 4` -> `ori 2`). Uebernommen aus
 * item_modal_common.c, damit beide Prompts identisch tippen. */
#define DISCARD_MSG_RELOAD 2

enum { D_AUS = 0, D_WARTET, D_FRAGT };

static int     s_zustand   = D_AUS;
static uint8_t s_item      = 0;
static int     s_slot      = -1;
static int     s_choice    = 0;    /* 0 = Yes (vorbelegt), 1 = No                       */
static int     s_reveal    = 0;
static int     s_reveal_total = 0;
static int     s_reveal_timer = 0;
static int     s_gefragt   = 0;
static int     s_weggeworfen = 0;

void re15_discard_reset(void)
{
    /* Eine abgebrochene Abfrage darf keinen Slot mit Anzahl 0 hinterlassen — dasselbe,
     * was RE2 im Nein-Zweig tut (@0x800517C4 `sb v1,count` mit v1 = 1). */
    if (s_zustand != D_AUS && s_slot >= 0 && s_slot < RE15_INV_MAX_SLOTS
        && g_inv.slots[s_slot].id == s_item && g_inv.slots[s_slot].qty == 0)
        g_inv.slots[s_slot].qty = 1;
    s_zustand = D_AUS; s_item = 0; s_slot = -1; s_choice = 0;
    s_reveal = s_reveal_total = s_reveal_timer = 0;
}

void re15_discard_notice_message(unsigned room_id, uint8_t msg_id)
{
    if (s_zustand != D_AUS) return;            /* eine Abfrage laeuft schon              */
    uint8_t item = 0;
    for (int i = 0; i < RE15_DISCARD_SITE_COUNT; i++) {
        if (re15_discard_sites[i].room == (uint16_t)room_id
            && re15_discard_sites[i].msg == msg_id) { item = re15_discard_sites[i].item; break; }
    }
    if (!item) return;                         /* keine Benutzungsstelle                 */

    /* Der Gegenstand muss WIRKLICH getragen werden. RE1.5 prueft das im Skript NICHT
     * (ROOM4000 sub02 z.B. fragt gar nicht nach Besitz) — der Port darf deshalb nicht
     * anbieten, etwas wegzuwerfen, was gar nicht da ist. */
    int slot = re15_inv_find_item(item);
    if (slot < 0 || slot >= RE15_INV_MAX_SLOTS) return;

    /* RE2s Zaehler-Regel, auf RE1.5s Anzahl-Byte angewandt:
     *   80051808 lbu   v0,count
     *   80051810 addiu v0,v0,-1     ; Zaehler -= 1
     *   8005181C sb    v0,count
     *   80051824 bne   v0,zero,...  ; > 0 -> KEINE Abfrage
     * Gemessen (tools/gen_discard_sites.py): alle 9 aufgenommenen Gegenstaende werden mit
     * Anzahl 1 ausgegeben, und jeder hat genau EINE Benutzungsstelle — die Anzahl faellt
     * hier also von 1 auf 0 und es wird gefragt. Die Regel steht trotzdem als Zaehler da
     * und nicht als "immer fragen": so kann ein Gegenstand mit mehreren Benutzungsstellen
     * nicht versehentlich zu frueh gemeldet werden. */
    uint8_t n = g_inv.slots[slot].qty;
    if (n == 0) return;                        /* schon verbraucht -> nichts zu tun      */
    n--;
    g_inv.slots[slot].qty = n;
    if (n != 0) return;                        /* @0x80051824                            */

    s_item    = item;
    s_slot    = slot;
    s_choice  = 0;                             /* Yes vorbelegt — RE2 setzt die Auswahl-
                                                * zelle beim Nachrichtenstart auf exakt
                                                * 0x80, unteres Nibble 0 = Yes
                                                * (@0x8002FE88); der Aufnahme-Prompt des
                                                * Ports startet ebenso auf 0.            */
    s_zustand = D_WARTET;                      /* erst die Raum-Nachricht ausreden lassen */
}

void re15_discard_tick(uint16_t pad_edge, uint16_t pad_held)
{
    if (s_zustand == D_AUS) return;

    if (s_zustand == D_WARTET) {
        /* RE2 wartet an genau dieser Stelle darauf, dass das Nachrichtensystem frei wird:
         *   800517E0 lbu  v0,[0x800E873C]
         *   800517F0 andi v0,v0,0x80
         *   800517F4 bne  v0,zero,0x80051870   ; belegt -> weiter warten
         * Im Port sind das die beiden Nachrichten-Zustaende von g_scd. */
        if (g_scd.message_active || g_scd.message_fsm_active) return;
        /* Der Gegenstand kann in der Zwischenzeit verschwunden sein (Itembox o.ae.). */
        if (s_slot < 0 || s_slot >= RE15_INV_MAX_SLOTS || g_inv.slots[s_slot].id != s_item) {
            re15_discard_reset(); return;
        }
        s_reveal       = 0;
        s_reveal_total = re15_item_prompt_walk(RE15_DISCARD_PROMPT_KEY, s_item,
                                               0, 0, 0);   /* nur zaehlen */
        s_reveal_timer = 1;                    /* Startwert 1 << s1, s1 = 0 (@0x800281a0-ac) */
        s_zustand      = D_FRAGT;
        s_gefragt++;
        return;
    }

    /* --- D_FRAGT: tippen, dann Yes/No ------------------------------------------------ */
    if (s_reveal < s_reveal_total) {
        int budget = 1;                                   /* s2 = 1            @0x800281d8 */
        int t0     = s_reveal_timer;
        s_reveal_timer = t0 - 1;                          /* @0x800281f0/f8               */
        if (((s_reveal_timer & 0xff) != 0) && (pad_held & 0x4000)) {
            s_reveal_timer = t0 - 4;                      /* @0x80028228                  */
            if (DISCARD_MSG_RELOAD < 4) budget = 2;       /* @0x8002822c/38               */
        }
        if (s_reveal_timer > 0) return;                   /* @0x8002823c-48               */
        s_reveal += budget;
        if (s_reveal > s_reveal_total) s_reveal = s_reveal_total;
        s_reveal_timer = DISCARD_MSG_RELOAD;
        return;                                           /* vor dem Ende keine Eingabe   */
    }

    /* Auswahl + Bestaetigen — dieselben virtuellen Bits wie der Aufnahme-Prompt
     * (item_modal_common.c Zustand 6): 0x3000 = Menue links/rechts (roh Steuerkreuz),
     * 0x4000 = BESTAETIGEN (roh SQUARE, Preset-Tabelle @0x80073dbc[14]). */
    if (pad_edge & 0x3000) s_choice ^= 1;
    /* ABBRECHEN mit CROSS (virtuell 0x8000) = "No". Das ist die Konvention des Ports
     * (Bestaetigen SQUARE, Abbrechen CROSS) und KEINE byte-true Regel: RE2s Abfrage
     * kennt nur den Bestaetigen-Knopf (FUN_80030844, DAT_800ce310 & 0x1000), die
     * physische Taste dahinter ist in Untersuchung A ausdruecklich NICHT aufgeloest
     * worden. Wer abbricht, behaelt den Gegenstand — der sichere Ausgang. */
    if (pad_edge & 0x8000) { s_choice = 1; }
    else if (!(pad_edge & 0x4000)) return;

    if (s_choice == 0) {
        /* JA: Slot leeren + nachruecken. RE2: `sb zero,id` @0x80051774,
         * `sb zero,flags` @0x80051794, `jal FUN_80069714` @0x80051798.
         * Der Port hat dafuer re15_inv_remove_slot (Consume @0x8004aef0-af28) samt
         * anschliessender Kompaktierung (FUN_8004dadc). */
        if (s_slot >= 0 && s_slot < RE15_INV_MAX_SLOTS && g_inv.slots[s_slot].id == s_item) {
            re15_inv_remove_slot(s_slot);
            s_weggeworfen++;
        }
        s_zustand = D_AUS; s_item = 0; s_slot = -1;
        s_reveal = s_reveal_total = s_reveal_timer = 0;
        return;
    }
    /* NEIN: Anzahl auf 1 zurueck (@0x800517C4) — sonst liefe sie beim naechsten
     * Gebrauch auf 0xFF unter, und es wuerde nie wieder gefragt. Die Abfrage kommt
     * beim naechsten Gebrauch erneut, genau wie in RE2. */
    re15_discard_reset();
}

int re15_discard_active(void) { return s_zustand != D_AUS; }
int re15_discard_frozen(void) { return s_zustand == D_FRAGT; }

int re15_discard_prompt(uint8_t *out_item, int *out_choice)
{
    if (s_zustand != D_FRAGT) return 0;
    if (out_item)   *out_item   = s_item;
    if (out_choice) *out_choice = s_choice;
    return RE15_DISCARD_PROMPT_KEY;
}

int re15_discard_reveal(void)       { return s_reveal; }
int re15_discard_reveal_total(void) { return s_reveal_total; }
int re15_discard_ready(void)        { return s_zustand == D_FRAGT && s_reveal >= s_reveal_total; }
int re15_discard_gefragt(void)      { return s_gefragt; }
int re15_discard_weggeworfen(void)  { return s_weggeworfen; }
