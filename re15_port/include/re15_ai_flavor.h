/*
 * RE1.5 Rebuilt — AI FLAVOR switch (PORT OPTION, not present in any original).
 *
 * The user can pick which game's ZOMBIE brain runs:
 *   RE15 (default) — the byte-true RE1.5 zombie AI (enemy_ai_common.c). Unchanged, always the default,
 *                    so every byte-true campaign/test keeps passing.
 *   RE2            — the Resident Evil 2 (retail, Leon) zombie AI, RE'd from the RE2 enemy overlay
 *                    EMZ0.BIN (loads RAW @0x80100000; disassemble with
 *                    .claude/skills/re15-psx-disasm/scripts/re2_disasm.py --bin EMZ0.BIN).
 *
 * WELLE C erweitert den Schalter per Nutzer-Auftrag auf den HUND (Cerberus 0x20, RE2-Modul
 * EMD0G_MOD0.BIN == CDEMD0.EMS TOC-Sektor 1206, geladen @0x80100000). WELLE D auf die KRÄHE
 * (0x21, EMOVL21_S0.BIN == CDEMD0.EMS Sektor 0x528, Slot 0 @0x80100000).
 * Spider/Maggot/Birkin/NPCs bleiben in beiden Modi auf dem RE1.5-Brain — deren byte-true
 * Kampagnen bleiben unberührt, und der RE1.5-Default ist weiterhin in ALLEN Typen byte-identisch.
 */
#ifndef RE15_AI_FLAVOR_H
#define RE15_AI_FLAVOR_H

typedef enum {
    RE15_AI_FLAVOR_RE15 = 0,   /* byte-true RE1.5 (DEFAULT) */
    RE15_AI_FLAVOR_RE2  = 1    /* RE2 retail zombie brain   */
} re15_ai_flavor_t;

re15_ai_flavor_t re15_ai_flavor(void);
void             re15_ai_flavor_set(re15_ai_flavor_t f);

/* ---- MODELLHERKUNFT — ⛔ NUR NOCH TEST-HAKEN, KEIN MENUE-ZUSTAND MEHR ----------------------
 * WELLE G (2026-08-19) hatte den OPTIONS→AI-Schalter auf DREI Stufen erweitert:
 *   RE1.5 / RE2 (RE1.5-Modelle + RE2-Gehirn/-Animation, Hybrid-Rig) / RE2 MODELS.
 * ⛔ 2026-08-21 hat der Nutzer die DRITTE Stufe abbestellt ("Mittlerweile ist RE 2 AI schon so
 * gut, dass wir diese Option im Optionsmenue sowie seine eigenen Models entfernen koennen.
 * Wir konzentrieren uns jetzt nur noch auf RE 1.5 AI und RE 2 AI."). Der Port faehrt unter dem
 * RE2-Flavor JETZT IMMER das Hybrid-Rig: platform/pc/main.c pc_enemy_load ruft
 * pc_enemy_hybrid_re15_models bedingungslos und liest dieses Flag NICHT MEHR.
 *
 * Es bleibt nur als Schalter fuer den Rig-Umbau selbst stehen (tests/unit/test_re2_hybrid_rig.c,
 * test_re2_lyer_1140.c pruefen beide Modellherkuenfte am Bank-Umbau, nicht am Menue) — der
 * Speicher dafuer liegt in enemy_ai_re2_zombie.c. KEIN Spiel-Codepfad darf neu darauf keyen.
 * ⛔ NICHT die Clip-Wahl: re15_victim_clip_map (enemy_ai_common.c) waehlt CLIP-Indizes in der
 *    Victim-BANK — das ist ANIMATION, die im RE2-Modus RE2 bleibt, und keyt auf
 *    re15_ai_flavor(). */
