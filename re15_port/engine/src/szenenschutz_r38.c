/*
 * szenenschutz_r38.c — Runde 38 Spur B "szenenschutz". Kopf, Belege und Begruendung: include/re15_szenenschutz_r38.h,
 * Dossier analysis/befunde_runde38/B_szenenschutz.md.
 *
 * (A) Solange die Szene den Spieler haelt (g_scd.player_mode == 2), sieht jeder Gegner in seinem FREIEN Gehirn
 *     (+0x4 == 1) den Spieler durch die Original-Tore als "nicht angreifbar": Spieler+0x93 Bit 0 (RE1.5, 0x800acae7)
 *     und PL+0x1D3 & 0x80 (RE2, Port re15_player_is_grabbed). Die Sicht gilt NUR waehrend des Ticks dieses einen
 *     Gegners (Haken in re15_enemy_ai_run_all); danach steht wieder der echte Wert. Skript-Rollen (+0x4 != 1, z.B. die
 *     1171-Kraehe in FLIGHT-2) ticken mit dem echten Wert — ihr Szenen-Angriff laeuft wie bisher.
 * (B) RE2-Hund: Angriffs-Commits ruhen, solange die RE1.5-Freigabe-Sperrzeit +0x1d6 (= dog_atk_cd, 0x78 aus
 *     @0x801115e8-0x801115ec) laeuft; Abzug je ACTIVE-Bild wie @0x8010dd5c-0x8010dd70.
 */
#include <stdint.h>
#include <stdio.h>
#include "re15_szenenschutz_r38.h"
#include "re15_actor.h"
#include "re15_scd.h"        /* g_scd.player_mode */
#include "re15_engine.h"     /* g_engine.frame_count (Protokoll) */
#include "re15_ai_flavor.h"  /* re15_ai_re2_for_type (RE2-Kraehe) */
#include "re15_fenster1120.h" /* re15_re2crow_zwang_ist (Fensterkraehe ROOM1120 laeuft immer ueber das RE2-Gehirn) */

extern int re15_player_victim_state(void);   /* enemy_ai_common.c — Opfer-FSM des Spielers */

/* ------------------------------------------------------------------------------------------------------------ */
/* (A) Szenenschutz                                                                                              */
/* ------------------------------------------------------------------------------------------------------------ */

static int      s_aktiv;            /* dieser KI-Durchlauf laeuft unter Szenenschutz                         */
static int      s_sicht;            /* der gerade tickende Gegner sieht den Spieler als "nicht angreifbar"   */
static int      s_slot = -1;        /* zuletzt angemeldeter Akteur (-1 = keiner offen)                       */
static int      s_slot_geschuetzt;  /* ... und ob die Sicht fuer ihn galt                                    */
static uint8_t  s_hr_echt;          /* echter Spieler+0x93 (hit_react) — die Sicht setzt nur Bit 0 dazu     */
static int16_t  s_hp_vor;           /* Spieler-HP vor dem geschuetzten Tick (Sicherheitsnetz)                */
static int      s_opfer_vor;        /* Opfer-FSM vor dem geschuetzten Tick (nur Protokoll)                   */
static int      s_war_aktiv;        /* Flanke an/aus fuer das Protokoll                                      */
static uint32_t s_ticks, s_abgefangen, s_fenster_ticks;

int re15_szenenschutz_haelt(void)
{
    /* "Halten" = eine Szene fuehrt den Spieler UND der Port ignoriert das Pad (Nutzer: "wo er sich noch nicht mal
     * bewegen kann"). Massgeblich ist das Bewegungs-Gate des Ports (player_common.c re15_player_tick:
     * `g_scd.player_mode != 2 && !msg_block`) — Nachbesserung 1, Mangel M2 der Abnahme 0.
     * (1) player_mode 2 = "skript-gefuehrt, Pad wird ignoriert"; gesetzt AUSSCHLIESSLICH aus cine_active =
     *     flag(1,27) || flag(2,7) (platform/pc/main.c, Block "BL-round 2026-05-29"), inklusive der 15 Letterbox-Bilder
     *     ohne Plc_ret. Das Original haelt den Spieler ueber Set(2,7) = Bit 0x01000000 (Pad-Aufbereitung
     *     @0x800304f4 `lw aca40` / @0x800304f8 `lui v1,0x100` / @0x80030500 `beq`).
     * (2) Spieler+0x4 == 4 (Plc-Executor: Plc_motion @0x80041bb0 / Plc_dest @0x80041c14 `sb 4`, frei erst Plc_ret
     *     @0x80041f90) haelt NUR, solange der Port das Pad wirklich sperrt = das Nachrichten-Tor des Bewegungs-Gates
     *     (re15_player_msg_sperre). Im Original laeuft der Spieler-Dispatcher bei +0x4 == 4 ueber den Executor
     *     @0x80073f90[4] = 0x80030660 statt ueber den Pad-Zweig [1] = 0x80031de8 (@0x80031c8c `lbu v1,aca58` /
     *     @0x80031cac `lw v0,0(at)` / @0x80031cb4 `jalr`); der Port bindet sein Bewegungs-Gate NICHT an +0x4 (vorbestehend,
     *     v0.8.25 gleich; gemessen ROOM1230 sub21: pst=4, pm=0, Leon laeuft 2100 Einheiten) — dort ist Leon im Port
     *     beweglich und darum nicht gehalten. Dossier "Nachbesserung 1" N1.M2. */
    extern int re15_player_msg_sperre(void);   /* player_common.c — Nachrichten-Tor des Bewegungs-Gates */
    if (g_scd.player_mode == 2) return 1;
    return g_actors[RE15_ACTOR_SLOT_PLAYER].state == 4 && re15_player_msg_sperre();
}

