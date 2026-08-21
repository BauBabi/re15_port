/*
 * room_common.c — SHARED cross-room transition state + logic.
 *
 * The cross-room door transition is the SAME on PSX and PC; only the asset I/O
 * (RDT load = CD vs file, render reset = VRAM/BG vs texcache, BG decode = MDEC
 * vs file) is architecture-specific and supplied by each port through callbacks
 * in re15_room_apply_ctx_t. This file holds the cross-room state globals + the
 * request queue + the apply logic so the two main loops cannot drift.
 *
 * (Split out of the PSX-only re15_room.c — which now keeps ONLY the CD loader —
 * so the PC build can link the same transition; see re15_room.h.)
 */
#include <stdint.h>
#include "re15_room.h"
#include "re15_rdt.h"
#include "re15_actor.h"
#include "re15_enemy.h"   /* re15_enemy_reset on room change */
#include "re15_enemy_ai.h"    /* re15_player_cmd_reset — Kommandoregister @0x8001CBDC/@0x80031518 */
#include "re15_savepoint.h"   /* re15_savepoint_reset on room change */
#include "re15_itembox.h"     /* re15_itembox_reset on room change (box pending) */
#include "re15_scd.h"
#include "re15_camera.h"
#include "re15_light.h"
#include "re15_msg.h"
#include "re15_collision.h"
#include "re15_stair.h"
#include "re15_climb.h"   /* re15_climb_reset — Kletter-FSM (Substate 9/10) */
#include "re15_audio.h"
#include "re15_fade.h"        /* re15_fade_config/kick/done — die Transitions-Blende */

/*=========================================================================
 * SELBSTHEILENDER KAMERA-APPLY — byte-true FUN_8002137c @0x800214dc-0x80021514
 * (Vergleich + Dirty) und FUN_80021bbc @0x80021be0-0x80021bfc (Apply-Kopf).
 * Die vollstaendige Adress-/Schreiberliste steht am Prototyp in re15_scd.h.
 *
 * Das Original bewertet in JEDEM Bild neu, ob der ANGEZEIGTE Cut (work_vars[0x0A],
 * DAT_800b0fe4) noch dem ANGEFORDERTEN (DAT_800afbb5) entspricht, und setzt das
 * Dirty-Flag DAT_800b5457 so lange jedes Bild neu, bis der Apply nachgezogen hat.
 * Genau daran haengt der Kamerawechsel beim Durchlaufen eines Raums: der
 * RVD-Zonen-Scan FUN_80014230 schreibt ueber FUN_800142f4 @0x80014300 NUR
 * DAT_800afbb5 und bewaffnet NICHTS. Ohne diesen Vergleich gibt es keinen
 * Zonen-Kamerawechsel.
 *=======================================================================*/
int re15_cam_present_tick(void)
{
    /* @0x800214f8 `lh v1,DAT_800b0fe4` / @0x80021500 `lbu v0,DAT_800afbb5` /
     * @0x80021508 `beq v1,v0,LAB_80021518` / @0x80021514 `sb 1,DAT_800b5457`.
     * (Das Gate @0x800214e8 `andi DAT_800aca3c,0x80` hat im Port kein Gegenstueck —
     * Bit 0x80 der Flag-Bank 1 wird von keinem Port-Subsystem gesetzt; sobald es das
     * tut, gehoert es hier davor.) */
    if ((int)g_scd.work_vars[0x0A] != (int)g_scd.cam_id)
        g_scd.cam_change_pending = 1;

    /* @0x80021538 `beq v0,zero,LAB_80021568` — nicht dirty, kein Apply. */
    if (!g_scd.cam_change_pending)
        return 0;

    /* Apply-Kopf FUN_80021bbc: @0x80021bf4 `sh v0,DAT_800b0fe8` (work_vars[0x0C] =
     * alter angezeigter Cut) und @0x80021bfc `sh v1,DAT_800b0fe4` (work_vars[0x0A] =
     * jetzt angezeigter = angeforderter Cut). Danach @0x80021618 `sb zero,DAT_800b5457`. */
    g_scd.work_vars[0x0C] = g_scd.work_vars[0x0A];
    g_scd.work_vars[0x0A] = (int16_t)g_scd.cam_id;
    g_scd.cam_change_pending = 0;
    return 1;
}

