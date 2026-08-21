/* test_fade_step0_hold.c — Kandidat 3 aus Commit 3b141d9d:
 * ROOM11F0-sub18 @0x174A traegt `56 00 02 07 00 00` = Sce_fade_set mit SCHRITTWEITE 0.
 * Vermutung war: "wenn der Port diesen Kanal kickt (also von sich aus weiterzaehlt oder ihn
 * beendet), haelt das Original statt dessen einen KONSTANTEN Overlay-Pegel" — das waere
 * buchstaeblich "das Bild aendert sich nicht", als PRAESENTATIONS-Effekt.
 *
 * Gemessen wird hier NICHT der Zustand allein, sondern der GERENDERTE Pegel: out_r/out_g/out_b
 * und `drawn` sind exakt die Werte, die platform/pc/src/render_pc.c:873ff in den Overlay-Fill
 * schiebt (dort: `if (!fc->drawn || (out_r==0&&out_g==0&&out_b==0)) continue;`).
 *
 * === ORIGINAL-BELEG (ghidra1_V2.txt, selbst nachdisassembliert) =========================
 * SCD 0x56 -> LAB_80042a58 (Tabelle PTR_LAB_800744a8[0x56]):
 *   80042a70  ori  a3,zero,0x7           ; bucket = 7 KONSTANT
 *   80042a74  lbu  a0,0x2(s0)            \ a0 = (pc[2]<<8) | pc[1]
 *   80042a78  lbu  v0,0x1(s0)            |
 *   80042a84  sll  a0,a0,0x8             |
 *   80042a8c  _or  a0,v0,a0              /
 *   80042a7c  lh   a1,0x4(s0)            ; SCHRITT = s16 @pc+4
 *   80042a80  lbu  a2,0x3(s0)            ; RGB-Maske = pc[3]
 *   80042a88  jal  FUN_800217b0
 *   80042a90  addiu s0,s0,0x6            ; pc += 6
 * FUN_800217b0 (= re15_fade_config):
 *   800217b4  move t0,a2                 ; Maske retten
 *   800217b8  srl  a2,a0,0x8             ; abr  = hi(a0)
 *   800217bc  andi a0,a0,0xff            ; chan = lo(a0)
 *   800217c0-d8  s0 = 0x800b5458 + chan*0x44
 *   800217e4  sh   a1,0x2(s0)            ; +0x02 = SCHRITT
 *   800217e8  sb   a2,0x4(s0)            ; +0x04 = abr
 *   800217f0  sb   a3,0x8(s0)            ; +0x08 = bucket
 *   800217f4-800 / 8002180c-18 / 80021824-30 : +0x05/06/07 = (t0&4/2/1) ? 0xff : 0
 *   -> +0x00 (der PEGEL) wird NICHT angefasst.
 * SCD 0x57 -> LAB_80042ab4: a0 = lbu pc[1], a1 = lhu pc[2], jal FUN_800216ec, pc += 4.
 * FUN_800216ec (= re15_fade_kick):
 *   80021704  lh   v0,0x2(v1)            ; SCHRITT
 *   8002170c  beq  v0,zero,LAB_80021724
 *   80021710  _slti v0,v0,0x1            \ Schritt != 0 -> Pegel = (Schritt<=0) ? 0x7fff : 0
 *   80021714-20  subu/andi 0x7fff/sh     /
 *   80021728  _sh  a1,0x0(v1)            ; Schritt == 0 -> Pegel = a1  (DER FALL HIER)
 * FUN_80021880 (Tick @0x80020f44, 4 Kanaele, stride 0x44):
 *   800218c0  lhu  v0,0x0(s0)            ; Pegel
 *   800218cc  bltz (Pegel<<16) -> LAB_800219d4   ; bit15 gesetzt = Kanal AUS
 *   800218d0  _sra a0,v0,0x17            ; Helligkeit = Pegel>>7
 *   800218e4/8fc/914  and  Helligkeit & Maske -> Prim-RGB
 *   8001c91c-2c  lhu Pegel; lhu Schritt; addu; sh  ; Pegel += Schritt  (KEIN Clamp)
 *   8002195c/8002199c/800219cc  AddPrim                    ; JEDES Bild gezeichnet
 * => SCHRITT 0 bedeutet: Pegel KONSTANT, Prim wird jedes Bild mit derselben Helligkeit
 *    gezeichnet, und der Kanal wird NIE fertig (FUN_8002178c = Pegel & 0x8000).
 *
 * === ROOM11F0 sub18, aus der ausgelieferten RDT (SCD-Sub-Tabelle @0x0D34, Eintrag 18) ====
 *   0x174A  56 00 02 07 00 00   chan 0, abr 2, Maske 7 (RGB), Schritt 0
 *   0x1750  57 00 00 08         Pegel = 0x0800 -> Helligkeit 0x0800>>7 = 16
 *   0x1754  09 0a 02 00         Sleep 2
 *   0x1758  57 00 00 00         Pegel = 0
 *   0x175C  09 0a 03 00         Sleep 3
 *   0x1760  57 00 00 08         Pegel = 0x0800
 *   0x1764  09 0a 04 00         Sleep 4
 *   0x1768  57 00 00 00         Pegel = 0
 *   0x176C  09 0a 03 00         Sleep 3
 * = ein GEHALTENES Lichtflackern in 4 Phasen. Danach bleibt Kanal 0 mit Pegel 0/Schritt 0
 * aktiv (bit15 frei) und zeichnet einen unsichtbaren Prim — auch im Original.
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
#include "re15_collision.h"
#include "re15_fade.h"
#include "re15_msg.h"

extern scd_vm_t g_scd;
extern int scd_thread_start(int slot, const uint8_t *pc);
extern uint16_t g_scd_pad_edge;
extern uint16_t g_scd_pad_held;

static int g_fail = 0;
static void chk(const char *what, long got, long want)
{
    if (got != want) { printf("FAIL %-48s got=%ld want=%ld\n", what, got, want); g_fail++; }
    else               printf("ok   %-48s %ld\n", what, got);
}

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

int main(void)
{
    size_t sz = 0;
    uint8_t *raw = read_file(RE15_ASSET_PSX_DIR "/STAGE1/ROOM11F0.RDT", &sz);
    if (!raw) { printf("SKIP: ROOM11F0.RDT fehlt\n"); return 77; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(raw, sz, &rdt) < 0) { printf("SKIP: RDT-Parse\n"); return 77; }

    /* ---- B: die BYTES, auf denen alles beruht ------------------------------------- */
    static const uint8_t WANT_56[6] = { 0x56, 0x00, 0x02, 0x07, 0x00, 0x00 };
    chk("B1 @0x174A = 56 00 02 07 00 00",
        (long)(memcmp(raw + 0x174A, WANT_56, 6) == 0), 1);
    chk("B2 @0x1750 = 57 00 00 08 (Pegel 0x0800)",
        (long)(raw[0x1750] == 0x57 && raw[0x1751] == 0x00 &&
               raw[0x1752] == 0x00 && raw[0x1753] == 0x08), 1);
    chk("B3 @0x1758 = 57 00 00 00 (Pegel 0)",
        (long)(raw[0x1758] == 0x57 && raw[0x1759] == 0x00 &&
               raw[0x175A] == 0x00 && raw[0x175B] == 0x00), 1);
    /* Sub-Tabelle @0x0D34: Eintrag 18 muss genau auf sub18 zeigen. */
    {
        uint32_t base = 0x0D34;
        uint32_t off  = (uint32_t)(raw[base + 36] | (raw[base + 37] << 8));  /* tab[18] */
        chk("B4 SCD-Sub-Tabelle[18] -> 0x16F6", (long)(base + off), 0x16F6);
    }

    /* ---- Aufbau: Raum laden, sub18 als Thread 4 starten ---------------------------- */
    re15_actor_init(); scd_vm_init(); re15_aot_init();
    re15_collision_reset_band();
    re15_fade_init();
    g_current_room_id = 0x11F0; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -1432; pl->y = 0; pl->z = -15600; pl->rot_y = 0;
    re15_collision_set_band(0);
    scd_room_reenter(&rdt, pl->x, pl->z, 0);

    /* Kanal 0 in den Auslieferungs-Ruhezustand: FUN_80021764 = Pegel 0xffff (Kanal AUS). */
    re15_fade_kill(0);
    for (int f = 0; f < 40; f++) { scd_vm_tick(); re15_fade_tick(); }
    chk("A0 Kanal 0 vor sub18 aus (bit15)", (long)re15_fade_done(0), 1);

    scd_thread_start(4, raw + 0x16F6);

    /* ---- 200 Bilder in der Reihenfolge des Ports: VM-Tick, Nachricht, Fade-Tick ----- */
    struct { uint16_t level; int16_t step; uint8_t r, g, b, drawn; } tl[200];
    int nvis = 0, nfr = 0;
    for (int f = 0; f < 200; f++) {
        scd_vm_tick();
        /* Die Nachricht wie ein Spieler wegbestaetigen (virtuell 0x4000 = physisch QUADRAT,
         * Tabelle @0x80073dbc[14]); sonst parkt sub18 an seinem Message_on(2) @0x1742. */
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        {
            static int mp = 0;
            if (g_scd.message_active && ((mp++ % 6) == 0)) g_scd_pad_edge = 0x4000;
            const unsigned char *mr = NULL; int ml = 0, mi = 0;
            re15_msg_tick(&mr, &ml, &mi);
        }
        re15_fade_tick();                       /* render_pc.c:873 */
        tl[f].level = g_fade_ch[0].level; tl[f].step = g_fade_ch[0].step;
        tl[f].r = g_fade_ch[0].out_r; tl[f].g = g_fade_ch[0].out_g;
        tl[f].b = g_fade_ch[0].out_b; tl[f].drawn = g_fade_ch[0].drawn;
        nfr++;
        if (tl[f].drawn && (tl[f].r | tl[f].g | tl[f].b)) nvis++;
    }

    /* ---- F1: Konfiguration byte-true uebernommen ----------------------------------- */
    chk("F1 Schritt == 0 (@0x800217e4 sh a1,0x2)",  (long)g_fade_ch[0].step, 0);
    chk("F1 abr == 2 (@0x800217e8 sb a2,0x4)",      (long)g_fade_ch[0].abr, 2);
    chk("F1 Maske R/G/B == 0xff (Maske 7)",
        (long)(g_fade_ch[0].mask_r == 0xFF && g_fade_ch[0].mask_g == 0xFF &&
               g_fade_ch[0].mask_b == 0xFF), 1);
    chk("F1 bucket == 7 (@0x80042a70 ori a3,0x7)",  (long)g_fade_ch[0].ot_bucket, 7);

    /* ---- F2: der Pegel DRIFTET NICHT. Bei Schritt 0 darf ueber die ganze Spur nur
     *          {0x0800, 0x0000} auftreten — jedes andere Wort waere ein Selbst-Weiterzaehlen. */
    {
        int bad = 0, ph = 0; unsigned phv[10]; int phlen[10];
        memset(phv, 0, sizeof phv); memset(phlen, 0, sizeof phlen);
        int cur = -1;
        for (int f = 0; f < nfr; f++) {
            /* Bei Schritt 0 darf NUR auftreten: der Ruhewert 0xFFFF (Kanal aus, vom
             * re15_fade_kill oben), 0x0800 und 0x0000 — die drei Werte, die die Bytes
             * vorgeben. Jedes andere Wort waere ein Selbst-Weiterzaehlen. */
            if (tl[f].level != 0x0800 && tl[f].level != 0x0000 && tl[f].level != 0xFFFF) bad++;
            if ((int)tl[f].level != cur) {
                cur = (int)tl[f].level;
                if (ph < 10) { phv[ph] = tl[f].level; phlen[ph] = 0; ph++; }
            }
            if (ph > 0 && ph <= 10) phlen[ph-1]++;
        }
        chk("F2 kein fremder Pegel (Schritt 0 = HALTEN)", (long)bad, 0);
        printf("     Phasen:");
        for (int i = 0; i < ph && i < 10; i++) printf(" 0x%04X x%d", phv[i], phlen[i]);
        printf("\n");
        /* GEMESSENE Spur = exakt die Bytes: 0xFFFF (aus) -> 0x0800 x2 (Sleep 2 @0x1754)
         * -> 0x0000 x3 (Sleep 3 @0x175C) -> 0x0800 x4 (Sleep 4 @0x1764) -> 0x0000 (Rest). */
        chk("F2 Phase 0 = 0xFFFF (Kanal war aus)",        (long)phv[0], 0xFFFF);
        chk("F2 Phase 1 = 0x0800 (@0x1750)",              (long)phv[1], 0x0800);
        chk("F2 Phase 1 haelt 2 Bilder (Sleep @0x1754)",  (long)phlen[1], 2);
        chk("F2 Phase 2 = 0x0000 (@0x1758)",              (long)phv[2], 0x0000);
        chk("F2 Phase 2 haelt 3 Bilder (Sleep @0x175C)",  (long)phlen[2], 3);
        chk("F2 Phase 3 = 0x0800 (@0x1760)",              (long)phv[3], 0x0800);
        chk("F2 Phase 3 haelt 4 Bilder (Sleep @0x1764)",  (long)phlen[3], 4);
        chk("F2 Phase 4 = 0x0000 (@0x1768)",              (long)phv[4], 0x0000);
        chk("F2 nach Phase 4 keine weitere Aenderung",    (long)(ph <= 5), 1);
    }

    /* ---- F3: der GERENDERTE Pegel — genau die Groessen, die render_pc.c ausgibt ----- */
    {
        int bad = 0;
        for (int f = 0; f < nfr; f++) {
            if (tl[f].level & 0x8000u) {                      /* @0x800218cc bltz -> kein AddPrim */
                if (tl[f].drawn) bad++;
                continue;
            }
            uint8_t want = (uint8_t)(tl[f].level >> 7);       /* @0x800218d0 sra v0,0x17 */
            if (tl[f].r != want || tl[f].g != want || tl[f].b != want) bad++;
            if (!tl[f].drawn) bad++;                          /* Kanal aktiv -> jedes Bild AddPrim */
        }
        chk("F3 out_rgb == Pegel>>7 in JEDEM aktiven Bild", (long)bad, 0);
        printf("     sichtbare Bilder (out != 0): %d von %d\n", nvis, nfr);
        chk("F3 Helligkeit der hellen Phasen == 16", (long)(0x0800 >> 7), 16);
        /* 2 + 4 = die beiden Sleeps hinter den hellen Kicks. Ein Kanal, der sich selbst
         * weiterzaehlt, kaeme hier nie auf genau 6. */
        chk("F3 genau 6 sichtbare Bilder (2 + 4)", (long)nvis, 6);
    }

    /* ---- F4: Endzustand — Kanal bleibt AKTIV mit Pegel 0 (kein Auto-Kill) ---------- */
    chk("F4 Endpegel == 0",                          (long)g_fade_ch[0].level, 0);
    chk("F4 Kanal NICHT beendet (bit15 frei)",       (long)re15_fade_done(0), 0);
    chk("F4 Endschritt weiterhin 0",                 (long)g_fade_ch[0].step, 0);
    /* F5: unsichtbar — genau die Abbruchbedingung aus render_pc.c:874. */
    chk("F5 Overlay unsichtbar (r|g|b == 0)",
        (long)((g_fade_ch[0].out_r | g_fade_ch[0].out_g | g_fade_ch[0].out_b) == 0), 1);

    printf(g_fail ? "\n%d FAIL\n" : "\nALLE OK\n", g_fail);
    return g_fail ? 1 : 0;
}
