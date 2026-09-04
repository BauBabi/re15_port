/* Audio: kein Puffer wird freigegeben, waehrend der Mixer-Thread daraus liest.
 *
 * ⛔ ANLASS — ein ausgelieferter Absturz, zweimal. Nutzer 2026-09-04: "Irgendwie
 * crashed mir jetzt am laufenden Bande die Anwendung weg" und "Aber er stuerzt jetzt
 * auch teilweise im intro schon ab". Windows Error Reporting nannte fuer v0.5.7 UND
 * v0.5.8 dasselbe: Modul re15_pc.exe, Ausnahme 0xc0000005, Fehleroffset 0x13300.
 * Aufgeloest gegen genau das ausgelieferte Binary (ImageBase 0x140000000):
 *
 *   140013300: 41 0f bf 04 4b   movswl (%r11,%rcx,2),%eax   <audio_callback+0x1e0>
 *   140013305: 41 83 f8 1f      cmp    $0x1f,%r8d
 *
 * = `int32_t smp = s_xa.pcm[s_xa.pos];` plus `rem < MIXER_TAIL_FADE_SAMPLES` (32).
 * Der XA-Mixer las aus FREIGEGEBENEM Speicher: re15_voice_load_clip gab bei jedem
 * Raumwechsel den Sprach-Cache frei - ohne Sperre und ohne den Stream zu loesen -,
 * waehrend s_xa.pcm noch in genau diesen Puffer zeigte.
 *
 * ⛔ WARUM EINE QUELLTEXT-PRUEFUNG UND KEINE LAUFZEIT-PROBE. Die Testschiene linkt
 * ausschliesslich re15_engine; audio_pc.c gehoert zur PC-Plattform und ist von keinem
 * Testbinary aus erreichbar. Genau deshalb ist der Fehler durch 267 gruene Tests
 * hindurch in zwei Pakete gelangt. Geprueft wird darum der SPERR-VERTRAG am Text:
 * jede Freigabe eines Puffers, aus dem der Mix-Callback liest, muss zwischen
 * SDL_LockAudioDevice und SDL_UnlockAudioDevice stehen.
 *
 * ⛔ GEGENPROBE, GEFAHREN 2026-09-04: die Schranke wurde gegen den Stand VOR dem Fix
 * gehalten (`git show HEAD:...` in ein Ausweichverzeichnis, Pfad als argv[1]). Ergebnis:
 *   FAIL: der XA-Stream wird geloest, wenn sein Clip freigegeben wird
 *   FAIL: KEINE Freigabe eines Mixer-Puffers ausserhalb der Sperre
 * Sie meldet den Fehler also wirklich, fuer den sie geschrieben ist. Ein Waechter, von
 * dem das nicht gezeigt wurde, ist keiner.
 *
 * Der Test gibt seine ABDECKUNG aus (jede gepruefte Freigabe mit ihrer Sperrtiefe),
 * damit nicht unbemerkt eine Stelle aus dem Suchmuster faellt und die Schiene still
 * gruen bleibt - der Fehler, den es hier schon gab (Memory reai-v2-schiene-abdeckung).
 */
#include <stdio.h>
#include <string.h>

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

/* Puffer, aus denen audio_callback liest. Eine Freigabe, die einen dieser Namen
 * nennt, muss unter der Geraetesperre stehen. */
static const char *MIXER_PUFFER[] = {
    "s_voice_clip", "s_fmv_audio", "vag_pcm", "s->seq"
};
#define N_PUFFER ((int)(sizeof MIXER_PUFFER / sizeof MIXER_PUFFER[0]))

/* Helfer, deren Vertrag lautet "der AUFRUFER haelt die Sperre". Nur mit Begruendung
 * und nur, wenn die Aufrufstellen nachgeprueft sind - sonst waere das ein Schlupfloch.
 *   ss_free_bank: Aufrufer sind re15_bgm_vabw_clear/_apply; deren Aufrufstellen liegen
 *   nachweislich in Sperrbereichen (u.a. Zeile 2921 zwischen Lock 2900 und Unlock 2941),
 *   und der Vertrag steht als Kommentar direkt ueber der Funktion. Nachgeprueft
 *   2026-09-04. */
static const char *AUFRUFER_SPERRT[] = { "ss_free_bank" };
#define N_VERTRAG ((int)(sizeof AUFRUFER_SPERRT / sizeof AUFRUFER_SPERRT[0]))

static int zaehle(const char *zeile, const char *muster)
{
    int n = 0; const char *p = zeile;
    while ((p = strstr(p, muster)) != NULL) { n++; p += strlen(muster); }
    return n;
}

