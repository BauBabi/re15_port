/*
 * re15_itembox.h — die ITEM BOX, VOLLSTAENDIG nach RESIDENT EVIL 2.
 *
 * NUTZER-AUFTRAG 2026-08-30: "sichere die aktuelle Item Box implementierung, aber
 * eigentlich waere mir doch lieb, wenn du die Itembox mit dem KOMPLETTEN Mechanismus
 * aus Resident Evil 2 uebernimmst und portierst."
 * Der vorherige HYBRID-Stand (RE2-Transfer + selbst erfundene 4x8-Seiten mit L1/R1)
 * ist gesichert unter analysis/itembox_re2/backup_hybrid_v1/ (Tag itembox-hybrid-v1,
 * Branch backup/itembox-hybrid-v1). Was sich aendert, steht dort im README.
 *
 * DAS RE2-MODELL (alles aus RE2-Leon SLUS-00748 belegt — Ableitung:
 * analysis/itembox_re2/re2-box-transfer.md):
 *   - BOX-Array @DAT_800d4a68 + slot*4, Slot-Format {id@+0, qty@+1, size@+2} —
 *     identisch zum Inventar-Format @DAT_800d4a3c (RE1.5 nutzt dieselbe 4-Byte-Form).
 *   - KAPAZITAET 64 Plaetze: die Auswahl-Formel maskiert mit 0x3f
 *     (`uVar15 = DAT_800d5c14 + 2 & 0x3f`, FUN_800703b8 Kopf).
 *   - RING statt Seiten: DAT_800d5c14 ist ein SCROLL-Stand; der ausgewaehlte Platz
 *     ist IMMER `(scroll + 2) & 0x3f` — der Cursor steht fest, der Inhalt wandert.
 *   - size 3 = "2-Zellen-Waffe liegt in der Box" (`sb 3 -> box.size`, Faelle 2/3).
 *   - Transfer = FUN_800703b8: EINE Operation (Tausch inv[cursor] <-> box[pick]) mit
 *     Munitions-Sonderweg, 2-Zellen-Behandlung und Abweisung + Melde-Zustand 5.
 *
 * RE1.5-ANPASSUNGEN (unvermeidbar, jeweils benannt): Munitions-Id-Bereich und
 * Kapazitaets-Tabelle stammen aus RE1.5 (0x15..0x21 / Prop-Tabelle @0x80074DA8)
 * statt RE2s 0x14..0x1F / DAT_800a9e1c; das Inventar hat 10 Plaetze ohne RE2s
 * Personal-Slot 10.
 */
#ifndef RE15_ITEMBOX_H
#define RE15_ITEMBOX_H

#include <stdint.h>
#include "re15_inventory.h"   /* re15_inv_slot_t {id,qty,flags(kind),pad} */

/* KAPAZITAET: 64 — die Auswahl-Maske 0x3f in FUN_800703b8 (`DAT_800d5c14 + 2 & 0x3f`)
 * definiert den Ring; Box-Array @0x800d4a68..0x800d4b68 = 64 * 4 Byte. */
#define RE15_BOX_SLOTS      64
/* Sichtbares Fenster in den Ring und die FESTE Cursor-Position darin.
 * RE2: der gewaehlte Platz ist immer `scroll + 2` — der Cursor bewegt sich NIE,
 * der Inhalt wandert unter ihm durch. Unser Box-Panel fasst 10 Zellen, zeigt also
 * die Ring-Plaetze scroll..scroll+9; die Auswahl sitzt fest auf Zelle 2. */
#define RE15_BOX_WINDOW     10
#define RE15_BOX_PICK_ROW    2

/* SCROLL-ANIMATION (RE2 Sub-Zustaende 2/3, @0x8007000c / @0x8007019c):
 * 6 Animations-Frames zu je 3 Pixeln, danach 1 Commit-Frame, der den Scroll-Stand
 * um 1 weiterschaltet und den Pixel-Versatz nullt = 7 Frames je Zeile. RE2s
 * Zeilenraster ist 20 px, die Animation legt nur 18 px zurueck — die restlichen
 * 2 px macht der Commit als Sprung (Beleg: analysis/itembox_re2/re2-box-screen.md).
 * Die Schulter-Tasten springen OHNE Animation (Sofort-Commit @0x8007005c). */
#define RE15_BOX_SCROLL_FRAMES  6
#define RE15_BOX_SCROLL_STEP_PX 3
#define RE15_BOX_ROW_PX        20

