/*
 * kanister2080_r41.c — Runde 41 Spur G "kanister2080". Nutzer-Wortlaut, Formen und alle Konstanten mit Adressen:
 * include/re15_kanister2080_r41.h und analysis/befunde_runde41/G_kanister2080.md.
 *
 * Aufbau:
 *   P1  ROOM2080/2081 nach Bit 110: beim ersten Betreten zuerst der KNALL (Nachtrag 17: Knall-Bank Satz 0 = der Tuerknall der
 *       Irons-/Elza-Szene, RE2 ROOM1060 Se_on 0x0a @0x0F48), dann startet ein Port-Programm aus Original-Opcodes die 8 Zombies
 *       (8 x Sce_em_set, Form ROOM2020 sub00 @0x008F6) auf Band 8 = oberhalb der Treppe (Nachtrag 20), Bit 114.
 *   P2  Kanister-Prop an der Reling (Rueckprojektion der Gas_Can.bmp-Marke, stehend wie RE2 ROOM40A0 @0x0452), bis die Szene
 *       ihn nimmt (Bit 115). Gezeichnet von platform/pc/src/kanister2080_pc.c ueber re15_k2080_kanister_lage.
 *   P3  ROOM2090/2091: Ausloeser = RE2-Schwaeche des Alligators (+0x218 & 0x20, HP < 25000 @0x80101ebc-dc,
 *       re15_gr2_geschwaecht) in freier Lage (Verfolgung/Belagerung, nicht im Biss/R1) -> Programm START (Riegel, Raumwechsel
 *       nach 2080) -> Programm SZENE (Marvin/John, Kopfschuesse, Kanister, RE2-Wurf ROOM7020 sub12) -> Programm RUECKKEHR in
 *       2090 (Kanister im Becken ueber re15_gr2_kanister_setzen, Bits 115/116, Ende RE2 sub18 @0x0A2C-0A38).
 *   Takt (scd_vm.c Bildtakt): Signale der Programme (Bank 5), Anvisieren (0xC8/Bild @0x80033f90), Kopfschuss, Kanister in
 *       den Haenden, Wurfbahn (Add_speed/Add_aspeed @0x80055ab0/@0x80055b2c).
 */
#include "re15_kanister2080_r41.h"

#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_gameflow.h"
#include "re15_msg.h"
#include "re15_engine.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"       /* RE15_AI_GRID_SKIP */
#include "re15_skeleton.h"
#include "re15_damage.h"
#include "re15_gator_re2_r41.h"
#include "re15_boss_gator.h"
#include "re15_anim_select.h"   /* re15_actor_playback_slot (Rueckwaerts-Clip, @0x8001f34c-54) */
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>
#endif

extern unsigned g_current_room_id;
extern uint8_t  re15_em_status_zone(void);
extern int      re15_player_weapon_fire(int weapon_id);
extern int      re15_player_is_dead(void);
/* Nachbesserung 2 (N1): Sprachspur des Ports (scd_vm.c, je Bild von der Plattform gestempelt) = RE2 0x800cfbd8 & 0x20 */
extern int      g_re15_voice_laeuft, g_re15_voice_restbilder;
/* Nachbesserung 2 (N3): Spur-B-Bausteine der Verfolgung (enemy_ai_boss_gator.c / enemy_ai_common.c) */
extern void     re15_gator_boss_steuerziel(const re15_actor_t *e, int32_t *tx, int32_t *tz);
extern void     re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);
extern void     re15_ai_advance(re15_actor_t *e, int32_t sp);

#include "gen/kanister2080_r41_texte.inc"

void (*re15_k2080_knall_hook)(void)   = NULL;
void (*re15_k2080_tuerton_hook)(void) = NULL;
void (*re15_k2080_platsch_hook)(void) = NULL;
void (*re15_k2080_schuss_hook)(void)  = NULL;

/* ---- Lagen der 8 Zombies (PORT-WAHL, probe_lagen.c, Dossier 3.2) ------------------------------------------------------
 * Laufgang Band 8 zwischen Treppenabsatz (Westen) und Balkon (Osten): je >= 700 von einer Band-8-Wandzelle (Radius 400),
 * paarweise >= 1393 (>= 800 = zwei Radien, +0x78-Satz +6 FUN_8003a31c), alle mit Fuss UND Kopf im Bild von Cut 2 und Cut 7.
 * Blick auf den Treppenabsatz (Marvins Start): Gier = atan2(-dz, dx) in 4096tel (Form elzazombies_r39.c k_lagen_nach). */
static const struct { int16_t x, z, gier; } k_zombies[RE15_K2080_N_ZOMBIES] = {
    { -12800, -19600, 2698 }, { -11400, -20000, 2569 }, { -10000, -19300, 2392 }, { -12300, -18300, 2455 },
    { -10700, -17500, 2204 }, { -12700, -15800, 1838 }, { -11300, -15300, 1833 }, {  -9900, -14700, 1819 },
};

/* ---- Opcode-Formen (Satzbreiten = s_opcode_sizes, scd_vm.c). Vorbild je Zeile: -------------------------------------------
 *   Sce_em_set  `44 ss tt gg ee 00 p6 kk xx xx yy yy zz zz 00 00 rr rr 00 00`  ROOM2020 sub00 @0x008F6 (Zombie), ROOM2000
 *               @0x017B4 (Marvin `44 01 40 40 01 00 00 ff ..`), ROOM2001 @0x0179E (John)
 *   Sleep       `09 0a nn nn`            ROOM1021 sub02 @0x020CA
 *   Set         `22 bb ii vv`            ROOM2080 sub02 @0x0CB6 (Set(2,7,1)) / @0x0CBA (Set(1,27,1))
 *   Cut_chg     `29 cc`                  ROOM2080 sub02 @0x0CC2
 *   Work_set    `2e kk ii 00`            ROOM1021 sub02 @0x020C2 (`2e 02 00` + Nop)
 *   Plc_motion  `3f ss cc 00`            ROOM1021 sub02 @0x020C6 (2,6) / @0x020CE (2,7) / @0x020DE (1,7)
 *   Plc_flg     `43 00 80 00`            ROOM1021 sub02 @0x020EE (rueckwaerts)
 *   Plc_dest    `40 00 mm ff xx xx zz zz` ROOM1021 sub02 @0x020B2 (`40 00 04 22 ..`), Ankunft -> (5,ff) (@0x80041c24)
 *   Poll        `11 00 08 00 02 00 12 04 21 05 ff 00`  Do {Evt_next} Edwhile Ck(5,ff)=0, ROOM1150 sub08 @0x0112A..0x01132
 *   Message_on  `2b id 00 00`            ROOM1021 sub02 @0x02082
 *   Door_aot_set `3b ss 02 31 00 00 <8 x 0> xx xx yy yy zz zz rr rr st rm ct ee 00 ..`  ROOM2080 main00 @0x0BC2
 *   Aot_on      `47 ss`                  ROOM1240 sub02 / irons_tod_1150.c OP_AOT_ON
 *   Plc_ret     `2e 01 00 00` + `42 00`  RE2 ROOM7020 sub18 @0x0A2C/@0x0A2F, RE1.5 ROOM2080 sub02 @0x0CE8/@0x0CEC
 *   Cut_auto    `3c 01`                  ROOM2080 sub02 @0x0CDE, RE2 ROOM7020 sub18 @0x0A38 */
#define LE16(v)            (uint8_t)((uint16_t)(v) & 0xff), (uint8_t)(((uint16_t)(v) >> 8) & 0xff)
#define OP_SLEEP(n)        0x09, 0x0a, LE16(n)
#define OP_SET(b,i,v)      0x22, (b), (i), (v)
#define OP_CUT(c)          0x29, (c)
#define OP_WORK(k,i)       0x2e, (k), (i), 0x00
#define OP_MOTION(s,c)     0x3f, (s), (c), 0x00
#define OP_FLG_REV         0x43, 0x00, 0x80, 0x00
#define OP_MSG(id)         0x2b, (id), 0x00, 0x00
#define OP_POLL(bit)       0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, (bit), 0x00
#define OP_SIG(bit)        OP_SET(RE15_K2080_SIG_BANK, (bit), 1)
#define OP_RIEGEL          OP_SET(2, 7, 1), OP_SET(1, 27, 1)
#define OP_RIEGEL_LOS      OP_SET(2, 7, 0), OP_SET(1, 27, 0)
#define OP_AOT_ON          0x47, RE15_K2080_TUER_SLOT
/* Nachbesserung 2 (N2): Dir_set `33 00 rx ry rz` - RE2 ROOM7020 sub12 @0x0748 `33 00 00 00 74 0e 00 00` nach Cut_chg 7 @0x0726;
 * RE1.5-Handler @0x80041080 (`lhu/sh` +0x68/+0x6a/+0x6c des Work_set-Objekts @0x80041088-a8, pc + 8), Port scd_vm.c op_dir_set */
#define OP_DIR(gier)       0x33, 0x00, LE16(0), LE16(gier), LE16(0)
#define OP_END             0x01, 0x00

