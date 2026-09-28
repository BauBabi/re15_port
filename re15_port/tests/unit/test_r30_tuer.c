/* test_r30_tuer.c — RIEGEL fuer den "Tuer verschlossen"-Ton (⛔ RE2-ERGAENZUNG).
 *
 * Dossier: analysis/befunde_runde30/tuer-verschlossen.md (Plan §5 Schritt 6).
 * Belege fuer jede Zahl: Kopf von include/re15_lock_se.h, engine/src/lock_se_common.c,
 * tools/re2_door_se_cut.py und tools/gen_lock_se_sites.py.
 *
 * EIN Programm, sechs Teile (je ein ctest-Eintrag, Aufruf mit dem Teil-Namen):
 *   bank     (a) Mini-Bank shared_assets/RE2/TUERSE.VBS mit dem Port-VAB-Code laden:
 *                je Satz Rohsatz, Stimme, Tone-Attribute, Wellen-Bytes, pitch, Samples.
 *   raeume   (b) die 98 Raeume des Dossiers im Frisch-Zustand, JEDER aktive Text-/Ereignis-/
 *                Marken-/Tuer-Platz einzeln angefahren (Verfahren der Sonde
 *                probe_r30_tuer_verschlossen): an genau 51 Plaetzen geht eine Tabellen-
 *                Nachricht auf und faellt GENAU EIN Tuer-Ton mit dem Satz der Art, an allen
 *                uebrigen 0; im Kontrolllauf ohne Druck ueberall 0; fuenf Pflichtfaelle.
 *   r4000    (c) ROOM4000 sub02 (@Datei 0x01426) direkt gestartet: RE1.5s eigenes
 *                Se_on(2,0x0f) @Datei 0x0142E faellt genau einmal, der RE2-Tuer-Ton nie.
 *   tabelle  (d) Tabellen-Volllauf ueber alle 54 Zeilen: mit jedem gesetzten Weg genau 1
 *                Aufruf mit dem Satz der Art, mit dem nicht gesetzten Weg 0; jede Nachricht
 *                ist im ausgelieferten RDT ein Schloss-Text und keine Ja/Nein-Abfrage.
 *   strom    (e) Nachbesserung: ROOM5080/5081 msg 2 "The door won't open until the power is
 *                restored!". Frisch ist Platz 0 die Tuer (Raumwechsel, 0 Toene); nach den
 *                ECHTEN Aot_reset-Bytes aus sub02 (@Datei 0x007C2 / 0x007BA) geht dort msg 2
 *                auf und faellt genau 1 x ZU_P (RE2 ROOM7020 @Datei 0x0159E).
 *   stimme   (f) Nachbesserung: der Skript-Haken sitzt HINTER der Stimmen-Warte-Schranke von
 *                op_message_on. ROOM3091 sub02 Message_on 6 (@Datei 0x0159A) mit laufender
 *                Stimme: waehrend des Parkens 0 Toene, nach dem Freigeben genau 1.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */
#include "re15_vab.h"
#include "re15_lock_se.h"
#include "gen/re2_door_bank.inc"   /* RE2_DOOR_SE_ZU_A / _B / _E (nur Makros) */

/* SOLL der Zuordnung Art -> Satz = die Vorgabe der Runde 30 (lock_se_common.c,
 * RE15_LOCK_SE_SATZ_K/_M, dort als Port-Wahl gekennzeichnet): K -> Welle E (ROOM2110),
 * M -> Welle A (ROOM1140). Tauscht der Nutzer nach dem Anhoeren, wandern diese zwei Zeilen
 * mit — der Riegel pinnt die Wahl absichtlich, damit ein Tausch nie still passiert. */
#define SOLL_SATZ_K RE2_DOOR_SE_ZU_E
#define SOLL_SATZ_M RE2_DOOR_SE_ZU_A
/* Art S ist KEINE Wahl: RE2 ROOM7020 sub06 @Datei 0x0159E Se_on(2,0x16) beim woertlich
 * gleichen Text (lock_se_common.c, RE15_LOCK_SE_SATZ_S). */
#define SOLL_SATZ_S RE2_DOOR_SE_ZU_P