typedef enum {
    RE15_AI_MODELS_RE15 = 0,   /* RE1.5-Mesh + -Textur, RE2-Skelettordnung (Hybrid) */
    RE15_AI_MODELS_RE2  = 1    /* RE2-Mesh + -Textur (DEFAULT, = bisheriges Verhalten) */
} re15_ai_models_t;

re15_ai_models_t re15_ai_models(void);
void             re15_ai_models_set(re15_ai_models_t m);

/* Does the RE2 brain own this actor type? (zombie family only) */
int re15_re2z_owns_type(unsigned type);

/* ---- PORT-OPTION 2026-08-20: "RE2-ZOMBIE-UEBERNAHME IM RE1.5-MODUS" -------------------------
 * ⛔ KEIN byte-true Verhalten — eine AUSDRUECKLICH vom Nutzer verlangte Abweichung vom
 * RE1.5-Original ("... Gliedmassen abgeschossen werden koennen ... sowie die Schadenswerte fuer
 * Zombies ... moechte ich auch in RE1.5 AI haben"). Der Schalter zieht ZWEI vollstaendig belegte
 * RE2-Systeme in den RE1.5-Modus, und zwar NUR fuer die Zombie-Familie (re15_re2z_owns_type):
 *   (1) den ZERLEGER (Gliedmassen-Abschuss): re15_re15_re2z_gore_hit() ruft aus dem
 *       RE1.5-Trefferpfad den Applier-Stempel (+0x1D0 @0x80041A0C-84, Zonen-Reserve
 *       @0x80041900-9C) und den Zerleger @0x80105288-3D8 auf. Es wird KEIN RE1.5-Zustandsfeld
 *       angefasst — nur re2z_*-Felder, die das RE1.5-Gehirn nirgends liest.
 *   (2) das SCHADENS-/HP-MODELL (re15_damage.c): RE2-Schadenszeile 0x800A6A88[typ] +
 *       RE2-INIT-HP (@0x8010C670, +15 bei Raum-Gegnerzahl < 4, Typ 0x11 fest 250 @0x801008C8).
 *       BEIDE HAELFTEN ZUSAMMEN — nur eine davon waere eine unbelegte Balance-Aenderung.
 * Hund 0x20, Kraehe 0x21 und Spinne 0x25/0x26 bleiben im RE1.5-Modus UNVERAENDERT.
 *
 * DEFAULT = AN. Zurueck auf den byte-true RE1.5-Auslieferungsstand: RE15_RE15_RE2Z_IMPORT=0
 * oder re15_re15_re2z_import_set(0) — genau diesen Hebel benutzen die byte-true-PINs, um
 * weiterhin die ORIGINALWERTE zu pruefen.
 * (Der Einstieg des Zerlegers, re15_re15_re2z_gore_hit(), steht bei den uebrigen
 * Gore-Bruecken-Deklarationen in re15_actor.h.) */
int  re15_re15_re2z_import(void);
void re15_re15_re2z_import_set(int on);
int  re15_re15_re2z_import_owns(unsigned type);   /* RE1.5-Flavor + Option + Zombie-Typ */

/* WELLE C: the FULL RE2 ownership set (zombie family + dog 0x20). NUR der Asset-Loader keyt
 * darauf (platform/pc/main.c pc_enemy_load). Review-Fix #9 — ausdruecklich NICHT darauf keyen:
 * die Anim-Advance-/Loco-/SFX-Exemptions bleiben auf re15_re2z_owns_type (player_common.c
 * re15_type_self_advances_anim + enemy_ai_common.c), die Victim-Map keyt auf Flavor +
 * g_player_victim_type. Der Hund braucht sie nicht (Typ 0x20 self-advanct schon immer).
 * ⚠️ Welle D: ein NEUER Typ in diesem Set erbt die Exemptions NICHT automatisch — jede
 * geteilte Stelle muss einzeln geprueft und explizit verdrahtet werden. */
int re15_re2_owns_type(unsigned type);

#include <stdint.h>

