/*
 * werfer_r35.c — Runde 35 Spur B: Werfer-Klasse (15..18), Flammenwerfer (14), Colt Python (20).
 * Belege: Dossier analysis/befunde_runde35/B_werfer.md §2 (RE1.5 PSX.EXE / RE2 info/re2leon/PSX.EXE,
 * selbst disassembliert mit re15_disasm.py / re2_disasm.py). Einordnung Beta -> Retail im Kopf von
 * include/re15_werfer.h.
 *
 * Was hier liegt:
 *   - die RE2-Effekt-Handler [9]/[10]/[11] (GL) und [17] (Rakete) als Bild-1-Spawns in die
 *     RE2-FX-Maschine (re2_fx.c, Ops 7/15/17/22/23/24/47/70 dort, Runde-35-Block),
 *   - der RE2-Handler [16] (Flammenstrahl) fuer die RE1.5-Dauerfeuer-Schleife,
 *   - Fuel-Takt des Flammenwerfers nach RE2 FUN_8006a0cc,
 *   - Bank-/Clip-Umsetzung fuer die Leon-Bank PL00W0F und die Nachlade-/Rueckstoss-Regeln.
 * Die Colt Python (20) braucht hier keinen Handler: ihre Entladung ist die RE1.5-Revolver-Zeile
 * 0x800339A4 (ENT[20] in game_step_common.c), Begruendung Dossier §3.5.
 */
#include <string.h>
#include <stdio.h>
#include "re15_werfer.h"
#include "re15_actor.h"
#include "re15_damage.h"
#include "re15_inventory.h"
#include "re2_fx.h"

extern re15_actor_t g_actors[];
extern int re15_player_granate_frame(void);        /* player_common.c: Rueckstoss-Bild oder -1 */
extern int re15_player_aim_elevation(void);        /* player_common.c: -1 / 0 / +1 */
extern int re15_player_gunbone_matrix(int32_t rot[9], int32_t t[3]);   /* re15_damage.c */
extern FILE *re15_waffen_log(void);                /* player_common.c (Mess-Harness) */

static inline void wr16(uint8_t *b, int o, uint32_t v) { b[o] = (uint8_t)v; b[o + 1] = (uint8_t)(v >> 8); }
static inline void wr32(uint8_t *b, int o, uint32_t v)
{ b[o] = (uint8_t)v; b[o + 1] = (uint8_t)(v >> 8); b[o + 2] = (uint8_t)(v >> 16); b[o + 3] = (uint8_t)(v >> 24); }

int re15_werfer_ist(int id)        { return id >= 15 && id <= 18; }
int re15_werfer_nachladbar(int id) { return id == 15 || id == 16 || id == 17 || id == 20; }
int re15_werfer_bank_id(int id)    { return (id == 16 || id == 17) ? 15 : id; }

int re15_werfer_clip_remap(int id, int clip_n, int clip)
{
    if (!(id >= 15 && id <= 17) || clip_n != 11) return clip;
    if (clip == 13) return 6;                      /* kein Nachlade-Clip in PL00W0F -> Hold (Bild 1) */
    if (clip >= 6 && clip <= 12) return clip - 2;  /* 6->4 Heben, 7->5 Feuer, 8->6 Hold, 9/10/11/12 -> 7/8/9/10 */
    return clip;
}

int re15_werfer_recoil_break(int id)
{
    /* @0x80074090 + (id-1)*5 byte 2: 3/4 = 7, 5..13 = 10, 14..18 und 20 = 0 (unfertige Saetze),
     * 19 = 10. PORT-WAHL fuer die unfertigen Saetze: 10 (Klasse der schweren Waffen). */
    if (id >= 15 && id <= 18) return 10;
    if (id == 20) return 10;
    return -1;                                     /* kein Eingriff */
}

/* ---- Waffenknochen-Matrix im RE2-Abbild (MATRIX: short m[3][3] @+0, long t[3] @+20) -------
 * Bleibt als statischer Puffer gueltig (spawn_kern haelt den Zeiger als +0x6C; gelesen nur bei
 * Status 0x800, das keiner unserer Effekte setzt). */
static uint8_t  s_mtx[32];
static unsigned s_spawns = 0;
static int      s_last_frame = -1, s_last_w = -1;
static uint16_t s_fuel_takt = 0;                   /* DAT_800d5c1c (RE2) */

