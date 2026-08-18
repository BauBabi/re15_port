/*
 * RE1.5 Rebuilt — RESIDENT EVIL 2 SPIDER brain (kind 0x25 Adult / 0x26 Baby).
 * PORT OPTION (OPTIONS→AI→RE2), kein RE1.5-Original-Verhalten. WELLE E.
 *
 * QUELLE + HERKUNFTSBEWEIS (selbst nachgerechnet 2026-08-18, nicht uebernommen):
 *   CDEMD0.EMS-Sektor-TOC der RE2-Retail-EXE @0x8009adf4 (vendored engine/src/gen/
 *   re2_ems_toc.inc, 300 x {u32 Sektor, u32 Groesse}, 4 Records je kind ab 0x10):
 *     kind 0x25: [Ovl-Slot1 Sektor 1812/0x66D4 B] [Ovl-Slot0 Sektor 1825/0x66D4 B]
 *                [TIM Sektor 1838/0x10420 B]      [EMD Sektor 1871/0xD0C8 B]
 *     kind 0x26: [Ovl-Slot1 Sektor 1898/0x10FA B] [Ovl-Slot0 Sektor 1901/0x10FA B]
 *                [TIM Sektor 1904/0x8220 B]       [EMD Sektor 1921/0x1A14 B]
 *   Der Slot-0-Record ist gelinkt @0x80100000. Verifikation (SHA1, eigener Lauf):
 *     shared_assets/RE2/CDEMD0.EMS[0x390800 .. +26324]  == info/re2leon/COMMON/BIN/EMS25.BIN
 *     shared_assets/RE2/CDEMD0.EMS[0x3B6800 .. + 4346]  == info/re2leon/COMMON/BIN/EMS26.BIN
 *   → die KI-Module liegen im AUSGELIEFERTEN Archiv des Ports; nichts Fremdes wird geladen.
 *   disassemble: .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr> <n> --bin EMS25.BIN
 *
 * ADULT (EMS25.BIN) — Root @0x801000C8:
 *   Pause-Gate `lw 0x800CFBDC & 0x20000000` @0x801000D4-E4; +0x1D3-low7-Dec @0x801000EC-100;
 *   +0x23D-Dec @0x80100104-114; Dispatch `lbu +0x4` @0x80100118 ueber die Zustandstabelle
 *   @0x80106420 (a0=self, a1=+0x108, a2=+0x17C):
 *     [0] 0x801001EC INIT   [1] 0x801005AC ACTIVE   [2] 0x80102C78 HURT
 *     [3] 0x80103C80 DEATH  [4] 0x80104CF0          [5][6] NULL   [7] 0x80104CF8 CORPSE
 *   Danach +0x106 = (-Y)/1800 (Magic-Div 0x91A2B3C5>>10 @0x80100140-70, numerisch
 *   nachgerechnet: 1800 mit Trunkierung), FUN_80035530(self), FUN_8003567C(self,1536),
 *   FUN_80104DE8(self) (Boden-Automat +0x225), FUN_80106140(self) (FX-Emitter +0x23B).
 *
 *   ACTIVE @0x801005AC ist eine ZWEISTUFIGE Maschine — erst der OBERFLAECHEN-MODUS +0x222
 *   (`lbu v0,546(s2)` @0x801005D4, Tabelle @0x80106440), dann je Modus die Substate-Tabelle
 *   auf +0x5:
 *     Modus 0 BODEN  @0x80100688 -> Tabelle @0x80106450 (10 Substates)
 *     Modus 1 DECKE  @0x801013AC -> Tabelle @0x8010649C
 *     Modus 2 UEBERG.@0x80101CC8 -> Tabelle @0x801064CC
 *     Modus 3 WAND   @0x80102B40 -> Tabelle @0x801064FC
 *   +0x222 wird NICHT geraten: die INIT-Sprungtabelle @0x80100004 (12 Eintraege, Index
 *   `+0x10E & 0xF` @0x80100430-58) setzt 0 (@0x80100460), 1 (@0x8010047C, dazu Roll
 *   +0x78 = 2048 = 180° -> kopfueber) oder 3 (@0x801004B4, dazu Yaw +0x76 =
 *   ((+0x10E-4)>>1)<<10 = Wandquadrant).
 *
 * PORT-MAPPINGS (nichts erfunden — jede Luecke ist unten als OPEN markiert):
 *   - +0x10E (RE2-Spawn-Deskriptor) <- RE1.5 actor.grid_id. BEIDE sind das Spawn-Byte, das
 *     Boden- gegen Decken-Spawn waehlt: der RE1.5-Spinnen-INIT liest `grid_id & 0xf` genauso
 *     (<2 Boden / 2..0x1f Decken-Abwurf, enemy_ai_common.c re15_adult_spider_ai_tick).
 *     DEKLARIERTES MAPPING, kein byte-true Beleg fuer die Gleichheit der Wertebereiche.
 *   - Ziel-Wahl FUN_80105F98 (@0x80105F98-6000): waehlt Partner (*0x800CFE18) statt Spieler,
 *     wenn `0x800CFBD8 & 0x10000000` und dessen Distanz kleiner ist. RE1.5 hat keinen
 *     Partner-Slot -> +0x237 bleibt IMMER 0, der Partner-Zweig ist tot (dokumentiert, nicht
 *     entfernt: die Distanzquelle +0x1F4 waere sonst stumm falsch).
 *   - FUN_800152C8(self, yaw_off) (EXE, selbst disassembliert @0x800152C8-534C):
 *       lhu +0x76; += a1; RotMatrix(0,yaw+a1,0) 0x8008E1F4; ApplyMatrixSV 0x8008DBA4 auf
 *       dem SVECTOR (+0x144,+0x146,+0x148); +0x38 += out.x @0x80015314-20; +0x40 += out.z
 *       @0x80015324-34. Port: re2s_thrust() — dieselbe Rotation ueber re15_cos/sin_q12
 *       (das Welle-B-Muster re2z_thrust, identische Yaw-Konvention).
 *   - FUN_80015614(self,tx,tz,arc) = re15_ai_arc_test: selbst disassembliert @0x80015614-98,
 *     Rueckgabe 0 (innerhalb ±arc) / +arc / -arc — DIESELBE Semantik wie die Port-Funktion,
 *     inklusive des `< 0x801`-Vorzeichen-Splits.
 *   - FUN_80015558(self,tx,tz,slew) = re15_enemy_steer_point (Welle-B/C-Zuordnung).
 *   - FUN_8002959C = re15_re2_advance_959c (Welle C, Kern 0x80029B28-4C).
 *   - FUN_80015FE8 = re15_re2_rand (der EINE RE2-PRNG; die ZIEHUNGSANZAHL ist Verhalten und
 *     wird hier 1:1 nachgezogen, auch wenn der Wert verworfen wird).
 *   - OPEN: FUN_80105070 (Bone-Root-Motion aus der Skelett-Matrix in +0x144, EXE-Matrix-
 *     Kette 0x8008E1F4/0x8002CE94/0x8008D2F4) hat keinen Port-Kanal (der Port hat keine
 *     per-Bone-Weltmatrix im AI-Tick) -> der Root-Motion-Anteil ist 0, der Tabellen-Anteil
 *     (Speed 100/60/20) laeuft byte-true. Das ist EINE dokumentierte Luecke, kein Ersatzwert.
 *   - OPEN: word0-Bits aus FUN_80106200 (@0x80106200-B8) und FUN_80104DE8 sind Render-/
 *     Draw-Bits ohne Port-Kanal -> nicht modelliert (kein Konsument im Modul).
 *   - Rumble 0x8003947C/0x80039514 (@0x80105B44/54): Pad-Aktuator, im Port nicht vorhanden.
 */

#include <stdint.h>
#include <stdlib.h>   /* getenv, atoi */
#include <stdio.h>
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 / re15_atan2_q12 */
#include "re15_damage.h"     /* re15_ai_arc_test (== FUN_80015614), re15_enemy_player_dist */
#include "re15_enemy_ai.h"   /* re15_actor_clip_len */
#include "re15_enemy.h"      /* re15_enemy_find (RE2-Bank des Welle-A-Loaders) */
#include "re15_esp.h"        /* re15_esp_fx_spawn_ex (RE1.5-Blut-Stand-in) */

extern void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);