/*=========================================================================
 * RAUM-TRANSITIONS-PRAESENTATION — byte-true Auszug der Transitions-FSM
 * FUN_8001c958 (Zustands-Byte DAT_800b5359, Sprungtabelle @0x8001069c;
 * selbst nachdisassembliert 2026-08-17):
 *
 *   table[0]=0x8001c9c8 (State 1)  table[1]=0x8001ca98 (2)  table[2]=0x8001cbb8 (3)
 *   table[3]=0x8001cc34 (State 4)  table[4]=0x8001cc70 (State 5)
 *   Dispatch: @0x8001c994 `lbu v0,DAT_800b5359` / @0x8001c99c `addiu v1,v0,-1` /
 *             @0x8001c9a0 `sltiu v0,v1,0x5` / @0x8001c9b0 `addiu at,at,1692` (=0x8001069c) /
 *             @0x8001c9c0 `jr v0`.
 *
 * SCHARFMACHEN (die Tuer setzt aca3c|0x8000, der Frame-Kopf zieht nach):
 *   @0x8001cda0 `andi v0,v1,0x8000` / @0x8001cda4 `beq -> Subsysteme`
 *   @0x8001cdb0 `lbu DAT_800acae7` (+0x93) / @0x8001cdb8 `bne -> Subsysteme`
 *   @0x8001cdc4 `sw aca3c|0x40`  @0x8001cdd0 `sb 1,DAT_800b5359`
 *   @0x8001cdd8-e8 `lw g_pauseflags; lui v1,0xff00; or; sw`  ← ARM-FREEZE
 *
 * STATE 1 (Tuer-Zweig): @0x8001ca20-2c `aca38|=0x10000`, @0x8001ca30
 *   `jal FUN_80021634(1,0)`, @0x8001ca44 `sw zero,g_pauseflags`, @0x8001ca48-50
 *   `aca38|=0x4000`, @0x8001ca54 `jal FUN_8001d600` (RAUMLADER, SYNCHRON) —
 *   dort @0x8001d82c-34 `FUN_80021634(2,0)` = MODE-2-SCHWARZ, die beiden
 *   Warteschleifen @0x8001d850-868 (`aca38 & 0x10000`) und @0x8001dabc-d4
 *   (`aca38 & 0x2000000`), Freigabe @0x8001dadc-ec (`FUN_80021634(0,0)` +
 *   `DAT_800b5457 = 1`) — dann `j 0x8001cbbc` in den State-3-Rumpf.
 *
 * STATE-3-RUMPF (die EINBLENDUNG) @0x8001cbb8-cc28:
 *   @0x8001cbb8 `ori a0,0x200`   (ch 0, abr 2)
 *   @0x8001cbbc `addiu a1,zero,-6144`  (Schritt -0x1800)
 *   @0x8001cbc0 `ori a2,0x7`     (rgb-Maske 7)
 *   @0x8001cbcc `lw t1,g_pauseflags`      (alter Wert)
 *   @0x8001cbdc `sb zero,0x800aca58`      ← SPIELER-KOMMANDOWORT = 0
 *   @0x8001cbe4 `sw 7,g_pauseflags`
 *   @0x8001cbfc `sw t1,DAT_8008f628`      (Sicherung der FSM, NICHT DAT_800b853c)
 *   @0x8001cc00 `jal FUN_800217b0`        = re15_fade_config(0,2,7,-0x1800,0)
 *   @0x8001cc18 `jal FUN_800216ec`        = re15_fade_kick(0,0)  -> level 0x7fff
 *   @0x8001cc20-28 `sb 4,DAT_800b5359`
 *
 * STATE 4 @0x8001cc34-6c:  DAT_800b5359 = 5 (@0x8001cc48-50);
 *   `DAT_8008f628 |= g_pauseflags` (@0x8001cc58/@0x8001cc64);
 *   `g_pauseflags |= 0xff000000` (@0x8001cc5c `lui v0,0xff00` + @0x8001cc6c `sw`)
 *   ← DIE LOGIK STEHT WAEHREND DER GANZEN EINBLENDUNG (Spieler 0x80000000,
 *     AI 0x20000000, Anim/Action 0x10000000, SCD 0x02000000, Pad 0x01000000).
 *
 * STATE 5 @0x8001cc70-94:  `jal FUN_8002178c(0)` (Blende fertig?) / `beq v0,zero`
 *   -> weiter warten; sonst `sb zero,DAT_800b5359` (@0x8001cc8c) +
 *   `sw DAT_8008f628,g_pauseflags` (@0x8001cc84/@0x8001cc94) = FREIGABE.
 *
 * Dauer der Einblendung ist damit AUS DEN BYTES gerechnet, nicht gemessen:
 * level 0x7fff, Schritt -0x1800 -> 0x7fff,0x67ff,0x4fff,0x37ff,0x1fff,0x07ff,
 * dann Unterlauf auf 0xefff (Bit15 = fertig) = 6 gezeichnete Frames, Freigabe
 * im 7. Poll.
 *
 * OFFEN (bewusst NICHT mit einer Zahl gefuellt): die beiden Lader-Warteschleifen
 * @0x8001d850-868 / @0x8001dabc-d4 halten im Original zusaetzlich so lange an,
 * wie das CD-Laufwerk braucht — das ist Hardware-Zeit und KEINE Konstante. Der
 * Port laedt synchron aus dem Dateisystem, dort vergeht dabei kein Frame.
 *=========================================================================*/