/* ---- Waffenrahmen des Leon-Granatwerfers PL00W0F (PORT-ZUORDNUNG, Dossier §4.1 G1 / §3.7) ----
 * Die RE2-Handler rechnen im Rahmen der Knochenmatrix mit "+y = Lauf, +x = oben" (Versatz
 * {120,1200,0}, Geschwindigkeit der Runde (vx,vy,vz) mit vy = Vortrieb; FUN_8001d894 Status 0x400
 * @0x8001d960: Welt = t + M * (Versatz + lokal)). Das gilt fuer RE2 PL01W09 (Rumpf x 64..205,
 * Muendungsring (64,960,+-70) @0x194 der MD1), Elza PL04W0F (Ring (61..202,1153,+-70)), den
 * Flammenwerfer PL00W0E (= RE2 PL00W10 punktgleich) und den Raketenwerfer PL00W12 — NICHT fuer
 * Leons PL00W0F.PLW: dessen Netz (dir[2] MD1 @0x50A8) ist im Knochenrahmen um 35,62 Grad von +y
 * nach +x gedreht modelliert:
 *   Muendungsring (654,1343,-41) @0x518C, (758,1278,-41) @0x519C, (654,1343,48) @0x51AC,
 *                 (758,1278,48) @0x51BC                      -> Mitte (706, 1310.5, 3.5)
 *   Laufring      (326,886,62) @0x51C4, (326,886,-58) @0x51CC, (429,818,71) @0x5224,
 *                 (429,818,-65) @0x522C                      -> Mitte (377.5, 852, 2.5)
 *   Achse = (328.5, 458.5) / 564.0 -> sin 2386, cos 3330 (Q12).
 * Gemessen vorher: Laufachse des Knochens (3478,2079,-417)/4096 = 30,5 Grad ABWAERTS, die gerade
 * Runde faellt 304 je 509 Vortrieb. Rahmen des Netzes: x' = (cos,-sin,0), y' = (sin,cos,0):
 *   M' = M * [x' y' z]  (Spalte 0' = cos*Sp0 - sin*Sp1, Spalte 1' = sin*Sp0 + cos*Sp1).
 * Versatz im Netzrahmen = Ringmitte (x' -189.4, y' 1476.7, z 3.5) plus der RE2-Abstand zum
 * EIGENEN Ring: RE2 {120,1200,0} gegen Rumpfmitte 134.5 / Ring 960 = (-14.5, +240, 0)
 *   -> {-204, 1717, 3}.
 * Greift nur fuer die 11-Clip-Bank (Leon; dasselbe Kriterium wie re15_werfer_clip_remap). */
#define W0F_SIN 2386
#define W0F_COS 3330
static int s_bank_clips = 0;                       /* Clip-Zahl der gefuehrten Bank (aim_clip_wirksam) */

int re15_werfer_rahmen(int id, int clip_n, int32_t rot[9], int16_t ofs[4])
{
    if (!(id >= 15 && id <= 17) || clip_n != 11) return 0;
    for (int z = 0; z < 3; z++) {
        const int32_t x = rot[z * 3 + 0], y = rot[z * 3 + 1];
        rot[z * 3 + 0] = (int32_t)(((int64_t)W0F_COS * x - (int64_t)W0F_SIN * y) >> 12);
        rot[z * 3 + 1] = (int32_t)(((int64_t)W0F_SIN * x + (int64_t)W0F_COS * y) >> 12);
    }
    if (ofs) { ofs[0] = -204; ofs[1] = 1717; ofs[2] = 3; ofs[3] = 0; }
    return 1;
}

static int mtx_bauen_ofs(int16_t ofs[4])
{
    int32_t r[9], t[3];
    if (!re15_player_gunbone_matrix(r, t)) return 0;
    (void)re15_werfer_rahmen(re15_player_equipped_weapon(), s_bank_clips, r, ofs);
    for (int k = 0; k < 9; k++) {
        int32_t v = r[k];
        if (v > 32767) v = 32767; else if (v < -32768) v = -32768;
        wr16(s_mtx, 2 * k, (uint32_t)v);
    }
    s_mtx[18] = 0; s_mtx[19] = 0;
    wr32(s_mtx, 20, (uint32_t)t[0]); wr32(s_mtx, 24, (uint32_t)t[1]); wr32(s_mtx, 28, (uint32_t)t[2]);
    return 1;
}
static int mtx_bauen(void) { return mtx_bauen_ofs(NULL); }

