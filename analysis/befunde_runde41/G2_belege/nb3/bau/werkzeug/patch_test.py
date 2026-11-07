# -*- coding: utf-8 -*-
"""NB3-Bau: Riegel nb3(_elza), Anpassung gang/re2, exe-Auswerter obenlog/haenglog in test_r41_gatorfix.c."""
import io, sys
P = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/tests/unit/test_r41_gatorfix.c"
s = io.open(P, encoding="utf-8", newline="").read()
crlf = "\r\n" in s
s = s.replace("\r\n", "\n")
def rep(old, new, n=1):
    global s
    c = s.count(old)
    if c != n:
        print("FUNDSTELLE %d statt %d:\n%s" % (c, n, old[:300])); sys.exit(1)
    s = s.replace(old, new)

# g2r_alle: Staende 0/1 = vor NB3
rep("    re15_gr2_rw_alt_setzen(stand == 0); re15_gr2_v4_alt_setzen(stand <= 1);\n",
    "    re15_gr2_rw_alt_setzen(stand == 0); re15_gr2_v4_alt_setzen(stand <= 1);\n"
    "    re15_gator_boss_nb3_alt_setzen(stand <= 1);   /* G2-RE2 NB3: Staende nb5/f942 = vor NB3 (a661eb8aa), neu/ohne = Bau */\n")
rep("re15_gr2_rw_alt_setzen(0); re15_gr2_v4_alt_setzen(0); return 0; }",
    "re15_gr2_rw_alt_setzen(0); re15_gr2_v4_alt_setzen(0); re15_gator_boss_nb3_alt_setzen(0); return 0; }")
rep("    re15_gr2_rw_alt_setzen(0); re15_gr2_v4_alt_setzen(0);\n    return 1;\n}",
    "    re15_gr2_rw_alt_setzen(0); re15_gr2_v4_alt_setzen(0); re15_gator_boss_nb3_alt_setzen(0);\n    return 1;\n}")

# teil_re2: Rampe-Riegel nach Vorgabe A
rep("""        PRUEF(t2318 > t2318_o, "Rampe-x2318 (ohne Kanister fast sicher): Treffer mit Kanister %d > ohne Kanister %d", t2318, t2318_o);
        int r0 = 0; for (int i = 0; i < 16; i++) if (M[2][i].bilder && k_re2_lagen[i].y != 0) r0 += M[2][i].rammen;
        PRUEF(r0 == 0, "Gegenprobe ohne Kanister: kein Rammen auf der Rampe (%d) - die Querung mit Rammen gehoert zum Kanister (Auftrag)", r0);""",
"""        /* G2-RE2 NB3 (Vorgabe A, Nutzer 2026-11-06 "den Spieler oben auf der Rampe auch rammen koennen ... darf halt oben nicht sicher sein"): die Figur oben
         * wird mit und ohne Kanister getroffen und gerammt - vor NB3 war Rampe-x2318 ohne Kanister fast sicher (3 Treffer in 1800 Bildern, 0 Rammen). */
        PRUEF(t2318 >= 1 && t2318_o >= 1, "NB3 Rampe-x2318: Treffer mit Kanister %d und ohne Kanister %d (je >= 1; vor NB3 ohne Kanister 3, Riegel 'mit > ohne')", t2318, t2318_o);
        int r0 = 0; for (int i = 0; i < 16; i++) if (M[2][i].bilder && k_re2_lagen[i].y != 0) r0 += M[2][i].rammen;
        PRUEF(r0 >= 1, "NB3 (Vorgabe A): Rammen auch ohne Kanister auf der Rampe (%d; vor NB3 0 = Gegenprobe des Auftrags NB2)", r0);""")

# teil_gang: Lagen ausserhalb der NB3-Baender, Stand a661eb8aa = Pruefhaken
rep("""    re15_gr2_rw_alt_setzen(alt); re15_gator_boss_tempo_alt_setzen(tempo);
    re15_actor_t *g = szene_bis_kampf(elza);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int k = 0;
    if (g) {
        static const int32_t lage[3][3] = { { -6000, 0, -16000 }, { 6732, 0, -13687 }, { 4428, -1800, -16300 } };""",
"""    re15_gr2_rw_alt_setzen(alt); re15_gator_boss_tempo_alt_setzen(tempo); re15_gator_boss_nb3_alt_setzen(alt);
    re15_actor_t *g = szene_bis_kampf(elza);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int k = 0;
    if (g) {
        /* G2-RE2 NB3: Lagen AUSSERHALB der freigegebenen Aenderungen (Figur oben, Wasser naeher als 1800 vor einer Blockkante): Westkanal,
         * Nordbecken 2500 vor der Rampen-Nordkante (vorher (6732,0,-13687) = 813 vor der Kante, Vorgabe B), Suedbecken 2500 vor der Plattform-
         * Suedkante (vorher (4428,-1800,-16300) oben auf der Rampe, Vorgabe A); die beiden alten Lagen prueft der Riegel nb3 als Gegenprobe
         * (sie MUESSEN zwischen den Staenden abweichen). alt = Stand vor G2-RE2 (NB5) und vor NB3 (a661eb8aa) per Pruefhaken. */
        static const int32_t lage[3][3] = { { -6000, 0, -16000 }, { 6732, 0, -12000 }, { 1000, 0, -22500 } };""")
