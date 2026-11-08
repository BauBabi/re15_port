# -*- coding: utf-8 -*-
"""NB3-Bau: Patch enemy_ai_boss_gator.c (Vorgabe A + B, Pruefhaken nb3_alt). Jede Fundstelle muss genau einmal vorkommen."""
import io, sys
P = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/engine/src/enemy_ai_boss_gator.c"
s = io.open(P, encoding="utf-8", newline="").read()
crlf = "\r\n" in s
s = s.replace("\r\n", "\n")
orig_len = len(s)
def rep(old, new, n=1):
    global s
    c = s.count(old)
    if c != n:
        print("FUNDSTELLE %d statt %d:\n%s" % (c, n, old[:200])); sys.exit(1)
    s = s.replace(old, new)

# 1) Pruefhaken + Konstante nach den Rampen-Konstanten
rep("#define GB_RAMP_Z0     -18100\n#define GB_RAMP_Z1     -14500\n",
"""#define GB_RAMP_Z0     -18100
#define GB_RAMP_Z1     -14500
/* Runde 41 Spur G2-RE2 NB3 (Nutzerentscheid 2026-11-06 zu F1-F4 der Abnahme 4, Messblatt analysis/befunde_runde41/G2_MESSBLATT_NB3.md):
 *  Vorgabe A ("Er soll nur den Spieler oben auf der Rampe auch rammen koennen, das der Spieler oben nicht ausweichen kann. Alternativ kann er
 *  auch hoch beissen oder sowas, aber er darf halt oben nicht sicher sein." / "Das kann er gerne machen."): steht die Figur GB_OBEN_OHNE Bilder
 *  oben (Rampe/Plattform/Ostnische) ohne Treffer, quert er ueber sie hinweg (QUER-CROSS der Belagerung, Rammen in GBP_CROSS auch ohne Kanister,
 *  unabhaengig von der Kamera - F1 "noe, wozu wenn es gut funktioniert?").
 *  Vorgabe B ("Wenn er haengt muessen wir nachbessern ... Er darf nicht zu lange haengen ... das hilft natuerlich nicht bei der Platform."):
 *  eine Figur im Koerper-Band (GB_KOERPER_M) VOR einer Blockkante ist kein Umweg-Fall mehr (gb_punkt_im_band, Zielstrecken gegen den Block
 *  selbst); die Ostnische (x > GB_RAMP_X1 im Rampenband) wird belagert wie eine Figur oben.
 *  Pruefhaken s_gb_nb3_alt = 1: Stand a661eb8aa (Gegenproben der Riegel nb3, kein Spielweg). */
static int s_gb_nb3_alt = 0;
void re15_gator_boss_nb3_alt_setzen(int an) { s_gb_nb3_alt = an; }
int  re15_gator_boss_nb3_alt(void) { return s_gb_nb3_alt; }
#define GB_OBEN_OHNE     300    /* Bilder oben ohne Treffer bis zur Querung ueber die Figur = der NB5-Wert der Pause nach einer Querung mit
                                 * der Figur oben (cross_cd 300, Spur B "Leon OBEN: 10 s auf der Seite bleiben", GBP_CROSS unten) - PORT-WAHL
                                 * nach dem Nutzer-Wortlaut "darf halt oben nicht sicher sein" (Takt frei: "Das kann er gerne machen"); RE2 hat
                                 * im Alligator-Raum keine erhoehte Flaeche und keinen Takt dafuer (Messblatt NB3 Abschnitt 3). */
""")

# 2) gb_state_t: Zaehler
rep("    uint8_t  p3_kreise;\n} gb_state_t;",
"""    uint8_t  p3_kreise;
    int16_t  oben_ohne;            /* Runde 41 G2-RE2 NB3 (Vorgabe A): Bilder, die die Figur oben/in der Ostnische steht, ohne Treffer
                                    * (0 beim Treffer gb_biss_abschluss und sobald sie nicht mehr oben ist) */
} gb_state_t;""")

