/*
 * birkinfilm_r42.c — Runde 42 Strang "birkinfilm": Leons erstes Treffen mit Annette (ROOM6000 sub06) mit Rahmenzeilen und dem
 * RE2-Film "Birkins Mutation". Herleitung, Ablauf und Belege: include/re15_birkinfilm_r42.h und der Kopf von
 * tools/r42_birkinfilm/szene_bauen.py (erzeugt gen/birkinfilm_r42.inc). Film und Blenden: engine/src/zws_r41.c (Tabellenzeile
 * "birk6000", Blendenart RE15_ZWS_BLENDART_4090).
 *
 * PORT-STORY. Kein Asset-Patch: der Port tauscht nur das Ziel des Evt_exec sub06 (sub00 @0x00F58), wenn sub06 im geladenen RDT
 * bytegleich k_bf_6000_sub06 ist (Form story6_r42.c umleiten, hier die ganzen 316 Bytes).
 */
#include <stdint.h>
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "re15_birkinfilm_r42.h"
#include "re15_scd.h"
#include "re15_msg.h"
#include "re15_engine.h"

#include "gen/birkinfilm_r42.inc"

extern unsigned g_current_room_id;

#ifdef RE15_PLATFORM_PC
#define BF_LOG(...) do { fprintf(stderr, "[birkinfilm] F%u ", (unsigned)g_engine.frame_count); fprintf(stderr, __VA_ARGS__); \
                         fputc('\n', stderr); } while (0)
#else
#define BF_LOG(...) do { } while (0)
#endif

unsigned g_re15_bf_umleitungen = 0;
unsigned g_re15_bf_warten = 0;
static unsigned s_warte = 0;

/* ==== Texte (Haken in re15_msg_load_room_block nach den RDT-Texten, Form story6_r42.c) ==================================== */
const uint8_t *re15_birkinfilm_r42_text(uint8_t id, int *out_len)
{
    for (int i = 0; i < BF_TEXTE_N; i++)
        if (k_bf_texte[i].id == id) {
            if (out_len) *out_len = k_bf_texte[i].n;
            return k_bf_texte[i].b;
        }
    if (out_len) *out_len = 0;
    return NULL;
}

void re15_birkinfilm_r42_texte(uint16_t room_id)
{
    if (room_id != RE15_BF_RAUM) return;   /* nur Leons ROOM6000; ROOM6001 (Elza) hat keine Annette-Szene */
    for (int i = 0; i < BF_TEXTE_N; i++) {
        re15_msg_install_text(k_bf_texte[i].id, k_bf_texte[i].b, (size_t)k_bf_texte[i].n);
        const int d = re15_msg_compute_duration(k_bf_texte[i].b, k_bf_texte[i].n, 0);   /* wie re15_msg_load_room_block */
        if (d > 0 && d < 65535) re15_msg_install_durations(k_bf_texte[i].id, d);
    }
    s_warte = 0;
    BF_LOG("ROOM6000: %d Rahmenzeilen eingesetzt (msg 40..42)", BF_TEXTE_N);
}

/* ==== Programm ============================================================================================================ */
const uint8_t *re15_birkinfilm_r42_programm(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_bf_6000_prog;
    return k_bf_6000_prog;
}

const uint8_t *re15_birkinfilm_r42_evt_exec(uint16_t room_id, uint8_t sub, const uint8_t *pc)
{
    if (room_id != RE15_BF_RAUM || sub != RE15_BF_SUB || !pc) return pc;
    if (memcmp(pc, k_bf_6000_sub06, sizeof k_bf_6000_sub06) != 0) {
        BF_LOG("ROOM6000 sub06 weicht von der Signatur ab - Original-Ablauf ohne Film");
        return pc;
    }
    g_re15_bf_umleitungen++;
    BF_LOG("ROOM6000 Evt_exec sub06 -> Programm (%u B): Rahmenzeilen 40/41, Film R409BIRK, Rahmenzeile 42 vor msg 3",
           (unsigned)sizeof k_bf_6000_prog);
    return k_bf_6000_prog;
}

/* ==== Warte-Zeile 43 (RE2 Wsleep @0x027E2) ================================================================================ */
int re15_birkinfilm_r42_message_on(const uint8_t *pc)
{
    if (!pc || (uint16_t)g_current_room_id != RE15_BF_RAUM || pc[1] != RE15_BF_MSG_WARTEN) return 0;
    /* Der Stimmen-Riegel davor (scd_vm.c) hat das Ende von Xa 0x11 abgewartet. PORT-WAHL: auch den Untertitel abwarten. */
    if (g_scd.message_active && s_warte < RE15_BF_WARTEN_NOTBREMSE) { s_warte++; return 2; }
    g_re15_bf_warten = s_warte;
    BF_LOG("Warte-Zeile 43: Stimme und Untertitel zu Ende (%u Takte auf den Untertitel)%s", s_warte,
           s_warte >= RE15_BF_WARTEN_NOTBREMSE ? " NOTBREMSE" : "");
    s_warte = 0;
    return 1;
}

/* ==== Stimmen ============================================================================================================= */
const char *re15_birkinfilm_r42_stimme(unsigned room_id, int voice_id, int *lese)
{
    if ((room_id & 0xFFFFu) != RE15_BF_RAUM) return NULL;
    const char *d = NULL;
    int l = 0;
    switch (voice_id) {
        case RE15_BF_MSG_XA09: d = RE15_BF_XA09_DATEI; l = RE15_BF_XA09_LESE; break;
        case RE15_BF_MSG_XA11: d = RE15_BF_XA11_DATEI; l = RE15_BF_XA11_LESE; break;
        case RE15_BF_MSG_XA14: d = RE15_BF_XA14_DATEI; l = RE15_BF_XA14_LESE; break;
        default: return NULL;
    }
    if (lese) *lese = l;
    return d;
}
