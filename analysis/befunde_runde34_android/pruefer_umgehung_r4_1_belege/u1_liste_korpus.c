/* Pruefer UMGEHUNG R4-1, H5: Listen-Dateien durch re15_abgleich_lesen (asset_abgleich.c, ausgelieferte Datei,
 * UNVERAENDERT mituebersetzt). Je Datei eine Zeile: "<datei>\t<rc>\t<n>\t<summe>\t<fehler>".
 * Zusatzmodus "planen <neu> <alt>": Abgleich zweier Listen -> je Eintrag "<pfad>\t<aktion>", danach "weg\t<pfad>". */
#include "asset_abgleich.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *lesen(const char *pfad, size_t *len)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *b = (char *)malloc((size_t)n + 1);
    if (!b) { fclose(f); return NULL; }
    size_t got = fread(b, 1, (size_t)n, f);
    fclose(f);
    *len = got;
    return b;
}

int main(int argc, char **argv)
{
    if (argc == 4 && strcmp(argv[1], "planen") == 0) {
        size_t ln = 0, la = 0;
        char *tn = lesen(argv[2], &ln), *ta = lesen(argv[3], &la);
        re15_abgleich_liste_t neu, alt;
        char f1[256], f2[256];
        if (!tn || !ta) return 3;
        int r1 = re15_abgleich_lesen(&neu, tn, ln, f1, sizeof f1), r2 = re15_abgleich_lesen(&alt, ta, la, f2, sizeof f2);
        if (r1 || r2) { printf("liste ungueltig: %d %s / %d %s\n", r1, f1, r2, f2); return 1; }
        re15_abgleich_plan_t p;
        if (re15_abgleich_planen(&p, &neu, &alt)) return 4;
        static const char *nm[] = { "BEHALTEN", "GEAENDERT", "NEU", "PRUEFEN" };
        for (size_t i = 0; i < neu.n; i++) printf("%s\t%s\n", neu.e[i].pfad, nm[p.aktion[i] & 3]);
        for (size_t j = 0; j < p.n_weg; j++) printf("weg\t%s\n", p.weg[j]);
        re15_abgleich_plan_frei(&p);
        re15_abgleich_frei(&neu);
        re15_abgleich_frei(&alt);
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        size_t len = 0;
        char *t = lesen(argv[i], &len);
        if (!t) { printf("%s\tLESEFEHLER\n", argv[i]); continue; }
        re15_abgleich_liste_t l;
        char fehler[256];
        int rc = re15_abgleich_lesen(&l, t, len, fehler, sizeof fehler);
        printf("%s\t%d\t%zu\t%lld\t%s\n", argv[i], rc, l.n, l.summe, rc ? fehler : "-");
        re15_abgleich_frei(&l);
        free(t);
    }
    return 0;
}