static int soll_satz(int art)
{
    return art == RE15_LOCK_SE_ART_K ? SOLL_SATZ_K : art == RE15_LOCK_SE_ART_M ? SOLL_SATZ_M
         : art == RE15_LOCK_SE_ART_S ? SOLL_SATZ_S : -1;
}
static char art_zeichen(int art)
{
    return art == RE15_LOCK_SE_ART_K ? 'K' : art == RE15_LOCK_SE_ART_M ? 'M'
         : art == RE15_LOCK_SE_ART_S ? 'S' : '?';
}

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;
extern int g_test_door_se_last, g_test_door_se_count;   /* test_support.c-Spion */

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { printf("  FEHLER: "); printf(__VA_ARGS__); \
                              printf("\n"); g_fail++; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ============================ (a) die Bank ================================== */
static void teil_bank(void)
{
    printf("(a) Mini-Bank TUERSE.VBS\n");
    re15_door_bank_rec_t rec;
    re15_door_bank_rec(&rec);
    char p[600];
    snprintf(p, sizeof p, "%s/TUERSE.VBS", RE15_ASSET_RE2_DIR);
    size_t n = 0;
    uint8_t *vbs = slurp(p, &n);
    if (!vbs) { printf("  FEHLER: %s nicht lesbar\n", p); g_fail++; return; }
    /* 29976 B = re2_door_se_cut.py-Ausgabe (md5 777e1f43e65f3c68941a635d12886a63; vor der
     * Nachbesserung 23832 B ohne Satz 3). */
    CHECK(n == 29976u && n == rec.vbd_off + rec.vbd_size, "Dateigroesse %zu (TOC %u)", n,
          rec.vbd_off + rec.vbd_size);
    CHECK(rec.edt_size == 3144u && rec.vbd_size == 26832u && rec.se_count == 4 &&
          rec.se_zu_p == 3, "TOC edt %u vbd %u saetze %d zu_p %d", rec.edt_size, rec.vbd_size,
          rec.se_count, rec.se_zu_p);

    const uint8_t *edt = vbs + rec.edt_off;
    uint32_t vh_off = (uint32_t)edt[rec.edt_size-8]         | ((uint32_t)edt[rec.edt_size-7] << 8)
                    | ((uint32_t)edt[rec.edt_size-6] << 16) | ((uint32_t)edt[rec.edt_size-5] << 24);
    CHECK(vh_off == 0x20u && memcmp(edt + vh_off, "pBAV", 4) == 0, "vh_off 0x%X", vh_off);
    re15_vab_t vab;
    if (re15_vab_parse(edt + vh_off, rec.edt_size - vh_off, &vab) != 0) {
        printf("  FEHLER: re15_vab_parse\n"); g_fail++; free(vbs); return;
    }
    CHECK(vab.vag_count == 4, "vag_count %d != 4", vab.vag_count);

    /* Soll aus den RE2-Bytes (info/re2leon/PL0/RDT, Datei-Offsets, s. re2_door_se_cut.py):
     *   ZU_A ROOM1140 Satz 0x16: EDT @0x01448 00 00 74 16, Tone @0x01DB0, VAG @0x02870  7184 B
     *   ZU_B ROOM1050 Satz 0x16: EDT @0x019FC 00 00 84 16, Tone @0x02384, VAG @0x074A4  3232 B
     *   ZU_E ROOM2110 Satz 0x16: EDT @0x02EFC 00 00 74 00, Tone @0x03864, VAG @0x07054 10272 B
     *   ZU_P ROOM7020 Satz 0x16: EDT @0x05CE8 00 00 e3 00, Tone @0x06730, VAG @0x1DD50  6144 B
     * In der Mini-Bank sind nur Tone-Index (EDT-Byte 2 oben) und VAG-Index umgesetzt.
     *
     * pitch: RE2 rechnet die Tonhoehe mit DEMSELBEN note2pitch2 wie RE1.5, deshalb darf der
     * Riegel sie mit re15_vab_note2pitch2 pinnen. Se_on 0x8005ba28 legt note = Tone[+6]
     * (@0x8005bbec / @0x8005bbf4 sh +0xa) und fine = Tone[+5] (@0x8005bbf8 / @0x8005bc00
     * sh +0xc) ab; @0x8005c788 jal 0x8007fdc8; dort @0x80080110 jal 0x80083010 (note2pitch2,
     * a0 = note @0x800800fc, a1 = fine @0x80080114), solange Tone[+0x16] != 0xff
     * (@0x8007ffa0 / @0x800800f0). 0x80083010 ist Befehl fuer Befehl RE1.5 @0x80056b2c; die
     * 12x16-Tabelle RE2 @0x800aba40 ist bytegleich zu RE1.5 @0x80077520 (384 B, md5
     * 6b1d457a486b2a3a5c17e9a040a00c12). Samples: Dekoder bis einschliesslich END-Block. */
    static const struct {
        int se; uint8_t roh[4]; int voice, prio; unsigned vol, center, shift, note, bytes;
        unsigned pitch; int samples;
    } soll[4] = {
        { RE2_DOOR_SE_ZU_A, {0x00,0x00,0x04,0x16},   6, 4, 110, 85,  42, 66,  7184, 0x589, 12544 },
        { RE2_DOOR_SE_ZU_B, {0x00,0x00,0x14,0x16},   6, 4, 105, 91,   0, 67,  3232, 0x400,  5628 },
        { RE2_DOOR_SE_ZU_E, {0x00,0x00,0x24,0x00}, -16, 4, 127, 84,  57, 66, 10272, 0x5f3, 17948 },
        /* ZU_P: 384 Bloecke, END-Flag in Block 382 -> 383 x 28 = 10724 Samples;
         * note2pitch2(72, 57, 102, 57): (57+57)>>3 = 14, sem 30, Oktave -3,
         * DAT_80077520[6*16+14] >> 3 = 0x2f9 (8193 Hz) -> 1,309 s. */
        { RE2_DOOR_SE_ZU_P, {0x00,0x00,0x33,0x00}, -16, 3, 127, 102, 57, 72,  6144, 0x2f9, 10724 },
    };
    const uint8_t *vb = vbs + rec.vbd_off;
    for (int k = 0; k < 4; k++) {
        int se = soll[k].se;
        CHECK(memcmp(edt + se * 4, soll[k].roh, 4) == 0, "Satz %d Rohsatz %02x %02x %02x %02x",
              se, edt[se*4], edt[se*4+1], edt[se*4+2], edt[se*4+3]);
        re15_edt_rec_t r;
        if (re15_edt_decode(edt, se, &r) != 0 || r.empty) { printf("  FEHLER: Satz %d leer\n", se); g_fail++; continue; }
        CHECK(r.voice == soll[k].voice && r.prio_nib == soll[k].prio, "Satz %d Stimme %d prio %d",
              se, r.voice, r.prio_nib);
        int vags[8], tones[8];
        int cnt = re15_edt_resolve_layers_ex(edt, &vab, se, vags, tones, 8);
        CHECK(cnt == 1, "Satz %d: %d Lagen", se, cnt);
        if (cnt != 1) continue;
        CHECK(vags[0] == k, "Satz %d -> VAG %d", se, vags[0]);
        const re15_vab_tone_t *t = &vab.tones[tones[0]];
        CHECK(t->vol == soll[k].vol && t->center_note == soll[k].center &&
              t->pitch_shift == soll[k].shift && t->min_note == soll[k].note,
              "Satz %d Tone vol%u center%u shift%u note%u", se, t->vol, t->center_note,
              t->pitch_shift, t->min_note);
        uint32_t vsz = vab.samples[vags[0]].size, off = vab.samples[vags[0]].offset;
        CHECK(vsz == soll[k].bytes, "Satz %d Welle %u B", se, vsz);
        uint16_t pitch = re15_vab_note2pitch2(t->min_note, t->pitch_shift, t->center_note, t->pitch_shift);
        CHECK(pitch == soll[k].pitch, "Satz %d pitch 0x%03x", se, pitch);
        size_t cap = (vsz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t) + 16);
        int len = (pcm && off + vsz <= rec.vbd_size) ? re15_vag_adpcm_decode(vb + off, vsz, pcm, cap) : -1;
        CHECK(len == soll[k].samples, "Satz %d %d Samples", se, len);
        printf("   Satz %d: Stimme %d prio %d  vol%u center%u shift%u note%u  %u B  pitch 0x%03x"
               " = %u Hz  %d Samples = %.3f s\n", se, r.voice, r.prio_nib, t->vol, t->center_note,
               t->pitch_shift, t->min_note, vsz, pitch, (unsigned)((44100u * pitch) >> 12), len,
               len > 0 ? (double)len / (double)((44100u * pitch) >> 12) : 0.0);
        free(pcm);
    }
    /* Kein weiterer Satz belegt (SE-Map 0x20 B = 8 Saetze, 4..7 muessen leer sein). */
    for (int se = 4; se < (int)(vh_off / 4); se++) {
        re15_edt_rec_t r;
        CHECK(re15_edt_decode(edt, se, &r) != 0 || r.empty, "Satz %d unerwartet belegt", se);
    }
    free(vbs);
}