/* ---- Zustand -------------------------------------------------------------------------------------------------------- */
static int      s_phase = RE15_K2080_AUS;
static int32_t  s_leon[4];            /* x, y, z, Gier beim Ausloesen (Ziel der Rueckkehr) */
static int      s_leon_cut;
static int      s_leon_etage;
static int      s_zaehler[5];
static uint8_t  s_prog_spawn[RE15_K2080_N_ZOMBIES * 20 + 8];
static int      s_prog_spawn_n;
static uint8_t  s_prog_szene[1024];
static int      s_prog_szene_n;
static uint8_t  s_prog_start[64];
static int      s_prog_start_n;
static uint8_t  s_prog_rueck[96];
static int      s_prog_rueck_n;
/* Kanister in 2080: 0 steht, 1 in den Haenden, 2 faellt, 3 weg (im Wasser/Raum verlassen) */
static int      s_kan_zustand;
static int32_t  s_kan_pos[3];         /* Fall: Lage, Geschwindigkeit, Beschleunigung (RE2 Faden+0x158/+0x164) */
static int32_t  s_kan_v[3], s_kan_a[3];
static int32_t  s_kan_rot[9];
static int      s_kan_bilder;
static int32_t  s_lande[2];
static int      s_lande_ok;
static int      s_ziel_slot = -1;     /* Aktor, den der NPC gerade anvisiert */
static int      s_ziel_n;             /* abgegebene Schuesse */
static int      s_set_2090;           /* 1 = Kanister in 2090 gesetzt (beim Verlassen wieder entfernen) */
/* Orchestrator-Entscheid FG-E3 (AUFTRAG.md 2026-10-25 00:56): nach der Szene kaempft DERSELBE Alligator weiter (HP, Intro
 * gesehen, Lage/Zustand). Der Boss-Zustand des Moduls (s_gb[slot] in enemy_ai_boss_gator.c, die RE2-Bausteine je Slot in
 * gator_re2_r41.c) liegt in Datei-Statics und ueberlebt den Raumwechsel; neu gesetzt wird nur der AKTOR (main.c-Portblock,
 * state 0 -> Boss-INIT). Die Kopie des Aktors beim Ausloesen wird beim Rueckweg in DENSELBEN Slot zurueckgelegt, bevor der
 * Portblock einen neuen setzt - der Boss-Takt sieht state/hp != 0 und ueberspringt INIT/Untertauchen/Intro. */
static re15_actor_t s_gator;
static int      s_gator_slot = -1;
static uint64_t s_kopf_ab;            /* Aktor-Bits: Kopf-ab gemeldet */
static int      s_wurf_gier = -1;     /* Nachbesserung 2 (N3): gewaehlte Wurfrichtung dieser Szene (-1 = aus Bit 117) */
static int      s_z42_an = -1, s_z42_aus = -1;   /* Nachbesserung 2 (N1): Messzeile Standzeit der Zeile 42 (Bildnummern) */
static int      s_anschw;             /* Nachbesserung 2 (N3): Messzeile Anschwimmen (0 aus, 1 an) */

#ifdef RE15_PLATFORM_PC
#define K_LOG(...) do { fprintf(stderr, "[k2080] F%u ", (unsigned)g_engine.frame_count); fprintf(stderr, __VA_ARGS__); \
                        fputc('\n', stderr); } while (0)
#else
/* PSX: keine Ausgabe; die Argumente bleiben "benutzt" (sonst -Wunused-variable fuer reine Log-Werte wie slot/t). */
static void k_log_leer(const char *f, ...) { (void)f; }
#define K_LOG(...) k_log_leer(__VA_ARGS__)
#endif

static int bit(int b)               { return re15_game_flag_get(RE15_K2080_BANK, (uint8_t)b) != 0; }
static void bit_setzen(int b, int v) { re15_game_flag_set(RE15_K2080_BANK, (uint8_t)b, v); }
static int sig(int b)               { return re15_game_flag_get(RE15_K2080_SIG_BANK, (uint8_t)b) != 0; }
static void sig_setzen(int b, int v) { re15_game_flag_set(RE15_K2080_SIG_BANK, (uint8_t)b, v); }
static int raum_2080(unsigned r)    { return r == RE15_K2080_RAUM_2080 || r == RE15_K2080_RAUM_2081; }
static int raum_2090(unsigned r)    { return r == RE15_K2080_RAUM_2090 || r == RE15_K2080_RAUM_2091; }
static int elza(void)               { return re15_char_variant() != 0; }

uint8_t re15_k2080_npc_typ(void) { return elza() ? RE15_K2080_NPC_ELZA : RE15_K2080_NPC_LEON; }
int  re15_k2080_phase(void)      { return s_phase; }
int  re15_k2080_zaehler(int was) { return (was >= 0 && was < 5) ? s_zaehler[was] : 0; }
int  re15_k2080_zombie_lage(int i, int32_t *x, int32_t *z, int16_t *gier)
{
    if (i < 0 || i >= RE15_K2080_N_ZOMBIES) return 0;
    if (x) *x = k_zombies[i].x;
    if (z) *z = k_zombies[i].z;
    if (gier) *gier = k_zombies[i].gier;
    return 1;
}
int re15_k2080_landepunkt(int32_t *x, int32_t *z)
{
    if (!s_lande_ok) return 0;
    if (x) *x = s_lande[0];
    if (z) *z = s_lande[1];
    return 1;
}
void re15_k2080_reset(void)
{
    s_phase = RE15_K2080_AUS; s_kan_zustand = 0; s_ziel_slot = -1; s_ziel_n = 0; s_lande_ok = 0; s_set_2090 = 0;
    s_wurf_gier = -1; s_z42_an = s_z42_aus = -1; s_anschw = 0; s_gator_slot = -1;
    memset(s_zaehler, 0, sizeof s_zaehler);
}

/* ---- Programme --------------------------------------------------------------------------------------------------------- */
static int put(uint8_t *b, int n, int cap, const uint8_t *src, int len)
{
    if (n < 0 || n + len > cap) return -1;
    memcpy(b + n, src, (size_t)len);
    return n + len;
}
#define PUT(buf, n, ...) do { const uint8_t _t[] = { __VA_ARGS__ }; (n) = put((buf), (n), (int)sizeof(buf), _t, (int)sizeof _t); } while (0)

static int zombie_tot(int i)
{
    return re15_game_flag_get(re15_em_status_zone(), (uint8_t)(RE15_K2080_TOT0 + i)) != 0;
}
static int lebende_zombies(void)
{
    int n = 0;
    for (int i = 0; i < RE15_K2080_N_ZOMBIES; i++) if (!zombie_tot(i)) n++;
    return n;
}
/* Die 8 Saetze (der Tot-Bit-Gate @0x80042128-38 laesst erledigte Zombies aus). */
static int saetze_zombies(uint8_t *b, int n, int cap)
{
    for (int i = 0; i < RE15_K2080_N_ZOMBIES && n >= 0; i++) {
        const uint8_t s[20] = { 0x44, (uint8_t)i, RE15_K2080_ZOMBIE_TYP, RE15_K2080_ZOMBIE_GRID, RE15_K2080_ETAGE, 0x00,
                                0x01, (uint8_t)(RE15_K2080_TOT0 + i),
                                LE16(k_zombies[i].x), LE16(RE15_K2080_Y), LE16(k_zombies[i].z), 0x00, 0x00,
                                LE16(k_zombies[i].gier), 0x00, 0x00 };
        n = put(b, n, cap, s, 20);
    }
    return n;
}
static int satz_tuer(uint8_t *b, int n, int cap, int32_t x, int32_t y, int32_t z, int gier, uint8_t raum, uint8_t cut,
                     uint8_t etage)
{
    const uint8_t s[32] = { 0x3b, RE15_K2080_TUER_SLOT, 0x02, 0x31, 0x00, 0x00, 0, 0, 0, 0, 0, 0, 0, 0,
                            LE16(x), LE16(y), LE16(z), LE16(gier), RE15_K2080_STAGE_BYTE, raum, cut, etage,
                            0, 0, 0, 0, 0, 0 };
    return put(b, n, cap, s, 32);
}

static int wurf_gier(void);   /* Nachbesserung 2: Wahl/Bit 117/Messhaken, definiert bei der Wurfbahn */

static void baue_spawn(void)
{
    int n = 0;
    n = saetze_zombies(s_prog_spawn, n, (int)sizeof s_prog_spawn);
    PUT(s_prog_spawn, n, OP_END);
    s_prog_spawn_n = n;
}

