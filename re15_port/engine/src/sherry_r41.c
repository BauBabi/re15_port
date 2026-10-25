/*
 * RE1.5 Rebuilt — Runde 41 Spur K "stage3szenen_b": Sherry als Spielfigur (Modell 0x0F). Belege: include/re15_sherry_r41.h.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_sherry_r41.h"
#include "re15_scd.h"
#include "re15_engine.h"
#include "re15_room.h"
#include "re15_actor.h"

extern int  re15_player_equipped_weapon(void);   /* re15_damage.c (DAT_800aca5d) */
extern void re15_player_waffe_roh(int id);       /* re15_damage.c, Runde 41 Spur K: aca5d ohne Ausruest-Platz */
extern void re15_wound_reset(void);              /* re15_damage.c (FUN_80037c1c) */

void re15_sherry_r41_messhaken(void)
{
    static int s_init = 0, s_done = 0;
    static long s_bild = -1;
    static unsigned s_raum = 0;
    static const char *s_spec = NULL;
    if (!s_init) {
        s_init = 1;
        const char *e = getenv("RE15_WORKVAR_AT");
        if (e && *e) {
            const char *at = strrchr(e, '@');
            if (at) {
                s_bild = atol(at + 1);
                const char *hs = strchr(at, '#');
                if (hs) s_raum = (unsigned)strtoul(hs + 1, NULL, 16);
                s_spec = e;
            }
        }
    }
    if (!s_spec || s_done) return;
    if ((long)g_engine.frame_count < s_bild) return;
    if (s_raum != 0 && g_current_room_id != s_raum) return;
    s_done = 1;
    const char *p = s_spec;
    while (*p && *p != '@') {
        char *q = NULL;
        long idx = strtol(p, &q, 0);
        if (q == p) break;
        p = q;
        while (*p == ':' || *p == ' ') p++;
        long val = strtol(p, &q, 0);
        if (q == p) break;
        p = q;
        if (idx >= 0 && idx < 256) {
            g_scd.work_vars[idx] = (int16_t)val;
            fprintf(stderr, "[workvar-at] Bild %u Raum %04X: work_vars[0x%02lX] = %ld\n",
                    (unsigned)g_engine.frame_count, g_current_room_id, idx, val);
        }
        while (*p == ',' || *p == ' ') p++;
    }
}

static int s_modell = -1;

/* Waffe je Modell, DEBUG.BIN-Tabelle @0x800c00d4 (DEBUG.BIN resident ab 0x800c0000, Datei-Offset 0xd4: `01` x15, `00`;
 * Leser FUN_800c00a8: `lbu t1,-13732(t1)` (aca5c) / `addiu t0,t0,212` / `addu t0,t0,t1` / `lbu v0,0(t0)`, Aufruf aus
 * FUN_800314b0 `jal 0x800c00a8` @0x80031680, Speicher `sb v0,-13731(at)` (aca5d) @0x80031690). Fuer 0x0F zusaetzlich
 * `sb zero,-13731(at)` @0x800316c4 (hinter `bne v1,v0(=0xf)` @0x800316b8). Laufzeit-RAM @0x800c00a8.. = DEBUG.BIN
 * bytegleich (Original-Spielstand lauf2_30c1.sav, Nachbesserung 1). */
static const uint8_t k_debug_waffe[16] = { 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1, 0 };

int  re15_sherry_r41_modell(void) { return s_modell; }

void re15_sherry_r41_modell_init(int modell)
{
    s_modell = modell & 0x0F;
}

int re15_sherry_r41_ruestungswechsel(int alt, int neu)
{
    alt &= 0x0F; neu &= 0x0F;
    if (alt == neu) return 1;
    /* gleiche Figur = beide Leon-Varianten 0..3 oder beide Elza-Varianten 4..7 (Waffen-Basis @0x800741e8: 76 x4 / 97 x4;
     * Figuren-Bit 2 der Modell-Nibble `andi v1,v1,0x4` @0x80012088). Marvin 0x0D und Sherry 0x0F sind eigene Figuren. */
    return (alt <= 7 && neu <= 7 && ((alt & 4) == (neu & 4))) ? 1 : 0;
}