/* ---- WELLE C: the RE2 retail DOG (Cerberus, kind 0x20) brain — enemy_ai_re2_dog.c ---------
 * re15_re2dog_tick REPLACES the RE1.5 dog dispatch (re15_dog_ai_tick) when the RE2 flavor is
 * selected. RE'd from EMD0G_MOD0.BIN: root @0x80100004, state table @0x80105438, 17 ACTIVE
 * substates @0x80105464, HURT @0x801032A8, DEATH @0x801040DC, CORPSE @0x801049EC. */
int  re15_re2dog_tick(int slot);                  /* 1 = handled (RE2 dog brain owns this actor) */
void re15_re2dog_audio_hook(void (*se_fn)(int se_id, int flag2000), void (*bank_fn)(int bank));
void re15_re2dog_room_reset(void);                /* room load: 0x800CFBF4-Analog-Bits löschen
                                                   * (einziger EXE-Clear: FUN_80052f3c @0x80052f3c) */

/* The shared RE2 PRNG @0x80015FE8 (state 0x800CE318 — ONE generator for every RE2 overlay;
 * the dog draws from the same stream as the zombie). Defined in enemy_ai_re2_zombie.c. */
uint32_t re15_re2_rand(void);

/* ---- WELLE D: the RE2 retail CROW (kind 0x21) brain — enemy_ai_re2_crow.c -----------------
 * re15_re2crow_tick REPLACES the RE1.5 crow dispatch (re15_crow_ai_tick) when the RE2 flavor
 * is selected. RE'd from EMOVL21_S0.BIN (== CDEMD0.EMS Sektor 0x528, Slot 0 @0x80100000):
 * Root @0x8010013C, state table @0x80104908, DEC/EXEC @0x80104928/@0x80104964 (15 Subs),
 * HURT rows @0x80104A18, State-4 @0x80104A64, CORPSE @0x80104A70. */
int  re15_re2crow_tick(int slot);                 /* 1 = handled (RE2 crow brain owns this actor) */
void re15_re2crow_audio_hook(void (*se_fn)(int se_id, int flag2000), void (*bank_fn)(int bank));
void re15_re2crow_se_play(int se_id);             /* Testhaken auf den ENEMSE-Pfad des Brains */

#include "re15_actor.h"

/* ---- WELLE E: das RE2-SPINNEN-Brain (kind 0x25 Adult / 0x26 Baby) — enemy_ai_re2_spider.c --
 * re15_re2spider_tick ERSETZT den RE1.5-Spinnen-Dispatch (re15_adult_spider_ai_tick bzw.
 * re15_spider_ai_tick) unter dem RE2-Flavor. RE'd aus EMS25.BIN / EMS26.BIN (== CDEMD0.EMS
 * Sektor 1825 bzw. 1901, Slot 0 @0x80100000; SHA1-Gleichheit im Dateikopf belegt):
 *   Adult: Root @0x801000C8, Zustandstabelle @0x80106420, ACTIVE @0x801005AC mit der
 *          OBERFLAECHEN-Schicht +0x222 (@0x80106440) ueber vier Substate-Tabellen
 *          (@0x80106450 Boden / @0x8010649C Decke / @0x801064CC Uebergang / @0x801064FC Wand),
 *          HURT @0x80102C78, DEATH @0x80103C80, State 4 @0x80104CF0, CORPSE @0x80104CF8.
 *   Baby:  Root @0x8010001C, Zustandstabelle @0x80101084 (HURT/State4/CORPSE sind `jr ra`). */
int  re15_re2spider_tick(int slot);               /* 1 = handled (RE2-Spinnen-Brain besitzt ihn) */
int  re15_re2spider_baby_tick(int slot);          /* WELLE F: Baby 0x26 (EMS26.BIN @0x8010001C) */
/* Besitz-Gate: Welle E deckt den BODEN-Modus (+0x222 == 0) end-to-end ab. Decken-/Wand-Spawns
 * (INIT-Sprungtabelle @0x80100004 Index 2..11 -> +0x222 = 1 bzw. 3) bleiben ueber ihre GANZE
 * Lebenszeit auf dem unveraenderten byte-true RE1.5-Brain — kein Flavor-Wechsel mitten im Kampf. */
