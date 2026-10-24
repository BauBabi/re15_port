/*
 * Runde 41 Spur K "stage3szenen_b": Riegel fuer die Spielfigur SHERRY (Modell 0x0F) und die ROOM3091-Szene.
 * Dossier analysis/befunde_runde41/K_stage3szenen_b.md, Code engine/src/sherry_r41.c, platform/pc/src/sherry_pc_r41.c,
 * include/re15_sherry_r41.h.
 *
 *   pins     Original-Bytes: PSX.EXE (Datei-Tabellen @0x80073f70/@0x800741e8, Waffe 0 @0x800316b8-c4, kein Hand-Mesh
 *            @0x80036c08, Bewegungs-Zeile 15 @0x80073ec2/@0x80073ee2/@0x80073f02/@0x80073f42, Ziel-Sperren
 *            @0x80032014/@0x80032430/@0x80032720/@0x80032a80/@0x80032cb4), ROOM3061 @0x011F6 Save 0x10=0x0F,
 *            ROOM3091 @0x01D9C/@0x01DA8 zurueck auf Elza, RBJ 3091 rec2 Clips 11..13 Kindhoehe / 0..10 Erwachsen.
 *   waffe    Engine: Modellwechsel 4 -> 0x0F setzt Waffe 0 (@0x800316c4), Ausruesten gesperrt, Tabelle Zeile 15;
 *            zurueck -> vorherige Waffe (PORT-WAHL), Leon/Elza-Werte unveraendert.
 *   koerper  PC-Modul am echten PL04/PL0F: Koerper (EDD/EMR) + W00-Bank + Hand-Meshes aus, Wunden geloescht, CORE 0x0F;
 *            zurueck auf Elza stellt alles wieder her; gleiche Familie (Elza 4->5, Leon 0->1 Weste) aendert nichts.
 *   sherry <debug.log> <state.txt>   Auswertung integration_r41_stage3szenen_b_sherry (echte exe, Schacht-Tuer 3061 -> 30C1):
 *            Koerper-/Waffen-Zeilen, Knochenlage (Kind), Gehen 50 / Rennen 120 je Bild (= Original-Messung
 *            K_belege/k1_orig_tempo_30c1.txt / k1_orig_rennen_30c1.txt), kein Zielen mit R1.
 *   szene <anim.txt> <debug.log>     Auswertung integration_r41_stage3szenen_b_3091 (echte exe, Szene ueber die Riegeltuer):
 *            Sub-0-Clipfolge je Aktor = ORIGINAL (DuckStation-GDB, K_belege/k2_orig_3091_gdb_lauf2.log.gz).
 */
#include "re15_sherry_r41.h"
#include "sherry_pc_r41.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_damage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif
#ifndef RE15_PSX_EXE
#error RE15_PSX_EXE fehlt
#endif

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { g_fail++; printf("  FAIL: " __VA_ARGS__); printf("\n"); } \
                              else { printf("  ok:   " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *datei(const char *pfad, long *n)
{
    FILE *f = fopen(pfad, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); *n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)*n + 1);
    if (b && fread(b, 1, (size_t)*n, f) != (size_t)*n) { free(b); b = NULL; }
    fclose(f); return b;
}
static uint8_t *psx(const char *rel, long *n)
{
    char p[512]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, rel); return datei(p, n);
}
static uint32_t le32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }
static uint16_t le16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static int16_t  ls16(const uint8_t *p) { return (int16_t)le16(p); }

/* ---- Stubs der Plattform (sherry_pc_r41.c ruft sie) ---- */
static int g_core_idx = -1;
void re15_audio_prime_core(int idx) { g_core_idx = idx; }
uint8_t *re15_pc_read_any(const char *rel, int *size)
{
    long n = 0; uint8_t *b = psx(rel, &n);
    if (size) *size = (int)n;
    return b;
}

