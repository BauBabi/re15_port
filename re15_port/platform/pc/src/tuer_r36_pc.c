/* tuer_r36_pc.c — Runde 36 Spur G: Pruefhaken RE15_TUER_ARCHIV (nur Messwerkzeug, keine Spiellogik).
 *
 * RE15_TUER_SEITE (main.c, Runde 31) spielt nur Tuerseiten, die in der Zuordnungstabelle stehen. Fuer
 * die Wahl eines Archivs (Spur G Punkt 1: Fahrstuhl ROOM1080, grosse Tuer ROOM2090) muss man auch
 * Archive/Varianten ansehen koennen, die (noch) keiner Seite zugeordnet sind:
 *
 *   RE15_TUER_ARCHIV="27:0,2A:2,26:0,P26W:3"
 *     NN:V[:B]   RE2-Archiv DOOR<NN>.DO2 (hex), Variante V (var 12), optional Bit 7 (var 14)
 *     Kkkk:V     Port-Archiv shared_assets/RE15DOOR/<Kkkk>.DO2 (Kennung aus gen/re15_tuer_eigen.inc)
 *
 * Jeder Eintrag laeuft durch denselben Laeufer wie im Spiel (door_scene_pc.c tuer_laeufer) mit
 * Seite = 901, 902, ... in der Reihenfolge der Liste - zusammen mit RE15_TUER_BOGEN=<dir> entstehen
 * S901_anfang.ppm / S901_mitte.ppm usw., mit RE15_TUER_SERIE=<dir> alle Bilder. Die Zuordnung Seite ->
 * Archiv schreibt der Haken nach <RE15_TUER_BOGEN>/archiv_liste.txt (die GUI-exe hat kein stderr).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_door_seq.h"
#include "re15_aufschliessen_r36.h"

extern re15_door_seq_anfrage_t g_door_seq_anfrage;

/* Runde 36 Spur G (Punkt 3): den PC-Spieler der Mini-Bank KARTSE.VBS bei der Engine anmelden
 * (engine/src/aufschliessen_r36.c bleibt plattformfrei; PSX/Tests: nicht angemeldet = stumm). */
void re15_audio_re2_karte_se(int se_id);   /* platform/pc/src/audio_pc.c */
void re15_pc_aufschliessen_anmelden(void)
{
    re15_aufschliessen_r36_setze_spieler(re15_audio_re2_karte_se);
}

static int eigen_nach_kennung(const char *k, int n)
{
    for (int i = 1; i <= re15_door_seq_eigen_anzahl(); i++) {
        const re15_tuer_eigen_t *e = re15_door_seq_eigen(i);
        if (e && (int)strlen(e->kennung) == n && strncmp(e->kennung, k, (size_t)n) == 0) return i;
    }
    return 0;
}

int re15_pc_tuer_archiv_test(void)
{
    const char *liste = getenv("RE15_TUER_ARCHIV");
    if (!liste || !*liste) return 0;
    const char *bogen = getenv("RE15_TUER_BOGEN");
    FILE *prot = NULL;
    if (bogen && *bogen) {
        char p[512];
        snprintf(p, sizeof p, "%s/archiv_liste.txt", bogen);
        prot = fopen(p, "w");
    }
    int nr_lauf = 0;
    const char *t = liste;
    while (*t) {
        const char *e = strchr(t, ',');
        int len = e ? (int)(e - t) : (int)strlen(t);
        char tok[32];
        if (len > 0 && len < (int)sizeof tok) {
            memcpy(tok, t, (size_t)len); tok[len] = 0;
            re15_door_seq_anfrage_t q;
            memset(&q, 0, sizeof q);
            q.archiv  = RE15_DOOR_ARCHIV_RE2;
            q.spender = RE15_DOOR_KEIN_SPENDER;
            char *c1 = strchr(tok, ':');
            int ok = 0;
            if (c1) {
                *c1 = 0;
                char *c2 = strchr(c1 + 1, ':');
                if (c2) { *c2 = 0; q.bit7 = (uint8_t)(atoi(c2 + 1) != 0); }
                q.variante = (uint8_t)atoi(c1 + 1);
                int eig = eigen_nach_kennung(tok, (int)strlen(tok));
                if (eig) {
                    const re15_tuer_eigen_t *ed = re15_door_seq_eigen(eig);
                    q.eigen = (uint8_t)eig; q.re2_nr = ed->basis; ok = 1;
                } else {
                    char *ende = NULL;
                    long nr = strtol(tok, &ende, 16);
                    if (ende && *ende == 0 && nr >= 0 && nr <= 0x36) { q.re2_nr = (uint8_t)nr; ok = 1; }
                }
            }
            if (ok) {
                q.tuer_nr = q.re2_nr;               /* var 15 = Payload+12 = Archivnummer (@0x80013e90/98) */
                q.seite   = (uint16_t)(901 + nr_lauf);
                q.aktiv   = 1;
                if (prot) fprintf(prot, "S%03u %s DOOR%02X V%u Bit7 %u%s\n", q.seite, tok, q.re2_nr,
                                  q.variante, q.bit7, q.eigen ? " (Port-Archiv)" : "");
                g_door_seq_anfrage = q;
                re15_door_seq_ausfuehren();
                nr_lauf++;
            } else if (prot) {
                fprintf(prot, "?? %s nicht erkannt\n", tok);
            }
        }
        if (!e) break;
        t = e + 1;
    }
    if (prot) fclose(prot);
    return 1;
}
