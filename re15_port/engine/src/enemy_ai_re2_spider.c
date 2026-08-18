/*
 * RE1.5 Rebuilt — RESIDENT EVIL 2 SPIDER brain (kind 0x25 Adult / 0x26 Baby).
 * PORT OPTION (OPTIONS→AI→RE2), kein RE1.5-Original-Verhalten. WELLE E + WELLE F.
 *
 * WELLE F schliesst die in Welle E offen gelassenen Punkte:
 *   - ACTIVE Modus 1 DECKE @0x801013AC, Modus 2 FADEN @0x80101CC8, Modus 3 WAND @0x80102B40
 *     (+ Flock-Mutex 0x800CFBF4 Bit 0x20, Abseil-Uebergang @0x80101B5C, Hochklettern @0x801026B0)
 *   - alle sieben HURT-Sonderzeilen (@0x80102FDC/@0x8010370C/@0x80103800/@0x801038E4/
 *     @0x801039AC/@0x80103B10/@0x80103B78) und alle fuenf DEATH-Sonderzeilen (@0x801044D0/
 *     @0x8010493C/@0x80104A5C/@0x80104B88/@0x80104C5C) samt Bein-Abtrennung FUN_80105BF0
 *   - der Baby-Spawner FUN_80105D38 (Typ 0x26, Deckel 18, 8 Aufrufstellen) UND das komplette
 *     Baby-Brain aus EMS26.BIN (Root @0x8010001C, Tabelle @0x80101084)
 *   - das Besitz-Gate re15_re2spider_owns ist damit ausnahmslos (kein Flavor-Split mehr)
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
#include "re15_math.h"      /* re15_squareroot0 (== SquareRoot0 0x8008D2F4) */

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
    e->re2s_yaw226 = 0; e->re2s_p228 = 0; e->re2s_next231 = 0; e->re2s_f236 = 0;
    e->re2s_legs220 = 0; e->re2s_fall223 = 0;                        /* 12 Worte ab +0x218 */
    e->re2s_sink23e = 0; e->re2s_sink23f = 0; e->re2s_sink244 = 0;
    e->re2s_water10c = 0; e->re2s_partner240 = -1;   /* +0x240: kein Erzeuger im Port (s. Header) */
    e->re2s_legn221 = 8;      /* +0x221 = 8 Beine @0x80100374 -> `sb s0,545(s2)` @0x801003B4 */
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

/* ---- Modul-lokale Oberflaechen-/Faden-Helfer — im Port OPEN -------------------------------
 * FUN_80105070(self,1)  @0x80105070 : Bone-Root-Motion in +0x144 (Skelett-Weltmatrix)
 * FUN_8010521C(self)    @0x8010521C : Anker-Quadrant -> +0x230
 * FUN_80105300(self,d)  @0x80105300 : Faden-Drehung (Anker-Yaw + Position)
 * FUN_80105360(self,dx,dy,dz) @0x80105360 : rot(+0x74,+0x76,+0x78)+(dx,dy,dz) -> RotMatrix
 *                        0x8008E1F4 + ApplyMatrixLV 0x8008DBA4 auf (+0x144,+0x146,+0x148),
 *                        Ergebnis auf +0x38/+0x3C/+0x40 (@0x801053C8/DC/F0)
 * FUN_8010540C(self,yaw) @0x8010540C : Faden-Stopp-Test
 * FUN_801054D0(self,+0x228,+0x230) @0x801054D0 : Anker verfuegbar?
 * FUN_80105634(self,arc) @0x80105634 : Faden-Ausschlag
 * FUN_801058A4(self,id,yaw) @0x801058A4 : Oberflaechen-Anker setzen
 * FUN_80015350(self,dy,dz)  @0x80015350 : 3-Achsen-Variante von FUN_800152C8 (RE2_Quellcode_V2
 *                        selbst gelesen: RotMatrix(0, +0x76+dy, +0x78+dz) auf +0x144..+0x148,
 *                        Ergebnis auf X/Y/Z)
 * Alle brauchen entweder eine Bone-Weltmatrix im AI-Tick, die gerichtete SCA-Abfrage
 * FUN_8004C1BC oder die PsyQ-RotMatrix-Reihenfolge (RotMatrix vs. RotMatrixYXZ/ZYX — im Repo
 * NICHT belegt, LIBGTE.H fuehrt alle drei). Nichts davon hat der Port -> OPEN mit dem
 * NEUTRALEN Wert (0 / keine Bewegung), KEIN geschaetzter Ersatz. Wichtig: KEINER dieser
 * Aufrufe liegt auf einem Modus-0-Pfad — der ausgelieferte (Boden-)Spinnenlauf ist davon
 * nicht betroffen. */
static void re2s_rope_move(re15_actor_t *e)              { (void)e; }              /* @0x80105360 */
static void re2s_rope_turn(re15_actor_t *e, int d)       { (void)e; (void)d; }     /* @0x80105300 */
static int  re2s_rope_swing(re15_actor_t *e, int arc)    { (void)e; (void)arc; return 0; }
static int  re2s_rope_stop(re15_actor_t *e, int yaw)     { (void)e; (void)yaw; return 0; }
static int  re2s_anchor_ready(re15_actor_t *e)           { (void)e; return 0; }    /* @0x801054D0 */
static void re2s_thrust3(re15_actor_t *e, int dy, int dz){ (void)e; (void)dy; (void)dz; }

/* ==========================================================================================
 * WELLE F — ACTIVE Modus 1 (DECKE) @0x801013AC / Modus 2 (FADEN) @0x80101CC8 /
 *           Modus 3 (WAND) @0x80102B40
 *
 * ERREICHBARKEIT — GEMESSEN, nicht geschaetzt (2026-08-18):
 *  (a) Eigener Zensus ueber ALLE 240 ausgelieferten RE1.5-RDTs (810 `Sce_em_set`-Records,
 *      Opcode 0x44, Stride 20 — Typ = pc[2] (scd_vm.c:3122), Deskriptor = pc[3] -> actor.grid_id
 *      (scd_vm.c:3201)): 58 Records vom Typ 0x25, davon 50x Deskriptor 0x00 und 8x 0x41 —
 *      (Deskriptor & 0xF) liegt zu 100% in {0,1} = BODEN. NULL Decken-Spawns (2/3), NULL
 *      Wand-Spawns (4..11). Raeume: STAGE2 ROOM2030/2050/2060/2070/20A0 (+ ..1-Varianten).
 *      Typ 0x26 (Baby) kommt game-weit in KEINEM Sce_em_set vor.
 *  (b) Eigener Store-Scan ueber EMS25.BIN (alle sb/sh/sw mit Offset 546 = +0x222): geschrieben
 *      wird der Oberflaechen-Modus NUR in INIT (@0x80100460 = 0 / @0x80100480 = 1 /
 *      @0x801004C0 = 3), in Modus 1 (@0x80101B5C = 2), in Modus 2 (@0x801026B0 = 1) und in
 *      HURT/DEATH (@0x80103370 / @0x80103FE0 / @0x8010433C, alle = 0). In Modus 0: NIRGENDS.
 *  => Ein Boden-Spawn kann seinen Modus NIE wechseln. Unter den ausgelieferten Raumdaten sind
 *     die drei Handler hier TOTER PFAD. Sie werden trotzdem byte-true portiert, weil damit das
 *     Besitz-Gate re15_re2spider_owns() ohne Ausnahme greifen kann (Welle E musste Decken-/
 *     Wandspawns dem RE1.5-Brain lassen) — und weil ein Debug-/Test-Spawn sie erreichen kann.
 *
 * FLOCK-MUTEX: 0x800CFBF4 Bit 0x20. Das ist DASSELBE Wort und DASSELBE Bit, das der RE2-Hund
 * benutzt (g_re2_room_gflags, enemy_ai_re2_dog.c) — im Original teilen sich die Module die
 * Adresse, der Port bildet das 1:1 ab. Nur EINE Spinne darf gleichzeitig am Faden haengen.
 */

/* --- FUN_80101D04(self, +0x108) @0x80101D04-DB8 — "an die Oberflaeche anlegen" ---
 *   +0x230 & 1 -> +0x74 = +0x76, +0x78 = 0        (@0x80101D10-30)
 *   sonst      -> +0x74 = 0,     +0x78 = 1024     (@0x80101D34-3C)
 *   Hitbox-Block @0x801063E0 -> +0x84..+0xA0 (@0x80101D40-84) — im Port OPEN (kein Hitbox-Kanal
 *   im AI-Tick; dieselbe dokumentierte Luecke wie der INIT-Block @0x801063A0).
 *   +0x225 = 1 (@0x80101D8C); +0x226 = 0 (@0x80101D9C); +0x14C = 0x00030000 (@0x80101DA0);
 *   FUN_8002959C(a3=1024) (@0x80101DA4). */
static void re2s_surface_attach(re15_actor_t *e)
{
    if (e->re2s_q230 & 1u) { e->rot_x = (int16_t)e->rot_y; e->rot_z = 0; }   /* @0x80101D24-30 */
    else                   { e->rot_x = 0; e->rot_z = 1024; }                /* @0x80101D34-3C */
    e->re2s_gs225  = 1;                                    /* @0x80101D8C */
    e->re2s_yaw226 = 0;                                    /* @0x80101D9C */
    re2s_clip(e, 0x00030000u);                             /* @0x80101DA0 */
    (void)re2s_advance(e, 1024);                           /* @0x80101DA4 */
}

/* --- FUN_80102B10(self) @0x80102B10-3C -> Faden-/Wand-Yaw ---
 *   a = (u16)+0x226; +0x230 == 0 -> a = -a (@0x80102B18-20)
 *   v1 = a - (((s32)(a << 16)) >> 18)          (@0x80102B28-30)
 *   return ((+0x230 << 10) + v1) & 0xfff       (@0x80102B24/34/3C) */
static int re2s_thread_yaw(const re15_actor_t *e)
{
    int32_t a = (int32_t)e->re2s_yaw226;                   /* lhu @0x80102B14 */
    if (e->re2s_q230 == 0) a = -a;                         /* @0x80102B18-20 */
    int32_t t = (int32_t)(int16_t)(uint16_t)(a & 0xffff);  /* sll v1,a0,16 @0x80102B28 */
    t >>= 2;                                               /* sra v1,v1,18 @0x80102B2C */
    t = a - t;                                             /* subu @0x80102B30 */
    return (int)(((((int32_t)e->re2s_q230) << 10) + t) & 0xfff);  /* @0x80102B24/34/3C */
}

/* --- Partner-Distanz (mode1 sub1 @0x80101548-B8, mode2 sub5 @0x801025E0-650) ---
 * +0x240 hat im Modul GENAU EINEN Schreiber, den INIT-Clear `sw zero,576(s2)` @0x8010042C
 * (eigener Store-Scan ueber alle sb/sh/sw mit Offset 576). Der Zeiger wird ausserhalb des
 * Moduls gesetzt; der Port hat dafuer keinen Erzeuger -> re2s_partner240 ist immer -1 und der
 * Rueckgabewert immer 9999 (exakt der Original-Zweig `beq s1,zero` @0x80101550/@0x801025E8).
 * `want_mode` = der +0x222-Wert, den der Partner haben MUSS, damit seine echte Distanz zaehlt:
 *   mode1 sub1 verlangt `+0x222 < 2` (`sltiu v1,v1,0x2` @0x801015A8),
 *   mode2 sub5 verlangt `+0x222 == 1` (`beq v1,v0` mit v0 = 1 @0x80102644). */
static int32_t re2s_partner_dist(const re15_actor_t *e, int mode_lt2)
{
    if (e->re2s_partner240 < 0) return 9999;               /* @0x80101550 / @0x801025E8 */
    const re15_actor_t *p = &g_actors[e->re2s_partner240];
    if (p->state == 7u) return 9999;                       /* CORPSE @0x80101560 / @0x801025F8 */
    int32_t dx = e->x - p->x, dz = e->z - p->z;            /* @0x80101568-90 / @0x80102600-28 */
    int32_t d  = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));  /* 0x8008D2F4 */
    int ok = mode_lt2 ? (p->re2s_mode222 < 2u)             /* @0x801015A8 */
                      : (p->re2s_mode222 == 1u);           /* @0x80102644 */
    return ok ? d : 9999;                                  /* @0x801015B4 / @0x8010264C */
}

/* ---------------------------- Modus 1 (DECKE) @0x801013AC --------------------------------
 * Substate-Tabelle @0x8010649C (12 Eintraege, aus der Datei gelesen):
 *   [0]=0x801013E8 [1..3]=0x80101524 [4]=0x80100A7C(Modus-0-Turn) [5][6]=0x80100B80(Modus-0-
 *   Stalk) [7]=0x801016F4 [8]=0x80100F88(Modus-0-Switch) [9]=0x80101214(Modus-0-Leap)
 *   [10]=0x8010199C [11]=0x80101BC8 */

/* --- Substate 0 @0x801013E8 — Decken-IDLE + Entscheidungsleiter --- */
static void re2s_m1_sub0(re15_actor_t *e, re15_actor_t *pl)
{
    re2s_idle_clip(e);                                     /* @0x801013FC */
    uint32_t d = re2s_tdist(e) & 0xffffu;                  /* lhu +0x1F0 @0x80101420 */
    int a128 = re2s_arc(e, pl, 128);                       /* @0x80101424, Ergebnis -> s2 */
    (void)re2s_arc(e, pl, 256);                            /* @0x80101434 (Ergebnis verworfen) */
    (void)re2s_arc(e, pl, 1024);                           /* @0x80101440 (Ergebnis verworfen) */

    /* 0x800CFBF6 Bit 0 wird game-weit NIE gesetzt (Beleg im Modus-0-Block oben) -> tot. */
    if (0) {                                               /* @0x80101448-58 */
        re2s_word(e, 0x901u);                              /* @0x80101464 */
        if (re2s_rand() & 1u) re2s_word(e, 0x501u);        /* @0x80101468-74 */
    }

    if (e->re2s_t218 != 0) return;                         /* @0x80101478-80 */

    if ((uint16_t)(d - 3001u) < 0x1193u) {                 /* 3001..7499 @0x80101484-90 */
        if ((a128 & 0xffff) != 0) re2s_word(e, 0x101u);    /* @0x80101498-4C4 */
        else {
            uint32_t r = re2s_rand() & 0xfu;               /* @0x801014A0-A8 */
            re2s_word(e, ((832u >> r) & 1u) ? 0x701u : 0x101u);   /* @0x801014AC-C4 */
        }
    }
    if (d < 0xbb8u && (a128 & 0xffff) == 0) {              /* @0x801014C8-D8 */
        uint32_t r = re2s_rand() & 0xfu;                   /* @0x801014E0-E8 */
        re2s_word(e, ((832u >> r) & 1u) ? 0x801u : 0x701u);/* @0x801014EC-504 */
    }
}

