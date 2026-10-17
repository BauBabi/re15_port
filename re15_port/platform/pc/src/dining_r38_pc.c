/*
 * dining_r38_pc.c — Runde 38 Spur F "dining1141": PC-Seite der drei Port-Baenke (engine/src/dining_r38.c,
 * include/re15_dining_r38.h; Dossier analysis/befunde_runde38/F_dining1141.md).
 *
 *   0x3B  Rolladen = die snd0-Bank von ROOM1131.RDT (RDT-Kopf +0x08 EDT / +0x0C VH / +0x10 VB, FUN_80045024 Bank 2
 *         -> `lw a0,0x8` @0x80045138 = RDT-snd0), gelesen aus dem ausgelieferten CD-Baum (kein Asset-Patch).
 *         Saetze 0x0A/0x0B/0x0C = VAG 4/5/8 (rolltor_sound.md 4.4, Pitch -41/-40/-36 Halbtoene ueber note2pitch2).
 *   0x3C  Klopfen  = RE2 ROOM1060 snd0, bytegleich als shared_assets/RE2/KLOPF1060.EDT/.VH/.VB
 *         (tools/r38_dining/re2_export.py), Saetze 0x0A/0x0B.
 *   0x3D  Hilferuf = Johns Sprachzeile synchro/STAGE1/room1141/main42.wav ("Help me out.", Nutzer-Stimme; der Nutzer
 *         darf die Datei ersetzen) ueber den SPRACH-Pfad (audio_pc.c re15_voice_play: Pegel-Boden 45 % Vollaussteuerung,
 *         XA-Strom wie jede synchro-Zeile), NICHT als SE: gemessen (Lauf m1, Kreuzkorrelation tools/r38_dining/
 *         ton_mitschnitt.py + Pegel) lag die alte Ein-Ton-SE-Bank mit Verstaerkung 0,23-0,25 im Mix (se_play_layers:
 *         vol = (127 * 0x4000 / 127) >> 1 = 0x2000 = 0,25) und damit ~12 dB unter den Dialogzeilen (main40 im selben
 *         Lauf 1,04) und unter der Raummusik — der Hilferuf waere kaum zu hoeren gewesen.
 * 0x3B/0x3C gespielt ueber dieselbe Stimmen-Maschine wie jede SE (re15_audio_se_play_bank -> se_play_layers).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_audio.h"
#include "re15_vab.h"
#include "re15_dining_r38.h"
#include "re15_engine.h"
#include "asset_root_pc.h"

extern long re15_audio_cap_tick(void);   /* audio_pc.c (Runde 38 Spur F): Mitschnitt-Takt */
extern void re15_audio_se_play_bank(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                    const int *decoded_len, int satz);   /* audio_pc.c */

typedef struct {
    int        state;                         /* 0 ungeprueft, 1 geladen, -1 fehlt */
    re15_vab_t vab;
    uint8_t   *edt;
    int        edt_count;
    int16_t   *decoded[RE15_VAB_MAX_SAMPLES];
    int        decoded_len[RE15_VAB_MAX_SAMPLES];
} bank_t;

static bank_t s_bank[3];
static unsigned s_gespielt[3];