/* SZENE 2080 (Leon: Marvin, Elza: John). Ablauf je Zeile im Dossier §3.3. */
static void baue_szene(void)
{
    uint8_t *b = s_prog_szene; int n = 0; const int cap = (int)sizeof s_prog_szene;
    const uint8_t npc = re15_k2080_npc_typ();
    const int schuesse = lebende_zombies();
    {   /* Abschnitt 1 (erster VM-Lauf): NPC + die lebenden Zombies (Form ROOM2000 @0x017B4 / ROOM2020 @0x008F6) */
        const uint8_t s[20] = { 0x44, RE15_K2080_NPC_SLOT, npc, RE15_K2080_NPC_GRID, RE15_K2080_ETAGE, 0x00, 0x00, 0xff,
                                LE16(RE15_K2080_NPC_X), LE16(RE15_K2080_Y), LE16(RE15_K2080_NPC_Z), 0x00, 0x00,
                                LE16(0), 0x00, 0x00 };
        n = put(b, n, cap, s, 20);
    }
    n = saetze_zombies(b, n, cap);
    PUT(s_prog_szene, n, OP_SLEEP(1));                                   /* Abschnittsende (irons_tod_1150.c OP_ABSCHNITT_ENDE) */
    PUT(s_prog_szene, n, OP_RIEGEL);                                     /* ROOM2080 sub02 @0x0CB6/@0x0CBA */
    PUT(s_prog_szene, n, OP_CUT(RE15_K2080_CUT_SCHUSS));
    PUT(s_prog_szene, n, OP_SIG(RE15_K2080_SIG_TUERTON));                /* "erneut die Geraeusche einer schliessenden Tuer" */
    /* Nachbesserung 1 (M5): KEIN eigenes Warten nach dem Ton - die ROOM1021-Form legt direkt an (Plc_motion(2,6) @0x020C6, dann
     * Sleep 110 @0x020CA); der Ton laeuft in diesen 110 Bildern. Das fruehere Sleep 30 war der RE2-Wert @0x076A aus einem anderen
     * Zusammenhang (dort nach Message_on). */
    PUT(s_prog_szene, n, OP_WORK(2, RE15_K2080_NPC_SLOT));
    if (schuesse > 0) {
        PUT(s_prog_szene, n, OP_SIG(RE15_K2080_SIG_ZIEL));               /* erstes Ziel waehrend des Anlegens */
        PUT(s_prog_szene, n, OP_MOTION(2, RE15_K2080_CLIP_ZIELEN));      /* ROOM1021 @0x020C6 */
        PUT(s_prog_szene, n, OP_SLEEP(110));                             /* ROOM1021 @0x020CA */
        for (int i = 0; i < schuesse; i++) {
            if (i > 0) {   /* Nachbesserung 1 (M5): statt Sleep 10 warten, bis das Nachdrehen (0xC8/Bild @0x80033f90) am Ziel ist */
                PUT(s_prog_szene, n, OP_SET(RE15_K2080_SIG_BANK, RE15_K2080_SIG_AUSGERICHTET, 0));
                PUT(s_prog_szene, n, OP_SIG(RE15_K2080_SIG_ZIEL));
                PUT(s_prog_szene, n, OP_POLL(RE15_K2080_SIG_AUSGERICHTET));
            }
            PUT(s_prog_szene, n, OP_MOTION(2, RE15_K2080_CLIP_FEUER));   /* ROOM1021 @0x020CE */
            PUT(s_prog_szene, n, OP_SIG(RE15_K2080_SIG_SCHUSS));
            PUT(s_prog_szene, n, OP_SLEEP(30));                          /* ROOM1021 @0x020D2 */
        }
        PUT(s_prog_szene, n, OP_MOTION(2, RE15_K2080_CLIP_ZIELEN), OP_FLG_REV);   /* ROOM1021 @0x020EA/@0x020EE */
        PUT(s_prog_szene, n, OP_SLEEP(20));                              /* ROOM1021 @0x020F2 */
    }
    PUT(s_prog_szene, n, OP_CUT(RE15_K2080_CUT_KANISTER));
    PUT(s_prog_szene, n, OP_SET(RE15_K2080_SIG_BANK, RE15_K2080_SIG_ANKUNFT, 0));
    {   const uint8_t s[8] = { 0x40, 0x00, 0x05, RE15_K2080_SIG_ANKUNFT, LE16(RE15_K2080_NPC_STAND_X + RE15_K2080_LAUF_UEBER),
                               LE16(RE15_K2080_NPC_STAND_Z) };
        n = put(b, n, cap, s, 8); }                                      /* Plc_dest RUN ("Rennt zu den Gas Kanister") */
    PUT(s_prog_szene, n, OP_POLL(RE15_K2080_SIG_ANKUNFT));
    /* Nachbesserung 1 (M1, Nachtrag 24b): "nimmt ihn auf in beide Haende" = Marvins Original-Hinknien/Aufstehen ROOM11B0 sub06
     * (Dossier 8.1): Plc_motion(1,11) @0x01650, Sleep 60 @0x01654, Plc_motion(1,11) + Plc_flg 0x80 @0x01658/@0x0165C, Sleep 30
     * @0x01660. Griff im tiefsten Bild (Loco c11 Bild 24, beide Haende seitlich am Kanister), das Aufstehen traegt ihn hoch. Der
     * Kanister steht schon an der Reling - kein Gang danach (vorher Loco c2 = PL00-KLETTERN @0x80038134, dann Plc_dest WALK und
     * Eigen c0 = RUN-Gait @0x8005157c als "Tragen"). */
    PUT(s_prog_szene, n, OP_MOTION(1, RE15_K2080_CLIP_KNIEN));           /* @0x01650 `3f 01 0b 00` */
    PUT(s_prog_szene, n, OP_SLEEP(RE15_K2080_KNIEN_GRIFF));               /* bis Bild 24 (25 Bilder) ... */
    PUT(s_prog_szene, n, OP_SIG(RE15_K2080_SIG_AUFHEBEN));
    PUT(s_prog_szene, n, OP_SLEEP(RE15_K2080_KNIEN_HALT - RE15_K2080_KNIEN_GRIFF));   /* ... zusammen Sleep 60 @0x01654 */
    PUT(s_prog_szene, n, OP_MOTION(1, RE15_K2080_CLIP_KNIEN), OP_FLG_REV);   /* @0x01658/@0x0165C: aufstehen */
    PUT(s_prog_szene, n, OP_SLEEP(RE15_K2080_AUFSTEHEN_HALT));            /* @0x01660 */
    /* ---- RE2 ROOM7020 sub12 ---- */
    PUT(s_prog_szene, n, OP_CUT(RE15_K2080_CUT_WURF));                   /* Cut_chg 7 @0x0726 */
    /* Nachbesserung 2 (N2): die Wurfrichtung erst MIT dem Schnitt (RE2: Work_set(2,0) @0x073C, Pos_set @0x0740, Dir_set
     * `33 00 00 00 74 0e 00 00` @0x0748 im ersten Bild von Cut 7, kein Sleep/Evt_next dazwischen); bis hierher behaelt der Werfer
     * die Gier des Laufs (Hinknien/Aufstehen mit Blick auf den Kanister). Vorher drehte der Takt ihn schon im Griffbild (Cut 5)
     * in einem Bild um 98 Grad (Abnahme 1 N2). Work_set(2, NPC) steht seit dem Anlegen (ROOM1021 @0x020C2). */
    PUT(s_prog_szene, n, OP_DIR(wurf_gier()));
    /* Plc_motion @0x0750: RE2 spielt Adas Wurf-Clip `3f 0a 12 40` (Sub 0x0a, Clip 0x12 IHRER Bank). PORT-WAHL (Nachbesserung 1,
     * M5): Marvin/John haben keinen Wurf-Clip; ihr Eigen c6 hebt beide Haende von der Huefte nach vorn oben ((-31,-1602) ->
     * (764..917,-2534..-2458), Dossier 1.8/8.1) = das Hochstemmen ueber die Reling, dieselbe Bank wie ROOM1021 @0x020C6. */
    PUT(s_prog_szene, n, OP_MOTION(2, RE15_K2080_CLIP_ZIELEN));
    PUT(s_prog_szene, n, OP_MSG(42));                                    /* Xa_on 0 + Message_on 5 @0x0760/@0x0764 */
    PUT(s_prog_szene, n, OP_SLEEP(30));                                  /* @0x076A */
    PUT(s_prog_szene, n, OP_MOTION(2, RE15_K2080_CLIP_ZIELEN));          /* @0x0782 (PORT-WAHL Clip wie @0x0750) */
    PUT(s_prog_szene, n, OP_SLEEP(20));                                  /* @0x0786 */
    /* Nachbesserung 2 (N1): Wsleep `0b` @0x079A + Wsleeping `0c` @0x079B - weiter erst, wenn die Zeile 42 und ihre Stimme zu Ende
     * sind (Wsleeping @0x80053ac0: 0x800cfbd8 & 0x20 / 0x800e873c & 0x80, s. RE15_K2080_SIG_WSLEEP). RE2 wartet nach dem Loslassen;
     * im Port ist das Loslassen an den Raumwechsel nach 2090 gebunden (FG-E4), und der Raumwechsel beendet Zeile und Stimme (RE2-
     * Raumlader @0x8004a1c4 -> CdlSetmode 0xa0 ohne XA @0x800130f0; RE1.5 reloziert den Nachrichtenblock der neuen RDT
     * @0x800397fc-834). Das Warten steht darum VOR dem Loslassen (Dossier 9.1). Poll-Form wie @0x079C-07A4 (mind. ein Bild wie
     * Wsleeping, das immer mit 2 zurueckkehrt @0x80053b18). Die Opcodes 0x0b/0x0c selbst bedeuten im RE1.5-Interpreter etwas
     * anderes (Zaehler-Push @0x8003f490 / Warte-Gate @0x8003f4c4), deshalb das Signal. */
    PUT(s_prog_szene, n, OP_SET(RE15_K2080_SIG_BANK, RE15_K2080_SIG_WSLEEP, 0));
    PUT(s_prog_szene, n, OP_POLL(RE15_K2080_SIG_WSLEEP));
    PUT(s_prog_szene, n, OP_SIG(RE15_K2080_SIG_WURF));                   /* Super_reset @0x078E + Evt_exec sub13 @0x0796 */
    PUT(s_prog_szene, n, OP_SLEEP(1));   /* Abschnittsende (irons_tod_1150.c OP_ABSCHNITT_ENDE): der Bildtakt sieht das Loslassen,
                                          * bevor die Tuer feuert (Bank 5 ist Raum-Scratch, der 2090-Aufbau loescht ihn) */
    /* FG-E4 (Orchestrator-Entscheid 2026-10-25 00:56): der Fall laeuft "von unten" in ROOM2090/2091 Cut 12 - mit dem Loslassen
     * der Raumwechsel (Tuer ohne Sequenz, 1 Bild), Leon zurueck auf seine Lage beim Ausloesen; die RE2-Folge danach (Warten auf
     * den Aufschlag @0x079C-07A4, Sleep 15 @0x07A8, Schnitt auf den Fangenden @0x07D4) laeuft in 2090 (baue_rueck). */
    n = satz_tuer(b, n, cap, s_leon[0], s_leon[1], s_leon[2], s_leon[3], RE15_K2080_RAUM_BYTE_2090, RE15_K2080_CUT_FALL,
                  (uint8_t)s_leon_etage);
    PUT(s_prog_szene, n, OP_AOT_ON);
    PUT(s_prog_szene, n, OP_END);
    s_prog_szene_n = n;
}

static void baue_start(void)
{
    int n = 0; const int cap = (int)sizeof s_prog_start;
    PUT(s_prog_start, n, OP_RIEGEL);
    n = satz_tuer(s_prog_start, n, cap, RE15_K2080_PARK_X, RE15_K2080_PARK_Y, RE15_K2080_PARK_Z, 0,
                  RE15_K2080_RAUM_BYTE_2080, RE15_K2080_CUT_SCHUSS, RE15_K2080_PARK_ETAGE);
    PUT(s_prog_start, n, OP_AOT_ON);
    PUT(s_prog_start, n, OP_END);
    s_prog_start_n = n;
}

