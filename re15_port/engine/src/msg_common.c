/*
 * RE1.5 Rebuilt — Dialog message duration parser (cross-platform).
 *
 * Models the PSX RE1.5 dialog FSM at 0x80028134 (verarbeite_dialog_
 * zustandsmaschine in render_dialog_state_machine.c). Our engine renders
 * the subtitle text in one go rather than scrolling character-by-character;
 * we still need the total duration so the message stays visible for the
 * same wall-clock period as the original. So we walk the .msg bytes once
 * and accumulate the equivalent number of 30 Hz ticks.
 */
#include "re15_msg.h"
#include "re15_scd.h"   /* g_scd — shared subtitle-tick state */

#define DIALOG_BASE_SCROLL_SPEED  2  /* "g_dialogBasisGeschwindigkeit = 2 << istZweiterPuffer" */

/* Text/raw table dimensions. The PSX target has only ~2MB RAM and is already
 * near the limit (128KB prim buffers + ~358KB incbin assets); the full
 * [256][160]+[256][192] tables (~88KB) overflowed BSS past 0x80200000 → the
 * stack collided with BSS → hang. RE1.5 rooms use only a handful of messages
 * (room1170 = 12), so the PSX build uses a small table; the PC keeps the full
 * domain. msg_id is bound-checked against MSG_TABLE_N everywhere it indexes
 * these tables (durations stays [256] — only 512 B — so any byte id is safe). */
#ifdef PSN00BSDK
#define MSG_TABLE_N    32
#define MSG_TEXT_LEN   128
#define MSG_RAW_LEN    128
#else
#define MSG_TABLE_N    256
#define MSG_TEXT_LEN   160
#define MSG_RAW_LEN    192
#endif

/* Pre-computed duration table populated by host at startup; queried by SCD VM
 * via the registered provider. msg_id is u8, so 256 entries cover the full
 * domain. 0 = no override (engine falls back to 90-frame default). */
static unsigned short s_msg_durations[256];

void re15_msg_install_durations(unsigned char msg_id, int frames)
{
    if (frames < 0)        frames = 0;
    if (frames > 0xFFFF)   frames = 0xFFFF;
    s_msg_durations[msg_id] = (unsigned short)frames;
}

static int re15_msg_lookup_duration(unsigned char msg_id)
{
    return (int)s_msg_durations[msg_id];
}

/* One-line install: hook the SCD VM. Called by the host (PC main) after
 * the duration table has been populated. */
extern void re15_scd_set_msg_duration_provider(int (*)(unsigned char));
void re15_msg_install_provider(void)
{
    re15_scd_set_msg_duration_provider(re15_msg_lookup_duration);
}