/* DAT_8008f628 — der EIGENE Sicherungsplatz der Transitions-FSM. Bewusst NICHT
 * g_re15_pauseflags_saved (= DAT_800b853c): das gehoert dem Message-System, und
 * beide Freezes koennen sich ueberlappen. */
static uint32_t s_trans_pause_saved = 0;
/* DAT_800b5359 — Zustands-Byte der Transitions-FSM (0 = keine Transition laeuft). */
static uint8_t  s_trans_state = 0;

void re15_room_transition_present(void)
{
    /* State 1 @0x8001ca44: der Raumwechsel loescht die Pause-Flags KOMPLETT (und
     * damit auch einen haengenden Message-Freeze — "kein Bit bleibt stehen"). */
    re15_pauseflags_clear();
    /* State-3-Rumpf, in der Reihenfolge der Instruktionen. */
    s_trans_pause_saved = g_re15_pauseflags;          /* @0x8001cbcc lw -> @0x8001cbfc sw */
    /* @0x8001cbdc `sb zero,0x800aca58` = Spieler+0x4 auf Kommando 0; der cmd-0-Handler
     * @0x800318f8 raeumt auf und setzt das Wort noch im selben Durchlauf auf 1
     * (@0x8003192c `sw v0(=1),0x800aca58`). Der Port hat keinen cmd-0-Tick, also
     * direkt die 1 — exakt wie re15_room_apply_pending es fuer die Tuer schon tut. */
    g_actors[RE15_ACTOR_SLOT_PLAYER].state       = 1;
    g_actors[RE15_ACTOR_SLOT_PLAYER].sub_state_1 = 0;   /* @0x80031974-Umfeld / cmd-0-INIT */
    g_actors[RE15_ACTOR_SLOT_PLAYER].sub_state_2 = 0;
    g_re15_pauseflags = 7;                             /* @0x8001cbc4 ori 0x7 + @0x8001cbe4 sw */
    re15_fade_config(0, 2, 7, (int16_t)-0x1800, 0);    /* FUN_800217b0(0x200,-6144,7,0) @0x8001cc00 */
    re15_fade_kick(0, 0);                              /* FUN_800216ec(0,0,..) @0x8001cc18 */
    s_trans_state = 4;                                 /* @0x8001cc20-28 */
}

