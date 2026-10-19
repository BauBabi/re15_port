/*
 * RE1.5 Rebuilt — Runde 39 Spur P "johnruf1041": Johns Hilferufe ab ROOM1041, Elzas Satz, Karte mit ROOM1141, Panikphase
 * (Bank 9 Bit 97) mit Leons Schlussmusik MAIN01 bis zu Johns Rettung, lauterer Ruf im Flur ROOM1131.
 * Herleitung und alle Konstanten: include/re15_johnruf_r39.h, Programme mit Listing: engine/src/gen/johnruf_r39_programme.inc
 * (tools/r39_johnruf/programme_bauen.py), Dossier analysis/befunde_runde39/P_johnruf1041.md.
 */
#include "re15_johnruf_r39.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "re15_scd.h"
#include "re15_msg.h"
#include "re15_map_hint.h"
#include "re15_menu.h"
#include "re15_audio.h"
#include "re15_engine.h"   /* g_engine.frame_count (Protokoll) */
#include "re15_tuersperre_r39.h"   /* Nachbesserung 1: Tuersperren 1041/1051 (Panikphase) und 10B1 (immer) */

#include "gen/johnruf_r39_programme.inc"

extern unsigned g_current_room_id;

/* ============================ Zustand (fluechtig; alles Dauerhafte steht in Bit 97 und den Original-Flags) ============ */
static int            s_xa_vol = RE15_R39P_XA_NORMAL;   /* 0x800d5b50; je Raum-Init 120 (FUN_80052f3c @0x80053038) */
static const uint8_t *s_karte_pc = NULL;   /* Warte-Zeile 59, deren Kartenhinweis angefordert ist (parkt) */
static uint8_t        s_ruf1131 = 0;       /* der lautere Ruf in 1131 lief in dieser Panikphase (Sitzung, Dossier 6) */
static uint8_t        s_b97_vorher = 0;    /* Flanke von Bit 97 (neue Phase -> s_ruf1131 = 0) */
static uint8_t        s_bgm_offen = 0;     /* Musikfenster offen (Form cut_10f0.c s_bgm_offen) */
static uint8_t        s_bgm_stand = 0;     /* letzte Auskunft an die Audio-Schicht: 1 = 0xFF01 geliefert */
static uint32_t       s_vm_tick_aufbau = 0;   /* g_scd.tick_count am Ende des Raumaufbaus */
static unsigned       s_zaehler[4];        /* 0 Rufe 1041, 1 Rufe 1131, 2 Kartenhinweise, 3 Rettungen */

static int flag(int bank, int bit) { return re15_game_flag_get((uint8_t)bank, (uint8_t)bit) != 0; }
static int panik(void)          { return flag(RE15_R39P_BIT_BANK, RE15_R39P_BIT_PANIK); }
static int john_verschwunden(void) { return flag(RE15_R39P_F0_BANK, RE15_R39P_F0_BIT); }

static int im_programm(const uint8_t *pc, const uint8_t *p, size_t n) { return pc && pc >= p && pc < p + n; }
static int in_meinen_programmen(const uint8_t *pc)
{
    return im_programm(pc, k_r39p_p_ruf1041, sizeof k_r39p_p_ruf1041) ||
           im_programm(pc, k_r39p_p_ruf1131, sizeof k_r39p_p_ruf1131);
}

static int programm_laeuft(const uint8_t *p, size_t n)
{
    for (int s = 0; s < SCD_THREAD_COUNT; s++) {
        const scd_thread_t *t = &g_scd.threads[s];
        if (t->active && im_programm(t->pc, p, n)) return 1;
    }
    return 0;
}

/* Wie scd_event_fire (scd_vm.c): erster freier Ereignis-Faden SCD_EVENT_SLOT_FIRST..LAST. */
static int starten(const uint8_t *p, size_t n)
{
    if (programm_laeuft(p, n)) return -1;
    for (int slot = SCD_EVENT_SLOT_FIRST; slot <= SCD_EVENT_SLOT_LAST; slot++)
        if (!g_scd.threads[slot].active && scd_thread_start(slot, p) == 0) return slot;
    return -1;
}