/* ---------- pins ---------- */
static const uint8_t *exe_at(const uint8_t *exe, long n, uint32_t addr)
{
    uint32_t t = le32(exe + 0x18);
    long o = 0x800 + (long)(addr - t);
    return (o >= 0 && o < n) ? exe + o : NULL;
}
static int bytes_gleich(const uint8_t *exe, long n, uint32_t addr, const char *hex)
{
    const uint8_t *p = exe_at(exe, n, addr);
    if (!p) return 0;
    for (size_t i = 0; hex[i * 3] && hex[i * 3 + 1]; i++) {
        unsigned v; if (sscanf(hex + i * 3, "%2x", &v) != 1) return 0;
        if (p[i] != (uint8_t)v) return 0;
        if (!hex[i * 3 + 2]) break;
    }
    return 1;
}
static void t_pins(void)
{
    long n = 0; uint8_t *exe = datei(RE15_PSX_EXE, &n);
    CHECK(exe != NULL, "PSX.EXE lesbar (%s)", RE15_PSX_EXE);
    if (!exe) return;
    /* Datei-Tabellen + CD-Dateitabelle @0x8006f440 (Eintrag id-1: u32 Groesse @+4) gegen die PLD-/PLW-Dateien */
    const uint8_t *pld = exe_at(exe, n, 0x80073f70 + 15 * 2), *plw = exe_at(exe, n, 0x800741e8 + 15 * 2);
    CHECK(pld && le16(pld) == 0x004B, "@0x80073f70[15] = 0x%04x (Soll 0x004B = Datei 75)", pld ? le16(pld) : 0);
    CHECK(plw && le16(plw) == 118, "@0x800741e8[15] = %u (Soll 118 = PL0FW00)", plw ? le16(plw) : 0);
    long s0 = 0, s1 = 0; uint8_t *a = psx("PLD/PL0F.PLD", &s0), *b = psx("PLD/PL0FW00.PLW", &s1);
    const uint8_t *e74 = exe_at(exe, n, 0x8006f440 + 74 * 8), *e117 = exe_at(exe, n, 0x8006f440 + 117 * 8);
    CHECK(a && e74 && (long)le32(e74 + 4) == s0, "CD-Eintrag 74 Groesse %u == PLD/PL0F.PLD %ld B", e74 ? le32(e74 + 4) : 0, s0);
    CHECK(b && e117 && (long)le32(e117 + 4) == s1, "CD-Eintrag 117 Groesse %u == PLD/PL0FW00.PLW %ld B", e117 ? le32(e117 + 4) : 0, s1);
    free(a); free(b);
    CHECK(bytes_gleich(exe, n, 0x80031694, "0f 00 02 34") && bytes_gleich(exe, n, 0x800316b8, "03 00 62 14") &&
          bytes_gleich(exe, n, 0x800316c4, "5d ca 20 a0"),
          "FUN_800314b0: ori v0,0xf @0x80031694 / bne v1,v0 @0x800316b8 / sb zero,DAT_800aca5d @0x800316c4 (Modell 0x0F -> Waffe 0)");
    CHECK(bytes_gleich(exe, n, 0x80036bf8, "5d ca 63 90") && bytes_gleich(exe, n, 0x80036c08, "65 00 60 10"),
          "FUN_80036b68: lbu v1,DAT_800aca5d @0x80036bf8 / beq v1,zero,0x80036da0 @0x80036c08 (Waffe 0 -> kein Hand-Mesh)");
    CHECK(bytes_gleich(exe, n, 0x80073ec2, "32 30") && bytes_gleich(exe, n, 0x80073ee2, "23 30") &&
          bytes_gleich(exe, n, 0x80073f02, "00 50") && bytes_gleich(exe, n, 0x80073f42, "78 40"),
          "Zeile 15: GEHEN [50,48] @0x80073ec2, RUECK [35,48] @0x80073ee2, DREHEN [0,80] @0x80073f02, RENNEN [120,64] @0x80073f42");
    { static const uint32_t ad[5] = { 0x80032014, 0x80032430, 0x80032720, 0x80032a80, 0x80032cb4 };
      int ok = 1;
      for (int i = 0; i < 5; i++) {
          const uint8_t *p = exe_at(exe, n, ad[i]);
          const uint8_t *l = exe_at(exe, n, ad[i] - 8);
          uint32_t w = p ? le32(p) : 0;
          if (!p || !l || !(l[0] == 0x5d && l[1] == 0xca && l[2] == 0x42 && l[3] == 0x90)) ok = 0;   /* lbu v0,DAT_800aca5d */
          if (((w >> 26) != 4 && (w >> 26) != 5) || ((w >> 21) & 31) != 2 || ((w >> 16) & 31) != 0) ok = 0; /* beq/bne v0,zero */
      }
      CHECK(ok, "Ziel nur mit Waffe != 0: lbu v0,DAT_800aca5d + beq/bne v0,zero an @0x80032014/30/720/a80/cb4"); }
    free(exe);
    /* Raumskripte */
    long r1 = 0, r2 = 0;
    uint8_t *d3061 = psx("STAGE3/ROOM3061.RDT", &r1), *d3091 = psx("STAGE3/ROOM3091.RDT", &r2);
    CHECK(d3061 && r1 > 0x11FC && memcmp(d3061 + 0x011F6, "\x24\x10\x0f\x00\x47\x03", 6) == 0,
          "ROOM3061 @0x011F6 `24 10 0f 00` (work_vars[0x10]=0x0F) + @0x011FA `47 03` (Aot_on Schacht-Tuer)");
    CHECK(d3091 && r2 > 0x1DAC && memcmp(d3091 + 0x01D9C, "\x24\x10\x04\x00", 4) == 0 &&
          memcmp(d3091 + 0x01DA8, "\x24\x10\x05\x00", 4) == 0, "ROOM3091 @0x01D9C/@0x01DA8 `24 10 04/05 00` = zurueck auf Elza");
    if (d3091) {   /* RBJ rec2: Clip 11..13 Wurzel-y ~ -1257..-1261 (Kind), 0..10 ~ -1678..-1723 (Erwachsen) */
        uint32_t s = le32(d3091 + 0x5C), total = le32(d3091 + s);
        uint32_t pre = le32(d3091 + s + total + 2 * 8), edd = le32(d3091 + s + total + 2 * 8 + 4);
        const uint8_t *emr = d3091 + s + pre + 4, *e = d3091 + s + edd;
        uint16_t o_frm = le16(emr + 2), ksz = le16(emr + 6);
        int kind = 1, erw = 1;
        for (int c = 0; c < 14; c++) {
            uint16_t off = le16(e + c * 4 + 2);
            int kf = (int)(le32(e + off) & 0xFFF);
            int y = ls16(emr + o_frm + kf * ksz + 2);
            if (c >= 11 && (y < -1300 || y > -1200)) kind = 0;
            if (c <= 10 && (y > -1600)) erw = 0;
        }
        CHECK(kind && erw, "ROOM3091 RBJ rec2: Clips 11..13 Kindhoehe (Sherry = Spieler), 0..10 Erwachsenenhoehe (Elza = Gegner 1)");
    }
    free(d3061); free(d3091);
}