/* ---- ENEMSE-Audio-Hook -------------------------------------------------------------------
 * Paar-Tabelle @0x800A7400 (RE2 PSX.EXE, vendored s_re2_enemse_pairs): kind 0x25 steht in
 * GENAU ZWEI Zeilen — 53 = {0x25,0x1F} und 54 = {0x25,0x20} — beide Male als ERSTER kind
 * (FUN_80052b38-Treffer (a) -> die zweite Map-Haelfte gehoert dem PARTNER, die Spinne spielt
 * aus der ERSTEN Haelfte, flag2000 = 0). EDT-Probe ueber ENEMSE.VBS (eigener Lauf 2026-08-18,
 * TOC @0x800A7B1C): die ersten Haelften von Bank 53 und 54 sind BYTE-IDENTISCH (memcmp), ids
 * 0..13 belegt, 14/15 = 0xFFFFFFFF (stumm). Das Modul feuert die ids {0,1,3,5,6,8,9} (eigener
 * jal-0x8005bd6c-Scan ueber EMS25.BIN) — alle gedeckt. -> Default 53.
 * kind 0x26 steht in Zeile 24 = {0x26,0x00} (reine Baby-Bank, erste Haelfte); ids 0..8 belegt,
 * das Baby-Modul feuert nur id 7 (@0x80100CF4 in EMS26.BIN) — gedeckt.
 * Bank-Wahl bleibt wie bei Hund/Kraehe eine DEKLARIERTE NAEHERUNG: der byte-true Mechanismus
 * FUN_80052b38 keyt auf das RE2-Raum-Byte Spawn-Record+7, das RE1.5-Raeume nicht liefern
 * (vollstaendige Begruendung + Refutation in re2_ems.c re2_enemse_select_bank). */
#define RE2SPIDER_ENEMSE_BANK      53
#define RE2SPIDER_BABY_ENEMSE_BANK 24

static void (*s_re2s_se_fn)(int se_id, int flag2000) = 0;
static void (*s_re2s_bank_fn)(int bank) = 0;

void re15_re2spider_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int), int baby)
{
    s_re2s_se_fn   = se_fn;
    s_re2s_bank_fn = bank_fn;
    if (s_re2s_bank_fn) {
        const char *ov = getenv("RE15_RE2_SPIDER_SE_BANK");
        s_re2s_bank_fn(ov ? atoi(ov)
                          : (baby ? RE2SPIDER_BABY_ENEMSE_BANK : RE2SPIDER_ENEMSE_BANK));
    }
}
static void re2s_se(int id) { if (s_re2s_se_fn) s_re2s_se_fn(id, 0); }   /* erste Map-Haelfte */

/* ---- kleine Helfer ------------------------------------------------------------------------ */

/* Der PRNG-Zaehler ist selbst Verhalten (Welle-B-Disziplin): jede `jal 0x80015fe8`-Stelle im
 * Original zieht hier genau einmal, auch wenn das Ergebnis verworfen wird. */
static uint32_t re2s_rand(void) { return re15_re2_rand(); }

/* Zustandswort `sw <w>,4(self)` — +0x4/+0x5/+0x6/+0x7 in EINEM Store (little-endian). */
static void re2s_word(re15_actor_t *e, uint32_t w)
{
    e->state       = (uint8_t)(w & 0xffu);
    e->sub_state_1 = (uint8_t)((w >> 8)  & 0xffu);
    e->sub_state_2 = (uint8_t)((w >> 16) & 0xffu);
    e->sub_state_3 = (uint8_t)((w >> 24) & 0xffu);
}

/* Clip-Wort `sw (rate<<16)|(frame<<8)|clip, 332(self)` (+0x14C) — dieselbe Konvention, die
 * Welle B/C/D etabliert haben: rate-Haelfte -> Crossfade-Seed, Clip-Byte -> motion. */
static void re2s_clip(re15_actor_t *e, uint32_t w)
{
    e->motion      = (int16_t)(w & 0xffu);
    e->anim_frame  = (int32_t)((w >> 8) & 0xffu);
    e->anim_frac   = (uint8_t)((w >> 16) & 0xffu);
    e->anim_freeze = 0;
    e->anim_flags &= (uint16_t)~(0x80u | 0x04u);
}

/* FUN_8002959C(self, +0x108, +0x17C, blend) — 1 Frame/Tick, Wrap auf 0, `done` NUR am
 * Wrap-Tick (Kern 0x80029B28-4C, Welle C). */
static int re2s_advance(re15_actor_t *e, int blend) { return re15_re2_advance_959c(e, blend); }

/* FUN_800152C8(self, yaw_off): (+0x144,+0x146,+0x148) um (+0x76 + yaw_off) gedreht auf
 * +0x38/+0x40 addieren. Der Port fuehrt nur die X/Z-Ebene (Y-Komponente +0x146 hat im
 * Adult-Modul keinen Schreiber ausser INIT-Clear -> bleibt 0). */
static void re2s_thrust(re15_actor_t *e, int yaw_off)
{
    int32_t sx = e->speed_h, sz = e->re2s_z148;
    if (!sx && !sz) return;
    int a = (int)e->rot_y + yaw_off;
    int32_t c = re15_cos_q12(a), s = re15_sin_q12(a);
    e->x += (int32_t)((c * sx - s * sz) >> 12);
    e->z -= (int32_t)((s * sx + c * sz) >> 12);
}

/* Distanz zum aktuellen Ziel: +0x237 ? (+0x1F4 & 0x3FFFFFFF) : +0x1F0. +0x237 ist im Port
 * immer 0 (kein Partner-Slot), der Zweig bleibt als Beleg stehen. */
static uint32_t re2s_tdist(const re15_actor_t *e) { return e->ai_dist; }

/* FUN_80105190(self, arc) @0x80105190-5218: Arc-Test gegen das Ziel; bei +0x222 != 0 wird das
 * Ziel am eigenen Ort GESPIEGELT (`subu a1,v1,a1` / `subu a2,v0,a2` @0x801051E0/EC), weil die
 * Spinne an Decke/Wand verkehrt haengt. Rueckgabe = FUN_80015614 (0 / ±arc). */
static int re2s_arc(const re15_actor_t *e, const re15_actor_t *pl, int arc)
{
    int32_t tx = pl->x, tz = pl->z;
    if (e->re2s_mode222 != 0) {                    /* @0x801051BC-EC */
        tx = e->x - (pl->x - e->x);
        tz = e->z - (pl->z - e->z);
    }
    return re15_ai_arc_test(e, tx, tz, arc);
}

/* FUN_800154AC(ax,az,bx,bz) — Peilung A->B. Port-Konvention wie re2z_sector
 * (enemy_ai_re2_zombie.c): atan2 minus den +0x400-Mesh-Versatz. */
static int re2s_bearing(int32_t ax, int32_t az, int32_t bx, int32_t bz)
{
    return ((int)re15_atan2_q12(bz - az, bx - ax) - 0x400) & 0xfff;
}

/* ---- FUN_80106004 — Schritt-SE-Rotation @0x80106004-98 -----------------------------------
 *   lw +0x178; *(ptr) & 0x100000  -> nur auf dem Frame-Flag-Tick
 *   +0x10C != 0 ? se_id = 10 : se_id = tbl[+0x238]   (tbl @0x801066C8 = {0,8,9}, selbst
 *   aus der Datei gelesen)      ; FUN_8005BD6C(se_id, self) @0x80106050
 *   +0x238 = (+0x238 + 1) % 3   (Magic-Mod @0x80106058-84)
 * PORT: der Frame-Flag-Kanal (EDD-Flagwort *(+0x178) Bit 0x100000) existiert im Port nicht als
 * per-Frame-Bit -> der SE wird am CLIP-WRAP gefeuert (dieselbe dokumentierte Naeherung, die
 * Welle C fuer den Hunde-Schritt-SE benutzt). Die %3-Rotation laeuft byte-true. */
static void re2s_step_se(re15_actor_t *e, int frame_flag)
{
    if (!frame_flag) return;
    static const uint8_t tbl[3] = { 0, 8, 9 };            /* @0x801066C8 */
    re2s_se(tbl[e->re2s_se238 % 3u]);                     /* +0x10C-Zweig (se 10): OPEN, kein
                                                           * Port-Produzent fuer +0x10C */
    e->re2s_se238 = (uint8_t)((e->re2s_se238 + 1u) % 3u); /* @0x80106058-84 */
}

/* ---- FUN_80106140 — FX-Emitter @0x80106140-61FC ------------------------------------------
 *   +0x23B == 0 -> nichts. Sonst +0x23B--; nur wenn (+0x23B & 1) == 0 (@0x8010616C-70):
 *   DREI Durchlaeufe (@0x801061D4-DC), je: bone = tbl[rand & 7] (tbl @0x801066CC =
 *   {0,3,6,9,11,0,16,19}); wenn (bone_record.word0 & 0x4B) == 1 -> FUN_801056DC(self,bone,9).
 * PORT: FUN_801056DC ist der Bone-Partikel-Spawner (Tabelle @0x80106684, 6-Byte-Records) —
 * der Port hat keinen Bone-Partikel-Kanal -> RE1.5-ESP-Blut-Stand-in am Aktor-Ursprung
 * (dieselbe deklarierte Naeherung wie re2z_blood_fx). Die RNG-Ziehungen (3 je Feuer-Tick)
 * laufen byte-true, weil sie den geteilten Strom bewegen. */
