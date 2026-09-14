/* probe_stimme_takt.c — MESSUNG (kein Test, kein add_test):
 * Wieviele Bilder gibt der Port jeder vertonten Dialogzeile, und steht beim
 * naechsten Message_on noch der Text-FSM der vorigen Zeile?
 *
 * WARUM MIT DEM ECHTEN CODE UND NICHT MIT EINEM NACHBAU:
 * Zwei Groessen entscheiden ueber den neuen Riegel in op_message_on
 * (scd_vm.c:1500-1516), und beide liegen auf EIN Bild genau:
 *   (1) die STANDZEIT des Dialog-FSM = wieviele re15_msg_tick-Aufrufe vergehen,
 *       bis re15_dialog_step fsm==6 erreicht und g_scd.message_fsm_active loescht
 *       (msg_common.c:539-549).  Der Riegel ist mit `!g_scd.message_fsm_active`
 *       gegatet — steht der Text noch, wartet er NICHT.
 *   (2) der ABSTAND zweier Message_on in Bildern, gemessen am echten SCD-VM
 *       (scd_vm_tick), nicht an einer Laengentabelle.
 * Ein Python-Nachbau war bei (1) um genau 1 Bild daneben (er zaehlte den
 * Aufruf, der fsm==6 SETZT, doppelt).  Bei Abstaenden wie 120/121 kippt das
 * die Aussage.  Deshalb hier die Originalfunktionen.
 *
 * Aufruf:  re15_port/build/tests/unit/probe_stimme_takt.exe
 * KEIN add_test — reine Messung.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_msg.h"
#include "re15_room.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_SYNCHRO_DIR
#define RE15_SYNCHRO_DIR "synchro"
#endif

static uint8_t *slurp(const char *p, long *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f); *n = sz; return b;
}

/* Aufnahme-Laenge in Bildern — GENAU wie audio_pc.c wav_find_data (:1463) +
 * re15_voice_load_clip (:1590ff): data-Groesse UNSIGNED gegen die vorhandenen
 * Bytes klemmen, dann out_n = src_n * 44100 / rate, Dauer = out_n / 44100. */
static double wav_bilder(const char *pfad, int *defekt)
{
    long n = 0;
    uint8_t *b = slurp(pfad, &n);
    *defekt = 0;
    if (!b) return -1.0;
    if (n < 44 || memcmp(b, "RIFF", 4) || memcmp(b + 8, "WAVE", 4)) { free(b); return -1.0; }
    int rate = 22050, ch = 1, bits = 16, fmt = 1;
    long p = 12; long dbytes = 0; int gefunden = 0;
    while (p + 8 <= n) {
        uint32_t csz = (uint32_t)b[p+4] | ((uint32_t)b[p+5]<<8) |
                       ((uint32_t)b[p+6]<<16) | ((uint32_t)b[p+7]<<24);
        const uint8_t *body = b + p + 8;
        if (!memcmp(b + p, "fmt ", 4) && p + 8 + 16 <= n) {
            fmt  = body[0] | (body[1] << 8);
            ch   = body[2] | (body[3] << 8);
            rate = body[4] | (body[5]<<8) | (body[6]<<16) | (body[7]<<24);
            bits = body[14] | (body[15] << 8);
            if (fmt == 0xFFFE && (long)csz >= 40 && p + 8 + 26 <= n)
                fmt = body[24] | (body[25] << 8);
        } else if (!memcmp(b + p, "data", 4)) {
            uint32_t avail = (uint32_t)(n - (long)(body - b));
            if (csz > avail) *defekt = 1;
            dbytes = (long)(csz > avail ? avail : csz);
            gefunden = 1;
            break;
        }
        p += 8 + (long)csz + ((long)csz & 1);
    }
    free(b);
    if (!gefunden || rate <= 0 || ch <= 0) return -1.0;
    long src_n = (fmt == 1 && bits == 16) ? (dbytes / 2) / ch
                                          : (bits ? dbytes / ((bits/8) * ch) : 0);
    double out_n = (double)src_n * 44100.0 / (double)rate;
    return out_n / 44100.0 * 30.0;
}