/* ---------- waffe ---------- */
static void t_waffe(void)
{
    re15_sherry_r41_modell_init(4);
    re15_player_set_equipped_weapon(3);
    CHECK(re15_player_equipped_weapon() == 3, "Elza traegt Waffe 3 (Vorbedingung)");
    CHECK(re15_sherry_r41_tabelle(3, 0, 200) == 200 && re15_sherry_r41_tabelle(2, 1, 96) == 96,
          "Elza: Port-Werte unveraendert (Rennen 200, Drehen 96)");
    re15_sherry_r41_modell_setzen(15);
    CHECK(re15_sherry_r41_modell() == 15, "Modell 0x0F gebunden");
    CHECK(re15_player_equipped_weapon() == 0, "Waffe 0 nach dem Wechsel (@0x800316c4)");
    re15_player_set_equipped_weapon(3);
    CHECK(re15_player_equipped_weapon() == 0 && re15_sherry_r41_waffe_gesperrt(), "Ausruesten gesperrt solange Sherry");
    CHECK(re15_sherry_r41_tabelle(0, 0, 75) == 50 && re15_sherry_r41_tabelle(1, 0, 70) == 35 &&
          re15_sherry_r41_tabelle(2, 1, 96) == 80 && re15_sherry_r41_tabelle(3, 0, 200) == 120 &&
          re15_sherry_r41_tabelle(3, 1, 72) == 64 && re15_sherry_r41_tabelle(0, 1, 48) == 48,
          "Zeile 15: Gehen 50, Rueck 35, Drehen 80, Rennen 120/64, Geh-Drehung 48");
    re15_sherry_r41_modell_setzen(4);
    CHECK(re15_player_equipped_weapon() == 3 && !re15_sherry_r41_waffe_gesperrt(), "zurueck auf Elza: Waffe 3 wieder da (PORT-WAHL)");
    re15_sherry_r41_modell_setzen(5);
    CHECK(re15_player_equipped_weapon() == 3, "Elza 4 -> 5 (gleiche Familie): Waffe bleibt");
}

