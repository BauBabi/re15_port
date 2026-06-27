/*
 * RE1.5 Rebuilt — PSX dialog message duration parser.
 *
 * PSX RE1.5 .msg files contain message text with embedded control codes:
 *   0x01 + N = wait-at-end (state 6 = timed close, N << buffer_doubling frames)
 *   0x02 + N = wait-mid-page (state 3 = timed wait between pages)
 *   0x04 + N = change scroll speed (N ticks per char)
 *   0x05 + RR GG = color change (3 bytes total)
 *   0x06 + N = call subroutine
 *   0x07 = return from sub
 *   0x08 = line break
 *   0x09-0x0B = extended codes (skip)
 *   Other bytes 0x0C-0xF7 = printable char (counts toward scroll time)
 *
 * Reference: RE_15_modified_V2/render_dialog_state_machine.c
 * (FUN @ 0x80028134 in PSX RE1.5).
 *
 * Total display duration =
 *     scroll_time (chars × scroll_speed)
 *   + wait_codes (page waits + end hold)
 *
 * The buffer_doubling flag (`istZweiterPuffer`) multiplies all wait
 * durations and the base scroll speed by 2 for PAL/double-buffer mode.
 * Set to 1 for canonical cinematic timing (matches ablauf observation).
 */
#ifndef RE15_MSG_H
#define RE15_MSG_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Compute total dialog display duration in 30Hz ticks for a parsed
 * .msg file body. The body starts with the message-begin code (0x04 + 0x00
 * or 0x04 + 0x02) and ends with a 0x01-followed-by-duration sequence.
 *
 * Returns:
 *   total_frames > 0 = scroll + holds combined, scaled per
 *                      buffer_doubling.
 *   0                = malformed / empty.
 *
 * buffer_doubling: 0 = single-buffer (NTSC-style), 1 = double-buffer
 *                  (PAL-style). RE1.5 cinematic mode uses 1.
 */
int re15_msg_compute_duration(const uint8_t *msg_data,
                              size_t          msg_len,
                              int             buffer_doubling);

/* Populate per-message duration table (msg_id u8 → frames). */
void re15_msg_install_durations(unsigned char msg_id, int frames);

/* Decode a .msg body to a printable ASCII string (control codes consumed per the
 * PSX dialog FSM; newline → space). Returns chars written (excl. NUL). This is
 * the runtime replacement for the hardcoded re15_to_re2_room1170_subtitle table. */
int re15_msg_decode_text(const uint8_t *msg_data, size_t msg_len,
                         char *out, size_t out_sz);

/* Decode + store the text for msg_id (queried later via re15_msg_get_text). */
void re15_msg_install_text(unsigned char msg_id, const uint8_t *data, size_t len);

/* Return the decoded subtitle text for msg_id, or NULL if none. */
const char *re15_msg_get_text(int msg_id);

/* Return the raw .msg body for msg_id (with embedded control/color codes) so the
 * renderer can draw glyphs in the per-speaker colour (0x05 code). NULL if none. */
const unsigned char *re15_msg_get_raw(int msg_id, int *out_len);

/* 1 if message `msg_id` is a YES/NO CHOICE prompt (its .msg has the 0x03 choice code).
 * Used to gate the Message_on YES/NO selection FSM (arg2&0x80 alone is NOT enough — a
 * plain blocking message shares arg2=0x80; only the .msg content distinguishes them). */
int re15_msg_is_choice(int msg_id);

/* Open a message into the byte-true typewriter FSM (FUN_80028134): glyph-by-glyph reveal,
 * 0x02 page break = wait for the action button (down-arrow), action HELD = fast-forward,
 * 0x03 = YES/NO. `blocking` = the SCD thread parks until resolved (choice/yes-no); 0 = a
 * non-blocking examine line. The FSM is advanced by re15_msg_tick each frame. */
void re15_dialog_open(int msg_id, int blocking);

/* Hook our msg duration table into the SCD VM Message_on opcode. */
void re15_msg_install_provider(void);

/* Generic per-room loader: interpret a raw RDT MESSAGE block (RDT+0x3c) and
 * install every message's text + duration, then the provider. Standard RE format:
 * u16 off[n_off]; off[0] = n_off*2; EVERY off[0..n_off-1] is a message START (no
 * trailing terminator — the original indexes off[id] and each body self-terminates
 * on its 0x01 end-code), so message count = n_off; body i = block[off[i]..off[i+1]]
 * (or ..block_size for the last). Replaces the old room1170 incbin — works
 * generically for every room. `block_size` must span all bodies (the next RDT
 * block boundary, NOT off[n_off-1] — see rdt_common.c). */
void re15_msg_load_room_block(const uint8_t *block, int block_size);

/* Shared subtitle-display tick (the cross-port dialog-dismiss FSM).
 *
 * Both the PSX and PC main loops must drive the on-screen subtitle IDENTICALLY:
 * a message stays up purely for its own duration countdown (g_scd.message_display_
 * frames), and is dismissed SOLELY when that reaches 0 — exactly the original
 * FUN_80028134 end-wait (DAT_800b8525). Nothing else (Cut_chg / Plc_ret / Cut_auto
 * / letterbox-close) may clear it; that divergence is what dropped sub14's main16.
 *
 * Call once per rendered frame. If a subtitle is live it returns its raw .msg body
 * (with embedded 0x05 colour codes) via *out_raw / *out_len for the caller's
 * platform glyph renderer, and decrements the countdown by one tick. Returns 1 when
 * a subtitle should be drawn this frame, 0 otherwise. The ONLY per-port code left is
 * the actual glyph draw (PSX SPRT vs PC framebuffer), keeping the lifetime logic in
 * one place. */
int re15_msg_tick(const unsigned char **out_raw, int *out_len, int *out_msg_id);

/* Shared .msg text LAYOUT walk (the cross-port glyph-placement loop).
 *
 * The control-code parsing is byte-identical to FUN_80028868 and was duplicated
 * verbatim in both ports' glyph renderers (a drift risk — a control-code fix in one
 * port silently desyncs the other). This walks the body ONCE and calls back per
 * printable glyph; the ONLY per-port code left is the glyph EMISSION (PSX SPRT+
 * DR_TPAGE vs PC software blit) and the per-glyph pen ADVANCE (the two ports use
 * different width tables — DEBUG.BIN-derived but stored separately).
 *
 * Codes: 0x01 end · 0x05 colour attr (next byte &7) · 0x02/04/06/09/0A/0B = code+1
 * arg (skipped) · 0x03/07 skip · 0x08 newline (16px) · 0x00 space · else printable
 * glyph at (penx,peny) with attr, then pen += width_fn(glyph). `ctx` is passed
 * through to both callbacks. */
typedef void (*re15_msg_glyph_fn)(int penx, int peny, unsigned char glyph,
                                  int attr, void *ctx);
typedef int  (*re15_msg_width_fn)(unsigned char glyph, void *ctx);
void re15_msg_layout(int x, int y, const unsigned char *raw, int len,
                     re15_msg_glyph_fn glyph_fn, re15_msg_width_fn width_fn,
                     void *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RE15_MSG_H */