int  re15_re2spider_owns(const re15_actor_t *e);
void re15_re2spider_audio_hook(void (*se_fn)(int se_id, int flag2000), void (*bank_fn)(int bank),
                               int baby);
void re15_re2spider_room_reset(void);             /* Raumladen: Spawn-Budget der Adult-Spinne   */

/* Das geteilte Raum-Flag-Wort 0x800CFBF4 (Hund-Bits 0x20/0x40/0x80 + Kraehen-Flock-Mutex
 * Bit 0x1). EIN Original-Wort -> EIN Port-Global (enemy_ai_re2_dog.c); einziger EXE-Clear
 * ist der Room-Init FUN_80052f3c -> re15_re2dog_room_reset/re15_re2z_rng_reset. */
extern uint16_t g_re2_room_gflags;


/* RE2 zombie brain entry points (enemy_ai_re2_zombie.c). */
#include <stdint.h>
#include "re15_actor.h"

/* Der geteilte 0x8002959c-Anim-Advancer (Kern 0x80029B28-4C: +1/Tick, Wrap auf 0, done NUR
 * am Wrap-Tick; frac-Decay @0x800299C0-CC). Definiert in enemy_ai_re2_dog.c (Welle C),
 * benutzt von Hund UND Kraehe (44 jal 0x8002959c im Kraehen-Modul). */
int re15_re2_advance_959c(re15_actor_t *e, int blend);

/* FUN_800401d4 mit MODE-Parameter (Kraehen-Peck a0=5, a1=aliveflag @0x8010265C-64) —
 * enemy_ai_re2_zombie.c, teilt den EINEN One-Save-Latch mit Zombie-/Hunde-Biss. */
int re15_re2_player_damage_mode(re15_actor_t *pl, int dmg, int mode);

/* WELLE D: LOS-Shim (enemy_ai_common.c) — MAPPING fuer den RE2-Ray 0x80050858(self,PL,0x8400,0)
 * (Root @0x801001C0-E8: ret==0 -> +0x22A|=2 "Sicht frei"). Der Port fährt den byte-true
 * RE1.5-Ray FUN_8003dcc4 (re15_los_ray_blocked, alle 4 Regionen in EINEM Tick, ohne den
 * RE1.5-16-Tick-Amortisierer und ohne FOV-Kegel — 0x80050858 ist ein reiner Kollisionsstrahl).
 * Rueckgabe 1 = Sicht frei (Mode 0x8400 selbst nicht RE'd — deklariertes MAPPING). */
int re15_re2_los_clear(re15_actor_t *e, re15_actor_t *pl);
void re15_re2z_gait_init(re15_actor_t *e);        /* seed the gait row/timer  @0x80101A7C-AC   */
int  re15_re2z_walk_turn(re15_actor_t *e, int32_t px, int32_t pz, uint32_t dist); /* @0x80101BAC */
void re15_re2z_rng_reset(void);                   /* re-seed the RE2 PRNG on room load (also
                                                   * clears the FUN_800401d4 one-save latch)    */

/* ---- WELLE B: the FULL RE2 zombie brain -----------------------------------------------------
 * re15_re2z_tick REPLACES the RE1.5 state dispatch for an owned zombie when the RE2 flavor is
 * selected (hooked in re15_enemy_ai_live_tick / re15_zgirl_ai_tick). It runs the RE2 overlay's
 * root prolog (cooldown bank @0x8010045C-98) + state machine: ACTIVE @0x8010114C with the
 * decision-then-executor double dispatch (@0x801011A8-EC), HURT @0x80104F40, DEATH @0x80108250,
 * CORPSE @0x8010A440, state 8 @0x80109CFC. Presentation uses the REAL RE2 EM01x bank the Welle-A
 * loader fills (clip indices are RE2-native) + the ENEMSE SE bank via the audio hook below. */