# 3) Helfer nach gb_seg_frei
rep("""static int gb_seg_frei(int32_t x0, int32_t z0, int32_t x1, int32_t z1, int32_t m)
{
    return !gb_seg_hits_platform(x0, z0, x1, z1, m)
        && !gb_seg_hits_ramp(x0, z0, x1, z1, m);
}
""",
"""static int gb_seg_frei(int32_t x0, int32_t z0, int32_t x1, int32_t z1, int32_t m)
{
    return !gb_seg_hits_platform(x0, z0, x1, z1, m)
        && !gb_seg_hits_ramp(x0, z0, x1, z1, m);
}
/* Runde 41 Spur G2-RE2 NB3 (Vorgabe B): liegt der Punkt im Koerper-Band (GB_KOERPER_M 1800) VOR einer Blockkante von Plattform oder Rampe,
 * aber nicht im Block selbst? Fuer so ein Ziel meldete gb_seg_hits_rect "Punkt innen -> Schnitt" (Messblatt NB3 2.3: Figur (4073,-19322) 1222
 * vor der Rampen-Suedkante, (7668,-16403) Ostnische, (4252,-19953)) und die Verfolgung lief in den Ring-/Westumlauf-Umweg: Pendel im Kasten
 * (3156..4879, -24085..-21690), Kopf 6830-10057 von der Figur = der Haenger. Im Band gilt die Strecke gegen den Block selbst (m = 0); die
 * SCA-Klemme (hit_radius_min 1466) haelt die Wurzel ohnehin von der Kante, der Biss reicht hinueber (Maul 2600 + 1500, LUNGE dist < 3200). */
static int gb_punkt_im_band(int32_t x, int32_t z)
{
    const int32_t m = GB_KOERPER_M;
    if (x >= GB_PLAT_X0 && x <= GB_PLAT_X1 && z >= GB_PLAT_Z0 && z <= GB_PLAT_Z1) return 0;
    if (x >= GB_RAMP_X0 && x <= GB_RAMP_X1 && z >= GB_RAMP_Z0 && z <= GB_RAMP_Z1) return 0;
    return (x >= GB_PLAT_X0 - m && x <= GB_PLAT_X1 + m && z >= GB_PLAT_Z0 - m && z <= GB_PLAT_Z1 + m)
        || (x >= GB_RAMP_X0 - m && x <= GB_RAMP_X1 + m && z >= GB_RAMP_Z0 - m && z <= GB_RAMP_Z1 + m);
}
/* NB3 (Vorgabe B): die Figur steht in der OSTNISCHE neben der Rampe (x > GB_RAMP_X1 im Rampenband; die Figur kommt nur dort ueber x 7200
 * hinaus - exe-Lauf wand_elza: x 7668 nur bei z -17577..-14994). Durch das Wasser unerreichbar: die Wurzel klemmt an Ostwand (7200 - 1466)
 * und Suedkante, der Maulpunkt 2600 voraus laege im Rampenblock (gb_wand_dazwischen). Sie wird belagert wie eine Figur oben (GBP_GUARD,
 * Kantenziel der Rampe), und die Querung der Vorgabe A ueberstreicht sie (Bahn-x bis GB_RAMP_X1 - 700 = 6500, Koerperkreis 1050 + 450). */
static int gb_pl_nische(const re15_actor_t *pl)
{
    return pl->x > GB_RAMP_X1 && pl->z >= GB_RAMP_Z0 && pl->z <= GB_RAMP_Z1;
}
""")

# 4) gb_kanten_umweg: Zielstrecken im Band gegen den Block selbst
rep("""    if (gb_seg_frei(e->x, e->z, *tx, *tz, GB_KOERPER_M)) return -1;
    for (i = 0; i < 4; i++) { kx[i] = gb_ecke[i][0]; kz[i] = gb_ecke[i][1]; }
    kx[4] = *tx; kz[4] = *tz;
    for (i = 0; i < NK; i++) {
        fest[i] = 0; prev[i] = -2;
        if (gb_seg_frei(e->x, e->z, kx[i], kz[i], GB_KOERPER_M)) {
""",
"""    /* NB3 (Vorgabe B): Ziel im Koerper-Band vor einer Blockkante -> die Strecken ZUM ZIEL gegen den Block selbst (m 0) statt
     * "Punkt innen" (s. gb_punkt_im_band); Start-/Eckenstrecken behalten GB_KOERPER_M. */
    const int32_t mz = (!s_gb_nb3_alt && gb_punkt_im_band(*tx, *tz)) ? 0 : GB_KOERPER_M;
    if (gb_seg_frei(e->x, e->z, *tx, *tz, mz)) return -1;
    for (i = 0; i < 4; i++) { kx[i] = gb_ecke[i][0]; kz[i] = gb_ecke[i][1]; }
    kx[4] = *tx; kz[4] = *tz;
    for (i = 0; i < NK; i++) {
        fest[i] = 0; prev[i] = -2;
        if (gb_seg_frei(e->x, e->z, kx[i], kz[i], i == 4 ? mz : GB_KOERPER_M)) {
""")
rep("""            if (fest[j] || !gb_seg_frei(kx[u], kz[u], kx[j], kz[j], GB_KOERPER_M))
                continue;
""",
"""            if (fest[j] || !gb_seg_frei(kx[u], kz[u], kx[j], kz[j], j == 4 ? mz : GB_KOERPER_M))
                continue;
""")