/* ======================= (d) Tabellen-Volllauf =============================== */
static uint8_t          *s_raw = NULL;
static size_t            s_rawsz = 0;
static re15_rdt_t        s_rdt;

static int rdt_laden(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR,
             (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    return 0;
}

static void teil_tabelle(void)
{
    printf("(d) Tabellen-Volllauf\n");
    int n = re15_lock_se_site_count(), nK = 0, nM = 0, nS = 0, aufrufe = 0;
    CHECK(n == 54, "Tabelle hat %d Zeilen statt 54", n);
    CHECK(re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_K) == SOLL_SATZ_K, "Art K -> Satz %d",
          re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_K));
    CHECK(re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_M) == SOLL_SATZ_M, "Art M -> Satz %d",
          re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_M));
    CHECK(re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_S) == SOLL_SATZ_S, "Art S -> Satz %d",
          re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_S));
    for (int i = 0; i < n; i++) {
        unsigned room = 0; int msg = 0, art = 0, wege = 0;
        re15_lock_se_site(i, &room, &msg, &art, &wege);
        if (art == RE15_LOCK_SE_ART_K) nK++; else if (art == RE15_LOCK_SE_ART_M) nM++;
        else if (art == RE15_LOCK_SE_ART_S) nS++;
        CHECK(wege >= 1 && wege <= 3, "Zeile %d wege %d", i, wege);
        int satz = soll_satz(art);
        CHECK(satz >= 0, "Zeile %d: unbekannte Art %d", i, art);
        for (int weg = RE15_LOCK_WEG_AOT; weg <= RE15_LOCK_WEG_SKRIPT; weg <<= 1) {
            int c0 = g_test_door_se_count; unsigned z0 = g_re15_lock_se_zaehler;
            g_test_door_se_last = -1;
            int r = re15_lock_se_notice(room, (uint8_t)msg, weg);
            if (wege & weg) {
                aufrufe++;
                CHECK(r == satz && g_test_door_se_count == c0 + 1 && g_test_door_se_last == satz &&
                      g_re15_lock_se_zaehler == z0 + 1 && g_re15_lock_se_letzter == satz,
                      "ROOM%04X msg %d weg %d: r=%d Aufrufe %d Satz %d (soll %d)", room, msg, weg,
                      r, g_test_door_se_count - c0, g_test_door_se_last, satz);
            } else {
                CHECK(r == -1 && g_test_door_se_count == c0 && g_re15_lock_se_zaehler == z0,
                      "ROOM%04X msg %d weg %d (nicht gesetzt): r=%d Aufrufe %d", room, msg, weg,
                      r, g_test_door_se_count - c0);
            }
        }
        /* Die Nachricht ist im AUSGELIEFERTEN RDT ein Schloss-Text und keine Ja/Nein-Abfrage
         * (op_message_on parkt eine Abfrage VOR dem Einhaengepunkt — dort fiele nie ein Ton). */
        if (rdt_laden((uint16_t)room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; continue; }
        re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
        const char *t = re15_msg_get_text(msg);
        /* Art S: der Wortlaut von RE2 ROOM7020 msg 2 (@Datei 0x01F69), kein lock/latched. */
        CHECK(t && (art == RE15_LOCK_SE_ART_S
                        ? (strstr(t, "won't open until") != NULL && strstr(t, "power is restored") != NULL)
                        : (strstr(t, "lock") != NULL || strstr(t, "latched") != NULL)),
              "ROOM%04X msg %d Art %c \"%.60s\" ist kein Schloss-Text", room, msg,
              art_zeichen(art), t ? t : "?");
        CHECK(!re15_msg_is_choice(msg), "ROOM%04X msg %d ist eine Ja/Nein-Abfrage", room, msg);
    }
    CHECK(nK == 17 && nM == 35 && nS == 2, "Arten K %d / M %d / S %d statt 17 / 35 / 2", nK, nM, nS);
    /* Gegenproben: Nachrichten, die NICHT in der Tabelle stehen, auf beiden Wegen stumm. */
    static const struct { uint16_t room; uint8_t msg; const char *was; } nicht[] = {
        { 0x1070, 4,  "Spinde (The lockers all appear to be locked.)" },
        { 0x10D0, 7,  "Kartenleser selbst (A card reader...)" },
        { 0x10D0, 4,  "Ziffernfeld falscher Code (Wrong code, try again.)" },
        { 0x10D0, 5,  "Ziffernfeld richtig (You've opened the lock.)" },
        { 0x10D0, 10, "Wegweiser (Left: First Aid Room...)" },
        { 0x4090, 5,  "Ziffernfeld einer Maschine (A numerical keypanel...)" },
        /* Nachbesserung: "The door won't open!" ist in RE2 ein reiner Text-Platz, und der
         * RE2-Text-Handler PTR_800a73c4[4] = 0x80051948 ruft nur @0x80051968 jal 0x8002fe38. */
        { 0x4070, 0,  "Tuer ohne Schloss-Grund (The door won't open!)" },
        /* Rolltor, keine Tuer; RE2s "shutter ... sealed firmly/tightly sealed in place" liegen
         * ebenfalls auf Text-Plaetzen (ROOM60B0 sub00 @0x012C0, ROOM60C0 sub02 @0x01084,
         * ROOM6160 sub00 @0x01EFC) -> stumm. */
        { 0x11B0, 14, "Rolltor (The shutter is tightly sealed in place.)" },
        { 0x5080, 3,  "Strom schon da (The power already has been supplied...)" },
    };
    for (size_t k = 0; k < sizeof nicht / sizeof nicht[0]; k++)
        for (int weg = RE15_LOCK_WEG_AOT; weg <= RE15_LOCK_WEG_SKRIPT; weg <<= 1) {
            int c0 = g_test_door_se_count;
            int r = re15_lock_se_notice(nicht[k].room, nicht[k].msg, weg);
            CHECK(r == -1 && g_test_door_se_count == c0, "ROOM%04X msg %d (%s) weg %d: Ton",
                  nicht[k].room, nicht[k].msg, nicht[k].was, weg);
        }
    printf("   %d Zeilen (K %d / M %d / S %d), %d Aufrufe mit gesetztem Weg, %d Gegenproben stumm\n",
           n, nK, nM, nS, aufrufe, (int)(sizeof nicht / sizeof nicht[0]));
}