int  re15_re2z_tick(int slot);                    /* 1 = handled (RE2 brain owns this actor)    */

/* ---- WELLE 5: der RE2-KRIECHER -------------------------------------------------------------
 * Setzt +0x10E Bit 0 (`andi 0xffc0 / ori 1 / sh 270` @0x80104590-98) und das Zustandswort, das
 * den Kriecher-Substate waehlt: `sub` 1 = Kampf-Eintritt = GRIFF (`sw 0x101` @0x8010458C),
 * `sub` 0 = die Kriech-LOKOMOTION (`sw 1` @0x80107A58, der Ausgang des Kriecher-HURT).
 * Setzt zusaetzlich den Ein-Angreifer-Riegel Spieler+0x1D3 |= 0x80 (@0x8010459C-B0) und die
 * SCA-Zeile 8 (BRUECKE zu den RE1.5-Raumdaten, s. Block an der Definition). */
void re15_re2z_enter_crawler(re15_actor_t *e, re15_actor_t *pl, unsigned sub);

/* ---- RE1.5-Waffe -> RE2-ATTACKEN-ID (= Zeile der Trefferreaktions-Tabelle @0x8010C940) -------
 * Im Original ist diese Zeile die ITEM-ID der gefuehrten Waffe: EQUIP FUN_8006B000 schreibt sie
 * nach 0x800D5BFA (`sb v0,23546(at)` @0x8006B09C, Gate `sltiu <0x14` @0x8006B040), FUN_8003BAF0
 * uebernimmt sie nach `+0x10E` (`sh v0,270(s2)` @0x8003BD4C) und der Applier stempelt sie als
 * `+0x5 = (a1>>16)+1` @0x80041AA0-B4 (a1 aus `((+0x10E & 0xFFF)-1)<<16` @0x80047EB4-C8).
 * Der Port schiesst mit dem RE1.5-Hitscan (`+0x5 = weapon_id` @0x800124BC) — ein ANDERER Id-Raum.
 * Diese beiden Funktionen uebersetzen ihn; die vollstaendige Begruendung je Waffe steht als
 * Tabelle in enemy_ai_re2_zombie.c. `col` ist die gestempelte Spalte (+0x1D2), `survived` = 0,
 * wenn der Treffer toedlich war (dann laeuft die DEATH-Wurzel und die Zeile ist folgenlos).
 * GARANTIE: der Rueckgabewert liegt in 1..19 und trifft fuer survived != 0 nie eine NULL-Zelle. */
uint8_t re15_re2z_row_for_weapon (unsigned weapon_id,  unsigned col, int survived);
uint8_t re15_re2z_row_for_atktype(unsigned attack_type, unsigned col, int survived);

/* PC registers the ENEMSE playback here (engine stays link-clean for the PSX target, which
 * never runs the RE2 flavor). bank_fn selects the ENEMSE bank (audio_pc load_re2_enemy_se). */
void re15_re2z_audio_hook(void (*se_fn)(int se_id, int flag2000), void (*bank_fn)(int bank));

/* ---- WELLE B shims exported from enemy_ai_common.c (wrap its statics) ---------------------- */
void re15_re2z_player_pin(void);                  /* s_player_grabbed = 1 (per-frame pin)       */
int  re15_re2z_mash(void);                        /* re15_mash_pressed() (FUN_8001598C twin)    */
void re15_re2z_footlock(int slot, re15_actor_t *e);  /* clip-driven walk movement              */
void re15_re2z_victim_begin(re15_actor_t *zombie, re15_actor_t *player, int behind);
void re15_re2z_victim_devour(re15_actor_t *zombie, int behind);  /* Kill-Tick-Richtung
                                                   * (dir<<8)|6 @0x80102928-50 — explizit    */