# 5) CHASE verbunden
rep("""                if (gb_seg_hits_platform(e->x, e->z, zp->x, zp->z, GB_KOERPER_M))
                    { gb_ring_target(e, zp, &tx, &tz); g->dbg_zweig = 5; }
                else if (gb_seg_hits_ramp(e->x, e->z, zp->x, zp->z, GB_KOERPER_M)) {
""",
"""                if (!s_gb_nb3_alt && gb_punkt_im_band(zp->x, zp->z) && gb_seg_frei(e->x, e->z, zp->x, zp->z, 0))
                    { g->dbg_zweig = 7; }   /* NB3 (Vorgabe B): Figur im Band vor der Kante, Strecke gegen den Block frei -> direkt */
                else if (gb_seg_hits_platform(e->x, e->z, zp->x, zp->z, GB_KOERPER_M))
                    { gb_ring_target(e, zp, &tx, &tz); g->dbg_zweig = 5; }
                else if (gb_seg_hits_ramp(e->x, e->z, zp->x, zp->z, GB_KOERPER_M)) {
""")

# 6) CHASE player_on_platform: Ostnische
rep("""             (pl->x >= GB_RAMP_X0 && pl->x <= GB_RAMP_X1 &&
              pl->z >= GB_RAMP_Z0 && pl->z <= GB_RAMP_Z1));
        if (g->cross_cd > 0) g->cross_cd--;
""",
"""             (pl->x >= GB_RAMP_X0 && pl->x <= GB_RAMP_X1 &&
              pl->z >= GB_RAMP_Z0 && pl->z <= GB_RAMP_Z1));
        if (!s_gb_nb3_alt && gb_pl_nische(pl)) player_on_platform = 1;   /* NB3 (Vorgabe B): Ostnische = belagern wie oben */
        if (g->cross_cd > 0) g->cross_cd--;
""")

# 7) GUARD oben: Ostnische
rep("""                    (pl->x >= GB_RAMP_X0 && pl->x <= GB_RAMP_X1 &&
                     pl->z >= GB_RAMP_Z0 && pl->z <= GB_RAMP_Z1));
        {   const char *tv6 = getenv("RE15_GB_TEST");
            if (tv6 && *tv6 == '6') oben = 1;   /* Sichtlauf: Belagerung erzwingen */
""",
"""                    (pl->x >= GB_RAMP_X0 && pl->x <= GB_RAMP_X1 &&
                     pl->z >= GB_RAMP_Z0 && pl->z <= GB_RAMP_Z1));
        if (!s_gb_nb3_alt && gb_pl_nische(pl)) oben = 1;   /* NB3 (Vorgabe B): Ostnische = belagern wie oben */
        {   const char *tv6 = getenv("RE15_GB_TEST");
            if (tv6 && *tv6 == '6') oben = 1;   /* Sichtlauf: Belagerung erzwingen */
""")

# 8) Zaehler je Bild vor dem Phasen-Switch
rep("""    int32_t dist = re15_enemy_player_dist(e, pl);

    switch (g->phase) {
    case GBP_LURK:
""",
"""    int32_t dist = re15_enemy_player_dist(e, pl);
    {   /* NB3 (Vorgabe A): Bilder oben (Rampe/Plattform y < -900 wie player_on_platform, oder Ostnische) ohne Treffer - Treffer nullt in
         * gb_biss_abschluss; nicht oben nullt. Ab GB_OBEN_OHNE quert die Belagerung ueber die Figur (GBP_GUARD). */
        const int oben_jetzt = ((pl->y < -900)
                                && ((pl->x >= GB_PLAT_X0 && pl->x <= GB_PLAT_X1 && pl->z >= GB_PLAT_Z0 && pl->z <= GB_PLAT_Z1)
                                    || (pl->x >= GB_RAMP_X0 && pl->x <= GB_RAMP_X1 && pl->z >= GB_RAMP_Z0 && pl->z <= GB_RAMP_Z1)))
                               || gb_pl_nische(pl);
        if (!oben_jetzt) g->oben_ohne = 0;
        else if (g->oben_ohne < 30000) g->oben_ohne++;
    }

    switch (g->phase) {
    case GBP_LURK:
""")

# 9) Treffer nullt den Zaehler
rep("""static void gb_biss_abschluss(re15_actor_t *e, gb_state_t *g, re15_actor_t *pl)
{
    re15_player_take_damage(pl, GB_BITE_TYPE, e->x, e->z);
""",
"""static void gb_biss_abschluss(re15_actor_t *e, gb_state_t *g, re15_actor_t *pl)
{
    g->oben_ohne = 0;                          /* NB3 (Vorgabe A): Treffer - die Figur war nicht sicher */
    re15_player_take_damage(pl, GB_BITE_TYPE, e->x, e->z);
""")