rep("""    re15_gr2_rw_alt_setzen(0); re15_gator_boss_tempo_alt_setzen(0);
    return k;
}""",
"""    re15_gr2_rw_alt_setzen(0); re15_gator_boss_tempo_alt_setzen(0); re15_gator_boss_nb3_alt_setzen(0);
    return k;
}""")
rep("""    printf("== gang%s (G2-RE2: Gang ohne Kanister gleich NB5) ==\\n", elza ? "_elza" : "");""",
    """    printf("== gang%s (G2-RE2: Gang ohne Kanister gleich NB5; NB3: Lagen ausserhalb der Baender, alter Stand = Pruefhaken rw_alt + nb3_alt) ==\\n", elza ? "_elza" : "");""")

# Neuer Block vor main
neu = r'''
/* ==== Runde 41 Spur G2-RE2 NB3 - BAUER: Riegel nb3(_elza) und exe-Auswerter obenlog / haenglog ==========================================================
 * Dossier G2_gatorfix.md "G2-RE2 NB3 (Rampe oben, Plattform-Suedkante)", Messblatt G2_MESSBLATT_NB3.md Abschnitt 4 (Riegel A1/A2/B1/B2/B3/B4).
 * nb3: (A1) 18 Rampen-Lagen oben x 3600 Bilder, mit und ohne Kanister: in jeder Lage erster Treffer <= 600 Bilder und laengste Pause ohne Treffer <= 600;
 *      (B1) 11 Wasser-Lagen vor der Rampen-/Plattform-Suedkante x 3600, mit und ohne: Haenge-Fenster (RW-Mass) 0; (B3) ohne Fortschritt zur Figur <= 450;
 *      dazu (B1b, PORT-WAHL mit der A1-Schranke) laengste Pause ohne Treffer in den Wasser-Lagen <= 600 - der erste NB3-Bau liess ihn an der Klemmlinie
 *      784 vor der Figur stehen, 104 s ohne Biss (Maulpunkt im Block), das RW-Mass sah es nicht (Kopf < 4500).
 *      Gegenproben Stand a661eb8aa (Pruefhaken re15_gator_boss_nb3_alt_setzen): R2318/-16300 ohne Treffer in 3600 Bildern (Messblatt 1.2 "KEIN TREFFER"),
 *      W4200/-18800/c2 >= 1 Haenge-Fenster (Messblatt 2.2: 3), und die aus dem Riegel gang entfernten Lagen (6732,0,-13687) / (4428,-1800,-16300)
 *      weichen zwischen den Staenden ab (die Aenderung liegt genau im Band vor der Kante bzw. oben).
 * obenlog <lauf>: exe-Mass A2 (Messblatt 1.3, G2_belege/nb3/werkzeug/oben3.py als C): Figur oben = climb.log y < -900 im Grundriss Rampe/Plattform; Treffer =
 *      hp-Flanke < 100 in state.log (letzte Folge); Art aus gator_trag.log (ph 7 = Querung: RAMMEN-Zeile in gator_boss.log = Rammen, sonst CROSS-Biss; 5 =
 *      Hochbiss; 3 = Biss); je Oben-Abschnitt erster Treffer und Pausen. Riegel: erster Treffer <= 600 Bilder nach dem ersten Oben-Bild, keine Pause > 600,
 *      solange sie oben ist (ein Abschnitt bis zum Laufende zaehlt mit seinem Rest; ein verlassener Abschnitt ohne Treffer darf nicht laenger als 600 sein).
 * haenglog <lauf> [mit]: exe-Mass B2/B3 (werkzeug/haeng3.py als C): Haenge-Fenster wie gator_rw_r41.c re15_gr2_rw_nachtakt (b) aus gator_trag.log (150 Bilder
 *      Phase 2 ohne Biss/Treffer, Wurzel-Kasten < 2500, Kopf 3236 vor der Wurzel > 4500 von der Figur, netto < 600 in 60 Bildern), laengste Zeit ohne
 *      Fortschritt (Mass mass_v4.py); "mit": dazu 0 Zeilen "G2RE2 HAENGT" / "RUECKWAERTS an" / "TAUCHEN an" in gator_boss.log. Riegel: Fenster 0, <= 450. */
#define NB3_PAUSE_MAX 600   /* Messblatt NB3 Abschnitt 4 (A1): Pause nach der Querung 300 (NB5) + laengste Querung 240 + Hochbiss-Fenster 45 = 585, aufgerundet */
#define NB3_OHNE_MAX  450   /* Messblatt NB3 Abschnitt 4 (B3): 3 Haenge-Fenster = die Zeit, nach der der Rueckfall (Rueckwaerts 150, Tauchen 150 + 30) aufgegeben haette */
typedef struct { int nl, schlecht, erst_max, pause_max, haeng_sum, haeng_max, ohne_max, treffer_min, rammen; char erst_wo[32], pause_wo[32], haeng_wo[32], ohne_wo[32]; } nb3_sum_t;
static void nb3_raster(int elza, int stand, int bilder, int oben, const int32_t (*lagen)[3], int nl, nb3_sum_t *S, int zeilen)
{
    memset(S, 0, sizeof *S); S->treffer_min = 1 << 30; S->erst_max = -1;
    strcpy(S->erst_wo, "-"); strcpy(S->pause_wo, "-"); strcpy(S->haeng_wo, "-"); strcpy(S->ohne_wo, "-");
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int i = 0; i < nl; i++) {
        char name[32];
        if (oben) snprintf(name, sizeof name, "R%d/%d", lagen[i][0], lagen[i][1]);
        else      snprintf(name, sizeof name, "W%d/%d/c%d", lagen[i][0], lagen[i][1], lagen[i][2]);
        re15_actor_t *g = szene_bis_kampf(elza);
        if (!g || !g2_kanister_ins_maul(g, pl)) { printf("    %s: kein Tragen\n", name); continue; }
        if (stand == 3) re15_gr2_kanister_entfernen();
        int cam = lagen[i][2];
        if (oben) { g2_lage_t gl = { name, lagen[i][0], -1800, lagen[i][1] }; cam = g2_lage_kamera(&gl, 0); if (cam < 0) cam = 2; }
        static nb3_treffer_t tr[512]; nb3_m_t m;
        const int ramm0 = re15_gator_boss_rammen();
        nb3_lauf(g, pl, name, lagen[i][0], oben ? -1800 : 0, lagen[i][1], cam, bilder, stand, NULL, tr, 512, &m);
        nb3_druck(name, cam, &m, tr, zeilen);
        const int pause = nb3_pause(&m, tr), erst = m.ntr ? tr[0].t : m.n;
        S->nl++; S->rammen += re15_gator_boss_rammen() - ramm0;
        if (!m.ntr) S->schlecht++;
        if (erst > S->erst_max) { S->erst_max = erst; snprintf(S->erst_wo, sizeof S->erst_wo, "%s", name); }
        if (pause > S->pause_max) { S->pause_max = pause; snprintf(S->pause_wo, sizeof S->pause_wo, "%s", name); }
        S->haeng_sum += m.haeng_n;
        if (m.haeng_n > S->haeng_max) { S->haeng_max = m.haeng_n; snprintf(S->haeng_wo, sizeof S->haeng_wo, "%s", name); }
        if (m.ohne_max > S->ohne_max) { S->ohne_max = m.ohne_max; snprintf(S->ohne_wo, sizeof S->ohne_wo, "%s", name); }
        if (m.ntr < S->treffer_min) S->treffer_min = m.ntr;
    }
    if (S->treffer_min == 1 << 30) S->treffer_min = 0;
}
/* eine Gang-Lage wie g2r_ohne_lauf (Stand alt = Pruefhaken rw_alt + nb3_alt), n Bilder, Rueckgabe = Bilder bis zum Tragen */
static int nb3_gang_lage(int elza, int alt, const int32_t *lage, int32_t (*z)[6], int n)
{
    re15_gr2_rw_alt_setzen(alt); re15_gator_boss_nb3_alt_setzen(alt);
    re15_actor_t *g = szene_bis_kampf(elza);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int k = 0;
    if (g) for (int t = 0; t < n; t++) {
        pl->x = lage[0]; pl->y = lage[1]; pl->z = lage[2]; pl->hp = 100; pl->hit_react = 0;
        re15_collision_set_band(re15_collision_band_from_y(lage[1]));
        frame(0, 0);
        if (re15_gr2_traegt()) break;
        z[k][0] = g->x; z[k][1] = g->z; z[k][2] = g->rot_y & 0xfff; z[k][3] = g->motion; z[k][4] = g->anim_frame; z[k][5] = g->y; k++;
    }
    re15_gr2_rw_alt_setzen(0); re15_gator_boss_nb3_alt_setzen(0);
    return k;
}
static void teil_nb3(int elza)
{
    printf("== nb3%s (G2-RE2 NB3: Rampe oben nicht sicher (Vorgabe A), kein Haenger vor der Suedkante (Vorgabe B) - Nutzerentscheid 2026-11-06) ==\n", elza ? "_elza" : "");
    const char *bs = getenv("G2_NB3_BILDER"); const int bilder = bs ? atoi(bs) : 3600;
    static const int32_t R[18][3] = {
        { 2318, -17600, -1 }, { 2800, -17600, -1 }, { 3350, -17600, -1 }, { 4428, -17600, -1 }, { 5500, -17600, -1 }, { 6500, -17600, -1 },
        { 2318, -16300, -1 }, { 2800, -16300, -1 }, { 3350, -16300, -1 }, { 4428, -16300, -1 }, { 5500, -16300, -1 }, { 6500, -16300, -1 },
        { 2318, -15000, -1 }, { 2800, -15000, -1 }, { 3350, -15000, -1 }, { 4428, -15000, -1 }, { 5500, -15000, -1 }, { 6500, -15000, -1 } };
    static const int32_t W[11][3] = {
        { 3000, -18800, 0 }, { 4073, -19322, 0 }, { 4252, -19953, 0 }, { 3000, -18800, 2 }, { 4200, -18800, 2 }, { 3600, -18750, 2 },
        { 6400, -18700, 2 }, { 2318, -18568, 2 }, { 4428, -18951, 2 }, { 1000, -20700, 2 }, { 3000, -21000, 0 } };
    nb3_sum_t A[2], B[2];
    for (int k = 0; k < 2; k++) {
        const int stand = k == 0 ? 2 : 3; const char *w = stand == 2 ? "mit" : "ohne";
        printf("  -- Rampe oben (A1), %s Kanister, %d Bilder je Lage --\n", w, bilder);
        nb3_raster(elza, stand, bilder, 1, R, 18, &A[k], 2);
        printf("    SUMME A1 %s Kanister: %d von %d Lagen ohne Treffer; spaetester erster Treffer t%d (%s); laengste Pause ohne Treffer %d B. = %.1f s (%s); Rammen %d; wenigste Treffer je Lage %d\n",
               w, A[k].schlecht, A[k].nl, A[k].erst_max, A[k].erst_wo, A[k].pause_max, A[k].pause_max / 30.0, A[k].pause_wo, A[k].rammen, A[k].treffer_min);
        printf("  -- Wasser vor der Suedkante (B1/B3), %s Kanister --\n", w);
        nb3_raster(elza, stand, bilder, 0, W, 11, &B[k], 2);
        printf("    SUMME B1 %s Kanister: Haenge-Fenster %d (hoechstens %d je Lage, %s); ohne Fortschritt max %d B. = %.1f s (%s); laengste Pause ohne Treffer %d B. = %.1f s (%s); wenigste Treffer je Lage %d; Rammen %d\n",
               w, B[k].haeng_sum, B[k].haeng_max, B[k].haeng_wo, B[k].ohne_max, B[k].ohne_max / 30.0, B[k].ohne_wo, B[k].pause_max, B[k].pause_max / 30.0, B[k].pause_wo, B[k].treffer_min, B[k].rammen);
    }
    for (int k = 0; k < 2; k++) {
        const char *w = k == 0 ? "mit" : "ohne";
        PRUEF(A[k].nl == 18 && A[k].schlecht == 0 && A[k].erst_max <= NB3_PAUSE_MAX && A[k].pause_max <= NB3_PAUSE_MAX,
              "A1 %s Kanister: in jeder der 18 Rampen-Lagen erster Treffer <= %d Bilder und laengste Pause ohne Treffer <= %d (ohne Treffer %d, spaetester erster t%d %s, Pause %d %s)",
              w, NB3_PAUSE_MAX, NB3_PAUSE_MAX, A[k].schlecht, A[k].erst_max, A[k].erst_wo, A[k].pause_max, A[k].pause_wo);
        PRUEF(A[k].rammen >= 1, "A1 %s Kanister: Rammen auf der Rampe >= 1 (%d) - 'den Spieler oben auf der Rampe auch rammen koennen'", w, A[k].rammen);
        PRUEF(B[k].nl == 11 && B[k].haeng_sum == 0, "B1 %s Kanister: Haenge-Fenster (RW-Mass) 0 in jeder der 11 Wasser-Lagen (%d, hoechstens %d in %s)", w, B[k].haeng_sum, B[k].haeng_max, B[k].haeng_wo);
        PRUEF(B[k].ohne_max <= NB3_OHNE_MAX, "B3 %s Kanister: laengste Zeit ohne Fortschritt zur Figur <= %d Bilder (%d = %.1f s, %s)", w, NB3_OHNE_MAX, B[k].ohne_max, B[k].ohne_max / 30.0, B[k].ohne_wo);
        PRUEF(B[k].pause_max <= NB3_PAUSE_MAX, "B1b %s Kanister: laengste Pause ohne Treffer in den Wasser-Lagen <= %d (%d, %s) - kein Stehen an der Klemmlinie ohne Biss", w, NB3_PAUSE_MAX, B[k].pause_max, B[k].pause_wo);
    }
    PRUEF(A[0].erst_max == A[1].erst_max && A[0].pause_max == A[1].pause_max && B[0].haeng_sum == B[1].haeng_sum && B[0].pause_max == B[1].pause_max,
          "mit = ohne Kanister (Vorschlag 4) in den Kennzahlen: erster t%d/t%d, Pause oben %d/%d, Fenster %d/%d, Pause Wasser %d/%d",
          A[0].erst_max, A[1].erst_max, A[0].pause_max, A[1].pause_max, B[0].haeng_sum, B[1].haeng_sum, B[0].pause_max, B[1].pause_max);
    {   /* Gegenproben Stand a661eb8aa (Pruefhaken), ohne Kanister */
        static const int32_t Rg[1][3] = { { 2318, -16300, -1 } }, Wg[1][3] = { { 4200, -18800, 2 } };
        nb3_sum_t Ag, Bg;
        printf("  -- Gegenprobe Stand a661eb8aa (Pruefhaken nb3_alt), ohne Kanister --\n");
        re15_gator_boss_nb3_alt_setzen(1);
        nb3_raster(elza, 3, bilder, 1, Rg, 1, &Ag, 1);
        nb3_raster(elza, 3, bilder, 0, Wg, 1, &Bg, 1);
        re15_gator_boss_nb3_alt_setzen(0);
        PRUEF(Ag.schlecht == 1 || Ag.erst_max > NB3_PAUSE_MAX, "Gegenprobe a661eb8aa R2318/-16300: kein Treffer in %d Bildern (Messblatt 1.2) bzw. erster > %d (ohne Treffer %d, erster t%d) - rot ohne den Bau",
              bilder, NB3_PAUSE_MAX, Ag.schlecht, Ag.erst_max);
        PRUEF(Bg.haeng_sum >= 1, "Gegenprobe a661eb8aa W4200/-18800/c2: Haenge-Fenster >= 1 (%d; Messblatt 2.2: 3) - rot ohne den Bau", Bg.haeng_sum);
    }
    {   /* Gegenprobe: die beiden aus dem Riegel gang entfernten Lagen weichen zwischen den Staenden ab */
        enum { N = 900 }; static int32_t za[N][6], zb[N][6];
        static const int32_t lg[2][3] = { { 6732, 0, -13687 }, { 4428, -1800, -16300 } };
        for (int li = 0; li < 2; li++) {
            const int na = nb3_gang_lage(elza, 1, lg[li], za, N), nb = nb3_gang_lage(elza, 0, lg[li], zb, N);
            int diff = 0, erst = -1;
            for (int i = 0; i < na && i < nb; i++) if (memcmp(za[i], zb[i], sizeof za[i])) { diff++; if (erst < 0) erst = i; }
            printf("    Gang-Lage (%d,%d,%d): Stand a661eb8aa %d Bilder, neu %d Bilder, abweichend %d (erste Abweichung Bild %d)\n", lg[li][0], lg[li][1], lg[li][2], na, nb, diff, erst);
            PRUEF(na >= N * 9 / 10 && diff > 0, "Gegenprobe Gang-Lage (%d,%d,%d): weicht zwischen a661eb8aa und neu ab (%d von %d Bildern) - die Aenderung liegt im Band vor der Kante bzw. oben", lg[li][0], lg[li][1], lg[li][2], diff, na);
        }
    }
}
/* ---- exe-Auswerter (Logs eines Laufordners) ---- */
typedef struct { int f, px, pz, hp, cam, y, gx, gz, rot, gmo, gaf; } nb3_st_t;
static nb3_st_t s_nb3_st[40000];
static int nb3_state_lesen(const char *dir)   /* LETZTE Folge der state.log (F faellt zurueck = neuer Besuch); climb.log T-Zeile i = state.log F-Zeile i (Figur-y) */
{
    char pf[1024]; static char z[16384];
    static int ys[120000]; int ny = 0;
    snprintf(pf, sizeof pf, "%s/climb.log", dir);
    FILE *f = fopen(pf, "r");
    if (f) { while (fgets(z, sizeof z, f)) { if (z[0] != 'T') continue; const char *p = strstr(z, "pos=("); int x = 0, y = 0, w = 0;
                                             if (ny < 120000) ys[ny++] = (p && sscanf(p, "pos=(%d,%d,%d)", &x, &y, &w) == 3) ? y : 0; } fclose(f); }
    snprintf(pf, sizeof pf, "%s/state.log", dir);
    if (!(f = fopen(pf, "r"))) { printf("state.log fehlt: %s\n", pf); return -1; }
    int i = 0, n = 0;
    while (fgets(z, sizeof z, f)) {
        int fr; if (z[0] != 'F' || sscanf(z, "F%d ", &fr) != 1) continue;
        const int idx = i++;
        if (n > 0 && fr < s_nb3_st[n - 1].f) n = 0;
        nb3_st_t r; memset(&r, 0, sizeof r); r.f = fr; r.cam = -1; r.gmo = -1; r.gaf = -1;
        const char *pl = strstr(z, " PL(");
        if (!pl || sscanf(pl, " PL(%d,%d,rot=%*d,hp=%d)", &r.px, &r.pz, &r.hp) != 3) continue;
        { const char *c = strstr(z, " cam="); if (c) r.cam = atoi(c + 5); }
        { const char *g = strstr(z, "[35 t=23 ");
          if (g) { const char *m = strstr(g, " mo="), *a = strstr(g, " af="), *at = strstr(g, "@(");
                   if (m) r.gmo = atoi(m + 4); if (a) r.gaf = atoi(a + 4); if (at) sscanf(at, "@(%d,%d,r%d", &r.gx, &r.gz, &r.rot); } }
        r.y = (idx < ny) ? ys[idx] : 0;
        if (n < 40000) s_nb3_st[n++] = r;
    }
    fclose(f);
    return n;
}
static int nb3_phasen_lesen(const char *dir, int8_t *ph, int nmax)   /* gator_trag.log: Phase je Bild */
{
    char pf[1024]; static char z[4096]; snprintf(pf, sizeof pf, "%s/gator_trag.log", dir);
    memset(ph, -1, (size_t)nmax);
    FILE *f = fopen(pf, "r"); if (!f) return 0;
    int n = 0; while (fgets(z, sizeof z, f)) { int fr, p; if (sscanf(z, "TRAG F%d ph=%d", &fr, &p) == 2 && fr >= 0 && fr < nmax) { ph[fr] = (int8_t)p; n++; } }
    fclose(f); return n;
}
static int nb3_boss_zeilen(const char *dir, const char *muster, uint8_t *bild, int nmax)   /* Zeilen "<muster> F<n>" in gator_boss.log: Zahl, Bilder markiert */
{
    char pf[1024]; static char z[4096]; snprintf(pf, sizeof pf, "%s/gator_boss.log", dir);
    FILE *f = fopen(pf, "r"); if (!f) return 0;
    int n = 0;
    while (fgets(z, sizeof z, f)) {
        const char *p = strstr(z, muster); if (!p) continue;
        n++;
        if (bild) { const char *q = strchr(p + strlen(muster), 'F'); if (q) { const int fr = atoi(q + 1); if (fr >= 0 && fr < nmax) bild[fr] = 1; } }
    }
    fclose(f); return n;
}
static int nb3_oben_grund(const nb3_st_t *r) { return r->y < -900 && ((r->px > 1850 && r->px < 7200 && r->pz > -18100 && r->pz < -14500) || (r->px > -1700 && r->px < 1850 && r->pz > -20000 && r->pz < -12450)); }
static int obenlog(const char *dir)
{
    const int n = nb3_state_lesen(dir);
    if (n <= 0) { printf("obenlog: %s: keine state.log\n", dir); return 2; }
    static int8_t ph[40000]; static uint8_t ramm[40000];
    nb3_phasen_lesen(dir, ph, 40000); memset(ramm, 0, sizeof ramm); nb3_boss_zeilen(dir, "RAMMEN", ramm, 40000);
    int oben_n = 0, erst_f = -1, letzt_f = -1, treffer = 0, tr_oben = 0, je[5] = { 0, 0, 0, 0, 0 };
    int erst_offs = -1, erst_tf = -1, pause_max = 0, abschn = 0, verlassen_max = 0, a = -1, letzt_tr = -1;
    const char *erst_art = "-";
    for (int i = 0; i <= n; i++) {
        const int ob = i < n && nb3_oben_grund(&s_nb3_st[i]);
        const int tr = i < n && s_nb3_st[i].hp < 100 && (i == 0 || s_nb3_st[i - 1].hp >= 100);
        if (ob) { oben_n++; if (erst_f < 0) erst_f = s_nb3_st[i].f; letzt_f = s_nb3_st[i].f; }
        if (tr) treffer++;
        if (ob && a < 0) { a = i; letzt_tr = -1; abschn++; }
        if (ob && tr) {
            const int f = s_nb3_st[i].f, p = (f >= 0 && f < 40000) ? ph[f] : -1;
            const int art = (f >= 0 && f < 40000 && ramm[f]) ? 1 : p == 7 ? 2 : p == 5 ? 3 : p == 3 ? 4 : 0;
            const int g = i - (letzt_tr < 0 ? a : letzt_tr);
            je[art]++; tr_oben++;
            if (g > pause_max) pause_max = g;
            if (abschn == 1 && letzt_tr < 0) { erst_offs = i - a; erst_art = nb3_art(art); erst_tf = f; }
            letzt_tr = i;
            printf("    Treffer F%-5d %-10s Figur (%d,%d,%d) oben | Gator Wurzel (%d,%d) Gier %d Clip %d Bild %d | cam %d\n", f, nb3_art(art), s_nb3_st[i].px, s_nb3_st[i].y, s_nb3_st[i].pz,
                   s_nb3_st[i].gx, s_nb3_st[i].gz, s_nb3_st[i].rot, s_nb3_st[i].gmo, s_nb3_st[i].gaf, s_nb3_st[i].cam);
        }
        if (!ob && a >= 0) {
            const int ende = i - 1, rest = ende - (letzt_tr < 0 ? a : letzt_tr);
            if (i >= n) { if (rest > pause_max) pause_max = rest; }                        /* oben bis zum Laufende: der Rest zaehlt als Pause */
            else if (letzt_tr < 0 && rest > verlassen_max) verlassen_max = rest;           /* ohne Treffer verlassen (Sturz/Weg): darf nicht lang sein */
            printf("  oben F%d..F%d (%d Bilder = %.1f s): Treffer im Abschnitt %s, Rest nach dem letzten Treffer %d Bilder%s\n", s_nb3_st[a].f, s_nb3_st[ende].f, ende - a + 1, (ende - a + 1) / 30.0,
                   letzt_tr < 0 ? "keiner" : "ja", rest, i >= n ? " (bis zum Laufende)" : "");
            a = -1;
        }
    }
    const int ok = oben_n > 0 && erst_offs >= 0 && erst_offs <= NB3_PAUSE_MAX && pause_max <= NB3_PAUSE_MAX && verlassen_max <= NB3_PAUSE_MAX;
    printf("obenlog: %d Bilder (F%d..F%d); Figur oben %d Bilder (erstes F%d, letztes F%d, %d Abschnitte); Treffer gesamt %d, oben %d (Rammen %d, CROSS-Biss %d, Hochbiss %d, Biss %d, sonst %d); "
           "erster Treffer oben nach %d Bildern (%s F%d); laengste Pause ohne Treffer oben %d Bilder; ohne Treffer verlassener Abschnitt bis %d Bilder -> %s (Schranke %d)\n",
           n, s_nb3_st[0].f, s_nb3_st[n - 1].f, oben_n, erst_f, letzt_f, abschn, treffer, tr_oben, je[1], je[2], je[3], je[4], je[0], erst_offs, erst_art, erst_tf, pause_max, verlassen_max,
           ok ? "ok" : "VERFEHLT", NB3_PAUSE_MAX);
    return ok ? 0 : 1;
}
typedef struct { int f, ph, mo, af, gier, gx, gz, px, pz; } nb3_tr_t;
static nb3_tr_t s_nb3_tr[40000];
static int haenglog(const char *dir, int mit)
{
    char pf[1024]; static char z[4096];
    snprintf(pf, sizeof pf, "%s/gator_trag.log", dir);
    FILE *f = fopen(pf, "r"); if (!f) { printf("haenglog: %s fehlt\n", pf); return 2; }
    int n = 0;
    while (fgets(z, sizeof z, f)) {
        nb3_tr_t r; int tx, tz;
        if (sscanf(z, "TRAG F%d ph=%d mo=%d af=%d gier=%d pos=(%d,%d) takt=(%d,%d)", &r.f, &r.ph, &r.mo, &r.af, &r.gier, &r.gx, &r.gz, &tx, &tz) != 9) continue;
        const char *sp = strstr(z, "spieler=("); if (!sp || sscanf(sp, "spieler=(%d,%d)", &r.px, &r.pz) != 2) continue;
        if (n < 40000) s_nb3_tr[n++] = r;
    }
    fclose(f);
    const int ns = nb3_state_lesen(dir);
    static int hp_von[40000], cam_von[40000], y_von[40000];
    for (int i = 0; i < 40000; i++) { hp_von[i] = 100; cam_von[i] = -1; y_von[i] = 0; }
    int treffer = 0;
    for (int i = 0; i < ns; i++) { const int fr = s_nb3_st[i].f; if (fr >= 0 && fr < 40000) { hp_von[fr] = s_nb3_st[i].hp; cam_von[fr] = s_nb3_st[i].cam; y_von[fr] = s_nb3_st[i].y; }
                                   if (s_nb3_st[i].hp < 100 && (i == 0 || s_nb3_st[i - 1].hp >= 100)) treffer++; }
    static double kd[40000];
    for (int i = 0; i < n; i++) { const double th = s_nb3_tr[i].gier * 2.0 * 3.14159265358979 / 4096.0;
                                  const double hx = s_nb3_tr[i].gx + cos(th) * 3236.0 - s_nb3_tr[i].px, hz = s_nb3_tr[i].gz - sin(th) * 3236.0 - s_nb3_tr[i].pz; kd[i] = sqrt(hx * hx + hz * hz); }
    /* laengste Zeit ohne Fortschritt (mass_v4.py / Riegel re2) */
    double best = -1; int lauf = 0, ohne_max = 0, ohne_bis = -1, p0x = 0, p0z = 0, p0_ok = 0;
    for (int i = 0; i < n; i++) {
        const double mx = (double)s_nb3_tr[i].px - p0x, mz = (double)s_nb3_tr[i].pz - p0z;
        if (!p0_ok || kd[i] <= 4500 || sqrt(mx * mx + mz * mz) > 1500 || kd[i] < best - 300) { lauf = 0; best = kd[i]; p0x = s_nb3_tr[i].px; p0z = s_nb3_tr[i].pz; p0_ok = 1; }
        else { lauf++; if (lauf > ohne_max) { ohne_max = lauf; ohne_bis = s_nb3_tr[i].f; } }
    }
    /* Haenge-Fenster (RW-Mass gator_rw_r41.c (b)) */
    int seit_biss = 1 << 20, hn = 0, fenster = 0, fenster_f1 = -1; static int32_t hx[150], hz[150]; static int hf[150];
    for (int i = 0; i < n; i++) {
        const int F = s_nb3_tr[i].f, p = s_nb3_tr[i].ph, Fi = (F >= 0 && F < 40000) ? F : 0;
        if (p == 3 || p == 8 || hp_von[Fi] < 100) seit_biss = 0; else if (seit_biss < (1 << 20)) seit_biss++;
        if (p != 2) { hn = 0; continue; }
        hx[hn % 150] = s_nb3_tr[i].gx; hz[hn % 150] = s_nb3_tr[i].gz; hf[hn % 150] = F; hn++;
        if (hn >= 150 && seit_biss >= 150) {
            int32_t x0 = hx[0], x1 = x0, z0 = hz[0], z1 = z0; const int fa = hf[hn % 150];
            for (int k = 1; k < 150; k++) { if (hx[k] < x0) x0 = hx[k]; if (hx[k] > x1) x1 = hx[k]; if (hz[k] < z0) z0 = hz[k]; if (hz[k] > z1) z1 = hz[k]; }
            const int i60 = (hn - 60) % 150;
            const double nx = (double)s_nb3_tr[i].gx - hx[i60], nz = (double)s_nb3_tr[i].gz - hz[i60];
            if (x1 - x0 < 2500 && z1 - z0 < 2500 && kd[i] > 4500 && sqrt(nx * nx + nz * nz) < 600) {
                fenster++; if (fenster_f1 < 0) fenster_f1 = F;
                printf("    Haenge-Fenster F%d (ab F%d): Wurzel (%d..%d, %d..%d) jetzt (%d,%d) Gier %d Clip %d Bild %d | Figur (%d,%d,%d) cam %d hp %d | Kopf %d von der Figur\n",
                       F, fa, x0, x1, z0, z1, s_nb3_tr[i].gx, s_nb3_tr[i].gz, s_nb3_tr[i].gier, s_nb3_tr[i].mo, s_nb3_tr[i].af, s_nb3_tr[i].px, y_von[Fi], s_nb3_tr[i].pz, cam_von[Fi], hp_von[Fi], (int)kd[i]);
                hn = 0;
            }
        }
    }
    const int h = nb3_boss_zeilen(dir, "G2RE2 HAENGT", NULL, 0), rw = nb3_boss_zeilen(dir, "RUECKWAERTS an", NULL, 0), ta = nb3_boss_zeilen(dir, "TAUCHEN an", NULL, 0);
    int phn[16]; memset(phn, 0, sizeof phn);
    for (int i = 0; i < n; i++) if (s_nb3_tr[i].ph >= 0 && s_nb3_tr[i].ph < 16) phn[s_nb3_tr[i].ph]++;
    const int ok = n > 0 && fenster == 0 && ohne_max <= NB3_OHNE_MAX && (!mit || (h == 0 && rw == 0 && ta == 0));
    printf("haenglog: %d TRAG-Bilder F%d..F%d; Haenge-Fenster %d (erstes F%d); ohne Fortschritt max %d Bilder = %.1f s (bis F%d); Treffer %d; Zeilen HAENGT %d, RUECKWAERTS an %d, TAUCHEN an %d%s; "
           "Phasen 2:%d 3:%d 5:%d 7:%d 12:%d -> %s (Fenster 0, ohne Fortschritt <= %d)\n",
           n, n ? s_nb3_tr[0].f : -1, n ? s_nb3_tr[n - 1].f : -1, fenster, fenster_f1, ohne_max, ohne_max / 30.0, ohne_bis, treffer, h, rw, ta, mit ? " (mit Kanister: je 0 verlangt)" : "",
           phn[2], phn[3], phn[5], phn[7], phn[12], ok ? "ok" : "VERFEHLT", NB3_OHNE_MAX);
    return ok ? 0 : 1;
}

int main(int argc, char **argv)
'''
rep("\nint main(int argc, char **argv)\n", neu)