static void baue_rueck(void)
{
    int n = 0;
    PUT(s_prog_rueck, n, OP_SLEEP(1));                                   /* Abschnittsende */
    PUT(s_prog_rueck, n, OP_RIEGEL);
    /* FG-E4: Cut 12 von der Tuer, der Kanister faellt (Takt fall_takt) - warten auf den Aufschlag wie RE2 sub12 */
    PUT(s_prog_rueck, n, OP_POLL(RE15_K2080_SIG_GELANDET));              /* Do/Edwhile Ck(5,0x0e) @0x079C-07A4 */
    PUT(s_prog_rueck, n, OP_SLEEP(15));                                  /* @0x07A8 */
    PUT(s_prog_rueck, n, OP_CUT((uint8_t)s_leon_cut));                   /* Cut_chg 11 @0x07D4 = Schnitt auf den Fangenden */
    /* Nachbesserung 1 (M5): kein Sleep 30. RE2 wartet auf Cut 11 mit Wsleep/Wsleeping @0x081A/@0x081B - Wsleeping (@0x80053ac0)
     * gibt ab, solange 0x800cfbd8 & 0x20 (@0x80053acc) oder 0x800e873c & 0x80 (@0x80053ae4) steht; 0x800e873c = 0x80 setzt der
     * Nachrichten-Start FUN_8002fe38 (@0x8002fe80/88) fuer Leons Zeile @0x07E6/@0x07F4. Der Port hat diese Zeile nicht -> das
     * Warten ist 0 Bilder. */
    PUT(s_prog_rueck, n, OP_WORK(1, 0), 0x42, 0x00);                     /* sub18 @0x0A2C/@0x0A2F */
    PUT(s_prog_rueck, n, OP_RIEGEL_LOS);                                 /* @0x0A30/@0x0A34 */
    PUT(s_prog_rueck, n, 0x3c, 0x01);                                    /* Cut_auto 1 @0x0A38 */
    PUT(s_prog_rueck, n, OP_END);
    s_prog_rueck_n = n;
}

const uint8_t *re15_k2080_programm(int nr, int *out_len)
{
    switch (nr) {
    case 0: baue_spawn(); if (out_len) *out_len = s_prog_spawn_n; return s_prog_spawn;
    case 1: baue_szene(); if (out_len) *out_len = s_prog_szene_n; return s_prog_szene;
    case 2: baue_start(); if (out_len) *out_len = s_prog_start_n; return s_prog_start;
    case 3: baue_rueck(); if (out_len) *out_len = s_prog_rueck_n; return s_prog_rueck;
    default: if (out_len) *out_len = 0; return NULL;
    }
}

static int starten(const uint8_t *p, int n)
{
    if (!p || n <= 0) return -1;
    for (int slot = SCD_EVENT_SLOT_FIRST; slot <= SCD_EVENT_SLOT_LAST; slot++)
        if (!g_scd.threads[slot].active && scd_thread_start(slot, p) == 0) return slot;
    return -1;
}

/* ---- Wurfbahn (RE2 ROOM7020 sub13): je Bild Add_speed (Lage += v @0x80055abc-e0), dann Add_aspeed (v += a
 * @0x80055b2c-90), solange y < Landeflaeche - 800 (Member_cmp 0x0c Op 3 @0x80055f10). Die Vektoren gelten im Blickrahmen des
 * Werfers (PORT-WAHL: RE2 wirft nach +x; gedreht um die Gier, x' = x cos + z sin, z' = -x sin + z cos = RotMatrixY). ----- */
static void vektor_drehen(int16_t gier, int32_t vx, int32_t vz, int32_t *ox, int32_t *oz)
{
    const int32_t c = re15_cos_q12(gier), s = re15_sin_q12(gier);              /* Q12 wie RotMatrixY (@0x80068130) */
    *ox = (int32_t)(((int64_t)vx * c + (int64_t)vz * s) >> 12);
    *oz = (int32_t)((-(int64_t)vx * s + (int64_t)vz * c) >> 12);
}
static void bahn_start(const int32_t start[3], int16_t gier)
{
    memcpy(s_kan_pos, start, sizeof s_kan_pos);
    vektor_drehen(gier, RE15_K2080_WURF_VX, RE15_K2080_WURF_VZ, &s_kan_v[0], &s_kan_v[2]);
    vektor_drehen(gier, RE15_K2080_WURF_AX, RE15_K2080_WURF_AZ, &s_kan_a[0], &s_kan_a[2]);
    s_kan_v[1] = RE15_K2080_WURF_VY;
    s_kan_a[1] = RE15_K2080_WURF_AY;
    s_kan_bilder = 0;
}
/* 1 = noch im Fall. */
static int bahn_schritt(int32_t wasser_y)
{
    if (!(s_kan_pos[1] < wasser_y - RE15_K2080_WURF_STOP)) return 0;   /* Edwhile-Pruefung vor dem naechsten Durchlauf */
    for (int i = 0; i < 3; i++) s_kan_pos[i] += s_kan_v[i];           /* Add_speed */
    for (int i = 0; i < 3; i++) s_kan_v[i] += s_kan_a[i];             /* Add_aspeed */
    s_kan_bilder++;
    return 1;
}
void re15_k2080_bahn_rechnen(const int32_t start[3], int16_t gier, int32_t wasser_y, int32_t ende[3], int *bilder)
{
    int32_t p[3], v[3], a[3];
    memcpy(p, s_kan_pos, sizeof p); memcpy(v, s_kan_v, sizeof v); memcpy(a, s_kan_a, sizeof a);
    const int nb = s_kan_bilder;
    bahn_start(start, gier);
    while (bahn_schritt(wasser_y) && s_kan_bilder < 4096) { }
    if (ende) memcpy(ende, s_kan_pos, sizeof s_kan_pos);
    if (bilder) *bilder = s_kan_bilder;
    memcpy(s_kan_pos, p, sizeof p); memcpy(s_kan_v, v, sizeof v); memcpy(s_kan_a, a, sizeof a); s_kan_bilder = nb;
}

static int32_t wasser_2080(void)
{
    return re15_game_flag_get(3, 245) ? RE15_K2080_WASSER_TIEF : RE15_K2080_WASSER_HOCH;   /* sub00 @0x0C02 Ck(3,245) */
}
/* FG-E4: der Wurf faellt in ROOM2090/2091 zu Ende (Cut 12); Landeflaeche = Wasseroberflaeche des Beckens. */
static int32_t wasser_2090(void) { return RE15_K2080_WASSER_2090; }
/* MESSHAKEN RE15_K2080_WURF_GIER=<n> (nur PC, env-gegatet, KEIN Spielverhalten): eine feste Gier fuer Messlaeufe der Landestelle
 * (Dossier 8.9); -1 = keiner. */
static int wurf_gier_messhaken(void)
{
#ifdef RE15_PLATFORM_PC
    static int s_g = -0x7fff;
    if (s_g == -0x7fff) { const char *e = getenv("RE15_K2080_WURF_GIER"); s_g = (e && *e) ? (atoi(e) & 0xfff) : -1; }
    return s_g;
#else
    return -1;
#endif
}
/* Wurfrichtung (Gier des Werfers ab dem Schnitt auf Cut 7, die RE2-Bahn wird um sie gedreht). Nachbesserung 2 (N3): gewaehlt
 * beim Bau des Szenen-Programms (re15_k2080_wurf_gier_wahl), gemerkt in Bit 117 fuer den Raumaufbau ohne Laufzeit-Bahn. */
static int wurf_gier(void)
{
    const int m = wurf_gier_messhaken();
    if (m >= 0) return m;
    if (s_wurf_gier >= 0) return s_wurf_gier;
    return bit(RE15_K2080_BIT_SUEDWURF) ? RE15_K2080_WURF_GIER_SUED : RE15_K2080_WURF_GIER_NORD;
}
/* Nennbahn einer Wurfrichtung g (Wurf-Stand RE15_K2080_NPC_WURF_X/STAND_Z + Haende am Ende von Eigen c6, gedreht um g) bis zur
 * 2090-Landeflaeche (RE2 sub13 @0x088E-08B0) - dieselbe Rechnung wie der Raumaufbau ohne Laufzeit-Bahn. */
static void nenn_landung(int g, int32_t out[2])
{
    int32_t start[3], ende[3], dx = 0, dz = 0; int nb = 0;
    vektor_drehen((int16_t)g, RE15_K2080_HAND_VOR, 0, &dx, &dz);
    start[0] = RE15_K2080_NPC_WURF_X + dx; start[1] = RE15_K2080_Y + RE15_K2080_HAND_HOCH; start[2] = RE15_K2080_NPC_STAND_Z + dz;
    re15_k2080_bahn_rechnen(start, (int16_t)g, wasser_2090(), ende, &nb);
    out[0] = ende[0]; out[1] = ende[2];
}
/* Nachbesserung 2 (N3): von den zwei gemessenen Wurfrichtungen (Header RE15_K2080_WURF_GIER_NORD/_SUED) die, deren Landepunkt
 * WEITER vom Alligator beim Ausloesen liegt - RE2 nimmt den Kanister nur aus dem Anlauf (Zielmodus ab 8880 voraus @0x80100a88-8c,
 * Fenster 8601..9000 @0x80100ad8-ec), ein Landepunkt neben dem Gator waere fuer ihn schon "vorbei". Ohne gesicherten Alligator:
 * Nordbecken (Stand Nachbesserung 1). */