/* ---------- koerper ---------- */
#define WMAX 21
static re15_emd_skeleton_t  g_skel, g_pl00_skel, g_w01_skel, g_wpn_skel[WMAX];
static re15_emd_animation_t g_anim, g_pl00_anim, g_w01_anim, g_wpn_anim[WMAX];
static int g_bank_ok[WMAX], g_md1_ok[WMAX];

static int teil(const uint8_t *b, long n, int idx, const uint8_t **p, size_t *len)
{
    unsigned long o = 0, l = 0;
    if (!re15_pld_part(b, n, idx, &o, &l)) return 0;
    *p = b + o; *len = (size_t)l; return 1;
}
/* Boot wie main.c: Koerper aus PL<fam>.PLD, W-Banken PL<fam>Wxx (Verbund Bindelage + W-Keyframes). */
static int boot(const char *fam)
{
    char nm[64]; long n = 0;
    snprintf(nm, sizeof nm, "PLD/%s.PLD", fam);
    uint8_t *b = psx(nm, &n); if (!b) return 0;
    const uint8_t *p; size_t l;
    if (!teil(b, n, RE15_PLD_EDD, &p, &l) || re15_emd_parse_animation(p, l, &g_anim) != 0) return 0;
    if (!teil(b, n, RE15_PLD_EMR, &p, &l) || re15_emd_parse_skeleton(p, l, &g_skel) != 0) return 0;
    g_pl00_skel = g_skel; g_pl00_anim = g_anim;
    for (int w = 0; w < WMAX; w++) {
        g_bank_ok[w] = 0; g_md1_ok[w] = 0;
        snprintf(nm, sizeof nm, "PLD/%sW%02X.PLW", fam, w);
        long wn = 0; uint8_t *wb = psx(nm, &wn); if (!wb) continue;
        re15_emd_skeleton_t raw;
        if (teil(wb, wn, RE15_PLD_EDD, &p, &l) && re15_emd_parse_animation(p, l, &g_wpn_anim[w]) == 0 &&
            teil(wb, wn, RE15_PLD_EMR, &p, &l) && re15_emd_parse_skeleton(p, l, &raw) == 0) {
            g_wpn_skel[w] = g_skel;
            g_wpn_skel[w].keyframe_data = raw.keyframe_data; g_wpn_skel[w].keyframe_data_size = raw.keyframe_data_size;
            g_wpn_skel[w].keyframe_count = raw.keyframe_count; g_wpn_skel[w].keyframe_size_bytes = raw.keyframe_size_bytes;
            g_bank_ok[w] = 1; g_md1_ok[w] = 1;
        }
    }
    g_w01_anim = g_wpn_anim[1]; g_w01_skel = g_wpn_skel[1];
    return 1;
}
static void anmelden(int modell)
{
    re15_sherry_pc_r41_baenke_t kb = { &g_skel, &g_pl00_skel, &g_w01_skel, &g_anim, &g_pl00_anim, &g_w01_anim,
                                       g_wpn_anim, g_wpn_skel, g_bank_ok, g_md1_ok, WMAX };
    re15_sherry_pc_r41_anmelden(&kb, modell);
}
static int clips_sind(const re15_emd_animation_t *a, const int *soll, int n)
{
    if (a->clip_count < n) return 0;
    for (int i = 0; i < n; i++) if (a->clips[i].frame_count != soll[i]) return 0;
    return 1;
}
static void t_koerper(void)
{
    CHECK(boot("PL04"), "Elza-Boot aus PL04.PLD + PL04W00..14.PLW");
    anmelden(4);
    re15_player_set_equipped_weapon(3);
    re15_wound_add(2, 5);
    int vor_ok = 0; for (int w = 0; w < WMAX; w++) vor_ok += g_bank_ok[w] + g_md1_ok[w];
    long n = 0; uint8_t *f = psx("PLD/PL0F.PLD", &n);
    g_core_idx = -1;
    int r = re15_sherry_pc_r41_koerper(15, f, (int)n);
    CHECK(r == 1, "Wechsel 4 -> 0x0F bindet neu (Rueckgabe %d)", r);
    CHECK(g_pl00_skel.bone_count == 15 && g_pl00_skel.bone_relative_pos[0][1] == -1268,
          "Koerper-EMR = PL0F: %d Knochen, Wurzel-Bind y %d (Soll 15 / -1268; Elza -1709)",
          g_pl00_skel.bone_count, g_pl00_skel.bone_relative_pos[0][1]);
    { static const int soll[6] = { 28, 28, 133, 30, 20, 38 };
      CHECK(g_pl00_anim.clip_count == 24 && clips_sind(&g_pl00_anim, soll, 6), "Koerper-EDD = PL0F: 24 Clips {28,28,133,30,20,38,...}"); }
    CHECK(g_skel.bone_relative_pos[0][1] == -1268 && g_anim.clip_count == 24, "def-Bank = neue Basis (RBJ legt sich danach darueber)");
    { static const int soll[9] = { 20, 1, 40, 1, 1, 28, 15, 1, 8 };
      int nur0 = g_bank_ok[0] == 1, kein_mesh = 1;
      for (int w = 1; w < WMAX; w++) if (g_bank_ok[w]) nur0 = 0;
      for (int w = 0; w < WMAX; w++) if (g_md1_ok[w]) kein_mesh = 0;
      CHECK(nur0 && clips_sind(&g_wpn_anim[0], soll, 9) && g_wpn_anim[0].clip_count == 9,
            "W-Bank 0 = PL0FW00 (9 Clips {20,1,40,1,1,28,15,1,8}), keine andere Bank (@0x800741e8[15] = 118)");
      CHECK(g_wpn_skel[0].bone_relative_pos[0][1] == -1268 && g_wpn_skel[0].keyframe_count == 115,
            "W-Verbund: Bindelage PL0F + 115 Keyframes der PL0FW00");
      CHECK(kein_mesh, "kein Hand-/Waffen-Mesh (Waffe 0, @0x80036c08)");
      CHECK(clips_sind(&g_w01_anim, soll, 9), "Geh-Quelle (w01) = W00 der Familie"); }
    CHECK(re15_player_equipped_weapon() == 0, "Waffe 0 (@0x800316c4)");
    CHECK(re15_wound_level(2) == 0, "Wunden geloescht (FUN_80037c1c @0x800316c8)");
    CHECK(g_core_idx == 15, "CORE-Bank des Modells 0x0F (FUN_800440c4 @0x800316e8 -> CORE0F)");
    free(f);
    /* zurueck auf Elza (ROOM3091 @0x01D9C) */
    f = psx("PLD/PL04.PLD", &n);
    r = re15_sherry_pc_r41_koerper(4, f, (int)n);
    int nach_ok = 0; for (int w = 0; w < WMAX; w++) nach_ok += g_bank_ok[w] + g_md1_ok[w];
    { static const int soll[3] = { 34, 34, 50 };
      CHECK(r == 1 && g_pl00_skel.bone_relative_pos[0][1] == -1709 && clips_sind(&g_pl00_anim, soll, 3) &&
            g_skel.bone_relative_pos[0][1] == -1709 && nach_ok == vor_ok,
            "zurueck auf Elza: Bind -1709, Clips {34,34,50,...}, alle %d W-Banken/Meshes wieder da", vor_ok); }
    CHECK(re15_player_equipped_weapon() == 3 && g_core_idx == 4, "Waffe 3 zurueck, CORE04");
    free(f);
    /* gleiche Familie: Elza 4 -> 5 (EDD/EMR md5-gleich) */
    f = psx("PLD/PL05.PLD", &n);
    g_core_idx = -1;
    r = re15_sherry_pc_r41_koerper(5, f, (int)n);
    CHECK(r == 0 && g_pl00_skel.bone_relative_pos[0][1] == -1709 && g_core_idx == -1, "Elza 4 -> 5: nichts neu gebunden");
    free(f);
    /* Leon + Weste (Raumlader-Weg ROOM1190) */
    CHECK(boot("PL00"), "Leon-Boot");
    anmelden(0);
    f = psx("PLD/PL01.PLD", &n);
    int16_t y0 = g_pl00_skel.bone_relative_pos[0][1];
    r = re15_sherry_pc_r41_koerper(1, f, (int)n);
    CHECK(r == 0 && g_pl00_skel.bone_relative_pos[0][1] == y0 && g_bank_ok[3] == 1,
          "Leon 0 -> 1 (Weste): Koerper/W-Banken unveraendert (Bind y %d)", y0);
    free(f);
}

