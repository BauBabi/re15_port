/* probe_r17_untertitel.c — MESSSONDE + PIN zur NUTZER-ENTSCHEIDUNG "Untertitel so lange
 * einblenden wie die Sprachausgabe".
 *
 * Nutzer-Auftrag 2026-09-20: "dann haben wir die voiceovers zum teil angepasst das es passt.
 * das ist gut. koennen wir einfach die Untertitel Einblendung So lange ebenfalls verlaengern?"
 *
 * ⛔ KEINE byte-true Frage: RE1.5 hat gar keine englische Sprachausgabe, synchro/ ist eigene
 * Produktion des Projekts. Byte-true bleibt die Text-MASCHINE selbst (FUN_80028134) — die
 * wird nicht angefasst; angehaengt wird nur ein NACHHALL hinter ihrem regulaeren Ende.
 *
 * MESS-AUFBAU (kein Nachbau — der echte Code-Pfad):
 *   - Die .msg-Bytes kommen aus der echten RDT (re15_msg_load_room_block + re15_msg_get_raw).
 *   - Getickt wird die echte FSM ueber re15_msg_tick -> re15_dialog_step.
 *   - Die Tonlaenge kommt aus der echten WAV unter synchro/ (Kopf selbst gelesen).
 *   - VORHER  = derselbe Lauf mit g_re15_voice_laeuft = 0. Dann ist der Nachhall-Zweig
 *               unerreichbar, der Code verhaelt sich exakt wie vor der Aenderung.
 *   - NACHHER = g_re15_voice_laeuft = 1 und g_re15_voice_restbilder je Bild heruntergezaehlt,
 *               genau wie die Plattform es stempelt (audio_pc.c re15_audio_tick).
 *
 * Zusaetzlich festgehalten: waehrend des Nachhalls sind message_display_frames UND
 * message_query beide 0 — das sind die zwei Werte, an denen msg_block haengt
 * (player_common.c:756, aot_common.c:845). Der Spieler wird also NICHT laenger blockiert.
 *
 * Aufruf: probe_r17_untertitel [pin]
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_msg.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_SYNCHRO_DIR
#define RE15_SYNCHRO_DIR "synchro"
#endif

extern scd_vm_t g_scd;
extern int      g_re15_voice_laeuft;
extern int      g_re15_voice_restbilder;

static int g_fail = 0;
#define CHECK(name, cond) do {                                          \
        int _c = (cond);                                                \
        printf("  [%s] %s\n", _c ? "OK  " : "FAIL", (name));            \
        if (!_c) g_fail++;                                              \
    } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static uint32_t rd32(const uint8_t *b) { return (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24); }
static uint16_t rd16(const uint8_t *b) { return (uint16_t)(b[0] | (b[1] << 8)); }

/* Tonlaenge in 30-Hz-Bildern. Die synchro-WAVs mischen 32000 und 22050 Hz und haben
 * unterschiedlich lange Koepfe — deshalb wird der fmt-/data-Chunk gesucht, nicht 44 Bytes
 * geraten (derselbe Grund wie in audio_pc.c). */
static int wav_bilder(const char *pfad)
{
    size_t n = 0;
    uint8_t *b = slurp(pfad, &n);
    if (!b || n < 44 || memcmp(b, "RIFF", 4) || memcmp(b + 8, "WAVE", 4)) { free(b); return -1; }
    uint32_t rate = 0; uint16_t kan = 1, bits = 16; uint32_t datalen = 0;
    size_t o = 12;
    while (o + 8 <= n) {
        uint32_t clen = rd32(b + o + 4);
        if (!memcmp(b + o, "fmt ", 4) && clen >= 16 && o + 8 + 16 <= n) {
            kan  = rd16(b + o + 8 + 2);
            rate = rd32(b + o + 8 + 4);
            bits = rd16(b + o + 8 + 14);
        } else if (!memcmp(b + o, "data", 4)) {
            datalen = clen;
            if (o + 8 + clen > n) datalen = (uint32_t)(n - o - 8);
            break;
        }
        o += 8 + clen + (clen & 1);
    }
    free(b);
    if (!rate || !datalen || !kan || !bits) return -1;
    double samples = (double)datalen / ((double)kan * (double)bits / 8.0);
    return (int)(samples / (double)rate * 30.0 + 0.5);
}

/* Einen Lauf: Zeile oeffnen, ticken, sichtbare Bilder zaehlen. */
typedef struct { int sichtbar; int block_bilder; int nachhall_bilder; } lauf_t;

static lauf_t lauf(int msg_id, int ton_bilder)
{
    lauf_t r; memset(&r, 0, sizeof r);
    memset(&g_scd, 0, sizeof g_scd);
    re15_pauseflags_clear();
    g_re15_voice_restbilder = ton_bilder;
    re15_dialog_open_mask(msg_id, 0, 0u);
    for (int f = 0; f < 2000; f++) {
        const unsigned char *raw = NULL; int len = 0, id = 0;
        int sichtbar = re15_msg_tick(&raw, &len, &id);
        if (!sichtbar) break;
        r.sichtbar++;
        if (g_scd.message_display_frames > 0 || g_scd.message_query) r.block_bilder++;
        if (g_scd.message_fsm == 7 || g_scd.message_nachhall) r.nachhall_bilder++;
        if (g_re15_voice_laeuft && g_re15_voice_restbilder > 0) g_re15_voice_restbilder--;
    }
    return r;
}