/* Funktionskopf am Zeilenanfang: "static void name(" / "void name(" -> name. */
static void kopf_name(const char *z, char *aus, size_t n)
{
    const char *k = strchr(z, '(');
    const char *e;
    size_t len;
    aus[0] = 0;
    if (!k || z[0] == ' ' || z[0] == '\t' || z[0] == '#' ||
        z[0] == '/' || z[0] == '*' || z[0] == '}') return;
    e = k;
    while (e > z && (e[-1] == ' ' || e[-1] == '\t')) e--;
    k = e;
    while (k > z && (((k[-1] >= 'a' && k[-1] <= 'z') || (k[-1] >= 'A' && k[-1] <= 'Z') ||
                      (k[-1] >= '0' && k[-1] <= '9') || k[-1] == '_'))) k--;
    len = (size_t)(e - k);
    if (len == 0 || len >= n) return;
    memcpy(aus, k, len); aus[len] = 0;
}

/* argv[1] = abweichender Wurzelpfad. Damit laesst sich die Schranke gegen einen
 * ALTEN Stand halten und zeigen, dass sie den Fehler, fuer den sie geschrieben ist,
 * auch wirklich meldet - ein Waechter, der nie ausschlaegt, ist keiner. */
int main(int argc, char **argv)
{
    char pfad[600], zeile[2048], funk[128] = "", kopf[128];
    FILE *f;
    int tiefe = 0, nr = 0, geprueft = 0, ungesperrt = 0, vertrag = 0;
    int negativ = 0, loesung_da = 0, callback_lesestelle = 0;

    printf("=== Audio: Freigaben stehen unter der Geraetesperre ===\n");

    snprintf(pfad, sizeof pfad, "%s/platform/pc/src/audio_pc.c",
             argc > 1 ? argv[1] : RE15_PORT_SRC_DIR);
    f = fopen(pfad, "rb");
    if (!f) { printf("  FAIL: %s nicht lesbar\n", pfad); return 1; }

    while (fgets(zeile, sizeof zeile, f)) {
        int i, treffer = 0;
        nr++;
        { char *k = strchr(zeile, '\n'); if (k) *k = 0; }

        /* Nur ueberschreiben, wenn die Zeile wirklich ein Funktionskopf ist -
         * sonst verloere sich der Name mitten in der Funktion. */
        kopf_name(zeile, kopf, sizeof kopf);
        if (kopf[0]) { strncpy(funk, kopf, sizeof funk - 1); funk[sizeof funk - 1] = 0; }

        /* Die Lesestelle, an der es krachte - sie muss es noch geben, sonst prueft
         * dieser Test eine Funktion, die es nicht mehr gibt. */
        if (strstr(zeile, "s_xa.pcm[s_xa.pos]")) callback_lesestelle = 1;
        /* Die Loesung des Streams beim Verwerfen des Caches. */
        if (strstr(zeile, "s_xa.pcm == s_voice_clip")) loesung_da = 1;

        {
            const char *p = zeile;
            while (*p == ' ' || *p == '\t') p++;
            if (p[0] != '*' && !(p[0] == '/' && (p[1] == '*' || p[1] == '/'))) {
                tiefe += zaehle(zeile, "SDL_LockAudioDevice(s_audio_dev)");
                tiefe -= zaehle(zeile, "SDL_UnlockAudioDevice(s_audio_dev)");
            }
        }
        if (tiefe < 0) negativ = 1;

        if (!strstr(zeile, "free(")) continue;
        for (i = 0; i < N_PUFFER; i++) if (strstr(zeile, MIXER_PUFFER[i])) treffer = 1;
        if (!treffer) continue;

        geprueft++;
        {
            int erlaubt = (tiefe > 0), ist_vertrag = 0;
            for (i = 0; i < N_VERTRAG; i++)
                if (strcmp(funk, AUFRUFER_SPERRT[i]) == 0) ist_vertrag = 1;
            if (ist_vertrag) { erlaubt = 1; vertrag++; }
            printf("  [Abdeckung] Zeile %4d  Sperrtiefe %d  %-16s %s%s\n",
                   nr, tiefe, funk[0] ? funk : "(?)",
                   erlaubt ? "OK" : "UNGESPERRT",
                   ist_vertrag ? " (Vertrag: Aufrufer sperrt)" : "");
            if (!erlaubt) ungesperrt++;
        }
    }
    fclose(f);

    printf("  [Abdeckung] %d Zeilen gelesen, %d Freigaben geprueft "
           "(%d davon per Aufrufer-Vertrag)\n", nr, geprueft, vertrag);

    CHECK("die Datei wurde gelesen und enthaelt Freigaben zum Pruefen", geprueft >= 3);
    CHECK("die Lesestelle s_xa.pcm[s_xa.pos] existiert noch (Test prueft das Richtige)",
          callback_lesestelle);
    CHECK("Sperrtiefe wird nie negativ (Lock/Unlock sind paarig)", !negativ);
    CHECK("Sperrtiefe endet bei 0 (keine offene Sperre)", tiefe == 0);
    CHECK("der XA-Stream wird geloest, wenn sein Clip freigegeben wird", loesung_da);
    CHECK("KEINE Freigabe eines Mixer-Puffers ausserhalb der Sperre", ungesperrt == 0);

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
