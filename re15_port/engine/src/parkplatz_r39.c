/*
 * RE1.5 Rebuilt — Runde 39 Spur O "parkplatz11c0": Tuersperre ROOM11C0 waehrend des Gorilla-Kampfes.
 * Herleitung, Belege und alle Konstanten: include/re15_parkplatz_r39.h; die Programme mit Listing:
 * engine/src/gen/parkplatz_r39_programme.inc (tools/r39_parkplatz/programme_bauen.py); Dossier
 * analysis/befunde_runde39/O_parkplatz11c0.md. Muster: dining_r38.c (Runde 38 F, Sperre 1141 msg 41, Anker, Messhaken).
 */
#include "re15_parkplatz_r39.h"

#include <stddef.h>
#include <string.h>

#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_msg.h"
#include "re15_room.h"
#include "re15_actor.h"
#include "re15_engine.h"           /* g_engine.frame_count (Messschrieb) */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>   /* getenv/atoi (Messhaken) */
#endif

#include "gen/parkplatz_r39_programme.inc"   /* Programme SZENE/ENDE, Text msg 40, Anker (programme_bauen.py) */

unsigned g_re15_r39o_szene_zaehler    = 0;
unsigned g_re15_r39o_ende_zaehler     = 0;
unsigned g_re15_r39o_eintritt_zaehler = 0;

static void log_f(const char *s)
{
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[parkplatz] F%u %s\n", (unsigned)g_engine.frame_count, s);   /* Spielbild fuer den Messschrieb */
#else
    (void)s;
#endif
}

/* ============================ Anker (Quellbereiche im GELADENEN RDT, FNV-1a wie dining_r38.c) ============================ */
static uint32_t fnv(const uint8_t *p, uint32_t n)
{
    uint32_t h = 0x811C9DC5u;
    for (uint32_t i = 0; i < n; i++) { h ^= p[i]; h *= 0x01000193u; }
    return h;
}

static int bereich_ok(uint32_t off, uint32_t len, uint32_t soll)
{
    const uint8_t *raw = g_room_rdt.raw;
    if (!raw || (uint32_t)g_room_rdt.raw_size < off + len) return 0;
    return fnv(raw + off, len) == soll;
}

int re15_parkplatz_r39_anker_ok(void)
{
    if ((unsigned)g_current_room_id != RE15_R39O_RAUM) return 0;
    return bereich_ok(RE15_R39O_ANKER_MAIN00_TUER_OFF, RE15_R39O_ANKER_MAIN00_TUER_LEN, RE15_R39O_ANKER_MAIN00_TUER_FNV) &&
           bereich_ok(RE15_R39O_ANKER_SUB00_03_OFF, RE15_R39O_ANKER_SUB00_03_LEN, RE15_R39O_ANKER_SUB00_03_FNV);
}

/* sub00 @0x017C8/@0x017D0: (4,64)==1 && (3,67)==0 = die Szene hat begonnen, die Ende-Szene noch nicht. */
int re15_parkplatz_r39_kampf(void)
{
    return re15_game_flag_get(RE15_R39O_ADA_BANK, RE15_R39O_ADA_BIT) &&
           !re15_game_flag_get(RE15_R39O_ENDE_BANK, RE15_R39O_ENDE_BIT);
}

/* ============================ Text / Programme ============================ */
const uint8_t *re15_parkplatz_r39_text(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_r39o_msg_sperre;
    return k_r39o_msg_sperre;
}

const uint8_t *re15_parkplatz_r39_programm(int nr, int *out_len)
{
    if (nr == 0) { if (out_len) *out_len = (int)sizeof k_r39o_p_szene; return k_r39o_p_szene; }
    if (nr == 1) { if (out_len) *out_len = (int)sizeof k_r39o_p_ende;  return k_r39o_p_ende; }
    if (out_len) *out_len = 0;
    return NULL;
}

static void meldung_einsetzen(void)
{
    re15_msg_install_text((unsigned char)RE15_R39O_MSG_SPERRE, k_r39o_msg_sperre, sizeof k_r39o_msg_sperre);
    int d = re15_msg_compute_duration(k_r39o_msg_sperre, sizeof k_r39o_msg_sperre, 0);
    if (d > 0 && d < 65535) re15_msg_install_durations((unsigned char)RE15_R39O_MSG_SPERRE, d);
}