/* ---------- Auswertungen der Integrationslaeufe ---------- */
static char *text(const char *pfad) { long n = 0; uint8_t *b = datei(pfad, &n); if (b) b[n] = 0; return (char *)b; }

static void t_sherry(const char *dlog, const char *slog)
{
    char *d = text(dlog), *st = text(slog);
    CHECK(d != NULL && st != NULL, "debug.log / state.txt lesbar");
    if (!d || !st) { free(d); free(st); return; }
    CHECK(strstr(d, "[sherry-r41] Spielermodell 4 -> 15, Waffe (aca5d) 0") != NULL,
          "Raumlader: Modell 4 -> 0x0F, Waffe 0 (@0x800316c4)");
    CHECK(strstr(d, "[sherry-r41] Koerper -> Familie 118 (PL0F): 15 Knochen, 24 Clips, W00 9 Clips, Wurzel-Bind y -1268") != NULL,
          "Koerper PL0F (EDD/EMR @0x8003154c/@0x80031578) + W00 (@0x800741e8[15] = 118)");
    /* letzter [bone]-Block (Bild 60 in ROOM30C1) */
    int y[15]; for (int i = 0; i < 15; i++) y[i] = 99999;
    for (char *p = strstr(d, "[bone] "); p; p = strstr(p + 1, "[bone] ")) {
        int bi, rx, ry, rz, wx, wy, wz;
        if (sscanf(p, "[bone] %d: rel=(%d,%d,%d) world=(%d,%d,%d)", &bi, &rx, &ry, &rz, &wx, &wy, &wz) == 7 && bi >= 0 && bi < 15)
            y[bi] = wy;
    }
    CHECK(y[0] >= -1300 && y[0] <= -1200 && y[8] > -2000 && y[4] > -150 && y[7] > -150 && y[4] < 0,
          "Knochen in 30C1: Huefte y %d (Kind ~-1261, Elza-Skelett -1723), Brust y %d (> -2000; Elza -2346), Knoechel %d/%d am Boden",
          y[0], y[8], y[4], y[7]);
    /* Schritte je Bild im letzten Raumabschnitt (30C1) */
    char *abschnitt = st, *zeile = st; long last = -1;
    for (char *q = st; q && *q; ) {
        long f = strtol(q + 1, NULL, 10);
        if (f < last - 5) abschnitt = q;
        last = f;
        char *nl = strchr(q, '\n'); q = nl ? nl + 1 : NULL;
    }
    int lauf_n = 0, lauf_ok = 0, geh_n = 0, geh_ok = 0, ziel_n = 0, ziel_ok = 0;
    long lx = 0, lz = 0; int hat = 0;
    for (zeile = abschnitt; zeile && *zeile; ) {
        unsigned f, pad; long x, z; int mo = -1, pst = -1;
        if (sscanf(zeile, "F%u pad=%x PL(%ld,%ld,", &f, &pad, &x, &z) == 4) {
            char *m = strstr(zeile, " mo="); if (m) mo = atoi(m + 4);
            char *ps = strstr(zeile, " pst="); if (ps) pst = atoi(ps + 5);
            long dx = x - lx, dz = z - lz; long d2 = dx * dx + dz * dz;
            if (hat && f >= 302 && f <= 309 && pad == 0x0010) { geh_n++; if (d2 == 50L * 50L) geh_ok++; }
            if (hat && f >= 332 && f <= 341 && pad == 0x4010) { lauf_n++; if (d2 == 120L * 120L) lauf_ok++; }
            if (f >= 400 && f <= 439 && pad == 0x0800) { ziel_n++; if (mo >= 200 && mo <= 214 && pst == 1) ziel_ok++; }
            lx = x; lz = z; hat = 1;
        }
        char *nl = strchr(zeile, '\n'); zeile = nl ? nl + 1 : NULL;
    }
    CHECK(geh_n >= 6 && geh_ok == geh_n, "Gehen: %d/%d Bilder je 50 (Zeile 15 @0x80073ec2; Original k1_orig_tempo_30c1: 50)", geh_ok, geh_n);
    CHECK(lauf_n >= 8 && lauf_ok == lauf_n, "Rennen: %d/%d Bilder je 120 (@0x80073f42; Original k1_orig_rennen_30c1: 120)", lauf_ok, lauf_n);
    CHECK(ziel_n >= 30 && ziel_ok == ziel_n, "R1 gehalten: %d/%d Bilder ohne Zielen (Waffe 0, @0x80032014 ff.)", ziel_ok, ziel_n);
    free(d); free(st);
}

