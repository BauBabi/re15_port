/* test_re15_re2z_gore_se.c — NACHZUG zu 83b7740c: DAS ABRISS-GERAEUSCH (SE 9) IM RE1.5-MODUS.
 *
 * 83b7740c hat den RE2-Zerleger @0x80105288-3D8 unter der Port-Option
 * `re15_re15_re2z_import()` in den RE1.5-Modus geholt und dabei EINE Luecke ausdruecklich
 * benannt: der SE, den der Zerleger als ERSTES spielt, blieb stumm, weil platform/pc/main.c
 * den ENEMSE-Hook nur im RE2-ASSET-ZWEIG von `pc_enemy_load` registrierte. Diese Datei belegt
 * den Laut Schritt fuer Schritt an den ECHTEN Bytes und pinnt beide Richtungen.
 *
 * ⛔ Der Zerleger selbst ist KEIN byte-true RE1.5-Verhalten, sondern die vom Nutzer verlangte
 *    Uebernahme. Byte-true ist alles UNTER dem Schalter — also auch dieser SE.
 *
 * STUFE 1  Der Ausloeser im RE2-Zombie-Overlay (EMOVL10_S0.BIN, geladen @0x80100000):
 *          das Dreifach-Gate und `addiu a0,zero,9` / `jal 0x8005bd6c`.
 * STUFE 2  Der BANK-Mechanismus in info/re2leon/PSX.EXE: die Bank haengt an der SOUND-ID aus
 *          dem Sce_em_set-Record (+7 -> entity+0x1FA), NICHT am Gegner-kind.
 * STUFE 3  Die Paar-Tabelle @0x800A7400: Zeile 0 = {0x03,0x00} -> Bank 0.
 * STUFE 4  ZENSUS ueber die 250 echten RE2-RDTs: die Zombie-kinds tragen ueberwiegend
 *          Sound-Id 0x03 -> der Port-Bestandswert RE2Z_ENEMSE_BANK = 0 ist bestaetigt.
 * STUFE 5  DATEN-GEGENPROBE an ENEMSE.VBS: fuehrt Bank 0 die id 9 ueberhaupt? (Ja:
 *          EDT-Map-Eintrag 9 = 0x02A30000.) NEGATIVPROBE an einer Bank, die sie NICHT fuehrt.
 * STUFE 6  PORT-PIN: RE1.5-Modus + Option AN -> der Abriss feuert SE 9 ueber genau den Hook,
 *          den platform/pc/main.c jetzt setzt; Bank 0, flag2000 = 0.
 * STUFE 7  NEGATIV: Option AUS -> kein SE (und main.c wuerde den Hook gar nicht erst setzen).
 * STUFE 8  ABGRENZUNG + REGRESSIONSWACHEN: Hund/Kraehe/Spinne bekommen den Hook im
 *          RE1.5-Modus nicht; im RE2-Modus ist die neue main.c-Zeile tot; und die BESTEHENDEN
 *          RE1.5-Raum-SEs (FUN_800453d0-Spion) sind mit und ohne Hook BIT-GLEICH.
 * STUFE 9  KANAL + PRIORITAET mit den ECHTEN Bank-0-Werten (Kanal 2, Prio-Nibble 3) und der
 *          Beleg, dass die RE2-Kanaele einen EIGENEN Mixer-Slot-Block haben.
 */
#include "re15_ai_flavor.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_damage.h"
#include "re15_player.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_skeleton.h"      /* re15_sin_q12 / re15_cos_q12 */
#include "re15_vab.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif
#ifndef RE15_RE2_EXE_PATH
#define RE15_RE2_EXE_PATH "../info/re2leon/PSX.EXE"
#endif
#ifndef RE15_RE2_RDT_DIR
#define RE15_RE2_RDT_DIR "../info/re2leon/PL0/RDT"
#endif
#ifndef RE15_RE2_OVL_ZOMBIE
#define RE15_RE2_OVL_ZOMBIE "../info/re2leon/COMMON/BIN/EMOVL10_S0.BIN"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

extern uint32_t re15_re2_rand_draws(void);
extern int16_t  re15_atan2_q12(int32_t dz, int32_t dx);
/* Der FUN_800453d0-Spion des Harnischs (tests/test_support.c) — die RE1.5-Raum-SEs. */
extern int g_test_room_se_log[2048];
extern int g_test_room_se_n;

static uint8_t *slurp(const char *p, long *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b && n) *n = sz; return b;
}
static uint32_t rd32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* RE2 PSX.EXE: t_addr 0x80010000, Text ab Datei-Offset 0x800 (PS-X-EXE-Header, selbst
 * gelesen @Datei 0x18/0x1C — dieselbe Rechnung wie tools/gen_re2_ems_toc.py). */
static long fo_exe(uint32_t a) { return (long)(a - 0x80010000u) + 0x800; }
/* Das Gegner-AI-Overlay liegt VORGELINKT auf 0x80100000 (Slot-0-Kopie, CDEMD0-TOC-Eintrag +0;
 * Lader FUN_8001b710 kopiert nach 0x80100000 + slot*0xD000) -> Datei-Offset = Adresse - Basis. */
static long fo_ovl(uint32_t a) { return (long)(a - 0x80100000u); }