static int spawn(uint32_t a0, int16_t a1, const int16_t ofs[4])
{
    int i = re2fx_spawn_sofort(a0, a1, s_mtx, ofs);   /* FUN_8001bf10 (sofort, 0xA003) */
    FILE *wl = re15_waffen_log();
    if (wl) {
        /* Mess-Harness: Knochenlage t (MATRIX +20) und Laufachse = Spalte 1 der Rotation (m[r][1]). */
        const int32_t tx = (int32_t)(s_mtx[20] | (s_mtx[21] << 8) | (s_mtx[22] << 16) | ((uint32_t)s_mtx[23] << 24));
        const int32_t ty = (int32_t)(s_mtx[24] | (s_mtx[25] << 8) | (s_mtx[26] << 16) | ((uint32_t)s_mtx[27] << 24));
        const int32_t tz = (int32_t)(s_mtx[28] | (s_mtx[29] << 8) | (s_mtx[30] << 16) | ((uint32_t)s_mtx[31] << 24));
        fprintf(wl, "    RE2SPAWN a0=%08x a1=%d ofs=(%d,%d,%d) platz=%d knochen=(%d,%d,%d) lauf=(%d,%d,%d)\n",
                (unsigned)a0, (int)a1, (int)ofs[0], (int)ofs[1], (int)ofs[2], i, (int)tx, (int)ty, (int)tz,
                (int)(int16_t)(s_mtx[2] | (s_mtx[3] << 8)), (int)(int16_t)(s_mtx[8] | (s_mtx[9] << 8)),
                (int)(int16_t)(s_mtx[14] | (s_mtx[15] << 8)));
    }
    if (i >= 0 && i < RE2FX_PLAETZE) s_spawns++;
    return i;
}

unsigned re15_werfer_spawns(void) { return s_spawns; }
void re15_werfer_reset(void) { s_spawns = 0; s_last_frame = -1; s_last_w = -1; s_fuel_takt = 0; }

/* Mess-Harness (nur mit RE15_WAFFEN_LOG): Weltlage (+0x34/36/38) der lebenden Geschoss-Plaetze je
 * Bild — Bank 2 (GL-Runde Skr 4, Rakete Skr 5) und Flammenstrahl (Bank 3 Skr 5). Kein Verhalten. */
static void flug_log(void)
{
    FILE *wl = re15_waffen_log();
    if (!wl) return;
    for (int i = 0; i < RE2FX_PLAETZE; i++) {
        const uint8_t *b = re2fx_platz(i);
        if (!b || !((b[0x18] | (b[0x19] << 8)) & 1)) continue;
        if (!(b[0x1C] == 2 || (b[0x1C] == 3 && (b[0x1E] & 7) == 5))) continue;
        /* kontakt = FUN_8004fba0-Abbild am Geschoss; fuss = dasselbe auf Standhoehe des Spielers
         * (liegt (x,z) in einer Zelle seines Bandes?) — zeigt, ob das Geschoss UEBER einer Wand fliegt. */
        const int32_t P[3] = { (int16_t)(b[0x34] | (b[0x35] << 8)), (int16_t)(b[0x36] | (b[0x37] << 8)),
                               (int16_t)(b[0x38] | (b[0x39] << 8)) };
        const int32_t Q[3] = { P[0], g_actors[RE15_ACTOR_SLOT_PLAYER].y - 1, P[2] };
        int k = 0, kf = 0;
        (void)re2fx_boden_sonde(P, 2, 8192u, 1, &k);
        (void)re2fx_boden_sonde(Q, 2, 8192u, 1, &kf);
        fprintf(wl, "    RE2FLUG platz=%d bank=%d sub=%d op=%d/%d welt=(%d,%d,%d) kontakt=%d fuss=%d\n", i, b[0x1C], b[0x1E],
                b[0], b[1], (int)P[0], (int)P[1], (int)P[2], k, kf);
    }
}