/* --- Substates 1/2/3 @0x80101524 — Decken-LAUF + Abseil-Entscheidung --- */
static void re2s_m1_walk(re15_actor_t *e, re15_actor_t *pl)
{
    re2s_m0_walk(e, pl);                                   /* jal 0x801008CC @0x80101540 */

    int32_t pd = re2s_partner_dist(e, 1);                  /* @0x80101548-B8 */

    /* FUN_8004C1BC(&+0x38, 800, 1 << +0x106, 512) != 0 -> Ende (@0x801015D0-D8) und
     * FUN_8004C1BC(&+0x38, 800, 1 << +0x106, 1024) == 0 -> Ende (@0x801015EC-F4).
     * Das ist die SCA-Decken-/Boden-Abfrage der EXE (FUN_8004C1BC, RE2_Quellcode_V2:
     * ratan2-Sektorsuche ueber die Kollisions-Liste). Der Port hat KEINEN Zwilling dieser
     * gerichteten Abfrage -> das Gate ist OPEN und laesst das Abseilen NICHT zu. Kein
     * Ersatzwert, keine Naeherung: die restliche Kette darunter ist trotzdem byte-true
     * portiert, damit sie sofort lebt, sobald der Port die Abfrage bekommt. */
    if (1) return;                                         /* OPEN @0x801015D0-F4 */

    if (pd < 2500) {                                       /* @0x801015F8-604 */
        if (e->re2s_partner240 >= 0) { e->sub_state_1 = 11; e->sub_state_2 = 0; e->sub_state_3 = 0; }
        else                         { e->sub_state_1 = 4;  e->sub_state_2 = 0; e->sub_state_3 = 0; }
        return;                                            /* @0x80101608-CC */
    }
    if (e->re2s_c232 != 0) { e->sub_state_1 = 4; e->sub_state_2 = 0; e->sub_state_3 = 0; return; }
                                                           /* @0x80101614-C8 */
    if (g_re2_room_gflags & 0x20u) {                       /* @0x80101624-38 */
        e->sub_state_1 = 4; e->sub_state_2 = 0; e->sub_state_3 = 0; return;
    }
    /* FUN_8010521C(self) @0x80101640 -> +0x230; == 0 -> Ende; sonst +0x230 = ret-1
     * (@0x80101648-60). FUN_8010540C(self, +-1024) @0x80101668/@0x80101678: beide muessen 0
     * liefern. FUN_801054D0(self, +0x228, +0x230) @0x80101690: muss != 0 liefern.
     * Alle vier sind Oberflaechen-/Ankersucher ohne Port-Kanal -> OPEN (s. Datei-Ende). */
}

/* --- Substate 7 @0x801016F4 — Decken-ANSCHLEICHEN + Absetzen (9 Phasen) ---
 * Phasentabelle @0x8010006C (Gate `sltiu v0,v1,0x9` @0x80101718, aus der Datei gelesen):
 *   [0]=0x80101740 [1]=0x80101758 [2]=0x80101794 [3]=0x801017B8 [4]=0x80101814
 *   [5]=0x80101848 [6]=0x80101918 [7]=0x80101934 [8]=0x80101968 */
static void re2s_m1_attack(re15_actor_t *e, re15_actor_t *pl)
{
    int wrapped, turn;
    switch (e->sub_state_2) {
    case 0:                                                /* @0x80101740 */
        re2s_clip(e, 0x00030004u);                         /* Clip 4 @0x80101740-48 */
        e->sub_state_2 = 1;                                /* @0x80101750 */
        e->re2s_z148   = 0;                                /* +0x148 = 0 @0x80101754 */
        /* fallthrough */
    case 1:                                                /* @0x80101758 */
        wrapped        = re2s_advance(e, 1024);            /* @0x80101764 */
        e->speed_h     = 100;                              /* +0x144 = 100 @0x8010177C */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + wrapped);   /* @0x80101780-88 */
        re2s_thrust(e, e->re2s_yaw21a);                    /* @0x80101784 */
        break;
    case 2:                                                /* @0x80101794 */
        re2s_clip(e, 0x00030005u);                         /* Clip 5 @0x80101794-A0 */
        e->re2z_t158   = (int16_t)((re2s_rand() & 0x1fu) + 45u);  /* @0x801017A4-AC */
        e->sub_state_2 = 3;                                /* @0x801017B0-B4 */
        /* fallthrough */
    case 3:                                                /* @0x801017B8 */
        turn     = re2s_arc(e, pl, 64);                    /* @0x801017BC-C0 */
        e->rot_y = (int16_t)(e->rot_y + turn);             /* @0x801017D4-E4 */
        (void)re2s_advance(e, 1024);                       /* @0x801017E0 */
        if ((turn & 0xffff) == 0) e->sub_state_2 = 4;      /* @0x801017E8-F4 */
        {   uint16_t t = (uint16_t)e->re2z_t158;
            e->re2z_t158 = (int16_t)(t - 1u);              /* @0x80101808 (immer) */
            if (t == 0u) e->sub_state_2 = 6;               /* @0x80101804-14/@0x80101910 */
        }
        break;
    case 4:                                                /* @0x80101814 */
        re2s_clip(e, 0x00030005u);                         /* Clip 5 @0x80101814-1C */
        e->sub_state_2 = 5;                                /* @0x80101820-28 */
        e->re2z_t158   = (int16_t)((re2s_rand() & 0x1fu) + 10u);  /* @0x8010182C-34 */
        e->re2z_dir16a = 1;                                /* +0x16A = 1 @0x80101838-3C */
        e->speed_h     = 300;                              /* +0x144 = 300 @0x80101840-44 */
        /* fallthrough */
    case 5:                                                /* @0x80101848 */
        turn     = re2s_arc(e, pl, 16);                    /* @0x8010184C-50 */
        e->rot_y = (int16_t)(e->rot_y + turn);             /* @0x80101858-68 */
        re2s_thrust(e, e->re2s_yaw21a);                    /* @0x80101864 */
        (void)re2s_advance(e, 1024);                       /* @0x80101878 */
        e->re2z_dir16a--;                                  /* @0x80101880-8C */
        if ((uint8_t)e->re2z_dir16a == 0u) {               /* @0x80101890-94 */
            re2s_se(3);                                    /* @0x80101898-9C */
            /* FUN_801058A4(self, 138, +0x76) @0x801018AC-B0 — Oberflaechen-Anker, OPEN */
            e->re2z_dir16a = 12;                           /* @0x801018B4-B8 */
        }
        {   uint16_t t = (uint16_t)e->re2z_t158;
            e->re2z_t158 = (int16_t)(t - 1u);              /* @0x801018CC (immer) */
            if (t == 0u) { e->sub_state_2 = 6; break; }    /* @0x801018C8/@0x8010190C */
            if (re2s_arc(e, pl, 512) != 0) { e->sub_state_2 = 6; break; }   /* @0x801018D4-E0 */
            if (e->ai_contact & 1u)        { e->sub_state_2 = 6; break; }   /* @0x801018E8-F4 */
            /* +0xD != 255 (Body-Push-Stempel) -> +0x6 = 6 (@0x801018FC-904). Im Port ist +0xD
             * nicht als Pusher-Id vorhanden -> Gate OPEN, feuert nicht (weiterlaufen). */
        }
        break;
    case 6:                                                /* @0x80101918 */
        re2s_clip(e, 0x00030007u);                         /* Clip 7 @0x80101918-20 */
        e->speed_h     = 100;                              /* @0x80101924-28 */
        e->sub_state_2 = 7;                                /* @0x8010192C-30 */
        /* fallthrough */
    case 7:                                                /* @0x80101934 */
        re2s_thrust(e, e->re2s_yaw21a);                    /* @0x80101934-38 */
        wrapped        = re2s_advance(e, 1024);            /* @0x8010194C */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + wrapped);   /* @0x80101954-64 */
        break;
    case 8:                                                /* @0x80101968 */
        re2s_word(e, 1u);                                  /* @0x80101968-6C */
        e->re2s_t218 = 60;                                 /* +0x218 = 60 @0x80101970-74 */
        break;
    default: break;
    }
    re2s_step_se(e, 0);                                    /* @0x80101978 */
}

/* Vorwaerts: Modus-1-Substate 10 uebergibt am Ende direkt an Modus-2-Substate 0. */
static void re2s_m2_sub0(re15_actor_t *e, re15_actor_t *pl);

/* --- Substate 10 @0x8010199C — auf den Anker-Yaw drehen und ABSEILEN --- */
static void re2s_m1_hang(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {                              /* @0x801019B8-F8 (if-Kette 0..3) */
    case 0: {                                              /* @0x80101A00 */
        uint16_t yaw = (uint16_t)((uint16_t)e->rot_y & 0xfffu);   /* @0x80101A00-0C */
        e->sub_state_2 = 1;                                /* @0x80101A04 */
        e->re2z_t158   = (int16_t)((uint16_t)(e->re2s_yaw226 - yaw) & 0xfffu);  /* @0x80101A08-18 */
        e->re2z_t15a   = 24;                               /* +0x15A = 24 @0x80101A20-24 */
        e->rot_y       = (int16_t)yaw;                     /* @0x80101A30 (immer) */
        if ((uint16_t)e->re2z_t158 & 0x800u) {             /* @0x80101A28-2C */
            e->re2z_t158 = (int16_t)((uint16_t)((uint16_t)e->rot_y - e->re2s_yaw226) & 0xfffu);
                                                           /* @0x80101A34-4C */
            e->re2z_t15a = (int16_t)(-e->re2z_t15a);       /* @0x80101A50-54 */
        }
        re2s_clip(e, 0x00030003u);                         /* Clip 3 @0x80101A58-60 */
        /* fallthrough */
    }
    case 1:                                                /* @0x80101A64 */
        if (e->re2z_t158 > 0) {                            /* blez @0x80101A6C */
            e->rot_y     = (int16_t)(e->rot_y + e->re2z_t15a);   /* @0x80101A74-88 */
            e->re2z_t158 = (int16_t)(e->re2z_t158 - 24);   /* @0x80101A84-90 */
        } else {
            e->sub_state_2 = 2;                            /* @0x80101A98 */
            e->rot_y       = (int16_t)e->re2s_yaw226;      /* @0x80101A94-9C */
        }
        (void)re2s_advance(e, 1024);                       /* @0x80101AAC */
        break;
    case 2:                                                /* @0x80101ABC */
        e->sub_state_2 = 3;                                /* @0x80101ABC-C0 */
        e->speed_h     = 25;                               /* +0x144 = 25 @0x80101AC4-C8 */
        e->re2s_z148   = 0;                                /* +0x148 = 0 @0x80101ACC */
        /* Hitbox-Block @0x801063C0 -> +0x84..+0xA0 (@0x80101AD0-B14) — OPEN */
        re2s_clip(e, 0x0003000eu);                         /* Clip 14 @0x80101B18-20 */
        /* fallthrough */
    case 3:                                                /* @0x80101B24 */
        re2s_thrust(e, e->re2s_yaw21a);                    /* @0x80101B28 */
        if (!re2s_advance(e, 1024)) break;                 /* @0x80101B3C-44 */
        re2s_word(e, 1u);                                  /* @0x80101B50-54 */
        e->re2s_mode222 = 2;                               /* +0x222 = 2 @0x80101B58-5C */
        e->speed_h      = 450;                             /* +0x144 = 450 @0x80101B60-68 */
        re2s_thrust(e, e->re2s_yaw21a);                    /* @0x80101B64 */
        e->re2s_f236    = 0;                               /* +0x236 = 0 @0x80101B80 */
        e->y            = e->y + 1250;                     /* @0x80101B74-88 */
        e->re2s_y22c    = e->y;                            /* +0x22C = Y @0x80101B8C */
        g_re2_room_gflags &= (uint16_t)~0x20u;             /* Mutex FREI @0x80101B98-A0 */
        re2s_m2_sub0(e, pl);                               /* jal 0x80101DBC @0x80101B9C */
        break;
    default: break;
    }
    re2s_step_se(e, 0);                                    /* @0x80101BA4 */
}

/* --- Substate 11 @0x80101BC8 — zum PARTNER laufen ---
 * Nur erreichbar, wenn +0x240 != 0 (@0x80101604) — im Port also NIE (s. re2s_partner_dist). */
static void re2s_m1_regroup(re15_actor_t *e)
{
    const re15_actor_t *p = (e->re2s_partner240 >= 0) ? &g_actors[e->re2s_partner240] : 0;
    if (e->sub_state_2 == 0) {                             /* @0x80101BE4-EC */
        re2s_clip(e, 0x00030002u);                         /* Clip 2 @0x80101BF4-00 */
        e->sub_state_2 = 1;                                /* @0x80101C08 */
        e->re2z_t158   = 60;                               /* +0x158 = 60 @0x80101C04-0C */
        e->re2z_t15a   = p ? (int16_t)re15_ai_arc_test(e, p->x, p->z, 64) : 0;  /* @0x80101C10-20 */
    }
    e->rot_y = (int16_t)(e->rot_y + e->re2z_t15a);         /* @0x80101C30-44 */
    (void)re2s_advance(e, 1024);                           /* @0x80101C40 */
    re2s_step_se(e, 0);                                    /* @0x80101C48 */
    /* FUN_80105070(self,1) @0x80101C54 = Bone-Root-Motion in +0x144 — OPEN (s. Dateikopf). */
    e->speed_h   = 0;
    e->speed_h   = (int16_t)(e->speed_h + 100);            /* += *0x80106480 (=100) @0x80101C5C-80 */
    e->re2z_t158 = (int16_t)(e->re2z_t158 - 1);            /* @0x80101C68-74 */
    if ((uint16_t)e->re2z_t158 == 0u) re2s_word(e, 1u);    /* @0x80101C78-88 */
    if (e->ai_contact & 1u) return;                        /* +0x110 & 1 @0x80101C8C-98 */
    re2s_thrust(e, e->re2s_yaw21a);                        /* @0x80101CA0-A4 */
}

static void re2s_mode1(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_1) {                              /* @0x801013B4, Tabelle @0x8010649C */
    case 0:  re2s_m1_sub0(e, pl);   break;
    case 1: case 2: case 3: re2s_m1_walk(e, pl); break;
    case 4:  re2s_m0_turn(e);       break;                 /* geteilt @0x80100A7C */
    case 5: case 6: re2s_m0_stalk(e, pl); break;           /* geteilt @0x80100B80 */
    case 7:  re2s_m1_attack(e, pl); break;
    case 8:  re2s_m0_switch(e);     break;                 /* geteilt @0x80100F88 */
    case 9:  re2s_m0_leap(e, pl);   break;                 /* geteilt @0x80101214 */
    case 10: re2s_m1_hang(e, pl);   break;
    case 11: re2s_m1_regroup(e);    break;
    default: break;
    }
}

/* ---------------------------- Modus 2 (FADEN) @0x80101CC8 ---------------------------------
 * Substate-Tabelle @0x801064CC (8 Eintraege): [0]=0x80101DBC [1]=0x80101F10 [2]=0x80101FF4
 * [3]=0x801020CC [4]=0x80102268 [5]=0x801024F8 [6]=0x801027B4 [7]=0x80102958 */

/* --- Substate 0 @0x80101DBC — am Faden haengen, Abstiegsstrecke berechnen --- */
static void re2s_m2_sub0(re15_actor_t *e, re15_actor_t *pl)
{
    re2s_surface_attach(e);                                /* FUN_80101D04 @0x80101DCC */

    int32_t dy = pl->y - e->y;                             /* +0x3C-Differenz @0x80101DD4-08 */
    int32_t n;
    if (dy >= 3501) {                                      /* slti 3501 @0x80101E0C-10 */
        /* FUN_800527B4(X,Z) @0x80101E20 — der sce==7-AOT-Scan (Wasser-Oberflaechen-Y). Der Port
         * hat dafuer keinen Zwilling; der Original-Rueckgabewert OHNE Treffer ist 0 (siehe
         * re2s_water10c) -> hier 0. OPEN, aber kein erfundener Wert. */
        int32_t floor_y = 0;                               /* FUN_800527B4 @0x80101E20 (OPEN) */
        if (floor_y >= e->dog_floor_y) {                   /* +0x1C2 @0x80101E28-34 */
            n = (dy - 3500) / 100;                         /* Magic-Div @0x80101E3C-58 */
        } else {
            n = (floor_y - (e->y + 2200)) / 100;           /* @0x80101E64-84 */
            if ((int16_t)n < 0) n = 0;                     /* @0x80101E8C-98 */
        }
        e->re2z_t158 = (int16_t)n;
        if ((int16_t)e->re2z_t158 != 0) {                  /* @0x80101E9C-A4 */
            uint32_t r = re2s_rand();                      /* @0x80101EAC */
            e->sub_state_1  = 1;                           /* @0x80101EB8 */
            e->re2s_next231 = 2;                           /* +0x231 = 2 @0x80101EC0 */
            e->sub_state_2  = 0; e->sub_state_3 = 0;       /* @0x80101ECC */
            e->re2z_t158    = (int16_t)((uint16_t)e->re2z_t158 + (r & 3u));  /* @0x80101EC4-D8 */
            return;
        }
    }
    e->sub_state_1 = 2;                                    /* @0x80101EDC */
    e->sub_state_2 = 1; e->sub_state_3 = 0;                /* `sh 1,6(s0)` @0x80101EE8 */
    e->re2z_t158   = (int16_t)((re2s_rand() & 0xfu) + 30u);/* @0x80101EE4-F4 */
}