int re15_msg_compute_duration(const uint8_t *msg_data,
                              size_t          msg_len,
                              int             buffer_doubling)
{
    if (!msg_data || msg_len < 4) return 0;

    /* Header: 0x04 0x00 (or 0x04 0x02). Skip 2 bytes. */
    size_t i = 0;
    if (msg_data[0] == 0x04) {
        i = 2;
    }

    int doubling     = buffer_doubling ? 1 : 0;
    int scroll_speed = DIALOG_BASE_SCROLL_SPEED << doubling;
    int total_ticks  = 0;

    while (i < msg_len) {
        uint8_t b = msg_data[i];

        if (b == 0x01) {
            /* End-of-message: wait N frames before clearing (state 6),
             * or wait for keypress if N == 0 (state 5, conservatively
             * model as "long" but bounded). */
            if (i + 1 < msg_len) {
                uint8_t n = msg_data[i + 1];
                total_ticks += (n == 0) ? 60 : ((int)n << doubling);
            }
            /* End reached. */
            break;
        }
        if (b == 0x02) {
            /* Mid-page wait (state 3): N frames between pages. */
            if (i + 1 < msg_len) {
                uint8_t n = msg_data[i + 1];
                total_ticks += (n == 0) ? 60 : ((int)n << doubling);
            }
            i += 2;
            continue;
        }
        if (b == 0x03) {
            /* Control code (state→4 in FUN_80028134), 1 byte, no scroll time.
             * Was missing → a 0x03 byte fell through to the printable branch and
             * wrongly inflated the duration. Mirrors re15_msg_decode_text. */
            i += 1;
            continue;
        }
        if (b == 0x04) {
            /* Change scroll speed. */
            if (i + 1 < msg_len) {
                uint8_t n = msg_data[i + 1];
                if (n != 0) {
                    scroll_speed = (int)n << doubling;
                }
            }
            i += 2;
            continue;
        }
        if (b == 0x05) {
            /* Color/attr: 2 bytes (code + 1 arg) per disasm FUN_80028134
             * (`case 5: ... text_ptr += 2`). No time impact. (Was wrongly 3.) */
            i += 2;
            continue;
        }
        if (b == 0x06) {
            /* Call subroutine: 2 bytes. No time impact in our approximation. */
            i += 2;
            continue;
        }
        if (b == 0x07) {
            /* Return from subroutine: 1 byte. */
            i += 1;
            continue;
        }
        if (b == 0x08) {
            /* Line break: 1 byte, no scroll time. */
            i += 1;
            continue;
        }
        if (b == 0x09 || b == 0x0A || b == 0x0B) {
            /* Extended codes — skip 1 arg. */
            i += 2;
            continue;
        }
        if (b == 0x00) {
            /* Space char. NO time added — see the printable-char note below. */
            i += 1;
            continue;
        }

        /* Printable char. NO time added (2026-06-08, byte-exact RE of
         * FUN_80028134): the per-glyph `scroll_speed` is the original's
         * TYPEWRITER reveal (state 1), which we DON'T render — we draw the full
         * decoded line instantly. In the original the COMPLETE readable line is
         * on screen only for the `0x01 N` end-hold (state 6 countdown, N<<dbl;
         * the cutscene .msg all end `01 63` = 99 frames ≈ 3.3 s). Accumulating
         * the typewriter time here held the full line typewriter+hold (~207 f)
         * instead of hold (99 f), so the LAST line lingered ~108 f (3.6 s) into
         * gameplay (no successor Message_on to clear it). Count ONLY the
         * 0x01 N end-hold + 0x02 N page-waits = the original's full-line window. */
        i += 1;
    }

    (void)scroll_speed;   /* now only the 0x04 semantics; typewriter time dropped */
    return total_ticks;
}

/*=========================================================================
 * Runtime .msg → text decode (replaces hardcoded re15_to_re2 subtitle strings)
 *
 * Decodes the .msg body to a printable ASCII string, consuming the control
 * codes exactly as the PSX dialog FSM (FUN_80028134): 0x01 end, 0x02/0x04/0x05/
 * 0x06/0x09-0x0B = code + 1 arg, 0x03/0x07 = 1 byte, 0x08 = newline, 0x00 =
 * space, 0x0C..0xF7 = printable glyph (glyph index = byte). The glyph→ASCII map
 * is the verified ASCII region of the .msg charset (digits @0x0C, ':' @0x16,
 * A-Z @0x1D, a-z @0x3D, punctuation). Newline → space (single-line subtitle).
 *=========================================================================*/

/* Glyph byte → ASCII string (NULL = unknown/extended, skipped). */
static const char *re15_msg_glyph(uint8_t b)
{
    static char one[2];
    one[1] = 0;
    if (b == 0x00) return " ";
    if (b >= 0x0C && b <= 0x15) { one[0] = (char)('0' + (b - 0x0C)); return one; }
    if (b == 0x16) return ":";
    if (b == 0x18) return ",";
    if (b == 0x19) return "\"";
    if (b == 0x1A) return "!";
    if (b == 0x1B) return "?";
    if (b == 0x1C) return "!?";
    if (b >= 0x1D && b <= 0x36) { one[0] = (char)('A' + (b - 0x1D)); return one; }
    if (b == 0x37) return "[";
    if (b == 0x38) return "/";
    if (b == 0x39) return "]";
    if (b == 0x3A) return "'";
    if (b == 0x3B) return "-";
    if (b == 0x3C) return ".";
    if (b >= 0x3D && b <= 0x56) { one[0] = (char)('a' + (b - 0x3D)); return one; }
    if (b == 0x57) return ".";   /* period/ellipsis in the English font (verified:
                                  * main06 "great..." / main15 "Leon: ..." = 0x57×3) */
    if (b == 0xF2) return "...";
    return 0;
}