/* ================ gemeinsamer Raum-Lauf (b)/(c) — Verfahren der Sonde ================ */
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_q_se = 0, s_q_se_2_0f = 0;

static void queue_leeren(int zaehlen)
{
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) {
        if (!zaehlen || e.kind != SCD_AUDIO_SE_ON) continue;
        s_q_se++;
        if (e.bank == 2 && e.sample_id == 0x0f) s_q_se_2_0f++;
    }
}

static void frame(uint16_t held, uint16_t edge, int zaehlen)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    queue_leeren(zaehlen);
}

static int room_boot(uint16_t room)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = 0; pl->z = 0; pl->rot_y = 0;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    s_shown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 30; f++) frame(0, 0, 0);
    /* Eintritts-Zwischensequenz zu Ende laufen lassen (sperrt sonst alle Aktions-Plaetze). */
    int warm = 0;
    while (warm < 3000 &&
           (g_scd.player_mode == 2 || g_scd.letterbox_countdown != 0 || g_scd.message_active ||
            g_scd.message_display_frames > 0 || g_scd.message_query)) {
        uint16_t e = ((warm % 10) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e, 0);
        warm++;
    }
    return 0;
}

/* 620er-Vorwaertspunkt wie aot_common.c (FUN_80042bac @0x80042bd0 `ori 0x26c`). */
static int vorwaerts_im_platz(const re15_aot_t *a, int32_t px, int32_t pz, int yaw)
{
    int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
    int32_t fx = px + (int32_t)((620 * c) >> 12);
    int32_t fz = pz - (int32_t)((620 * s) >> 12);
    if (a->has_quad) return re15_aot_point_in_quad(fx, fz, a->xs, a->zs);
    long dx = (long)fx - (long)a->x, dz = (long)fz - (long)a->z;
    if (dx < 0) dx = -dx;
    if (dz < 0) dz = -dz;
    return dx <= a->half_w && dz <= a->half_h;
}

