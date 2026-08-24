/* Siehe asset_selftest_pc.h fuer das WARUM. NUR DIAGNOSE — keine Spiel-Logik, keine
 * Aenderung an der Aufloeser-Kette. Dieses Modul RUFT asset_root_pc.c nur auf. */
#include "asset_selftest_pc.h"
#include "asset_root_pc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Welche Wurzelliste eine Datei bedient — dieselbe Aufteilung wie in asset_root_pc.h. */
typedef enum { K_CD = 0, K_SHARED, K_BASE, K_RE2 } probe_kind_t;

typedef struct {
    probe_kind_t kind;
    const char  *rel;
    const char  *breaks;   /* Was der Nutzer VERLIERT, wenn diese Datei nicht gefunden wird. */
} probe_t;

/* -------------------------------------------------------------------------------------------
 * DIE KRITIKLISTE.
 *
 * Aufnahmekriterium: fehlt die Datei zur Laufzeit, sieht oder hoert der Nutzer den Verlust
 * SOFORT — und zwar STILL, ohne Absturz und ohne Fehlermeldung im Spiel. Genau diese Klasse
 * hat den 0.3.19-Report ausgeloest. Die Spalte `breaks` ist keine Doku-Deko: sie steht in der
 * Ausgabe, damit ein Nutzer-Log allein schon sagt, WAS fehlt und WARUM das auffiel.
 *
 * Bewusst NICHT drin: alles, dessen Fehlen ohnehin laut knallt (der Startraum-RDT z.B. bleibt
 * trotzdem drin, weil er die billigste Gegenprobe fuer "CD-Wurzel stimmt ueberhaupt" ist),
 * und alles, was nur eine spaete Stage betrifft — das Gate soll schnell und eindeutig sein.
 * ------------------------------------------------------------------------------------------- */
static const probe_t k_probes[] = {
    /* --- DIE SPIELSCHRIFT. Der 0.3.19-Fehler selbst. Fehlt TEX.TIM, gibt es NULL Spieltext:
     *     keine CONFIG-Labels, keine Untertitel, keine Dialoge, keine Item-Namen. Ein
     *     6x8-Ersatzfont existiert auf Nutzerwunsch nicht mehr, es bleibt also nichts uebrig. */
    { K_CD,     "DATA/TEX.TIM",        "Spielschrift — ohne sie KEIN Text: CONFIG-Labels, Untertitel, Dialoge, Item-Namen" },
    { K_CD,     "BIN/DEBUG.BIN",       "Glyphen-Breiten @0x4416 — ohne sie ist jeder Text falsch umbrochen" },

    /* --- Front-End: Titel, Spielerwahl, CONFIG-Bildschirm. */
    { K_CD,     "DATA/TITLEU.TIM",     "Titelbild" },
    { K_CD,     "DATA/TMOJI.TIM",      "Titel-Menuetexte" },
    { K_CD,     "DATA/C_BACK2.TIM",    "CONFIG-Hintergrund" },
    { K_CD,     "DATA/CONFIG.TIM",     "CONFIG-Bildschirm (Controller-Grafik)" },
    { K_CD,     "DATA/SELECTH.TIM",    "Spielerauswahl-Hintergrund" },

    /* --- Inventar/Status. Eigener Lader (inv_render_pc.c), eigene fruehere Wurzelliste. */
    { K_CD,     "DATA/ITEMALL.PIX",    "Item-Symbole im Inventar" },
    { K_CD,     "ITEM/ITPS.ITP",       "Item-Vorschaubilder (CHECK-Ansicht)" },
    { K_CD,     "DATA/ST_00.TIM",      "Status-Bildschirm" },
    { K_CD,     "ITEM/STPIC_00.TIM",   "Status-Portrait" },

    /* --- Raum, Hintergrund, Modelle. Beweist, dass die CD-Wurzel als Ganzes greift. */
    { K_CD,     "STAGE1/ROOM1240.RDT", "Startraum" },
    { K_CD,     "BSS/ROOM1240/BG00.BSS", "Raum-Hintergrundbild (sonst schwarz)" },
    { K_CD,     "PLD/PL00.MD1",        "Spielermodell" },
    { K_CD,     "EMD/CDEMD0.EMS",      "Gegnermodell-Archiv" },

    /* --- Ton. SOUND/*.BGM lief frueher ueber einen Leser OHNE Paket-Wurzel: im direkt
     *     gestarteten 0.3.19-Paket war die Musik deshalb STUMM ("[bgm] MAIN28.BGM nicht
     *     lesbar" in der mitgelieferten debug.log). */
    { K_CD,     "SOUND/MAIN28.BGM",    "Musik (Startraum) — sonst voellig stumm" },
    { K_CD,     "SOUND/CORE00.EDH",    "Kern-Soundeffektbank" },
    { K_CD,     "DATA/CORE00.ESP",     "globale Effektbank (Treffer-Effekte)" },

    /* --- Effekt-Texturen. Liegen als GESCHWISTER von PSX/, nicht darin — genau daran ist
     *     jede frueher cwd-relative Kette gescheitert (sie hing "extracted_fx/" an die
     *     PSX-Wurzel und suchte in shared_assets/PSX/extracted_fx/, wo nichts liegt). */
    { K_SHARED, "extracted_fx/effect0_blood.tim",  "Blut-Sprites" },
    { K_SHARED, "extracted_fx/effect2_muzzle.tim", "Muendungsfeuer" },
    { K_SHARED, "extracted_fx/effect3_smoke.tim",  "Rauch" },
    { K_SHARED, "extracted_fx/effect4_shell.tim",  "Patronenhuelsen" },
    { K_SHARED, "extracted_fx/effect8_fire.tim",   "Feuer" },

    /* --- OPTIONS -> AI = RE2. Fehlt eines der beiden, faellt die Option STILL auf RE1.5
     *     zurueck — der Nutzer waehlt etwas aus, das nichts tut. */
    { K_RE2,    "CDEMD0.EMS",          "RE2-KI-Option: Gegnermodelle (sonst stille Rueckkehr zu RE1.5)" },
    { K_RE2,    "ENEMSE.VBS",          "RE2-KI-Option: Gegner-Soundeffekte" },

    /* --- Sprachausgabe. Liegt NEBEN shared_assets, in der Paketwurzel. */
    { K_BASE,   "synchro/STAGE1/room1170/main00.wav", "Sprachausgabe (Helipad-Intro)" },
};
#define N_PROBES ((int)(sizeof k_probes / sizeof k_probes[0]))