/* ---- GL [9]/[10]/[11] und Rakete [17], Rueckstossbild 1 ----------------------------------- */
void re15_werfer_tick(void)
{
    flug_log();
    int w = re15_player_equipped_weapon();
    if (!re15_werfer_ist(w)) { s_last_frame = -1; s_last_w = w; return; }
    int f = re15_player_granate_frame();            /* acae9 im Rueckstoss, sonst -1 */
    if (f < 0) { s_last_frame = -1; s_last_w = w; return; }
    if (f == s_last_frame && w == s_last_w) return; /* der Handler laeuft je Bild genau einmal */
    s_last_frame = f; s_last_w = w;
    if (f != 1) return;                             /* `lbu v1,333(s0) / addiu v0,zero,1 / bne` */
    /* GL: Versatz {120,1200,0} (`addiu v0,zero,120 / 1200` @0x80044bc4-d4 bzw. @0x80044f7c-8c);
     * Leons PL00W0F: Netzrahmen + Versatz aus re15_werfer_rahmen (oben). */
    int16_t OG[4] = { 120, 1200, 0, 0 };
    if (!mtx_bauen_ofs(w == 18 ? NULL : OG)) return;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int16_t gier = (int16_t)pl->rot_y;        /* RE2 `lh a1,118(s1)` = Spieler +0x76 */
    /* Bezugsebene des RE2-Bodentests (FUN_8004fba0: Grundebene y 0) = Standhoehe des Werfers —
     * PORT-ZUORDNUNG wie granate_boden (re15_esp.h) fuer RE1.5-Raeume mit Boden != 0. */
    re2fx_boden_basis_setzen(pl->y);

    if (w == 18) {
        /* Rakete 0x80045588: 0x01002000 a1=0 {0,1100,0} (@0x800455a8-d0); 0x020D1000 a1=Gier
         * (@0x800455d8-ec); 0x030A1A00 a1=0 (@0x800455f0-604); 0x030A1500 a1=0 {300,-900,0}
         * (@0x80045608-2c). */
        static const int16_t OM[4] = { 0, 1100, 0, 0 }, OR[4] = { 300, -900, 0, 0 };
        spawn(0x01002000u, 0, OM);
        spawn(0x020D1000u, gier, OM);
        spawn(0x030A1A00u, 0, OM);
        spawn(0x030A1500u, 0, OR);
        return;
    }
    /* GL: Muendung 0x01002000 a1 = 0 (@0x80044ba8-e4 / @0x80044f68-98). */
    spawn(0x01002000u, 0, OG);
    if (w == 15) {
        /* Explosiv 0x80044B44: 5 x 0x020C0A00 (Bank 2 Skr 4, CLUT-Zeile 1, Skala 0x0A00), a1 = Gier
         * (`lh a1,118(s0)` @0x80044bf4 usw.), danach je Platz acc.x (+0x08 = 0x800d8cf8+i*0x7C),
         * vel.x (+0x0C) aus der Tabelle @0x80011030 `00 00 88 ff 10 ff fa 00 c8 00 96 00` =
         * {0,-120,-240 | 250,200,150}, Zeile = Bit 15 des Zielworts (0x800cfd4c >> 15 = HOCH),
         * vel.y (+0x0E), vel.z (+0x10): Runde 1 @0x80044c18-64, 2 @0x80044c98-e8, 3 @0x80044d1c-6c,
         * 4 @0x80044da0-f0, 5 @0x80044e24-74 (r34-Dossier re_saeure_brand.md §2.2). */
        static const int16_t VX[2][3] = { { 0, -120, -240 }, { 250, 200, 150 } };
        static const struct { int8_t accx; uint8_t spalte; int16_t vy, vz; } R[5] = {
            { -10, 0, 600,    0 }, { -20, 1, 400,  100 }, { -20, 1, 400, -100 },
            { -40, 2, 250,   50 }, { -40, 2, 250,  -50 } };
        const int hoch = (re15_player_aim_elevation() > 0) ? 1 : 0;
        for (int k = 0; k < 5; k++) {
            int i = spawn(0x020C0A00u, gier, OG);
            if (i < 0 || i >= RE2FX_PLAETZE) continue;   /* `sltiu v0,a0,0xff / beq` @0x80044c04-08 */
            uint8_t *b = re2fx_platz_sonde(i);
            b[0x08] = (uint8_t)R[k].accx;
            wr16(b, 0x0C, (uint32_t)(uint16_t)VX[hoch][R[k].spalte]);
            wr16(b, 0x0E, (uint32_t)(uint16_t)R[k].vy);
            wr16(b, 0x10, (uint32_t)(uint16_t)R[k].vz);
        }
    } else {
        /* Brand 0x80044F44 / Saeure 0x80045090 (bytegleich): 0x020C1000 a1 = Gier (@0x80044f9c-b0),
         * 0x03081200 a1 = 0 (@0x80044fb4-c8). Die Art der Runde setzt Op 17 aus der Waffen-Id. */
        spawn(0x020C1000u, gier, OG);
        spawn(0x03081200u, 0, OG);
    }
}

