/* probe_gen_11f0_cursor_view.c — MESSSONDE "Cursor-/Bedien-Screen ROOM11F0, Hintergrund schwarz?"
 *
 * NUTZER-BEFUND (Spieltest v0.3.5): "Beim Cursor im Generator-Raum (ROOM 11F0) ist der
 * Hintergrund noch schwarz."
 *
 * WAS DER SCREEN IST (roh aus ROOM11F0.RDT, main=RDT+0x40->0x0CC4, sub=RDT+0x44->0x0D34):
 *   sub00 @0x0D64: Aot_set slot1 sce=3 flags=0x31, Rect x[-2200..-1400] z[-15850..-14450],
 *                  eventId pc[17]=0x10 -> sub16 (das Bedienfeld an der Wand).
 *   sub16 @0x15A2: Message_on(0), Evt_next, Message_on(1), Evt_next,
 *                  Ifel_ck @0x15AE / Ck(0x0c,0x1f)==0 -> Aot_reset(1) @0x15B6,
 *                  ** Cut_chg 0x0A @0x15C0 **, Set(5,0x00..0x0C)=1 @0x15C2..0x15F2.
 *   sub01 @0x108C: solange Ck(5,0)==1 -> Sce_key_ck up/down/left/right (@0x1098/0x10B0/
 *                  0x10C8/0x10E0) -> Evt_exec sub02..05; die bewegen Objekt 0 ("Maschine")
 *                  per Speed_set(2,+-200)/Speed_set(0,+-200) = DER CURSOR.
 *   sub17 @0x16F0: Cut_old + Cut_auto(1) = Screen verlassen.
 *   => Der "Cursor-Screen" ist KEIN eigenes UI-Overlay (kein Keypad-/EDIT-Screen), sondern
 *      der Raum unter KAMERA-CUT 10 + 11 Obj_model_set-Props.
 *
 * KAMERA-CUT 10 (RDT+0x24 -> Tabelle @Datei 0x60, 32 B/Eintrag, Eintrag 10 @0x1A0):
 *   flag=0 fov=26684 pos=(-19628,-17442,22616) tgt=(-19628,15928,22617)
 *   = Nadir-Kamera 17442 Einheiten UEBER dem Schalter-Diorama (Props x -25975/-18775,
 *     z 17800..26000), Blickrichtung exakt +Y (senkrecht nach unten).
 *
 * ORIGINAL-BELEG, dass dort das RAUMBILD (kein Schwarz) steht:
 *   Cut_chg-Handler @0x800402a0: DAT_800b0fe4 = pc[1] (@0x800402fc), DAT_800aca3c |= 0x100
 *   (@0x800402d4/d8), DAT_800b5457 = 1 (@0x800402f4), jal FUN_800142f4 (@0x80040300).
 *   Anzeige-Task @0x800214f4-0x80021514: DAT_800b0fe4 != DAT_800afbb5 -> DAT_800b5457 = 1;
 *   @0x8002152c-60: Flag gesetzt -> jal FUN_80021bbc = BG-NACHLADEN.
 *   FUN_80021bbc @0x80021c88-0x80021cb8: `lh a3,DAT_800b0fe4` (der neue CUT) und
 *   `lh a1,DAT_800b0fe2` (Raum), Adresse = s1 + (a1<<5) + a3*2 mit s1 = DAT_800b52c8 ->
 *   `lhu` = die BG-LBA -> DAT_800be574. Der BG-INDEX IST ALSO DER CUT-INDEX; Cut 10 laedt
 *   BG10. Es gibt KEINE Unterdrueckung des Hintergrunds fuer diesen Screen.
 *   (Waehrend des CD-Lesens haelt das Original sogar den ALTEN Cut: @0x80021bdc-0x80021bfc
 *   schreibt DAT_800b0fe4 = DAT_800afbb5 zurueck — also erst recht kein Schwarz.)
 *
 * WAS DIESE SONDE MISST (kein Pixelvergleich — SDL_RenderReadPixels UND ffmpeg-gdigrab
 * liefern in einer nicht-interaktiven Sitzung BEIDE Schwarz, Kontrolle: das Titelbild kam
 * mit max=24 zurueck; der einzige verwertbare Pixelpfad war der neue main.c-Haken
 * RE15_FBDUMP, der den Software-Framebuffer roh dumpt und BG10 zeigt):
 *   M1  BG10 existiert, ist ein VIDEO-Chunk und dekodiert NICHT schwarz (Engine-Decoder).
 *   M2  Der Screen erreicht Cut 10 ueber den echten Bytecode (sub16 -> g_scd.cam_id).
 *   M3  Die Cut-10-Kamera-Bytes stimmen mit der RDT ueberein.
 *   M4  Die 11 Props verdecken den Hintergrund NICHT: Bildschirm-Bbox jedes Props unter
 *       der Cut-10-Kamera, mit derselben Q12-Projektion wie der PC-Prop-Renderer
 *       (main.c: re15_camera_compose_view_bone + re15_gte_divide, Near-Clip _vz<64).
 *       Ein Prop, der den 320x240-Rahmen ueberdeckt, WAERE der schwarze Hintergrund.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_md1.h"
#include "re15_bss.h"
#include "re15_camera.h"
#include "re15_math.h"
#include "re15_msg.h"

extern uint16_t g_scd_pad_edge;
extern uint16_t g_scd_pad_held;

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern scd_vm_t g_scd;

static int g_fail = 0;
#define CHECK(cond, ...) do { \
    if (!(cond)) { g_fail++; printf("  FAIL: " __VA_ARGS__); printf("\n"); } \
} while (0)

#define SCR_W 320
#define SCR_H 240

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

/* Cut-10-SOLL aus den RDT-Bytes @Datei 0x1A0 (Tabelle 0x60 + 10*32). */
static const int32_t SOLL_CUT10[7] = { 26684, -19628, -17442, 22616, -19628, 15928, 22617 };

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t sz = 0;

    /* ================= M1: BG10 ist da und dekodiert NICHT schwarz ================= */
    printf("===== M1: BSS/ROOM11F0/BG10.BSS (der Hintergrund des Cursor-Screens) =====\n");
    {
        snprintf(path, sizeof path, "%s/BSS/ROOM11F0/BG10.BSS", base);
        size_t bsz = 0;
        uint8_t *bg = read_file(path, &bsz);
        CHECK(bg != NULL, "BG10.BSS nicht lesbar (%s)", path);
        if (bg) {
            re15_bss_chunk_t ch;
            int parsed = re15_bss_parse_chunk(bg, bsz, &ch);
            CHECK(parsed, "BG10.BSS: Header nicht parsebar");
            CHECK(parsed && re15_bss_chunk_has_video(&ch),
                  "BG10.BSS: id=0x%04X != 0x3800 (kein Video-Chunk)", parsed ? ch.id : 0);
            if (parsed && re15_bss_chunk_has_video(&ch)) {
                size_t cap = ((size_t)ch.run_length_words + 2) * 4;
                int16_t *co = (int16_t *)malloc(cap * sizeof(int16_t));
                uint8_t *rgb = (uint8_t *)malloc((size_t)SCR_W * SCR_H * 3);
                int w = (co && rgb)
                      ? re15_bss_vlc_decode(ch.vlc_payload, ch.vlc_payload_size,
                                            ch.run_length_words, ch.quant, ch.version, co, cap)
                      : -1;
                int rv = (w > 0) ? re15_bss_mdec_decode(co, (size_t)w, SCR_W, SCR_H, rgb) : -1;
                CHECK(rv == 0, "BG10.BSS dekodiert nicht (vlc=%d mdec=%d)", w, rv);
                if (rv == 0) {
                    long sum = 0; int mx = 0, lit = 0;
                    for (int i = 0; i < SCR_W * SCR_H; i++) {
                        int l = (rgb[i*3] + rgb[i*3+1] + rgb[i*3+2]) / 3;
                        sum += l; if (l > mx) mx = l; if (l >= 8) lit++;
                    }
                    int mean = (int)(sum / (SCR_W * SCR_H));
                    printf("   BG10 rl=%u q=%u ver=%u -> mean=%d max=%d hell(>=8)=%d/%d\n",
                           ch.run_length_words, ch.quant, ch.version, mean, mx, lit, SCR_W * SCR_H);
                    /* Schwelle bewusst grob: es geht NUR um "schwarz vs. nicht schwarz".
                     * Gemessen 2026-08-21: mean=48, max=253, hell=45570/76800 (59.3%). */
                    CHECK(mean > 8, "BG10 ist praktisch SCHWARZ (mean=%d)", mean);
                    CHECK(lit > SCR_W * SCR_H / 4,
                          "BG10 hat nur %d helle Pixel — waere ein schwarzer Screen", lit);
                }
                free(co); free(rgb);
            }
            free(bg);
        }
    }

    /* ================= RDT laden ================= */
    snprintf(path, sizeof path, "%s/STAGE1/ROOM11F0.RDT", base);
    uint8_t *dat = read_file(path, &sz);
    if (!dat) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(dat, sz, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    /* ================= M2: sub16 fuehrt wirklich auf Cut 10 ================= */
    printf("\n===== M2: sub16 (Bedienfeld, AOT slot1 sce=3 ev=0x10) -> Cut_chg @0x15C0 =====\n");
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x11F0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -1800; pl->y = 0; pl->z = -15100; pl->rot_y = 0;   /* im Panel-Rect @0x0D64 */
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    {
        /* Der Bytecode-Zweig haengt an Ck(0x0c,0x1f)==0 (@0x15B2) — Bank 12 ist beim
         * frischen Raum-Init 0, der Zweig laeuft also. */
        CHECK(re15_game_flag_get(0x0c, 0x1f) == 0,
              "flag(0x0c,0x1f)=%d != 0 — sub16 wuerde den Cut-Zweig ueberspringen",
              re15_game_flag_get(0x0c, 0x1f));
        g_scd.cam_id = 0; g_scd.cam_change_pending = 0;
        int th = scd_event_fire(16);
        CHECK(th >= 0, "scd_event_fire(16) fand keinen freien Thread");
        /* sub16 zeigt ZUERST Message_on(0) + Message_on(1) (@0x15A2/@0x15A8). Die Dialog-
         * Maschine wird — wie im Spiel — pro Bild getickt und mit der Bestaetigungs-Taste
         * quittiert (virtuelles Bit 0x4000 = physisch QUADRAT, msg_common.c:422/434,
         * @0x80028570 / @0x80073dbc[14]). Ohne das haengt sub16 an der ersten Seite, genau
         * wie im Live-Lauf (dort dauerte es 346 Bilder, bis der Nutzer durchgeklickt hatte). */
        re15_msg_load_room_block(rdt.messages, rdt.messages_size);
        for (int t = 0; t < 4000 && (int)g_scd.cam_id != 10; t++) {
            scd_vm_tick();
            /* Druck-FLANKE jedes 4. Bild (gehalten waere nur EINE Flanke). */
            g_scd_pad_edge = ((t & 3) == 0) ? 0x4000u : 0u;
            g_scd_pad_held = 0u;
            const unsigned char *mraw; int mlen, mid;
            re15_msg_tick(&mraw, &mlen, &mid);
        }
        g_scd_pad_edge = 0u;
        printf("   cam_id=%u cam_change_pending=%u cut_auto_enabled=%u  flag(5,0)=%d\n",
               g_scd.cam_id, g_scd.cam_change_pending, g_scd.cut_auto_enabled,
               re15_game_flag_get(5, 0));
        CHECK((int)g_scd.cam_id == 10, "cam_id=%u != 10 (Cut_chg 0x0A @0x15C0)", g_scd.cam_id);
        CHECK(re15_game_flag_get(5, 0) == 1,
              "flag(5,0) nicht gesetzt — sub01 wuerde den Cursor nicht fahren (@0x15C2)");
    }

    /* ================= M3: Kamera-Bytes von Cut 10 ================= */
    printf("\n===== M3: Kamera-Cut 10 (RDT+0x24, Eintrag 10 @Datei 0x1A0) =====\n");
    CHECK(rdt.cut_count > 10, "cut_count=%d <= 10 — Cut 10 existiert nicht", rdt.cut_count);
    if (rdt.cut_count <= 10) { free(dat); return 1; }
    const re15_camera_cut_t *c10 = &rdt.cuts[10];
    printf("   flag=%u fov=%u pos=(%ld,%ld,%ld) tgt=(%ld,%ld,%ld) pri=0x%lX\n",
           c10->flag, c10->fov, (long)c10->pos_x, (long)c10->pos_y, (long)c10->pos_z,
           (long)c10->target_x, (long)c10->target_y, (long)c10->target_z,
           (unsigned long)c10->pri_offset);
    CHECK((int32_t)c10->fov == SOLL_CUT10[0] && c10->pos_x == SOLL_CUT10[1] &&
          c10->pos_y == SOLL_CUT10[2] && c10->pos_z == SOLL_CUT10[3] &&
          c10->target_x == SOLL_CUT10[4] && c10->target_y == SOLL_CUT10[5] &&
          c10->target_z == SOLL_CUT10[6],
          "Cut-10-Kamera weicht von den RDT-Bytes ab");

    /* ================= M4: verdecken die Props den Hintergrund? ================= */
    printf("\n===== M4: Bildschirm-Bbox der Props unter Cut 10 (Screen %dx%d) =====\n",
           SCR_W, SCR_H);
    re15_camera_view_t view;
    int vok = (re15_camera_build_view(c10, &view) == 0);
    CHECK(vok, "re15_camera_build_view(cut10) fehlgeschlagen");
    printf("   fov_screen_dist=%d\n", (int)view.fov_screen_dist);

    const int cx = SCR_W / 2, cy = SCR_H / 2;
    int covering = 0, drawn = 0;
    for (int pi = 0; vok && pi < (int)g_scd.prop_count && pi < 16; pi++) {
        if (!g_scd.props[pi].active) continue;
        int oid = (int)g_scd.props[pi].obj_id;
        if (oid < 0 || oid >= rdt.prop_count || !rdt.prop_md1[oid]) continue;
        re15_md1_t md1;
        if (re15_md1_parse(rdt.prop_md1[oid], (size_t)rdt.prop_md1_size[oid], &md1) != 0) continue;

        /* Rigid-Prop = EIN "Bone" mit Identitaets-Rotation. Fuer die Frage
         * "verdeckt es den Bildschirm?" reicht die Bbox ohne rot_z (512/1536 kippt den
         * Schalter nur um seine eigene Achse und aendert die Bbox um <1 Prozent). */
        static const int32_t I9[9] = { 4096,0,0, 0,4096,0, 0,0,4096 };
        int32_t wt[3] = { g_scd.props[pi].x, g_scd.props[pi].y, g_scd.props[pi].z };
        int32_t rot[9], tr[3];
        re15_camera_compose_view_bone(&view, I9, wt, rot, tr);

        int minx = 1 << 28, maxx = -(1 << 28), miny = 1 << 28, maxy = -(1 << 28);
        int nvis = 0, nclip = 0;
        for (int m = 0; m < md1.mesh_count; m++) {
            const re15_md1_mesh_t *me = &md1.meshes[m];
            for (int v = 0; v < me->tri_vertex_count; v++) {
                int32_t _x = me->tri_vertices[v].x, _y = me->tri_vertices[v].y,
                        _z = me->tri_vertices[v].z;
                int32_t vx = (int32_t)(((int64_t)_x*rot[0] + (int64_t)_y*rot[1] + (int64_t)_z*rot[2]) >> 12) + tr[0];
                int32_t vy = (int32_t)(((int64_t)_x*rot[3] + (int64_t)_y*rot[4] + (int64_t)_z*rot[5]) >> 12) + tr[1];
                int32_t vz = (int32_t)(((int64_t)_x*rot[6] + (int64_t)_y*rot[7] + (int64_t)_z*rot[8]) >> 12) + tr[2];
                if (vz < 64) { nclip++; continue; }              /* Near-Clip wie main.c */
                int32_t ir1 = vx > 0x7FFF ? 0x7FFF : (vx < -0x8000 ? -0x8000 : vx);
                int32_t ir2 = vy > 0x7FFF ? 0x7FFF : (vy < -0x8000 ? -0x8000 : vy);
                uint32_t sz3 = (uint32_t)(vz > 0xFFFF ? 0xFFFF : vz);
                uint32_t n = re15_gte_divide((uint32_t)view.fov_screen_dist, sz3);
                int sx = cx + (int)(((int64_t)ir1 * (int64_t)n) >> 16);
                int sy = cy + (int)(((int64_t)ir2 * (int64_t)n) >> 16);
                if (sx < minx) minx = sx; if (sx > maxx) maxx = sx;
                if (sy < miny) miny = sy; if (sy > maxy) maxy = sy;
                nvis++;
            }
        }
        if (nvis == 0) { printf("   pi=%2d oid=0x%02X  keine sichtbaren Vertices (near-clip %d)\n",
                                pi, oid, nclip); continue; }
        drawn++;
        int full = (minx <= 0 && maxx >= SCR_W - 1 && miny <= 0 && maxy >= SCR_H - 1);
        if (full) covering++;
        printf("   pi=%2d oid=0x%02X pos=(%6ld,%5ld,%6ld) bbox x[%5d..%5d] y[%5d..%5d]"
               " (%3dx%3d px) %s\n",
               pi, oid, (long)g_scd.props[pi].x, (long)g_scd.props[pi].y,
               (long)g_scd.props[pi].z, minx, maxx, miny, maxy,
               maxx - minx + 1, maxy - miny + 1,
               full ? "**VOLLBILD - VERDECKT DEN HINTERGRUND**" : "");
    }
    printf("   sichtbare Props: %d, davon bildschirmfuellend: %d\n", drawn, covering);
    CHECK(covering == 0,
          "%d Prop(s) decken unter Cut 10 den ganzen Bildschirm ab -> schwarzer Hintergrund",
          covering);
    /* Der Cursor (Objekt 0x00, sub00 @0x0E54) und die 10 Schalter (0x02..0x0B @0x0E76..0x0FA8)
     * MUESSEN unter der Nadir-Kamera sichtbar sein — sonst gibt es gar keinen Cursor. */
    CHECK(drawn >= 11, "nur %d Props sichtbar (SOLL >= 11: Maschine + 10 Schalter)", drawn);

    printf("\n===== BEFUND: %s (%d Fehler) =====\n", g_fail ? "DIVERGENT" : "OK", g_fail);
    free(dat);
    return g_fail ? 1 : 0;
}