int re15_room_transition_active(void) { return s_trans_state != 0; }

int re15_room_transition_tick(void)
{
    if (s_trans_state == 4) {                          /* Handler @0x8001cc34 */
        s_trans_state        = 5;                      /* @0x8001cc48-50 */
        s_trans_pause_saved |= g_re15_pauseflags;      /* @0x8001cc58 or + @0x8001cc64 sw */
        g_re15_pauseflags   |= 0xFF000000u;            /* @0x8001cc5c lui 0xff00 + @0x8001cc6c sw */
        return 1;
    }
    if (s_trans_state == 5) {                          /* Handler @0x8001cc70 */
        if (!re15_fade_done(0)) return 1;              /* @0x8001cc70 jal FUN_8002178c(0) + @0x8001cc78 beq */
        s_trans_state     = 0;                         /* @0x8001cc8c sb zero,DAT_800b5359 */
        g_re15_pauseflags = s_trans_pause_saved;       /* @0x8001cc84 lw + @0x8001cc94 sw */
        return 0;
    }
    return 0;
}

void re15_room_transition_reset(void)
{
    s_trans_state       = 0;
    s_trans_pause_saved = 0;
}

/* Cross-room state — the parsed RDT of the resident room + the queued change.
 * (Moved here from re15_room.c so BOTH ports link it.) The actual RDT BYTES
 * live in each port's loader buffer; g_room_rdt just holds the parsed pointers. */
re15_rdt_t          g_room_rdt;
int                 g_room_rdt_ok    = 0;
unsigned            g_current_room_id = 0x1170;   /* room resident in g_room_rdt */
re15_room_change_t  g_room_change    = {0};       /* queued cross-room transition */

void re15_room_request_change(unsigned room_id, int32_t x, int32_t y, int32_t z,
                              int16_t yaw, int target_cut)
{
    g_room_change.pending    = 1;
    g_room_change.room_id    = room_id;
    g_room_change.x = x; g_room_change.y = y; g_room_change.z = z;
    g_room_change.yaw_4096   = yaw;
    g_room_change.target_cut = target_cut;
}