/* ============================ Installer ============================ */
void re15_johnruf_r39_install(uint16_t room_id)
{
    const unsigned raum = (unsigned)room_id & 0xFFFFu;
    s_xa_vol = RE15_R39P_XA_NORMAL;       /* RE2: jeder Raum-Skript-Init setzt 120 (FUN_80052f3c @0x80053014/@0x80053038) */
    s_karte_pc = NULL;
    s_vm_tick_aufbau = g_scd.tick_count;   /* NACH dem Init-Lauf, VOR jedem Spielbild-Lauf (Form cut_10f0.c) */
    re15_tuersperre_r39_install(room_id);  /* Nachbesserung 1: Texte 1041/41, 1051/40, 10B1/40 + Sperren (re15_tuersperre_r39.h) */

    if (raum == RE15_R39P_RAUM_1041) {
        re15_msg_install_text((unsigned char)RE15_R39P_MSG_SATZ, k_r39p_msg40, sizeof k_r39p_msg40);
        {
            int d = re15_msg_compute_duration(k_r39p_msg40, sizeof k_r39p_msg40, 0);
            if (d > 0 && d < 65535) re15_msg_install_durations((unsigned char)RE15_R39P_MSG_SATZ, d);
        }
        if (!john_verschwunden() || panik()) return;
        {
            const int slot = starten(k_r39p_p_ruf1041, sizeof k_r39p_p_ruf1041);
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[johnruf] F%u ROOM1041: John verschwunden ((4,0xF0)=1), Bit 97=0 -> RUF1041 %s (Faden %d)\n",
                    (unsigned)g_engine.frame_count, slot >= 0 ? "gestartet" : "NICHT gestartet", slot);
#else
            (void)slot;
#endif
        }
        return;
    }
    if (raum == RE15_R39P_RAUM_1131) {
        if (!panik() || !john_verschwunden() || s_ruf1131) return;
        {
            const int slot = starten(k_r39p_p_ruf1131, sizeof k_r39p_p_ruf1131);
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[johnruf] F%u ROOM1131 in der Panikphase -> RUF1131 %s (Faden %d)\n",
                    (unsigned)g_engine.frame_count, slot >= 0 ? "gestartet" : "NICHT gestartet", slot);
#else
            (void)slot;
#endif
        }
    }
}

/* ============================ Opcode 0x5F = RE2 Xa_vol (nur in meinen Programmen) ============================ */
int re15_johnruf_r39_op_xa_vol(scd_thread_t *t)
{
    /* PC-SCHRANKE wie rolltor_1050.c (0x62): RDT-Bytecode sieht exakt op_unknown (RE1.5-Default LAB_8003f1d8 pc+=1). */
    if (!in_meinen_programmen(t->pc)) {
        t->pc += 1;
        return 1;
    }
    {
        const unsigned v = t->pc[1];                         /* @0x80058098 lbu v1,1(v0) */
        if (v < RE15_R39P_XA_GRENZE) s_xa_vol = (int)v;      /* @0x800580a0 sltiu / @0x800580b0 sw v1,0x5b50(at) */
#ifdef RE15_PLATFORM_PC
        if (getenv("RE15_SE_DEBUG"))
            fprintf(stderr, "[johnruf] F%u Xa_vol(%u) ROOM%04X -> CD_VOL %u (Runde 39 Spur P)\n", (unsigned)g_engine.frame_count,
                    v, (unsigned)g_current_room_id, re15_johnruf_r39_xa_cdvol());
#endif
    }
    t->pc += 2;                                              /* @0x800580bc addiu v0,v0,2 */
    return 1;                                                /* @0x800580c8 addiu v0,zero,1 */
}

unsigned re15_johnruf_r39_xa_cdvol(void)
{
    /* RE2 FUN_80013ac8: Stereo L=R=vol (@0x80013ae4), Mono vol>>1 je Weg (@0x80013b08) -> relativ zum Standard 120 der
     * Faktor vol/120. Der Port-Mischer haelt den Standard als CD_VOL 0x3FFF (audio_pc.c re15_xa_init). */
    unsigned c = (unsigned)RE15_R39P_CD_VOL_VOLL * (unsigned)s_xa_vol / (unsigned)RE15_R39P_XA_NORMAL;
    return c > 0x7FFFu ? 0x7FFFu : c;
}

/* ============================ Message_on in meinen Programmen ============================ */
static void stimme(int id)
{
    scd_audio_event_t ev;
    memset(&ev, 0, sizeof ev);
    ev.kind = (uint8_t)SCD_AUDIO_VOICE_ON;          /* derselbe Weg wie scd_vm.c scd_queue_voice */
    ev.sample_id = (uint8_t)id;
    scd_audio_queue_push(&ev);
}

