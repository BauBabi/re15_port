/* test_birkin_formen.c — Formpruefung der Hangar-/Labor-Nachbildung (Bahnsteig-Form: test_birkin_bahnsteig.c).
 *
 * EM30_HANGAR.EMD (re15_port/tools/birkin_formen.py) ist KEIN Original-Asset, sondern aus
 * EM30 (CDEMD0.EMS, Typ 0x30) abgeleitete Gestaltung. Seit Runde 41 Spur SF (Nutzer: "Der Schwanz ist viel zu
 * starr") haengt der Schwanz als eigene Knochenkette 16-22 an Knochen 8; Mesh i == Knochen i, das knochenlose
 * Rohr-Mesh 16 des Capcom-Modells entfaellt. Der Test haelt fest, damit die Datei im Port mit den EM30-Clips,
 * der EM30-KI und der Hangar-Szene (Knochen 15) laeuft:
 *   - re15_emd_parse_container liest die Datei,
 *   - Knochen 0-15 haben dieselben Eltern wie EM30, 16-22 bilden eine Kette ab Knochen 8,
 *   - Clip-Zahl und EDD sind byte-gleich; Keyframes: Nachbesserung 2 duckt Knochen 1-10/13 und senkt die Wurzel
 *     (Becken 0, Unterarme/Haende 11/12/14/15, Wurzel x/z, Spur wie EM30, Fussgelenke bleiben bis auf die Spreizung),
 *     die Schwanzwinkel haengen dahinter und bewegen sich,
 *   - Meshes 0-15: Topologie gleich, UVs gleich ausser den umtexturierten Teilen (Beine 1-6, Kopf 9,
 *     Sichel 15 nur CLUT); Meshes 16-22 zeigen nur in ihre eigenen Arrays,
 *   - Runde 41 SF Nachbesserung 1: Oberschenkel 1/4 an y 360 geteilt (kurze Hose; jedes alte Dreieck bleibt, jedes
 *     geteilte Viereck wird zu zwei, neue Ecken liegen hinter den alten), Haut-CLUTs 480-482 umgefaerbt (Jeans-Eintraege
 *     mit b > r unveraendert), Zusatzzeilen 483 (Schwanz, Meshes 16-22), 484 (Sichel, Mesh 15), 485 (Kopf, Mesh 9),
 *   - die Geometrie ist tatsaechlich anders (sonst waere die Datei nutzlos).
 *
 * V8 Runde 6b (P5-neu / P4-neu nach Pruefung V8 r5d "Kragen", "graue Reifen"; gemessen am Asset 95a783a6, sfv8/r6b pins.py / pins2.py,
 * Ausgabe sfv8/r6c/pins.out; erster Stand der Runde 6b 382bbfe7: Mesh 16 noch 20 Flaechen auf 495, sonst gleiche Zahlen):
 * Mesh 8 traegt statt des Einsatzes der Runde 5d (14 Ecken, 12 Dreiecke, 6 Vierecke, Zeile 494) EIN geschlossenes Nackenstueck
 * (tools/r41_sichel/v8/nacken_r6.py, nacken_v8.json: 7 Ecken ab Index 69, 10 Dreiecke ab Index 77, kein Viereck, Zeile 493, Textur-Seite 0,
 * UV im Rechteck u 43-53 / v 88-101); 29 EM30-Flaechen mit anderer UV (Runde 5: 28, dazu d53). Schwanz (unterseite_r5.py Runde 6):
 * Mantel, Dorn und Ring-Deckel an den Grenzen theta 110 / 120 geteilt, Flaechen auf Zeile 495 je Glied 21 / 30 / 41 / 31 / 30 / 30 / 30
 * (Runde 5d: 8 / 8 / 16 / 8 / 8 / 8 / 16). Gegenprobe V8 r6a 7a65e3f0 und V8 r5d c4976c62: rot (Mesh 8 83 Ecken / 89 Dreiecke / 33 Vierecke,
 * Schwanz 72 Flaechen auf 495).
 * V8 Runde 6a (P6, Nutzer 2026-11-10 "Die Vorlage hat eine breite Hand mit breiten Unterarmt. [...] Bei uns ist die Hand ganz klein und
 * zierlich und wirkt dadurch verkümmert."): Klauenhand Mesh 12 und Unterarm Mesh 11 neu geformt (tools/r41_sichel/v8/hand_r6.py, Stufe h,
 * hand_v8.json; Messblatt analysis/befunde_runde41/SF_MESSBLATT_HAND.md): nur Ecken und Normalen, Flaechen / UV / CLUT-Zeilen wie zuvor.
 * Gemessen am Asset 7a65e3f0 (sfv8/r6h): Mesh 12 Spanne x / y / z 484 / 716 / 433, Wurzelring y -22; Mesh 11 Spanne x / z 444 / 407.
 * Gegenprobe V8 r5d c4976c62 und V7: rot (Mesh 12 Spanne 343 / 594 / 235, Wurzelring y 16-19; Mesh 11 Spanne 516 / 472).
 * V8 Runde 5d (P5, Nutzer 2026-11-09 "es gibt immer noch eine Lücke zwischen Rücken und Kopf beim 2 beiner Birkin"): Mesh 8 traegt
 * hinter seinen EM30-Elementen einen geschlossenen Einsatz (tools/r41_sichel/v8/nacken_r5.py: 14 Ecken, 12 Dreiecke, 6 Vierecke,
 * alle auf Zeile 494 mit Texeln der Wirtsflaeche Dreieck 65); gemessen am Asset c4976c62 (sfv8/r5p6). Gegenprobe V8 Runde 4
 * 10d02173, Stand 91885ed8c (5fd35ff1) und V7: rot (Mesh 8 ohne Einsatz).
 * Zweig r41/sichel_v8 (V8 Runde 5 nach Pruefung V8 r4: Brust auf Original-Zeile 481 mit eigener UV, fast weisse Texel durch Abbildungen
 * auf Original-Haut ersetzt; dazu P4 Schwanz-Unterseite fleischig auf Zeile 495 (72 Flaechen der Meshes 16-22, Oberseite 487); Gegenprobe
 * V8 Runde 4 10d02173: rot, Brust 34 Flaechen auf 488, Schwanz 0 Flaechen auf 495).
 * Bis Runde 4 (V8 Koerperbau nach Nutzer-Skizze, Runde 4 nach Pruefung V8 r3): Spiel-Asset = V8 Runde 4 (sha256 10d02173...,
 * v8_kette.sh); dazu CLUT_ZEILEN 16, Zeilen 493 / 494 / 495 = Original-EM30-Zeilen 480 / 481 / 482 BYTEGLEICH (Orchestrator O1: "dessen
 * Texelstruktur UND dessen Original-Paletteneintraege der Zeilen 480-482"). Runde 4 (tools/r41_sichel/v8/adern_r4.py): Mesh 8 ausser der
 * Brust ganz auf 493/494 (Sicht-Pruefer r3 "harte Farbnaht rosa/grau"; 56 + 14 Flaechen, davon 10 mit UV-Schub: Rueckenfleisch 4, Huegel 1,
 * blass (W)-rechts 5), Mesh 13 alle 56 auf 493 (Feld C mit einem Schub auf Original-Oberarm-Haut, 24 Flaechen), Mesh 14 alle 37 auf 495
 * (Unterarmband 4 Flaechen mit Schub/Skalierung), gemessen am Asset 10d02173 (sfv8/r4). Gegenprobe V8 Runde 3 (66806bc4): rot (Mesh 8
 * 20 + 7 Flaechen auf 480/481, Mesh 13 Feld C auf 480/489, 3 statt 10 Flaechen mit UV-Schub).
 * Seit v0.8.30 (Einbau V7, Nutzerfreigabe 2026-11-08 woertlich "Naja, gehen wir so erst mal ins rennnen.") ist das Spiel-Asset die
 * Sichel-Form V7 (sha256 319bbd17..., Pin release/em30_hangar.sha256, Bau re15_port/tools/r41_sichel/v7_kette.sh), nicht mehr die
 * Ausgabe von birkin_formen.py (Referenz 2, cfd1ac42). Die Erwartungen unten sind an V7 gemessen (eigener Leser in Python:
 * MD1/TIM-Leser von tools/emd_ansichtsblatt.py / birkin_formen.md1_lesen, Dossier SF_sichel.md "Einbau v0.8.30") und je Zeile mit
 * dem Abschnitt des Dossiers analysis/befunde_runde41/SF_sichel.md belegt (V6 Kopf, V5 wie Foto, V6 drei Punkte, V7 linke Hand).
 * PORT-WAHL / Gestaltung, keine @0x-Konstante (die Form gibt es auf keiner Disc):
 *   - Kopf Mesh 9 = Original-Kopf des RE1.5-Birkin (UV bis auf das CLUT-Feld = EM30, hoechstens 4 Ecken geglaettet), CLUT-Zeile 486
 *     (= EM30-Zeile 0 bytegleich) - V6 Kopf Schritt 1/3, T-H6;
 *   - Brust Mesh 8 / Oberarm Mesh 13 / Klauenhand Mesh 12 nur im CLUT-Feld umtexturiert (488 / 489 / 490) - V5 wie Foto b);
 *   - Daumenkralle 6 Flaechen auf Zeile 491 (grau), Fingerkrallen 18 Flaechen auf Zeile 492 (rot) - V7 linke Hand 2.;
 *   - Schwanzglieder 16-22 (Mantel und die V6-Deckel-Dreiecke) auf Zeile 487 - V5 wie Foto a) Farbe, V6 drei Punkte 2. Deckel;
 *   - 13 CLUT-Zeilen (<= 16, Port-Texturhoehe tex_h = tim->height x n_cluts = 3328 <= 4096, render_pc.c
 *     re15_render_pc_upload_tim_slot) - V7 linke Hand T-V7f;
 *   - Zeilen 480-482: die 30 blauen Eintraege mit Paletten-a* >= 10 sind grau (V5 wie Foto b), Nutzer (5)/(8) "sonst dominiert im
 *     original eher das grau"), alle anderen blauen Eintraege unveraendert.
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_md1.h"
#include "re15_skeleton.h"
#include "re15_tim.h"

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

typedef struct {
    re15_md1_t           md1;
    re15_emd_skeleton_t  skel;
    re15_emd_animation_t anim;
    re15_tim_t           tim;
} modell_t;

static modell_t s_basis, s_form;

#define KETTE 7                  /* Schwanzglieder, birkin_formen.py FORMEN['hangar'] schwanz_kette glieder */
static int spanne(const re15_md1_mesh_t *b, int achse)   /* V8 Runde 6a: groesste minus kleinste Ecken-Koordinate (0 x, 1 y, 2 z) */
{
    int lo = 32767, hi = -32768;
    for (int v = 0; v < b->tri_vertex_count; v++) {
        int w = achse == 0 ? b->tri_vertices[v].x : achse == 1 ? b->tri_vertices[v].y : b->tri_vertices[v].z;
        if (w < lo) lo = w;
        if (w > hi) hi = w;
    }
    return hi - lo;
}
static int umtexturiert(int m)   /* birkin_formen.py FORMEN['hangar'] umtexturieren (1-6, 9, 15); V7: dazu 8, 12, 13 (V5 wie Foto b) */
{
    return (m >= 1 && m <= 6) || m == 8 || m == 9 || m == 12 || m == 13 || m == 14 || m == 15;   /* V8 Runde 2: dazu 14 (X3) */
}
static int nur_clut(int m)       /* V7: UV bis auf das CLUT-Feld = EM30 (V6 Kopf Schritt 1: Mesh 9 = Original-Mesh; V5 wie Foto b):
                                  * nur CLUT-Felder fuer 8/12/13; Sichel 15 wie Referenz 2). Gemessen: genau 8, 9, 12, 13, 15 */
{
    return m == 9 || m == 12 || m == 15;   /* V8 Runde 4: Meshes 8 / 13 / 14 haben Flaechen mit UV-Schub (eigene Pruefung je Mesh) */
}
static int uv_ohne_clut_anders(const re15_md1_mesh_t *a, const re15_md1_mesh_t *b)   /* V8: Zahl der Flaechen mit anderer UV */
{
    int n = 0;
    for (int t = 0; t < a->triangle_count; t++) {
        re15_md1_tri_uv_t x = a->triangle_uvs[t], y = b->triangle_uvs[t];
        x.clut = y.clut = 0;
        n += memcmp(&x, &y, sizeof x) != 0;
    }
    for (int q = 0; q < a->quad_count; q++) {
        re15_md1_quad_uv_t x = a->quad_uvs[q], y = b->quad_uvs[q];
        x.clut = y.clut = 0;
        n += memcmp(&x, &y, sizeof x) != 0;
    }
    return n;
}
static int uv_ohne_clut_gleich(const re15_md1_mesh_t *a, const re15_md1_mesh_t *b)
{
    for (int t = 0; t < a->triangle_count; t++) {
        re15_md1_tri_uv_t x = a->triangle_uvs[t], y = b->triangle_uvs[t];
        x.clut = y.clut = 0;
        if (memcmp(&x, &y, sizeof x)) return 0;
    }
    for (int q = 0; q < a->quad_count; q++) {
        re15_md1_quad_uv_t x = a->quad_uvs[q], y = b->quad_uvs[q];
        x.clut = y.clut = 0;
        if (memcmp(&x, &y, sizeof x)) return 0;
    }
    return 1;
}
static int zeile_zahl(const re15_md1_mesh_t *b, unsigned zeile)   /* Flaechen eines Meshes auf CLUT-Zeile `zeile` */
{
    int n = 0;
    for (int q = 0; q < b->quad_count; q++) n += b->quad_uvs[q].clut == (uint16_t)(zeile << 6);
    for (int t = 0; t < b->triangle_count; t++) n += b->triangle_uvs[t].clut == (uint16_t)(zeile << 6);
    return n;
}
static int geteilt(int m)        /* birkin_formen.py FORMEN['hangar'] teilen (kurze Hose, N1.6) */
{
    return m == 1 || m == 4;
}
#define CLUT_SCHWANZ (487u << 6)  /* V7 (V5 wie Foto a) Farbe: Zeile 487; Referenz 2: 483). Gemessen: Meshes 16-22 alle Flaechen 487 */
#define CLUT_SICHEL  (484u << 6)  /* birkin_formen.py umtexturieren Mesh 15 (V7 unveraendert, gemessen 484:50) */
#define CLUT_KOPF    (486u << 6)  /* V7 (V6 Kopf Schritt 1/3: Original-Kopf auf Zeile 486 = EM30-Zeile 0; Referenz 2: 485). Gemessen 486:91 */
#define CLUT_ZEILEN  16           /* V8 Runde 2 Adern: 16 Zeilen 480-495 = V7 (13) + X1 493 / X2 494 / X3 495 (gemessen: 4096 Eintraege,
                                   * Port-Texturhoehe 256 x 16 = 4096 = TEX_H_MAX); V7 13, V8 Runde 1 15 (Gegenprobe: hier rot) */
