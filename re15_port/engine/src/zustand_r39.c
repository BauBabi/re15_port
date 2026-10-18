/* zustand_r39.c — Runde 39 Spur F "zustand" (Dossier analysis/befunde_runde39/F_zustand.md).
 *
 * 1. GIFT (Beta -> Retail): RE1.5 zieht im Kommando-1-Prolog alle 121 Bilder 2 HP ab (@0x80031e44-ecc,
 *    Runde 38 spinnen_r38.c), RE2 Retail im Spieler-Takt FUN_8003bfac alle 31 Bilder 1 HP bis HP 1 —
 *    selbst disassembliert (RE2 PSX.EXE Leon, ghidra_re2_Leon.txt):
 *      8003c130 lhu v0,-0x1f0(v0)   = DAT_800cfe10 (Gift-Wort, Op 32 schreibt 0x1518 @0x80020130)
 *      8003c138 beq v0,zero,8003c184                 kein Gift -> Ende
 *      8003c144 lbu v1,-0x1ee(v1)   = DAT_800cfe12 (Takt-Zaehler)
 *      8003c14c addiu v0,v1,0xff ; 8003c154 sb v0,-0x1ee(at)   Zaehler - 1 (u8)
 *      8003c158 bne v1,zero,8003c184 ; Delay 8003c15c li v0,0x1e   lief noch -> Ende
 *      8003c164 sb v0,-0x1ee(at)                     Zaehler := 30
 *      8003c168 lh v0,0x156(s0) ; 8003c174 slti v0,v0,0x2 ; 8003c178 bne   HP < 2 -> Ende
 *      8003c17c addiu v0,v1,-0x1 ; 8003c180 sh v0,0x156(s0)       HP := HP - 1
 *    Periode also 31 Bilder (30 .. 0, Abzug bei 0). Op 32 (Giftkugel der Spinne) setzt den Zaehler
 *    auf 30: `li v1,0x1e` @0x80020138 / `sb v1,-0x1ee(at)` @0x80020140 -> erster Abzug 31 Bilder nach
 *    dem Treffer. Keine weiteren Stores im Takt (kein Kommando-/Zustandswort, kein SE): der Takt ist
 *    KEIN Treffer. Gift-Zustand im Port = RE1.5-Bit 0x2 von 0x800acaec (status_flags; Gegengift/Blau
 *    loeschen es @0x8004af8c) — RE2 fragt nur `!= 0` ab (Statusschirm FUN_80069538 @0x80069544,
 *    Heilung @0x8006b1e8 `sh zero`, Skript Poison_ck/Poison_clr @0x8005930c/@0x800591a8).
 * 2. KLEIDUNG (NUTZER-VORGABE): volle Heilung -> ganze Kleidung. Im Original loescht Heilung nichts
 *    (blood_decals.md: Writer auf 0x800b10ec nur Builder FUN_80037c1c + Akku FUN_80037edc); erst der
 *    Spieler-Lade-Pfad FUN_800314b0 baut Tabelle (FUN_80037c1c) und Haut (FUN_8004ee78, Slot 0x17
 *    @0x800315b0/@0x80031814; Builder-Aufrufe @0x800316c8/@0x800318cc) neu. FORM: "vollstaendig" = HP >= 100 (`sh s0`, s0 = 100 @0x8004ae14, der Wert des
 *    Heilsprays) + Westen-Bonus des Ports; Ausloeser = der Heil-Applier (Tabelle @0x80010fbc, Port
 *    re15_item_use_apply), Wiederherstellung = Tabelle nullen + Haut neu laden (= Spieler-Laden).
 * 3. EKG: RE1.5-Farbtabelle @0x8007622c hat fuer Gift (Eintrag 3) 20 ff 20 = dasselbe Gruen wie FINE
 *    (gemessen, Dossier 2.1). RE2 FUN_800754ac faerbt die Spur je Zustand aus @0x800ab160 (6 Bytes je
 *    Zustand: r g b, dr dg db; Linie = base - step * i), POISON = Eintrag 4 @0x800ab178: ff 20 ff / 08 01 08.
 *
 * Messschiene (env, kein Spielverhalten):
 *   RE15_R39_GIFT_AT=<bild>   setzt im genannten Spielbild das Gift (Bit 0x2 + Zaehler 30 wie Op 32)
 *   RE15_R39_LOG=<datei>      je Spielbild: F<n> aktiv hp status z15 z2 cond menu abzug
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_zustand_r39.h"
#include "re15_engine.h"
#include "re15_inv_screen.h"
#include "re15_menu.h"
#include "re15_spinnen_r38.h"
#include "re15_scd.h"        /* re15_vest_hp_bonus */
#include "re15_damage.h"     /* re15_wound_reset / re15_wound_level */