# main: Pruefhaken-Umgebung, Werkzeuge, Riegel
rep("""    if (getenv("G2_RE2_ALT")) re15_gr2_rw_alt_setzen(1);   /* G2-RE2: Werkzeug-Laeufe (fuzz/bahn) im Stand NB5 - Vergleich mit dem NB5-Bau */""",
"""    if (getenv("G2_RE2_ALT")) re15_gr2_rw_alt_setzen(1);   /* G2-RE2: Werkzeug-Laeufe (fuzz/bahn) im Stand NB5 - Vergleich mit dem NB5-Bau */
    if (getenv("G2_NB3_ALT")) re15_gator_boss_nb3_alt_setzen(1);   /* G2-RE2 NB3: Werkzeug-Laeufe (nb3_oben/nb3_haengt) im Stand a661eb8aa */
    if (!strcmp(teil, "obenlog") && argc >= 3)  return obenlog(argv[2]);                                            /* NB3 exe-Riegel rampe_oben */
    if (!strcmp(teil, "haenglog") && argc >= 3) return haenglog(argv[2], argc >= 4 && !strcmp(argv[3], "mit"));    /* NB3 exe-Riegel rampe/abn2_wand_elza */""")
rep("""    if (alle || !strcmp(teil, "v4"))        teil_v4(0);
    if (alle || !strcmp(teil, "v4_elza"))   teil_v4(1);""",
"""    if (alle || !strcmp(teil, "v4"))        teil_v4(0);
    if (alle || !strcmp(teil, "v4_elza"))   teil_v4(1);
    if (alle || !strcmp(teil, "nb3"))       teil_nb3(0);
    if (alle || !strcmp(teil, "nb3_elza"))  teil_nb3(1);""")

io.open(P, "w", encoding="utf-8", newline="").write(s.replace("\n", "\r\n") if crlf else s)
print("OK crlf=%d" % crlf)
