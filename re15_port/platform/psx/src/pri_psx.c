/*
 * RE1.5 Rebuilt — sprite.pri FOREGROUND ATLAS, PSX backend (2026-06-09).
 *
 * Byte-true port of the original foreground-occlusion source: each camera cut that
 * has foreground occluders (railings, boxes, light fixtures) carries an 8-bit atlas
 * (our BSS-SLD decode — SldDecoder/FUN_800c47e8, VRAM-verified byte-exact). On
 * cut-change we CD-load that cut's atlas TIM and upload it to a free VRAM slot; the
 * sprite.pri SPRTs (emitted in render.c, byte-true FUN_800392d4/FUN_80039590) sample
 * it so the foreground occludes the actors.
 *
 * VRAM placement (our layout — the original used (320,256)/(0,480), which here is
 * the heli prop + CLUT band): image at (384,256), CLUT at (384,490). Both sit in the
 * free region (384..640, 256..496) — RIGHT of the heli (320..384), BELOW the obj
 * props (y<256), LEFT of the BG cache (x>=640). The ROOM1170 masks only sample
 * srcY<=88, so 128 uploaded rows is ample.
 */
#include <stdint.h>
#include <stdio.h>
#include <psxgpu.h>
#include "re15_cdfs.h"
#include "re15_room.h"
#include "re15_tim.h"
#include "re15_sld.h"

/* bg_psx.c — der zuletzt geladene BSS-Cut-Chunk (Quelle des Vordergrundatlas). */
extern const uint8_t *re15_bg_last_chunk(int *out_size, int *out_cut);

/* The obj-prop bump allocator (asset_psx.c) packs TWO 256-tall slots per 64-wide
 * column from x384, so the 5 ROOM1170 props (obj 0/1/3/4/5; obj2=heli is separate)
 * fill (384,0),(384,256),(448,0),(448,256),(512,0) — leaving the 6th slot (512,256)
 * UNUSED and x576..640 untouched. So the atlas image lands at (512,256) in that free
 * (512..640, 256..512) block; CLUT at (0,502) (below the framebuffers at y<480 and the
 * obj/leon/elliot/heli CLUT band at y480..491). An EARLIER (384,256) placement
 * overwrote the 2nd prop row = obj01 = the helipad box → "box top broken". (Multi-room
 * caveat: a 6-prop room would claim (512,256) → reserve a dedicated atlas slot then.) */
#define RE15_PRI_VRAM_X    512
#define RE15_PRI_VRAM_Y    256
/* ⛔ WAR 128 mit der Begruendung "die ROOM1170-Masken sampeln nur srcY<=88, 128 Zeilen
 * reichen". Das galt, solange nur ROOM1170 lief. Gemessen ueber ALLE Masken des Spiels
 * (Original + nachgezeichnet): die groesste benutzte Atlas-Zeile ist 256, und 2080 der
 * 16654 Masken sampeln jenseits von Zeile 128 — sie haetten auf der PSX Fremdinhalt aus
 * dem VRAM gezogen. Der freie Block ist laut Kommentar oben (512..640, 256..512), also
 * genau 256 Zeilen hoch; die Erhoehung bleibt darin. */
#define RE15_PRI_VRAM_ROWS 256
#define RE15_PRI_CLUT_X    0
#define RE15_PRI_CLUT_Y    502

/* Read by render.c's re15_render_pri_sprites(). */
int      re15_pri_psx_ok    = 0;   /* 1 = current cut has a loaded foreground atlas */
uint16_t re15_pri_psx_tpage = 0;   /* 8-bit tpage handle for the atlas              */
uint16_t re15_pri_psx_clut  = 0;   /* CLUT handle for the atlas                      */