int re15_msg_decode_text(const uint8_t *msg_data, size_t msg_len,
                         char *out, size_t out_sz)
{
    if (!out || out_sz == 0) return 0;
    out[0] = 0;
    if (!msg_data || msg_len < 2) return 0;

    size_t i = 0, o = 0;
    while (i < msg_len && o + 1 < out_sz) {
        uint8_t b = msg_data[i];
        if (b == 0x01) break;                                   /* end of message */
        if (b == 0x02 || b == 0x04 || b == 0x05 || b == 0x06 ||
            b == 0x09 || b == 0x0A || b == 0x0B) { i += 2; continue; } /* code + 1 arg */
        if (b == 0x03) break;                  /* YES/NO choice marker — question ends here */
        if (b == 0x07)                         { i += 1; continue; }   /* sub-string return */
        if (b == 0x08)                         { out[o++] = ' '; i += 1; continue; } /* newline */
        const char *s = re15_msg_glyph(b);
        if (s) { while (*s && o + 1 < out_sz) out[o++] = *s++; }
        i += 1;
    }
    while (o > 0 && out[o - 1] == ' ') o--;                     /* trim trailing spaces */
    out[o] = 0;
    return (int)o;
}

/* Decoded-text table, parallel to the duration table. */
static char          s_msg_text[MSG_TABLE_N][MSG_TEXT_LEN];
static unsigned char s_msg_text_set[MSG_TABLE_N];

const char *re15_msg_get_text(int msg_id)
{
    if (msg_id < 0 || msg_id >= MSG_TABLE_N || !s_msg_text_set[msg_id]) return 0;
    return s_msg_text[msg_id];
}

/* Raw .msg body store — the renderer parses these bytes directly (with the
 * embedded 0x05 color codes) so glyph colour follows the per-speaker attribute,
 * exactly like the PSX dialog renderer FUN_80028868. */
static unsigned char  s_msg_raw[MSG_TABLE_N][MSG_RAW_LEN];
static unsigned short s_msg_raw_len[MSG_TABLE_N];

void re15_msg_install_text(unsigned char msg_id, const uint8_t *data, size_t len)
{
    if (msg_id >= MSG_TABLE_N) return;   /* PSX: small table — high ids unsupported (no crash) */
    int n = re15_msg_decode_text(data, len, s_msg_text[msg_id], sizeof s_msg_text[0]);
    s_msg_text_set[msg_id] = (unsigned char)(n > 0);
    if (len > sizeof s_msg_raw[0]) len = sizeof s_msg_raw[0];
    for (size_t k = 0; k < len; k++) s_msg_raw[msg_id][k] = data[k];
    s_msg_raw_len[msg_id] = (unsigned short)len;
}

const unsigned char *re15_msg_get_raw(int msg_id, int *out_len)
{
    if (msg_id < 0 || msg_id >= MSG_TABLE_N || s_msg_raw_len[msg_id] == 0) {
        if (out_len) *out_len = 0;
        return 0;
    }
    if (out_len) *out_len = s_msg_raw_len[msg_id];
    return s_msg_raw[msg_id];
}

/* Returns 1 if message `msg_id` is a YES/NO CHOICE prompt — its .msg body carries the 0x03
 * "choice" control code (before the 0x01 end). This distinguishes a REAL YES/NO selection
 * (e.g. "Will you push it?") from a PLAIN message that merely has the Message_on arg2 0x80
 * bit (e.g. ROOM1140's "You've opened the lock"). Both share arg2=0x80,arg3=0xFF — only the
 * .msg content differs — so the SCD opcode handler must consult this, else every arg2&0x80
 * message wrongly enters the selection FSM and parks with no text shown. Walks the control
 * codes exactly like re15_msg_decode_text so a 0x03 that is a code ARGUMENT isn't misread. */
int re15_msg_is_choice(int msg_id)
{
    int len = 0;
    const unsigned char *m = re15_msg_get_raw(msg_id, &len);
    if (!m) return 0;
    for (size_t i = 0; i < (size_t)len; ) {
        uint8_t b = m[i];
        if (b == 0x01) break;                                   /* end of message */
        if (b == 0x03) return 1;                                /* choice marker → YES/NO */
        if (b == 0x02 || b == 0x04 || b == 0x05 || b == 0x06 ||
            b == 0x09 || b == 0x0A || b == 0x0B) { i += 2; continue; }   /* code + 1 arg */
        if (b == 0x07)                          { i += 1; continue; }   /* return */
        i += 1;
    }
    return 0;
}