/* --- Opcode-Laengen: dieselbe Tabelle, die der VM benutzt (scd_vm.c:166) --- */
static const uint8_t OPLEN[256] = {
    [0x00]=1,[0x01]=2,[0x02]=1,[0x03]=4,[0x04]=4,[0x05]=2,[0x06]=4,[0x07]=4,
    [0x08]=2,[0x09]=4,[0x0A]=3,[0x0B]=1,[0x0C]=1,[0x0D]=6,[0x0E]=2,[0x0F]=4,
    [0x10]=2,[0x11]=4,[0x12]=2,[0x13]=4,[0x14]=6,[0x15]=4,[0x16]=2,[0x17]=6,
    [0x18]=2,[0x19]=2,[0x1A]=2,[0x1B]=6,[0x1C]=1,[0x1D]=1,[0x1E]=1,[0x20]=1,
    [0x21]=4,[0x22]=4,[0x23]=6,[0x24]=4,[0x25]=3,[0x26]=6,[0x27]=4,[0x28]=1,
    [0x29]=2,[0x2A]=1,[0x2B]=4,[0x2C]=20,[0x2D]=34,[0x2E]=3,[0x2F]=4,
    [0x30]=1,[0x31]=1,[0x32]=8,[0x33]=8,[0x34]=4,[0x35]=3,[0x36]=12,[0x37]=4,
    [0x38]=12,[0x39]=4,[0x3A]=16,[0x3B]=32,[0x3C]=2,[0x3D]=3,[0x3E]=6,[0x3F]=4,
    [0x40]=8,[0x41]=10,[0x42]=1,[0x43]=4,[0x44]=20,[0x45]=3,[0x46]=10,[0x47]=2,
    [0x48]=16,[0x49]=8,[0x4A]=2,[0x4B]=3,[0x4C]=18,[0x4D]=10,[0x4E]=5,[0x4F]=22,
    [0x50]=22,[0x51]=4,[0x52]=4,[0x53]=3,[0x54]=6,[0x55]=6,[0x56]=6,[0x57]=4,
    [0x58]=4,[0x59]=4,[0x5A]=6,[0x5B]=4,[0x5C]=4,[0x5D]=4,[0x5E]=4,[0xFE]=5,
};
static int oplen(const uint8_t *d, int pc)
{
    uint8_t op = d[pc];
    if (op == 0x2C) return (d[pc+3] & 0x80) ? 28 : 20;
    if (op == 0x3B) return (d[pc+3] & 0x80) ? 40 : 32;
    if (op == 0x50) return (d[pc+3] & 0x80) ? 30 : 22;
    int n = OPLEN[op];
    return n ? n : 1;
}

void scd_register_current_rdt(const re15_rdt_t *rdt);   /* scd_vm.c:148 */

typedef struct { int blk; int idx; int pc; int msg; int mask; } stelle_t;

static re15_rdt_t s_rdt;