/* Fuer eine ENEMSE-Bank: den EDT-Map-Eintrag 9 holen und den dahinter liegenden VAG im
 * VBD lokalisieren (byte-true dieselbe Kette wie audio_pc.c: EDT-Record -> vh_off @[size-8]
 * -> VH -> Tone [prog*16+tone] -> 1-basierter vag_index -> VB-Offset/-Groesse).
 * Rueckgabe 0 = gefunden, 1 = Eintrag ist 0xFFFFFFFF (stumm), -1 = Fehler. */
static re15_vab_t s_vab_scratch;                       /* gross — niemals auf den Stack */
static int bank_id9_vag(const uint8_t *vbs, long vsz, int bank, uint32_t *out_entry,
                        re2_enemse_se_t *out_se, const uint8_t **out_bytes, uint32_t *out_size)
{
    re2_enemse_rec_t rec;
    if (re2_enemse_toc_entry(bank, &rec) != 0) return -1;
    if (rec.edt_off + rec.edt_size > (uint32_t)vsz) return -1;
    if (rec.vbd_off + rec.vbd_size > (uint32_t)vsz) return -1;
    if (rec.edt_size < 12) return -1;
    const uint8_t *edt = vbs + rec.edt_off;
    uint32_t vh_off = rd32(edt + rec.edt_size - 8);
    if (vh_off + 0x20u > rec.edt_size || vh_off / 4 <= 9) return -1;
    uint32_t e9 = rd32(edt + 9 * 4);
    if (out_entry) *out_entry = e9;
    re2_enemse_se_t se;
    re2_enemse_decode_entry(e9, &se);
    if (out_se) *out_se = se;
    if (se.silent) return 1;
    if (re15_vab_parse(edt + vh_off, (size_t)rec.edt_size - vh_off, &s_vab_scratch) != 0) return -1;
    int ti = se.prog * RE15_VAB_TONES_PER_PROGRAM + se.tone;
    if (ti < 0 || ti >= RE15_VAB_TOTAL_TONES) return -1;
    int vag = (int)s_vab_scratch.tones[ti].vag_index - 1;
    if (vag < 0 || vag >= s_vab_scratch.vag_count) return -1;
    uint32_t off = s_vab_scratch.samples[vag].offset, sz = s_vab_scratch.samples[vag].size;
    if (!sz || off + sz > rec.vbd_size) return -1;
    if (out_bytes) *out_bytes = vbs + rec.vbd_off + off;
    if (out_size)  *out_size  = sz;
    return 0;
}

/* ===== Mess-Haken auf den ENEMSE-Hook ================================================== */
static int g_se_log[64], g_se_n = 0, g_flag_seen = -1, g_bank_sel = -1;
static void cap_se(int se_id, int flag2000)
{
    if (g_se_n < 64) g_se_log[g_se_n++] = se_id;
    g_flag_seen = flag2000;
}
static void cap_bank(int bank) { g_bank_sel = bank; }
static void cap_reset(void) { g_se_n = 0; g_flag_seen = -1; g_bank_sel = -1; }

/* ===== Der Trefferrahmen (identisch zu test_re15_re2z_import.c) ========================= */
static uint8_t *s_rdt = NULL; static long s_rdt_sz = 0;

static int bringup(int flavor, int import_on)
{
    re15_rdt_t rdt;
    if (re15_rdt_parse(s_rdt, (size_t)s_rdt_sz, &rdt) != 0) return -1;
    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    re15_re2_damage_model_set(1);
    re15_re15_re2z_import_set(import_on);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    if (rdt.main_scd)   scd_thread_start(0, rdt.main_scd);
    if (rdt.sub_scd[0]) scd_thread_start(1, rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->x = 0; pl->y = 0; pl->z = 0;
    for (int i = 0; i < 30; i++) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type < 0x40) return s;
    return -1;
}
static re15_actor_t *arm(int slot, uint8_t type, int hp)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    re15_actor_t *e = &g_actors[slot];
    e->active = 1; e->type = type;
    re15_enemy_apply_hitbox(e, type);
    e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->hp = (int16_t)hp; e->hit_react = 0; e->hit_stun = 0;
    e->grid_id = (uint8_t)(e->grid_id & 0x7f);
    e->aim_band = 2; e->anim_flags = 0; e->re2_hp_stamped = 1;
    e->rot_y = 0;
    return e;
}
static int bearing_to(const re15_actor_t *e, const re15_actor_t *pl)
{
    return ((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 1024) & 0xfff;
}
static void place(re15_actor_t *e, int32_t back, unsigned want)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int target = (want == 0x40u) ? 3072 : (want == 0x80u) ? 1024 : (want == 0x20u) ? 0 : 2048;
    int best = 0, best_err = 1 << 30;
    for (int k = 0; k < 64; k++) {
        int a = (k * 64) & 0xfff;
        re15_actor_t probe = *pl;
        probe.x = e->x + (int32_t)(((int64_t)re15_cos_q12(a) * back) >> 12);
        probe.z = e->z + (int32_t)(((int64_t)re15_sin_q12(a) * back) >> 12);
        int d = (bearing_to(e, &probe) - ((int)e->rot_y & 0xfff)) & 0xfff;
        int err = (d - target) & 0xfff;
        if (err > 2048) err = 4096 - err;
        if (err < best_err) { best_err = err; best = a; }
    }
    pl->x = e->x + (int32_t)(((int64_t)re15_cos_q12(best) * back) >> 12);
    pl->z = e->z + (int32_t)(((int64_t)re15_sin_q12(best) * back) >> 12);
    pl->y = e->y;
    pl->rot_y = (int16_t)bearing_to(e, pl);
    pl->hit_react = 0;
}
static int shoot(re15_actor_t *e, unsigned want)
{
    place(e, 4000, want);
    e->hit_react = 0;
    re15_player_set_equipped_weapon(3);
    return re15_player_weapon_fire(3) != 0;
}