void re15_msg_load_room_block(const uint8_t *block, int block_size)
{
    if (!block || block_size < 4) return;
    /* off[0] = n_off*2 (the offset-table size in bytes). EVERY one of the n_off
     * entries off[0..n_off-1] is a message START — there is NO trailing terminator
     * offset (the original FUN_80027e68 indexes `off[id]` directly and the dialog
     * FSM FUN_80028134 stops at the body's own 0x01 end-code, so it never needs
     * off[id+1]). Message count = n_off (NOT n_off-1). The body for message i runs
     * to off[i+1] when that is a valid greater offset, else to the block end — this
     * is what bounds the LAST message (e.g. room1170 main16/id 16, off[16]=785),
     * which the old `n_msg=n_off-1` rule silently dropped, so Message_on(16) found
     * no text/duration and rendered nothing. SCD Message_on(N) indexes this table
     * directly — the same `off[N]` the original room SCD uses. */
    int off0  = block[0] | (block[1] << 8);
    int n_off = off0 / 2;
    if (n_off < 1) return;
    int n_msg = n_off;
    if (n_msg > MSG_TABLE_N) n_msg = MSG_TABLE_N;
    for (int i = 0; i < n_msg; i++) {
        int o0 = block[i * 2] | (block[i * 2 + 1] << 8);
        /* End = next offset if it is a strictly-greater, in-range value; otherwise
         * (the last message) the message-block end. */
        int o1 = block_size;
        if (i + 1 < n_off) {
            int nxt = block[(i + 1) * 2] | (block[(i + 1) * 2 + 1] << 8);
            if (nxt > o0 && nxt <= block_size) o1 = nxt;
        }
        if (o1 <= o0 || o0 >= block_size) continue;
        const uint8_t *m = block + o0;
        size_t len = (size_t)(o1 - o0);
        int d = re15_msg_compute_duration(m, len, /*buffer_doubling=*/0);
        if (d > 0 && d < 65535) re15_msg_install_durations((unsigned char)i, d);
        re15_msg_install_text((unsigned char)i, m, len);
    }
    re15_msg_install_provider();
}

/* ===== BYTE-TRUE dialog typewriter FSM (FUN_80028134) ======================
 *
 * Open a message into the typewriter FSM: reveal glyphs one-by-one, pause at each 0x02
 * page break for the action button (down-arrow), let the action button HELD fast-forward
 * the typing, then 0x03 → YES/NO. The render draws only the revealed window [message_page,
 * message_parse). `blocking` = the SCD thread parks until the FSM resolves (choice / yes-no);
 * non-blocking examine lines (re15_scd_show_message) display in parallel. */
void re15_dialog_open(int msg_id, int blocking)
{
    g_scd.message_id          = (uint8_t)msg_id;
    g_scd.message_active      = 1;
    g_scd.message_fsm_active  = 1;
    g_scd.message_fsm         = 0;     /* TYPING */
    g_scd.message_parse       = 0;
    g_scd.message_page        = 0;
    g_scd.message_scroll      = 2;     /* DAT_800b8524 default: 2 frames per glyph */
    g_scd.message_timer       = 1;
    g_scd.message_color       = 0;
    g_scd.message_select      = 0;
    g_scd.message_choice      = 0;
    g_scd.message_blink       = 0;
    g_scd.message_query       = blocking ? 1 : 0;
    g_scd.message_display_frames = 2;  /* keep the box alive while the FSM owns the lifetime */
}

/* Advance the dialog FSM one frame (byte-true FUN_80028134). Reads the press EDGE
 * (g_aot_action_pressed → page-advance / yes-no confirm) and the HELD level
 * (g_scd_action_held → fast-forward). Clears message_active when the line is fully
 * dismissed; a YES/NO confirm writes the answer to flag (12,31). */