/* ---- Flammenwerfer [16] 0x800454a0 ------------------------------------------------------- */
void re15_werfer_flamme_bild(int f)
{
    if (f < 0) return;
    if (f % 3 == 1) {                                /* Magic 0xAAAAAAAB, `bne v1,s1(=1)` @0x800454b8-e0 */
        if (mtx_bauen()) {
            static const int16_t OF[4] = { 150, 1200, 0, 0 };   /* @0x800454f0-508 */
            const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            re2fx_boden_basis_setzen(pl->y);         /* PORT-ZUORDNUNG wie oben */
            spawn(0x031D1200u, (int16_t)pl->rot_y, OF);   /* `lui a0,0x31d / ori 0x1200` @0x800454e4-e8 */
            /* 0x800DF349 := 1 (@0x80045518-20): Flammen-Latch, Leser @0x80026784/@0x8002693c
             * (Gegnerschleife) — im Port ohne Konsument (Dossier OFFEN). */
        }
    }
    if (f == 1)  re15_audio_re2_arms_se(0x10, 0);     /* 0x01000001 @0x80045534-44 (ARMS10 Satz 0) */
    if (f == 11) re15_audio_re2_arms_se(0x10, 11);    /* 0x010B0001 @0x80045550-6c (ARMS10 Satz 11) */
}

/* Leerschuss-Ton (Nachbesserung 1, M2). BEFUND: der zweite Abzug mit leerem Raketenwerfer war
 * STUMM — der RE1.5-Klick 0x01010001 trifft ARMS12 Satz 1 = ff ff ff ff (ARMS12.EDH Dateibytes
 * 4..7; die Bank hat nur Satz 0 `00 00 13 20` und Satz 10 `00 00 33 20` @0x28).
 * RE2 Retail (Beta -> Retail, Sound ist RE2): Waffe 17 laeuft ueber die Standard-FSM
 * (Dispatch-Tabelle @0x800a6f38 + 17*4 = @0x800a6f7c -> 0x80043230, Sub-Tabelle @0x800a7048[1] =
 * Haltezustand 0x8004362c). Dort:
 *   8004382c jal 0x80069f54 (Id & 0xfff)        Magazin > 0?  -> Zustand 2 (Feuer) @0x80043838-40
 *   80043844-54 lw 0x800ce310 / andi 0x40 / beq Tasten-FLANKE, sonst nichts
 *   8004385c jal 0x8006a23c                     Nachladen moeglich? Fuer Id 17 hart 0:
 *                                               `addiu v0,zero,17 / bne v1,v0 / addu v0,zero,zero`
 *                                               @0x8006a28c-9c
 *   80043864 beq v0,zero,0x80043894
 *   80043868 lui a0,0x101 / 80043894 ori a0,a0,0x1 / 80043898 jal 0x8005ba28 / addiu a1,s0,56
 *                                               = SE 0x01010001 an der Spielerlage
 * 0x01010001 = Bank 1 (ARMS der gefuehrten Waffe, FUN_80059c74) Satz 1; RE2 ARMS11.EDH Dateibytes
 * 4..7 = 00 00 54 16. (Derselbe Zweig spielt den Klick auch fuer die Granatwerfer 9/10/11:
 * `addiu v0,v0,-9 / sltiu v0,v0,0x3 / bne` @0x80043878-80 — RE2 laedt den GL NICHT per Abzug nach;
 * der Port laedt 15/16/17 nach RE1.5-Anlage nach, PORT-WAHL Dossier §3.5 Punkt 4.) */
int re15_werfer_leer_ton(int id)
{
    if (id != 18) return 0;
    re15_audio_re2_arms_se(0x11, 1);
    return 1;
}