int re15_k2080_wurf_gier_wahl(int gslot, int32_t gx, int32_t gz)
{
    if (gslot <= 0) return RE15_K2080_WURF_GIER_NORD;
    int32_t ln[2], ls[2];
    nenn_landung(RE15_K2080_WURF_GIER_NORD, ln);
    nenn_landung(RE15_K2080_WURF_GIER_SUED, ls);
    const int64_t nx = (int64_t)ln[0] - gx, nz = (int64_t)ln[1] - gz, sx = (int64_t)ls[0] - gx, sz = (int64_t)ls[1] - gz;
    return (sx * sx + sz * sz > nx * nx + nz * nz) ? RE15_K2080_WURF_GIER_SUED : RE15_K2080_WURF_GIER_NORD;
}

/* ---- NPC -------------------------------------------------------------------------------------------------------------- */
static re15_actor_t *npc_aktor(void)
{
    const uint8_t t = re15_k2080_npc_typ();
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == t) return &g_actors[s];
    return NULL;
}
static int zombie_lebt(const re15_actor_t *e)
{
    return e->active && e->type == RE15_K2080_ZOMBIE_TYP && e->hp >= 0 && e->state != 3 && e->state != 7;
}
/* Mitte der Haende (Bones 11/14) im aktuellen Bild in Welt; 0 = keine NPC-Bank (Fallback: Ende von Eigen c6). */
static int haende_welt(const re15_actor_t *n, int32_t out[3])
{
    int32_t mv = RE15_K2080_HAND_VOR, mh = RE15_K2080_HAND_HOCH, mq = 0;
    int ok = 0;
    const re15_enemy_bank_t *bk = re15_enemy_find(n->type);
    if (bk && bk->own_ok && n->anim_frame < 4096) {
        const re15_emd_skeleton_t  *sk = (n->sub_state_1 == 1) ? &bk->skel_loco : &bk->skel_own;
        const re15_emd_animation_t *an = (n->sub_state_1 == 1) ? &bk->anim_loco : &bk->anim_own;
        int c = (int)n->motion;
        if (sk && an && c >= 0 && c < an->clip_count && an->clips[c].frame_count > 0) {
            int f = (int)n->anim_frame;
            if (f >= an->clips[c].frame_count) f = an->clips[c].frame_count - 1;
            /* Nachbesserung 1: der GEZEICHNETE Slot (rueckwaerts fc-1-f @0x8001f34c-54, Plc_flg 0x80 beim Aufstehen) */
            f = re15_actor_playback_slot(n, (uint32_t)f, an->clips[c].frame_count);
            const int kf = (int)(an->frames[an->clips[c].first_frame + f] & 0xfffu);
            re15_skel_pose_t p[RE15_EMD_MAX_BONES];
            if (sk->bone_count > 14 && re15_skel_compute_pose(sk, kf, p) == 0) {
                mv = (p[11].trans[0] + p[14].trans[0]) / 2;
                mh = (p[11].trans[1] + p[14].trans[1]) / 2;
                mq = (p[11].trans[2] + p[14].trans[2]) / 2;
                ok = 1;
            }
        }
    }
    int32_t dx = 0, dz = 0;
    vektor_drehen(n->rot_y, mv, mq, &dx, &dz);
    out[0] = n->x + dx; out[1] = n->y + mh; out[2] = n->z + dz;
    return ok;
}
/* Kanister quer vor der Brust: Laengsachse (MD1 y) auf die Querachse des NPC (lokal z) = RotY(gier) * RotX(1024); Mitte der
 * Laenge (y -1392) in die Haende. PORT-WAHL (RE2 haengt den Werfer an Adas Teil, Super_set @0x072C; die Lage im Maul des
 * Alligators ist ebenfalls quer, B §2.5). */
static void kanister_in_haenden(const re15_actor_t *n)
{
    int32_t h[3];
    haende_welt(n, h);
    const int32_t c = re15_cos_q12(n->rot_y), s = re15_sin_q12(n->rot_y);
    /* RotY(g) = [c 0 s; 0 1 0; -s 0 c], RotX(90) = [1 0 0; 0 0 -1; 0 1 0] -> M = RotY*RotX = [c s 0; 0 0 -1; -s c 0] (Q12) */
    const int32_t m[9] = { c, s, 0,  0, 0, -4096,  -s, c, 0 };
    memcpy(s_kan_rot, m, sizeof s_kan_rot);
    /* s_kan_pos = MITTE des Kanisters = Mitte der Haende (die Wurfbahn laeuft auf der Mitte, RE2: Super_reset uebernimmt die
     * Weltlage des angehaengten Objekts @0x800558b8-d8); der MD1-Ursprung (Fuss) folgt in kanister_ursprung. */
    memcpy(s_kan_pos, h, sizeof s_kan_pos);
}
/* MD1-Ursprung (Fuss, y 0 des Modells) zur Mitte c: o = c - M * (0, -1392, 0). */
static void kanister_ursprung(const int32_t c[3], const int32_t rot[9], int32_t o[3])
{
    const int32_t halb = -RE15_K2080_KAN_LAENGE / 2;
    o[0] = c[0] - (int32_t)(((int64_t)rot[1] * halb) >> 12);
    o[1] = c[1] - (int32_t)(((int64_t)rot[4] * halb) >> 12);
    o[2] = c[2] - (int32_t)(((int64_t)rot[7] * halb) >> 12);
}

int re15_k2080_kanister_lage(int32_t pos[3], int32_t rot[9])
{
    if (s_kan_zustand == 2 && raum_2090((unsigned)g_current_room_id & 0xFFFFu)) {   /* FG-E4: Fall in 2090 Cut 12 */
        kanister_ursprung(s_kan_pos, s_kan_rot, pos); memcpy(rot, s_kan_rot, 9 * sizeof(int32_t));
        return 1;
    }
    if (!raum_2080((unsigned)g_current_room_id & 0xFFFFu)) return 0;
    if (s_phase != RE15_K2080_IN_2080 && bit(RE15_K2080_BIT_SZENE)) return 0;   /* Marvin/John hat ihn genommen */
    if (s_kan_zustand == 0) {
        pos[0] = RE15_K2080_KAN_X; pos[1] = RE15_K2080_Y; pos[2] = RE15_K2080_KAN_Z;
        memset(rot, 0, 9 * sizeof(int32_t)); rot[0] = rot[4] = rot[8] = 4096;   /* RE2 ROOM40A0 @0x0452 Drehung (0,0,0) */
        return 1;
    }
    if (s_kan_zustand == 1 || s_kan_zustand == 2) {
        kanister_ursprung(s_kan_pos, s_kan_rot, pos); memcpy(rot, s_kan_rot, 9 * sizeof(int32_t));
        return 1;
    }
    return 0;
}

/* ---- Takt ------------------------------------------------------------------------------------------------------------- */
static void ziel_waehlen(re15_actor_t *n)
{
    s_ziel_slot = -1;
    long best = 0x7fffffffL;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (!zombie_lebt(e)) continue;
        long dx = (long)e->x - n->x, dz = (long)e->z - n->z;     /* ohne <stdlib.h> (PSX-Bau: nur unter RE15_PLATFORM_PC) */
        if (dx < 0) dx = -dx;
        if (dz < 0) dz = -dz;
        const long d = dx + dz;
        if (d < best) { best = d; s_ziel_slot = s; }   /* naechster zuerst (Gun-FSM-Zielsuche FUN_80037250, Form) */
    }
    if (s_ziel_slot <= 0) sig_setzen(RE15_K2080_SIG_AUSGERICHTET, 1);   /* kein Ziel mehr: nicht warten */
    K_LOG("Ziel %d: Slot %d", s_ziel_n + 1, s_ziel_slot);
}
static void nachfuehren(re15_actor_t *n)
{
    if (s_ziel_slot <= 0 || s_ziel_slot >= RE15_ACTOR_MAX) return;
    const re15_actor_t *e = &g_actors[s_ziel_slot];
    int soll = ((int)re15_atan2_q12(e->z - n->z, e->x - n->x) - 0x400) & 0xfff;   /* Mesh-Gier zum Ziel (re15_actor.h:1196) */
    int d = ((soll - (int)n->rot_y + 0x800) & 0xfff) - 0x800;
    const int fertig = (d >= -RE15_K2080_NACHFUEHREN && d <= RE15_K2080_NACHFUEHREN);   /* dieser Schritt erreicht das Ziel */
    if (d > RE15_K2080_NACHFUEHREN) d = RE15_K2080_NACHFUEHREN;
    if (d < -RE15_K2080_NACHFUEHREN) d = -RE15_K2080_NACHFUEHREN;
    n->rot_y = (int16_t)((n->rot_y + d) & 0xfff);
    if (fertig && !sig(RE15_K2080_SIG_AUSGERICHTET)) {     /* Nachbesserung 1 (M5): Ende des Wartens = Ende des Nachdrehens */
        sig_setzen(RE15_K2080_SIG_AUSGERICHTET, 1);
        K_LOG("Ziel Slot %d ausgerichtet (Gier %d)", s_ziel_slot, (int)n->rot_y);
    }
}
/* Kopfschuss: der Anwender des Spielerschusses mit der Magnum-Kritklasse (w7) aus der Lage des NPC. */
static void schuss(re15_actor_t *n)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (re15_k2080_schuss_hook) re15_k2080_schuss_hook();
    s_ziel_n++;
    s_zaehler[1]++;
    if (s_ziel_slot <= 0 || !zombie_lebt(&g_actors[s_ziel_slot])) { K_LOG("Schuss %d: kein Ziel", s_ziel_n); return; }
    re15_actor_t *z = &g_actors[s_ziel_slot];
    z->grid_id = (uint8_t)(z->grid_id & ~RE15_AI_GRID_SKIP);                 /* Halt aufheben (@0x80100450-5c) */
    const re15_actor_t sicher = *pl;
    pl->x = n->x; pl->y = n->y; pl->z = n->z; pl->rot_y = n->rot_y; pl->floor = n->floor;
    const int r = re15_player_weapon_fire(7);                                 /* w7 = Magnum-Kritklasse @0x80012380-88 */
    pl->x = sicher.x; pl->y = sicher.y; pl->z = sicher.z; pl->rot_y = sicher.rot_y; pl->floor = sicher.floor;
    const int getroffen = r > 0 ? r - 1 : -1;
    if (getroffen > 0) {
        const re15_actor_t *t = &g_actors[getroffen];
        K_LOG("Schuss %d: Treffer Slot %d (Ziel %d) hp=%d state=%d zeile=%d", s_ziel_n, getroffen, s_ziel_slot,
              (int)t->hp, (int)t->state, (int)t->sub_state_1);
    } else {
        K_LOG("Schuss %d: kein Treffer auf Slot %d", s_ziel_n, s_ziel_slot);
    }
}