static const char *ROOMS[] = {
    "STAGE1/ROOM1050.RDT", "STAGE1/ROOM1090.RDT", "STAGE1/ROOM10D0.RDT",
    "STAGE1/ROOM1150.RDT", "STAGE1/ROOM1170.RDT", "STAGE1/ROOM11B0.RDT",
    "STAGE1/ROOM11C0.RDT", "STAGE1/ROOM1240.RDT", "STAGE2/ROOM2000.RDT",
};

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("== Sprach-Takt: Aufnahme gegen Skript, mit dem ECHTEN VM gemessen ==\n");
    printf("   Standzeit = Zahl der re15_msg_tick-Aufrufe, bis message_fsm_active faellt\n");
    printf("   Abstand   = Zahl der scd_vm_tick-Aufrufe bis zum naechsten Message_on\n\n");
    printf("%-6s %-6s %9s %8s %9s  %-11s %s\n",
           "ROOM", "MSG", "Aufnahme", "Abstand", "Standzeit", "Riegel", "Anmerkung");

    for (unsigned ri = 0; ri < sizeof(ROOMS)/sizeof(ROOMS[0]); ri++) {
        char fp[512];
        snprintf(fp, sizeof fp, "%s/%s", RE15_ASSET_PSX_DIR, ROOMS[ri]);
        long sz = 0;
        uint8_t *b = slurp(fp, &sz);
        if (!b) { printf("!! %s nicht lesbar\n", fp); continue; }
        if (re15_rdt_parse(b, (size_t)sz, &s_rdt) != 0) { printf("!! %s parse\n", fp); free(b); continue; }

        unsigned rid = 0;
        const char *nm = strstr(ROOMS[ri], "ROOM");
        sscanf(nm + 4, "%4x", &rid);
        unsigned stage = rid >> 12;

        g_current_room_id = rid;
        re15_msg_clear_room_block();
        re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
        scd_register_current_rdt(&s_rdt);

        /* alle Message_on-Stellen einsammeln (linear je Sub, Grenze = naechster
         * Sub-Start bzw. naechster Zeiger der RDT-Adresstabelle) */
        int grenze[64]; int ng = 0;
        for (int i = 0; i < RE15_RDT_MAX_SUB_SCD && ng < 60; i++)
            if (s_rdt.sub_scd[i]) grenze[ng++] = (int)(s_rdt.sub_scd[i] - b);
        if (s_rdt.main_scd && ng < 62) grenze[ng++] = (int)(s_rdt.main_scd - b);
        grenze[ng++] = (int)sz;

        stelle_t st[128]; int nst = 0;
        for (int pass = 0; pass < 2; pass++) {
            int cnt = pass ? RE15_RDT_MAX_SUB_SCD : 1;
            for (int i = 0; i < cnt && nst < 120; i++) {
                const uint8_t *sp = pass ? s_rdt.sub_scd[i] : s_rdt.main_scd;
                if (!sp) continue;
                int start = (int)(sp - b);
                int end = (int)sz;
                for (int k = 0; k < ng; k++)
                    if (grenze[k] > start && grenze[k] < end) end = grenze[k];
                int pc = start, guard = 0;
                while (pc < end && pc + 4 <= (int)sz && guard++ < 20000) {
                    if (b[pc] == 0x2B && nst < 120) {
                        st[nst].blk = pass; st[nst].idx = i; st[nst].pc = pc;
                        st[nst].msg = b[pc+1];
                        st[nst].mask = b[pc+2] | (b[pc+3] << 8);
                        nst++;
                    }
                    pc += oplen(b, pc);
                }
            }
        }

        for (int m = 0; m < 64; m++) {
            char wp[512];
            snprintf(wp, sizeof wp, "%s/STAGE%u/room%04X/main%02d.wav",
                     RE15_SYNCHRO_DIR, stage, rid, m);
            int defekt = 0;
            double bilder = wav_bilder(wp, &defekt);
            if (bilder < 0) {
                snprintf(wp, sizeof wp, "%s/STAGE%u/room%04x/main%02d.wav",
                         RE15_SYNCHRO_DIR, stage, rid, m);
                bilder = wav_bilder(wp, &defekt);
            }
            if (bilder < 0) continue;   /* nicht vertont */

            /* (1) Standzeit des Dialog-FSM mit dem ECHTEN Code */
            int standzeit = -1;
            {
                re15_dialog_open_mask(m, 0, 0u);
                for (int f = 1; f <= 3000; f++) {
                    re15_msg_tick(NULL, NULL, NULL);
                    if (!g_scd.message_fsm_active) { standzeit = f; break; }
                }
                g_scd.message_active = 0;
                g_scd.message_fsm_active = 0;
                g_scd.message_display_frames = 0;
            }

            /* (2) Abstand bis zum naechsten Message_on — echter VM.
             * Startpunkt: die Stelle dieses Message_on; wir setzen den Thread
             * DAHINTER (pc+4), damit die Zeile nicht sofort als "naechste" zaehlt. */
            int abstand = -1, killer = -1;
            for (int s = 0; s < nst; s++) {
                if (st[s].msg != m) continue;
                if (getenv("RE15_TAKT_DEBUG"))
                    fprintf(stderr, "[takt] %04X msg%d Stelle %s%02d @0x%04X\n",
                            rid, m, st[s].blk ? "sub" : "main", st[s].idx, st[s].pc);
                re15_pauseflags_clear();
                scd_vm_init();
                scd_vm_set_room_init(1);          /* kein sub01-Reseed waehrend der Messung */
                scd_register_current_rdt(&s_rdt);
                g_scd.message_active = 0;
                g_scd.message_fsm_active = 0;
                g_scd.message_id = 0xFF;
                scd_thread_reseed(0, b + st[s].pc + 4);
                int gefunden = -1;
                for (int f = 1; f <= 1500; f++) {
                    scd_vm_tick();
                    /* Message_on setzt message_id (msg_show bzw. re15_dialog_open_mask) */
                    if (g_scd.message_id != 0xFF) { gefunden = f; break; }
                    /* Dialog-FSM NICHT weiterdrehen: Maske 0 friert nichts ein,
                     * und das message_fsm_active der Vorzeile interessiert hier nicht. */
                }
                /* -1: der Zaehler beginnt bei dem Bild, in dem die Zeile SELBST
                 * laeuft (Message_on und das folgende Sleep liegen im gleichen
                 * Tick, op_message_on gibt 1 = weiter zurueck). Das Budget der
                 * Aufnahme ist also gefunden-1 Bilder. */
                if (gefunden > 0 && (abstand < 0 || gefunden - 1 < abstand)) {
                    abstand = gefunden - 1;
                    killer  = (int)g_scd.message_id;
                }
            }

            const char *riegel;
            if (standzeit < 0)                riegel = "?";
            else if (abstand < 0)             riegel = "egal";
            else if (rid == 0x1170 || rid == 0x1240) riegel = "JA(Volltext)";
            /* message_fsm_active faellt im msg-Tick des Bildes F+Standzeit-1
             * (main.c:4409 laeuft NACH scd_vm_tick:4284). Der naechste
             * Message_on laeuft im VM-Teil von Bild F+Abstand. Der Riegel
             * greift also genau dann, wenn Abstand >= Standzeit. */
            else if (standzeit > abstand)     riegel = "NEIN";
            else                              riegel = "JA";

            char anm[160]; anm[0] = 0;
            if (abstand > 0) {
                double f = bilder - (double)abstand;
                snprintf(anm, sizeof anm, "%s %.1f B, Killer main%02d%s",
                         f > 0 ? "FEHLEN" : "Reserve", f > 0 ? f : -f,
                         killer, defekt ? "  [data-Groesse defekt]" : "");
            } else {
                snprintf(anm, sizeof anm, "kein weiteres Message_on in 1500 B%s",
                         defekt ? "  [data-Groesse defekt]" : "");
            }
            printf("%04X   main%02d %9.1f %8d %9d  %-11s %s\n",
                   rid, m, bilder, abstand, standzeit, riegel, anm);
        }
        free(b);
    }
    return 0;
}