typedef struct {
    int gemessen, stand, raumwechsel, msg_n, msg_ids[8], tuer, tuer_satz;
} platz_t;

/* Platz `slot` im SCHON GELADENEN Raum anfahren und (mit/ohne Druck) messen. */
static void messen_platz(int slot, int druck, platz_t *out)
{
    memset(out, 0, sizeof *out); out->tuer_satz = -1;
    re15_aot_t *a = &g_aot.slots[slot];
    if (!a->active) return;
    uint8_t typ = a->type, band = a->band;
    long ax = (long)a->x, az = (long)a->z;
    if (a->has_quad) {
        ax = ((long)a->xs[0] + a->xs[1] + a->xs[2] + a->xs[3]) / 4;
        az = ((long)a->zs[0] + a->zs[1] + a->zs[2] + a->zs[3]) / 4;
    }
    if (typ == RE15_AOT_TYPE_DOOR) band = g_aot.door_params[slot].band;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    static const int dxf[5] = { 0, 1, -1, 0, 0 }, dzf[5] = { 0, 0, 0, 1, -1 };
    int gefunden = 0;
    for (int k = 0; k < 5 && !gefunden; k++) {
        long zx = ax + dxf[k] * (long)(a->has_quad ? 0 : a->half_w / 2);
        long zz = az + dzf[k] * (long)(a->has_quad ? 0 : a->half_h / 2);
        for (int d = 0; d < 8 && !gefunden; d++) {
            int yaw = d * 512;
            int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
            int32_t px = (int32_t)zx - (int32_t)((620 * c) >> 12);
            int32_t pz = (int32_t)zz + (int32_t)((620 * s) >> 12);
            pl->rot_y = (int16_t)yaw; pl->x = px; pl->z = pz;
            if (!(band & 0x80)) { re15_collision_set_band((int)band); pl->floor = band; }
            frame(0, 0, 0);
            if (g_room_change.pending) g_room_change.pending = 0;
            if (pl->x == px && pl->z == pz && (int)pl->rot_y == yaw &&
                vorwaerts_im_platz(a, pl->x, pl->z, yaw))
                gefunden = 1;
        }
    }
    int c0 = g_test_door_se_count; g_test_door_se_last = -1;
    g_room_change.pending = 0;
    int prev_act = 0, prev_id = -1;
    out->stand = gefunden;
    if (gefunden && !g_scd.message_active) {
        out->gemessen = 1;
        for (int f = 0; f < 150; f++) {
            uint16_t e = 0;
            if (f == 0 && druck) e = RE15_PAD_BIT_SQUARE;
            else if (g_scd.message_active && (f % 10) == 0) e = RE15_PAD_BIT_SQUARE;  /* blaettern */
            frame(e, e, 1);
            int act = (int)g_scd.message_active, id = (int)g_scd.message_id;
            if (act && (!prev_act || id != prev_id) && out->msg_n < 8) out->msg_ids[out->msg_n++] = id;
            prev_act = act; prev_id = id;
            if (g_room_change.pending) { out->raumwechsel = 1; break; }
        }
    }
    out->tuer = g_test_door_se_count - c0;
    if (out->tuer) out->tuer_satz = g_test_door_se_last;
}

static void messen(uint16_t room, int slot, int druck, platz_t *out)
{
    memset(out, 0, sizeof *out); out->tuer_satz = -1;
    if (room_boot(room) != 0) return;
    messen_platz(slot, druck, out);
}

/* Tabellen-Stelle zu einer aufgegangenen Nachricht? -> Art, sonst -1. */
static int tabellen_art(unsigned room, int msg)
{
    for (int i = 0; i < re15_lock_se_site_count(); i++) {
        unsigned r = 0; int m = 0, art = 0, wege = 0;
        re15_lock_se_site(i, &r, &m, &art, &wege);
        if (r == room && m == msg) return art;
    }
    return -1;
}

/* Die 98 Raeume, die laut Zensus einen Schloss-Text tragen (Dossier §2.2,
 * build/r30_tuer-verschlossen/port_messung.txt). */
static const uint16_t k_raeume[98] = {
    0x1011,0x1030,0x1031,0x1040,0x1041,0x1050,0x1051,0x1070,0x1071,0x1090,0x10C0,0x10C1,
    0x10D0,0x10D1,0x10F0,0x10F1,0x1100,0x1101,0x1120,0x1121,0x1130,0x1131,0x1140,0x1141,
    0x1170,0x1171,0x1180,0x1181,0x11A0,0x11A1,0x11B0,0x11B1,0x11D0,0x11D1,0x11E0,0x11E1,
    0x1211,0x1230,0x1231,0x2020,0x2021,0x2030,0x2031,0x2050,0x2051,0x2070,0x2071,0x20A0,
    0x20A1,0x3010,0x3011,0x3020,0x3021,0x3040,0x3041,0x3050,0x3051,0x3060,0x3061,0x3090,
    0x3091,0x30C0,0x30C1,0x30D0,0x30D1,0x4000,0x4001,0x4010,0x4011,0x4050,0x4051,0x4070,
    0x4071,0x4080,0x4081,0x4090,0x4091,0x40A0,0x40A1,0x5000,0x5001,0x5011,0x5060,0x5061,
    0x5070,0x5071,0x5080,0x5081,0x50A0,0x50A1,0x50C0,0x50C1,0x5120,0x5121,0x6000,0x6001,
    0x6010,0x6011,
};

