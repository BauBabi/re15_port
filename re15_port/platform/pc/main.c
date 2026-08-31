/*
 * RE1.5 Rebuilt — PC port main (Phase 4.2..4.5.4, 2026-05-18).
 *
 * Cross-platform: builds on Windows (MSVC, gcc-mingw) and Linux (gcc, clang).
 * Renders into a 320x240 SDL2 window scaled 4x (1280x960). VSync drives the
 * main loop at 60 FPS. Same splash + demos as the PSX target.
 *
 * Portability notes:
 *   - All output uses snprintf (no MSVC-only sprintf_s / no Linux-only asprintf)
 *   - SDL2 is the only platform-API dependency, statically linked via FetchContent
 *   - File I/O via plain fopen/fread (POSIX + Windows CRT)
 *   - No <windows.h>, no <unistd.h>, no Sleep()/usleep() — SDL handles timing
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>      /* AttachConsole — GUI-Subsystem-Begleiter, siehe main() */
#endif
#include <SDL_timer.h>   /* SDL_GetTicks/SDL_Delay — Frame-Timing (ohne main-Umleitung) */
#include "re15_engine.h"
#include "re15_tim.h"
#include "re15_scd.h"
#include "re15_fade.h"      /* fade channels + the letterbox counter (FUN_80021a0c) */
#include "re15_md1.h"

re15_engine_state_t g_engine;

extern uint8_t *re15_asset_read_file(const char *path, int *out_size);
extern void re15_render_pc_upload_tim(const re15_tim_t *tim);

/* M-round (2026-05-25): round-half-away-from-zero float→int helper.
 * Plain `(int)f` truncates toward zero, which asymmetrically biases
 * negative screen coords up and positive down — a major contributor to
 * the user-reported "wiederkehrende Pixel Verschiebungen". The PSX GPU
 * receives already-integer GTE output, so the PC port must round the
 * float-projected coords the same way to match pixel placement. */
static inline int RNDI(float f) {
    return (int)(f >= 0.0f ? f + 0.5f : f - 0.5f);
}

#include "re15_bg.h"
#include "re15_montage_fx.h"   /* Pre-Intro-Praesentation nach RE2-Vorbild (OPENING.BIN) */
#include "re15_audio.h"
#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_camera.h"
#include "re15_math.h"      /* re15_gte_divide — byte-true GTE RTPS perspective divide */
#include "re15_light.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_to_re2.h"
#include "re15_rdt.h"
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_pri.h"
#include "re15_collision.h"
#include "re15_stair.h"
#include "re15_game_step.h"   /* SHARED per-frame interpreter step (PSX+PC) */
#include "re15_menu.h"        /* re15_menu_* — inventory/weapon-select overlay (8.20) */
#include "re15_item_icon.h"   /* re15_item_icon_* — byte-true ITEMALL grid icons (8.22) */
#include "re15_item_modal.h"  /* re15_item_modal_* — item-get zoom/flip pickup presentation (U11) */
#include "re15_itps.h"        /* re15_itps_set_data — the per-item modal picture sheet (ITPS.ITP, U11) */
#include "re15_item_use.h"    /* heal classifier gate + applier table (wave 3: prompt-less direct heal) */
#include "re15_damage.h"      /* re15_player_equipped_weapon (ARMS CONTROL panel, 8.23) */
#include "asset_root_pc.h"    /* gemeinsame Asset-Wurzel-Aufloesung (exe-relativ, 2026-08-24) */
#include "asset_selftest_pc.h" /* RE15_ASSET_SELFTEST=1 — Paket-Gate, reine Diagnose */

/* (Wave 1 inventory rebuild: the former FAITHFUL-LINE helpers re15_pc_panel/re15_pc_ecg/
 * re15_pc_draw_item_icon are gone — the status screen is now the byte-true display list of
 * re15_inv_screen.c (engine) rasterized by inv_render_pc.c; see re15_inv_screen.h.) */
#include "re15_inv_screen.h"  /* byte-true status-screen display list (wave 1) */
#include "re15_re2doc.h"      /* FILE-Bildebene: RE2-Dokumentseiten */
#include "re15_room.h"
#include "re15_debug_menu.h"        /* SHARED cross-room transition (re15_room_apply_pending) */
#include "re15_enemy.h"       /* generic enemy-model registry (re15_enemy_find/alloc/reset) */
#include "re2_ems.h"          /* WELLE A: RE2-Flavor-Asset-Loader (CDEMD0.EMS-TOC, PC-only) */
#include "re15_enemy_ai.h"    /* re15_player_victim_state/type — Leon's grab-victim render override */
#include "re15_ems.h"         /* enemy-model archive index (load EMDs out of CDEMD*.EMS) */
#include "re15_room_list.h"   /* GENERATED room-id list for the [ / ] debug room-browser */
#include "re15_room_spawns.h" /* GENERATED per-room entry spawn (inbound-door landing spot) */
#include "re15_anim_select.h"  /* SHARED actor bank/clip selection view-model */
#include "re15_esp.h"          /* Phase ESP-C: op-0x3a effect-sprite bank + particle pool */
#include "re15_gameflow.h"     /* FE-0.2 top-level mode machine (BOOT/TITLE/INGAME/GAMEOVER) */
#include "re15_str.h"          /* FE-3 STR/MDEC opening-movie demux+decode (CAPCOM.STR) */
#include "re15_xa.h"           /* FE-3 CD-XA ADPCM opening-movie audio decode */
#include "re15_savedata.h"     /* FE-4 game-state save block */
#include "re15_memcard.h"      /* FE-4 byte-true PSX .mcr backend */
#include "re15_savepoint.h"    /* FE-4 phone save-point pending signal */
#include "re15_itembox.h"      /* ITEM BOX (RE1.5-hybrid): box-AOT pending signal + storage */

#define RE15_TIM_SLOT_EFFECT 19   /* effect-sprite TIM render slot (0..18 used by chars/props) */
#define RE15_TIM_SLOT_EFFECT_ROOM(i) (36 + (i))  /* Raum-ESP-TIM je Effekt-Index 0..7 (byte-true
                                   * FUN_800194f8 laedt JEDE TIM der RDT-Tabelle; Slot 19 hielt
                                   * nur eff[0] — ROOM11E0 ids {5,7,0x11} brauchen drei) */
#define RE15_TIM_SLOT_EFFECT_GLOBAL 20 /* GLOBAL effect bank (CORE00.ESP) sprite sheet — effect-id 0
                                        * hit/blood. Its texture is NOT in any RDT; it lives in VRAM
                                        * (tpage 0x001f -> VRAM(960,256) 4-bit, clut 0x7951). Extracted
                                        * byte-true from the live ShowVRAM ground truth into a TIM
                                        * (tools/vram_png_to_tim.py) shipped at extracted_fx/effect0_blood.tim. */
#define RE15_TIM_SLOT_FX_MUZZLE 21 /* global effect-id 2 — muzzle flash/sparks (handgun discharge
                                    * 0x02000800 @0x800337bc); ShowVRAM-extracted sheet. */
#define RE15_TIM_SLOT_FX_SMOKE  22 /* global effect-id 3 — gun smoke (0x03000c00) */
#define RE15_TIM_SLOT_FX_SHELL  23 /* global effect-id 4 — shell eject/debris (0x04000800) */
#define RE15_TIM_SLOT_FX_FIRE   44 /* global effect-id 8 — FEUER/Explosion. Fuenftes und letztes
                                    * Sheet der CORE00-Bank; ROOM1090 zieht daraus die Flammen der
                                    * Truemmer-Varianten 0 und 3 (Sprungtabelle @0x80100364, s.
                                    * re15_esp_type26_flame). Quelle der Textur: DATA/TEX.TIM
                                    * (Spalten 192..255) — siehe k_gfx unten. */

/* GLOBAL-Bank-Sheet-Index (re15_esp_global_sheet_index) -> Render-Slot. Die Reihenfolge ist die
 * des Index; k_gfx (Ladetabelle, weiter unten) benutzt DIESELBE. */
static const int k_global_fx_slot[RE15_ESP_GLOBAL_SHEETS] = {
    RE15_TIM_SLOT_EFFECT_GLOBAL,   /* 0: effect-id 0x00 blood  */
    RE15_TIM_SLOT_FX_MUZZLE,       /* 1: effect-id 0x02 muzzle */
    RE15_TIM_SLOT_FX_SMOKE,        /* 2: effect-id 0x03 smoke  */
    RE15_TIM_SLOT_FX_SHELL,        /* 3: effect-id 0x04 shell  */
    RE15_TIM_SLOT_FX_FIRE,         /* 4: effect-id 0x08 fire   */
};
#define RE15_TIM_SLOT_WPN_MELEE 24 /* RESERVED/unused: the in-hand weapon now textures from the */
#define RE15_TIM_SLOT_WPN_GUN   25 /* character's own body-skin TIM (slot 0), page 0x81/clut-1 — */
                                   /* NOT a separate PLW dir[3]/PL04 atlas (byte-true FUN_80036b68). */
/* Render-TIM-Slot fuer Room-Prop obj_id `op`: 0..5 -> 4..9 (historisch), 6..15 -> 26..35.
 * Noetig, weil das Original Room-Props NIE bei 6 kappt: die Objekt-Schleife FUN_800436a8
 * laeuft bis RDT-Header nOmodel (`lbu v0,2(v0)` @0x80043758 / `sltu s0,nOmodel` @0x800437ac),
 * der Obj_model_set-Handler LAB_80040914 indexiert den Pool DIREKT per obj_id
 * (`a3*0x94 + DAT_800b3f98` @0x8004093c-58) und bindet das Modell pro obj_id aus der
 * RDT+0x30-Tabelle (@0x80040ab4-adc -> FUN_8002b898). ROOM11F0/11F1 (Generator-Raum)
 * traegt nOmodel=12 — mit dem alten 6er-Cap fehlten die Schalter obj 0x06..0x0B. */
/* ⛔ PROP 16 DARF NICHT AUF 36 FALLEN. Die alte Formel lieferte fuer op=16 den
 * Wert 26+10 = 36 — und 36..43 sind RE15_TIM_SLOT_EFFECT_ROOM(0..7), die Raum-ESP-
 * Texturen. Ein blosses Anheben von RE15_RDT_MAX_PROPS haette die Weste ueber die
 * Effekt-Textur gelegt statt sie zu zeigen. Prop 16 bekommt deshalb den bis dahin
 * unbenutzten Slot 45 (RE15_TIM_SLOT_MAX dafuer 45 -> 46). Belegung s. render_pc.c. */
#define RE15_TIM_SLOT_PROP(op) ((op) < 6  ? 4 + (op)        : \
                                (op) < 16 ? 26 + ((op) - 6) : 45)

extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);

/* Phase ESP-C: the current room's parsed effect-sprite bank (borrows the resident RDT buf;
 * bound via re15_esp_set_room_bank so op_sce_espr_on can resolve effect ids to anim records). */
static re15_esp_t s_room_esp;
static re15_esp_t s_global_esp;        /* the GLOBAL effect bank CORE00.ESP (effect-id 0 = hit fx) */
static uint8_t   *s_global_esp_buf = NULL;

/* Parse `room_id`'s ESP section from its RDT and bind it as the active effect bank. */
static void pc_load_room_esp(const uint8_t *rdt_buf, int rdt_size, unsigned room_id)
{
    /* Der CLEAR sitzt seit dem Teardown-Umbau in re15_room_reset_render_pc (room_pc.c), NICHT mehr
     * hier: dieser Parser lief nur, wenn ueberhaupt ein RDT-Puffer vorlag, sodass ein Raum ohne
     * Effekt-Sektion die Bank des Vorraums behielt (17 von 206 RDTs haben keine). Im Original sind
     * Loeschen und Neu-Parsen ebenfalls getrennt: FUN_80019354 nullt erst 96 Effekt-Slots
     * (@0x80019378) und setzt die ID-Maps auf -1 (@0x80019388-E4), danach erst wird geparst. */
    if (!rdt_buf || rdt_size < 0x5C) return;
    #define U32LE(o) ((uint32_t)rdt_buf[o] | ((uint32_t)rdt_buf[(o)+1]<<8) | \
                      ((uint32_t)rdt_buf[(o)+2]<<16) | ((uint32_t)rdt_buf[(o)+3]<<24))
    uint32_t idh = U32LE(0x4C), pe = U32LE(0x50), tb = U32LE(0x54), te = U32LE(0x58);
    #undef U32LE
    int rc = re15_esp_parse(rdt_buf, (size_t)rdt_size, idh, pe, tb, te, &s_room_esp);
    if (rc == 0) {
        re15_esp_set_room_bank(&s_room_esp);
        fprintf(stderr, "[esp] room %04X: %d effect bank(s) parsed (e.g. id 0x%02x: %u anim/%u cells)\n",
                room_id, s_room_esp.id_count,
                s_room_esp.id_count ? s_room_esp.eff[0].effect_id : 0,
                s_room_esp.id_count ? s_room_esp.eff[0].count_a : 0,
                s_room_esp.id_count ? s_room_esp.eff[0].count_b : 0);
        /* JEDE Effekt-TIM in ihren eigenen Render-Slot (36+i) — byte-true FUN_800194f8:
         * das Original laedt die TIM-Tabelle KOMPLETT (LoadImage je TIM, VRAM-Spalte ab
         * x=960 abwaerts) und patcht CLUT/TPAGE je Effekt in dessen EFF-Header. Der alte
         * Ein-Slot-Upload (nur eff[0] -> Slot 19) liess z.B. den ROOM11E0-Strom-Effekt
         * (id 0x11 = eff[2], eigene TIM @0x25A74) aus der TIM von Effekt 5 sampeln. */
        for (int ei = 0; ei < s_room_esp.id_count; ei++) {
            if (!s_room_esp.eff[ei].tim_off || (uint32_t)rdt_size <= s_room_esp.eff[ei].tim_off)
                continue;
            re15_tim_t tim;
            if (re15_tim_parse(rdt_buf + s_room_esp.eff[ei].tim_off,
                               (size_t)rdt_size - s_room_esp.eff[ei].tim_off, &tim) == 0) {
                re15_render_pc_upload_tim_slot(&tim, RE15_TIM_SLOT_EFFECT_ROOM(ei));
                fprintf(stderr, "[esp] effect TIM (id 0x%02x) -> slot %d: %dx%d\n",
                        s_room_esp.eff[ei].effect_id, RE15_TIM_SLOT_EFFECT_ROOM(ei),
                        tim.width, tim.height);
            }
        }
    } else {
        fprintf(stderr, "[esp] room %04X: no effect bank (rc=%d)\n", room_id, rc);
    }
}

/* Phase ESP-C draw: project each live effect particle (byte-true owner+offset world pos, same
 * camera transform as the player) and emit a billboard quad textured from the effect TIM.
 * The texture is byte-true: room fx sample the RDT effect TIM (slot 19); GLOBAL-bank fx (effect-id
 * 0 hit/blood from CORE00.ESP, whose sheet is VRAM-only) sample the byte-true extracted TIM (slot 20).
 * FAITHFUL-LINE (flagged): the exact anim-record -> coord-cell mapping + the cell's screen geometry
 * are the data-driven bit (C3_RENDER_DESIGN.md §2c, TBD via live capture / the PTR_LAB_80071d40 draw
 * dispatch); here the cell index = frame % count_b, a fixed 24px UV span at the cell's byte-true (u,v)
 * origin (the coord record's w/h = signed pivot offsets, deferred with the draw routine), and a
 * ~world-600 half-size billboard. Position, effect TIM, and cell origin are byte-true. */
FILE *pc_fx_log_handle(void)
{
    static FILE *s_fxlog = NULL; static int s_fxlog_init = 0;
    if (!s_fxlog_init) { s_fxlog_init = 1;
        const char *fl = getenv("RE15_FX_LOG");
        if (fl && *fl) s_fxlog = fopen(fl, "w"); }
    return s_fxlog;
}

/* `has_region`/`rxs`/`rzs` = das Region-Quad des AKTIVEN Cuts (DAT_800ac790). Effekte werden
 * damit genauso gecullt wie Spieler/NPCs/Props — siehe den Beleg am Test unten. */
static void pc_draw_effects(const re15_camera_view_t *cam, int cx, int cy,
                            int has_region, const int16_t rxs[4], const int16_t rzs[4])
{
    extern int  re15_render_pc_dbg_slot_loaded(int slot);
    extern void re15_render_pc_bind_tim_slot(int slot);
    const re15_esp_t *global_bank = re15_esp_global_bank();
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f) continue;
        if (!re15_esp_fx_visible(f)) continue;   /* byte-true flags bit1 gate (frozen/staggered = hidden) */
        /* ⛔ REGION-SICHTBARKEIT — Nutzer-Befund 2026-08-27: "alle Effekte wie Strom, Feuer etc.
         * ueberdecken nicht sichtbare Bereiche. Zum Beispiel wenn sie noch um die Ecke hinter der
         * Kamera sind." Der Port hatte diesen Test fuer Effekte NICHT (nur fuer Spieler/NPC/Prop).
         *
         * Das Original testet ihn in der ESP-Slot-Schleife FUN_80053240 unmittelbar nach den
         * beiden flags-Gates und VOR jeder Projektion (eigene Disasm, PSX.EXE):
         *     800532fc: andi v0,v1,0x1          ; aktiv?
         *     80053300: beq  v0,zero,0x80053474
         *     80053308: andi v0,v1,0x2          ; sichtbar?
         *     8005330c: beq  v0,zero,0x80053474
         *     80053310: addiu a0,sp,24          ; a0 = &{x,y,z}
         *     80053314: lh   v0,-68(s0)         ; s0 = slot+0x6C -> slot+0x28 = World-X
         *     8005331c: lw   a1,-14448(a1)      ; a1 = DAT_800ac790 = Region-Quad des akt. Cuts
         *     80053324: lh   v0,-66(s0)         ;                     slot+0x2A = World-Y
         *     80053330: lh   v0,-64(s0)         ;                     slot+0x2C = World-Z
         *     80053334: jal  0x80014368         ; Punkt-im-Viereck (X/Z)
         *     8005333c: beq  v0,zero,0x80053474 ; AUSSERHALB -> Slot komplett uebersprungen
         * DAT_800ac790 wird beim Cut-Wechsel gesetzt: @0x80021c00 `jal 0x80014324` /
         * @0x80021c0c `sw v0,-14448(at)`; FUN_80014324 liefert den ersten RVD-Satz (20 Byte,
         * Tabelle = RDT+0x28) mit rec[+2] == Cut-Id.
         * Getestet wird slot+0x28 — genau das Feld, aus dem FUN_800534c4 danach per RTPS
         * projiziert (@0x800534ec `addiu v0,a1,40`), im Port also x+xlat_x / z+xlat_z.
         * has_region == 0 -> nicht cullen (dieselbe Rueckfallregel wie bei Prop/NPC/Spieler). */
        if (re15_esp_fx_culled(f->x + f->xlat_x, f->z + f->xlat_z, has_region, rxs, rzs))
            continue;
        /* Each particle animates from ITS OWN resolved bank: the room ESP (RDT-TIM slot 19) or the
         * GLOBAL bank CORE00.ESP, whose sheets live only in VRAM -> the byte-true extracted TIMs.
         * Global ids have per-id sheets (0 blood / 2 muzzle / 3 smoke / 4 shell) in slots 20-23. */
        const re15_esp_t *bank = f->bank;
        int slot = RE15_TIM_SLOT_EFFECT;
        if (bank && bank == global_bank) {
            /* Sheet-Zuordnung einmalig in der Engine (re15_esp_global_sheet_index): id 0/2/3/4/8.
             * Die alte switch-Kaskade kannte 0x08 NICHT und schickte es per `default` auf das
             * BLUT-Sheet (Slot 20) — dort sind 9 der 10 Flammen-Zellen von id 0x08 (UV v=168/208,
             * S=40) vollstaendig transparent, weshalb die ROOM1090-Truemmer der Varianten 0 und 3
             * unsichtbar blieben (gemessen: 0/1600 opake Texel je Zelle gegen 324..787/1600 auf
             * dem Feuer-Sheet). */
            int si = re15_esp_global_sheet_index(f->effect_id);
            slot = (si >= 0) ? k_global_fx_slot[si] : RE15_TIM_SLOT_EFFECT_GLOBAL;
        } else if (bank && f->eff_idx >= 0) {
            /* ROOM-Bank: die TIM DIESES Effekts (Upload-Schleife oben; byte-true
             * FUN_800194f8-Analog — auf PSX zeigen die gepatchten TPAGE-Page-Bits auf
             * die per-TIM-VRAM-Spalte, auf PC ist die Seite = der per-Id-Slot). */
            slot = RE15_TIM_SLOT_EFFECT_ROOM(f->eff_idx);
        }
        if (!re15_render_pc_dbg_slot_loaded(slot)) continue;   /* that bank's texture not loaded */
        re15_render_pc_bind_tim_slot(slot);
        /* SPLATTER physics offset (byte-true xlat): a physics particle draws at anchor + xlat. */
        /* byte-true integer GTE RTPS (same path as the character mesh + shadows):
         * view = (rot·world)>>12 + trans, no float. rot is int32 Q12, trans int32. */
        int32_t wx = f->x + f->xlat_x;
        int32_t wy = f->y + f->xlat_y;
        int32_t wz = f->z + f->xlat_z;
        int32_t vx = (int32_t)(((int64_t)cam->rot[0]*wx + (int64_t)cam->rot[1]*wy + (int64_t)cam->rot[2]*wz) >> 12) + cam->trans[0];
        int32_t vy = (int32_t)(((int64_t)cam->rot[3]*wx + (int64_t)cam->rot[4]*wy + (int64_t)cam->rot[5]*wz) >> 12) + cam->trans[1];
        int32_t vz = (int32_t)(((int64_t)cam->rot[6]*wx + (int64_t)cam->rot[7]*wy + (int64_t)cam->rot[8]*wz) >> 12) + cam->trans[2];
        /* RE15_FX_LOG: per-particle draw decision trace (debug harness) */
        {
            FILE *fl = pc_fx_log_handle();
            if (fl) {
                extern int re15_render_pc_dbg_textri_count(void);
                fprintf(fl, "id=%d sub=%d eidx=%d frame=%d w(%d,%d,%d) phys=%d xlat=(%d,%d,%d) drift=(%d,%d,%d) slot=%d q=%d\n",
                        f->effect_id, f->sub_index, f->eff_idx, f->frame,
                        f->x, f->y, f->z, f->phys, f->xlat_x, f->xlat_y, f->xlat_z,
                        f->drift_x, f->drift_y, f->drift_z, slot,
                        re15_render_pc_dbg_textri_count());
                fflush(fl);
            }
        }
        /* Near-Gate byte-true: das Original verwirft das Sprite, wenn SZ3>>2 == 0, also SZ3 < 4
         * (FUN_800534c4, eigene Disasm: @0x80053570 `lw a0,4(sp)` = SZ3, @0x80053578
         * `sra v0,a0,2`, @0x8005357c `beq v0,zero,0x800537b4` = Epilog). SZ3 ist der von der GTE
         * auf [0,0xFFFF] gesaettigte View-Z -> hinter der Kamera = 0 = verworfen. Der Port hatte
         * hier `vz < 64` und liess damit Effekte mit View-Z 4..63 aus. Ein FAR-Gate hat das
         * Original nicht (SZ3 saettigt, OT-Index SZ3>>6 <= 1023 = OT-Groesse). */
        {
            uint32_t sz3_gate = (uint32_t)(vz < 0 ? 0 : (vz > 0xFFFF ? 0xFFFF : vz));
            if ((sz3_gate >> 2) == 0) continue;       /* auch der Schutz fuer gte_divide unten */
        }
        /* byte-true GTE RTPS centre: IR1/IR2 sat s16, SZ3 sat u16, UNR reciprocal, IR·n>>16
         * (truncation, not RNDI). Matches the mesh/shadow projection exactly. */
        int32_t _ir1 = vx > 0x7FFF ? 0x7FFF : (vx < -0x8000 ? -0x8000 : vx);
        int32_t _ir2 = vy > 0x7FFF ? 0x7FFF : (vy < -0x8000 ? -0x8000 : vy);
        uint32_t _sz3 = (uint32_t)(vz > 0xFFFF ? 0xFFFF : vz);
        uint32_t _n = re15_gte_divide((uint32_t)cam->fov_screen_dist, _sz3);
        int sx = cx + (int)(((int64_t)_ir1 * (int64_t)_n) >> 16);
        int sy = cy + (int)(((int64_t)_ir2 * (int64_t)_n) >> 16);
        /* ===== BYTE-TRUE SPRITE BUILD (FUN_800534c4, arbitrated wf_efa45868-e53) =============
         * anim record = {u8 coord_start, u8 n_sprites, u8 duration, u8 S}: byte0 = the FIRST
         * coord-cell index (NOT frame%count!), byte1 = quads this frame, byte3 = S = the SxS UV
         * cell size AND the scale numerator. Screen size (16.16):
         *   step16 = (S * scale16 * camf) / (sz<<4)      sz = view z; camf = RDT cam fov>>7
         *   w16    = defW(Q12, 0x1000 for the weapon fx) * step16;  h16 analog
         * corner  = projected center + s8(coord dx/dy) * (w16/S)    (dx=dy=-S/2 -> centered)
         * UV      = (u,v)..(u+S,v+S), minus 1 texel per axis when its step > 0x1ffff
         * blend   = ABE (def status bit4; muzzle 0x93 / smoke 0x13 -> ON) = PSX ABR0 50/50. */
        int S = 24, nspr = 1, cstart = f->frame;      /* legacy fallback when no record resolves */
        if (bank && f->eff_idx >= 0) {
            re15_esp_anim_t a;
            if (re15_esp_anim(bank, f->eff_idx, f->frame, &a) == 0) {
                cstart = a.desc & 0xff;
                nspr   = (a.desc >> 8) & 0xff;
                S      = (a.param >> 8) & 0xff;
                if (nspr < 1) nspr = 1;
                if (S < 1) S = 24;
            }
        }
        int camf = 208;                               /* ROOM1140 fallback (26684>>7) */
        {
            extern int pc_fx_camf(void);              /* RDT cam record fov>>7 (below) */
            int cf = pc_fx_camf();
            if (cf > 0) camf = cf;
        }
        int sz = (int)vz; if (sz < 1) sz = 1;
        int64_t step16 = ((int64_t)S * (int64_t)f->scale16 * (int64_t)camf) / ((int64_t)sz << 4);
        /* defW/defH: der Original-Draw FUN_800534c4 liest slot+0x04/+0x06 — im Port bisher
         * fest 0x1000. Der FEUER-Oszillator (Routine 18 = FUN_80017c8c, 2026-08-29) schreibt
         * die Felder jeden Frame; NUR fuer solche Slots (aktuelle Row-Routine 17/18) werden
         * sie live gelesen. KONSERVATIV: die uebrigen Row-Fx (Blut/Muendung/Huelse) behalten
         * 0x1000, bis deren Row-Bytes +0x04/+0x06 einzeln nachgeprueft sind (OPEN — eine
         * falsche 0 dort machte die Sprites unsichtbar). */
        int64_t defw = 0x1000, defh = 0x1000;
        if (f->rows_base) {
            uint16_t selA = (uint16_t)(f->row[0x00] | (f->row[0x01] << 8));
            if (selA == 17 || selA == 18) {
                defw = (int64_t)(uint16_t)(f->row[0x04] | (f->row[0x05] << 8));
                defh = (int64_t)(uint16_t)(f->row[0x06] | (f->row[0x07] << 8));
                if (defw <= 0) defw = 0x1000;
                if (defh <= 0) defh = defw;
            }
        }
        int64_t w16 = defw * step16;                  /* w16 = defW * step16 (RAW mult, byte-true —
                                                       * the 16.16 result absorbs the Q12:
                                                       * px = w16>>16 @0x800535f0) */
        int64_t h16 = (defh == defw) ? w16 : defh * step16;
        int64_t stepX = (S > 0) ? w16 / S : 0;
        int64_t stepY = (S > 0) ? h16 / S : 0;
        int ute = (stepX > 0x1ffff) ? 1 : 0;          /* >=2x magnification edge trim */
        int vte = (stepY > 0x1ffff) ? 1 : 0;
        /* ===== BLEND, byte-true (ROOM11E0 Strom-Effekt 2026-08-08) =====
         * ABE = flags Bit4 (Draw FUN_800534c4: prim code 0x2C |= `(flags>>3)&2` -> 0x2E),
         * ABR = TPAGE-Bits 5-6 (POLY_FT4 word5 = tpage<<16; Seed = EFF-hdr u16 @+6,
         * Routine 8 @0x80017608-20 / Routine 10 @0x800176d8-ec ORen row[0x16] hinein).
         * ROOM11E0 id 0x11 row0: flags=0x13, row[0x16]=0x20 -> ABE an, ABR1 = B+F ADDITIV.
         * Muzzle (CORE00 id2, rows 0x93/0x20) ist damit ebenfalls ABR1 (die alte 50/50-
         * Annahme war der Platzhalter, solange TPAGE nicht modelliert war); Blut (id0,
         * Routine 3 flags:=0x13, TPAGE bleibt hdr-Seed 0x001F) = ABR0 50/50.
         * Legacy-Fx (ohne Row-VM, flags==0) behalten den alten Pfad (Global 2/3 = 50/50). */
        int abe, abr;
        if (f->rows_base) {
            abe = (f->flags & 0x10) != 0;
            abr = (f->tpage >> 5) & 3;
        } else {
            abe = (bank == global_bank && (f->effect_id == 2 || f->effect_id == 3));
            abr = 0;
        }
        /* OT-Skala: Effekt-Sprite und sprite.pri-Maske haengen im Original in DERSELBEN OT
         * (Basis 0x800AA6D8 + cut<<12 — ESP: @0x80053270 `addiu a3,a3,-13772` = 0x800aca34,
         * @0x8005329c `addiu a3,a3,-9052` = 0x800AA6D8, @0x800532d0 `sll a2,a2,12`; Maske:
         * @0x800395e0 `addiu s5,s5,-13772` / @0x800395e4 `addiu s6,s5,-9052` = dieselbe Basis).
         * Die Buckets: Effekt = SZ3>>6 (@0x80053620 `sra v0,a0,6`), Maske = depth
         * (@0x80039658 `sll a0,a0,2` = Wort-Index). Der PC sortiert nach View-Z und bildet
         * Masken auf depth*64 ab (re15_pri.h:107); damit die RELATIVE Ordnung stimmt, muss
         * key(Effekt)/64 == SZ3>>6 gelten, also key = vz. Mesh-Dreiecke benutzen bereits
         * avgz = vz (main.c PROJECT_VERT `(out_wz) = (float)_vz`).
         * Vorher stand hier `vz >> 4` = Faktor 16 zu klein: jeder Effekt landete damit vor
         * praktisch jeder Maske und jedem Charakter-Mesh — die sprite.pri-Verdeckung ("hinter
         * der Wand") war fuer Effekte faktisch abgeschaltet. */
        int z = (int)vz;
        {
            extern FILE *pc_fx_log_handle(void);
            FILE *fl = pc_fx_log_handle();
            if (fl) fprintf(fl, "   -> sx=%d sy=%d S=%d n=%d c0=%d w16=%lld camf=%d\n",
                            sx, sy, S, nspr, cstart, (long long)w16, camf);
        }
        extern void re15_render_pc_set_tri_alpha(int a);
        extern void re15_render_pc_set_tri_blend(int m);
        if (abe) {
            switch (abr) {
            case 1:  re15_render_pc_set_tri_blend(1); re15_render_pc_set_tri_alpha(255); break;
                     /* ABR1 = 1.0*B + 1.0*F (SDL ADD, voller Beitrag) */
            case 2:  re15_render_pc_set_tri_blend(2); re15_render_pc_set_tri_alpha(255); break;
                     /* ABR2 = B - F (REV_SUBTRACT) */
            case 3:  re15_render_pc_set_tri_blend(1); re15_render_pc_set_tri_alpha(64);  break;
                     /* ABR3 = B + F/4 (SDL ADD: dst + src*srcA/255, 64/255 ~ 1/4) */
            default: re15_render_pc_set_tri_alpha(128); break;   /* ABR0 = 0.5*B + 0.5*F */
            }
        }
        for (int q = 0; q < nspr; q++) {
            re15_esp_coord_t c;
            if (!bank || f->eff_idx < 0 ||
                re15_esp_coord(bank, f->eff_idx, cstart + q, &c) != 0) break;
            int x0 = sx + (int)(((int64_t)(int8_t)c.w * stepX) >> 16);
            int y0 = sy + (int)(((int64_t)(int8_t)c.h * stepY) >> 16);
            int x1 = sx + (int)((((int64_t)(int8_t)c.w * stepX) + w16) >> 16);
            int y1 = sy + (int)((((int64_t)(int8_t)c.h * stepY) + h16) >> 16);
            if (x1 <= x0 || y1 <= y0) continue;
            int u0 = c.u, v0 = c.v, u1 = c.u + S - ute, v1 = c.v + S - vte;
            re15_render_textured_tri(x0, y0, u0, v0,  x1, y0, u1, v0,
                                     x0, y1, u0, v1,  0, 0, z, 128, 128, 128);
            re15_render_textured_tri(x1, y0, u1, v0,  x1, y1, u1, v1,
                                     x0, y1, u0, v1,  0, 0, z, 128, 128, 128);
        }
        if (abe) { re15_render_pc_set_tri_blend(0); re15_render_pc_set_tri_alpha(255); }
    }
}

/* The active camera's fx scale field: u16 @ RDT + cut*0x20 + 0x62, >>7 (byte-true
 * FUN_800534c4 camf; ROOM1140 = 26684>>7 = 208). The main loop publishes it per frame. */
static int s_fx_camf = 0;
int  pc_fx_camf(void) { return s_fx_camf; }
static void pc_fx_set_camf(const uint8_t *rdt_raw, size_t rdt_size, int cut)
{
    size_t off = (size_t)cut * 0x20 + 0x62;
    if (rdt_raw && off + 2 <= rdt_size) {
        uint16_t v = (uint16_t)(rdt_raw[off] | (rdt_raw[off + 1] << 8));
        s_fx_camf = v >> 7;
    }
}

/* AZ-round 2026-05-28: sprite.pri overdraw — push parsed mask list to
 * the renderer's overdraw layer (declared in render_pc.c). */
extern void re15_render_pc_set_pri_rects(const int *src_x, const int *src_y,
                                         const int *dst_x, const int *dst_y,
                                         const int *w, const int *h,
                                         const int *depth, int count);
extern void re15_render_pc_set_pri_player(int sx, int sy, int z);

/* PC data-driven per-room prop set (parity with PSX re15_load_room_props): loads
 * room_id's Obj_model_set prop MD1s + TIMs into md1[0..5]/ok[0..5] (TIMs → render
 * slots 4..9). Resets all slots first. room1140 = obj00 only; room1170 = obj00-05.
 * Called inline at boot AND in the cross-room consume (the s_room_prop_* arrays are
 * local to main, so PC keeps rc.load_props=NULL and reloads inline rather than via
 * the PSX-style fn-ptr callback). */
extern uint8_t *re15_asset_read_file(const char *path, int *size);
extern void     re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);

/* Asset-Pfad-Konsolidierung (2026-07-02): read a global asset from the EINEN Asset-Wurzel
 * shared_assets/PSX/<rel>. Dieser Baum enthält jetzt ALLES — den rohen CD-Baum (RDT unter
 * STAGE{N}/, DATA, gepackte Container) PLUS die vor-extrahierten Assets (PLD-Split, RBJ/,
 * per-cut BSS/). Die alte psx_dev/assets_shared- + re15_reborn-Abhängigkeit ist entfernt.
 * Auflösungsreihenfolge (seit 2026-08-24 in asset_root_pc.c, siehe dort): env-Overrides →
 * exe-Verzeichnis + Vorfahren → cwd + Vorfahren → Compile-Default; je Wurzel erst der
 * CD-Baum, dann der Geschwisterbaum (extracted_fx/, RE2/), dann die Paketwurzel (synchro/).
 * Returns a malloc'd buffer (caller frees/keeps); *size set; NULL if not found. */
static uint8_t *pc_read_shared(const char *rel, int *size)
{
    /* 2026-08-24 (Nutzer-Report 0.3.19): die vier hier fest verdrahteten Wurzel-Bloecke
     * (env RE15_ASSET_ROOT -> Compile-Default -> env/Default RE15_CD_ROOT samt "/../"-
     * Geschwisterpfad -> cwd-Leiter) sind ersetzt durch die gemeinsame Wurzelliste in
     * asset_root_pc.c. Die deckt dieselben Faelle ab — inklusive des Geschwisterbaums
     * (extracted_fx/ liegt NEBEN PSX/, nicht darin: genau daran ist "extracted_fx/*.tim"
     * im Paket gescheitert) — und kennt zusaetzlich das VERZEICHNIS DER LAUFENDEN EXE.
     * Reihenfolge: env-Overrides zuerst, exe-Verzeichnis vor cwd, Compile-Default zuletzt. */
    return re15_pc_read_any(rel, size);
}

/* Scratch for re15_apply_room_cinematic (the shared overlay parses into this before copying
 * to the real destination). File-scope static so it's NOT a ~7 KB stack local; PC RAM is
 * unconstrained. (The PSX caller instead reuses its CD staging buffer — see asset_psx.c.) */
static re15_emd_skeleton_t  s_cine_scratch_skel;
static re15_emd_animation_t s_cine_scratch_anim;

/* Generic enemy loader (globalization 2026-06-13): load EM<type>.EMD from the shared
 * tree into a registry bank + upload its texture to render slot 11+bank. Lazy: called
 * from the NPC render loop the first time an actor of `type` appears. Replaces the
 * em21-only special case for every OTHER enemy type. */
/* Cached CDEMD0.EMS — the enemy-model archive every generic enemy loads from
 * (the disc has no per-type EM<NN>.EMD; ~4.7 MB, read once, kept resident). */
static const uint8_t *pc_cdemd(size_t *out_sz)
{
    static uint8_t *s_ems = NULL; static int s_sz = 0; static int s_tried = 0;
    if (!s_tried) { s_tried = 1; s_ems = pc_read_shared("EMD/CDEMD0.EMS", &s_sz); }
    if (out_sz) *out_sz = (size_t)s_sz;
    return s_ems;
}

/* Cached RE2 CDEMD0.EMS (WELLE A) — das RE2-Gegner-Archiv fuer den OPTIONS→AI→RE2-Flavor.
 * Liegt per Nutzer-Entscheidung unter shared_assets/RE2/ (NICHT im PSX-Baum — der RE1.5-
 * Single-Asset-Root bleibt unangetastet). ~10.6 MB, einmal gelesen, resident (die RE2-Bank
 * aliast hinein wie die RE1.5-Bank in ihren malloc-Blob). env RE15_RE2_ASSET_ROOT
 * uebersteuert; sonst <shared>/RE2/ ueber die gemeinsame Wurzelliste (asset_root_pc.c). */
static const uint8_t *pc_re2_cdemd(size_t *out_sz)
{
    static uint8_t *s_ems = NULL; static int s_sz = 0; static int s_tried = 0;
    if (!s_tried) {
        /* 2026-08-24: env RE15_RE2_ASSET_ROOT behaelt Vorrang, danach <shared>/RE2/ ueber die
         * gemeinsame Wurzelliste (asset_root_pc.c) statt der eigenen cwd-Leiter. */
        s_tried = 1;
        s_ems = re15_pc_read_re2("CDEMD0.EMS", &s_sz);
    }
    if (out_sz) *out_sz = (size_t)s_sz;
    return s_ems;
}

/* WELLE A: RE2-Bank fuer `type` aus CDEMD0.EMS laden (TOC @0x8009adf4, Splitter re2_ems.c).
 * Generisch ueber den kind gebaut (Hund/Kraehe folgen in Welle C/D); der RE1.5-Typ IST der
 * RE2-kind fuer die Zombie-Familie (RE2 klemmt kinds 0x10..0x1F auf ein Overlay,
 * FUN_8001b710; EMD/TIM-Records sind per-kind). Rueckgabe 1 = Bank gefuellt. */
static int pc_enemy_load_re2(uint8_t type, re15_enemy_bank_t *eb)
{
    extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
    size_t ems_sz = 0;
    const uint8_t *ems = pc_re2_cdemd(&ems_sz);
    if (!ems) {
        static int warned = 0;
        if (!warned) {
            warned = 1;
            fprintf(stderr, "[enemy] RE2-Flavor: shared_assets/RE2/CDEMD0.EMS fehlt -> "
                            "Fallback auf RE1.5-Modelle\n");
        }
        return 0;
    }
    re15_tim_t tim = {0};
    if (re2_ems_load_bank(ems, ems_sz, type, eb, &tim) != 0) {
        fprintf(stderr, "[enemy] RE2 EM0%02X: kein TOC-Eintrag/Parse-Fehler -> RE1.5-Fallback\n", type);
        return 0;
    }
    eb->buf = NULL;                                    /* Bank aliast das residente EMS */
    int slot = 11 + (int)(eb - g_enemy);
    if (tim.width > 0 && tim.height > 0 && slot < 24) {
        re15_render_pc_upload_tim_slot(&tim, slot);    /* TIM-Slot-Upload wie der RE1.5-Pfad */
        eb->pc_tex_slot = slot;
    }
    eb->ok = 1;
    fprintf(stderr, "[enemy] RE2 EM0%02X loaded: %d meshes, %d bones, %d clips -> slot %d\n",
            type, eb->md1.mesh_count, eb->skel.bone_count, eb->anim.clip_count, eb->pc_tex_slot);
    return 1;
}

/* Den RE1.5-EMD-Blob fuer `type` besorgen (Split-Datei oder Blob aus CDEMD0.EMS) — der
 * Aufrufer bekommt einen PRIVATEN malloc-Puffer und uebernimmt ihn als eb->buf. Aus
 * pc_enemy_load herausgezogen, weil der WELLE-G-Hybrid denselben Blob braucht. */
static uint8_t *pc_enemy_read_re15_emd(uint8_t type, size_t *out_len)
{
    uint8_t *buf = NULL; char rel[32]; int sz = 0;
    snprintf(rel, sizeof rel, "EMD/EM%02X.EMD", type);
    buf = pc_read_shared(rel, &sz);                    /* malloc'd if it exists */
    if (buf) { if (out_len) *out_len = (size_t)sz; return buf; }
    int idx = re15_ems_index_for_type(type);
    size_t ems_sz = 0; const uint8_t *ems = (idx >= 0) ? pc_cdemd(&ems_sz) : NULL;
    size_t off = 0, len = 0;
    if (ems && re15_ems_get_entry(ems, ems_sz, idx, &off, &len) == 0) {
        buf = (uint8_t *)malloc(len);                  /* private copy: banks free their buf */
        if (buf) { memcpy(buf, ems + off, len); if (out_len) *out_len = len; return buf; }
    }
    return NULL;
}

/* WELLE G — HYBRID: die frisch geladene RE2-Bank behaelt Skelett-HIERARCHIE + ALLE Animationen,
 * bekommt aber die RE1.5-GEOMETRIE (MD1 + TIM) und die RE1.5-Bind-Laengen (permutiert, weil die
 * beiden Rigs die Bones anders ordnen — Messwerte + Tabellen in re2_ems.h/re2_ems.c).
 * Schlaegt irgendein Schritt fehl, bleibt die reine RE2-Bank stehen (sichtbarer Fallback statt
 * eines halb umgebauten Skeletts) und der Grund steht auf stderr. */
static void pc_enemy_hybrid_re15_models(uint8_t type, re15_enemy_bank_t *eb)
{
    extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
    size_t buflen = 0;
    uint8_t *buf = pc_enemy_read_re15_emd(type, &buflen);
    if (!buf) {
        fprintf(stderr, "[enemy] Hybrid EM%02X: RE1.5-Modell nicht gefunden -> RE2-Modell bleibt\n",
                type);
        return;
    }
    /* re15_md1_t/-animation_t sind gross (Frames[1664]) — niemals Stack-Locals. Die Anim wird
     * nur zum Parsen gebraucht und danach verworfen: die Clips bleiben RE2. */
    static re15_md1_t           s_md15;
    static re15_emd_skeleton_t  s_sk15;
    static re15_emd_animation_t s_an15;
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (re15_emd_parse_container(buf, buflen, &s_md15, &s_sk15, &s_an15, &tim) != 0) {
        free(buf);
        fprintf(stderr, "[enemy] Hybrid EM%02X: RE1.5-EMD-Parse fehlgeschlagen -> RE2-Modell bleibt\n",
                type);
        return;
    }
    int unmapped = -1;
    int rc = re2_hybrid_apply(eb, (int)type, &s_md15, &s_sk15, &unmapped);
    if (rc != 0) {
        free(buf);
        fprintf(stderr, "[enemy] Hybrid EM%02X: re2_hybrid_apply rc=%d -> RE2-Modell bleibt\n",
                type, rc);
        return;
    }
    eb->buf = buf;                                     /* md1 zeigt JETZT in diesen Puffer; die
                                                        * RE2-Skelette/EDDs aliasen weiter das
                                                        * residente EMS (wird nie freigegeben) */
    /* SITZ-IMPORT (10D0 sel 0x0e): die RE1.5-Aktions-Bank des Typs 0x10 dauerhaft behalten und
     * an die Engine melden (re15_re2z_set_re15_pose_bank) — s_sk15/s_an15 sind SHARED-Scratch
     * (der naechste Hybrid-Parse ueberschreibt sie), deshalb eine dedizierte Kopie. Der
     * RE2-Flavor posiert/taktet damit die RE1.5-Schlaf-Clips 0x2A/0x29 (RE1.5-INIT
     * @0x80100F64-FD4; FK-Beleg: 0x2A f0 = Wand-SITZ, RE2-Bank hat keinen Sitz-Clip). */
    if (type == 0x10) {
        static re15_emd_skeleton_t  s_sk15_sit;
        static re15_emd_animation_t s_an15_sit;
        s_sk15_sit = s_sk15;
        s_an15_sit = s_an15;
        re15_re2z_set_re15_pose_bank(&s_sk15_sit, &s_an15_sit);
    }
    int slot = (eb->pc_tex_slot >= 0) ? eb->pc_tex_slot : 11 + (int)(eb - g_enemy);
    if (tim.width > 0 && tim.height > 0 && slot < 24) {
        re15_render_pc_upload_tim_slot(&tim, slot);    /* RE1.5-TIM ersetzt die RE2-Textur */
        eb->pc_tex_slot = slot;
    }
    fprintf(stderr, "[enemy] Hybrid EM%02X: RE1.5-Geometrie (%d Meshes) unter RE2-Rig "
                    "(%d Bones, %d Clips), %d Kanten ohne Zuordnung -> slot %d\n",
            type, eb->md1.mesh_count, eb->skel.bone_count, eb->anim.clip_count,
            unmapped, eb->pc_tex_slot);
}

/* `allow_re2` = 0 zwingt den RE1.5-Pfad, auch wenn re15_re2_owns_type(type) gilt.
 * ⛔ NOETIG SEIT 2026-08-22 fuer TYP 0x26. Die Zahl bedeutet in den beiden Spielen NICHT
 * dasselbe: in RE1.5 ist 0x26 der FEUER-EMITTER von ROOM1090 (Dispatch 0x80072bac[0x26] =
 * 0x80116288; Registrierung `addiu v0,v0,25224` @0x8011E8F4 -> `sw v0,11332(at)` @0x8011E8FC),
 * in RE2 die Baby-Spinne, die die Adult zur Laufzeit ausstoesst (`jal 0x8001ad3c` /
 * `addiu a0,zero,38` @0x80105DE4-E8). Fuer die sieben RDT-gesetzten 0x26er von ROOM1090 muss
 * deshalb die RE1.5-Bank EM26 geladen werden (1 Mesh / 1 Bone / 1 Clip), nicht die RE2-Bank
 * EM026 (1 Mesh / 1 Bone / 3 Clips) — sonst faehrt der Emitter mit RE2-Clip-Indizes.
 * Die Registry fuehrt EINE Bank je Typ; beide Populationen koennen nie im selben Raum stehen
 * (Zensus: Typ 0x26 steht in genau 7 Records, alle in ROOM1090 @0x2214..0x228C, und dort steht
 * kein einziger Adult 0x25), die Weiche ist also eindeutig. */
/* ⛔ NUTZER-MANDAT (2026-08-23): "Bei RE2AI haben die Zombies nicht den Sound von RE1.5 AI.
 * Sound usw soll uebernommen werden, nur KI nicht." — Im RE2-Flavor wird fuer die ZOMBIE-
 * Familie dieser MAPPER als SE-Hook registriert: die byte-true RE2-Trigger (Ids/Timing,
 * enemy_ai_re2_zombie.c) bleiben unveraendert, nur die Sample-Quelle wechselt von der
 * ENEMSE-Bank auf die RE1.5-snd1-Raumbank (FUN_800453d0-Pfad). Die ZUORDNUNG ist eine
 * dokumentierte Mapping-Entscheidung (RE2-Kontext -> semantisch gleicher RE1.5-SE), die
 * RE1.5-Seite jeder Zeile ist adressbelegt:
 *   ENEMSE 0/1 (Schritt L/R, Pair-1 Clip-0-Frame-Flags f20/f62) -> RE1.5 SE 1
 *       (RE1.5-Schritt: LOCO-Frame-Bits SE 1, 2x/Zyklus via FUN_8001b38c @0x8001b3b4)
 *   ENEMSE 3 (Biss)        -> SE 3 (Frame-Bit 3 Clips 0x27/0x28 via @0x8001b3cc;
 *                                   Devour-Biss @0x801045ec-f4)
 *   ENEMSE 4/10 (Moan A)   -> SE 4 (snd1-Moan-Paar 4/5)
 *   ENEMSE 5/11 (Moan B)   -> SE 5 (Feeder-/Aufsteh-Moan @0x801039dc-e0 / @0x80104ae0)
 *   ENEMSE 9 (Abriss)      -> SE 9 (RE1.5-Kriech-Grab-Abriss, enemy_ai_common.c)
 *   ENEMSE 12 (Fall/Hit)   -> SE 6 (Hurt @0x80106954)
 *   ENEMSE 13 (Fall/Death) -> SE 8 (Death-Paar 5/8, FUN_80107cb0)
 * Unbekannte Ids fallen auf die ENEMSE-Bank zurueck (nichts verstummt still). Der RE1.5-
 * Flavor-Abriss-Import (re2z_se(9), Registrierung unten) behaelt bewusst das RE2-Sample. */
static void pc_re2z_se_re15(int se_id, int flag2000)
{
    int m;
    switch (se_id) {
    case 0: case 1: m = 1; break;
    /* NEU (FINDING 4, 2026-08-24): der KOPF-ABRISS am abgeworfenen Kriecher. Die RE2-Seite
     * ruft `addiu a0,zero,2` @0x80102B2C + `jal 0x8005BD6C` @0x80102B58 (Zweig A, Frame 22)
     * bzw. `addiu a0,zero,8` @0x80102B44 (Zweig B, Frame 27). Beide Ids waren hier NICHT
     * gemappt und fielen auf das RE2-Sample zurueck — im RE1.5-Sound-Modus also der falsche
     * Klang fuer genau das Geraeusch, das der Nutzer vermisst.
     * RE1.5-Zwillinge, byte-belegt in derselben Choreografie (STAGE1.BIN, Kriech-Grab P5):
     *   Zweig A (Leon, DAT_800ACA5C & 4 == 0): `ori a0,zero,0x9` @0x80104088 +
     *      `jal 0x800453d0` @0x800400B4  -> RAUM-SE 9 = das Abriss-Geraeusch
     *   Zweig B (Elza): SE 9 @0x801040DC/EC, danach rng&1 ? SE 5 @0x8010410C : SE 8
     *      @0x8010411C -> die 8 ist der Zwilling der RE2-Id 8 */
    case 2:  m = 9; break;
    case 3:  m = 3; break;
    case 8:  m = 8; break;
    case 4:  m = 4; break;
    case 5:  m = 5; break;
    case 9:  m = 9; break;
    case 10: m = 4; break;
    case 11: m = 5; break;
    case 12: m = 6; break;
    case 13: m = 8; break;
    default: m = -1; break;
    }
    if (m >= 0) { re15_audio_room_se(m); return; }
    re15_audio_re2_enemy_se(se_id, flag2000);
}

static void pc_enemy_load_ex(uint8_t type, int allow_re2)
{
    extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
    if (type == 0 || re15_enemy_find(type)) return;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb) return;                                   /* registry full */

    /* RE2-Flavor-Zweig (WELLE A) — VOR dem RE1.5-Zweig, nur wenn ein RE2-Brain den Typ
     * besitzt (re15_re2_owns_type: Zombie-Familie + Hund 0x20). Fehlt das RE2-Archiv oder
     * der Record, faellt der Lauf UNVERAENDERT in den byte-true RE1.5-Pfad darunter. */
    /* ⚠️ MIXED (2026-08-23): DAS IST DIE EINE STELLE, DIE DIE BANK WAEHLT. Beide Ladewege —
     * die Roster-Schleife (main.c:~4845) UND der Render-Loop (main.c:~6459) — landen hier in
     * pc_enemy_load_ex, dieses Tor entscheidet also fuer BEIDE. Typ-bezogen heisst: im
     * MIXED-Modus holt GENAU 0x20 die RE2-Bank (CDEMD0.EMS/EMD0G_MOD0), jeder andere Typ die
     * RE1.5-Bank. (Der dritte Ladeaufruf, der 0x26-Vorzug bei einer RE2-Adult-Spinne, ist
     * separat gegatet und im MIXED-Modus aus — s. dort.) */
    if (allow_re2 && re15_ai_re2_for_type(type) && re15_re2_owns_type(type)) {
        /* WELLE B/C: ENEMSE-Wiedergabe im Engine-Brain registrieren (PC-only Symbole — der
         * Funktionszeiger haelt engine/ link-sauber fuers PSX-Target) + Bank waehlen.
         * Zombie: Bank 0 (Beleg enemy_ai_re2_zombie.c RE2Z_ENEMSE_BANK). HUND: Bank 31 +
         * flag2000 (Paar-Tabelle @0x800A7400: kind 0x20 ist in allen 13 Zeilen der ZWEITE
         * kind; Zeile 31 = {0x00,0x20} = reine Hunde-Bank; EDT-Map-Probe: alle Hunde-Baenke
         * tragen dieselbe zweite Haelfte — enemy_ai_re2_dog.c RE2DOG_ENEMSE_BANK). Der
         * ENEMSE-Slot ist wie im Original EINE Raum-Bank; kein Auslieferungs-RE1.5-Raum
         * mischt Hunde mit Zombies (Memory reai-v2-dog-ai), Konflikt daher nur theoretisch
         * (letzter Ladevorgang gewinnt). */
        if (type == 0x20)
            re15_re2dog_audio_hook(re15_audio_re2_enemy_se, re15_audio_re2_enemy_bank);
        else if (type == 0x21)
            /* WELLE D — KRAEHE: Paar-Tabelle @0x800A7400 (file 0x97C00, selbst gelesen
             * 2026-08-16): kind 0x21 steht in GENAU EINER Zeile, 21 = {0x21, 0x00} = reine
             * Kraehen-Bank, ERSTE Haelfte (flag2000=0). EDT-Probe Bank 21: ids 0..14 live,
             * 15+ leer — SEs 1..6 alle belegt (enemy_ai_re2_crow.c RE2CROW_ENEMSE_BANK). */
            re15_re2crow_audio_hook(re15_audio_re2_enemy_se, re15_audio_re2_enemy_bank);
        else if (type == 0x25 || type == 0x26)
            /* WELLE E — SPINNE: Paar-Tabelle @0x800A7400: kind 0x25 steht in den Zeilen 53
             * ({0x25,0x1F}) und 54 ({0x25,0x20}), beide Male als ERSTER kind (flag2000=0);
             * die ersten Map-Haelften beider Zeilen sind byte-identisch (memcmp ueber die
             * dekodierten EDT-Maps). kind 0x26 steht in Zeile 24 = {0x26,0x00}.
             * Belege + Refutation: enemy_ai_re2_spider.c RE2SPIDER_ENEMSE_BANK. */
            re15_re2spider_audio_hook(re15_audio_re2_enemy_se, re15_audio_re2_enemy_bank,
                                      type == 0x26);
        else
            /* ZOMBIE-Familie im RE2-Flavor: RE1.5-Sound-Mapper statt ENEMSE-Player
             * (Nutzer-Mandat 2026-08-23, s. pc_re2z_se_re15 oben). Die Bank-Registrierung
             * bleibt — der Fallback unbekannter Ids spielt weiter ENEMSE. */
            re15_re2z_audio_hook(pc_re2z_se_re15, re15_audio_re2_enemy_bank);
        if (pc_enemy_load_re2(type, eb)) {
            /* "AI RE2" behaelt Gehirn + Animation aus RE2, tauscht aber Mesh/Textur/Bind-Laengen
             * gegen RE1.5. ⛔ 2026-08-21: das war bis hierher an re15_ai_models() gebunden (dritte
             * Menue-Stufe "RE2 MODELS"). Die Stufe ist auf Nutzer-Entscheidung entfernt, also
             * laeuft der Hybrid BEDINGUNGSLOS — der reine RE2-Modell-Zweig ist damit tot und weg.
             * pc_enemy_load_re2 bleibt: es liefert Skelett, EDD/Clips und die Bank, die der
             * Hybrid umbaut (und ist der sichtbare Fallback, wenn der Umbau scheitert). */
            pc_enemy_hybrid_re15_models(type, eb);
            return;
        }
    }

    /* ---- NACHZUG zu 83b7740c: DAS ABRISS-GERAEUSCH IM RE1.5-MODUS ---------------------------
     * Die Port-Option "RE2-Zombie-Uebernahme im RE1.5-Modus" (re15_re15_re2z_import, Default AN)
     * fahrt im RE1.5-Modus den RE2-Zerleger @0x80105288-3D8. Dessen erste Amtshandlung nach dem
     * Dreifach-Gate ist ein SE:
     *     801052b4  addiu a0,zero,9        ; SE-id 9
     *     801052b8  jal   0x8005bd6c       ; FUN_8005bd6c = der RE2-Gegner-SE-Trigger
     * Der Port ruft das an derselben Stelle (enemy_ai_re2_zombie.c re2z_leg_gore -> re2z_se(9)),
     * aber `s_re2z_se_fn` war im RE1.5-Modus NULL, weil der Hook bisher nur oben im RE2-Asset-
     * Zweig registriert wurde -> der Abriss war STUMM (im Commit als Luecke (d) benannt).
     * Registriert wird deshalb GENAU fuer die Typen, die die Option ohnehin fuehrt:
     * re15_re15_re2z_import_owns() == RE1.5-Flavor + Option AN + 0x10/0x11/0x12/0x13/0x16/0x18.
     *
     * BANK 0 (== RE2Z_ENEMSE_BANK, derselbe Wert wie im RE2-Modus; re15_re2z_audio_hook waehlt
     * sie selbst). Belegt an den echten Bytes (tests/unit/test_re15_re2z_gore_se.c):
     *   - Bank-Wahl FUN_80052b38 liest die SOUND-ID aus dem Sce_em_set-Record (+7 -> entity+0x1FA,
     *     `lbu v0,7(v0)` @0x80057274 / `sb v0,506(s0)` @0x80057280; verglichen wird nur +0x1FA,
     *     `lb v1,506(a0)` @0x80052C48) gegen die Paar-Tabelle @0x800A7400; Zeilenindex == Bank.
     *   - Zensus ueber alle 250 RDTs in info/re2leon/PL0/RDT (nur die SCD-Sektionen 17/18):
     *     die Zombie-Familie hat 79 registrierte Spawn-Records, haeufigste Sound-Id 0x03 (18x)
     *     -> Zeile 0 = {0x03,0x00} -> Bank 0, erste Haelfte (flag2000 = 0); Bank 0 ist damit
     *     auch die haeufigste Zombie-Bank. Das Original waehlt sie PRO RAUM, der Port fuehrt
     *     eine feste — fuer die id 9 ist das nachweislich folgenlos, s. naechster Punkt.
     *   - Daten-Gegenprobe ENEMSE.VBS + TOC @0x800A7B1C: Bank 0 fuehrt die id 9 LIVE,
     *     EDT-Map-Eintrag 9 = 0x02A30000 -> prog 0, tone 10, Kanal 2, Prio-Nibble 3. In ALLEN
     *     Baenken, die der Zensus fuer die Zombie-Familie liefert (0/1/2/5/53), ist dieser
     *     Eintrag UND das dahinter liegende VAG-Sample (2208 B) BYTE-IDENTISCH — die Bankwahl
     *     kann den Abriss-Laut also gar nicht aendern. Gegenprobe: die Hunde-Bank 6 fuehrt die
     *     id 9 als 0xFFFFFFFF (stumm), die Wahl ist also nicht beliebig.
     *
     * KEINE NEBENWIRKUNG AUF DEN RE1.5-KLANG (jede Haelfte geprueft):
     *   - `re15_audio_re2_enemy_bank` ist nur ein LATCH; ENEMSE.VBS wird lazy erst im ersten
     *     `re15_audio_re2_enemy_se` geladen (audio_pc.c) — also fruehestens beim Abriss. Die
     *     RE1.5-Baenke (snd0 Schritte, snd1 Raum-/Combat-SE, ARMS, CORE) werden nicht angefasst.
     *   - Wiedergabe laeuft auf dem EIGENEN RE2-Slot-Block MIXER_RE2SE_CH_FIRST..+7; die 8 festen
     *     RE1.5-SE-Stimmen (Slots 0..7) und der freie Pool (8..11) bleiben bit-identisch.
     *   - Der ZWEITE re2z_se-Pfad, die Frame-Flag-SFX `re15_re2z_se_play` (0x801016c8), ist im
     *     RE1.5-Modus unerreichbar: sein einziger Aufrufer (enemy_ai_common.c) steht hinter
     *     `re15_ai_flavor() == RE15_AI_FLAVOR_RE2`. Es kommt also NUR der Gore-SE dazu. */
    if (re15_re15_re2z_import_owns(type))
        re15_re2z_audio_hook(re15_audio_re2_enemy_se, re15_audio_re2_enemy_bank);

    /* Enemy models live inside CDEMD0.EMS (no per-type EM<NN>.EMD on the disc).
     * Try a standalone split file first (back-compat / future), else extract the
     * type's EMD blob out of the archive (re15_ems, the byte-true port of the
     * Java EMS splitter). Either way `buf` ends up a private, bank-owned copy. */
    size_t buflen = 0;
    uint8_t *buf = pc_enemy_read_re15_emd(type, &buflen);
    if (!buf) {
        eb->type = 0;
        fprintf(stderr, "[enemy] EM%02X model not found (no split file, not in CDEMD0.EMS)\n", type);
        return;
    }

    re15_tim_t tim = {0};
    if (re15_emd_parse_container(buf, buflen, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) {
        free(buf); eb->type = 0;
        fprintf(stderr, "[enemy] EM%02X EMD parse FAILED\n", type);
        return;
    }
    eb->buf = buf;                                     /* md1/skel/anim point into buf */
    /* Load the LOCOMOTION bank (bank 0) too — the STAGE1 zombie APPROACH state (+0x5=0x13) plays it
     * (the 6-clip loco set with the 0x2000 foot-lock frame flags), which parse_container skips in
     * favour of the 43-clip action bank. Aliases into buf (no extra alloc). */
    eb->loco_ok = (re15_emd_parse_loco_bank(buf, buflen, &eb->skel_loco, &eb->anim_loco) == 0);
    /* Load the GRAB-VICTIM bank (bank 2) — the animation the grab plays on LEON (struggle clips 0-5 /
     * collapse 6/7); PL00-compatible. Used by the player-victim FSM when a zombie grabs the player. */
    eb->victim_ok = (re15_emd_parse_victim_bank(buf, buflen, &eb->skel_victim, &eb->anim_victim) == 0);
    /* Load the ENTITY-OWN channel bank (bank 1 = dir[3]/[4]) — the +0x170/+0x174 pair the NPC
     * state-4 executor subs {2,4,5,6,9} and the Plc_dest walk play (FUN_80022300 @0x800224b8/c8;
     * marvin_spawn_anim.md F1). own_ok=0 for dir[3]-empty types (dog/crow/gorilla). */
    eb->own_ok = (re15_emd_parse_own_bank(buf, buflen, &eb->skel_own, &eb->anim_own) == 0);
    int slot = 11 + (int)(eb - g_enemy);
    if (tim.width > 0 && tim.height > 0 && slot < 24) {
        re15_render_pc_upload_tim_slot(&tim, slot);
        eb->pc_tex_slot = slot;
    }
    eb->ok = 1;
    fprintf(stderr, "[enemy] EM%02X loaded: %d meshes, %d bones, %d clips -> slot %d\n",
            type, eb->md1.mesh_count, eb->skel.bone_count, eb->anim.clip_count, eb->pc_tex_slot);
}

/* Default: der RE2-Zweig darf greifen (unveraendertes Verhalten fuer alle Typen ausser 0x26,
 * dessen Aufrufer die Herkunft mitgeben — s. Block ueber pc_enemy_load_ex). */
static void pc_enemy_load(uint8_t type) { pc_enemy_load_ex(type, 1); }

/* ─────────────────────────────────────────────────────────────────────────────────────
 * R.P.D.-RUESTUNG / SPIELERMODELL-WECHSEL — byte-true.
 *
 * NUTZER-BEFUND 2026-08-26: "Nach anziehen der Weste im ROOM 1190 aendert sich Leons
 * outfit nicht und wahrscheinlich hat er auch die Vorteile der Weste nicht."
 *
 * DIE KETTE, GANZ GEMESSEN (ROOM1190.RDT, Datei-Offsets; PSX.EXE-Adressen):
 *  1. Prop 16 = die Weste auf einem Staender. Obj_model_set @0x002D4C (sub14),
 *     pos (20480,-1500,-21168) — sie fehlte bis 2026-08-26 wegen der Prop-Kappung
 *     (s. RE15_RDT_MAX_PROPS in re15_rdt.h; der Nutzer sah ein gelbes Viereck).
 *  2. Aot_set @0x002D90 (sub14): slot 16, sce 3, Rechteck (19700,-22000)+(1000,1500) —
 *     enthaelt die Weste. Nutzlast pc[14..19] = ff 00 18 0f 00 00, also (u16@+2)>>8 = 15.
 *     sce-3-Handler @0x800430F0 ruft FUN_8003ee3c(cond, sub) = "fuehre sub15 aus".
 *  3. sub15 @0x002DA6 ist ein UMSCHALTER auf Flag(3,0x75):
 *        21 03 75 00   Flag(3,0x75) pruefen
 *        2b 04 80 ff   Meldung 0x04  "There is one R.P.D. armor that should fit you.
 *                                     Will you equip it?"
 *        2b 05 80 ff   Meldung 0x05  "Will you unequip the armor?"
 *        22 03 75 01 / 22 03 75 00   Flag setzen / loeschen
 *        24 10 01 00 / 24 10 00 00   work_vars[0x10] = 1 / 0
 *        47 0f                       Aot_on(15)
 *     (Die Meldungstexte sind mit dem Dekoder DES PORTS aus der Message-Tabelle des
 *      Raums gelesen, nicht uebersetzt — probe_1190_texte.)
 *  4. AOT-Slot 15 = Door_aot_set @0x002D70, Rechteck (0,0,0,0) = reiner Skript-Ausloeser;
 *     next_pos (19100,0,-21250), Zielraum 0x19 = ROOM1190 SELBST, Cut 13. Das Anlegen
 *     ist also ein Raum-Neuladen an derselben Stelle.
 *  5. Und HIER greift, was dieser Block nachbaut — der Raumlader FUN_800396fc:
 *        80039760  lbu  a0,0x0(s0)=>DAT_800aca5c   ; Modell/Waffen-Byte
 *        80039768  lh   v1,DAT_800b0ff0            ; work_vars[0x10]
 *        8003976c  andi v0,a0,0xf                  ; untere Nibble = MODELL-Index
 *        80039770  beq  v0,v1,LAB_80039790         ; unveraendert -> nichts tun
 *        80039774  _andi v0,a0,0xf0                ; sonst obere Nibble (Waffe) behalten
 *        8003977c  lbu  v1,DAT_800b0ff0
 *        80039784  or   v0,v0,v1
 *        80039788  jal  FUN_800314b0               ; Spielermodell NEU LADEN
 *        8003978c  _sb  v0,0x0(s0)
 *     FUN_800314b0 @0x800314d4-1c: `lbu v0,DAT_800aca5c; sll v0,v0,1;
 *     lhu a0,DAT_80073f70[v0]; jal FUN_80013b60` — Datei-Id-Tabelle @0x80073f70 ist
 *     16 u16: 0x3C,0x3D,…,0x4B. Also schlicht Datei-Id = 0x3C + Index = PL0<Index>.PLD.
 *
 * WAS GETAUSCHT WIRD — GEMESSEN, NICHT ANGENOMMEN. PL00.PLD gegen PL01.PLD, Komponenten
 * ueber die Verzeichnistabelle (u32 @0 = Tabellenanfang, dann 4 u32: EDD, EMR, MD1, TIM):
 *     EDD (Anim)    beide 3160 B  — sha 26198800428ae2ec == 26198800428ae2ec  GLEICH
 *     EMR (Skelett) beide 57136 B — sha 38ae9ec1f0a7f134 == 38ae9ec1f0a7f134  GLEICH
 *     MD1 (Mesh)    28916 B vs 30708 B                                        VERSCHIEDEN
 *     TIM (Textur)  beide 99872 B — sha 3bd14dc6… vs 99a7994a…                VERSCHIEDEN
 * Animation und Skelett sind byte-identisch, es wechseln also NUR Mesh und Textur. Der
 * Schnitt ist gegengeprueft: die vier aus PL00.PLD geschnittenen Bloecke sind byte-gleich
 * mit den separat ausgelieferten PL00.EDD/.EMR/.MD1/.TIM.
 * PL01s Textur zeigt eine ROTE Weste mit "POLICE"-Schriftzug und RPD-Wappen ueber
 * derselben Uniform (selbst dekodiert und angesehen) = die R.P.D.-Ruestung.
 *
 * KEINE SCHADENSMINDERUNG IM ORIGINAL: Flag(3,0x75) hat game-weit 14 Fundstellen —
 * gesetzt/geloescht nur in ROOM1190/1191 sub15, gelesen in deren sub14 und in ROOM3060
 * sub03, ROOM3091 sub07, ROOM40A0/40A1 sub06. Die EXE liest das Flag-Wort 0x800b1004
 * NIRGENDS (keine Code-Referenz im Ghidra-Dump). Der Bit-0x4-Test im Schadenspfad
 * (@0x80012088 `andi v1,v1,0x4`) trifft die Modell-Nibbles 4..7 = die ZWEITE Figur, nicht
 * die Ruestung (Modell 1). Der Vorteil der Weste ist also Skript-seitig, nicht in der
 * Schadensrechnung — das ist hier bewusst NICHT erfunden. */
static re15_md1_t *s_player_md1_ref = NULL;   /* auf main()s `md1` gesetzt, s. Registrierung */
static int  s_player_model_idx = 0;      /* untere Nibble von DAT_800aca5c */
static uint8_t *s_player_pld_buf = NULL; /* bleibt am Leben: md1/tim aliasen ihn */

/* Rueckruf-Form fuer die Engine (re15_scd_set_player_model_sync): der Raumlader ruft
 * ohne Argumente, das Ziel-Mesh steht in s_player_md1_ref. */
static int pc_sync_player_model(re15_md1_t *pl_md1);
static void pc_player_model_sync_cb(void)
{ if (s_player_md1_ref) (void)pc_sync_player_model(s_player_md1_ref); }

/* Liefert 1, wenn Mesh/Textur getauscht wurden. */
static int pc_sync_player_model(re15_md1_t *pl_md1)
{
    int want = (int)g_scd.work_vars[0x10];
    if (want < 0 || want > 15) return 0;          /* Tabelle @0x80073f70 hat 16 Eintraege */
    if (want == s_player_model_idx) return 0;     /* @0x80039770 `beq v0,v1` */

    char path[64];
    snprintf(path, sizeof path, "PLD/PL%02X.PLD", (unsigned)want);
    int sz = 0;
    uint8_t *buf = pc_read_shared(path, &sz);
    if (!buf || sz < 16) {
        fprintf(stderr, "[pld] %s nicht ladbar - Modellwechsel uebersprungen\n", path);
        return 0;
    }
    /* Schnitt ueber die EINE Regel der Engine (re15_pld_part) — nicht hier nachgebaut,
     * damit der Pin test_1190_ruestung P4f genau diese Regel gegen die separat
     * ausgelieferten PL00.EDD/.EMR/.MD1/.TIM gegenpruefen kann. */
    unsigned long md1_off = 0, md1_len = 0, tim_off = 0, tim_len = 0;
    if (!re15_pld_part(buf, (long)sz, RE15_PLD_MD1, &md1_off, &md1_len) ||
        !re15_pld_part(buf, (long)sz, RE15_PLD_TIM, &tim_off, &tim_len)) {
        fprintf(stderr, "[pld] %s: Verzeichnis unbrauchbar - Modellwechsel uebersprungen\n",
                path);
        free(buf);
        return 0;
    }

    re15_md1_t nm = {0};
    re15_tim_t nt;
    int okm = re15_md1_parse(buf + md1_off, (int)md1_len, &nm) == 0;
    int okt = re15_tim_parse(buf + tim_off, (int)tim_len, &nt) == 0;
    if (!okm || !okt) {
        fprintf(stderr, "[pld] %s: MD1 %s / TIM %s - Modellwechsel uebersprungen\n",
                path, okm ? "ok" : "FEHLER", okt ? "ok" : "FEHLER");
        free(buf);
        return 0;
    }
    /* Erst jetzt den alten Puffer freigeben — das neue Mesh aliast den NEUEN. */
    free(s_player_pld_buf);
    s_player_pld_buf   = buf;
    *pl_md1            = nm;
    s_player_model_idx = want;
    re15_render_pc_upload_tim_slot(&nt, 0);       /* Slot 0 = Spielerhaut */
    fprintf(stderr, "[pld] Spielermodell -> %s (Mesh %d Teile, Textur %dx%d)%s\n",
            path, nm.mesh_count, nt.width, nt.height,
            want == 1 ? "  [R.P.D.-Ruestung an]" : "");
    return 1;
}

static void pc_load_room_prop_set(const re15_rdt_t *rdt,
                                  re15_md1_t md1[RE15_RDT_MAX_PROPS],
                                  int ok[RE15_RDT_MAX_PROPS])
{
    /* Globalization Phase 3-A (2026-06-13): the room's Obj_model_set props (MD1+TIM)
     * are SLICED from the parsed RDT (rdt->prop_md1/tim[], the 0x30 model-pointer
     * table) — was per-room room####_obj##.{md1,tim} files. Pointers alias the resident
     * RDT buffer (byte-true == the old files). prop[op] → md1[op] + render slot
     * RE15_TIM_SLOT_PROP(op). Parity with PSX re15_load_room_props. Caller passes the
     * right RDT: &rdt at boot, &g_room_rdt on the cross-room reload.
     *
     * KEIN 6er-Cap mehr (Nutzer-Report ROOM11F0: 6 der 10 Generator-Schalter fehlten):
     * das Original laedt ALLE nOmodel Modelle — Schleifen-Schranke `lbu v0,2(v0)` =
     * RDT+0x02 nOmodel @0x80043758 / `sltu s0,nOmodel` @0x800437ac (FUN_800436a8);
     * Modell-Bind pro obj_id @0x80040ab4-adc (LAB_80040914 -> FUN_8002b898).
     * ROOM11F0/11F1 nOmodel=12. Game-weites Maximum: nOmodel=17 (ROOM1190/1191).
     * ERLEDIGT 2026-08-26 (stand hier als "OFFEN, separates Thema"): RE15_RDT_MAX_PROPS
     * ist auf 17 = das gemessene Maximum angehoben, Prop-Pool und Textur-Slots ziehen
     * mit. Es war die POLIZEIWESTE in ROOM1190, die dadurch wegfiel — der Nutzer sah
     * an ihrer Stelle ein gelbes Viereck. Herleitung + Zensus stehen bei
     * RE15_RDT_MAX_PROPS in re15_rdt.h. */
    for (int op = 0; op < RE15_RDT_MAX_PROPS; op++) ok[op] = 0;
    if (!rdt) return;
    int nprops = rdt->prop_count < RE15_RDT_MAX_PROPS ? rdt->prop_count
                                                      : RE15_RDT_MAX_PROPS;
    for (int op = 0; op < nprops; op++) {
        const uint8_t *mb = rdt->prop_md1[op]; int msz = rdt->prop_md1_size[op];
        const uint8_t *tb = rdt->prop_tim[op]; int tsz = rdt->prop_tim_size[op];
        if (mb && re15_md1_parse(mb, (size_t)msz, &md1[op]) == 0) ok[op] = 1;
        /* Der GPU-FARBSCHLUESSEL steckt seit 2026-08-21 im Upload selbst (EINE Regel fuer
         * ALLE Slots, re15_tim.h / render_pc.c) — hier ist kein Sonderfall mehr noetig.
         * Die Herleitung fuer den Raum-Prop-Fall bleibt stehen, weil sie den Original-
         * Zeichenpfad belegt (Nutzer-Report 2026-08-21: "bei dem Raetsel, wo man die
         * Schalter druecken muss, bewegt man einen Cursor — der ist im Original der
         * Hintergrund transparent und bei uns schwarz").
         * Raum-Objekte zeichnet das Original ueber die Objekt-Schleife FUN_8002c18c:
         * pro Objekt `bVar2 = (obj[+0x0C] & 2) != 0` und dann — bei (obj[+0x0C] & 0x10) == 0 —
         * FUN_800254a0 (Tri) / FUN_800256b0 (Quad), die den Primitiv-Code als
         * `param_3->cd = param_4 << 1 | 0x34` bzw. `| 0x3c` setzen: POLY_GT3/POLY_GT4,
         * Bit 0x02 = ABE.
         * obj[+0x0C] kommt aus dem Obj_model_set-Handler LAB_80040914: `lh v0,8(a2)`
         * @0x800409a8 + `sw v0,12(a1)` @0x800409b0, also pc[8..9] des Opcodes.
         * ROOM11F0s Cursor ist Objekt 0x00 (Record @Datei 0x00E54:
         * 2d 00 04 00 00 00 00 01 00 00 ...) mit pc[8..9] = 0x0000 -> obj+0x0C = 0 ->
         * ABE-Bit 0, Code bleibt 0x34/0x3C = OPAK. Semi-Transparenz kann es also NICHT
         * sein; es bleibt der GPU-Farbschluessel "Texel-Wert 0000h wird nicht gezeichnet"
         * (psx-spx-Zitat in re15_tim.h). Der haengt auf der Hardware am TEXEL-WERT, nicht
         * am Zeichenbefehl — es gibt keinen Per-Objekt-Schalter, der ihn abschalten
         * koennte —, deshalb gilt er fuer alle Raum-Props, nicht nur fuer obj 0x00.
         * Gemessen auf den Assets (scratchpad/uvcov11f0.py + propkey_census.py):
         * ROOM11F0 prop[0] = 101215 von 109042 gesampelten Texeln (92.8%) sind Index 0
         * mit CLUT[0]=0x0000; die 11 uebrigen Props des Raums haben 0.0% und aendern sich
         * dadurch byte-genau NICHT. Game-weit betrifft die Regel 122 von 570 Prop-Modellen;
         * dieselbe 92.8%-Textur taucht in 18 Raeumen auf = EIN wiederverwendetes
         * Cursor-/Marker-Modell. Gepinnt in tests/unit/test_prop_texel_key_11f0.c. */
        if (tb) { re15_tim_t tt; if (re15_tim_parse(tb, tsz, &tt) == 0)
                      re15_render_pc_upload_tim_slot(&tt, RE15_TIM_SLOT_PROP(op)); }
    }
}

/* ── FE-4 memory-card save/load ────────────────────────────────────────────── */
#define RE15_CARD_PATH "re15_card.mcr"      /* the PSX card image, next to the CWD */

/* Deferred resume: CONTINUE loads a save block here + arms the flag; the game loop
 * applies it AFTER the room + player have been set up, so the saved state wins over
 * the new-game briefing defaults. */
static re15_savedata_t s_resume_sd;
static int             s_resume_pending = 0;
/* Save-Zaehler = RE1.5 DAT_800b0fbd im LIVE-Game-State-Block (byte-true, analysis/save_counter.md):
 * frisches Spiel = 0 (EXE-Image @Datei 0xa17bd; New-Game-Zweig setzt 0), SAVE speichert den Wert
 * PRE-Inkrement (Titel /NN/ @0x80026eac-f0) und inkrementiert NUR nach Erfolg (@0x80026488-9c),
 * LOAD/CONTINUE restauriert ihn wholesale aus dem Save (@0x80026290-a0). Die Karte/der Ziel-Slot
 * wird fuer die Nummer NIE gelesen (der fruehere max-across-slots-Seed war erfunden). */
static uint16_t        s_save_counter = 0;

/* The memory-card slot screen — a blocking modal (like the title). save_mode:
 * 1 = save *sd to the chosen slot; 0 = load the chosen slot into s_resume_sd and
 * return its resume room in *out_room (NOT restored yet — the game loop defers it).
 * Returns the chosen slot (0..4), or -1 on cancel/failure. */
/* Write `sd` to `slot` with a byte-true RE1.5 directory title ("BIO HAZARD <char> /NN/",
 * DAT_800107cc/f8 templates: Elza/Leon + the 2-digit save counter). Returns 1 on success. */
static int pc_do_save(int slot, const re15_savedata_t *sd)
{
    char title[RE15_MC_TITLE_LEN];
    /* On-card BIOS-Titel = byte-true SJIS-Kette (FUN_80026e54): Template 0x2a B @0x80026e98
     * + Zaehler-Ziffern +0x25/+0x27 @0x80026ec8/@0x80026f00 + SJIS-Ortsname 0x13 B @0x80026f28.
     * (Vorher ASCII-snprintf — sichtbar nur fuer externe PSX-Kartenmanager; die In-Game-
     * Slot-Liste rekonstruiert ihre Zeile aus dem Blob.) End-to-End-Referenz: Mod-Karte
     * re15_save_final_1.mcd traegt exakt diese Kette (bug_save_room_name_recheck.md §3.6). */
    re15_mc_compose_title(title, sd->character, sd->save_count % 100, sd->loc_idx);
    return re15_memcard_save(RE15_CARD_PATH, slot, sd, title) == 0;
}

/* Build the byte-true RE1.5 slot title as raw sysmes atlas codes. Template DEBUG.BIN @0x617f
 * (verified verbatim): 05 07 "Leon" 05 00 38 0c 0c 38 ...  /  05 06 "Elza" 05 00 38 0c 0c 38 ...
 *
 * ORTSNAME (Historie + Entscheidung): Der Resolver FUN_80026e4c ist in der ausgelieferten
 * PSX.EXE ein return-0-Stub (@0x80026e4c jr ra / @0x80026e50 addu v0,zero,zero, Datei 0x1764c)
 * — das Original zeigt in JEDEM Raum sysmes 0x1a = "Irons' Office". Die Anzeige-Maschinerie
 * existiert aber komplett: Slot-Zeile = sysmes(0x1a + resolver) (@0x80026818 ori a0,0x1a +
 * @0x80026820 addu a0,a0,v0, memcpy 0x10 @0x8002682c-30, Getter FUN_800c00e4: ptr = tab +
 * u16[tab+idx*2], tab = DEBUG.BIN Datei 0x5f96; die 8 Namen liegen @0x6197-0x6210). Der Port
 * uebernimmt per NUTZER-ENTSCHEIDUNG 2026-08-08 den Vorprojekt-Patch (reAi patch_save_final.py
 * Block [Z], SAVE_LOC_FUNC @0x80070890): Index aus dem Save-Punkt — 0 Schreibmaschine
 * ("Irons' Office"), 1 Telefon ROOM1070/1071 ("Medical Room" sysmes 0x1b). Details + Registry:
 * re15_savepoint.h/.c; Transport im Blob: re15_savedata.h loc_idx (Patch-Analog Karte +0x203).
 * DOKUMENTIERTE RESTDIVERGENZ (unveraendert, analysis/bug_save_room_name.md F2/F4):
 * die Zeile ist ~28 px zu breit, weil der Port das `05 00` des Templates als Farb-Op + Space liest
 * statt als Farb-Op MIT Argument, und weil re15_render_pc_game_codes fuer Code 0x00 hart 8 px
 * vorrueckt statt der 4 px aus der Breitentabelle @0x800c4416.
 *
 * "Irons' Office": Apostroph = Atlas 0x3A. The 0x05 op is
 * the colour selector (`attr = next_byte & 7`, cf. msg_common.c:537): Leon = 07 (attr 7, blue
 * 32,80,232), Elza = 06 (attr 6, red 152,0,72), 05 00 resets the counter/room to the default
 * palette. re15_render_pc_game_codes interprets the embedded 0x05 ops. */

/* sysmes-Getter (byte-true FUN_800c00e4, DEBUG.BIN laedt RAW @0x800c0000):
 * ptr = tab + u16[tab + idx*2], tab = Datei-Offset 0x5f96. Die Tabelle hat exakt 34
 * Eintraege 0x00..0x21 (erster Offset 0x44 -> 0x44/2 = 34; recheck-Dossier §3.3).
 * NULL, wenn DEBUG.BIN fehlt oder idx ausserhalb. */
static const uint8_t *pc_sysmes(int idx)
{
    static uint8_t *s_dbg = NULL; static int s_dbg_tried = 0, s_dbg_size = 0;
    if (!s_dbg_tried) { s_dbg_tried = 1; s_dbg = pc_read_shared("BIN/DEBUG.BIN", &s_dbg_size); }
    if (!s_dbg || idx < 0 || idx > 0x21) return NULL;
    const unsigned tab = 0x5f96;
    unsigned off = tab + (unsigned)(s_dbg[tab + idx * 2] | (s_dbg[tab + idx * 2 + 1] << 8));
    if ((int)off + 0x10 > s_dbg_size) return NULL;
    return s_dbg + off;
}

static int pc_slot_title_codes(uint8_t *tc, int character, int count, int loc)
{
    static const uint8_t leon[] = { 0x28, 0x41, 0x4b, 0x4a };                       /* "Leon" */
    static const uint8_t elza[] = { 0x21, 0x48, 0x56, 0x3d };                       /* "Elza" */
    int n = 0;
    const uint8_t *nm = character ? elza : leon;
    tc[n++] = 0x05; tc[n++] = character ? 0x06 : 0x07; /* name colour: Elza=06 (red), Leon=07 (blue) */
    for (int k = 0; k < 4; k++)  tc[n++] = nm[k];
    tc[n++] = 0x05; tc[n++] = 0x00;                    /* reset to default palette */
    tc[n++] = 0x00;                                    /* space */
    tc[n++] = 0x38;                                    /* counter delimiter glyph */
    tc[n++] = (uint8_t)(0x0c + (count / 10) % 10);     /* tens digit ('0' = atlas 0x0c) */
    tc[n++] = (uint8_t)(0x0c + count % 10);            /* ones digit */
    tc[n++] = 0x38;                                    /* counter delimiter glyph */
    tc[n++] = 0x00; tc[n++] = 0x00;                    /* gap */
    /* Ortsname = sysmes(0x1a + loc), datengetrieben aus DEBUG.BIN (Getter oben; memcpy-Laenge
     * 0x10 @0x80026830 ori a2,zero,0x10 — der Drucker stoppt am Terminator 0x01, also werden
     * maximal 16 Bytes bis exklusive 0x01 uebernommen). Fallback ohne DEBUG.BIN: das alte
     * Literal (byte-gleich zu sysmes 0x1a, probe_save_room_name M4). */
    const uint8_t *loc_nm = pc_sysmes(0x1a + loc);
    if (loc_nm) {
        for (int k = 0; k < 0x10 && loc_nm[k] != 0x01; k++) tc[n++] = loc_nm[k];
    } else {
        static const uint8_t room[] = { 0x25,0x4e,0x4b,0x4a,0x4f, 0x3a, 0x00, 0x2b,0x42,0x42,0x45,0x3f,0x41 }; /* "Irons' Office" */
        for (int k = 0; k < 13; k++) tc[n++] = room[k];
    }
    return n;
}

/* Render the STATIC card screen (BG + header + 5-slot list + exit row) — shared by the byte-true
 * fade-IN on open (FUN_800264e8) and fade-OUT on close (FUN_80026594). The interactive main loop
 * draws its own copy plus the cursor/overwrite/result sub-screens. */
static void pc_draw_card_static(const re15_tim_t *bg, const int used[],
                                const int slot_char[], const int slot_cnt[],
                                const int slot_loc[], int save_mode)
{
    extern void re15_render_pc_show_cardbg(const re15_tim_t *tim);
    extern int  re15_render_pc_game_text(int x, int y, const char *str, int attr);
    extern int  re15_render_pc_game_text_width(const char *str);
    extern int  re15_render_pc_game_codes(int x, int y, const uint8_t *codes, int n, int attr);
    if (bg->pixels) re15_render_pc_show_cardbg(bg);
    else            re15_render_background_gradient(0, 0, 0, 0, 0, 0);
    re15_render_pc_game_text(160 - re15_render_pc_game_text_width("Memory Card 1") / 2, 24, "Memory Card 1", 0);
    for (int i = 0; i < RE15_SAVE_SLOTS; i++) {
        if (used[i]) { uint8_t tc[48]; int tn = pc_slot_title_codes(tc, slot_char[i], slot_cnt[i], slot_loc[i]);
                       re15_render_pc_game_codes(41, 56 + i * 20, tc, tn, 0); }
        else re15_render_pc_game_text(41, 56 + i * 20, "NO DATA", 0);
    }
    re15_render_pc_game_text(41, 156, save_mode ? "Do not save" : "Do not load", 0);
}

/* The byte-true RE1.5 memory-card SAVE/LOAD screen (FUN_80025c00 / FUN_80026658), adapted for
 * the PC single-card backend: the "MEMORY CARD BG" (DATA/TYPE00.TIM) full-screen, the dim panel
 * (drawn in render_pc), a header, the 5-slot list + an EXIT row, the SAVE/LOAD label, a blinking
 * cursor, and the OVERWRITE-confirm + result sub-screens. Coords are the RE'd 320x240 positions.
 * save_mode: 1 = SAVE (*sd), 0 = LOAD (into s_resume_sd, resume room -> *out_room). Returns the
 * chosen slot (0..4) or -1 on cancel/exit. */
static int pc_run_memcard_screen(int save_mode, const re15_savedata_t *sd, uint16_t *out_room)
{
    extern int  re15_render_pc_game_text(int x, int y, const char *str, int attr);   /* RE1.5 game font */
    extern int  re15_render_pc_game_text_width(const char *str);
    extern int  re15_render_pc_game_codes(int x, int y, const uint8_t *codes, int n, int attr);
    extern void re15_render_pc_show_cardbg(const re15_tim_t *tim);
    extern void re15_render_pc_hide_cardbg(void);
    extern void re15_render_pc_card_cursor(int x, int y, int show);
    extern void re15_render_pc_hide_title(void);
    extern void re15_render_pc_hide_title_menu(void);
    extern void re15_render_pc_screenshot(const char *path);
    re15_render_pc_hide_title();        /* CONTINUE opens this from the title loop — the card BG must win */
    re15_render_pc_hide_title_menu();   /* also hide the TMOJI menu sprites (NEW GAME/LOAD GAME/OPTION) */

    /* "MEMORY CARD BG" = DATA/TYPE00.TIM (CD file 0x23): a pre-rendered PC/CRT terminal desk
     * (RE1.5's save point is a computer). Full-screen 320x240; loaded once. */
    static re15_tim_t s_card_bg = {0};
    static int s_card_bg_tried = 0;
    if (!s_card_bg_tried) { s_card_bg_tried = 1; int bsz = 0;
        uint8_t *bb = pc_read_shared("DATA/TYPE00.TIM", &bsz);
        if (bb) re15_tim_parse(bb, bsz, &s_card_bg); }

    int  used[RE15_SAVE_SLOTS];
    char titles[RE15_SAVE_SLOTS][RE15_MC_TITLE_LEN];
    re15_memcard_list(RE15_CARD_PATH, used, titles);
    /* In-game slot label = byte-true RE1.5 sysmes idx 0x17 template (FUN_80026658), reconstructed
     * from each slot's savedata (character + save counter) — NOT the on-card BIOS title. */
    int slot_char[RE15_SAVE_SLOTS], slot_cnt[RE15_SAVE_SLOTS], slot_loc[RE15_SAVE_SLOTS];
    for (int i = 0; i < RE15_SAVE_SLOTS; i++) {
        slot_char[i] = 0; slot_cnt[i] = 0; slot_loc[i] = 0;
        if (used[i]) {
            re15_savedata_t sl;
            if (re15_memcard_load(RE15_CARD_PATH, i, &sl) == 0) {
                slot_char[i] = sl.character; slot_cnt[i] = sl.save_count % 100;
                slot_loc[i]  = sl.loc_idx;   /* Ortsindex aus dem Blob (Patch-Analog: Pfad A
                                              * SAVE_LOC_FUNC lbu v0,3(a0) @0x800708a0 liest
                                              * den Kartenblock-Header) */
            }
            else used[i] = 0;
        }
    }

    enum { ST_LIST, ST_OVERWRITE, ST_RESULT } st = ST_LIST;
    int cursor = 0, ow = 0 /* overwrite cursor defaults to Yes/top (FUN_80025c00 case7: uVar8=0) */, result = -1, running = 1;
    const char *msg = 0;
    unsigned blink = 0;
    /* FUN_80025c00 auto-repeat counter (local_40): 0x13 initial, 0x05 repeat. Arm the 0x13 initial
     * delay on entry so a direction HELD from gameplay (walking onto the phone, no fresh press) does
     * NOT step the cursor on frame 1 — only a FRESH press does. (The original steps a held direction
     * via the local_40==0 auto-repeat; the initial delay debounces the carried-over hold.) */
    int nav_delay = 0x13;
    int auto_drive = getenv("RE15_CARD_AUTO") != NULL;
    /* TEST-HAKEN (kein Spielverhalten): RE15_CARD_SLOT="a,b,c" waehlt beim AUTO-DRIVE je
     * Screen-Aufruf der Reihe nach den Start-Cursor (sonst bestaetigt der Auto-Drive immer
     * Slot 0). Nur damit laesst sich die Nutzer-Sequenz "Spielstand laden -> auf einen NEUEN
     * Slot speichern" headless fahren (integration_save_counter_pin). */
    if (auto_drive) {
        const char *cs = getenv("RE15_CARD_SLOT");
        if (cs) {
            static int cs_idx = 0;
            const char *p = cs;
            for (int i = 0; i < cs_idx && p; i++) { p = strchr(p, ','); if (p) p++; }
            if (p && *p) { int n = atoi(p); if (n >= 0 && n < RE15_SAVE_SLOTS) cursor = n; }
            cs_idx++;
        }
    }
    const char *card_shot = getenv("RE15_CARD_SHOT");
    unsigned af = 0;

    /* Byte-true card-screen FADE-IN on open (FUN_800264e8 @0x8002653c: fade channel step -0x1800,
     * level 0x7fff ramping down, brightness level>>7 = 255,207,159,111,63,15 over 6 frames, then
     * clear). Mirror of the exit fade-out. (af starts at 0 in the main loop below, so the auto-drive
     * screenshot/confirm timing is unaffected.) */
    {
        extern void re15_render_pc_set_fade(int a);
        for (int level = 0x7FFF; !(level & 0x8000); level -= 0x1800) {
            re15_render_begin_frame();
            re15_input_tick();
            pc_draw_card_static(&s_card_bg, used, slot_char, slot_cnt, slot_loc, save_mode);
            re15_render_pc_set_fade(level >> 7);
            re15_render_end_frame();
            /* ⛔ SOUND-PUMPE auch in den Menue-Schleifen (Nutzer-Auftrag 2026-08-31:
             * "in den Optionen ... fehlt der Menu Bewegungssound ... Auch im Hauptmenu:
             * New Game, Load etc."). URSACHE: re15_audio_tick() — die Pumpe, die die
             * angestossenen Stimmen weiterfuehrt — lief NUR in der Spielschleife.
             * Titel, Optionen, Speicher- und Ladeschirm haben eigene Schleifen und
             * blieben deshalb stumm, obwohl die SE-Aufrufe dort teils schon standen. */
            re15_audio_tick();
        }
        re15_render_pc_set_fade(0);
    }

    while (running) {
        re15_render_begin_frame();
        re15_input_tick();
        if (s_card_bg.pixels) re15_render_pc_show_cardbg(&s_card_bg);
        else re15_render_background_gradient(0, 0, 0, 0, 0, 0);   /* card BG missing -> BLACK, not blue */

        /* All text in the RE1.5 GAME font (TEX.TIM), attr 0; centered via glyph width. Strings are
         * the byte-true RE1.5 sysmes (DEBUG.BIN @0x5fd9): idx2 "Memory Card 1" header, idx0/1
         * "Do not save/load" exit, idx0x10 "NO DATA", idx8 "Overwrite?", idx9/0xa Yes/No,
         * idx0x11 "Access error", idx0x15 "Fail in save". */
        re15_render_pc_game_text(160 - re15_render_pc_game_text_width("Memory Card 1") / 2, 24, "Memory Card 1", 0);
        for (int i = 0; i < RE15_SAVE_SLOTS; i++) {                        /* 5 slots x=41 y=56+20i */
            if (used[i]) { uint8_t tc[48]; int tn = pc_slot_title_codes(tc, slot_char[i], slot_cnt[i], slot_loc[i]);
                           re15_render_pc_game_codes(41, 56 + i * 20, tc, tn, 0); }
            else re15_render_pc_game_text(41, 56 + i * 20, "NO DATA", 0);  /* sysmes idx 0x0f */
        }
        re15_render_pc_game_text(41, 156, save_mode ? "Do not save" : "Do not load", 0);  /* row 5 = exit */

        if (st == ST_OVERWRITE) {
            re15_render_pc_game_text(160 - re15_render_pc_game_text_width("Overwrite?") / 2, 178, "Overwrite?", 0);
            re15_render_pc_game_text(139, 196, "Yes", 0);
            re15_render_pc_game_text(139, 214, "No", 0);
        } else if (st == ST_RESULT && msg) {
            re15_render_pc_game_text(160 - re15_render_pc_game_text_width(msg) / 2, 196, msg, 0);
        }

        /* the ▶ selection cursor (render_pc draws a filled white triangle at this position) */
        if (st == ST_LIST)           re15_render_pc_card_cursor(25, 56 + cursor * 20, !(blink & 0x10));  /* FUN_80026be8(0x19,row*0x14+0x38) */
        else if (st == ST_OVERWRITE) re15_render_pc_card_cursor(123, 196 + ow * 18, !(blink & 0x10));
        else                         re15_render_pc_card_cursor(0, 0, 0);
        re15_render_end_frame();
        re15_audio_tick();
        blink++;

        uint16_t pp = g_engine.pad_pressed;
        if (auto_drive) {
            { const char *sa = getenv("RE15_CARD_SHOT_AF"); unsigned saf = sa ? (unsigned)atoi(sa) : 3;
              if (card_shot && af == saf) re15_render_pc_screenshot(card_shot); }
            { const char *hd = getenv("RE15_CARD_HOLD"); unsigned hold = hd ? (unsigned)atoi(hd) : 6;
              if (af == hold || af == hold + 20) pp |= RE15_PAD_BIT_SQUARE; }   /* open, then confirm/dismiss */
            af++;
        }
        /* Byte-true confirm/cancel (FUN_80025c00 cases 1/2/6): the FSM checks the VIRTUAL edge
         * word (DAT_800ac76c) bit 0x4000 (= remap-entry 14 <- RAW SQUARE @0x80073dbc[14]) plus
         * raw START to PROCEED/confirm (andi 0x800 auf DAT_800ac75c @0x80025e98), and virtual
         * 0x8000 (= entry 15 <- RAW CROSS) to go BACK/cancel (andi 0x8000 @0x80025ea0). Same
         * □-confirm/✕-cancel convention as every other virtual-word menu.
         * ZURUECKGENOMMEN 2026-08-05 (Nutzer): hier stand kurzzeitig
         * `re15_pad_virtual_word(pc_pad_config(pp))`, damit der Card-Screen der
         * OPTIONS-Button-Config folgt (unter Type B/C bestaetigt das Original mit ○,
         * v0x4000<-0x0020 @0x80073ddc[14]). Ausloeser war ein Report ueber die
         * Bestaetigen-/Abbrechen-Belegung im Ladebildschirm — der sich als RDP-Problem
         * der Eingabe herausstellte, NICHT als Port-Fehler. Der Zusatz ist darum wieder
         * draussen; der Card-Screen liest wie zuvor das rohe Pad. Die RE-Belege zur
         * Belegung bleiben in analysis/confirm_cancel_mapping.md erhalten. */
        uint16_t vp     = re15_pad_virtual_word(pp);
        uint16_t ok     = (uint16_t)((vp & 0x4000) | (pp & RE15_PAD_BIT_START));
        uint16_t cancel = (uint16_t)(vp & 0x8000);

        /* Byte-true cursor navigation (FUN_80025c00): read the HELD pad (DAT_800ac760, raw
         * PadRead layout) with a 19/5-frame auto-repeat — up = UP, down = DOWN | SELECT
         * (0x4100 = PADLdown | PADselect, so SELECT also steps down). A fresh press steps
         * immediately + arms the 0x13 initial delay; while held, repeat every 0x05 frames. */
        uint16_t held    = g_engine.pad_current;
        uint16_t nav_msk = RE15_PAD_BIT_UP | RE15_PAD_BIT_DOWN | RE15_PAD_BIT_SELECT;
        int nav_step = 0;
        if ((pp & nav_msk) == 0 && nav_delay != 0) { nav_delay--; }
        else { nav_delay = (pp & nav_msk) ? 0x13 : 0x05; nav_step = 1; }
        int nav_up   = nav_step && (held & RE15_PAD_BIT_UP);
        int nav_down = nav_step && (held & (RE15_PAD_BIT_DOWN | RE15_PAD_BIT_SELECT));

        /* Menue-SEs des Save-Screens — CORE-Bank-4-Triade wie im RE2-MEM_CARD.BIN-Vorbild des
         * portierten Save-Systems (jal-Zensus: 0x0404 Cursor x5 @0x801c05e0/0604/0778/07bc/166c,
         * 0x0405 Cancel x8, 0x0406 Confirm x9 via RE2-SE-Player FUN_8005ba28) UND der RE1.5-eigenen
         * gameweiten Menue-Konvention (Inventar FUN_8004a0cc: 0x0404 @0x8004a478ff, 0x0406
         * @0x8004a51c-20, 0x0405 @0x8004a660-64). RE1.5s EIGENER (dormanter) Card-Screen
         * FUN_80025c00 ist sound-frei (unfertiger Dev-Code, vollstaendiger jal-Zensus) — die
         * Beeps folgen dem RE2-Template, dem dieser Screen ohnehin entstammt
         * (analysis/typewriter_sounds.md TW-3/TW-4, beide CONFIRMED). */
        {
            extern void re15_audio_core_se(int se_id);
            int cur_moved = 0;
            if (st == ST_LIST)       cur_moved = (nav_up || nav_down);
            else if (st == ST_OVERWRITE) cur_moved = (nav_up || nav_down);
            if (cur_moved) re15_audio_core_se(4);            /* Cursor-Blip 0x0404 */
            if (ok)        re15_audio_core_se(6);            /* Confirm 0x0406     */
            else if (cancel) re15_audio_core_se(5);          /* Cancel 0x0405      */
        }
        if (st == ST_LIST) {
            if (nav_up)   { cursor = (cursor + RE15_SAVE_SLOTS) % (RE15_SAVE_SLOTS + 1); blink = 0; }
            if (nav_down) { cursor = (cursor + 1) % (RE15_SAVE_SLOTS + 1); blink = 0; }
            if (cancel) running = 0;
            else if (ok) {
                if (cursor == RE15_SAVE_SLOTS) running = 0;               /* EXIT row */
                else if (save_mode) {                                    /* SAVE */
                    if (used[cursor]) { st = ST_OVERWRITE; ow = 0; blink = 0; }  /* default Yes (FUN_80025c00) */
                    else if (pc_do_save(cursor, sd)) { result = cursor; running = 0; }  /* no completion msg (byte-true) */
                    else { msg = "Fail in save"; st = ST_RESULT; }       /* sysmes idx 0x15 */
                } else {                                                 /* LOAD */
                    /* Empty slot -> sysmes 0x14 "There are no files" (FUN_80025c00 case7: empty
                     * marker 5 -> unaff_s1=3 -> state 0xc -> idx unaff_s1+0x11=0x14); the LIST already
                     * labels empty slots "NO DATA" (0x10). Load I/O failure -> sysmes 0x17 "Fail in
                     * load" (case9: unaff_s1=6 -> idx 0x17), not "Access error" (0x12, card-level). */
                    if (!used[cursor]) { msg = "There are no files"; st = ST_RESULT; }
                    else if (re15_memcard_load(RE15_CARD_PATH, cursor, &s_resume_sd) == 0) {
                        if (out_room) *out_room = s_resume_sd.room; result = cursor; running = 0;
                    } else { msg = "Fail in load"; st = ST_RESULT; }
                }
            }
        } else if (st == ST_OVERWRITE) {
            if (nav_up || nav_down) { ow ^= 1; blink = 0; }
            if (cancel) { st = ST_LIST; blink = 0; }
            else if (ok) {
                if (ow == 0) {                                           /* Yes -> overwrite */
                    if (pc_do_save(cursor, sd)) { result = cursor; running = 0; }  /* byte-true: no completion msg */
                    else { msg = "Fail in save"; st = ST_RESULT; }
                } else { st = ST_LIST; blink = 0; }                      /* No */
            }
        } else {                                                         /* ST_RESULT */
            if (ok || cancel) {
                if (result >= 0) running = 0;                            /* saved/loaded -> done */
                else { st = ST_LIST; msg = 0; }                          /* error -> back to list */
            }
        }
    }
    /* LOAD chosen: byte-true card-screen FADE-OUT to black (FUN_80026594 @0x800265a0-e8): the exit
     * kicks fade channel 0 with step 0x1800, ramping level 0 -> 0x8000 (done bit15) while polling —
     * re15_fade_tick brightness = level>>7 (fade_common.c:63), so the overlay steps 0,48,96,144,192,
     * 240 over exactly 6 frames, then @0x800265ec holds a static 0x7fff (>>7 = 255 = full black). The
     * room's SCD fades it back in. (The port previously flashed the title's blue-ish backdrop.) */
    if (!save_mode && result >= 0) {
        extern void re15_render_pc_set_fade(int a);
        for (int level = 0; !(level & 0x8000); level += 0x1800) {   /* step 0x1800, done at bit15 */
            re15_render_begin_frame();
            re15_input_tick();
            pc_draw_card_static(&s_card_bg, used, slot_char, slot_cnt, slot_loc, save_mode);
            re15_render_pc_set_fade(level >> 7);                     /* byte-true brightness = level>>7 */
            re15_render_end_frame();
            re15_audio_tick();
        }
        re15_render_pc_set_fade(255);   /* @0x800265ec: static 0x7fff hold -> full black until the room fades in */
    }
    re15_render_pc_hide_cardbg();
    return result;
}

/* PLAYER-SELECT 3D MODELS (Leon PL00 left / Elza PL01 right) — byte-true camera + transform
 * (RE15_PLAYER_SELECT_DRAW.md, workflow wf_0aab308c, savestate-verified). Self-contained render:
 * reuses the port's pose (re15_compute_actor_kf/re15_skel_compute_pose) + camera-compose + textri
 * building blocks, with NO game-state coupling. Camera (wide idle shot): projection H=1000, screen
 * center (160,120); global view R=diag(4104,4096,4104), TR=(0,0,20039). Models at world POS
 * (Leon -1568 / Elza +1568, Y=2036, Z=0), rot_y 0x404, idle clip 2. Both drawn every frame. */
typedef struct { re15_md1_t md1; re15_emd_skeleton_t skel; re15_emd_animation_t anim; int tim_slot; int ok;
                 re15_md1_t wpn_md1; int wpn_ok; int wpn_tim_slot;
                 /* the PARSED body TIM stays with the model so the caller can RE-UPLOAD it into the
                  * VRAM slot on every select entry (see pselect_upload_tim). Its pixel/CLUT pointers
                  * alias the resident file buffer read in pselect_load_model (never freed). */
                 re15_tim_t tim; int tim_ok; } pselect_model_t;

static void pselect_load_model(pselect_model_t *m, const char *md1p, const char *eddp,
                               const char *base_emrp, const char *kf_emrp, const char *timp, int tim_slot,
                               const char *wpn_plw, int wpn_tim_slot)
{
    extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
    memset(m, 0, sizeof *m); m->tim_slot = tim_slot; m->wpn_tim_slot = wpn_tim_slot;
    int sz = 0;
    uint8_t *mb = pc_read_shared(md1p, &sz); if (!mb || re15_md1_parse(mb, sz, &m->md1) != 0) return;
    uint8_t *eb = pc_read_shared(eddp, &sz); if (!eb || re15_emd_parse_animation(eb, sz, &m->anim) != 0) return;
    /* The skeleton HIERARCHY + bind pose come from the BASE PLD EMR (a full EMR: header @0x64
     * bones-table / @0xb0 keyframes). The weapon PLW EMR is KEYFRAME-ONLY (8-byte header + N*80
     * keyframes, NO bone table) and shares the base skeleton — byte-true to the original, which
     * keeps Leon's resident PL00.PLD skeleton (@0x801bd814) and points model+0x170 at the weapon
     * keyframes. Parsing the keyframe-only PLW EMR as a full EMR read the hierarchy out of its
     * 8-byte header → garbage parents → the folded/flying skeleton. So: parse base for the
     * skeleton, then OVERRIDE the keyframe block with the weapon EMR's keyframes. */
    uint8_t *rb = pc_read_shared(base_emrp, &sz); if (!rb || re15_emd_parse_skeleton(rb, sz, &m->skel) != 0) return;
    if (kf_emrp) {
        int ksz = 0; uint8_t *kb = pc_read_shared(kf_emrp, &ksz);
        if (kb && ksz > 8) {
            int koff = kb[2] | (kb[3] << 8);   /* weapon EMR header @2 = keyframes_offset (= 8) */
            int kfs  = m->skel.keyframe_size_bytes > 0 ? m->skel.keyframe_size_bytes : 80;
            m->skel.keyframe_data      = kb + koff;
            m->skel.keyframe_data_size = (size_t)(ksz - koff);
            m->skel.keyframe_count     = (ksz - koff) / kfs;
        }
    }
    int tsz = 0; uint8_t *tb = pc_read_shared(timp, &tsz);   /* buffer stays resident (tim aliases it) */
    if (tb && re15_tim_parse(tb, tsz, &m->tim) == 0) {
        m->tim_ok = 1;
        re15_render_pc_upload_tim_slot(&m->tim, tim_slot);
    }
    /* EQUIPPED WEAPON (W03 handgun): the hand+gun mesh is PLW section [2] (de[2]..de[3]), drawn on
     * bone 11 at render time. It is textured from the character's BODY skin TIM (all its tris read
     * page 0x81 / clut 0x7840 = the same tpage as the body's hand mesh) — the PLW's own dir[3] TIM
     * is NOT referenced by this mesh, so we don't upload it. (void)wpn_tim_slot. */
    (void)wpn_tim_slot;
    if (wpn_plw) {
        int psz = 0; uint8_t *plw = pc_read_shared(wpn_plw, &psz);   /* stays resident (MD1 borrows) */
        if (plw && psz >= 16) {
            uint32_t diroff = (uint32_t)(plw[0] | (plw[1]<<8) | (plw[2]<<16) | ((uint32_t)plw[3]<<24));
            uint32_t de[4] = {0};
            if (diroff + 16 <= (uint32_t)psz)
                for (int k = 0; k < 4; k++)
                    de[k] = (uint32_t)(plw[diroff+4*k] | (plw[diroff+4*k+1]<<8) |
                                       (plw[diroff+4*k+2]<<16) | ((uint32_t)plw[diroff+4*k+3]<<24));
            if (de[3] > de[2] && re15_md1_parse(plw + de[2], (int)(de[3] - de[2]), &m->wpn_md1) == 0)
                m->wpn_ok = 1;
        }
    }
    m->ok = 1;
    if (getenv("RE15_PSELECT_DIAG")) {
        fprintf(stderr, "PSELECT LOAD base=%s kf=%s: md1.mesh=%d edd.clip=%d emr.bone=%d emr.kf=%d\n",
                base_emrp, kf_emrp ? kf_emrp : "(none)", m->md1.mesh_count, m->anim.clip_count,
                m->skel.bone_count, m->skel.keyframe_count);
        if (m->wpn_ok && m->wpn_md1.mesh_count > 0) {
            const re15_md1_mesh_t *wm = &m->wpn_md1.meshes[0];
            fprintf(stderr, "  WPN mesh: tris=%d quads=%d tvtx=%d page=0x%x (body page 0x%x)\n",
                    wm->triangle_count, wm->quad_count, wm->tri_vertex_count,
                    wm->triangle_count>0 ? wm->triangle_uvs[0].page : 0,
                    m->md1.mesh_count>11 && m->md1.meshes[11].triangle_count>0 ? m->md1.meshes[11].triangle_uvs[0].page : 0);
        }
    }
}

static void pselect_render_model(const pselect_model_t *m, int32_t px, int32_t py, int32_t pz,
                                 int16_t rot_y, uint32_t frame, const re15_camera_view_t *cam)
{
    extern void re15_render_pc_bind_tim_slot(int slot);
    if (!m->ok || m->anim.clip_count <= 0 || m->md1.mesh_count <= 0) return;
    re15_actor_t a; memset(&a, 0, sizeof a);
    /* BYTE-TRUE idle clip = model+0x94 = 2. Verified against the LIVE player-select savestate
     * (mzd_after_flow.sav): both model structs read model->0x94 = 2, ->0x95 (anim_frame) = 37,
     * ->0x6a rot = 1028. FUN_8001f314 (@0x8001f324 `lbu v0,148(t0)`) reads the clip from
     * g_entity->0x94; TITLE.BIN never writes +0x94 (the EXE model spawn seeds it to 2). The
     * original renders clip 2 as an upright arms-down idle — the port's clip-2 contortion is a
     * POSE-computation bug, NOT a wrong clip. */
    int clip = 2;   /* RE15_PSELECT_CLIP overrides for the clip sweep */
    { const char *cc = getenv("RE15_PSELECT_CLIP"); if (cc) clip = atoi(cc); }
    a.motion = (int16_t)clip; a.anim_frame = (int32_t)frame; a.rot_y = rot_y;
    /* QUERY mode: re15_skel_compute_pose reads g_anim_pose_actor for the FRAC crossfade + the
     * player head-look; the player-select is a standalone render (no game actor), so force NULL =
     * raw keyframe pose, no crossfade, no head-look (same as the enemy_ai/damage pose queries).
     * Leaving it at a stale in-game pointer blended the idle against a foreign skeleton. */
    extern void *g_anim_pose_actor; void *save_gpa = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int kf = re15_compute_actor_kf(&m->anim, &m->skel, &a, clip, frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    if (re15_skel_compute_pose(&m->skel, kf, poses) != 0) { g_anim_pose_actor = save_gpa; return; }
    g_anim_pose_actor = save_gpa;
    if (getenv("RE15_PSELECT_DIAG") && frame == 37) {
        fprintf(stderr, "PSELECT POSE tim%d kf=%d bones=%d root.trans=(%d,%d,%d) b8.trans=(%d,%d,%d) b6=(%d,%d,%d)\n",
                m->tim_slot, kf, m->skel.bone_count,
                (int)poses[0].trans[0],(int)poses[0].trans[1],(int)poses[0].trans[2],
                (int)poses[8].trans[0],(int)poses[8].trans[1],(int)poses[8].trans[2],
                (int)poses[6].trans[0],(int)poses[6].trans[1],(int)poses[6].trans[2]); }
    int32_t yaw[9]; re15_camera_yaw_matrix_angle(rot_y, yaw);
    int nb = m->skel.bone_count; if (nb > m->md1.mesh_count) nb = m->md1.mesh_count;
    const int cx = 160, cy = 120;    /* OFX/OFY */
    const uint8_t C = 0x80;          /* neutral fallback tint when a mesh part lacks per-vertex normals */
    /* NCCT PER-VERTEX LIGHTING — BYTE-TRUE, verified end-to-end against the live original
     * (mzd_after_flow.sav, 2026-07-14). The player-select models are GTE NCCT-lit (front-end draw
     * FUN_8001e8c8 → FUN_800254a0, GTE cmd 0x4b18043f), NOT flat. The cut below is read
     * FIELD-FOR-FIELD from the scene light struct @0x80190088 (NOT guessed/calibrated):
     *   global_scale=1, type_flags=(1,1,1) directional, colors all (128,128,128),
     *   ambient/BackColor (76,76,86), positions L0(-4600,-4000,-23200)/L1(-99,-2000,-29200)/
     *   L2(2000,2000,2000), ranges 20000/10000/20000.
     * Cross-checked against the FINAL GTE matrices in RAM: LLM @0x80076d14 =
     *   {786,684,3964 / 14,281,4089 / -2365,-2365,-2365}  — which is exactly
     *   -normalize(position) per light, i.e. re15_light_setup_actor reproduces the GTE LLM to ±1;
     *   LCM @0x80076d34 = 2048 = colour 128<<4 for all 3 lights. So light DATA + NCCT MATH are byte-true.
     * (A one-time WRONG Z-negation experiment was reverted: the positions above are the byte-true ones.)
     * Verified visually at native 320×240 vs the PSX framebuffer: Leon clearly lit, Elza near-black.
     * The only remaining brightness delta vs a DuckStation *capture* is a GLOBAL ~7/255 framebuffer-
     * precision offset (settings DitheringMode=TrueColor round-to-nearest vs the port's <<3 truncate)
     * that shifts the un-lit static backdrop by the SAME amount as the lit model — an engine-wide
     * texture-precision matter, not a player-select lighting difference. Same pipeline for both
     * characters; only the mirrored world-X (Leon -1568 / Elza +1568) changes the light direction. */
    static const re15_light_cut_t s_ps_cut = {
        1, {1,1,1}, {{128,128,128},{128,128,128},{128,128,128}}, {76,76,86},
        {{-4600,-4000,-23200},{-99,-2000,-29200},{2000,2000,2000}}, {20000,10000,20000} };
    re15_actor_lightctx_t lctx_world;
    { int32_t apos[3] = { px, py, pz }; re15_light_setup_actor(&s_ps_cut, apos, NULL, &lctx_world); }
    /* NCCT ON by default (disable for A/B with RE15_PSELECT_NO_NCCT). Byte-true: the models ARE GTE
     * NCCT-lit; the selected char is clearly lit and the UNSELECTED char is near-black — matching the
     * PSX framebuffer (verified at native 320×240: Leon lit, Elza near-black; the earlier "too bright"
     * apparent gap was a mis-aligned crop between the 1024-wide capture and the 640-wide render). */
    int ncct_on = getenv("RE15_PSELECT_NO_NCCT") == NULL;
    /* EQUIPPED WEAPON (byte-true, workflow wf_8837a549 + user-corrected): the player-select draws the
     * model through ONE shared skeleton-part pass (FUN_8001e8c8 walks model+0x188 parts); the
     * equipped weapon is part[11] = a HAND+GUN mesh (35 verts) that REPLACES the plain right-hand
     * mesh (23 verts) — so the character's hand is drawn INCLUDING the weapon. BOTH characters carry
     * it (user: "sowohl Leon als auch Elza haben ihre Hand inklusive Waffe" — the savestate that
     * showed only Leon armed was a transient load state). So at bone 11, draw the weapon MD1
     * (PLW dir[2]) with the weapon TIM (PLW dir[3]) using bone 11's own composed matrix, each
     * character its own PLW (Leon PL00W03 / Elza PL04W03) — same slot-per-tri queue as in-game. */
    for (int bi = 0; bi < nb; bi++) {
        const re15_skel_pose_t *p = &poses[bi];
        int32_t yr[9], yt[3];
        for (int r = 0; r < 3; r++) for (int c = 0; c < 3; c++) {
            int64_t s = 0; for (int k = 0; k < 3; k++) s += (int64_t)yaw[r*3+k] * (int64_t)p->rot[k*3+c];
            yr[r*3+c] = (int32_t)(s >> 12); }
        for (int r = 0; r < 3; r++) {
            int64_t s = 0; for (int k = 0; k < 3; k++) s += (int64_t)yaw[r*3+k] * (int64_t)p->trans[k];
            yt[r] = (int32_t)(s >> 12); }
        int32_t bwt[3] = { yt[0] + px, yt[1] + py, yt[2] + pz };
        int32_t bm[9], bt[3];
        re15_camera_compose_view_bone(cam, yr, bwt, bm, bt);
        /* rotate the world light context into THIS bone's frame (yr = R_y(yaw)×pose.rot = the bone's
         * world rotation), so a RAW bone-local vertex normal shades as L_world·N_world (byte-true
         * per-bone SetLightMatrix, FUN_8001e9ec). */
        re15_actor_lightctx_t lctx; re15_light_ctx_rotate_for_bone(&lctx_world, yr, &lctx);
        /* EQUIPPED WEAPON (byte-true, verified against the live original savestate): at bone 11 the
         * hand+gun mesh REPLACES the plain hand. Attach = bone 11 — the port's bone-11 world matrix
         * is byte-identical to the original's part[11] matrix (@+0x40 rot [2 204 4088]/[523 4048
         * -205]/[-4060 522 -18], trans (-2118,385,-66)). Mesh = PL04W03.PLW dir[2] (verts match the
         * savestate 6/6; a shared weapon buffer, so BOTH characters use PL04W03). The mesh is
         * textured entirely from the character's BODY skin TIM — every tri/quad reads page 0x81 /
         * clut 0x7840, the SAME tpage as body mesh 11 — so it binds m->tim_slot (NOT the separate
         * PLW dir[3] gun TIM, which this mesh does not reference). Both characters carry it. */
        int wpn_here = (bi == 11 && m->wpn_ok && m->wpn_md1.mesh_count > 0);
        const re15_md1_mesh_t *mesh = wpn_here ? &m->wpn_md1.meshes[0] : &m->md1.meshes[bi];
        /* The shared PL04W03 hand+gun mesh is textured from PL04's skin TIM (the gun art lives in
         * PL04.TIM, slot 21) for BOTH characters — Leon's own body TIM (slot 20) has no gun there. */
        re15_render_pc_bind_tim_slot(wpn_here ? 21 : m->tim_slot);
#define PSV(vp, ox, oy, owz) do { \
        int32_t _x=(vp)->x,_y=(vp)->y,_z=(vp)->z; \
        int32_t _vx=(int32_t)(((int64_t)_x*bm[0]+(int64_t)_y*bm[1]+(int64_t)_z*bm[2])>>12)+bt[0]; \
        int32_t _vy=(int32_t)(((int64_t)_x*bm[3]+(int64_t)_y*bm[4]+(int64_t)_z*bm[5])>>12)+bt[1]; \
        int32_t _vz=(int32_t)(((int64_t)_x*bm[6]+(int64_t)_y*bm[7]+(int64_t)_z*bm[8])>>12)+bt[2]; \
        if(_vz<64){(owz)=-1.0f;(ox)=0;(oy)=0;} else { \
            int32_t _i1=_vx>0x7FFF?0x7FFF:(_vx<-0x8000?-0x8000:_vx); \
            int32_t _i2=_vy>0x7FFF?0x7FFF:(_vy<-0x8000?-0x8000:_vy); \
            uint32_t _s3=_vz>0xFFFF?0xFFFFu:(uint32_t)_vz; \
            uint32_t _n=re15_gte_divide((uint32_t)cam->fov_screen_dist,_s3); \
            (ox)=cx+(int)(((int64_t)_i1*(int64_t)_n)>>16); \
            (oy)=cy+(int)(((int64_t)_i2*(int64_t)_n)>>16); \
            (owz)=(float)_vz; } } while(0)
#define PSBF(ax,ay,bx,by,ccx,ccy) (((long long)((bx)-(ax))*((ccy)-(ay)) - (long long)((by)-(ay))*((ccx)-(ax))) <= 0)
        for (int ti = 0; ti < mesh->triangle_count; ti++) {
            const re15_md1_triangle_t *t = &mesh->triangles[ti];
            if (t->v0 >= (uint32_t)mesh->tri_vertex_count || t->v1 >= (uint32_t)mesh->tri_vertex_count ||
                t->v2 >= (uint32_t)mesh->tri_vertex_count) continue;
            int sx0,sy0,sx1,sy1,sx2,sy2; float wz0,wz1,wz2;
            PSV(&mesh->tri_vertices[t->v0],sx0,sy0,wz0);
            PSV(&mesh->tri_vertices[t->v1],sx1,sy1,wz1);
            PSV(&mesh->tri_vertices[t->v2],sx2,sy2,wz2);
            if (wz0 < 0 || wz1 < 0 || wz2 < 0) continue;
            if (PSBF(sx0,sy0,sx1,sy1,sx2,sy2)) continue;
            const re15_md1_tri_uv_t *uv = &mesh->triangle_uvs[ti];
            int pxo = (int)((uv->page & 0xF) * 128);
            int avgz = (int)((wz0 + wz1 + wz2) * (1.0f/3.0f));
            uint8_t r0,g0,b0,r1,g1,b1,r2,g2,b2;
            if (ncct_on && mesh->tri_normals && t->n0 < (uint32_t)mesh->tri_normal_count &&
                t->n1 < (uint32_t)mesh->tri_normal_count && t->n2 < (uint32_t)mesh->tri_normal_count) {
                const re15_md1_vertex_t *nn0=&mesh->tri_normals[t->n0], *nn1=&mesh->tri_normals[t->n1], *nn2=&mesh->tri_normals[t->n2];
                re15_light_shade_vertex(&lctx, nn0->x,nn0->y,nn0->z, &r0,&g0,&b0);
                re15_light_shade_vertex(&lctx, nn1->x,nn1->y,nn1->z, &r1,&g1,&b1);
                re15_light_shade_vertex(&lctx, nn2->x,nn2->y,nn2->z, &r2,&g2,&b2);
            } else { r0=r1=r2=C; g0=g1=g2=C; b0=b1=b2=C; }
            re15_render_textured_tri_lit(sx0,sy0,(int)uv->u0+pxo,(int)uv->v0, sx1,sy1,(int)uv->u1+pxo,(int)uv->v1,
                sx2,sy2,(int)uv->u2+pxo,(int)uv->v2, 0,(int)uv->clut,avgz, r0,g0,b0,r1,g1,b1,r2,g2,b2);
        }
        for (int qi = 0; qi < mesh->quad_count; qi++) {
            const re15_md1_quad_t *q = &mesh->quads[qi];
            if (q->v0 >= (uint32_t)mesh->quad_vertex_count || q->v1 >= (uint32_t)mesh->quad_vertex_count ||
                q->v2 >= (uint32_t)mesh->quad_vertex_count || q->v3 >= (uint32_t)mesh->quad_vertex_count) continue;
            int sx0,sy0,sx1,sy1,sx2,sy2,sx3,sy3; float wz0,wz1,wz2,wz3;
            PSV(&mesh->quad_vertices[q->v0],sx0,sy0,wz0);
            PSV(&mesh->quad_vertices[q->v1],sx1,sy1,wz1);
            PSV(&mesh->quad_vertices[q->v2],sx2,sy2,wz2);
            PSV(&mesh->quad_vertices[q->v3],sx3,sy3,wz3);
            if (wz0 < 0 || wz1 < 0 || wz2 < 0 || wz3 < 0) continue;
            if (PSBF(sx0,sy0,sx1,sy1,sx2,sy2)) continue;
            const re15_md1_quad_uv_t *uv = &mesh->quad_uvs[qi];
            int pxo = (int)((uv->page & 0xF) * 128);
            int az1 = (int)((wz0+wz1+wz3)*(1.0f/3.0f)), az2 = (int)((wz0+wz3+wz2)*(1.0f/3.0f));
            uint8_t qr0,qg0,qb0,qr1,qg1,qb1,qr2,qg2,qb2,qr3,qg3,qb3;
            if (ncct_on && mesh->quad_normals && q->n0 < (uint32_t)mesh->quad_normal_count && q->n1 < (uint32_t)mesh->quad_normal_count &&
                q->n2 < (uint32_t)mesh->quad_normal_count && q->n3 < (uint32_t)mesh->quad_normal_count) {
                const re15_md1_vertex_t *m0=&mesh->quad_normals[q->n0], *m1=&mesh->quad_normals[q->n1],
                                        *m2=&mesh->quad_normals[q->n2], *m3=&mesh->quad_normals[q->n3];
                re15_light_shade_vertex(&lctx, m0->x,m0->y,m0->z, &qr0,&qg0,&qb0);
                re15_light_shade_vertex(&lctx, m1->x,m1->y,m1->z, &qr1,&qg1,&qb1);
                re15_light_shade_vertex(&lctx, m2->x,m2->y,m2->z, &qr2,&qg2,&qb2);
                re15_light_shade_vertex(&lctx, m3->x,m3->y,m3->z, &qr3,&qg3,&qb3);
            } else { qr0=qr1=qr2=qr3=C; qg0=qg1=qg2=qg3=C; qb0=qb1=qb2=qb3=C; }
            re15_render_textured_tri_lit(sx0,sy0,(int)uv->u0+pxo,(int)uv->v0, sx1,sy1,(int)uv->u1+pxo,(int)uv->v1,
                sx3,sy3,(int)uv->u3+pxo,(int)uv->v3, 0,(int)uv->clut,az1, qr0,qg0,qb0,qr1,qg1,qb1,qr3,qg3,qb3);
            re15_render_textured_tri_lit(sx0,sy0,(int)uv->u0+pxo,(int)uv->v0, sx3,sy3,(int)uv->u3+pxo,(int)uv->v3,
                sx2,sy2,(int)uv->u2+pxo,(int)uv->v2, 0,(int)uv->clut,az2, qr0,qg0,qb0,qr3,qg3,qb3,qr2,qg2,qb2);
        }
#undef PSV
#undef PSBF
    }
}

/* PLAYER-SELECT scene — "PLEASE SELECT MAIN CAST" (byte-true TITLE.BIN task @0x80101094; full spec
 * in RE15_PLAYER_SELECT_DRAW.md). Runs after NEW-GAME confirm; returns the chosen character
 * (0=Leon, 1=Elza) = DAT_800aca5c>>2. This FOUNDATION does the byte-true state machine, input and
 * highlight crossfade over the SELECTH.TIM backdrop; the live PL00/PL01 3D models, the SELECTH3
 * name/profile SPRT overlays and the idle->zoom camera lerp are layered on in the next increments.
 *  - INPUT (sub1 @0x80101258): TOGGLE Leon<->Elza = D-pad L/R (mask 0xa000), GATED on the pulse
 *    scene+0x32e>=0x80 (16-frame debounce, @0x801012ac); CONFIRM = any face button or Start
 *    (mask 0x8f0, @0x80101268). Pad word @0x800ac762 is byte-swapped in the original; the port pad
 *    is already normalised so LEFT/RIGHT + face/START map 1:1.
 *  - PULSE (@0x8010080c): scene+0x32e init 0x80, +8/frame clamp 0x80, reset 0 on toggle. */
/* CONFIRM-ZOOM byte-true helpers (TITLE.BIN sub3-7). idiv40 = the exact trunc-toward-zero /40 used by
 * both the SPRT animator (@0x80100ac4) and the pan setup (magic 0x66666667, sra 4). */
static inline int32_t re15_idiv40(int32_t x)
{
    int32_t q = (int32_t)(((int64_t)x * (int64_t)0x66666667) >> 32);
    return (q >> 4) - (x >> 31);
}
/* Fade driver 0x8010026c (keyed on cz_c7): phase 0 → arm busy (c4=1); phase 1 → ramp c6 += 4 until it
 * wraps 0 after 64 ticks then latch 0xff; phase 2+ → release (c4=0). c6 = the sub4 fade level. */
static void re15_cz_fade_tick(int *c4, int *c6, int *c7)
{
    if (*c7 == 0)      { *c4 = 1; *c7 = 1; }
    else if (*c7 == 1) { *c6 = (*c6 + 4) & 0xff; if (*c6 == 0) { *c6 = 0xff; *c7 = 2; } }
    else               { *c4 = 0; }
}

static int pc_run_player_select(void)
{
    extern void re15_render_pc_player_select(const re15_tim_t *bg, int sel, int pulse);
    extern void re15_render_pc_pselect_dim(int level);
    extern void re15_render_pc_pselect_slide(int sel, int nx, int ny, int sx, int sy);
    extern void re15_render_pc_pselect_groupa(int on);
    extern void re15_render_pc_hide_player_select(void);
    extern void re15_render_pc_hide_title(void);
    extern void re15_render_pc_hide_title_menu(void);
    extern void re15_render_pc_set_title_fade(int a);
    extern void re15_render_pc_screenshot(const char *path);
    /* The menu confirm REPLACES the title task with this one (FUN_80029ba4 @0x80102c9c), so the
     * NEW GAME / LOAD GAME / OPTION menu + the title art STOP being drawn entirely. */
    re15_render_pc_hide_title_menu();
    re15_render_pc_hide_title();
    extern void re15_render_pc_pselect_text(const re15_tim_t *atlas, int sel);
    static re15_tim_t s_sel_bg = {0};
    if (!s_sel_bg.pixels) { int sz = 0; uint8_t *b = pc_read_shared("DATA/SELECTH.TIM", &sz);
                            if (b) re15_tim_parse(b, sz, &s_sel_bg); }   /* one-time; buffer kept */
    /* SELECTH3.TIM = the byte-true name/profile TEXT atlas (8bpp 256x256, colours baked in). */
    static re15_tim_t s_sel_txt = {0};
    if (!s_sel_txt.pixels) { int sz = 0; uint8_t *b = pc_read_shared("DATA/SELECTH3.TIM", &sz);
                             if (b) re15_tim_parse(b, sz, &s_sel_txt); }
    /* Load Leon(PL00) + Elza(PL08) once (TIM slots 20/21, free during the front-end).
     * BYTE-TRUE model ids (loader FUN_80101720 + CD dir @0x8006f43c): Leon = the resident PL00
     * (buffer 0x801bd814); Elza = CD file-id 0x44 whose dir-entry size (182928) is EXACTLY PL08.PLD.
     * (PL01 is Leon in a red POLICE costume — the wrong "different-costume Leon" I mis-loaded before.) */
    static pselect_model_t s_leon = {0}, s_elza = {0}; static int s_models_loaded = 0;
    if (!s_models_loaded) { s_models_loaded = 1;
        /* BYTE-TRUE anim source: the resident Leon in the player-select uses the WEAPON-variant
         * animation, NOT PL00.PLD's base EDD. Verified against the live savestate: the model's
         * anim table @0x801d7708 == PL00W03.EDD byte-for-byte (14 clips, clip2 = 52 frames ping-
         * ponging keyframes 38-51) and its skeleton @0x801d7bc0 == PL00W03.EMR. The base PL00.EDD
         * (24 clips, clip2 = keyframes 68+) is a DIFFERENT animation set → posed the wrong keyframe
         * (46 vs 105) → the folded/lying model. Body mesh + texture stay PL00 (PLW carries only the
         * weapon mesh). W03 = the handgun stance (Leon holds the pistol in the idle, as in the original). */
        /* WEAPON MESH = PL04W03.PLW dir[2] for BOTH characters (byte-true: the live savestate's Leon
         * part[11] weapon vertices match PL04W03 6/6, NOT PL00W03 — the player-select shares one weapon
         * buffer and PL04 is loaded last, so both models' hand+gun mesh is PL04W03's). Leon's body/anim
         * stays PL00/PL00W03; only the displayed weapon geometry is PL04W03. */
        pselect_load_model(&s_leon, "PLD/PL00.MD1", "PLD/PL00W03.EDD", "PLD/PL00.EMR", "PLD/PL00W03.EMR", "PLD/PL00.TIM", 20, "PLD/PL04W03.PLW", 22);
        /* BYTE-TRUE right character = ELZA = PL04 (blonde, red "RACCOON" biker suit), NOT PL08/Jill.
         * The live savestate's right model (g_entity 0x800ace20) reads: anim EDD @0x801460f4 ==
         * PL04W03.EDD and skeleton EMR @0x80146688 == PL04W03.EMR (both byte-verified). The MZD disc's
         * player-select CD id 0x44 loads PL04, not the prototype's PL08 — that is why my earlier
         * "right = PL08" (proto-disc directory) rendered Jill. Elza also holds the W03 handgun idle. */
        pselect_load_model(&s_elza, "PLD/PL04.MD1", "PLD/PL04W03.EDD", "PLD/PL04.EMR", "PLD/PL04W03.EMR", "PLD/PL04.TIM", 21, "PLD/PL04W03.PLW", 23); }
    /* PER-ENTRY VRAM RE-UPLOAD (fix Nutzer-Report "Modelle kaputt nach Tod -> NEW GAME", 2026-08-17).
     * The mesh/anim/skeleton PARSE is a process-lifetime cache (above), but the TIM SLOTS are NOT ours
     * between two select entries: every in-game boot overwrites slots 20-23 with the GLOBAL effect
     * sheets (RE15_TIM_SLOT_EFFECT_GLOBAL=20 blood, FX_MUZZLE=21, FX_SMOKE=22, FX_SHELL=23 — main.c
     * ~L2262-2284, one "[esp] global effect-* TIM -> slot N" line per boot in debug.log). Leon's body
     * skin lives in slot 20 and Elza's in slot 21, so the SECOND select (after Tod -> Titel -> NEW GAME)
     * rendered both characters out of the effect sheets = "Modelle kaputt".
     * The original re-does BOTH the load and the VRAM upload on every select entry: the menu confirm
     * REPLACES the title task with the select task (TITLE.BIN `addiu a0,0x80101094` @0x80102c98 +
     * `jal 0x80029ba4` @0x80102c9c), whose scene-init FUN_80101720 (called @0x801011d8) CD-loads the
     * models (`jal 0x80013b60` @0x80101808, file id from the table read @0x801017fc) and registers
     * their TIMs into VRAM (`jal 0x80022150` @0x80101870, a0=2). There is no process-lifetime cache.
     * The port keeps the parse cached (no CD here) but mirrors the VRAM half unconditionally. */
    {
        extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
        if (s_leon.tim_ok) re15_render_pc_upload_tim_slot(&s_leon.tim, s_leon.tim_slot);  /* @0x80101870 */
        if (s_elza.tim_ok) re15_render_pc_upload_tim_slot(&s_elza.tim, s_elza.tim_slot);
        if (getenv("RE15_PSELECT_DIAG"))
            fprintf(stderr, "PSELECT TIM RE-UPLOAD: leon slot %d ok=%d, elza slot %d ok=%d\n",
                    s_leon.tim_slot, s_leon.tim_ok, s_elza.tim_slot, s_elza.tim_ok);
    }
    /* Byte-true camera (wf_0aab308c): global view R=diag(4104,4096,4104), TR=(0,0,20039), H=1000. */
    re15_camera_view_t cam = {0};
    cam.rot[0] = 4104; cam.rot[4] = 4096; cam.rot[8] = 4104;
    cam.trans[0] = 0; cam.trans[1] = 0; cam.trans[2] = 20039;
    cam.fov_screen_dist = 1000;
    int sel = 0;        /* scene+0x394: Leon default (@0x801011f8) */
    int pulse = 0x80;   /* scene+0x32e: init 0x80 (@0x80100548) -> first toggle is instant */
    /* FADE-IN: sub0 kicks fade ch0 abr2-subtractive step -0x400 with level 0x7fff (FUN_800217b0
     * @0x8010121c + FUN_800216ec @0x80101240) -> brightness (level>>7) ramps 0xff->0 over 32 frames. */
    int fade_level = 0x7fff;   /* brightness = fade_level>>7 */
    /* CONFIRM-ZOOM FSM (TITLE.BIN LEVEL-2 scene+0x392, sub3-7). cz = -1 idle, 0..5 the FSM state.
     * sub3(1f init) -> sub4(65f 2D-layer fade via the c4/c6/c7 driver) -> sub5(1f pan/dolly setup) ->
     * sub6(41f: pan selected char world-X->0, spin rot_y+=8, camera dolly pos.z += quadratic accum,
     * rebuild LookAt) -> sub7(60f hold + 32f fade-to-black) -> handoff. */
    int cz = -1;                          /* -1 = idle input phase; 0..5 = confirm LEVEL-2 state */
    int cz_pflag = 0, cz_pcnt = 0;        /* FLAG scene+0x268 / Counter +0x26a (PROFILE-Additiv-Rampe) */
    int cz_c4 = 0, cz_c6 = 0, cz_c7 = 0;  /* fade driver 0x8010026c (busy / ramp level / phase) */
    int cz_396 = 0;                       /* shared counter scene+0x396 */
    int32_t cz_panvel = 0, cz_accel = 0, cz_accum = 0;  /* model pan vel (scene+0), accel04 (+4), cam-z accum (+0x74) */
    int32_t pan_x[2]  = { -1568, 1568 };  /* selected char world-X (field+0x34); pans toward 0 */
    int32_t pan_ry[2] = { 0x404, 0x404 }; /* selected char rot_y (field+0x6a); += 8/frame */
    int32_t blk_x[4], blk_y[4], blk_dx[4] = {0}, blk_dy[4] = {0};  /* SPRT name/sub blocks, 16.16 */
    int elza_alive = 1;                   /* the unselected char is disabled at the sub4->sub5 edge */
    int cz_black = 0;                      /* sub7 fade-to-black brightness (0..255, +8/frame) */
    /* seed SPRT blocks from the byte-true idle positions (@0x80102624): blk0/2 = names, blk1/3 = subs */
    { static const int bx[4] = {16,24,168,176}, by[4] = {48,216,48,216};
      for (int i=0;i<4;i++){ blk_x[i]=bx[i]<<16; blk_y[i]=by[i]<<16; } }
    static const int32_t cz_target[4][2] = { {11<<16,20<<16},{176<<16,208<<16},{11<<16,20<<16},{176<<16,208<<16} };
    unsigned frames = 0;
    const char *ps_shot = getenv("RE15_PSELECT_SHOT");
    int auto_drive = getenv("RE15_PSELECT_AUTO") != NULL;
    /* 30fps cap: the idle clip 2 advances +0x95 by 1 PER FRAME (FUN_8001f314 POST-inc), so the
     * scene MUST run at the PSX 30Hz cadence or the animation plays far too fast (the game loop
     * caps the same way). Without this the models twitch / cycle wrong. */
    uint32_t ps_last = SDL_GetTicks();
    while (re15_gameflow_mode() == RE15_MODE_TITLE) {
        if (pulse < 0x80) { pulse += 8; if (pulse > 0x80) pulse = 0x80; }
        /* ---- CONFIRM-ZOOM FSM step (byte-true LEVEL-2 sub3-7; runs BEFORE the render like the original
         * dispatch order sub2 -> renderers). Advances the state + counters + pan/dolly/slide. ---- */
        if (cz == 0) {                               /* sub3 START-ZOOM (once): arm the fade driver */
            cz_c6 = 0; re15_cz_fade_tick(&cz_c4, &cz_c6, &cz_c7); cz = 1;
            cz_pflag = 1; cz_pcnt = 0x20;            /* FLAG scene+0x268=1 + Counter +0x26a=0x20
                                                      * (@0x80101374-80): PROFILE-Badges ab jetzt
                                                      * ADDITIV mit RGB-Rampe Counter<<2 */
        } else if (cz == 1) {                        /* sub4 ZOOM-LOOP: fade the 2D layer over 64f */
            re15_cz_fade_tick(&cz_c4, &cz_c6, &cz_c7);
            if (cz_c4 == 0) { elza_alive = 0; cz_396 = 0; cz = 2; }   /* fade done: disable other char, arm pan/anim */
        } else if (cz == 2) {                        /* sub5 PAN-SETUP + animator SETUP (once) */
            cz_panvel = re15_idiv40(pan_x[sel]); cz_accel = 0x14; cz_accum = 0; cz_396 = 40;
            { int b0 = sel ? 2 : 0; for (int k = 0; k < 2; k++) { int b = b0 + k;
                blk_dx[b] = re15_idiv40(cz_target[b][0] - blk_x[b]);
                blk_dy[b] = re15_idiv40(cz_target[b][1] - blk_y[b]); } }
            cz = 3;
        } else if (cz == 3) {                        /* sub6 PAN (41f): model to centre + spin + camera dolly + slide */
            pan_x[sel] -= cz_panvel; pan_ry[sel] += 8;
            cz_accum += cz_accel; cz_accel += 0xA;
            { int b0 = sel ? 2 : 0; for (int k = 0; k < 2; k++) { int b = b0 + k; blk_x[b] += blk_dx[b]; blk_y[b] += blk_dy[b]; } }
            { int c = cz_396; cz_396--; if (c == 0) { cz_396 = 60; cz = 4; } }
        } else if (cz == 4) {                        /* sub7 FADE: 60f hold, then fade-to-black */
            int c = cz_396; cz_396--; if (c == 0) cz = 5;
        } else if (cz == 5) {                        /* handoff: ramp the fade-to-black (32f @ +8/frame) */
            cz_black += 8; if (cz_black > 255) cz_black = 255;
        }
        /* camera DOLLY (sub6+): pos.z = -20000 + accum; view (LookAt) keeps R constant, TR.z = R[8]*(-pos.z)/4096
         * (FUN_80053ca4). accum grows quadratically -> ease-in zoom 20039 -> ~11000 (~1.82x). */
        if (cz >= 3) cam.trans[2] = (int32_t)(((int64_t)4104 * (20000 - cz_accum)) >> 12);
        int cz_dim = (cz == 1) ? cz_c6 : (cz >= 2 ? 255 : 0);   /* sub4 ramp; then backdrop fully black */

        if (cz_pflag && cz_pcnt > 0) cz_pcnt--;      /* Counter-Dec pro Frame (@0x80100f44-50) */

        re15_render_begin_frame();
        re15_input_tick();
        re15_render_background_gradient(0, 0, 0, 0, 0, 0);
        re15_render_pc_player_select(&s_sel_bg, sel, pulse);
        {   /* FLAG/Counter + Pan-Arm-Namen-Disable an den Renderer spiegeln (pselect_info_bg.md §4) */
            extern void re15_render_pc_pselect_confirm(int flag, int counter, int hide_names);
            re15_render_pc_pselect_confirm(cz_pflag, cz_pcnt, cz >= 2);
        }
        re15_render_pc_pselect_text(&s_sel_txt, sel);   /* name/profile text overlays (groups B+C) */
        if (cz_dim) re15_render_pc_pselect_dim(cz_dim);
        if (cz >= 2) re15_render_pc_pselect_groupa(0);   /* stop the half-screen dim once the char pans to centre */
        if (cz >= 3) { int b0 = sel ? 2 : 0;             /* sub6: the sliding name+subtitle rows */
            re15_render_pc_pselect_slide(sel, blk_x[b0] >> 16, blk_y[b0] >> 16, blk_x[b0+1] >> 16, blk_y[b0+1] >> 16); }
        /* the live 3D models: the SELECTED char pans (pan_x/pan_ry); the unselected is drawn until sub4 disables it. */
        { const char *cv=getenv("RE15_PSELECT_CUR"); uint32_t acur = cv ? (uint32_t)atoi(cv) : frames;
          if (sel == 0 || elza_alive) pselect_render_model(&s_leon, pan_x[0], 2036, 0, (int16_t)pan_ry[0], acur, &cam);
          if (sel == 1 || elza_alive) pselect_render_model(&s_elza, pan_x[1], 2036, 0, (int16_t)pan_ry[1], acur, &cam); }
        { int br = fade_level >> 7; if (br > 255) br = 255; if (br < 0) br = 0;
          if (cz_black > br) br = cz_black;   /* sub7 fade-to-black wins over the initial fade-in */
          re15_render_pc_set_title_fade(br); }
        re15_render_end_frame();
        re15_audio_tick();
        { const char *af = getenv("RE15_PSELECT_SHOT_AF"); unsigned sf = af ? (unsigned)atoi(af) : 40;
          if (ps_shot && frames == sf) re15_render_pc_screenshot(ps_shot); }
        { const char *ser = getenv("RE15_PSELECT_SERIES");   /* frame-series dump for the video compare */
          if (ser) { unsigned lim = getenv("RE15_PSELECT_SERIES_N") ? (unsigned)atoi(getenv("RE15_PSELECT_SERIES_N")) : 60;
                     if (frames < lim) { char pb[512]; snprintf(pb, sizeof pb, "%s/f%03u.png", ser, frames);
                                         re15_render_pc_screenshot(pb); } else break; } }

        if (cz < 0) { fade_level -= 0x400; if (fade_level < 0) fade_level = 0; }   /* fade-in (idle only) */
        if (cz == 5 && cz_black >= 255) break;   /* confirm fade-to-black complete -> hand off to the game */

        uint16_t pp = g_engine.pad_pressed;
        if (auto_drive) { if (frames == 40 && getenv("RE15_PSELECT_AUTO_SWITCH")) pp |= RE15_PAD_BIT_RIGHT;  /* -> Elza */
                          if (frames == 70) pp |= RE15_PAD_BIT_CROSS; }    /* confirm (default = Leon) */
        if (cz < 0 && fade_level == 0) {   /* input active once faded in (sub1 idle) */
            if ((pp & (RE15_PAD_BIT_LEFT | RE15_PAD_BIT_RIGHT)) && pulse >= 0x80) { sel ^= 1; pulse = 0; }
            if (pp & (RE15_PAD_BIT_CROSS | RE15_PAD_BIT_SQUARE | RE15_PAD_BIT_TRIANGLE |
                      RE15_PAD_BIT_CIRCLE | RE15_PAD_BIT_START))
                cz = 0;   /* CONFIRM -> start the confirm-zoom FSM (sub3) */
        }
        frames++;
        /* 30fps cap (byte-true PSX 30Hz cadence) so the idle clip advances 1 frame/tick, not
         * uncapped -> the models animated far too fast without this (game loop caps @4431). */
        { uint32_t now = SDL_GetTicks(); uint32_t el = now - ps_last;
          if (el < 33) SDL_Delay(33 - el); ps_last = SDL_GetTicks(); }
    }
    re15_render_pc_set_title_fade(0);
    re15_render_pc_hide_player_select();
    return sel;
}

/* OPTIONS / controller-CONFIG screen (byte-true EXE task @0x8002dde4, RE workflow wf_ff4bebb2 +
 * RE_15_Quellcode_V2/FUN_8002dfb0). Entered from the title menu OPTION item (TITLE.BIN FUN_801025f0 →
 * FUN_80029a98(1,0x8002dde4)). This FOUNDATION renders the byte-true C_BACK2.TIM backdrop (the OPTIONS
 * title, the CONFIG/SOUND/EXIT tabs, the DualShock graphic + the label boxes are baked in), the
 * fade-in/out, and the tab cursor; the per-button action LABELS (game-font strings drawn by the
 * printf-style FUN_800279c8 at the DAT_80073d2c slot positions) and the functional A/B/C/EDIT remap
 * (tables @0x80073dbc/ddc/dfc verified from RAM) are the next increments. Returns when EXIT/Cancel. */
/* Controller-config preset (OPTIONS TYPE A/B/C), byte-true (workflow wf_df9c864a + FUN_80030444, tables
 * @0x80073dbc/ddc/dfc). The runtime remap BUILDS a 16-bit logical action word from the raw pad
 * (logical bit i set iff REMAP[type][i] & raw), which the game reads. The port's game readers already
 * encode TYPE A's physical layout, so we build the logical word then re-express it into TYPE A's
 * physical bits (out |= REMAP[A][i] for each set logical bit), keeping the non-config buttons
 * (dialog Circle, menu Start, …) as pass-through. TYPE A is therefore IDENTITY (no regression).
 *   A vs B: bits 6,7,14 (ACTION/ATTACK) SQUARE→CIRCLE — the attack button moves to Circle (NOT a turn swap).
 *   A vs C: + tank-turn onto the shoulders (i1 Right→R1, i3 Left→L1) + Aim/Ready R1→R2 (i8).
 * Tables are in the PORT's SIO/RE15_PAD_BIT order (the EXE's PsyQ-order hex is byte-swapped). INGAME only. */
static int s_config_type = 0;
static const uint16_t k_pad_remap[3][16] = {
    /* A */ { 0x0010,0x0020,0x0040,0x0080, 0x0010,0x0040, 0x8000,0x8000, 0x0800,0x4000,0x0800,0x0400, 0x0080,0x0020,0x8000,0x4000 },
    /* B */ { 0x0010,0x0020,0x0040,0x0080, 0x0010,0x0040, 0x2000,0x2000, 0x0800,0x4000,0x0800,0x0400, 0x0080,0x0020,0x2000,0x4000 },
    /* C */ { 0x0010,0x0800,0x0040,0x0400, 0x0010,0x0040, 0x2000,0x2000, 0x0200,0x4000,0x0800,0x0400, 0x0080,0x0020,0x2000,0x4000 },
};
static void pc_edit_build_remap(uint16_t remap[16]);   /* fwd — type 3 (EDIT) builds its remap from d08 */
static uint16_t pc_pad_config(uint16_t p)
{
    if (s_config_type <= 0) return p;   /* TYPE A = identity */
    const uint16_t *TA = k_pad_remap[0];
    const uint16_t *TT;
    uint16_t edit_remap[16];
    if (s_config_type <= 2) TT = k_pad_remap[s_config_type];
    else { pc_edit_build_remap(edit_remap); TT = edit_remap; }     /* TYPE 3 = EDIT custom map */
    uint16_t cfg = 0; for (int i = 0; i < 16; i++) cfg |= TA[i];   /* buttons the config owns */
    uint16_t out = (uint16_t)(p & ~cfg);                          /* non-config buttons pass through */
    for (int i = 0; i < 16; i++) if (TT[i] & p) out |= TA[i];     /* fire TYPE-A physical bit i when TYPE-T's button is held */
    return out;
}

/* ASCII -> TEX.TIM game-font glyph code (decoded from the font grid, shots/texfont_grid.png):
 * A-Z @0x1D, a-z @0x3D, 0-9 @0x0C (contiguous; 0x0b=down-arrow), space=0, + config-label punctuation. */
static int pc_font_code(char c)
{
    if (c >= 'A' && c <= 'Z') return 0x1D + (c - 'A');
    if (c >= 'a' && c <= 'z') return 0x3D + (c - 'a');
    if (c >= '0' && c <= '9') return 0x0C + (c - '0');
    switch (c) { case ' ': return 0x00; case '.': return 0x57; case ',': return 0x18;
                 case '/': return 0x38; case '(': return 0x37; case ')': return 0x39;
                 case '\'': return 0x3A; case '-': return 0x3B; case ':': return 0x16;
                 case ';': return 0x17; case '!': return 0x1A; case '?': return 0x1B; }
    return 0x00;
}

/* Live per-box action-code array (byte-true DAT_80073d08): the active preset A/B/C (copied in by the
 * picker) or the EDIT custom map. pc_config_draw_labels + the ingame remap read this. */
static unsigned char s_config_d08[16] = {0x07,0x07,0x00,0x03,0x02,0x01,0x07,0x06,0x07,0x04,0x07,0x05,0x0e,0x0f,0x15,0x17};
/* The 3 preset code tables (byte-true DAT_80073cd8, copied into d08 on TYPE A/B/C confirm). */
static const unsigned char s_config_preset[3][16] = {
    {0x07,0x07,0x00,0x03,0x02,0x01,0x07,0x06,0x07,0x04,0x07,0x05,0x0e,0x0f,0x15,0x17},  /* A */
    {0x07,0x07,0x00,0x03,0x02,0x01,0x07,0x06,0x07,0x07,0x04,0x05,0x0e,0x0f,0x16,0x17},  /* B */
    {0x07,0x03,0x00,0x07,0x07,0x01,0x06,0x02,0x07,0x07,0x04,0x05,0x0e,0x0f,0x16,0x17},  /* C */
};
/* SOUND setting: 1 = stereo, 0 = mono (byte-true scene S[7] / persisted DAT_800b0fce; default stereo). */
static int s_config_sound = 1;

/* the 16 grid-slot / action-label box positions (DAT_80073d2c) — shared by the labels + the EDIT slot cursor. */
static const struct { int x, y; } k_cfg_box[16] = {
    {31,52},{31,71},{46,91},{9,111},{84,111},{46,131},{217,52},{217,71},
    {203,91},{164,111},{239,111},{203,131},{137,182},{137,200},{281,182},{281,200} };

/* CONFIG action labels — byte-true (grid FUN_8002ffb8). Per box i, draw the string/glyph for
 * id = DAT_80073d08[i] (s_config_d08) at box position DAT_80073d2c[i] (+ per-id x-nudge, +1 y), rendered
 * with the game font (FUN_80028ec4) using CLUT 0x7810 (normal, attr 0) except id 7 = "Not set" which
 * uses CLUT 0x7990 (dim, attr 3). id<0x12 = CONFIG.TIM icon (msg-font approximation); id>=0x12 = font
 * string. Plus 4 fixed labels (Upper/Lower Attack, Confirm, Cancel). */
static void pc_config_draw_labels(void)
{
    extern int re15_render_pc_config_text(int x, int y, const unsigned char *codes, int len, int attr);
    static const char *STR[0x0c] = {
        "Forward", "Backward", "L. Turn", "R. Turn", "OK/Attack", "Run", "Aim", "Not set",
        "Upper Attack", "Lower Attack", "Confirm", "Cancel" };
    /* id 0x12-0x19 = button-legend symbols. The EXE draws them via FUN_800279c8's own symbol atlas;
     * mapped here to the TEX.TIM msg-font button glyphs (△0x06 ○0x07 ✕0x08 □0x09). The confirm/cancel
     * legend (ids 0x15/0x16 = □, 0x17 = ✕) is what actually shows for TYPE A/B/C. */
    static const unsigned char SPR[8] = { 0x06,0x07,0x06,0x09,0x09,0x08,0x07,0x08 };   /* id 0x12..0x19 */
    static const int NUDGE[0x12] = { 0x16,0x16,0x0f,0x0f,0x02,0x09,0x0f,0x0f, 0,0,0,0,0,0,0,0,0,0 };
    unsigned char buf[24];
    for (int i = 0; i < 16; i++) {
        int id = s_config_d08[i];
        int x = k_cfg_box[i].x + (id < 0x12 ? NUDGE[id] : 0), y = k_cfg_box[i].y + 1, n = 0;
        int attr = (id == 7) ? 3 : 0;   /* "Not set" -> dim CLUT 0x7990 (attr 3), else normal 0x7810 (attr 0) */
        if (id < 0x0c)       { for (const char *p = STR[id]; *p && n < 24; p++) buf[n++] = (unsigned char)pc_font_code(*p); }
        else if (id < 0x12)  { buf[n++] = (unsigned char)(0x98 - (id - 0x0e)); }   /* 0x0e→0x98 .. 0x11→0x95 */
        else if (id <= 0x19) { buf[n++] = SPR[id - 0x12]; }
        if (n) re15_render_pc_config_text(x, y, buf, n, attr);
    }
    static const struct { int x, y, id; } FIX[4] = { {30,183,8},{30,201,9},{174,183,10},{174,201,11} };
    for (int k = 0; k < 4; k++) {
        int n = 0; for (const char *p = STR[FIX[k].id]; *p && n < 24; p++) buf[n++] = (unsigned char)pc_font_code(*p);
        re15_render_pc_config_text(FIX[k].x, FIX[k].y, buf, n, 0);
    }
}

/* OPTIONS screen state machine — byte-true (EXE task @0x8002dde4). Screens:
 *   TOP    : CONFIG(0)/SOUND(1)/EXIT(2)            LEFT/RIGHT wrap 0..2      (FUN_8002e774)
 *   PICKER : TYPE A(0)/B(1)/C(2)/EDIT(3)/EXIT(4)   LEFT/RIGHT wrap 0..4      (FUN_8002e980)
 *   SOUND  : Stereo/Mono                           UP|DOWN toggles           (FUN_8002f0d4)
 *   EDIT   : 16-slot grid + panel A/B                                        (FUN_8002ebbc)
 * confirm = Square|Circle (raw 0xa0), cancel = Cross (raw 0x40) — RE1.5 config convention. Positions:
 * DAT_80073d6c (top) / DAT_80073d78 (picker), y=24; blue cursor TILE RGB(0,0,0x80) @50% ABE, 48x16
 * (top/EDIT/EXIT) or 16x16 (TYPE A/B/C). */
enum { CFG_TOP = 0, CFG_PICKER, CFG_SOUND, CFG_EDIT, CFG_AI };
/* PORT EXTENSION (no original): a 4th TOP entry "AI" that picks the zombie brain
 * (RE1.5 byte-true default vs the RE2 retail brain). The three byte-true entries keep their exact
 * DAT_80073d6c x positions and are NOT moved — the OPTIONS panel is pixel-verified against the PSX
 * and it ENDS at x=296 (EXIT is 248+48), so a 4th tab on that row would hang off the panel (it did:
 * shots/_aitab_top2.png). The extension therefore lives in the free strip BELOW the bottom box
 * (which ends at y=220), where it cannot disturb one byte-true pixel. */
static const int k_cfg_top_x[3]  = { 147, 198, 248 };            /* DAT_80073d6c.x (y=24) */
#define CFG_AI_X 14
#define CFG_AI_Y 225
/* 2026-08-23 (dritte Stufe "AI  MIXED"): die breiteste Beschriftung ist jetzt "AI  MIXED".
 * Breiten GEMESSEN aus der Glyphen-Vorschubtabelle, die der Renderer selbst benutzt
 * (BIN/DEBUG.BIN Datei-Offset 0x4416 == RAM 0x800c4416, render_pc.c:2300 — s_msgfont_w[code],
 * Code-Abbildung pc_font_code; Summe der Advance-Bytes je Glyph):
 *   "AI  RE1.5" = 56 px | "AI  RE2" = 46 px | "AI  MIXED" = 62 px
 * Text beginnt bei CFG_AI_X+5 = 19, also Kasten-Ende >= 19+62+5 = 86 -> Breite >= 72.
 * 96 REICHT und bleibt unveraendert (der byte-true OPTIONS-Kasten endet bei y=220, dieser
 * Streifen liegt darunter; x=14..110 bleibt weit innerhalb der 320er-Breite). */
#define CFG_AI_W 96
static const int k_cfg_pick_x[5] = { 147, 164, 181, 198, 248 }; /* DAT_80073d78.x (y=24) */

/* Der AI-Schalter hat DREI Zustaende (Menue-Index == re15_ai_flavor_t, 1:1):
 *   0 = RE1.5-Gehirn      + RE1.5-Modelle   ("AI  RE1.5")
 *   1 = RE2-Gehirn/-Anim  + RE1.5-Modelle   ("AI  RE2")     <- Hybrid-Rig, DEFAULT
 *   2 = RE1.5 ueberall AUSSER dem HUND 0x20 ("AI  MIXED")   <- 2026-08-23, Nutzer-Auftrag
 *
 * ⚠️ MIXED ist ein VOLLWERTIGES drittes Flavor-Enum-Glied, KEIN zweites orthogonales Flag.
 * Welle G hatte fuer die (spaeter entfernte) "RE2 MODELS"-Stufe bewusst ein Zweitflag gewaehlt,
 * weil damals 17 Stellen `== RE15_AI_FLAVOR_RE2` verglichen und einzeln haetten geprueft werden
 * muessen. Genau dieses Problem ist jetzt an der Wurzel geloest: es gibt keinen globalen
 * Flavor-Vergleich mehr, alle Stellen fragen re15_ai_re2_for_type(typ) (s. re15_ai_flavor.h).
 *
 * ⛔ ENTFERNT 2026-08-21: die alte dritte Stufe "AI  RE2 MODELS". Der Port faehrt unter dem
 * RE2-Flavor IMMER das Hybrid-Rig; das orthogonale re15_ai_models() liest main.c nicht mehr.
 * ROBUSTHEIT: pc_ai_mode() liefert nur 0/1/2 (unbekannter Flavor -> 0 = byte-true RE1.5),
 * pc_ai_mode_set() KLEMMT jeden Fremdwert auf [0,2]. */
static int pc_ai_mode(void)
{
    switch (re15_ai_flavor()) {
        case RE15_AI_FLAVOR_RE2:   return 1;
        case RE15_AI_FLAVOR_MIXED: return 2;
        default:                   return 0;
    }
}
static void pc_ai_mode_set(int m)
{
    if (m < 0) m = 0;
    if (m > 2) m = 2;                     /* Fremdwert -> hoechster gueltiger Index */
    re15_ai_flavor_set(m == 0 ? RE15_AI_FLAVOR_RE15
                     : m == 1 ? RE15_AI_FLAVOR_RE2
                              : RE15_AI_FLAVOR_MIXED);
}

/* EDIT custom-remap screen (FUN_8002ebbc). s_edit_phase: 0=slot-select, 1=panel A (basic action), 2=panel B
 * (special code). Slot-grid nav = byte-true jump-table transitions (0x80010a30/a70/aa8/ae8); panels + assign
 * dedup + remap rebuild all byte-true (workflow wf_abeec930). Panel GLYPHS are the game-font approximation
 * (action names / button symbols) — the exact CONFIG.TIM streams need a dynamic ligature-table dump. */
static int s_edit_phase = 0, s_edit_slot = 0, s_edit_code = 0;   /* scene S[3]/S[5]/S[6] */
static const uint8_t k_edit_up[16]    = {13,0,1,2,2,3,15,6,7,8,8,9,5,12,11,14};
static const uint8_t k_edit_down[16]  = {1,2,3,5,5,12,7,8,9,11,11,14,13,0,15,6};
static const uint8_t k_edit_right[16] = {6,7,8,10,3,11,0,1,2,4,9,5,14,15,12,13};
static const uint8_t k_edit_left[16]  = {6,7,8,4,9,11,0,1,2,10,3,5,14,15,12,13};
static const uint8_t k_editB_up[12]   = {5,0,1,2,3,4,11,6,7,8,9,10};
static const uint8_t k_editB_down[12] = {1,2,3,4,5,0,7,8,9,10,11,6};
static const uint8_t k_editB_lr[12]   = {6,7,8,9,10,11,0,1,2,3,4,5};
/* per-grid-slot physical mask (DAT_80073c88) + special-code masks (0x80073c84 + code*2, code 14..25),
 * both in the EXE's standard PSX-SIO order; converted to the port's RE15_PAD_BIT order at remap-build. */
static const uint16_t k_box_mask[16]     = {0x0001,0x0004,0x1000,0x8000,0x2000,0x4000,0x0002,0x0008,0x0010,0x0080,0x0020,0x0040,0x1000,0x4000,0x2000,0x8000};
static const uint16_t k_special_mask[12] = {0x1000,0x4000,0x2000,0x8000,0x0004,0x0001,0x0010,0x0080,0x0020,0x0040,0x0008,0x0002}; /* code 14..25 */

/* EDIT assign — panel A (basic codes 0..7, swap-dedup over grid slots 0..11; code 7 = "Not set", no
 * dedup / duplicates allowed). Byte-true FUN_8002f714. */
static void pc_edit_assign_a(void)
{
    int newcode = s_edit_code;                          /* 0..7 */
    if (newcode == 7) { s_config_d08[s_edit_slot] = 7; return; }
    int old = s_config_d08[s_edit_slot];
    for (int i = 11; i >= 0; --i) if (s_config_d08[i] == newcode) s_config_d08[i] = (unsigned char)old;
    s_config_d08[s_edit_slot] = (unsigned char)newcode;
}
/* EDIT assign — panel B (special code = cursor+14, swap-dedup over slots 12..15, first match). FUN_8002f7e8. */
static void pc_edit_assign_b(void)
{
    int newcode = s_edit_code + 14;                     /* 14..25 */
    int old = s_config_d08[s_edit_slot];
    for (int i = 12; i < 16; ++i) if (s_config_d08[i] == newcode) { s_config_d08[i] = (unsigned char)old; break; }
    s_config_d08[s_edit_slot] = (unsigned char)newcode;
}
/* Build the EDIT custom remap[16] (logical action -> physical port-pad mask) from s_config_d08, byte-true
 * rebuild loop @0x8002e5d0 (code->action LUT 0x80010a10). The box/special masks are the EXE's standard
 * PSX-SIO bits; byte-swapped to the port's RE15_PAD_BIT order (matches k_pad_remap exactly for A). The
 * fixed actions {10..13} are seeded from TYPE A (the byte-true rebuild leaves them 0 — they are engine
 * defaults not exposed by the editor). */
static void pc_edit_build_remap(uint16_t remap[16])
{
    for (int i = 0; i < 16; i++) remap[i] = k_pad_remap[0][i];   /* seed non-editable actions from TYPE A */
    for (int slot = 0; slot < 16; slot++) {
        int c = s_config_d08[slot];
        if (c >= 7) continue;
        uint16_t m = (uint16_t)(((k_box_mask[slot] & 0xFF) << 8) | (k_box_mask[slot] >> 8));
        switch (c) {
            case 0: remap[0] = m; break;
            case 1: remap[2] = m; break;
            case 2: remap[1] = m; break;
            case 3: remap[3] = m; break;
            case 4: remap[7] = m; remap[6] = m; break;
            case 5: remap[9] = m; break;
            case 6: remap[8] = m; break;
        }
    }
    static const int spdst[4] = { 4, 5, 14, 15 };
    for (int k = 0; k < 4; k++) {
        int code = s_config_d08[12 + k];
        if (code >= 14 && code <= 25) {
            uint16_t sm = k_special_mask[code - 14];
            remap[spdst[k]] = (uint16_t)(((sm & 0xFF) << 8) | (sm >> 8));
        }
    }
}

/* Draw the current sub-screen's CONFIG.TIM tiles + selected-item highlight + text over the backdrop
 * (called after re15_render_pc_config_clear + the backdrop + the 16 action labels). */
static void pc_config_draw_overlay(const re15_tim_t *tim, int screen, int cur)
{
    extern void re15_render_pc_config_rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    extern void re15_render_pc_config_tile(const re15_tim_t *t, int su, int sv, int w, int h, int dx, int dy);
    extern void re15_render_pc_config_tile_ov(const re15_tim_t *t, int su, int sv, int w, int h, int dx, int dy);
    extern void re15_render_pc_config_rect_ov(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    extern int  re15_render_pc_config_text(int x, int y, const unsigned char *codes, int len, int attr);
    /* PORT EXTENSION — the "AI  <flavor>" row in the free strip under the bottom box (y=220..240).
     * `lit` draws the same blue 50%-ABE cursor tile the byte-true tabs use, so it reads as one UI. */
    void pc_config_draw_ai_row(int lit) {
        if (lit) re15_render_pc_config_rect(CFG_AI_X, CFG_AI_Y, CFG_AI_W, 14, 0, 0, 0x80, 128);
        unsigned char b[20]; int n = 0;
        static const char *k_ai_top[3] = { "AI  RE1.5", "AI  RE2", "AI  MIXED" };
        int mode = pc_ai_mode(); if (mode < 0 || mode > 2) mode = 0;
        const char *s = k_ai_top[mode];
        for (const char *p = s; *p && n < 20; p++) b[n++] = (unsigned char)pc_font_code(*p);
        re15_render_pc_config_text(CFG_AI_X + 5, CFG_AI_Y + 2, b, n, 0);
    }
    if (screen == CFG_TOP) {
        pc_config_draw_ai_row(cur == 3);          /* PORT EXTENSION, below the byte-true panel */
        if (cur < 3)
            re15_render_pc_config_rect(k_cfg_top_x[cur], 24, 48, 16, 0, 0, 0x80, 128); /* blue tab cursor @50% */
    } else if (screen == CFG_AI) {
        /* PORT EXTENSION — AI picker, laid out like the SOUND screen (same CONFIG.TIM label
         * boxes at uv 0,232 88x18), with the row-below marker kept lit so it is obvious where
         * this screen was entered from. 2026-08-23: DRITTE Zeile "MIXED" bei y=89 — exakt das
         * 19-px-Raster (51/70/89) und exakt die Koordinaten, die die (entfernte) WELLE-G-Zeile
         * schon benutzt hat (23c20b0f), also kein neu geratenes Layout. Beschriftungen GEMESSEN
         * gegen die Vorschubtabelle BIN/DEBUG.BIN@0x4416 (render_pc.c:2300):
         * "RE1.5" 34 px | "RE2" 24 px | "MIXED" 40 px. Kasten x=116..204, Text ab x=124 ->
         * die laengste Zeile endet bei 164, bleibt in der 88-px-Kachel. */
        static const struct { const char *s; int tile_y, text_y; } k_ai_rows[3] = {
            { "RE1.5", 51, 53 }, { "RE2", 70, 71 },
            { "MIXED", 89, 90 },                      /* Raster 19 px, wie 23c20b0f */
        };
        for (int r = 0; r < 3; r++)
            re15_render_pc_config_tile(tim, 0, 232, 88, 18, 116, k_ai_rows[r].tile_y);
        pc_config_draw_ai_row(1);
        for (int r = 0; r < 3; r++) {
            unsigned char b[16]; int n = 0;
            for (const char *p = k_ai_rows[r].s; *p && n < 16; p++) b[n++] = (unsigned char)pc_font_code(*p);
            re15_render_pc_config_text(124, k_ai_rows[r].text_y, b, n, (pc_ai_mode() == r) ? 0 : 3);
        }
    } else if (screen == CFG_PICKER) {
        /* opaque CONFIG.TIM tiles cover the baked CONFIG/SOUND: "TYPE A B C" then "EDIT" ("EXIT" stays baked). */
        re15_render_pc_config_tile(tim, 0, 192, 50, 24, 147, 16);
        re15_render_pc_config_tile(tim, 0, 216, 48, 16, 198, 24);
        re15_render_pc_config_rect(k_cfg_pick_x[cur], 24, (cur < 3) ? 16 : 48, 16, 0, 0, 0x80, 128);
    } else if (screen == CFG_SOUND) {
        /* SOUND label boxes (CONFIG.TIM uv 0,232 88x18) + STEREO/MONO text; current setting -> dim CLUT
         * 0x7990 (attr 3), other -> normal 0x7810 (attr 0). Tab cursor stays on SOUND (198). */
        re15_render_pc_config_tile(tim, 0, 232, 88, 18, 116, 51);
        re15_render_pc_config_tile(tim, 0, 232, 88, 18, 116, 70);
        re15_render_pc_config_rect(k_cfg_top_x[1], 24, 48, 16, 0, 0, 0x80, 128);
        /* Current setting -> BRIGHT (attr 0 / CLUT 0x7810), the other -> DIM (attr 3 / 0x7990). Verified
         * against the PSX SOUND screen (STEREO current = bright 177, MONO = dim 132). */
        { unsigned char b[8]; int n = 0; const char *s = "STEREO"; for (const char *p = s; *p; p++) b[n++] = (unsigned char)pc_font_code(*p);
          re15_render_pc_config_text(119, 53, b, n, s_config_sound ? 0 : 3); }
        { unsigned char b[8]; int n = 0; const char *s = "MONO";   for (const char *p = s; *p; p++) b[n++] = (unsigned char)pc_font_code(*p);
          re15_render_pc_config_text(131, 71, b, n, s_config_sound ? 3 : 0); }
    } else if (screen == CFG_EDIT) {
        /* EDIT is a sub-state of the KEY/picker screen, so the picker bar stays visible with EDIT
         * highlighted (the "TYPE A B C" + "EDIT" tiles over the baked tabs, blue cursor on EDIT). */
        re15_render_pc_config_tile(tim, 0, 192, 50, 24, 147, 16);
        re15_render_pc_config_tile(tim, 0, 216, 48, 16, 198, 24);
        re15_render_pc_config_rect(k_cfg_pick_x[3], 24, 48, 16, 0, 0, 0x80, 128);
        /* slot-select YELLOW cursor (0xc0c000 @50%) at the selected grid box: w=72 for the 12 basic slots,
         * 16 for the 4 special slots (FUN_8002f518). */
        re15_render_pc_config_rect(k_cfg_box[s_edit_slot].x, k_cfg_box[s_edit_slot].y,
                                   (s_edit_slot < 12) ? 72 : 16, 16, 0xc0, 0xc0, 0x00, 128);
        if (s_edit_phase == 1) {
            /* Panel A = the "ACT" box (CONFIG.TIM uv 0,48) with 8 empty rows + a BLUE row cursor (this
             * prototype does not render row glyphs — the action is the row position). The panel sits on the
             * side opposite the selected box: right (s2=187) for slots 0..5, left (37) for 6..11. Byte-true
             * cursor = flat blue TILE 72x16 @ (s2+12, code*16+54). */
            int s2 = (s_edit_slot < 6) ? 0xbb : 0x25;
            /* ACT box = 2 CONFIG.TIM tiles (byte-true, scene prim scan mzd_edit_panelA.sav): top uv(0,48)
             * 96x48 @(s2,35) = "ACT" header + first rows; bottom uv(0,80) 96x112 @(s2,83) = lower rows. */
            re15_render_pc_config_tile_ov(tim, 0, 48, 96, 48, s2, 35);
            re15_render_pc_config_tile_ov(tim, 0, 80, 96, 112, s2, 83);
            /* row cursor = the BRIGHT-blue TILE RGB(0,0,0xc0) @50% (SETUP tile 3), 72x16 @ (s2+12,code*16+54). */
            re15_render_pc_config_rect_ov(s2 + 12, s_edit_code * 16 + 54, 72, 16, 0, 0, 0xc0, 128);
        } else if (s_edit_phase == 2) {
            /* Panel B — the "KEY" box (CONFIG.TIM uv 88,0) + 8 assignable special-button glyphs at the
             * byte-true screen positions (prim-buffer scan of FUN_8002fb94's SPRTs: all clut 0x7810, 16x16,
             * V=32, U=48..160 = msg-font glyph codes 3..10 = L2/R2/L1/R1/△/○/✕/□). + BLUE cursor. */
            static const struct { int x, y; unsigned char g; } BICON[8] = {
                {216, 55, 7}, {216, 71,10}, {216, 87, 8}, {216,103, 9},   /* △ □ ○ ✕ */
                {216,119, 6}, {216,135, 4},                                 /* R1 R2 */
                {200,119, 5}, {200,135, 3} };                               /* L1 L2 */
            /* KEY box = 2 CONFIG.TIM SPRTs (byte-true, scene prim scan @0x80190148): left half
             * uv(96,0) 32x128 @(187,35) = "KE" + left cell column; right half uv(96,128) 24x128 @(219,35)
             * = "Y" + right cell column. */
            re15_render_pc_config_tile_ov(tim, 96,   0, 32, 128, 187, 35);
            re15_render_pc_config_tile_ov(tim, 96, 128, 24, 128, 219, 35);
            for (int i = 0; i < 8; i++)
                re15_render_pc_config_text(BICON[i].x, BICON[i].y, &BICON[i].g, 1, 0);
            /* Byte-true extra glyphs from the overlay prim buffer (mzd_edit_panelB.sav, 0x800c7000 code-0x7c
             * SPRTs, uv->msg-font code): the "R1 O8!?KQW" debug string overlaid on the two "Not set" boxes
             * (rows y55/y87) + the header letters D@(200,87) / E@(200,119). Glyphs: R1=0x06, O=0x0c, 8=0x14,
             * !?=0x1c, K=0x27, Q=0x2d, W=0x33, D=0x20, E=0x21. */
            static const struct { short x, y; unsigned char g; } PBX[] = {
                {219,55,0x06},{232,55,0x0c},{244,55,0x14},{256,55,0x1c},{268,55,0x27},{280,55,0x2d},{292,55,0x33},{306,55,0x3b},{317,55,0x44},{329,55,0x4d},
                {200,87,0x20},{219,87,0x06},{232,87,0x0c},{244,87,0x14},{256,87,0x1c},{268,87,0x27},{280,87,0x2d},{292,87,0x33},{306,87,0x3b},{317,87,0x44},{329,87,0x4d},
                {200,119,0x21} };
            for (int i = 0; i < (int)(sizeof(PBX)/sizeof(PBX[0])); i++)
                re15_render_pc_config_text(PBX[i].x, PBX[i].y, &PBX[i].g, 1, 0);
            /* cell cursor = BRIGHT-blue TILE RGB(0,0,0xc0) @50% (SETUP tile 3), 16x16 @ (199/215, code*16+54). */
            int bx = (s_edit_code < 6) ? 199 : 215, by = ((s_edit_code < 6) ? s_edit_code : (s_edit_code - 6)) * 16 + 54;
            re15_render_pc_config_rect_ov(bx, by, 16, 16, 0, 0, 0xc0, 128);
        }
    }
}

static void pc_run_config(void)
{
    extern void re15_render_pc_config(const re15_tim_t *bg, int tab);
    extern void re15_render_pc_hide_config(void);
    extern void re15_render_pc_hide_title(void);
    extern void re15_render_pc_hide_title_menu(void);
    extern void re15_render_pc_set_title_fade(int a);
    extern void re15_render_pc_config_clear(void);
    extern void re15_render_pc_config_rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    extern void re15_render_pc_config_tile(const re15_tim_t *t, int su, int sv, int w, int h, int dx, int dy);
    extern int  re15_render_pc_config_text(int x, int y, const unsigned char *codes, int len, int attr);
    extern void re15_audio_set_mono(int mono);
    re15_render_pc_hide_title_menu();
    re15_render_pc_hide_title();
    static re15_tim_t s_cfg_bg = {0};      /* C_BACK2.TIM backdrop (OPTIONS title + tabs + controller) */
    if (!s_cfg_bg.pixels) { int sz = 0; uint8_t *b = pc_read_shared("DATA/C_BACK2.TIM", &sz);
                            if (b) re15_tim_parse(b, sz, &s_cfg_bg); }
    static re15_tim_t s_cfg_tim = {0};     /* CONFIG.TIM 8bpp icon/tile sheet (picker + SOUND tiles) */
    if (!s_cfg_tim.pixels) { int sz = 0; uint8_t *b = pc_read_shared("DATA/CONFIG.TIM", &sz);
                             if (b) re15_tim_parse(b, sz, &s_cfg_tim); }

    /* keep the live label array in sync with the active preset A/B/C on entry (EDIT keeps its custom map). */
    if (s_config_type >= 0 && s_config_type <= 2) memcpy(s_config_d08, s_config_preset[s_config_type], 16);

    int screen = CFG_TOP, cur = 0;
    /* debug: start the INTERACTIVE options screen on a given tab, so a gdigrab capture of the REAL
     * window can inspect a screen without depending on synthetic key injection (SDL does not see
     * SendKeys). Same numbering as the RE15_CONFIG_SHOT path, plus 8 = the AI tab. */
    { const char *pt = getenv("RE15_CONFIG_TAB");
      if (pt && getenv("RE15_CONFIG_TEST")) {
          int t = atoi(pt);
          if (t <= 2)      { screen = CFG_TOP;    cur = t; }
          else if (t == 3) { screen = CFG_PICKER; cur = 0; }
          else if (t == 4) { screen = CFG_SOUND;  }
          else if (t == 8) { screen = CFG_AI;     cur = 3; }
      } }
    int fade_level = 0x7fff;   /* brightness = fade_level>>7; fade IN from black */
    int fading_out = 0;
    uint32_t last = SDL_GetTicks();
    while (re15_gameflow_mode() == RE15_MODE_TITLE) {
        re15_render_begin_frame();
        re15_input_tick();
        re15_render_background_gradient(0, 0, 0, 0, 0, 0);
        re15_render_pc_config_clear();
        re15_render_pc_config(&s_cfg_bg, 0);   /* backdrop */
        pc_config_draw_labels();               /* 16 action labels (+ "Not set" dim) */
        pc_config_draw_overlay(&s_cfg_tim, screen, cur);

        { int br = fade_level >> 7; re15_render_pc_set_title_fade(br > 255 ? 255 : (br < 0 ? 0 : br)); }
        re15_render_end_frame();
        re15_audio_tick();

        if (!fading_out) { fade_level -= 0x400; if (fade_level < 0) fade_level = 0; }
        else { fade_level += 0x400; if (fade_level >= 0x7fff) break; }

        if (!fading_out && fade_level == 0) {
            uint16_t pp = g_engine.pad_pressed;
            int confirm = (pp & (RE15_PAD_BIT_SQUARE | RE15_PAD_BIT_CIRCLE)) != 0;   /* raw 0xa0 */
            int cancel  = (pp & RE15_PAD_BIT_CROSS) != 0;                            /* raw 0x40 */
            /* MENUE-TOENE im OPTIONS-Schirm (Nutzer-Auftrag 2026-08-31: "in den
             * Optionen, wenn wir uns bewegen, fehlt der Menu Bewegungssound ... Wenn
             * wir was auswaehlen ist das ebenfalls der gleiche Sound wie im Player
             * Inventory. Brechen wir eine Aktion ab, ist es der Abbruch Sound").
             * Es sind die originalen RE1.5-Menuetoene aus SE-Bank 4 (CORE00):
             * 4 = Cursor, 5 = Abbruch, 6 = Auswahl. Die STELLEN sind ergaenzt —
             * RE1.5 spielt im Config-Schirm selbst keine (EXE-weiter Scan: 41
             * Se_on-Aufrufe, keiner davon hier). */
            {
                extern void re15_audio_core_se(int se_id);
                if (pp & (RE15_PAD_BIT_LEFT | RE15_PAD_BIT_RIGHT |
                          RE15_PAD_BIT_UP   | RE15_PAD_BIT_DOWN)) re15_audio_core_se(4);
                else if (confirm)                                 re15_audio_core_se(6);
                else if (cancel)                                  re15_audio_core_se(5);
            }
            if (screen == CFG_TOP) {
                if (pp & RE15_PAD_BIT_LEFT)  { if (--cur < 0) cur = 3; }
                if (pp & RE15_PAD_BIT_RIGHT) { if (++cur > 3) cur = 0; }
                if (confirm) {
                    if (cur == 0)      { screen = CFG_PICKER; cur = 0; }   /* CONFIG -> preset picker (cursor 0) */
                    else if (cur == 1) { screen = CFG_SOUND; }             /* SOUND  -> sound screen */
                    else if (cur == 2) { fading_out = 1; }                 /* EXIT   -> leave config */
                    else               { screen = CFG_AI; }                /* AI (port extension) */
                }
            } else if (screen == CFG_PICKER) {
                if (pp & RE15_PAD_BIT_LEFT)  { if (--cur < 0) cur = 4; }
                if (pp & RE15_PAD_BIT_RIGHT) { if (++cur > 4) cur = 0; }
                if (cancel) { screen = CFG_TOP; cur = 0; }                 /* Cross -> back to top (CONFIG) */
                else if (confirm) {
                    if (cur <= 2)      { s_config_type = cur; memcpy(s_config_d08, s_config_preset[cur], 16); }  /* apply preset + relabel */
                    else if (cur == 3) { screen = CFG_EDIT; s_edit_phase = 0; s_edit_slot = 0; s_config_type = 3; }  /* EDIT custom */
                    else               { screen = CFG_TOP; cur = 0; }      /* EXIT -> back to top */
                }
            } else if (screen == CFG_AI) {
                /* PORT EXTENSION: pick the AI mode. 2026-08-23: wieder DREI Zustaende (RE1.5 /
                 * RE2 / MIXED), also wieder ein ZYKLUS statt des Toggles — DOWN = +1, UP = -1
                 * (== +2) modulo 3, genau wie in WELLE G (23c20b0f). Der `^1`-Toggle haette die
                 * dritte Zeile unerreichbar gemacht. */
                if (pp & RE15_PAD_BIT_DOWN)    pc_ai_mode_set((pc_ai_mode() + 1) % 3);
                else if (pp & RE15_PAD_BIT_UP) pc_ai_mode_set((pc_ai_mode() + 2) % 3);
                if (confirm || cancel) { screen = CFG_TOP; cur = 3; }
            } else if (screen == CFG_SOUND) {
                if (pp & (RE15_PAD_BIT_UP | RE15_PAD_BIT_DOWN)) s_config_sound ^= 1;   /* raw 0x5000 toggle */
                if (confirm)     { re15_audio_set_mono(!s_config_sound); screen = CFG_TOP; cur = 1; }  /* save+apply */
                else if (cancel) { screen = CFG_TOP; cur = 1; }           /* back to top (SOUND), no save */
            } else if (screen == CFG_EDIT) {
                if (s_edit_phase == 0) {   /* slot-select: byte-true grid-jump LUTs, confirm -> panel, cancel -> picker */
                    if (pp & RE15_PAD_BIT_UP)    s_edit_slot = k_edit_up[s_edit_slot];
                    if (pp & RE15_PAD_BIT_DOWN)  s_edit_slot = k_edit_down[s_edit_slot];
                    if (pp & RE15_PAD_BIT_LEFT)  s_edit_slot = k_edit_left[s_edit_slot];
                    if (pp & RE15_PAD_BIT_RIGHT) s_edit_slot = k_edit_right[s_edit_slot];
                    if (confirm) { s_edit_code = 0; s_edit_phase = (s_edit_slot < 12) ? 1 : 2; }
                    else if (cancel) { screen = CFG_PICKER; cur = 3; }
                } else if (s_edit_phase == 1) {   /* panel A: 8 basic actions, UP/DOWN wrap [0,7] */
                    if (pp & RE15_PAD_BIT_UP)   s_edit_code = (s_edit_code + 7) & 7;
                    if (pp & RE15_PAD_BIT_DOWN) s_edit_code = (s_edit_code + 1) & 7;
                    if (confirm) { pc_edit_assign_a(); s_edit_phase = 0; }
                    else if (cancel) { s_edit_phase = 0; }
                } else {   /* panel B: 12 special codes, 2col x 6row */
                    if (pp & RE15_PAD_BIT_UP)    s_edit_code = k_editB_up[s_edit_code];
                    if (pp & RE15_PAD_BIT_DOWN)  s_edit_code = k_editB_down[s_edit_code];
                    if (pp & (RE15_PAD_BIT_LEFT | RE15_PAD_BIT_RIGHT)) s_edit_code = k_editB_lr[s_edit_code];
                    if (confirm) { pc_edit_assign_b(); s_edit_phase = 0; }
                    else if (cancel) { s_edit_phase = 0; }
                }
            }
        }
        { uint32_t now = SDL_GetTicks(); uint32_t el = now - last; if (el < 33) SDL_Delay(33 - el); last = SDL_GetTicks(); }
    }
    re15_render_pc_set_title_fade(0);
    re15_render_pc_hide_config();
}

int main(int argc, char *argv[])
{
    (void) argc; (void) argv;

#ifdef _WIN32
    /* GUI-SUBSYSTEM-BEGLEITER (2026-08-17, siehe platform/pc/CMakeLists.txt):
     * Die exe ist jetzt eine GUI-Anwendung (Subsystem 2), weil der Loader beim
     * ANLEGEN einer Konsole fuer das alte CUI-Subsystem haengen blieb — vor main(),
     * ohne Fenster und ohne debug.log (gdb-Stack: LdrInitializeThunk -> LdrLoadDll
     * -> KernelBase!AttachConsole -> ntdll!ZwCreateFile).
     * GUI-Prozesse bekommen keine Konsole; damit Shell-Laeufe und CTest ihre
     * stdout-Ausgabe (u.a. der --headless-JSON-Status) weiterhin sehen, haengen wir
     * uns an eine BEREITS EXISTIERENDE Eltern-Konsole an. Das ist der harmlose Fall:
     * schlaegt fehl (0), wenn keine da ist — es wird KEINE erzeugt, also auch nicht
     * der Loader-Pfad betreten, der den Haenger ausloeste. */
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        setvbuf(stdout, NULL, _IONBF, 0);
    }
#endif

    /* Phase 4.5.10-J: redirect stderr to debug.log so user can read
     * exact numerical state. */
    freopen("debug.log", "w", stderr);
    setvbuf(stderr, NULL, _IONBF, 0);   /* unbuffered for live tail */

    /* Erste Zeile im Log: WO die Assets herkommen. Genau diese Auskunft fehlte beim
     * 0.3.19-Report — das Paket suchte still an einem Pfad, den es nicht gibt. */
    re15_pc_asset_roots_report();

    /* PAKET-GATE (nur Diagnose, 2026-08-24): RE15_ASSET_SELFTEST=1 prueft, ob DIESE exe aus
     * DIESEM Arbeitsverzeichnis ihre kritischen Assets findet, meldet zu jedem Treffer den
     * gewinnenden Vollpfad und beendet sich (0 = alles da, 2 = etwas fehlt). Steht hier, VOR
     * render/input/audio: kein Fenster, kein Ton, ~50 ms. Ohne die Variable wirkungslos.
     * release/make_package.sh fuehrt das gegen den fertigen Paketordner aus — genau der Test,
     * der beim 0.3.19-Report gefehlt hat (das Gate prueft sonst nur, ob Dateien DALIEGEN). */
    re15_pc_asset_selftest_maybe_run();

    re15_render_init();
    re15_input_init();
    re15_audio_init();   /* FE-3: open the audio device early so the opening movie has sound
                          * (idempotent — the later call in the game-init path is a no-op). */

    /* FE-0.3 / FE-1.3 — top-level mode machine: immer Boot zum TITLE.
     * Der RE15_START_ROOM-Schnellweg ist entfernt (siehe re15_gameflow_init). */
    re15_gameflow_init();
    /* FE-3 — the CAPCOM.STR opening (re15_str.c video + re15_xa.c audio, byte-true). It is the WHOLE
     * boot intro: the "This game contains scenes of explicit violence and gore." disclaimer over a
     * lab scene (~frame 80-300), then the CAPCOM logo (~frame 320-500) — CAPTURE-VERIFIED against the
     * real MZD disc (shots/boot_seq.png = BIOS -> disclaimer -> CAPCOM -> title; shots/capcom_frames.png
     * = the decoded movie). The EXE main-loop plays it via gate DAT_800aca38 bit 0x8000 @main.c:20
     * (one-shot, cleared @0x80020c48). My earlier "dormant" was for the un-modded prototype EXE where
     * nothing sets that bit — but the MZD build the port targets sets it on boot and DOES play the
     * intro. So it plays on every title boot (default ON); RE15_NO_INTRO skips it for quick iteration,
     * RE15_FMV_SHOT="<frame>:<path.bmp>" dumps one decoded frame then exits. */
    g_gameflow.boot_movie = getenv("RE15_NO_INTRO") ? 0 : 1;
    if (re15_gameflow_mode() == RE15_MODE_TITLE && g_gameflow.boot_movie) {
        g_gameflow.boot_movie = 0;                                    /* one-shot clear (byte-true) */
        extern void re15_render_pc_show_fmv(const uint32_t *rgba, int w, int h);
        extern void re15_render_pc_hide_fmv(void);
        extern void re15_render_pc_screenshot(const char *path);
        int msz = 0;
        uint8_t *mbuf = pc_read_shared("MOVIE/CAPCOM.STR", &msz);
        re15_str_t movie = {0};
        if (mbuf && re15_str_open(&movie, mbuf, (size_t) msz) == 0) {
            uint32_t *frame_rgba = (uint32_t *) malloc((size_t) movie.width * movie.height * 4);
            extern void   re15_fmv_audio_start(const int16_t *src, int src_frames, int src_rate, int cd_vol);
            extern void   re15_fmv_audio_stop(void);
            extern double re15_fmv_audio_time(void);
            const char *fmv_shot = getenv("RE15_FMV_SHOT");
            int  shot_frame = -1; char shot_path[256] = {0};
            if (fmv_shot) sscanf(fmv_shot, "%d:%255s", &shot_frame, shot_path);

            /* Decode the movie's CD-XA soundtrack up-front (the whole STR is in RAM) and start it.
             * The PSX has the CD controller auto-decode XA into the SPU CD-input at CD_initvol
             * (0x3FFF); the port software-decodes (re15_xa.c) and mixes at the same gate. */
            int16_t *apcm = NULL; int aframes = 0, arate = 0, achans = 0;
            int have_audio = 0;
            if (!fmv_shot && re15_xa_decode_movie(mbuf, (size_t) msz, &apcm, &aframes, &arate, &achans) == 0) {
                /* byte-true movie CD->SPU gain = 100/128 (CdlATV {0x64,0,0x64,0}, FUN_8002acac
                 * stereo branch fed by FUN_8002ac84(100)); the fades are instant gates. */
                re15_fmv_audio_start(apcm, aframes, arate, 100);
                /* Audio is the master clock ONLY when the SDL audio device is actually running; with
                 * RE15_NOAUDIO (or a session with no audio endpoint) the mixer callback never fires, so
                 * the clock would stay 0 and the frame-wait below would hang forever — fall back to the
                 * fixed 30fps timer in that case. */
                have_audio = g_audio.initialized && (re15_fmv_audio_time() >= 0.0);
            }

            /* Cadence (byte-true): on PSX the CD streams audio continuously and video frames
             * present as they arrive (FUN_8002a630) — AUDIO is the master clock. So we present
             * video frame f when the audio playback position reaches f/30 s (the authored ~30fps:
             * 525 frames / 17.55s = 29.92fps). If audio is unavailable, fall back to a fixed 30fps
             * SDL_Delay timer (same as the main loop). Skip = raw START (FUN_80029cd8:24, bit 0x800). */
            const uint32_t fmv_budget_ms = 1000u / 30u;
            uint32_t fmv_last = 0;
            int skipped = 0;
            for (int f = 0; f < movie.num_frames && frame_rgba && !skipped; f++) {
                if (re15_str_decode(&movie, f, frame_rgba) != 0) continue;
                if (have_audio) {
                    /* wait until the audio clock reaches this frame (audio = master) */
                    double frame_time = (double) f / 30.0;
                    for (;;) {
                        double t = re15_fmv_audio_time();
                        if (t < 0.0 || t >= frame_time) break;   /* audio ended, or reached frame */
                        if (g_engine.pad_pressed & RE15_PAD_BIT_START) { skipped = 1; break; }
                        re15_input_tick();
                        SDL_Delay(2);
                    }
                    if (skipped) break;
                }
                re15_render_begin_frame();
                re15_input_tick();
                re15_render_pc_show_fmv(frame_rgba, movie.width, movie.height);
                re15_render_end_frame();
                re15_audio_tick();
                if (fmv_shot && f == shot_frame) {
                    re15_render_pc_screenshot(shot_path);
                    exit(0);                                /* one-shot verification probe */
                }
                if (g_engine.pad_pressed & RE15_PAD_BIT_START) skipped = 1;
                if (have_audio) {
                    if (re15_fmv_audio_time() < 0.0) break;  /* audio finished -> end the movie */
                } else {
                    uint32_t now = SDL_GetTicks();           /* no audio: fixed 30fps timer */
                    if (fmv_last != 0 && now - fmv_last < fmv_budget_ms)
                        SDL_Delay(fmv_budget_ms - (now - fmv_last));
                    fmv_last = SDL_GetTicks();
                }
            }
            re15_fmv_audio_stop();
            if (apcm) free(apcm);
            free(frame_rgba);
            re15_render_pc_hide_fmv();
        }
        free(mbuf);
    }

    /* FE-5.3 mode-cycle: on death the game loop sets mode=TITLE and exits; control jumps back
     * HERE (after boot/FMV, before the title menu) so the REAL title menu runs — its CONTINUE
     * reloads a card save via the FE-4.3 path, NEW GAME restarts. Reuses byte-true code rather
     * than the old inline "reload-the-current-room" death stand-in. */
re_title:;
    if (re15_gameflow_mode() == RE15_MODE_TITLE) {
        /* BOOT TITLE (FE-1.3): DATA/TITLEU.TIM + blinking PRESS START, until START = NEW GAME.
         * NEW GAME falls through to the normal boot of the intro (ROOM1240 = 0x1240 = start_room). */
        extern void re15_render_pc_show_title(const re15_tim_t *tim);
        extern void re15_render_pc_hide_title(void);
        extern void re15_render_pc_screenshot(const char *path);
        extern void re15_render_pc_title_menu(const re15_tim_t *tmoji, int cursor);   /* byte-true TMOJI sprites */
        extern void re15_render_pc_hide_title_menu(void);
        extern void re15_render_pc_set_title_fade(int a);   /* front-end fade (menu->player-select) */
        re15_tim_t s_boot_title = {0}, s_tmoji = {0};
        { int tsz = 0; uint8_t *tb = pc_read_shared("DATA/TITLEU.TIM", &tsz);
          if (tb) re15_tim_parse(tb, tsz, &s_boot_title); }
        { int msz = 0; uint8_t *mb = pc_read_shared("DATA/TMOJI.TIM", &msz);   /* the menu-text sprite sheet */
          if (mb) re15_tim_parse(mb, msz, &s_tmoji); }
        const char *t_shot = getenv("RE15_TITLE_SHOT");   /* debug: dump the title/menu frame + auto-advance */
        unsigned tblink = 0;
        int cursor = 0;   /* 0=NEW GAME 1=LOAD GAME 2=OPTION — byte-true 3-item menu (TITLE.BIN FUN_80102b00) */
        /* Title-Init laedt die CORE-SE-Bank 0x11 (FUN_800440c4(0x11) @0x80102704-08 in TITLE.BIN):
         * CORE11.EDH/.VB (Datei-ids 195/196) traegt den "Biohazard 2!"-Announcer als SE 0
         * (Stereo-Paar VAG5 L / VAG6 R, 44100 Hz) + den Cursor-Blip SE 4 (VAG2, 22050 Hz).
         * analysis/title_fade_voice.md §2.3. */
        { extern void re15_audio_prime_core(int idx); re15_audio_prime_core(0x11); }
        if (getenv("RE15_PSELECT_TEST")) {   /* debug: jump straight into the LEON/ELZA player-select */
            int ch = pc_run_player_select();
            fprintf(stderr, "[pselect] returned character %d (%s)\n", ch, ch ? "Elza" : "Leon");
            exit(0);
        }
        if (getenv("RE15_CONFIG_TEST")) {    /* debug: jump straight into the OPTIONS/config screen */
            const char *cs = getenv("RE15_CONFIG_SHOT");
            if (cs) { extern void re15_render_pc_config(const re15_tim_t *bg, int tab);
                      extern void re15_render_pc_config_clear(void);
                      extern void re15_render_pc_screenshot(const char *path);
                      static re15_tim_t bg = {0}; int sz=0; uint8_t *b = pc_read_shared("DATA/C_BACK2.TIM",&sz);
                      if (b) re15_tim_parse(b,sz,&bg);
                      static re15_tim_t tim = {0}; int tsz=0; uint8_t *tb = pc_read_shared("DATA/CONFIG.TIM",&tsz);
                      if (tb) re15_tim_parse(tb,tsz,&tim);
                      (void)sz; (void)tsz;
                      /* RE15_CONFIG_TAB selects the screen for the shot: 0/1/2 = top menu cursor CONFIG/SOUND/EXIT,
                       * 3 = preset picker, 4 = SOUND, 5 = EDIT slot-select, 6 = EDIT panel A, 7 = EDIT panel B.
                       * RE15_CONFIG_CUR = picker cursor / EDIT slot; RE15_CONFIG_CODE = EDIT panel cursor. */
                      int t = atoi(getenv("RE15_CONFIG_TAB")?getenv("RE15_CONFIG_TAB"):"0");
                      /* 8 = der AI-Screen (wie im interaktiven pc_run_config oben);
                       * RE15_CONFIG_AI=0|1|2 waehlt den Zustand fuer den Shot vor, damit die
                       * Beschriftungs-BREITE aller Zustaende am gerenderten Bild und nicht nach
                       * Gefuehl geprueft werden kann. Das ist der MENUE-INDEX (== re15_ai_flavor_t):
                       * 0 RE1.5 / 1 RE2 / 2 MIXED. ⚠️ Die 2 hiess bis WELLE G "RE2 MODELS" — diese
                       * Stufe ist seit 2026-08-21 weg und hatte ausserhalb dieses Screenshot-
                       * Harness keinen Konsumenten, die Zahl ist damit frei neu belegt.
                       * pc_ai_mode_set KLEMMT alles ausserhalb [0,2]. */
                      if (t == 8) { const char *ai = getenv("RE15_CONFIG_AI");
                                    pc_ai_mode_set(ai ? atoi(ai) : 0); }
                      int screen = (t == 8) ? CFG_AI
                                 : (t <= 2) ? CFG_TOP
                                 : (t == 3) ? CFG_PICKER : (t == 4 ? CFG_SOUND : CFG_EDIT);
                      int cur = (t == 8) ? 3
                              : (t <= 2) ? t : atoi(getenv("RE15_CONFIG_CUR")?getenv("RE15_CONFIG_CUR"):"0");
                      if (t >= 5 && t <= 7) { s_config_type = 3; s_edit_phase = t - 5; s_edit_slot = cur;
                                    s_edit_code = atoi(getenv("RE15_CONFIG_CODE")?getenv("RE15_CONFIG_CODE"):"0"); }
                      for (int f=0; f<4; f++) {   /* render a few frames so the shot reads a presented buffer, not stale black */
                          re15_render_begin_frame(); re15_render_background_gradient(0,0,0,0,0,0);
                          re15_render_pc_config_clear();
                          re15_render_pc_config(&bg, 0); pc_config_draw_labels();
                          pc_config_draw_overlay(&tim, screen, cur); re15_render_end_frame(); }
                          re15_audio_tick();
                      re15_render_pc_screenshot(cs); exit(0); }
            pc_run_config(); exit(0);
        }
        /* The real RE1.5 title menu, VISUALLY verified against the PSX (shots/psx_title_a.png): the 3 rows
         * NEW GAME / LOAD GAME / OPTION + the CAPCOM copyright are SPRITES from DATA/TMOJI.TIM, drawn at
         * x=0x20, y=0x85/0x99/0xad (draw code FUN_801027a0, descriptor 0x801028ac); the active row uses the
         * white CLUT sub-palette, the others blue. Shown immediately (init skips "press any button").
         * (My earlier "LOAD DATA"/"CONFIG" was wrong — those ASCII strings @0x80 are the vestigial dev
         * menu; the real sprite labels read LOAD GAME / OPTION.) */
        while (re15_gameflow_mode() == RE15_MODE_TITLE) {
            re15_render_begin_frame();
            re15_input_tick();                       /* SDL_QUIT -> exit(0) inside; refreshes pad */
            re15_render_background_gradient(8, 8, 16, 0, 0, 0);
            if (s_boot_title.pixels) re15_render_pc_show_title(&s_boot_title);
            uint16_t pp = g_engine.pad_pressed;
            if (getenv("RE15_CONTINUE_TEST")) {              /* debug: auto-drive title -> LOAD GAME */
                if (tblink == 8)  cursor = 1;
                if (tblink == 16) pp |= RE15_PAD_BIT_CROSS;  /* confirm LOAD GAME -> load screen */
            }
            /* Draw the byte-true NEW GAME / LOAD GAME / OPTION + copyright sprites from TMOJI.TIM
             * (active row white, others blue) at x=0x20, y=0x85/0x99/0xad. */
            re15_render_pc_title_menu(&s_tmoji, cursor);

            if (pp & (RE15_PAD_BIT_UP | RE15_PAD_BIT_DOWN)) {
                /* Cursor-Blip = CORE-SE 4 (FUN_80045024(0x04040000,0) @0x80102b1c-24 UP /
                 * @0x80102b6c-74 DOWN). */
                extern void re15_audio_core_se(int se_id);
                re15_audio_core_se(4);
            }
            if (pp & RE15_PAD_BIT_UP)    cursor = (cursor + 2) % 3;
            if (pp & RE15_PAD_BIT_DOWN)  cursor = (cursor + 1) % 3;
            /* confirm = any of the four face buttons OR Start (byte-true mask 0x8f0 @0x800bc762) */
            uint16_t confirm = pp & (RE15_PAD_BIT_CROSS | RE15_PAD_BIT_SQUARE |
                                     RE15_PAD_BIT_TRIANGLE | RE15_PAD_BIT_CIRCLE | RE15_PAD_BIT_START);
            if (confirm) {
                { extern int re15_fade_log_on(void);
                  if (re15_fade_log_on())
                      fprintf(stderr, "[flow] title confirm tblink=%u cursor=%d\n", tblink, cursor); }
                /* "Biohazard 2!"-Announcer = CORE-SE 0, VOR dem Fade und bei JEDEM Menuepunkt
                 * (FUN_80045024(0x04000000,0) @0x80102c20-24; Gate nur pad&0x8f0 @0x80102c14 —
                 * kein Cursor-Check). Die Stimme (3.96 s) laeuft ueber Fade + Player-Select weiter
                 * (kein Stop-Call im Pfad). */
                { extern void re15_audio_core_se(int se_id); re15_audio_core_se(0); }
                /* BLOCKING-Fade @0x80102ccc — fuer ALLE drei Menuepunkte (Call @0x80102c48 VOR dem
                 * Item-Dispatch @0x80102c60). Drei Phasen der EXE-Fade-Engine (Integrator-Semantik
                 * FUN_80021880: Prim-B = level>>7 VOR der Integration, level += step, fertig am
                 * Vorzeichen-Bit; Engine-Tick = 2 Vsyncs im 480i-Title — DuckStation-gemessen,
                 * jede Stufe exakt 2 Capture-Frames):
                 *   A @0x80102cd4-e4: (0x100,-0x800,7,0) ADDITIV-Weiss, kick step<0 -> level=0x7fff
                 *     => B 255->15, -16/Tick, 16 Ticks (Weiss-BLITZ);
                 *   B @0x80102d28-4c: (0x200,+0xe0,7,1) SUBTRAKTIV, kick step>0 -> level=0
                 *     => B 0->255, +1.75/Tick, 147 Ticks (Fade-to-black, ~4.9 s);
                 *   C @0x80102d78-9c: (0x200,0,7,1) + kick(0x7fff) => B=255 FEST (Schwarz halten;
                 *     im Original CD-Latenz, im Port latenzfrei). Der alte 32x(+8)-Alpha-Loop war
                 *     ein unbelegter Rate-Defekt (0.53 s statt ~5.45 s). */
                {
                    extern void re15_render_pc_title_fade_add(int b);
                    extern void re15_render_pc_title_fade_sub(int b);
                    uint16_t level; int16_t step; int subph;
                    uint32_t tf_last = SDL_GetTicks();
                    for (int phase = 0; phase < 2; phase++) {
                        if (phase == 0) { level = 0x7fff; step = -0x800; subph = 0; } /* A: kick @0x80102cf8 */
                        else            { level = 0;      step = 0x00e0; subph = 1; } /* B: kick @0x80102d48 */
                        while (!(level & 0x8000u)) {
                            int B = level >> 7;
                            for (int v = 0; v < 2; v++) {         /* Tick = 2 Vsyncs (Flip @0x80102d18) */
                                re15_render_begin_frame();
                                re15_input_tick();
                                re15_render_background_gradient(8, 8, 16, 0, 0, 0);
                                if (s_boot_title.pixels) re15_render_pc_show_title(&s_boot_title);
                                re15_render_pc_title_menu(&s_tmoji, cursor);   /* Redraw @0x80102d10 */
                                if (subph) re15_render_pc_title_fade_sub(B);
                                else       re15_render_pc_title_fade_add(B);
                                re15_render_end_frame();
                                re15_audio_tick();
                                { uint32_t now = SDL_GetTicks(); uint32_t el = now - tf_last;
                                  if (el < 16) SDL_Delay(16 - el); tf_last = SDL_GetTicks(); }
                            }
                            level = (uint16_t)(level + (uint16_t)step);
                        }
                        if (phase == 0) re15_render_pc_title_fade_add(0);
                    }
                    re15_render_pc_title_fade_sub(255);           /* Phase C: Schwarz halten */
                }
                if (cursor == 0) {                            /* NEW GAME -> player-select */
                    /* danach REPLACES der Confirm die Task mit dem Player-Select
                     * (FUN_80029ba4 @0x80102c9c). */
                    { extern void re15_render_pc_title_fade_sub(int b); re15_render_pc_title_fade_sub(0); }
                    { extern int re15_fade_log_on(void);
                      if (re15_fade_log_on()) fprintf(stderr, "[flow] pselect enter\n"); }
                    int ch = pc_run_player_select();          /* "PLEASE SELECT MAIN CAST" (@0x80101094) */
                    { extern int re15_fade_log_on(void);
                      if (re15_fade_log_on()) fprintf(stderr, "[flow] pselect done ch=%d\n", ch); }
                    re15_gameflow_new_game(ch);               /* ch = DAT_800aca5c>>2 (0=Leon,1=Elza) */
                    /* Game-Start laedt die CHARAKTER-CORE-Bank: FUN_800440c4(DAT_800aca5c)
                     * @0x800316d8-e8 (0 = Leon -> CORE00, 4 = Elza -> CORE04). Die noch spielende
                     * Announcer-Voice liest ihre alte Generation weiter (audio_pc.c). */
                    { extern void re15_audio_prime_core(int idx); re15_audio_prime_core(ch ? 4 : 0); }
                    /* NEW GAME setzt den Save-Zaehler zurueck (Nutzer-Report 2026-08-03; analysis/
                     * save_counter.md SC-2): der Zaehler DAT_800b0fbd ist Teil des LIVE-Game-State-
                     * Blocks — ein frisches Spiel startet mit 0 (EXE-Image-Byte @Datei 0xa17bd = 00;
                     * einziger Writer ist das Post-Save-Inkrement @0x8002649c, restauriert nur vom
                     * LOAD-memcpy @0x80026290-a0). Ohne den Reset erbte ein New-Game-Save den
                     * Zaehler eines zuvor geladenen Spielstands. */
                    s_save_counter = 0;
                }
                else if (cursor == 1) {                       /* LOAD GAME -> FE-4 memory-card load screen */
                    { extern void re15_render_pc_title_fade_sub(int b); re15_render_pc_title_fade_sub(0); }
                    uint16_t resume_room = 0;
                    if (pc_run_memcard_screen(0, NULL, &resume_room) >= 0) {
                        /* loaded into s_resume_sd; enter INGAME at the saved room, then the
                         * game loop applies s_resume_sd after the room + player are set up. */
                        s_resume_pending        = 1;
                        g_gameflow.character     = s_resume_sd.character;
                        g_gameflow.start_room    = resume_room;
                        g_gameflow.enter_ingame  = 1;
                        g_gameflow.mode          = RE15_MODE_INGAME;   /* exits the title loop */
                        /* Charakter-CORE-Bank wie der Game-Start (FUN_800440c4(DAT_800aca5c)
                         * @0x800316d8-e8): 0 = Leon -> CORE00, 4 = Elza -> CORE04. */
                        { extern void re15_audio_prime_core(int idx);
                          re15_audio_prime_core(s_resume_sd.character ? 4 : 0); }
                    } else {
                        tblink = 0;   /* zurueck zum Titel: Fade-in erneut (Sub-Screen-Exit +0x400
                                       * @0x801024f0-500 laeuft im Original im Sub-Screen selbst) */
                    }
                }
                else if (cursor == 2) {                       /* OPTION -> controller-CONFIG screen */
                    { extern void re15_render_pc_title_fade_sub(int b); re15_render_pc_title_fade_sub(0); }
                    pc_run_config();                          /* byte-true EXE task @0x8002dde4 (foundation) */
                    if (s_boot_title.pixels) re15_render_pc_show_title(&s_boot_title);   /* restore title art */
                    tblink = 0;                               /* Titel-Fade-in erneut */
                }
            }
            /* Title-FADE-IN nach Boot/FMV: dieselbe Fade-Engine, `FUN_800217b0(0x200,-0x400,7,3)`
             * @0x80102054-64 = SUBTRAKTIV, B = 255 -> 0 mit -8/Tick, 32 Ticks (Tick = 2 Vsyncs).
             * Bei 60-fps-Frames: Tick = tblink>>1. (Der alte `255 - tblink*13` ueber 20 Frames
             * war eine geratene Rate.) */
            { extern void re15_render_pc_title_fade_sub(int b);
              int tk = (int)(tblink >> 1); int B = 255 - tk * 8; if (B < 0) B = 0;
              re15_render_pc_title_fade_sub(B); }
            re15_render_end_frame();
            re15_audio_tick();
            re15_render_pc_hide_title_menu();   /* stop drawing the menu sprites once the title yields */
            { unsigned t_af = 22; const char *afe = getenv("RE15_TITLE_SHOT_AF"); if (afe) t_af = (unsigned)atoi(afe);
              if (t_shot && tblink == t_af) { re15_render_pc_screenshot(t_shot); re15_gameflow_new_game(0); } }
            tblink++;
        }
        re15_render_pc_hide_title();
    }

    /* RNG DETERMINISM — byte-true (2026-07-27, audit wf_228686ee, adversarially verified). The earlier
     * comment here claimed the original RNG "evolves from boot -> each playthrough differs"; that premise
     * is FALSE. RE1.5 has NO entropy source: the gameplay RNG state @0x800AC774 is a DEAD STORE — the
     * whole-EXE xref is exactly one READ (lhu t1 @0x8001af28, t1 never used) + two WRITES (the RNG's own
     * sw a0 @0x8001af48 and a constant 0x1c3 store @0x80031634 that nothing ever reads). FUN_8001af20
     * hashes the CALLER's incidental a0 register (usually a fixed pointer, e.g. *attack_workstruct loaded
     * @0x8001772c before jal @0x80017740), not the stored state. With fixed inputs every roll is
     * DETERMINISTIC on real hardware — "the chair zombie always shambles" IS the byte-true behavior, not
     * a defect to randomize away. The old wall-clock reseed (SDL_GetTicks^time) manufactured variation the
     * console never exhibits and broke headless A/B reproducibility (identical draw count, differing state
     * every run). Default now = deterministic (fixed compile-time seed 0x2545f491, the sole seed). Opt into
     * per-launch variety only with RE15_RNG_ENTROPY (casual play, never a parity run). */
    if (getenv("RE15_RNG_ENTROPY")) {
        extern void re15_damage_seed_rng(uint32_t seed);
        re15_damage_seed_rng((uint32_t)SDL_GetTicks() ^ 0xA5F15A3Du ^ (uint32_t)time(NULL));
    }

    /* BE-round REVERTED (2026-05-28): tried 60fps default + 60Hz SCD as
     * PSX-canonical, but our cinematic was tuned for 30Hz SCD so all
     * Sleep gates now elapse 2× faster, breaking cut-chain timing.
     * Keep 30fps default; rotor stroboscopic strobe remains a known
     * artifact. RE15_FPS=60 still available for future re-tuning round. */
    int target_fps = 30;
    {
        const char *fps_env = getenv("RE15_FPS");
        if (fps_env) {
            int parsed = atoi(fps_env);
            if (parsed >= 15 && parsed <= 240) target_fps = parsed;
        }
    }
    const uint32_t frame_budget_ms = (uint32_t)(1000 / target_fps);
    fprintf(stderr, "[fps] target=%d FPS, frame_budget=%ums (RE15_FPS env to override)\n",
            target_fps, frame_budget_ms);

    /* AI-round (2026-05-26): scale frame-count-based gates by target_fps.
     * All historic frame_count constants (1800 sub00 spawn, 3390 Leon visible,
     * shot_frames[] autoshot targets) were tuned for 60fps render. At 30fps
     * each frame = 2x wall-time so these gates would fire at half the rate
     * → Leon wouldn't be visible until 113s instead of 56s.
     * Use FRAME_AT_60(n) = n at 60fps target, n/2 at 30fps target, etc. */
#define FRAME_AT_60(n)  (((n) * target_fps) / 60)

    /* Phase 4.5.6.4: software MDEC + decode bundled BG (PC: no-op chip init).
     * Der eigentliche Boot-BG-Preload ist NACH die Raum-Initialisierung verschoben
     * (vor den Game-Loop, s.u.) — Beleg + Messung dort. */
    re15_bg_init();

    /* Phase 4.5.9 / globalization Phase 3-A (2026-06-13): load + parse the room RDT
     * EARLY — before audio + props — so the footstep VAB, the Obj_model_set props,
     * and the heli/pilot can be SLICED from it (g_room_rdt prop + snd banks) exactly
     * like the PSX engine, instead of separate room####_obj## / snd0 files. The local
     * `rdt`/`rdt_ok`/`rdt_buf` keep driving the ~32 main-loop refs (cuts/zones/scd/
     * cross-room); we ALSO publish to g_room_rdt + g_current_room_id so the shared
     * footstep loader (audio_pc) and the cross-room machinery see the boot room.
     * (Was loaded later, after props — moved up; RDT parse only needs file I/O.) */
    int rdt_size = 0;
    /* FE-4: Boot-Raum = g_gameflow.start_room (0x1240 bei NEW GAME, sonst der CONTINUE-Raum).
     * Die frueher hier moegliche RE15_START_ROOM-Ueberschreibung ist ENTFERNT — sie bootete an
     * re15_room_apply_pending vorbei und lieferte damit einen anderen Zustand als jeder echte
     * Raumwechsel. Zu einem beliebigen Raum kommt man ueber das Debug-Menue (Backspace). */
    unsigned boot_room = (unsigned)g_gameflow.start_room;
    /* Asset-Pfad-Konsolidierung (2026-07-02): der Boot-Room-RDT kommt aus dem CD-Layout
     * STAGE{N}/ROOM%04X.RDT (N = room_id>>12), NICHT mehr aus der entfernten flachen RDT/-Struktur.
     * Identische Auflösung wie re15_room_load (room_pc.c) für die Cross-Room-Transitions. */
    char rdt_path[32]; snprintf(rdt_path, sizeof rdt_path, "STAGE%u/ROOM%04X.RDT",
                                (boot_room >> 12) & 0xFu, boot_room);
    uint8_t *rdt_buf = pc_read_shared(rdt_path, &rdt_size);
    fprintf(stderr, "[boot] room RDT: %s (%d bytes)\n", rdt_path, rdt_size);
    re15_rdt_t rdt = {0};
    int rdt_ok = 0;
    if (rdt_buf && re15_rdt_parse(rdt_buf, (size_t)rdt_size, &rdt) == 0) {
        rdt_ok = 1;
        g_room_rdt        = rdt;        /* publish for the shared footstep loader + parity */
        g_room_rdt_ok     = 1;
        g_current_room_id = boot_room;
        fprintf(stderr, "[rdt] nCut=%d nDoor=%d nItem=%d zones=%d main_scd=%zuB sub=%d props=%d\n",
                rdt.nCut, rdt.nDoor, rdt.nItem, rdt.zone_count,
                rdt.main_scd_size, rdt.sub_scd_count, rdt.prop_count);
    }
    /* RAUM-INIT-RESET AUF DEM BOOT-/LADE-PFAD — das fehlende Gegenstueck zum Original.
     *
     * Der Tod fuehrt IN-PROCESS hierher zurueck (mode == RE15_MODE_TITLE -> `goto re_title`), also
     * laeuft nach Tod -> Title -> NEW GAME/LOAD genau dieser Block noch einmal — aber ohne den
     * Raum-Init-Reset, den ein normaler Raumwechsel macht (room_common.c). Im Original raeumt JEDER
     * Raum-Load das Spieler-Kommandoregister ab: FUN_800314b0 nullt @0x80031518 `sw zero,0x800aca58`
     * (cmd/Variante/Phase in EINEM Wort) und setzt @0x80031718 HP=100 / @0x80031720 Status=0; einziger
     * Caller ist der Raumlader FUN_800396fc (jal @0x80039788). Zusaetzlich nullt der Karten-Screen-Exit
     * @0x8001cbdc `sb zero,0x800aca58` — genau der Pfad, den der Port als "Title -> LOAD GAME -> Spiel"
     * nachbaut.
     *
     * Ohne das ueberlebte der DEVOUR-Kommandozustand (g_player_victim, Original DAT_800aca58 = 6) den
     * Tod: im ersten Frame nach dem Laden lief die COLLAPSE-Phase weiter, setzte die Position ABSOLUT
     * aus anchor(0,0) (Teleport an den Weltursprung) und bei anim_frame == 0x23 erneut hp = -1
     * (byte-true FUN_8010a6f8 @0x8010a7e8/@0x8010a80c) -> zweiter Tod ~1 s nach dem Laden -> Title ->
     * Load -> Endlosschleife. Traf jeden Grab-/Devour-Tod, also den Normalfall; NEW GAME direkt nach
     * dem Tod war genauso betroffen.
     *
     * STELLE: nach dem RDT-Install, aber VOR pc_load_room_esp und weit vor dem ersten pc_enemy_load
     * (re15_apply_room_cinematic) und dem Raum-RBJ-Bind — re15_enemy_reset() ruft selbst
     * re15_rbj_bind_room(NULL,0) und wuerde spaeter platziert die eben gebundene Cutscene-Bank
     * wegreissen. Auf dem ERSTEN Boot ist der Aufruf ein No-op (alles steht bereits auf 0). */
    re15_enemy_reset();       /* Victim-/Crow-Flock-/Spawn-Count-Reset + Banks der Vor-Session */
    re15_player_cmd_reset();  /* Knockdown-/Flinch-/Event-Reach-Statics ausserhalb von g_actors */

    /* Phase ESP-C: parse + bind this room's effect-sprite bank for op-0x3a spawns. */
    pc_load_room_esp(rdt_buf, rdt_size, boot_room);

    /* Phase ESP-D: load the GLOBAL effect bank CORE00.ESP once at game-init (FUN_8001923c) so the
     * universal hit effects (effect-id 0, used by the master-table hit handlers) resolve. */
    {
        int gsz = 0;
        s_global_esp_buf = pc_read_shared("DATA/CORE00.ESP", &gsz);
        if (s_global_esp_buf && re15_esp_parse_global(s_global_esp_buf, (size_t)gsz, &s_global_esp) == 0) {
            re15_esp_set_global_bank(&s_global_esp);
            fprintf(stderr, "[esp] global bank CORE00.ESP: %d effects (incl effect-id 0 hit fx)\n",
                    s_global_esp.id_count);
        } else {
            fprintf(stderr, "[esp] global bank CORE00.ESP NOT loaded\n");
        }
        /* The GLOBAL effect textures are in NO RDT — CORE00.ESP itself carries only anim/coord
         * records plus a `word1 = (tpage<<16)|clut` per effect that points at a VRAM page the boot
         * installer uploaded. Ids 0/2/3/4 were recovered in 2026-07 from a live ShowVRAM screenshot.
         *
         * Id 8 (FIRE) is sourced from the FILE instead — `DATA/TEX.TIM`, the 1280x256 4bpp common
         * atlas (tools/tex_tim_effect_slice.py). Belegt 2026-08-21: TEX.TIM's image block (@file
         * 0x620, 320 halfwords x 256 rows, uploads to VRAM(0,0)) has its halfword columns 192..319
         * blitted verbatim to VRAM(896,256) — all 32768 halfwords of that rect are byte-identical
         * to the ShowVRAM ground truth, so the page is NOT "repacked" as extracted_fx/README.md
         * claimed. id 8 = tpage 0x001e -> columns 192..255 (pixels @file 0x7a0, stride 640) with
         * clut 0x7911 -> the CLUT block's row 4 / col 16 (@file 0x134, 16 entries).
         * The file route is also STRICTLY better than the VRAM one: the ShowVRAM PNG has no alpha,
         * so halfword bit 15 is lost — and in a 4bpp page bit 15 is the high bit of every 4th texel
         * index, i.e. indices 8..15 collapse to 0..7 there. TEX.TIM keeps them (4162 such halfwords
         * in this page alone).
         *
         * FARBSCHLUESSEL: die fire-CLUT hat 0x0000 auf Index 0 UND Index 15
         * (`0000 ffff … 8003 0001 0000`), und Index 0 ist 68% jeder Flammenzelle. Die
         * Sonderbehandlung ("key"-Spalte) ist seit 2026-08-21 WEG: der Upload wendet die
         * byte-true GPU-Regel (aufgeloester Texel-Wert 0x0000 = nicht zeichnen) auf JEDE
         * Textur an, also auch auf die vier anderen Sheets hier. Fuer 21/22/23 aendert das
         * nichts (16bpp — die liefen schon immer ueber dieselbe Wert-Regel), fuer Slot 20
         * (4bpp Blut) auch nicht: dort sind Index-0-Menge und Wert-0-Menge deckungsgleich
         * (gemessen 55934 == 55934, probe_texel_key_census). */
        static const struct { const char *file; int slot; const char *tag; } k_gfx[] = {
            { "extracted_fx/effect0_blood.tim",  RE15_TIM_SLOT_EFFECT_GLOBAL, "0 blood"  },
            { "extracted_fx/effect2_muzzle.tim", RE15_TIM_SLOT_FX_MUZZLE,     "2 muzzle" },
            { "extracted_fx/effect3_smoke.tim",  RE15_TIM_SLOT_FX_SMOKE,      "3 smoke"  },
            { "extracted_fx/effect4_shell.tim",  RE15_TIM_SLOT_FX_SHELL,      "4 shell"  },
            { "extracted_fx/effect8_fire.tim",   RE15_TIM_SLOT_FX_FIRE,       "8 fire"   },
        };
        for (size_t gi = 0; gi < sizeof(k_gfx)/sizeof(k_gfx[0]); gi++) {
            int bsz = 0;
            uint8_t *gtim = pc_read_shared(k_gfx[gi].file, &bsz);
            if (gtim) {
                re15_tim_t btim;
                if (re15_tim_parse(gtim, bsz, &btim) == 0) {
                    re15_render_pc_upload_tim_slot(&btim, k_gfx[gi].slot);
                    fprintf(stderr, "[esp] global effect-%s TIM -> slot %d: %dx%d %dbpp\n",
                            k_gfx[gi].tag, k_gfx[gi].slot, btim.width, btim.height, btim.bpp);
                } else {
                    fprintf(stderr, "[esp] %s parse FAILED\n", k_gfx[gi].file);
                }
                free(gtim);
            } else {
                fprintf(stderr, "[esp] %s NOT found\n", k_gfx[gi].file);
            }
        }
    }

    /* Phase 4.6.1: SDL audio device + SCD audio queue consumer. Silent
     * playback callback until 4.6.3 wires the ADPCM mixer in. */
    re15_audio_init();

    /* Load + parse test asset. Try several relative paths so it works whether
     * run from build/Release/, from project root, or installed bin/. */
    int tim_size = 0;
    uint8_t *tim_buf = pc_read_shared("PLD/PL00.TIM", &tim_size);
    re15_tim_t tim;
    int tim_ok = 0;
    if (tim_buf && re15_tim_parse(tim_buf, tim_size, &tim) == 0) {
        tim_ok = 1;
        fprintf(stderr, "[tim] loaded test.tim: %dx%d bpp=%d clut=%d\n",
                tim.width, tim.height, tim.bpp, tim.has_clut);
        /* Phase 4.5.5: upload to GPU as SDL_Texture so SDL_RenderGeometry
         * can sample it for textured 3D triangles. */
        re15_render_pc_upload_tim(&tim);
    } else {
        fprintf(stderr, "[tim] FAILED to load test.tim\n");
    }

    /* Phase 4.5: load MD1 mesh — same path-search pattern */
    int md1_size = 0;
    uint8_t *md1_buf = pc_read_shared("PLD/PL00.MD1", &md1_size);
    /* Zero-init avoids "possibly uninitialised" warnings on toolchains that
     * can't see the md1_ok guard implies md1 is populated. The fields the
     * renderer reads later (mesh_count, meshes[]) are then well-defined. */
    re15_md1_t md1 = {0};
    int md1_ok = (md1_buf && re15_md1_parse(md1_buf, md1_size, &md1) == 0);
    /* Das Spieler-Mesh der Engine bekanntgeben und den Modell-Rueckruf einhaengen: ab
     * jetzt zieht JEDER Raumlade-Weg das Modell nach work_vars[0x10] nach — auch die
     * SELBST-Tuer der R.P.D.-Ruestung in ROOM1190, die keinen Raumwechsel ausloest. */
    s_player_md1_ref = &md1;
    re15_scd_set_player_model_sync(pc_player_model_sync_cb);
    if (md1_ok) {
        fprintf(stderr, "[md1] loaded test.md1: %d meshes\n", md1.mesh_count);
    }

    /* Forward-declare render helpers used by the per-prop loader below. */
    extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
    extern void re15_render_pc_bind_tim_slot(int slot);

    /* Generic per-prop loader for ALL room obj models (obj_id 0..RE15_RDT_MAX_PROPS-1).
     * Each gets its own MD1 + TIM (slots 4..9 fuer obj 0..5, 26..35 fuer obj 6..15 —
     * RE15_TIM_SLOT_PROP) so the prop renderer can bind the correct texture per prop.
     * (Slot 0=Leon, 1=Elliot, 2=heli body legacy, 3=pilot legacy.) Das alte [6]-Array
     * kappte ROOM11F0s 12 Props (Original: KEIN Cap, Schranke = nOmodel @0x80043758). */
    re15_md1_t s_room_prop_md1[RE15_RDT_MAX_PROPS] = {0};
    int        s_room_prop_ok [RE15_RDT_MAX_PROPS] = {0};
    /* Data-driven per-room prop set (parity with PSX). Boots ROOM1170; the cross-
     * room consume reloads for the destination room (room1140 etc.). */
    pc_load_room_prop_set(&rdt, s_room_prop_md1, s_room_prop_ok);   /* boot room RDT (room1170) */

    /* Helicopter body (obj 0x02) + Pilot (obj 0x05) — Phase 3-A (2026-06-13): both are
     * SLICED from the RDT prop table (prop[2] heli, prop[5] pilot), TIMs → render slots
     * 2/3. Were room1170_obj0[25].{md1,tim} files; pointers alias the resident RDT
     * buffer (byte-true == those files). (slot 0=Leon, 1=Elliot, 2=heli, 3=pilot.) */
    re15_md1_t heli_md1 = {0};
    int heli_ok = (rdt_ok && rdt.prop_md1[2] &&
                   re15_md1_parse(rdt.prop_md1[2], (size_t)rdt.prop_md1_size[2], &heli_md1) == 0);
    if (heli_ok) fprintf(stderr, "[md1] heli body (RDT prop2): %d meshes\n", heli_md1.mesh_count);
    if (rdt_ok && rdt.prop_tim[2]) {
        re15_tim_t heli_tim;
        if (re15_tim_parse(rdt.prop_tim[2], rdt.prop_tim_size[2], &heli_tim) == 0) {
            re15_render_pc_upload_tim_slot(&heli_tim, 2);
            fprintf(stderr, "[tim] heli TIM (RDT prop2) in slot 2: %dx%d\n", heli_tim.width, heli_tim.height);
        } else {
            fprintf(stderr, "[tim] heli TIM parse FAILED — heli will render with Leon's TIM\n");
        }
    }

    re15_md1_t pilot_md1 = {0};
    int pilot_ok = (rdt_ok && rdt.prop_md1[5] &&
                    re15_md1_parse(rdt.prop_md1[5], (size_t)rdt.prop_md1_size[5], &pilot_md1) == 0);
    if (pilot_ok) fprintf(stderr, "[md1] pilot (RDT prop5): %d meshes\n", pilot_md1.mesh_count);
    if (rdt_ok && rdt.prop_tim[5]) {
        re15_tim_t pilot_tim;
        if (re15_tim_parse(rdt.prop_tim[5], rdt.prop_tim_size[5], &pilot_tim) == 0) {
            re15_render_pc_upload_tim_slot(&pilot_tim, 3);
            fprintf(stderr, "[tim] pilot TIM (RDT prop5) in slot 3: %dx%d\n", pilot_tim.width, pilot_tim.height);
        } else {
            fprintf(stderr, "[tim] pilot TIM parse FAILED — pilot will render with Leon's TIM\n");
        }
    }

    /* Phase 4.5.13-R23: Elliot's actual model is PL05.PLD (not em47 EMD).
     * Load his MD1 mesh + EDD/EMR for skeletal animation so NPC[1] (type
     * 0x47) renders as Elliot instead of a Leon-clone. */
    int elliot_md1_size = 0;
    uint8_t *elliot_md1_buf = pc_read_shared("PLD/ELLIOT.MD1", &elliot_md1_size);
    re15_md1_t elliot_md1 = {0};
    int elliot_ok = (elliot_md1_buf && re15_md1_parse(elliot_md1_buf, elliot_md1_size, &elliot_md1) == 0);
    int elliot_edd_size = 0, elliot_emr_size = 0;
    uint8_t *elliot_edd_buf = pc_read_shared("PLD/ELLIOT.EDD", &elliot_edd_size);
    uint8_t *elliot_emr_buf = pc_read_shared("PLD/ELLIOT.EMR", &elliot_emr_size);
    re15_emd_animation_t elliot_anim = {0};
    re15_emd_skeleton_t  elliot_skel = {0};
    int elliot_skel_ok = 0;
    if (elliot_edd_buf && elliot_emr_buf) {
        if (re15_emd_parse_animation(elliot_edd_buf, elliot_edd_size, &elliot_anim) == 0 &&
            re15_emd_parse_skeleton (elliot_emr_buf, elliot_emr_size, &elliot_skel) == 0) {
            elliot_skel_ok = 1;
            fprintf(stderr, "[elliot] PL05 loaded: %d meshes, %d bones, %d clips\n",
                    elliot_md1.mesh_count, elliot_skel.bone_count, elliot_anim.clip_count);
        }
    }
    /* Register Elliot's own cinematic EDD with the NPC motion executor so his Plc_motion GESTURE
     * clips (ROOM1170 intro "Hey!" wave etc.) wrap anim_frame at THEIR real length, not the shared
     * EM040/Irons table (which froze clips 15/16/20/25 on frame 0). Stable function-scope storage. */
    { extern void re15_npc_set_elliot_anim(const re15_emd_animation_t *);
      re15_npc_set_elliot_anim(&elliot_anim); }

    /* AD-round (2026-05-26): load PL00W01 (handgun weapon track) for the
     * RUN/WALK_FORWARD animation. PL00.EDD only has Walk_Backward / Damage
     * / Climb / Stairs. The real forward-walk + run live in the weapon
     * track EDDs (PL00W*.PLW) per RE2-Nov96 architecture. Per bio-lib enum
     * weapon track: clip 0 = Run, clip 5 = Walk_Forward.
     * PL00W01.edd verified: 14 clips, clip 5 = 30 frames (Walk_Forward),
     * clip 0 = 22 frames (Run). */
    int w01_edd_size = 0, w01_emr_size = 0;
    uint8_t *w01_edd_buf = pc_read_shared("PLD/PL00W01.EDD", &w01_edd_size);
    uint8_t *w01_emr_buf = pc_read_shared("PLD/PL00W01.EMR", &w01_emr_size);
    re15_emd_animation_t w01_anim = {0};
    re15_emd_skeleton_t  w01_skel_raw = {0};   /* W01-owned bind+keyframes */
    re15_emd_skeleton_t  w01_skel = {0};        /* composite used by renderer */
    int w01_ok = 0;
    if (w01_edd_buf && w01_emr_buf) {
        if (re15_emd_parse_animation(w01_edd_buf, w01_edd_size, &w01_anim) == 0 &&
            re15_emd_parse_skeleton (w01_emr_buf, w01_emr_size, &w01_skel_raw) == 0) {
            w01_ok = 1;
            fprintf(stderr, "[w01] PL00W01 weapon-track: %d bones, %d clips, %d kf\n",
                    w01_skel_raw.bone_count, w01_anim.clip_count, w01_skel_raw.keyframe_count);
        }
    }
    /* PL00W03 = the GUN carry set (byte-true item->bank map: the equip loader @0x80036b80 loads
     * CD file DAT_800741e8[char]=76 + item — melee trio W00≡W01≡W02 (items 0-2, md5-identical),
     * gun pair W03≡W04 (items 3/4). Same 14-clip layout as W01 (raise 6=10f, holds 8/10/12=1f,
     * recoils 7/9/11=23/24/24f, reload 0xD=32f). W03.EDD/.EMR = byte-true slices of PL00W03.PLW
     * (dir[0]/dir[1]), extracted the same way as the vendored W01 pair (slice==vendored verified). */
    int w03_edd_size = 0, w03_emr_size = 0;
    uint8_t *w03_edd_buf = pc_read_shared("PLD/PL00W03.EDD", &w03_edd_size);
    uint8_t *w03_emr_buf = pc_read_shared("PLD/PL00W03.EMR", &w03_emr_size);
    re15_emd_animation_t w03_anim = {0};
    re15_emd_skeleton_t  w03_skel_raw = {0};
    re15_emd_skeleton_t  w03_skel = {0};
    int w03_ok = 0;
    if (w03_edd_buf && w03_emr_buf) {
        if (re15_emd_parse_animation(w03_edd_buf, w03_edd_size, &w03_anim) == 0 &&
            re15_emd_parse_skeleton (w03_emr_buf, w03_emr_size, &w03_skel_raw) == 0) {
            w03_ok = 1;
            fprintf(stderr, "[w03] PL00W03 gun-track: %d bones, %d clips, %d kf\n",
                    w03_skel_raw.bone_count, w03_anim.clip_count, w03_skel_raw.keyframe_count);
        }
    }
    /* WEAPON-IN-HAND MODELS (room-fix #3, byte-true mechanism: the melee DRAW's anim-event
     * attaches the equipped weapon's model pointers @0x800356f0-70 into kine+1900..1912 —
     * drawn as extra parts on the weapon bone (bone 11). The MESHES live in the PLW
     * containers: dir[2] = MD1 (1 mesh), dir[3] = TIM; PL00.MD1's meshes 15/16 are only the
     * empty attach slots). Slice + parse both class models; TIMs -> slots 24/25. */
    /* IN-HAND WEAPON MESHES for EVERY weapon (byte-true, RE'd via workflow wf_05bf645c + FUN_80036b68
     * @0x80036b84): the equip loader reads charid DAT_800aca5c, indexes the file base table
     * @0x800741e8 = {76,76,76,76,97,97,97,97} (char 0-3 -> base 76 = PL00 family = LEON; char 4-7 ->
     * base 97 = PL04 family = ELZA), and loads CD file base+aca5d = the character's OWN PLW; the mesh
     * is that PLW's dir[2] (hand+weapon). So Leon's handgun is PL00W03.PLW dir[2] — NOT the shared PL04
     * geometry (PL04W03 vs PL00W03 = 0/35 verts). Each is textured from the CHARACTER'S OWN body-skin
     * TIM (slot 0 = PL00.TIM for Leon), page 0x81 / clut 0x7840 -> slab 1 — NOT PL04.TIM. (The old
     * "shared PL04W03 dir[2] + PL04.TIM slot 25" model drew ELZA'S dark-gun assets on Leon -> black gun;
     * the char-select's PL04W03 buffer was a select-screen quirk, not the in-game per-character load.) */
    #define RE15_WPN_MDL_MAX 21   /* W00..W14 for the current character's family */
    const char *wpn_fam = (g_gameflow.character == 0) ? "PL00" : "PL04";  /* base_table charid split */
    re15_md1_t wpn_md1[RE15_WPN_MDL_MAX]; int wpn_md1_ok[RE15_WPN_MDL_MAX] = {0};
    for (int wi = 0; wi < RE15_WPN_MDL_MAX; wi++) {
        char plw_name[32]; snprintf(plw_name, sizeof plw_name, "PLD/%sW%02X.PLW", wpn_fam, wi);
        int psz = 0;
        uint8_t *plw = pc_read_shared(plw_name, &psz);   /* stays resident (MD1 borrows) */
        if (!plw || psz < 16) continue;
        uint32_t diroff = (uint32_t)(plw[0] | (plw[1]<<8) | (plw[2]<<16) | ((uint32_t)plw[3]<<24));
        if (diroff + 16 > (uint32_t)psz) continue;
        uint32_t de[4];
        for (int k = 0; k < 4; k++)
            de[k] = (uint32_t)(plw[diroff+4*k] | (plw[diroff+4*k+1]<<8) |
                               (plw[diroff+4*k+2]<<16) | ((uint32_t)plw[diroff+4*k+3]<<24));
        if (de[2] >= de[3] || de[3] > (uint32_t)psz) continue;
        if (re15_md1_parse(plw + de[2], (int)(de[3] - de[2]), &wpn_md1[wi]) == 0)
            wpn_md1_ok[wi] = 1;
    }
    /* NO separate weapon TIM upload: the in-hand mesh reads its texture from the character's own
     * body-skin TIM already resident in slot 0 (PL00.TIM for Leon @main.c:1907), the same slot the
     * body binds @4327 — the gun art lives on page 0x81 / clut-1 of that skin atlas. */
    {
        int nloaded = 0; for (int wi = 0; wi < RE15_WPN_MDL_MAX; wi++) nloaded += wpn_md1_ok[wi];
        fprintf(stderr, "[wpn] loaded %d/%d %sW** in-hand meshes (textured from body-skin slot 0)\n",
                nloaded, RE15_WPN_MDL_MAX, wpn_fam);
    }

    /* Elliot TIM into slot 1. */
    int elliot_tim_size = 0;
    uint8_t *elliot_tim_buf = pc_read_shared("PLD/ELLIOT.TIM", &elliot_tim_size);
    re15_tim_t elliot_tim;
    if (elliot_tim_buf && re15_tim_parse(elliot_tim_buf, elliot_tim_size, &elliot_tim) == 0) {
        re15_render_pc_upload_tim_slot(&elliot_tim, 1);
        fprintf(stderr, "[tim] elliot TIM in slot 1: %dx%d\n", elliot_tim.width, elliot_tim.height);
    } else {
        fprintf(stderr, "[tim] elliot TIM FAILED to load — NPC type 0x47 will use Leon's TIM\n");
    }
    if (elliot_ok) {
        fprintf(stderr, "[md1] loaded elliot mesh: %d meshes\n", elliot_md1.mesh_count);
    } else {
        fprintf(stderr, "[md1] elliot MD1 FAILED to load — NPC type 0x47 will use Leon's mesh\n");
    }

    /* Phase 4.5.7.3: load EDD (animation) + EMR (skeleton) for the
     * skeletal renderer. The EMR pointer is held by skel.keyframe_data
     * — keep emr_buf alive for the program's lifetime. */
    int edd_size = 0, emr_size = 0;
    uint8_t *edd_buf = pc_read_shared("PLD/PL00.EDD", &edd_size);
    uint8_t *emr_buf = pc_read_shared("PLD/PL00.EMR", &emr_size);

    re15_emd_animation_t anim = {0};
    re15_emd_skeleton_t  skel = {0};
    int skel_ok = 0;
    if (edd_buf && emr_buf
        && re15_emd_parse_animation(edd_buf, (size_t)edd_size, &anim) == 0
        && re15_emd_parse_skeleton (emr_buf, (size_t)emr_size, &skel) == 0) {
        skel_ok = 1;
        fprintf(stderr, "[skel] PL00: %d bones, %d clips, %d keyframes\n",
                skel.bone_count, anim.clip_count, skel.keyframe_count);
    }

    /* Phase 4.5.13-B2: try to load room-local animation.rbj (cinematic
     * anim source). If present + parsable, overlay rbj's keyframes + EDD
     * on PL00's skeleton. SCD Plc_motion(0, N, 0) will then index into
     * rbj's clip table (which has the actual cinematic animations like
     * the "Hey!" gesture at clip 15). */
    int rbj_size = 0;
    /* RE15_RBJ lets a debug run point the player's cinematic bank at a different room's
     * RBJ (e.g. RBJ/ROOM1150.RBJ for the Irons kneel) so RE15_CLIP_TEST can play those
     * clips. Default = the ROOM1170 intro bank. */
    const char *rbj_path = getenv("RE15_RBJ");
    char rbj_default[64];
    if (!rbj_path || !*rbj_path) {
        /* Room-aware default (2026-06-17): the cinematic bank must match the BOOT
         * room (was hardcoded ROOM1170 → booting room1150 used the wrong kneel clips). */
        snprintf(rbj_default, sizeof rbj_default, "RBJ/ROOM%04X.RBJ", boot_room);
        rbj_path = rbj_default;
    }
    uint8_t *rbj_buf = pc_read_shared(rbj_path, &rbj_size);
    /* RE1.5 ships NO standalone RBJ files — every room's cinematic anim lives INSIDE its
     * RDT (@0x5C, now parsed into rdt.animation). Fall back to that byte-true slice when
     * the (optional/debug) standalone file is absent — this is what makes room1150's Irons
     * kneel + any other room's cinematics actually load. `rbj_borrowed` aliases the resident
     * RDT buffer (must NOT be freed). The RE15_RBJ env override still wins for debug runs. */
    int rbj_borrowed = 0;
    if (!rbj_buf && rdt_ok && rdt.animation && rdt.animation_size > 0) {
        rbj_buf = (uint8_t *)rdt.animation;     /* alias into resident RDT — do not free */
        rbj_size = rdt.animation_size;
        rbj_borrowed = 1;
    }
    fprintf(stderr, "[rbj] loading cinematic bank: %s (%d bytes%s)\n",
            rbj_path, rbj_size, rbj_borrowed ? ", from RDT@0x5C" : "");
    /* X-round (2026-05-25): rbj overlay DISABLED. Deep RE of rbj keyframes
     * proved the overlay clips contain NO walk cycle — all "moving" clips
     * have correlated shoulders (both arms doing same motion = gestures)
     * and single-leg knee-lifts (no alternating thigh stride). The original
     * cinematic uses rbj for POSED moments (Speed-baked single-step toward
     * camera) — NOT for visible walking. PL00.edd / em47.edd contain the
     * REAL walk cycle at clip 4 (anti-correlated thighs, ~30° per leg).
     * Reverting to native EDDs gives proper walk animation for both
     * Leon and Elliot. Cost: Plc_motion(0, N) gestures during dialog now
     * use PL00/em47 native clips at N (which DO have multi-frame data
     * per agent table — e.g. PL00 clip 15=24f, 16=16f, 20=42f). */
    /* AC-round (2026-05-25): rbj overlay on BOTH Leon and Elliot for the
     * cinematic. Per bio-lib enum (Megan Grass) PL00.EDD's clip indices
     * are: 0=Walk_Backward, 4=Falling, 12=Damage_Laying, 19=Stairs_Begin,
     * 20=Stairs_Ascend, 22/23=Idle_Caution/Danger. **PL00.EDD has NO
     * forward walk** — Walk_Forward only exists in PL00W*.EDD (weapon
     * track) which RE1.5 may not load. So Leon's walks during cinematic
     * have to come from rbj overlay's clip 15 (cinematic walking pose
     * with pointing arms, baked Speed — perfect for sub02 "Hey wait!"
     * narrative beat per memory phase4_5_13_rbj_byte_layout). */
    /* AE-round (2026-05-26): build composite W01 skel = PL00 bind + W01
     * keyframes. Direct use of W01.EMR caused "scrambled" rendering because
     * W01.EMR has a DIFFERENT bind pose (gun-holding stance with arm raised)
     * than the PL00.MD1 mesh expects. Solution: keep PL00 bind, use W01's
     * keyframe pool for Walk_Forward + Run animation. Build composite ONCE
     * before any rbj overlay (rbj only changes keyframe_data, bind stays). */
    if (w01_ok && skel_ok) {
        w01_skel = skel;   /* copy bone hierarchy + bind pose from PL00 */
        w01_skel.keyframe_data       = w01_skel_raw.keyframe_data;
        w01_skel.keyframe_data_size  = w01_skel_raw.keyframe_data_size;
        w01_skel.keyframe_count      = w01_skel_raw.keyframe_count;
        w01_skel.keyframe_size_bytes = w01_skel_raw.keyframe_size_bytes;
        fprintf(stderr, "[w01-composite] PL00 bind + W01 keyframes: %d bones, %d kf\n",
                w01_skel.bone_count, w01_skel.keyframe_count);
    }
    if (w03_ok && skel_ok) {
        w03_skel = skel;   /* same composite pattern: PL00 bind + W03 keyframe pool */
        w03_skel.keyframe_data       = w03_skel_raw.keyframe_data;
        w03_skel.keyframe_data_size  = w03_skel_raw.keyframe_data_size;
        w03_skel.keyframe_count      = w03_skel_raw.keyframe_count;
        w03_skel.keyframe_size_bytes = w03_skel_raw.keyframe_size_bytes;
        fprintf(stderr, "[w03-composite] PL00 bind + W03 keyframes: %d bones, %d kf\n",
                w03_skel.bone_count, w03_skel.keyframe_count);
    }
    /* Seed the aim-FSM per-clip lengths with the START bank (byte-true default equip = the
     * KNIFE, item 1 -> melee bank W01). The per-frame equip watcher below re-feeds on switch. */
    {
        extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);
        uint16_t fcs[14]; int n = (w01_anim.clip_count < 14) ? w01_anim.clip_count : 14;
        for (int i = 0; i < n; i++) fcs[i] = (uint16_t)w01_anim.clips[i].frame_count;
        if (w01_ok) re15_player_set_aim_clip_lens(fcs, n);
    }
    /* The ACTIVE player W bank (equip-dependent; watcher in the main loop switches these). */
    re15_emd_skeleton_t  *wact_skel = &w01_skel;
    re15_emd_animation_t *wact_anim = &w01_anim;
    int                   wact_ok   = w01_ok;

    /* Unify 2026-06-06: keep the PL00 BASE track (PL00.edd clips, incl. clip 22/23
     * injured idle) BEFORE the rbj overlay below overwrites anim/skel. Mirrors PSX
     * asset_psx.c:329-331 (re15_pl00_*). The shared anim selector uses it for the
     * HP-gated injured idle (clip 22 when HP<50 / clip 23 when HP<30). */
    re15_emd_animation_t pl00_anim = {0};
    re15_emd_skeleton_t  pl00_skel = {0};
    int pl00_ok = 0;
    if (skel_ok) { pl00_anim = anim; pl00_skel = skel; pl00_ok = 1; }

    /* Save the BASE (pre-overlay) Elliot skeleton so a cross-room reload re-overlays from
     * base instead of stacking overlays (parity with the pl00_skel base saved above). */
    re15_emd_skeleton_t elliot_base_skel = elliot_skel;
    /* ... und dazu seine BASIS-ANIMATION. Ohne die laesst sich Elliot beim Raumwechsel nicht auf
     * den Ausgangszustand zuruecksetzen: der Overlay schreibt *elliot_anim komplett neu
     * (enemy_common.c re15_apply_room_cinematic), es gab aber bisher nichts, worauf man
     * zuruecksetzen koennte. Das Original braucht das nicht — dort verschwindet die Bank mit dem
     * Arena-Reset (@0x80039738) von selbst. */
    re15_emd_animation_t elliot_base_anim = elliot_anim;
    /* SHARED room-cinematic overlay (enemy_common.c) — the SAME single source of truth the
     * PSX port (re15_load_room_cinematic) and the cross-room reload below use: Leon (overlaid
     * from the clean pl00 base) + Elliot (from his base) + per-room RBJ→enemy rebind
     * (pc_enemy_load force-loads the rebind target, e.g. ROOM1150 prone Irons onto em45). */
    if (rbj_buf) {
        re15_apply_room_cinematic(rbj_buf, (size_t)rbj_size, boot_room,
                pl00_ok ? &pl00_skel : NULL, &skel, &anim,
                &elliot_base_skel, elliot_skel_ok, &elliot_skel, &elliot_anim,
                &s_cine_scratch_skel, &s_cine_scratch_anim, pc_enemy_load);
        /* RBJ-MARKER-BINDER (byte-true FUN_8001b3f8 @0x80039a08): registriert die rohe
         * RBJ-Sektion; die Record->Entity-Bindung (Marker-Bit (1+i) -> Aktor-Slot 1+i,
         * Kanal +0x180 = Executor-Sub 0) loest die Engine lazy nach den Spawns auf. */
        re15_rbj_bind_room(rbj_buf, (size_t)rbj_size);
        fprintf(stderr, "[rbj] boot room %04X cinematic overlay: %d clips, %d kf\n",
                boot_room, anim.clip_count, skel.keyframe_count);
    }

    /* (em21 special load removed 2026-06-14f — type 0x21 crows now load via the GENERIC
     * enemy loader pc_enemy_load(0x21) → EMD/EM21.EMD into their own render slot, exactly
     * like every other enemy. No per-type special case.) */

    /* RDT (room container) was loaded + parsed EARLY above (before audio/props) so
     * props/footstep/heli/pilot can be sliced from it — globalization Phase 3-A. The
     * `rdt`/`rdt_ok`/`rdt_buf` locals from there drive everything below.
     * (RVD CAM_SWITCH zones are installed in scd_register_room_events() at slot_offset
     * 16 — SCD main00's Door_aot_set(0..6)+Aot_set(7..10) overwrite slots 0-10.) */

    /* Phase 4.4: SCD VM init + start demo thread */
    scd_vm_init();
    /* Byte-true STAGE1 briefing loadout into g_inv (handgun + 2 stacks; savestate-confirmed).
     * scd_vm_init just cleared it; populate the game-start inventory here. (Per-room persistence
     * across a room_unload -> scd_vm_init is a separate concern; the briefing/combat room boots
     * with this. Phase 2b: the full inventory screen renders g_inv + the item classification.) */
    re15_inv_load_briefing();
    re15_itembox_init();   /* ITEM BOX starts empty (the RE1.5 new-game zero loop shape
                            * FUN_8003e4f4 @0x8003e52c-554; a CONTINUE load overwrites
                            * it from the v4 save block right after). */
    /* RE15_EQUIP=<item> (debug harness): equip an item at boot without the menu — input scripts
     * have no START/menu tokens, so deterministic gun probes (ammo chain, discharge fx) need this.
     * The byte-true default stays the briefing knife (aca5d=1, slot 0). */
    {
        const char *eqe = getenv("RE15_EQUIP");
        if (eqe && *eqe) {
            extern void re15_player_set_equipped_weapon(int weapon_id);
            re15_player_set_equipped_weapon((int)strtol(eqe, NULL, 0));
            fprintf(stderr, "[equip] RE15_EQUIP -> item %ld (slot %d)\n",
                    strtol(eqe, NULL, 0), re15_inv_equipped_slot());
        }
    }
    /* Load the item-icon sheet via the cwd-independent asset root (pc_read_shared) and hand it to the
     * engine — re15_asset_read_file only fopen's a raw relative path, which fails when the .exe runs
     * from the build dir (that made the icons blank AND re-tried the open every pixel = dog-slow). */
    {
        int isz = 0;
        uint8_t *ipix = pc_read_shared("DATA/ITEMALL.PIX", &isz);
        if (ipix) re15_itemall_set_pix(ipix, isz);   /* buffer intentionally kept for the program's life */
    }
    /* Item-get modal per-item PICTURE sheet (ITEM/ITPS.ITP, U11) — same cwd-independent root as the
     * icons. The modal draws the 112×72 TIM at id×0x3000 (re15_itps_pixel). */
    {
        int psz = 0;
        uint8_t *ipic = pc_read_shared("ITEM/ITPS.ITP", &psz);
        if (ipic) re15_itps_set_data(ipic, psz);     /* kept for the program's life */
    }
    scd_register_room_events(rdt_ok ? &rdt : NULL);

    /* AW-round 2026-05-28: pre-parse ROOM1170 .msg files for canonical
     * dialog display durations (PSX dialog FSM at 0x80028134 reads embedded
     * text codes 0x01 / 0x04 to set per-message timer). Without this, all
     * subtitles flash for 90 frames (1.5s); ablauf shows 7-15s per message.
     * Lookup is registered with the SCD VM via the msg-duration callback. */
    /* Per-room subtitle text + display durations come from the RDT MESSAGE block
     * (g_room_rdt.messages @ RDT+0x3c) via the shared loader — globalization Phase 3-B
     * (2026-06-13), parity with the PSX engine (asset_psx.c). Replaces the old loop that
     * read 17 separate room1170_msg##.msg files: re15_msg_load_room_block installs the
     * durations + decoded text + the SCD-VM provider for every message in the block
     * (same byte-true .msg bodies, same FUN_80028134 30Hz duration logic). */
    if (g_room_rdt_ok && g_room_rdt.messages)
        re15_msg_load_room_block(g_room_rdt.messages, g_room_rdt.messages_size);

    /* BE-round: load per-cut lighting data (light.lit) so the renderer
     * can modulate vertex tints atmospherically. Without this every cut
     * renders at full (255,255,255) — heli looks bleached, sky cuts
     * lack their dim mood, interior cuts lose the red ambient. */
    /* Per-cut lighting (NCCT) comes from the RDT LIGHT block (g_room_rdt.lights @
     * RDT+0x2c) — globalization Phase 3-B (2026-06-13), parity with PSX (asset_psx.c).
     * Replaces reading a separate room1170_light.lit file (same byte-true bytes). */
    if (g_room_rdt_ok && g_room_rdt.lights &&
        re15_light_parse(g_room_rdt.lights, (size_t)g_room_rdt.lights_size, &g_re15_room_lights) == 0) {
        g_re15_room_lights_ok = 1;
        re15_light_apply_cut(&g_re15_room_lights, 0);
        fprintf(stderr, "[light] RDT: %d cuts, cut 0 tint=(%u,%u,%u)\n",
                g_re15_room_lights.cut_count,
                g_re15_light_tint[0], g_re15_light_tint[1], g_re15_light_tint[2]);
    } else {
        fprintf(stderr, "[light] RDT light block missing — neutral tint\n");
    }

    /* [RL-1] Pre-stage ROOM1170's INTRO story flags — but ONLY on an intro-path boot (direct
     * 0x1170 debug boot, or the new-game 0x1240 montage that hands into 0x1170). These are
     * ROOM1170-specific: forcing them for a debug boot of ANY OTHER room is ROOM1170 residue
     * (z4/195 is also read by ROOM1140 → its SCD would branch as if the intro had run). Gated
     * so booting room X loads room X's own progression, zero ROOM1170 leak. (CONTINUE overwrites
     * these with the saved flags below.) */
    if (boot_room == 0x1170 || boot_room == 0x1240) {
        /* DATA-DRIVEN intro (keystone parity): set ONLY (3,193,1) — the flag sub03 would set on
         * the PRIOR visit. With (3,193)=1 AND (3,125)=0, room1170 main00 itself fires
         * Evt_exec(0x180B) → sub11 (narrator) through op_evt_exec; sub11 then sets
         * (3,125)/(4,242)/(2,7) + Cut_chg(7) + its 4 messages from its OWN bytecode. */
        re15_game_flag_set(3, 193, 1);
        /* KEIN Pre-Stage von (4,195): der fruehere `flag_set(4,195,1)` hier beruhte auf einer
         * FEHLLESUNG der 1170-Blockstruktur ("Aussenbereich sonst Dead-End"). Byte-Befund
         * (analysis/door_lock_1170.md §1/§8, ROOM1170.RDT main00): der Ifel_ck/Else_ck-Block
         * @0x0124-0x0164 umfasst NUR Slot 5 — `Else_ck 07 00 26 00` @0x0140 springt auf 0x0166,
         * und DORT beginnt Door_aot_set slot6 (`3b 06` — Datei-verifiziert) bereits HINTER dem
         * Block; Tuer 4 (->ROOM1130) und die Treppen-AOTs sind ebenfalls unconditional. Mit
         * (4,195)==0 installiert Slot 5 das Aot_set-sce-1-Zwillingsrecord (@0x012c, msg 0x0c =
         * "It's locked from the other side.", Handler LAB_80043084) statt der scharfen Tuer —
         * der byte-true Lock. Unlock NUR in ROOM1140 sub02 `Set(4,0xc3,1)` @sub-SCD 0x0190
         * (Innenseite der Tuer). Das Pre-Stage schaltete die Tuer am Treppenende frei
         * (Nutzer-Report 2026-08-03) und ist ersatzlos gestrichen. */
    }


    /* Phase 4.5.12-H: refined position from agent F1's precise reverse-
     * projection. F1 measured Leon as 65 native px (head-to-feet, not
     * head-to-wrist) and solved the floor-constrained projection for
     * cut 0 → world (+1272, -7965, +10898). vz=8166. This position is
     * along sub02's Plc_dest walk path (intermediate waypoint during
     * the helicopter intro), driven by the C-level walk solver, not a
     * static Pos_set/Member_set.
     *
     * AM-round (2026-05-26): Y corrected from -7965 to -7200. Per 30-agent
     * RE: -7965 was misappropriated from sub14's railing-area Pos_set
     * (post-cinematic monologue), but helipad floor (cinematic surface) is
     * Y=-7200 per sub15 NPC spawns (Elliot, pilot, heli all at -7200).
     * Leon was floating 765 units above floor pre-fix. */
    /* [RL-1] Per-room boot spawn — ONE keyed-off-the-room-id resolution, no ROOM1170 residue:
     *   ROOM1170 is the helicopter INTRO; its spawn is a cinematic waypoint on sub02's Plc_dest
     *   walk path (Y=-7200 helipad floor, see the AM-round derivation above) — NOT a door spawn,
     *   so it is the one explicit special case. EVERY other room uses ITS inbound-door spawn from
     *   the generated re15_room_spawns[] table — the SAME table the cross-room door transition
     *   reads (room1150 floor Y=0 etc.), so boot and reload land the player identically. */
    if (boot_room == 0x1170) {
        g_actors[RE15_ACTOR_SLOT_PLAYER].x     = 1272;
        g_actors[RE15_ACTOR_SLOT_PLAYER].y     = -7200;
        g_actors[RE15_ACTOR_SLOT_PLAYER].z     = 10898;
        g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = 0;
        fprintf(stderr, "[boot] player spawn for ROOM1170 = (1272,-7200,10898) yaw=0 (intro waypoint)\n");
    } else {
        int bidx = 0;
        for (int i = 0; i < RE15_ROOM_COUNT; i++)
            if (re15_room_ids[i] == boot_room) { bidx = i; break; }
        const re15_room_spawn_t *bsp = &re15_room_spawns[bidx];
        g_actors[RE15_ACTOR_SLOT_PLAYER].x     = bsp->x;
        g_actors[RE15_ACTOR_SLOT_PLAYER].y     = bsp->y;
        g_actors[RE15_ACTOR_SLOT_PLAYER].z     = bsp->z;
        g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = bsp->yaw;
        fprintf(stderr, "[boot] player spawn for ROOM%04X = (%d,%d,%d) yaw=%d\n",
                boot_room, bsp->x, bsp->y, bsp->z, bsp->yaw);
    }
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp     = 100;   /* RE1.5 max HP (DAT_800acaee
        init 0x64), matching the PSX build — drives the HP-gated injured idle
        (clip22 <50 / clip23 <30). Was 1024 (pre-unify), which never injured. */
    /* Collision floor band from the spawn Y (band = -(Y/0x708); ROOM1170 = 4).
     * RESTORED 2026-06-07 to push-out band 4 (the band-4-FREE complement is walkable;
     * band-4 cells are walls). */
    re15_collision_set_band(re15_collision_band_from_y(g_actors[RE15_ACTOR_SLOT_PLAYER].y));
    /* BOOT-BGM: der Startraum wird an re15_room_apply_pending VORBEI installiert (Uebergabe
     * §3 Schritt 14), also feuert der Raumwechsel-Aufruf aus room_common.c hier nicht. Im
     * Original haengt die Auswahl am Raumlader FUN_800396fc, der auch fuer den ersten Raum
     * laeuft — deshalb hier einmal mit der ECHTEN boot_room-ID nachziehen (nicht hartkodiert). */
    re15_audio_start_room_bgm((int)((boot_room >> 12) - 1), (int)((boot_room >> 4) & 0xff));
    /* Dito fuer die Raum-Sound-Baenke: re15_audio_init laeuft vor dem ersten RDT-Parse, deshalb
     * hier einmal fuer den Startraum binden (danach uebernimmt room_common.c pro Raumwechsel). */
    re15_audio_load_room_banks();
    /* PARITY STATE-INJECT (RE15_PLAYER_POS="x,z,rot"): teleport the player after spawn so a port
     * run can START from the SAME player pose as a DuckStation savestate — isolates the zombie-AI
     * parity from the (fuzzy vgamepad) input-replay. Reusable parity tool (memory: parity-oracle). */
    {
        const char *pp = getenv("RE15_PLAYER_POS");
        int px = 0, pz = 0, prot = 0;
        if (pp && *pp && sscanf(pp, "%d,%d,%d", &px, &pz, &prot) >= 2) {
            g_actors[RE15_ACTOR_SLOT_PLAYER].x     = px;
            g_actors[RE15_ACTOR_SLOT_PLAYER].z     = pz;
            g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = (int16_t)prot;
            fprintf(stderr, "[parity] player teleported to (%d,%d) rot=%d\n", px, pz, prot);
        }
    }
    /* Phase 4.5.13-A6: user-verified ground truth:
     *   0=walk, 12=fall, 15=climb, 20=march, 22=hurt1, 23=hurt2
     * Try clip 7 (113 frames, wrist at head level per J2 — 2nd-most-
     * likely wave candidate after clip 12 which user confirmed FALL).
     * Press SPACE/BACKSPACE in-game to cycle clips. */
    /* Phase 4.5.13-B1 quick-test: test.emr/test.edd now contain Leon's
     * PL00 skeleton + ROOM1170's animation.rbj cinematic keyframes (671)
     * + rbj's 25-clip EDD. sub02 uses Plc_motion(0, 15, 0) at "Hey!" so
     * motion=15 should now show the ACTUAL dialog gesture (20-frame
     * loop), not PL00's stand-up-from-prone climb. */
    /* Start in IDLE (200 → W01 clip 3, arms-down), NOT bare clip 0: motion 0 is not a
     * locomotion sentinel and would render the DEFAULT bank clip 0 = the boot room's RBJ
     * overlay clip 0 (a gesture, e.g. ROOM1150). 200 is the bank-independent idle. */
    re15_actor_set_motion(&g_actors[RE15_ACTOR_SLOT_PLAYER], 200);
    /* Cut 0 = the wide overhead helipad shot used in original.png +
     * sub02's "Hey! Hey wait!" line. Per agent B4's BMP audit cut 0
     * is the wide R-logo helipad with city skyline + neon signs. */
    g_scd.cam_id              = 0;
    g_scd.cam_change_pending  = 1;

    const uint8_t *main_pc = (rdt_ok && rdt.main_scd)
        ? rdt.main_scd
        : scd_fallback_bytecode();
    scd_thread_start(0, main_pc);

    /* Phase 4.5.13-D: ROOM1170 authentic cinematic chain wire-up.
     *
     * The "Hey! Hey wait!" intro (sub02) is gated through this
     * RE2-faithful chain (decoded from ROOM1170 scd/*.c):
     *
     *   main00         registers doors + branches on flags (3,193)+(3,125)
     *   sub00 case 0   goto sub15           (locals[10]==0 at boot ✓)
     *   sub15 Ck(4,242,1)  Evt_exec(0x1802) → sub02 ✓ cinematic
     *
     * Flag (4,242,1) is normally set by sub11 on the player's SECOND
     * room visit (after sub03 sets (3,193,1) on first visit). To play
     * the cinematic on FIRST entry without forcing a multi-room loop,
     * pre-stage the chain flags here — equivalent to "the player has
     * already completed sub03+sub11 on prior visits". The real
     * sub00→sub15→sub02 chain then fires through the unmodified
     * disassembled bytecode (no opcode hacks, no script edits). */
    /* [RL-1] (3,193) is already pre-staged by the intro-gated block above (boot 0x1170/0x1240
     * only) — no second unconditional set here, so a debug boot of another room stays clean.
     * (3,125) and (4,242) are NOT forced — sub11 sets them itself once main00 Evt_execs it;
     * sub00→sub15 then sees (4,242) and spawns the helipad cinematic (sub02). The SCD data
     * drives the progression. */

    /* sub11 (narrator) is NOT spawned here — main00 itself fires Evt_exec(0x180B)
     * → sub11 (data-driven). sub00 (→sub15→sub02 helipad) is deferred to the main
     * loop until sub11 ends (mirrors the PSX keystone; the door-3 self-reentry the
     * original uses for that handoff is the not-yet-built multi-room mechanism). */

    /* FE-4 CONTINUE resume — apply the loaded save NOW, BEFORE main00 ticks. main00
     * registers doors/AOTs, spawns Sce_em enemies and branches on story flags (Ck),
     * so the saved g_game.flags + character MUST be in place before scd_vm_tick or the
     * room initialises against new-game defaults (wrong doors/enemies/events/camera).
     * The restore overwrites the new-game scaffolding (forced intro flags, default
     * spawn/hp/band set above) with the saved state; the AOT-settle below then primes
     * edge-state from the SAVED player position. (New game: s_resume_pending == 0 → no
     * change to the intro path.) */
    int s_resume_cut = -1;   /* >=0 → apply the saved camera cut after room-init (below) */
    if (s_resume_pending) {
        uint16_t rr = 0;
        re15_savedata_restore(&s_resume_sd, &rr);
        s_resume_pending = 0;
        s_resume_cut   = (int)s_resume_sd.camera_cut;   /* restore the save-time framing/background */
        /* [PORT-ABWEICHUNG, Nutzer-Entscheid 2026-08-17] — Save-Nummer zaehlt MONOTON hoch.
         *
         * ORIGINAL (byte-true belegt, analysis/save_counter.md §0.2): der Zaehler ist Zustand
         * des laufenden SPIELSTRANGS, nicht der Karte. Der LOAD restauriert ihn WHOLESALE aus
         * dem Save-Block — `memcpy(&DAT_800b0dbc, buf, 0x1430)` @0x80026290-a0, der Zaehler
         * DAT_800b0fbd liegt bei Block+0x201 — und springt am Post-Inkrement VORBEI
         * (@0x800262a4 `j 0x800264a0`; das Inkrement selbst @0x8002648c `lbu` / @0x80026494
         * `addiu 1` / @0x8002649c `sb` laeuft nur im Save-Erfolgszweig @0x80026230). Gespeichert
         * wird immer der PRE-Inkrement-Stand (Write-Quelle ist Live-RAM 0x800b0dbc,
         * @0x800261ec/@0x80026224). FOLGE IM ORIGINAL: der erste Save nach einem Load traegt
         * NOCHMAL dieselbe Nummer; wer nur "laden -> einmal speichern -> beenden" spielt, sieht
         * die Nummer nie steigen.
         *
         * ENTSCHEIDUNG (wie bei den Ortsnamen 2026-08-08, Muster re15_savepoint.c): der Port
         * weicht hier BEWUSST ab und restauriert den geladenen Stand + 1, damit auch der erste
         * Save nach einem Load hochzaehlt. Das ist die EINZIGE Abweichung — alles andere bleibt
         * byte-true: neues Spiel startet bei 0 (EXE-Image @Datei 0xa17bd = 00, main.c NEW-GAME-
         * Zweig), gespeichert wird weiter PRE-Inkrement, und das Post-Inkrement nach erfolgreichem
         * Write (@0x8002648c-9c) bleibt unveraendert. Kein Karten-/Slot-Wert geht in die Nummer
         * ein (das Original liest sie dort NIE). Eingefroren von integration_save_counter_pin. */
        s_save_counter = (uint16_t)(s_resume_sd.save_count + 1);
        /* Re-prime the ARMS SE bank to the restored weapon: the room-init primed the default
         * (bank1), and re15_player_set_equipped_weapon (in restore) sets the weapon id but not the
         * cached SE bank — so a save with a gun equipped would fire the wrong SE without this. */
        { extern void re15_audio_prime_weapon(int weapon_id);
          extern int  re15_player_equipped_weapon(void);
          int wid = re15_player_equipped_weapon();
          if (wid >= 1) re15_audio_prime_weapon(wid); }
        fprintf(stderr, "[save] CONTINUE: resumed in room %04x (hp=%d)\n", rr, g_actors[0].hp);
    }

    /* MESS-HAKEN RE15_SET_FLAG="<bank>:<bit>[,<bank>:<bit>...]" (bit dezimal oder 0x-hex) —
     * setzt Story-Flags EINMAL beim Spielstart, damit ein Messlauf einen SPAETEREN
     * Fortschrittsstand nachstellen kann, ohne die halbe Raumkette zu spielen. Gesetzt werden
     * ausschliesslich Flags, die das Spiel selbst per `Set` schreibt (z.B. ROOM1090 sub03
     * @0x24D2 `Set(3,0xbb,1)`, das die vier Zombies in sub00 @0x2376 freischaltet). Reiner
     * Messhaken, env-gegated, kein Spielverhalten. */
    { const char *sf = getenv("RE15_SET_FLAG");
      if (sf && *sf) { const char *p = sf;
          while (*p) { int bank = (int)strtol(p, (char **)&p, 0);
              while (*p == ':' || *p == ' ') p++;
              int bit = (int)strtol(p, (char **)&p, 0);
              if (bank >= 0 && bank < 32 && bit >= 0 && bit < 256) {
                  re15_game_flag_set((uint8_t)bank, (uint8_t)bit, 1);
                  fprintf(stderr, "[setflag] flag(%d,%d/0x%02x) = 1\n", bank, bit, bit);
              }
              while (*p && *p != ',') p++;
              while (*p == ',' || *p == ' ') p++;
          } } }

    /* RVD-AUTO-SCAN AN bei JEDEM Raum-Start — auch Session-Boot/LOAD (byte-true):
     * FUN_800396fc laeuft bei jedem Raum-Start — Tuerpfad `jal FUN_800396fc` @0x8001d988
     * (FUN_8001d600) UND Session-Start/LOAD `jal` @0x8001d5ac (FUN_8001d22c; der LOAD
     * bootet durch dieselbe Kette, Save-Record statt New-Game-Defaults). Ihr Kopf loescht
     * das Scan-Gate: `lw v0,DAT_800aca3c` @0x8003970c, `lui v1,0xffff` @0x80039710,
     * `and v0,v0,v1` @0x80039728, `sw v0,DAT_800aca3c` @0x80039730 — Bit 0x100 liegt in
     * der unteren Haelfte → RVD-Scan AN. ERST DANACH ruft dieselbe Funktion die
     * SCD-Raum-Init `jal FUN_8003ef6c` @0x80039a00 — die Skripte (Cut_chg LAB_800402a0
     * `ori 0x100` @0x800402d4 setzt das Bit wieder / Cut_old/Cut_auto geben frei) haben
     * das LETZTE Wort. Der Tuerpfad des Ports hat exakt diese Reihenfolge schon
     * (scd_room_reenter: cut_auto_enabled=1 @scd_room_setup.c:114, dann Init-Tick);
     * der Boot-/CONTINUE-Pfad hier liess das Gate faelschlich AUS (gemessen Run C0:
     * Save in 1150 @cut 6, Spieler 60 s idle IN der 6->0-RVD-Zone, kein Pick). */
    g_scd.cut_auto_enabled = 1;

    /* Phase 4.5.12: prime AOT edge-state from spawn pos so door zones
     * the player materializes inside don't auto-trigger on frame 1.
     * (Tick the SCD VM once first so main00's Door_aot_set / Aot_set
     * registrations are present — and so main00's Evt_exec(0x180B) spawns sub11.) */
    scd_vm_tick();
    re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                       g_actors[RE15_ACTOR_SLOT_PLAYER].z);

    /* main00's boot tick fired Evt_exec(0x180B) → sub11 into an event slot. Detect
     * it: a running narrator means we're in the pre-intro and sub00 (helipad) is
     * held until it ends. If none spawned (flags already advanced), run sub00 now. */
    int s_preintro = 0, s_sub11_slot = -1, s_sub00_spawned = 0;
    for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++) {
        if (g_scd.threads[s].active) { s_sub11_slot = s; s_preintro = 1; break; }
    }
    if (!s_preintro && rdt_ok && rdt.sub_scd[0]) {
        scd_thread_start(1, rdt.sub_scd[0]);
        s_sub00_spawned = 1;
        /* sub00 SOFORT einmal laufen lassen — als INIT-Lauf, nicht als Gameplay-Frame.
         * Im Original startet die SCD-Raum-Init FUN_8003ef6c Slot 1 mit sub00 (a1 = 0,
         * @0x8003efd8) und ruft den Dispatcher DIREKT (@0x8003f018); sub00 bekommt damit genau
         * einen Lauf, bevor ab dem ersten Gameplay-Frame FUN_8003f038 den Slot permanent mit
         * sub01 ueberschreibt. Der Boot-Pfad hier startet sub00 erst NACH seinem Init-Tick (die
         * Pre-Intro-Erkennung oben braucht dessen Ergebnis) — ohne diesen Lauf wuerde der
         * Per-Frame-Reseed in scd_vm_tick sub00 verdraengen, BEVOR es sein erstes Opcode
         * ausfuehrt (gemessen an ROOM1150: sub00s Evt_exec spawnte keinen Thread mehr).
         * scd_room_reenter macht an derselben Stelle genau dasselbe. */
        scd_vm_set_room_init(1);
        scd_vm_tick();
        scd_vm_set_room_init(0);
        /* KEIN re15_audio_start_room_bgm(0, 0x17) mehr. Das war hart auf ROOM1170 verdrahtet und
         * lief zusammen mit dem prozess-globalen One-Shot darauf hinaus, dass der ERSTE Aufruf
         * die BGM fuer die ganze Session festnagelt — in ROOM1240 spielte deshalb der Track von
         * ROOM1170 (gemeldet 2026-08-01). Das Original waehlt die BGM im RAUMLADER
         * FUN_800396fc: @0x8003971C FUN_800443ec (KeyOff + Fade-Start) und @0x800397xx
         * FUN_80044210 (Latch + Laden) — also pro Raum aus der Tabelle, nie hartkodiert.
         * Der Port macht das jetzt in room_common.c beim Raumwechsel. */
    }

    /* FE-4 CONTINUE: restore the SAVE-TIME camera cut LAST — after the room default (cam_id=0
     * above) and after main00/sub00, either of which may issue its own Cut_chg. On a load there
     * is no door to set the entry cut and the player is teleported (not walked) to the saved
     * position, so the per-frame RVD transition scan does not fire to correct cut 0 → the saved
     * position renders under the wrong camera + wrong background. Re-asserting the stored cut is
     * the load-path equivalent of the door's target_cut. (New game: s_resume_cut < 0 → untouched.) */
    if (s_resume_cut >= 0) {
        g_scd.cam_id             = (uint8_t)s_resume_cut;
        g_scd.cam_change_pending = 1;
    }

    /* BOOT-BG-PRELOAD — NACH Raum-Init + Entry-Cut, byte-true zur Raumlader-Reihenfolge.
     * Gemessen (RE15_FADE_LOG+bg-log, 2026-08-10, Flow Title->NEW GAME->Select->Boot):
     * der alte Preload stand VOR `g_current_room_id = boot_room` und cachte deshalb
     * ROOM1170#00 (g_current_room_id-Default, room_common.c:34); Game-Frame 0 blittet den
     * Cache VOR dem Cut-Wechsel-Block (Blit ~L3069 < Load ~L3433) -> 1 ungedeckter Frame
     * ROOM1170-Helipad zwischen Select-Schwarz und den ROOM1240-Montage-Stills
     * (Nutzer-Report v0.1.1). Das Original kennt diesen Frame nicht: der Raumlader
     * FUN_8001d600 setzt ZUERST den Modus-2-SCHWARZ-Clear (FUN_80021634(2,0),
     * Boot-Zweig @0x8001d620-28, Tuer-Zweig @0x8001d830-34; Game-Init FUN_800161e0
     * killt zusaetzlich Fade-Kanal 0 @0x80016420 + Schwarz-Clear @0x80016424), laedt DANN
     * Raum (FUN_800396fc) + Entry-Cut-BG (FUN_80013c50 @0x8001da80; Boot-Cut = 0 via
     * DAT_800b0fe4=0, Tuer = Ziel-Cut DAT_800afbb5), wartet die Loads ab
     * (@0x8001d850-868 Fade 0x10000; @0x8001dabc-d4 BG 0x2000000) und gibt den BG erst
     * DANACH frei (FUN_80021634(0,0) @0x8001dadc-e0 + DAT_800b5457=1 @0x8001dae4-ec).
     * PC-Aequivalent (Loads synchron, das Schwarz-Fenster hat hier null Frames): den
     * Entry-Cut-BG des BOOT-Raums JETZT laden — g_current_room_id ist der Boot-Raum,
     * g_scd.cam_id der Entry-Cut (NEW GAME = 0 wie DAT_800b0fe4=0; CONTINUE = Save-Cut).
     * Frame 0 blittet damit denselben BG, den das Original als ersten Frame zeigt
     * (ROOM1240 Cut 0 = schwarz dekodierendes MDEC-Still BG00). Kein erfundener Fade.
     *
     * FEHLSCHLAG = SCHWARZ HALTEN (Fix Report "ROOM1170-Blitzer nach Charakterwahl", 2026-08-17).
     * Der alte Fallback `re15_bg_load_test_asset()` malte bei JEDEM transienten Load-Fehlschlag
     * `DATA/TEST.BSS` in den BG-Cache — und TEST.BSS ist BYTE-IDENTISCH zu BSS/ROOM1170/BG00.BSS
     * (MD5 beider Dateien gleich, Recherche-Lane 2026-08-17), also das HELIPAD. Der Cache wird ab
     * Frame 0 geblittet und erst beim naechsten erfolgreichen Cut-Load ersetzt (fruehestens F161)
     * -> genau der gemeldete ROOM1170-Blitzer nach der Charakterwahl.
     * Das Original kennt keinen Ersatz-BG: der Raumlader haelt den MODE-2-SCHWARZ-Clear
     * (`ori a0,0x2; jal 0x80021634; addu a1,zero,zero` @0x8001d620-28, Tuer-Zweig @0x8001d830-34)
     * und gibt den BG erst NACH den Load-Waits frei (`jal 0x80021634` mit a0=0/a1=0 @0x8001dadc-e0
     * + `DAT_800b5457 = 1` @0x8001dae4-ec; BG-Wait-Schleife @0x8001dabc-d4, Maske 0x2000000) —
     * bleibt der Load aus, bleibt das Bild SCHWARZ. Der Port-Aequivalent: Cache verwerfen; die
     * Frame-Schleife malt dann `re15_render_background_gradient(0,0,0,0,0,0)` (main.c ~L3193).
     * Das Log ist bewusst UNGEGATET (kein RE15_FADE_LOG noetig): ein echter Load-Fehlschlag beim
     * Nutzer soll im debug.log selbstbeweisend sein. (DATA/TEST.BSS bleibt im Asset-Baum — der
     * Disc-Spiegel ist vollstaendig; nur der Code malt es nicht mehr als BG.) */
    if (re15_bg_load_cut((int)g_scd.cam_id) != 0) {
        re15_bg_invalidate();     /* BLACK HOLD (@0x8001d620-28 .. @0x8001dadc-ec), NIE TEST.BSS */
        fprintf(stderr, "[bg] BOOT-BG LOAD FAILED room=%04x cut=%d -> holding BLACK "
                        "(byte-true @0x8001d620-28; no TEST.BSS fallback)\n",
                g_current_room_id, (int)g_scd.cam_id);
    }

    g_engine.frame_count = 0;

    /* DEBUG-HARNESS: RE15_BOOT_EXIT_AT=<n> beendet den Prozess beim n-ten GAME-BOOT (1-basiert).
     * Der Tod fuehrt IN-PROCESS zum Titel zurueck (`goto re_title`) und der ganze Boot-Block laeuft
     * erneut — ohne Abbruchbedingung laeuft ein Tod->Titel->NEW-GAME-Harness endlos. Mit n=3 laufen
     * Boot 1 und Boot 2 VOLLSTAENDIG durch (das ist der Zyklus, in dem prozess-lange `static`-Latches
     * auffliegen), dann endet der Prozess sauber. Reiner Testhaken, kein Spielverhalten. */
    { static int s_boot_n = 0; static int s_boot_exit_at = -2;
      if (s_boot_exit_at == -2) { const char *be = getenv("RE15_BOOT_EXIT_AT");
                                  s_boot_exit_at = (be && *be) ? atoi(be) : -1; }
      s_boot_n++;
      if (s_boot_exit_at > 0 && s_boot_n >= s_boot_exit_at) {
          fprintf(stderr, "[flow] BOOT_EXIT_AT: boot #%d reached -> exit\n", s_boot_n);
          fflush(stderr); exit(0);
      } }

    { extern int re15_fade_log_on(void);
      if (re15_fade_log_on())
          fprintf(stderr, "[flow] game loop start room=%04x preintro=%d\n",
                  g_current_room_id, s_preintro); }

    int sx = 32, sy = 32, sdx = 1, sdy = 1;

    /* `running` is never set to 0 — SDL_QUIT triggers exit(0) inside
     * re15_render_begin_frame — but having a real loop variable avoids
     * the "for(;;) → unreachable return" + "no return from non-void"
     * compiler warning split between MSVC and gcc. */
    volatile int running = 1;
    /* (FE-4 CONTINUE resume is applied above, before scd_vm_tick, so main00 initialises
     * the room from the saved story flags — not new-game defaults.) */

    /* debug: RE15_GIVE_CARD drops a MEMORY CARD in inventory; RE15_SAVE_TEST also fires a
     * save-point this frame (exercise the save flow without navigating to a phone). GIVE_CARD
     * alone lets a REAL phone examine be tested (walk onto the phone AOT + press SQUARE). */
    if (getenv("RE15_GIVE_CARD")) {   /* debug: drop a MEMORY CARD so the consume-per-save path is exercised */
        for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
            if (g_inv.slots[i].id == 0) { g_inv.slots[i].id = 0x21; g_inv.slots[i].qty = 2; break; }
    }
    if (getenv("RE15_SAVE_TEST")) re15_savepoint_set_pending(1);   /* fire a save-point this frame (card not required) */
    if (getenv("RE15_BOX_TEST"))  re15_itembox_set_pending(1);     /* fire a box AOT this frame (exercise the full
                                                                    * request -> stage freeze/fade -> box-screen path) */

    while (running) {
        /* FE-4 phone SAVE (outside the game frame): a save-point phone was examined?
         * gate on the MEMORY CARD item (0x21, RE1.5's ink-ribbon equivalent) — if held,
         * open the save screen and consume one card on a successful save. */
        /* TEST-HAKEN (kein Spielverhalten): RE15_SAVE_TEST_AGAIN=<frame> feuert einen ZWEITEN
         * Save-Point im selben Prozess. Der Save-Zaehler ist Sitzungs-Zustand (DAT_800b0fbd),
         * also braucht sein Pin zwei Saves in EINER Sitzung. */
        { static int s_save_again = -2;
          if (s_save_again == -2) { const char *sa = getenv("RE15_SAVE_TEST_AGAIN");
                                    s_save_again = (sa && *sa) ? atoi(sa) : -1; }
          if (s_save_again >= 0 && (int)g_engine.frame_count == s_save_again)
              re15_savepoint_set_pending(1); }
        if (re15_savepoint_pending()) {
            re15_savepoint_set_pending(0);
            /* Examining a save-point phone ALWAYS opens the save screen (replacing the dormant RE1.5
             * "you can save your progress with this — save is not available in this preview" flavor
             * message). The MEMORY CARD (0x21, RE1.5's ink-ribbon equivalent) is consumed per save WHEN
             * held; but since the accessible RE1.5 content has no card pickup, saving is not hard-gated
             * on it (otherwise the whole feature is unreachable). */
            /* FULLY dismiss the flavor message (same reset as the msg-FSM DONE state,
             * msg_common.c:467). Clearing ONLY message_active leaves message_display_frames > 0,
             * which msg_block (player_common.c:306) gates the player on — so after the save screen
             * closed, Leon stayed frozen forever (msg_tick no longer runs to count it down). */
            g_scd.message_active         = 0;         /* suppress the flavor message, open the menu */
            g_scd.message_select         = 0;
            g_scd.message_fsm_active     = 0;
            g_scd.message_display_frames = 0;
            g_scd.message_query          = 0;
            int mc = -1;
            for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
                if (g_inv.slots[i].id == 0x21 && g_inv.slots[i].qty > 0) { mc = i; break; }
            re15_savedata_t sd;
            /* BYTE-TRUE Zaehler-Semantik (analysis/save_counter.md SC-1/SC-3, CONFIRMED):
             * Der Save speichert den LIVE-Zaehler DAT_800b0fbd PRE-Inkrement (GSB-memcpy
             * @0x800261c4-d8 + Titel /NN/ @0x80026eac-f0 laufen VOR dem Karten-Write; das
             * Inkrement folgt NUR nach Erfolg @0x80026488-9c). Frisches Spiel = 0 (EXE-Image
             * @0xa17bd), LOAD restauriert ihn wholesale — die Karte/der Ziel-Slot wird fuer
             * die Nummer NIE gelesen. Der alte Port-Seed aus dem Karten-Maximum
             * (re15_memcard_max_save_count) war erfunden und liess New-Game-Saves den
             * Zaehler alter Slots erben (Nutzer-Report); ersatzlos gestrichen. Erster Save
             * eines frischen Spiels heisst /00/ (Original), nicht /01/. */
            int scount = (int)s_save_counter;
            re15_savedata_capture(&sd, g_engine.frame_count, (uint16_t)scount);
            /* Store the GAMEPLAY cut latched when the phone was examined (before its sub's Cut_chg to
             * the desk close-up), not the live cut sampled here — which is the transient close-up. The
             * original loads the gameplay perspective, not the close-up. re15_savedata_capture seeded
             * camera_cut from the live cam_id; override with the latched cut + recompute the checksum. */
            { int gc = re15_savepoint_saved_cut();
              if (gc >= 0) { sd.camera_cut = (uint8_t)gc; sd.checksum = re15_savedata_checksum(&sd); } }
            /* Byte-true: the card (RE1.5's ink-ribbon) consumed by THIS save is part of the saved
             * state — reflect the decrement in the CAPTURED block so reloading doesn't hand it back
             * (the live inventory is decremented on success below). Recompute the checksum. */
            if (mc >= 0) {
                if (sd.inv[mc].qty > 0) sd.inv[mc].qty--;
                if (sd.inv[mc].qty == 0) { sd.inv[mc].id = 0; sd.inv[mc].flags = 0; }
                sd.checksum = re15_savedata_checksum(&sd);
            }
            if (pc_run_memcard_screen(1, &sd, 0) >= 0) {
                s_save_counter = (uint16_t)(scount + 1);   /* DAT_800b0fbd++ NUR nach Erfolg
                                                            * (@0x80026488-9c, Post-Inkrement) */
                if (mc >= 0 && --g_inv.slots[mc].qty == 0) { g_inv.slots[mc].id = 0; g_inv.slots[mc].flags = 0; }
                /* n = die GESCHRIEBENE Nummer (Pre-Inkrement, @0x800261c4-d8/@0x80026eac-f0),
                 * next = der Sitzungs-Zaehler danach (Post-Inkrement @0x80026488-9c). */
                fprintf(stderr, "[save] saved (room %04x) slot n=%d -> next=%u; card=%s\n",
                        g_current_room_id, scount, (unsigned)s_save_counter,
                        mc >= 0 ? "consumed" : "none");
                /* TEST-HAKEN (kein Spielverhalten): RE15_SAVE_TEST_EXIT_AFTER=<n> beendet den
                 * Prozess, sobald n Saves geschrieben sind — deterministisches Lauf-Ende fuer
                 * den Zaehler-Pin (die Karte ist hier bereits geschlossen geschrieben). */
                { static int s_saves_done = 0, s_save_exit = -2;
                  if (s_save_exit == -2) { const char *se = getenv("RE15_SAVE_TEST_EXIT_AFTER");
                                           s_save_exit = (se && *se) ? atoi(se) : -1; }
                  if (s_save_exit > 0 && ++s_saves_done >= s_save_exit) {
                      fprintf(stderr, "[save] SAVE_TEST_EXIT_AFTER: %d saves -> exit\n", s_saves_done);
                      fflush(stderr); exit(0);
                  } }
            }
            /* No post-save re-examine cooldown: the examine fires only on a fresh action-button
             * EDGE (a held button cannot re-fire), so each DELIBERATE press re-opens the menu with
             * no dead period. (A prior 90f cooldown swallowed clicks within ~3s of closing the menu.) */
        }

        /* ITEM BOX (RE1.5-hybrid, default-on — the save-phone precedent): a safe-room
         * box AOT was examined? The flavor message was suppressed at the SCD level
         * (scd_vm.c intercept; RE15_BOX_PREVIEW_MSG=1 restores the shipped message
         * byte-true) — open the box subscreen through the SHARED menu transition
         * (freeze + stage fade-out/hold-black, menu_common.c re15_menu_request_box). */
        if (re15_itembox_pending()) {
            re15_itembox_set_pending(0);
            re15_menu_request_box();
        }

        re15_render_begin_frame();
        re15_input_tick();

        /* DEBUG: RE15_KILL_AT=<frame> drops the player's HP to 0 at that frame to exercise the
         * death FSM + FE-5.3 death->TITLE mode-cycle deterministically (no combat-path tuning). */
        { static int s_kill_at = -2;
          if (s_kill_at == -2) { const char *k = getenv("RE15_KILL_AT"); s_kill_at = k ? atoi(k) : -1; }
          if (s_kill_at >= 0 && (int)g_engine.frame_count == s_kill_at) g_actors[RE15_ACTOR_SLOT_PLAYER].hp = -1; }  /* dead = hp<0 */

        /* DEBUG: RE15_INV_SHOT=<path> — wave-1 acceptance harness: force-open the status
         * screen at frame 30 (briefing loadout = the mzd_inv_open.sav items); the render
         * block puts the screen into the savestate's acceptance state; the shot is taken
         * after end_frame at frame 34, then exit.
         * RE15_INV_GRID_SHOT=1 (wave 2): additionally inject a SQUARE edge at frame 31 —
         * the tab-select ITEM confirm (VIRTUAL 0x4000 <- RAW SQUARE, @0x80073dbc[14];
         * wave-6 finding 4 flipped these injections from CROSS) — so the live FSM runs
         * the entry slide (7 frames, +14/frame @0x8004a394) into GRID mode; the shot is
         * taken at frame 50 instead. */
        if (getenv("RE15_INV_SHOT") && !getenv("RE15_INV_MIX_SHOT") &&
            g_engine.frame_count == 30 && !re15_menu_is_open())
            re15_menu_toggle();
        if (getenv("RE15_INV_GRID_SHOT") && g_engine.frame_count == 31)
            g_engine.pad_pressed |= RE15_PAD_BIT_SQUARE;
        /* DEBUG: RE15_BOX_SHOT=1 — ITEM BOX acceptance harness: instant box-screen
         * open at F30 (briefing loadout), confirm at F32 (inventory side -> box
         * side), confirm at F34 (swap: cursor-0 item deposited into box page 0
         * slot 0 via the RE2-ported transfer), auto-exit at F90. Combine with
         * RE15_INV_FB_SHOT=shots/itembox_port.bmp [+RE15_INV_FB_SHOT_AT=<n>]
         * for the software-framebuffer pixel dump. */
        if (getenv("RE15_BOX_SHOT")) {
            if (g_engine.frame_count == 30 && !re15_menu_is_open())
                re15_menu_toggle_box();
            if (g_engine.frame_count == 32 || g_engine.frame_count == 34)
                g_engine.pad_pressed |= RE15_PAD_BIT_SQUARE;
            if (g_engine.frame_count >= 90) running = 0;
        }
        /* RE15_INV_CMD_SHOT=1 (wave 3): SQUARE at F31 (tab ITEM confirm -> entry slide
         * F32-38, GRID F39) + SQUARE at F45 (grid confirm on the cursor slot -> 25d6=0 +
         * 25c2=3, command slide-in F46-52, state 4 from F53); shot at F60 = the command
         * stage with the g7 label (cursor 0 == equipped 0 -> unequip face uv(0x10,0xa0)). */
        if (getenv("RE15_INV_CMD_SHOT") &&
            (g_engine.frame_count == 31 || g_engine.frame_count == 45))
            g_engine.pad_pressed |= RE15_PAD_BIT_SQUARE;
        /* RE15_INV_CHECK_SHOT=1 (wave 4): SQUARE F31 (ITEM) + SQUARE F45 (grid confirm ->
         * command stage from F53) + LEFT F54 (25d6=1 CHECK) + SQUARE F56 (dispatch [1]
         * @0x8004a558-64: 25d6=0 + 25c2=9). CHECK FSM: slide-out F57-63 (panels -36/f,
         * 25ee 166->264), photo load + struct init F64, panel slide-in F65-74 (+22/f,
         * x=13 + desc-msg open at F74), typewriter 2f + 4f/glyph (knife entry 1 = 15
         * ticks -> complete at F132), PRESS-WAIT after; shot at F140 = the settled
         * examine screen (photo + full desc). */
        if (getenv("RE15_INV_CHECK_SHOT")) {
            if (g_engine.frame_count == 31 || g_engine.frame_count == 45 ||
                g_engine.frame_count == 56)
                g_engine.pad_pressed |= RE15_PAD_BIT_SQUARE;
            if (g_engine.frame_count == 54)
                g_engine.pad_pressed |= RE15_PAD_BIT_LEFT;
        }
        /* RE15_INV_FILE_SHOT=1 (FILE wave): R1 at F31 = the instant FILE launch
         * (@0x80049834-4c: tab=3 + 25c1=2). FUN_800c6ca0: enter slide F32-61 (30
         * frames @0x800c6cdc), transition F62 (25c2=1), list after; shot at F75 =
         * the page-0 row list ("Files": row 0 Chris' Diary, rows 1-9 underscores,
         * title highlight box).
         * RE15_INV_FILE_DOC_SHOT=1: additionally SQUARE F70 (row select @0x800c6ea0)
         * + SQUARE F72 (open reader @0x800c704c: 25c2=3, page 0); shot at F85 = the
         * reader title card ("Operation Report" centered, footer 1/7, right arrow). */
        if ((getenv("RE15_INV_FILE_SHOT") || getenv("RE15_INV_FILE_DOC_SHOT")) &&
            g_engine.frame_count == 31)
            g_engine.pad_pressed |= RE15_PAD_BIT_R1;
        if (getenv("RE15_INV_FILE_DOC_SHOT") &&
            (g_engine.frame_count == 70 || g_engine.frame_count == 72))
            g_engine.pad_pressed |= RE15_PAD_BIT_SQUARE;
        /* RE15_INV_MAP_SHOT=1 (MAP wave): L1 at F31 = the instant MAP launch
         * (@0x8004980c-30: tab=1 + 25c1=1 + entry init/CD-load dispatch). FUN_8004c058:
         * slide-out F32-56 (25 frames @0x8004c0bc), upload+arena F57 (c2=1), interactive
         * after; shot at F75 = the settled map viewer (page per the current room's
         * stage init; ROOM1140 -> page 4 = MAP05.PIX, marker at the slot-20 row). */
        /* RE15_INV_OPEN_AT=<frame> — wie RE15_INV_SHOT/RE15_INV_MAP_SHOT, nur mit
         * FREIEM Zeitpunkt: oeffnet den Statusbildschirm im genannten Frame und
         * startet zwei Frames spaeter per L1 den MAP-Schirm. Gebraucht, um die Karte
         * in einem Raum zu begutachten, den man erst per RE15_DEBUG_JUMP erreicht
         * (die festen F30/F31 der Schienen oben liegen vor jedem Raumwechsel).
         * Zusammen mit RE15_INV_FB_SHOT=<datei.bmp> + RE15_INV_FB_SHOT_AT=<frame>
         * ergibt das ein pixelgenaues Bild dessen, was der Kartenschirm zeichnet. */
        {
            const char *oa = getenv("RE15_INV_OPEN_AT");
            if (oa && *oa) {
                int of = atoi(oa);
                if ((int)g_engine.frame_count == of && !re15_menu_is_open())
                    re15_menu_toggle();
                if ((int)g_engine.frame_count == of + 2)
                    g_engine.pad_pressed |= RE15_PAD_BIT_L1;
            }
        }
        if (getenv("RE15_INV_MAP_SHOT") && g_engine.frame_count == 31)
            g_engine.pad_pressed |= RE15_PAD_BIT_L1;
        /* RE15_INV_MIX_SHOT=1 (wave 5): seed Green 0x24 + Red 0x25 at F40 + open; SQUARE
         * F41 (tab ITEM confirm, virtual 0x4000 <- RAW SQUARE -> entry slide F42-48,
         * GRID F50) + SQUARE F50 (grid confirm on the Green -> 25d6=0 + 25c2=3, slide-in
         * F51-57, state 4 F58) +
         * RIGHT F59 (25d6=3 EXCHANGE) + SQUARE F61 (dispatch [3] @0x8004a570: 25c2=7) +
         * RIGHT F63 pressed+held (second cursor FUN_80048904: 0 -> 1 = the Red) +
         * SQUARE F65 (confirm: matcher pair 0x24+0x25 -> result 0x27 pic 1 @0x80074d54,
         * executor action 1 mix-merge into slot 0 + compaction; result anim c3=1 runs
         * F66-82: grow F66-73, shrink F74-81, terminal F82 -> state 6 -> GRID F90).
         * Shot at F100 = the settled grid with G.R MEDICINE MIX (MIXITEM tile 1 art +
         * qty 2 + name print). */
        if (getenv("RE15_INV_MIX_SHOT")) {
            if (g_engine.frame_count == 40 && !re15_menu_is_open()) {
                re15_inv_init();
                g_inv.slots[0].id = 0x24; g_inv.slots[0].qty = 1;   /* GREEN MEDICINE */
                g_inv.slots[1].id = 0x25; g_inv.slots[1].qty = 1;   /* RED MEDICINE   */
                re15_menu_toggle();
            }
            if (g_engine.frame_count == 41 || g_engine.frame_count == 50 ||
                g_engine.frame_count == 61 || g_engine.frame_count == 65)
                g_engine.pad_pressed |= RE15_PAD_BIT_SQUARE;
            if (g_engine.frame_count == 59)
                g_engine.pad_pressed |= RE15_PAD_BIT_RIGHT;
            if (g_engine.frame_count == 63) {
                g_engine.pad_pressed |= RE15_PAD_BIT_RIGHT;
                g_engine.pad_current |= RE15_PAD_BIT_RIGHT;
            }
        }

        /* FE-5.1/5.2: track the START-menu pause/inventory in the FE-0 mode machine. The status/
         * inventory screen (re15_menu_toggle) freezes the world — byte-true inline behavior in
         * re15_game_step is unchanged; the mode now reflects it (INVENTORY while up, INGAME while
         * playing). The death block below sets TITLE + exits, which supersedes this. */
        { extern int re15_menu_is_open(void);
          if (re15_gameflow_mode() != RE15_MODE_TITLE)
              g_gameflow.mode = re15_menu_is_open() ? RE15_MODE_INVENTORY : RE15_MODE_INGAME; }

        /* RAUM-TRANSITIONS-FSM (FUN_8001c958 States 4/5) — MUSS vor allen Subsystemen
         * laufen: im Original dispatcht die FSM @0x8001c994, die Subsystem-Aufrufe
         * (SCD @0x8001cdec, Spieler @0x8001ce0c, AOT @0x8001ce1c, Action-Driver
         * @0x8001ce2c) folgen erst danach und gaten sich selbst am Pause-Wort.
         * State 4 (@0x8001cc34-6c) friert mit `g_pauseflags |= 0xff000000` ein,
         * State 5 (@0x8001cc70-94) gibt frei, sobald die -0x1800-Einblendung durch
         * ist (FUN_8002178c). Belege vollstaendig in room_common.c. */
        re15_room_transition_tick();

        /* Phase 4.5.6.4: paint cached MDEC BG into the software
         * framebuffer (replaces the gradient when an asset loaded).
         * Match PSX flow: BG first, meshes/HUD layer on top. */
        if (re15_bg_is_loaded()) {
            /* MONTAGE-PRAESENTATION (Nutzer-Auftrag 2026-08-30: RE2s Pre-Intro uebernehmen).
             * Nur im Standbild-Raum ROOM1240: statt des 1-Frame-Hartschnitts blendet das
             * neue Bild additiv auf (RE2-Rampe +2/Frame bis 0x60 = 48 Frames, Prim 0x2E +
             * TPAGE-ABR=1 @0x801c1b94-c24) waehrend das vorige ausblendet, mit vertikalem
             * Wandern bzw. zentriertem Zoom je Bild (re15_montage_fx.c). RE15_MONTAGE_STOCK=1
             * = byte-true Hartschnitt. Der Erzaehler-Raum 1170 bleibt unberuehrt. */
            re15_montage_fx_set_active(g_current_room_id == 0x1240);
            if (re15_montage_fx_active()) {
                re15_montage_fx_tick();
                re15_bg_blit_montage(re15_montage_fx_level_new(),
                                     re15_montage_fx_level_prev(),
                                     re15_montage_fx_pan_y(),
                                     re15_montage_fx_base_zoom() + re15_montage_fx_zoom_px(),
                                     re15_montage_fx_prev_pan_y(),
                                     re15_montage_fx_prev_base_zoom()
                                         + re15_montage_fx_prev_zoom_px());
            } else {
                re15_bg_blit(0, 0);
            }
        } else {
            /* No room MDEC background yet (room-load gap / the LOAD->resume transition): the original
             * is CUT-to-black + fade-in (see reai-v2-door-transition), so a not-yet-loaded BG is BLACK,
             * not the old bright-blue dev placeholder that flashed blue "beim Laden". */
            re15_render_background_gradient(0, 0, 0, 0, 0, 0);
        }

        /* PRE-INTRO → HELIPAD handoff: once main00's narrator (sub11, in event slot
         * s_sub11_slot) ends, fire the FAITHFUL door-3 self-reentry (sub11's Aot_on(3)
         * → door 3, dest = THIS room): re-run the room's init(main00)+main(sub00) with
         * sub11's flags persisting → main00 Ck(3,125,1)→BGM, sub00→sub15 Ck(4,242,1) →
         * Evt_exec(0x1802) helipad cinematic. (Replaces the hand-deferred sub00 spawn.)
         * Start the BGM bank first, then re-enter. Frame cap = safety. */
        /* FALLBACK ONLY: the byte-true intro handoff is sub11's Aot_on(3) → door 3 → the
         * game-step self-room scenario reenter (which sets g_scd_self_reenter_fired). That is
         * the SINGLE reenter the original performs. This hand-deferred block predates that path
         * and must NOT fire a redundant SECOND reenter — after sub02 cleared (4,242), a second
         * sub00→sub15 takes the ELSE branch and spawns the 7 intro crows (measured F900). So gate
         * it on the latch: fire only if the faithful door-3 reenter has NOT already happened
         * (pathological "door 3 never fired" safety net; frame cap kept as a last resort). */
        if (s_preintro && !s_sub00_spawned && !g_scd_self_reenter_fired &&
            (!g_scd.threads[s_sub11_slot].active ||
             g_engine.frame_count >= (uint32_t)FRAME_AT_60(1800)) &&
            rdt_ok) {
            s_sub00_spawned = 1;
            /* (kein hartkodiertes ROOM1170-BGM mehr — siehe oben; die Auswahl haengt am Raumlader) */
            scd_room_reenter(&rdt,
                             g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                             g_actors[RE15_ACTOR_SLOT_PLAYER].z, 0);  /* helipad scenario */
            fprintf(stderr, "[scd] door-3 self-reentry FALLBACK: re-ran main00+sub00 at frame %d\n",
                    g_engine.frame_count);
        }

        /* (Same-room SCENARIO re-entry consume MOVED into the shared
         * re15_game_step() — it now fires same-frame right after the AOT scan,
         * matching PSX. The old early-consume here ran a frame ahead of the
         * scan, which was the PC/PSX drift.) */

        /* SCD VM ticks at the AH-round 30Hz (pre-BE: tuned timing). At
         * 30fps target SCD ticks every frame. At 60fps target (env override),
         * SCD ticks every 2nd frame so SCD remains 30Hz. */
        if ((target_fps == 30 || (g_engine.frame_count & 1) == 0) && re15_item_modal_active()) {
            /* ITEM-GET MODAL aktiv: byte-true Freeze — der SCD-Tick unterbleibt (das Original haelt
             * mit g_pauseflags|=0xff000000 den SCD-Runner @0x8001cdec an; game_step early-returnt
             * fuer Spieler/Gegner/Anim). Der MODAL-TICK selbst laeuft NICHT mehr hier, sondern NACH
             * re15_game_step (siehe dort) — byte-true zur Frame-Ordnung des Original-Hauptloops
             * FUN_8001c6e8: Spieler-Dispatcher @0x8001ce0c (jal 0x80031c44, einziger Weg zum
             * ACTION-Scan @0x80031fe4) VOR der Modal-FSM @0x8001ce34 (jal 0x8001db28).
             * An der alten Stelle (VOR dem Step) beendete der Bestaetigungs-SQUARE das Modal im
             * selben Host-Frame (state 6->7->0), der Step lief danach weiter, las dieselbe
             * Frame-Flanke (game_step_common.c g_aot_action_pressed) und der AOT-Scan feuerte das
             * zweite, ueberlappende ITEM-AOT ohne neuen Druck (Nutzer-Report 2026-08-29
             * "Item-Stapel"). */
        } else if ((target_fps == 30 || (g_engine.frame_count & 1) == 0)
                   && re15_menu_gameplay_frozen()) {
            /* STATUS SCREEN (wave 2): gameplay is FULLY SUSPENDED while the menu is open or
             * a menu transition runs — the original suspends the whole gameplay TASK
             * (status|=0x40, FUN_80029bf8(0) @0x800460bc), whose tail contains the SCD
             * dispatch, the walkers and the effect driver (@0x8001cdec-ce58) — so the SCD
             * VM / walker steps / esp fx below must NOT tick (the port's old "SCD keeps
             * running under the menu" model was the spec-flagged divergence). The menu FSM
             * itself ticks inside re15_game_step (re15_menu_fsm_tick). */
        } else if (target_fps == 30 || (g_engine.frame_count & 1) == 0) {
            scd_vm_tick();
            /* RE15_ITEM_MODAL_TEST: debug — force-start the item-get pickup MODAL once (frame 40) to
             * visually verify the zoom/spin/flip presentation. Overlays don't show in the autoshot BMP
             * (SDL_RenderReadPixels captures only RenderCopy'd textures) — verify via an ffmpeg video. */
            if (getenv("RE15_ITEM_MODAL_TEST") && g_engine.frame_count == 40)
                re15_item_modal_start(0x15, 50, 0, -1, 0xFF);   /* H.GUN BULLETS (ITPS picture id 0x15) */
            /* RE15_ITEM_USE_TEST: debug — seed a Green Medicine as the only item + open the inventory
             * at frame 40 to visually verify the DIRECT heal (wave 3: confirm -> USE -> green ECG wipe
             * -> consume; NO prompt — spec inv_wave3_spec.md heal flow @0x8004adcc). */
            if (getenv("RE15_ITEM_USE_TEST") && g_engine.frame_count == 40) {
                re15_inv_init();
                g_inv.slots[0].id = 0x24; g_inv.slots[0].qty = 1;   /* Green Medicine, only item */
                re15_menu_toggle();                                  /* open (cursor snaps to slot 0) */
            }
            /* RE15_FORCE_SPLAT: debug — throw one byte-true blood splatter burst at the player at
             * F30 to visually verify the physics (gravity + RNG spread + floor bounce). */
            if (getenv("RE15_FORCE_SPLAT") && g_engine.frame_count == 30) {
                extern void re15_esp_fx_splatter(const re15_esp_t *, uint8_t, int,
                                                 int32_t, int32_t, int32_t, int32_t);
                re15_actor_t *pp = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                re15_esp_fx_splatter(re15_esp_global_bank(), 0, 12,
                                     pp->x, pp->y - 1400, pp->z, pp->y);
            }
            /* TEXT-FREEZE (Fix-Runde Cluster 1, Fund 5): BEIDE Treiber liefen bisher
             * ungegatet durch jeden eingefrorenen Text. Die Gates sitzen jetzt IN der Engine,
             * genau wie im Original — dort traegt jeder Leser sein Bit selbst:
             *   re15_esp_fx_tick            -> RE15_PAUSE_ACTION (0x10000000), Selbst-Gate im
             *                                  Prolog von FUN_80019e20 (@0x80019e28 lw /
             *                                  @0x80019e3c and / @0x80019e40 bne -> Epilog
             *                                  0x8001a4a4); Beleg in re15_esp.c.
             *   re15_actor_step_all_walkers -> pro Slot: Spieler RE15_PAUSE_PLAYER
             *                                  (@0x80031c78 -> 0x80073f90[4] -> 0x80073e30[4]
             *                                  = WALK-Handler 0x80030af0), NPC RE15_PAUSE_AI
             *                                  (@0x8011c5c0); Beleg in actor_locomotion.c.
             * So gilt das Gate fuer PC- UND PSX-Loop und ist unit-testbar (test_text_freeze). */
            re15_esp_fx_tick(re15_esp_room_bank());   /* Phase ESP-C: advance effect particles (30Hz) */
            /* Walker steps once per 30 Hz SCD tick. (A 2026-06-01 disasm trace
             * suggested the PSX walker runs at 60 Hz → tried 2× stepping, but the
             * USER confirmed 2× FEELS TOO FAST vs PSX — so the PSX position-advance
             * is effectively 30 Hz-content-paced like the Sleeps. Keep 1× = correct
             * wall-clock walk speed.) */
            re15_actor_step_all_walkers();

            /* BL-round 2026-05-29: canonical player-mode + letterbox FSMs (30Hz).
             * ENTER (scripted): while sub02 scripts Leon (its guard flags 1,27/2,7),
             * the player is SCRIPTED (PAD ignored, SCD owns the actor) and the
             * letterbox is held on. PSX enters via Plc_call→mode2; our SCD drives
             * Leon via Plc_motion/Plc_dest, so we enter on these scripting guards. */
            /* EXIT (2026-06-03 audit fix): the handoff is now FLAG-DRIVEN, matching
             * the PSX build and the reduced op_plc_ret (which now only sets
             * player_mode + idle + pc, NOT the letterbox/NPC). sub02 itself does
             * Set(2,7,0)/Set(1,27,0) right before Plc_ret, so the cinematic ends when
             * those guard flags clear; on that set→clear edge we start the orthogonal
             * letterbox close ramp (FUN_80021a0c, 15 frames). Elliot/heli are hidden
             * by camera framing (cut 0x03), not a despawn — see the audit. */
            static int s_cine_was_active = 0;
            int cine_active = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
            /* Byte-true LETTERBOX counter (FUN_80021a0c @0x80020f34, once per frame): ramps
             * ±0x10 within [0, 0xF0] on the LIVE flag(1,27) bit — the bars' subtractive gray
             * level the renderer draws (bg - level). Replaces the binary 24/0 bar toggle;
             * the countdown below stays as the player_mode HANDOFF timer (its 15 decrements
             * == the 15 level steps). Trace wf_bba41002 (#21). */
            re15_letterbox_tick(re15_game_flag_get(1, 27));
            if (getenv("RE15_FLAG_TRACE") && (g_engine.frame_count % 300) == 0)
                fprintf(stderr, "[cine] F%u z1/27=%d z2/7=%d cine=%d pmode=%d letterbox=%d\n",
                        g_engine.frame_count, re15_game_flag_get(1, 27), re15_game_flag_get(2, 7),
                        cine_active, g_scd.player_mode, g_scd.letterbox_countdown);
            if (cine_active) {
                g_scd.player_mode         = 2;    /* scripted */
                g_scd.letterbox_countdown = -1;   /* bars held while scripted */
            } else if (s_cine_was_active) {
                g_scd.letterbox_countdown = 15;   /* canonical FUN_80021a0c 0xF0→0 */
            }
            s_cine_was_active = cine_active;
            if (g_scd.letterbox_countdown > 0 &&
                --g_scd.letterbox_countdown == 0) {
                g_scd.player_mode            = 0;
                /* Do NOT zero message_display_frames here. The original's
                 * cinematic-end path (Plc_ret 0x80041f88, Cut_chg, Cut_auto)
                 * never touches the dialog FSM — a subtitle is dismissed SOLELY
                 * by its own end-wait countdown (DAT_800b8525 in FUN_80028134).
                 * Zeroing it on letterbox-close prematurely killed the LAST
                 * message of a cinematic (e.g. sub14's main16), which — unlike
                 * the earlier lines — has no successor Message_on to re-arm the
                 * display. The message's own duration now governs its lifetime,
                 * exactly like the PSX. */
                re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                                   g_actors[RE15_ACTOR_SLOT_PLAYER].z);
                fprintf(stderr, "[scd F%u] letterbox closed -> gameplay (player_mode=0)\n",
                        g_engine.frame_count);
            }
        }

        /* Phase 4.6.1: drain SCD audio events into the SDL audio backend. */
        re15_audio_tick();

        /* Phase 4.3 legacy: TIM atlas blit (top-right corner) — used to
         * be the "we loaded a texture" debug indicator. Now that the
         * skeletal Leon renders fully via SDL_RenderGeometry sampling the
         * same TIM, the atlas-preview overlap was confusing — disabled
         * to mirror the PSX side (see psx_dev/re15_reborn/main.c:77). */
        (void) tim_ok;
        /* if (tim_ok) re15_tim_blit_pc(&tim, SCREEN_XRES - 128, 8); */

        /* Phase 4.2 demo: bouncing square */
        if (sx < 0 || sx > (SCREEN_XRES - 24)) sdx = -sdx;
        if (sy < 0 || sy > (SCREEN_YRES - 24)) sdy = -sdy;
        sx += sdx;
        sy += sdy;
        /* I-round disable: bouncing orange square — leftover demo cruft. */
        /* re15_render_tile(sx, sy, 24, 24, 1, 255, 128, 0); */
        (void)sx; (void)sy;

        /* Title silenced too for clean cinematic view (was over UI). */
        /* re15_debug_text(8, 16, 0, "RE1.5 Rebuilt"); */

        /* Subtitle text (the one HUD line we actually want during cinematic).
         * Lifetime + dismiss are driven by the SHARED re15_msg_tick (msg_common.c) so
         * PSX and PC behave identically — a message stays up for its own duration and
         * is dismissed only when that expires (sub14's last line main16 no longer
         * gets cut short). The only per-port code is the glyph draw below. */
        {
            extern int re15_render_pc_msg_text(int, int, const unsigned char *, int);
            const unsigned char *raw = 0;
            int raw_len = 0, msg_id = 0;
            int live = re15_msg_tick(&raw, &raw_len, &msg_id);
            if (live) {
                /* Render the raw .msg body with the real TEX.TIM glyph font in the
                 * per-speaker colour (0x05 code → CLUT row), at the RE'd box origin
                 * (34,180) for the 0x300 dynamic Message_on.
                 * KEIN Ersatz-Font mehr (Nutzer 2026-08-03: "diesen fallback font will ich
                 * gar nicht mehr haben"): die alte 6x8-Notschrift zeichnete denselben Text ein
                 * ZWEITES Mal in einer Fremdschrift. Das Original hat nur diese eine Schrift —
                 * fehlt sie, wird eben nichts gezeichnet. Der Item-Prompt schreibt in DIESELBE
                 * Box (34,180), deshalb weicht die Message, solange er offen ist (sonst
                 * ueberlagern sich zwei Texte an derselben Stelle). */
                if (raw && raw_len > 0 && !re15_item_modal_prompt(NULL, NULL))
                    re15_render_pc_msg_text(34, 180, raw, raw_len);
                /* PAGE BREAK (FSM state 1): blinking down-arrow = press action for next page. */
                if (g_scd.message_fsm == 1 && (g_scd.message_blink & 0x18)) {
                    extern void re15_render_pc_down_arrow(int x, int y);
                    re15_render_pc_down_arrow(160, 222);
                }
            }
            /* YES/NO selection prompt + blinking '>' cursor (mirrors the PSX path;
             * byte-true layout from FUN_80028134 state 4: Yes col 160, No col 230, row
             * 196). Option glyph codes are the .msg charset: "Yes"={0x35,0x41,0x4F},
             * "No"={0x2A,0x4B}. The cursor is a '>' triangle (the original's menu-font
             * glyph 0x26 = an 8×8 TEX.TIM sub-region we don't load) and blinks per the
             * state-4 timer (visible when message_blink & 0x18). */
            if (g_scd.message_select) {
                /* Byte-true positions (FUN_80028134 state 4): cursor cell X =
                 * choice*0x46 + 0xa0 = 160 (Yes) / 230 (No); options at 0xae=174 / 244;
                 * row 196. Each option sits 14px right of its cursor. */
                extern void re15_render_pc_cursor(int x, int y);
                static const unsigned char yes_g[3] = { 0x35, 0x41, 0x4F };
                static const unsigned char no_g[2]  = { 0x2A, 0x4B };
                re15_render_pc_msg_text(174, 196, yes_g, 3);
                re15_render_pc_msg_text(244, 196, no_g,  2);
                if (g_scd.message_blink & 0x18)
                    re15_render_pc_cursor((g_scd.message_choice ? 230 : 160), 196);
            }
        }

        /* INVENTORY / STATUS SCREEN — wave 1 display list + wave 2 LIVE FSM (specs
         * shots/inv_plan.md + shots/inv_wave2_spec.md; RE1.5-EXE + DEBUG.BIN). The FSM
         * (menu_common.c, ticked inside re15_game_step) owns ALL the original's screen
         * registers in g_inv_screen (tab 25bc / cursors 25bd-be / dim 25ca / slides
         * 25ea 25ee / item state 25c2 / name print); this block only refreshes the
         * per-frame DERIVED values (ECG advance, condition classifier) and rasterizes.
         * The open/close fades render via the shared fade channel 0 in end_frame
         * (FUN_800217b0(0x200,±0x1800,7,0) — armed by the FSM). RE15_INV_SHOT forces
         * the mzd_inv_open.sav acceptance state (tab-select, cursor on FILE). */
        if (re15_menu_is_open()) {
            re15_actor_t *plr = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            re15_inv_screen_ecg_tick();                    /* FUN_80048a44 sweep/glow/wipe advance */
            /* condition = STATELESS per-frame recompute (FUN_8004ed6c, called from the
             * draw path @0x80049a7c/@0x800479f0/@0x80047a08 — never cached): thresholds
             * 80/20 @0x800112b4/b5; poison = DAT_800acaec bit 2 (the same bit the heal
             * cure clears @0x8004af8c) = the port's player status_flags & 2. */
            g_inv_screen.cond = (uint8_t)re15_inv_screen_condition(
                plr->hp, (plr->status_flags & 2) ? 1 : 0);
            if (getenv("RE15_INV_SHOT") && !getenv("RE15_INV_SHOT_LIVE")
                && !getenv("RE15_INV_GRID_SHOT") && !getenv("RE15_INV_CMD_SHOT")
                && !getenv("RE15_INV_CHECK_SHOT") && !getenv("RE15_INV_MIX_SHOT")
                && !getenv("RE15_INV_MAP_SHOT") && !getenv("RE15_INV_FILE_SHOT")
                && !getenv("RE15_INV_FILE_DOC_SHOT")) {
                /* mzd_inv_open.sav DISPLAYED frame: tab-select, tab=FILE(3), highlight 0,
                 * ECG sweep 0x60 / LED glow 0x18 (two ticks behind the stored RAM values
                 * 0x62/0x20 — double-buffer flip lag, solved from both fb halves). */
                g_inv_screen.tab = 3; g_inv_screen.highlight = 0;
                g_inv_screen.tab_base_y = 0xa6;
                g_inv_screen.item_cursor = 0; g_inv_screen.second_cursor = 0;
                g_inv_screen.ecg_sweep = 0x60; g_inv_screen.ecg_glow = 0x18;
            }
            {
                static re15_inv_op_t s_inv_ops[RE15_INV_MAX_OPS];
                extern int re15_inv_render_pc_draw(const re15_inv_op_t *ops, int n);
                int inv_n = re15_inv_screen_build(&g_inv_screen, s_inv_ops, RE15_INV_MAX_OPS);
                re15_inv_render_pc_draw(s_inv_ops, inv_n);

                /* BILD-EBENE DES FILE-SCHIRMS (Port-Erweiterung, Beleg-Block bei
                 * re15_inv_render_pc_file_image): zeichnet die zwei RE2-Sprites ueber den
                 * Leser, sobald ein Dokument gewaehlt ist. Ohne Auswahl passiert nichts und
                 * der Schirm bleibt der byte-true Textleser von RE1.5.
                 * RE15_DOC="<nr>" waehlt zum Ansehen ein Dokument (0..24). */
                {
                    extern void re15_inv_render_pc_file_image(int doc, int page, int ox, int oy);
                    static int s_doc_env = -2;
                    if (s_doc_env == -2) {
                        const char *dv = getenv("RE15_DOC");
                        s_doc_env = (dv && *dv) ? atoi(dv) : -1;
                        if (s_doc_env >= 0) re15_re2doc_select(s_doc_env);
                    }
                    int doc = re15_re2doc_selected();
                    /* Leser offen = FILE-Welle (substate 2) im Zustand 3 bzw. 4..7 (Seitenwechsel) —
                     * dieselbe Bedingung, unter der re15_inv_screen.c emit_file_reader ruft
                     * (@0x800c6f94 / @0x800c6fc8-d0). */
                    if (doc >= 0 && g_inv_screen.substate == 2 &&
                        (g_inv_screen.item_state == 3 ||
                         (g_inv_screen.item_state >= 4 && g_inv_screen.item_state <= 7))) {
                        /* Seite: der Leser zaehlt ab 0; die Titelseite ist Seite -1. */
                        int pg = (int)g_inv_screen.file_reader_page - 1;
                        /* Bildlage: die 256x256-Flaeche mittig im 320x240-Schirm. */
                        re15_inv_render_pc_file_image(doc, pg, 32, -8);
                    }
                }
            }

            /* WAVE 3: the invented "Will you use the X?" Yes/No prompt overlay was REMOVED —
             * the original heal flow (classifier c3=3 -> FUN_8004adcc @0x8004adcc-0x8004b070)
             * has NO prompt and NO message (spec inv_wave3_spec.md "PORT DIVERGENCE 2"); the
             * only feedback is the ECG condition wipe + sweep reset, which render through the
             * display list above. The cant-use message ("You can't use it here.", c3=6
             * @0x8004b250) also renders inside the display list (g_inv_screen.msg_reveal). */
        }

        /* Phase 4.4.3: drain audio queue (consumes events without HUD). */
        scd_audio_event_t evt;
        while (scd_audio_queue_pop(&evt)) (void)evt;

        /* Phase 4.5.10-I: minimal but clear diagnostic — drop the magenta
         * marker (was making bone 0's marker invisible) and trust the
         * per-bone tiles + bbox in the actual mesh-render path. */
        {
            extern int re15_render_pc_dbg_textri_count(void);
            extern int re15_render_pc_dbg_tim_loaded(void);
            extern int re15_render_pc_dbg_min_sx(void);
            extern int re15_render_pc_dbg_max_sx(void);
            extern int re15_render_pc_dbg_min_sy(void);
            extern int re15_render_pc_dbg_max_sy(void);
            (void)re15_render_pc_dbg_textri_count;
        }

        /* Phase 4.5: rotating MD1 wireframe (orthographic projection).
         * Sums all meshes' triangle wires. Phase 4.5.2 will add proper 3D. */
        if (md1_ok) {
            int total_verts = 0, total_tris = 0;
            for (int i = 0; i < md1.mesh_count; i++) {
                total_verts += md1.meshes[i].tri_vertex_count + md1.meshes[i].quad_vertex_count;
                total_tris  += md1.meshes[i].triangle_count;
            }
            (void)total_verts; (void)total_tris;

            /* Phase 4.5.7.3: skeletal multi-mesh render. PC mirror of
             * mesh_psx_render_skeletal — computes per-bone world poses
             * from the EMR keyframe, then renders each MD1 mesh with
             * its bone's matrix applied to vertices before the
             * perspective project step.
             *
             * Wireframe-only initially to match PSX-side checkpoint
             * (textured per-bone CLUT routing comes later). */
            int   cx = SCREEN_XRES / 2;
            int   cy = SCREEN_YRES / 2;

            /* Phase 4.5.9: prefer RDT-parsed cuts; fall back to static. */
            const re15_camera_cut_t *active_cuts = rdt_ok
                ? rdt.cuts : re15_camera_room1100_cuts;
            int active_cut_count = rdt_ok
                ? rdt.cut_count : re15_camera_room1100_cut_count;

            /* RE2-pure cut handling: cuts change ONLY via SCD Cut_chg.
             *
             * AN-round (2026-05-26, post-user-feedback): REVERTED all
             * animator hacks. Per agent-confirmed PSX behavior: cut framing
             * is STATIC from cuts[N] per cut, no per-frame interpolation.
             * The previous snap-to-focus, work_slot tracking, and lerp
             * were symptom-fixing hacks. Box positions appearing to "jump"
             * between cuts was the smoking-gun that proper Member_set /
             * Obj_model_set semantics are wrong somewhere — fixing the
             * camera animator can't compensate for wrong actor positions. */
            static int s_last_cut_idx = -1;
            /* [ENTFERNT 2026-08-08] s_fade_frames/s_intro_faded — die BN-Runden-Heuristik
             * "15-Frame-Fade-in beim Cut-Wechsel auf 0" hatte KEIN Original-Gegenstück und
             * erzeugte das gemeldete Phantom-Fade in ROOM1150 (Kamera Mitte->Tür):
             *   - RVD-Kamerawechsel im Original: FUN_80014230 -> FUN_800142f4 schreibt NUR
             *     `sb a0,DAT_800afbb5` @0x80014300 (Cut-Index) + `sw v0,DAT_800ac794`
             *     @0x80014310 (RVD-Gruppenzeiger); kein Zugriff auf die Fade-Kanaele
             *     (DAT_800b5458 / FUN_800217b0/FUN_800216ec/FUN_80021764).
             *   - Das echte Intro-/Raum-Fade-in ist der Tuer-FSM-Kick (FUN_8001c958 state 3:
             *     `li a1,-0x1800` @0x8001cbbc -> FUN_800217b0 @0x8001cc00) — im Port byte-true
             *     in apply_pending (re15_fade_config/kick unten) fuer JEDEN Raumwechsel inkl.
             *     1240->1170-Intro.
             *   - Spielstart/Load bootet OHNE Fade: Game-Init FUN_800161e0 killt Kanal 0
             *     (`jal FUN_80021764` @0x80016420) und setzt Schwarz-Clear (FUN_80021634(2,0)
             *     @0x80016424) — der erste Raum erscheint direkt.
             * Gemessen (RE15_FADE_LOG, Run D): CONTINUE-Load auf Cut!=0 liess den One-Shot
             * scharf; der naechste RVD-Wechsel 1->0 in 1150 feuerte das 15-Frame-set_fade. */
            /* BO-round (Tier-3): active cut's region quad (anchor zone), for the
             * per-object region cull (PSX FUN_8002c18c → FUN_80014368). Refreshed
             * on cut change; props/NPCs outside it are not drawn. */
            static int16_t cam_region_xs[4] = {0}, cam_region_zs[4] = {0};
            static int     cam_has_region   = 0;
            int active_cut_idx = (s_last_cut_idx < 0) ? 0 : s_last_cut_idx;
            { const char *fc = getenv("RE15_FORCE_CUT");   /* TEMP: pin the camera to a cut to see the kneel on-camera */
              if (fc && *fc) { g_scd.cam_id = (uint8_t)atoi(fc); g_scd.cam_change_pending = 1; } }
            /* MESS-SONDE (env-gegatet, im Normalpfad stumm): pro Bild den Kamera-Zustand.
             * Zeigt, ob der ANGEZEIGTE Cut (s_last_cut_idx) dem ANGEFORDERTEN (g_scd.cam_id,
             * == DAT_800afbb5) folgt. Bleiben die beiden auseinander, ist der Apply-Pfad
             * defekt — genau der Nutzer-Befund "das Bild wechselt nicht". */
            static int s_cam_trace = -1;
            if (s_cam_trace < 0) s_cam_trace = getenv("RE15_CAM_TRACE") ? 1 : 0;
            if (s_cam_trace) {
                static int s_ct_last = -12345;
                int ct_now = ((int)g_scd.cam_id << 20) | ((s_last_cut_idx & 0xFF) << 12)
                           | ((g_scd.work_vars[0x0A] & 0xFF) << 4)
                           | (g_scd.cam_change_pending ? 2 : 0) | (g_scd.cut_auto_enabled ? 1 : 0);
                if (ct_now != s_ct_last || (g_engine.frame_count % 60) == 0) {
                    s_ct_last = ct_now;
                    fprintf(stderr, "[cam-trace] F%u room=%04x req(cam_id)=%u shown(s_last)=%d "
                            "wv0A=%d pending=%u auto=%u pl=(%ld,%ld) rot=%d\n",
                            g_engine.frame_count, g_current_room_id, (unsigned)g_scd.cam_id,
                            s_last_cut_idx, (int)g_scd.work_vars[0x0A],
                            (unsigned)g_scd.cam_change_pending,
                            (unsigned)g_scd.cut_auto_enabled,
                            (long)g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                            (long)g_actors[RE15_ACTOR_SLOT_PLAYER].z,
                            (int)g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y);
                }
            }
            /* SELBSTHEILENDER KAMERA-APPLY (byte-true FUN_8002137c @0x800214f4-0x80021514 +
             * FUN_80021bbc @0x80021bf4/fc) — die gemeinsame Engine-Fassung, damit PC, PSX und
             * die Mess-Sonden EINE Regel teilen. Sie bewertet in JEDEM Bild neu, ob der
             * angezeigte Cut (work_vars[0x0A]) noch dem angeforderten (g_scd.cam_id ==
             * DAT_800afbb5) entspricht, und bewaffnet das Dirty-Flag sonst neu. Hier stand
             * bis 2026-08-21 nur `if (g_scd.cam_change_pending)`: ein EINMAL-Flag. Ging es
             * verloren, blieb das Bild DAUERHAFT stehen (Nutzer-Befund "nach dem
             * Generator-Raetsel wechselt die Kamera nicht mehr").
             *
             * Apply INSTANTLY: die PSX haelt den alten Cut ~6 Bilder, waehrend sie den BG der
             * neuen Kamera von CD liest (FUN_80013c50) — reine HARDWARE-LADEZEIT, keine
             * Spiellogik, deshalb darf der PC sofort schalten. */
            if (re15_cam_present_tick()) {
                active_cut_idx = (int)g_scd.cam_id;
                if (active_cut_idx >= active_cut_count)
                    active_cut_idx = active_cut_count - 1;
            }
            if (active_cut_idx != s_last_cut_idx) {
                { extern int re15_fade_log_on(void);
                  if (re15_fade_log_on())
                      fprintf(stderr, "[fade-log] F%u room=%04x CUT %d -> %d\n",
                              g_engine.frame_count, g_current_room_id,
                              s_last_cut_idx, active_cut_idx); }
                /* KEIN Fade beim Kamerawechsel — byte-true: der RVD-Cut-Apply
                 * FUN_800142f4 schreibt nur den Cut-Index (`sb` @0x80014300) und den
                 * RVD-Gruppenzeiger (`sw` @0x80014310); die Fade-Engine wird auf diesem
                 * Pfad nie beruehrt (siehe Kommentar an s_last_cut_idx oben). */
                s_last_cut_idx = active_cut_idx;
                /* BO-round (Tier-3): refresh the active cut's region quad. */
                cam_has_region = rdt_ok
                    ? re15_rdt_get_region_quad(&rdt, active_cut_idx,
                                               cam_region_xs, cam_region_zs)
                    : 0;
                /* MONTAGE: vor dem Laden das noch stehende Bild als AUSBLENDENDE Ebene
                 * sichern (RE2 haelt beide Elemente gleichzeitig aktiv, hide_others=0 —
                 * die Ueberlappung der 48-Frame-Rampen IST die Kreuzblende). */
                if (g_current_room_id == 0x1240 && !re15_montage_fx_stock()) {
                    re15_bg_snapshot_prev();
                    re15_montage_fx_on_cut(active_cut_idx, re15_bg_prev_ready());
                }
                re15_bg_load_cut(active_cut_idx);

                /* BE-round: switch lighting tint to the new cut. */
                if (g_re15_room_lights_ok) {
                    re15_light_apply_cut(&g_re15_room_lights, active_cut_idx);
                    fprintf(stderr,
                            "[light] cut %d tint=(%u,%u,%u)\n",
                            active_cut_idx,
                            g_re15_light_tint[0], g_re15_light_tint[1],
                            g_re15_light_tint[2]);
                }

                /* AZ-round 2026-05-28: parse sprite.pri for this cut and
                 * push the mask list to the renderer's BG-overdraw layer.
                 * NULL section (pri_offset bytes 0xFFFFFFFF) → no masks,
                 * overdraw disabled for this cut. */
                re15_pri_cut_t pri = {0};
                int pri_n = 0;
                if (rdt_ok && rdt_buf && active_cut_idx >= 0
                    && active_cut_idx < active_cut_count) {
                    pri_n = re15_pri_parse_section(rdt_buf, (size_t)rdt_size,
                                                   active_cuts[active_cut_idx].pri_offset,
                                                   &pri);
                }
                /* sprite.pri FOREGROUND OCCLUSION (2026-06-09): the AZ-round bug
                 * (sampling the main BG cache → ghost sky patches) is fixed — we
                 * now load the cut's dedicated foreground ATLAS (decoded byte-true
                 * from the room's BSS SLD block, VRAM-verified) and feed the real
                 * masks. The overdraw re-blits atlas[srcX,srcY,w,h]→screen[dstX,dstY]
                 * after the actors so railings/boxes occlude them (byte-true to PSX
                 * FUN_800392d4/FUN_80039590). Cuts with no foreground → atlas absent
                 * → overdraw off. */
                extern int re15_pri_load_cut_atlas(int cut_idx);
                int has_fg = re15_pri_load_cut_atlas(active_cut_idx);
                /* RE15_NO_PRI=1: measurement A/B — suppress the foreground overdraw
                 * so a diff against the normal frame shows exactly which pixels the
                 * sprite.pri masks paint, and onto what. */
                if (has_fg && pri_n > 0 && !getenv("RE15_NO_PRI")) {
                    int sx[RE15_PRI_MAX_MASKS_PER_CUT], sy[RE15_PRI_MAX_MASKS_PER_CUT];
                    int dx[RE15_PRI_MAX_MASKS_PER_CUT], dy[RE15_PRI_MAX_MASKS_PER_CUT];
                    int pw[RE15_PRI_MAX_MASKS_PER_CUT], ph[RE15_PRI_MAX_MASKS_PER_CUT];
                    int pz[RE15_PRI_MAX_MASKS_PER_CUT];
                    /* DRAWN count, not BUILT count (@0x80039358) — see re15_pri.h. */
                    int n = pri.draw_count;
                    if (n > RE15_PRI_MAX_MASKS_PER_CUT) n = RE15_PRI_MAX_MASKS_PER_CUT;
                    for (int m = 0; m < n; m++) {
                        sx[m] = pri.masks[m].srcX;          sy[m] = pri.masks[m].srcY;
                        dx[m] = (int16_t)pri.masks[m].dstX; dy[m] = (int16_t)pri.masks[m].dstY;
                        pw[m] = pri.masks[m].width;         ph[m] = pri.masks[m].height;
                        pz[m] = pri.masks[m].depth;
                    }
                    re15_render_pc_set_pri_rects(sx, sy, dx, dy, pw, ph, pz, n);
                } else {
                    re15_render_pc_set_pri_rects(NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0);
                }
                fprintf(stderr, "[pri] cut=%d pri_offset=0x%X masks=%d fg_atlas=%d\n",
                        active_cut_idx,
                        (unsigned)active_cuts[active_cut_idx].pri_offset, pri_n, has_fg);
            }
            re15_camera_view_t cam_view;
            /* BYTE-TRUE DEATH-CAMERA ORBIT (per-frame FUN_80015850 + init FUN_80015284, RE'd
             * wf_32d97217). On death the camera CUTS to a corpse-orbiting pose and slowly circles it:
             *   cam.x = (rcos(yaw)*radius)>>12 + corpse.x ;  cam.z = (rsin(yaw)*radius)>>12 + corpse.z
             *   yaw = (yaw - yaw_step) & 0xfff ;  radius -= dist_step ;  cam.y -= y_step   (PSX-down: cranes UP)
             * with the look-at easing toward the (live) corpse: XZ /60, Y toward corpse.y-400 /20.
             * radius seeds to the room-cam→corpse distance and dollies to 3300 over 3 frames
             * (dist_step=(r-3300)/3, then frozen). Uses the BIOS rcos/rsin/SquareRoot0 the ROM uses
             * (0x80068348/0x800683e8/0x80065f60). The STEADY orbit (yaw=0x80, yaw_step=0xc=12,
             * radius=3300, dist_step=0, y_step=100) is byte-true VERIFIED against the death savestates:
             * yaw=(128-12*frame)&0xfff → frame 108 = 2928, matching RAM (0x800b525c) at counter 109.
             * FLY-IN (FLAG B RESOLVED, savestate-derived — implemented in the INIT block below, s_dc_yaw /
             * s_dc_yaw_step): the ~3-frame fly-in SPIN (yaw_step=0x384=900 + a yaw seeded from the corpse
             * model's facing point P+0x5b4/0x5bc via ratan2-0xd2c) IS reproduced — the seed reduces to
             * (-rot_y - 0xd2c) & 0xfff (=710·forward(rot_y), verified 3/3 death saves) and swings at
             * 900/frame for 3 frames before settling, so the cut + fly-in spin + orbit are all byte-true. Replaces the old fabricated {0x1f4,0xbb8,0x1f4}+drift glide. */
            re15_camera_cut_t death_cut;
            const re15_camera_cut_t *view_cut = &active_cuts[active_cut_idx];
            {
                static int s_dc_on = 0, s_dc_updates = 0;
                static re15_camera_cut_t s_dc;
                static int32_t s_dc_radius, s_dc_yaw, s_dc_yaw_step, s_dc_dist_step, s_dc_y_step;
                if (g_death_cam) {
                    re15_actor_t *dcp = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                    if (!s_dc_on) {                                    /* INIT (path A), once */
                        s_dc = *view_cut;                             /* seed cam.xyz + fov from the room cut */
                        int32_t rdx = s_dc.pos_x - dcp->x, rdz = s_dc.pos_z - dcp->z;
                        s_dc_radius    = (int32_t)re15_squareroot0((uint32_t)(rdx*rdx + rdz*rdz));
                        s_dc_dist_step = (s_dc_radius - 3300) / 3;    /* PHASE A: dolly radius to 3300 in 3 frames */
                        s_dc_y_step    = (s_dc.pos_y - (dcp->y - 400)) / 3;  /* PHASE A: dolly cam.y to corpse.y-400 */
                        /* FLY-IN yaw SEED (FLAG B RESOLVED, savestate-derived): RE1.5 seeds
                         * yaw = ratan2(dz,dx) - 0xd2c where (dx,dz) = the corpse model's facing
                         * endpoint (P+0x5b4/0x5bc) minus the corpse = 710·(cos(rot_y),-sin(rot_y)),
                         * i.e. the player's own rot_y at death (confirmed: 3/3 death saves match
                         * 710·forward(rot_y) to ±1u). ratan2 of that forward = -rot_y, so the seed
                         * is (-rot_y - 0xd2c) & 0xfff. Verified: combat_death rot_y=1199 → seed 3621,
                         * minus 3×900 fly-in = 921 = the RAM yaw (0x800b525c) at phase 5. */
                        s_dc_yaw       = (-(int32_t)dcp->rot_y - 0xd2c) & 0xfff;
                        s_dc_yaw_step  = 0x384;                       /* 900/frame fast fly-in swing (phase A) */
                        s_dc.target_x  = dcp->x;                      /* look-at seed = corpse (no offset) */
                        s_dc.target_z  = dcp->z;
                        s_dc.target_y  = dcp->y - 400;
                        s_dc_updates   = 0;
                        s_dc_on = 1;
                    }
                    /* PER-FRAME UPDATE (FUN_80015850) */
                    s_dc_radius -= s_dc_dist_step;
                    s_dc.pos_y  -= s_dc_y_step;
                    s_dc.pos_x   = ((re15_rcos(s_dc_yaw) * s_dc_radius) >> 12) + dcp->x;
                    s_dc.pos_z   = ((re15_rsin(s_dc_yaw) * s_dc_radius) >> 12) + dcp->z;
                    s_dc_yaw     = (s_dc_yaw - s_dc_yaw_step) & 0xfff;
                    s_dc.target_x += (dcp->x - s_dc.target_x) / 60;   /* look-at ease /60 XZ */
                    s_dc.target_z += (dcp->z - s_dc.target_z) / 60;
                    s_dc.target_y += (dcp->y - (s_dc.target_y + 400)) / 20;  /* /20 Y, +400 head */
                    s_dc_updates++;
                    if (s_dc_updates == 3) {                          /* PHASE A -> B (settle), ONCE after the
                                                                       * 3-frame dolly + fly-in swing (block @0x8001572c) */
                        s_dc_dist_step = 0;                           /* freeze radius at ~3300 */
                        s_dc_y_step    = 0x64;                        /* 100/frame steady crane-up */
                        s_dc_yaw       = 0x80;                        /* 128 constant reset (ends the fly-in) */
                        s_dc_yaw_step  = 0xc;                         /* 12/frame steady orbit */
                    }
                    death_cut = s_dc;
                    view_cut  = &death_cut;
                } else if (s_dc_on) {
                    s_dc_on = 0; s_dc_updates = 0;                    /* revive/reload -> normal cam */
                }
            }
            re15_camera_build_view(view_cut, &cam_view);

            /* sprite.pri depth gate: project the player into camera + screen space
             * (same scale as the per-tri avg_z / mask depth<<3) so a mask BEHIND him
             * that he OVERLAPS on screen is skipped — he stops being occluded by
             * foreground he stands in front of, without dropping foreground elsewhere. */
            {
                re15_actor_t *plz = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                /* MEASUREMENT: RE15_POCC_TP="frame,x,z,rot" — pin the player at a scanned
                 * "behind the mask" world spot from a given frame on (post-intro), so the
                 * port and the PSX original can be photographed at the SAME pose. */
                {
                    const char *tp = getenv("RE15_POCC_TP");
                    if (tp && *tp) {
                        /* "frame,x,z,rot[;x,z,rot]..." — glide through WAYPOINTS from a
                         * latched start at ~250 u/frame, holding the last. Waypoints matter:
                         * the camera cut comes from the path-dependent RVD auto-scan (in the
                         * original too), so reproducing a PSX route needs the same route. */
                        int wx[8], wz[8], wr[8], nw = 0, tf = 0;
                        tf = atoi(tp);
                        /* c always points at the separator BEFORE a triple (',' for the
                         * first, ';' for each following), so one sscanf form serves both. */
                        const char *c = strchr(tp, ',');
                        while (c && nw < 8) {
                            int a = 0, b = 0, r = 0;
                            if (sscanf(c + 1, "%d,%d,%d", &a, &b, &r) != 3) break;
                            wx[nw] = a; wz[nw] = b; wr[nw] = r; nw++;
                            c = strchr(c + 1, ';');
                        }
                        if (nw > 0 && (int)g_engine.frame_count >= tf) {
                            static int have_start = 0, sx0 = 0, sz0 = 0, f0 = 0;
                            if (!have_start) {
                                have_start = 1; sx0 = plz->x; sz0 = plz->z;
                                f0 = (int)g_engine.frame_count;
                            }
                            int k = (int)g_engine.frame_count - f0;
                            int px = sx0, pz2 = sz0;
                            for (int w = 0; w < nw; w++) {
                                int dx = wx[w] - px, dz = wz[w] - pz2;
                                int ad = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
                                int steps = ad / 250 + 1;
                                if (k <= steps) {
                                    plz->x = px + (int)((long)dx * k / steps);
                                    plz->z = pz2 + (int)((long)dz * k / steps);
                                    break;
                                }
                                k -= steps; px = wx[w]; pz2 = wz[w];
                                plz->x = px; plz->z = pz2; plz->rot_y = (int16_t)wr[w];
                            }
                        }
                    }
                }
                /* MEASUREMENT: RE15_POCC_SWEEP="startframe" — step the player through a
                 * floor grid (the GAME picks the cut via its own RVD logic), and report
                 * every spot where he lands INSIDE an active mask rect. This is the honest
                 * "can Leon stand behind a foreground element at all" census. */
                {
                    const char *sw = getenv("RE15_POCC_SWEEP");
                    int sf = 0;
                    if (sw && sscanf(sw, "%d", &sf) == 1 && (int)g_engine.frame_count >= sf) {
                        const int X0 = -12000, X1 = 15000, Z0 = -10000, Z1 = 17000, STEP = 500;
                        const int NX = (X1 - X0) / STEP + 1;
                        int idx = ((int)g_engine.frame_count - sf) / 3;
                        int gx = X0 + (idx % NX) * STEP;
                        int gz = Z0 + (idx / NX) * STEP;
                        if (gz <= Z1) { plz->x = gx; plz->z = gz; }
                    }
                }
                long pvx = ((long)plz->x * cam_view.rot[0] + (long)plz->y * cam_view.rot[1]
                          + (long)plz->z * cam_view.rot[2]) / 4096 + cam_view.trans[0];
                long pvy = ((long)plz->x * cam_view.rot[3] + (long)plz->y * cam_view.rot[4]
                          + (long)plz->z * cam_view.rot[5]) / 4096 + cam_view.trans[1];
                long pvz = ((long)plz->x * cam_view.rot[6] + (long)plz->y * cam_view.rot[7]
                          + (long)plz->z * cam_view.rot[8]) / 4096 + cam_view.trans[2];
                int psx = 160, psy = 120;
                if (pvz > 64) {
                    psx = 160 + (int)(pvx * cam_view.fov_screen_dist / pvz);
                    psy = 120 + (int)(pvy * cam_view.fov_screen_dist / pvz);
                }
                re15_render_pc_set_pri_player(psx, psy, (int)pvz);
                /* MEASUREMENT PROBE (RE15_POCC=1): per-frame player camera-Z vs each
                 * active pri mask's threshold — the ground-truth instrument for the
                 * sprite.pri occlusion crossover. Pure logging, no behavior. */
                /* RE15_POCC_SCAN: once per cut, back-project a floor grid to find the
                 * world positions that land INSIDE a mask rect (= "player stands behind
                 * the foreground element"), and report the camera-Z there. This is the
                 * instrument that decides the mask-vs-character crossover empirically. */
                if (getenv("RE15_POCC_SCAN")) {
                    static int scanned_cut = -999;
                    if (scanned_cut != active_cut_idx) {
                        scanned_cut = active_cut_idx;
                        extern int re15_render_pc_debug_pri_rects(int *dx, int *dy, int *w,
                                                                  int *h, int *dep, int max);
                        int rx[64], ry[64], rw[64], rh[64], rd[64];
                        int rn = re15_render_pc_debug_pri_rects(rx, ry, rw, rh, rd, 64);
                        for (int wx = -13000; wx <= 15000; wx += 200) {
                            for (int wz = -11000; wz <= 17000; wz += 200) {
                                long vx = ((long)wx * cam_view.rot[0] + (long)plz->y * cam_view.rot[1]
                                         + (long)wz * cam_view.rot[2]) / 4096 + cam_view.trans[0];
                                long vy = ((long)wx * cam_view.rot[3] + (long)plz->y * cam_view.rot[4]
                                         + (long)wz * cam_view.rot[5]) / 4096 + cam_view.trans[1];
                                long vz = ((long)wx * cam_view.rot[6] + (long)plz->y * cam_view.rot[7]
                                         + (long)wz * cam_view.rot[8]) / 4096 + cam_view.trans[2];
                                if (vz <= 64) continue;
                                int ssx = 160 + (int)(vx * cam_view.fov_screen_dist / vz);
                                int ssy = 120 + (int)(vy * cam_view.fov_screen_dist / vz);
                                for (int r = 0; r < rn; r++) {
                                    if (ssx >= rx[r] && ssx < rx[r] + rw[r] &&
                                        ssy >= ry[r] && ssy < ry[r] + rh[r]) {
                                        fprintf(stderr, "[poccscan] cut=%d world=(%d,%d) scr=(%d,%d) "
                                                "vz=%ld maskdepth=%d k32=%d k64=%d occ32=%d occ64=%d\n",
                                                active_cut_idx, wx, wz, ssx, ssy, vz, rd[r],
                                                rd[r] * 32, rd[r] * 64,
                                                (int)(vz > rd[r] * 32), (int)(vz > rd[r] * 64));
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                /* Sweep report: is the player's projected point inside an ACTIVE mask? */
                if (getenv("RE15_POCC_SWEEP")) {
                    extern int re15_render_pc_debug_pri_rects(int *dx, int *dy, int *w,
                                                              int *h, int *dep, int max);
                    int rx[64], ry[64], rw[64], rh[64], rd[64];
                    int rn = re15_render_pc_debug_pri_rects(rx, ry, rw, rh, rd, 64);
                    for (int r = 0; r < rn; r++) {
                        if (pvz > 64 && psx >= rx[r] && psx < rx[r] + rw[r] &&
                            psy >= ry[r] && psy < ry[r] + rh[r]) {
                            fprintf(stderr, "[poccsweep] cut=%d world=(%d,%d) scr=(%d,%d) vz=%ld "
                                    "maskdepth=%d k32=%d k64=%d occ32=%d occ64=%d floor=%d\n",
                                    active_cut_idx, plz->x, plz->z, psx, psy, pvz, rd[r],
                                    rd[r] * 32, rd[r] * 64,
                                    (int)(pvz > rd[r] * 32), (int)(pvz > rd[r] * 64),
                                    re15_collision_on_floor(&g_room_rdt, plz->x, plz->z));
                            /* RE15_POCC_SHOT: photograph the qualifying spots so the
                             * occlusion can be judged on PIXELS, not on the model. */
                            const char *shot_cut = getenv("RE15_POCC_SHOT");
                            if (shot_cut
                                && (atoi(shot_cut) == 0 || atoi(shot_cut) == active_cut_idx)
                                && re15_collision_on_floor(&g_room_rdt, plz->x, plz->z)) {
                                extern void re15_render_pc_screenshot(const char *path);
                                static int shot_n = 0;
                                if (shot_n < 40 && ((int)g_engine.frame_count % 3) == 2) {
                                    char pth[256];
                                    snprintf(pth, sizeof pth,
                                             "shots/occ_c%d_%d_%d.bmp",
                                             active_cut_idx, plz->x, plz->z);
                                    re15_render_pc_screenshot(pth);
                                    shot_n++;
                                }
                            }
                            break;
                        }
                    }
                }
                if (getenv("RE15_POCC")) {
                    extern int re15_render_pc_debug_pri(int *depths, int max);
                    int md[64];
                    int mn = re15_render_pc_debug_pri(md, 64);
                    fprintf(stderr, "[pocc] F%u cut=%d pl=(%d,%d,%d) vz=%ld scr=(%d,%d) masks=%d",
                            (unsigned)g_engine.frame_count, active_cut_idx, plz->x, plz->y, plz->z,
                            pvz, psx, psy, mn);
                    for (int mi2 = 0; mi2 < mn; mi2++)
                        fprintf(stderr, " | d=%d k32=%d k64=%d occ32=%d occ64=%d",
                                md[mi2], md[mi2] * 32, md[mi2] * 64,
                                (int)(pvz > md[mi2] * 32), (int)(pvz > md[mi2] * 64));
                    fprintf(stderr, "\n");
                }
            }

            /* Helicopter rotor (BGM SUB layer = SsSeq slots 1/2): on/off is driven ONLY by the
             * SCD's Sce_bgm_control (0x54) opcode — op1 = SsSeqSetVol(slot vol) + SsSeqPlay
             * (@0x80044e00-44), op2 = SsSeqStop (@0x80044e50-84) — plus the room-entry auto-play
             * gate FUN_800444b0 for SEQ#2. The former per-frame distance+azimuth ATTENUATION of the
             * playing layer was a PORT INVENTION and is gone (re15_audio_rotor_update is now a
             * no-op; FUN_80045a64 has exactly 4 callers @0x800451cc/@0x8004527c/@0x800454e8/
             * @0x80045830, ALL in the SE path, none on a SsSeq volume) — likewise the
             * `player_mode != 2` hard-mute, which had no original counterpart. re15_rotor_drive
             * stays wired (shared with the PSX main loop) but no longer touches the BGM master. */
            re15_rotor_drive(&active_cuts[active_cut_idx]);

            /* Shadow orientation is built PER-ACTOR at each shadow draw below via
             * re15_camera_yaw_matrix_angle(actor.rot_y) — byte-true RotMatrixY(actor rot_y), the
             * mechanism RE1.5 FUN_8001b064 actually uses (RE'd wf_13911cba). The former camera-yaw
             * form (built here from the cut forward) was a misread of DAT_800ac784=actor, now gone. */

            /* BJ-round 2026-05-29: cinematic letterbox. PSX cutscenes (the intro)
             * draw ~17px black bars top+bottom (measured from ablauf 217030:
             * content rows 77..1003 of 1080 → ~17px in 240-space), hiding the
             * topmost building/window row that's visible during gameplay
             * (non-cutscene.png). Gate on the cinematic flags sub02 sets. */
            {
                /* Letterbox: now the byte-true FUN_80021a0c counter model — the renderer
                 * reads g_letterbox_level directly (ticked above on the live flag(1,27));
                 * the old binary set_letterbox(24/0) toggle is superseded (#21). */

                /* Frontend-Overlay-Release im Gameplay (Ersatz des entfernten 15-Frame-
                 * Heuristik-Ramps): das s_fade_alpha-Overlay gehoert den Frontend-Screens
                 * (Card-Save/-Load haelt beim Exit 255 = @0x800265ec static 0x7fff) und dem
                 * Death-FSM (setzt es weiter unten JEDEN Frame neu). Im Original loest der
                 * Game-Init FUN_800161e0 den Front-End-Hold durch den Kanal-0-KILL
                 * (`jal FUN_80021764` @0x80016420) + Schwarz-Clear (FUN_80021634(2,0)
                 * @0x80016424); das per-Frame-Nullen hier ist dessen Port-Gegenstueck.
                 * Raum-Fades laufen NICHT hierueber, sondern byte-true ueber die
                 * Fade-Kanaele (g_fade_ch, re15_fade_tick im Renderer). */
                {
                    extern void re15_render_pc_set_fade(int a);
                    re15_render_pc_set_fade(0);
                }

                /* PRE-INTRO NARRATOR BLACK — the byte-true mechanism is a VOID CAMERA, not a fill.
                 * RE'd 2026-07-22 (workflow wf_72a0fb3a + self-verified from the RDT):
                 *   ROOM1170 main00 (RDT @file 0x12c8): Ck(3,125,==0) [first visit] -> Door_aot_set(3,
                 *   dest 0x1170) + Evt_exec(0x180B) = sub11 (the narrator). sub11 does Cut_chg(7) +
                 *   Message_on(8..11) + Set(3,125,1) (so it is once-only) + Aot_on(3) (self-reenter ->
                 *   sub00 -> sub15 = Elliot 0x47 + the crate Obj_model_set(0x2D) props + sub02 helipad).
                 *   ROOM1170 CUT 7's camera (RDT @file 0x160) is a VOID camera pos(-12834,-3114,-9774)
                 *   tgt(-7794,-2196,-22446) — BYTE-IDENTICAL to the dummy camera all 9 ROOM1240 montage
                 *   cuts share. It looks away from the helipad geometry (crates + player sit near
                 *   origin/+Z = BEHIND this camera), so the per-vertex near-clip (wz<0 -> cull, ~L4327)
                 *   drops the whole 3D scene; combined with the real black MDEC still BG07 (decodes to
                 *   pure 0,0,0), cut 7 renders BLACK with NO fill needed. The crates+Elliot are not even
                 *   spawned yet (0x2D + type 0x47 are only in sub15, after the narrator's self-reenter).
                 * So the port ALREADY blacks the narrator via the correct per-cut camera + near-clip.
                 * s_scene_black below is now a PRECISE, redundant safety net GATED EXACTLY ON the
                 * RDT-verified narrator cut (room 0x1170 + cut 7): it can never bleed onto the ROOM1240
                 * montage stills (room!=0x1170) nor the ROOM1170 helipad cinematic (sub02 = cut 0/2), and
                 * it no longer depends on the fragile s_preintro latch (which report 1 suspected of being
                 * 0 at runtime in the real new-game flow -> the old gate's failure mode). It is byte-true-
                 * ALIGNED (its output == the void-camera black at exactly this cut). Drop it entirely once
                 * a windowed/real-flow capture confirms the void camera near-clips the helipad on screen.
                 * NB: gating on cut 7 only works because the REAL fix is in re15_room_apply_pending
                 * (room_common.c): it used to clobber sub11's Cut_chg(7) on the 1240->1170 door
                 * (cam_change_pending=0 + cut_auto_enabled=1), so cut 7 NEVER held in the real flow and the
                 * CAM_SWITCH auto-scan picked the player's helipad zone (the narration read over the
                 * helipad). apply_pending now honours a SCD-init Cut_chg, so cut 7 holds through the
                 * narrator (headless-verified: sub11 messages 8-11 run at cut=7 cut_auto=0). */
                {
                    extern void re15_render_pc_set_scene_black(int on);
                    re15_render_pc_set_scene_black(g_current_room_id == 0x1170 && active_cut_idx == 7);
                }
            }
            /* Action-button press edge (Square = Enter on PC). */
            /* --- SHARED interpreter step (commons re15_game_step) ---
             * The canonical PSX-order: action latch -> stair traversal OR
             * (player move + SCA/object collision) -> door/stair AOT scan ->
             * same-room scenario re-entry consume -> fired-event dispatch.
             * IDENTICAL on PSX and PC (game_step_common.c) so the interpreter
             * cannot drift between the ports; only render/input/audio/asset and
             * the SCD-tick cadence stay per-port. */
            {
                re15_game_ctx_t gctx;
                gctx.rdt         = &rdt;
                gctx.rdt_ok      = rdt_ok;
                gctx.pl00_skel   = &pl00_skel;
                gctx.pl00_anim   = &pl00_anim;
                gctx.w01_anim    = &w01_anim;   /* walk-source = footstep flags */
                gctx.cam_view    = &cam_view;
                gctx.active_cut  = active_cut_idx;
                /* apply the OPTIONS controller preset (TYPE A = identity → byte-true default). */
                gctx.pad_current = pc_pad_config((uint16_t)g_engine.pad_current);
                gctx.pad_pressed = pc_pad_config((uint16_t)g_engine.pad_pressed);
                /* (Hier stand kurzzeitig ein RE15_AUTOWALK-Hack, der VORWAERTS in das Pad-Wort
                 * ODERte. Entfernt: RE15_INPUT_SCRIPT in input_pc.c kann das laengst und tut es
                 * am richtigen Ort — im echten Eingabepfad, ueber alle Modi hinweg. "U10" haelt
                 * VORWAERTS zehn Sekunden. Zwei Wege fuer dieselbe Sache waeren genau die Art
                 * Parallelpfad, die den RE15_START_ROOM-Aerger verursacht hat.) */

                /* ==== AUTOPILOT ============================================================
                 * RE15_AUTOPILOT="aot:<n>" | "xz:<x>,<z>"  [+ ",hold" | ",noact"]
                 *
                 * ZWECK: einen bestimmten Punkt im Raum ANSTEUERN und dort ausloesen, ohne
                 * hand-getimte Tastenskripte. Ein zeitbasiertes Skript ("6 s vorwaerts") trifft
                 * eine Tuer nur zufaellig; damit war ich bisher darauf angewiesen, mir Logs
                 * schicken zu lassen. Der Autopilot regelt statt zu timen.
                 *
                 * Er erzeugt NUR Pad-Bits und laeuft damit durch denselben Eingabepfad wie ein
                 * Mensch — kein Parallelweg an der Spiellogik vorbei.
                 *
                 * SELBSTKORRIGIEREND statt winkel-geraten: die Blickrichtung ist
                 * (cos(rot_y), -sin(rot_y)) (actor_locomotion.c: x += c*v, z -= s*v). Aus dem
                 * 2D-Kreuzprodukt Blick x Ziel folgt die Drehseite. Welche Taste rot_y erhoeht,
                 * MUSS ich nicht wissen: der Regler dreht, misst, ob die Fehlstellung kleiner
                 * wird, und dreht sonst um. Das haelt auch, wenn sich die Konvention aendert. */
                {
                    /* Zielkette: "xz:a,b;xz:c,d;aot:3" — Zwischenziele werden der Reihe nach
                     * abgehakt, nur das LETZTE loest aus. Die Zwischenziele nimmt man am besten
                     * aus den Nav-Zonen des Raums (RDT-Bloecke @0x38: Rechteck + Link-Maske);
                     * Zonen-MITTEN liegen im Begehbaren, die Verbindung dazwischen ist kurz.
                     * Ein einzelnes fernes Ziel reicht nicht: ROOM1130 ist die Kette 3->2->1->0,
                     * und der Regler blieb am Zonenuebergang z=4500 haengen. */
                    #define AP_MAX_GOALS 8
                    static int ap_init = 0, ap_hold = 0, ap_act = 1;
                    static int ap_n = 0, ap_idx = 0;
                    static int ap_gmode[AP_MAX_GOALS];              /* 1 = aot, 2 = xz */
                    static int ap_gslot[AP_MAX_GOALS];
                    static int32_t ap_gx[AP_MAX_GOALS], ap_gz[AP_MAX_GOALS];
                    int ap_mode = 0, ap_slot = -1;
                    static int32_t ap_tx = 0, ap_tz = 0;
                    static int ap_turn = +1, ap_flipwait = 0, ap_msgwait = 0;
                    static long long ap_bestcross = -1;
                    static int ap_stuck = 0, ap_avoid = 0, ap_avoid_dir = +1;
                    static int32_t ap_lastx = 0, ap_lastz = 0;
                    if (!ap_init) {
                        ap_init = 1;
                        const char *s = getenv("RE15_AUTOPILOT");
                        if (s && *s) {
                            if (strstr(s, "noact")) ap_act = 0;
                            const char *p = s;
                            while (*p && ap_n < AP_MAX_GOALS) {
                                while (*p == ';' || *p == ' ') p++;
                                if (!strncmp(p, "aot:", 4)) {
                                    ap_gmode[ap_n] = 1; ap_gslot[ap_n] = atoi(p + 4); ap_n++;
                                } else if (!strncmp(p, "xza:", 4)) {
                                    /* xza = Punkt ansteuern UND dort die Aktionstaste halten.
                                     * Fuer Treppen und Tueren, ohne sich auf AOT-SLOT-NUMMERN zu
                                     * verlassen: die sind ueber Szenarien hinweg NICHT stabil —
                                     * nach dem Tor-Wechsel (Selbst-Tuer, cut 11) waren die Slots
                                     * 8 und 9 neu belegt und lagen ploetzlich beim Spieler. */
                                    int gx = 0, gz = 0;
                                    if (sscanf(p + 4, "%d,%d", &gx, &gz) == 2) {
                                        ap_gmode[ap_n] = 3; ap_gx[ap_n] = gx; ap_gz[ap_n] = gz; ap_n++;
                                    }
                                } else if (!strncmp(p, "xz:", 3)) {
                                    int gx = 0, gz = 0;
                                    if (sscanf(p + 3, "%d,%d", &gx, &gz) == 2) {
                                        ap_gmode[ap_n] = 2; ap_gx[ap_n] = gx; ap_gz[ap_n] = gz; ap_n++;
                                    }
                                }
                                while (*p && *p != ';') p++;
                            }
                            fprintf(stderr, "[auto] Autopilot: %s (%d Ziele)\n", s, ap_n);
                        }
                    }
                    if (ap_idx < ap_n) {
                        ap_mode = ap_gmode[ap_idx]; ap_slot = ap_gslot[ap_idx];
                        ap_tx = ap_gx[ap_idx];      ap_tz = ap_gz[ap_idx];
                    }
                    /* RE15_AOT_DUMP=1: alle aktiven AOTs des aktuellen Raums einmal auflisten.
                     * Ohne das muss man raten, WO ein Schalter/eine Tuer liegt — mit dem Dump
                     * hat man Typ, Mittelpunkt und Ausdehnung und kann den Autopiloten direkt
                     * darauf ansetzen. */
                    {
                        static unsigned aotd_room = 0;
                        if (getenv("RE15_AOT_DUMP") && aotd_room != g_current_room_id &&
                            g_engine.frame_count > 40) {
                            aotd_room = g_current_room_id;
                            static const char *tn[] = { "GENERIC","DOOR","ITEM","CAM_SWITCH",
                                                        "STAIR","MESSAGE","AUTO_EVENT","EXAMINE" };
                            fprintf(stderr, "[aot] --- Raum %04X ---\n", g_current_room_id);
                            for (int ai = 0; ai < RE15_AOT_MAX; ai++) {
                                const re15_aot_t *a = &g_aot.slots[ai];
                                if (!a->active) continue;
                                char dst[40] = "";
                                if (a->type == RE15_AOT_TYPE_DOOR) {
                                    const re15_aot_door_params_t *dp = &g_aot.door_params[ai];
                                    /* Zielraum wie in room_common: (stage+1)<<12 | room<<4 */
                                    snprintf(dst, sizeof dst, " -> ROOM%X%02X0 cut=%u",
                                             dp->dest_stage + 1, dp->dest_room, dp->target_cut);
                                }
                                fprintf(stderr, "[aot] %2d %-10s Mitte(%6d,%6d) halb(%5d,%5d) "
                                                "ev=%u sce=%02X band=%u%s\n",
                                        ai, (a->type < 8) ? tn[a->type] : "?",
                                        a->x, a->z, a->half_w, a->half_h,
                                        a->event_id, a->sce_flags, a->band, dst);
                            }
                        }
                    }
                    /* Offene Nachrichten IMMER wegbestaetigen — auch waehrend der Cutscene.
                     * Das Intro besteht aus Dialogboxen; wird keine bestaetigt, laeuft es nie zu
                     * Ende und das Gameplay beginnt gar nicht (gemessen: bei Frame 12660 stand der
                     * Spieler noch mit mo=211 am Helipad). Dieser Teil gehoert deshalb VOR die
                     * Gameplay-Schranke. */
                    /* CUTSCENE WEITERKLICKEN. Nicht nur bei g_scd.message_active: das Intro
                     * wartet an Stellen auf einen Tastendruck, an denen dieses Flag NICHT steht —
                     * gemessen stand der Spieler sonst noch bei Frame 5900 mit mo=211 am Helipad,
                     * player_mode blieb 2. Das ist KEIN Fehler des Spiels, sondern genau das
                     * Verhalten fuer jemanden, der den Controller nicht anfasst; frueher kam der
                     * Lauf nur durch, weil der noch ungegatete Regler nebenbei Tasten drueckte.
                     * Bestaetigt wird in RE1.5 mit QUADRAT (virtueller Remap @0x80073dbc),
                     * Dialoge nehmen zusaetzlich KREUZ — beide abwechselnd als Flanke. */
                    /* WICHTIG: waehrend einer Cutscene (player_mode == 2) NICHT klicken.
                     * Das Helipad-Intro ist sehr lang und laeuft von selbst zu Ende — es braucht
                     * keinen Tastendruck. Wer dort mitklickt, bringt das Skript aus der Spur:
                     * gemessen wurden die Intro-Ende-Flags flag(1,27)/flag(2,7) dann NIE beide
                     * geloescht, player_mode blieb 2 und der Spieler wurde nie frei. Ohne Klicken
                     * endet die Flag-Spur sauber mit "z2/7 = 0" und "z1/27 = 0".
                     * Im Gameplay (Examine-Dialoge) darf und soll der Autopilot dagegen weiterklicken. */
                    if (ap_mode && g_scd.message_active && g_scd.player_mode != 2) {
                        int ph = ap_msgwait++ % 24;
                        if (ph == 0)  gctx.pad_current |= RE15_PAD_BIT_SQUARE;
                        if (ph == 12) gctx.pad_current |= RE15_PAD_BIT_CROSS;
                    }
                    /* GESTEUERT wird dagegen nur im echten Gameplay. Waehrend Intro/Cutscene haelt
                     * player_mode 2 den Spieler ohnehin fest, und seine Koordinaten sind dort
                     * Platzhalter — gemessen hakte der Regler sonst sofort ein Zwischenziel bei
                     * (-31000,31000) ab und war aus dem Tritt, bevor der Lauf begann. */
                    if (ap_mode && g_scd.player_mode != 2) {
                        re15_actor_t *ap_pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                        int32_t tx = ap_tx, tz = ap_tz;
                        int have_target = (ap_mode == 2 || ap_mode == 3);
                        if (ap_mode == 1 && ap_slot >= 0 && ap_slot < RE15_AOT_MAX) {
                            const re15_aot_t *ga = &g_aot.slots[ap_slot];
                            /* Ein Slot zaehlt nur als Ziel, wenn er aktiv ist UND ein echtes
                             * Rechteck hat. Nach dem Selbst-Tuer-Reenter werden die AOTs neu
                             * installiert; dabei stehen Slots kurzzeitig aktiv, aber entartet
                             * (Mitte (0,0), halb (0,0)) in der Liste — der Regler hat die dann
                             * faelschlich als "erreicht" abgehakt (gemessen: Slots 9 und 8 galten
                             * schon bei (3512,14578) als erledigt, obwohl die Treppen bei
                             * x<-20000 liegen). */
                            if (ga->active && (ga->half_w > 0 || ga->half_h > 0)) {
                                tx = ga->x; tz = ga->z;
                                have_target = 1;
                            }
                        }
                        /* HAT DIE AKTION GEGRIFFEN? Tuer und Treppe VERSETZEN den Spieler. Dieser
                         * Sprung ist die einzige verlaessliche Erfolgsmeldung — und er muss HIER
                         * geprueft werden, VOR der Ankunftspruefung: nach dem Versetzen ist der
                         * Spieler weit vom alten Ziel entfernt, der Halte-Zweig laeuft also gar
                         * nicht mehr an. Genau deshalb steuerte der Regler nach dem geoeffneten
                         * Tor wieder auf das Tor zu, obwohl er laengst im Hof stand. */
                        if (ap_hold > 0) {
                            long long jx = ap_pl->x - ap_lastx, jz = ap_pl->z - ap_lastz;
                            if (jx * jx + jz * jz > 3000LL * 3000LL) {
                                fprintf(stderr, "[auto] Aktion hat versetzt -> (%d,%d), Ziel %d fertig\n",
                                        ap_pl->x, ap_pl->z, ap_idx);
                                ap_idx++; ap_hold = 0; ap_stuck = 0; ap_avoid = 0; ap_bestcross = -1;
                                ap_lastx = ap_pl->x; ap_lastz = ap_pl->z;
                                have_target = 0;          /* diesen Frame nicht mehr lenken */
                            }
                        }
                        /* (Das Wegbestaetigen offener Nachrichten liegt WEITER OBEN und ist auf
                         * das Gameplay beschraenkt. Hier stand frueher ein zweiter, ungegateter
                         * Nachrichten-Zweig — und weil message_active nach dem Intro STEHEN bleibt
                         * (RE-Semantik: bis der Spieler wegdrueckt), sass der Regler dort fest und
                         * hat nie gelenkt, obwohl player_mode laengst 0 war.) */
                        if (have_target) {
                            /* NICHT auf der Luftlinie steuern — die endet zuverlaessig in einer
                             * Wand (gemessen: ROOM1130, Dauerhaenger bei (-768,3332)). Der Raum
                             * bringt seinen eigenen Navigationsgraphen mit (RDT-Bloecke), und die
                             * Gegner-KI hat dafuer schon einen byte-true Pfadfinder. Den benutzen:
                             * Zwischenpunkt holen und DEN ansteuern; das Endziel bleibt der
                             * Abstands-Test unten. */
                            int32_t ftx = tx, ftz = tz;
                            {
                                uint8_t zs = re15_nav_zone_from_pos((int16_t)ap_pl->x, (int16_t)ap_pl->z);
                                uint8_t zg = re15_nav_zone_from_pos((int16_t)tx, (int16_t)tz);
                                if (zs != zg) {
                                    re15_nav_pathfind(ap_pl, zs, zg, (int16_t)tx, (int16_t)tz);
                                    int16_t wx = 0, wz = 0;
                                    re15_nav_dbg_waypoint(&wx, &wz);
                                    if (wx || wz) { tx = wx; tz = wz; }
                                }
                            }
                            int32_t dx = tx - ap_pl->x, dz = tz - ap_pl->z;
                            long long dist2 = (long long)dx * dx + (long long)dz * dz;
                            /* ZIELWINKEL AUSRECHNEN, nicht ausprobieren.
                             * Blickrichtung ist (cos(rot_y), -sin(rot_y)) (actor_locomotion.c:
                             * x += c*v, z -= s*v). Fuer die Richtung (dx,dz) muss also gelten
                             * cos(theta) ~ dx und sin(theta) ~ -dz, d.h. theta = atan2(-dz, dx),
                             * umgerechnet in die 0..4095-Einheiten des Spiels.
                             * Welche Taste dreht wohin? Steht im Port, muss nicht geraten werden:
                             * LEFT erhoeht rot_y, RIGHT verringert ihn (player_common.c:385-386,
                             * jeweils & 0xfff). Die frueher hier stehende Ausprobier-Heuristik
                             * ("dreh, miss, sonst andersrum") hat den Spieler Runden drehen
                             * lassen — das war der Grund, nicht die Geometrie. */
                            int tgt_yaw = (int)(atan2((double)(-dz), (double)dx)
                                                * 4096.0 / (2.0 * 3.14159265358979)) & 0xfff;
                            int yaw_err = (((tgt_yaw - (int)ap_pl->rot_y) + 0x800) & 0xfff) - 0x800;
                            long long acr = (yaw_err < 0 ? -yaw_err : yaw_err);
                            long long cross = yaw_err;     /* >0 = nach LINKS drehen */
                            long long dot   = (acr < 0x400) ? 1 : -1;   /* Ziel grob voraus? */
                            /* Ankunft zaehlt gegen das ENDziel, gesteuert wird auf den Zwischenpunkt. */
                            long long fdx = ftx - ap_pl->x, fdz = ftz - ap_pl->z;
                            long long fdist2 = fdx * fdx + fdz * fdz;
                            int last = (ap_idx >= ap_n - 1);
                            long long arrive = last ? 700LL * 700LL : 1200LL * 1200LL;
                            if (fdist2 < arrive && !last) {
                                /* Zwischenziele vom Typ AOT (Treppe, Selbst-Tuer) muessen AUSGELOEST
                                 * werden, nicht nur beruehrt: die Treppe startet ueber den
                                 * Aktionsknopf (re15_stair_try_start(rdt, g_aot_action_pressed) in
                                 * game_step_common.c). Deshalb dort 40 Frames die Taste halten und
                                 * erst danach weiterschalten; reine xz-Punkte werden nur passiert. */
                                /* (Die Teleport-Erkennung liegt WEITER OBEN, vor der
                                 * Ankunftspruefung — hier waere sie wirkungslos, weil der Spieler
                                 * nach dem Versetzen gar nicht mehr in Zielnaehe ist.) */
                                if ((ap_mode == 1 || ap_mode == 3) && ap_hold < 90) {
                                    /* PULSEN, nicht dauerhalten. Eine Tuer oeffnet auf die
                                     * PRESS-FLANKE (DAT_800ac76c & 0x80 = virtuelle Flanke = rohes
                                     * Quadrat -> ACTION-Scan FUN_80042bac(kind=0x10) -> sce-2
                                     * @0x800430bc). Ein 40-Frame-Dauerdruck erzeugt genau EINE
                                     * Flanke ganz am Anfang — und wenn der Spieler da noch laeuft
                                     * oder nicht sauber im Rechteck steht, ist sie verpufft
                                     * (gemessen: im Rechteck des Tors gestanden, gehalten, nichts
                                     * passiert). 3 an / 3 aus liefert alle 6 Frames eine neue
                                     * Flanke und deckt nebenbei den 9-Frame-Halte-Zaehler ab. */
                                    /* ...und dabei WEITER auf das Ziel ausrichten. Der
                                     * ACTION-Scan prueft einen Punkt 620 Einheiten VOR dem
                                     * Spieler gegen das AOT-Rechteck (FUN_80042bac, kind=0x10) —
                                     * wer daneben schaut, loest nichts aus, auch wenn er mitten
                                     * im Rechteck steht (genau so blieb das Tor bei (3512,14578)
                                     * zu). Erst ausrichten, dann druecken. */
                                    if (acr > 60) {          /* erst sauber anschauen */
                                        gctx.pad_current |= (yaw_err > 0) ? RE15_PAD_BIT_LEFT
                                                                          : RE15_PAD_BIT_RIGHT;
                                    } else if ((ap_hold % 6) < 3) {
                                        gctx.pad_current |= RE15_PAD_BIT_SQUARE;
                                        /* Und die FLANKE. g_aot_action_pressed kommt aus
                                         * pad_PRESSED (game_step_common.c:156), nicht aus
                                         * pad_current — wer nur letzteres setzt, erzeugt nie eine
                                         * Tuer-Aktion. Genau daran ist das Tor bei (3512,14578)
                                         * haengen geblieben: im Rechteck, ausgerichtet, gepulst,
                                         * und trotzdem passierte nichts. */
                                        if ((ap_hold % 6) == 0) gctx.pad_pressed |= RE15_PAD_BIT_SQUARE;
                                    }
                                    if (ap_hold++ == 0)
                                        fprintf(stderr, "[auto] AOT-Zwischenziel %d (Slot %d) erreicht "
                                                        "bei (%d,%d) — loese aus\n",
                                                ap_idx, ap_slot, ap_pl->x, ap_pl->z);
                                } else {
                                    fprintf(stderr, "[auto] Zwischenziel %d erreicht bei (%d,%d)\n",
                                            ap_idx, ap_pl->x, ap_pl->z);
                                    ap_idx++; ap_hold = 0;
                                    ap_stuck = 0; ap_avoid = 0; ap_bestcross = -1;
                                }
                            } else if (fdist2 < arrive) {
                                /* Am Ziel: Aktion HALTEN. Eine Tuer braucht 9 Frames Halten
                                 * (obj+0x8C, FUN_8002bd44) — ein Tipp reicht nicht. */
                                if (ap_act) gctx.pad_current |= RE15_PAD_BIT_SQUARE;
                                if (ap_hold++ == 0)
                                    fprintf(stderr, "[auto] Ziel erreicht (%d,%d), halte Aktion\n",
                                            ap_pl->x, ap_pl->z);
                            } else if (ap_avoid > 0) {
                                /* WAND-AUSWEICHEN: die Luftlinie zum Ziel ist selten frei. Steht der
                                 * Spieler trotz gedruecktem VORWAERTS, wird hier eine Weile im Bogen
                                 * gelaufen (drehen UND vorwaerts = Tank-Kurve), danach zielt der
                                 * Regler wieder direkt. Bringt das nichts, wird die Bogenrichtung
                                 * gewechselt, sodass beide Seiten probiert werden. */
                                ap_avoid--;
                                gctx.pad_current |= RE15_PAD_BIT_UP;
                                gctx.pad_current |= (ap_avoid_dir > 0) ? RE15_PAD_BIT_RIGHT
                                                                       : RE15_PAD_BIT_LEFT;
                            } else {
                                /* Erst auf den Zielwinkel drehen, dann geradeaus. 40 Einheiten
                                 * Toleranz = gut 3 Grad (4096 = 360). */
                                if (acr > 40) {
                                    gctx.pad_current |= (yaw_err > 0) ? RE15_PAD_BIT_LEFT
                                                                      : RE15_PAD_BIT_RIGHT;
                                } else {
                                    gctx.pad_current |= RE15_PAD_BIT_UP;
                                }
                            }
                            /* Fest? Position hat sich trotz Laufversuch kaum bewegt. */
                            {
                                int32_t mx = ap_pl->x - ap_lastx, mz = ap_pl->z - ap_lastz;
                                if ((long long)mx * mx + (long long)mz * mz < 400LL) {
                                    if (++ap_stuck > 25 && ap_avoid == 0) {
                                        ap_avoid = 45; ap_avoid_dir = -ap_avoid_dir; ap_stuck = 0;
                                        fprintf(stderr, "[auto] fest bei (%d,%d) — Bogen %s\n",
                                                ap_pl->x, ap_pl->z, ap_avoid_dir > 0 ? "rechts" : "links");
                                    }
                                } else ap_stuck = 0;
                                ap_lastx = ap_pl->x; ap_lastz = ap_pl->z;
                            }
                            if ((g_engine.frame_count % 60) == 0)
                                fprintf(stderr, "[auto] Spieler(%d,%d) zone=%u -> Wegpunkt(%d,%d) "
                                                "Endziel(%d,%d) zone=%u d=%lld pad=%04X motion=%d\n",
                                        ap_pl->x, ap_pl->z,
                                        re15_nav_zone_from_pos((int16_t)ap_pl->x, (int16_t)ap_pl->z),
                                        tx, tz, ftx, ftz,
                                        re15_nav_zone_from_pos((int16_t)ftx, (int16_t)ftz),
                                        (long long)(fdist2 / 1000), gctx.pad_current, ap_pl->motion);
                        }
                    }
                }

                /* ORIGINAL-DEBUG-MENUE ("UTILITY MENU", PSX.EXE @0x80014444) — Logik in
                 * engine/src/debug_menu_common.c, jede Konstante dort mit ihrer Adresse.
                 *
                 * WARUM IM PORT: fuer den 1:1-Vergleich muessen Original und Port einen Raum ueber
                 * DENSELBEN Weg betreten. Solange das Original per Debug-JUMP hineinspringt und der
                 * Port per RE15_START_ROOM gesetzt wird, vergleicht der Harness zwei verschiedene
                 * Situationen.
                 *
                 * Das Menue liest im Original zwei Halbwoerter: 0x800AC760 = held und 0x800AC762 =
                 * edge (Schreiber @0x80030564 bzw. @0x800305A0). In diesem Wort liegt das D-Pad auf
                 * den Bits 12-15 und liegen die Face-Tasten auf 4-7 — gegenueber dem Pad-Wort des
                 * Ports sind die beiden Bytes also vertauscht; re15_debug_menu_pad() dreht sie.
                 *
                 * SPIELERPOSITION (KORRIGIERT 2026-08-23): der MENUE-Lade-Zweig @0x80014A44-58
                 * schreibt zwar nur 0x800B5359=1, 0x800AC9A8=0 und 0x800BBE5C=0 — aber die
                 * dadurch gestartete Transition-FSM (@0x8001c990, liest 0x800B5359 @0x8001c994)
                 * ruft den JUMP-Executor FUN_8001d600 (@0x8001ca54), dessen DEBUG-Zweig
                 * (0x800AC9A8==0 @0x8001d618) die Position AUS DEM DEBUG.BIN-TABELLENSATZ liest
                 * (X @0x8001d6a8, Z @0x8001d720, Band @0x8001d798, Y=-Band*0x708 @0x8001d7b8-d4,
                 * Cut/Szenario=0 @0x8001d818-20, Yaw unangetastet). Der Sprung unten nutzt genau
                 * diese Felder (re15_debug_menu_jump_spawn); re15_room_spawns ist nur noch der
                 * Fallback ohne DEBUG.BIN. */
                {
                    uint16_t dbg_held = re15_debug_menu_pad(gctx.pad_current);
                    uint16_t dbg_edge = re15_debug_menu_pad(gctx.pad_pressed);
                    /* RE15_DEBUG_JUMP="<hexraum>@<frame>" — automatisierter Messlauf-Sprung.
                     * Er setzt NUR den Menue-Cursor und legt die Lade-Flanke an; ausgefuehrt wird
                     * er vom normalen re15_debug_menu_tick() unten, also ueber genau denselben
                     * Weg wie ein Quadrat-Druck des Nutzers. Ersetzt RE15_START_ROOM, das an
                     * re15_room_apply_pending vorbeibootete und darum anderes zeigte als das Spiel. */
                    {
                        static int dj_room = -1, dj_frame = 0, dj_read = 0, dj_done = 0;
                        if (!dj_read) {
                            dj_read = 1;
                            const char *dj = getenv("RE15_DEBUG_JUMP");
                            if (dj && *dj) {
                                unsigned r = 0; int f = 0;
                                if (sscanf(dj, "%x@%d", &r, &f) >= 1) { dj_room = (int)r; dj_frame = f; }
                                /* "@gp" = springe, sobald das SPIEL wirklich spielbar ist. Ein fester
                                 * Frame reicht nicht: waehrend des Intros halten die Latches
                                 * flag(1,27)/flag(2,7) den Spieler jeden Frame auf player_mode 2
                                 * (oben, "scripted"), und ein Sprung mitten heraus liefert einen
                                 * Spieler, der keine Eingabe annimmt. Gemessen: Sprung bei Frame 1
                                 * nach ROOM1130 -> pmode blieb 2, der Autopilot stand fest. */
                                if (strstr(dj, "@gp")) dj_frame = -1;
                            }
                        }
                        if (dj_room >= 0 && !dj_done &&
                            (dj_frame < 0 ? (g_scd.player_mode != 2 && g_engine.frame_count > 60)
                                          : ((int)g_engine.frame_count >= dj_frame))) {
                            dj_done = 1;
                            if (re15_debug_menu_point_at((unsigned)dj_room)) {
                                /* MESSLAUF-FALLE (gemessen 2026-08-22): laeuft im selben Bild der
                                 * Autopilot/ein Input-Skript, liegen LINKS/RECHTS im HELD-Wort und
                                 * re15_debug_menu_tick() verschiebt den gerade gesetzten Cursor noch
                                 * VOR dem Laden um eine Zeile — der Sprung landete auf ROOM1080
                                 * statt ROOM1090 ("AUTO-JUMP -> ROOM1090" + "JUMP -> 108 ELEVATOR").
                                 * Fuer dieses eine Bild die Halte-Bits fallen lassen; die Lade-Flanke
                                 * bleibt exakt der Quadrat-Druck von oben. */
                                dbg_held = 0;
                                dbg_edge |= RE15_DBG_EDGE_LOAD;   /* == Quadrat in der JUMP-Zeile */
                                fprintf(stderr, "[debug-menu] AUTO-JUMP -> ROOM%04X (Frame %u)\n",
                                        (unsigned)dj_room, g_engine.frame_count);
                            } else {
                                fprintf(stderr, "[debug-menu] AUTO-JUMP: ROOM%04X steht nicht in der "
                                                "JUMP-Tabelle\n", (unsigned)dj_room);
                            }
                        }
                    }
                    /* JUMP-Tabelle einmalig aus DEBUG.BIN laden (RAW-Abbild @0x800c0000 im
                     * Original; Tabelle @Datei 0x263a/0x2642 — Muster wie die Breitentabelle
                     * DEBUG.BIN[0x4416] in render_pc.c). Ohne Datei bleibt der Header-Fallback. */
                    {
                        static int dbin_once = 0;
                        if (!dbin_once) {
                            dbin_once = 1;
                            int dbsz = 0;
                            uint8_t *db = pc_read_shared("BIN/DEBUG.BIN", &dbsz);
                            if (db) { re15_debug_menu_set_bin(db, (unsigned)dbsz); free(db); }
                        }
                    }
                    if (getenv("RE15_DBGMENU_DIAG") && (g_engine.pad_pressed & RE15_PAD_BIT_SELECT))
                        fprintf(stderr, "[debug-menu] DIAG raw=%04x cfg=%04x frame=%u\n",
                                g_engine.pad_pressed, gctx.pad_pressed, g_engine.frame_count);
                    if (!re15_debug_menu_open()) {
                        if (gctx.pad_pressed & RE15_PAD_BIT_SELECT) {
                            re15_debug_menu_toggle();
                            /* Open-Block @0x80014490-0x800144E0: Cursor auf den AKTUELLEN Raum
                             * (DAT_800bbe5e := DAT_800b0fe0, (&DAT_800bbe5f)[st] := DAT_800b0fe2). */
                            re15_debug_menu_sync_cursor(g_current_room_id);
                            fprintf(stderr, "[debug-menu] OPEN (frame %u)\n", g_engine.frame_count);
                        }
                    } else {
                        if (re15_debug_menu_tick(dbg_held, dbg_edge)) {
                            const re15_debug_menu_t *dm = re15_debug_menu_state();
                            unsigned  droom = (unsigned)dm->load_room << 4;   /* 0x114 -> ROOM1140 */
                            int32_t   dx = 0, dy = 0, dz = 0;
                            int16_t   dyaw = 0;
                            int       dcut = 0, dband = 0;
                            /* ⛔ FIX 2026-08-23 (Nutzer-Report "Debug-Menue schickt an andere
                             * Stellen als das Original, z.B. room105"): der JUMP-Executor
                             * FUN_8001d600 (Debug-Zweig, DAT_800ac9a8==0 @0x8001d618) liest den
                             * Spawn AUS DEM DEBUG.BIN-TABELLENSATZ: X @0x8001d6a8, Z @0x8001d720,
                             * Band @0x8001d798, Y = -Band*0x708 @0x8001d7b8-d4; Cut/Szenario = 0
                             * (`sb zero DAT_800afbb5` @0x8001d818 / `sh zero DAT_800b0fe4`
                             * @0x8001d820); Yaw wird NICHT geschrieben (einziger Yaw-Store der
                             * Funktion ist der Tuer-Zweig @0x8001d8e8) -> Spieler-Yaw bleibt.
                             * Vorher nahm der Port den Tuer-Eintritts-Spawn aus re15_room_spawns
                             * (fuer 1050: X 14700/Z -13500/Cut 4 statt 20600/12350/Cut 0 = das
                             * andere Korridor-Ende). re15_room_spawns bleibt nur als Fallback
                             * ohne DEBUG.BIN-Tabelle (solche Raeume kann das Original-Menue gar
                             * nicht anspringen). */
                            if (re15_debug_menu_jump_spawn(&dx, &dz, &dband)) {
                                dy   = -(int32_t)dband * 0x708;   /* @0x8001d7b8-d4 (225*8=1800) */
                                dyaw = g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y;
                                dcut = 0;                         /* @0x8001d818-20 */
                            } else {
                                for (int _ri = 0; _ri < RE15_ROOM_COUNT; _ri++)
                                    if (re15_room_ids[_ri] == droom) {
                                        const re15_room_spawn_t *rs = &re15_room_spawns[_ri];
                                        dx = rs->x; dy = rs->y; dz = rs->z;
                                        dyaw = rs->yaw; dcut = rs->cut;
                                        break;
                                    }
                            }
                            re15_room_request_change(droom, dx, dy, dz, dyaw, dcut);
                            fprintf(stderr, "[debug-menu] JUMP -> %03x %s (ROOM%04X) spawn=(%d,%d,%d) cut=%d\n",
                                    dm->load_room, re15_debug_menu_room_name(), droom,
                                    dx, dy, dz, dcut);
                        }
                        /* KEIN Pad-Schlucken. Im Original friert das Menue NICHTS ein: FUN_8001443c
                         * kehrt zurueck, danach laufen State-Dispatch, Subsystem-Ticks und die
                         * Entity-/AI-Schleife normal weiter (@0x8001C990), und alle Pad-Zugriffe der
                         * Menuefunktion sind reine LESEzugriffe — 0x800AC760/762 werden nirgends
                         * geloescht (@0x800145EC ff.). Der Spieler laeuft also weiter, waehrend das
                         * Menue offen ist. Mein erster Wurf hat hier das Pad genullt; das war der
                         * Grund, warum sich beim Druck auf Select nichts mehr bewegte. */
                    }
                }
                /* RL-4 per-room model-bank preload: load every model the room's roster needs BEFORE
                 * the engine AI reads it. scd_vm_tick (above) spawns the roster via Sce_em_set; the
                 * enemy AI inside re15_game_step then reads re15_enemy_find(type) for the byte-true EM
                 * clip lengths / skeleton. The lazy render-loop loader (pc_enemy_load @NPC render)
                 * runs AFTER this step, so without a pre-load the AI saw a NULL bank on the frames
                 * before the actor first rendered and fell back to the EM040 (s_irons_clip_len) table.
                 * Loading the roster here from the spawn table makes each bank resident the frame its
                 * actor spawns. pc_enemy_load is idempotent (early-returns once re15_enemy_find hits).
                 * 0x47 SEIT 7b-UMSTELLUNG 2026-08-09 MITGELADEN: Elliots Plc_dest-Walks laufen jetzt
                 * byte-true in der State-4-Sub-VM, deren OWN-Kanal (+0x170/+0x174, f314 @0x80051714/18)
                 * die EM047-Bank-1 ist (CDEMD0.EMS Blob 21 @0x336000, 6 Clips {22,16,52,1,50,30}) —
                 * re15_actor_clip_len/re15_npc_channel_anim brauchen die Registry-Bank. Mesh/TIM
                 * rendern weiter aus ELLIOT.PLD (anim_select-0x47-Zweig unveraendert). */
                for (int _pi = 1; _pi < RE15_ACTOR_MAX; _pi++)
                    if (g_actors[_pi].active && g_actors[_pi].type)
                        /* ⛔ 0x26 nach HERKUNFT (2026-08-22): ein per RDT-`Sce_em_set` gesetzter
                         * 0x26er ist in RE1.5 der FEUER-EMITTER (0x80072bac[0x26] = 0x80116288,
                         * Registrierung @0x8011E8F4/@0x8011E8FC) und braucht die RE1.5-Bank EM26;
                         * nur die vom RE2-Adult-Spawner erzeugten (`addiu a0,zero,38`
                         * @0x80105DE8, Feld re2s_baby_spawned) sind echte RE2-Baby-Spinnen. */
                        pc_enemy_load_ex(g_actors[_pi].type,
                                         g_actors[_pi].type != 0x26u
                                             || g_actors[_pi].re2s_baby_spawned);
                /* WELLE F: die RE2-Adult-Spinne (0x25) erzeugt Baby-Spinnen (0x26) ZUR LAUFZEIT
                 * (FUN_80105D38, Aufrufstellen @0x8010322C/@0x801033D8/@0x801034DC/@0x80104478/
                 * @0x801045A4/@0x801046B8/@0x801047D8/@0x80104830). Die Roster-Schleife darueber
                 * holt die Bank erst, wenn das erste Baby schon steht — die AI laese im
                 * Spawn-Frame eine NULL-Bank. Deshalb hier vorziehen, sobald ein Adult im Raum
                 * ist; pc_enemy_load ist idempotent.
                 * ⛔ KORREKTUR 2026-08-22: der alte Kommentar behauptete "Typ 0x26 steht in KEINEM
                 * Sce_em_set der 240 RDTs (eigener Zensus: 0 Records)". Das war der Werkzeug-
                 * Fehler, der schon im Kopf von enemy_ai_re2_spider.c widerrufen ist — es sind 7
                 * Records, alle in ROOM1090 (@0x2214/0x2228/0x223C/0x2250/0x2264/0x2278/0x228C,
                 * `44 xx 26 vv ...`). Die sind aber Feuer-Emitter, keine Spinnen, und ROOM1090
                 * fuehrt keinen Adult 0x25 — dieser Vorzug feuert dort also nicht. */
                /* MIXED (2026-08-23): typ-bezogen auf die SPINNE — 0x25/0x26 sind dort RE1.5,
                 * der Vorzug entfaellt korrekt (es gibt dann gar keinen RE2-Baby-Spawner). */
                if (re15_ai_re2_for_type(0x25u))
                    for (int _pi = 1; _pi < RE15_ACTOR_MAX; _pi++)
                        if (g_actors[_pi].active && g_actors[_pi].type == 0x25) {
                            pc_enemy_load_ex(0x26, 1);     /* echte RE2-Babys -> RE2-Bank */
                            break;
                        }
                /* OPFER-BANK-LEIHGABE fuer die ROOM1210-Arme (Typ 0x1A, Nutzer-Auftrag "hole die
                 * Animation aus Resident Evil 2"). EM01A hat kein Paar 3 (gemessen: 4 Meshes,
                 * 4 Bones, 4 Clips, nur HAUPT + LOCO), also leiht der Arm die Opfer-Bank des
                 * ZOMBIES — Paar 3 posiert LEON und ist PL00-kompatibel, ist also dasselbe
                 * Datenmaterial, das Leon in jedem anderen Griff schon traegt (Beleg-Block bei
                 * re15_victim_donor_set). Der Ladeweg ist der normale: pc_enemy_load waehlt
                 * nach AI-Geschmack, unter RE2 kommt damit RE2s Ringkampf, unter RE1.5 der
                 * eigene — beide Belegungen stehen byte-true in re15_victim_clip_map.
                 * Steht pro Bild neu, damit die Anmeldung mit dem Raum kommt UND geht. */
                {   int _wr = 0;
                    for (int _pi = 1; _pi < RE15_ACTOR_MAX; _pi++)
                        if (g_actors[_pi].active && g_actors[_pi].type == 0x1Au) { _wr = 1; break; }
                    static int _wr_logged = 0;
                    if (_wr) {
                        pc_enemy_load(0x10);               /* idempotent */
                        re15_enemy_bank_t *_db = re15_enemy_find(0x10);
                        if (_db && _db->victim_ok) re15_victim_donor_set(0x1Au, 0x10u);
                        else                       re15_victim_donor_set(0, 0);
                        if (!_wr_logged) {
                            _wr_logged = 1;
                            fprintf(stderr, "[enemy] EM01A-Griff: Opferbank von EM010 geliehen "
                                            "(victim_ok=%d, %d Clips)\n",
                                    _db ? (int)_db->victim_ok : -1,
                                    (_db && _db->victim_ok) ? _db->anim_victim.clip_count : -1);
                        }
                    } else {
                        re15_victim_donor_set(0, 0);
                        _wr_logged = 0;
                    }
                }
                re15_game_step(&gctx);
                /* ITEM-GET-MODAL-FSM — NACH dem Spieler-Step, byte-true zur Frame-Ordnung des
                 * Originals (Hauptloop FUN_8001c6e8: Dispatcher @0x8001ce0c `jal 0x80031c44` ->
                 * ... -> Modal-FSM @0x8001ce34 `jal 0x8001db28`; selbst nachdisassembliert
                 * 2026-08-29). Wirkung: (a) im Confirm-Frame sieht der Step das Modal noch AKTIV
                 * (Early-Return game_step_common.c) -> kein ACTION-Scan mit der Confirm-Flanke;
                 * im Folgeframe ist die Host-Flanke weg -> das zweite Item eines Stapels braucht
                 * einen NEUEN SQUARE-Druck. Das Original sichert dasselbe zusaetzlich ueber den
                 * Edge-Verbrauch des Confirms (`ori v0,zero,0xffff` @0x80028578 + `sw ->
                 * DAT_800ac768` @0x80028588: Folgeframe-Flanke = (prev^held)&held = 0) und den
                 * sce-9-Re-Arm-Guard auf den Modal-State (@0x8004332c/34). (b) der Modal-INIT
                 * (state 1) tickt wieder im TRIGGER-Frame (der Scan armt s_state=1, die FSM tickt
                 * danach) statt einen Host-Frame spaeter.
                 * Pad-Woerter wie an der alten Stelle: VIRTUELLE Flanke/HELD (wave-6 finding 4,
                 * Tabelle @0x80073dbc via FUN_80030444; Typewriter-FF liest DAT_800ac768
                 * @0x8002820c/@0x80028214). Kadenz unveraendert 30 Hz.
                 * OFFEN (eigene Paritaetsfrage, nicht dieser Report): das Original flutet beim
                 * Message-Dismiss das virtuelle HELD-Wort (0xFFFF @0x80028588, auch page-advance
                 * @0x80028468 / full-message @0x800286b0) — dadurch ist dort im Folgeframe JEDE
                 * Taste flankenlos, nicht nur SQUARE. Der Port hat kein persistentes
                 * prev/held/edge-Tripel; falls das je sichtbar wird, gehoert der Verbrauch in ein
                 * eigenes virtuelles Pad-Modell. */
                if ((target_fps == 30 || (g_engine.frame_count & 1) == 0) && re15_item_modal_active()) {
                    re15_item_modal_tick(re15_pad_virtual_word(pc_pad_config((uint16_t)g_engine.pad_pressed)),
                                         re15_pad_virtual_word(pc_pad_config((uint16_t)g_engine.pad_current)));
                }
            }
            /* PARITY STATE-LOG (RE15_STATE_LOG=path): append per-tick player pose + each live
             * enemy's AI state so the port run can be diffed NUMERICALLY against the DuckStation
             * savestate (re15_enemy_state.py). One line/tick: F<frame> pad=<hex> PL(x,z,rot,hp)
             * then per zombie [slot type st ss1 dist]. This is the parity oracle's measuring tape. */
            {
                static FILE *s_state_log = NULL; static int s_slog_init = 0;
                if (!s_slog_init) { s_slog_init = 1;
                    const char *lp = getenv("RE15_STATE_LOG");
                    if (lp && *lp) s_state_log = fopen(lp, "w"); }
                if (s_state_log) {
                    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                    extern int re15_player_aim_clip(void);
                    extern int re15_render_pc_dbg_slot_loaded(int slot);
                    {
                        int eqs = re15_inv_equipped_slot();
                        int mag = (eqs >= 0 && eqs < RE15_INV_MAX_SLOTS) ? g_inv.slots[eqs].qty : -1;
                        int bxs = re15_inv_find_item(0x15);
                        int box = (bxs >= 0) ? g_inv.slots[bxs].qty : -1;
                        fprintf(s_state_log,
                                "F%u pad=%04x PL(%d,%d,rot=%d,hp=%d) pst=%d ps1=%d ps2=%d mo=%d ac=%d fx=%d mg=%d bx=%d sl=%d%d%d%d cam=%d",
                                g_engine.frame_count, g_engine.pad_current,
                                pl->x, pl->z, pl->rot_y, pl->hp,
                                pl->state, pl->sub_state_1, pl->sub_state_2,
                                pl->motion, re15_player_aim_clip(), re15_esp_fx_count(), mag, box,
                                re15_render_pc_dbg_slot_loaded(20), re15_render_pc_dbg_slot_loaded(21),
                                re15_render_pc_dbg_slot_loaded(22), re15_render_pc_dbg_slot_loaded(23),
                                (int)g_scd.cam_id);
                    }
                    for (int si = 1; si < RE15_ACTOR_MAX; si++) {
                        re15_actor_t *e = &g_actors[si];
                        if (!e->active || e->type == 0) continue;
                        fprintf(s_state_log, " [%d t=%02x st=%d ss1=%d ss2=%d ss3=%d g=%02x mo=%d af=%d "
                                             "stun=%d d=%u @(%d,%d,r%d)]",
                                si, e->type, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
                                e->grid_id, e->motion, e->anim_frame, (int)e->hit_stun,
                                e->ai_dist, e->x, e->z, e->rot_y);
                    }
                    fputc('\n', s_state_log); fflush(s_state_log);
                }
            }
            /* DEATH PRESENTATION (byte-true FUN_8003694c): fade the scene to black over the 0x78 death
             * timer (g_death_fade), then show the YOU DIED graphic (YOUDIED.TIM) for ~2s before the RE
             * continue = reload the current room. (Original: YOU DIED -> title; the port has no title
             * screen, so the continue-reload is the tail — a healthy room clears g_gameover_active.) */
            {
                extern void re15_render_pc_set_fade(int a);
                extern void re15_render_pc_set_white_fade(int a);
                extern void re15_render_pc_set_black_bg(int on);
                extern void re15_render_pc_set_gameover_flyin(int t);
                extern void re15_render_pc_show_gameover(const re15_tim_t *tim);
                extern void re15_render_pc_hide_gameover(void);
                extern void re15_render_pc_show_title(const re15_tim_t *tim);
                extern void re15_render_pc_hide_title(void);
                static re15_tim_t s_youdied = {0};
                static int s_yd_tried = 0;
                /* THE BYTE-TRUE YOU-DIED CHAIN (game_step FSM drives; this block only renders):
                 * white additive flash + heartbeat pulses (g_death_white), flat-black background
                 * (g_death_blackbg), YOU DIED letters flying in over 50 frames (g_death_flyin),
                 * the sub-6 exit fade-to-black (g_death_fade), then the mode-cycle to TITLE. */
                if (re15_player_is_dead() || g_gameover_active) {
                    re15_render_pc_set_fade(g_death_fade);
                    re15_render_pc_set_white_fade(g_death_white);
                    re15_render_pc_set_black_bg(g_death_blackbg);
                    if (g_death_flyin >= 0) {
                        if (!s_yd_tried) { s_yd_tried = 1; int ysz = 0;
                            uint8_t *yb = pc_read_shared("DATA/YOUDIED.TIM", &ysz);
                            if (yb) re15_tim_parse(yb, ysz, &s_youdied); }
                        if (s_youdied.pixels) {
                            re15_render_pc_show_gameover(&s_youdied);
                            re15_render_pc_set_gameover_flyin(g_death_flyin);
                        }
                    }
                }
                if (g_gameover_active) {
                    /* FE-5.3: the YOU-DIED presentation completed (sub-6 ctr 0x6d). The original stops
                     * all audio and leaves the in-game module for the attract/title flow (@0x80015810-38,
                     * gate @0x8001d1e8). Byte-true: hand off to the TITLE MENU via the mode-cycle — set
                     * mode=TITLE and break the game loop; main() jumps back to re_title, where CONTINUE
                     * reloads the last card save (FE-4.3) and NEW GAME restarts. (Replaces the old inline
                     * "reload-the-current-room" stand-in.) */
                    re15_audio_seq_ctl(0, 2);    /* SsSeqStop the 3 BGM/SEQ layers (FUN_80043958) */
                    re15_audio_seq_ctl(1, 2);
                    re15_audio_seq_ctl(2, 2);
                    re15_render_pc_hide_gameover();
                    re15_render_pc_set_gameover_flyin(-1);
                    re15_render_pc_set_black_bg(0);
                    re15_render_pc_set_white_fade(0);
                    re15_render_pc_set_fade(0);
                    /* Reset the world to a fresh-boot base: NEW GAME (re15_gameflow_new_game doesn't
                     * clear these) starts clean; CONTINUE applies its save over the clean base. The
                     * death FSM self-clears when the re-entered game revives the player (game_step:142). */
                    memset(&g_inv,  0, sizeof g_inv);
                    memset(&g_game, 0, sizeof g_game);
                    g_gameflow.mode = RE15_MODE_TITLE;
                    running = 0;
                }
            }
            /* DEBUG: per-tick kneel trace — find the exact frames + camera cut where Leon
             * kneels (motion 10-12), so the autoshot can target the kneel-down vs ablauf4. */
            {
                re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                if (pl->motion >= 10 && pl->motion <= 12)
                    fprintf(stderr, "[knee] F%u cut=%d mo=%d af=%d frac=%d | pl pos=(%d,%d,%d) active=%d rot=%d\n",
                            (unsigned)g_engine.frame_count, active_cut_idx,
                            (int)pl->motion, (int)pl->anim_frame, (int)pl->anim_frac,
                            (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->active, (int)pl->rot_y);
                if ((g_engine.frame_count % 30u) == 0u)
                    fprintf(stderr, "[walk] F%u cut=%d mo=%d af=%d frz=%d | pl pos=(%d,%d,%d) rot=%d\n",
                            (unsigned)g_engine.frame_count, active_cut_idx, (int)pl->motion, (int)pl->anim_frame,
                            (int)pl->anim_freeze, (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->rot_y);
                /* RE15_INV_DBG: per-frame status-screen FSM probe (wave-2 bring-up). */
                if (getenv("RE15_INV_DBG"))
                    fprintf(stderr, "[invdbg] F%u stage=%d open=%d phase=%d sub=%d frozen=%d "
                            "fade0(lvl=%04x step=%d drawn=%d out=%d) bg=%d\n",
                            (unsigned)g_engine.frame_count, re15_menu_stage(), re15_menu_is_open(),
                            re15_menu_phase(), re15_menu_substate(), re15_menu_gameplay_frozen(),
                            (unsigned)g_fade_ch[0].level, (int)g_fade_ch[0].step,
                            (int)g_fade_ch[0].drawn, (int)g_fade_ch[0].out_r,
                            re15_bg_is_loaded());
                /* RE15_MOTRACE: log player motion/af/flags/player_mode EVERY frame (find which
                 * clip drives the standing arm-gesture before the kneel). */
                if (getenv("RE15_MOTRACE"))
                    fprintf(stderr, "[mot] F%u cut=%d mo=%d af=%d flg=0x%02X frac=%d pmode=%d\n",
                            (unsigned)g_engine.frame_count, active_cut_idx, (int)pl->motion,
                            (int)pl->anim_frame, (unsigned)pl->anim_flags, (int)pl->anim_frac,
                            (int)g_scd.player_mode);
            }
            /* DEBUG: force-fire a room SCD event so an AOT-triggered cutscene runs headless
             * for frame-exact comparison vs ablauf4 (RE15_FORCE_EVENT=8 = room1150 sub08
             * Irons-kneel, normally fired by AOT 6 when Leon walks to Irons). Fires once,
             * after the room SCD (main00) has installed AOTs + actors. */
            {
                static int s_fe_init = 0, s_fe_id = -1, s_fe_done = 0, s_fe_frame = 20;
                if (!s_fe_init) { const char *fe = getenv("RE15_FORCE_EVENT");
                                  if (fe && *fe) { s_fe_id = atoi(fe);
                                      /* "N@F": erst ab Frame F feuern (Debug-Harness — der
                                       * room-ok-Check allein reicht bei RE15_GOTO_ROOM nicht,
                                       * die Room-ID steht schon VOR dem SCD-Swap um). */
                                      const char *at = strchr(fe, '@');
                                      if (at) s_fe_frame = atoi(at + 1); }
                                  s_fe_init = 1; }
                /* If RE15_GOTO_ROOM is used, wait until we are actually IN that room
                 * (g_current_room_id) before firing — so the kneel fires AFTER the
                 * cross-room cinematic-bank reload, testing the real door path. */
                const char *fe_goto = getenv("RE15_GOTO_ROOM");
                unsigned fe_target = (fe_goto && *fe_goto) ? (unsigned)strtol(fe_goto, NULL, 16) : 0;
                int fe_room_ok = (fe_target == 0) || (g_current_room_id == fe_target);
                if (s_fe_id >= 0 && !s_fe_done && g_engine.frame_count >= (uint32_t)s_fe_frame && fe_room_ok) {
                    extern int scd_event_fire(uint8_t);
                    fprintf(stderr, "[force-event] scd_event_fire(%d) at F%u\n",
                            s_fe_id, (unsigned)g_engine.frame_count);
                    scd_event_fire((uint8_t)s_fe_id);
                    s_fe_done = 1;
                }
            }
            /* DEBUG: RE15_SUBSTART="N@F" — startet sub_scd[N] des AKTUELLEN Raums direkt als
             * Thread (exakt die probe_marvin_10d0-Methode scd_thread_start(3, sub_scd[N])),
             * ohne Event-Slot/Prompt-Interferenz. Fuer Render-Verifikation von Cutscenes. */
            {
                static int s_ss_init = 0, s_ss_id = -1, s_ss_frame = 20, s_ss_done = 0;
                static unsigned s_ss_room = 0;   /* optional "#<hexraum>"-Gate (Messlauf) */
                if (!s_ss_init) { const char *se = getenv("RE15_SUBSTART");
                    if (se && *se) { s_ss_id = atoi(se);
                        const char *at = strchr(se, '@'); if (at) s_ss_frame = atoi(at + 1);
                        /* Ohne Raum-Gate feuert der Haken im ERSTEN Raum, dessen Bildzaehler
                         * die Schwelle reisst (der Zaehler wird bei jedem Raumwechsel auf 0
                         * gesetzt) — bei einer Messung im Zielraum also im falschen Raum. */
                        const char *hs = strchr(se, '#');
                        if (hs) s_ss_room = (unsigned)strtol(hs + 1, NULL, 16); }
                    s_ss_init = 1; }
                if (s_ss_id >= 0 && !s_ss_done && g_engine.frame_count >= (uint32_t)s_ss_frame &&
                    (s_ss_room == 0 || g_current_room_id == s_ss_room)) {
                    extern int scd_thread_start(int slot, const uint8_t *pc);
                    if (g_room_rdt_ok && s_ss_id < RE15_RDT_MAX_SUB_SCD &&
                        g_room_rdt.sub_scd[s_ss_id]) {
                        scd_thread_start(3, g_room_rdt.sub_scd[s_ss_id]);
                        fprintf(stderr, "[substart] sub_scd[%d] at F%u\n",
                                s_ss_id, (unsigned)g_engine.frame_count);
                    }
                    s_ss_done = 1;
                }
            }
            /* RE15_GOTO_ROOM=<hex>: debug — auto-queue ONE cross-room change to that
             * room at F30, to test the cross-room cinematic-bank reload headless
             * (e.g. boot 1170 → goto 1150, confirm ROOM1150.RBJ re-overlays). */
            {
                static int s_goto_done = 0, s_goto_id = -1, s_goto_init = 0;
                if (!s_goto_init) { const char *gr = getenv("RE15_GOTO_ROOM");
                    if (gr && *gr) s_goto_id = (int)strtol(gr, NULL, 16); s_goto_init = 1; }
                if (s_goto_id >= 0 && !s_goto_done && g_engine.frame_count >= 30 && !g_room_change.pending) {
                    int idx = 0;
                    for (int i = 0; i < RE15_ROOM_COUNT; i++)
                        if (re15_room_ids[i] == (unsigned)s_goto_id) { idx = i; break; }
                    const re15_room_spawn_t *sp = &re15_room_spawns[idx];
                    g_room_change.pending    = 1;
                    g_room_change.room_id    = re15_room_ids[idx];
                    g_room_change.x = sp->x; g_room_change.y = sp->y; g_room_change.z = sp->z;
                    g_room_change.yaw_4096   = sp->yaw;
                    g_room_change.target_cut = sp->cut;
                    s_goto_done = 1;
                    fprintf(stderr, "[goto] queued room change -> ROOM%04X at F%u\n",
                            (unsigned)s_goto_id, (unsigned)g_engine.frame_count);
                }
            }
            /* DEV ROOM-BROWSER (globalization 2026-06-13): F1 / F2 cycle through every
             * room in the shared tree (re15_room_list.h) by queuing a g_room_change —
             * the SAME consume path below then loads it (RDT + render reset + props +
             * scd_room_reenter + BG + light + msg, all from the RDT). Lets us load ANY
             * of the 240 rooms and see how far it gets. Guarded so it never overrides a
             * door-queued change. Spawn (0,0,0)/cut 0 — may land in a wall, fine for a
             * browse. PC-only, on the FUNCTION keys so it never touches the game keys. */
            {
                extern int re15_input_debug_fkey(int n);
                int rsel = 0;
                if      (re15_input_debug_fkey(2)) rsel = +1;   /* F2 = next room */
                else if (re15_input_debug_fkey(1)) rsel = -1;   /* F1 = prev room */
                if (rsel && !g_room_change.pending) {
                    int idx = 0;
                    for (int i = 0; i < RE15_ROOM_COUNT; i++)
                        if (re15_room_ids[i] == g_current_room_id) { idx = i; break; }
                    idx = (idx + rsel + RE15_ROOM_COUNT) % RE15_ROOM_COUNT;
                    const re15_room_spawn_t *sp = &re15_room_spawns[idx];
                    g_room_change.pending    = 1;
                    g_room_change.room_id    = re15_room_ids[idx];
                    /* Land at the room's authored inbound-door spawn (walkable, correct
                     * band) so you can walk to its doors; (0,0,0) if no inbound door. */
                    g_room_change.x = sp->x; g_room_change.y = sp->y; g_room_change.z = sp->z;
                    g_room_change.yaw_4096   = sp->yaw;
                    g_room_change.target_cut = sp->cut;
                    fprintf(stderr, "[roomsel] -> ROOM%04X (%d/%d) spawn(%d,%d,%d) cut%d\n",
                            g_room_change.room_id, idx + 1, RE15_ROOM_COUNT,
                            sp->x, sp->y, sp->z, sp->cut);
                }
            }
            /* Cross-room DOOR transition (SHARED re15_room_apply_pending,
             * room_common.c): a door to a DIFFERENT room queued g_room_change in
             * the scan above. Identical LOGIC to PSX; the PC ARCH callbacks are
             * a FILE RDT loader + a no-op render reset + the BG decode. (The BG
             * cut names are now room-aware (bg_pc.c re15_bg_room_prefix builds "room%04x" from
             * g_current_room_id; re15_bg_load_cut bg_pc.c:262-264), and per-room props
             * (pc_load_room_prop_set) + the cinematic/enemy model bank (per-dest_room RBJ reload +
             * re15_apply_room_cinematic below) reload too; the destination RDT,
             * collision, SCD + spawn already switch correctly.) */
            {
                extern int  re15_room_load(unsigned);
                extern void re15_room_reset_render_pc(void);
                extern int  re15_bg_load_cut(int);
                unsigned dest_room = g_room_change.room_id;  /* capture before apply consumes it */
                re15_room_apply_ctx_t rc;
                rc.rdt              = &rdt;
                rc.rdt_ok           = &rdt_ok;
                rc.active_cuts      = &active_cuts;
                rc.active_cut_count = &active_cut_count;
                rc.cam_active_cut   = &active_cut_idx;
                rc.cam_view         = &cam_view;
                rc.load_rdt         = re15_room_load;
                rc.reset_render     = re15_room_reset_render_pc;
                rc.load_bg_cut      = re15_bg_load_cut;
                rc.load_props       = 0;   /* PC reloads props INLINE below (the s_room_prop_*
                                            * arrays are local to main → no fn-ptr closure; the
                                            * callback stays NULL, behaviour is parity w/ PSX). */
                rc.load_cinematic   = 0;   /* PC keeps its boot-loaded Elliot/rbj resident (not
                                            * RAM-constrained) → no per-room cinematic reload. */
                if (re15_room_apply_pending(&rc)) {
                    /* RE-POINT THE RAW RDT BYTES AT THE NEW ROOM.
                     * rc only carries the PARSED rdt + the cut table; the consumers that
                     * parse sections straight out of the file (sprite.pri masks, camera FX
                     * CAMF, room ESP) use this raw buffer. Without this they kept reading the
                     * BOOT room's bytes with the NEW room's offsets — silently, because a
                     * bogus offset just yields "no data". Invisible to every RE15_START_ROOM
                     * harness (there boot room == the room under test), but in a real
                     * playthrough ROOM1240 -> ROOM1170 it made every sprite.pri cut parse 0
                     * masks: the helipad lamps and the container foreground occluded NOTHING.
                     * (Found by scripting boot -> NEW GAME instead of jumping into the room.) */
                    {
                        extern const unsigned char *re15_room_pc_bytes(int *size);
                        int nsz = 0;
                        const unsigned char *nbuf = re15_room_pc_bytes(&nsz);
                        if (nbuf && nsz > 0) {
                            rdt_buf  = (uint8_t *)nbuf;
                            rdt_size = nsz;
                            pc_load_room_esp(rdt_buf, rdt_size, dest_room);
                        }
                    }
                    /* BYTE-TRUE DOOR TRANSITION FADE-IN (RE1.5 transition FSM FUN_8001c958 state-1,
                     * RE'd wf_8e6a4d88): a PLAIN door CUTS to black on the warp frame (there is NO
                     * gradual fade-OUT — that path is exclusively the montage/cut flag aca3c&0x8000),
                     * then FADES IN over 6 frames. Kick the byte-true fade channel with the exact
                     * transition args (FUN_800217b0(0x200,-6144,7,0) → ch0, abr2 subtractive, rgb_mask7,
                     * step -0x1800; FUN_800216ec → level 0x7fff): level += -0x1800/frame, overlay
                     * brightness = level>>7 = 0xFF→0xCF→0x9F→0x6F→0x3F→0x0F→done, drawn by render_pc's
                     * re15_fade_tick, so the new room emerges from black. RE1.5 has NO door-model
                     * animation and NO door SFX in this path (the transition STOPS sound, plays none;
                     * door-open audio is a room-SCD Se_on — see aot_common.c). */
                    /* Kick the exact PSX fade tween (FUN_800217b0(0x200,-6144,7,0) + FUN_800216ec):
                     * ch0, abr2 subtractive, rgb_mask7, step -0x1800 → level 0x7FFF; render_pc's
                     * re15_fade_tick ramps darkness = level>>7 (0xFF→0xCF→…→0x0F→done) over 6 frames so
                     * the new room emerges from black. VERIFIED byte-true + RENDERS CORRECTLY on the PC
                     * screen (a gdigrab screen-recording of the real window shows the door cut to black
                     * then the destination room fade in over exactly 6 game frames). NB: the RE15_AUTOSHOT
                     * BMP tool (SDL_RenderReadPixels) does NOT capture these blend/overlay draws — verify
                     * fades/letterbox by recording the window with ffmpeg, not by autoshot brightness. */
                    { extern int re15_fade_log_on(void);
                      if (re15_fade_log_on())
                          fprintf(stderr, "[fade-log] F%u room=%04x TUER-EINTRITTS-FADE kick "
                                  "(main.c apply_pending)\n",
                                  g_engine.frame_count, g_current_room_id); }
                    /* Der Kick steckt jetzt in re15_room_transition_present() (room_common.c) —
                     * zusammen mit dem, was direkt daneben im selben State-3-Rumpf steht und im
                     * Port fehlte: das Spieler-Kommandowort (@0x8001cbdc) und der Logik-Freeze
                     * fuer die Dauer der Blende (State 4 @0x8001cc34-6c, Freigabe State 5
                     * @0x8001cc70-94). Damit laufen Kreuz-Raum-Tuer und Self-Reenter durch
                     * DIESELBE Praesentation, wie im Original (FUN_8001d600 verzweigt nur am
                     * Tuer-Record-Zeiger @0x8001d618, nicht am Zielraum). */
                    re15_room_transition_present();

                    /* The shared transition set the door's ENTRY cut in the
                     * frame-local active_cut_idx (via rc.cam_active_cut). The PC
                     * loop re-seeds active_cut_idx from the persistent
                     * s_last_cut_idx at the TOP of every frame, so without
                     * persisting it here the NEXT frame reverts to the previous
                     * room's last cut — leaving the player framed by a camera
                     * that faces away from the new spawn (root behind the
                     * near-plane -> whole mesh culled -> "Leon not loaded").
                     * The PSX loop carries cam_active_cut across frames in the
                     * loop scope, so it never had this bug; here we sync the
                     * static to match. (2026-06-08 cross-room player-missing fix.) */
                    s_last_cut_idx = active_cut_idx;

                    /* PRE-INTRO-Kette ÜBER Räume (Pre-Intro 1240 → Intro 1170):
                     * Nach dem Wechsel in den Intro-Raum hat scd_room_reenter dessen
                     * main00 ausgeführt → Evt_exec(0x180B)=sub11 (Narrator) liegt jetzt
                     * in einem Event-Slot. Den s_preintro-Handoff (Narrator-Ende →
                     * Helipad-Self-Reentry, siehe oben) hier NEU scharfmachen — er war
                     * sonst nur beim Boot aktiv, weshalb das via-Tür eingetretene 1170
                     * zwar den Narrator zeigte, aber nie das Elliot/Pilot-Helipad-
                     * Cinematic startete. `rdt` ist hier bereits der neue Raum (1170). */
                    /* Re-arm ONLY when entering the HELIPAD INTRO room (1170) — the sole room whose
                     * main00 spawns the narrator sub11 into an event slot. For EVERY other room the
                     * event slots hold ordinary sub00-spawned threads (e.g. ROOM1150's sub02/sub05
                     * dialogue helpers); mistaking those for the narrator armed the door-3 F900
                     * frame-cap fallback (above), which then re-ran main00+sub00 MID-CUTSCENE and
                     * killed it — the Irons-office freeze after leaving+re-entering through the door
                     * (measured: pmode stuck 2, cutscene thread gone at F900). Always RESET the latch;
                     * only SET it for 1170. */
                    s_preintro = 0; s_sub11_slot = -1; s_sub00_spawned = 0;
                    if (g_current_room_id == 0x1170) {
                        for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++) {
                            if (g_scd.threads[s].active) {
                                s_sub11_slot = s; s_preintro = 1; break;
                            }
                        }
                    }
                    g_engine.frame_count = 0;   /* Frame-Cap des Handoffs relativ zum Eintritt */
                    /* Refresh the per-cut region-quad cull for the NEW room/cut (same
                     * as the PSX fix): next frame the cut-change block sees no change
                     * (entry cut == last) so it won't refresh, leaving the player +
                     * props culled against the PREVIOUS room's stale region quad
                     * ("Leon invisible until you walk to a camera change"). */
                    cam_has_region = rdt_ok
                        ? re15_rdt_get_region_quad(&rdt, active_cut_idx,
                                                   cam_region_xs, cam_region_zs)
                        : 0;
                    /* Data-driven per-room props (parity with PSX re15_load_room_props):
                     * reload the destination room's Obj_model_set prop set so room1140
                     * shows ITS prop, not room1170's box. */
                    pc_load_room_prop_set(&rdt, s_room_prop_md1, s_room_prop_ok);   /* &rdt is synced to the dest room here */
                    /* SPIELERMODELL nach work_vars[0x10] — byte-true an der Stelle, an der
                     * es das Original tut: im RAUMLADER (FUN_800396fc @0x80039760-8c), nicht
                     * im Skript. Das Anlegen der R.P.D.-Ruestung in ROOM1190 laeuft ueber
                     * genau diesen Weg: sub15 setzt work_vars[0x10]=1 und feuert einen
                     * Null-Rechteck-Tuer-AOT zurueck in DENSELBEN Raum. Herleitung und
                     * Messwerte stehen bei pc_sync_player_model. */
                    pc_sync_player_model(&md1);

                    /* 2026-06-17 FIX — ROOM-AWARE CINEMATIC BANK. The boot RBJ overlay
                     * (ROOM1170 by default) was kept RESIDENT across rooms (old
                     * rc.load_cinematic=0), so room1150's Plc_motion(0,10/11) indexed
                     * ROOM1170's clips → Leon did a helipad point/reach instead of the
                     * Irons kneel ("streckt die Hand aus, bevor er sich beugt"). Each
                     * room's cinematics live in ITS RBJ → reload + re-overlay Leon
                     * (and Elliot) from the PRESERVED base PL00 on every room change. */
                    {
                        static uint8_t *s_room_rbj = NULL;   /* keep alive: parse_rbj refs it */
                        static unsigned s_rbj_room  = 0xFFFFFFFFu;
                        if (dest_room != s_rbj_room) {
                            char rpath[64];
                            snprintf(rpath, sizeof rpath, "RBJ/ROOM%04X.RBJ", dest_room);
                            int rsz = 0;
                            uint8_t *rbuf = pc_read_shared(rpath, &rsz);
                            /* RE1.5 has no standalone RBJ — slice the dest room's cinematic anim
                             * from its now-resident RDT (@0x5C, rdt.animation; apply_pending above
                             * updated the local `rdt` in-place to dest_room via rc.rdt = &rdt).
                             * Borrowed alias into the resident RDT buffer → keep s_room_rbj=NULL so
                             * it is never freed. */
                            int rbj_borrowed = 0;
                            if ((!rbuf || rsz <= 0) && rdt_ok &&
                                rdt.animation && rdt.animation_size > 0) {
                                rbuf = (uint8_t *)rdt.animation;
                                rsz  = rdt.animation_size;
                                rbj_borrowed = 1;
                            }
                            if (rbuf && rsz > 0) {
                                if (s_room_rbj) free(s_room_rbj);
                                s_room_rbj = rbj_borrowed ? NULL : rbuf;
                                s_rbj_room = dest_room;
                                /* SHARED overlay (enemy_common.c) — identical math to the PSX
                                 * re15_load_room_cinematic: Leon (from pl00 base) + Elliot (from
                                 * his base) + per-room enemy rebind (pc_enemy_load force-loads). */
                                re15_apply_room_cinematic(rbuf, (size_t)rsz, dest_room,
                                        pl00_ok ? &pl00_skel : NULL, &skel, &anim,
                                        &elliot_base_skel, elliot_skel_ok, &elliot_skel, &elliot_anim,
                                        &s_cine_scratch_skel, &s_cine_scratch_anim, pc_enemy_load);
                                /* Marker-Binder (FUN_8001b3f8): Raum-RBJ registrieren — die
                                 * Engine bindet Records lazy an die gespawnten Entity-Slots.
                                 * (rbuf bleibt resident: s_room_rbj bzw. RDT-Alias.) */
                                re15_rbj_bind_room(rbuf, (size_t)rsz);
                                fprintf(stderr, "[rbj] room %04X cinematic overlay: %d clips, %d kf\n",
                                        dest_room, anim.clip_count, skel.keyframe_count);
                            } else if (pl00_ok) {
                                /* 2026-07-31 FIX — Nutzer-Report: "Leon fuehrt in den Raum zu dem wir
                                 * springen noch Skript-Aktionen von dem Raum davor aus. Das macht er
                                 * im Original nicht."
                                 *
                                 * URSACHE: hat der Zielraum keine eigene Cinematic-Animation, BEHIELT
                                 * der Port die Bank des VORHERIGEN Raumes ("keeping current bank").
                                 * Leons Motion-Indizes zeigten damit weiter auf die Clips des alten
                                 * Raumes — schon Motion 0 ist dort eine Skript-Geste statt der Idle-
                                 * Pose, also spielt er sichtbar die Szene von vorher weiter.
                                 *
                                 * DAS ORIGINAL KANN DAS GAR NICHT: der Raumlader FUN_800396FC setzt
                                 * die Per-Raum-Arena zurueck (@0x80039738/40/48 schreiben 0x800AC77C /
                                 * 0x800AC778 / 0x800BBEB0 auf die Arena-Basis 0x800AC780) — die
                                 * Overlay-Daten des alten Raumes sind danach schlicht weg, und
                                 * FUN_8001B3F8 re-pointet die Animations-Zeiger nur fuer die im
                                 * RDT-Block +0x5C gesetzten Maskenbits. Kein Animationsblock im
                                 * Zielraum = Spieler zeigt auf die PL00-BASIS, nicht auf die alte Bank.
                                 * Dazu passt der Spieler-Reset des Transitions-Schwanzes:
                                 * @0x8001CBDC `sb zero,0x800ACA58` nullt das Kommando-Register
                                 * (Spieler+0x04). Ist dort 4 (= der SCD-Motion-Executor, gesetzt von
                                 * Plc_motion @0x80041B90), hoert der Skript-Antrieb damit auf; der
                                 * Routine-0-Handler @0x800318F8 setzt anschliessend Motion +0x94 = 0.
                                 *
                                 * Der Port bildet den Arena-Reset nach, indem er Leon aus der
                                 * BEWAHRTEN PL00-Basis wiederherstellt — genau das, was
                                 * re15_apply_room_cinematic sonst als Ausgangspunkt nimmt
                                 * (enemy_common.c: *leon_skel = *s aus pl00_base). */
                                skel = pl00_skel;
                                anim = pl00_anim;
                                /* ELLIOT GENAUSO. Der erste Wurf dieses Fixes hat nur Leon
                                 * zurueckgesetzt — das war asymmetrisch und damit derselbe Fehler
                                 * eine Etage tiefer: Elliot behielt die Clips des Vorraums. Im
                                 * Original faellt sein Resident-Flag bei JEDEM Raumwechsel, weil die
                                 * Arena weg ist (@0x80039738). */
                                if (elliot_skel_ok) {
                                    elliot_skel = elliot_base_skel;
                                    elliot_anim = elliot_base_anim;
                                }
                                s_rbj_room = dest_room;
                                fprintf(stderr, "[rbj] room %04X has no RBJ (%s) — Leon auf PL00-Basis "
                                        "zurueckgesetzt (Arena-Reset @0x80039738)\n", dest_room, rpath);
                            } else {
                                fprintf(stderr, "[rbj] room %04X has no RBJ (%s) und keine PL00-Basis\n",
                                        dest_room, rpath);
                            }
                        }
                    }
                }
            }
            /* PC-ONLY DEV TOOL (NOT gameplay; architecture-irrelevant): the motion-lock + PL00 clip
             * cycler. Applied AFTER the shared step so it overrides SCD/Plc_motion for visual clip
             * identification. GATED behind RE15_MOTION_DEBUG — it reads SELECT (0x0001) and the
             * TRIANGLE/CIRCLE bits (0x1000/0x2000), which are now the real Select and △/○ face
             * buttons, so left ungated it would hijack them during normal play (△/○ would cycle the
             * player animation). */
            {
                extern int re15_input_debug_fkey(int n);
                static int  s_motion_dev  = -1;
                if (s_motion_dev < 0) s_motion_dev = getenv("RE15_MOTION_DEBUG") ? 1 : 0;
                static int  s_motion_lock = 0;
                static int  s_locked_clip = 0;
                if (s_motion_dev && re15_input_debug_fkey(3)) {   /* F3 = lock toggle */
                    s_motion_lock = !s_motion_lock;
                    s_locked_clip = (int)g_actors[RE15_ACTOR_SLOT_PLAYER].motion;
                }
                int cyc = 0;
                if      (s_motion_dev && re15_input_debug_fkey(5)) cyc = +1;   /* F5 = clip + */
                else if (s_motion_dev && re15_input_debug_fkey(4)) cyc = -1;   /* F4 = clip - */
                if (cyc && anim.clip_count > 0) {
                    if (s_motion_lock) {
                        s_locked_clip += cyc;
                        while (s_locked_clip < 0) s_locked_clip += anim.clip_count;
                        while (s_locked_clip >= anim.clip_count) s_locked_clip -= anim.clip_count;
                        re15_actor_set_motion(&g_actors[RE15_ACTOR_SLOT_PLAYER],
                                              (int16_t)s_locked_clip);
                    } else {
                        re15_player_cycle_motion(cyc, anim.clip_count);
                    }
                }
                if (s_motion_lock && anim.clip_count > 0) {
                    re15_actor_set_motion(&g_actors[RE15_ACTOR_SLOT_PLAYER],
                                          (int16_t)s_locked_clip);
                }
            }
            /* AOT/INV/NPC HUD lines silenced for cleaner cinematic view. */
            if (g_inv.last_pickup_display_frames > 0) {
                g_inv.last_pickup_display_frames--;
            }
            /* CLIP-TEST mode (2026-05-25): RE15_CLIP_TEST=N forces Leon's
             * motion = N every frame, makes him visible from frame 0, and
             * places him at a known visible position so autoshot captures
             * isolated clip poses for empirical walk-cycle identification.
             * Bypasses SCD's Plc_motion writes. Use together with
             * RE15_AUTOSHOT=1 to dump pose snapshots, or with
             * RE15_AUTOSHOT_SERIES for per-frame inspection.
             *
             * RE15_CLIP_TEST=ALL sweeps clips 0..23 in one run: motion
             * cycles every 100 frames, autoshot fires at +5,+15,+25,+35,+45
             * within each 100-frame window — produces 120 BMPs labelled
             * clip_NN_kfXX.bmp. */
            static int  s_clip_test_inited = 0;
            static int  s_clip_test_id     = -1;
            static int  s_clip_test_all    = 0;
            if (!s_clip_test_inited) {
                const char *ct = getenv("RE15_CLIP_TEST");
                if (ct && *ct) {
                    if (ct[0] == 'A' || ct[0] == 'a') {
                        s_clip_test_all = 1;
                        s_clip_test_id  = 0;  /* start at clip 0 */
                        fprintf(stderr, "[clip-test] RE15_CLIP_TEST=ALL — "
                                "sweeping clips 0..23, 100f each, 5 shots/clip\n");
                    } else {
                        s_clip_test_id = atoi(ct);
                        fprintf(stderr, "[clip-test] RE15_CLIP_TEST=%d active — "
                                "forcing motion=%d, Leon visible from F0\n",
                                s_clip_test_id, s_clip_test_id);
                    }
                }
                s_clip_test_inited = 1;
            }
            if (s_clip_test_id >= 0 && anim.clip_count > 0) {
                int cid;
                if (s_clip_test_all) {
                    /* Cycle clip every 100 frames. set_motion resets
                     * anim_frame to 0 on motion-change, so each clip's
                     * 100-frame window starts fresh. */
                    cid = ((int)g_engine.frame_count / 100) % 24;
                } else {
                    cid = s_clip_test_id;
                }
                if (cid >= anim.clip_count) cid = cid % anim.clip_count;
                /* DEBUG: play a PREV clip (RE15_PREV_CLIP, default 0=idle) for the first 40
                 * frames so the player accumulates a real prev-pose, THEN switch to the test
                 * clip — reproduces the cutscene's <prev>→kneel crossfade (not a 0-prev snap).
                 * Set RE15_PREV_CLIP=4 (walk) or 9 (turn) to test the auto-walk approach. */
                if (!s_clip_test_all && g_engine.frame_count < 40) {
                    const char *pc = getenv("RE15_PREV_CLIP");
                    cid = (pc && *pc) ? atoi(pc) : 0;
                }
                /* Re-assert each frame so SCD/Plc_motion cannot override. */
                re15_actor_set_motion(&g_actors[RE15_ACTOR_SLOT_PLAYER],
                                      (int16_t)cid);
                /* Park Leon at a well-framed spot for the helipad cut 0
                 * wide shot. AM-round: Y -7965→-7200 (helipad floor). */
                g_actors[RE15_ACTOR_SLOT_PLAYER].x = 1272;
                g_actors[RE15_ACTOR_SLOT_PLAYER].y = -7200;
                g_actors[RE15_ACTOR_SLOT_PLAYER].z = 10898;
                /* Face camera-front-right so limb motion is broadside. */
                g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = 1024; /* ~90° */
                g_actors[RE15_ACTOR_SLOT_PLAYER].walk_active = 0;
                /* Force camera back to cut 0 every frame — sub11 fires
                 * Cut_chg(7) at F0 which switches to a black/missing BG
                 * and an off-Leon framing. Pinning to cut 0 keeps the
                 * wide helipad shot Leon is positioned for. Rebuild the
                 * current frame's cam_view so the capture this tick is
                 * already from cut 0 (not next tick). */
                if (active_cut_idx != 0) {
                    g_scd.cam_id             = 0;
                    g_scd.cam_change_pending = 0;  /* consumed inline */
                    s_last_cut_idx           = 0;
                    active_cut_idx           = 0;
                    re15_bg_load_cut(0);
                    re15_camera_build_view(&active_cuts[0], &cam_view);
                }
            }

            /* FORCE_MOTION mode (2026-05-25): RE15_FORCE_MOTION=N is the
             * minimal-impact variant of RE15_CLIP_TEST. It ONLY overrides
             * the player's motion every frame — no teleport, no rot_y
             * change, no visibility override. The cinematic still plays
             * normally; we just pin Leon's clip so we can observe the
             * forced pose during the windows where SCD makes him visible. */
            static int s_force_motion_inited = 0;
            static int s_force_motion_id     = -1;
            if (!s_force_motion_inited) {
                const char *fm = getenv("RE15_FORCE_MOTION");
                if (fm && *fm) {
                    s_force_motion_id = atoi(fm);
                    fprintf(stderr, "[force-motion] RE15_FORCE_MOTION=%d active — "
                            "pinning player motion every frame\n",
                            s_force_motion_id);
                }
                s_force_motion_inited = 1;
            }
            if (s_force_motion_id >= 0 && anim.clip_count > 0) {
                int fid = s_force_motion_id;
                if (fid >= anim.clip_count) fid = fid % anim.clip_count;
                re15_actor_set_motion(&g_actors[RE15_ACTOR_SLOT_PLAYER],
                                      (int16_t)fid);
            }

            /* Phase 4.5.9-D: player state from g_actors[0]. */
            const re15_actor_t *player_ref = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            int32_t model_pos_x = player_ref->x;
            int32_t model_pos_y = player_ref->y;
            int32_t model_pos_z = player_ref->z;
            float   screen_dist = (float)cam_view.fov_screen_dist;

            /* AO9-round (2026-05-26): position-based visibility gate.
             * Previously a hardcoded `frame_count >= FRAME_AT_60(3390)`
             * gate hid Leon until ~F1695 (30fps), causing him to "pop
             * into" the scene 1+ frames AFTER sub02's Cut_chg(0x00) +
             * Member_set teleport (lines 67-73). In the original PSX,
             * the cut switch and position teleport happen on the SAME
             * tick, so Leon is already at his cinematic position when
             * the camera reveals him (ablauf intro00218628 shows him
             * present from frame 1 of the new cut).
             *
             * The frame-window heuristic was fragile because sub02's
             * absolute timing depends on FPS scaling. Position-based
             * gate is robust: Leon's spawn = (1272, ...), sub02 line
             * 10-12 teleports him FAR OFF (-31000, 31000) for dialog
             * cuts, line 71-73 brings him back (2300, ?, 14365) for
             * his appearance. |x| < 25000 captures all on-stage
             * positions; the -31000 hidden state is rejected.
             *
             * I2-round (2026-05-24): per ablauf 001-013, Leon is HIDDEN
             * during sub11 narrator (cut 7) and during sub02's
             * Elliot+Pilot+Heli dialog phase. He only becomes visible
             * when sub02 fires the second Cut_chg(0) (Leon-arrival).
             *
             * Without this gate, Leon stands as a R.P.D. statue in
             * cuts 1/2 where the original game frames him out of view.
             * The real engine relied on positional framing (Leon spawn
             * outside cam frustum) — our coord system doesn't reproduce
             * that exactly, so we use a frame-window heuristic.
             *
             * CLIP_TEST override: force-visible from frame 0 so we can
             * see the pose immediately without waiting 56s of intro. */
            int player_visible;
            if (s_clip_test_id >= 0) {
                player_visible = 1;
            } else {
                /* Per-cut region-quad cull (byte-true FUN_80039ca0→FUN_80014368 vs the
                 * active cut's quad — the SAME test the props use), replacing the eyeballed
                 * `x > -30000` off-stage gate (#18 2026-06-09). Leon is drawn unless his
                 * world XZ is outside the active cut's region quad: the SCD off-stage hide
                 * (Member_set(0,-31000)) lands outside → hidden; the whole on-stage outdoor
                 * area (incl. the staircase x≈-27410) is inside its cut's quad → shown. */
                player_visible = !(cam_has_region &&
                    !re15_aot_point_in_quad(player_ref->x, player_ref->z,
                                            cam_region_xs, cam_region_zs));
            }
            /* MESS-HAKEN RE15_VIS_TRACE=1 — reine Messausgabe (kein Verhalten). Protokolliert
             * pro Bild GENAU die Groessen, aus denen der Spieler-Cull entsteht: aktiver Cut,
             * angeforderter Cut, der Kamera-Spiegel work_vars[0x0A], Position/Yaw, ob der Cut
             * ueberhaupt ein Anker-Quad hat (DAT_800ac790), das Punkt-im-Quad-Ergebnis und die
             * Walker-Felder des laufenden Plc_dest (Modus/FSM/Ziel). */
            { static int s_vt = -1;
              if (s_vt < 0) { const char *e = getenv("RE15_VIS_TRACE"); s_vt = (e && *e) ? atoi(e) : 0; }
              if (s_vt) {
                  const re15_actor_t *vp = player_ref;
                  fprintf(stderr,
                      "[vis] F%u room=%04x cut=%d camid=%d wv10=%d pos=(%ld,%ld,%ld) yaw=%d "
                      "hasrgn=%d inrgn=%d vis=%d pmode=%d wact=%d wmode=%d wfsm=%d dest=(%d,%d) "
                      "mo=%d st=%d s1=%d\n",
                      g_engine.frame_count, g_current_room_id, active_cut_idx,
                      (int)g_scd.cam_id, (int)g_scd.work_vars[0x0A],
                      (long)vp->x, (long)vp->y, (long)vp->z, (int)vp->rot_y,
                      cam_has_region,
                      cam_has_region ? re15_aot_point_in_quad(vp->x, vp->z,
                                                              cam_region_xs, cam_region_zs) : -1,
                      player_visible, (int)g_scd.player_mode,
                      (int)vp->walk_active, (int)vp->walk_mode, (int)vp->walk_fsm,
                      (int)vp->walk_dest_x, (int)vp->walk_dest_z,
                      (int)vp->motion, (int)vp->state, (int)vp->sub_state_1);
              } }
            /* No far-clip here. PSX GTE has no upper-bound OTZ check —
             * FUN_80016b54 @0x80016d24 does `bltz v0, skip` (OTZ<0 only).
             * The |x|<25000 proxy above already hides any teleport-hidden
             * actor (sub02 sets x=-31000 before cut 6). The per-vertex
             * _vz<64 near-clip in the projection macro guards div-by-zero.
             * BB-round's 32000 far-clip was based on a false premise and
             * has been removed (BM-round already removed it from props for
             * the same reason). */

            /* SHARED anim selection (unify 2026-06-06): the bank/clip view-model is
             * game logic, so it lives in anim_select_common.c and is IDENTICAL to
             * the PSX build — locomotion 105/100 -> PL00W01 walk/run, idle-fidget
             * 200/210/211/212 -> PL00W01, HP-gated injured idle 213/214 -> PL00
             * base, else the actor's own cinematic bank (motion = direct clip
             * index). Fill the bank table from this build's locals. */
            /* EQUIP WATCHER — the byte-true W-bank switch (loader @0x80036b80: CD file 76+item):
             * items 0-2 -> melee bank (W01 trio), items 3+ -> gun bank (W03 pair). The WHOLE
             * carry set switches (locomotion + aim clips share the 14-clip layout), and the aim
             * FSM gets the new bank's per-clip frame counts (recoil 23f vs draw 15f cadence). */
            /* DERIVED state, NO process-lifetime latch (fix Nutzer-Report "Messer-Anims trotz
             * Handfeuerwaffe nach Load", 2026-08-17). The old `static int s_wgun` survived the
             * in-process `goto re_title` after death: the boot block re-inits the LOCALS
             * wact_skel/wact_anim/wact_ok to the W01 (knife) defaults (main.c ~L2595) and re-seeds
             * the aim clip lengths from W01 (~L2588), but s_wgun kept the PREVIOUS session's value,
             * so after Tod -> Titel -> LOAD with a gun equipped `want_gun(1) == s_wgun(1)` and the
             * watcher never fired -> the recoil clip rendered from the MELEE bank (measured fc=25
             * = PL00W01 clip7 instead of fc=23 = PL00W03 clip7, every session after the first).
             * The original has no such latch: the weapon bank is re-derived UNCONDITIONALLY on every
             * player load — FUN_800314b0 `jal FUN_80036b68` @0x800316f0 (FUN_800314b0 itself is
             * called by the room loader FUN_800396fc @0x80039788 and by @0x8001d5a4), and
             * FUN_80036b68 reads charid/equipped fresh (`lbu DAT_800aca5c` @0x80036b84,
             * `lbu DAT_800aca5d` @0x80036b8c), looks up the CD file base (`lhu DAT_800741e8[char*2]`
             * @0x80036bb4), CD-loads it (`jal 0x80013b60` @0x80036bc0) and OVERWRITES both bank
             * pointers (`sw v0,DAT_800acbc8` @0x80036be4 / `sw v0,DAT_800acbc4` @0x80036c04) —
             * no "last state" comparison anywhere. Comparing the BOUND pointer reproduces that:
             * the locals reset on every (re)boot, so the first frame after a boot always rebinds. */
            {
                extern int  re15_player_equipped_weapon(void);
                extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);
                int is_gun_bound = (wact_anim == &w03_anim);   /* == DAT_800acbc8 readback */
                int want_gun = (re15_player_equipped_weapon() >= 3) && w03_ok;
                if (want_gun != is_gun_bound) {
                    wact_skel = want_gun ? &w03_skel : &w01_skel;
                    wact_anim = want_gun ? &w03_anim : &w01_anim;
                    wact_ok   = want_gun ? w03_ok    : w01_ok;
                    uint16_t fcs[14];
                    int n = (wact_anim->clip_count < 14) ? wact_anim->clip_count : 14;
                    for (int i = 0; i < n; i++) fcs[i] = (uint16_t)wact_anim->clips[i].frame_count;
                    re15_player_set_aim_clip_lens(fcs, n);
                    /* Fires only on an actual bank switch (rare) — and MUST fire again after every
                     * death->title->boot, which is exactly what the removed latch prevented. The
                     * recoil clip 7 frame count is the bank fingerprint: W01 (knife) = 25, W03
                     * (gun) = 23 (EDD clip tables, measured). */
                    fprintf(stderr, "[equip] W-bank -> %s (recoil clip7 fc=%d)\n",
                            want_gun ? "W03" : "W01",
                            (n > 7) ? (int)fcs[7] : -1);
                }
            }
            re15_anim_banks_t banks = {
                .def_mesh = &md1, .def_skel = &skel, .def_anim = &anim,
                .w01_skel = wact_skel, .w01_anim = wact_anim, .w01_ok = wact_ok,
                .pl00_skel = &pl00_skel, .pl00_anim = &pl00_anim, .pl00_ok = pl00_ok,
                .elliot_mesh = &elliot_md1, .elliot_skel = &elliot_skel,
                .elliot_anim = &elliot_anim, .elliot_ok = (elliot_ok && elliot_skel_ok),
            };
            re15_anim_view_t av;
            re15_actor_anim_select(player_ref, 1, &banks, &av);
            const re15_emd_skeleton_t  *p_skel = av.skel;
            const re15_emd_animation_t *p_anim = av.anim;
            int p_clip_override = av.clip_override;
            /* LEON GRAB-VICTIM render override (state 5 struggle / state 6 collapse): while a zombie
             * has Leon grabbed (or the grab killed him), pose Leon from the grabbing zombie's EMD
             * BANK 2 (the grab-victim set) at player->motion (the struggle/collapse clip the victim
             * FSM set). Byte-true: @0x8010a28c/@0x8010a6f8 animate the PLAYER from DAT_800acbcc/acbd0
             * = *(zombie+0x178)/+0x17c (bank 2), NOT his own PL00 set. This is what stops Leon from
             * freezing during the grab/death (the "no Leon reactions / death finish missing"). */
            if (re15_player_victim_state() != 0 && !re15_player_victim_own_bank() && pl00_ok) {
                /* re15_player_victim_own_bank(): GORILLA-Wurf P3-P6 (Hook 0x8011c118) —
                 * Leon spielt seine EIGENEN Aufsteh-Clips 0x10/0xb (anim_set auf
                 * [acad8]/[acbc0] @0x8011c34c-60), also KEIN Opfer-Bank-Override. */
                re15_enemy_bank_t *vb = re15_enemy_find(re15_player_victim_type());
                if (vb && vb->victim_ok && vb->anim_victim.clip_count > 0) {
                    /* Pose Leon with HIS OWN structure (PL00 bone hierarchy + bind = his proportions)
                     * but bank 2's keyframe POSES + clip table. The original renders the player with
                     * the player entity's skeleton; the victim EDD/EMR only supply the clips+poses. */
                    static re15_emd_skeleton_t s_victim_skel;
                    s_victim_skel = pl00_skel;                              /* Leon's bones + bind pose */
                    s_victim_skel.keyframe_data       = vb->skel_victim.keyframe_data;   /* bank2 poses */
                    s_victim_skel.keyframe_data_size  = vb->skel_victim.keyframe_data_size;
                    s_victim_skel.keyframe_count      = vb->skel_victim.keyframe_count;
                    s_victim_skel.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
                    p_skel = &s_victim_skel;
                    p_anim = &vb->anim_victim;
                    /* player->motion IS the bank-2 clip the victim FSM set (struggle 0-5 / collapse
                     * 6-7). re15_compute_actor_kf takes clip_override>=0 as the LITERAL clip index and
                     * loops slot = anim_frame % frame_count — so pass motion, NOT a bare 1 (that pinned
                     * Leon to clip 1: the collapse never rendered + behind-grab showed the wrong pose;
                     * caught by the byte-true render-path verify, invisible to the motion-value test). */
                    p_clip_override = (int)player_ref->motion;
                }
            }
            /* AIM render override: while aiming, Leon poses from the EQUIPPED W bank — his own
             * bones + the weapon pool, same pattern as the grab-victim override. Melee (items
             * 0-2, bank W01): draw clip 0xD (FUN_80035538); gun (items 3+, bank W03): the gun-FSM
             * clip (6 raise / 8|10|12 hold / 7|9|11 recoil). */
            {
                extern int re15_player_aim_active(void);
                if (re15_player_aim_active() && wact_ok &&
                    re15_player_victim_state() == 0) {
                    extern int re15_player_aim_clip(void);
                    p_skel = wact_skel;              /* composite: PL00 bones + active W pool */
                    p_anim = wact_anim;
                    p_clip_override = re15_player_aim_clip();
                }
            }
            /* PLC_DEST-MODE-6 EVENT-REACH render override: der Spieler-Sub 0x800517f0 spielt
             * PLW-Paar-B-Clips 1 (einmal) -> 2 (Loop) auf dem +0x170-Kanal (@0x80051884/88;
             * FUN_80036b68 laedt dort die aktive W-Bank) — dieselbe Bank-Composite wie der
             * Aim-Override. FSM/Advance: game_step_common re15_player_event_reach_tick. */
            {
                extern int re15_player_event_reach_clip(void);
                extern int re15_player_aim_active(void);
                int erc = re15_player_event_reach_clip();
                if (erc >= 0 && wact_ok && re15_player_victim_state() == 0 &&
                    !re15_player_aim_active()) {
                    p_skel = wact_skel;
                    p_anim = wact_anim;
                    p_clip_override = erc;
                }
            }
            int kf_idx = 0;
            if (player_visible && skel_ok && p_anim->clip_count > 0) {
                /* The platform owns the fps policy: at 30fps target anim_frame is
                 * already 30Hz; at 60fps halve to PSX-canonical 30Hz. */
                uint32_t cur = (target_fps == 30)
                    ? (uint32_t)player_ref->anim_frame
                    : ((uint32_t)player_ref->anim_frame >> 1);
                kf_idx = re15_compute_actor_kf(p_anim, p_skel, player_ref,
                                               p_clip_override, cur);
            }
            re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
            int pose_ok = 0;
            if (player_visible && skel_ok) {
                g_anim_pose_actor = player_ref;   /* FRAC crossfade for the player body */
                pose_ok = (re15_skel_compute_pose(p_skel, kf_idx, poses) == 0);
                /* RE15_POSE_DUMP: LEON render-level pose (grab-start / release investigations) */
                {
                    static FILE *s_lp = NULL; static int s_lp_tried = 0;
                    if (!s_lp_tried) { s_lp_tried = 1;
                        const char *pd = getenv("RE15_POSE_DUMP");
                        if (pd && *pd) { char pb[512]; snprintf(pb, sizeof pb, "%s.leon", pd); s_lp = fopen(pb, "w"); }
                    }
                    if (s_lp && pose_ok) {
                        fprintf(s_lp, "[leon] F%u vs=%d mo=%d af=%d frac=%d kf=%d ovr=%d rot=%d "
                                "b9(%d,%d,%d) b13(%d,%d,%d)\n",
                                (unsigned)g_engine.frame_count, re15_player_victim_state(),
                                player_ref->motion, player_ref->anim_frame, player_ref->anim_frac,
                                kf_idx, p_clip_override, (int)player_ref->rot_y,
                                (int)poses[9].trans[0], (int)poses[9].trans[1], (int)poses[9].trans[2],
                                (int)poses[13].trans[0], (int)poses[13].trans[1], (int)poses[13].trans[2]);
                        fflush(s_lp);
                    }
                }
            }
            /* Per-bone composed matrix `view × bone_world` (Q12 rotation
             * + world-unit translation, float copies for in-macro math).
             * Set inside the mesh loop below by re15_camera_compose_
             * view_bone. */
            int32_t bone_m[9] = {4096,0,0,  0,4096,0,  0,0,4096};   /* Q12 view×bone (was float; byte-true RTPS) */
            int32_t bone_t[3] = {0, 0, 0};
            /* bone-11 composed matrix capture (the weapon-in-hand attach bone) */
            int32_t wpn_bone_m[9], wpn_bone_t[3];
            int32_t wpn_yawed[9];
            int     wpn_bone_valid = 0;

/* Phase 4.5.8.2: bone_m / bone_t now hold view × bone_world. Vertex maps
 * directly into camera space — no extra MESH_POS_Z hack. Perspective
 * divide uses screen_dist from the active cut. out_wz emits the
 * camera-space z for back-to-front sort (Phase 4.5.7.7). */
/* H28 fix (2026-05-24): set out_wz to a negative sentinel for behind-near
 * plane verts so callers can skip the tri (PSX gte_nclip equivalent). Was
 * clamping _vz to 1.0 which projected behind-camera verts to ±million
 * pixels, producing exploded triangles + wraparound + CLUT smear. Near
 * plane bumped from 1 to 64 to also kill the GTE-overflow case noted in
 * bugfix_psx_canonical_pipeline_2026_05_18. */
/* Byte-true GTE RTPS (integer, replaces the old float divide+transform — the last pixel-shift
 * source): view = (bone_m·v)>>12 + bone_t; IR1/IR2 sat s16, SZ3 sat u16; sx = OFX + (IR·n)>>16
 * with n = the GTE UNR divide (re15_gte_divide). H = fov_screen_dist. */
#define PROJECT_VERT(vp, out_sx, out_sy, out_wz) do { \
                int32_t _x = (vp)->x, _y = (vp)->y, _z = (vp)->z; \
                int32_t _vx = (int32_t)(((int64_t)_x*bone_m[0] + (int64_t)_y*bone_m[1] + (int64_t)_z*bone_m[2]) >> 12) + bone_t[0]; \
                int32_t _vy = (int32_t)(((int64_t)_x*bone_m[3] + (int64_t)_y*bone_m[4] + (int64_t)_z*bone_m[5]) >> 12) + bone_t[1]; \
                int32_t _vz = (int32_t)(((int64_t)_x*bone_m[6] + (int64_t)_y*bone_m[7] + (int64_t)_z*bone_m[8]) >> 12) + bone_t[2]; \
                if (_vz < 64) { (out_wz) = -1.0f; (out_sx) = 0; (out_sy) = 0; } \
                else { \
                    int32_t _ir1 = _vx > 0x7FFF ? 0x7FFF : (_vx < -0x8000 ? -0x8000 : _vx); \
                    int32_t _ir2 = _vy > 0x7FFF ? 0x7FFF : (_vy < -0x8000 ? -0x8000 : _vy); \
                    uint32_t _sz3 = _vz > 0xFFFF ? 0xFFFFu : (uint32_t)_vz; \
                    uint32_t _n = re15_gte_divide((uint32_t)cam_view.fov_screen_dist, _sz3); \
                    (out_sx) = cx + (int)(((int64_t)_ir1 * (int64_t)_n) >> 16); \
                    (out_sy) = cy + (int)(((int64_t)_ir2 * (int64_t)_n) >> 16); \
                    (out_wz) = (float)_vz; \
                } \
            } while (0)

#define IS_BACKFACE(ax, ay, bx, by, cx_, cy_) \
                (((long long)((bx) - (ax)) * ((cy_) - (ay)) \
                - (long long)((by) - (ay)) * ((cx_) - (ax))) <= 0)
            const int backface_cull = 1;
            (void)backface_cull;  /* wireframe doesn't cull yet — symmetric to PSX wireframe path */

            /* Phase 4.5.9-D: model Y-rotation derived from player rot_y. */
            int32_t face_s = re15_sin_q12((int)player_ref->rot_y);
            int32_t face_c = re15_cos_q12((int)player_ref->rot_y);
            int32_t yaw_rot_q12[9] = {
                 face_c, 0,  face_s,
                 0,      0x1000, 0,
                -face_s, 0,  face_c
            };

            /* RE1.5 CHARACTER SHADOW (FUN_8001b064 + FUN_8001af5c, 2026-05-29).
             * A subtractive textured floor quad under the actor: 4 corners at
             * (±500, 0, ±600) world units (a2=0x1f4 half-width, a3=0x258
             * half-depth), centered on the actor (offset a0=a1=0), at the floor
             * Y, yaw-rotated by the actor facing (RotMatrixY). The PSX builds
             * the corners in the actor's rotated+translated frame and projects
             * via RotAverage4 — we mirror that with re15_camera_compose_view_bone
             * (view × RotY(facing) @ actor world pos) + the standard projection.
             * Floor Y = the actor's own Y (= floorIdx·−0x708 on the helipad,
             * where every actor stands on the −7200 floor). Player-only, matching
             * the PSX caller (player update FUN_80031c44). */
            if (player_visible) {
                /* Half-extents = descriptor +0xc/+0xe (= player entity+0xbc/+0xbe). RAM-CONFIRMED
                 * 500/600 across every alive-player savestate (they grow ONLY at death = the blood
                 * pool below); a workflow finder's "square ~400" claim was wrong — the savestate is
                 * the arbiter. NOT a square. */
                int32_t SH_HALF_X = 500;                /* entity+0xbc, RAM-confirmed */
                int32_t SH_HALF_Z = 600;                /* entity+0xbe, RAM-confirmed */
                /* DEATH BLOOD POOL (LAB_8003694c @0x800369bc-d8): the quad half-extents grow
                 * +0xc per death tick (live terminal 0x7ac/0x810 after ~122 ticks). */
                if (re15_player_is_dead() && g_death_pool > 0) {
                    SH_HALF_X += 12 * g_death_pool;
                    SH_HALF_Z += 12 * g_death_pool;
                }
                int32_t sh_corner[4][3] = {
                    { -SH_HALF_X, 0,  SH_HALF_Z },   /* ecke1 (-X,+Z) → uv(0,0) */
                    { -SH_HALF_X, 0, -SH_HALF_Z },   /* ecke2 (-X,-Z) → uv(1,0) */
                    {  SH_HALF_X, 0,  SH_HALF_Z },   /* ecke3 (+X,+Z) → uv(0,1) */
                    {  SH_HALF_X, 0, -SH_HALF_Z },   /* ecke4 (+X,-Z) → uv(1,1) */
                };
                /* Center on the ACTOR position. FUN_8001b064 centers on the
                 * camera focus cam+0x34/+0x3c, but FUN_800369f8 shows that focus
                 * is the LIVE soft-tracked look-at that lerps toward the character
                 * — so the actor position is its faithful analog. (The STATIC cut
                 * target is the framing point ~4000u off Leon → put the quad above
                 * his feet = shadow "gone"; verified via the projection log.)
                 * Rotation = CAMERA yaw (sh_cam_yaw_rot, = RotMatrixY(cam+0x6a)) —
                 * a camera-aligned quad covers both feet; actor-yaw swung it off
                 * one foot. Floor Y = model_pos_y (the −7200 helipad floor). */
                int32_t sh_world[3] = { model_pos_x, model_pos_y, model_pos_z };
                int32_t sh_rot[9], sh_trans[3];
                /* byte-true: RotMatrixY(PLAYER rot_y) via the trig LUT — FUN_8001b064 @0x8001b0e4
                 * builds the shadow orientation from the ACTOR's entity+0x6a, not the camera yaw
                 * (RE'd wf_13911cba; the "camera yaw" note was a misread of DAT_800ac784=actor). */
                int32_t sh_actor_yaw[9];
                re15_camera_yaw_matrix_angle(g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y, sh_actor_yaw);
                re15_camera_compose_view_bone(&cam_view, sh_actor_yaw, sh_world,
                                              sh_rot, sh_trans);
                int32_t sbm[9], sbt[3];   /* Q12 (byte-true integer RTPS) */
                for (int i = 0; i < 9; i++) sbm[i] = sh_rot[i];
                for (int i = 0; i < 3; i++) sbt[i] = sh_trans[i];
                int sx[4], sy[4], sok = 1;
                for (int v = 0; v < 4; v++) {
                    int32_t _x = sh_corner[v][0], _y = sh_corner[v][1], _z = sh_corner[v][2];
                    int32_t _vx = (int32_t)(((int64_t)_x*sbm[0] + (int64_t)_y*sbm[1] + (int64_t)_z*sbm[2]) >> 12) + sbt[0];
                    int32_t _vy = (int32_t)(((int64_t)_x*sbm[3] + (int64_t)_y*sbm[4] + (int64_t)_z*sbm[5]) >> 12) + sbt[1];
                    int32_t _vz = (int32_t)(((int64_t)_x*sbm[6] + (int64_t)_y*sbm[7] + (int64_t)_z*sbm[8]) >> 12) + sbt[2];
                    if (_vz < 64) { sok = 0; break; }   /* H28 near-clip */
                    int32_t _ir1 = _vx > 0x7FFF ? 0x7FFF : (_vx < -0x8000 ? -0x8000 : _vx);
                    int32_t _ir2 = _vy > 0x7FFF ? 0x7FFF : (_vy < -0x8000 ? -0x8000 : _vy);
                    uint32_t _sz3 = _vz > 0xFFFF ? 0xFFFFu : (uint32_t)_vz;
                    uint32_t _n = re15_gte_divide((uint32_t)cam_view.fov_screen_dist, _sz3);
                    sx[v] = cx + (int)(((int64_t)_ir1 * (int64_t)_n) >> 16);
                    sy[v] = cy + (int)(((int64_t)_ir2 * (int64_t)_n) >> 16);
                }
                if (sok) {
                    if (getenv("RE15_SHADOW_DBG")) {
                        int minx=sx[0],maxx=sx[0],miny=sy[0],maxy=sy[0];
                        for (int v=1; v<4; v++){ if(sx[v]<minx)minx=sx[v]; if(sx[v]>maxx)maxx=sx[v];
                            if(sy[v]<miny)miny=sy[v]; if(sy[v]>maxy)maxy=sy[v]; }
                        fprintf(stderr, "[shadow] F%u cut=%d pl=(%d,%d) rot=%d halfXZ=(%d,%d) "
                                "corners=(%d,%d)(%d,%d)(%d,%d)(%d,%d) bbox=%dx%d\n",
                                (unsigned)g_engine.frame_count, active_cut_idx,
                                g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z,
                                g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y, SH_HALF_X, SH_HALF_Z,
                                sx[0],sy[0],sx[1],sy[1],sx[2],sy[2],sx[3],sy[3], maxx-minx, maxy-miny);
                    }
                    if (re15_player_is_dead() && g_death_pool > 0)
                        /* BLOOD POOL (byte-true LAB_8003694c): the shadow quad turns dark-red
                         * (subtractive color 0x38/0xff/0xff @0x8003699c-b8) — the SIZE growth
                         * (+0xc/frame) is applied to sh_corner above via g_death_pool. */
                        re15_render_shadow_quad_c(sx[0], sy[0], sx[1], sy[1],
                                                  sx[2], sy[2], sx[3], sy[3],
                                                  0x38, 0xff, 0xff);
                    else
                        re15_render_shadow_quad(sx[0], sy[0], sx[1], sy[1],
                                                sx[2], sy[2], sx[3], sy[3]);
                }
            }

            /* CANONICAL per-bone NCCT lighting (2026-06-02, mirrors the PSX-native
             * mesh_psx.c + FUN_8001e9ec). Build the WORLD-space context ONCE here
             * (actor_rot = NULL → ctx->L stays world-space); the per-bone fold
             * (L_bone = bone_rot^T × L_world) happens inside the bone loop so a
             * posed limb is lit by L_world · N_world, not the body yaw. The old
             * body-only path (actor_rot = yaw_rot_q12, single ctx) was an
             * approximation that mis-shaded articulated bones. */
            re15_actor_lightctx_t lctx_player, lctx_player_world;
            int player_lit = (g_re15_room_lights_ok &&
                              g_re15_active_cut >= 0 &&
                              g_re15_active_cut < g_re15_room_lights.cut_count);
            if (player_lit) {
                /* Canonical (2026-05-29): eval_pos = the actor's OWN world
                 * position, matching PSX FUN_80053fc0(param_1 = actor pos) —
                 * its caller FUN_80039ca0 passes the actor record's position
                 * field (puVar3+0x14), confirmed in RE_15_Quellcode_V2/.
                 * Lighting only runs inside the player_visible gate (|x|<25000),
                 * so a teleport-hidden Leon is never lit — as PSX culls hidden
                 * actors before shading them. */
                int32_t actor_pos_w[3] = {
                    (int32_t)player_ref->x,
                    (int32_t)player_ref->y,
                    (int32_t)player_ref->z
                };
                re15_light_setup_actor(
                    &g_re15_room_lights.cuts[g_re15_active_cut],
                    actor_pos_w, NULL, &lctx_player_world);
            } else {
                memset(&lctx_player, 0, sizeof(lctx_player));
            }

            /* Bind Leon's TIM (slot 0) for player render. */
            re15_render_pc_bind_tim_slot(0);

            /* 1:1 mesh_idx == bone_idx (PL00 convention; meshes 15..16
             * are weapon slots). See mesh_psx.c for the rationale. */
            int n_bones = pose_ok ? skel.bone_count : 0;
            if (n_bones > md1.mesh_count) n_bones = md1.mesh_count;
            for (int bi = 0; bi < n_bones; bi++) {
                /* Phase 4.5.10-L: revert to 1:1 mesh-bone mapping.
                 * Phase 4.5.10-H tried bone_mesh_index[] but that field's
                 * semantics weren't what we thought (it's the EMR child
                 * traversal table, NOT a bone→mesh permutation). Memory
                 * note phase4_5_7_5_textured_skeletal confirms 1:1 is
                 * correct per revengi/BioModels/lib_bio (3 independent
                 * RE2 reimplementations). */
                int mi = bi;

                /* Phase 4.5.8.2 + 4.4.5.1: pre-rotate pose by R_y(yaw),
                 * then compose view × (R_y × bone) per bone. */
                const re15_skel_pose_t *p = &poses[bi];
                int32_t yawed_rot[9];
                for (int r = 0; r < 3; r++) {
                    for (int c = 0; c < 3; c++) {
                        int64_t s = 0;
                        for (int k = 0; k < 3; k++) {
                            s += (int64_t)yaw_rot_q12[r*3 + k]
                                 * (int64_t)p->rot[k*3 + c];
                        }
                        yawed_rot[r*3 + c] = (int32_t)(s >> 12);
                    }
                }
                int32_t yawed_trans[3];
                for (int r = 0; r < 3; r++) {
                    int64_t s = 0;
                    for (int k = 0; k < 3; k++) {
                        s += (int64_t)yaw_rot_q12[r*3 + k] * (int64_t)p->trans[k];
                    }
                    yawed_trans[r] = (int32_t)(s >> 12);
                }
                int32_t bone_world_trans[3] = {
                    yawed_trans[0] + model_pos_x,
                    yawed_trans[1] + model_pos_y,
                    yawed_trans[2] + model_pos_z,
                };
                /* BLADE/HAND WORLD POINT capture (byte-true kine layout: the melee SLASH reads
                 * *(0x800acbdc)+0x7b8 = bone-11 matrix translation — per-bone stride 0xAC, matrix
                 * @+0x40, translation @+0x14 -> 11*0xAC+0x40+0x14 = 0x7b8; resolver LAB_80040b88).
                 * Hand the player's bone-11 world origin to the damage resolver each rendered
                 * frame (1-frame-stale vs the PSX in-frame pose pass — faithful-line). */
                if (bi == 11) {
                    extern void re15_player_set_hand_world(int32_t x, int32_t y, int32_t z);
                    extern void re15_player_set_hand_rot(const int32_t r[9]);
                    re15_player_set_hand_world(bone_world_trans[0], bone_world_trans[1],
                                               bone_world_trans[2]);
                    re15_player_set_hand_rot(yawed_rot);   /* R_gunbone (WORLD rot) for the discharge-FX anchor */
                }
                /* CANONICAL per-bone light fold (2026-06-02): rotate the world
                 * light dirs into THIS bone's frame so the raw bone-local normals
                 * shade as L_world · N_world. yawed_rot = R_y(yaw) × pose.rot is the
                 * bone's world rotation (same matrix used for the vertex transform).
                 * Mirrors PSX SetLightMatrix(LLM_world × bone_rot)/bone. */
                if (player_lit)
                    re15_light_ctx_rotate_for_bone(&lctx_player_world, yawed_rot,
                                                   &lctx_player);
                /* Phase 4.5.12-G diag: dump each bone's WORLD y so we can
                 * see Leon's actual span at runtime (vs E3 agent's predicted
                 * 829 units after clip 0 frame 0 YXZ Euler). One-shot on
                 * frame 60. */
                if (g_engine.frame_count == 60) {
                    fprintf(stderr, "[bone] %2d: rel=(%d,%d,%d) world=(%d,%d,%d)\n",
                            bi, (int)p->trans[0], (int)p->trans[1], (int)p->trans[2],
                            (int)bone_world_trans[0], (int)bone_world_trans[1],
                            (int)bone_world_trans[2]);
                }
                /* AL-round (2026-05-26): dump Leon's root bone + view+screen
                 * pos right after the visibility gate fires so we can see
                 * whether Q12 fix put him off-cut. */
                if (bi == 0 && (g_engine.frame_count == 1700 ||
                                g_engine.frame_count == 1750 ||
                                g_engine.frame_count == 1800)) {
                    int32_t cr[9], ct[3];
                    re15_camera_compose_view_bone(&cam_view, yawed_rot, bone_world_trans,
                                                  cr, ct);
                    float _proj = (cam_view.fov_screen_dist > 0 && ct[2] > 1)
                        ? (float)cam_view.fov_screen_dist / (float)ct[2]
                        : 0.0f;
                    fprintf(stderr,
                        "[F%u-leon-root] world=(%d,%d,%d) view=(%d,%d,%d) "
                        "sd=%d screen=(%.1f,%.1f) cam_pos=(%d,%d,%d) cam_tgt=(%d,%d,%d) cut=%d visible=%d\n",
                        g_engine.frame_count,
                        (int)player_ref->x, (int)player_ref->y, (int)player_ref->z,
                        (int)ct[0], (int)ct[1], (int)ct[2],
                        (int)cam_view.fov_screen_dist,
                        (float)cx + ct[0] * _proj, (float)cy + ct[1] * _proj,
                        (int)active_cuts[active_cut_idx].pos_x,
                        (int)active_cuts[active_cut_idx].pos_y,
                        (int)active_cuts[active_cut_idx].pos_z,
                        (int)active_cuts[active_cut_idx].target_x,
                        (int)active_cuts[active_cut_idx].target_y,
                        (int)active_cuts[active_cut_idx].target_z,
                        active_cut_idx, player_visible);
                    fflush(stderr);
                }
                int32_t combined_rot[9];
                int32_t combined_trans[3];
                re15_camera_compose_view_bone(&cam_view, yawed_rot, bone_world_trans,
                                               combined_rot, combined_trans);
                for (int k = 0; k < 9; k++) bone_m[k] = combined_rot[k];
                bone_t[0] = combined_trans[0];
                bone_t[1] = combined_trans[1];
                bone_t[2] = combined_trans[2];
                /* save bone 11's COMPOSED matrix for the weapon-in-hand draw below */
                if (bi == 11) {
                    for (int k = 0; k < 9; k++) { wpn_bone_m[k] = bone_m[k]; wpn_yawed[k] = yawed_rot[k]; }
                    wpn_bone_t[0] = bone_t[0]; wpn_bone_t[1] = bone_t[1]; wpn_bone_t[2] = bone_t[2];
                    wpn_bone_valid = 1;
                }

                /* Phase 4.5.10-I DEBUG: HD per-bone marker. 8×8 high-contrast
                 * tile per bone. If projected position is OFF-screen, render
                 * a CLAMPED edge marker so we still see it. Plus dump bone
                 * 0 details (camera-space coords + screen) on a clear HUD
                 * line. */
                {
                    float _vx = bone_t[0];
                    float _vy = bone_t[1];
                    float _vz = bone_t[2];
                    if (_vz < 1.0f) _vz = 1.0f;
                    float _proj = screen_dist / _vz;
                    int sx = cx + RNDI(_vx * _proj);
                    int sy = cy + RNDI(_vy * _proj);
                    /* Clamp off-screen markers to edge so user SEES the
                     * direction Leon's bones flew off-screen to. */
                    int sx_clamp = sx;
                    int sy_clamp = sy;
                    int off_screen = 0;
                    if (sx_clamp < 4)   { sx_clamp = 4;   off_screen = 1; }
                    if (sx_clamp > SCREEN_XRES - 12) { sx_clamp = SCREEN_XRES - 12; off_screen = 1; }
                    if (sy_clamp < 4)   { sy_clamp = 4;   off_screen = 1; }
                    if (sy_clamp > SCREEN_YRES - 12) { sy_clamp = SCREEN_YRES - 12; off_screen = 1; }
                    static const uint8_t s_bone_colors[16][3] = {
                        {255, 255, 0},   /* yellow  bone 0 */
                        {0,   255, 0},   /* green   */
                        {255, 0,   0},   /* red     */
                        {0,   255, 255}, /* cyan    */
                        {255, 0,   255}, /* magenta */
                        {255, 128, 0},   /* orange  */
                        {128, 255, 128},
                        {255, 255, 255},
                        {128, 128, 255},
                        {255, 128, 128},
                        {128, 255, 0},
                        {0,   128, 255},
                        {200, 200, 0},
                        {0,   200, 200},
                        {200, 0,   200},
                        {255, 192, 64},
                    };
                    int ci = bi & 0xF;
                    (void)ci; (void)s_bone_colors; (void)sx_clamp; (void)sy_clamp;
                    /* I-round disable (2026-05-24): debug bone markers were
                     * the source of user-reported "multi-colored cubes"
                     * overlaying Leon — 15×8×8 colored tiles drawn over
                     * his mesh. Disabled now that bone math is correct. */
                    /* re15_render_tile(sx_clamp, sy_clamp, 8, 8, 0,
                                     s_bone_colors[ci][0],
                                     s_bone_colors[ci][1],
                                     s_bone_colors[ci][2]); */
                    (void)off_screen;
                    if (bi == 0 && g_engine.frame_count == 60) {
                        /* Phase 4.5.10-J: one-shot dump to debug.log at
                         * frame 60 so user can paste exact numbers. */
                        fprintf(stderr,
                            "[F60] cut=%d sd=%d\n"
                            "  cut.pos=(%d,%d,%d)\n"
                            "  cut.target=(%d,%d,%d)\n"
                            "  cut.fov=%u\n"
                            "  VT=(%d,%d,%d)\n"
                            "  VR0=(%d,%d,%d) VR1=(%d,%d,%d) VR2=(%d,%d,%d)\n"
                            "  Player=(%ld,%ld,%ld) rot_y=%d\n"
                            "  pose[0].trans=(%ld,%ld,%ld)\n"
                            "  bone_world=(%ld,%ld,%ld)\n"
                            "  combined_trans=(%d,%d,%d)\n"
                            "  combined_rot[0..2]=(%d,%d,%d)\n"
                            "  proj sx=%d sy=%d\n",
                            active_cut_idx, (int)screen_dist,
                            (int)active_cuts[active_cut_idx].pos_x,
                            (int)active_cuts[active_cut_idx].pos_y,
                            (int)active_cuts[active_cut_idx].pos_z,
                            (int)active_cuts[active_cut_idx].target_x,
                            (int)active_cuts[active_cut_idx].target_y,
                            (int)active_cuts[active_cut_idx].target_z,
                            (unsigned)active_cuts[active_cut_idx].fov,
                            (int)cam_view.trans[0], (int)cam_view.trans[1], (int)cam_view.trans[2],
                            (int)cam_view.rot[0], (int)cam_view.rot[1], (int)cam_view.rot[2],
                            (int)cam_view.rot[3], (int)cam_view.rot[4], (int)cam_view.rot[5],
                            (int)cam_view.rot[6], (int)cam_view.rot[7], (int)cam_view.rot[8],
                            (long)g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                            (long)g_actors[RE15_ACTOR_SLOT_PLAYER].y,
                            (long)g_actors[RE15_ACTOR_SLOT_PLAYER].z,
                            (int)g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y,
                            (long)poses[0].trans[0], (long)poses[0].trans[1], (long)poses[0].trans[2],
                            (long)bone_world_trans[0], (long)bone_world_trans[1], (long)bone_world_trans[2],
                            (int)combined_trans[0], (int)combined_trans[1], (int)combined_trans[2],
                            (int)combined_rot[0], (int)combined_rot[1], (int)combined_rot[2],
                            sx, sy);
                    }
                    /* B0/VT/VR HUD silenced for cleaner cinematic view. */
                }

                const re15_md1_mesh_t *m = &md1.meshes[mi];

                /* Phase 4.5.7.5: textured triangles. The MD1's per-tri
                 * `page` selects which 128-pixel-wide sub-page of the TIM
                 * the UVs sample from. PC's SDL_Texture is the WHOLE TIM
                 * as one image, so we add page_x_offset to each UV.x to
                 * map page-local 0..127 → full-TIM pixel coord.
                 *
                 * page bits 0..3 = VRAM_x / 64. For 8bpp, each VRAM column
                 * holds 2 pixels, so page_x_offset = (page & 0xF) * 128.
                 *
                 * Caveat (#PC-2): only the first CLUT is decoded into the
                 * SDL_Texture, so body/limb colors will be off until the
                 * PC backend learns to switch CLUTs per triangle. */
                for (int ti = 0; ti < m->triangle_count; ti++) {
                    const re15_md1_triangle_t *t = &m->triangles[ti];
                    if (t->v0 >= (uint32_t)m->tri_vertex_count) continue;
                    if (t->v1 >= (uint32_t)m->tri_vertex_count) continue;
                    if (t->v2 >= (uint32_t)m->tri_vertex_count) continue;
                    int sx0, sy0, sx1, sy1, sx2, sy2;
                    float wz0, wz1, wz2;
                    PROJECT_VERT(&m->tri_vertices[t->v0], sx0, sy0, wz0);
                    PROJECT_VERT(&m->tri_vertices[t->v1], sx1, sy1, wz1);
                    PROJECT_VERT(&m->tri_vertices[t->v2], sx2, sy2, wz2);
                    /* H28: skip tris where any vert is behind near-plane
                     * (was clamping to vz=1 → exploded projections). */
                    if (wz0 < 0 || wz1 < 0 || wz2 < 0) continue;
                    /* Backface cull — REQUIRED. PSX's gte_nclip drops these
                     * implicitly on the hardware side; PC needs the explicit
                     * check or back-facing tris render too, producing the
                     * "duplicate arms / atlas-on-the-right" pattern the user
                     * saw (each mesh drawn twice with its back-face UVs). */
                    if (IS_BACKFACE(sx0, sy0, sx1, sy1, sx2, sy2)) continue;
                    const re15_md1_tri_uv_t *uv = &m->triangle_uvs[ti];
                    int page_x_offset = (int)((uv->page & 0x000F) * 128);
                    /* Phase 4.5.7.7: avg world-Z for back-to-front sort. */
                    int avg_z = (int)((wz0 + wz1 + wz2) * (1.0f / 3.0f));
                    /* BF-round: per-vertex NCCT shading. Lookup normals by
                     * tri.n0/n1/n2 indices; fall back to tint if missing. */
                    uint8_t r0t, g0t, b0t, r1t, g1t, b1t, r2t, g2t, b2t;
                    if (m->tri_normals &&
                        t->n0 < (uint32_t)m->tri_normal_count &&
                        t->n1 < (uint32_t)m->tri_normal_count &&
                        t->n2 < (uint32_t)m->tri_normal_count) {
                        const re15_md1_vertex_t *n0 = &m->tri_normals[t->n0];
                        const re15_md1_vertex_t *n1 = &m->tri_normals[t->n1];
                        const re15_md1_vertex_t *n2 = &m->tri_normals[t->n2];
                        re15_light_shade_vertex(&lctx_player, n0->x, n0->y, n0->z, &r0t, &g0t, &b0t);
                        re15_light_shade_vertex(&lctx_player, n1->x, n1->y, n1->z, &r1t, &g1t, &b1t);
                        re15_light_shade_vertex(&lctx_player, n2->x, n2->y, n2->z, &r2t, &g2t, &b2t);
                    } else {
                        r0t = r1t = r2t = g_re15_light_tint[0];
                        g0t = g1t = g2t = g_re15_light_tint[1];
                        b0t = b1t = b2t = g_re15_light_tint[2];
                    }
                    re15_render_textured_tri_lit(
                        sx0, sy0, (int)uv->u0 + page_x_offset, (int)uv->v0,
                        sx1, sy1, (int)uv->u1 + page_x_offset, (int)uv->v1,
                        sx2, sy2, (int)uv->u2 + page_x_offset, (int)uv->v2,
                        0, (int)uv->clut, avg_z,
                        r0t, g0t, b0t, r1t, g1t, b1t, r2t, g2t, b2t);
                }

                /* Quads: 4-edge wireframe loop in (v0,v1,v3,v2) winding. */
                for (int qi = 0; qi < m->quad_count; qi++) {
                    const re15_md1_quad_t *q = &m->quads[qi];
                    if (q->v0 >= (uint32_t)m->quad_vertex_count) continue;
                    if (q->v1 >= (uint32_t)m->quad_vertex_count) continue;
                    if (q->v2 >= (uint32_t)m->quad_vertex_count) continue;
                    if (q->v3 >= (uint32_t)m->quad_vertex_count) continue;
                    int sx0, sy0, sx1, sy1, sx2, sy2, sx3, sy3;
                    float wz0, wz1, wz2, wz3;
                    PROJECT_VERT(&m->quad_vertices[q->v0], sx0, sy0, wz0);
                    PROJECT_VERT(&m->quad_vertices[q->v1], sx1, sy1, wz1);
                    PROJECT_VERT(&m->quad_vertices[q->v2], sx2, sy2, wz2);
                    PROJECT_VERT(&m->quad_vertices[q->v3], sx3, sy3, wz3);
                    /* H28: skip quads where any vert is behind near-plane. */
                    if (wz0 < 0 || wz1 < 0 || wz2 < 0 || wz3 < 0) continue;
                    /* Backface cull — same reason as the triangle path. */
                    if (IS_BACKFACE(sx0, sy0, sx1, sy1, sx2, sy2)) continue;
                    const re15_md1_quad_uv_t *uv = &m->quad_uvs[qi];
                    int page_x_offset = (int)((uv->page & 0x000F) * 128);
                    /* Phase 4.5.7.7: avg world-Z for back-to-front sort.
                     * Each split tri gets its OWN 3-vertex average so the
                     * two halves of a quad sort independently — matches
                     * PSX's gte_avsz3 per-poly OTZ rather than the quad-
                     * wide gte_avsz4 (we lose 4-vert averaging but gain
                     * finer self-sort on twisted quads). */
                    int avg_z1 = (int)((wz0 + wz1 + wz3) * (1.0f / 3.0f));
                    int avg_z2 = (int)((wz0 + wz3 + wz2) * (1.0f / 3.0f));
                    /* BF-round: per-vertex shading for the 4 quad corners. */
                    uint8_t qr0, qg0, qb0, qr1, qg1, qb1, qr2, qg2, qb2, qr3, qg3, qb3;
                    if (m->quad_normals &&
                        q->n0 < (uint32_t)m->quad_normal_count &&
                        q->n1 < (uint32_t)m->quad_normal_count &&
                        q->n2 < (uint32_t)m->quad_normal_count &&
                        q->n3 < (uint32_t)m->quad_normal_count) {
                        const re15_md1_vertex_t *qn0 = &m->quad_normals[q->n0];
                        const re15_md1_vertex_t *qn1 = &m->quad_normals[q->n1];
                        const re15_md1_vertex_t *qn2 = &m->quad_normals[q->n2];
                        const re15_md1_vertex_t *qn3 = &m->quad_normals[q->n3];
                        re15_light_shade_vertex(&lctx_player, qn0->x, qn0->y, qn0->z, &qr0, &qg0, &qb0);
                        re15_light_shade_vertex(&lctx_player, qn1->x, qn1->y, qn1->z, &qr1, &qg1, &qb1);
                        re15_light_shade_vertex(&lctx_player, qn2->x, qn2->y, qn2->z, &qr2, &qg2, &qb2);
                        re15_light_shade_vertex(&lctx_player, qn3->x, qn3->y, qn3->z, &qr3, &qg3, &qb3);
                    } else {
                        qr0 = qr1 = qr2 = qr3 = g_re15_light_tint[0];
                        qg0 = qg1 = qg2 = qg3 = g_re15_light_tint[1];
                        qb0 = qb1 = qb2 = qb3 = g_re15_light_tint[2];
                    }
                    /* Quad → 2 tris with (v0,v1,v3,v2) winding (matches PSX). */
                    re15_render_textured_tri_lit(
                        sx0, sy0, (int)uv->u0 + page_x_offset, (int)uv->v0,
                        sx1, sy1, (int)uv->u1 + page_x_offset, (int)uv->v1,
                        sx3, sy3, (int)uv->u3 + page_x_offset, (int)uv->v3,
                        0, (int)uv->clut, avg_z1,
                        qr0, qg0, qb0, qr1, qg1, qb1, qr3, qg3, qb3);
                    re15_render_textured_tri_lit(
                        sx0, sy0, (int)uv->u0 + page_x_offset, (int)uv->v0,
                        sx3, sy3, (int)uv->u3 + page_x_offset, (int)uv->v3,
                        sx2, sy2, (int)uv->u2 + page_x_offset, (int)uv->v2,
                        0, (int)uv->clut, avg_z2,
                        qr0, qg0, qb0, qr3, qg3, qb3, qr2, qg2, qb2);
                }
            }

            /* WEAPON TEXTURE COMPOSITE (byte-true FUN_80036b68 @0x80036c08): the equip commit DMAs the
             * equipped weapon's OWN dir[3] TIM (a 56x32 gun/knife sprite) into VRAM at the spot the in-hand
             * mesh samples. The mesh (PL00W<aca5d> dir[2]) reads page 0x81 (x+128) / clut 0x7840 (slab 1,
             * V+256) at u~72-127 / v~108-139, i.e. slot-0 texels (~200,~364). So we blit the current
             * weapon's decoded dir[3] there, per-equip — WITHOUT it, the mesh samples the stale body-skin
             * placeholder = the untextured knife/gun the user reported. (PL00.TIM/PL04.TIM do NOT bake the
             * per-weapon art; each PLW carries its own dir[3], DMA'd on equip.) Re-blit only on aca5d change. */
            {   /* Blut-Decal-Sync (analysis/blood_decals.md D2/D4): bei Level-Aenderung ODER
                 * Slot-0-Reupload (Atlas-Rebuild wischt die Stempel — das Original persistiert
                 * sie im VRAM, der Port re-appliziert; Modell = der Original-Re-Insert
                 * LAB_80037d1c) alle Panels mit level>0 aus der Damage-Bank blitten.
                 * LUT @0x80074208 (PSX.EXE file 0x64a08; Halfword-x -> Pixel = x*2):
                 * Zeile 0 = Leon (PL00), Zeile 1 = Elza-Slot 4 (PL04).
                 * srcX = 576+(p&3)*16 hw (@0x80037c64-84) -> TIM-px 256+(p&3)*32;
                 * srcY = level*128+128+(p>=4?0x40:0) (@0x80037f48-74) -> TIM-Zeile srcY-256. */
                extern int re15_wound_generation(void);
                extern int re15_wound_level(int panel);
                extern int re15_render_pc_wound_blit(int sx,int sy,int dx,int dy,int w,int h);
                extern unsigned re15_render_pc_slot0_generation(void);
                static const uint8_t WLUTX[2][8] = { {67,43,31,66,22,16,6,0},
                                                     {67,43,31,66,23,7,7,24} };
                static const uint8_t WLUTY[2][8] = { {35,44,192,175,116,26,102,0},
                                                     {35,44,192,175,99,34,115,6} };
                static int s_wnd_gen = -1; static unsigned s_wnd_slot_gen = 0xffffffffu;
                {   /* Original-Debug-Eingang LAB_80037de4 ("alles verwunden", shipped ohne
                     * Caller) als Render-Pfad-Probe: RE15_WOUND_DEBUG=<level 1|2> */
                    static int s_wdbg = -1;
                    if (s_wdbg < 0) { const char *e = getenv("RE15_WOUND_DEBUG");
                                      s_wdbg = e ? atoi(e) : 0;
                                      if (s_wdbg > 0) { extern void re15_wound_debug_all(int);
                                                        re15_wound_debug_all(s_wdbg); } }
                }
                unsigned wsg = re15_render_pc_slot0_generation();
                if (re15_wound_generation() != s_wnd_gen || wsg != s_wnd_slot_gen) {
                    int chr = (g_gameflow.character == 0) ? 0 : 1;
                    int ok = 1, any = 0;
                    for (int p = 0; p < 8; p++) {
                        int lv = re15_wound_level(p);
                        if (lv <= 0) continue;
                        any = 1;
                        int sxp = 256 + (p & 3) * 32;
                        int syp = (lv * 128 + 128 + ((p >= 4) ? 0x40 : 0)) - 256;
                        ok &= re15_render_pc_wound_blit(sxp, syp,
                                                        (int)WLUTX[chr][p] * 2, (int)WLUTY[chr][p],
                                                        32, 64);
                    }
                    if (any)
                        fprintf(stderr, "[wound] sync: applied=%d ok=%d gen=%d slotgen=%u\n",
                                any, ok, re15_wound_generation(), re15_render_pc_slot0_generation());
                    if (ok || !any) { s_wnd_gen = re15_wound_generation();
                                      s_wnd_slot_gen = re15_render_pc_slot0_generation(); }
                }
            }
            {
                extern int  re15_player_equipped_weapon(void);
                extern int  re15_render_pc_composite_slot0(const uint32_t *wpn, int ww, int wh, int dx, int dy);
                extern unsigned re15_render_pc_slot0_generation(void);
                /* On each (character,weapon) change, decode the equipped weapon's dir[3] sprite and REBUILD
                 * slot 0 (body skin + weapon composited) via re15_render_pc_composite_slot0 — a fresh texture
                 * upload, NOT a per-frame sub-rect update (hardware GL drivers drop those on a sampled STATIC
                 * texture -> untextured weapon on the user's GPU while a software renderer showed it fine).
                 * TARGET TEXEL (re-derived from the mesh UVs, per-prim): each PL00W** dir[2] mesh has TWO
                 * UV bands on page 0x81 — the HAND band at v108-157 (reads real skin) and the WEAPON band
                 * at u72-126 / v224-251, a 56x28 region that exactly matches the 56x32 dir[3] sprite. So
                 * the sprite belongs at X = page-0x81 base (0xF*128=128) + u_min 72 = 200, Y = clut-0x7840
                 * slab-1 V-offset (256) + v_min 224 = 480. (An earlier composite used v_min 108 -> Y=364,
                 * which patched the HAND band and left the blade/barrel reading stale skin = untextured.) */
                static int s_wpn_key = -1; static unsigned s_wpn_gen = 0u;
                int eqw = re15_player_equipped_weapon();
                int key = (g_gameflow.character << 8) | (eqw & 0xFF);
                unsigned gen = re15_render_pc_slot0_generation();
                /* Re-composite when the weapon changes OR slot 0 was re-uploaded (which wipes the
                 * baked-in weapon), and RETRY next frame whenever the composite could not apply yet
                 * (e.g. the skin base is not uploaded at this point in the boot/resume order). */
                if (eqw > 0 && eqw < RE15_WPN_MDL_MAX && (key != s_wpn_key || gen != s_wpn_gen)) {
                    const char *fam = (g_gameflow.character == 0) ? "PL00" : "PL04";
                    char nm[32]; snprintf(nm, sizeof nm, "PLD/%sW%02X.PLW", fam, eqw);
                    int psz = 0; uint8_t *plw = pc_read_shared(nm, &psz);
                    if (plw && psz > 16) {
                        uint32_t diroff = (uint32_t)(plw[0]|(plw[1]<<8)|(plw[2]<<16)|((uint32_t)plw[3]<<24));
                        if (diroff + 16 <= (uint32_t)psz) {
                            uint32_t de3 = (uint32_t)(plw[diroff+12]|(plw[diroff+13]<<8)|
                                                     (plw[diroff+14]<<16)|((uint32_t)plw[diroff+15]<<24));
                            re15_tim_t wt; int pr = (de3 < (uint32_t)psz) ? re15_tim_parse(plw+de3,(int)(psz-de3),&wt) : -1;
                            if (pr==0 && wt.has_clut && wt.pixels && wt.clut && wt.bpp == 8) {
                                int cw = wt.width, ch = wt.height;
                                uint32_t *rgba = (uint32_t*)malloc((size_t)cw*ch*4);
                                if (rgba) {
                                    const uint8_t *src = (const uint8_t*)wt.pixels;   /* 8bpp indices */
                                    for (int i = 0; i < cw*ch; i++) {
                                        uint16_t c = wt.clut[src[i]];       /* dir[3]'s OWN 256-entry CLUT */
                                        uint32_t R=(uint32_t)((c&0x1F)<<3), G=(uint32_t)(((c>>5)&0x1F)<<3), B=(uint32_t)(((c>>10)&0x1F)<<3);
                                        rgba[i] = 0xFF000000u | (R<<16) | (G<<8) | B;
                                    }
                                    if (re15_render_pc_composite_slot0(rgba, cw, ch, 200, 480)) {
                                        s_wpn_key = key; s_wpn_gen = gen;   /* latch ONLY when it actually applied */
                                    }
                                    free(rgba);
                                }
                            }
                        }
                    }
                }
            }

            /* WEAPON-IN-HAND — byte-true: the equip commit FUN_80036b68 @0x80036c08 attaches the in-hand
             * weapon mesh (kine+0x76c/770/774/778) UNCONDITIONALLY when aca5d!=0, so the EQUIPPED weapon
             * (knife/handgun/shotgun/...) is in Leon's hand whenever equipped — not only while aiming
             * (savestate 3: attach pointers all set). The mesh = PL00W<aca5d> dir[2] (Leon; base_table
             * @0x800741e8 = {76,76,76,76,97,97,97,97}, FUN_80036b68 @0x80036b84), textured via the dir[3]
             * composite above at page 0x81 / clut 0x7840 -> slab 1 of the body-skin slot 0. */
            {
                extern int re15_player_equipped_weapon(void);
                int eq = re15_player_equipped_weapon();     /* aca5d = the equipped weapon id */
                int wi = (eq >= 0 && eq < RE15_WPN_MDL_MAX) ? eq : 0;
                int vis = wpn_md1_ok[wi];                    /* show whatever weapon is equipped */
                if (vis && wpn_bone_valid && player_visible &&
                    re15_player_victim_state() == 0) {
                    re15_render_pc_bind_tim_slot(0);   /* body-skin TIM (PL00.TIM for Leon); gun art page 0x81/clut-1 */
                    for (int k = 0; k < 9; k++) bone_m[k] = wpn_bone_m[k];
                    bone_t[0] = wpn_bone_t[0]; bone_t[1] = wpn_bone_t[1]; bone_t[2] = wpn_bone_t[2];
                    if (player_lit)
                        re15_light_ctx_rotate_for_bone(&lctx_player_world, wpn_yawed, &lctx_player);
                    const re15_md1_mesh_t *m = &wpn_md1[wi].meshes[0];
                    for (int ti = 0; ti < m->triangle_count; ti++) {
                        const re15_md1_triangle_t *t = &m->triangles[ti];
                        if (t->v0 >= (uint32_t)m->tri_vertex_count ||
                            t->v1 >= (uint32_t)m->tri_vertex_count ||
                            t->v2 >= (uint32_t)m->tri_vertex_count) continue;
                        int sx0, sy0, sx1, sy1, sx2, sy2;
                        float wz0, wz1, wz2;
                        PROJECT_VERT(&m->tri_vertices[t->v0], sx0, sy0, wz0);
                        PROJECT_VERT(&m->tri_vertices[t->v1], sx1, sy1, wz1);
                        PROJECT_VERT(&m->tri_vertices[t->v2], sx2, sy2, wz2);
                        if (wz0 < 0 || wz1 < 0 || wz2 < 0) continue;
                        if (IS_BACKFACE(sx0, sy0, sx1, sy1, sx2, sy2)) continue;
                        const re15_md1_tri_uv_t *uv = &m->triangle_uvs[ti];
                        int pxo = (int)((uv->page & 0x000F) * 128);
                        int avg_z = (int)((wz0 + wz1 + wz2) * (1.0f / 3.0f));
                        uint8_t r0t,g0t,b0t,r1t,g1t,b1t,r2t,g2t,b2t;
                        if (m->tri_normals && t->n0 < (uint32_t)m->tri_normal_count &&
                            t->n1 < (uint32_t)m->tri_normal_count &&
                            t->n2 < (uint32_t)m->tri_normal_count) {
                            const re15_md1_vertex_t *n0 = &m->tri_normals[t->n0];
                            const re15_md1_vertex_t *n1 = &m->tri_normals[t->n1];
                            const re15_md1_vertex_t *n2 = &m->tri_normals[t->n2];
                            re15_light_shade_vertex(&lctx_player, n0->x,n0->y,n0->z, &r0t,&g0t,&b0t);
                            re15_light_shade_vertex(&lctx_player, n1->x,n1->y,n1->z, &r1t,&g1t,&b1t);
                            re15_light_shade_vertex(&lctx_player, n2->x,n2->y,n2->z, &r2t,&g2t,&b2t);
                        } else {
                            r0t=r1t=r2t=g_re15_light_tint[0];
                            g0t=g1t=g2t=g_re15_light_tint[1];
                            b0t=b1t=b2t=g_re15_light_tint[2];
                        }
                        re15_render_textured_tri_lit(
                            sx0, sy0, (int)uv->u0 + pxo, (int)uv->v0,
                            sx1, sy1, (int)uv->u1 + pxo, (int)uv->v1,
                            sx2, sy2, (int)uv->u2 + pxo, (int)uv->v2,
                            0, (int)uv->clut, avg_z,
                            r0t,g0t,b0t, r1t,g1t,b1t, r2t,g2t,b2t);
                    }
                    for (int qi = 0; qi < m->quad_count; qi++) {
                        const re15_md1_quad_t *q = &m->quads[qi];
                        if (q->v0 >= (uint32_t)m->quad_vertex_count ||
                            q->v1 >= (uint32_t)m->quad_vertex_count ||
                            q->v2 >= (uint32_t)m->quad_vertex_count ||
                            q->v3 >= (uint32_t)m->quad_vertex_count) continue;
                        int sx0,sy0,sx1,sy1,sx2,sy2,sx3,sy3;
                        float wz0,wz1,wz2,wz3;
                        PROJECT_VERT(&m->quad_vertices[q->v0], sx0, sy0, wz0);
                        PROJECT_VERT(&m->quad_vertices[q->v1], sx1, sy1, wz1);
                        PROJECT_VERT(&m->quad_vertices[q->v2], sx2, sy2, wz2);
                        PROJECT_VERT(&m->quad_vertices[q->v3], sx3, sy3, wz3);
                        if (wz0 < 0 || wz1 < 0 || wz2 < 0 || wz3 < 0) continue;
                        if (IS_BACKFACE(sx0, sy0, sx1, sy1, sx2, sy2)) continue;
                        const re15_md1_quad_uv_t *uv = &m->quad_uvs[qi];
                        int pxo = (int)((uv->page & 0x000F) * 128);
                        int avg_z1 = (int)((wz0 + wz1 + wz3) * (1.0f / 3.0f));
                        int avg_z2 = (int)((wz0 + wz3 + wz2) * (1.0f / 3.0f));
                        uint8_t c0[3], c1[3], c2[3], c3[3];
                        c0[0]=c1[0]=c2[0]=c3[0]=g_re15_light_tint[0];
                        c0[1]=c1[1]=c2[1]=c3[1]=g_re15_light_tint[1];
                        c0[2]=c1[2]=c2[2]=c3[2]=g_re15_light_tint[2];
                        if (m->quad_normals && q->n0 < (uint32_t)m->quad_normal_count &&
                            q->n1 < (uint32_t)m->quad_normal_count &&
                            q->n2 < (uint32_t)m->quad_normal_count &&
                            q->n3 < (uint32_t)m->quad_normal_count) {
                            const re15_md1_vertex_t *n;
                            n=&m->quad_normals[q->n0]; re15_light_shade_vertex(&lctx_player,n->x,n->y,n->z,&c0[0],&c0[1],&c0[2]);
                            n=&m->quad_normals[q->n1]; re15_light_shade_vertex(&lctx_player,n->x,n->y,n->z,&c1[0],&c1[1],&c1[2]);
                            n=&m->quad_normals[q->n2]; re15_light_shade_vertex(&lctx_player,n->x,n->y,n->z,&c2[0],&c2[1],&c2[2]);
                            n=&m->quad_normals[q->n3]; re15_light_shade_vertex(&lctx_player,n->x,n->y,n->z,&c3[0],&c3[1],&c3[2]);
                        }
                        re15_render_textured_tri_lit(
                            sx0, sy0, (int)uv->u0 + pxo, (int)uv->v0,
                            sx1, sy1, (int)uv->u1 + pxo, (int)uv->v1,
                            sx3, sy3, (int)uv->u3 + pxo, (int)uv->v3,
                            0, (int)uv->clut, avg_z1,
                            c0[0],c0[1],c0[2], c1[0],c1[1],c1[2], c3[0],c3[1],c3[2]);
                        re15_render_textured_tri_lit(
                            sx0, sy0, (int)uv->u0 + pxo, (int)uv->v0,
                            sx3, sy3, (int)uv->u3 + pxo, (int)uv->v3,
                            sx2, sy2, (int)uv->u2 + pxo, (int)uv->v2,
                            0, (int)uv->clut, avg_z2,
                            c0[0],c0[1],c0[2], c3[0],c3[1],c3[2], c2[0],c2[1],c2[2]);
                    }
                }
            }
#undef PROJECT_VERT
#undef IS_BACKFACE

            /* NPC skeletal render: same MD1 mesh as Leon (we don't have
             * Elliot's PLD parsed yet), positioned at the NPC's world
             * pos using its motion/anim_frame for pose. Visually appears
             * as a second Leon-shaped character — communicates presence
             * even though wrong model. Bone-by-bone projection mirrors
             * the player loop above. */
            for (int npc_i = 1; npc_i < RE15_ACTOR_MAX; npc_i++) {
                const re15_actor_t *npc = &g_actors[npc_i];
                if (!npc->active) continue;

                /* Load the enemy model the moment the actor is ACTIVE — BEFORE the region-quad cull
                 * below, so an OFF-SCREEN actor still gets its EMD into the shared registry. The AI's
                 * root-motion (re15_clip_root_motion*) reads that bank regardless of visibility, and the
                 * PSX loads the model at Sce_em_set SPAWN (not lazily on first draw) — so a walking actor
                 * outside the current camera cut must still have its keyframes. (Was gated after the cull
                 * at the old site below; a culled off-screen enemy then had bank=NULL and froze.) */
                if (npc->type && !re15_enemy_find(npc->type))
                    /* inkl. 0x47 seit 7b (EM047-Own-Bank-Kanal).
                     * ⛔ 0x26 nach HERKUNFT (2026-08-22) — dieselbe Weiche wie in der
                     * Roster-Schleife: RDT-gesetzte 0x26er sind die FEUER-EMITTER von ROOM1090
                     * (0x80072bac[0x26] = 0x80116288, Registrierung @0x8011E8F4/@0x8011E8FC) und
                     * brauchen die RE1.5-Bank EM26; nur vom RE2-Adult-Spawner erzeugte
                     * (`addiu a0,zero,38` @0x80105DE8) sind echte Baby-Spinnen. DIESE Stelle ist
                     * die frueheste — sie laeuft im Render-Durchgang und hat die Bank vor der
                     * Roster-Schleife geholt (gemessen: ohne den Zusatz stand trotz Brain-Fix
                     * weiter "RE2 EM026 loaded: ... 3 clips" im Log). */
                    pc_enemy_load_ex(npc->type,
                                     npc->type != 0x26u || npc->re2s_baby_spawned);

                /* BO-round (Tier-3): canonical per-cut REGION-QUAD cull, same as
                 * the prop path (PSX FUN_8002c18c → FUN_80014368). Replaces the
                 * BH-round |x|<25000 teleport-hide proxy: Elliot at the off-stage
                 * (-31000,…) hide corner is outside every cut-0..6 quad → not
                 * drawn (nor lit), exactly as PSX. On-stage NPCs are inside →
                 * drawn. Falls back to nothing-culled if the cut has no region. */
                if (cam_has_region &&
                    !re15_aot_point_in_quad(npc->x, npc->z,
                                            cam_region_xs, cam_region_zs)) {
                    continue;
                }

                /* RE1.5 character shadow for this NPC — FUN_8001b064 is called
                 * PER-ENTITY (corners from param_1+0xc/+0xe), so each on-stage
                 * actor gets one. Actor-position center + CAMERA-yaw rotation
                 * (same corrected scheme as the player). */
                {
                    /* byte-true actor-yaw shadow matrix = RotMatrixY(npc->rot_y) via the trig LUT. */
                    int32_t nyaw[9];
                    re15_camera_yaw_matrix_angle(npc->rot_y, nyaw);
                    /* CORPSE BLOOD POOL (root-state-7 sub0/1, FUN_80109554 @0x801095e8-614 +
                     * @0x80109710-24): the shadow recolors 0x00ffff38 (subtractive dark red) and
                     * its half-extents grow +8/frame for the 0x5a pool budget (600/700 base ->
                     * ~1328/1428). Pool progress = 0x5a - +0x9e while settling (sub1), full after. */
                    int32_t nhx = 500, nhz = 600;
                    int corpse_pool = 0;
                    uint8_t sh_r = 0, sh_g = 0, sh_b = 0;     /* only read when corpse_pool/tint set */
                    int crow_tinted = 0;
                    /* The growing dark-red blood pool is the ZOMBIE root's corpse-settle (FUN_80109554):
                     * only the live-step zombie types (0x10/0x11/0x12/0x16/0x18) actually run
                     * re15_enemy_corpse_settle (the writer of grab_kill_ctr=0x5a). The other enemies
                     * (dog/spider/maggot/cockroach/alligator/tyrant/ivy/birkin/zgirl) reach state 7
                     * WITHOUT it, so its grab_kill_ctr/sub_state_1 hold unrelated values -> gating the pool
                     * on state==7 alone drew a spurious full-size zombie blood pool under those corpses
                     * (audit wf_246147e3). Gate on the zombie type: those others keep the normal shadow.
                     * KORREKTUR (crow_death_pool.md): die KRAEHE (0x21) gehoert NICHT zu "keine
                     * Pool-Maschine" — sie hat ihre EIGENE (Corpse-Handler [0] 0x80115830, +10/Tick
                     * fuer 51 Ticks, Farbe 0x00ffff38 @0x80115880-c8) und schreibt sie in die
                     * crow_shadow_*-Felder (AI-seitig, ACTIVE-Tail @0x80115fa0-6058 + Corpse). */
                    int nis_zombie = (npc->type == 0x10 || npc->type == 0x11 || npc->type == 0x12 ||
                                      npc->type == 0x16 || npc->type == 0x18);
                    /* Der RE1.5-Zweig gilt NUR im RE1.5-Modus: er liest grab_kill_ctr(+0x9E),
                     * das re15_enemy_corpse_settle (FUN_80109554) fuellt. Im RE2-Modus laeuft
                     * statt dessen FUN_8010A440 (Zweig unten) — und weil die RE2-Leiche +0x9E
                     * nie schreibt, lieferte diese Formel dort grow = 0x5a = die volle Lache
                     * ab dem ERSTEN Leichenframe (Nutzer-Report 2026-08-21). */
                    /* MIXED: typ-bezogen — die Zombie-Leiche ist dort RE1.5, also RE1.5-Formel. */
                    if (npc->state == RE15_AI_STATE_CORPSE && nis_zombie
                        && !re15_ai_re2_for_type(npc->type)) {
                        int grow = (npc->sub_state_1 <= 1)
                                     ? (0x5a - (npc->grab_kill_ctr > 0 ? npc->grab_kill_ctr : 0))
                                     : 0x5a;
                        if (grow < 0) grow = 0;
                        nhx = 600 + 8 * grow;                 /* af5c base for zombies = 600/700 */
                        nhz = 700 + 8 * grow;
                        corpse_pool = 1;
                        sh_r = 0x38; sh_g = 0xff; sh_b = 0xff;
                    }
                    /* RE2-KI-MODUS: die Zombie-Leiche hat eine EIGENE Lachen-Maschine, und die
                     * RE1.5-Formel darueber ist fuer sie falsch. Nutzer-Report 2026-08-21 ("der
                     * am Boden getoetete Zombie laeuft SOFORT aus"): die RE2-Leiche schreibt
                     * grab_kill_ctr(+0x9E) nie, also lieferte der Zweig oben grow = 0x5a-0 = 90
                     * = volle Lache im ERSTEN Leichenframe (gemessen 64/64 Seeds,
                     * probe_re2z_corpse). Byte-true ist stattdessen FUN_8010A440:
                     * Grower [+0x16C]+4/+6 += 5 pro Tick (@0x8010a5f0-608 Sub 1, @0x8010a664-678
                     * Sub 3, @0x8010a780-94 Sub 8) fuer +0x16A = 120 Ticks (@0x8010a508-10),
                     * Tint [+0x16C]+28/+68 = (alt&0xff000000)|0x00BFBF10 (@0x8010a4c0-508).
                     * Die AI schreibt dieselben Kanaele wie die Kraehe -> hier nur lesen. */
                    if (nis_zombie && npc->crow_shadow_w != 0) {
                        nhx = npc->crow_shadow_w; nhz = npc->crow_shadow_h;
                        corpse_pool = npc->crow_pool ? 1 : 0;
                        if (corpse_pool) { sh_r = 0x10; sh_g = 0xBF; sh_b = 0xBF; }
                    }
                    /* ⛔ NUTZER-REPORT 2026-08-21: "platzende Kraehen ... hinterlassen immer noch
                     * Schatten". URSACHE war GENAU HIER. Die RE2-Kraehe GIBT ihren Schatten-/
                     * Prim-Record im GIB- und im Wandsplat-Zweig FREI:
                     *   80103c24 lw v1,364(a0) / 80103c34 sb zero,14(v1)   (CORPSE Sub 1, GIB)
                     *   80103ca8 lw v1,364(s1) / 80103cbc sb zero,14(v1)   (CORPSE Sub 2, Wand)
                     * rec+0x0E ist das BELEGT-BYTE des 50-Slot-Pools ab 0x800CE698 (Allokator
                     * 0x80016480: Freisuche `lbu v0,14(t0)` @0x800164AC-B4, Belegen `sb 5,14(t0)`
                     * @0x800164D4) — 0 heisst FREI, es wird NICHTS mehr gezeichnet. Der Port
                     * bildet die Freigabe auf crow_shadow_w == 0 ab; der ALTE Test
                     * `crow_shadow_w != 0` fiel dann aber durch und liess nhx/nhz auf dem
                     * 500x600-DEFAULT stehen -> unter der geplatzten Kraehe blieb der normale
                     * Charakter-Schatten liegen. Jetzt: kein Record -> kein Quad.
                     * (Vor dem allerersten INIT-Tick ist crow_shadow_w ebenfalls 0 — dort
                     * existiert der Record im Original auch noch nicht, s. INIT @0x80100400.) */
                    int crow_no_record = 0;
                    if (npc->type == 0x21 && re15_ai_re2_for_type(npc->type)
                        && npc->crow_shadow_w == 0)
                        crow_no_record = 1;
                    if (npc->type == 0x21 && npc->crow_shadow_w != 0) {
                        /* Krähe: Groesse/Farbe kommen KOMPLETT aus der AI (Prim-Feld-Port von
                         * +0xbc/+0xbe/+0xc4/+0xec bzw. rec+0x04/+0x06/+0x1C/+0x44). */
                        nhx = npc->crow_shadow_w; nhz = npc->crow_shadow_h;
                        if (npc->crow_pool) {
                            corpse_pool = 1;
                            if (re15_ai_re2_for_type(npc->type)) {
                                /* RE2-Kraehen-Leiche: Record-Recolor 0x80016FE4(rec, 0x00BFBF10)
                                 * @0x80103A20 (Normal) / @0x80103F14 (Launch) — dieselbe Funktion
                                 * und dieselbe Konstante wie die RE2-Zombie-Leiche darueber. */
                                sh_r = 0x10; sh_g = 0xBF; sh_b = 0xBF;
                            } else {
                                sh_r = 0x38; sh_g = 0xff; sh_b = 0xff;   /* 0x00ffff38 @0x80115880-c8 */
                            }
                        } else {
                            /* Hoehen-Tint v (32..128, @0x80115fe8-6028; RE2-Kraehe: der
                             * Alloc-Default 0x00808080 @0x80016500-04). Backend-Konvention: der
                             * neutrale PSX-Prim 0x80 == Weiss (siehe re15_render_shadow_quad),
                             * also v*2, Clamp 255. Wipe (v=0) -> zeichnet nichts Sichtbares. */
                            int tv = (int)npc->crow_tint * 2; if (tv > 255) tv = 255;
                            sh_r = sh_g = sh_b = (uint8_t)tv;
                            crow_tinted = 1;
                        }
                    }
                    int32_t nsh_c[4][3] = {
                        { -nhx, 0,  nhz }, { -nhx, 0, -nhz },
                        {  nhx, 0,  nhz }, {  nhx, 0, -nhz },
                    };
                    /* Quad-Hoehe = BODEN, nicht Actor-Y: FUN_8001b064 setzt `t[1] = param_2`
                     * (Decompile RE_15_Quellcode_V2) — das Y kommt als a1 rein, und der
                     * Kraehen-Root-Post-Pass uebergibt a1 = +0x1ba = die Floor-Referenz
                     * (@0x8011221c-234, alle States). Fuer Boden-Gegner ist y == floor,
                     * die Kraehe liegt/fliegt aber UEBER +0x1ba (Leiche: floor-400
                     * @0x801148e4) — mit npc->y schwebte die Blutlache im Koerper
                     * (Nutzer-Report 2026-08-03). */
                    /* ⛔ 2026-08-21: unter dem RE2-Flavor uebernimmt re15_re2crow_tick den GANZEN
                     * Dispatch und kehrt VOR `e->crow_floor = re15_crow_room_coll(e)`
                     * (enemy_ai_common.c) zurueck — crow_floor(+0x1BA) wird dort also NIE
                     * geschrieben und der Quad lag auf Y = 0 statt auf dem Boden. Die
                     * RE2-Kraehe fuehrt ihre Boden-Referenz in +0x1C2: INIT laesst
                     * `0x8004FBA0(&pos,250,1024,0)` @0x801003F8 laufen und legt das Ergebnis mit
                     * `sh v0,450(s1)` @0x80100414 dort ab; jede Landung des Overlays rechnet
                     * `+0x1C2 - 250` (@0x80102B60 Flug-Treffer, @0x80103E6C Wandsplat). Der
                     * Port-Kanal dafuer ist dog_floor_y. */
                    int32_t nsh_y = npc->y;
                    if (npc->type == 0x21)
                        nsh_y = re15_ai_re2_for_type(npc->type)
                                  ? (int32_t)npc->dog_floor_y      /* +0x1C2 @0x80100414 */
                                  : (int32_t)npc->crow_floor;      /* +0x1BA @0x8011221c-234 */
                    int32_t nsh_world[3] = { npc->x, nsh_y, npc->z };
                    int32_t nsh_rot[9], nsh_trans[3];
                    /* byte-true: RotMatrixY(ACTOR rot_y) via the trig LUT (FUN_8001b064 @0x8001b0e4
                     * uses entity+0x6a, NOT the camera yaw — RE'd wf_13911cba). nyaw above IS that
                     * matrix; the old camera-yaw was the port author's misread of DAT_800ac784. */
                    re15_camera_compose_view_bone(&cam_view, nyaw, nsh_world,
                                                  nsh_rot, nsh_trans);
                    int32_t nm[9], nt[3];   /* Q12 (byte-true integer RTPS) */
                    for (int i = 0; i < 9; i++) nm[i] = nsh_rot[i];
                    for (int i = 0; i < 3; i++) nt[i] = nsh_trans[i];
                    int nsx[4], nsy[4], nok = 1;
                    for (int v = 0; v < 4; v++) {
                        int32_t _x = nsh_c[v][0], _y = nsh_c[v][1], _z = nsh_c[v][2];
                        int32_t _vx = (int32_t)(((int64_t)_x*nm[0] + (int64_t)_y*nm[1] + (int64_t)_z*nm[2]) >> 12) + nt[0];
                        int32_t _vy = (int32_t)(((int64_t)_x*nm[3] + (int64_t)_y*nm[4] + (int64_t)_z*nm[5]) >> 12) + nt[1];
                        int32_t _vz = (int32_t)(((int64_t)_x*nm[6] + (int64_t)_y*nm[7] + (int64_t)_z*nm[8]) >> 12) + nt[2];
                        if (_vz < 64) { nok = 0; break; }
                        int32_t _ir1 = _vx > 0x7FFF ? 0x7FFF : (_vx < -0x8000 ? -0x8000 : _vx);
                        int32_t _ir2 = _vy > 0x7FFF ? 0x7FFF : (_vy < -0x8000 ? -0x8000 : _vy);
                        uint32_t _sz3 = _vz > 0xFFFF ? 0xFFFFu : (uint32_t)_vz;
                        uint32_t _n = re15_gte_divide((uint32_t)cam_view.fov_screen_dist, _sz3);
                        nsx[v] = cx + (int)(((int64_t)_ir1 * (int64_t)_n) >> 16);
                        nsy[v] = cy + (int)(((int64_t)_ir2 * (int64_t)_n) >> 16);
                    }
                    if (nok && !crow_no_record) {
                        if (corpse_pool || crow_tinted)
                            re15_render_shadow_quad_c(nsx[0], nsy[0], nsx[1], nsy[1],
                                                      nsx[2], nsy[2], nsx[3], nsy[3],
                                                      sh_r, sh_g, sh_b);   /* pool 0x38/0xff/0xff
                                                                            * bzw. Kraehen-Grau-Tint */
                        else
                            re15_render_shadow_quad(nsx[0], nsy[0], nsx[1], nsy[1],
                                                    nsx[2], nsy[2], nsx[3], nsy[3]);
                    }
                }

                /* GIB-Scatter (Kraehe): Original zerlegt den Koerper in die 13 Bone-Parts
                 * (@0x80114a50-aa4) und toetet sie nach 50 Ticks (@0x80114b78) -> Koerper weg.
                 * Der Port verbirgt das Mesh ab GIB-step-0 (crow_hide, ESP-Feder-Stand-in
                 * kommt aus der AI; Part-Scatter-Mover nicht RE'd, crow_death_pool.md §4.2).
                 * Der (unsichtbare 1x1-)Schatten oben zeichnet weiter wie im Original. */
                if (npc->type == 0x21 && npc->crow_hide) continue;

                /* (enemy-model load moved ABOVE the region cull — see the pc_enemy_load call right
                 * after the !npc->active guard; visibility-independent so off-screen AI has its bank.) */

                /* SHARED anim selection (unify 2026-06-06) — same view-model as the
                 * player + the PSX build (anim_select_common.c). is_player=0 enables
                 * the Elliot (type 0x47) branch: his own PL05 bank + mesh; everyone
                 * else falls back to Leon's PL00. Locomotion/idle sentinels route to
                 * PL00W01 / PL00-base exactly as for the player. (em21_* banks fields are
                 * no longer set — type 0x21 goes through the generic registry now.) */
                re15_anim_banks_t banks = {
                    .def_mesh = &md1, .def_skel = &skel, .def_anim = &anim,
                    .w01_skel = &w01_skel, .w01_anim = &w01_anim, .w01_ok = w01_ok,
                    .pl00_skel = &pl00_skel, .pl00_anim = &pl00_anim, .pl00_ok = pl00_ok,
                    .elliot_mesh = &elliot_md1, .elliot_skel = &elliot_skel,
                    .elliot_anim = &elliot_anim, .elliot_ok = (elliot_ok && elliot_skel_ok),
                };
                re15_anim_view_t av;
                re15_actor_anim_select(npc, 0, &banks, &av);
                int is_elliot = (av.mesh == &elliot_md1);
                const re15_md1_t           *npc_md1  = av.mesh;
                const re15_emd_skeleton_t  *npc_skel = av.skel;
                const re15_emd_animation_t *npc_anim = av.anim;
                /* WELLE G: Bone-Slot -> Mesh-Index NUR fuer eine Hybrid-Bank, und NUR wenn wir
                 * wirklich deren MD1 zeichnen (nicht den Elliot-/PL00-Fallback). */
                const int8_t *npc_remap = NULL;
                {
                    re15_enemy_bank_t *hb = re15_enemy_find(npc->type);
                    if (hb && hb->remap_ok && npc_md1 == &hb->md1) npc_remap = hb->mesh_remap;
                }

                /* LOCO-BANK render: the STAGE1 zombie WALKING states pose the locomotion bank (bank0,
                 * entity+0x84), NOT the 43-clip action bank. W1 disasm 2026-07-03: the ENGAGE (+0x5=2)
                 * IS the aware walk (bank0 clip +0x1d4 in {2..5} — the arms-out per-zombie walks),
                 * the 0x13 lurch plays bank0 clip 1, and the TURN (+0x5=7) pivots on the same +0x1d4
                 * clip. motion carries the clip index.
                 * HURT TOO (corrected 2026-07-28): the stagger handler FUN_80105B7C plays the SAME
                 * bank — both of its animating phases load `lw a0,132(v0)` = entity+0x84 = bank0
                 * (@0x80105d3c phase 1 / @0x80105de0 phase 3, each `jal 0x8001f314` anim_set with
                 * a1 = +0x16c, a3 = 0x200), and phase 0 sets +0x94 = +0x1d4 = the very same walk
                 * variant {2..5}. The old gate excluded state 2 on the assumption "+0x5 = weapon id
                 * collides with 2" — savestate 4 refutes it (+0x5 = 3 = the equipped ITEM id), and the
                 * exclusion made the stagger resolve clip 2..5 in the 43-clip ACTION bank = a
                 * completely different animation. Measured: not one of that bank's 2401 keyframes
                 * reproduces the original's part angles. Gate on the clip, not on +0x5.
                 * (Downed/lying hits keep the action bank — those handlers set their own clips.) */
                if (re15_actor_uses_loco_bank(npc)) {   /* THE single rule — enemy_ai_common.c;
                                                        * re15_actor_clip_len() must agree with the
                                                        * bank picked here or the frame clock
                                                        * compares against the wrong clip */
                    re15_enemy_bank_t *lb = re15_enemy_find(npc->type);
                    if (lb && lb->loco_ok && (int)npc->motion < lb->anim_loco.clip_count) {
                        npc_skel = &lb->skel_loco;
                        npc_anim = &lb->anim_loco;
                        av.clip_override = (int)npc->motion;   /* bank0: 1 = lurch, 2..5 = engage walks */
                    }
                }

                /* EXECUTOR-KANAL-Render (Sub->Kanal-Map, marvin_10d0.md Verify D2/D4): Sub 0
                 * spielt den RBJ-BINDER-Kanal +0x180/+0x184 (@0x80050d40/48 — FUN_8001b3f8
                 * bindet den Raum-Record per Marker; 10D0: REC1 -> Marvin), Sub 1 die
                 * Loco-Bank (+0x84/+0x16c), Sub 3 die Victim-Bank (+0x178/+0x17c). Ohne den
                 * Override posierten Marvins Cutscene-Gesten aus der EM040-eigenen Bank
                 * (falsche Clips/Laengen). Muss mit re15_npc_channel_anim (enemy_ai_common.c)
                 * uebereinstimmen — derselbe Kanal fuer Laenge UND Pose. */
                if (npc->state == 4 && npc->sub_state_1 == 0 && !npc->walk_active) {
                    /* (walk_active: der Plc_dest-Walker besitzt den Clip (5, eigene Bank) —
                     * State/Sub sind dann nur Reste der letzten Geste, kein RBJ-Posing.) */
                    const re15_emd_skeleton_t  *rs = re15_actor_rbj_skel((int)(npc - g_actors));
                    const re15_emd_animation_t *ra = re15_actor_rbj_anim((int)(npc - g_actors));
                    if (rs && ra && (int)npc->motion < ra->clip_count) {
                        npc_skel = rs; npc_anim = ra;
                        av.clip_override = (int)npc->motion;
                    }
                } else if (npc->state == 4 && !npc->walk_active &&
                           (npc->sub_state_1 == 1 || npc->sub_state_1 == 7 || npc->sub_state_1 == 8)) {
                    /* subs 1/7/8 -> Loco-Paar +0x84/+0x16c (f314-Loads sub 1 @0x80050e64,
                     * sub 7 @0x80051a20/24, sub 8 @0x80051c18/1c — marvin_spawn_anim.md Verify). */
                    re15_enemy_bank_t *lb = re15_enemy_find(npc->type);
                    if (lb && lb->loco_ok && (int)npc->motion < lb->anim_loco.clip_count) {
                        npc_skel = &lb->skel_loco; npc_anim = &lb->anim_loco;
                        av.clip_override = (int)npc->motion;
                    }
                } else if (npc->state == 4 && !npc->walk_active && npc->sub_state_1 == 3) {
                    re15_enemy_bank_t *lb = re15_enemy_find(npc->type);
                    if (lb && lb->victim_ok && (int)npc->motion < lb->anim_victim.clip_count) {
                        npc_skel = &lb->skel_victim; npc_anim = &lb->anim_victim;
                        av.clip_override = (int)npc->motion;
                    }
                } else if (re15_actor_uses_own_bank(npc)) {
                    /* ⛔ TYP-TOR (Nutzer-Report 2026-08-29 "schwarze Dreiecke ueber dem Feuer"):
                     * dieser ganze Block laeuft fuer JEDEN gezeichneten Aktor, nicht nur fuer die
                     * NPC-Familie. In v0.3.28 stand hier ein nacktes `npc->state == 1` — und
                     * State 1 ist der normale AKTIV-Zustand fast jedes Gegners. Jeder Zombie,
                     * jede Spinne und jeder Feuer-Emitter (0x26) mit eigener Bank 1 posierte
                     * damit aus dem FALSCHEN Skelett; das ist die Geometrie, die als schwarze
                     * Dreiecke erscheint. Die Eskorte betrifft ausschliesslich 0x40..0x4d
                     * (Dispatch-Tabelle 0x80072bac, NPC-Wurzeln 0x8011c5a0/0x8011cb70/...),
                     * also gehoert das Tor auch hierher. Die Engine-Seite
                     * (re15_actor_clip_len) war von Anfang an so gegatet.
                     *
                     * `state == 1` = die ESKORTE. Alle fuenf portierten Exec-Subs posieren dort
                     * aus +0x170/+0x174 = Bank 1:
                     * Stehen @0x8004f384-88, Gehen @0x8004f5c0-c4, Drehen @0x8004f7bc-c0,
                     * Nah @0x8004fb14, Laufen @0x8004ff68 (jeweils `lw a0,368(v0)` /
                     * `lw a1,372(v0)` unmittelbar vor `jal 0x8001f314`). Vorher fiel der
                     * Renderer hier auf die Container-Bank durch - dort ist Adas Clip 5 eine
                     * andere 20-Bilder-Animation und Clip 2 ein 50-Bilder-Sturz. */
                    /* Subs {2,4,5,6,9} + Plc_dest-Walk -> die EIGENE BANK 1 (+0x170/+0x174 =
                     * dir[4]/dir[3]; Kanal-Loader FUN_80022300 @0x800224b8/c8, Sub-Loads
                     * 2 @0x80050f88/90, Walk @0x800512bc/c0, 6 @0x80051884/88, Turn @0x80051e9c).
                     * NICHT eb->anim (largest-bank dir[1]) — das war Marvins falsche Spawn-/
                     * Start-/Lauf-Animation (marvin_spawn_anim.md F1/F2, Savestate r10d0_walk1:
                     * Clip 1 = 16f / Clip 2 = 52f / Walk-Clip 5 = 30f aus Bank 1).
                     * SEIT 7b (2026-08-09) AUCH ELLIOT (das alte `!is_elliot` ist gestrichen):
                     * sein Run/Walk posiert das Original vom OWN-Kanal (f314 a0/a1 = +0x170/+0x174
                     * @0x80051714/18) = EM047 Bank 1 (15 Bones == ELLIOT.PLD-15-Part-Mesh; Blob 21
                     * @CDEMD0.EMS 0x336000, Clips {22,16,52,1,50,30}) — NICHT mehr W01-Clip 0
                     * (Leons Run). Mesh + TIM (2 tpages/2 CLUT-Zeilen, xshift/yshift) bleiben aus
                     * dem anim_select-0x47-Zweig = ELLIOT.PLD, nur skel/anim wechseln hier. */
                    re15_enemy_bank_t *lb = re15_enemy_find(npc->type);
                    if (lb && lb->own_ok && (int)npc->motion < lb->anim_own.clip_count) {
                        npc_skel = &lb->skel_own; npc_anim = &lb->anim_own;
                        av.clip_override = (int)npc->motion;
                    }
                } else if (npc->re2z_re15_pose) {
                    /* SITZ-IMPORT (10D0 sel 0x0e): der RE2-Flavor-Sitzer posiert die
                     * RE1.5-Aktions-Bank (Clip 0x2A Sitz / 0x29 Aufstehen — RE1.5-INIT
                     * @0x80100F64-FD4). Dieselbe Bank wie re15_actor_clip_len; das
                     * RE1.5-Skelett ist identitaets-gemappt -> KEIN Hybrid-Remap. */
                    const re15_emd_skeleton_t  *rs15 = re15_re2z_re15_pose_skel();
                    const re15_emd_animation_t *ra15 = re15_re2z_re15_pose_anim();
                    if (rs15 && ra15 && (int)npc->motion < ra15->clip_count) {
                        npc_skel = rs15; npc_anim = ra15;
                        av.clip_override = (int)npc->motion;
                        npc_remap = NULL;
                    }
                }

                /* DIAG (RE15_ENEMY_DIAG=1): one line per actor on first render — proves the
                 * generic enemy uses its OWN model (mesh != Leon's def_mesh) vs a Leon fallback,
                 * and shows motion/anim_frame so a "static Leon" report can be diagnosed headless. */
                if (getenv("RE15_ENEMY_DIAG")) {
                    static uint32_t s_diag_seen = 0;   /* bitmask by actor index, one-shot */
                    int ai = (int)(npc - g_actors);
                    if (ai >= 0 && ai < 32 && !(s_diag_seen & (1u << ai))) {
                        s_diag_seen |= (1u << ai);
                        const char *which = (av.mesh == &md1) ? "LEON-FALLBACK"
                                          : is_elliot ? "elliot"
                                          : (av.pc_tex_slot >= 0) ? "enemy-bank" : "other";
                        fprintf(stderr, "[enemy-diag] actor%d type=0x%02x model=%s slot=%d "
                                "motion=%u anim_frame=%u clips=%d\n",
                                ai, npc->type, which, av.pc_tex_slot,
                                (unsigned)npc->motion, (unsigned)npc->anim_frame,
                                npc_anim->clip_count);
                    }
                }

                /* Bind the NPC's TIM: a generic enemy bank's own slot (av.pc_tex_slot,
                 * which now covers the crows too), else Elliot (1) / Leon (0). */
                re15_render_pc_bind_tim_slot(av.pc_tex_slot >= 0 ? av.pc_tex_slot
                                             : (is_elliot ? 1 : 0));

                if (npc_anim->clip_count <= 0 || npc_skel->keyframe_count <= 0) continue;
                /* Platform fps policy: 30fps target = raw anim_frame; 60fps halves. */
                uint32_t ncur = (target_fps == 30)
                    ? (uint32_t)npc->anim_frame
                    : ((uint32_t)npc->anim_frame >> 1);
                int npc_kf = re15_compute_actor_kf(npc_anim, npc_skel, npc,
                                                   av.clip_override, ncur);
                re15_skel_pose_t npc_poses[RE15_EMD_MAX_BONES];
                g_anim_pose_actor = npc;   /* FRAC crossfade for this NPC/enemy body */
                if (re15_skel_compute_pose(npc_skel, npc_kf, npc_poses) != 0) continue;
                /* RE15_POSE_DUMP=1: per-frame RENDER-LEVEL pose log (verify the posed keyframes reach
                 * the screen — the walk-look investigations). b13 = the em10 reach forearm. */
                {
                    static FILE *s_pose_fp = NULL; static int s_pose_tried = 0;
                    if (!s_pose_tried) { s_pose_tried = 1;
                        const char *pd = getenv("RE15_POSE_DUMP");
                        if (pd && *pd) s_pose_fp = fopen(pd, "w");
                    }
                    if (s_pose_fp) {
                        fprintf(s_pose_fp, "[pose] F%u slot%d ss1=%d mo=%d af=%d frac=%d kf=%d "
                                "b9(%d,%d,%d) b13(%d,%d,%d)\n",
                                (unsigned)g_engine.frame_count, (int)(npc - g_actors),
                                npc->sub_state_1, npc->motion, npc->anim_frame, npc->anim_frac, npc_kf,
                                (int)npc_poses[9].trans[0], (int)npc_poses[9].trans[1], (int)npc_poses[9].trans[2],
                                (int)npc_poses[13].trans[0], (int)npc_poses[13].trans[1], (int)npc_poses[13].trans[2]);
                        fflush(s_pose_fp);
                    }
                }

                int32_t nfs = re15_sin_q12((int)npc->rot_y);
                int32_t nfc = re15_cos_q12((int)npc->rot_y);
                int32_t nyaw[9] = { nfc, 0, nfs, 0, 0x1000, 0, -nfs, 0, nfc };
                /* ENTITY-RENDER-SCALE +0x166 (Gate Flag 0x800): das Original skaliert die
                 * Root-Matrix VOR der Bone-Schleife uniform per ScaleMatrix (FUN_8001e8c8
                 * @0x8001e904 andi 0x800; lh +0x166 @0x8001e91c/28/38; jal ScaleMatrix
                 * @0x8001e940) — Bone-Offsets UND Vertices skalieren mit, die Weltposition
                 * nicht. Gorilla-Boss 0x27 = 0x1b33 (1.7x, Init @0x80117148-4c). Port:
                 * render_scale_q12 != 0 == Flag gesetzt. */
                if (npc->render_scale_q12) {
                    int k;
                    for (k = 0; k < 9; k++)
                        nyaw[k] = (nyaw[k] * (int32_t)npc->render_scale_q12) >> 12;
                }

                /* Canonical (2026-05-29): eval_pos = the NPC's own world
                 * position, matching PSX FUN_80053fc0 / FUN_80039ca0
                 * (puVar3+0x14). Safe: the teleport-hide cull above already
                 * dropped any off-stage NPC, so we only light visible actors. */
                /* CANONICAL per-bone (2026-06-02): world-space ctx built once
                 * (rot=NULL); per-bone fold below. Was body-only (nyaw, approx). */
                re15_actor_lightctx_t lctx_npc, lctx_npc_world;
                int npc_lit = (g_re15_room_lights_ok &&
                               g_re15_active_cut >= 0 &&
                               g_re15_active_cut < g_re15_room_lights.cut_count);
                if (npc_lit) {
                    int32_t npc_pos_w[3] = {
                        (int32_t)npc->x,
                        (int32_t)npc->y,
                        (int32_t)npc->z
                    };
                    re15_light_setup_actor(
                        &g_re15_room_lights.cuts[g_re15_active_cut],
                        npc_pos_w, NULL, &lctx_npc_world);
                } else {
                    memset(&lctx_npc, 0, sizeof(lctx_npc));
                }

                /* ⛔ PART-ZEICHENMASKE (Nutzer-Report "schwarze Dreiecke ueber dem Feuer",
                 * zweimal gemeldet). Das Original entscheidet PRO KOERPERTEIL ueber Bit 0 der
                 * Part-Flags (Part-Array entity+0x188), und der Zeichner steigt ohne dieses
                 * Bit VOR jeder Mesh-Ausgabe aus:
                 *     8001ecc4: andi v0,v1,0x1
                 *     8001ecc8: beq  v0,zero,0x8001ee48     ; -> Epilog, kein Mesh
                 * Der Binder setzt es normal auf 1 (`ori v0,zero,0x1` @0x8001e74c /
                 * `sw v0,0(s2)` @0x8001e758); der Typ-0x26-INIT LOESCHT es
                 * (`lw a0,392(v0)` / `and` mit -2 / `sw v0,0(a0)` @0x801165d0-e4).
                 * Die sieben ROOM1090-Feuer-Emitter sind damit UNSICHTBARE Traeger fuer ihre
                 * Flammen-Effekte. Ihr Mesh ist ein einziges Dreieck (EM26: 1 Bone, 3 Verts,
                 * 1 Face) — und genau das hat der Port gezeichnet.
                 * Die Maske sitzt hier, NACH Schatten und Lichtkontext: im Original haengen
                 * Schatten (@0x80116740) und Kollision (@0x80116368) an eigenen Aufrufen und
                 * bleiben unberuehrt. */
                if (npc->no_draw) continue;

                int npc_bones = npc_skel->bone_count;
                /* Ohne Remap ist Mesh-Index == Bone-Index (emd_common.c:190), also darf die
                 * Schleife nie ueber die Mesh-Zahl hinaus. MIT Remap (WELLE-G-Hybrid) gilt das
                 * NICHT: der RE2-Hund hat 17 Bones gegen 15 RE1.5-Meshes, und die Klammer haette
                 * genau die beiden LETZTEN Slots (15/16 = linkes Hinterbein) verschluckt — die
                 * -1-Eintraege in mesh_remap uebernehmen die Aufgabe pro Slot. */
                if (!npc_remap && npc_bones > npc_md1->mesh_count) npc_bones = npc_md1->mesh_count;
                if (npc_bones > RE15_EMD_MAX_BONES) npc_bones = RE15_EMD_MAX_BONES;
                /* ---- RE2-GORE: Part-Sichtbarkeit + Part-Tinte (Zwilling FUN_80027160) ----
                 * gore_on == 0 im GESAMTEN RE1.5-Pfad (re15_re2z_gore_active gated auf
                 * Flavor + RE2-Zombie-Typ + INIT-Seed) — dann bleibt der Renderpfad Byte
                 * fuer Byte der alte. Nur wenn der RE2-Zombie-Brain seinen Modellblock
                 * geseedet hat, traegt gore_draw[] das Bit-0-DRAW-ENABLE (@0x8002737C /
                 * @0x800273C4, flacher Walk mit Eltern-Kaskade @0x80027480-94) und
                 * gore_tint[] das Farbwort +0x70 (@0x80027900), das NCCT (@0x80027D10)
                 * mit dem Beleuchtungsergebnis MULTIPLIZIERT. */
                uint8_t  gore_draw[RE15_EMD_MAX_BONES];
                uint32_t gore_tint[RE15_EMD_MAX_BONES];
                uint8_t  gore_mesh[RE15_EMD_MAX_BONES];
                int gore_on = re15_re2z_gore_resolve(npc, npc_skel->bone_parent, npc_bones,
                                                     gore_draw, gore_tint, gore_mesh);
                /* NCCT-Modulation des Farbworts +0x70: das Wort geht als CVECTOR ins
                 * GTE-RGB-Register (`sw a2,0x10(sp)` @0x80027C08 — LOW BYTE = R, Byte 3
                 * ist der GPU-Primitiv-Code `sb a3,0x13(sp)` @0x80027C18 — dann
                 * `ldrgb v0` @0x80027C2C) und NCCT (@0x80027D10) rechnet
                 * out = RGB_reg * (BackColor + LCM*LLM*N). Die Tinte ERSETZT die
                 * Vertexfarbe also nicht, sie SKALIERT das Beleuchtungsergebnis; neutral
                 * ist 0x808080 (FUN_80028368.c:55 `puVar8[-9] = 0x808080`). Die
                 * Vertexfarben des Ports stehen im selben PSX-Primitiv-Raum
                 * (render_pc.c psx_prim_to_sdl_vert: "final = (tex x prim) / 0x80"),
                 * also ist ein Modulationsschritt genau `prim * tint / 0x80`. */
#define RE2_GORE_TINT(r_, g_, b_) do { if (gore_on) {                                  \
        uint32_t _t = gore_tint[nbi];                                                  \
        int _r = ((int)(r_) * (int)( _t         & 0xFFu)) >> 7;                        \
        int _g = ((int)(g_) * (int)((_t >>  8)  & 0xFFu)) >> 7;                        \
        int _b = ((int)(b_) * (int)((_t >> 16)  & 0xFFu)) >> 7;                        \
        (r_) = (uint8_t)(_r > 255 ? 255 : _r);                                         \
        (g_) = (uint8_t)(_g > 255 ? 255 : _g);                                         \
        (b_) = (uint8_t)(_b > 255 ? 255 : _b); } } while (0)
                for (int nbi = 0; nbi < npc_bones; nbi++) {
                    /* Bit 0 klar -> dieser Part wird nicht gezeichnet. Das `continue` ist
                     * flach wie im Original (`_addiu s2,s2,0xac` im Delay-Slot @0x800273A4 /
                     * @0x800273F8 / @0x8010740C) — die Kinder verschwinden NICHT automatisch
                     * mit, dafuer sorgt allein die Kaskade in re15_re2z_gore_resolve. */
                    if (gore_on && !gore_draw[nbi]) continue;
                    const re15_skel_pose_t *np = &npc_poses[nbi];
                    int32_t nyawed_rot[9];
                    for (int r = 0; r < 3; r++) {
                        for (int c = 0; c < 3; c++) {
                            int64_t s = 0;
                            for (int k = 0; k < 3; k++)
                                s += (int64_t)nyaw[r*3+k] * (int64_t)np->rot[k*3+c];
                            nyawed_rot[r*3+c] = (int32_t)(s >> 12);
                        }
                    }
                    int32_t nyawed_trans[3];
                    for (int r = 0; r < 3; r++) {
                        int64_t s = 0;
                        for (int k = 0; k < 3; k++)
                            s += (int64_t)nyaw[r*3+k] * (int64_t)np->trans[k];
                        nyawed_trans[r] = (int32_t)(s >> 12);
                    }
                    int32_t nbone_world_trans[3] = {
                        nyawed_trans[0] + npc->x,
                        nyawed_trans[1] + npc->y,
                        nyawed_trans[2] + npc->z,
                    };
                    /* ---- RE2-GORE: DAS FREIFLIEGENDE TEIL --------------------------------
                     * Traegt der Part Bit 0x40, ueberspringt der Original-Zeichner die
                     * Eltern-Verkettung und nimmt die Matrix aus dem Part-Record selbst
                     * (`andi v0,s3,0x40` @0x80027498 / `bne v0,zero,0x800275E4` @0x8002749C);
                     * die Flugphysik laeuft im selben Zug (`jal 0x80028AD8` @0x800276A0 /
                     * `jal 0x80028DAC` @0x80027B98) — im Original haengt sie also am ZEICHNEN,
                     * genau wie hier. Der Aufruf schreibt Rotation UND Translation um; im
                     * RE1.5-Flavor liefert er immer 0 und laesst beide unberuehrt. */
                    if (gore_on)
                        (void)re15_re2z_gore_part_matrix(npc, nbi,
                                                         (uint32_t)g_engine.frame_count,
                                                         nyawed_rot, nbone_world_trans);
                    /* CANONICAL per-bone light fold (2026-06-02). */
                    if (npc_lit)
                        re15_light_ctx_rotate_for_bone(&lctx_npc_world, nyawed_rot,
                                                       &lctx_npc);
                    int32_t ncomb_rot[9];
                    int32_t ncomb_trans[3];
                    re15_camera_compose_view_bone(&cam_view, nyawed_rot, nbone_world_trans,
                                                   ncomb_rot, ncomb_trans);

                    int32_t nbone_m[9], nbone_t[3];   /* Q12 view×bone (byte-true integer RTPS) */
                    for (int i = 0; i < 9; i++) nbone_m[i] = ncomb_rot[i];
                    for (int i = 0; i < 3; i++) nbone_t[i] = ncomb_trans[i];

                    /* AK-round F950 dump (2026-05-26): Elliot's root bone view-
                     * space + projected screen for cut 2 framing diagnosis. */
                    if (is_elliot && nbi == 0 &&
                        g_engine.frame_count == 950) {
                        float _proj0 = (cam_view.fov_screen_dist > 0 && nbone_t[2] > 1.0f)
                            ? (float)cam_view.fov_screen_dist / nbone_t[2]
                            : 0.0f;
                        fprintf(stderr,
                            "[F950-elliot-root] world=(%d,%d,%d) view=(%d,%d,%d) "
                            "sd=%d screen=(%.1f,%.1f) cam_pos=(%d,%d,%d) cam_tgt=(%d,%d,%d) fov=%u cut=%d\n",
                            (int)npc->x, (int)npc->y, (int)npc->z,
                            (int)nbone_t[0], (int)nbone_t[1], (int)nbone_t[2],
                            (int)cam_view.fov_screen_dist,
                            (float)cx + nbone_t[0] * _proj0,
                            (float)cy + nbone_t[1] * _proj0,
                            (int)active_cuts[active_cut_idx].pos_x,
                            (int)active_cuts[active_cut_idx].pos_y,
                            (int)active_cuts[active_cut_idx].pos_z,
                            (int)active_cuts[active_cut_idx].target_x,
                            (int)active_cuts[active_cut_idx].target_y,
                            (int)active_cuts[active_cut_idx].target_z,
                            (unsigned)active_cuts[active_cut_idx].fov,
                            active_cut_idx);
                    }

                    /* GEOMETRIE-QUELLE: normal das eigene Mesh; nach dem Zerleger-Stumpf-Tausch
                     * (`lw 2588/2592/2596/2600` -> `sw 8/12/16/20` @0x8010531C-50) liefert der
                     * RESERVE-Part 15 die vier Geometrie-Woerter des Oberschenkels. */
                    int nmi = nbi;
                    if (gore_on && gore_mesh[nbi] < (uint8_t)npc_md1->mesh_count)
                        nmi = (int)gore_mesh[nbi];
                    /* WELLE G — HYBRID-BANK ("RE2 AI" mit RE1.5-Modellen): der Bone-Slot ist ein
                     * RE2-Slot, das Mesh liegt im RE1.5-MD1 mit ANDERER Ordnung. -1 = der Slot
                     * hat kein RE1.5-Gegenstueck (RE2-Hund: die zwei zusaetzlichen Pfoten-
                     * Gelenke 7/10) -> nichts zeichnen. Tabellen/Messung: re2_ems.h.
                     * ⚠️ GORE-GRENZE: der Zerleger stempelt part_mesh = RESERVE-Mesh 15
                     * (@0x8010531C-50); das gibt es NUR im RE2-MD1 (17 Meshes) — der RE1.5-MD1
                     * hat 0..14. Die Klammer oben greift dann und der Oberschenkel bleibt INTAKT
                     * (Schienbein+Fuss fliegen weiter weg): im Hybrid gibt es KEINEN Stumpf.
                     * Selbst gemessen: KEIN RE1.5-Zombie-EMD (EM10/11/12/13/16/18) traegt ein
                     * 16. Mesh, aus dem sich ein Stumpf bauen liesse (mesh_count == bone_count). */
                    if (npc_remap) {
                        int r = (nmi >= 0 && nmi < RE15_EMD_MAX_BONES) ? (int)npc_remap[nmi] : -1;
                        if (r < 0) continue;
                        nmi = r;
                    }
                    if (nmi >= npc_md1->mesh_count) continue;
                    const re15_md1_mesh_t *nm = &npc_md1->meshes[nmi];
                    for (int ti = 0; ti < nm->triangle_count; ti++) {
                        const re15_md1_triangle_t *tri = &nm->triangles[ti];
                        if (tri->v0 >= (uint32_t)nm->tri_vertex_count) continue;
                        if (tri->v1 >= (uint32_t)nm->tri_vertex_count) continue;
                        if (tri->v2 >= (uint32_t)nm->tri_vertex_count) continue;
                        float ax[3], ay[3], wz[3];
                        const re15_md1_vertex_t *vp[3] = {
                            &nm->tri_vertices[tri->v0],
                            &nm->tri_vertices[tri->v1],
                            &nm->tri_vertices[tri->v2],
                        };
                        int ok = 1;
                        for (int v = 0; v < 3; v++) {
                            int32_t _x = vp[v]->x, _y = vp[v]->y, _z = vp[v]->z;
                            int32_t _vx = (int32_t)(((int64_t)_x*nbone_m[0] + (int64_t)_y*nbone_m[1] + (int64_t)_z*nbone_m[2]) >> 12) + nbone_t[0];
                            int32_t _vy = (int32_t)(((int64_t)_x*nbone_m[3] + (int64_t)_y*nbone_m[4] + (int64_t)_z*nbone_m[5]) >> 12) + nbone_t[1];
                            int32_t _vz = (int32_t)(((int64_t)_x*nbone_m[6] + (int64_t)_y*nbone_m[7] + (int64_t)_z*nbone_m[8]) >> 12) + nbone_t[2];
                            if (_vz < 64) { ok = 0; break; } /* BO: H28 near-plane (was 1.0) */
                            int32_t _ir1 = _vx > 0x7FFF ? 0x7FFF : (_vx < -0x8000 ? -0x8000 : _vx);
                            int32_t _ir2 = _vy > 0x7FFF ? 0x7FFF : (_vy < -0x8000 ? -0x8000 : _vy);
                            uint32_t _sz3 = _vz > 0xFFFF ? 0xFFFFu : (uint32_t)_vz;
                            uint32_t _n = re15_gte_divide((uint32_t)cam_view.fov_screen_dist, _sz3);
                            ax[v] = (float)(cx + (int)(((int64_t)_ir1 * (int64_t)_n) >> 16));
                            ay[v] = (float)(cy + (int)(((int64_t)_ir2 * (int64_t)_n) >> 16));
                            wz[v] = (float)_vz;
                        }
                        if (!ok) continue;
                        const re15_md1_tri_uv_t *uv = &nm->triangle_uvs[ti];
                        int page_off = (int)((uv->page & 0x000F) * 128);
                        /* G-round (2026-05-24): use real avg_z for depth sort
                         * (was hardcoded 1 = always-on-top which broke heli/
                         * NPC overlap — Leon-cubes always drew on top of heli
                         * even when behind it). */
                        int npc_avg_z = (int)((wz[0] + wz[1] + wz[2]) * (1.0f / 3.0f));
                        /* BF-round: per-vertex NCCT shading for NPC. */
                        uint8_t nr0, ng0, nb0, nr1, ng1, nb1, nr2, ng2, nb2;
                        if (nm->tri_normals &&
                            tri->n0 < (uint32_t)nm->tri_normal_count &&
                            tri->n1 < (uint32_t)nm->tri_normal_count &&
                            tri->n2 < (uint32_t)nm->tri_normal_count) {
                            const re15_md1_vertex_t *nn0 = &nm->tri_normals[tri->n0];
                            const re15_md1_vertex_t *nn1 = &nm->tri_normals[tri->n1];
                            const re15_md1_vertex_t *nn2 = &nm->tri_normals[tri->n2];
                            re15_light_shade_vertex(&lctx_npc, nn0->x, nn0->y, nn0->z, &nr0, &ng0, &nb0);
                            re15_light_shade_vertex(&lctx_npc, nn1->x, nn1->y, nn1->z, &nr1, &ng1, &nb1);
                            re15_light_shade_vertex(&lctx_npc, nn2->x, nn2->y, nn2->z, &nr2, &ng2, &nb2);
                        } else {
                            nr0 = nr1 = nr2 = g_re15_light_tint[0];
                            ng0 = ng1 = ng2 = g_re15_light_tint[1];
                            nb0 = nb1 = nb2 = g_re15_light_tint[2];
                        }
                        RE2_GORE_TINT(nr0, ng0, nb0);
                        RE2_GORE_TINT(nr1, ng1, nb1);
                        RE2_GORE_TINT(nr2, ng2, nb2);
                        re15_render_textured_tri_lit(
                            (int)ax[0], (int)ay[0], (int)uv->u0 + page_off, (int)uv->v0,
                            (int)ax[1], (int)ay[1], (int)uv->u1 + page_off, (int)uv->v1,
                            (int)ax[2], (int)ay[2], (int)uv->u2 + page_off, (int)uv->v2,
                            0, (int)uv->clut, npc_avg_z,
                            nr0, ng0, nb0, nr1, ng1, nb1, nr2, ng2, nb2);
                    }
                    /* quads similarly */
                    for (int qi = 0; qi < nm->quad_count; qi++) {
                        const re15_md1_quad_t *qd = &nm->quads[qi];
                        if (qd->v0 >= (uint32_t)nm->quad_vertex_count) continue;
                        if (qd->v1 >= (uint32_t)nm->quad_vertex_count) continue;
                        if (qd->v2 >= (uint32_t)nm->quad_vertex_count) continue;
                        if (qd->v3 >= (uint32_t)nm->quad_vertex_count) continue;
                        float ax[4], ay[4], wz[4];
                        const re15_md1_vertex_t *vp[4] = {
                            &nm->quad_vertices[qd->v0],
                            &nm->quad_vertices[qd->v1],
                            &nm->quad_vertices[qd->v2],
                            &nm->quad_vertices[qd->v3],
                        };
                        int ok = 1;
                        for (int v = 0; v < 4; v++) {
                            int32_t _x = vp[v]->x, _y = vp[v]->y, _z = vp[v]->z;
                            int32_t _vx = (int32_t)(((int64_t)_x*nbone_m[0] + (int64_t)_y*nbone_m[1] + (int64_t)_z*nbone_m[2]) >> 12) + nbone_t[0];
                            int32_t _vy = (int32_t)(((int64_t)_x*nbone_m[3] + (int64_t)_y*nbone_m[4] + (int64_t)_z*nbone_m[5]) >> 12) + nbone_t[1];
                            int32_t _vz = (int32_t)(((int64_t)_x*nbone_m[6] + (int64_t)_y*nbone_m[7] + (int64_t)_z*nbone_m[8]) >> 12) + nbone_t[2];
                            if (_vz < 64) { ok = 0; break; } /* BO: H28 near-plane (was 1.0) */
                            int32_t _ir1 = _vx > 0x7FFF ? 0x7FFF : (_vx < -0x8000 ? -0x8000 : _vx);
                            int32_t _ir2 = _vy > 0x7FFF ? 0x7FFF : (_vy < -0x8000 ? -0x8000 : _vy);
                            uint32_t _sz3 = _vz > 0xFFFF ? 0xFFFFu : (uint32_t)_vz;
                            uint32_t _n = re15_gte_divide((uint32_t)cam_view.fov_screen_dist, _sz3);
                            ax[v] = (float)(cx + (int)(((int64_t)_ir1 * (int64_t)_n) >> 16));
                            ay[v] = (float)(cy + (int)(((int64_t)_ir2 * (int64_t)_n) >> 16));
                            wz[v] = (float)_vz;
                        }
                        if (!ok) continue;
                        const re15_md1_quad_uv_t *uv = &nm->quad_uvs[qi];
                        int page_off = (int)((uv->page & 0x000F) * 128);
                        /* G-round: real depth for both quad-split triangles. */
                        int npc_qz1 = (int)((wz[0] + wz[1] + wz[3]) * (1.0f / 3.0f));
                        int npc_qz2 = (int)((wz[0] + wz[3] + wz[2]) * (1.0f / 3.0f));
                        /* BF-round: per-vertex shading for NPC quad. */
                        uint8_t nqr0, nqg0, nqb0, nqr1, nqg1, nqb1;
                        uint8_t nqr2, nqg2, nqb2, nqr3, nqg3, nqb3;
                        if (nm->quad_normals &&
                            qd->n0 < (uint32_t)nm->quad_normal_count &&
                            qd->n1 < (uint32_t)nm->quad_normal_count &&
                            qd->n2 < (uint32_t)nm->quad_normal_count &&
                            qd->n3 < (uint32_t)nm->quad_normal_count) {
                            const re15_md1_vertex_t *qn0 = &nm->quad_normals[qd->n0];
                            const re15_md1_vertex_t *qn1 = &nm->quad_normals[qd->n1];
                            const re15_md1_vertex_t *qn2 = &nm->quad_normals[qd->n2];
                            const re15_md1_vertex_t *qn3 = &nm->quad_normals[qd->n3];
                            re15_light_shade_vertex(&lctx_npc, qn0->x, qn0->y, qn0->z, &nqr0, &nqg0, &nqb0);
                            re15_light_shade_vertex(&lctx_npc, qn1->x, qn1->y, qn1->z, &nqr1, &nqg1, &nqb1);
                            re15_light_shade_vertex(&lctx_npc, qn2->x, qn2->y, qn2->z, &nqr2, &nqg2, &nqb2);
                            re15_light_shade_vertex(&lctx_npc, qn3->x, qn3->y, qn3->z, &nqr3, &nqg3, &nqb3);
                        } else {
                            nqr0=nqr1=nqr2=nqr3=g_re15_light_tint[0];
                            nqg0=nqg1=nqg2=nqg3=g_re15_light_tint[1];
                            nqb0=nqb1=nqb2=nqb3=g_re15_light_tint[2];
                        }
                        RE2_GORE_TINT(nqr0, nqg0, nqb0);
                        RE2_GORE_TINT(nqr1, nqg1, nqb1);
                        RE2_GORE_TINT(nqr2, nqg2, nqb2);
                        RE2_GORE_TINT(nqr3, nqg3, nqb3);
                        re15_render_textured_tri_lit(
                            (int)ax[0], (int)ay[0], (int)uv->u0 + page_off, (int)uv->v0,
                            (int)ax[1], (int)ay[1], (int)uv->u1 + page_off, (int)uv->v1,
                            (int)ax[3], (int)ay[3], (int)uv->u3 + page_off, (int)uv->v3,
                            0, (int)uv->clut, npc_qz1,
                            nqr0, nqg0, nqb0, nqr1, nqg1, nqb1, nqr3, nqg3, nqb3);
                        re15_render_textured_tri_lit(
                            (int)ax[0], (int)ay[0], (int)uv->u0 + page_off, (int)uv->v0,
                            (int)ax[3], (int)ay[3], (int)uv->u3 + page_off, (int)uv->v3,
                            (int)ax[2], (int)ay[2], (int)uv->u2 + page_off, (int)uv->v2,
                            0, (int)uv->clut, npc_qz2,
                            nqr0, nqg0, nqb0, nqr3, nqg3, nqb3, nqr2, nqg2, nqb2);
                    }
                }
#undef RE2_GORE_TINT
            }

            /* I-round disable (2026-05-24): NPC name-label overlay
             * removed — was drawing "Elliot(x,z)" text on top of him. */

            /* Obj_model_set prop rendering: render each placed prop with its
             * OWN MD1 + TIM (slots 4..9 for obj_id 0..5). Helicopter body
             * (0x02), rotor (0x03), light disc (0x04), pilot (0x05), and
             * the helipad fixtures (0x00, 0x01) all get authentic textures.   */
            /* ⛔ NICHT 16: das Original klemmt den Objekt-Durchlauf nirgends, es laeuft
             * bis nOmodel (FUN_800436a8 @0x80043758/@0x800437ac) ueber einen per obj_id
             * indizierten Pool (@0x8004093c-58, Schrittweite 148). ROOM1190/1191 haben 17.
             * Mit dem alten Literal waere Prop 16 — die POLIZEIWESTE — zwar geparst, aber
             * nie GEZEICHNET worden. */
            for (int pi = 0; pi < g_scd.prop_count && pi < RE15_SCD_MAX_PROPS; pi++) {
                if (!g_scd.props[pi].active) continue;
                /* Byte-true per-prop cull, SHARED with the PSX port (re15_prop_culled
                 * in re15_aot.h). Every room1170 prop is type 0 (real type byte pc[2],
                 * not the old pc[4]=BAND read) → whole-object FUN_80014368 region-quad
                 * cull, with a sink-gate (x<-25000) fallback for region-less cinematic
                 * cuts. This PC loop already renders all active props (no type gate),
                 * so only the cull rule changed; both ports now cull identically. */
                if (re15_prop_culled((int)g_scd.props[pi].obj_type,
                                     g_scd.props[pi].x, g_scd.props[pi].z,
                                     cam_has_region, cam_region_xs, cam_region_zs))
                    continue;
                int oid = (int)g_scd.props[pi].obj_id;
#ifdef RE15_PROP_TRACE
                { static int pn=0; if((pn++%60)==0)
                    fprintf(stderr,"[PROP] pi=%d oid=%d type=%d @(%d,%d,%d)  PL@(%d,%d)\n",
                        pi, oid, (int)g_scd.props[pi].obj_type, g_scd.props[pi].x, g_scd.props[pi].y, g_scd.props[pi].z,
                        g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z); }
#endif
                const re15_md1_t *prop_md1 = NULL;
                int prop_md1_ok = 0;
                /* Kein 6er-Cap (ROOM11F0: 12 Props, Schalter obj 0x06..0x0B waren
                 * unsichtbar). Original rendert jeden Pool-Eintrag bis nOmodel
                 * (FUN_800436a8 @0x80043758/0x800437ac; Draw FUN_8002c18c pro Objekt). */
                if (oid >= 0 && oid < RE15_RDT_MAX_PROPS && s_room_prop_ok[oid]) {
                    prop_md1 = &s_room_prop_md1[oid];
                    prop_md1_ok = 1;
                    re15_render_pc_bind_tim_slot(RE15_TIM_SLOT_PROP(oid));
                }

                /* Render prop's MD1 mesh as TEXTURED triangles. */
                if (prop_md1_ok) {
                    int32_t prop_x = g_scd.props[pi].x;
                    /* Typ-4-Props (schiebbare Kisten) zeichnet das Original 900 hoeher —
                     * byte-true FUN_8002c18c @0x8002c23c/@0x8002c24c, siehe
                     * re15_prop_render_y() in re15_aot.h. Nur die DARSTELLUNG; die
                     * Kollisions-/Push-Position g_scd.props[pi].y bleibt roh. */
                    int32_t prop_y = re15_prop_render_y((int)g_scd.props[pi].obj_type,
                                                        g_scd.props[pi].y);
                    int32_t prop_z = g_scd.props[pi].z;
                    int16_t prop_rx = g_scd.props[pi].rot_x;
                    int16_t prop_ry = g_scd.props[pi].rot_y;
                    int16_t prop_rz = g_scd.props[pi].rot_z;

                    /* BM-round 2026-05-29: REMOVED the BA-round whole-prop
                     * view_z>32000 far-clip. It was added on a FALSE premise —
                     * PSX has NO far-clip in its render loop (FUN_80016b54 only
                     * does the behind-camera near-clip OTZ<0). That cull made the
                     * cut-6 sky-view helicopter (props 2/3, flown far by sub12/13)
                     * VANISH mid-scene once its view_z crossed 32000, whereas PSX
                     * keeps it visible the whole scene. The per-vertex near-clip
                     * (_vz<64) below already guards div-by-zero; distant props
                     * shrink to sub-pixel naturally, matching PSX (the teleport-
                     * hidden fixtures become a ~2-3px dot, as on PSX). */
                    /* I3-round diag: log each prop's render pos once. */
                    static int s_prop_logged[RE15_SCD_MAX_PROPS] = {0};   /* MUSS mit der
                                                                           * Schleife oben
                                                                           * wachsen, sonst
                                                                           * Schreiber hinter
                                                                           * das Feld */
                    if (!s_prop_logged[pi]) {
                        s_prop_logged[pi] = 1;
                        fprintf(stderr, "[prop-render] pi=%d oid=0x%02X pos=(%d,%d,%d) rot=(%d,%d,%d) meshes=%d\n",
                                pi, oid, prop_x, prop_y, prop_z,
                                prop_rx, prop_ry, prop_rz, prop_md1->mesh_count);
                    }
                    /* Build full Euler rotation matrix (RE2 stock RotMatrix
                     * convention = Ry * Rx * Rz, per skeleton_common.c
                     * mat3_from_euler). Without rot_x the tail rotor (Obj
                     * 0x04, rot_x=1024 = 90°) would render flat-horizontal
                     * instead of vertical-on-the-side. Local names rsx/rcx/
                     * etc. avoid shadowing the outer-scope `cx,cy` which
                     * are the screen center used by projection below. */
                    int32_t rsx = re15_sin_q12((int)prop_rx), rcx = re15_cos_q12((int)prop_rx);
                    int32_t rsy = re15_sin_q12((int)prop_ry), rcy = re15_cos_q12((int)prop_ry);
                    int32_t rsz = re15_sin_q12((int)prop_rz), rcz = re15_cos_q12((int)prop_rz);
                    #define Q12_MUL(a,b)   ((int32_t)(((int64_t)(a) * (int64_t)(b)) >> 12))
                    /* M = Ry * Rx * Rz (PSX YXZ Euler). Direct expansion: */
                    int32_t prot_q12[9];
                    /* Row 0 */
                    prot_q12[0] = Q12_MUL(rcz, rcy);
                    prot_q12[1] = -Q12_MUL(rsz, rcy);
                    prot_q12[2] = rsy;
                    /* Row 1 — 2-term int64-accumulated to mirror M-round Q12 fix. */
                    {
                        int64_t t1 = ((int64_t)rsz * (int64_t)rcx) << 12;
                        int64_t t2 = (int64_t)rcz * (int64_t)rsy * (int64_t)rsx;
                        prot_q12[3] = (int32_t)((t1 + t2) >> 24);
                    }
                    {
                        int64_t t1 = ((int64_t)rcz * (int64_t)rcx) << 12;
                        int64_t t2 = (int64_t)rsz * (int64_t)rsy * (int64_t)rsx;
                        prot_q12[4] = (int32_t)((t1 - t2) >> 24);
                    }
                    prot_q12[5] = -Q12_MUL(rcy, rsx);
                    /* Row 2 */
                    {
                        int64_t t1 = ((int64_t)rsz * (int64_t)rsx) << 12;
                        int64_t t2 = (int64_t)rcz * (int64_t)rsy * (int64_t)rcx;
                        prot_q12[6] = (int32_t)((t1 - t2) >> 24);
                    }
                    {
                        int64_t t1 = ((int64_t)rcz * (int64_t)rsx) << 12;
                        int64_t t2 = (int64_t)rsz * (int64_t)rsy * (int64_t)rcx;
                        prot_q12[7] = (int32_t)((t1 + t2) >> 24);
                    }
                    prot_q12[8] = Q12_MUL(rcy, rcx);
                    #undef Q12_MUL

                    /* CANONICAL per-bone (2026-06-02): a rigid prop is ONE "bone"
                     * with world rotation prot_q12. Build the world-space ctx
                     * (rot=NULL) then fold prot_q12 in once (L_world · N_world).
                     * Was body-only (prot_q12 to setup_actor) — for a single rigid
                     * rotation that was already equivalent, but we now route it
                     * through the same canonical helper for consistency. */
                    re15_actor_lightctx_t lctx_prop, lctx_prop_world;
                    if (g_re15_room_lights_ok &&
                        g_re15_active_cut >= 0 &&
                        g_re15_active_cut < g_re15_room_lights.cut_count) {
                        int32_t prop_pos_w[3] = { prop_x, prop_y, prop_z };
                        re15_light_setup_actor(
                            &g_re15_room_lights.cuts[g_re15_active_cut],
                            prop_pos_w, NULL, &lctx_prop_world);
                        re15_light_ctx_rotate_for_bone(&lctx_prop_world, prot_q12,
                                                       &lctx_prop);
                    } else {
                        memset(&lctx_prop, 0, sizeof(lctx_prop));
                    }

                    for (int hbi = 0; hbi < prop_md1->mesh_count; hbi++) {
                        const re15_md1_mesh_t *hm = &prop_md1->meshes[hbi];
                        int32_t world_trans[3] = { prop_x, prop_y, prop_z };
                        int32_t comb_rot[9];
                        int32_t comb_trans[3];
                        re15_camera_compose_view_bone(&cam_view, prot_q12, world_trans,
                                                       comb_rot, comb_trans);
                        int32_t bm[9], bt[3];   /* Q12 view×bone — byte-true integer RTPS (was float) */
                        for (int i = 0; i < 9; i++) bm[i] = comb_rot[i];
                        for (int i = 0; i < 3; i++) bt[i] = comb_trans[i];

                        /* Textured rendering using bound prop TIM slot. */
                        for (int ti = 0; ti < hm->triangle_count; ti++) {
                            const re15_md1_triangle_t *tr = &hm->triangles[ti];
                            if (tr->v0 >= (uint32_t)hm->tri_vertex_count) continue;
                            if (tr->v1 >= (uint32_t)hm->tri_vertex_count) continue;
                            if (tr->v2 >= (uint32_t)hm->tri_vertex_count) continue;
                            int ax[3], ay[3], wz_avg = 0;
                            int ok = 1;
                            const re15_md1_vertex_t *vp[3] = {
                                &hm->tri_vertices[tr->v0],
                                &hm->tri_vertices[tr->v1],
                                &hm->tri_vertices[tr->v2],
                            };
                            for (int v = 0; v < 3; v++) {
                                int32_t _x = vp[v]->x, _y = vp[v]->y, _z = vp[v]->z;
                                int32_t _vx = (int32_t)(((int64_t)_x*bm[0] + (int64_t)_y*bm[1] + (int64_t)_z*bm[2]) >> 12) + bt[0];
                                int32_t _vy = (int32_t)(((int64_t)_x*bm[3] + (int64_t)_y*bm[4] + (int64_t)_z*bm[5]) >> 12) + bt[1];
                                int32_t _vz = (int32_t)(((int64_t)_x*bm[6] + (int64_t)_y*bm[7] + (int64_t)_z*bm[8]) >> 12) + bt[2];
                                if (_vz < 64) { ok = 0; break; }  /* H28 near-clip */
                                int32_t _ir1 = _vx>0x7FFF?0x7FFF:(_vx<-0x8000?-0x8000:_vx);
                                int32_t _ir2 = _vy>0x7FFF?0x7FFF:(_vy<-0x8000?-0x8000:_vy);
                                uint32_t _sz3 = (uint32_t)(_vz>0xFFFF?0xFFFF:_vz);
                                uint32_t _n = re15_gte_divide((uint32_t)cam_view.fov_screen_dist, _sz3);
                                ax[v] = cx + (int)(((int64_t)_ir1*(int64_t)_n)>>16);
                                ay[v] = cy + (int)(((int64_t)_ir2*(int64_t)_n)>>16);
                                wz_avg += (int)_vz;
                            }
                            if (!ok) continue;
                            wz_avg /= 3;
                            int wz_for_sort = wz_avg;
                            const re15_md1_tri_uv_t *uv = &hm->triangle_uvs[ti];
                            int page_off = (int)((uv->page & 0x000F) * 128);
                            /* BF-round: per-vertex shading for prop tri. */
                            uint8_t pr0, pg0, pb0, pr1, pg1, pb1, pr2, pg2, pb2;
                            if (hm->tri_normals &&
                                tr->n0 < (uint32_t)hm->tri_normal_count &&
                                tr->n1 < (uint32_t)hm->tri_normal_count &&
                                tr->n2 < (uint32_t)hm->tri_normal_count) {
                                const re15_md1_vertex_t *pn0 = &hm->tri_normals[tr->n0];
                                const re15_md1_vertex_t *pn1 = &hm->tri_normals[tr->n1];
                                const re15_md1_vertex_t *pn2 = &hm->tri_normals[tr->n2];
                                re15_light_shade_vertex(&lctx_prop, pn0->x, pn0->y, pn0->z, &pr0, &pg0, &pb0);
                                re15_light_shade_vertex(&lctx_prop, pn1->x, pn1->y, pn1->z, &pr1, &pg1, &pb1);
                                re15_light_shade_vertex(&lctx_prop, pn2->x, pn2->y, pn2->z, &pr2, &pg2, &pb2);
                            } else {
                                pr0 = pr1 = pr2 = g_re15_light_tint[0];
                                pg0 = pg1 = pg2 = g_re15_light_tint[1];
                                pb0 = pb1 = pb2 = g_re15_light_tint[2];
                            }
                            re15_render_textured_tri_lit(
                                ax[0], ay[0], (int)uv->u0 + page_off, (int)uv->v0,
                                ax[1], ay[1], (int)uv->u1 + page_off, (int)uv->v1,
                                ax[2], ay[2], (int)uv->u2 + page_off, (int)uv->v2,
                                0, (int)uv->clut, wz_for_sort,
                                pr0, pg0, pb0, pr1, pg1, pb1, pr2, pg2, pb2);
                        }
                        for (int qi = 0; qi < hm->quad_count; qi++) {
                            const re15_md1_quad_t *qd = &hm->quads[qi];
                            if (qd->v0 >= (uint32_t)hm->quad_vertex_count) continue;
                            if (qd->v1 >= (uint32_t)hm->quad_vertex_count) continue;
                            if (qd->v2 >= (uint32_t)hm->quad_vertex_count) continue;
                            if (qd->v3 >= (uint32_t)hm->quad_vertex_count) continue;
                            int ax[4], ay[4], wz_avg = 0;
                            const re15_md1_vertex_t *vp[4] = {
                                &hm->quad_vertices[qd->v0],
                                &hm->quad_vertices[qd->v1],
                                &hm->quad_vertices[qd->v2],
                                &hm->quad_vertices[qd->v3],
                            };
                            int ok = 1;
                            for (int v = 0; v < 4; v++) {
                                int32_t _x = vp[v]->x, _y = vp[v]->y, _z = vp[v]->z;
                                int32_t _vx = (int32_t)(((int64_t)_x*bm[0] + (int64_t)_y*bm[1] + (int64_t)_z*bm[2]) >> 12) + bt[0];
                                int32_t _vy = (int32_t)(((int64_t)_x*bm[3] + (int64_t)_y*bm[4] + (int64_t)_z*bm[5]) >> 12) + bt[1];
                                int32_t _vz = (int32_t)(((int64_t)_x*bm[6] + (int64_t)_y*bm[7] + (int64_t)_z*bm[8]) >> 12) + bt[2];
                                if (_vz < 64) { ok = 0; break; }  /* H28 near-clip */
                                int32_t _ir1 = _vx>0x7FFF?0x7FFF:(_vx<-0x8000?-0x8000:_vx);
                                int32_t _ir2 = _vy>0x7FFF?0x7FFF:(_vy<-0x8000?-0x8000:_vy);
                                uint32_t _sz3 = (uint32_t)(_vz>0xFFFF?0xFFFF:_vz);
                                uint32_t _n = re15_gte_divide((uint32_t)cam_view.fov_screen_dist, _sz3);
                                ax[v] = cx + (int)(((int64_t)_ir1*(int64_t)_n)>>16);
                                ay[v] = cy + (int)(((int64_t)_ir2*(int64_t)_n)>>16);
                                wz_avg += (int)_vz;
                            }
                            if (!ok) continue;
                            wz_avg /= 4;
                            const re15_md1_quad_uv_t *uv = &hm->quad_uvs[qi];
                            int page_off = (int)((uv->page & 0x000F) * 128);
                            /* BF-round: per-vertex shading for prop quad. */
                            uint8_t pqr0, pqg0, pqb0, pqr1, pqg1, pqb1;
                            uint8_t pqr2, pqg2, pqb2, pqr3, pqg3, pqb3;
                            if (hm->quad_normals &&
                                qd->n0 < (uint32_t)hm->quad_normal_count &&
                                qd->n1 < (uint32_t)hm->quad_normal_count &&
                                qd->n2 < (uint32_t)hm->quad_normal_count &&
                                qd->n3 < (uint32_t)hm->quad_normal_count) {
                                const re15_md1_vertex_t *qn0 = &hm->quad_normals[qd->n0];
                                const re15_md1_vertex_t *qn1 = &hm->quad_normals[qd->n1];
                                const re15_md1_vertex_t *qn2 = &hm->quad_normals[qd->n2];
                                const re15_md1_vertex_t *qn3 = &hm->quad_normals[qd->n3];
                                re15_light_shade_vertex(&lctx_prop, qn0->x, qn0->y, qn0->z, &pqr0, &pqg0, &pqb0);
                                re15_light_shade_vertex(&lctx_prop, qn1->x, qn1->y, qn1->z, &pqr1, &pqg1, &pqb1);
                                re15_light_shade_vertex(&lctx_prop, qn2->x, qn2->y, qn2->z, &pqr2, &pqg2, &pqb2);
                                re15_light_shade_vertex(&lctx_prop, qn3->x, qn3->y, qn3->z, &pqr3, &pqg3, &pqb3);
                            } else {
                                pqr0=pqr1=pqr2=pqr3=g_re15_light_tint[0];
                                pqg0=pqg1=pqg2=pqg3=g_re15_light_tint[1];
                                pqb0=pqb1=pqb2=pqb3=g_re15_light_tint[2];
                            }
                            re15_render_textured_tri_lit(
                                ax[0], ay[0], (int)uv->u0 + page_off, (int)uv->v0,
                                ax[1], ay[1], (int)uv->u1 + page_off, (int)uv->v1,
                                ax[3], ay[3], (int)uv->u3 + page_off, (int)uv->v3,
                                0, (int)uv->clut, wz_avg,
                                pqr0, pqg0, pqb0, pqr1, pqg1, pqb1, pqr3, pqg3, pqb3);
                            re15_render_textured_tri_lit(
                                ax[0], ay[0], (int)uv->u0 + page_off, (int)uv->v0,
                                ax[3], ay[3], (int)uv->u3 + page_off, (int)uv->v3,
                                ax[2], ay[2], (int)uv->u2 + page_off, (int)uv->v2,
                                0, (int)uv->clut, wz_avg,
                                pqr0, pqg0, pqb0, pqr3, pqg3, pqb3, pqr2, pqg2, pqb2);
                        }
                    }
                    continue;
                }

                /* Other props (helicopter parts, lights) — small markers. NOTE: the mustard tile
                 * below is a PORT PLACEHOLDER for props with no MD1 mesh, not a byte-true render;
                 * only its centre projection is made uniform with the integer GTE RTPS. */
                int32_t fx = g_scd.props[pi].x, fy = g_scd.props[pi].y, fz = g_scd.props[pi].z;
                int32_t vx = (int32_t)(((int64_t)cam_view.rot[0]*fx + (int64_t)cam_view.rot[1]*fy + (int64_t)cam_view.rot[2]*fz) >> 12) + cam_view.trans[0];
                int32_t vy = (int32_t)(((int64_t)cam_view.rot[3]*fx + (int64_t)cam_view.rot[4]*fy + (int64_t)cam_view.rot[5]*fz) >> 12) + cam_view.trans[1];
                int32_t vz = (int32_t)(((int64_t)cam_view.rot[6]*fx + (int64_t)cam_view.rot[7]*fy + (int64_t)cam_view.rot[8]*fz) >> 12) + cam_view.trans[2];
                if (vz < 64) continue;
                int32_t _ir1 = vx>0x7FFF?0x7FFF:(vx<-0x8000?-0x8000:vx);
                int32_t _ir2 = vy>0x7FFF?0x7FFF:(vy<-0x8000?-0x8000:vy);
                uint32_t _sz3 = (uint32_t)(vz>0xFFFF?0xFFFF:vz);
                uint32_t _n = re15_gte_divide((uint32_t)cam_view.fov_screen_dist, _sz3);
                int sx_p = cx + (int)(((int64_t)_ir1*(int64_t)_n)>>16);
                int sy_p = cy + (int)(((int64_t)_ir2*(int64_t)_n)>>16);
                int sz = (int)(((int64_t)2000 * _n) >> 16);
                if (sz < 8) sz = 8;
                if (sz > 80) sz = 80;
                re15_render_tile(sx_p - sz/8, sy_p - sz/8, sz/4, sz/4, 0, 200, 180, 50);
                (void)pi;
            }

            /* Cut/player HUD + projection marker silenced. */
            (void)active_cut_count;

            /* Phase ESP-C: draw the op-0x3a effect particles (after actors, in cam_view scope). */
            pc_fx_set_camf(rdt_buf, (size_t)rdt_size, (int)g_scd.cam_id);
            pc_draw_effects(&cam_view, cx, cy,
                            cam_has_region, cam_region_xs, cam_region_zs);
        }

        /* INVENTORY on top (Phase 8.26 / wave 1): the screen is drawn into the framebuffer, but
         * end_frame composites the queued 3D meshes, character-shadow blobs AND the room PRI
         * overdraw rects ON TOP of the framebuffer — the PSX screen task draws none of the frozen
         * scene, so drop all three while the menu is open. */
        if (re15_menu_is_open()) {
            extern void re15_render_pc_clear_scene_overlays(void);
            re15_render_pc_clear_scene_overlays();
        }

        /* ITEM-GET MODAL overlay: hand the current-frame item quad to the renderer. It is drawn
         * AFTER the 3D meshes in end_frame (see re15_render_pc_item_modal) so the zooming/flipping
         * item box floats ABOVE the frozen scene (the game is paused underneath — NOT cleared, unlike
         * the full-screen inventory). Byte-true FUN_8001db28; the box art is the per-item 112×72
         * picture from ITEM/ITPS.ITP (re15_itps_pixel, id×0x3000). */
        {
            extern void re15_render_pc_item_modal(int on, const int *cx, const int *cy,
                                                  uint8_t type, int face);
            int mqx[4], mqy[4], mface = 0; uint8_t mtype = 0;
            int mdraw = re15_item_modal_quad(mqx, mqy, &mtype, &mface);
            if (mdraw) re15_render_pc_item_modal(1, mqx, mqy, mtype, mface);
            else       re15_render_pc_item_modal(0, NULL, NULL, 0, 0);
            /* RE15_MODAL_LOG: FILE trace of the live modal FSM (stderr goes to the void for the SDL
             * exe) — proves the presentation ticks in the running game with the right progression. */
            {
                static FILE *ml = NULL; static int mli = 0;
                if (!mli) { mli = 1; const char *p = getenv("RE15_MODAL_LOG"); if (p && *p) ml = fopen(p, "w"); }
                if (ml && re15_item_modal_active()) {
                    fprintf(ml, "F%u state=%u f630=%d draw=%d type=0x%02x face=%d",
                            g_engine.frame_count, re15_item_modal_state(), re15_item_modal_frame(),
                            mdraw, mtype, mface);
                    if (mdraw) fprintf(ml, " quad TL(%d,%d) TR(%d,%d) BL(%d,%d) BR(%d,%d)",
                                       mqx[0],mqy[0], mqx[1],mqy[1], mqx[2],mqy[2], mqx[3],mqy[3]);
                    fprintf(ml, "\n"); fflush(ml);
                }
            }
        }

        /* ITEM-GET MODAL message box (states 5/6, byte-true FUN_80027e68 a1=0x100 → the message VM):
         * "WILL YOU TAKE THE <item>." with a Yes/No cursor when there's room, or "YOU CAN'T CARRY ANY
         * MORE ITEMS" when full. Box origin (34,180) is byte-true (DAT_800b8534=0x22 / 8536=0xb4); the
         * text glyphs + item name render in the real TEX.TIM message font via byte-true glyph replay
         * (re15_render_pc_item_prompt, render_pc.c:1681, using re15_msgfont_glyph; the prompt's own
         * glyph bytes come from the BSS scripts @0x800c4fc6 + name blob) — byte-true end-to-end, no
         * longer a 6×8-overlay faithful-line. */
        /* ORIGINAL-DEBUG-MENUE ZEICHNEN — Layout byte-true aus PSX.EXE @0x80014AB4..0x80014C08.
         * Der Zeichenblock enthaelt GENAU 7 Aufrufe der Text-Queue FUN_800279C8. a0 ist die gepackte
         * Position (y<<16)|x (low half = X, high half = Y, belegt am Queue-Walker @0x800291B4), a1=3
         * das Attribut. a1 ist fuer ALLE SIEBEN identisch 3 — es gibt also KEINE Farbhervorhebung der
         * gewaehlten Zeile, die Auswahl zeigt allein der Cursor.
         *   1 @0x80014AD0  x=104 y=84        "- DEBUG MENU -"  (String @0x8001052C)
         *   2 @0x80014AEC  x=96  y=100       "UTILITY MENU"    (@0x8001053C)
         *   3 @0x80014B14  x=96  y=108       "JUMP %d"         (@0x8001054C) Arg = Stage + 1
         *   4 @0x80014B44  x=144 y=108       "%2X"             (@0x80010554) Arg = Raumindex
         *   5 @0x80014BB4  x=168 y=108       "%s"              (@0x80010558) Arg = Name aus DEBUG.BIN
         *   6 @0x80014BD0  x=96  y=116       "MEMORY VIEWER"   (@0x8001055C)
         *   7 @0x80014BFC  x=80  y=100+8*sel ">"               (@0x8001056C) sel = 0x800BBE5D
         * Zeilenabstand 8 px, Cursor-Spalte 16 px links vom Text.
         *
         * GLYPHEN byte-true (2026-08-09): attr=3 ohne Bit 0x80 -> Flusher-Pfad FUN_80029214
         * = die 8x8-FESTBREITEN-Debugschrift, SPRT_8 @tpage(768,256) 4bpp, CLUT (256,480).
         * Quelle: TEX.TIM ("TEX TIM" Datei-ID 0x21 @0x800212B8, Slot-Upload sh 0x001b ->
         * DAT_800aca4c @0x800212D0/D8 -> Pixel @VRAM(704,256), tpage 768 = TIM-Texel x=256,
         * Glyphzeilen v=0..31). Gezeichnet via re15_render_pc_debug_text (render_pc.c, dort
         * die volle Beleg-Kette). Box = FUN_80014CC4: TILE (0x20,0x4C,0x100,0x38), r=g=0xff
         * (Open-Block ori 0xff @0x80014510, sh -> 0x800bbe66 @0x80014518), b=0, SemiTrans +
         * DrawMode-tpage 0x40 -> ABR2 SUBTRAKTIV. Die Einblend-Rampe +8/Frame (@0x80014D70/90,
         * Zustand 0) ist im MZD-Build TOTER CODE: der Open-Block setzt den Zustand DIREKT auf 1
         * (sw 1 @0x8001449C) und die Rampe auf 0xff — Zustand 0 ist unerreichbar (einziger
         * Schreiber von DAT_8008f618 ist das Menue selbst, BSS-Startwert 0 zaehlt nicht, weil
         * das Menue nur bei DAT_800bbe5c!=0 zeichnet). */
        if (re15_debug_menu_open()) {
            extern int  re15_render_pc_debug_text(int x, int y, const char *str);
            extern void re15_render_pc_debug_box(int rg);
            const re15_debug_menu_t *dm = re15_debug_menu_state();
            char dbuf[64];
            re15_render_pc_debug_box(0xff);                            /* FUN_80014CC4, Rampe 0xff */
            re15_render_pc_debug_text(104, 84,  "- DEBUG MENU -");     /* @0x80014AD0, String @0x8001052C */
            re15_render_pc_debug_text(96,  100, "UTILITY MENU");       /* @0x80014AEC, @0x8001053C */
            snprintf(dbuf, sizeof dbuf, "JUMP %d", dm->stage + 1);     /* @0x80014B14, @0x8001054C */
            re15_render_pc_debug_text(96,  108, dbuf);
            /* "%2X" @0x80010554: der Formatter FUN_800279c8 (case 'X') druckt bei fester Breite
             * ALLE Nibbles inkl. fuehrender Nullen -> C-Aequivalent ist %02X, nicht %2X. */
            snprintf(dbuf, sizeof dbuf, "%02X", dm->room_idx[dm->stage]); /* @0x80014B44 */
            re15_render_pc_debug_text(144, 108, dbuf);
            re15_render_pc_debug_text(168, 108, re15_debug_menu_room_name()); /* @0x80014BB4, "%s" @0x80010558 */
            re15_render_pc_debug_text(96,  116, "MEMORY VIEWER");      /* @0x80014BD0, @0x8001055C */
            re15_render_pc_debug_text(80,  100 + 8 * dm->row, ">");    /* @0x80014BFC, ">" @0x8001056C */
        }

        {
            extern void re15_render_pc_text_overlay(int x, int y, const char *text);
            extern int  re15_render_pc_text_overlay_n(int x, int y, const char *text, int n);
            extern void re15_render_pc_cursor(int x, int y);
            uint8_t ptype = 0; int pchoice = 0;
            int prompt = re15_item_modal_prompt(&ptype, &pchoice);
            int reveal = re15_item_modal_reveal();   /* typewriter budget (2 frames/glyph) */
            if (prompt) {
                /* Byte-true GAME-FONT render: replay the prompt's own glyph bytes (BSS scripts + name
                 * blob) through the TEX.TIM message font — "Will you take the <Item>." / "You can't
                 * carry any more items", exact font + Title-Case, typewritered to `reveal` glyphs. */
                extern void re15_render_pc_item_prompt(int x, int y, int prompt_type, uint8_t item_id, int reveal);
                re15_render_pc_item_prompt(34, 180, prompt, ptype, reveal);
                if (prompt == 1 && re15_item_modal_prompt_ready()) {  /* Yes/No only after the text types out */
                    static const unsigned char yes_g[3] = { 0x35, 0x41, 0x4f };  /* "Yes" (game glyphs) */
                    static const unsigned char no_g[2]  = { 0x2a, 0x4b };         /* "No"                */
                    extern int re15_render_pc_msg_text(int x, int y, const unsigned char *raw, int len);
                    re15_render_pc_msg_text(190, 202, yes_g, 3);
                    re15_render_pc_msg_text(234, 202, no_g,  2);
                    re15_render_pc_cursor(pchoice ? 224 : 180, 203);  /* ▶ on the current choice */
                }
            }
        }

        /* MESS-HAKEN RE15_FBDUMP="<frame>:<pfad.ppm>" (2026-08-21) — dumpt den SOFTWARE-
         * FRAMEBUFFER (die 2D-Ebene: BG-Blit + Overlays) roh als PPM, VOR end_frame.
         * Grund: `re15_render_pc_screenshot` (SDL_RenderReadPixels) und ffmpeg-gdigrab
         * liefern in einer nicht-interaktiven Sitzung BEIDE komplett Schwarz (gemessen:
         * auch das Titelbild kommt mit max=24 zurueck) — Pixel-Verifikation ist dort
         * unmoeglich. Dieser Haken umgeht SDL vollstaendig und beantwortet die Frage
         * "ist der HINTERGRUND schwarz?" (der Hintergrund IST die 2D-Ebene; die 3D-Props
         * gehen ueber SDL_RenderGeometry und stehen bewusst NICHT drin). Rein env-gegatet,
         * Normalpfad unberuehrt. */
        { static int s_fbd_init = 0; static long s_fbd_frame = -1; static char s_fbd_path[256];
          if (!s_fbd_init) { s_fbd_init = 1;
              const char *e = getenv("RE15_FBDUMP");
              if (e && *e) { const char *c = strchr(e, ':');
                  if (c && (size_t)(c - e) < 16) { s_fbd_frame = atol(e);
                      snprintf(s_fbd_path, sizeof s_fbd_path, "%s", c + 1); } } }
          if (s_fbd_frame >= 0 && (long)g_engine.frame_count == s_fbd_frame) {
              extern uint32_t *re15_pc_framebuffer(void);
              const uint32_t *fb = re15_pc_framebuffer();
              FILE *f = fb ? fopen(s_fbd_path, "wb") : NULL;
              if (f) { fprintf(f, "P6\n%d %d\n255\n", SCREEN_XRES, SCREEN_YRES);
                  for (int i = 0; i < SCREEN_XRES * SCREEN_YRES; i++) {
                      unsigned char rgb[3] = { (unsigned char)(fb[i] >> 24),
                                               (unsigned char)(fb[i] >> 16),
                                               (unsigned char)(fb[i] >>  8) };
                      fwrite(rgb, 1, 3, f); }
                  fclose(f);
                  fprintf(stderr, "[fbdump] F%u -> %s\n", g_engine.frame_count, s_fbd_path); } } }

        /* MESS-HAKEN RE15_FRAMEDUMP="<frame>:<pfad.ppm>" (2026-08-21) — im Unterschied zu
         * RE15_FBDUMP (nur die 2D-Ebene) liefert das den KOMPLETT komponierten Frame
         * inklusive der ueber SDL_RenderGeometry gezeichneten 3D-Aktoren/Props. Der Auftrag
         * wird hier gesetzt und in re15_render_end_frame() unmittelbar VOR SDL_RenderPresent
         * ausgefuehrt — nach dem Present ist der Backbuffer undefiniert, genau daran
         * scheitern RE15_AUTOSHOT/gdigrab (s. Kommentar am AUTOSHOT-Block). Rein env-gegatet. */
        /* Zwei Formen: EINZELBILD "<frame>:<pfad.ppm>" und SERIE
         * "<start>-<end>/<step>:<pfad-praefix>" (schreibt <praefix>NNNNNN.ppm). */
        { static int s_fdd_init = 0; static long s_fdd_frame = -1, s_fdd_end = -1, s_fdd_step = 1;
          static char s_fdd_path[256];
          if (!s_fdd_init) { s_fdd_init = 1;
              const char *e = getenv("RE15_FRAMEDUMP");
              if (e && *e) { const char *c = strrchr(e, ':');
                  if (c && (size_t)(c - e) < 32) { s_fdd_frame = atol(e);
                      const char *dash = strchr(e, '-');
                      if (dash && dash < c) { s_fdd_end = atol(dash + 1);
                          const char *sl = strchr(dash, '/');
                          if (sl && sl < c) { long st = atol(sl + 1); if (st > 0) s_fdd_step = st; } }
                      snprintf(s_fdd_path, sizeof s_fdd_path, "%s", c + 1); } } }
          if (s_fdd_frame >= 0) {
              long f = (long)g_engine.frame_count;
              extern void re15_render_pc_request_readback(const char *path);
              if (s_fdd_end < 0) {
                  if (f == s_fdd_frame) re15_render_pc_request_readback(s_fdd_path);
              } else if (f >= s_fdd_frame && f <= s_fdd_end &&
                         ((f - s_fdd_frame) % s_fdd_step) == 0) {
                  char p[320]; snprintf(p, sizeof p, "%s%06ld.ppm", s_fdd_path, f);
                  re15_render_pc_request_readback(p);
              } } }

        re15_render_end_frame();

        /* RE15_INV_SHOT continuation: capture the presented acceptance frame + exit
         * (re15_render_pc_screenshot writes BMP regardless of the extension; set
         * RE15_AUTOSHOT_SMALL=1 for a native 320x240 dump). */
        { static const char *s_inv_shot = NULL; static int s_inv_shot_init = 0;
          static uint32_t s_inv_shot_frame = 34;
          if (!s_inv_shot_init) { s_inv_shot = getenv("RE15_INV_SHOT"); s_inv_shot_init = 1;
              /* grid-mode shot (wave 2): CROSS injected at F31, entry slide F32-38,
               * GRID from F39 — capture well inside grid mode. */
              if (getenv("RE15_INV_GRID_SHOT")) s_inv_shot_frame = 50;
              /* command-stage shot (wave 3): 2nd CROSS at F45, state 4 from F53. */
              if (getenv("RE15_INV_CMD_SHOT"))  s_inv_shot_frame = 60;
              /* CHECK/examine shot (wave 4): settled photo + full knife desc. */
              if (getenv("RE15_INV_CHECK_SHOT")) s_inv_shot_frame = 140;
              /* EXCHANGE/mix shot (wave 5): settled grid with the G.R mix result. */
              if (getenv("RE15_INV_MIX_SHOT"))  s_inv_shot_frame = 100;
              /* MAP shot (MAP wave): slide F32-56, upload F57, interactive after. */
              if (getenv("RE15_INV_MAP_SHOT"))  s_inv_shot_frame = 75;
              /* FILE shots (FILE wave): slide F32-61, list from F63; reader from F72. */
              if (getenv("RE15_INV_FILE_SHOT")) s_inv_shot_frame = 75;
              if (getenv("RE15_INV_FILE_DOC_SHOT")) s_inv_shot_frame = 85; }
          if (s_inv_shot && *s_inv_shot && g_engine.frame_count == s_inv_shot_frame) {
              extern void re15_render_pc_screenshot(const char *path);
              re15_render_pc_screenshot(s_inv_shot);
              fprintf(stderr, "[inv] acceptance shot -> %s\n", s_inv_shot);
              exit(0);
          } }

        /* AH-round: 30 FPS cap. SDL2 VSync runs at the monitor refresh
         * (60/144/etc) which makes our engine run at non-PSX speeds.
         * SDL_Delay-based cap is portable + matches PSX 30Hz cinematic
         * pacing. Override via RE15_FPS env var (option-menu hook). */
        {
            static uint32_t last_tick = 0;
            uint32_t now = SDL_GetTicks();
            uint32_t elapsed = now - last_tick;
            if (elapsed < frame_budget_ms && last_tick != 0) {
                SDL_Delay(frame_budget_ms - elapsed);
            }
            last_tick = SDL_GetTicks();
        }
        /* AUTO-SCREENSHOT for ablauf diff (2026-05-24). When env
         * RE15_AUTOSHOT=1, snap frames at preset intervals from sub11
         * start through sub02 finish, dump as shots/NN.bmp.
         *
         * WARUM AUTOSHOT REGELMAESSIG SCHWARZE/WEISSE BILDER LIEFERT (2026-08-21; hat in
         * mehreren Sitzungen Messungen verdorben — u.a. wurde das TITELBILD so mit max=24
         * "gemessen"): dieser Block steht NACH `re15_render_end_frame()`, und end_frame
         * schliesst mit `SDL_RenderPresent`. Nach dem Present ist der Backbuffer je nach
         * Backend undefiniert (bei ACCELERATED typischerweise verworfen), also liest
         * `re15_render_pc_screenshot` -> `SDL_RenderReadPixels` nicht mehr das gerade
         * gezeigte Bild. Ein Schwarz-/Weissbild von hier ist darum KEIN Befund ueber das
         * Rendering. Wer Pixel messen will, nimmt eines von beiden:
         *   RE15_FBDUMP    = "<frame>:<pfad.ppm>" — nur die 2D-Ebene (BG-Blit + Overlays),
         *                    VOR end_frame; enthaelt KEINE 3D-Props/Aktoren (die laufen
         *                    ueber SDL_RenderGeometry).
         *   RE15_FRAMEDUMP = "<frame>:<pfad.ppm>" — der KOMPLETT komponierte Frame,
         *                    zurueckgelesen INNERHALB von end_frame direkt VOR dem Present
         *                    (render_pc.c, re15_render_pc_request_readback).
         * Siehe auch Skill `re15-port-visual-verify` (gdigrab auf das echte Fenster). */
        {
            static const char *autoshot = NULL;
            static int s_autoshot_inited = 0;
            if (!s_autoshot_inited) {
                autoshot = getenv("RE15_AUTOSHOT");
                s_autoshot_inited = 1;
            }
            if (autoshot && *autoshot == '1') {
                extern void re15_render_pc_screenshot(const char *path);
                /* RE15_AUTOSHOT_SERIES=START,END,STEP captures every STEP-th
                 * frame in [START,END] for per-frame motion / jitter analysis.
                 * Bypasses the fixed shot_frames list. */
                static const char *series_env = NULL;
                static int s_series_init = 0;
                static int series_start = -1, series_end = -1, series_step = 1;
                static int next_series_capture = -1;
                /* CLIP_TEST=ALL bookkeeping: capture 5 shots per clip at
                 * fixed offsets within each 100-frame window. */
                static int s_clip_all_mode = 0;
                if (!s_series_init) {
                    series_env = getenv("RE15_AUTOSHOT_SERIES");
                    const char *ct = getenv("RE15_CLIP_TEST");
                    /* RE15_CLIP_TEST=ALL overrides series with a sweep. */
                    if (ct && (ct[0] == 'A' || ct[0] == 'a')) {
                        s_clip_all_mode = 1;
                        fprintf(stderr, "[autoshot] CLIP_TEST=ALL sweep — "
                                "capturing clips 0..23 at kf 0..4 each\n");
                    } else if (series_env) {
                        int a = -1, b = -1, c = 1;
                        if (sscanf(series_env, "%d,%d,%d", &a, &b, &c) >= 2) {
                            series_start = a;
                            series_end   = b;
                            series_step  = (c > 0 ? c : 1);
                            next_series_capture = a;
                        }
                    } else if (ct && *ct) {
                        /* Sensible default for single-clip mode so user
                         * doesn't have to remember the series env. Captures
                         * 5 anim-frame offsets: 5,15,25,35,45 → covers up
                         * to ~22 clip-frames at /2 ratio, enough for one
                         * full cycle of the typical 20-30f gait clip. */
                        series_start = 5;
                        series_end   = 45;
                        series_step  = 10;
                        next_series_capture = 5;
                        fprintf(stderr, "[autoshot] CLIP_TEST=%s default series 5..45 step 10\n", ct);
                    }
                    s_series_init = 1;
                }
                /* CLIP_TEST=ALL sweep: 5 captures per 100-frame clip window
                 * at offsets +5/+15/+25/+35/+45. Stops after clip 23 (frame
                 * 2400). */
                if (s_clip_all_mode) {
                    int f      = (int)g_engine.frame_count;
                    int clip_i = f / 100;
                    int off    = f - clip_i * 100;
                    int kf_idx_out = -1;
                    if      (off == 5)  kf_idx_out = 0;
                    else if (off == 15) kf_idx_out = 1;
                    else if (off == 25) kf_idx_out = 2;
                    else if (off == 35) kf_idx_out = 3;
                    else if (off == 45) kf_idx_out = 4;
                    if (clip_i < 24 && kf_idx_out >= 0) {
                        char path[96];
                        snprintf(path, sizeof path,
                                 "shots/clip_%02d_kf%02d.bmp",
                                 clip_i, kf_idx_out);
                        re15_render_pc_screenshot(path);
                    }
                    if (f >= 2400) {
                        fprintf(stderr, "[autoshot] CLIP_TEST=ALL done (clip 23 captured)\n");
                        running = 0;
                    }
                }
                else if (next_series_capture >= 0 &&
                    g_engine.frame_count == (uint32_t)next_series_capture) {
                    char path[96];
                    const char *ct = getenv("RE15_CLIP_TEST");
                    if (ct && *ct) {
                        /* CLIP_TEST mode: name shots by clip + keyframe-
                         * within-series so a sweep of clips produces
                         * comparable filenames. */
                        int kf_in_series =
                            (next_series_capture - series_start) / series_step;
                        snprintf(path, sizeof path,
                                 "shots/clip_%02d_kf%02d.bmp",
                                 atoi(ct), kf_in_series);
                    } else {
                        snprintf(path, sizeof path, "shots/series_f%05d.bmp",
                                 next_series_capture);
                    }
                    re15_render_pc_screenshot(path);
                    next_series_capture += series_step;
                    if (next_series_capture > series_end) {
                        fprintf(stderr, "[autoshot] series done\n");
                        running = 0;
                    }
                }
                /* Frames to capture (covers narrator + helipad cinematic +
                 * post-cinematic gameplay where Elliot should be despawned
                 * and rotor should be off-screen). */
                /* AI-round: scale shot_frames by target_fps. Original values
                 * tuned at 60fps; at 30fps each frame = 2x wall-time so we
                 * need to halve to capture the same cinematic moments. */
                static int shot_frames[14];
                static int shot_frames_inited = 0;
                if (!shot_frames_inited) {
                    int base[] = {600, 1850, 2100, 2904, 3022, 3500, 3700,
                                  3900, 4200, 4400, 4600, 5000, 5500, 6000};
                    for (int i = 0; i < 14; i++)
                        shot_frames[i] = FRAME_AT_60(base[i]);
                    shot_frames_inited = 1;
                }
                static int next_shot = 0;
                int N = (int)(sizeof(shot_frames)/sizeof(shot_frames[0]));
                if (series_env || s_clip_all_mode || getenv("RE15_CLIP_TEST")) {
                    /* series / clip-test mode active — skip canonical shots */
                }
                else if (next_shot < N && g_engine.frame_count >= (uint32_t)shot_frames[next_shot]) {
                    char path[64];
                    snprintf(path, sizeof path, "shots/%02d_f%d.bmp",
                             next_shot, shot_frames[next_shot]);
                    re15_render_pc_screenshot(path);
                    next_shot++;
                    if (next_shot >= N) {
                        /* Also dump TIM slots so we can compare with raw textures. */
                        extern void re15_render_pc_dump_slot(int slot, const char *path);
                        re15_render_pc_dump_slot(0, "shots/slot0_leon_tim.bmp");
                        re15_render_pc_dump_slot(1, "shots/slot1_elliot_tim.bmp");
                        re15_render_pc_dump_slot(4, "shots/slot4_obj0_tim.bmp");
                        re15_render_pc_dump_slot(5, "shots/slot5_obj1_tim.bmp");
                        re15_render_pc_dump_slot(6, "shots/slot6_obj2_heli_tim.bmp");
                        re15_render_pc_dump_slot(7, "shots/slot7_obj3_rotor_tim.bmp");
                        re15_render_pc_dump_slot(8, "shots/slot8_obj4_light_tim.bmp");
                        re15_render_pc_dump_slot(9, "shots/slot9_obj5_pilot_tim.bmp");
                        fprintf(stderr, "[autoshot] all %d shots captured, exiting\n", N);
                        running = 0;
                    }
                }
            }
        }
        g_engine.frame_count++;
    }
    /* FE-5.3: the death FSM set mode=TITLE and broke the game loop — go back to the title menu
     * (YOU DIED -> TITLE). CONTINUE there reloads the last card save; NEW GAME restarts. Any other
     * exit (SDL_QUIT calls exit() directly) falls through to a normal return. */
    if (re15_gameflow_mode() == RE15_MODE_TITLE) goto re_title;
    return 0;
}