static void re2s_fx_tick(re15_actor_t *e)
{
    if (e->re2s_fx23b == 0) return;                       /* @0x80106160 */
    e->re2s_fx23b--;                                      /* @0x80106164-68 */
    if (e->re2s_fx23b & 1u) return;                       /* @0x8010616C-70 */
    for (int i = 0; i < 3; i++) {                         /* @0x801061D4-DC */
        (void)re2s_rand();                                /* @0x8010617C */
        /* (bone_record & 0x4B) == 1 (@0x801061C0-C4): das Bone-Record-Wort hat im Port keinen
         * Zwilling -> das Gate ist OPEN und feuert hier NICHT. Der RNG-Zug bleibt. */
    }
}

/* ---- FUN_80105AE0 / FUN_801059F0 — der ANGRIFF ------------------------------------------
 * FUN_80105AE0(self, range, y_tol) @0x80105AE0-BD8:
 *   |+0x3C - *0x800CFC34| < y_tol  (@0x80105AF8-B18)  ->  FUN_801059F0(self, 0, range)
 *   Treffer -> SE 1 (`addiu a0,zero,1` @0x80105B34, `jal 0x8005bd6c` @0x80105B38),
 *              Rumble 0x8003947C(2,0) @0x80105B44 + 0x80039514(8,200,0) @0x80105B54 (kein
 *              Port-Kanal), Rueckgabe 1.
 * FUN_801059F0(self, which, range) @0x801059F0-5ADC:
 *   dist = which ? (+0x1F4 & 0x3FFFFFFF) : +0x1F0   (@0x80105A10-34)
 *   dist < range ?  (@0x80105A3C-40)
 *   Ziel +0x1D3 & 0x80 gesetzt -> 0 (@0x80105A48-54: schon von jemand anderem geclaimt)
 *   SPIELERSCHADEN: `addiu a0,zero,20` @0x80105A78 ; `jal 0x800401d4` @0x80105A7C mit
 *   `addu a1,zero,zero` @0x80105A80  ->  20 HP, Modus 0.
 *   Rueckgabe von 0x800401D4 == 2 -> FUN_80105F68: Ziel +0x4 = 3 (TOD) @0x80105F84-94
 *   sonst  -> a1 = FUN_80015910(self, Ziel) & 0xFF ; FUN_80105F20: Ziel +0x4 = 2,
 *             +0x5 = a1 + 2, +0x6/+0x7 = 0, +0x1D3 |= 0x80 (@0x80105F40-64).
 * PORT: der Schaden laeuft ueber re15_re2_player_damage_mode(pl, 20, 0) — dieselbe
 * One-Save-Latch-Funktion, die Zombie/Hund/Kraehe teilen. Die Trefferrichtung
 * FUN_80015910 hat keinen Port-Zwilling -> die Spieler-HURT-Variante bleibt dem
 * Port-Schadensweg ueberlassen (OPEN, wie beim Hunde-Biss). */
#define RE2SPIDER_BITE_DMG 20      /* @0x80105A78 */

static int re2s_attack(re15_actor_t *e, re15_actor_t *pl, int range, int y_tol)
{
    int32_t dy = e->y - pl->y;                             /* +0x3C - 0x800CFC34 @0x80105AF8-08 */
    if (dy < 0) dy = -dy;                                  /* @0x80105B0C-14 */
    if (dy >= y_tol) return 0;                             /* @0x80105B18-1C */
    if (re2s_tdist(e) >= (uint32_t)range) return 0;        /* @0x80105A3C-40 */
    if (re15_player_is_grabbed()) return 0;                /* Ziel+0x1D3 & 0x80 @0x80105A48-54 */
    (void)re15_re2_player_damage_mode(pl, RE2SPIDER_BITE_DMG, 0);   /* @0x80105A78-80 */
    re2s_se(1);                                            /* @0x80105B34-38 */
    return 1;
}

/* ============================ INIT — Zustand 0 @0x801001EC ================================
 * +0x4 = 1 (Zustandswort 0x00000001) @0x80100214; word0 |= 0x00C00000 @0x80100224.
 * HP-Zug @0x80100240-308: zwei Globals-Bits waehlen eine von vier 16-Halbwort-Tabellen,
 * Index rand & 0xF, dazu ein zweiter Zug rand & 3:
 *     *0x800CFB74 & 0x40 == 0 : &0x20 ? tbl@0x801062E4 : tbl@0x801062C4
 *     *0x800CFB74 & 0x40 != 0 : &0x20 ? tbl@0x80106354 : tbl@0x80106334
 *   `sh v0,342(s2)` @0x80100308 -> +0x156 = HP.
 *   0x800CFB74 ist das RE2-Schwierigkeits-/Szenario-Wort; RE1.5 hat keinen Zwilling ->
 *   MAPPING: beide Bits 0 (die Basis-Tabelle @0x801062C4). DEKLARIERT, nicht geraten:
 *   die anderen drei Tabellen stehen unten mit ihren Adressen, sie sind nur unerreichbar.
 * Danach: +0x151/+0x152/+0x153 = 6 @0x80100318-20; +0x1D0/+0x1CC/+0x1C8 = 0 @0x80100324-2C;
 *   12 Worte ab +0x218 auf 0 @0x80100330-40; +0x14C = 1 @0x8010035C (Clip 1) mit
 *   FUN_8002959C(a3=256) @0x80100358; +0x1C0 = 0 / +0x1C1 = 8 @0x80100380-84;
 *   +0x221 = 8 @0x801003B4; +0x1E8 = 1 @0x801003B8; Hitbox-Block @0x801063A0 -> +0x84..+0xA0
 *   @0x801003BC-400; +0x1EE = 800 @0x80100420.
 * Zum Schluss die Spawn-Sprungtabelle @0x80100004[+0x10E & 0xF] (@0x80100430-58, Gate < 12). */
static const int16_t s_re2s_hp_base[16] =        /* @0x801062C4 (Bits 0x40/0x20 beide 0) */
    { 99, 99, 99, 99, 119, 99, 99, 119, 99, 99, 99, 119, 99, 89, 99, 99 };
/* Unerreichbar unter dem Port-MAPPING, aber belegt: @0x801062E4 == @0x80106354 =
 * {79,89,99,79,49,89,79,69,89,69,89,89,99,69,49,79}; @0x80106334 =
 * {109,109,109,109,129,109,109,129,109,109,109,129,109,99,109,109}. */

static void re2s_init(re15_actor_t *e)
{
    re2s_word(e, 0x00000001u);                             /* @0x80100214 */

    uint32_t r1 = re2s_rand();                             /* @0x80100240 / @0x801002A4 */
    uint32_t r2 = re2s_rand();                             /* @0x801002C0 / @0x801002E4 */
    int16_t hp = (int16_t)(s_re2s_hp_base[r2 & 0xfu] + (int)(r1 & 3u));  /* @0x801002E4-308 */
    if (!e->re2s_seeded) { e->hp = hp; e->re2s_seeded = 1; }

    e->re2z_pool151 = 6; e->re2z_pool152 = 6; e->re2z_pool153 = 6;   /* @0x80100318-20 */
    e->re2s_t218 = 0; e->re2s_yaw21a = 0; e->re2s_t21c = 0;          /* @0x80100330-40 (12 Worte */
    e->re2s_c21e = 0; e->re2s_mode222 = 0; e->re2s_gs225 = 0;        /*  ab +0x218 = 0)          */
    e->re2s_y22c = 0; e->re2s_q230 = 0; e->re2s_c232 = 0;
    e->re2s_snap233[0] = e->re2s_snap233[1] = e->re2s_snap233[2] = 0;
    e->re2s_tgt237 = 0; e->re2s_se238 = 0; e->re2s_c23a = 0;
    e->re2s_row23c = 0; e->re2s_dead239 = 0; e->re2s_done224 = 0;
    e->re2s_fx23b = 0; e->re2s_c23d = 0;
    e->speed_h = 0; e->re2d_vy146 = 0; e->re2s_z148 = 0;
    e->re2z_t158 = 0; e->re2z_t15a = 0;

    re2s_clip(e, 0x00000001u);                             /* +0x14C = 1 @0x8010035C */
    (void)re2s_advance(e, 256);                            /* @0x80100358 */

    /* Spawn-Sprungtabelle @0x80100004, Index (+0x10E & 0xF), Gate `sltiu v0,v1,0xc`
     * @0x8010043C. Eintraege (aus der Datei gelesen):
     *   [0]=0x80100460 [1]=0x8010058C(nichts) [2]=[3]=0x8010047C [4..11]=0x801004B4 */
    unsigned k = (unsigned)(e->re2z_f10e & 0xfu);
    if (k >= 12u) return;                                  /* @0x8010043C-40 */
    if (k == 0u) {                                         /* @0x80100460 BODEN */
        e->re2s_mode222 = 0;                               /* +0x222 = 0 @0x80100460 */
        e->re2s_yaw21a  = 0;                               /* +0x21A = 0 @0x80100464 */
    } else if (k == 2u || k == 3u) {                       /* @0x8010047C DECKE */
        e->re2s_mode222 = 1;                               /* @0x80100480 */
        e->re2s_yaw21a  = 2048;                            /* @0x80100488 */
        e->rot_z        = 2048;                            /* +0x78 = 2048 (kopfueber) @0x8010048C */
        e->re2s_y22c    = e->y + 1250;                     /* @0x801004A4-B0 */
    } else if (k >= 4u) {                                  /* @0x801004B4 WAND */
        e->re2s_mode222 = 3;                               /* @0x801004C0 */
        e->re2s_yaw21a  = 2048;                            /* @0x801004C8 */
        e->re2s_q230    = (uint8_t)(((int)(e->re2z_f10e & 0xffu) - 4) >> 1);   /* @0x801004CC-DC */
        e->rot_y        = (int16_t)(e->re2s_q230 << 10);   /* +0x76 = q<<10 @0x801004E0-E4 */
        unsigned hi = (unsigned)((e->re2z_f10e & 0xf0u) >> 4);                 /* @0x80100548-58 */
        e->re2s_y22c    = hi ? (1250 - 1800 * (int32_t)hi) : -3960;            /* @0x8010055C-80 */
        e->re2s_gs225   = 1;                               /* +0x225 = 1 @0x80100584-88 */
    }
    /* k == 1: die Tabelle springt direkt auf den Epilog @0x8010058C — nichts zu tun. */
}