/* Vordergrundatlas des Cuts in den VRAM bringen. Rueckgabe 1 = dieser Cut hat einen.
 *
 * ⛔ VORHER wurde ausschliesslich die vorextrahierte Datei \BSS\ROOM####\PRI##.TIM
 * geladen. Die existiert nur fuer 209 der 359 Cuts, die im Original ueberhaupt einen
 * Vordergrund tragen — fuer die uebrigen fiel die Verdeckung ersatzlos aus. Auf dem
 * PC-Ziel ist genau dieser Defekt behoben (bg_pc.c), die PSX zog bis hier nach.
 *
 * Jetzt zuerst der byte-treue Weg des Originals: der Trailer am Ende der geladenen
 * Cut-Daten zeigt auf den SLD-Block IM BSS-Chunk (s. re15_sld.h, FUN_80021bbc).
 * Den Chunk hat re15_bg_load_cut() unmittelbar vorher geladen und veroeffentlicht —
 * ein zweiter CD-Zugriff waere auf echter Hardware eine spuerbare Verzoegerung.
 * Die Laenge L kommt aus der erzeugten Tabelle statt aus der 137 KB grossen
 * Overlay-Datei (re15_sld_used_len_tab, gegen Drift durch unit_sld_atlas gesichert).
 *
 * ⚠️ UNGETESTET: das PSX-Ziel baut mit PSn00bSDK 0.24 derzeit nicht (FindPSn00bSDK
 * erwartet das alte Layout). Der Code spiegelt den auf dem PC gemessenen Pfad, ist
 * aber auf Hardware bislang nicht ausgefuehrt worden. */
static uint8_t s_sld_tim[RE15_SLD_MAX_UNPACKED];

static int pri_upload_tim(const uint8_t *data, int n)
{
    re15_tim_t t;
    if (re15_tim_parse(data, n, &t) != 0 || t.bpp != 8 || !t.has_clut) return 0;

    int rows = (t.height < RE15_PRI_VRAM_ROWS) ? t.height : RE15_PRI_VRAM_ROWS;
    RECT pr = { RE15_PRI_VRAM_X, RE15_PRI_VRAM_Y, (short)(t.width / 2), (short)rows };
    LoadImage(&pr, (const uint32_t *) t.pixels);
    RECT cr = { RE15_PRI_CLUT_X, RE15_PRI_CLUT_Y, 256, 1 };
    LoadImage(&cr, (const uint32_t *) t.clut);
    DrawSync(0);

    re15_pri_psx_tpage = getTPage(1, 0, RE15_PRI_VRAM_X, RE15_PRI_VRAM_Y);
    re15_pri_psx_clut  = getClut(RE15_PRI_CLUT_X, RE15_PRI_CLUT_Y);
    re15_pri_psx_ok    = 1;
    return 1;
}

int re15_pri_psx_load_cut(int cut_idx)
{
    re15_pri_psx_ok = 0;
    if (cut_idx < 0) return 0;

    /* 1. Laufzeit-Auszug aus dem BSS-Chunk (deckt alle 359 Cuts). */
    {
        int chunk_size = 0, chunk_cut = -1;
        const uint8_t *chunk = re15_bg_last_chunk(&chunk_size, &chunk_cut);
        if (chunk && chunk_size > 0 && chunk_cut == cut_idx) {
            uint16_t L = 0;
            int stage = (int)((g_current_room_id >> 12) & 0xF);
            int room  = (int)((g_current_room_id >> 4)  & 0xFF);
            if (re15_sld_used_len_tab(stage, room, cut_idx, &L) == RE15_SLD_OK) {
                int len = 0;
                if (re15_sld_atlas_from_chunk(chunk, chunk_size, L,
                                              s_sld_tim, (int)sizeof s_sld_tim, &len)
                        == RE15_SLD_OK && pri_upload_tim(s_sld_tim, len))
                    return 1;
            }
        }
    }

    /* 2. Rueckfall: vorextrahierte Datei (Alt-Baum). */
    {
        char name[40];
        sprintf(name, "\\BSS\\ROOM%04X\\PRI%02d.TIM;1", g_current_room_id, cut_idx);
        int n = re15_cd_load_file(name, re15_cd_staging, RE15_CD_STAGING_SIZE);
        if (n > 0 && pri_upload_tim(re15_cd_staging, n)) return 1;
    }
    return 0;
}