static int gator_frei(int *slot_out)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != 0x23 || !re15_gator_boss_active(e)) continue;
        if (slot_out) *slot_out = s;
        if (re15_gator_boss_untergetaucht(e) || !re15_gator_boss_verfolgt(e)) return 0;   /* RE2: +0x1DA == 1 (Form) */
        if (re15_gator_boss_im_fressen(e) || re15_gator_fressen_hold()) return 0;
        if (re15_gr2_r1_laeuft(s) || re15_gr2_tot() || re15_gr2_traegt()) return 0;
        return 1;
    }
    return 0;
}

static void ausloesen(int gslot)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (gslot > 0 && gslot < RE15_ACTOR_MAX) {           /* FG-E3: derselbe Alligator nach der Szene */
        s_gator = g_actors[gslot]; s_gator_slot = gslot;
        K_LOG("FG-E3: Alligator gesichert Slot %d (%d,%d,%d) Gier %d hp %d Phase %d", gslot, (int)s_gator.x, (int)s_gator.y,
              (int)s_gator.z, (int)s_gator.rot_y & 0xfff, (int)s_gator.hp, re15_gator_boss_phase(&g_actors[gslot]));
    }
    s_leon[0] = pl->x; s_leon[1] = pl->y; s_leon[2] = pl->z; s_leon[3] = pl->rot_y & 0xfff;
    s_leon_cut = (int)g_scd.cam_id;
    s_leon_etage = (int)pl->floor;
    baue_start();
    const int slot = starten(s_prog_start, s_prog_start_n);
    s_zaehler[3]++;
    s_phase = RE15_K2080_NACH_2080;
    K_LOG("AUSLOESER geschwaecht (RE2 +0x218&0x20 @0x80101ebc-dc), Treffer %d: Spieler (%d,%d,%d) Gier %d Cut %d -> ROOM%04X "
          "(Programm START Faden %d)", re15_gr2_treffer(), (int)s_leon[0], (int)s_leon[1], (int)s_leon[2], (int)s_leon[3],
          s_leon_cut, (unsigned)(RE15_K2080_RAUM_2080 | (unsigned)re15_char_variant()), slot);
}

/* MESSHAKEN RE15_K2080_GATOR_HP=<n> (env-gegatet, KEIN Spielverhalten; Muster RE15_R39_MESS_SPIELER_HP): setzt die HP des
 * Boss-Alligators EINMAL je Raumeintritt auf n, damit ein Messlauf die Schwelle (HP < 25000 @0x80101ebc-dc) mit wenigen ECHTEN
 * Treffern erreicht statt mit 43 Pistolenschuessen (Dossier 4). */
static void messhaken_gator_hp(void)
{
#ifdef RE15_PLATFORM_PC
    static int s_n = -2; static unsigned s_raum_gesetzt = 0xffffffffu;
    if (s_n == -2) { const char *e = getenv("RE15_K2080_GATOR_HP"); s_n = (e && *e) ? atoi(e) : -1; }
    if (s_n < 0 || s_raum_gesetzt == g_current_room_id) return;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != 0x23 || !re15_gator_boss_active(e) || e->state == 0) continue;
        e->hp = (int16_t)s_n;
        s_raum_gesetzt = g_current_room_id;
        K_LOG("MESSHAKEN RE15_K2080_GATOR_HP: Slot %d hp = %d", s, s_n);
        break;
    }
#endif
}

/* MESSHAKEN EBENE (Nachtrag 20, kein Verhalten): aktive 2080-Zombies, davon westlich der Band-8-Absatzwand (@0x05C4, x < -16650
 * = auf der Treppe @0x0BE2) bzw. nicht auf der Hoehe ihres Bandes (y != -14400). */
int re15_k2080_zombies_oben(int *aktiv, int *auf_treppe, int *falsche_hoehe)
{
    int n = 0, w = 0, h = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != RE15_K2080_ZOMBIE_TYP) continue;
        n++;
        if (e->x < RE15_K2080_ABSATZ_WAND_X) w++;
        if (e->y != RE15_K2080_Y) h++;
    }
    if (aktiv) *aktiv = n;
    if (auf_treppe) *auf_treppe = w;
    if (falsche_hoehe) *falsche_hoehe = h;
    return n;
}
static void messzeile_ebene(void)
{
#ifdef RE15_PLATFORM_PC   /* nur Messzeile: auf der PSX ohne Ausgabe (K_LOG leer) */
    if ((g_engine.frame_count % 30u) != 0) return;
    int n = 0, w = 0, h = 0;
    if (re15_k2080_zombies_oben(&n, &w, &h) == 0) return;
    int32_t x0 = 0x7fffffff, x1 = -0x7fffffff, y0 = 0x7fffffff, y1 = -0x7fffffff;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != RE15_K2080_ZOMBIE_TYP) continue;
        if (e->x < x0) x0 = e->x;
        if (e->x > x1) x1 = e->x;
        if (e->y < y0) y0 = e->y;
        if (e->y > y1) y1 = e->y;
    }
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    K_LOG("Ebene: %d Zombies x %d..%d y %d..%d, auf der Treppe (x < %d) %d, nicht auf y %d: %d | Spieler (%d,%d,%d)", n,
          (int)x0, (int)x1, (int)y0, (int)y1, RE15_K2080_ABSATZ_WAND_X, w, RE15_K2080_Y, h, (int)pl->x, (int)pl->y, (int)pl->z);
#endif
}

/* Ein Fall-Bild (RE2 ROOM7020 sub13: Add_speed/Add_aspeed bis y >= Landeflaeche - 800 @0x08B0); am Ende Aufschlag (Set(5,0x0e)
 * @0x08B6, Se_on @0x08BA -> ROOM40A0 Satz 0x0b @0x0686) und - in 2090 - der Kanister liegt dort, wo er aufschlug (FG-E3 "der
 * Kanister landet so ..."): Spur-B-API re15_gr2_kanister_setzen auf der Becken-Hoehe der Aufnahme (GB_WATER_Y). */
static void fall_takt(int32_t wasser_y)
{
    if (bahn_schritt(wasser_y)) return;
    s_kan_zustand = 3;
    sig_setzen(RE15_K2080_SIG_GELANDET, 1);
    if (re15_k2080_platsch_hook) re15_k2080_platsch_hook();
    K_LOG("Aufschlag im Wasser bei (%d,%d,%d) Bild %d (RE2 ROOM40A0 Se_on Satz 0x0b @0x0686)",
          (int)s_kan_pos[0], (int)s_kan_pos[1], (int)s_kan_pos[2], s_kan_bilder);
    if (raum_2090((unsigned)g_current_room_id & 0xFFFFu)) {
        re15_gr2_kanister_setzen(s_kan_pos[0], RE15_K2080_GATOR_WASSER_Y, s_kan_pos[2]);
        s_set_2090 = 1;
        s_zaehler[4]++;
        K_LOG("ROOM%04X Kanister im Becken (%d,%d,%d) = Aufschlagstelle (Bit %d)", (unsigned)g_current_room_id & 0xFFFFu,
              (int)s_kan_pos[0], RE15_K2080_GATOR_WASSER_Y, (int)s_kan_pos[2], RE15_K2080_BIT_KANISTER);
    }
}