/* TASTEN-WIEDERHOLUNG des Item-Schirms (RE2 FUN_800689bc: Maske 0xf01c,
 * Parameter 0x060a @0x800689f4) = Anlauf 10 Frames, danach alle 6 Frames.
 * Gehaltenes HOCH/RUNTER umgeht das Gate (Zustaende 2/3 pruefen den rohen
 * Halte-Zustand selbst) und laeuft deshalb gleichmaessig 1 Zeile je 7 Frames. */
#define RE15_BOX_REPEAT_DELAY  10
#define RE15_BOX_REPEAT_RATE    6

/* 2-Zellen-Waffe in der Box: size 3 (RE2 `sb 3 -> box.size` @0x80070ad0-dc / @0x80070c30-3c). */
#define RE15_BOX_KIND_WIDE  3

typedef struct {
    /* FLACHER 64-Platz-Ring — 1:1 RE2s Box-Array @DAT_800d4a68 (Schrittweite 4,
     * {id,qty,size,pad}). Kein Seiten-Modell mehr. */
    re15_inv_slot_t slots[RE15_BOX_SLOTS];
} re15_itembox_t;

extern re15_itembox_t g_itembox;

/* Zero the whole box — the RE1.5 new-game init shape (FUN_8003e4f4 zero loop
 * @0x8003e52c-554 clears all four arrays; box starts empty like RE2's
 * new-game zero region, spec §1). */
void re15_itembox_init(void);

/* ---- transfer engine — RE2 FUN_800703b8 semantics (ONE operation: SWAP
 * inv[cursor] ⇄ box[page][slot]; no deposit/withdraw commands, spec §3) -------- */
enum {
    RE15_BOX_XFER_OK     = 0,
    RE15_BOX_XFER_REJECT = 1   /* THE ONLY reject: box holds a 2-cell weapon and
                                * empties==0 || (empties==1 && cursor empty)
                                * (@0x800707dc-ec). No box-full reject exists
                                * (fixed slots, 1:1 swap — spec §3 tail). */
};
int re15_itembox_transfer(int inv_cursor, int box_slot);

/* Compaction-on-open (RE2 box task: FUN_80069714 @0x80068c60 = quirk 9) is done
 * by the shared menu phase-0 init (re15_inv_compact @0x800464a0 — the RE1.5
 * status screen compacts at open too, so ONE call covers both cites). */

/* ---- trigger registry (the 16 safe-room box AOTs) --------------------------- */
/* 1 if a Message_on of msg_id in room_id is the shipped "Itembox is not
 * available in this preview" flavor message (the box AOT identity). */
int  re15_itembox_is(unsigned room_id, uint8_t msg_id);

/* One-shot open request (save-phone precedent, re15_savepoint_pending shape). */
int  re15_itembox_pending(void);
void re15_itembox_set_pending(int on);
void re15_itembox_reset(void);   /* clear pending on room change */

/* ---- box screen FSM (status-screen-style subscreen; menu substate 4) --------
 * Cursor model per RE2's box FSM (spec §2.3/§2.4, panels @0x800a9bb4 + sub-state
 * DAT_800d5bf2): panel 2 = EXIT row, panel 3 = main; main states 0 = inventory
 * grid, 1 = box side, 4 = swap (transient), 5 = message wait. RE2 states 2/3
 * (6-frame scroll anim) have NO analog — replaced by the instant page flip
 * [DESIGN, §6]. Ticked by menu_common.c when substate==4. */
void re15_itembox_screen_open(void);                    /* on subscreen entry   */
int  re15_itembox_screen_tick(uint16_t pressed, uint16_t held); /* 1 = close req */

/* introspection (tests) */
int  re15_itembox_screen_state(void);   /* main state 0/1/5 (RE2 DAT_800d5bf2)  */
int  re15_itembox_screen_panel(void);   /* 3 = main, 2 = EXIT row (DAT_800d5bf1)*/
int  re15_itembox_screen_scroll(void);  /* Scroll-Stand 0..63 (RE2 DAT_800d5c14) */
int  re15_itembox_screen_pick(void);    /* gewaehlter Platz = (scroll+2)&0x3f    */
int  re15_itembox_screen_pixoff(void);  /* Scroll-Pixelversatz (RE2 DAT_800d5c15,
                                         * vorzeichenbehaftet: 0,3..18 bzw. 0,-3..-18) */

/* flat accessors for persistence (savedata v4 box[32]: page*8+i order). */
void re15_itembox_export(re15_inv_slot_t out[RE15_BOX_SLOTS]);
void re15_itembox_import(const re15_inv_slot_t in[RE15_BOX_SLOTS]);

#endif /* RE15_ITEMBOX_H */