/* --- Substate 1 @0x80101F10 (auch Modus 3 [1]) — am Faden ABSEILEN --- */
static void re2s_m2_descend(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {                             /* @0x80101F2C-34 */
        e->sub_state_2 = 1;                                /* @0x80101F48 */
        e->speed_h     = 100;                              /* +0x144 = 100 @0x80101F4C-50 */
        e->re2d_vy146  = 0;                                /* +0x146 = 0 @0x80101F54 */
        e->re2s_z148   = 0;                                /* +0x148 = 0 @0x80101F58 */
        re2s_clip(e, 0x00030002u);                         /* Clip 2 @0x80101F3C-5C */
    }
    if ((int16_t)e->re2z_t158 != 0) {                      /* @0x80101F60-68 */
        re2s_rope_move(e);                                 /* FUN_80105360(self,0,0,0) @0x80101F78 */
        e->re2z_t158 = (int16_t)((uint16_t)e->re2z_t158 - 1u);   /* @0x80101F80-8C */
    }
    (void)re2s_advance(e, 1024);                           /* @0x80101F9C */
    re2s_step_se(e, 0);                                    /* @0x80101FA4 */
    if ((int16_t)e->re2z_t158 != 0) return;                /* @0x80101FAC-B4 */
    e->sub_state_2 = 0; e->sub_state_3 = 0;                /* @0x80101FC0 */
    e->sub_state_1 = e->re2s_next231;                      /* +0x5 = +0x231 @0x80101FBC-C8 */
    e->re2z_t158   = (int16_t)((re2s_rand() & 0xfu) + 15u);/* @0x80101FCC-D4 */
}

/* --- Substate 2 @0x80101FF4 — am Faden WARTEN --- */
static void re2s_m2_wait(re15_actor_t *e)
{
    if (e->sub_state_2 == 0) {                             /* @0x80102004-0C */
        e->sub_state_2 = 1;                                /* @0x80102014-18 */
        re2s_clip(e, 0x00030000u);                         /* Clip 0 @0x8010201C-20 */
    }
    (void)re2s_advance(e, 1024);                           /* @0x80102024 */
    e->re2z_t158 = (int16_t)((uint16_t)e->re2z_t158 - 1u); /* @0x8010202C-38 */
    if ((uint16_t)e->re2z_t158 != 0u) return;              /* @0x8010203C-40 */

    uint32_t d   = re2s_tdist(e);                          /* @0x80102048-80 */
    uint8_t  nxt = 3;                                      /* @0x8010206C/@0x80102088 */
    if (d < 0xbb8u) {                                      /* 3000 @0x80102064/@0x80102080 */
        uint32_t r = re2s_rand() & 0xfu;                   /* @0x8010208C-94 */
        nxt = ((832u >> r) & 1u) ? 7u : 3u;                /* @0x80102098-AC */
    }
    e->sub_state_1 = nxt;                                  /* @0x801020B0 */
    e->sub_state_2 = 0; e->sub_state_3 = 0;                /* @0x801020B4 */
}

/* --- Substate 3 @0x801020CC (auch Modus 3 [3]) — am Faden PENDELN/DREHEN --- */
static void re2s_m2_swing(re15_actor_t *e)
{
    switch (e->sub_state_2) {                              /* @0x801020E8-128 (if-Kette 0..3) */
    case 0: {                                              /* @0x80102130 */
        e->sub_state_2 = 1;                                /* @0x8010213C */
        int16_t sw = (int16_t)re2s_rope_swing(e, 32);      /* FUN_80105634(self,32) @0x80102138 */
        e->re2z_t15a = sw;                                 /* +0x15A @0x80102140 */
        if (sw == 0) { e->sub_state_1 = 7; e->sub_state_2 = 0; e->sub_state_3 = 0; return; }
                                                           /* @0x8010214C-58/@0x801021C8 */
        int16_t step = (sw > 0) ? (int16_t)1024 : (int16_t)-1024;  /* @0x8010215C-64 */
        e->re2z_t158 = step;                               /* @0x80102174 */
        if (e->re2s_q230 == 0) e->re2z_t158 = (int16_t)(-step);    /* @0x80102168-7C */
        /* fallthrough */
    }
    case 1: {                                              /* @0x80102180 */
        uint32_t r = re2s_rand();                          /* @0x80102184 */
        e->sub_state_2 = 2;                                /* @0x80102188 */
        e->re2z_dir16a = (uint8_t)((r & 0xfu) + 15u);      /* +0x16A @0x80102194-9C */
        re2s_clip(e, 0x00030002u);                         /* Clip 2 @0x8010218C-A0 */
        /* fallthrough */
    }
    case 2: {                                              /* @0x801021A4 */
        re2s_rope_turn(e, e->re2z_t15a);                   /* FUN_80105300 @0x801021A8 */
        if (((uint16_t)e->re2s_yaw226 & 0xfffu) == ((uint16_t)e->re2z_t158 & 0xfffu)) {
            e->sub_state_1 = 6; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* @0x801021B0-D0 */
            break;
        }
        e->re2z_dir16a--;                                  /* @0x801021D4-E0 */
        if ((uint8_t)e->re2z_dir16a != 0u) break;          /* @0x801021E4-E8 */
        e->sub_state_2 = 3;                                /* @0x801021EC-F4 */
        e->re2z_dir16a = (uint8_t)((re2s_rand() & 0xfu) + 1u);   /* @0x801021F8-200 */
        re2s_clip(e, 0x00030000u);                         /* Clip 0 @0x80102204-0C */
        break;
    }
    case 3:                                                /* @0x80102210 */
        e->re2z_dir16a--;                                  /* @0x80102210-1C */
        if ((uint8_t)e->re2z_dir16a == 0u) e->sub_state_2 = 1;   /* @0x80102220-2C */
        break;
    default: break;
    }
    (void)re2s_advance(e, 1024);                           /* @0x8010223C */
    re2s_step_se(e, 0);                                    /* @0x80102244 */
}

/* --- Substate 4 @0x80102268 (auch Modus 3 [4]) — Faden-SCHWUNG (7 Phasen) ---
 * Phasentabelle @0x80100094 (Gate `sltiu v0,v1,0x7` @0x8010228C): [0]=0x801022B4
 * [1]=0x801022F8 [2]=0x80102350 [3]=0x80102400 [4]=0x80102424 [5]=0x8010243C [6]=0x80102488
 * Ausschlagstabelle @0x801064EC (8 s16, selbst gelesen): {64,-64,64,-64,32,-64,64,-32} */
static const int16_t s_re2s_swing[8] = { 64, -64, 64, -64, 32, -64, 64, -32 };   /* @0x801064EC */

static void re2s_m2_pump(re15_actor_t *e)
{
    switch (e->sub_state_2) {
    case 0:                                                /* @0x801022B4 */
        e->sub_state_2 = 1;                                /* @0x801022C0 (immer) */
        e->re2z_t15a = e->re2s_next231
                     ? (int16_t)64                         /* @0x801022E8-EC */
                     : s_re2s_swing[re2s_rand() & 7u];     /* @0x801022C4-E4 */
        e->re2s_f236 = 1;                                  /* +0x236 = 1 @0x801022F0-F4 */
        /* fallthrough */
    case 1:                                                /* @0x801022F8 */
        e->sub_state_2 = 2;                                /* @0x80102304 (immer) */
        e->re2z_dir16a = e->re2s_next231
                       ? (uint8_t)0xffu                    /* @0x8010231C */
                       : (uint8_t)((re2s_rand() & 0xfu) + 15u);   /* @0x80102308-18 */
        e->re2z_dir16a = e->re2z_dir16a;                   /* sb @0x80102320 */
        /* fallthrough */
    case 2: {                                              /* @0x80102350 (Tabellen-Eintrag) */
        int a = e->re2z_t15a; if (a < 0) a = -a;           /* @0x80102324-34 */
        re2s_clip(e, (a == 32) ? 0x00030002u : 0x00030001u);  /* @0x80102338-4C */
        re2s_rope_turn(e, e->re2z_t15a);                   /* FUN_80105300 @0x80102354 */
        int b = e->re2z_t15a; if (b < 0) b = -b;           /* @0x8010235C-6C */
        uint32_t lhs = (uint32_t)(b << 1);                 /* @0x80102384 */
        uint32_t rhs = (uint32_t)((((uint32_t)e->re2s_yaw226 & 0xfffu)
                                   + (uint32_t)(b - 2048)) & 0xfffu);   /* @0x80102370-80 */
        if ((int32_t)lhs >= (int32_t)rhs) {                /* slt @0x80102388-8C */
            if (g_re2_room_gflags & 0x20u) {               /* @0x80102394-A4 */
                e->sub_state_2 = 4;                        /* @0x801023AC */
                re2s_clip(e, 0x00030000u);                 /* Clip 0 @0x801023B0-B8 */
            } else {
                e->sub_state_2 = 5;                        /* @0x801023BC-C0 */
            }
        }
        e->re2z_dir16a--;                                  /* @0x801023C4-D0 */
        if ((uint8_t)e->re2z_dir16a != 0u) break;          /* @0x801023D4-D8 */
        e->sub_state_2 = 3;                                /* @0x801023DC-E4 */
        e->re2z_dir16a = (uint8_t)((re2s_rand() & 0xfu) + 15u);  /* @0x801023E8-F0 */
        re2s_clip(e, 0x00030000u);                         /* Clip 0 @0x801023F4-FC */
        break;
    }
    case 3:                                                /* @0x80102400 */
        e->re2z_dir16a--;                                  /* @0x80102400-0C */
        if ((uint8_t)e->re2z_dir16a == 0u) e->sub_state_2 = 1;   /* @0x80102410-20 */
        break;
    case 4:                                                /* @0x80102424 */
        if (g_re2_room_gflags & 0x20u) break;              /* Mutex belegt -> warten @0x80102434 */
        /* fallthrough auf den P5-Eintrag @0x8010243C */
    case 5:                                                /* @0x8010243C */
        e->sub_state_2   = 6;                              /* @0x80102440-44 */
        e->re2s_yaw226   = 2048;                           /* +0x226 = 2048 @0x80102448-4C */
        e->re2s_f236     = 1;                              /* +0x236 = 1 @0x80102450-5C */
        g_re2_room_gflags |= 0x20u;                        /* Mutex NEHMEN @0x80102460-6C */
        e->speed_h       = 100;                            /* +0x144 = 100 @0x80102470-74 */
        e->re2d_vy146    = 0;                              /* @0x80102478 */
        e->re2s_z148     = 0;                              /* @0x8010247C */
        re2s_clip(e, 0x00030002u);                         /* Clip 2 @0x8010243C/@0x80102484 */
        break;
    case 6:                                                /* @0x80102488 */
        re2s_rope_move(e);                                 /* FUN_80105360(self,0,0,0) @0x80102494 */
        if (e->re2s_y22c < e->y) break;                    /* slt @0x8010249C-AC */
        e->y           = e->re2s_y22c;                     /* @0x801024B4 */
        e->sub_state_1 = 5;                                /* @0x801024B8 */
        e->sub_state_2 = 0; e->sub_state_3 = 0;            /* @0x801024BC */
        break;
    default: break;
    }
    (void)re2s_advance(e, 1024);                           /* @0x801024CC */
    re2s_step_se(e, 0);                                    /* @0x801024D4 */
}

/* --- Substate 5 @0x801024F8 (auch Modus 3 [5]) — am Faden HOCHKLETTERN --- */
static void re2s_m2_climb(re15_actor_t *e)
{
    switch (e->sub_state_2) {                              /* @0x8010251C-54 */
    case 0:                                                /* @0x8010255C */
        e->sub_state_2 = 1;                                /* @0x8010256C (immer) */
        if (e->re2s_q230 & 1u) { e->speed_h = 0; e->re2d_vy146 = 25; }   /* @0x80102570-7C */
        else                   { e->speed_h = 25; e->re2d_vy146 = 0; }   /* @0x80102580-88 */
        e->re2s_z148 = 0;                                  /* @0x8010258C */
        /* Hitbox-Block @0x801063C0 -> +0x84..+0xA0 (@0x80102590-D4) — OPEN */
        re2s_clip(e, 0x00030000u);                         /* Clip 0 @0x801025D8-DC */
        /* fallthrough */
    case 1:                                                /* @0x801025E0 */
        if (re2s_partner_dist(e, 0) >= 2501) {             /* @0x801025E0-54 */
            e->sub_state_2 = 2;                            /* @0x80102664-68 */
            re2s_clip(e, 0x0003000eu);                     /* Clip 14 @0x8010265C-6C */
        }
        (void)re2s_advance(e, 1024);                       /* @0x80102678 */
        break;
    case 2:                                                /* @0x80102688 */
        re2s_thrust3(e, 0, (int16_t)e->re2s_yaw226);       /* FUN_80015350 @0x8010268C */
        if (!re2s_advance(e, 1024)) break;                 /* @0x801026A0-A8 */
        e->re2s_mode222 = 1;                               /* +0x222 = 1 (DECKE) @0x801026B0 */
        e->re2s_yaw21a  = 2048;                            /* +0x21A = 2048 @0x801026C4 (immer) */
        e->rot_y = (int16_t)(((e->re2s_q230 & 1u) ? (uint16_t)e->rot_x : (uint16_t)e->rot_y)
                             + 2048u);                     /* @0x801026B4-E0 */
        e->rot_z        = 2048;                            /* +0x78 = 2048 @0x801026EC-F0 */
        e->re2s_c232    = 150;                             /* +0x232 = 150 @0x801026F4-F8 */
        e->rot_x        = 0;                               /* +0x74 = 0 @0x80102704 */
        e->speed_h      = 1225;                            /* +0x144 = 1225 @0x80102700-08 */
        e->y            = e->y - 475;                      /* @0x801026FC-14 */
        re2s_thrust(e, e->re2s_yaw21a);                    /* @0x80102710 */
        /* Hitbox-Block @0x801063A0 -> +0x84..+0xA0 (@0x80102718-5C) — OPEN */
        re2s_word(e, 1u);                                  /* @0x80102770 */
        e->re2s_f236    = 0;                               /* +0x236 = 0 @0x80102774 */
        g_re2_room_gflags &= (uint16_t)~0x20u;             /* Mutex FREI @0x80102778-84 */
        e->re2s_gs225   = 0;                               /* +0x225 = 0 @0x80102788 */
        break;
    default: break;
    }
    re2s_step_se(e, 0);                                    /* @0x8010278C */
}