/* ---- Gift ---------------------------------------------------------------------------------------- */
static uint8_t  s_takt;                /* RE2 DAT_800cfe12 */
static unsigned s_takte;
static int      s_env_init;
static long     s_gift_at = -1;
static FILE    *s_log;

static void env_init(void)
{
    if (s_env_init) return;
    s_env_init = 1;
    const char *g = getenv("RE15_R39_GIFT_AT");
    if (g && *g) s_gift_at = atol(g);
    const char *l = getenv("RE15_R39_LOG");
    if (l && *l) s_log = fopen(l, "w");
}

void re15_zustand_r39_gift_setzen(void) { s_takt = 30; }           /* @0x80020138-40 */
void re15_zustand_r39_reset(void) { s_takt = 0; s_takte = 0; }
uint8_t  re15_zustand_r39_gift_zaehler(void) { return s_takt; }
unsigned re15_zustand_r39_gift_takte(void)   { return s_takte; }

int re15_zustand_r39_spieler_takt(re15_actor_t *pl, int aktiv)
{
    if (!pl) return 0;
    env_init();
    if (aktiv && s_gift_at >= 0 && (long)g_engine.frame_count == s_gift_at) {
        pl->status_flags |= 0x2u;
        re15_zustand_r39_gift_setzen();
        fprintf(stderr, "[r39-zustand] F%u Messhaken: Gift gesetzt (status 0x%04X, Zaehler 30)\n",
                (unsigned)g_engine.frame_count, (unsigned)pl->status_flags);
    }
    int abzug = 0;
    if (aktiv && (pl->status_flags & 0x2u)) {                     /* @0x8003c130-38 (Gift != 0) */
        uint8_t v1 = s_takt;                                      /* @0x8003c144 */
        s_takt = (uint8_t)(v1 + 0xffu);                           /* @0x8003c14c-54 */
        if (v1 == 0) {                                            /* @0x8003c158 */
            s_takt = 0x1e;                                        /* @0x8003c15c-64 */
            if (pl->hp >= 2) {                                    /* `slti v0,v0,0x2 / bne` @0x8003c174-78 */
                pl->hp = (int16_t)(pl->hp - 1);                   /* @0x8003c17c-80 */
                abzug = 1;
                s_takte++;
            }
        }
    }
    if (s_log) {
        fprintf(s_log, "F%u aktiv=%d hp=%d status=0x%04X z15=%u z2=%u cond=%d menu=%d abzug=%d\n",
                (unsigned)g_engine.frame_count, aktiv, (int)pl->hp, (unsigned)pl->status_flags,
                (unsigned)re15_spinnen_r38_gift_zaehler(), (unsigned)s_takt,
                re15_inv_screen_condition(pl->hp, (pl->status_flags & 2u) ? 1 : 0),
                re15_menu_is_open(), abzug);
        fflush(s_log);
    }
    return abzug;
}

/* ---- Kleidung ------------------------------------------------------------------------------------ */
static int s_haut_neu;

int re15_zustand_r39_hp_voll(void) { return 100 + re15_vest_hp_bonus(); }   /* s0 = 100 @0x8004ae14 */

int re15_zustand_r39_heilung(uint8_t item_id)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (pl->hp < re15_zustand_r39_hp_voll()) return 0;
    int wunden = 0;
    for (int p = 0; p < 8; p++) if (re15_wound_level(p) > 0 || re15_wound_acc(p) > 0) wunden = 1;
    re15_wound_reset();                 /* = Wund-Builder FUN_80037c1c (Level + Akku := 0) */
    s_haut_neu = 1;                     /* = Haut-Upload des Spieler-Ladens FUN_8004ee78 (Slot 0x17) */
    fprintf(stderr, "[r39-zustand] Heilung 0x%02X: HP %d >= %d voll -> Wund-Tabelle genullt%s, Spielerhaut neu\n",
            (unsigned)item_id, (int)pl->hp, re15_zustand_r39_hp_voll(), wunden ? " (hatte Wunden)" : "");
    return 1;
}
int re15_zustand_r39_haut_neu_pending(void) { return s_haut_neu; }
int re15_zustand_r39_haut_neu_abholen(void) { int p = s_haut_neu; s_haut_neu = 0; return p; }

/* ---- EKG ----------------------------------------------------------------------------------------- */
int re15_zustand_r39_ekg_farbe(int cond, int i, uint8_t *r, uint8_t *g, uint8_t *b)
{
    if (cond != 3 || i < 0 || i > 31) return 0;                   /* FUN_8004ed6c: 3 = Gift */
    /* RE2 @0x800ab178: ff 20 ff, Schritt @0x800ab17b: 08 01 08 (FUN_800754ac `base - step * i`) */
    *r = (uint8_t)(0xff - 8 * i);
    *g = (uint8_t)(0x20 - i);
    *b = (uint8_t)(0xff - 8 * i);
    return 1;
}