int re15_szenenschutz_gegen(const re15_actor_t *e)
{
    if (!e || !e->active) return 0;
    if (e->type >= 0x40) return 0;          /* NPC-Typen (0x40..0x4d) greifen den Spieler nie an. Grenze 0x40 = die
                                             * Gegner/NPC-Trennung des Originals, PSX.EXE 0x800509e4 ("naechster
                                             * lebender Gegner"): @0x80050a70 `lbu v0,-146(s0)` (+0x8 Typ) /
                                             * @0x80050a78 `sltiu v0,v0,0x40` / @0x80050a7c `beq v0,zero,0x80050ae4`
                                             * (Typ >= 0x40 uebersprungen); Port enemy_ai_common.c re15_npc_nearest_enemy */
    if (e->state != 1) return 0;            /* nur das FREIE Gehirn (+0x4 == 1 ACTIVE); Skript-Zustaende (Hund 4/5/6,
                                             * Kraehe 4 FLIGHT-2, NPC-Executor 4) sind Szenen-Rollen */
    if (e->type == 0x21 && (e->grid_id & 0x40))
        return 0;                           /* Kraehe mit Skript-Marke: INIT-Weiche grid&0x40 -> FLIGHT-2
                                             * (@0x80112408) = die 1171-Szenenkraehe, auch unter dem RE2-Gehirn */
    return 1;
}

/* Den offenen Akteur abschliessen: nach einem geschuetzten Tick den echten Spielerwert zurueck (und HP-Abzug
 * zurueckgeben), nach einem ungeschuetzten Tick dessen Wirkung als neuen echten Wert uebernehmen. */
static void abschliessen(void)
{
    if (s_slot < 0) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (s_slot_geschuetzt) {
        if (pl->hp < s_hp_vor) {
            fprintf(stderr, "[szenenschutz] F%u Treffer abgefangen: Slot %d Typ 0x%02X zog %d HP ab (Spieler bleibt %d)\n",
                    (unsigned)g_engine.frame_count, s_slot, (unsigned)g_actors[s_slot].type,
                    (int)(s_hp_vor - pl->hp), (int)s_hp_vor);
            pl->hp = s_hp_vor;
            s_abgefangen++;
        }
        if (re15_player_victim_state() != s_opfer_vor)
            fprintf(stderr, "[szenenschutz] F%u WARNUNG: Slot %d Typ 0x%02X aenderte die Opfer-FSM %d -> %d\n",
                    (unsigned)g_engine.frame_count, s_slot, (unsigned)g_actors[s_slot].type,
                    s_opfer_vor, re15_player_victim_state());
        pl->hit_react = s_hr_echt;
    } else {
        s_hr_echt = pl->hit_react;          /* Szenen-Rolle durfte treffen: ihr Ergebnis ist ab jetzt echt */
    }
    s_slot = -1;
    s_sicht = 0;
}

void re15_szenenschutz_durchlauf_beginn(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_slot  = -1;
    s_sicht = 0;
    s_aktiv = pl->active && pl->hp >= 0 && re15_szenenschutz_haelt();
    s_hr_echt = pl->hit_react;
    if (s_aktiv != s_war_aktiv) {
        if (s_aktiv) {
            s_fenster_ticks = 0;
            fprintf(stderr, "[szenenschutz] F%u an (Szene haelt den Spieler)\n", (unsigned)g_engine.frame_count);
        } else {
            fprintf(stderr, "[szenenschutz] F%u aus (%u geschuetzte Gegner-Ticks)\n",
                    (unsigned)g_engine.frame_count, (unsigned)s_fenster_ticks);
        }
        s_war_aktiv = s_aktiv;
    }
}