/* ============================ ACTIVE Modus 0 (BODEN) =====================================
 * Handler @0x80100688: `lbu +0x5` @0x80100690 -> Tabelle @0x80106450 (aus der Datei gelesen):
 *   [0]=0x80100784 [1]=[2]=[3]=0x801008CC [4]=0x80100A7C [5]=[6]=0x80100B80
 *   [7]=0x80100CE0  [8]=0x80100F88  [9]=0x80101214
 * Direkt dahinter (@0x80106478) liegen die drei 3-Wort-Records der Lauf-Substates. */

/* --- FUN_801006C4 — der IDLE-Clip-Treiber (+0x6) @0x801006C4-780 ---
 *   P0 @0x80100704: +0x14C = 0x00030000 (Clip 0, Rate 3); +0x6 = 1; +0x158 = (rand+10) & 0x1F
 *   P1 @0x80100728: FUN_8002959C(a3=1024); +0x158-- (Store IMMER, Delay-Slot @0x80100748);
 *                   war +0x158 == 0 -> Zustandswort 0x101 (Sub 1) @0x80100754, und mit
 *                   (rand & 1) != 0 stattdessen 0x401 (Sub 4) @0x80100760-64. */
static void re2s_idle_clip(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {                             /* @0x801006E8 */
        re2s_clip(e, 0x00030000u);                         /* @0x80100704-08 */
        e->sub_state_2 = 1;                                /* @0x80100714 */
        e->re2z_t158 = (int16_t)((re2s_rand() + 10u) & 0x1fu);   /* @0x80100718-20 */
        return;                                            /* faellt im Original durch: P1 folgt */
    }
    if (e->sub_state_2 != 1) return;                       /* @0x801006F4-FC */
    (void)re2s_advance(e, 1024);                           /* @0x80100730-34 */
    uint16_t t = (uint16_t)e->re2z_t158;
    e->re2z_t158 = (int16_t)(t - 1u);                      /* @0x80100748 (immer) */
    if (t != 0u) return;                                   /* @0x80100744 */
    re2s_word(e, 0x101u);                                  /* @0x80100754 */
    if (re2s_rand() & 1u) re2s_word(e, 0x401u);            /* @0x80100750-64 */
}

/* --- Substate 0 @0x80100784 — IDLE + Entscheidungsleiter --- */
static void re2s_m0_sub0(re15_actor_t *e, re15_actor_t *pl)
{
    re2s_idle_clip(e);                                     /* @0x80100798 */
    uint32_t d = re2s_tdist(e) & 0xffffu;                  /* lhu +0x1F0 @0x801007BC */
    int a128  = re2s_arc(e, pl, 128);                      /* @0x801007C0 */
    int a256  = re2s_arc(e, pl, 256);                      /* @0x801007D0 */
    int a1024 = re2s_arc(e, pl, 1024);                     /* @0x801007DC */
    (void)a256;   /* das Ergebnis wird im Original berechnet und NICHT gelesen (@0x801007D4
                   * ueberschreibt s2 mit dem 128er-Ergebnis) — der Aufruf bleibt fuer die
                   * Seiteneffekt-Treue stehen. */

    if ((a1024 & 0xffff) != 0 && d < 0x1f40u)              /* @0x801007E4-F4 */
        re2s_word(e, 0x501u);                              /* @0x801007FC */

    /* `lhu 0x800CFBF6 & 1` @0x80100800-10. BELEGT, dass dieser Zweig auch im ORIGINAL tot ist:
     * ein eigener Immediate-Scan der RE2-Retail-PSX.EXE ueber alle sb/sh/sw mit Offset 0xFBF6
     * findet GENAU VIER Schreiber — den Clear `andi v1,v1,0xffe0` @0x8003BFE0 und drei Setzer
     * `ori 0x2` @0x8003CC80, `ori 0x2` @0x8003D6B4, `ori 0x4` @0x8003D18C. Bit 0x01 wird
     * NIRGENDS gesetzt; die fuenf Gegner-Module (EMS25/EMS26/EMZ0/EMD0G_MOD0/EMOVL21_S0)
     * enthalten ausschliesslich `lhu` auf diese Adresse. Der Port bildet das 1:1 ab (Bit 0
     * konstant 0) — Substate 9 ist dadurch aus Substate 0 unerreichbar, exakt wie im Original.
     * (Der Port-Zwilling fuer die Bits 0x2/0x4 ist re2z_cfbf6 in enemy_ai_re2_zombie.c.) */
    if (0) {                                               /* 0x800CFBF6 & 1 @0x80100800-10 */
        re2s_word(e, 0x901u);                              /* @0x8010081C */
        if (re2s_rand() & 1u) re2s_word(e, 0x501u);        /* @0x80100818-2C */
    }

    if (e->re2s_t218 != 0) return;                         /* @0x80100830-38 */

    if (d < 0x1d4cu) {                                     /* @0x80100840 */
        if ((a128 & 0xffff) == 0 && d >= 0xbb9u)           /* @0x8010084C-58 */
            re2s_word(e, 0x701u);                          /* @0x80100860-64 */
    }
    if (d < 0x1770u && (a128 & 0xffff) == 0 && d >= 0x3e9u) {   /* @0x8010086C-80 */
        uint32_t r = re2s_rand() & 0xfu;                   /* @0x80100888-90 */
        /* `srav v1,832,r` @0x80100898, `andi v1,1` @0x8010089C: 832 = 0b1101000000,
         * Bits 6/8/9 -> 3 von 16 Ziehungen gehen auf Sub 8. */
        re2s_word(e, ((832u >> r) & 1u) ? 0x801u : 0x701u);     /* @0x801008A4-AC */
    }
}

/* --- Substates 1/2/3 @0x801008CC — LAUFEN (drei Gangarten) ---
 * Param-Records @0x80106478 (3 Worte je Substate, Index +0x5 - 1):
 *   Sub 1: {Clip 2, Drehrate 48, Speed 100}   @0x80106478/7C/80
 *   Sub 2: {Clip 3, Drehrate 16, Speed  60}   @0x80106484/88/8C
 *   Sub 3: {Clip 3, Drehrate 16, Speed  20}   @0x80106490/94/98  (aus der Datei gelesen) */
static const struct { uint8_t clip; int16_t turn; int16_t spd; } s_re2s_walk[3] = {
    { 2, 48, 100 }, { 3, 16, 60 }, { 3, 16, 20 }
};