int re15_johnruf_r39_message_on(const uint8_t *pc)
{
    if (!in_meinen_programmen(pc)) return 0;
    {
        const int id = pc[1];
        const unsigned raum = g_current_room_id & 0xFFFFu;
        if (id >= RE15_R39P_HILFE_ID0 && id < RE15_R39P_HILFE_ID0 + RE15_R39P_HILFE_N) {
            stimme(id);
            if (raum == RE15_R39P_RAUM_1131) { s_ruf1131 = 1; s_zaehler[1]++; } else s_zaehler[0]++;
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[johnruf] F%u ROOM%04X Ruf help%02d (Sprach-Id %d, Xa_vol %d -> CD_VOL %u)\n",
                    (unsigned)g_engine.frame_count, raum, id - RE15_R39P_HILFE_ID0 + 1, id, s_xa_vol, re15_johnruf_r39_xa_cdvol());
#endif
            return 1;
        }
        if (id != RE15_R39P_MSG_KARTE) return 0;     /* msg 40: normaler Weg (Text + Stimme main40.wav) */
        if (s_karte_pc != pc) {
            const int nr = re15_map_hint_eintrag_fuer(RE15_R39P_RAUM_1041, RE15_R39P_RAUM_1141);
            if (nr < 0) return 1;                    /* kein Eintrag: weiter ohne Karte */
            re15_map_hint_request(nr);               /* RE2 Opcode 0x84 @0x800591C4: Anforderung @0x800591e8 */
            s_karte_pc = pc;
            s_zaehler[2]++;
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[johnruf] F%u ROOM1041 Kartenhinweis angefordert (Eintrag %d: ROOM1041 -> ROOM1141), Faden parkt\n",
                    (unsigned)g_engine.frame_count, nr);
#endif
            return 2;
        }
        /* parken, bis der Hinweis-Schirm angenommen UND wieder zu ist (Form Spur H, tagebuch_r38.c) */
        if (re15_map_hint_pending() >= 0 || re15_menu_map_hint_active() || re15_menu_gameplay_frozen()) return 2;
        s_karte_pc = NULL;
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[johnruf] F%u ROOM1041 Karte zu: Faden laeuft weiter (Set(9,97)=1, Steuerung zurueck)\n",
                (unsigned)g_engine.frame_count);
#endif
        return 1;
    }
}

/* ============================ Stimme: Ordner + Stamm ============================ */
void re15_johnruf_r39_stimme(unsigned room_id, int voice_id, char *reldir, size_t nd, char *stamm, size_t ns)
{
    const unsigned raum = room_id & 0xFFFFu;
    if (raum != RE15_R39P_RAUM_1041 && raum != RE15_R39P_RAUM_1131) return;
    if (voice_id < RE15_R39P_HILFE_ID0 || voice_id >= RE15_R39P_HILFE_ID0 + RE15_R39P_HILFE_N) return;
    if (!reldir || !stamm || nd < 32 || ns < 7) return;
    /* Nutzer-Dateien bleiben, wo sie liegen (VERTRAG 1.2: Zuordnung im Code): synchro/STAGE1/room1141/help0k.wav */
    snprintf(reldir, nd, "synchro/STAGE%u/room%04X/", (RE15_R39P_RAUM_1141 >> 12) & 0xFu, RE15_R39P_RAUM_1141);
    snprintf(stamm, ns, "help%02d", voice_id - RE15_R39P_HILFE_ID0 + 1);
}

/* ============================ Musik (Form cut_10f0.c: Weiche + Sperre + Anstoss) ============================ */
static int weiche(int stage, int room)
{
    if (stage != 0) return -1;                                   /* nur STAGE1 */
    if (room == RE15_R39P_BGM_RAUM_1141) return -1;              /* im Gorilla-Raum dessen eigene Musik (Leon: 11C0) */
    if (!s_bgm_offen || !panik()) return -1;
    return RE15_R39P_BGM_EINTRAG;
}

int re15_johnruf_r39_bgm_eintrag(int stage, int room)
{
    const int e = weiche(stage, room);
    s_bgm_stand = (uint8_t)(e >= 0);
    return e;
}