/* Sub-0-Clipfolgen des ORIGINALS (k2_orig_3091.py, Lauf 2, 2953 Szenenbilder): Clip*2 (+1 = rueckwaerts, Plc_flg 0x80) */
#define R(c) ((c) * 2 + 1)
static const int k_soll_spieler[] = { 26, 24, R(12), 26, 22, R(11), 26, 22, R(11), 26, 24, R(12), 26, 24, R(12), 22, R(11), 26 };
static const int k_soll_john[]    = { 6, R(3), 4, 0, R(0), 16, 0, R(0), 16, 2, R(1), 16, 8, R(4), 6, R(3), 16, 2, R(1), 0, R(0), 16,
                                      8, R(4), 0, R(0), 16, 6, R(3), 0, R(0), 16 };
static const int k_soll_elza[]    = { 8, R(4), 6, R(3), 18, 2, R(1), 0, R(0), 18, 8, R(4), 18, 2, R(1), 0, R(0), 18, 8, R(4), 18,
                                      10, R(5), 4, 18, 12, R(6), 18 };
/* Bildzahlen der Raum-RBJ-Clips (ROOM3091 rec1/rec2, EDD) - trennen Kanal 0 von der eigenen Bank */
static const int k_fc_rec1[10] = { 20, 30, 30, 20, 30, 25, 35, 50, 24, 20 };
static const int k_fc_rec2[14] = { 20, 30, 30, 20, 30, 25, 30, 50, 18, 24, 20, 15, 30, 40 };