/* ============================ Raumaufbau ============================ */
void re15_parkplatz_r39_install(uint16_t room_id)
{
    if (room_id != RE15_R39O_RAUM) return;        /* ROOM11C1 (Elza: 5 Hunde, kein Begleiter) bleibt unberuehrt */
    meldung_einsetzen();                          /* gilt an jedem Raumstart-Weg; der Teardown loescht Port-Ids wieder */
    if (!re15_parkplatz_r39_anker_ok()) {
        log_f("ROOM11C0: RDT weicht von den Ankern ab -> keine Sperre (Original-Ablauf)");
        return;
    }
    if (!re15_parkplatz_r39_kampf()) return;      /* erster Besuch ((4,64)=0): Programm SZENE sperrt; nach dem Kampf: frei */
    const re15_aot_t *a = &g_aot.slots[RE15_R39O_TUER_SLOT];
    if (!a->active || a->type != RE15_AOT_TYPE_DOOR) return;
    /* Wiedereintritt im Kampf (sub00 @0x017C8 Else / @0x017D0 Ck(3,67)==0: Gorillas neu, Ada versteckt): derselbe
     * Aot_reset wie Programm SZENE +0000 (`46 00 01 31 28 00 ff ff 00 00`), aus C wie dining_r38.c tuer_1131. */
    re15_aot_retype(RE15_R39O_TUER_SLOT, RE15_R39O_SCE_MESSAGE, RE15_R39O_SAT,
                    (uint16_t)RE15_R39O_MSG_SPERRE, (uint16_t)RE15_R39O_MASKE, 0);
    g_re15_r39o_eintritt_zaehler++;
    log_f("ROOM11C0: Kampf laeuft beim Eintritt ((4,64)=1, (3,67)=0) -> Tuer Slot 0 = Text msg 40");
}

/* ============================ Evt_exec-Haken ============================ */
const uint8_t *re15_parkplatz_r39_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *orig)
{
    if (room_id != RE15_R39O_RAUM || !orig) return orig;
    if (sub_id != RE15_R39O_SUB_SZENE && sub_id != RE15_R39O_SUB_ENDE) return orig;
    if (!re15_parkplatz_r39_anker_ok()) return orig;
    if (sub_id == RE15_R39O_SUB_SZENE) {
        /* Nur sperren, wenn die Freigabe noch kommen kann (Abnahme 0 M1). Die EINZIGE Freigabe ist sub01 @0x0182E
         * `21 03 43 00` Ck(3,67)==0 -> @0x01842 `04 ff 18 03` Evt_exec sub03 (Programm ENDE); (3,67) setzt allein sub03
         * @0x01B02 `22 03 43 01` (Zensus 240 RDTs, kein Loescher). Ist (3,67) schon 1 (Spielstand/Debug-Flags mit
         * (4,64)=0), greift sie nie mehr: Original-sub02, die Tuer bleibt die Original-Tuer. Nach sub02 @0x0184E
         * Set(4,64)=1 ist das genau "Kampf laeuft" (sub00 @0x017C8/@0x017D0) = re15_parkplatz_r39_kampf()/Installer. */
        if (re15_game_flag_get(RE15_R39O_ENDE_BANK, RE15_R39O_ENDE_BIT)) {
            log_f("ROOM11C0 sub01 @0x01824 Evt_exec sub02 bei (3,67)=1 -> keine Sperre (Freigabe sub01 @0x0182E "
                  "Ck(3,67)==0 greift nie), Original-sub02");
            return orig;
        }
        g_re15_r39o_szene_zaehler++;
        log_f("ROOM11C0 sub01 @0x01824 Evt_exec sub02 -> Programm SZENE (Tuer Slot 0 -> Text msg 40, Gosub sub02)");
        return k_r39o_p_szene;
    }
    g_re15_r39o_ende_zaehler++;
    log_f("ROOM11C0 sub01 @0x01842 Evt_exec sub03 -> Programm ENDE (Tuer Slot 0 zurueck -> ROOM11B0, Gosub sub03)");
    return k_r39o_p_ende;
}

/* Keine Stimme: msg 40 ist ein einfacher Raumtext wie Leons Tuertexte (Nachtrag 13, Orchestrator 2026-10-19). */

/* ============================ Messhaken (env-gegatet, kein Spielverhalten) ============================ */
void re15_parkplatz_r39_messhaken(void)
{
#ifdef RE15_PLATFORM_PC
    static int s_init = 0, s_starr = 0, s_n = -1, s_done = 0, s_kampf = 0;
    if (!s_init) {
        const char *e = getenv("RE15_R39O_GORILLAS_STARR"); s_starr = (e && *e && *e != '0');
        e = getenv("RE15_R39O_GORILLAS_TOT"); if (e && *e) s_n = atoi(e);
        s_init = 1;
    }
    if ((!s_starr && s_n < 0) || (unsigned)g_current_room_id != RE15_R39O_RAUM) return;
    if (!re15_parkplatz_r39_kampf() || g_scd.player_mode != 0) return;
    if (s_starr)   /* grid_id | 0x20 = Member_set 0x0C=0x30 (@0x800410b8), wie 11C0 sub02 @0x01A58 in der Szene */
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == RE15_R39O_GORILLA_TYP) g_actors[s].grid_id |= 0x20;
    if (s_n < 0 || s_done || ++s_kampf < s_n) return;
    s_done = 1;
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == RE15_R39O_GORILLA_TYP) { re15_actor_free(s); n++; }
    re15_game_flag_set(RE15_R39O_KILL_BANK, RE15_R39O_KILL_1, 1);   /* = Gorilla-Tod @0x8011b8f4 jal 0x8004ef90 */
    re15_game_flag_set(RE15_R39O_KILL_BANK, RE15_R39O_KILL_2, 1);
    fprintf(stderr, "[parkplatz] F%u MESSHAKEN RE15_R39O_GORILLAS_TOT: %d Gorillas heraus, Flags (7,0x60)/(7,0x61) = 1\n",
            (unsigned)g_engine.frame_count, n);
#endif
}