int re15_johnruf_r39_main_gesperrt(unsigned slot, int op, long cap_tick)
{
    if (slot != 0 || !s_bgm_stand) return 0;
#ifdef RE15_PLATFORM_PC
    if (getenv("RE15_BGM_CTL_DEBUG"))
        fprintf(stderr, "[bgm] Sce_bgm_control slot=0 op=%d in der Panikphase NICHT angewandt (Runde 39 Spur P) capTick=%ld\n",
                op, cap_tick);
#else
    (void)op; (void)cap_tick;
#endif
    return 1;
}

void re15_johnruf_r39_tick(void)
{
    re15_tuersperre_r39_tick();   /* Nachbesserung 1: Bit 97 in 1041 gestiegen -> Tuer nach 1031 Text, VOR dem AOT-Scan */
    /* Rettung: ROOM1141 sub02 @0x00C18 hat F0 geloescht (Szene mit John begonnen), sub01 @0x00C0C F2 (beide Gorillas tot,
     * @0x00C06 MAIN16 Stop) -> Panikphase vorbei, raumunabhaengig. */
    if (panik() && !john_verschwunden() && !flag(RE15_R39P_F2_BANK, RE15_R39P_F2_BIT)) {
        re15_game_flag_set(RE15_R39P_BIT_BANK, RE15_R39P_BIT_PANIK, 0);
        s_zaehler[3]++;
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[johnruf] F%u ROOM%04X John gerettet ((4,0xF0)=0, (4,0xF2)=0): Bit 97 = 0, Panikmusik aus\n",
                (unsigned)g_engine.frame_count, (unsigned)g_current_room_id);
#endif
    }
    {
        const uint8_t b97 = (uint8_t)panik();
        if (b97 && !s_b97_vorher) s_ruf1131 = 0;                 /* neue Panikphase: der 1131-Ruf steht wieder aus */
        s_b97_vorher = b97;
    }
    {
        const uint8_t war = s_bgm_offen;
        if (!panik())                                                         s_bgm_offen = 0;
        else if (!re15_cine_active() && g_scd.tick_count > s_vm_tick_aufbau) s_bgm_offen = 1;   /* nach dem Ende der Szene */
#ifdef RE15_PLATFORM_PC
        if (war != s_bgm_offen)
            fprintf(stderr, "[johnruf] F%u Panik-Musikfenster %s in ROOM%04X (Bit 97 = %d)\n", (unsigned)g_engine.frame_count,
                    s_bgm_offen ? "auf" : "zu", (unsigned)g_current_room_id, panik());
#else
        (void)war;
#endif
    }
    {
        const int stage = (int)((g_current_room_id >> 12) & 0xf) - 1;
        const int room  = (int)((g_current_room_id >> 4) & 0xff);
        const int soll  = weiche(stage, room) >= 0;
        if (soll == (int)s_bgm_stand) return;
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[johnruf] Raummusik-Anstoss in ROOM%04X: Soll %s (letzte Auskunft: %s)\n", (unsigned)g_current_room_id,
                soll ? "MAIN01" : "Tabelle", s_bgm_stand ? "MAIN01" : "Tabelle");
#endif
        if (soll) re15_audio_bgm_status_reset();   /* Skript-Latches der Tabellen-Musik gehoeren nicht auf MAIN01 (cut_10f0.c) */
        re15_audio_start_room_bgm(stage, room);
        s_bgm_stand = (uint8_t)soll;
    }
}

/* ============================ Pruefhaken ============================ */
const uint8_t *re15_johnruf_r39_programm(int nr, int *out_len)
{
    if (nr == 0) { if (out_len) *out_len = (int)sizeof k_r39p_p_ruf1041; return k_r39p_p_ruf1041; }
    if (nr == 1) { if (out_len) *out_len = (int)sizeof k_r39p_p_ruf1131; return k_r39p_p_ruf1131; }
    if (out_len) *out_len = 0;
    return NULL;
}
const uint8_t *re15_johnruf_r39_text(int *out_len) { if (out_len) *out_len = (int)sizeof k_r39p_msg40; return k_r39p_msg40; }
int  re15_johnruf_r39_xa_vol(void) { return s_xa_vol; }
int  re15_johnruf_r39_fenster(void) { return s_bgm_offen; }
int  re15_johnruf_r39_ruf1131_gespielt(void) { return s_ruf1131; }
unsigned re15_johnruf_r39_zaehler(int k) { return (k >= 0 && k < 4) ? s_zaehler[k] : 0u; }