/* --- Substate 6 @0x801027B4 (auch Modus 3 [6]) — der FADEN-BISS --- */
static void re2s_m2_bite(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_2) {                              /* @0x801027D0-14 */
    case 0:                                                /* @0x80102818 */
        e->sub_state_2 = 1;                                /* @0x8010281C (immer) */
        e->re2z_t158   = (int16_t)((re2s_rand() & 0xfu) + 30u);  /* @0x80102828-30 */
        e->speed_h     = 300;                              /* +0x144 = 300 @0x80102834-38 */
        e->re2d_vy146  = 0;                                /* @0x8010283C */
        e->re2s_z148   = 0;                                /* @0x80102840 */
        re2s_clip(e, 0x00030004u);                         /* Clip 4 @0x80102820-44 */
        /* fallthrough */
    case 1:                                                /* @0x80102848 */
        if (!re2s_advance(e, 1024)) break;                 /* @0x80102854-5C */
        e->sub_state_2 = 2;                                /* @0x8010286C/@0x80102904 */
        re2s_clip(e, 0x00030005u);                         /* Clip 5 @0x80102860-64/@0x8010290C */
        break;
    case 2: {                                              /* @0x80102870 */
        re2s_rope_move(e);                                 /* FUN_80105360(self,0,0,0) @0x8010287C */
        (void)re2s_advance(e, 1024);                       /* @0x80102890 */
        int stop = re2s_rope_stop(e, (int16_t)e->re2s_yaw226);   /* FUN_8010540C @0x8010289C */
        if (!stop) {                                       /* @0x801028A4 */
            /* +0xD != 255 -> Abbruch (@0x801028AC-B4): im Port kein Pusher-Stempel, OPEN. */
            if (!(e->ai_contact & 1u)) {                   /* +0x110 & 1 @0x801028BC-C8 */
                if (re2s_attack(e, pl, 2000, 4300)) {      /* FUN_80105AE0 @0x801028E8-EC */
                    e->sub_state_2 = 3;                    /* @0x80102900-04 */
                    re2s_clip(e, 0x00030007u);             /* Clip 7 @0x801028F8-FC/@0x8010290C */
                }
                break;
            }
        }
        e->sub_state_2 = 3;                                /* @0x801028D8-DC */
        re2s_clip(e, 0x00030007u);                         /* Clip 7 @0x801028D0-E0 */
        break;
    }
    case 3:                                                /* @0x80102910 */
        if (!re2s_advance(e, 1024)) break;                 /* @0x80102918-20 */
        e->sub_state_1 = 4;                                /* @0x80102928 */
        e->sub_state_2 = 0; e->sub_state_3 = 0;            /* @0x8010292C */
        e->re2s_next231 = 0;                               /* +0x231 = 0 @0x80102930 */
        break;
    default: break;
    }
    re2s_step_se(e, 0);                                    /* @0x80102934 */
}

/* --- Substate 7 @0x80102958 (auch Modus 3 [7]) — Faden-Anker suchen + Oberflaechenwechsel --- */
static void re2s_m2_anchor(re15_actor_t *e)
{
    switch (e->sub_state_2) {                              /* @0x80102978-B8 */
    case 0:                                                /* @0x801029C0 */
        e->sub_state_2 = 1;                                /* @0x801029C8 */
        e->re2z_t158   = 16;                               /* +0x158 = 16 @0x801029CC-D0 */
        re2s_clip(e, 0x00030001u);                         /* Clip 1 @0x80102998/@0x801029D4 */
        /* fallthrough */
    case 1: {                                              /* @0x801029D8 */
        int16_t sw = (int16_t)re2s_rope_swing(e, 64);      /* FUN_80105634(self,64) @0x801029DC */
        re2s_rope_turn(e, sw);                             /* FUN_80105300 @0x801029F0 */
        int done = (sw == 0);                              /* @0x801029F8 */
        if (!done) {
            e->re2z_t158 = (int16_t)((uint16_t)e->re2z_t158 - 1u);  /* @0x80102A00-0C */
            if ((uint16_t)e->re2z_t158 != 0u) {            /* @0x80102A10-14 */
                (void)re2s_advance(e, 1024);               /* @0x80102A9C */
                break;
            }
        }
        e->sub_state_2 = 2;                                /* @0x80102A2C */
        re2s_clip(e, 0x0003000bu);                         /* Clip 11 @0x80102A1C-34 */
        /* Ziel-Id 136/137 nach Distanz < 3000 (@0x80102A38-70), dann
         * FUN_801058A4(self, id, FUN_80102B10(self)) @0x80102A74-8C — Anker, im Port OPEN.
         * FUN_80102B10 wird trotzdem gerufen, weil es rein rechnend ist. */
        (void)re2s_thread_yaw(e);                          /* @0x80102A74 */
        (void)re2s_advance(e, 1024);                       /* @0x80102A9C */
        break;
    }
    case 2:                                                /* @0x80102AAC */
        if (!re2s_advance(e, 1024)) break;                 /* @0x80102AB8/@0x80102AD4 */
        e->sub_state_1  = 4;                               /* @0x80102ADC */
        e->sub_state_2  = 0; e->sub_state_3 = 0;           /* @0x80102AE0 */
        e->re2s_next231 = 0;                               /* @0x80102AE4 */
        break;
    case 3:                                                /* @0x80102AC8 */
        if (!re2s_surface_switch(e)) break;                /* FUN_8010109C @0x80102ACC-D4 */
        e->sub_state_1  = 4;                               /* @0x80102ADC */
        e->sub_state_2  = 0; e->sub_state_3 = 0;           /* @0x80102AE0 */
        e->re2s_next231 = 0;                               /* @0x80102AE4 */
        break;
    default: break;
    }
    /* Tail `jal 0x80016028` @0x80102AE8 — EXE-Sammelroutine ohne Port-Kanal (OPEN). */
}

static void re2s_mode2(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_1) {                              /* @0x80101CD0, Tabelle @0x801064CC */
    case 0: re2s_m2_sub0(e, pl);    break;
    case 1: re2s_m2_descend(e);     break;
    case 2: re2s_m2_wait(e);        break;
    case 3: re2s_m2_swing(e);       break;
    case 4: re2s_m2_pump(e);        break;
    case 5: re2s_m2_climb(e);       break;
    case 6: re2s_m2_bite(e, pl);    break;
    case 7: re2s_m2_anchor(e);      break;
    default: break;
    }
}

/* ---------------------------- Modus 3 (WAND) @0x80102B40 ----------------------------------
 * Substate-Tabelle @0x801064FC (8 Eintraege): [0]=0x80102B7C [1]=0x80101F10(Modus-2-Abseilen)
 * [2]=0x80102BB0 [3]=0x801020CC [4]=0x80102268 [5]=0x801024F8 [6]=0x801027B4 [7]=0x80102958
 * — sechs von acht Substates teilt die Wand mit dem Faden-Modus. */

/* --- Substate 0 @0x80102B7C — an die Wand anlegen --- */
static void re2s_m3_sub0(re15_actor_t *e)
{
    re2s_surface_attach(e);                                /* FUN_80101D04 @0x80102B88 */
    e->sub_state_1 = 2;                                    /* @0x80102B90-94 */
    e->sub_state_2 = 0; e->sub_state_3 = 0;                /* @0x80102B98 */
}

/* --- Substate 2 @0x80102BB0 — an der Wand LAUERN --- */
static void re2s_m3_lurk(re15_actor_t *e)
{
    (void)re2s_advance(e, 1024);                           /* @0x80102BC0 */
    uint32_t d = re2s_tdist(e);                            /* @0x80102BC8-EC */
    if (d >= 0x1c20u) {                                    /* 7200 @0x80102BF0-F4 */
        if (!re2s_anchor_ready(e)) return;                 /* FUN_801054D0 @0x80102C04-0C */
    }
    uint32_t d2 = re2s_tdist(e);                           /* @0x80102C14-4C */
    e->sub_state_1 = (d2 < 0xbb8u) ? 7u : 3u;              /* 3000 @0x80102C30-5C */
    e->sub_state_2 = 0; e->sub_state_3 = 0;                /* @0x80102C60 */
}