static void re2s_m0_walk(re15_actor_t *e, re15_actor_t *pl)
{
    unsigned i = (unsigned)(e->sub_state_1 - 1u);
    if (i > 2u) i = 2u;                                    /* Tabelle hat 3 Records */

    if (e->sub_state_2 == 0) {                             /* @0x801008F0 */
        e->sub_state_2 = 1;                                /* @0x80100934 */
        e->re2s_z148   = 0;                                /* +0x148 = 0 @0x8010093C */
        re2s_clip(e, 0x00030000u | s_re2s_walk[i].clip);   /* @0x80100938-48 */
        e->re2z_t158   = (int16_t)(re2s_rand() & 0x2eu);   /* @0x8010094C-50 */
    }
    if (e->sub_state_2 != 1) { re2s_thrust(e, e->re2s_yaw21a); return; }   /* @0x80100A54-58 */

    int turn = re2s_arc(e, pl, s_re2s_walk[i].turn);       /* @0x80100974-78 */
    e->rot_y = (int16_t)(e->rot_y + turn);                 /* +0x76 += @0x8010098C-9C */
    int wrapped = re2s_advance(e, 1024);                   /* @0x80100998 */
    re2s_step_se(e, wrapped);                              /* FUN_80106004 @0x801009A0 */
    /* FUN_80105070(self,1) @0x801009AC fuellt +0x144 mit der Bone-Root-Motion — OPEN
     * (kein Port-Kanal, s. Dateikopf); der Port startet den Schritt bei 0. */
    e->speed_h = 0;
    e->re2z_t15a = (int16_t)e->speed_h;                    /* +0x15A = +0x144 @0x801009B4-C4 */
    e->speed_h = (int16_t)(e->speed_h + s_re2s_walk[i].spd);   /* @0x801009DC-F4 */

    uint16_t t = (uint16_t)e->re2z_t158;
    e->re2z_t158 = (int16_t)(t - 1u);                      /* @0x801009E4-E8 */
    if (t == 0u) { re2s_word(e, 1u); }                     /* @0x801009F0/@0x80100A4C-50 */
    else if (e->re2s_mode222 == 0 && re2s_tdist(e) < 0x5dcu)    /* @0x801009F8-A48 */
        re2s_word(e, 1u);                                  /* Ziel unter 1500 -> neu entscheiden */

    re2s_thrust(e, e->re2s_yaw21a);                        /* FUN_800152C8 @0x80100A58 */
}

/* --- Substate 4 @0x80100A7C — Drehen auf der Stelle ---
 *   P0 @0x80100ABC: +0x14C = 0x00030003 (Clip 3); +0x6 = 1; +0x158 = (rand+10) & 0x3F;
 *                   +0x15A = 24, mit (rand & 1) == 0 -> -24 (@0x80100AE0-F8);
 *                   +0x222 != 0 -> +0x15A = -(+0x15A) (@0x80100AFC-B18).
 *   P1 @0x80100B20: +0x76 += +0x15A; FUN_8002959C(1024); FUN_80106004;
 *                   +0x158-- ; war 0 -> Zustandswort 1 (@0x80100B5C-60). */
static void re2s_m0_turn(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {                             /* @0x80100AA0 */
        re2s_clip(e, 0x00030003u);                         /* @0x80100ABC-C4 */
        e->sub_state_2 = 1;                                /* @0x80100AD0 */
        e->re2z_t158 = (int16_t)((re2s_rand() + 10u) & 0x3fu);   /* @0x80100AD4-DC */
        int16_t st = 24;                                   /* @0x80100AE0 */
        if ((re2s_rand() & 1u) == 0u) st = -24;            /* @0x80100AEC-F8 */
        if (e->re2s_mode222 != 0) st = (int16_t)(-st);     /* @0x80100AFC-B18 */
        e->re2z_t15a = st;
    } else if (e->sub_state_2 != 1) return;

    e->rot_y = (int16_t)(e->rot_y + e->re2z_t15a);         /* @0x80100B28-3C */
    int wrapped = re2s_advance(e, 1024);                   /* @0x80100B38 */
    re2s_step_se(e, wrapped);                              /* @0x80100B40 */
    uint16_t t = (uint16_t)e->re2z_t158;
    e->re2z_t158 = (int16_t)(t - 1u);                      /* @0x80100B58 */
    if (t == 0u) re2s_word(e, 1u);                         /* @0x80100B5C-60 */
}

/* --- Substates 5/6 @0x80100B80 — Anpirschen mit Drehen ---
 * Param-Tabelle @0x80100034 (4 Worte, aus der Datei gelesen): {Clip 2, Drehrate 128} fuer
 * Sub 5, {Clip 3, Drehrate 32} fuer Sub 6 (Index (+0x5 - 5) * 8 @0x80100C14-1C/@0x80100C50-58).
 *   P0 @0x80100C0C: +0x6 = 1; +0x14C = Clip | 0x30000; +0x15A = (rand & 0x1F) + 60
 *   P1 @0x80100C48: +0x76 += arc(Drehrate); +0x15A--; war 0 ODER Drehung == 0 -> +0x6 = 2;
 *                   FUN_8002959C(1024) + FUN_80106004
 *   P2 @0x80100CC0: Zustandswort 1 */
static const struct { uint8_t clip; int16_t turn; } s_re2s_stalk[2] = { { 2, 128 }, { 3, 32 } };

static void re2s_m0_stalk(re15_actor_t *e, re15_actor_t *pl)
{
    unsigned i = (unsigned)(e->sub_state_1 - 5u);
    if (i > 1u) i = 1u;

    if (e->sub_state_2 == 0) {                             /* @0x80100BE8 */
        e->sub_state_2 = 1;                                /* @0x80100C28 */
        re2s_clip(e, 0x00030000u | s_re2s_stalk[i].clip);  /* @0x80100C2C-38 */
        e->re2z_t15a = (int16_t)((re2s_rand() & 0x1fu) + 60u);   /* @0x80100C3C-44 */
    }
    if (e->sub_state_2 == 1) {                             /* @0x80100BD4 */
        int turn = re2s_arc(e, pl, s_re2s_stalk[i].turn);  /* @0x80100C5C-64 */
        e->rot_y = (int16_t)(e->rot_y + turn);             /* @0x80100C6C-78 */
        uint16_t t = (uint16_t)e->re2z_t15a;
        e->re2z_t15a = (int16_t)(t - 1u);                  /* @0x80100C84 */
        if (t == 0u || (turn & 0xffff) == 0)               /* @0x80100C80-8C */
            e->sub_state_2 = 2;                            /* @0x80100C94-98 */
        int wrapped = re2s_advance(e, 1024);               /* @0x80100CA8 */
        re2s_step_se(e, wrapped);                          /* @0x80100CB0 */
        return;
    }
    if (e->sub_state_2 == 2) re2s_word(e, 1u);             /* @0x80100CC0 */
}

/* --- Substate 7 @0x80100CE0 — LUNGE + BISS (9 Phasen, Sprungtabelle @0x80100044) ---
 * Tabelleneintraege (aus der Datei gelesen, Gate `sltiu v0,v1,0x9` @0x80100D24):
 *   [0]=0x80100D48 [1]=0x80100D60 [2]=0x80100D9C [3]=0x80100DC0 [4]=0x80100E24
 *   [5]=0x80100E50 [6]=0x80100F00 [7]=0x80100F1C [8]=0x80100F50 */
