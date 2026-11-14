p = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tests/unit/test_birkin_formen.c'
s = open(p, encoding='utf-8', newline='').read()
crlf = '\r\n' in s
s = s.replace('\r\n', '\n')
def rep(a, b):
    global s
    assert s.count(a) == 1, (s.count(a), a[:80])
    s = s.replace(a, b)
rep(""" * V8 Runde 6b (P5-neu / P4-neu nach Pruefung V8 r5d "Kragen", "graue Reifen"; gemessen am Asset 95a783a6, sfv8/r6b pins.py / pins2.py,
""", """ * V8 Runde 7c (P5, Stufe p = tools/r41_sichel/v8/p5_stufe.py: Kandidat R3 des P5-Ablaufs + Pflicht-Korrekturen K-a..K-e; Nutzer
 * "smooth straight to head without further hills...."): das Nackenstueck der Runde 6b ist wieder weg (Mesh 8 = 69 Ecken wie EM30); an
 * Mesh 8 haengen 4 Ergaenzungsdreiecke d77-d80 = die Haelften (0,1,3) / (0,3,2) der Vierecke v8 und v17 mit gleichen Ecken, Normalen,
 * Texeln und Zeile (Entscheid E11: der Port verwirft ein verdrehtes Viereck nach dem Testdreieck, engine/src/nclip_r38.c). Zeilen 493/494
 * 60/48 Flaechen, 31 EM30-Flaechen mit anderer UV (Runde 6b 29; dazu t8 und t60 von R3/7c). Gemessen am Asset 0c307f80 (sfv8/r7c/w,
 * Abschnitt "Unit-Test" der NOTIZ). Gegenprobe Stand 7b 87b02681: rot (87 Dreiecke, 76 Ecken, Nackenstueck).
 * V8 Runde 6b (P5-neu / P4-neu nach Pruefung V8 r5d "Kragen", "graue Reifen"; gemessen am Asset 95a783a6, sfv8/r6b pins.py / pins2.py,
""")
rep("""#define EINSATZ_D 10              /* V8 Runde 6b: Nackenstueck an Mesh 8 (nacken_r6.py, nacken_v8.json koerper "rechts") - Dreiecke */
#define EINSATZ_V 0               /* ... Vierecke (Runde 5d: 12 / 6 / 14) */
#define EINSATZ_E 7               /* ... Ecken */""", """#define EINSATZ_D 4               /* V8 Runde 7c: 4 Ergaenzungsdreiecke an Mesh 8 (p5_stufe.py ERGAENZEN, Haelften von v8 und v17) - Dreiecke
                                   * (Runde 6b: Nackenstueck nacken_r6.py 10) */
#define EINSATZ_V 0               /* ... Vierecke (Runde 5d: 12 / 6 / 14) */
#define EINSATZ_E 0               /* ... Ecken (Runde 7c: keine neuen; Runde 6b 7) */
#define ERG_QUAD_A 8              /* V8 Runde 7c: d77/d78 = Haelften von v8 [30,0,50,49], d79/d80 = Haelften von v17 [51,17,50,30] */
#define ERG_QUAD_B 17""")
a = s.index("                /* V8 Runde 6b: jede Flaeche des Nackenstuecks zeigt nur auf die 7 angehaengten Ecken")
b = s.index("                CHECK(zeile_zahl(b, 480) == 0 && zeile_zahl(b, 481) == 0, \"%s: Mesh 8 %d/%d Flaechen noch auf V7-Zeile 480/481")
s = s[:a] + """                /* V8 Runde 7c (E11): die angehaengten Dreiecke sind genau die Haelften (0,1,3) und (0,3,2) der Vierecke v8 und v17 - gleiche
                 * Ecken- und Normalen-Indizes, gleiche UV, CLUT und Seite; keine eigene Geometrie, keine eigenen Texel (Runde 6b: Nackenstueck) */
                {
                    int fehl = 0;
                    static const int H[2][3] = {{0, 1, 3}, {0, 3, 2}};
                    for (int t = a->triangle_count; t < b->triangle_count; t++) {
                        const int k = t - a->triangle_count, qi = k < 2 ? ERG_QUAD_A : ERG_QUAD_B;
                        if (k >= 4 || qi >= b->quad_count) { fehl++; continue; }
                        const re15_md1_quad_t *q = &b->quads[qi];
                        const re15_md1_quad_uv_t *qu = &b->quad_uvs[qi];
                        const uint16_t qv[4] = {q->v0, q->v1, q->v2, q->v3}, qn[4] = {q->n0, q->n1, q->n2, q->n3};
                        const uint8_t uu[4] = {qu->u0, qu->u1, qu->u2, qu->u3}, vv[4] = {qu->v0, qu->v1, qu->v2, qu->v3};
                        const int *h = H[k & 1];
                        const re15_md1_triangle_t *f = &b->triangles[t];
                        const re15_md1_tri_uv_t *u = &b->triangle_uvs[t];
                        fehl += !(f->v0 == qv[h[0]] && f->v1 == qv[h[1]] && f->v2 == qv[h[2]] &&
                                  f->n0 == qn[h[0]] && f->n1 == qn[h[1]] && f->n2 == qn[h[2]]);
                        fehl += !(u->u0 == uu[h[0]] && u->v0 == vv[h[0]] && u->u1 == uu[h[1]] && u->v1 == vv[h[1]] &&
                                  u->u2 == uu[h[2]] && u->v2 == vv[h[2]] && u->clut == qu->clut && u->page == qu->page);
                    }
                    CHECK(fehl == 0, "%s: Ergaenzungsdreiecke an Mesh 8: %d Verstoesse (nicht die Haelften von v8/v17 mit gleichen Ecken, "
                          "Normalen, UV, CLUT und Seite)", name, fehl);
                }
""" + s[b:]
rep("""                CHECK(uv_ohne_clut_anders(a, b) == 29, "%s: Mesh 8 %d Flaechen mit anderer UV (Soll 29, V8 Runde 6b: Runde 5 + d53)", name,
                      uv_ohne_clut_anders(a, b));""", """                CHECK(uv_ohne_clut_anders(a, b) == 31, "%s: Mesh 8 %d Flaechen mit anderer UV (Soll 31, V8 Runde 7c: Runde 6b 29 + t8, t60)",
                      name, uv_ohne_clut_anders(a, b));""")
s = s.replace("(V8 Runde 6b: 56 + Nackenstueck)", "(V8 Runde 7c: 56 + Ergaenzungsdreiecke)")
if crlf: s = s.replace('\n', '\r\n')
open(p, 'w', encoding='utf-8', newline='').write(s)
print('ok', crlf)