static void re2s_mode3(re15_actor_t *e, re15_actor_t *pl)
{
    switch (e->sub_state_1) {                              /* @0x80102B48, Tabelle @0x801064FC */
    case 0: re2s_m3_sub0(e);        break;
    case 1: re2s_m2_descend(e);     break;
    case 2: re2s_m3_lurk(e);        break;
    case 3: re2s_m2_swing(e);       break;
    case 4: re2s_m2_pump(e);        break;
    case 5: re2s_m2_climb(e);       break;
    case 6: re2s_m2_bite(e, pl);    break;
    case 7: re2s_m2_anchor(e);      break;
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

    switch (e->re2s_mode222) {                             /* @0x801005D4-F0, Tabelle @0x80106440 */
    case 0: re2s_mode0(e, pl); break;                      /* BODEN @0x80100688 */
    case 1: re2s_mode1(e, pl); break;                      /* DECKE @0x801013AC */
    case 2: re2s_mode2(e, pl); break;                      /* FADEN @0x80101CC8 */
    case 3: re2s_mode3(e, pl); break;                      /* WAND  @0x80102B40 */
    default: break;   /* Die Tabelle @0x80106440 hat GENAU 4 Eintraege; +0x222 kommt nur aus
                       * den sechs belegten Schreibstellen (s. WELLE-F-Kopf). */
    }

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

static void re2s_active(re15_actor_t *e, re15_actor_t *pl);   /* @0x801005AC (Re-Entry Zeile 16) */

/* ---- ZEILEN-UEBERSETZUNG (PORT-ZUORDNUNG, WELLE F) ----------------------------------------
 * Das Original speist +0x5 aus der ITEM-ID der gefuehrten Waffe + 1 (`addiu v1,v1,1` /
 * `sb v1,5(t0)` @0x80041AA0) — also 1..19, und der Zeilen-Dispatch @0x80102CD8 / @0x80103CE8
 * hat DESHALB keine Bereichspruefung (Zeile 0 zeigt in beiden Tabellen auf Datenwoerter bzw.
 * einen fremden Handler).
 * Der Port speist +0x5 aus dem RE1.5-Hitscan `+0x5 = weapon_id` (re15_damage.c
 * re15_player_weapon_fire, @0x800124BC) — RE1.5-Waffen-Ids 0..21, ein ANDERER Id-Raum.
 * GEMESSEN (2026-08-18): re15_enemy_take_damage (der zweite +0x5-Erzeuger, Nahkampf/attack_type)
 * hat in engine/ UND platform/ NULL Aufrufer — er existiert nur in Tests. Im ausgelieferten
 * Port ist +0x5 auf einem Treffer also EINDEUTIG eine RE1.5-Waffen-Id.
 * Die Karte RE1.5-Waffe -> RE2-Item-Id ist WAFFEN-IDENTITAET und damit konsumenten-unabhaengig;
 * sie ist Zeile fuer Zeile in enemy_ai_re2_zombie.c hergeleitet (Item-Definitionstabelle
 * @0x800A9E1C, PLW-Gegenprobe, Schadensrecords) — hier stehen dieselben Werte, NICHT neu
 * geraten. Was die Spinne NICHT teilt, ist der Zombie-Guard re2z_row_guard: der prueft die
 * ZOMBIE-2D-Tabelle @0x8010C940 und wuerde Zeilen auf 8 umlenken, die bei der Spinne sehr wohl
 * einen eigenen Handler haben (z.B. 17 -> @0x80102FDC). Deshalb eine eigene Klemme.
 * ERREICHBARKEIT der Sonderzeilen im Port (aus dieser Karte):
 *   w5,w6 -> 4 | w12 -> 15 | w19 -> 18   => @0x8010370C
 *   w7 -> 5 | w8 -> 7 | w13 -> 8 | w18 -> 17 => @0x80102FDC
 *   w9,w15 -> 9  => @0x80103800    w11,w17 -> 10 => @0x801038E4
 *   w10,w16 -> 11 => @0x801039AC   w14 -> 16 => @0x80103B78
 *   Zeile 14 (@0x80103B10 / @0x80104C5C) ist NICHT erreichbar: RE2-Id 14 = Spark Shot, dafuer
 *   hat RE1.5 keine Waffe (im Zombie-Block als ungenutzt belegt). Sie ist trotzdem portiert. */
static const uint8_t s_re2s_row_from_weapon[22] = {   /* Werte == re2z_row_from_weapon */
    /* 0*/  1, /* 1*/  1, /* 2*/  1, /* 3*/  3, /* 4*/  2, /* 5*/  4,
    /* 6*/  4, /* 7*/  5, /* 8*/  7, /* 9*/  9, /*10*/ 11, /*11*/ 10,
    /*12*/ 15, /*13*/  8, /*14*/ 16, /*15*/  9, /*16*/ 11, /*17*/ 10,
    /*18*/ 17, /*19*/ 18, /*20*/ 13, /*21*/  1
};

/* Uebersetzt +0x5 EINMAL pro frischem Treffer (+0x6 == 0 — genau der Tick, an dem das Original
 * die Zeile in +0x23C latcht). Ausserhalb 0..21 klemmt der Port auf 1 (die schwaechste gueltige
 * Zeile) — im Original unmoeglich, hier eine deklarierte PORT-SICHERUNG. */
static uint8_t re2s_row_translate(uint8_t port_row)
{
    return (port_row < 22u) ? s_re2s_row_from_weapon[port_row] : 1u;
}

/* ---- FUN_801056DC(self, part, fx) @0x801056DC-5898 — Partikel an einem MESH-TEIL ----------
 * fx-Record = @0x80106684 + 6*(fx & 0x7F) (11 Records, selbst gedumpt); Position = die
 * WELT-MATRIX des Teils (self+0x198 + 172*part, Translation +0x5C/+0x60/+0x64), Winkel
 * FUN_800154AC(X,Z,X,Z) — degeneriert, liefert IMMER 0x400 (@0x80105730). Der Port hat weder
 * das 20 x 172-Byte-Mesh-Teil-Array (+0x198) noch den ESP-Deskriptor-Raum von 0x8001BF10 ->
 * RE1.5-ESP-Blut am Aktor-Ursprung, dieselbe deklarierte Naeherung wie re2z_blood_fx und wie
 * der bereits ausgelieferte Welle-E-HURT-Zweig @0x80102DD0. */
static void re2s_gore(re15_actor_t *e)
{
    re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500,
                         e->x, e->y, e->z, (int16_t)e->rot_y);
}

/* ---- FUN_80105BF0 @0x80105BF0-D34 — EIN zufaelliges BEIN abtrennen ------------------------
 *   +0x23C == 14 -> sofort 1 (@0x80105C00-04: die Spark-Shot-Zeile trennt nie ab)
 *   k = rand & 7 (@0x80105C10-28)
 *   +0x221 < 3 (@0x80105C20) ODER (+0x220 >> k) & 1 (@0x80105C34) -> nur Gore:
 *        FUN_801056DC(self, 19, 1) @0x80105D18, Rueckgabe 1
 *   sonst: +0x220 |= 1<<k (@0x80105C64); +0x221-- (@0x80105C74);
 *          die zwei Mesh-Teile 2k+3 / 2k+4 im +0x198-Array (172-Byte-Stride) bekommen
 *          Flug-/Farbwerte (@0x80105CAC-CF4) — im Port OPEN (kein Mesh-Part-Array);
 *          FUN_801056DC(self, 2k+3, 1) @0x80105D00, Rueckgabe 0. */
static int re2s_sever_leg(re15_actor_t *e)
{
    if (e->re2s_row23c == 14u) return 1;                   /* @0x80105C00-04 */
    uint32_t k = re2s_rand() & 7u;                         /* @0x80105C10-28 */
    if (e->re2s_legn221 < 3u || ((e->re2s_legs220 >> k) & 1u)) {   /* @0x80105C20/@0x80105C34 */
        re2s_gore(e);                                      /* FUN_801056DC(self,19,1) @0x80105D18 */
        return 1;
    }
    e->re2s_legs220 = (uint8_t)(e->re2s_legs220 | (1u << k));      /* @0x80105C64 */
    e->re2s_legn221--;                                             /* @0x80105C74 */
    re2s_gore(e);                                          /* FUN_801056DC(self,2k+3,1) @0x80105D00 */
    return 0;
}

/* ---- FUN_80105D38 @0x80105D38-F04 — BABY-SPINNEN AUSSTOSSEN (Typ 0x26) --------------------
 *   *0x800CFB74 & 0x01000000 -> (rand & 3) != 0 bricht den GANZEN Spawn ab (@0x80105D58-7C).
 *     0x800CFB74 ist das RE2-Szenario-/Schwierigkeitswort; der Port bildet es (wie beim
 *     HP-Zug im INIT) mit BEIDEN Bits 0 ab -> der Zweig ist unerreichbar. DEKLARIERT.
 *   +0x23C == 14 -> gar kein Spawn (@0x80105D84-88).
 *   live = FUN_8001A5C0(self, 0, 38, 0) = Anzahl lebender Typ-0x26-Entities ohne self
 *   (live + count) >= 18 -> count = 17 - live (@0x80105DB0-C0); count == 0 -> return
 *   je Kind: FUN_8001AD3C(38) allokiert; +0x10E = (self+0x10C != 0) ? (mode|1) : mode
 *   (@0x80105E0C); X/Y/Z = self (@0x80105E18-30); +0x76 = self+0x222 < 2 ? rand<<4
 *   (@0x80105E58) : (self+0x230 << 10) - 512 + (rand<<2) (@0x80105E78); +0x1E8 = 1
 *   (@0x80105E80); Hitbox @0x80106400 -> +0x84..+0xA0; dann der Kind-INIT (@0x80105EE8). */
#define RE2SPIDER_BABY_CAP 18      /* `sltiu v0,v0,0x12` @0x80105DB0 */

static int re2s_spawn_babies(re15_actor_t *e, uint16_t mode, unsigned count)
{
    if (e->re2s_row23c == 14u) return 0;                   /* @0x80105D84-88 */

    unsigned live = 0;                                     /* FUN_8001A5C0 @0x80105D9C */
    for (int i = 0; i < RE15_ACTOR_MAX; i++)
        if (&g_actors[i] != e && g_actors[i].active && g_actors[i].type == 0x26u) live++;

    if (live + count >= (unsigned)RE2SPIDER_BABY_CAP) {    /* @0x80105DB0-B4 */
        if (live >= 17u) return 0;                         /* @0x80105DC0-CC */
        count = 17u - live;                                /* @0x80105DBC-C0 */
    }
    if (count == 0u) return 0;                             /* @0x80105DD8 */

    int made = 0;
    for (unsigned n = 0; n < count; n++) {                 /* @0x80105DDC/@0x80105EF8 */
        int slot = re15_actor_alloc(0x26);                 /* FUN_8001AD3C(38) @0x80105DE4 */
        if (slot < 0) break;                               /* @0x80105DF0 */
        re15_actor_t *c = &g_actors[slot];
        c->re2z_f10e = (uint16_t)(e->re2s_water10c ? (mode | 1u) : mode);   /* @0x80105E0C */
        c->grid_id   = (uint8_t)(c->re2z_f10e & 0xffu);    /* Port-Spiegel des Spawn-Bytes */
        c->x = e->x; c->y = e->y; c->z = e->z;             /* @0x80105E18-30 */
        c->rot_y = (e->re2s_mode222 < 2u)                                   /* @0x80105E3C */
                 ? (int16_t)(re2s_rand() << 4)                              /* @0x80105E50-58 */
                 : (int16_t)(((uint32_t)e->re2s_q230 << 10) - 512u
                             + (re2s_rand() << 2));                         /* @0x80105E68-78 */
        /* +0x1E8 = 1 (@0x80105E80) ist NICHT "Gravitation": FUN_80035408 liest es als die
         * ANZAHL der Hitbox-Kugeln (selbst nachgerechnet am Baby-Modul, das es ebenfalls auf 1
         * setzt @0x80100164). Der Port hat kein Kugel-Array -> OPEN, kein Ersatzfeld. */
        c->state = 0; c->sub_state_1 = 0; c->sub_state_2 = 0; c->sub_state_3 = 0;
        made++;
    }
    return made;
}

static void re2s_hurt_row_generic(re15_actor_t *e);

/* ---- HURT-Sonderzeile @0x80102FDC (Zeilen 5,6,7,8,17) — NIEDERSCHLAG -----------------------
 * Dispatch auf +0x222 (@0x80102FE4, Tabelle @0x80106580 = {0x80103018, 0x801030D4,
 * 0x8010340C, 0x8010340C}). MODUS 0 @0x80103018 (der einzige unter RE1.5-Raumdaten
 * erreichbare) ist eine 3-Phasen-Kette auf +0x6:
 *   P0 @0x80103074: +0x6 = 1 (@0x80103080); +0x14C = 0x00030001 (@0x80103088);
 *                   FUN_80105BF0(self) (@0x80103084)   -> faellt in P1
 *   P1 @0x8010308C: +0x6 += FUN_8002959C(a3=1024) (@0x8010309C-B0)
 *   P2 @0x801030B4: Zustandswort 0x901 (@0x80103068/@0x801030B4) = ACTIVE Sub 9
 * MODUS 1 @0x801030D4 / MODUS 2+3 @0x8010340C: fallen, Beine abwerfen, Baby-Spinnen
 * ausstossen (@0x8010322C / @0x801033D8 / @0x801034DC, alle `spawn(self, 0x2004, (rand&1)+1)`),
 * dann +0x222 = 0 (@0x80103370). Diese beiden Zweige sind hier portiert, aber unter den
 * ausgelieferten Raumdaten unerreichbar (s. WELLE-F-Kopf). */
static void re2s_hurt_row_knock(re15_actor_t *e)
{
    if (e->re2s_mode222 != 0u) {                           /* Tabelle @0x80106580 */
        /* MODUS 1 @0x801030D4 / MODUS 2+3 @0x8010340C — gemeinsamer Kopf: */
        if (e->re2s_f236 != 0u) {                          /* @0x801030F0 / @0x80103428 */
            (void)re2s_sever_leg(e);                       /* @0x80103100 / @0x80103438 */
            e->state       = 1;                            /* @0x8010313C / @0x80103474 */
            e->sub_state_1 = e->re2s_snap233[0];
            e->sub_state_2 = e->re2s_snap233[1];
            e->sub_state_3 = e->re2s_snap233[2];
            return;                                        /* Re-Entry in ACTIVE @0x80103138 */
        }
        if (e->re2z_f10e & 1u) {                           /* +0x10E & 1 @0x80103148 / @0x80103480 */
            if (e->re2s_mode222 == 1u) {
                if (e->sub_state_2 == 0u)
                    (void)re2s_spawn_babies(e, 0x2004u, (re2s_rand() & 1u) + 1u);  /* @0x801033D8 */
                /* Rest faellt auf den Boden-Automaten @0x801033E8 */
            } else {
                (void)re2s_sever_leg(e);                   /* @0x801034C0 */
                (void)re2s_spawn_babies(e, 0x2004u, (re2s_rand() & 1u) + 1u);      /* @0x801034DC */
                e->re2s_next231 = 1;                       /* @0x801034E8 */
                re2s_word(e, 0x401u);                      /* @0x801034F0 */
                return;
            }
        } else {
            /* Fall-Automat @0x80106590 / @0x8010659C: P0 loest von der Decke/Wand (inkl.
             * `spawn(self, 0x2004, (rand&1)+1)` @0x8010322C), P1 faellt mit Schwerkraft 40,
             * P2 @0x8010333C steht wieder auf und setzt +0x222 = 0 (@0x80103370).
             * Der Port fuehrt hier NUR den Zustandsfluss + den Spawn: die Fallbewegung
             * braucht FUN_80015350/FUN_80105360 (RotMatrix-Reihenfolge OPEN, s.o.). */
            switch (e->sub_state_2) {
            case 0:
                e->re2d_vy146  = 40;                       /* @0x801031CC-D4 */
                e->re2z_t158   = 2048;                     /* @0x801031D8-DC */
                e->sub_state_2 = 1;                        /* @0x801031E8 */
                e->re2s_fall223 = 1;                       /* @0x801031EC */
                e->speed_h     = 0;                        /* @0x801031F4 */
                e->re2s_z148   = 0;                        /* @0x801031F8 */
                re2s_clip(e, 0x00030001u);                 /* @0x801031FC */
                e->re2s_yaw21a = 0;                        /* @0x80103200 */
                e->re2z_self1d3 |= 0x80u;                  /* @0x80103214 */
                (void)re2s_sever_leg(e);                   /* @0x80103210 */
                (void)re2s_spawn_babies(e, 0x2004u, (re2s_rand() & 1u) + 1u);      /* @0x8010322C */
                break;
            case 1:
                e->re2d_vy146 = (int16_t)(e->re2d_vy146 + 40);   /* @0x8010327C-84 */
                if (e->re2z_t158 != 0) {                   /* @0x80103280 */
                    e->rot_z     = (int16_t)(e->rot_z + 512);    /* @0x80103290-98 */
                    e->re2z_t158 = (int16_t)(e->re2z_t158 - 512);/* @0x80103294-9C */
                }
                if (e->y >= (int32_t)e->dog_floor_y) {     /* @0x801032AC-B0 */
                    e->y = (int32_t)e->dog_floor_y;        /* @0x801032C0 */
                    if (e->re2z_t158 == 0) {               /* @0x801032D4 */
                        e->sub_state_2 = 2;                /* @0x801032E4-E8 */
                        re2s_clip(e, 0x00030009u);         /* Clip 9 @0x801032DC-EC */
                    }
                    re2s_se(e->re2s_c23a >= 0 ? 5 : 6);    /* @0x801032F0-304 */
                }
                (void)re2s_advance(e, 1024);               /* @0x80103318 */
                break;
            default:                                       /* P2 @0x8010333C */
                if (!re2s_advance(e, 1024)) break;         /* @0x8010334C */
                re2s_word(e, 1u);                          /* @0x8010335C */
                e->re2s_fall223  = 0;                      /* @0x8010336C */
                e->re2s_mode222  = 0;                      /* +0x222 = 0 @0x80103370 */
                e->re2z_self1d3 &= 0x7fu;                  /* @0x80103380 */
                break;
            }
            return;
        }
    }

    /* MODUS 0 @0x80103018 */
    switch (e->sub_state_2) {
    case 0:                                                /* @0x80103074 */
        e->sub_state_2 = 1;                                /* @0x80103080 */
        re2s_clip(e, 0x00030001u);                         /* Clip 1 @0x80103088 */
        (void)re2s_sever_leg(e);                           /* @0x80103084 */
        /* fallthrough */
    case 1:                                                /* @0x8010308C */
        e->sub_state_2 = (uint8_t)(e->sub_state_2 + re2s_advance(e, 1024));  /* @0x8010309C-B0 */
        break;
    case 2:                                                /* @0x801030B4 */
        re2s_word(e, 0x901u);                              /* @0x80103068/@0x801030B4 */
        break;
    default: break;
    }
}

/* ---- HURT-Sonderzeile @0x8010370C (Zeilen 4,15,18) — GORE-Salve, dann generische Zeile ----
 *   nur bei +0x6 == 0 (@0x80103728):
 *     FUN_801056DC(self, tbl[rand&3], 3)  tbl @0x801065A8 = {0,1,2,19} (@0x80103738-58)
 *     k = rand & 7 (@0x8010375C-68); (+0x220 >> k) & 1 == 0 ->
 *          FUN_801056DC(self, 2k+3, 3) (@0x8010376C-88)
 *     eines von +0x151/+0x152/+0x153 negativ (@0x8010378C/9C/AC) -> FUN_80105BF0(self)
 *          (@0x801037BC) und alle drei = 6 (@0x801037C4-D0)
 *   IMMER: generische Zeile @0x801037DC */
static void re2s_hurt_row_gore(re15_actor_t *e)
{
    if (e->sub_state_2 == 0u) {                            /* @0x80103728 */
        (void)re2s_rand();                                 /* Bone-Wahl @0x80103738 */
        re2s_gore(e);                                      /* FUN_801056DC(...,3) @0x80103754 */
        uint32_t k = re2s_rand() & 7u;                     /* @0x8010375C-68 */
        if (((e->re2s_legs220 >> k) & 1u) == 0u)           /* @0x8010376C-70 */
            re2s_gore(e);                                  /* FUN_801056DC(self,2k+3,3) @0x80103784 */
        if (e->re2z_pool151 < 0 || e->re2z_pool152 < 0 || e->re2z_pool153 < 0) {
            (void)re2s_sever_leg(e);                       /* @0x801037BC */
            e->re2z_pool151 = 6; e->re2z_pool152 = 6; e->re2z_pool153 = 6;  /* @0x801037C4-D0 */
        }
    }
    re2s_hurt_row_generic(e);                              /* @0x801037DC */
}

/* ---- HURT-Sonderzeile @0x80103800 (Zeile 9) — Blutspray, Anzahl aus +0x1D2 ---------------
 *   +0x5 = 7 (@0x8010380C, Delay-Slot -> LAEUFT VOR dem Aufruf); dann @0x80102FDC
 *   (@0x80103818). Danach hc3 = +0x1D2 / 3 (@0x80103820-38); wenn hc3 != 2:
 *   Schleife i < (2 - hc3) (@0x801038BC-C0): bone = (rand & 0xF) + 3; wenn
 *   (mesh[bone].word0 & 0x4B) == 1 -> FUN_801056DC(self, bone, 5) (@0x80103890). */
static void re2s_hurt_row_spray9(re15_actor_t *e)
{
    e->sub_state_1 = 7;                                    /* @0x8010380C */
    re2s_hurt_row_knock(e);                                /* @0x80103818 */
    uint32_t hc3 = (uint32_t)e->re2z_hits1d2 / 3u;         /* @0x80103820-38 */
    if (hc3 == 2u) return;                                 /* @0x80103830-40 */
    unsigned n = (hc3 < 2u) ? (2u - hc3) : 0u;             /* PORT-SICHERUNG: unsigned-Unterlauf
                                                            * (`sltu` @0x801038C0) geklemmt */
    for (unsigned i = 0; i < n; i++) {
        (void)re2s_rand();                                 /* bone = (rand&0xF)+3 @0x80103848-54 */
        /* mesh-Gate (+0x198-Array, 172-Byte-Stride) hat im Port keinen Zwilling -> OPEN */
        re2s_gore(e);                                      /* FUN_801056DC(self,bone,5) @0x80103890 */
    }
}

/* ---- HURT-Sonderzeile @0x801038E4 (Zeile 10) — 5 Blut-Emitter, dann generische Zeile ------
 *   Schleife i = 0..4 (@0x80103968): bone = (rand & 0xF) + 3 (@0x80103914-20); Mesh-Gate
 *   (@0x80103950-54) -> FUN_801056DC(self, bone, 5) (@0x8010395C). +0x5 = 2 (@0x80103970),
 *   dann generische Zeile (@0x80103980). */
static void re2s_hurt_row_spray10(re15_actor_t *e)
{
    for (int i = 0; i < 5; i++) { (void)re2s_rand(); re2s_gore(e); }   /* @0x80103914-68 */
    e->sub_state_1 = 2;                                    /* @0x80103970 */
    re2s_hurt_row_generic(e);                              /* @0x80103980 */
}

/* ---- HURT-Sonderzeile @0x801039AC (Zeile 11) — BEIN ABSCHIESSEN, dann generische Zeile ----
 *   k = rand & 7 (@0x801039CC-D4); s1 = 2k+3 (@0x801039EC);
 *   +0x221 >= 3 (@0x801039E4) UND (+0x220 >> k) & 1 == 0 (@0x801039F8-FC):
 *        +0x220 |= 1<<k (@0x80103A18); +0x221-- (@0x80103A3C); Flug-/Farbwerte auf dem
 *        Nachbar-Mesh-Teil (@0x80103A54-94) — im Port OPEN.
 *   IMMER: FUN_801056DC(self, s1, 7) (@0x80103AA0); Mesh-Farbe (@0x80103AE4, OPEN);
 *          +0x5 = 2 (@0x80103AE0); generische Zeile (@0x80103AE8). */
static void re2s_hurt_row_leg11(re15_actor_t *e)
{
    uint32_t k = re2s_rand() & 7u;                         /* @0x801039CC-D4 */
    if (e->re2s_legn221 >= 3u && ((e->re2s_legs220 >> k) & 1u) == 0u) {   /* @0x801039E4-FC */
        e->re2s_legs220 = (uint8_t)(e->re2s_legs220 | (1u << k));         /* @0x80103A18 */
        e->re2s_legn221--;                                                /* @0x80103A3C */
    }
    re2s_gore(e);                                          /* FUN_801056DC(self,2k+3,7) @0x80103AA0 */
    e->sub_state_1 = 2;                                    /* @0x80103AE0 */
    re2s_hurt_row_generic(e);                              /* @0x80103AE8 */
}

/* ---- HURT-Sonderzeile @0x80103B10 (Zeile 14) — Funken-Emitter -----------------------------
 *   +0x23B = 20 (@0x80103B34, Delay-Slot -> vor dem Aufruf); FUN_801056DC(self, 0, 10)
 *   (@0x80103B3C); +0x5 = 7 (@0x80103B50, Delay-Slot); dann @0x80102FDC (@0x80103B54).
 *   Wichtig: +0x23C bleibt 14 -> re2s_sever_leg und re2s_spawn_babies steigen sofort aus
 *   (@0x80105C04 / @0x80105D88). Im Port NICHT erreichbar (RE1.5 hat keine Spark-Shot). */
static void re2s_hurt_row_spark14(re15_actor_t *e)
{
    e->re2s_fx23b  = 20;                                   /* @0x80103B34 */
    re2s_gore(e);                                          /* @0x80103B3C */
    e->sub_state_1 = 7;                                    /* @0x80103B50 */
    re2s_hurt_row_knock(e);                                /* @0x80103B54 */
}

/* ---- HURT-Sonderzeile @0x80103B78 (Zeile 16) — einmalige Gore-Salve, dann ACTIVE ----------
 *   +0x23D == 0 (@0x80103BA4-AC): Schleife i = 0..4 (@0x80103C10) wie Zeile 10, danach
 *   +0x23D = 30 (@0x80103C18-1C).
 *   IMMER: Zustandswort = 1 | (+0x233<<8) | (+0x234<<16) | (+0x235<<24) (@0x80103C2C-54) und
 *   SOFORTIGER Re-Entry in ACTIVE im selben Frame (@0x80103C50). */
static void re2s_hurt_row_burst16(re15_actor_t *e, re15_actor_t *pl);

static void re2s_hurt(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0) {
        e->sub_state_1  = re2s_row_translate(e->sub_state_1);   /* PORT-ZUORDNUNG, s.o. */
        e->re2s_row23c  = e->sub_state_1;                  /* +0x23C @0x80102C94-AC */
    }
    /* +0x10C = FUN_800527B4(X,Z) (@0x80102CB0-D0) — im Port OPEN (bleibt 0 = kein Wasser). */

    switch (e->sub_state_1) {                              /* Tabelle @0x80106518 */
    case 4: case 15: case 18: re2s_hurt_row_gore(e);    return;
    case 5: case 6: case 7: case 8: case 17: re2s_hurt_row_knock(e); return;
    case 9:  re2s_hurt_row_spray9(e);   return;
    case 10: re2s_hurt_row_spray10(e);  return;
    case 11: re2s_hurt_row_leg11(e);    return;
    case 14: re2s_hurt_row_spark14(e);  return;
    case 16: re2s_hurt_row_burst16(e, pl); return;
    default: break;                                        /* 0..3, 12, 13, 19 = generisch */
    }
    re2s_hurt_row_generic(e);
}