static void re2s_m0_attack(re15_actor_t *e, re15_actor_t *pl)
{
    int wrapped;
    switch (e->sub_state_2) {
    case 0:   /* @0x80100D48 */
        re2s_clip(e, 0x00030004u);                         /* Clip 4 @0x80100D48-50 */
        e->sub_state_2 = 1;                                /* @0x80100D58 */
        e->re2s_z148   = 0;                                /* @0x80100D5C */
        /* faellt durch nach P1 */
        /* fallthrough */
    case 1:   /* @0x80100D60 */
        wrapped    = re2s_advance(e, 1024);                /* @0x80100D6C */
        e->speed_h = 100;                                  /* +0x144 = 100 @0x80100D84 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + wrapped);   /* @0x80100D88-90 */
        re2s_thrust(e, 0);                                 /* FUN_800152C8(self,0) @0x80100D8C */
        break;
    case 2:   /* @0x80100D9C */
        re2s_clip(e, 0x00030005u);                         /* Clip 5 @0x80100D9C-A8 */
        e->re2z_t158   = (int16_t)((re2s_rand() & 0x1fu) + 20u);  /* @0x80100DAC-B4 */
        e->sub_state_2 = 3;                                /* @0x80100DB8-BC */
        /* fallthrough */
    case 3: {  /* @0x80100DC0 — Zielen (arc 64) */
        int turn = re15_ai_arc_test(e, pl->x, pl->z, 64);  /* @0x80100DCC-D0 */
        e->rot_y = (int16_t)(e->rot_y + turn);             /* @0x80100DE4-F4 */
        (void)re2s_advance(e, 1024);                       /* @0x80100DF0 */
        if ((turn & 0xffff) == 0) e->sub_state_2 = 4;      /* @0x80100DF8-04 */
        uint16_t t = (uint16_t)e->re2z_t158;
        e->re2z_t158 = (int16_t)(t - 1u);                  /* @0x80100E18 */
        if (t == 0u) e->sub_state_2 = 6;                   /* @0x80100E14-20 */
        break;
    }
    case 4:   /* @0x80100E24 */
        re2s_clip(e, 0x00030005u);                         /* Clip 5 @0x80100E24-2C */
        e->sub_state_2 = 5;                                /* @0x80100E30-38 */
        e->re2z_t158   = (int16_t)((re2s_rand() & 0x1fu) + 10u);  /* @0x80100E3C-44 */
        e->speed_h     = 300;                              /* +0x144 = 300 @0x80100E48-4C */
        /* fallthrough */
    case 5: {  /* @0x80100E50 — der Sprung */
        re15_enemy_steer_point(e, pl->x, pl->z, 16);       /* FUN_80015558 @0x80100E5C-60 */
        re2s_thrust(e, e->re2s_yaw21a);                    /* @0x80100E64-68 */
        (void)re2s_advance(e, 1024);                       /* @0x80100E7C */
        uint16_t t = (uint16_t)e->re2z_t158;
        e->re2z_t158 = (int16_t)(t - 1u);                  /* @0x80100E94 */
        if (t == 0u) { e->sub_state_2 = 6; break; }        /* @0x80100E90/@0x80100ED8 */
        /* +0xD != 255 (Body-Push-Stempel) -> abbrechen @0x80100E98-A4: im Port ist +0xD
         * nicht als Pusher-Id vorhanden -> Gate OPEN, feuert nicht.
         * (+0x110 & 1) (Wandkontakt) -> abbrechen @0x80100EA8-B8. */
        if (e->ai_contact & 1u) { e->sub_state_2 = 6; break; }        /* @0x80100EB0-B8 */
        if (re15_ai_arc_test(e, pl->x, pl->z, 512) != 0) {            /* @0x80100EC8-D0 */
            e->sub_state_2 = 6; break;                                /* @0x80100ED8 */
        }
        if (re2s_attack(e, pl, 1500, 4300))                /* FUN_80105AE0 @0x80100EE4-E8 */
            e->sub_state_2 = 6;                            /* @0x80100EF0-F8 */
        break;
    }
    case 6:   /* @0x80100F00 */
        re2s_clip(e, 0x00030007u);                         /* Clip 7 @0x80100F00-08 */
        e->speed_h     = 100;                              /* @0x80100F0C-10 */
        e->sub_state_2 = 7;                                /* @0x80100F14-18 */
        /* fallthrough */
    case 7:   /* @0x80100F1C */
        re2s_thrust(e, e->re2s_yaw21a);                    /* @0x80100F1C-20 */
        wrapped = re2s_advance(e, 1024);                   /* @0x80100F34 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + wrapped);   /* @0x80100F3C-4C */
        break;
    case 8:   /* @0x80100F50 */
        re2s_word(e, 1u);                                  /* @0x80100F50-54 */
        e->re2s_t218 = 15;                                 /* +0x218 = 15 @0x80100F58-5C */
        break;
    default: break;
    }
    re2s_step_se(e, 0);   /* FUN_80106004 @0x80100F60 — Frame-Flag-Kanal OPEN (s. re2s_step_se) */
}

/* --- FUN_8010109C — die OBERFLAECHEN-UMSETZUNG (+0x7) @0x8010109C-1210 ---
 *   P0 @0x801010F4: +0x14C = 0x0003000A (Clip 10); +0x7 = 1
 *   P1 @0x80101104: FUN_8002959C(1024); auf dem Frame-Flag *(+0x178) & 0x10000
 *                   (@0x80101110-24): SE 3 (@0x80101128-34), +0x7 = 2 (@0x80101138), dann
 *                   +0x222 < 2 ? FUN_801058A4(self, +0x222+8, +0x76 + +0x21A)
 *                              : FUN_801058A4(self, dist<3000 ? 137 : 136, FUN_80102B10(self))
 *   P2 @0x801011E0: FUN_8002959C(1024); done -> +0x7 = 3
 *   P3: Rueckgabe 1 (fertig), sonst 0.
 * FUN_801058A4 ist der Oberflaechen-Anker (Positions-/Yaw-Setzer aus einer Bone-Matrix) — im
 * Port OPEN (keine Bone-Weltmatrix im AI-Tick); der Zustandsfluss laeuft byte-true. */
static int re2s_surface_switch(re15_actor_t *e)
{
    switch (e->sub_state_3) {
    case 0:                                                /* @0x801010F4 */
        re2s_clip(e, 0x0003000au);                         /* @0x801010F4-100 */
        e->sub_state_3 = 1;                                /* @0x801010F8-FC */
        /* fallthrough */
    case 1: {                                              /* @0x80101104 */
        int wrapped = re2s_advance(e, 1024);               /* @0x80101108 */
        if (wrapped) {                                     /* Frame-Flag-Naeherung, s.o. */
            re2s_se(3);                                    /* @0x80101128-34 */
            e->sub_state_3 = 2;                            /* @0x80101138 */
        }
        return 0;
    }
    case 2:                                                /* @0x801011E0 */
        if (re2s_advance(e, 1024)) e->sub_state_3 = 3;     /* @0x801011E4-F4 */
        return 0;
    case 3: return 1;                                      /* @0x801010E4-E8 */
    default: return 0;                                     /* @0x801010EC-F0 */
    }
}

/* --- Substate 8 @0x80100F88 — Oberflaechenwechsel einleiten ---
 *   P0 @0x80100FF0: +0x14C = 0x0003000B (Clip 11); +0x6 = 1;
 *                   FUN_801058A4(self, +0x222 + 8, (s16)(+0x76 + +0x21A))
 *   P1 @0x80101024: FUN_8002959C(1024); +0x6 += done
 *   P3 @0x8010104C: FUN_8010109C -> != 0 faellt nach P2
 *   P2 @0x80101060: Zustandswort 1; +0x218 = 15
 *   Tail: FUN_80106004 + FUN_80016028 */
static void re2s_m0_switch(re15_actor_t *e)
{
    switch (e->sub_state_2) {
    case 0:                                                /* @0x80100FF0 */
        re2s_clip(e, 0x0003000bu);                         /* @0x80100FF0-000 */
        e->sub_state_2 = 1;                                /* @0x80101004-08 */
        /* FUN_801058A4(self, +0x222+8, yaw+0x21A) @0x8010101C — OPEN (s.o.) */
        /* fallthrough */
    case 1: {                                              /* @0x80101024 */
        int wrapped = re2s_advance(e, 1024);               /* @0x80101030 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + wrapped);   /* @0x80101038-48 */
        break;
    }
    case 3:                                                /* @0x8010104C */
        if (!re2s_surface_switch(e)) break;                /* @0x80101050-58 */
        /* fallthrough */
    case 2:                                                /* @0x80101060 */
        re2s_word(e, 1u);                                  /* @0x80101064 */
        e->re2s_t218 = 15;                                 /* @0x80101060-68 */
        break;
    default: break;
    }
    re2s_step_se(e, 0);                                    /* @0x8010106C */
}

/* --- Substate 9 @0x80101214 — der SEITWAERTS-SPRUNG ---
 *   P0 @0x80101290: +0x14C = 0x00030008 (Clip 8); +0x6 = 1;
 *     Peilung FUN_800154AC(self -> Ziel) @0x801012B0 -> sp+50
 *     r1 = rand (@0x801012BC), r2 = rand (@0x801012C4)
 *     Winkel = Peilung + r1 + ((r2 & 1) << 11) + 896   (@0x801012D8-F4) -> +0x158
 *     RotMatrix(0, Winkel, 0) @0x801012F8
 *     r3 = rand (@0x80101300); Betrag = (r3 & 0x3F) + 400 (@0x80101314-1C)
 *     ApplyMatrixSV -> (+0x144,+0x146,+0x148) = R_y(Winkel) * (Betrag,0,0) @0x80101324
 *   P1 @0x8010132C: +0x76 += arc(96) @0x80101330; FUN_800152C8(self, +0x21A + +0x158)
 *                   @0x80101350; FUN_8002959C(1024) @0x80101364; +0x6 += done
 *   P2 @0x80101380: Zustandswort 1 (a0 traegt hier noch die 1 von @0x80101258); +0x21E = 0 */