static int folge(const char *anim, int wer, int *out, int cap)
{
    /* Abschnitt mit Typ 0x42 auf Aktor 2 = ROOM3091 */
    const char *beg = NULL, *q = anim; long last = -1; const char *abschnitt = anim; int hat42 = 0;
    for (; q && *q; ) {
        long f = strtol(q, NULL, 10);
        if (f < last - 5) { if (hat42) { beg = abschnitt; break; } abschnitt = q; }
        last = f;
        long fr, ak, ty; if (sscanf(q, "%ld %ld %ld", &fr, &ak, &ty) == 3 && ak == 2 && ty == 0x42) hat42 = 1;
        const char *nl = strchr(q, '\n'); q = nl ? nl + 1 : NULL;
    }
    if (!beg) beg = hat42 ? abschnitt : NULL;
    if (!beg) return 0;
    int n = 0, last_key = -1, last_fc = -1;
    last = -1;
    for (q = beg; q && *q; ) {
        long fr, ak, ty, mo, clip, fc, cur, slot, cl, lo, rev;
        if (sscanf(q, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", &fr, &ak, &ty, &mo, &clip, &fc, &cur, &slot, &cl, &lo, &rev) == 11) {
            if (fr < last - 5) break;
            last = fr;
            int ist = (wer == 0) ? (ty == 0 && ak != 1 && ak != 2) : (ak == wer);
            if (ist) {
                int key = (int)clip * 2 + (rev ? 1 : 0);
                if (key != last_key || fc != last_fc) {
                    last_key = key; last_fc = (int)fc;
                    int rbj = 0;
                    if (wer == 1) rbj = clip >= 0 && clip < 10 && fc == k_fc_rec1[clip];
                    else rbj = clip >= 0 && clip < 14 && fc == k_fc_rec2[clip] && (wer == 0 ? clip >= 11 : clip <= 10);
                    if (rbj && n < cap) out[n++] = key;
                }
            }
        }
        const char *nl = strchr(q, '\n'); q = nl ? nl + 1 : NULL;
    }
    return n;
}
static int gleich(const int *a, int na, const int *b, int nb) { if (na != nb) return 0; for (int i = 0; i < na; i++) if (a[i] != b[i]) return 0; return 1; }
static void drucke(const char *t, const int *a, int n)
{
    printf("    %s:", t); for (int i = 0; i < n; i++) printf(" %d%s", a[i] / 2, (a[i] & 1) ? "r" : ""); printf("\n");
}
static void t_szene(const char *anim_p, const char *dlog)
{
    char *anim = text(anim_p), *d = text(dlog);
    CHECK(anim != NULL && d != NULL, "anim.txt / debug.log lesbar");
    if (!anim || !d) { free(anim); free(d); return; }
    CHECK(strstr(d, "[rbj-binder] ROOM3091 Spieler rec2") != NULL && strstr(d, "Typ 0x42 Marker-Slot 2 -> rec2") != NULL &&
          strstr(d, "Typ 0x49 Marker-Slot 1 -> rec1") != NULL,
          "Binder (Spur J, FUN_8001b3f8): Spieler rec2, John rec1, Elza-NPC rec2 (= Original-RAM w180/w184)");
    CHECK(strstr(d, "[sherry-r41] Koerper -> Familie 118 (PL0F)") != NULL, "Spielfigur Sherry gebunden");
    int a[64], na;
    na = folge(anim, 0, a, 64); drucke("Spieler PORT", a, na);
    CHECK(gleich(a, na, k_soll_spieler, (int)(sizeof k_soll_spieler / sizeof k_soll_spieler[0])),
          "Spieler (Sherry): Sub-0-Clipfolge = Original (%d Abschnitte)", na);
    na = folge(anim, 1, a, 64); drucke("John PORT", a, na);
    CHECK(gleich(a, na, k_soll_john, (int)(sizeof k_soll_john / sizeof k_soll_john[0])),
          "Gegner 0 (John): Sub-0-Clipfolge = Original (%d Abschnitte)", na);
    na = folge(anim, 2, a, 64); drucke("Elza PORT", a, na);
    CHECK(gleich(a, na, k_soll_elza, (int)(sizeof k_soll_elza / sizeof k_soll_elza[0])),
          "Gegner 1 (Elza-NPC): Sub-0-Clipfolge = Original (%d Abschnitte)", na);
    free(anim); free(d);
}

int main(int argc, char **argv)
{
    const char *teil_ = argc > 1 ? argv[1] : "alle";
    printf("test_r41_stage3szenen_b %s\n", teil_);
    if (!strcmp(teil_, "pins")    || !strcmp(teil_, "alle")) t_pins();
    if (!strcmp(teil_, "waffe")   || !strcmp(teil_, "alle")) t_waffe();
    if (!strcmp(teil_, "koerper") || !strcmp(teil_, "alle")) t_koerper();
    if (!strcmp(teil_, "sherry") && argc > 3) t_sherry(argv[2], argv[3]);
    if (!strcmp(teil_, "szene")  && argc > 3) t_szene(argv[2], argv[3]);
    printf("%s (%d Fehler)\n", g_fail ? "FEHLGESCHLAGEN" : "BESTANDEN", g_fail);
    return g_fail ? 1 : 0;
}