#define EINSATZ_D 10              /* V8 Runde 6b: Nackenstueck an Mesh 8 (nacken_r6.py, nacken_v8.json koerper "rechts") - Dreiecke */
#define EINSATZ_V 0               /* ... Vierecke (Runde 5d: 12 / 6 / 14) */
#define EINSATZ_E 7               /* ... Ecken */
#define EINSATZ_U0 43             /* UV-Rechteck des Nackenstuecks, gemessen am Asset 95a783a6 (sfv8/r6b pins.py): u 43-53, v 88-101, */
#define EINSATZ_U1 53             /* Textur-Seite 0, Zeile 493 (= Original-EM30-Zeile 480, Rueckenhaut) */
#define EINSATZ_V0 88
#define EINSATZ_V1 101
#define Z_X1 493u                 /* V8: Ader-Zeile X1 = V7-Zeile 480 + 62 Original-EM30-Ader-Eintraege (a* >= 15, L* <= 38) */
#define Z_X2 494u                 /* V8: Ader-Zeile X2 = V7-Zeile 481 + 50 Original-EM30-Ader-Eintraege */
#define Z_X3 495u                 /* V8 Runde 2: Ader-Zeile X3 = V7-Zeile 482 + 88 Original-EM30-Ader-Eintraege (Unterarm-Wulst Mesh 14) */
#define TEX_H_MAX    4096         /* Port-Texturhoehe tex_h = tim->height x n_cluts (render_pc.c re15_render_pc_upload_tim_slot);
                                   * Grenze 4096 = 16 Paletten 480..495 x 256 (render_pc.c Kommentar tpage 0x1e/0x1f; Dossier V6 Kopf T-K1c) */