/* Die generische Zeile @0x80102D18 -> @0x80102D54 (Modus 0). */
static void re2s_hurt_row_generic(re15_actor_t *e)
{
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

/* HURT-Sonderzeile @0x80103B78 (Zeile 16) — Rumpf (Vorwaertsdeklaration oben). */
static void re2s_hurt_row_burst16(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->re2s_c23d == 0u) {                              /* +0x23D @0x80103BA4-AC */
        for (int i = 0; i < 5; i++) { (void)re2s_rand(); re2s_gore(e); }  /* @0x80103BBC-C10 */
        e->re2s_c23d = 30;                                 /* @0x80103C18-1C */
    }
    e->state       = 1;                                    /* @0x80103C2C-54 */
    e->sub_state_1 = e->re2s_snap233[0];
    e->sub_state_2 = e->re2s_snap233[1];
    e->sub_state_3 = e->re2s_snap233[2];
    re2s_active(e, pl);                                    /* Re-Entry im SELBEN Frame @0x80103C50 */
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
/* --- Generische DEATH-Zeile @0x80103D30 -> Modus 0 @0x80103D8C --- */
static void re2s_death_row_generic(re15_actor_t *e);

/* --- Der ABSCHLUSS-AUSBRUCH der generischen Zeile @0x80104440 (+0x224 != 0) ---
 *   FUN_8002959C(a3=512); +0x14C = 0x0007000D (@0x80104460);
 *   FUN_80105D38(self, 0x2002, (rand & 3) + 1) (@0x80104470-78)  <-- BABY-SPINNEN
 *   FUN_801056DC(self, 0, 0) (@0x80104488); Zustandswort = 7; word0 |= 0x42 (@0x801044A8);
 *   +0x23A < 0 -> Y = +0x10C + 70.
 * ERREICHBAR: die generische Zeile setzt beim ersten Tod HP = 1 und +0x224 = 1 (@0x80103EB0/B8);
 * ein WEITERER Treffer auf die Leiche schickt sie erneut in DEATH und damit hierher. */
static void re2s_death_done_generic(re15_actor_t *e)
{
    (void)re2s_advance(e, 512);                            /* @0x80104450 */
    re2s_clip(e, 0x0007000du);                             /* Clip 13, Rate 7 @0x80104460 */
    (void)re2s_spawn_babies(e, 0x2002u, (re2s_rand() & 3u) + 1u);   /* @0x80104470-78 */
    re2s_gore(e);                                          /* FUN_801056DC(self,0,0) @0x80104488 */
    re2s_word(e, 7u);                                      /* CORPSE @0x8010449C */
    if (e->re2s_c23a < 0)                                  /* @0x801044B0 */
        e->y = (int32_t)e->re2s_water10c + 70;             /* @0x801044C0 */
}

/* --- Der ABSCHLUSS-AUSBRUCH der Sonderzeile @0x801047F8 (+0x224 != 0) ---
 *   wie oben, aber FUN_80105D38(self, 0x2002, (rand & 3) + 6) (@0x80104828-30),
 *   FUN_801056DC(self, 19, 1) (@0x80104840) und zusaetzlich FUN_80104890(self) (@0x80104848). */
static void re2s_death_done_special(re15_actor_t *e)
{
    (void)re2s_advance(e, 512);                            /* @0x80104814 */
    re2s_clip(e, 0x0007000du);                             /* @0x80104818 */
    (void)re2s_spawn_babies(e, 0x2002u, (re2s_rand() & 3u) + 6u);   /* @0x80104828-30 */
    re2s_gore(e);                                          /* @0x80104840 */
    re2s_gore(e);                                          /* FUN_80104890 @0x80104848 (Sprays) */
    re2s_word(e, 7u);                                      /* @0x80104854 */
    if (e->re2s_c23a < 0)                                  /* @0x80104864 */
        e->y = (int32_t)e->re2s_water10c + 70;             /* @0x80104878 */
}

/* --- DEATH-Sonderzeile @0x801044D0 (Zeilen 5,6,7,8,17) — ZERPLATZEN ---
 * +0x224 != 0 -> @0x801047F8 (@0x801044D8-514). Sonst Dispatch auf +0x222 (Tabelle
 * @0x80106660 = {0x8010452C, 0x80104664, 0x801046EC, 0x801046EC}).
 * MODUS 0 @0x8010452C (der einzige unter RE1.5-Raumdaten erreichbare):
 *   +0x6 == 0: +0x6 = 1 (@0x80104570); +0x23A = 1 (@0x80104574);
 *              +0x14C = 0x0007000C (@0x80104578); word0 |= 2 (@0x80104584);
 *              FUN_801056DC(self,19,1) (@0x80104580); FUN_80104890(self) (@0x80104588);
 *              FUN_80105D38(self, 0x2002, (rand&3)+6) (@0x80104590-A4)   <-- 6..9 BABYS
 *   IMMER: FUN_8002959C(a3=512) (@0x801045B8); Frame-Flag *(+0x178) & 0x40000 (@0x801045D4);
 *          +0x10C == 0 -> Zustandswort 7 (@0x801045E4/@0x80104644)
 *          sonst Wasser-Zweig (@0x801045EC-3C).
 * Diese Zeile setzt WEDER HP = 1 NOCH +0x224 = 1 — die Leiche bleibt "unfertig". */
static void re2s_death_row_special(re15_actor_t *e)
{
    if (e->re2s_done224 != 0u) { re2s_death_done_special(e); return; }   /* @0x801044D8-514 */

    if (e->re2s_mode222 != 0u) {
        /* MODUS 1 @0x80104664 / MODUS 2+3 @0x801046EC — im Port nur der Spawn + der
         * Zustandsfluss; die Sturzbewegung braucht FUN_80015350/FUN_80105360 (OPEN, s.o.). */
        if (e->sub_state_2 == 0u) {
            re2s_gore(e);                                  /* @0x80104694 / @0x801047BC */
            re2s_gore(e);                                  /* FUN_80104890 @0x8010469C/@0x801047AC */
            (void)re2s_spawn_babies(e, 0x2004u, (re2s_rand() & 3u) + 6u);   /* @0x801046B8 /
                                                                            * @0x801047D8 */
            e->sub_state_2 = 1;
            e->re2s_fall223 = 1;                           /* @0x80104764 (nur Modus 2/3) */
        }
        if (re2s_advance(e, 512)) {                        /* @0x80103F88 / @0x801042E0 */
            e->re2s_mode222 = 0;                           /* @0x80103FE0 / @0x8010433C */
            re2s_word(e, 7u);
        }
        return;
    }

    /* MODUS 0 @0x8010452C */
    if (e->sub_state_2 == 0u) {                            /* @0x80104548 */
        e->sub_state_2 = 1;                                /* @0x80104570 */
        e->re2s_c23a   = 1;                                /* +0x23A = 1 @0x80104574 */
        re2s_clip(e, 0x0007000cu);                         /* Clip 12, Rate 7 @0x80104578 */
        re2s_gore(e);                                      /* FUN_801056DC(self,19,1) @0x80104580 */
        re2s_gore(e);                                      /* FUN_80104890(self) @0x80104588 */
        (void)re2s_spawn_babies(e, 0x2002u, (re2s_rand() & 3u) + 6u);   /* @0x80104590-A4 */
    }
    if (!re2s_advance(e, 512)) return;                     /* Frame-Flag 0x40000 @0x801045B8-D4 */
    /* +0x10C == 0 (Port: immer, s. re2s_water10c) -> direkt CORPSE @0x801045E4/@0x80104644 */
    re2s_word(e, 7u);                                      /* @0x80104644 */
}

/* --- DEATH-Sonderzeile @0x8010493C (Zeile 9) — 3 Gore-Salven, dann die Sonderzeile --- */
static void re2s_death_row9(re15_actor_t *e)
{
    if (e->sub_state_2 == 0u && e->re2s_done224 == 0u) {   /* @0x8010496C/@0x8010497C */
        re2s_gore(e);                                      /* Record 4/5 @0x801049A0 */
        for (int k = 0; k < 3; k++) {                      /* @0x80104A18 */
            (void)re2s_rand();                             /* bone = (rand&0xF)+3 @0x801049AC-B8 */
            re2s_gore(e); re2s_gore(e);                    /* @0x801049FC / @0x80104A0C */
        }
    }
    re2s_death_row_special(e);                             /* @0x80104A2C */
}

/* --- DEATH-Sonderzeile @0x80104A5C (Zeilen 10 und 16) — 4 Gore-Salven + Umfaerben --- */
static void re2s_death_row1016(re15_actor_t *e)
{
    if (e->sub_state_2 == 0u && e->re2s_done224 == 0u) {   /* @0x80104A8C/@0x80104A9C */
        re2s_gore(e);                                      /* @0x80104AC0 */
        for (int k = 0; k < 4; k++) {                      /* @0x80104B38 */
            (void)re2s_rand();                             /* @0x80104ACC-D8 */
            re2s_gore(e); re2s_gore(e);                    /* @0x80104B1C / @0x80104B2C */
        }
        /* FUN_8010609C(self, 0x00202F2F) @0x80104B44-4C — Mesh-Teil-Faerbung, im Port OPEN */
    }
    re2s_death_row_generic(e);                             /* @0x80104B58 */
}

/* --- DEATH-Sonderzeile @0x80104B88 (Zeile 11) — verkohlt, setzt +0x239 --- */
static void re2s_death_row11(re15_actor_t *e)
{
    if (e->sub_state_2 == 0u && e->re2s_done224 == 0u) {   /* @0x80104BAC/@0x80104BBC */
        /* FUN_8010609C(self, 0x00101F3F) @0x80104BF0 + Record-19-Block @0x80104BD0-F4 — OPEN */
        re2s_gore(e);                                      /* @0x80104C00 */
        re2s_gore(e);                                      /* @0x80104C10 */
        re2s_gore(e);                                      /* @0x80104C20 */
        e->re2s_dead239 = 1;                               /* +0x239 = 1 @0x80104C2C */
    }
    re2s_death_row_generic(e);                             /* @0x80104C38 */
}

/* --- DEATH-Sonderzeile @0x80104C5C (Zeile 14) — Blitz-Weiss + 30 Funken-Ticks ---
 * Im Port NICHT erreichbar (RE2-Id 14 = Spark Shot, RE1.5 hat keine). */
static void re2s_death_row14(re15_actor_t *e)
{
    if (e->re2s_done224 == 0u) {                           /* @0x80104C80 */
        /* FUN_8010609C(self, 0x003F3F3F) @0x80104C8C — OPEN */
        e->re2s_fx23b = 30;                                /* +0x23B = 30 @0x80104C98 */
        re2s_gore(e);                                      /* @0x80104CA4 */
        re2s_gore(e);                                      /* @0x80104CB4 */
    }
    e->sub_state_1 = 2;                                    /* @0x80104CC0 */
    re2s_death_row_generic(e);                             /* @0x80104CCC */
}

static void re2s_death(re15_actor_t *e, re15_actor_t *pl)
{
    (void)pl;
    if (e->sub_state_2 == 0) {
        e->sub_state_1 = re2s_row_translate(e->sub_state_1);     /* PORT-ZUORDNUNG, s. HURT */
        e->re2s_row23c = e->sub_state_1;                   /* +0x23C @0x80103C9C-B4 */
    }
    /* +0x10C = FUN_800527B4(X,Z) (@0x80103CB8-D8) — im Port OPEN (bleibt 0 = kein Wasser). */

    switch (e->sub_state_1) {                              /* Tabelle @0x801065A8 */
    case 5: case 6: case 7: case 8: case 17: re2s_death_row_special(e); break;
    case 9:  re2s_death_row9(e);    break;
    case 10: case 16: re2s_death_row1016(e); break;
    case 11: re2s_death_row11(e);   break;
    case 14: re2s_death_row14(e);   break;
    default: re2s_death_row_generic(e); break;             /* 1..4, 12, 13, 15, 18, 19 */
    }
    /* Tail FUN_80104F18(self) @0x80103D04 (Wasser-Sink-Treiber) — im Port OPEN, weil
     * +0x23E nur im Wasser-Zweig gesetzt wird und +0x10C hier immer 0 ist. */
}

static void re2s_death_row_generic(re15_actor_t *e)
{
    if (e->re2s_done224 != 0u) { re2s_death_done_generic(e); return; }   /* @0x80103D38 */
    if (e->re2s_mode222 != 0u) {
        /* Modus 1 @0x80103EDC / Modus 2+3 @0x801041CC — Sturz + Landung; die Bewegung braucht
         * FUN_80015350/FUN_80105360 (OPEN). Portiert ist der Zustandsfluss incl. +0x222 = 0
         * (@0x80103FE0 / @0x8010433C) und der +0x6==2-Abschluss (@0x80104154). */
        if (e->sub_state_2 == 0u) {
            e->re2d_vy146 = 40;                            /* @0x80103F2C */
            re2s_clip(e, 0x0000090cu);                     /* @0x80103F38 */
            e->speed_h = 0; e->re2s_z148 = 0; e->rot_z = 0;/* @0x80103F40-48 */
            e->y += 800;                                   /* @0x80103F50 */
            if (e->re2s_row23c != 1) re2s_gore(e);         /* @0x80103F58-60 */
            e->sub_state_2 = 1;                            /* @0x80103F24 */
            return;
        }
        if (e->sub_state_2 == 1u) {
            if (!re2s_advance(e, 512)) return;             /* @0x80103F88 */
            e->sub_state_2   = 2;                          /* @0x80103FD8 */
            e->re2s_mode222  = 0;                          /* @0x80103FE0 / @0x8010433C */
            re2s_se(e->re2s_c23a < 0 ? 6 : 5);             /* @0x8010400C */
            return;
        }
        if (!re2s_advance(e, 1024)) return;                /* @0x80104164 */
        re2s_word(e, 7u);                                  /* @0x80104190 */
        if (e->re2s_dead239 != 0u) return;                 /* @0x801041A0 */
        e->hp = 1;                                         /* @0x801041AC */
        e->re2s_done224 = 1;                               /* @0x801041B4 */
        return;
    }

    /* Modus 0 @0x80103D8C */
    if (e->sub_state_2 == 0) {                             /* @0x80103DB8 */
        re2s_clip(e, 0x0007000cu);                         /* Clip 12 @0x80103DC0-C8 */
        e->sub_state_2 = 1;                                /* @0x80103DD4 */
        if (e->re2s_row23c != 1)                           /* +0x23C != 1 @0x80103DDC */
            re2s_gore(e);                                  /* @0x80103DE4 */
    }
    int wrapped = re2s_advance(e, 512);                    /* @0x80103E08 */
    if (!wrapped) return;   /* Frame-Flag *(+0x178) & 0x80000 @0x80103E10-24 — im Port als
                             * Clip-Wrap genaehert (dieselbe deklarierte Naeherung wie der
                             * Schritt-SE; das Original feuert auf dem markierten EDD-Frame). */
    /* WELLE-F-FIX: +0x23A = -1 (@0x80103E84) und der +0x23E/+0x23F-Block (@0x80103E8C-94)
     * liegen im Original INNERHALB von `+0x10C != 0` (@0x80103E3C) — dem WASSER-Zweig. Welle E
     * hat sie unbedingt ausgefuehrt; das war eine Divergenz. +0x10C ist im Port immer 0. */
    if (e->re2s_water10c != 0) {                           /* @0x80103E3C */
        if (e->re2s_row23c == 9u || e->re2s_row23c == 10u || e->re2s_row23c == 16u)
            re2s_gore(e);                                  /* @0x80103E70 */
        e->re2s_c23a = -1;                                 /* @0x80103E84 (immer im Zweig) */
        if (e->re2s_sink23e == 0u) {                       /* @0x80103E88 */
            e->re2s_sink23e = 3;                           /* @0x80103E8C */
            e->re2s_sink23f = 1;                           /* @0x80103E94 */
        }
    }
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

/* ==========================================================================================
 * BABY-SPINNE (Typ 0x26) — EMS26.BIN, 4346 Byte, gelinkt @0x80100000. WELLE F.
 *
 * HERKUNFT: shared_assets/RE2/CDEMD0.EMS[0x3B6800 .. +4346] == info/re2leon/COMMON/BIN/EMS26.BIN
 * (SHA1-Gleichheit im Dateikopf belegt). TOC kind 0x26: Ovl-Slot0 Sektor 1901 / 0x10FA B.
 *
 * ERREICHBARKEIT (gemessen): Typ 0x26 kommt in KEINEM der 810 `Sce_em_set`-Records der 240
 * ausgelieferten RDTs vor — Baby-Spinnen entstehen AUSSCHLIESSLICH aus dem Laufzeit-Spawner der
 * Adult (FUN_80105D38, re2s_spawn_babies oben). Im Port ist der erreichbare Weg der
 * DEATH-Abschluss (@0x80104440 / @0x801047F8) und die schweren HURT-/DEATH-Zeilen.
 *
 * ROOT @0x8010001C:
 *   Pause-Gate `lw 0x800CFBDC & 0x20000000` (@0x8010002C-38) -> sofort return (der Aufrufer
 *     in enemy_ai_common.c haelt die Pause bereits ab, wie bei der Adult)
 *   +0x1D3: (v & 0x7F) != 0 -> v-1 (@0x80100040-54, dekrementiert das GANZE Byte)
 *   Dispatch `+0x4` ueber @0x80101084 (8 Eintraege, OHNE Bereichspruefung):
 *     [0] 0x801000DC INIT  [1] 0x801002A0 ACTIVE  [2] 0x80100BB8 HURT = `jr ra`
 *     [3] 0x80100BFC DEATH [4] 0x80100DD0 = `jr ra`  [5][6] NULL  [7] 0x80100DD8 = `jr ra`
 *   Tail: +0x106 = (-Y)/1800 (@0x80100080-B0); word0 == 0 -> return; FUN_80035530(self);
 *         FUN_8003567C(self, 1024).
 *
 * OPEN (kein Port-Kanal, jeweils mit dem Original-Nicht-Treffer-Wert):
 *   FUN_800527B4(X,Z) = der sce==7-Wasser-AOT-Scan -> 0 ("kein Wasser"). Damit sind der
 *     DROWN-Substate (@0x801009E4), der SINK-Todeszweig (@0x80100D40) und der Wasser-Revert im
 *     MOVE (@0x801006F4) unerreichbar. Sie sind trotzdem byte-true portiert.
 *   FUN_80016480 (Boden-Schatten-Slot @0x8010025C), FUN_80100F28 (ESP-Effekt-Records
 *     @0x801010DC), FUN_8003947C/0x80039514 (Rumble), FUN_80065B9C (Partner-Schaden).
 *   +0x1E8 = 1 (@0x80100164) ist die ANZAHL der Hitbox-Kugeln (FUN_80035408), NICHT Gravitation.
 *   Die X/Z-Anteile von FUN_80015350 (@0x8010079C / @0x801008E8) brauchen die PsyQ-RotMatrix-
 *     Reihenfolge (LIBGTE.H fuehrt RotMatrix / RotMatrixYXZ / RotMatrixZYX; im Repo NICHT
 *     belegt, welche 0x8008E1F4 ist) -> der Port fuehrt nur die Y-Komponente + den bereits
 *     etablierten Yaw-Schub re2s_thrust.
 */

/* Variante -> Start-Substate, Byte-Tabelle @0x801010A4 (roh `00 00 01 04 02 02 00 00`). */
static const uint8_t s_re2sb_start_sub[8] = { 0, 0, 1, 4, 2, 2, 0, 0 };   /* @0x801010A4 */

/* 0x800CFBF6 (Spieler-Schrittpuls) — derselbe Port-Zwilling, den enemy_ai_re2_zombie.c aus
 * FUN_80066BCC / FUN_8003D18C / FUN_8003CC80 hergeleitet hat: RUN setzt Bit 0x04, WALK 0x02,
 * alles andere nichts. Hier NACHGEZOGEN (nicht neu hergeleitet), weil das Baby `& 0x6` testet. */
static uint16_t re2sb_stepbits(const re15_actor_t *pl)
{
    if (pl->motion == 100) return 0x04u;                   /* RUN  @0x8003D18C */
    if (pl->motion == 105) return 0x02u;                   /* WALK @0x8003CC80 / @0x8003D6B4 */
    return 0x00u;
}

/* `probe` @0x80101020 — Boden-/Wasser-Orakel.
 *   fy = FUN_800527B4(X,Z)                       (@0x80101038; im Port 0 = kein Wasser)
 *   Y >= (s16)+0x1C2            -> 1 (gelandet)  (@0x8010104C)
 *   Y - 10 < fy                 -> 0 (noch frei) (@0x80101060)
 *   sonst                       -> fy            (< 0 = Wasser) */
static int re2sb_probe(const re15_actor_t *e)
{
    int32_t fy = (int32_t)e->re2s_water10c;                /* FUN_800527B4 @0x80101038 (OPEN) */
    if (e->y >= (int32_t)e->dog_floor_y) return 1;         /* @0x8010104C */
    if (e->y - 10 < fy) return 0;                          /* @0x80101060 */
    return (int)fy;
}

/* `bite_check` @0x80100DE8 — Biss gegen den Spieler; Rueckgabe 1 = getroffen.
 *   dist < (u16)Ziel[+0x9A] + 10        (@0x80100E40, die +10 als Literal @0x80100E3C)
 *   (u32)(Ziel.Y - self.Y) < 3000       (@0x80100E5C, UNSIGNED — ein Ziel UEBER der Spinne
 *                                        wickelt um und faellt durch)
 *   +0x221 != 0 -> 0                    (@0x80100E70, EIN Biss je Sprung)
 *   Ziel+0x1D3 & 0x80 -> 0              (@0x80100E84)
 *   SCHADEN: `addiu a0,zero,1` @0x80100EA8 / `jal 0x800401d4` @0x80100EAC, Modus a1 = 1
 *   danach: FUN_80100F28(self,0,1) (@0x80100EDC); +0x221 = 1 (@0x80100EEC);
 *           +0x144 = 90 (@0x80100EF8); +0x76 += 2048 (@0x80100EFC-F04, Kehrtwende). */
#define RE2SPIDER_BABY_BITE_DMG 1      /* @0x80100EA8 */
#define RE2SPIDER_BABY_BITE_PAD 10     /* @0x80100E3C */

static int re2sb_bite(re15_actor_t *e, re15_actor_t *pl)
{
    /* Ziel+0x9A ist der Nahkampf-Radius des OPFERS; der Port fuehrt ihn nicht als eigenes Feld
     * -> es bleibt die reine +10-Reichweite. OPEN (deklariert, kein geschaetzter Radius). */
    uint32_t reach = (uint32_t)RE2SPIDER_BABY_BITE_PAD;
    if (!(e->ai_dist < reach)) return 0;                   /* @0x80100E40 */
    if (!((uint32_t)(pl->y - e->y) < 3000u)) return 0;     /* @0x80100E5C (unsigned!) */
    if (e->re2s_legn221 != 0u) return 0;                   /* +0x221 @0x80100E70 */
    if (re15_player_is_grabbed()) return 0;                /* Ziel+0x1D3 & 0x80 @0x80100E84 */
    (void)re15_re2_player_damage_mode(pl, RE2SPIDER_BABY_BITE_DMG, 1);   /* @0x80100EA8-B0 */
    re2s_gore(e);                                          /* FUN_80100F28(self,0,1) @0x80100EDC */
    e->re2s_legn221 = 1;                                   /* @0x80100EEC */
    e->speed_h      = 90;                                  /* @0x80100EF8 */
    e->rot_y        = (int16_t)(e->rot_y + 2048);          /* @0x80100EFC-F04 */
    return 1;
}

/* ---- INIT @0x801000DC --------------------------------------------------------------------- */
static void re2sb_init(re15_actor_t *e)
{
    e->hp = 1;                                             /* +0x156 = 1 @0x801000F8 */
    re2s_clip(e, 0x000f0000u);                             /* Clip 0, Rate 15 @0x80100120 */
    e->re2s_c21e   = 120;                                  /* +0x21E = 120 @0x80100128 */
    e->re2s_t21c   = (int16_t)(re2s_rand() & 0x1fu);       /* +0x21C @0x80100140-50 */
    e->re2z_self1d3 = 6;                                   /* +0x1D3 = 6 @0x801001B0 */
    e->re2z_t158   = 0;                                    /* +0x158 = 0 @0x801001B4 */
    e->re2z_t15a   = 0;
    e->re2s_legn221 = 0;                                   /* +0x221 (Biss-Latch) */
    e->re2s_mode222 = 0;                                   /* +0x222 (airborne) */
    e->re2s_fall223 = 0;                                   /* +0x223 (Steuer-Bias) */
    e->speed_h = 0; e->re2d_vy146 = 0; e->re2s_z148 = 0;   /* @0x8010019C-A4 */
    e->rot_x = 0; e->rot_z = 0;                            /* @0x801001A8-AC */
    e->re2z_dir16a = 0;
    e->re2s_water10c = 0;                                  /* +0x10C: OPEN (s. Kopf) */

    unsigned v = (unsigned)(e->re2z_f10e & 0xffu);         /* +0x10E & 0xFF @0x801001C0 */
    e->state       = 1;                                    /* Zustandswort @0x801001CC */
    e->sub_state_1 = s_re2sb_start_sub[v & 7u];
    e->sub_state_2 = 0; e->sub_state_3 = 0;

    if (v >= 6u) return;                                   /* `sltiu v1,6` @0x801001D4 */
    if (v == 0u) {                                         /* @0x801001F8 */
        e->hp = -1;                                        /* UNVERWUNDBAR @0x80100204 */
        e->re2z_f10e = (uint16_t)(e->re2z_f10e | 0x4000u); /* @0x80100214-20 */
    } else if (v == 4u || v == 5u) {                       /* @0x80100224 */
        e->speed_h = 200;                                  /* +0x144 = 200 @0x80100234 */
    }
    /* Gemeinsamer Tail @0x80100244: FUN_80016480 (Schatten-Slot) + FUN_800527B4 -> word0 |= 0x400
     * (@0x8010026C-74) — beides ohne Port-Kanal (OPEN). */
}

/* ---- ACTIVE sub1 = 1 @0x801002FC — BODEN ---------------------------------------------------
 * Erst der +0x6-Automat (Tabelle @0x801010C0 = {0x8010051C WAIT, 0x80100630 MOVE, NULL, NULL}),
 * danach IMMER: Angriffs-Trigger (A), Zertret-Gate Spieler (B), Zertret-Gate Partner (C, im
 * Port tot — RE1.5 hat keinen Partner-Slot), FUN_8002959C(a3=1024) (D @0x801004F4). */
static void re2sb_ground(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0u) {                            /* WAIT @0x8010051C */
        if (e->sub_state_3 == 0u) {                        /* S0 @0x8010054C */
            uint32_t r = re2s_rand();                      /* @0x80100550 */
            uint8_t  m = e->re2s_legn221;                  /* +0x221 @0x80100558 */
            e->sub_state_3  = 1;                           /* @0x80100554 */
            e->re2s_legn221 = 0;                           /* @0x80100560 */
            e->re2z_t158    = (int16_t)((r + 30u) & (uint32_t)(m * 30u + 63u));  /* @0x80100574 */
            uint32_t r2     = re2s_rand();                 /* @0x80100578 */
            /* `srav 0x1111, r2 & 0xF` (@0x80100584-98): Treffer bei r2&0xF in {0,4,8,12} = 25 %
             * -> -64 = WEG vom Ziel (Flucht), +64 = HIN (FUN_80015558 mit negativem Schritt
             * peilt Bearing+0x800). */
            e->re2s_fall223 = (uint8_t)(int8_t)(((0x1111u >> (r2 & 0xfu)) & 1u) ? -64 : 64);
                                                           /* +0x223 @0x801005A4 */
            /* Partner-Zielwahl @0x801005A8-E4 — im Port tot, Ziel bleibt der Spieler. */
        } else if (e->sub_state_3 == 1u) {                 /* S1 @0x801005E8 */
            uint16_t t = (uint16_t)e->re2z_t158;
            e->re2z_t158 = (int16_t)(t - 1u);              /* @0x801005F0 */
            if (t == 0u) { e->sub_state_2 = 1; e->sub_state_3 = 0; }   /* `sh 1,6` @0x80100600 */
        }
        if (re2sb_probe(e) == 0) {                         /* @0x80100604-0C */
            e->sub_state_1 = 2; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* -> FALL */
        }
    } else if (e->sub_state_2 == 1u) {                     /* MOVE @0x80100630 */
        if (e->sub_state_3 == 0u) {                        /* S0 @0x80100664 */
            e->sub_state_3 = 1;                            /* @0x8010066C */
            e->speed_h     = (int16_t)((re2s_rand() & 0x3fu) + 90u);  /* @0x80100668-78 */
            e->re2s_z148   = 0;                            /* @0x8010067C */
            e->re2z_t158   = (int16_t)(re2s_rand() & 0x4eu);          /* @0x80100688 */
        }
        if (e->sub_state_3 == 1u) {                        /* S1 @0x8010068C */
            re15_enemy_steer_point(e, pl->x, pl->z, (int)(int8_t)e->re2s_fall223);  /* @0x80100698 */
            e->rot_y      = (int16_t)(e->rot_y + (int8_t)(uint8_t)e->re2s_c21e);    /* @0x801006B0 */
            e->re2s_c21e  = (uint16_t)(uint8_t)(-(int8_t)(uint8_t)e->re2s_c21e);    /* @0x801006C4 */
            uint16_t t = (uint16_t)e->re2z_t158;
            e->re2z_t158 = (int16_t)(t - 1u);              /* @0x801006C8 */
            if (t == 0u) { e->sub_state_2 = 0; e->sub_state_3 = 0; }   /* `sh 0,6` @0x801006CC */
        }
        re2s_thrust(e, (int)(int8_t)(uint8_t)e->re2s_c21e);/* FUN_800152C8 @0x801006DC */
        /* Wasser-Revert auf +0x44/+0x46/+0x48 @0x801006F4-14 — OPEN (+0x10C == 0). */
        if (re2sb_probe(e) == 0) {                         /* @0x80100718-20 */
            e->sub_state_1 = 2; e->sub_state_2 = 0; e->sub_state_3 = 0;
        }
    }

    /* (A) Angriffs-Trigger @0x80100340-D0 */
    int arc = re15_ai_arc_test(e, pl->x, pl->z, 256);      /* @0x80100350 */
    if (e->re2s_t21c != 0) e->re2s_t21c--;                 /* +0x21C @0x80100358-6C */
    if (re2s_tdist(e) < 2000u && arc == 0 && e->re2s_t21c == 0
        && !re15_player_is_grabbed()) {                    /* @0x80100388-C0 */
        e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* @0x801003CC-D0 */
    }

    /* (B) ZERTRETEN @0x801003D4-460 — vier Bedingungen, alle noetig:
     *   dist < 500                              (`sltiu 0x1f4` @0x801003DC)
     *   (0x800CFBF6 & 6) != 0                   (@0x801003EC-F4)
     *   +0x106 == Spieler+0x106 (gleiche Etage) (@0x80100410)
     *   Spieler+0x04 in {0x101,0x201,0x301,0x401} (@0x80100424-3C) = Zustand 1 und Substate
     *     1..4 — Substate 5 (RENNEN) ist ABSICHTLICH ausgeschlossen.
     *   Wirkung: +0x4 = 3 (DEATH) @0x80100448 + zwei Rumble-Queues (im Port kein Kanal). */
    if (re2s_tdist(e) < 500u
        && (re2sb_stepbits(pl) & 0x6u) != 0u
        && e->re2s_f106 == (uint8_t)((-pl->y) / 1800)
        && pl->state == 1u
        && pl->sub_state_1 >= 1u && pl->sub_state_1 <= 4u) {
        e->state = 3; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;  /* @0x80100448 */
        return;
    }
    (void)re2s_advance(e, 1024);                           /* @0x801004F4 */
}

/* ---- ACTIVE sub1 = 2 @0x80100748 — FALLEN ------------------------------------------------- */
static void re2sb_fall(re15_actor_t *e)
{
    if (e->sub_state_2 == 0u) {                            /* @0x80100774 */
        e->sub_state_2  = 1;                               /* @0x80100780 */
        e->re2d_vy146   = 40;                              /* +0x146 = 40 @0x80100784 */
        e->re2s_mode222 = 1;                               /* airborne @0x80100788 */
    }
    e->y += e->re2d_vy146;   /* FUN_80015350(self,0,0) @0x8010079C — nur die Y-Komponente,
                              * X/Z brauchen die RotMatrix-Reihenfolge (OPEN, s. Kopf) */
    e->re2d_vy146 = (int16_t)((e->re2d_vy146 < 800) ? e->re2d_vy146 + 40 : 800);  /* @0x801007B0-C0 */
    e->re2z_self1d3 = 255;                                 /* @0x801007C8 */
    int a3 = re2sb_probe(e);                               /* @0x801007CC */
    if (a3 != 0) {                                         /* @0x801007D8 */
        e->re2z_self1d3 = 0;                               /* @0x801007E4 */
        e->re2s_mode222 = 0;                               /* @0x801007E8 */
        if (a3 >= 0) {                                     /* LAND @0x801007F8/@0x8010082C */
            e->sub_state_1 = 1;                            /* @0x80100834 */
            e->y           = (int32_t)e->dog_floor_y;      /* @0x8010083C */
        } else {                                           /* WASSER @0x80100800 */
            e->y           = a3 + 10;                      /* @0x80100810 */
            e->sub_state_1 = 4;                            /* DROWN @0x8010081C */
        }
        e->sub_state_2 = 0; e->sub_state_3 = 0;            /* @0x8010084C */
    }
    (void)re2s_advance(e, 1024);                           /* @0x80100850 */
}

/* ---- ACTIVE sub1 = 3 @0x80100880 — SPRUNG-ANGRIFF ---------------------------------------- */
static void re2sb_leap(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0u) {                            /* @0x801008A8 */
        e->sub_state_2  = 1;                               /* @0x801008B0 */
        e->speed_h      = (int16_t)((re2s_rand() & 0x3fu) + 90u);  /* @0x801008AC-BC */
        e->re2d_vy146   = -300;                            /* @0x801008C4 */
        e->re2s_legn221 = 0;                               /* Biss-Latch @0x801008D0 */
        e->re2s_mode222 = 1;                               /* airborne @0x801008D4 */
    }
    e->y += e->re2d_vy146;                                 /* FUN_80015350 @0x801008E8 (s. Kopf) */
    re2s_thrust(e, 0);                                     /* X/Z-Anteil desselben Aufrufs */
    e->re2d_vy146 = (int16_t)(e->re2d_vy146 + 40);         /* @0x801008FC-904 */
    (void)re2sb_bite(e, pl);                               /* FUN_80100DE8(self,0) @0x80100900 */
    int s0 = re2sb_probe(e);                               /* @0x80100930 */
    if (s0 == 0) return;                                   /* @0x8010093C */
    e->re2s_mode222 = 0;                                   /* @0x80100958 */
    if (s0 >= 0) {                                         /* LAND @0x80100994 */
        e->sub_state_1 = 1;                                /* @0x80100998 */
        e->sub_state_2 = 0; e->sub_state_3 = 0;            /* @0x801009A0 */
        e->re2s_t21c   = (int16_t)((re2s_rand() & 0x1fu) + 30u);   /* +0x21C @0x8010099C-B0 */
        e->y           = (int32_t)e->dog_floor_y;          /* @0x801009C4 */
    } else {                                               /* WASSER @0x8010095C */
        e->hp    = -1;                                     /* @0x80100970 */
        e->y     = s0 + 10;                                /* @0x80100978 */
        e->state = 3; e->sub_state_1 = 0;                  /* @0x80100984 */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
    }
    /* Dieser Handler ruft KEIN FUN_8002959C — die Sprung-Pose steht still. */
}