int re15_werfer_fuel_bild(void)
{
    /* FUN_8006a0cc, Zweig Id 16 @0x8006a184-0x8006a21c (Delay-Slot-genau):
     *   8006a194 lhu v0,0(v1) / addiu 1 / sh         Zaehler++
     *   8006a1ac slti v0,v0,8 / bne -> return 1      < 8: nichts
     *   8006a1bc sh zero,0(v1)                       Zaehler := 0 (Delay-Slot von beq s0,zero)
     *   8006a1b8 beq s0,zero,0x8006a204              Menge 0 -> return 0
     *   8006a1c0 addiu s0,s0,-1 / jal 0x800694b8     Menge-1
     *   8006a1fc bne s0,zero,0x8006a20c              Menge jetzt 0 -> return 0 (0x8006a204)
     *   8006a20c addiu a1,s0,-1 / jal 0x800694b8     sonst Menge-1 nochmals, return 1 */
    s_fuel_takt++;
    if ((int16_t)s_fuel_takt < 8) return 1;
    s_fuel_takt = 0;
    if (!re15_ammo_mag_nonzero()) return 0;
    re15_ammo_consume();
    if (!re15_ammo_mag_nonzero()) return 0;
    re15_ammo_consume();
    return 1;
}

/* ---- Inventar-Kombination: Nachladen + Munitionswechsel (Runde 35 Spur B) -------------------
 * BEFUND RE1.5 (re15_disasm.py bytes 0x80074c88 / 0x80074da8): die Waffen-Zeilen 15..18/20 der
 * Eigenschaftstabelle tragen pair_count 0 (+9) und den NULL-Satz 0x80074c88 — der Matcher
 * FUN_8004e900 (@0x8004e9ec `beq cnt,zero`) lehnt JEDE Kombination ab. Die Saetze des
 * Granatwerfers liegen unverwiesen daneben: 0x80074cb4 `19 0f 02 00` (EXPLOSIVE RND + GL =
 * Nachladen, Aktion 2), 0x80074cb8 `1a 10 04 0c`, 0x80074cbc `1b 11 04 0c` (ACID/INCEND. ->
 * GL 0x10/0x11, Aktion 4, Bild 0x0c). Aktion 4 (@0x8004e538) schreibt die Ergebnis-Id in den
 * PARTNER-Platz und laesst die Munition unverbraucht (Beta-Rest, unerreichbar) -> Beta ->
 * Retail: der Munitionswechsel ist der RE2-Zustand 7/8 der Kombinier-Maschine FUN_8006b358
 * (Sprungtabelle @0x80011bb0: [7] 0x8006bc18, [8] 0x8006bd98), RE2-Saetze @0x800a9d10..
 * (`18 05 09 ff | 19 07 09 03 | 1a 07 09 03` = eigene Runde Nachladen, fremde Runde Tausch;
 * Gegenrichtung @0x800a9d88 `09 06 09 ff | 0a 08 09 04 | 0b 08 09 05`).
 * RE1.5-Ids: Runde = GL + 0x0a (0x19/0x1a/0x1b <-> 0x0f/0x10/0x11, aus den drei Saetzen oben;
 * RE2: +15). Bild der ZURUECKKOMMENDEN Runde = MIXITEM-Bild 0x0c/0x0d/0x0e (RE1.5-Saetze
 * @0x80074d24 `1e 19 06 0c`, @0x80074d28 `1f 1a 06 0d`, @0x80074d2c `20 1b 06 0e`; der
 * unverwiesene GL-Satz der 0x0f traegt genau 0x0c = Bild der EXPLOSIVE RND).
 * Nachladen = RE1.5-Aktionen 2/3 (x_reload, Magazin aus +0 der Zeile = 6): Form der fertigen
 * Saetze `17 07 02 00` @0x80074c9c / `07 07 03 00` @0x80074cf8 (Redhawk <-> MAGNUM).
 * Colt Python + MAGNUM BULLETS: dieselbe Form (PORT-WAHL Magnum-Revolver, Dossier §3.5).
 * Rueckgabe = Aktion (0 = kein Paar dieser Klasse -> der RE1.5-Matcher laeuft weiter). */