/* ======================= (b) 98 Raeume, Frisch-Zustand ======================== */
static void teil_raeume(void)
{
    printf("(b) 98 Raeume im Frisch-Zustand\n");
    int plaetze = 0, ohne = 0, offen = 0, tab = 0, tabK = 0, tabM = 0, tabS = 0, still = 0;
    unsigned stellen[64]; int n_stellen = 0;
    /* Pflichtfaelle: 1 = gesehen und richtig. */
    int p10d0_0 = 0, p1170_5 = 0, p1070_4 = 0, p10d0_17 = 0, p10d0_14 = 0;
    for (int ri = 0; ri < 98; ri++) {
        uint16_t room = k_raeume[ri];
        if (room_boot(room) != 0) continue;
        int slots[RE15_AOT_MAX], n = 0;
        for (int i = 0; i < RE15_AOT_MAX; i++) {
            const re15_aot_t *a = &g_aot.slots[i];
            if (!a->active) continue;
            if (a->type == RE15_AOT_TYPE_MESSAGE || a->type == RE15_AOT_TYPE_GENERIC ||
                a->type == RE15_AOT_TYPE_EXAMINE_WORKVAR || a->type == RE15_AOT_TYPE_DOOR)
                slots[n++] = i;
        }
        for (int j = 0; j < n; j++) {
            platz_t k, d;
            messen(room, slots[j], 0, &k);     /* Kontrolllauf ohne Druck */
            messen(room, slots[j], 1, &d);
            plaetze++;
            if (!d.stand) ohne++; else if (!d.gemessen) offen++;
            CHECK(k.tuer == 0, "ROOM%04X Platz %d: Tuer-Ton OHNE Druck (%d)", room, slots[j], k.tuer);
            int art = -1, amsg = -1;
            for (int m = 0; m < d.msg_n && art < 0; m++) {
                art = tabellen_art(room, d.msg_ids[m]);
                if (art >= 0) amsg = d.msg_ids[m];
            }
            if (art >= 0) {
                int satz = soll_satz(art);
                tab++;
                if (art == RE15_LOCK_SE_ART_K) tabK++;
                else if (art == RE15_LOCK_SE_ART_M) tabM++;
                else tabS++;
                CHECK(d.tuer == 1 && d.tuer_satz == satz,
                      "ROOM%04X Platz %d msg %d Art %c: %d Aufrufe, Satz %d (soll 1 x %d)",
                      room, slots[j], amsg, art_zeichen(art), d.tuer, d.tuer_satz, satz);
                unsigned key = ((unsigned)room << 8) | (unsigned)amsg; int neu = 1;
                for (int s = 0; s < n_stellen; s++) if (stellen[s] == key) neu = 0;
                if (neu && n_stellen < 64) stellen[n_stellen++] = key;
            } else {
                still++;
                CHECK(d.tuer == 0, "ROOM%04X Platz %d (keine Tabellen-Nachricht): %d Tuer-Toene",
                      room, slots[j], d.tuer);
            }
            /* Pflichtfaelle (Dossier §5 Schritt 6.2) */
            if (room == 0x10D0 && slots[j] == 0)
                p10d0_0 = d.msg_n >= 1 && d.msg_ids[0] == 6 && d.tuer == 1 && d.tuer_satz == SOLL_SATZ_K;
            if (room == 0x1170 && slots[j] == 5)
                p1170_5 = d.msg_n >= 1 && d.msg_ids[0] == 12 && d.tuer == 1 && d.tuer_satz == SOLL_SATZ_M;
            if (room == 0x1070 && slots[j] == 4)
                p1070_4 = d.msg_n >= 1 && d.msg_ids[0] == 4 && d.tuer == 0;
            if (room == 0x10D0 && slots[j] == 17)
                p10d0_17 = d.msg_n >= 1 && d.msg_ids[0] == 10 && d.tuer == 0;
            if (room == 0x10D0 && slots[j] == 14)
                p10d0_14 = d.raumwechsel && d.tuer == 0;
        }
    }
    printf("   %d Plaetze, %d ohne Standplatz, %d mit schon offenem Text (beide nicht gemessen),"
           " %d mit Tabellen-Nachricht (K %d / M %d, %d Stellen), %d uebrige\n",
           plaetze, ohne, offen, tab, tabK, tabM, n_stellen, still);
    /* Art S (ROOM5080/5081 msg 2) liegt im Frisch-Zustand NICHT auf einem Platz: erst sub02
     * (Generator) setzt ihn per Aot_reset. Den Weg prueft Teil (e). */
    CHECK(tab == 51 && tabK == 16 && tabM == 35 && tabS == 0 && n_stellen == 46,
          "Tabellen-Plaetze %d (K %d / M %d / S %d, %d Stellen) statt 51 (16 / 35 / 0, 46)",
          tab, tabK, tabM, tabS, n_stellen);
    CHECK(p10d0_0,  "Pflichtfall ROOM10D0 Platz 0 -> msg 6, 1 x ZU_E");
    CHECK(p1170_5,  "Pflichtfall ROOM1170 Platz 5 -> msg 12, 1 x ZU_A");
    CHECK(p1070_4,  "Pflichtfall ROOM1070 Platz 4 (Spinde) -> msg 4, 0 Toene");
    CHECK(p10d0_17, "Pflichtfall ROOM10D0 Platz 17 (Wegweiser) -> msg 10, 0 Toene");
    CHECK(p10d0_14, "Pflichtfall ROOM10D0 Platz 14 (offene Tuer) -> Raumwechsel, 0 Toene");
}

