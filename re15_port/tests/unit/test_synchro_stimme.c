/* test_synchro_stimme.c — PIN: die Sprachausgabe folgt der DATEI, nicht einer Raumliste.
 *
 * NUTZER-FRAGE (2026-08-26): "In synchro hatte ich die voiceovers entsprechend der message
 * zu der die voiceover gehoert sortiert. Wie haben wir uns drauf geeinigt, dass ich weitere
 * voiceovers ablege, die du automatisch korrekt einbindest?"
 *
 * ── DIE KONVENTION ───────────────────────────────────────────────────────────────────
 *     synchro/STAGE<n>/room<RAUM-ID 4 Hex>/main<MESSAGE-ID 2 Dezimal>.wav
 * Der Loader baut den Pfad genau so (audio_pc.c re15_voice_load_clip):
 *     snprintf(reldir, ..., "synchro/STAGE%u/room%04X/", room >> 12, room);
 *     snprintf(path,   ..., "%smain%02d.wav", reldir, voice_id);
 * mit voice_id = der Message-Id aus Message_on (Opcode 0x2B, pc[1]) und room = der
 * gepackten Raum-Id ((stage+1)<<12 | raum<<4 | variante). Gross-Hex zuerst, Klein-Hex als
 * Rueckfall (case-sensitive Dateisysteme).
 *
 * ── WAS VORHER FALSCH WAR ────────────────────────────────────────────────────────────
 * Die Stimme wurde nur gequeued, wenn der Raum in der HARTKODIERTEN Liste
 * `re15_room_has_voice = { 0x1170 }` stand UND im Full-Text-Zweig lag. Gemessen am
 * Bestand von synchro (2026-08-26): 37 aufgenommene WAVs, davon 11 erreichbar — die 26
 * Dateien fuer room10D0 (9), room1150 (11) und room1240 (6) waren stumm, obwohl korrekt
 * benannt. Die Liste war ausserdem ueberfluessig: re15_voice_play kehrt still zurueck,
 * wenn es fuer (Raum, Id) keine Datei gibt — der Loader IST das Tor.
 *
 * ── WAS DIESER PIN PRUEFT ────────────────────────────────────────────────────────────
 *   P1  Ein Message_on in einem Raum AUSSERHALB der alten Liste (ROOM10D0) queued eine
 *       Sprachausgabe mit der richtigen Message-Id. (Vorher: gar nichts.)
 *   P2  Dasselbe im Schreibmaschinen-Zweig UND im Full-Text-Raum (ROOM1170) — die Stimme
 *       haengt an keiner Darstellungs-Variante mehr.
 *   P3  DATEN-WACHE ueber alle abgelegten WAVs: der Name ist einer, den der Loader auch
 *       bildet (mainNN.wav, NN dezimal, NN < 64), und der room-Ordner liegt im STAGE-
 *       Ordner, den seine Raum-Id verlangt.
 *   P4  Jede WAV liegt in einem Format vor, das der Loader lesen kann: PCM 8/16/24/32
 *       oder IEEE-Float 32. (Vorher konnte er NUR 16 bit und verwarf den Rest still —
 *       daran hingen die 6 24-bit-Aufnahmen in room1240.)
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "re15_scd.h"
#include "re15_room.h"

extern void scd_vm_init(void);
extern void scd_vm_tick(void);
extern int  scd_thread_start(int slot, const uint8_t *pc);

static int g_fail = 0;
#define CHECK(cond, msg) do {                                       \
        if (!(cond)) { printf("  FAIL: %s\n", (msg)); g_fail++; }    \
        else         { printf("  ok  : %s\n", (msg)); }              \
    } while (0)

/* Message_on = Opcode 0x2B, 4 Bytes: [op, msg_id, arg2, arg3]; danach Evt_end (0x01, 2 B).
 * ⛔ Der Evt_end MUSS da sein: mit einem Nop laeuft der Thread hinter das Feld und fuehrt
 * fremden Speicher aus (genau das hat test_1190_weste unter Linux rot gemacht). */