int re15_room_apply_pending(const re15_room_apply_ctx_t *c)
{
    if (!g_room_change.pending) return 0;
    g_room_change.pending = 0;
    re15_savepoint_reset();   /* new room: drop any stale save pending/latched-cut from the old room */
    /* GLOBALE PAUSE-FLAGS beim Raumwechsel loeschen — byte-true: die Transitions-FSM schreibt
     * `sw zero,-13760(at)` auf 0x800aca40 an ZWEI Stellen, @0x8001ca44 und @0x8001caec. Ohne
     * das koennte ein Freeze, dessen Text der Raumwechsel wegraeumt, nie mehr aufgehoben werden
     * (der Restore-Pfad haengt am Message-Dismiss). */
    re15_pauseflags_clear();
    re15_itembox_reset();     /* likewise the box-screen pending (save-phone precedent) */
    g_scd_self_reenter_fired = 0;  /* new room: re-arm the intro self-reenter latch (a genuine
                                    * later re-entry must be able to run its own door scenario) */

    /* (1) ARCH: load the destination ROOM####.RDT (CD on PSX / file on PC) into
     * g_room_rdt. Abort the transition if it fails (player stays put). */
    if (c->load_rdt(g_room_change.room_id) != 0) return 0;

    /* (2) PER-RAUM-TEARDOWN — VOR dem Re-Aliasing, denn ab hier zeigt *c->rdt auf den neuen Raum
     * und jede Aufraeum-Entscheidung, die noch den alten Zustand braucht, kaeme zu spaet.
     *
     * REIHENFOLGE NACH DEM ORIGINAL: FUN_800396FC gibt die Arena frei (@0x80039738 setzt den
     * Bump-Pointer 0x800AC77C auf die Basis 0x800AC780) und laedt die neue RDT ab genau dieser
     * Adresse (@0x80039740/@0x800397E8) — Freigabe steht also VOR der Installation, nicht danach.
     * Der Port kann das Laden nicht davor schieben (Schritt 1 bricht bei Ladefehler ab und laesst
     * den Spieler stehen — diese Semantik bleibt), aber der Teardown gehoert vor das Re-Aliasing.
     * Frueher lief er als Schritt (3) NACH dem Alias und auf PC ausserdem als leerer Rumpf. */
    c->reset_render();

    /* (3) Re-alias the port's resident RDT to the freshly-parsed new room. */
    *c->rdt    = g_room_rdt;
    *c->rdt_ok = 1;

    /* (3b) ARCH: load the destination room's Obj_model_set prop set into the now-
     * reset prop VRAM (data-driven per room; clears the previous room's prop slots).
     * Ports without per-room props leave this NULL. */
    if (c->load_props) c->load_props(g_room_change.room_id);

    /* (4) Wipe the previous room's NPCs/props (re-seeds slot 0 = player); the
     * new room's SCD re-spawns its own via Obj_model_set. (scd_room_reenter
     * memset-clears g_scd props below; NOT done here as that path is also the
     * intro self-reentry, which must keep the cast.) */
    /* SPIELER-GESUNDHEIT UEBER DEN RAUMWECHSEL RETTEN — re15_actor_init() memset-t ALLE Slots und
     * setzt Slot 0 danach auf hp=100 (actor_common.c). Damit heilte der Port den Spieler an JEDER
     * Tuer voll und loeschte Gift/Blutung; g_actors[0].hp ist die echte Spieler-HP (der Todes-Check
     * re15_damage.c liest sie).
     *
     * DAS ORIGINAL TUT DAS NICHT: der Raumwechsel wischt NUR den Entity-Pool — FUN_8001A4C0 nullt
     * 20 Slots ab 0x800ACC2C mit Stride 0x1F4, und zwar ausschliesslich deren Feld +0x00
     * (@0x8001A4E8). Der SPIELERBLOCK liegt bei 0x800ACA54 und damit AUSSERHALB dieses Pools. Die
     * einzige HP-Schreibstelle im gesamten Raumwechsel-Pfad ist @0x80031718 (sh 0x64 -> +0x9A) in
     * FUN_800314B0, und die ist hinter dem Charakterwechsel-Gate @0x80039760-8C eingesperrt: sie
     * feuert nur, wenn (0x800ACA5C & 0x0F) != Etage 0x800B0FF0 — also beim Modellwechsel, NICHT bei
     * einem gewoehnlichen Raumwechsel. Dasselbe gilt fuer das Statuswort +0x98 (@0x80031720,
     * Bit 0x2 = Gift/Blutung).
     *
     * Deshalb: Slot 0 durch die Neuinitialisierung schicken (Hitbox, Root-Motion-Tracker, Typ/Flags
     * kommen sauber zurueck), aber die beiden Felder wiederherstellen, die das Original nicht
     * anfasst. */
    const int16_t  keep_hp     = g_actors[RE15_ACTOR_SLOT_PLAYER].hp;
    const uint16_t keep_status = g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags;
    re15_actor_init();
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp           = keep_hp;
    g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags = keep_status;

    /* SKRIPT-ANTRIEB DES SPIELERS BEENDEN — das fehlende Gegenstueck zum Original.
     *
     * Die Transitions-FSM nullt das Kommandoregister des Spielers: @0x8001CBDC 'sb zero,0x800ACA58'
     * (= Spieler +0x04, State 0). Der State-0-Handler @0x800318F8 (Tabelle @0x80073F90, Eintrag 0)
     * raeumt danach auf: Motion +0x94 = 0 (@0x80031924), +0x95 = 0 (@0x80031954), +0x1C4 = 0
     * (@0x8003197C), und setzt State auf 1 (@0x8003192C).
     *
     * WARUM DAS WICHTIGER IST ALS ES AUSSIEHT: genau dieser Reset macht im Original den
     * Clip-Index-Ueberlauf UNERREICHBAR. Das Original prueft nirgends gegen eine Clip-Anzahl —
     * Plc_motion schreibt den Index ungeprueft (@0x80041BA8 sb a1,148(v0)), und die drei Leser von
     * +0x94 (@0x8001F324, @0x8001F8E4, @0x8001AE48) rechnen EDD_base + motion*4 roh. Eine
     * Clip-Anzahl existiert im EDD nicht einmal als Feld. Der Ueberlauf ist im Original also nur
     * deshalb harmlos, WEIL der Raumwechsel Motion und State vorher auf 0 setzt.
     *
     * Der Port hatte dafuer kein Gegenstueck (scd_room_setup.c mappt nur die Port-Sentinels) und
     * fing den Ueberlauf stattdessen mit einem MODULO-Wrap ab (anim_select_common.c) — ein Netz,
     * das das Original nicht hat. Mit diesem Reset ist der Fall gar nicht mehr erreichbar. */
    {
        re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        p->motion      = 0;    /* +0x94 @0x80031924 */
        p->anim_frame  = 0;    /* +0x95 @0x80031954 */
        p->anim_flags  = 0;    /* +0x1C4 @0x8003197C */
        p->sub_state_1 = 0;
        p->sub_state_2 = 0;
        p->sub_state_3 = 0;
        p->state       = 1;    /* @0x8003192C — Routine 0 laeuft durch, State steht danach auf 1 */
        /* ...und das ROUTINEN-Register selbst. @0x8001CBDC nullt 0x800ACA58 (Spieler+0x04)
         * UNBEDINGT, also landet der Spieler nach JEDEM Raumwechsel wieder in Routine 0/1 =
         * Gameplay (Tabelle @0x80073F90). g_scd.player_mode ist das Port-Gegenstueck dieser
         * Routine (2 = SCRIPTED, Pad wird ignoriert) und blieb bisher ueber den Raumwechsel
         * stehen: wer waehrend einer Cutscene den Raum wechselte, behielt einen Spieler, der
         * keine Eingabe mehr annimmt. Gemessen beim Sprung aus dem Pre-Intro nach ROOM1130:
         * pmode blieb 2, frozen=0, motion=0 — der Spieler stand fest, obwohl nichts ihn hielt.
         * (Verwandt mit dem Irons-Freeze: dort war es ein Thread, hier das Routinen-Register.) */
        g_scd.player_mode = 0;
        /* (Der CUTSCENE-LATCH flag(2,7), aus dem main.c player_mode jeden Frame NEU ableitet, wird
         * im Raum-(Re)Load selbst geloescht — scd_room_reenter, neben der Bank-1-Maske. Dort liegt
         * der gemeinsame Engpass ALLER drei Raumwechsel-Aufrufer.) */
        /* ...und die Kommandoregister-Statics, die ausserhalb von g_actors leben (Knockdown-Phase,
         * Hit-Flinch, Event-Reach). Derselbe WORT-Store @0x8001CBDC / @0x80031518 raeumt sie im
         * Original mit weg; hier angehaengt an den bestehenden Block, damit die Reihenfolge stimmt. */
        re15_player_cmd_reset();
    }
    re15_enemy_reset();   /* drop the previous room's loaded enemy models (free PC bufs);
                           * the new room lazy-loads its own on first spawn. */

    /* (4b) ARCH: load the dest room's CINEMATIC overlay (rbj) + special NPC (Elliot) into the
     * per-room arena — data-driven per room (PSX; PC keeps its boot-loaded copies). Runs AFTER
     * reset_render (arena reset + Leon's skel re-pointed to PL00 base) and the enemy reset, so
     * it overlays from the clean base. NULL for ports/rooms without one. */
    if (c->load_cinematic) c->load_cinematic(g_room_change.room_id);

    /* (5) Re-point the camera cut table at the new room + set the ENTRY cut the
     * door specified (Door_aot_set dest_cut) — that cut frames the door spawn.
     * RVD zones then switch cuts as the player walks. */
    *c->active_cuts      = g_room_rdt.cuts;
    *c->active_cut_count = g_room_rdt.cut_count;
    int cut = g_room_change.target_cut;
    if (cut >= *c->active_cut_count) cut = *c->active_cut_count - 1;
    if (cut < 0) cut = 0;
    *c->cam_active_cut = cut;

    /* (6) Drop the player at the door's destination spawn (AFTER the actor wipe). */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = g_room_change.x;
    pl->y = g_room_change.y;
    pl->z = g_room_change.z;
    pl->rot_y = g_room_change.yaw_4096;

    /* (7) Re-init the SCD VM for the new room (main00+sub00 of the new RDT). This runs
     * main00 THROUGH its spawned sub's first yield — so a room whose main00 fires an
     * Evt_exec that immediately issues a Cut_chg has that Cut_chg queued here.
     *
     * entry_scenario = work_vars[0x0A] = der EINTRITTS-CUT der durchschrittenen Tuer.
     * byte-true FUN_8001d600 (Raum-Betreten): `DAT_800afbb5 = *(byte*)(DAT_800ac9a8+10)`
     * @0x8001d930 `lbu v1,10(a0)` und @0x8001d948 `sh v1,4068(at)` -> work_vars[0x0A].
     * DAT_800ac9a8 zeigt auf den ZIEL-Block der Tuer (= Door_aot_set pc+14: x,y,z,yaw,
     * stage@+8, room@+9, cut@+10, band@+0xb) — also exakt g_room_change.target_cut
     * (op_door_aot_set liest denselben Cut aus pc[24] = (pc+14)[10]).
     * Vorher stand hier hart 0, d.h. sub00's `Switch(work_vars[0x0A])` bekam game-weit
     * immer Fall 0 (z.B. ROOM1090: immer Cut_chg 8, nie Cut_chg 0x0B fuer Eintritt via
     * Cut 3 aus ROOM1050). Der Selbst-Raum-Pfad in game_step_common.c uebergab den
     * target_cut bereits korrekt — jetzt tun es beide. */
    scd_room_reenter(c->rdt, g_room_change.x, g_room_change.z,
                     (uint8_t)g_room_change.target_cut);

    /* (8) Camera precedence (byte-true, RE'd 2026-07-22 from the ROOM1170 pre-intro trace):
     * the new room's SCD init may have issued its OWN Cut_chg — e.g. ROOM1170 main00 ->
     * Evt_exec(0x180B)=sub11 (the pre-intro narrator) -> Cut_chg(7), whose cut 7 is a VOID
     * camera that renders the black narrator backdrop. The ORIGINAL runs main00 AFTER the door
     * cut, so that Cut_chg OVERRIDES the door entry cut and also turns the RVD auto-scan OFF
     * (op_cut_chg -> cut_auto_enabled=0). The port previously ALWAYS dropped the SCD's cut
     * (cam_change_pending=0) + forced cut_auto=1 below, so the CAM_SWITCH scan picked the
     * player's helipad zone and the narration read over the helipad instead of the black cut 7.
     * FIX: when the SCD init queued a DIFFERENT cut, HONOR it (keep cam_change_pending + the
     * cut_auto=0 it set); only otherwise force the door's entry cut (frames the teleported
     * player) + enable the gameplay auto-camera (steps 8b/12 below). */
    int scd_queued_cut = (g_scd.cam_change_pending && (int)g_scd.cam_id != cut)
                       ? (int)g_scd.cam_id : -1;
    if (scd_queued_cut >= 0) {
        cut = scd_queued_cut;          /* the scripted cut is authoritative; leave cam_change_pending=1
                                        * so the main-loop cut block applies it + loads its BG */
        *c->cam_active_cut = cut;
    } else {
        g_scd.cam_change_pending = 0;  /* no SCD cut -> the door's entry cut frames the spawn */
    }
    re15_camera_build_view(&(*c->active_cuts)[cut], c->cam_view);

    /* (9) ARCH: stream the entry cut's BG now (the per-cut load only fires on a
     * cut CHANGE; we just set cam to the entry cut after that block ran). */
    c->load_bg_cut(cut);

    /* (10) Per-room lighting: re-parse the new room's NCCT light set + apply. */
    if (c->rdt->lights &&
        re15_light_parse(c->rdt->lights, (size_t)c->rdt->lights_size,
                         &g_re15_room_lights) == 0) {
        g_re15_room_lights_ok = 1;
        re15_light_apply_cut(&g_re15_room_lights, cut);
    }

    /* (11) Per-room messages: interpret the new room's RDT message table. */
    re15_msg_load_room_block(c->rdt->messages, c->rdt->messages_size);

    /* (12) A door entry is GAMEPLAY → enable the RVD/CAM_SWITCH auto-camera
     * (scd_room_reenter memset-cleared g_scd.cut_auto). NOT when the SCD init locked a
     * scripted cut (its Cut_chg set cut_auto=0): re-enabling the scan would let it override
     * the scripted narrator cut 7 with the player's zone (the ROOM1170 pre-intro bug). */
    if (scd_queued_cut < 0)
        g_scd.cut_auto_enabled = 1;

    /* (13) New room floor band from the door spawn Y (-(Y/0x708)). */
    re15_collision_set_band(re15_collision_band_from_y(pl->y));

    /* (14) No half-finished stair traversal carries across rooms. */
    re15_stair_reset();
    /* (14b) dito fuer den Kletter-FSM (Substate 9/10) + das Standobjekt DAT_800ac788. */
    re15_climb_reset();

    /* (15) Per-room BGM (SS_BGMTBL): switch to the new room's track.
     *
     * STAGE WAR AUF 0 FESTGENAGELT, obwohl sie in der Raum-ID steckt. Im Original indiziert
     * FUN_80044210 die Tabelle mit BEIDEN Werten: Stage aus 0x800B0FE0 (@0x80044218) und Raum aus
     * 0x800B0FE2 (@0x80044244), Zeiger = base + tab[stage]*2 + room*2 (@0x8004425C/@0x80044264).
     * Mit stage=0 griffen alle STAGE2-6-Raeume in die STAGE1-Scheibe der Tabelle. Die Stage steckt
     * in der Raum-ID genau so, wie die PSX-Seite sie schon herausrechnet (platform/psx/main.c). */
    re15_audio_start_room_bgm((int)((g_room_change.room_id >> 12) - 1),
                              (int)((g_room_change.room_id >> 4) & 0xff));

    /* (16) Raum-Sound-Baenke neu binden (snd0 Schritte + snd1 Raum-SE). Im Original macht das
     * der Raumlader FUN_800396fc per FUN_80043eac (@0x80043eac, VH aus RDT+0xc / VB aus RDT+0x10)
     * und FUN_80043fb0 (@0x80043fb0, VB aus RDT+0x1c) — beide schliessen erst die Vorgaengerbank.
     * Beide Baenke sind aus dem RDT geschnitten, gelten also nur fuer DIESEN Raum. */
    re15_audio_load_room_banks();

    return 1;
}