/* ======================= (c) ROOM4000 sub02 ================================= */
static void teil_r4000(void)
{
    printf("(c) ROOM4000 sub02 — RE1.5s eigener Schloss-Ton bleibt RE1.5\n");
    if (room_boot(0x4000) != 0) return;
    /* sub02 @Datei 0x01426: Ifel_ck / Ck(3,32)==0 / 0x0142E Se_on(2,0x0f) / 0x0143A Message_on 0 */
    static const uint8_t se_on[4] = { 0x36, 0x02, 0x0f, 0x00 }, msg0[2] = { 0x2b, 0x00 };
    CHECK(s_rawsz > 0x01440 && memcmp(s_raw + 0x0142E, se_on, 4) == 0 &&
          memcmp(s_raw + 0x0143A, msg0, 2) == 0, "ROOM4000 @0x0142E/@0x0143A nicht Se_on(2,0x0f)/Message_on 0");
    CHECK(s_rdt.sub_scd[2] == s_raw + 0x01426, "sub02 liegt nicht @Datei 0x01426");
    int slot = -1;
    for (int i = 2; i < SCD_THREAD_COUNT; i++) if (!g_scd.threads[i].active) { slot = i; break; }
    if (slot < 0 || scd_thread_start(slot, s_rdt.sub_scd[2]) != 0) {
        printf("  FEHLER: sub02 startet nicht\n"); g_fail++; return;
    }
    s_q_se = 0; s_q_se_2_0f = 0;
    int c0 = g_test_door_se_count, msg0_auf = 0;
    for (int f = 0; f < 600; f++) {
        uint16_t e = (g_scd.message_active && (f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e, 1);
        if (g_scd.message_active && g_scd.message_id == 0) msg0_auf = 1;
    }
    int tuer = g_test_door_se_count - c0;
    printf("   Se_on(2,0x0f) %d x, SCD-Se_on gesamt %d, msg 0 aufgegangen %d, RE2-Tuer-Ton %d x\n",
           s_q_se_2_0f, s_q_se, msg0_auf, tuer);
    CHECK(s_q_se_2_0f == 1, "Se_on(2,0x0f) %d x statt 1", s_q_se_2_0f);
    CHECK(msg0_auf, "msg 0 ist nicht aufgegangen");
    CHECK(tuer == 0, "RE2-Tuer-Ton %d x auf dem Skript-Weg (soll 0)", tuer);
}

/* ================= (e) ROOM5080/5081 msg 2 — Tuer ohne Strom ================= */
static void teil_strom(void)
{
    printf("(e) ROOM5080/5081 msg 2 (ohne Strom) - Tuer-Platz 0 nach sub02s Aot_reset\n");
    /* sub02 @Datei 0x007C2 (5080) / 0x007BA (5081): Aot_reset Platz 0, sce 1, msg 2.
     * Platz 0 ist frisch der Tuer-Platz (Door_aot_set main00 @Datei 0x006FA). */
    static const struct { uint16_t room; uint32_t off; } f[2] = { { 0x5080, 0x007C2 }, { 0x5081, 0x007BA } };
    static const uint8_t reset[10] = { 0x46, 0x00, 0x01, 0x31, 0x02, 0x00, 0xff, 0xff, 0x00, 0x00 };
    static uint8_t prog[2][12];
    for (int k = 0; k < 2; k++) {
        platz_t vor, kontrolle, d;
        messen(f[k].room, 0, 1, &vor);
        CHECK(vor.raumwechsel && vor.tuer == 0, "ROOM%04X frisch Platz 0: Raumwechsel %d, %d Toene (soll Tuer, 0)",
              f[k].room, vor.raumwechsel, vor.tuer);
        if (room_boot(f[k].room) != 0) continue;
        CHECK(s_rawsz > f[k].off + 10 && memcmp(s_raw + f[k].off, reset, 10) == 0,
              "ROOM%04X @Datei 0x%05X ist nicht Aot_reset(0, sce 1, msg 2)", f[k].room, (unsigned)f[k].off);
        CHECK(s_raw[0x006FA] == 0x3B && s_raw[0x006FB] == 0x00, "ROOM%04X @0x006FA ist nicht Door_aot_set Platz 0", f[k].room);
        /* Genau diese zehn Bytes aus dem ausgelieferten RDT, dahinter Evt_end (01 00). */
        memcpy(prog[k], s_raw + f[k].off, 10); prog[k][10] = 0x01; prog[k][11] = 0x00;
        int slot = -1;
        for (int i = 2; i < SCD_THREAD_COUNT; i++) if (!g_scd.threads[i].active) { slot = i; break; }
        if (slot < 0 || scd_thread_start(slot, prog[k]) != 0) { printf("  FEHLER: Aot_reset startet nicht\n"); g_fail++; continue; }
        for (int i = 0; i < 5; i++) frame(0, 0, 0);
        const re15_aot_t *a = &g_aot.slots[0];
        CHECK(a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == 2,
              "ROOM%04X Platz 0 nach Aot_reset: aktiv %d Typ %d msg %d (soll Text msg 2)",
              f[k].room, a->active, a->type, a->event_id);
        messen_platz(0, 0, &kontrolle);
        messen_platz(0, 1, &d);
        printf("   ROOM%04X: frisch Raumwechsel %d / %d Toene; nach Aot_reset ohne Druck %d Toene,"
               " mit Druck msg %d, %d Toene, Satz %d\n", f[k].room, vor.raumwechsel, vor.tuer,
               kontrolle.tuer, d.msg_n ? d.msg_ids[0] : -1, d.tuer, d.tuer_satz);
        CHECK(kontrolle.tuer == 0, "ROOM%04X: Ton ohne Druck (%d)", f[k].room, kontrolle.tuer);
        CHECK(d.gemessen && d.msg_n >= 1 && d.msg_ids[0] == 2 && d.tuer == 1 && d.tuer_satz == SOLL_SATZ_S,
              "ROOM%04X Platz 0: msg %d, %d Toene, Satz %d (soll msg 2, 1 x %d)", f[k].room,
              d.msg_n ? d.msg_ids[0] : -1, d.tuer, d.tuer_satz, SOLL_SATZ_S);
    }
}

/* ============ (f) Skript-Haken hinter der Stimmen-Warte-Schranke ============ */
static void teil_stimme(void)
{
    extern int g_re15_voice_laeuft, g_re15_voice_restbilder;
    printf("(f) ROOM3091 sub02 Message_on 6 mit laufender Stimme - 1 Ton, nicht je Bild\n");
    if (room_boot(0x3091) != 0) return;
    /* ROOM3091.RDT @Datei 0x0159A `2b 06 ff ff` = Message_on 6 (Tabellenzeile Art M, Weg SKRIPT). */
    static const uint8_t msg6[4] = { 0x2b, 0x06, 0xff, 0xff };
    CHECK(s_rawsz > 0x015A0 && memcmp(s_raw + 0x0159A, msg6, 4) == 0, "ROOM3091 @0x0159A ist nicht Message_on 6");
    static uint8_t prog[6];
    memcpy(prog, s_raw + 0x0159A, 4); prog[4] = 0x01; prog[5] = 0x00;
    int slot = -1;
    for (int i = 2; i < SCD_THREAD_COUNT; i++) if (!g_scd.threads[i].active) { slot = i; break; }
    if (slot < 0 || scd_thread_start(slot, prog) != 0) { printf("  FEHLER: Thread startet nicht\n"); g_fail++; return; }
    enum { PARK = 40 };
    int c0 = g_test_door_se_count, geparkt_text = 0, msg6_auf = 0;
    g_re15_voice_laeuft = 1;
    for (int f = 0; f < PARK; f++) {
        g_re15_voice_restbilder = PARK - f;          /* die Plattform stempelt das je Bild */
        frame(0, 0, 0);
        if (g_scd.message_active && g_scd.message_id == 6) geparkt_text = 1;
    }
    int nach_park = g_test_door_se_count - c0;
    g_re15_voice_laeuft = 0; g_re15_voice_restbilder = 0;
    for (int f = 0; f < 200; f++) {
        uint16_t e = (g_scd.message_active && (f % 10) == 9) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e, 0);
        if (g_scd.message_active && g_scd.message_id == 6) msg6_auf = 1;
    }
    int gesamt = g_test_door_se_count - c0;
    printf("   %d Bilder geparkt: %d Toene, Text offen %d; nach der Freigabe: %d Toene gesamt,"
           " msg 6 offen %d, Satz %d\n", PARK, nach_park, geparkt_text, gesamt, msg6_auf,
           g_test_door_se_last);
    CHECK(nach_park == 0 && !geparkt_text, "waehrend die Stimme laeuft: %d Toene, Text %d (soll 0 / 0)",
          nach_park, geparkt_text);
    CHECK(gesamt == 1 && msg6_auf && g_test_door_se_last == SOLL_SATZ_M,
          "nach der Freigabe %d Toene (soll genau 1 x %d), msg 6 %d", gesamt, SOLL_SATZ_M, msg6_auf);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    int alle = strcmp(teil, "alle") == 0;
    if (alle || !strcmp(teil, "bank"))    teil_bank();
    if (alle || !strcmp(teil, "tabelle")) teil_tabelle();
    if (alle || !strcmp(teil, "r4000"))   teil_r4000();
    if (alle || !strcmp(teil, "raeume"))  teil_raeume();
    if (alle || !strcmp(teil, "strom"))   teil_strom();
    if (alle || !strcmp(teil, "stimme"))  teil_stimme();
    free(s_raw);
    if (g_fail) { printf("FAIL: %d Fehler (%s)\n", g_fail, teil); return 1; }
    printf("PASS (%s)\n", teil);
    return 0;
}