/* ---- ACTIVE sub1 = 4 @0x801009E4 — WASSER / ERTRINKEN ------------------------------------- */
static void re2sb_drown(re15_actor_t *e, re15_actor_t *pl)
{
    if (e->sub_state_2 == 0u) {                            /* @0x80100A14 */
        e->sub_state_2 = 1;                                /* @0x80100A18 */
        e->re2z_t158   = 450;                              /* Ertrink-Timer @0x80100A24 */
        e->re2z_t15a   = (int16_t)((re2s_rand() & 1u) ? 32 : -32);  /* @0x80100A20-3C */
        e->re2z_dir16a = (uint8_t)(re2s_rand() & 0x1fu);   /* @0x80100A38-44 */
    }
    int arc = re15_ai_arc_test(e, pl->x, pl->z, 256);      /* @0x80100A64 */
    int d   = (int)(int8_t)(uint8_t)e->re2s_c21e + (int)(uint16_t)e->re2z_t15a;   /* @0x80100A94 */
    e->re2s_c21e = (uint16_t)(uint8_t)(-(int8_t)(uint8_t)e->re2s_c21e);           /* @0x80100AA0 */
    e->rot_y     = (int16_t)(e->rot_y + d);                /* @0x80100AA4-AC */
    (void)re2s_advance(e, 1024);                           /* @0x80100AA8 */
    e->re2z_dir16a++;                                      /* @0x80100AB8 */
    if ((e->re2z_dir16a & 0xfu) == 0u) {                   /* @0x80100AC4 */
        re2s_gore(e);                                      /* Platscher @0x80100AD0 */
        e->re2z_dir16a = (uint8_t)(e->re2z_dir16a + (re2s_rand() & 7u));   /* @0x80100AD8-EC */
    }
    if (re2s_tdist(e) < 2000u && arc == 0 && !re15_player_is_grabbed()) {   /* @0x80100AFC-34 */
        e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0;         /* @0x80100B40 */
    }
    e->y = (int32_t)e->re2s_water10c + 10;                 /* @0x80100B58 */
    if (re2sb_probe(e) >= 0) {                             /* @0x80100B54-5C */
        e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
        return;
    }
    e->re2z_t158 = (int16_t)((uint16_t)e->re2z_t158 - 1u); /* @0x80100B78 */
    if ((uint16_t)e->re2z_t158 == 0u) {                    /* @0x80100B84 */
        e->hp    = -1;                                     /* @0x80100B8C */
        e->state = 3; e->sub_state_1 = 0;                  /* @0x80100B94 */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
    }
}