void re15_re2z_grab_anchor(re15_actor_t *e, re15_actor_t *pl, int clip);
void re15_re2z_grab_rootmotion(re15_actor_t *e);
void re15_re2z_move_root(re15_actor_t *e);        /* 0x80015e7c: per-frame clip root delta   */
void re15_re2z_se_play(int se_id);                /* ENEMSE-SE ueber den Audio-Hook (fuer den
                                                   * Frame-Flag-SFX-Pfad 0x801016c8)          */

/* ---- W2: the attack-decision ladder DECISION[1] @0x80101714 --------------------------------
 * Kept as a PURE function over an explicit gate struct: the control flow is fully verified, but
 * several gate INPUTS still have no proven producer in the port. Wiring a ladder whose gates are
 * silently zero would make the RE2 zombie only ever grab — so the flow lands (and is unit-tested)
 * before the wiring does. RE15_RE2_AI.md tracks which producers are still open. */
typedef struct {
    uint32_t dist;          /* self+0x1F0, distance to the player      @0x80101744 */
    int      arc1024;       /* re15_ai_arc_test(...,1024): 0 = INSIDE  @0x8010174c */
    int      arc512;        /* re15_ai_arc_test(...,512)               @0x80101754 */
    uint8_t  self_23e;      /* gates blocks A/B and J                  @0x80101790 */
    uint32_t self_1f4;      /* block A payload                         @0x801017a4 */
    int      a_sector_hit;  /* FUN_80015714(self, self+0x1F8, 256)==0  @0x801017d0 */
    uint8_t  self_106;      /* blocks B (!=) and G (==)                @0x80101808 */
    uint8_t  pl_106;        /*                                         @0x8010180c */
    uint8_t  pl_1d3;        /* whole byte in B/J; bit 0x80 in G        @0x8010181c */
    int16_t  self_1d4;      /* block C, mask 0xC000                    @0x80101830 */
    uint32_t self_110;      /* block C, bit 0                          @0x80101844 */
    uint16_t global_cfbf6;  /* 0x800CFBF6, masks 0x15 (D) / 0x17 (E)   @0x80101874 */
    uint16_t self_21a;      /* block G, bits 0x20 / 0x40               @0x80101928 */
    int      g1_sector_hit; /* FUN_80015758(..., yaw+256, 256)==0      @0x80101948 */
    int      g2_sector_hit; /* FUN_80015758(..., yaw-256, 256)==0      @0x8010198c */
    uint8_t  pl_8;          /* the G / J fork, tested == 15            @0x801018e8 */
    int16_t  pl_156;        /* block K, tested == -32768               @0x801019e8 */
} re15_re2z_gates_t;

typedef struct {
    int      wrote;         /* did any block store to +0x4 at all */
    uint32_t word;          /* the winning state word (last writer wins) */
    int      early_out;     /* block A returned early (j 0x80101a1c @0x801017e0) */
    int      claim_player;  /* block G set PL[0x1D3] |= 0x80 */
    int      set_10e_4000;  /* block K set self+0x10E |= 0x4000 */
    int      rng_draws;     /* 0..2 — the draw COUNT is itself behaviour */
} re15_re2z_decision_t;

int re15_re2z_decide_walk(const re15_re2z_gates_t *g, re15_re2z_decision_t *out);

/* Fill the gates from PORT state, then run the ladder. `player_claimed` is the port's equivalent of
 * PL+0x1D3 bit 0x80 (re15_player_is_grabbed). */
void re15_re2z_fill_gates(const re15_actor_t *e, const re15_actor_t *pl,
                          int player_claimed, re15_re2z_gates_t *g);
int  re15_re2z_walk_decide(const re15_actor_t *e, const re15_actor_t *pl,
                           int player_claimed, re15_re2z_decision_t *out);

#endif /* RE15_AI_FLAVOR_H */