int re15_werfer_paar(uint8_t id_a, uint8_t id_b, uint8_t *result, uint8_t *pic)
{
    const int gl_a = (id_a >= 15 && id_a <= 17), gl_b = (id_b >= 15 && id_b <= 17);
    const int rd_a = (id_a >= 0x19 && id_a <= 0x1b), rd_b = (id_b >= 0x19 && id_b <= 0x1b);
    if (gl_a && rd_b) {
        *result = id_a;                                   /* RE2 Feld 2 = eigene Id (`.. .. 09 ..`) */
        if (id_b == id_a + 0x0a) { *pic = 0; return 2; }  /* `19 0f 02 00` @0x80074cb4 */
        *pic = (uint8_t)(0x0c + (id_a - 15));             /* Bild der alten Runde (RE2 `.. 07 09 03`) */
        return 7;                                         /* RE2-Zustand 7 @0x8006bc18 */
    }
    if (rd_a && gl_b) {
        if (id_a == id_b + 0x0a) { *result = id_b; *pic = 0; return 3; }   /* Form `07 07 03 00` */
        *result = (uint8_t)(id_a - 0x0a);                 /* RE2 `0a 08 09 04`: GL der Runde A */
        *pic = (uint8_t)(0x0c + (id_b - 15));
        return 8;                                         /* RE2-Zustand 8 @0x8006bd98 */
    }
    if (id_a == 20 && id_b == 0x17) { *result = 20; *pic = 0; return 2; }
    if (id_a == 0x17 && id_b == 20) { *result = 20; *pic = 0; return 3; }
    return 0;
}

/* RE2-Zustand 7 @0x8006bc18 (Zustand 8 @0x8006bd98 = derselbe Tausch mit vertauschten Cursorn):
 *   8006bc5c lbu s0,id[A]                          alte GL-Id
 *   8006bc60 addiu v0,v0,-15 / 8006bc6c sb id[A]   GL := Runde B - 15      (RE1.5: - 0x0a)
 *   8006bc70-98                                    gefuehrte Waffe nachziehen (Port: equip_id_now
 *                                                  beim Schliessen, menu_common.c)
 *   8006bca0 addiu v1,s0,15 / 8006bcb4 sb id[B]    Runde := alte GL + 15   (RE1.5: + 0x0a)
 *   8006bcd8-0x8006bd0c                            Menge[A] <-> Menge[B]
 *   8006bd30 beq Menge[B],zero -> 8006bd6c         leer: FUN_8006947c(B,0,0,0) + Zelle leeren
 *   8006bd38-60 jal 0x80069bb4                     sonst Bild der Runde in Zelle B
 * Der Werfer nimmt wie in RE2 den GANZEN Stapel der neuen Runde (keine Kappe im Zustand 7);
 * das RE1.5-Magazin 6 (@0x80074e5c/e68/e74) gilt nur beim Nachladen. Breite Waffe (Zellen-Flags 1/2,
 * FUN_8004dc4c): die Schwanzzelle fuehrt Id/Menge der Kopfzelle mit. */
void re15_werfer_gl_tausch(int gl_slot, int rd_slot, int pic)
{
    extern void re15_inv_icon_mix_upload(int cell, int pic);
    extern void re15_inv_icon_blank(int cell);
    if (gl_slot < 0 || rd_slot < 0 || gl_slot >= RE15_INV_MAX_SLOTS || rd_slot >= RE15_INV_MAX_SLOTS) return;
    re15_inv_slot_t *a = &g_inv.slots[gl_slot], *b = &g_inv.slots[rd_slot];
    const uint8_t alt = a->id, menge = a->qty;
    a->id  = (uint8_t)(b->id - 0x0a);
    b->id  = (uint8_t)(alt + 0x0a);
    a->qty = b->qty;
    b->qty = menge;
    if (gl_slot + 1 < RE15_INV_MAX_SLOTS && g_inv.slots[gl_slot + 1].flags == 2) {
        g_inv.slots[gl_slot + 1].id  = a->id;
        g_inv.slots[gl_slot + 1].qty = a->qty;
        re15_inv_icon_blank(gl_slot + 1);
    }
    re15_inv_icon_blank(gl_slot);                         /* Zelle folgt der neuen Id */
    if (b->qty == 0) { b->id = 0; b->flags = 0; re15_inv_icon_blank(rd_slot); }
    else             re15_inv_icon_mix_upload(rd_slot, pic);
}

/* Fuer player_common.c (aim_clip_wirksam): Clip der gefuehrten Waffe gegen die Bank-Clipzahl. */
int re15_player_werfer_clip(int clip, int clip_n)
{
    s_bank_clips = clip_n;                          /* fuer re15_werfer_rahmen (PL00W0F = 11 Clips) */
    return re15_werfer_clip_remap(re15_player_equipped_weapon(), clip_n, clip);
}