static void re2s_m0_leap(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {
    case 0: {                                              /* @0x80101290 */
        re2s_clip(e, 0x00030008u);                         /* @0x80101298-9C */
        e->sub_state_2 = 1;                                /* @0x801012A0-A4 */
        int bear = re2s_bearing(e->x, e->z, pl->x, pl->z); /* @0x801012B0 */
        uint32_t r1 = re2s_rand();                         /* @0x801012BC */
        uint32_t r2 = re2s_rand();                         /* @0x801012C4 */
        int ang = (int)((uint16_t)(bear + r1 + ((r2 & 1u) << 11) + 896u));  /* @0x801012D8-F4 */
        e->re2z_t158 = (int16_t)ang;                       /* +0x158 @0x801012FC */
        uint32_t r3 = re2s_rand();                         /* @0x80101300 */
        int mag = (int)(r3 & 0x3fu) + 400;                 /* @0x80101314-1C */
        /* R_y(ang) * (mag,0,0) -> (+0x144,+0x146,+0x148) @0x80101324 */
        e->speed_h   = (int16_t)((re15_cos_q12(ang) * mag) >> 12);
        e->re2d_vy146 = 0;
        e->re2s_z148 = (int16_t)((-re15_sin_q12(ang) * mag) >> 12);
        /* fallthrough */
    }
    case 1: {                                              /* @0x8010132C */
        int turn = re2s_arc(e, pl, 96);                    /* @0x80101330-34 */
        e->rot_y = (int16_t)(e->rot_y + turn);             /* @0x80101344-54 */
        re2s_thrust(e, e->re2s_yaw21a + e->re2z_t158);     /* @0x80101350 */
        int wrapped = re2s_advance(e, 1024);               /* @0x80101364 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + wrapped);   /* @0x8010136C-7C */
        break;
    }
    case 2:                                                /* @0x80101380 */
        re2s_word(e, 1u);                                  /* @0x80101380 */
        e->re2s_c21e = 0;                                  /* +0x21E = 0 @0x80101384 */
        break;
    default: break;
    }
}

static void re2s_mode0(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_1) {                              /* @0x80100690, Tabelle @0x80106450 */
    case 0: re2s_m0_sub0(e, pl);   break;
    case 1: case 2: case 3: re2s_m0_walk(e, pl);  break;
    case 4: re2s_m0_turn(e);       break;
    case 5: case 6: re2s_m0_stalk(e, pl); break;
    case 7: re2s_m0_attack(e, pl); break;
    case 8: re2s_m0_switch(e);     break;
    case 9: re2s_m0_leap(e, pl);   break;
    default: break;
    }
}

/* ============================ ACTIVE @0x801005AC ==========================================
 *   FUN_80105F98(self) @0x801005C8 — Ziel-Wahl (+0x237); im Port immer Spieler.
 *   Modus-Dispatch `lbu +0x222` @0x801005D4 ueber @0x80106440.
 *   FUN_80106200(self) @0x801005F8 — word0-Draw-Bits (kein Port-Kanal, OPEN).
 *   +0x218 != 0 -> -- (@0x80100600-14); +0x232 != 0 -> -- (@0x80100618-28);
 *   +0x21C != 0 ? +0x21E++ (@0x8010063C-4C) : (+0x21E = 0, +0x233/4/5 = +0x5/+0x6/+0x7,
 *                                              @0x80100650-68). */
static void re2s_active(re15_actor_t *e, re15_actor_t *pl)
{
    e->re2s_tgt237 = 0;                                    /* FUN_80105F98 @0x80105FA0 */

    /* WELLE E deckt Modus 0 (BODEN) end-to-end ab. Modus 1/2/3 (Decke/Uebergang/Wand,
     * Handler @0x801013AC / @0x80101CC8 / @0x80102B40) sind NICHT portiert — der
     * Besitz-Gate re15_re2spider_owns() laesst Decken-/Wand-Spawns gar nicht erst in dieses
     * Brain, also ist der Zweig hier unerreichbar und wird NICHT erfunden. */
    if (e->re2s_mode222 == 0) re2s_mode0(e, pl);           /* @0x801005D4-F0 */

    if (e->re2s_t218 != 0) e->re2s_t218--;                 /* @0x80100600-14 */
    if (e->re2s_c232 != 0) e->re2s_c232--;                 /* @0x80100618-28 */
    if (e->re2s_t21c != 0) {
        e->re2s_c21e++;                                    /* @0x8010063C-4C */
    } else {
        e->re2s_c21e = 0;                                  /* @0x8010065C */
        e->re2s_snap233[0] = e->sub_state_1;               /* @0x80100650-60 */
        e->re2s_snap233[1] = e->sub_state_2;               /* @0x80100664 */
        e->re2s_snap233[2] = e->sub_state_3;               /* @0x80100668 */
    }
}

/* ============================ HURT — Zustand 2 @0x80102C78 ================================
 * Kopf @0x80102C78-CF4:
 *   +0x6 == 0 -> +0x23C = +0x5 (die Waffenzeile latchen) @0x80102C94-AC
 *   +0x10C == 0 -> +0x10C = FUN_800527B4(X,Z) (WASSER-Oberflaechen-Y, EXE @0x800527B4: AOT-Scan ueber sce==7) @0x80102CB0-D0
 *   Zeilen-Dispatch `lbu +0x5` @0x80102CD8 ueber die 20-Eintrag-Tabelle @0x80106518:
 *     rows 0,1,2,3,12,13,19 -> 0x80102D18   rows 5,6,7,8,17 -> 0x80102FDC
 *     rows 4,15,18          -> 0x8010370C   row 9 -> 0x80103800   row 10 -> 0x801038E4
 *     row 11 -> 0x801039AC  row 14 -> 0x80103B10  row 16 -> 0x80103B78
 *   0x80102D18 dispatcht dann +0x222 ueber @0x80106568 = {0x80102D54, 0x80102E78,
 *   0x80102F2C, 0x80102F2C}.
 * Modus 0 @0x80102D54:
 *   P0 @0x80102DB0: +0x14C = 0x00030001 (Clip 1); +0x6 = 1; +0x23C != 1 ->
 *                   FUN_801056DC(self,0,0) (Blut-Record 0 an Bone 0) @0x80102DD0
 *   P1 @0x80102DD8: FUN_8002959C(1024); +0x6 += done @0x80102DEC-FC
 *   P2 @0x80102E00: +0x5 == 1            -> Zustandswort 0x901 (ACTIVE Sub 9)
 *                   sonst +0x1D2 / 3 == 0 -> Zustandswort 0x901   (Magic-Div @0x80102E14-28)
 *                   sonst rand & 7 -> Byte-Tabelle @0x80106578 = {1,9,1,9,1,5,1,5}
 *                        -> Zustandswort (tbl<<8) + 1  @0x80102E34-58                       */
static const uint8_t s_re2s_hurt_next[8] = { 1, 9, 1, 9, 1, 5, 1, 5 };   /* @0x80106578 */

static void re2s_hurt(re15_actor_t *e, re15_actor_t *pl)
{
    (void)pl;
    if (e->sub_state_2 == 0) e->re2s_row23c = e->sub_state_1;   /* +0x23C @0x80102C94-AC */
    /* +0x10C (Boden-Y) hat im Port keinen Zwilling -> OPEN, kein Konsument in Modus 0. */

    /* PORT-SICHERUNG (deklariert, NICHT byte-true — dasselbe Muster wie RE2Z_ROW_FALLBACK):
     * die Zeilen-Handler @0x80102FDC/@0x8010370C/@0x80103800/@0x801038E4/@0x801039AC/
     * @0x80103B10/@0x80103B78 sind in Welle E NICHT portiert. Statt zu erfinden oder den
     * Aktor haengen zu lassen, laeuft die generische Zeile (@0x80102D18/@0x80102D54). */
    switch (e->sub_state_2) {
    case 0:                                                /* @0x80102DB0 */
        re2s_clip(e, 0x00030001u);                         /* @0x80102D90/B0/C4 */
        e->sub_state_2 = 1;                                /* @0x80102DBC */
        if (e->re2s_row23c != 1)                           /* +0x23C != 1 @0x80102DC0 */
            re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500,
                                 e->x, e->y, e->z, (int16_t)e->rot_y);   /* @0x80102DD0 */
        /* fallthrough */
    case 1: {                                              /* @0x80102DD8 */
        int wrapped = re2s_advance(e, 1024);               /* @0x80102DE4 */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + wrapped);   /* @0x80102DEC-FC */
        break;
    }
    case 2:                                                /* @0x80102E00 */
        if (e->sub_state_1 == 1) { re2s_word(e, 0x901u); break; }        /* @0x80102E08-58 */
        if ((e->re2z_hits1d2 / 3u) == 0u) { re2s_word(e, 0x901u); break; }  /* @0x80102E10-30 */
        re2s_word(e, ((uint32_t)s_re2s_hurt_next[re2s_rand() & 7u] << 8) + 1u);  /* @0x80102E34-58 */
        break;
    default: break;
    }
}