static int stimme_fuer(unsigned room_id, uint8_t msg_id)
{
    static uint8_t bc[6];
    bc[0] = 0x2B; bc[1] = msg_id; bc[2] = 0x00; bc[3] = 0x00;
    bc[4] = 0x01; bc[5] = 0x00;
    scd_vm_init();
    g_current_room_id = room_id;      /* nach scd_vm_init — das nullt g_scd, nicht die Raum-Id */
    scd_thread_start(0, bc);
    scd_vm_tick();
    scd_audio_event_t ev;
    while (scd_audio_queue_pop(&ev)) {
        if (ev.kind == (uint8_t)SCD_AUDIO_VOICE_ON) return (int)ev.sample_id;
    }
    return -1;
}

int main(void)
{
    printf("== test_synchro_stimme ==\n");

    /* ---- P1 / P2 ------------------------------------------------------------------ */
    {
        int a = stimme_fuer(0x10D0, 12);
        printf("  ROOM10D0 Message 12 -> VOICE_ON sample_id = %d\n", a);
        CHECK(a == 12, "P1 ROOM10D0 (war NICHT in der alten Liste) queued die Stimme");

        int b = stimme_fuer(0x1150, 7);
        printf("  ROOM1150 Message  7 -> VOICE_ON sample_id = %d\n", b);
        CHECK(b == 7, "P2a ROOM1150 ebenso (Schreibmaschinen-Zweig)");

        int c = stimme_fuer(0x1170, 0);
        printf("  ROOM1170 Message  0 -> VOICE_ON sample_id = %d\n", c);
        CHECK(c == 0, "P2b ROOM1170 weiterhin (Full-Text-Zweig, keine Regression)");

        int d = stimme_fuer(0x1240, 3);
        printf("  ROOM1240 Message  3 -> VOICE_ON sample_id = %d\n", d);
        CHECK(d == 3, "P2c ROOM1240 (Full-Text, war aber NICHT in der Stimm-Liste)");
    }

    /* ---- P3 / P4: die Dateien auf der Platte ---------------------------------------- */
    {
        int dateien = 0, namensfehler = 0, formatfehler = 0, stagefehler = 0;
        int unzugeordnet = 0;
        char sdir[512];
        for (unsigned st = 1; st <= 6; st++) {
            snprintf(sdir, sizeof sdir, "%s/STAGE%u", RE15_SYNCHRO_DIR, st);
            DIR *d = opendir(sdir);
            if (!d) continue;
            struct dirent *e;
            while ((e = readdir(d)) != NULL) {
                if (strncmp(e->d_name, "room", 4) != 0) continue;
                unsigned rid = 0;
                if (sscanf(e->d_name + 4, "%x", &rid) != 1) { stagefehler++; continue; }
                if ((rid >> 12) != st) {
                    printf("  ! %s/%s: Raum-Id 0x%04X gehoert zu STAGE%u, liegt in STAGE%u\n",
                           sdir, e->d_name, rid, rid >> 12, st);
                    stagefehler++;
                }
                char rdir[1024];
                snprintf(rdir, sizeof rdir, "%s/%s", sdir, e->d_name);
                DIR *r = opendir(rdir);
                if (!r) continue;
                struct dirent *f;
                while ((f = readdir(r)) != NULL) {
                    size_t n = strlen(f->d_name);
                    if (n < 5 || strcmp(f->d_name + n - 4, ".wav") != 0) continue;
                    dateien++;
                    int id = -1;
                    /* Zwei Faelle, die NICHT gleich behandelt werden duerfen:
                     *  - Eine Datei, die gar nicht wie "main..." heisst, ist ein noch NICHT
                     *    ZUGEORDNETER Roh-Export (z.B. "MiniMax_2026-08-27_23_50_03_Ada.wav").
                     *    Arbeit im Fluss, kein Ablage-Fehler — sie wird gemeldet, macht den
                     *    Test aber NICHT rot. Sonst blockiert jede unfertige Aufnahme den
                     *    Paketbau. Ablage dafuer ist synchro/unused/.
                     *  - Eine Datei, die "main..." heisst und die Regel trotzdem verfehlt
                     *    (einstellig, > 63, Zusatz im Namen), ist ein echter Ablage-Fehler:
                     *    sie SIEHT richtig aus und bleibt im Spiel trotzdem stumm. Genau das
                     *    soll diese Wache fangen. */
                    if (strncmp(f->d_name, "main", 4) != 0) {
                        printf("  ~ %s/%s\n"
                               "      noch nicht zugeordnet - wird nicht abgespielt. In "
                               "main<NN>.wav umbenennen (NN = Message-Id)\n"
                               "      oder nach synchro/unused/ legen.\n",
                               rdir, f->d_name);
                        unzugeordnet++;
                        continue;
                    }
                    /* n == 10 = strlen("mainNN.wav"): der Loader formatiert %02d, eine
                     * einstellige "main5.wav" wuerde er also NIE finden. */
                    if (sscanf(f->d_name, "main%d.wav", &id) != 1 ||
                        id < 0 || id >= 64 || n != 10) {
                        printf("  ! %s/%s: heisst main..., verfehlt aber die Regel "
                               "(NN dezimal, zweistellig, < 64) -> bleibt stumm\n",
                               rdir, f->d_name);
                        namensfehler++;
                        continue;
                    }
                    char fp[2048];
                    snprintf(fp, sizeof fp, "%s/%s", rdir, f->d_name);
                    FILE *fh = fopen(fp, "rb");
                    if (!fh) { formatfehler++; continue; }
                    uint8_t hdr[128];
                    size_t got = fread(hdr, 1, sizeof hdr, fh);
                    fclose(fh);
                    /* "fmt "-Chunk suchen: Tag +8, Kanalzahl +10, Bit-Tiefe +22 ab Chunk-Kopf. */
                    int bits = -1, chn = -1, tag = -1;
                    for (size_t i = 12; i + 26 <= got; i++) {
                        if (memcmp(hdr + i, "fmt ", 4) == 0) {
                            tag  = hdr[i +  8] | (hdr[i +  9] << 8);
                            chn  = hdr[i + 10] | (hdr[i + 11] << 8);
                            bits = hdr[i + 22] | (hdr[i + 23] << 8);
                            if (tag == 0xFFFE) tag = hdr[i + 32] | (hdr[i + 33] << 8);
                            break;
                        }
                    }
                    /* Was der Loader annimmt (audio_pc.c re15_voice_load_clip): PCM 8/16/24/32
                     * und IEEE-Float 32. Frueher stand dort hart `bits != 16` — die 6 24-bit-
                     * Aufnahmen in room1240 waren allein deshalb stumm. */
                    int akzeptiert = (chn >= 1) &&
                                     ((tag == 1 && (bits == 8 || bits == 16 ||
                                                    bits == 24 || bits == 32)) ||
                                      (tag == 3 && bits == 32));
                    if (!akzeptiert) {
                        printf("  ! %s: Format %d, %d bit, %d Kanaele -> der Loader kann das "
                               "nicht lesen\n", fp, tag, bits, chn);
                        formatfehler++;
                    }
                }
                closedir(r);
            }
            closedir(d);
        }
        printf("  Zensus: %d WAVs, %d Namensfehler, %d Formatfehler, %d Stage-Fehler, %d noch nicht zugeordnet\n",
               dateien, namensfehler, formatfehler, stagefehler, unzugeordnet);
        CHECK(dateien >= 30, "P3a der Zensus hat die synchro-WAVs wirklich gesehen");
        CHECK(namensfehler == 0,
              "P3b jede main-Datei folgt der Regel main<NN>.wav (NN dezimal, zweistellig, "
              "< 64) - sonst findet der Loader sie nie");
        CHECK(stagefehler == 0,
              "P3c jeder room-Ordner liegt im STAGE-Ordner, den seine Raum-Id verlangt");
        CHECK(formatfehler == 0,
              "P4 jede WAV liegt in einem Format vor, das der Loader lesen kann "
              "(PCM 8/16/24/32 oder Float 32)");
    }

    printf(g_fail ? "== test_synchro_stimme: %d FEHLER ==\n" : "== test_synchro_stimme: OK ==\n",
           g_fail);
    return g_fail ? 1 : 0;
}