/* Vier Pistolentreffer von der SEITE: 13 -> 9 -> 5 -> 1 -> -1, der vierte reisst.
 * Rueckgabe = Trefferindex des Abrisses (1-basiert) oder -1. */
static int tear_off(re15_actor_t *e)
{
    for (int h = 0; h < 6; h++) {
        if (!shoot(e, 0x40)) return -1;
        if (e->re2z_flags21a & 0x60u) return h + 1;
    }
    return -1;
}

int main(void)
{
    printf("=== test_re15_re2z_gore_se: das Abriss-Geraeusch im RE1.5-Modus ===\n");

    long ovl_sz = 0, exe_sz = 0;
    uint8_t *ovl = slurp(RE15_RE2_OVL_ZOMBIE, &ovl_sz);
    uint8_t *exe = slurp(RE15_RE2_EXE_PATH, &exe_sz);

    /* ================= 1) DER AUSLOESER IM ORIGINAL ====================================== */
    printf("\n=== 1) Der Zerleger @0x80105288-3D8 spielt SE 9 (EMOVL10_S0.BIN) ===\n");
    if (!ovl) {
        printf("  SKIP: %s nicht lesbar\n", RE15_RE2_OVL_ZOMBIE);
    } else {
        /* Das Dreifach-Gate — dieselben drei Bedingungen, die re2z_leg_gore im Port fahrt. */
        CHECK(rd32(ovl + fo_ovl(0x80105288)) == 0x30620060u,
              "@0x80105288 muss `andi v0,v1,0x60` (0x30620060) sein, ist 0x%08X",
              rd32(ovl + fo_ovl(0x80105288)));                 /* Einweg-Riegel +0x21A & 0x60  */
        CHECK(rd32(ovl + fo_ovl(0x8010529C)) == 0x04410050u,
              "@0x8010529C muss `bgez v0,...` (0x04410050) sein, ist 0x%08X",
              rd32(ovl + fo_ovl(0x8010529C)));                 /* (s8)+0x152 < 0               */
        CHECK(rd32(ovl + fo_ovl(0x801052A4)) == 0x962201D0u,
              "@0x801052A4 muss `lhu v0,464(s1)` (0x962201D0) sein, ist 0x%08X",
              rd32(ovl + fo_ovl(0x801052A4)));                 /* +0x1D0 (Trefferrichtung)     */
        /* ...und DANN, als erste Amtshandlung, der SE: */
        CHECK(rd32(ovl + fo_ovl(0x801052B4)) == 0x24040009u,
              "@0x801052B4 muss `addiu a0,zero,9` (0x24040009) sein, ist 0x%08X",
              rd32(ovl + fo_ovl(0x801052B4)));
        uint32_t jal_word = 0x0C000000u | ((0x8005bd6cu >> 2) & 0x03FFFFFFu);
        CHECK(rd32(ovl + fo_ovl(0x801052B8)) == jal_word,
              "@0x801052B8 muss `jal 0x8005bd6c` (0x%08X) sein, ist 0x%08X",
              jal_word, rd32(ovl + fo_ovl(0x801052B8)));
        printf("  @0x801052B4 = 0x%08X (addiu a0,zero,9), @0x801052B8 = 0x%08X (jal 0x8005bd6c)\n",
               rd32(ovl + fo_ovl(0x801052B4)), rd32(ovl + fo_ovl(0x801052B8)));
    }

    /* ================= 2) WOHER DIE BANK KOMMT ========================================== */
    printf("\n=== 2) FUN_8005bd6c spielt aus der RAUM-Bank; die Bank haengt an der SOUND-ID ===\n");
    if (!exe) {
        printf("  SKIP: %s nicht lesbar\n", RE15_RE2_EXE_PATH);
    } else {
        /* Sce_em_set (Opcode 0x44) — Record+3 = kind, Record+7 = SOUND-ID -> entity+0x1FA. */
        CHECK(rd32(exe + fo_exe(0x800571EC)) == 0x90440003u,
              "@0x800571EC muss `lbu a0,3(v0)` (0x90440003) sein, ist 0x%08X",
              rd32(exe + fo_exe(0x800571EC)));
        CHECK(rd32(exe + fo_exe(0x80057274)) == 0x90420007u,
              "@0x80057274 muss `lbu v0,7(v0)` (0x90420007) sein, ist 0x%08X",
              rd32(exe + fo_exe(0x80057274)));
        CHECK(rd32(exe + fo_exe(0x80057280)) == 0xA20201FAu,
              "@0x80057280 muss `sb v0,506(s0)` (0xA20201FA) sein, ist 0x%08X",
              rd32(exe + fo_exe(0x80057280)));
        /* Verglichen wird gegen die Paar-Tabelle AUSSCHLIESSLICH +0x1FA — nie der kind. */
        CHECK(rd32(exe + fo_exe(0x80052C48)) == 0x808301FAu,
              "@0x80052C48 muss `lb v1,506(a0)` (0x808301FA) sein, ist 0x%08X",
              rd32(exe + fo_exe(0x80052C48)));
        printf("  Record+3 -> kind (0x%08X), Record+7 -> +0x1FA (0x%08X/0x%08X), "
               "Tabellen-Vergleich liest +0x1FA (0x%08X)\n",
               rd32(exe + fo_exe(0x800571EC)), rd32(exe + fo_exe(0x80057274)),
               rd32(exe + fo_exe(0x80057280)), rd32(exe + fo_exe(0x80052C48)));
    }

    /* ================= 3) DIE PAAR-TABELLE @0x800A7400 ================================== */
    printf("\n=== 3) Paar-Tabelle @0x800A7400: Zeilenindex == ENEMSE-Bank ===\n");
    {
        int k0 = -1, k1 = -1;
        CHECK(re2_enemse_pair_row(0, &k0, &k1) == 0 && k0 == 0x03 && k1 == 0x00,
              "Zeile 0 muss {0x03,0x00} sein, ist {0x%02X,0x%02X}", k0, k1);
        int flag = -1;
        CHECK(re2_enemse_select_bank(0x03, 0, &flag) == 0,
              "select_bank(0x03,0) muss Bank 0 liefern, liefert %d",
              re2_enemse_select_bank(0x03, 0, &flag));
        CHECK(flag == 0, "Sound-Id 0x03 steht in der ERSTEN Haelfte -> flag2000 = 0, ist %d", flag);
        /* Gegenprobe: die Tabelle KANN keine kinds fuehren — sie traegt Werte < 0x10, die es im
         * CDEMD0-kind-Raum (Minimum 0x10) nicht gibt. */
        int low = 0;
        for (int r = 0; r < RE2_ENEMSE_BANK_COUNT; r++) {
            int a = -1, b = -1;
            if (re2_enemse_pair_row(r, &a, &b) != 0) break;
            if ((a > 0 && a < 0x10) || (b > 0 && b < 0x10)) low++;
        }
        CHECK(low > 0, "die Tabelle muss Werte < 0x10 fuehren (Beweis: keine kinds), gezaehlt %d", low);
        printf("  Zeile 0 = {0x03,0x00} -> Bank 0, flag2000 = 0; %d Zeilen mit Werten < 0x10\n", low);
    }

    /* ================= 4) ZENSUS UEBER DIE ECHTEN RE2-RAUMDATEN ========================= */
    printf("\n=== 4) Zensus: welche SOUND-ID tragen die Zombie-Spawns? (250 RDTs) ===\n");
    {
        /* Sce_em_set = Opcode 0x44, Record 22 B (Dispatch @0x800A74C8[0x44] = 0x8005714C).
         * Gescannt werden NUR die beiden SCD-Sektionen (Adresstabelle-Eintrag 17 = SCD-INIT,
         * 18 = SCD-MAIN) mit 2-Byte-Schritt und Koordinaten-Plausibilitaet — dasselbe,
         * schaerfere Verfahren wie in test_re2_crow_se_bank.c (ein Roh-Scan ueber die ganze
         * Datei zaehlt Datenbytes mit und verschiebt die Rangfolge).
         * Raumnamen: ROOM<stage><hh>0.RDT, stage '0'..'9'/'A'..'G' — alle 250 Dateien des
         * Verzeichnisses tragen dieses Muster (selbst geprueft), fehlende Namen fallen durch. */
        static const uint8_t ZKIND[] = { 0x10, 0x11, 0x12, 0x13, 0x16, 0x18 };
        static const char STAGE[] = "0123456789ABCDEFG";
        static const char HEX[]   = "0123456789ABCDEF";
        int cnt[6][256]; memset(cnt, 0, sizeof cnt);
        int files = 0;
        for (int st = 0; STAGE[st]; st++)
        for (int h1 = 0; h1 < 16; h1++)
        for (int h2 = 0; h2 < 16; h2++) {
            char path[512];
            snprintf(path, sizeof path, "%s/ROOM%c%c%c0.RDT",
                     RE15_RE2_RDT_DIR, STAGE[st], HEX[h1], HEX[h2]);
            long n = 0;
            uint8_t *d = slurp(path, &n);
            if (!d) continue;
            files++;
            if (n >= 8 + 23 * 4) {
                uint32_t offs[23];
                for (int i = 0; i < 23; i++) offs[i] = rd32(d + 8 + i * 4);
                for (int s = 0; s < 2; s++) {
                    uint32_t o = offs[s ? 18 : 17];
                    if (o == 0 || (long)o >= n) continue;
                    uint32_t e = (uint32_t)n;
                    for (int i = 0; i < 23; i++)
                        if (offs[i] > o && (long)offs[i] <= n && offs[i] < e) e = offs[i];
                    for (uint32_t q = o; q + 22 <= e; q += 2) {
                        if (d[q] != 0x44) continue;
                        int kind = d[q + 3], snd = d[q + 7];
                        int16_t x = (int16_t)(d[q + 10] | (d[q + 11] << 8));
                        int16_t z = (int16_t)(d[q + 14] | (d[q + 15] << 8));
                        if (kind < 0x10 || kind > 0x5A) continue;
                        if (x <= -32000 || x >= 32000 || z <= -32000 || z >= 32000) continue;
                        for (int k = 0; k < 6; k++) if (ZKIND[k] == kind) cnt[k][snd]++;
                    }
                }
            }
            free(d);
        }
        CHECK(files == 250, "es muessen 250 RDTs gelesen werden, gelesen %d", files);
        /* Aggregiert ueber die GANZE Zombie-Familie (der Geltungsbereich der Port-Option). */
        int agg[256]; memset(agg, 0, sizeof agg);
        for (int k = 0; k < 6; k++) {
            int tot = 0, top = -1, topn = 0;
            for (int s = 0; s < 256; s++) {
                tot += cnt[k][s]; agg[s] += cnt[k][s];
                if (cnt[k][s] > topn) { topn = cnt[k][s]; top = s; }
            }
            if (!tot) { printf("  kind 0x%02X: kein Record\n", ZKIND[k]); continue; }
            printf("  kind 0x%02X: %d Records, haeufigste Sound-Id 0x%02X (%dx) -> Bank %d\n",
                   ZKIND[k], tot, top, topn, re2_enemse_select_bank(top, 0, NULL));
        }
        int top = -1, topn = 0, regd = 0;
        for (int s = 1; s < 256; s++) {                 /* Sound-Id 0 = "nicht registrieren" */
            regd += agg[s];
            if (agg[s] > topn) { topn = agg[s]; top = s; }
        }
        int bank_hist[RE2_ENEMSE_BANK_COUNT]; memset(bank_hist, 0, sizeof bank_hist);
        for (int s = 1; s < 256; s++) {
            if (!agg[s]) continue;
            int b = re2_enemse_select_bank(s, 0, NULL);
            if (b >= 0 && b < RE2_ENEMSE_BANK_COUNT) bank_hist[b] += agg[s];
        }
        int topb = -1, topbn = 0;
        for (int b = 0; b < RE2_ENEMSE_BANK_COUNT; b++)
            if (bank_hist[b] > topbn) { topbn = bank_hist[b]; topb = b; }
        printf("  ZOMBIE-FAMILIE gesamt: %d registrierte Records, haeufigste Sound-Id 0x%02X "
               "(%dx) -> Bank %d; haeufigste Bank = %d (%dx)\n",
               regd, top, topn, re2_enemse_select_bank(top, 0, NULL), topb, topbn);
        CHECK(top == 0x03,
              "die haeufigste Sound-Id der Zombie-Familie muss 0x03 sein, ist 0x%02X", top);
        CHECK(re2_enemse_select_bank(0x03, 0, NULL) == 0, "Sound-Id 0x03 muss auf Bank 0 fuehren");
        CHECK(topb == 0,
              "damit muss Bank 0 (== RE2Z_ENEMSE_BANK) die haeufigste Zombie-Bank sein, ist %d",
              topb);
    }

    /* ================= 5) DATEN-GEGENPROBE AN ENEMSE.VBS ================================ */
    printf("\n=== 5) Fuehrt Bank 0 die id 9 ueberhaupt? (ENEMSE.VBS + TOC @0x800A7B1C) ===\n");
    int bank0_chan = -1, bank0_prio = -1;
    {
        long vsz = 0;
        uint8_t *vbs = slurp(RE15_ASSET_RE2_DIR "/ENEMSE.VBS", &vsz);
        if (!vbs) {
            printf("  SKIP: %s/ENEMSE.VBS nicht lesbar\n", RE15_ASSET_RE2_DIR);
        } else {
            re2_enemse_rec_t rec;
            CHECK(re2_enemse_toc_entry(0, &rec) == 0, "TOC-Eintrag Bank 0 fehlt");
            CHECK(rec.edt_off + rec.edt_size <= (uint32_t)vsz, "Bank-0-EDT liegt ausserhalb der VBS");
            const uint8_t *edt = vbs + rec.edt_off;
            uint32_t vh_off = rd32(edt + rec.edt_size - 8);
            CHECK(memcmp(edt + vh_off, "pBAV", 4) == 0,
                  "@edt[size-8] muss auf die VH \"pBAV\" zeigen (vh_off = 0x%X)", vh_off);
            int map_n = (int)(vh_off / 4);
            CHECK(map_n > 9, "die SE-Map muss mindestens 10 Eintraege haben, hat %d", map_n);
            uint32_t e9 = rd32(edt + 9 * 4);
            CHECK(e9 != 0xFFFFFFFFu, "Bank 0 muss die id 9 fuehren (0xFFFFFFFF = stumm)");
            CHECK(e9 == 0x02A30000u, "Bank-0-Map[9] muss 0x02A30000 sein, ist 0x%08X", e9);
            re2_enemse_se_t se; memset(&se, 0, sizeof se);
            const uint8_t *b0bytes = NULL; uint32_t b0size = 0, b0entry = 0;
            int rc = bank_id9_vag(vbs, vsz, 0, &b0entry, &se, &b0bytes, &b0size);
            bank0_chan = se.chan; bank0_prio = se.prio;
            CHECK(rc == 0, "Bank-0-id-9 muss einen echten VAG haben (rc = %d)", rc);
            CHECK(!se.silent && se.prog == 0 && se.tone == 10 && se.chan == 2 && se.prio == 3
                  && se.extra == 0 && se.vab_override < 0,
                  "Bank-0-id-9 muss prog0/tone10/chan2/prio3/extra0 sein, ist "
                  "prog%d/tone%d/chan%d/prio%d/extra%d/ovr%d",
                  se.prog, se.tone, se.chan, se.prio, se.extra, se.vab_override);
            if (rc == 0) {
                long pcm = (long)(b0size / 16) * 28;          /* ADPCM 16 B -> 28 Samples */
                printf("  Bank 0: Map[9] = 0x%08X -> prog %d, tone %d, %u VAG-Bytes = %ld "
                       "PCM-Samples (~%ld Frames @44100), Kanal %d, Prio-Nibble %d\n",
                       e9, se.prog, se.tone, (unsigned)b0size, pcm,
                       (pcm + 734) / 735, se.chan, se.prio);
            }
            /* NEGATIVPROBE: die Bank ist NICHT beliebig — die Hunde-Bank 6 (Sound-Id 0x0C,
             * be44e3f7) fuehrt an derselben Stelle 0xFFFFFFFF, dort waere der Abriss stumm. */
            {
                uint32_t d9 = 0;
                int r6 = bank_id9_vag(vbs, vsz, 6, &d9, NULL, NULL, NULL);
                CHECK(r6 == 1 && d9 == 0xFFFFFFFFu,
                      "NEGATIVPROBE: Bank 6 (Hund) darf die id 9 NICHT fuehren, hat 0x%08X", d9);
                printf("  NEGATIVPROBE Bank 6 (Hund): Map[9] = 0x%08X = stumm\n", d9);
            }
            /* ---- DIE ENTSCHEIDENDE GEGENPROBE ------------------------------------------------
             * Der Port fuehrt fuer die ganze Zombie-Familie EINE feste Bank, das Original waehlt
             * sie pro Raum aus der Sound-Id. Fuer den ABRISS-SE ist dieser Unterschied
             * nachweislich ohne Wirkung: in JEDER Bank, die im Zensus (Stufe 4) fuer die
             * Zombie-Familie ueberhaupt vorkommt, ist der Map-Eintrag 9 UND das dahinter
             * liegende VAG-Sample BYTE-IDENTISCH mit Bank 0. Die Bankwahl kann den Klang des
             * Abrisses also gar nicht aendern — Bank 0 ist nicht "zufaellig richtig", sondern
             * fuer die id 9 ununterscheidbar von den Alternativen. */
            if (rc == 0) {
                static const int zbanks[] = { 1, 2, 5, 53 };   /* Sound-Id 0x05/0x06/0x2F/0x25 */
                int same = 0;
                for (int i = 0; i < (int)(sizeof zbanks / sizeof zbanks[0]); i++) {
                    uint32_t en = 0, sz = 0; const uint8_t *by = NULL;
                    re2_enemse_se_t s2; memset(&s2, 0, sizeof s2);
                    if (bank_id9_vag(vbs, vsz, zbanks[i], &en, &s2, &by, &sz) != 0) {
                        CHECK(0, "Bank %d: id 9 nicht aufloesbar", zbanks[i]); continue;
                    }
                    int eq = (en == b0entry) && (sz == b0size) && by && b0bytes
                             && memcmp(by, b0bytes, sz) == 0;
                    CHECK(eq, "Bank %d muss fuer id 9 dasselbe Sample fuehren wie Bank 0 "
                              "(Eintrag 0x%08X vs 0x%08X, %u vs %u Bytes)",
                          zbanks[i], en, b0entry, (unsigned)sz, (unsigned)b0size);
                    if (eq) same++;
                }
                printf("  Bank 0 vs. 1/2/5/53: %d/4 fuehren fuer id 9 denselben Eintrag UND "
                       "dasselbe VAG-Sample (%u Bytes) -> die Bankwahl aendert den Abriss nicht\n",
                       same, (unsigned)b0size);
            }
            free(vbs);
        }
    }

    /* ================= 6) PORT-PIN ====================================================== */
    printf("\n=== 6) RE1.5-Modus + Option AN: der Abriss feuert SE 9 ===\n");
    s_rdt = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &s_rdt_sz);
    if (!s_rdt) {
        printf("  SKIP: ROOM1140.RDT nicht lesbar (%s)\n", RE15_ASSET_PSX_DIR);
    } else {
        int slot = bringup(RE15_AI_FLAVOR_RE15, 1);
        if (slot <= 0) { printf("  SKIP: kein Zombie gespawnt\n"); }
        else {
            /* GENAU die Bedingung, unter der platform/pc/main.c den Hook jetzt setzt. */
            CHECK(re15_re15_re2z_import_owns(0x10) == 1,
                  "im RE1.5-Modus mit Option AN muss re15_re15_re2z_import_owns(0x10) == 1 sein");
            cap_reset();
            re15_re2z_audio_hook(cap_se, cap_bank);            /* == platform/pc/main.c */
            CHECK(g_bank_sel == 0,
                  "die Registrierung muss sofort Bank 0 waehlen (RE2Z_ENEMSE_BANK), gewaehlt %d",
                  g_bank_sel);
            re15_actor_t *e = arm(slot, 0x10, 5000);
            int se_before_tear = g_se_n;
            int fired_at = tear_off(e);
            CHECK(fired_at == 4, "der Abriss muss beim VIERTEN Seitentreffer kommen, kam bei %d",
                  fired_at);
            CHECK(g_se_n == se_before_tear + 1,
                  "genau EIN SE darf dabei anfallen, angefallen %d", g_se_n - se_before_tear);
            CHECK(g_se_n > se_before_tear && g_se_log[se_before_tear] == 9,
                  "der SE muss die id 9 sein (`addiu a0,zero,9` @0x801052B4), ist %d",
                  g_se_n > se_before_tear ? g_se_log[se_before_tear] : -1);
            CHECK(g_flag_seen == 0,
                  "flag2000 muss 0 sein (erste Map-Haelfte, Zeile 0 = {0x03,0x00}), ist %d",
                  g_flag_seen);
            printf("  Abriss bei Treffer %d -> SE %d, flag2000 %d, Bank %d, Kanal %d, Prio %d\n",
                   fired_at, g_se_log[se_before_tear], g_flag_seen, g_bank_sel,
                   bank0_chan, bank0_prio);
            /* EINWEG: weitere Treffer duerfen KEINEN zweiten Abriss-SE mehr erzeugen
             * (`andi 0x60` / `bne` @0x80105288-8C). */
            int after = g_se_n;
            for (int h = 0; h < 4; h++) shoot(e, 0x40);
            CHECK(g_se_n == after,
                  "nach dem Abriss darf kein weiterer SE 9 kommen (Einweg-Riegel), kamen %d",
                  g_se_n - after);
            re15_re2z_audio_hook(NULL, NULL);
        }
    }

    /* ================= 7) NEGATIV: OPTION AUS =========================================== */
    printf("\n=== 7) NEGATIV: Option AUS -> kein SE ===\n");
    if (s_rdt) {
        int slot = bringup(RE15_AI_FLAVOR_RE15, 0);
        if (slot <= 0) { printf("  SKIP\n"); }
        else {
            CHECK(re15_re15_re2z_import_owns(0x10) == 0,
                  "mit abgeschalteter Option darf re15_re15_re2z_import_owns(0x10) nicht 1 sein "
                  "— platform/pc/main.c registriert den Hook dann gar nicht erst");
            cap_reset();
            re15_re2z_audio_hook(cap_se, cap_bank);     /* trotzdem registrieren = schaerfer */
            re15_actor_t *e = arm(slot, 0x10, 5000);
            for (int h = 0; h < 8; h++) shoot(e, 0x40);
            CHECK((e->re2z_flags21a & 0x60u) == 0, "ohne Option darf nichts abreissen");
            CHECK(g_se_n == 0, "ohne Option darf KEIN ENEMSE-SE kommen, kamen %d (erster: %d)",
                  g_se_n, g_se_n ? g_se_log[0] : -1);
            printf("  8 Seitentreffer, Option AUS: kein Abriss, %d SEs\n", g_se_n);
            re15_re2z_audio_hook(NULL, NULL);
        }
    }

    /* ================= 8) ABGRENZUNG + REGRESSIONSWACHEN ================================ */
    printf("\n=== 8) Abgrenzung: wer bekommt den Hook im RE1.5-Modus NICHT? ===\n");
    {
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        re15_re15_re2z_import_set(1);
        /* Der Geltungsbereich der Option IST der Geltungsbereich der Hook-Registrierung. */
        CHECK(re15_re15_re2z_import_owns(0x20) == 0, "Hund 0x20 darf den Hook nicht bekommen");
        CHECK(re15_re15_re2z_import_owns(0x21) == 0, "Kraehe 0x21 darf den Hook nicht bekommen");
        CHECK(re15_re15_re2z_import_owns(0x25) == 0, "Spinne 0x25 darf den Hook nicht bekommen");
        CHECK(re15_re15_re2z_import_owns(0x26) == 0, "Baby-Spinne 0x26 darf den Hook nicht bekommen");
        CHECK(re15_re15_re2z_import_owns(0x27) == 0, "Gorilla 0x27 darf den Hook nicht bekommen");
        for (unsigned t = 0x10; t <= 0x18; t++) {
            int want = (t == 0x10 || t == 0x11 || t == 0x12 || t == 0x13 || t == 0x16 || t == 0x18);
            CHECK(re15_re15_re2z_import_owns(t) == want,
                  "Typ 0x%02X: erwartet %d, ist %d", t, want, re15_re15_re2z_import_owns(t));
        }
        /* REGRESSIONSWACHE RE2-MODUS: dort ist die NEUE main.c-Zeile tot — der bestehende
         * RE2-Asset-Zweig registriert den Hook wie bisher (und zwar VOR pc_enemy_load_re2). */
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        CHECK(re15_re15_re2z_import_owns(0x10) == 0,
              "im RE2-Modus muss re15_re15_re2z_import_owns(0x10) == 0 sein (sonst wuerde der "
              "Hook doppelt registriert)");
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        printf("  Geltungsbereich = 0x10/0x11/0x12/0x13/0x16/0x18, RE1.5-Modus, Option AN\n");
    }

    printf("\n=== 8b) REGRESSIONSWACHE: die RE1.5-Raum-SEs sind mit und ohne Hook GLEICH ===\n");
    if (s_rdt) {
        /* Derselbe Ablauf zweimal — einmal ohne, einmal mit registriertem ENEMSE-Hook. Der
         * FUN_800453d0-Spion (re15_audio_room_se) protokolliert die RE1.5-Raum-SEs; sie muessen
         * bitgleich sein. Das ist die messbare Fassung von "der RE1.5-Sound aendert sich nicht":
         * der RE2-SE kommt auf einem EIGENEN Kanal ZUSAETZLICH, er ersetzt nichts. */
        int log_a[256], n_a = 0, log_b[256], n_b = 0, enemse_b = 0;
        for (int pass = 0; pass < 2; pass++) {
            int slot = bringup(RE15_AI_FLAVOR_RE15, 1);
            if (slot <= 0) { printf("  SKIP\n"); break; }
            g_test_room_se_n = 0;
            cap_reset();
            if (pass == 1) re15_re2z_audio_hook(cap_se, cap_bank);
            else           re15_re2z_audio_hook(NULL, NULL);
            re15_actor_t *e = arm(slot, 0x10, 5000);
            tear_off(e);                                       /* Abriss (SE 9 nur im 2. Lauf) */
            /* Lange genug laufen lassen, dass die RE1.5-Zombie-SEs (Stoehnen/Angriff, alle ueber
             * FUN_800453d0) tatsaechlich anfallen — der Spieler steht dabei in Reichweite. */
            for (int f = 0; f < 600; f++) {
                re15_enemy_ai_run_all(1); re15_actors_anim_advance();
                re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                pl->x = e->x + ((f & 64) ? 900 : -900); pl->z = e->z + 600;
                if (pl->hp < 40) pl->hp = 100;
            }
            e->hp = 15;                                        /* ...und dann totschiessen:  */
            for (int h = 0; h < 6 && e->state != 3; h++) shoot(e, 0x40);   /* Treffer- + Tod-SEs */
            for (int f = 0; f < 240; f++) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }
            int n = g_test_room_se_n; if (n > 256) n = 256;
            if (pass == 0) { n_a = n; memcpy(log_a, g_test_room_se_log, sizeof(int) * (size_t)n); }
            else { n_b = n; memcpy(log_b, g_test_room_se_log, sizeof(int) * (size_t)n);
                   enemse_b = g_se_n; }
            re15_re2z_audio_hook(NULL, NULL);
        }
        CHECK(n_a > 0, "der FUN_800453d0-Spion muss ueberhaupt etwas sehen, sonst misst die "
                       "Wache nichts (gesehen %d)", n_a);
        CHECK(n_a == n_b && memcmp(log_a, log_b, sizeof(int) * (size_t)(n_a < n_b ? n_a : n_b)) == 0,
              "die RE1.5-Raum-SEs muessen unveraendert bleiben: ohne Hook %d, mit Hook %d",
              n_a, n_b);
        /* Und die andere Haelfte derselben Aussage: der Hook fuegt ueber den GANZEN Ablauf
         * (Abriss + 840 KI-Frames + Tod) GENAU EINEN RE2-SE hinzu, nicht mehr. */
        CHECK(enemse_b == 1,
              "der ENEMSE-Hook darf im RE1.5-Modus genau 1 SE beitragen (den Abriss), "
              "beigetragen %d", enemse_b);
        printf("  RE1.5-Raum-SEs (FUN_800453d0): ohne Hook %d, mit Hook %d — identisch; "
               "RE2-SEs im selben Ablauf: %d (nur der Abriss)\n", n_a, n_b, enemse_b);
    }

    /* ================= 9) KANAL + PRIORITAET ============================================ */
    printf("\n=== 9) Kanal 2 / Prio-Nibble 3 (Bank-0-EDT) durch die RE2-Kanalmaschine ===\n");
    if (bank0_chan >= 0) {
        /* Das RE2-Gate FUN_8005c92c @0x8005c92c-68 ist zeilengleich mit dem RE1.5-Gate
         * FUN_80045a18 — der Port fahrt fuer beide re15_se_prio_gate (Herleitung in
         * audio_pc.c beim Kommentar zu re2se_prio_gate). */
        unsigned char prio[8]; memset(prio, 0, sizeof prio);
        CHECK(re15_se_prio_gate(prio, bank0_chan, bank0_prio) == 0,
              "SE 9 auf freiem Kanal %d muss durchkommen", bank0_chan);
        prio[bank0_chan] = (unsigned char)(bank0_prio & 7);
        /* Ein zweiter Abriss im selben Fenster kann es nicht geben (Einweg, Stufe 6) — aber ein
         * SCHWAECHERER SE desselben Kanals faellt jetzt aus, und ein gleich starker schneidet. */
        CHECK(re15_se_prio_gate(prio, bank0_chan, 1) == 1,
              "ein SE mit Prio 1 muss verworfen werden, solange Kanal %d auf 3 steht", bank0_chan);
        CHECK(re15_se_prio_gate(prio, bank0_chan, bank0_prio) == 0,
              "Gleichstand ohne Nibble-Bit 3 muss erlaubt sein (@0x8005c960-64)");
        /* Der Kanal liegt im gemessenen 2..7-Fenster -> er landet im EIGENEN RE2-Slot-Block
         * (MIXER_RE2SE_CH_FIRST = RE15_SE_VOICE_COUNT + 4 = 12, audio_pc.c), NICHT in den 8
         * festen RE1.5-SE-Stimmen (Slots 0..7) und nicht im freien Pool (8..11). */
        CHECK(bank0_chan >= 0 && bank0_chan < 8,
              "Kanal %d muss im RE2-Slot-Block liegen", bank0_chan);
        CHECK(RE15_SE_VOICE_COUNT == 8,
              "die RE1.5-SE-Stimmen muessen 8 bleiben (Slot-Trennung), sind %d",
              RE15_SE_VOICE_COUNT);
        printf("  Kanal %d, Prio-Nibble %d -> RE2-Slot %d (RE1.5-Stimmen 0..%d + Pool "
               "%d..%d unberuehrt)\n",
               bank0_chan, bank0_prio, RE15_SE_VOICE_COUNT + 4 + bank0_chan,
               RE15_SE_VOICE_COUNT - 1, RE15_SE_VOICE_COUNT, RE15_SE_VOICE_COUNT + 3);
    }

    free(ovl); free(exe); free(s_rdt);
    printf(fails ? "\ntest_re15_re2z_gore_se: %d FAIL\n" : "\ntest_re15_re2z_gore_se: OK\n", fails);
    return fails ? 1 : 0;
}