static void vags_dekodieren(bank_t *b, const uint8_t *vb, uint32_t vb_size)
{
    for (int i = 0; i < b->vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {
        uint32_t off = b->vab.samples[i].offset, sz = b->vab.samples[i].size;
        if (sz == 0 || off + sz > vb_size) continue;
        size_t cap = (sz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        b->decoded[i]     = pcm;
        b->decoded_len[i] = re15_vag_adpcm_decode(vb + off, sz, pcm, cap);
    }
}

static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

/* 0x3B: snd0 aus ROOM1131.RDT (RDT-Kopf +0x08/+0x0C/+0x10). */
static int lade_rolladen(bank_t *b)
{
    int sz = 0;
    uint8_t *rdt = re15_pc_read_cd(RE15_R38F_ROLLADEN_RDT, &sz);
    if (!rdt || sz < 0x40) { free(rdt); return -1; }
    uint32_t e = rd32(rdt + 0x08), h = rd32(rdt + 0x0C), v = rd32(rdt + 0x10);
    if (!(e < h && h < v && v < (uint32_t)sz) || re15_vab_parse(rdt + h, (size_t)(v - h), &b->vab) != 0) {
        free(rdt); return -1;
    }
    b->edt = (uint8_t *)malloc(h - e);
    if (!b->edt) { free(rdt); return -1; }
    memcpy(b->edt, rdt + e, h - e);
    b->edt_count = (int)((h - e) / 4);
    vags_dekodieren(b, rdt + v, (uint32_t)sz - v);
    free(rdt);
    return 0;
}

/* 0x3C: KLOPF1060.EDT/.VH/.VB (Muster audio_pc.c re2_schloss_laden). */
static int lade_klopfen(bank_t *b)
{
    int es = 0, hs = 0, vs = 0;
    uint8_t *edt = re15_pc_read_re2(RE15_R38F_KLOPF_DATEI ".EDT", &es);
    uint8_t *vh  = re15_pc_read_re2(RE15_R38F_KLOPF_DATEI ".VH", &hs);
    uint8_t *vb  = re15_pc_read_re2(RE15_R38F_KLOPF_DATEI ".VB", &vs);
    if (!edt || !vh || !vb || es < 4 || re15_vab_parse(vh, (size_t)hs, &b->vab) != 0) {
        free(edt); free(vh); free(vb); return -1;
    }
    vags_dekodieren(b, vb, (uint32_t)vs);
    free(vh); free(vb);
    b->edt = edt;
    b->edt_count = es / 4;
    return 0;
}

extern void re15_audio_stimme(uint16_t room, int voice_id);   /* audio_pc.c (Runde 38 Spur F) */
extern unsigned g_current_room_id;

static void spielen(int bank, int satz)
{
    int k = (bank == RE15_R38F_SE_BANK_ROLLADEN) ? 0 : (bank == RE15_R38F_SE_BANK_KLOPFEN) ? 1 : 2;
    if (k == 2) {   /* Hilferuf = Sprachzeile main42 des Raums (s. Kopf) */
        s_gespielt[2]++;
        if (getenv("RE15_SE_DEBUG"))
            fprintf(stderr, "[se] Se_on bank=0x%02X id=%d -> Runde 38 Spur F (Stimme main%02d.wav) F%u tick=%ld\n", bank,
                    satz, RE15_R38F_HILFE_STIMME, (unsigned)g_engine.frame_count, re15_audio_cap_tick());
        re15_audio_stimme((uint16_t)g_current_room_id, RE15_R38F_HILFE_STIMME);
        return;
    }
    bank_t *b = &s_bank[k];
    if (!b->state) {
        int rc = (k == 0) ? lade_rolladen(b) : lade_klopfen(b);
        b->state = rc == 0 ? 1 : -1;
        fprintf(stderr, "[dining] Port-Bank 0x%02X %s (%d VAGs, %d Saetze)\n", bank,
                rc == 0 ? "geladen" : "FEHLT -> stumm", b->vab.vag_count, b->edt_count);
    }
    if (b->state != 1 || satz < 0 || satz >= b->edt_count) return;
    s_gespielt[k]++;
    if (getenv("RE15_SE_DEBUG"))   /* Messschrieb: Spielbild + Mitschnitt-Takt (RE15_AUDIO_CAP_SYNC, Offset = Takt * 1470) */
        fprintf(stderr, "[se] Se_on bank=0x%02X id=%d -> Runde 38 Spur F (%s) F%u tick=%ld\n", bank, satz,
                k == 0 ? "ROOM1131-snd0 Rolladen" : "RE2 ROOM1060 Klopfen",
                (unsigned)g_engine.frame_count, re15_audio_cap_tick());
    re15_audio_se_play_bank(b->edt, &b->vab, b->decoded, b->decoded_len, satz);
}

unsigned re15_pc_dining_r38_gespielt(int k) { return (k >= 0 && k < 3) ? s_gespielt[k] : 0u; }

/* ============ MESSHAKEN Sichtbarkeit per Framedump (Dossier 4.3; env-gegatet, kein Spielverhalten) ============
 * Nachtrag 2 (Nutzer 2026-10-16 21:33): "Ich möchte keine plötzlich rein poppenden Charaktere haben...". Geprueft
 * wird: in keinem Bild von ROOM1141 erscheint John ohne vorherige Bewegung ins Bild. Methode (Riegel
 * integration_r38_dining1141_s): zwei gleiche Laeufe, A mit John, B mit unsichtbarem John (Mesh und Schatten aus, die
 * Spiellogik laeuft unveraendert — main.c posiert einen gecullten NPC weiter). Je Bild von ROOM1141 liest der Haken
 * das fertig komponierte Bild zurueck (re15_render_pc_request_readback = derselbe Weg wie RE15_FRAMEDUMP, unmittelbar
 * vor SDL_RenderPresent); die Pixel, in denen sich A und B unterscheiden, sind genau Johns sichtbare Pixel (Riegel-
 * Werkzeug test_r38_dining1141 sicht).
 *   RE15_R38F_SICHT=<praefix>  je Bild in ROOM1141: <praefix>NNNNNN.ppm (NNNNNN = g_engine.frame_count wie
 *                              RE15_FRAMEDUMP) und eine Zeile "[r38f-sicht] F<n> cam=<angefordert> anz=<angezeigt>
 *                              john=<da> (<x>,<z>) rolladen=<da> y=<y> elza=(<x>,<z>)" (Kamera, Lage Johns, des
 *                              Notrolladens Prop obj 0 und Elzas fuer die Regeln des Werkzeugs; angezeigt =
 *                              work_vars[0x0A], room_common.c re15_cam_present_tick @0x80021bfc)
 *   RE15_R38F_JOHN_AUS=1       John (Typ 0x49) in ROOM1141 nicht zeichnen.
 * Aufruf je NPC im Zeichendurchgang (main.c, hinter dem Regionen-Cull); Rueckgabe 1 = diesen NPC nicht zeichnen. */
#include "re15_actor.h"
#include "re15_scd.h"

int re15_pc_dining_r38_sicht(const re15_actor_t *npc)
{
    static int s_init = 0, s_aus = 0;
    static char s_praefix[256];
    static unsigned s_bild = 0xFFFFFFFFu;
    if (!s_init) {
        s_init = 1;
        const char *e = getenv("RE15_R38F_SICHT");
        if (e && *e) snprintf(s_praefix, sizeof s_praefix, "%s", e);
        e = getenv("RE15_R38F_JOHN_AUS");
        s_aus = (e && *e && *e != '0');
    }
    if ((!s_praefix[0] && !s_aus) || g_current_room_id != RE15_R38F_RAUM_1141 || !npc) return 0;
    if (s_praefix[0] && s_bild != (unsigned)g_engine.frame_count) {   /* einmal je Bild */
        s_bild = (unsigned)g_engine.frame_count;
        extern void re15_render_pc_request_readback(const char *path);
        char p[300];
        snprintf(p, sizeof p, "%s%06u.ppm", s_praefix, s_bild);
        re15_render_pc_request_readback(p);
        int jx = 0, jz = 0, jda = 0, ry = 0, rda = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == RE15_R38F_JOHN_TYP) {
                jx = g_actors[s].x; jz = g_actors[s].z; jda = 1; break;
            }
        for (int pi = 0; pi < g_scd.prop_count && pi < RE15_SCD_MAX_PROPS; pi++)
            if (g_scd.props[pi].active && g_scd.props[pi].obj_id == 0) { ry = g_scd.props[pi].y; rda = 1; break; }
        fprintf(stderr, "[r38f-sicht] F%u cam=%d anz=%d john=%d (%d,%d) rolladen=%d y=%d elza=(%d,%d)\n", s_bild,
                (int)g_scd.cam_id, (int)g_scd.work_vars[0x0A], jda, jx, jz, rda, ry,
                (int)g_actors[RE15_ACTOR_SLOT_PLAYER].x, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].z);
    }
    return s_aus && npc->type == RE15_R38F_JOHN_TYP;
}

void re15_pc_dining_r38_init(void)
{
    re15_dining_r38_se_hook = spielen;
}