void re15_sherry_r41_modell_setzen(int modell)
{
    int alt = s_modell;
    int w0 = re15_player_equipped_weapon(), hp0 = (int)g_actors[RE15_ACTOR_SLOT_PLAYER].hp;
    unsigned st0 = (unsigned)g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags;
    s_modell = modell & 0x0F;
    if (alt >= 0 && !re15_sherry_r41_ruestungswechsel(alt, s_modell)) {
        /* FIGUREN-WECHSEL (Leon <-> Marvin ROOM3020 @0x00D7A / ROOM3060 @0x010D2, Elza <-> Sherry ROOM3061 @0x011F6 /
         * ROOM3091 @0x01D9C): Schwanz von FUN_800314b0 wie im Original, bei JEDEM Neuladen (Runde 41 Spur K,
         * Nachbesserung 1 M2 - vorher PORT-WAHL "vorige Waffe zurueck, HP bleiben"):
         *   Waffe   := @0x800c00d4[Modell]   jal 0x800c00a8 @0x80031680, sb aca5d @0x80031690, 0x0F -> 0 @0x800316c4
         *   Wunden  := 0                     jal 0x80037c1c @0x800316c8
         *   HP      := 100                   `ori v0,zero,0x64` @0x80031710, `sh v0,-13586(at)` (+0x9a) @0x80031718
         *   Status  := 0                     `sh zero,-13588(at)` (+0x98, Bit 0x2 = Gift) @0x80031720
         * CORE-/ARMS-Bank (@0x800316e8 / @0x80031708) und Koerper (@0x8003154c/@0x80031578) laedt das PC-Modul. */
        re15_player_waffe_roh((int)k_debug_waffe[s_modell]);
        re15_wound_reset();
        /* HP := 100 ist im Original die Vollheilung. Die Port-Weste (Nutzer-Auftrag 2026-08-29: Modell 1 = +5 Hoechst-
         * energie, scd_room_setup.c re15_vest_hp_bonus; "voll" = 100 + Bonus wie item_use_common.c/zustand_r39.c) gilt
         * auch hier: Wechsel IN Modell 1 -> 105. Der Westen-Spiegel wird nachgezogen, damit re15_vest_hp_on_model_reload
         * (laeuft danach, @0x80039760-Stelle) den Figurwechsel nicht als An-/Ablegen liest (sonst Leon(1) -> Marvin 95). */
        g_actors[RE15_ACTOR_SLOT_PLAYER].hp = (int16_t)(100 + ((s_modell == 1) ? re15_vest_hp_bonus() : 0));
        re15_vest_model_mark((int16_t)s_modell);
        g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags = 0;
    }
    fprintf(stderr, "[sherry-r41] Spielermodell %d -> %d, Waffe (aca5d) %d, HP %d, Status %04x (vorher Waffe %d, HP %d, "
            "Status %04x)\n", alt, s_modell, re15_player_equipped_weapon(), (int)g_actors[RE15_ACTOR_SLOT_PLAYER].hp,
            (unsigned)g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags, w0, hp0, st0);
}

int re15_sherry_r41_waffe_gesperrt(void) { return s_modell == RE15_SHERRY_R41_MODELL; }

/* Zeile 15 der vier Tabellen [Tempo, Drehung], Byte-Index 15*2 = +30 (aus info/Re1.5/PSX.EXE gelesen):
 *   GEHEN  @0x80073ec2 = 32 30 -> [50,48]   Leser Tempo @0x800324d4, Drehung (+1) @0x800323a0
 *   RUECK  @0x80073ee2 = 23 30 -> [35,48]   Leser Tempo @0x80032d5c, Drehung (+1) @0x80032c4c
 *   DREHEN @0x80073f02 = 00 50 -> [0,80]    Leser Drehung (+1) @0x80032a08
 *   RENNEN @0x80073f42 = 78 40 -> [120,64]  Leser Tempo @0x800327e8, Drehung (+1) @0x80032688
 * (Zum Vergleich Zeile 0 Leon: [75,48] [70,48] [0,96] [200,72]; Zeile 4 Elza: [70,48] [65,48] [0,96] [210,72].) */
static const uint8_t s_zeile15[4][2] = { { 50, 48 }, { 35, 48 }, { 0, 80 }, { 120, 64 } };

int re15_sherry_r41_tabelle(int tab, int feld, int standard)
{
    if (s_modell != RE15_SHERRY_R41_MODELL) return standard;
    if (tab < 0 || tab > 3 || feld < 0 || feld > 1) return standard;
    return (int)s_zeile15[tab][feld];
}