static void vergleiche(const char *name, const uint8_t *em30, size_t em30_len)
{
    char pfad[512];
    snprintf(pfad, sizeof pfad, RE15_ASSET_PSX_DIR "/EMD/%s", name);
    size_t len = 0;
    uint8_t *blob = slurp(pfad, &len);
    CHECK(blob != NULL, "%s fehlt (Spiel-Asset V8, re15_port/tools/r41_sichel/v8_kette.sh)", pfad);
    if (!blob) return;
    CHECK(len >= em30_len, "%s: Laenge %zu < EM30 %zu", name, len, em30_len);

    memset(&s_form, 0, sizeof s_form);
    int rc = re15_emd_parse_container(blob, len, &s_form.md1, &s_form.skel, &s_form.anim, &s_form.tim);
    CHECK(rc == 0, "%s: re15_emd_parse_container rc=%d", name, rc);
    if (rc != 0) { free(blob); return; }

    const int nb0 = s_basis.skel.bone_count;
    CHECK(nb0 == 16, "EM30: %d Knochen statt 16", nb0);
    CHECK(s_form.skel.bone_count == nb0 + KETTE, "%s: Knochen %d != %d", name, s_form.skel.bone_count, nb0 + KETTE);
    CHECK(s_form.md1.mesh_count == nb0 + KETTE, "%s: Meshes %d != %d (Mesh i == Knochen i)",
          name, s_form.md1.mesh_count, nb0 + KETTE);
    CHECK(s_form.anim.clip_count == s_basis.anim.clip_count, "%s: Clips %d != %d",
          name, s_form.anim.clip_count, s_basis.anim.clip_count);
    CHECK(s_form.tim.width == s_basis.tim.width && s_form.tim.height == s_basis.tim.height,
          "%s: TIM %dx%d != %dx%d", name, s_form.tim.width, s_form.tim.height,
          s_basis.tim.width, s_basis.tim.height);
    for (int b = 0; b < nb0; b++)
        CHECK(s_form.skel.bone_parent[b] == s_basis.skel.bone_parent[b],
              "%s: Parent von Knochen %d %d != %d", name, b,
              s_form.skel.bone_parent[b], s_basis.skel.bone_parent[b]);
    for (int b = nb0; b < s_form.skel.bone_count; b++)
        CHECK(s_form.skel.bone_parent[b] == (b == nb0 ? 8 : b - 1),
              "%s: Schwanzknochen %d haengt an %d", name, b, s_form.skel.bone_parent[b]);
    CHECK(s_form.anim.frame_count == s_basis.anim.frame_count &&
          memcmp(s_form.anim.frames, s_basis.anim.frames,
                 sizeof(uint32_t) * (size_t)s_basis.anim.frame_count) == 0,
          "%s: EDD-Frames weichen von EM30 ab", name);
    /* Keyframes (Runde 41 SF Nachbesserung 2, birkin_formen.py ducken(): geduckte Haltung nach Video 4):
     *   - Wurzel x/z und Spur (Byte 6-11) byte-gleich mit EM30; Wurzel-Y nur tiefer (0..BECKEN, Y nach unten),
     *   - Becken (0), Unterarme/Haende (11, 12, 14, 15) mit EM30-Winkeln (Klaue, Sichel, Griffpunkt Knochen 15),
     *   - Fussgelenke (Ursprung Knochen 3/6, Modellraum) bleiben bis auf die Spreizung (<= SPREIZ + 40) liegen
     *     -> Bodenkontakt und Gangbild der EM30-Clips bleiben,
     *   - mehr als die Haelfte der Keyframes ist tatsaechlich geduckt (Wurzel >= 50 tiefer),
     *   - dahinter die Schwanzwinkel, die sich bewegen.
     * Nur die vom EDD referenzierten Keyframes (keyframe_count reicht bis zum Blobende). */
    {
        enum { BECKEN = 110, SPREIZ = 340 };   /* birkin_formen.py FORMEN['hangar'] ducken becken/spreiz */
        int kn = s_basis.anim.last_keyframe_index + 1;
        int ka = s_basis.skel.keyframe_size_bytes, kb = s_form.skel.keyframe_size_bytes;
        CHECK(kb >= 12 + (s_form.skel.bone_count * 36 + 7) / 8 && kb % 4 == 0,
              "%s: Keyframe-Groesse %d zu klein fuer %d Knochen", name, kb, s_form.skel.bone_count);
        int bewegt = 0, geduckt = 0, wurzel_abw = 0, winkel_abw = 0, fuss_weg = 0;
        double fuss_max = 0;
        const uint8_t *k0 = s_form.skel.keyframe_data;
        static const int fest[] = { 0, 12, 14, 15 };   /* Neuaufbau ab NB2: 11 = Klauen-Ellbogen 60 Grad nach vorn (NB3, Klaue) */
        for (int k = 0; k < kn; k++) {
            const uint8_t *a = s_basis.skel.keyframe_data + (size_t)k * ka;
            const uint8_t *b = s_form.skel.keyframe_data + (size_t)k * kb;
            int16_t ax, ay, az, bx, by, bz;
            re15_emd_get_keyframe_position(&s_basis.skel, k, &ax, &ay, &az);
            re15_emd_get_keyframe_position(&s_form.skel, k, &bx, &by, &bz);
            wurzel_abw += (ax != bx || az != bz || by < ay || by > ay + BECKEN || memcmp(a + 6, b + 6, 6) != 0);
            geduckt += by >= ay + 50;
            for (size_t i = 0; i < sizeof fest / sizeof fest[0]; i++) {
                int16_t w0[3], w1[3];
                re15_emd_get_keyframe_angles(&s_basis.skel, k, fest[i], &w0[0], &w0[1], &w0[2]);
                re15_emd_get_keyframe_angles(&s_form.skel, k, fest[i], &w1[0], &w1[1], &w1[2]);
                winkel_abw += memcmp(w0, w1, sizeof w0) != 0;
            }
            /* Fussgelenke im Modellraum: Wurzel + R0 (rel1 + R1 (rel2 + R2 rel3)) mit der Port-RotMatrix */
            for (int bein = 0; bein < 2; bein++) {
                double p[2][3];
                for (int s = 0; s < 2; s++) {
                    const re15_emd_skeleton_t *sk = s ? &s_form.skel : &s_basis.skel;
                    int16_t px, py, pz; re15_emd_get_keyframe_position(sk, k, &px, &py, &pz);
                    double R[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 }, t[3] = { px, py, pz };
                    const int kette[4] = { 0, 1 + 3 * bein, 2 + 3 * bein, 3 + 3 * bein };
                    for (int j = 0; j < 4; j++) {
                        int bb = kette[j];
                        if (j > 0) for (int r = 0; r < 3; r++)
                            t[r] += R[r * 3] * sk->bone_relative_pos[bb][0] + R[r * 3 + 1] * sk->bone_relative_pos[bb][1] +
                                    R[r * 3 + 2] * sk->bone_relative_pos[bb][2];
                        if (j == 3) break;
                        int16_t ex, ey, ez; int32_t m[9]; double M[9];
                        re15_emd_get_keyframe_angles(sk, k, bb, &ex, &ey, &ez);
                        re15_skel_euler_matrix(ex, ey, ez, m);
                        double Rn[9];
                        for (int q = 0; q < 9; q++) M[q] = m[q] / 4096.0;
                        for (int r = 0; r < 3; r++) for (int c = 0; c < 3; c++)
                            Rn[r * 3 + c] = R[r * 3] * M[c] + R[r * 3 + 1] * M[3 + c] + R[r * 3 + 2] * M[6 + c];
                        memcpy(R, Rn, sizeof R);
                    }
                    memcpy(p[s], t, sizeof t);
                }
                double dx = p[1][0] - p[0][0], dy = p[1][1] - p[0][1], dz = p[1][2] - p[0][2];
                double d = sqrt(dx * dx + dy * dy + dz * dz);
                if (d > fuss_max) fuss_max = d;
                fuss_weg += d > SPREIZ + 40;
            }
            bewegt += memcmp(b + ka, k0 + ka, (size_t)(kb - ka)) != 0;
        }
        CHECK(wurzel_abw == 0, "%s: %d Keyframes mit Wurzel x/z/Spur anders oder Becken ausserhalb 0..%d", name, wurzel_abw,
              BECKEN);
        CHECK(winkel_abw == 0, "%s: %d Winkel der Knochen 0/12/14/15 weichen von EM30 ab", name, winkel_abw);
        CHECK(fuss_weg == 0, "%s: %d Fussgelenke mehr als %d vom EM30-Ort (max %.0f)", name, fuss_weg, SPREIZ + 40, fuss_max);
        CHECK(geduckt > kn / 2, "%s: nur %d von %d Keyframes geduckt", name, geduckt, kn);
        CHECK(bewegt > kn / 2, "%s: Schwanz starr (%d von %d Keyframes anders als Keyframe 0)", name, bewegt, kn);
        fprintf(stderr, "%s: %d von %d Keyframes geduckt, Fussgelenk max %.0f vom EM30-Ort, %d mit bewegtem Schwanz\n",
                name, geduckt, kn, fuss_max, bewegt);
    }

    long geaendert = 0, uv_neu = 0;
    for (int m = 0; m < s_form.md1.mesh_count; m++) {
        const re15_md1_mesh_t *b = &s_form.md1.meshes[m];
        if (m < nb0) {
            const re15_md1_mesh_t *a = &s_basis.md1.meshes[m];
            if (geteilt(m)) {
                /* Dreiecke unveraendert (Topologie), Vierecke: mehr als vorher, alle Indizes im Block, alte Ecken
                 * an ihrem Platz (neue Ecken haengen hinten an). */
                CHECK(b->triangle_count == a->triangle_count && b->quad_count > a->quad_count &&
                      b->quad_vertex_count > a->quad_vertex_count, "%s: Mesh %d nicht geteilt (Quads %d/%d)", name, m,
                      b->quad_count, a->quad_count);
                CHECK(b->quad_count - a->quad_count == (b->quad_vertex_count - a->quad_vertex_count) / 2,
                      "%s: Mesh %d: %d neue Quads zu %d neuen Ecken", name, m, b->quad_count - a->quad_count,
                      b->quad_vertex_count - a->quad_vertex_count);
                if (a->triangle_count > 0)
                    CHECK(!memcmp(a->triangles, b->triangles, sizeof *a->triangles * (size_t)a->triangle_count),
                          "%s: Mesh %d Dreieckstopologie weicht ab", name, m);
                for (int q = 0; q < b->quad_count; q++) {
                    const re15_md1_quad_t *f = &b->quads[q];
                    CHECK(f->v0 < b->quad_vertex_count && f->v1 < b->quad_vertex_count && f->v2 < b->quad_vertex_count &&
                          f->v3 < b->quad_vertex_count && f->n0 < b->quad_normal_count && f->n1 < b->quad_normal_count &&
                          f->n2 < b->quad_normal_count && f->n3 < b->quad_normal_count,
                          "%s: Mesh %d Quad %d zeigt ausserhalb", name, m, q);
                }
                uv_neu++;
                for (int v = 0; v < a->tri_vertex_count; v++)
                    geaendert += memcmp(&a->tri_vertices[v], &b->tri_vertices[v], sizeof *a->tri_vertices) != 0;
                continue;
            }
            const int zd = m == 8 ? EINSATZ_D : 0, zv = m == 8 ? EINSATZ_V : 0, ze = m == 8 ? EINSATZ_E : 0;   /* V8 Runde 6b */
            CHECK(b->triangle_count == a->triangle_count + zd && b->quad_count == a->quad_count + zv &&
                  b->tri_vertex_count == a->tri_vertex_count + ze && b->quad_vertex_count == a->quad_vertex_count + ze,
                  "%s: Mesh %d Elementzahlen %d/%d/%d/%d weichen von EM30 %d/%d/%d/%d + Einsatz %d/%d/%d ab", name, m, b->triangle_count,
                  b->quad_count, b->tri_vertex_count, b->quad_vertex_count, a->triangle_count, a->quad_count, a->tri_vertex_count,
                  a->quad_vertex_count, zd, zv, ze);
            if (b->triangle_count != a->triangle_count + zd || b->quad_count != a->quad_count + zv) continue;
            if (a->triangle_count > 0)
                CHECK(!memcmp(a->triangles, b->triangles, sizeof *a->triangles * (size_t)a->triangle_count),
                      "%s: Mesh %d Dreieckstopologie weicht ab", name, m);
            if (a->quad_count > 0)
                CHECK(!memcmp(a->quads, b->quads, sizeof *a->quads * (size_t)a->quad_count),
                      "%s: Mesh %d Quadtopologie weicht ab", name, m);
            int uv_gleich =
                (a->triangle_count == 0 ||
                 !memcmp(a->triangle_uvs, b->triangle_uvs, sizeof *a->triangle_uvs * (size_t)a->triangle_count)) &&
                (a->quad_count == 0 ||
                 !memcmp(a->quad_uvs, b->quad_uvs, sizeof *a->quad_uvs * (size_t)a->quad_count));
            if (umtexturiert(m)) { CHECK(!uv_gleich, "%s: Mesh %d nicht umtexturiert", name, m); uv_neu++; }
            else CHECK(uv_gleich, "%s: Mesh %d UVs weichen ab", name, m);
            if (nur_clut(m))
                CHECK(uv_ohne_clut_gleich(a, b) && uv_ohne_clut_anders(a, b) == 0,
                      "%s: Mesh %d UVs ausser dem CLUT-Feld weichen von EM30 ab (%d Flaechen)", name, m, uv_ohne_clut_anders(a, b));
            if (m == 8) {                /* V8 Runde 5 (adern_r5.py, Sicht-Pruefer r4 (3) "graue Platte ... weisse Keile"): auch die Brust (34
                                          * Flaechen, bis Runde 4 Zeile 488) auf ihrer Original-Zeile 481 -> 494 mit eigener UV; Mesh 8 ganz auf
                                          * 493/494 (gemessen 493:56, 494:48, 488/480/481: 0); 28 Flaechen mit anderer UV als das Original-EM30
                                          * (rueckenfleisch 2, huegel 1, blass_rechts 3, weiss 20, blass_r5 2) */
                CHECK(zeile_zahl(b, 488) == 0, "%s: Brust (Mesh 8) %d Flaechen noch auf CLUT 488 (Soll 0, Runde 5: Original-Zeile 481)", name,
                      zeile_zahl(b, 488));
                CHECK(zeile_zahl(b, Z_X1) == 56 + EINSATZ_D + EINSATZ_V && zeile_zahl(b, Z_X2) == 48,
                      "%s: Mesh 8 Zeilen 493/494 %d/%d Flaechen statt %d/48 (V8 Runde 6b: 56 + Nackenstueck)", name, zeile_zahl(b, Z_X1),
                      zeile_zahl(b, Z_X2), 56 + EINSATZ_D + EINSATZ_V);
                /* V8 Runde 6b: jede Flaeche des Nackenstuecks zeigt nur auf die 7 angehaengten Ecken (die EM30-Ecken und -Flaechen bleiben an
                 * ihrem Platz), liegt auf Zeile 493 und Textur-Seite 0, und ihre UV-Ecken liegen im gemessenen Rechteck u 43-53, v 88-101
                 * (nacken_r6.py prueft je Texel, dass sie in der Original-Nutzung der Rueckenhaut liegen) */
                {
                    int fehl = 0;
#define IM_NACKEN(u, v) ((u) >= EINSATZ_U0 && (u) <= EINSATZ_U1 && (v) >= EINSATZ_V0 && (v) <= EINSATZ_V1)
                    for (int t = a->triangle_count; t < b->triangle_count; t++) {
                        const re15_md1_triangle_t *f = &b->triangles[t];
                        const re15_md1_tri_uv_t *u = &b->triangle_uvs[t];
                        fehl += !(f->v0 >= a->tri_vertex_count && f->v1 >= a->tri_vertex_count && f->v2 >= a->tri_vertex_count &&
                                  f->v0 < b->tri_vertex_count && f->v1 < b->tri_vertex_count && f->v2 < b->tri_vertex_count);
                        fehl += !(u->clut == (uint16_t)(Z_X1 << 6) && (u->page & 0xF) == 0 && IM_NACKEN(u->u0, u->v0) &&
                                  IM_NACKEN(u->u1, u->v1) && IM_NACKEN(u->u2, u->v2));
                    }
#undef IM_NACKEN
                    CHECK(fehl == 0, "%s: Nackenstueck an Mesh 8: %d Verstoesse (Ecken ausserhalb der 7 neuen, Zeile/Seite oder UV ausserhalb "
                          "u 43-53 / v 88-101)", name, fehl);
                }
                CHECK(zeile_zahl(b, 480) == 0 && zeile_zahl(b, 481) == 0, "%s: Mesh 8 %d/%d Flaechen noch auf V7-Zeile 480/481 (Naht, Runde 4)",
                      name, zeile_zahl(b, 480), zeile_zahl(b, 481));
                CHECK(uv_ohne_clut_anders(a, b) == 29, "%s: Mesh 8 %d Flaechen mit anderer UV (Soll 29, V8 Runde 6b: Runde 5 + d53)", name,
                      uv_ohne_clut_anders(a, b));
            }
            if (m == 14) {               /* V8 Runde 4: alle 37 Flaechen auf 495 (= Original-Zeile 482); Band v9/v10/d19/d22 mit Schub */
                CHECK(zeile_zahl(b, Z_X3) == 37 && zeile_zahl(b, 482) == 0, "%s: Unterarm (Mesh 14) %d Flaechen auf X3 (Soll 37), %d auf 482 (Soll 0)",
                      name, zeile_zahl(b, Z_X3), zeile_zahl(b, 482));
                CHECK(uv_ohne_clut_anders(a, b) == 4, "%s: Unterarm (Mesh 14) %d Flaechen mit anderer UV (Soll 4, Band)", name, uv_ohne_clut_anders(a, b));
            }
            if (m == 13) {               /* V8 Runde 4: alle 56 Flaechen auf 493; Feld C (24 Flaechen) mit einem Schub auf Oberarm-Haut */
                CHECK(zeile_zahl(b, Z_X1) == 56 && zeile_zahl(b, 480) == 0 && zeile_zahl(b, 489) == 0,
                      "%s: Oberarm (Mesh 13) %d Flaechen auf 493 (Soll 56), %d auf 480, %d auf 489 (Soll 0)", name, zeile_zahl(b, Z_X1),
                      zeile_zahl(b, 480), zeile_zahl(b, 489));
                CHECK(uv_ohne_clut_anders(a, b) == 24, "%s: Oberarm (Mesh 13) %d Flaechen mit anderer UV (Soll 24, Feld C)", name,
                      uv_ohne_clut_anders(a, b));
            }
            if (m == 12) {               /* V7 linke Hand 2.: Daumenkralle T 6 Flaechen -> 491 (grau), Fingerkrallen A/B/C 18 -> 492 (rot),
                                          * Rest der Klauenhand 490 (V5 wie Foto b, Kappe v16 zurueck auf 490); gemessen 490:34, 491:6, 492:18 */
                CHECK(zeile_zahl(b, 491) == 6, "%s: Daumenkralle (Mesh 12) %d Flaechen auf CLUT 491 statt 6", name, zeile_zahl(b, 491));
                CHECK(zeile_zahl(b, 492) == 18, "%s: Fingerkrallen (Mesh 12) %d Flaechen auf CLUT 492 statt 18", name,
                      zeile_zahl(b, 492));
                CHECK(zeile_zahl(b, 490) == b->quad_count + b->triangle_count - 24, "%s: Klauenhand (Mesh 12) %d Flaechen auf 490 statt %d",
                      name, zeile_zahl(b, 490), b->quad_count + b->triangle_count - 24);
                /* V8 Runde 6c (P7, Nutzer 2026-11-10: "sieht es so aus als waere die linke Hand verkehrt herum gedreht"; hand_r6.py /
                 * hand_v8.json skala 0.77 0.72 0.77, dreh_grad 0, Krallen-Wurzelring 1.6): breite Hand in Capcoms Lage - Spanne der
                 * 52 Ecken und Wurzelring (Ecken 1-4) hinter dem Gelenk; gemessen am Asset e9e71bca. Gegenproben: der um 118 Grad
                 * verdrehte Stand 95a783a6 hatte 484 / 716 / 433 (Spanne z = Dicke der Hand fast so gross wie x), r5d / V7
                 * 343 / 594 / 235, y 16-19.
                 * V8 Runde 7a (K3, hand_v8.json quelle.krallen_r7a / daumen_r7a: Krallenspitzen A/C an der B-Spitze, Daumen entlang
                 * seiner Achse laengs 0.75 quer 1.25): Spanne x 453 -> 462, z 241 -> 261, y bleibt 716; gemessen am Asset a474188a */
                CHECK(b->tri_vertex_count == 52 && spanne(b, 0) == 462 && spanne(b, 1) == 716 && spanne(b, 2) == 261,
                      "%s: Klauenhand (Mesh 12) %d Ecken, Spanne %d / %d / %d statt 52, 462 / 716 / 261", name, b->tri_vertex_count,
                      spanne(b, 0), spanne(b, 1), spanne(b, 2));
                for (int v = 1; v <= 4 && v < b->tri_vertex_count; v++)
                    CHECK(b->tri_vertices[v].y == -22, "%s: Klauenhand (Mesh 12) Wurzelring-Ecke %d y %d statt -22", name, v,
                          b->tri_vertices[v].y);
            }
            if (m == 11) {               /* V8 Runde 6c (P7): Unterarm nach dem Breitenprofil der Vorlage in den echten Szenenposen
                                          * (hand_v8.json unterarm verlauf, Ellbogen-Ende 1.2 / 1.15); gemessen am Asset e9e71bca,
                                          * Gegenproben: Stand 95a783a6 444 / 640 / 407, r5d / V7 Spanne x / z 516 / 472 */
                CHECK(b->tri_vertex_count == 18 && spanne(b, 0) == 464 && spanne(b, 1) == 640 && spanne(b, 2) == 426,
                      "%s: Unterarm (Mesh 11) %d Ecken, Spanne %d / %d / %d statt 18, 464 / 640 / 426", name, b->tri_vertex_count,
                      spanne(b, 0), spanne(b, 1), spanne(b, 2));
            }
            if (m == 9) {                /* N1.10: Kopf auf eigener CLUT-Zeile; V7: 486 (V6 Kopf) */
                int kopf = 0, ecken = 0;
                for (int q = 0; q < b->quad_count; q++) kopf += b->quad_uvs[q].clut == CLUT_KOPF;
                for (int t = 0; t < b->triangle_count; t++) kopf += b->triangle_uvs[t].clut == CLUT_KOPF;
                CHECK(kopf == b->quad_count + b->triangle_count, "%s: Kopf (Mesh 9) nur %d/%d Teile auf CLUT 486",
                      name, kopf, b->quad_count + b->triangle_count);
                /* V6 Kopf T-H6: Mesh 9 = Original-Mesh bis auf die Glaettung (<= 4 Ecken; gemessen 2: Ecken 54 und 60) */
                for (int v = 0; v < a->tri_vertex_count; v++)
                    ecken += memcmp(&a->tri_vertices[v], &b->tri_vertices[v], sizeof *a->tri_vertices) != 0;
                CHECK(ecken <= 4, "%s: Kopf (Mesh 9) %d Ecken anders als der Original-Kopf (Soll <= 4)", name, ecken);
            }
            if (m == 15) {
                int sichel = 0;
                for (int q = 0; q < b->quad_count; q++) sichel += b->quad_uvs[q].clut == CLUT_SICHEL;
                for (int t = 0; t < b->triangle_count; t++) sichel += b->triangle_uvs[t].clut == CLUT_SICHEL;
                CHECK(sichel == b->quad_count + b->triangle_count, "%s: Sichel (Mesh 15) nur %d/%d Teile auf CLUT 484",
                      name, sichel, b->quad_count + b->triangle_count);
            }
            for (int v = 0; v < a->tri_vertex_count; v++)
                geaendert += memcmp(&a->tri_vertices[v], &b->tri_vertices[v], sizeof *a->tri_vertices) != 0;
        } else {
            CHECK(b->quad_count > 0, "%s: Schwanzglied-Mesh %d leer", name, m);
            /* V8 Runde 5 (P4, Nutzer 2026-11-09 "Die Unterseite des großen Schwanzes ist fleischig ... Nur die oberseite ist durchweg grau";
             * tools/r41_sichel/v8/unterseite_r5.py): die Flaechen der Unterseite (Mittel |theta| < 130 Grad gegen die Innenrichtung des Bogens,
             * ohne Deckel und Wurzelflaechen) liegen auf Zeile 495 (= Original-EM30-Zeile 482) mit UV auf dem Haut/Fleisch-Verlauf des
             * Original-Unterarms (Textur-Seite 2); alle uebrigen (Oberseite, V6-Deckel) bleiben auf der Schwanzzeile 487. Gemessen am Asset
             * (v8_bau.py Protokoll "unterseite"): 16: 8, 17: 8, 18: 16, 19: 8, 20: 8, 21: 8, 22: 16 = 72 Flaechen.
             * V8 Runde 6b (P4-neu, Pruefung r5d "graue Reifen"): Grenze 110 / 120 Grad, Mantel / Dorn / Ring-Deckel an ihr geteilt, auch
             * die Deckel-Haelften der Unterseite auf 495; gemessen am Asset 95a783a6: 21 / 30 / 41 / 31 / 30 / 30 / 30 = 213 Flaechen
             * (erster Stand 382bbfe7: Glied 16 nur 20 - das Bandstueck v22 zwischen 110 und 120 Grad galt dem Bau noch als Wurzelflaeche
             * und blieb auf 487; szene_marker.wurzel_flaechen kennt seit 118fad898 die Teilungs-Ecken),
             * Ecken je Glied 43 / 32 / 47 / 32 / 32 / 32 / 32 (Runde 5d: 35 / 24 / 35 / 24 / 24 / 24 / 24).
             * V8 Runde 7b (K1 "Unterseite wie Fleisch, nicht wie eine glatte rote Flaeche", K2 "7 graue Deckel-Dreiecke";
             * tools/r41_sichel/v8/unterseite_r7b.py, unterseite_v8.json fleisch_r7b): je Glied 3 Abschnitte (neue Ringe auf den
             * Laengskanten, Form gleich), Fleisch in Original-Dichte; die 7 Kegel-Dreiecke und die Rohr-Vierecke der Wurzel unter der
             * Grenze auf 495. Gemessen am Asset f3e1ceff (sfv8/r7b): 55 / 50 / 51 / 51 / 50 / 50 / 50 = 357 Flaechen auf 495, Ecken
             * 75 / 62 / 63 / 64 / 62 / 62 / 62. Gegenprobe Stand a474188a: rot (213 Flaechen, Ecken wie Runde 6b). */
            static const int UNTEN[7] = {55, 50, 51, 51, 50, 50, 50};
            static const int ECKEN[7] = {75, 62, 63, 64, 62, 62, 62};
            CHECK(b->tri_vertex_count == ECKEN[m - 16], "%s: Schwanzglied %d: %d Ecken statt %d (V8 Runde 7b Abschnitte)", name, m,
                  b->tri_vertex_count, ECKEN[m - 16]);
            int auf487 = zeile_zahl(b, 487), auf495 = zeile_zahl(b, Z_X3);
            CHECK(auf495 == UNTEN[m - 16] && auf487 + auf495 == b->quad_count + b->triangle_count,
                  "%s: Schwanzglied %d: %d Flaechen auf 495 (Soll %d), %d auf 487, %d gesamt (V8 Runde 7b Unterseite)", name, m, auf495,
                  UNTEN[m - 16], auf487, b->quad_count + b->triangle_count);
            for (int q = 0; q < b->quad_count; q++)
                if (b->quad_uvs[q].clut == (uint16_t)(Z_X3 << 6))
                    CHECK((b->quad_uvs[q].page & 0xF) == 2, "%s: Schwanzglied %d Quad %d (Unterseite) Textur-Seite %d statt 2", name, m, q,
                          b->quad_uvs[q].page & 0xF);
            for (int q = 0; q < b->quad_count; q++) {
                const re15_md1_quad_t *f = &b->quads[q];
                CHECK(f->v0 < b->quad_vertex_count && f->v1 < b->quad_vertex_count &&
                      f->v2 < b->quad_vertex_count && f->v3 < b->quad_vertex_count &&
                      f->n0 < b->quad_normal_count && f->n1 < b->quad_normal_count &&
                      f->n2 < b->quad_normal_count && f->n3 < b->quad_normal_count,
                      "%s: Mesh %d Quad %d zeigt ausserhalb", name, m, q);
            }
            for (int t = 0; t < b->triangle_count; t++) {
                const re15_md1_triangle_t *f = &b->triangles[t];
                CHECK(f->v0 < b->tri_vertex_count && f->v1 < b->tri_vertex_count &&
                      f->v2 < b->tri_vertex_count && f->n0 < b->tri_normal_count &&
                      f->n1 < b->tri_normal_count && f->n2 < b->tri_normal_count,
                      "%s: Mesh %d Dreieck %d zeigt ausserhalb", name, m, t);
            }
        }
    }
    CHECK(geaendert > 0, "%s: keine einzige Vertex-Aenderung gegenueber EM30", name);
    /* CLUT-Block (N1.4/N1.5/N1.7; V7): 3 EM30-Zeilen + 10 Zusatzzeilen 483-492 (V7 linke Hand T-V7f: 13 Zeilen), hoechstens 16
     * Zeilen und Port-Texturhoehe tex_h = tim->height x n_cluts <= 4096 (render_pc.c re15_render_pc_upload_tim_slot; gemessen
     * 256 x 13 = 3328). Haut umgefaerbt; blaue Eintraege (Jeans) gleich - ausser den 30 mit Paletten-a* >= 10, die V5 wie Foto b)
     * grau macht (gemessen: 30 geaendert, alle neu mit 5-Bit-Spanne <= 1, C* <= 4,4). Zeile 486 = EM30-Zeile 0 bytegleich
     * (V6 Kopf Schritt 3). */
    const int zeilen = s_form.tim.clut_entries / 256;
    CHECK(s_basis.tim.clut_entries == 3 * 256 && s_form.tim.clut_entries == CLUT_ZEILEN * 256 &&
          s_form.tim.clut_y == s_basis.tim.clut_y, "%s: CLUT %d Eintraege (Soll %d x 256) ab y %d", name,
          s_form.tim.clut_entries, CLUT_ZEILEN, s_form.tim.clut_y);
    CHECK(s_form.tim.bpp == 8 && zeilen <= 16 && s_form.tim.height * zeilen <= TEX_H_MAX,
          "%s: %d CLUT-Zeilen (<= 16), Port-Texturhoehe %d x %d = %d (<= %d), bpp %d", name, zeilen, s_form.tim.height, zeilen,
          s_form.tim.height * zeilen, TEX_H_MAX, s_form.tim.bpp);
    if (s_form.tim.clut_entries == CLUT_ZEILEN * 256) {
        int blau_anders = 0, blau_bunt = 0, haut_anders = 0;
        for (int i = 0; i < 3 * 256; i++) {
            const uint16_t c = s_basis.tim.clut[i], d = s_form.tim.clut[i];
            const int r = c & 31, bl = (c >> 10) & 31;
            if (bl > r) {
                blau_anders += c != d;
                if (c != d) {
                    const int dr = d & 31, dg = (d >> 5) & 31, db = (d >> 10) & 31;
                    const int hi = dr > dg ? (dr > db ? dr : db) : (dg > db ? dg : db);
                    const int lo = dr < dg ? (dr < db ? dr : db) : (dg < db ? dg : db);
                    blau_bunt += hi - lo > 1;
                }
            } else haut_anders += c != d;
            CHECK((c == 0) == (d == 0), "%s: CLUT %d Transparenz geaendert (%04x -> %04x)", name, i, c, d);
        }
        CHECK(blau_anders == 30, "%s: %d blaue CLUT-Eintraege umgefaerbt (Soll 30: die mit a* >= 10, V5 wie Foto b)", name, blau_anders);
        CHECK(blau_bunt == 0, "%s: %d umgefaerbte blaue Eintraege nicht grau (5-Bit-Spanne > 1)", name, blau_bunt);
        CHECK(haut_anders > 200, "%s: nur %d Haut-Eintraege umgefaerbt", name, haut_anders);
        CHECK(!memcmp(s_form.tim.clut + 6 * 256, s_basis.tim.clut, 256 * sizeof(uint16_t)),
              "%s: CLUT-Zeile 486 ist nicht die EM30-Zeile 0 (Original-Kopf, V6 Kopf)", name);
        /* V8 Runde 3 (O1): Zeilen 493/494/495 = EM30-Zeilen 480/481/482 (CDEMD0.EMS Typ 0x30) bytegleich - jeder Eintrag aus dem Original */
        for (int k = 0; k < 3; k++) {
            int fremd = 0;
            for (int i = 0; i < 256; i++) fremd += s_form.tim.clut[(13 + k) * 256 + i] != s_basis.tim.clut[k * 256 + i];
            CHECK(fremd == 0, "%s: Zeile %d: %d Eintraege nicht = EM30-Zeile %d (V8 Runde 3: volle Original-Zeile)", name, 493 + k, fremd, 480 + k);
        }
        fprintf(stderr, "%s: %d Haut-CLUT-Eintraege umgefaerbt, %d blaue grau, %d Zusatzzeilen, Port-Texturhoehe %d\n", name,
                haut_anders, blau_anders, zeilen - 3, s_form.tim.height * zeilen);
    }
    fprintf(stderr, "%s: %d Meshes, %d Knochen, %d Clips, %ld Tri-Vertices umgeformt, %ld Teile umtexturiert\n",
            name, s_form.md1.mesh_count, s_form.skel.bone_count, s_form.anim.clip_count, geaendert, uv_neu);
    free(blob);
}

int main(void)
{
    size_t esz = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
    if (!ems) { fprintf(stderr, "SKIP: CDEMD0.EMS fehlt\n"); return 0; }
    size_t off = 0, len = 0;
    int idx = re15_ems_index_for_type(0x30);
    if (idx < 0 || re15_ems_get_entry(ems, esz, idx, &off, &len) != 0) {
        fprintf(stderr, "FAIL: EM30 nicht in CDEMD0.EMS\n");
        return 1;
    }
    int rc = re15_emd_parse_container(ems + off, len, &s_basis.md1, &s_basis.skel,
                                      &s_basis.anim, &s_basis.tim);
    CHECK(rc == 0, "EM30: re15_emd_parse_container rc=%d", rc);
    if (rc == 0) {
        vergleiche("EM30_HANGAR.EMD", ems + off, len);
    }
    free(ems);
    if (fails) { fprintf(stderr, "test_birkin_formen: %d Fehler\n", fails); return 1; }
    fprintf(stderr, "test_birkin_formen: OK\n");
    return 0;
}