/* Die vertonten Zeilen des Raums mit dem groessten gemessenen Fehlbetrag (ROOM1150,
 * analysis/voiceover_2026-09-14/BEFUND.md: main09 = 242,5 Bilder gegen 150 Bilder Abstand). */
static const int s_zeilen[] = { 2, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14 };

int main(int argc, char **argv)
{
    int pin = (argc > 1 && strcmp(argv[1], "pin") == 0);

    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1150.RDT", RE15_ASSET_PSX_DIR);
    size_t rawsz = 0;
    uint8_t *raw = slurp(rp, &rawsz);
    if (!raw) { printf("SKIP: %s fehlt\n", rp); return 77; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(raw, rawsz, &rdt) < 0) { printf("FAIL: RDT-Parse\n"); free(raw); return 1; }
    re15_msg_load_room_block(rdt.messages, rdt.messages_size);

    printf("=== Anzeigedauer gegen Tonlaenge — ROOM1150 (30 Hz) ===\n");
    printf("  %-6s %8s %10s %10s %10s %8s\n",
           "Zeile", "Ton", "vorher", "nachher", "Nachhall", "Block");
    int gemessen = 0, zu_kurz_vorher = 0, zu_kurz_nachher = 0;
    for (unsigned i = 0; i < sizeof s_zeilen / sizeof s_zeilen[0]; i++) {
        int id = s_zeilen[i];
        char wp[600];
        snprintf(wp, sizeof wp, "%s/STAGE1/room1150/main%02d.wav", RE15_SYNCHRO_DIR, id);
        int tb = wav_bilder(wp);
        if (tb < 0) { printf("  %-6d %8s  (keine Aufnahme)\n", id, "-"); continue; }

        g_re15_voice_laeuft = 0;
        lauf_t v = lauf(id, 0);                 /* VORHER: Nachhall-Zweig unerreichbar */
        g_re15_voice_laeuft = 1;
        lauf_t n = lauf(id, tb);                /* NACHHER */
        g_re15_voice_laeuft = 0;

        printf("  %-6d %8d %10d %10d %10d %8d\n",
               id, tb, v.sichtbar, n.sichtbar, n.nachhall_bilder, n.block_bilder);
        gemessen++;
        if (v.sichtbar < tb) zu_kurz_vorher++;
        if (n.sichtbar < tb) zu_kurz_nachher++;

        /* Der Text muss JETZT mindestens so lange stehen wie die Aufnahme laeuft
         * (Deckel 300 Bilder; laengste Aufnahme des Bestands 242,5). */
        char nm[128];
        snprintf(nm, sizeof nm, "Zeile %d: Anzeige %d >= Ton %d", id, n.sichtbar, tb);
        CHECK(nm, n.sichtbar >= (tb < 300 ? tb : 300));
        /* Und der Spieler darf dabei nicht laenger gesperrt sein als vorher. */
        snprintf(nm, sizeof nm, "Zeile %d: Sperre unveraendert (%d Bilder, vorher %d)",
                 id, n.block_bilder, v.block_bilder);
        CHECK(nm, n.block_bilder <= v.block_bilder);
    }
    printf("  gemessen: %d Zeilen; zu kurz VORHER %d, zu kurz NACHHER %d\n",
           gemessen, zu_kurz_vorher, zu_kurz_nachher);
    CHECK("mindestens 5 Zeilen gemessen", gemessen >= 5);
    CHECK("vorher waren Zeilen zu kurz (der gemeldete Fehler)", zu_kurz_vorher > 0);
    CHECK("nachher ist keine Zeile mehr zu kurz", zu_kurz_nachher == 0);

    /* Gegenprobe: OHNE laufende Aufnahme darf sich NICHTS aendern. */
    printf("\n=== Gegenprobe: ohne Aufnahme bleibt die Dauer exakt wie vorher ===\n");
    g_re15_voice_laeuft = 0;
    lauf_t a = lauf(2, 0);
    lauf_t b = lauf(2, 0);
    printf("  Zeile 2 ohne Aufnahme: %d / %d Bilder, Nachhall %d\n",
           a.sichtbar, b.sichtbar, a.nachhall_bilder);
    CHECK("ohne Aufnahme kein Nachhall", a.nachhall_bilder == 0);
    CHECK("ohne Aufnahme reproduzierbar", a.sichtbar == b.sichtbar);

    free(raw);
    printf("\n%s (%d Fehler)\n", g_fail ? "FEHLER" : "ALLES GRUEN", g_fail);
    if (!pin) return 0;
    return g_fail ? 1 : 0;
}