static const char *kind_name(probe_kind_t k)
{
    switch (k) {
        case K_CD:     return "cd";
        case K_SHARED: return "shared";
        case K_BASE:   return "base";
        case K_RE2:    return "re2";
    }
    return "?";
}

/* Die i-te Wurzel der jeweiligen Art. Fuer K_RE2 ist es die Shared-Wurzel; das "RE2/" haengt
 * der Aufrufer an (siehe probe_resolve). NULL = keine i-te Wurzel mehr. */
static const char *kind_root(probe_kind_t k, int i)
{
    switch (k) {
        case K_CD:     return re15_pc_cd_root(i);
        case K_SHARED: return re15_pc_shared_root(i);
        case K_BASE:   return re15_pc_base_root(i);
        case K_RE2:    return re15_pc_shared_root(i);
    }
    return NULL;
}

/* Die Datei ueber ihre Wurzelliste suchen und den GEWINNENDEN Vollpfad zurueckgeben.
 * BEWUSST ein eigenes fopen statt re15_pc_read_cd(): der Selbsttest muss melden, WELCHE
 * Wurzel gewonnen hat — und genau das gibt der Leser nicht heraus. Er laedt die Datei auch
 * nicht ein (CDEMD0.EMS allein sind 4,7 MB); ein oeffnen + Groesse reicht als Beweis. */
static int probe_resolve(const probe_t *p, char *path, size_t cap, long *out_size)
{
    for (int i = 0;; i++) {
        const char *r = kind_root(p->kind, i);
        if (!r) break;
        if (p->kind == K_RE2) snprintf(path, cap, "%s/RE2/%s", r, p->rel);
        else                  snprintf(path, cap, "%s/%s",     r, p->rel);
        FILE *f = fopen(path, "rb");
        if (!f) continue;
        fseek(f, 0, SEEK_END);
        long sz = ftell(f);
        fclose(f);
        if (sz <= 0) continue;              /* 0-Byte-Leiche zaehlt als fehlend */
        if (out_size) *out_size = sz;
        return 1;
    }
    path[0] = '\0';
    return 0;
}

/* Bericht doppelt ausgeben: stderr landet in der debug.log (immer da, auch beim Doppelklick),
 * stdout nur, wenn eine Eltern-Konsole angehaengt werden konnte. Das Gate liest die debug.log. */
static void emit(FILE *a, FILE *b, const char *line)
{
    if (a) fputs(line, a);
    if (b && b != a) fputs(line, b);
}

void re15_pc_asset_selftest_maybe_run(void)
{
    const char *sw = getenv("RE15_ASSET_SELFTEST");
    if (!sw || !sw[0] || (sw[0] == '0' && sw[1] == '\0')) return;

    char line[1024];
    char path[1024];
    int ok = 0, missing = 0;

    snprintf(line, sizeof line, "[selftest] exe-dir = %s\n",
             re15_pc_exe_dir()[0] ? re15_pc_exe_dir() : "(unbekannt)");
    emit(stderr, stdout, line);
    for (int i = 0; ; i++) {
        const char *r = re15_pc_cd_root(i);
        if (!r) { if (i == 0) emit(stderr, stdout, "[selftest] cd-root = (KEINE)\n"); break; }
        snprintf(line, sizeof line, "[selftest] cd-root[%d] = %s\n", i, r);
        emit(stderr, stdout, line);
    }

    for (int i = 0; i < N_PROBES; i++) {
        const probe_t *p = &k_probes[i];
        long sz = 0;
        if (probe_resolve(p, path, sizeof path, &sz)) {
            ok++;
            /* Der Vollpfad ist der eigentliche Beweis: nur so sieht das Gate, ob die Datei
             * aus dem PAKET kam oder aus einem Repo nebenan. */
            snprintf(line, sizeof line, "[selftest] OK      %-6s %-36s %9ld  <- %s\n",
                     kind_name(p->kind), p->rel, sz, path);
        } else {
            missing++;
            snprintf(line, sizeof line, "[selftest] MISSING %-6s %-36s  -- %s\n",
                     kind_name(p->kind), p->rel, p->breaks);
        }
        emit(stderr, stdout, line);
    }

    snprintf(line, sizeof line, "[selftest] RESULT ok=%d missing=%d\n", ok, missing);
    emit(stderr, stdout, line);
    fflush(stderr);
    fflush(stdout);
    exit(missing ? 2 : 0);
}