void re15_kanister2080_r41_tick(void)
{
    const unsigned raum = (unsigned)g_current_room_id & 0xFFFFu;
    if (raum_2090(raum)) {
        messhaken_gator_hp();
        /* Kanister verbraucht: Explosion = Tod des Alligators (B: R1-Endzustand) */
        if (bit(RE15_K2080_BIT_KANISTER) && re15_gr2_tot()) {
            bit_setzen(RE15_K2080_BIT_KANISTER, 0);
            K_LOG("Kanister verbraucht (R1-Tod) -> Bit %d = 0", RE15_K2080_BIT_KANISTER);
        }
        int gslot = -1;
        if (s_phase == RE15_K2080_AUS && bit(RE15_K2080_BIT_F_SZENE) && !bit(RE15_K2080_BIT_SZENE) &&
            !re15_player_is_dead() && !re15_game_flag_get(1, 27) && !re15_game_flag_get(2, 7) &&
            re15_gr2_geschwaecht() && gator_frei(&gslot))
            ausloesen(gslot);
        if (s_kan_zustand == 2) fall_takt(wasser_2090());   /* FG-E4: der Fall laeuft in 2090 (Cut 12) zu Ende */
        return;
    }
    if (raum_2080(raum)) messzeile_ebene();
    if (!raum_2080(raum) || s_phase != RE15_K2080_IN_2080) return;
    re15_actor_t *n = npc_aktor();
    {   /* Nachbesserung 2 (N1): Wsleeping-Bedingung (RE15_K2080_SIG_WSLEEP) je Bild - frei, wenn keine Nachricht mehr steht
         * (0x800e873c & 0x80, inkl. Nutzer-Nachhall einer laengeren Aufnahme) und keine Sprachaufnahme mehr laeuft
         * (0x800cfbd8 & 0x20). Das Programm loescht das Bit unmittelbar vor seinem Poll. */
        const int zeile = g_scd.message_active != 0;
        const int stimme = g_re15_voice_laeuft && g_re15_voice_restbilder > 0;
        if (zeile && g_scd.message_id == 42 && s_z42_an < 0) s_z42_an = (int)g_engine.frame_count;
        if (!zeile && s_z42_an >= 0 && s_z42_aus < 0) {
            s_z42_aus = (int)g_engine.frame_count;
            K_LOG("Zeile 42 stand F%d..F%d (%d Bilder), Stimme %s", s_z42_an, s_z42_aus - 1, s_z42_aus - s_z42_an,
                  stimme ? "laeuft noch" : "aus");
        }
        /* Notbremse fuer einen haengenden Sprachkanal wie die Port-Stimmenschranke (scd_vm.c RE15_VOICE_NOTBREMSE = 300 Bilder,
         * msg_common.c RE15_UNTERTITEL_NACHHALL_DECKEL = 300 > laengste Aufnahme 242,5 Bilder); RE2 kennt keinen Deckel. */
        const int notbremse = stimme && s_z42_aus >= 0 && (int)g_engine.frame_count - s_z42_aus >= 300;
        if (!zeile && (!stimme || notbremse) && !sig(RE15_K2080_SIG_WSLEEP)) {
            sig_setzen(RE15_K2080_SIG_WSLEEP, 1);
            if (s_z42_aus >= 0)
                K_LOG("Wsleeping frei (RE2 @0x079B: Zeile 42 und Stimme zu Ende%s) - Loslassen folgt", notbremse ? ", NOTBREMSE" : "");
        }
    }
    /* KEIN Halt der Zombies (entity+0x9 |= 0x20): gemessen (Lauf p3f) spielte der angehaltene RE2-Steher seinen Clip 1 bis zum
     * Ende weiter und lag ab Bild 40 am Boden. Die Steher (grid 0x02) bleiben von selbst stehen, solange der Spieler fern ist:
     * der Parkplatz liegt auf Band 5 >= 10000 entfernt (Form: Montage-Steher irons_tod_1150.c "standen still", re15_montage_r38.h). */
    if (n && (n->sub_state_1 == 4 || n->sub_state_1 == 5) && (g_engine.frame_count % 15u) == 0)   /* Messzeile Lauf */
        K_LOG("NPC Lauf Sub %d Phase %d (%d,%d) Gier %d Ziel (%d,%d)", (int)n->sub_state_1, (int)n->sub_state_2, (int)n->x,
              (int)n->z, (int)n->rot_y, (int)n->steer_x, (int)n->steer_z);
    if (sig(RE15_K2080_SIG_TUERTON)) {
        sig_setzen(RE15_K2080_SIG_TUERTON, 0);
        s_zaehler[0]++;
        if (re15_k2080_tuerton_hook) re15_k2080_tuerton_hook();
        K_LOG("Tuer-Ton DOOR06 Satz 1 (Door_exit @0x800141f0) - Szene");
    }
    /* MESSZEILE "Kopf platzt": RE2 FUN_801092C4 stempelt Part 8 (Kopf) mit Flagwort |= 0x4A (eigene Matrix + Drift,
     * @0x80109460-98) - je Zombie einmal, sobald der Kopf vom Rumpf ist. */
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != RE15_K2080_ZOMBIE_TYP || (s_kopf_ab & (1ull << s))) continue;
        if ((e->re2z_part_flags[8] & 0x40u) && (e->re2z_part_flags[8] & 0x08u)) {
            s_kopf_ab |= (1ull << s); s_zaehler[2]++;
            K_LOG("Kopf ab: Slot %d (Part 8 Flagwort 0x%X, RE2 FUN_801092C4 @0x80109460-98), %d von %d", s,
                  (unsigned)e->re2z_part_flags[8], s_zaehler[2], s_ziel_n);
        }
    }
    if (n && sig(RE15_K2080_SIG_ZIEL)) { sig_setzen(RE15_K2080_SIG_ZIEL, 0); ziel_waehlen(n); }
    if (n && s_ziel_slot > 0 && s_kan_zustand == 0) nachfuehren(n);
    if (n && sig(RE15_K2080_SIG_SCHUSS)) { sig_setzen(RE15_K2080_SIG_SCHUSS, 0); schuss(n); s_ziel_slot = -1; }   /* Anvisieren endet mit dem Schuss */
    if (n && sig(RE15_K2080_SIG_AUFHEBEN)) {
        sig_setzen(RE15_K2080_SIG_AUFHEBEN, 0);
        s_kan_zustand = 1;
        K_LOG("Kanister aufgehoben (Loco c%d Bild %d, Hinknien ROOM11B0 sub06 @0x01650)", (int)n->motion, (int)n->anim_frame);
    }
    /* Nachbesserung 2 (N2): kein Drehen mehr im Takt - der Werfer behaelt die Gier des Laufs, bis das Programm mit dem Schnitt auf
     * Cut 7 das Dir_set schreibt (RE2 @0x0748); vorher setzte der Takt ab dem Griff jedes Bild die Wurf-Gier (98 Grad Sprung). */
    if (n && s_kan_zustand == 1) kanister_in_haenden(n);
    if (n && sig(RE15_K2080_SIG_WURF)) {
        sig_setzen(RE15_K2080_SIG_WURF, 0);
        kanister_in_haenden(n);                          /* Super_reset @0x800558b8-d8: Weltlage der Haende */
        const int32_t start[3] = { s_kan_pos[0], s_kan_pos[1], s_kan_pos[2] };
        int32_t ende[3]; int nb = 0;
        re15_k2080_bahn_rechnen(start, n->rot_y, wasser_2090(), ende, &nb);   /* FG-E4: der Fall endet im 2090-Becken */
        s_lande[0] = ende[0]; s_lande[1] = ende[2]; s_lande_ok = 1;
        bahn_start(start, n->rot_y);
        s_kan_zustand = 2;
        K_LOG("Wurf: Start (%d,%d,%d) v=(%d,%d,%d) a=(%d,%d,%d) -> Landung (%d,%d,%d) nach %d Bildern (Wasser 2090 y %d)",
              (int)start[0], (int)start[1], (int)start[2], (int)s_kan_v[0], (int)s_kan_v[1], (int)s_kan_v[2],
              (int)s_kan_a[0], (int)s_kan_a[1], (int)s_kan_a[2], (int)ende[0], (int)ende[1], (int)ende[2], nb, (int)wasser_2090());
    }
    if (s_kan_zustand == 2) fall_takt(wasser_2090());   /* die ersten Fall-Bilder noch in 2080, dann der Raumwechsel (FG-E4) */
}

/* FG-E3: die Aktor-Kopie vom Ausloesen in DENSELBEN Slot zuruecklegen. Der main.c-Portblock setzt nur einen neuen Alligator,
 * wenn keiner aktiv ist; der Boss-Takt findet state/hp != 0 und die Datei-Statics von Slot s (Phase, Aggro, nicht untergetaucht,
 * Intro vorbei) und kaempft weiter. Ist der Slot belegt (anderer Raumaufbau), bleibt es beim neuen Alligator (Logzeile). */
static void gator_zuruecklegen(unsigned raum)
{
    if (s_gator_slot <= 0 || s_gator_slot >= RE15_ACTOR_MAX) return;
    re15_actor_t *e = &g_actors[s_gator_slot];
    if (e->active) {
        K_LOG("FG-E3: Slot %d belegt (Typ 0x%02X) - kein Zuruecklegen", s_gator_slot, (unsigned)e->type);
    } else {
        *e = s_gator;
        K_LOG("FG-E3: ROOM%04X derselbe Alligator in Slot %d (%d,%d,%d) Gier %d hp %d Phase %d", raum, s_gator_slot, (int)e->x,
              (int)e->y, (int)e->z, (int)e->rot_y & 0xfff, (int)e->hp, re15_gator_boss_phase(e));
    }
    s_gator_slot = -1;
}

/* ---- Nachbesserung 2 (N3): Anschwimmen (Herleitung am Header RE15_K2080_ANSCHWIMM_AB, Dossier 9.3) --------------------------- */
static int32_t isqrt64(int64_t q)
{
    if (q <= 0) return 0;
    int64_t r = q < 0x7fffffff ? q : 0x7fffffff, alt;
    do { alt = r; r = (r + q / r) / 2; } while (r < alt);
    return (int32_t)alt;
}
/* 1 = DIESER Szenen-Kanister liegt in 2090/2091 (Bit 116, im Raum gesetzt) noch frei im Wasser (B-Stufe 0); c = Lage, d2/ab2 =
 * Abstandsquadrat Wurzel <-> Kanister und RE2-Zielmodus-Grenze 8880 x Zeichen-Skala (wie das Fenster, gator_re2_r41.c gr2_scale). */