/* ============================ DEATH — Zustand 3 @0x80103C80 ===============================
 * Kopf @0x80103C80-D04: dieselbe +0x23C-/+0x10C-Vorbereitung wie HURT, dann Zeilen-Dispatch
 * ueber @0x801065A8, danach FUN_80104F18(self) @0x80103D04 und EXE 0x80018FB0 @0x80103D0C.
 * Zeilen-Handler @0x80103D30: +0x224 != 0 -> FUN_80104440; sonst +0x222 ueber @0x801065F8 =
 * {0x80103D8C, 0x80103EDC, 0x801041CC, 0x801041CC}.
 * Modus 0 @0x80103D8C:
 *   P0 (+0x6 == 0) @0x80103DC0: +0x14C = 0x0007000C (Clip 12, Rate 7); +0x6 = 1;
 *        +0x23C != 1 -> FUN_801056DC(self,0,0) @0x80103DE4; word0 |= 0x42 @0x80103DF4
 *   IMMER: FUN_8002959C(a3=512) @0x80103E08
 *   Auf dem Frame-Flag *(+0x178) & 0x80000 (@0x80103E10-24):
 *        word0 &= 0xE3FFFFFF @0x80103E38
 *        +0x10C != 0 (@0x80103E3C):
 *           +0x23C in {9,10} oder == 16 -> FUN_801056DC(self,0,7) @0x80103E70 (Gore-Record 7)
 *           +0x23A = -1 (IMMER, Delay-Slot @0x80103E84); +0x23E == 0 -> +0x23E = 3, +0x23F = 1
 *        Zustandswort = 7 (CORPSE) @0x80103E9C
 *        +0x239 == 0 -> HP(+0x156) = 1 @0x80103EB0 und +0x224 = 1 @0x80103EB8               */
static void re2s_death(re15_actor_t *e, re15_actor_t *pl)
{
    (void)pl;
    if (e->sub_state_2 == 0) e->re2s_row23c = e->sub_state_1;   /* +0x23C @0x80103C9C-B4 */
    if (e->sub_state_2 == 0) {                             /* @0x80103DB8 */
        re2s_clip(e, 0x0007000cu);                         /* Clip 12 @0x80103DC0-C8 */
        e->sub_state_2 = 1;                                /* @0x80103DD4 */
        if (e->re2s_row23c != 1)                           /* +0x23C != 1 @0x80103DDC */
            re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500,
                                 e->x, e->y, e->z, (int16_t)e->rot_y);   /* @0x80103DE4 */
    }
    int wrapped = re2s_advance(e, 512);                    /* @0x80103E08 */
    if (!wrapped) return;   /* Frame-Flag *(+0x178) & 0x80000 @0x80103E10-24 — im Port als
                             * Clip-Wrap genaehert (dieselbe deklarierte Naeherung wie der
                             * Schritt-SE; das Original feuert auf dem markierten EDD-Frame). */
    e->re2s_c23a = -1;                                     /* @0x80103E84 (immer) */
    re2s_word(e, 7u);                                      /* CORPSE @0x80103E9C */
    if (e->re2s_dead239 == 0u) {                           /* @0x80103EA0-A8 */
        e->hp = 1;                                         /* @0x80103EB0 */
        e->re2s_done224 = 1;                               /* @0x80103EB8 */
    }
}

/* ============================ Zustand 4 @0x80104CF0 = `jr ra` ============================= */
static void re2s_state4(re15_actor_t *e) { (void)e; }      /* @0x80104CF0-F4 */

/* ============================ CORPSE — Zustand 7 @0x80104CF8 ==============================
 *   +0x5 == 0 @0x80104D10: done = FUN_8002959C(a3=512) @0x80104D18;
 *        done != 0 -> +0x5 = 1 (@0x80104D2C) und +0x16A = (rand & 0x1F) + 30 (@0x80104D28-3C)
 *   +0x5 != 0 @0x80104D40: +0x16A--; wurde 0 -> +0x14C = 0x0007000D (Clip 13, Rate 7)
 *        @0x80104D5C-60 und +0x5 = 0 @0x80104D64
 *   +0x6 == 0 @0x80104D70: +0x6 = 1, +0x225 = 2, +0x15A = 0 (@0x80104D78-84)
 *   +0x23A < 0 -> FUN_80104F18(self) @0x80104D98 (Blut-Lache-Treiber; im Port OPEN)
 *   word0 |= 0x400 @0x80104DB0; Y < Spieler-Y -> word0 |= 0x00800000 @0x80104DD0 (Render).  */
static void re2s_corpse(re15_actor_t *e)
{
    if (e->sub_state_1 == 0) {                             /* @0x80104D10 */
        if (re2s_advance(e, 512)) {                        /* @0x80104D18-20 */
            e->sub_state_1 = 1;                            /* @0x80104D2C */
            e->re2z_dir16a = (uint8_t)((re2s_rand() & 0x1fu) + 30u);   /* +0x16A @0x80104D30-3C */
        }
    } else {                                               /* @0x80104D40 */
        e->re2z_dir16a--;                                  /* @0x80104D48-4C */
        if ((uint8_t)e->re2z_dir16a == 0u) {               /* @0x80104D50-54 */
            re2s_clip(e, 0x0007000du);                     /* Clip 13 @0x80104D5C-60 */
            e->sub_state_1 = 0;                            /* @0x80104D64 */
        }
    }
    if (e->sub_state_2 == 0) {                             /* @0x80104D70 */
        e->sub_state_2 = 1;                                /* @0x80104D78 */
        e->re2s_gs225  = 2;                                /* @0x80104D80 */
        e->re2z_t15a   = 0;                                /* @0x80104D84 */
    }
}

/* ============================ Besitz-Gate ================================================= */
/* Welle E besitzt einen Aktor nur, wenn sein Spawn-Deskriptor den BODEN-Modus waehlt — genau
 * die INIT-Sprungtabellen-Indizes 0 und 1 (@0x80100460 / @0x8010058C). Decken- (2/3) und
 * Wand-Spawns (4..11) landen in +0x222 = 1 bzw. 3, deren ACTIVE-Handler (@0x801013AC /
 * @0x80102B40) in Welle E NICHT portiert sind; die bleiben deshalb ueber ihre GANZE Lebenszeit
 * auf dem unveraenderten byte-true RE1.5-Brain (kein Flavor-Wechsel mitten im Kampf). */
int re15_re2spider_owns(const re15_actor_t *e)
{
    if (!e) return 0;
    if (e->type != 0x25u) return 0;   /* WELLE E portiert NUR die Adult-Spinne (EMS25.BIN).
                                       * Das Baby-Modul EMS26.BIN ist vollstaendig RE'd
                                       * (Root @0x8010001C, Tabelle @0x80101084, HP = 1
                                       * @0x801000F8, Biss 1 HP `addiu a0,zero,1` /
                                       * `jal 0x800401d4` @0x80100EAC, volle Wasserlogik
                                       * ueber FUN_800527B4), aber NOCH NICHT portiert —
                                       * es liefe sonst faelschlich auf den Adult-Tabellen.
                                       * 0x26 bleibt deshalb auf dem byte-true RE1.5-Brain. */
    return (unsigned)(e->grid_id & 0xfu) < 2u;             /* +0x10E & 0xF -> INIT-Tabelle */
}

void re15_re2spider_room_reset(void) { }                   /* Welle E haelt keinen Raum-Zustand */

/* ============================ Root @0x801000C8 ============================================ */
int re15_re2spider_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* Pause-Gate `lw 0x800CFBDC & 0x20000000` @0x801000D4-E4: der Freeze-Zweig @0x80100198
     * klemmt nur Y an die Boden-/Wandhoehe (FUN_800527B4 + +0x1C2) und kehrt zurueck.
     * Der Aufrufer (enemy_ai_common.c) haelt die Pause bereits ab. */

    if ((e->re2z_self1d3 & 0x7fu) != 0u)                   /* @0x801000EC-F8 */
        e->re2z_self1d3--;                                 /* @0x801000FC-100 (Bit 0x80 bleibt) */
    if (e->re2s_c23d != 0u) e->re2s_c23d--;                /* @0x80100104-114 */

    e->ai_dist = (uint32_t)re15_enemy_player_dist(e, pl);  /* +0x1F0 (EXE-seitig gefuellt) */

    switch (e->state) {                                    /* @0x80100118, Tabelle @0x80106420 */
    case 0: re2s_init(e);              break;              /* @0x801001EC */
    case 1: re2s_active(e, pl);        break;              /* @0x801005AC */
    case 2: re2s_hurt(e, pl);          break;              /* @0x80102C78 */
    case 3: re2s_death(e, pl);         break;              /* @0x80103C80 */
    case 4: re2s_state4(e);            break;              /* @0x80104CF0 */
    case 7: re2s_corpse(e);            break;              /* @0x80104CF8 */
    default: break;                                        /* [5]/[6] sind NULL @0x80106434/38 */
    }

    e->re2s_f106 = (uint8_t)((-e->y) / 1800);              /* @0x80100140-70 (Magic-Div) */
    re2s_fx_tick(e);                                       /* FUN_80106140 @0x80100188 */
    return 1;
}