/* ---- DEATH @0x80100BFC --------------------------------------------------------------------
 *   FUN_800527B4(X,Z) < (s16)+0x1C2 UND +0x222 == 0 -> SINK @0x80100D40, sonst SPLAT
 *   (@0x80100C20-44).
 *   SPLAT @0x80100C84: +0x6 != 0 -> FUN_8001B250(self) = DESPAWN (@0x80100D24); sonst
 *   +0x6 = 1 (@0x80100CA4), Schattenfarbe 0xBF10BF (@0x80100CD8-E0, kein Port-Kanal),
 *   SE 7 (@0x80100CF8), FUN_80100F28(self, 0, tbl[+0x5]) (@0x80100D10-14). */
static void re2sb_death(re15_actor_t *e)
{
    if (e->re2s_water10c < (int32_t)e->dog_floor_y && e->re2s_mode222 == 0u) {
        /* SINK @0x80100D40 — byte-true portiert; im Port nur erreichbar, wenn +0x1C2 > 0 ist
         * (das Modul setzt implizit +0x1C2 <= 0 in trockenen Raeumen voraus, s. Kopf). */
        e->y      += 30;                                   /* @0x80100D68 */
        e->rot_y   = (int16_t)(e->rot_y + 32);             /* @0x80100D74-7C */
        e->rot_z   = (int16_t)(e->rot_z + 64);             /* @0x80100D78-84 */
        (void)re2s_advance(e, 1024);                       /* @0x80100D80 */
        if (e->y >= (int32_t)e->dog_floor_y                /* @0x80100D94 */
            || e->y < (int32_t)e->re2s_water10c) {         /* @0x80100DA8 */
            e->active = 0;                                 /* FUN_8001B250 @0x80100DB4 */
        }
        return;
    }
    if (e->sub_state_2 != 0u) { e->active = 0; return; }   /* DESPAWN @0x80100C9C/@0x80100D24 */
    e->sub_state_2 = 1;                                    /* @0x80100CA4 */
    re2s_se(7);                                            /* SE 7 @0x80100CF8 */
    re2s_gore(e);                                          /* FUN_80100F28 @0x80100D14 */
}

/* ---- Root @0x8010001C ---------------------------------------------------------------------- */
int re15_re2spider_baby_tick(int slot)
{
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    if ((e->re2z_self1d3 & 0x7fu) != 0u)                   /* @0x80100040-54 */
        e->re2z_self1d3--;

    e->ai_dist = (uint32_t)re15_enemy_player_dist(e, pl);  /* +0x1F0 (EXE-seitig gefuellt) */

    switch (e->state) {                                    /* Tabelle @0x80101084 */
    case 0: re2sb_init(e); break;                          /* @0x801000DC */
    case 1:                                                /* @0x801002A0, Tabelle @0x801010AC */
        switch (e->sub_state_1) {
        case 0: (void)re2s_advance(e, 1024); break;        /* @0x801002DC (nur Anim, kein Ausgang) */
        case 1: re2sb_ground(e, pl);         break;        /* @0x801002FC */
        case 2: re2sb_fall(e);               break;        /* @0x80100748 */
        case 3: re2sb_leap(e, pl);           break;        /* @0x80100880 */
        case 4: re2sb_drown(e, pl);          break;        /* @0x801009E4 */
        default: break;
        }
        break;
    case 3: re2sb_death(e); break;                         /* @0x80100BFC */
    default: break;    /* [2] HURT und [4]/[7] sind `jr ra`; [5]/[6] sind NULL */
    }

    e->re2s_f106 = (uint8_t)((-e->y) / 1800);              /* @0x80100080-B0 */
    return 1;
}

/* ============================ Besitz-Gate ================================================= */
/* Welle E besitzt einen Aktor nur, wenn sein Spawn-Deskriptor den BODEN-Modus waehlt — genau
 * die INIT-Sprungtabellen-Indizes 0 und 1 (@0x80100460 / @0x8010058C). Decken- (2/3) und
 * Wand-Spawns (4..11) landen in +0x222 = 1 bzw. 3, deren ACTIVE-Handler (@0x801013AC /
 * @0x80102B40) in Welle E NICHT portiert sind; die bleiben deshalb ueber ihre GANZE Lebenszeit
 * auf dem unveraenderten byte-true RE1.5-Brain (kein Flavor-Wechsel mitten im Kampf). */
/* WELLE F: das Gate ist jetzt AUSNAHMSLOS. Welle E musste Decken- (+0x222 = 1) und Wandspawns
 * (+0x222 = 3) dem RE1.5-Brain lassen, weil deren ACTIVE-Handler nicht portiert waren; mit
 * Modus 1/2/3 (@0x801013AC / @0x80101CC8 / @0x80102B40) und der Baby-Spinne (EMS26.BIN) faellt
 * diese Ausnahme weg. Das Gate haengt weiterhin NUR an unveraenderlichen Feldern (Typ), also
 * kann es mitten im Kampf nicht kippen. Ausserhalb 0..11 greift im Original die INIT-Klemme
 * `sltiu v0,v1,0xc` @0x8010043C -> +0x222 bleibt 0 (Boden), also ebenfalls unser Modus 0. */
int re15_re2spider_owns(const re15_actor_t *e)
{
    if (!e) return 0;
    return (e->type == 0x25u) || (e->type == 0x26u);
}

void re15_re2spider_room_reset(void)
{
    /* Der Flock-/Faden-Mutex 0x800CFBF4 Bit 0x20 ist raum-lokal (FUN_80052F3C nullt das Wort bei
     * jedem Raum-Load). Der Port teilt das Wort mit Hund und Kraehe; re15_re2dog_room_reset()
     * loescht es bereits fuer alle. Hier nur das Spinnen-Bit, damit ein Raumwechsel mitten im
     * Abseilen keinen dauerhaft belegten Mutex hinterlaesst. */
    g_re2_room_gflags &= (uint16_t)~0x20u;                 /* @0x80101B98 / @0x80102778 */
}

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