# 10) GUARD Seitenwahl: Block zwischen "const int zielzone_b" und dem QUER-CROSS-if ersetzen
a = s.index("                    const int zielzone_b = zielzone;   /* Seite der Belagerung ohne Kanister (Leons naehere Kante) */")
b_anchor = "                    if (zg2 != 2 && zg2 != zielzone && g->cross_cd == 0) {\n"
b = s.index(b_anchor, a)
alt_block = s[a:b]
assert alt_block.count("re15_gr2_rw_traegt() && !re15_gr2_v4_alt() && zg2 != 2") == 1
alt_lines = alt_block.split("\n")[1:]
alt_in = "\n".join(("    " + ln) if ln.strip() else ln for ln in alt_lines).rstrip("\n")
new_block = """                    const int zielzone_b = zielzone;   /* Seite der Belagerung ohne Kanister (Leons naehere Kante) */
                    int quer_frei = 0;                 /* NB3 (Vorgabe A): Querung ueber die Figur auch waehrend cross_cd */
                    if (s_gb_nb3_alt) {
                        /* Stand a661eb8aa (Gegenprobe): nur mit dem Kanister, nur von der Kamera weg (NB2 Vorschlag 4, s. o.) */
""" + alt_in + """
                    } else if (zg2 != 2) {
                        /* Runde 41 Spur G2-RE2 NB3 (Vorgabe A, Nutzer 2026-11-06 "darf halt oben nicht sicher sein", "Das kann er gerne machen"):
                         * fuer alle, mit und ohne Kanister (Vorschlag 4: mit = ohne), ohne Kamera-Bedingung (F1 "noe"). Steht die Figur
                         * GB_OBEN_OHNE Bilder oben ohne Treffer (Hochbiss reicht nicht: Mittelband bei x < ~3400, Messblatt NB3 1.4), quert er
                         * ueber sie hinweg auf die andere Seite - auch mitten in der Pause nach der letzten Querung; das Bahn-Ende folgt ihr
                         * (GBP_CROSS (b)), Rammen bei Koerperkontakt (GBP_CROSS). Waehrend der Pause (cross_cd) belagert er die EIGENE Kante
                         * statt des Westumlaufs zur fernen (Seite wie mit dem Kanister in NB2); sonst wie NB5: Leons naehere Kante, Querung,
                         * wenn er auf der falschen Seite liegt. */
                        if (g->oben_ohne >= GB_OBEN_OHNE) { zielzone = !zg2; quer_frei = 1; }
                        else if (g->cross_cd > 0) zielzone = zg2;
                    }
"""
s = s[:a] + new_block + s[b:]
rep(b_anchor, "                    if (zg2 != 2 && zg2 != zielzone && (g->cross_cd == 0 || quer_frei)) {\n")

# 11) CROSS Rammen auch ohne Kanister
rep("""        if (re15_gr2_rw_traegt() && !s_gb_ramm_alt && !g->bite_done && g->kz_rueck != 2 && pl->hit_react == 0 && pl->hp >= 0   /* G2-RE2 NB2 Vorschlag 4 ("Rampe: rammen wie gehabt"): Rammen mit dem Kanister auch in der Ueberquerung wie ohne Kanister */""",
"""        if ((re15_gr2_rw_traegt() || !s_gb_nb3_alt)   /* NB3 (Vorgabe A): Rammen auch ohne Kanister - "den Spieler oben auf der Rampe auch rammen koennen" */
            && !s_gb_ramm_alt && !g->bite_done && g->kz_rueck != 2 && pl->hit_react == 0 && pl->hp >= 0   /* G2-RE2 NB2 Vorschlag 4 ("Rampe: rammen wie gehabt"): Rammen mit dem Kanister auch in der Ueberquerung wie ohne Kanister */""")

# 12) Messhaken fuer die Riegel
rep("""int  re15_gator_boss_rammen(void) { return s_gb_rammen; }               /* Messhaken: Rammen seit Programmstart */""",
"""int  re15_gator_boss_rammen(void) { return s_gb_rammen; }               /* Messhaken: Rammen seit Programmstart */
int  re15_gator_boss_oben_ohne(const re15_actor_t *e)                    /* NB3 Messhaken: Bilder oben ohne Treffer (GUARD-Querungszaehler) */
{ const int i = (int)(e - g_actors); return (i >= 0 && i < RE15_ACTOR_MAX) ? (int)s_gb[i].oben_ohne : -1; }""")

assert len(s) > orig_len, (len(s), orig_len)
io.open(P, "w", encoding="utf-8", newline="").write(s.replace("\n", "\r\n") if crlf else s)
print("OK", orig_len, "->", len(s))