static void re15_dialog_step(void)
{
    extern uint8_t  g_aot_action_pressed;
    extern uint8_t  g_scd_action_held;
    extern uint16_t g_scd_pad_edge;
    const uint16_t PAD_LEFT = 0x0080, PAD_RIGHT = 0x0020;

    int rlen = 0;
    const unsigned char *raw = re15_msg_get_raw((int)g_scd.message_id, &rlen);
    if (!raw || rlen <= 0) { g_scd.message_active = 0; return; }

    int act_edge = g_aot_action_pressed ? 1 : 0;
    int act_held = g_scd_action_held ? 1 : 0;
    int lr_edge  = (g_scd_pad_edge & (PAD_LEFT | PAD_RIGHT)) != 0;
    /* [#36a] page-wait (andi 0xc000 @0x80028458) and end-wait (andi 0xc000
     * @0x80028698) dismiss on EITHER action button, fresh press. g_scd_pad_edge is
     * the standard PSX logical pad word (Cross=0x4000, Square=0x8000). */
    int dismiss_edge = (g_scd_pad_edge & (0x4000u | 0x8000u)) != 0;

    switch (g_scd.message_fsm) {
    case 0: {  /* TYPING */
        int dec = (act_held && g_scd.message_scroll < 4) ? 4 : 1;   /* button = fast-forward */
        if (g_scd.message_timer > dec) { g_scd.message_timer = (uint16_t)(g_scd.message_timer - dec); break; }
        g_scd.message_timer = 0;
        for (;;) {
            if (g_scd.message_parse >= rlen) { g_scd.message_fsm = 6; break; }
            unsigned char b = raw[g_scd.message_parse];
            if (b == 0x01) {               /* end: arg 0 → wait button, N → hold N */
                g_scd.message_parse++;
                unsigned char a = (g_scd.message_parse < rlen) ? raw[g_scd.message_parse] : 0;
                g_scd.message_parse++;
                if (a == 0) g_scd.message_fsm = 4;
                else { g_scd.message_fsm = 5; g_scd.message_timer = a; }
                break;
            }
            if (b == 0x02) {               /* page break: arg 0 → wait button, N → timed */
                unsigned char a = (g_scd.message_parse + 1 < rlen) ? raw[g_scd.message_parse + 1] : 0;
                if (a == 0) g_scd.message_fsm = 1;
                else { g_scd.message_fsm = 2; g_scd.message_timer = a; }
                break;                      /* parse stays AT the 0x02 (page = [page,parse)) */
            }
            if (b == 0x03) { g_scd.message_fsm = 3; g_scd.message_select = 1; break; }  /* yes/no */
            if (b == 0x05) { g_scd.message_color = (g_scd.message_parse + 1 < rlen) ? raw[g_scd.message_parse + 1] : 0; g_scd.message_parse += 2; continue; }
            if (b == 0x04) { unsigned char a = (g_scd.message_parse + 1 < rlen) ? raw[g_scd.message_parse + 1] : 2; if (a) g_scd.message_scroll = a; g_scd.message_parse += 2; continue; }
            if (b == 0x08) { g_scd.message_parse++; continue; }   /* newline (render breaks) */
            g_scd.message_parse++;          /* reveal one glyph, then wait scroll frames */
            g_scd.message_timer = g_scd.message_scroll;
            break;
        }
        break;
    }
    case 1:  /* PAGE_WAIT — down-arrow, advance on a fresh action press */
        g_scd.message_blink--;
        if (dismiss_edge) { g_scd.message_parse += 2; g_scd.message_page = g_scd.message_parse;
                        g_scd.message_fsm = 0; g_scd.message_timer = g_scd.message_scroll; }  /* [#36a/e] */
        break;
    case 2:  /* PAGE_TIMED */
        if (g_scd.message_timer > 1) g_scd.message_timer--;
        else { g_scd.message_parse += 2; g_scd.message_page = g_scd.message_parse;
               g_scd.message_fsm = 0; g_scd.message_timer = g_scd.message_scroll; }  /* [#36e] DAT_800b8525=DAT_800b8524 @0x80028538 */
        break;
    case 3:  /* SELECT — YES/NO (left/right toggles, action confirms) */
        g_scd.message_blink--;
        if (lr_edge) { g_scd.message_choice ^= 1; g_scd.message_blink = 0; }
        if (act_edge) {
            re15_game_flag_set(12, 31, g_scd.message_choice);  /* 0=YES → Ck(12,31,0) true */
            g_scd.message_fsm = 6;
        }
        break;
    case 4:  /* END_WAIT — wait for a fresh action press to dismiss */
        if (dismiss_edge) g_scd.message_fsm = 6;          /* [#36a] Cross|Square @0x80028698 */
        break;
    case 5:  /* END_TIMED — auto-dismiss when the hold expires */
        if (g_scd.message_timer > 1) g_scd.message_timer--;
        else g_scd.message_fsm = 6;
        break;
    }
    if (g_scd.message_fsm == 6) {          /* DONE — fully reset so the player UNFREEZES */
        g_scd.message_active         = 0;
        g_scd.message_select         = 0;
        g_scd.message_fsm_active     = 0;
        g_scd.message_display_frames = 0;  /* msg_block (player_common.c) gates the player on
                                            * this — leaving it >0 froze Leon forever after a
                                            * dialog. The FSM owns the lifetime via the states,
                                            * not the countdown, so zero it on dismissal. */
    }
}