static int anschwimm_lage(const re15_actor_t *e, int32_t c[2], int64_t *d2, int64_t *ab2)
{
    if (!e || !raum_2090((unsigned)g_current_room_id & 0xFFFFu) || !s_set_2090 || !bit(RE15_K2080_BIT_KANISTER)) return 0;
    if (re15_gr2_kanister_stufe() != 0) return 0;
    int32_t pos[3], rot[9];
    if (!re15_gr2_kanister_welt(pos, rot)) return 0;
    c[0] = pos[0]; c[1] = pos[2];
    const int64_t dx = (int64_t)pos[0] - e->x, dz = (int64_t)pos[2] - e->z;
    const int32_t sk = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    const int64_t ab = ((int64_t)RE15_K2080_ANSCHWIMM_AB * sk) >> 12;
    *d2 = dx * dx + dz * dz; *ab2 = ab * ab;
    return 1;
}
int re15_k2080_r41_anschwimmen_aktiv(const re15_actor_t *e)
{
    int32_t c[2]; int64_t d2 = 0, ab2 = 0;
    return anschwimm_lage(e, c, &d2, &ab2) && d2 >= ab2;
}
int re15_k2080_r41_anschwimmen(int slot, re15_actor_t *e, int32_t dist)
{
    int32_t c[2] = { 0, 0 }; int64_t d2 = 0, ab2 = 0;
    const int lage = anschwimm_lage(e, c, &d2, &ab2);
    const int an = lage && d2 >= ab2;
    if (an != s_anschw) {
        s_anschw = an;
        if (lage)
            K_LOG("ANSCHWIMMEN %s: Gator Slot %d (%d,%d) Gier %d, Kanister (%d,%d) Abstand %d, Grenze %d (RE2-Zielmodus 8880 @0x80100a88-8c)",
                  an ? "an" : "aus", slot, (int)e->x, (int)e->z, (int)e->rot_y & 0xfff, (int)c[0], (int)c[1], (int)isqrt64(d2),
                  (int)isqrt64(ab2));
        else
            K_LOG("ANSCHWIMMEN aus: Gator Slot %d (%d,%d) Gier %d, Kanister nicht mehr frei (Spur-B-Stufe %d)", slot, (int)e->x,
                  (int)e->z, (int)e->rot_y & 0xfff, re15_gr2_kanister_stufe());
    }
    if (!an) return 0;
    int32_t tx = c[0], tz = c[1];
    re15_gator_boss_steuerziel(e, &tx, &tz);              /* Spur B: Kanten-Umweg um Plattform/Rampe + Klemmflaeche */
    const int soll = ((int)re15_atan2_q12(tz - e->z, tx - e->x) - 0x400) & 0xfff;   /* Mesh-Gier zum Ziel (re15_actor.h:1196) */
    const int dlt = ((soll - (int)e->rot_y + 0x800) & 0xfff) - 0x800;
    if (dlt > 0x300 || dlt < -0x300) {
        /* Wende auf der Stelle wie die Verfolgung (Spur B, enemy_ai_boss_gator.c "WENDE STATT BOGEN": ab 0x300 Fehlwinkel Slew 0x60,
         * Clip ohne Vortrieb) - RE2 hat keine Lenkung, sein Gator schwimmt die Bahn, auf der der Kanister liegt */
        re15_enemy_steer_point(e, tx, tz, 0x60);
        re15_gr2_chase_clip(slot, e, dist, NULL);
    } else {
        int32_t v = 0;
        re15_enemy_steer_point(e, tx, tz, 0x30);          /* Slew 0x30 wie die Verfolgung */
        re15_gr2_chase_clip(slot, e, dist, &v);            /* RE2 Sub 1/2: Clip 0/1 nach der Hysterese, Wurzelversatz */
        re15_ai_advance(e, v);
    }
    e->y = RE15_K2080_GATOR_WASSER_Y;                      /* GB_WATER_Y @0x8010c65c */
    return 1;
}

/* ---- Installer --------------------------------------------------------------------------------------------------------- */
void re15_kanister2080_r41_install(uint16_t room_id)
{
    const unsigned raum = (unsigned)room_id & 0xFFFFu;
    if (!raum_2090(raum) && s_set_2090) {                /* B's Kanister ist ein Port-Prop ohne Raumbindung */
        re15_gr2_kanister_entfernen();
        s_set_2090 = 0;
    }
    if (raum_2080(raum)) {
        re15_msg_install_text(42, elza() ? k_k2080_msg42_elza : k_k2080_msg42_leon,
                              elza() ? sizeof k_k2080_msg42_elza : sizeof k_k2080_msg42_leon);
        {
            const uint8_t *t = elza() ? k_k2080_msg42_elza : k_k2080_msg42_leon;
            const int len = elza() ? (int)sizeof k_k2080_msg42_elza : (int)sizeof k_k2080_msg42_leon;
            const int d = re15_msg_compute_duration(t, (size_t)len, 0);
            if (d > 0 && d < 65535) re15_msg_install_durations(42, d);
        }
        if (s_phase == RE15_K2080_NACH_2080) {
            s_phase = RE15_K2080_IN_2080;
            s_kan_zustand = 0; s_ziel_slot = -1; s_ziel_n = 0; s_kopf_ab = 0; s_z42_an = s_z42_aus = -1;
            /* Nachbesserung 2 (N3): Wurfrichtung nach der Lage des Alligators beim Ausloesen (Header RE15_K2080_WURF_GIER_NORD/_SUED) */
            s_wurf_gier = re15_k2080_wurf_gier_wahl(s_gator_slot, s_gator.x, s_gator.z);
            bit_setzen(RE15_K2080_BIT_SUEDWURF, s_wurf_gier == RE15_K2080_WURF_GIER_SUED);
            {   int32_t l[2]; nenn_landung(wurf_gier(), l);
                K_LOG("Wurfrichtung %d (%s, Alligator beim Ausloesen (%d,%d)) -> Nennlandung (%d,%d)%s", wurf_gier(),
                      wurf_gier() == RE15_K2080_WURF_GIER_SUED ? "Suedbecken" : "Nordbecken", (int)s_gator.x, (int)s_gator.z,
                      (int)l[0], (int)l[1], wurf_gier_messhaken() >= 0 ? " [MESSHAKEN RE15_K2080_WURF_GIER]" : ""); }
            baue_szene();
            const int slot = starten(s_prog_szene, s_prog_szene_n);
            bit_setzen(RE15_K2080_BIT_ANKUNFT, 1);
            K_LOG("ROOM%04X SZENE (%s 0x%02X, %d lebende Zombies, %d B Programm) Faden %d", raum, elza() ? "John" : "Marvin",
                  re15_k2080_npc_typ(), lebende_zombies(), s_prog_szene_n, slot);
            return;
        }
        s_kan_zustand = 0;
        if (!bit(RE15_K2080_BIT_F_SZENE) || bit(RE15_K2080_BIT_SZENE)) return;
        /* Nachtrag 17: ERST der Knall (Anstoss hier im Raumaufbau, vor dem ersten VM-Lauf), DANN die Zombies (Sce_em_set im
         * ersten VM-Lauf des Raumaufbaus) - Reihenfolge wie Runde 36 K3 "Erst Tuerknallen, dann Zombie Spawnen". */
        if (!bit(RE15_K2080_BIT_ANKUNFT)) {
            bit_setzen(RE15_K2080_BIT_ANKUNFT, 1);
            s_zaehler[0]++;
            if (re15_k2080_knall_hook) re15_k2080_knall_hook();
            K_LOG("Knall (Knall-Bank Satz 0 = RE2 ROOM1060 Se_on 0x0a @0x0F48, Tuerknall der Irons-/Elza-Szene) - erstes "
                  "Betreten nach Bit 110, Bit %d = 1", RE15_K2080_BIT_ANKUNFT);
        }
        baue_spawn();
        const int slot = starten(s_prog_spawn, s_prog_spawn_n);
        K_LOG("ROOM%04X nach Bit 110: %d lebende Zombies (Tot-Bits %d..%d) Faden %d, Band %d (oberhalb der Treppe @0x0BE2)", raum,
              lebende_zombies(), RE15_K2080_TOT0, RE15_K2080_TOT0 + RE15_K2080_N_ZOMBIES - 1, slot, RE15_K2080_ETAGE);
        return;
    }
    if (raum_2090(raum)) {
        if (s_phase == RE15_K2080_IN_2080) {             /* Rueckkehr aus dem 2080-Teil */
            s_phase = RE15_K2080_AUS;
            bit_setzen(RE15_K2080_BIT_SZENE, 1);
            bit_setzen(RE15_K2080_BIT_KANISTER, 1);
            gator_zuruecklegen(raum);                    /* FG-E3: derselbe Alligator (vor dem main.c-Portblock) */
            baue_rueck();
            const int slot = starten(s_prog_rueck, s_prog_rueck_n);
            if (s_kan_zustand != 2) sig_setzen(RE15_K2080_SIG_GELANDET, 1);   /* kein Fall unterwegs: nicht auf den Aufschlag warten */
            K_LOG("ROOM%04X RUECKKEHR: Spieler (%d,%d,%d), Programm Faden %d, Bits %d/%d = 1, Zone-7-Tot-Bits %d..%d gesetzt: %d von %d",
                  raum, (int)s_leon[0], (int)s_leon[1], (int)s_leon[2], slot, RE15_K2080_BIT_SZENE, RE15_K2080_BIT_KANISTER,
                  RE15_K2080_TOT0, RE15_K2080_TOT0 + RE15_K2080_N_ZOMBIES - 1, RE15_K2080_N_ZOMBIES - lebende_zombies(),
                  RE15_K2080_N_ZOMBIES);
        }
        if (bit(RE15_K2080_BIT_KANISTER) && s_kan_zustand != 2) {
            /* Kanister liegt schon (der Fall laeuft sonst noch, fall_takt legt ihn an der Aufschlagstelle ab). Spielstand ohne
             * Laufzeit-Bahn: dieselbe Bahn aus der Nennlage (Wurf-Stand + Eigen-c6-Ende, Wurf-Gier, 2090-Wasser). */
            if (!s_lande_ok) {
                nenn_landung(wurf_gier(), s_lande);       /* Bit 117: Nord- oder Suedwurf (Nachbesserung 2) */
                s_lande_ok = 1;
            }
            re15_gr2_kanister_setzen(s_lande[0], RE15_K2080_GATOR_WASSER_Y, s_lande[1]);
            s_set_2090 = 1;
            s_zaehler[4]++;
            K_LOG("ROOM%04X Kanister im Becken (%d,%d,%d) = Aufschlagstelle (Bit %d)", raum, (int)s_lande[0],
                  RE15_K2080_GATOR_WASSER_Y, (int)s_lande[1], RE15_K2080_BIT_KANISTER);
        }
        return;
    }
    if (s_phase == RE15_K2080_IN_2080 || s_phase == RE15_K2080_NACH_2080) {   /* Szene durch einen fremden Raum abgebrochen */
        K_LOG("Szene abgebrochen in ROOM%04X (Phase %d)", raum, s_phase);
        s_phase = RE15_K2080_AUS;
    }
}