/* RE2-Kraehe (EMOVL21_S0.BIN) bereits im Angriffs-Anflug — Sub 11 Anlauf (Strike-Commit bei Kontakt @0x80101F2C
 * `jal 0x80104088` a1=12) oder Sub 13 Hover-Harass (Griff-Commit @0x80102444 `jal 0x80104088` a1=14): keiner der beiden
 * Commits liest ein Spieler-Tor. Abbruch ueber den ORIGINAL-Listener 0x80104400: @0x80104410 `lhu v0,554(s0)` /
 * @0x80104418 `andi v0,v0,0x10` / @0x8010441c `beq` -> Vol 350 (@0x80104424-28), +0x220 = rand&0x9d (@0x80104434-3c),
 * Sub 4 (@0x80104444/@0x80104450), +0x21C = 30 (@0x8010444c-54), +0x22A &= 0xffa7 (@0x80104460-64). Die DECs von
 * Sub 11 (@0x80101D7C `jal 0x80104400`) und Sub 13 (@0x80102244 `jal 0x80104400`) rufen ihn im selben Bild VOR dem
 * EXEC (DEC->EXEC-Doppel-Dispatch @0x801005F8-69C) — der Commit kommt also nicht mehr zum Zug. Sub 14 (Griff laeuft
 * schon) bleibt unberuehrt: sein Listener (@0x80102484) wuerde den Griff ohne Opfer-Freigabe abbrechen. */
static void kraehe_re2_abbruch(int slot)
{
    re15_actor_t *e = &g_actors[slot];
    if (e->type != 0x21 || e->state != 1) return;
    if (e->sub_state_1 != 11 && e->sub_state_1 != 13) return;
    if (!re15_ai_re2_for_type(0x21u) && !re15_re2crow_zwang_ist(slot)) return;   /* nur das RE2-Gehirn */
    e->re2c_flags22a |= 0x10u;                       /* Abbruch-Bit 0x10 = Broadcast 16 (0x8010452C-Form) */
}

void re15_szenenschutz_gegner(int slot)
{
    abschliessen();
    if (!s_aktiv || slot <= RE15_ACTOR_SLOT_PLAYER || slot >= RE15_ACTOR_MAX) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_slot = slot;
    s_slot_geschuetzt = re15_szenenschutz_gegen(&g_actors[slot]);
    if (s_slot_geschuetzt) {
        s_hp_vor    = pl->hp;
        s_opfer_vor = re15_player_victim_state();
        pl->hit_react = (uint8_t)(s_hr_echt | 1u);   /* +0x93 Bit 0: "gerade getroffen" (Tor @0x8010e5c4,
                                                      * @0x80012e24) */
        s_sicht = 1;                                 /* PL+0x1D3&0x80 ueber re15_player_is_grabbed */
        s_ticks++;
        s_fenster_ticks++;
        kraehe_re2_abbruch(slot);
    } else {
        pl->hit_react = s_hr_echt;
        s_sicht = 0;
    }
}

void re15_szenenschutz_durchlauf_ende(void)
{
    abschliessen();
    if (s_aktiv) g_actors[RE15_ACTOR_SLOT_PLAYER].hit_react = s_hr_echt;
    s_sicht = 0;
}

int re15_szenenschutz_sicht(void) { return s_sicht; }

void re15_szenenschutz_zaehler(uint32_t *ticks, uint32_t *abgefangen)
{
    if (ticks) *ticks = s_ticks;
    if (abgefangen) *abgefangen = s_abgefangen;
}

void re15_szenenschutz_zaehler_reset(void)
{
    s_ticks = 0; s_abgefangen = 0; s_fenster_ticks = 0;
    s_aktiv = 0; s_war_aktiv = 0; s_sicht = 0; s_slot = -1;
}

/* ------------------------------------------------------------------------------------------------------------ */
/* (B) Freigabe-Sperrzeit des 1190-Sprung-Ausgangs im RE2-Hundegehirn                                            */
/* ------------------------------------------------------------------------------------------------------------ */

int re15_r38_hund_sperrzeit(const re15_actor_t *e)
{
    /* RE1.5 ATTACK-RANGE FUN_8010e568: @0x8010e578 `lh v0,470(v0)` / @0x8010e580 `bne v0,zero,0x8010e628`
     * -> solange +0x1d6 != 0 kein Biss-Entscheid. Unter dem RE2-Gehirn schreibt nur der RE1.5-Sprung-Ausgang
     * (re15_dog_state456 Phase 4, @0x801115e8-0x801115ec) diesen Wert; der Raumaufbau nullt alle Akteure
     * (scd_room_setup.c memset), ein frischer RE2-Hund hat also 0. */
    return e && e->dog_atk_cd != 0;
}

void re15_r38_hund_sperrzeit_takt(re15_actor_t *e)
{
    /* ACTIVE-Schwanz FUN_8010dbcc: @0x8010dd5c `lh v0,470(a0)` / @0x8010dd64 `beq v0,zero` / @0x8010dd6c
     * `addiu v0,v1,-1` / @0x8010dd70 `sh v0,470(a0)` — einmal je ACTIVE-Bild. Der Aufrufer steht im ACTIVE-Zweig
     * (re15_re2dog_tick case 1, hinter dem Sub-Dispatch wie der Schwanz hinter @0x8010dcf0). */
    if (e && e->dog_atk_cd) e->dog_atk_cd--;
}