/* Shared cross-port subtitle tick / dismiss FSM. Mirrors FUN_80028134's end-wait:
 * a subtitle lives for exactly its own duration countdown and is dismissed only when
 * that hits 0 — no Cut_chg / Plc_ret / letterbox-close shortcut (the PSX-vs-PC
 * divergence that silently killed sub14's last line main16). The single shared
 * lifetime here guarantees PSX and PC behave identically; the caller only does the
 * platform glyph draw with the returned raw body. */
int re15_msg_tick(const unsigned char **out_raw, int *out_len, int *out_msg_id)
{
    if (out_raw)    *out_raw    = 0;
    if (out_len)    *out_len    = 0;
    if (out_msg_id) *out_msg_id = (int)g_scd.message_id;

    /* TYPEWRITER message (examine + choice — the whole-game text mechanism): advance the
     * byte-true FSM one frame, then draw ONLY the revealed window of the current page,
     * [message_page, message_parse) — exactly FUN_80028868's [DAT_800b8528, DAT_800b852c).
     * The FSM owns the lifetime (it clears message_active on dismiss). */
    if (g_scd.message_fsm_active) {
        if (!g_scd.message_active) return 0;
        re15_dialog_step();
        if (!g_scd.message_active) return 0;          /* dismissed this frame */
        int rlen = 0;
        const unsigned char *raw = re15_msg_get_raw((int)g_scd.message_id, &rlen);
        int start = (int)g_scd.message_page;
        int end   = (int)g_scd.message_parse;
        if (end > rlen) end = rlen;
        if (start > end) start = end;
        if (out_raw) *out_raw = raw + start;
        if (out_len) *out_len = end - start;
        return 1;
    }

    /* Legacy all-at-once timed display — kept ONLY for the verified VOICED cinematic
     * subtitles (intro) so their tuned timing is untouched. */
    if (g_scd.message_display_frames <= 0)
        return 0;
    int rlen = 0;
    const unsigned char *raw = re15_msg_get_raw((int)g_scd.message_id, &rlen);
    if (out_raw)    *out_raw = raw;
    if (out_len)    *out_len = rlen;
    /* Decrement AFTER latching the body so the message is shown on its final tick
     * too (matches the original count-then-clear order). */
    g_scd.message_display_frames--;
    return 1;
}

/* Shared .msg text layout walk — see re15_msg.h. Control-code semantics are
 * byte-identical to FUN_80028868; the port supplies glyph emission + pen width. */
void re15_msg_layout(int x, int y, const unsigned char *raw, int len,
                     re15_msg_glyph_fn glyph_fn, re15_msg_width_fn width_fn,
                     void *ctx)
{
    if (!raw || len <= 0 || !glyph_fn || !width_fn) return;
    const int line_h = 16;
    int penx = x, peny = y, attr = 0;          /* attr 0 = white (default) */
    int space_w = width_fn(0x00, ctx);
    if (space_w <= 0) space_w = 5;
    for (int i = 0; i < len; ) {
        unsigned char b = raw[i];
        if (b == 0x01) break;                                  /* end of message  */
        if (b == 0x05) { attr = (i + 1 < len) ? (raw[i + 1] & 7) : 0; i += 2; continue; }
        if (b == 0x02 || b == 0x04 || b == 0x06 ||
            b == 0x09 || b == 0x0A || b == 0x0B) { i += 2; continue; } /* code + arg */
        if (b == 0x03) break;        /* YES/NO choice marker (2-byte 0x03 N): the QUESTION ends
                                      * here; Yes/No are system strings drawn separately. Render
                                      * stops at the 0x03 (byte-true FUN_80028868). */
        if (b == 0x07) { i += 1; continue; }                   /* sub-string return */
        if (b == 0x08) { penx = x; peny += line_h; i += 1; continue; } /* newline  */
        if (b == 0x00) { penx += space_w; i += 1; continue; }  /* space           */
        glyph_fn(penx, peny, b, attr, ctx);                    /* printable glyph */
        penx += width_fn(b, ctx);
        i += 1;
    }
}
