/*
 * Runde 36 Spur F "zombieki" — Gorilla-Schattenrecord und Blutlache der Leiche (Belege: re15_zombieki_r36.h).
 */
#include "re15_zombieki_r36.h"
#include "re15_room.h"
#include "re15_ai_flavor.h"
#include "re15_collision.h"
#include "re15_skeleton.h"   /* re15_cos_q12 / re15_sin_q12 */
#include "re15_scd.h"        /* g_scd.props (Obj_model_set) */
#include "re15_damage.h"     /* re15_enemy_bone_world_pos (Kopfteil 8) */
#include "re15_enemy_ai.h"   /* re15_re2z_owns_type, re15_nav_* */
#include <string.h>
#include <stdlib.h>   /* abs */

/* ----- Punkt 1: Sicht-Bit des RE2-Zombies (Belege: re15_zombieki_r36.h) ----- */
static int s_r36_zombie_sicht_an = 1;   /* NUR Sonden: 0 = alter Weg re15_re2_los_clear (Slab-Test) */
void re15_r36_zombie_sicht_enable(int an) { s_r36_zombie_sicht_an = an ? 1 : 0; }

/* ---- NB1 Mangel 1 / NB2 Mangel 2: Oberkante niedriger Nibble-3-Moebel (Belege: re15_zombieki_r36.h) ---- */
static int s_r36_hoehe_an = 1;   /* NUR Sonden: 0 = Abbildung vor NB1, 2 = NB1-Abbildung (Gegenprobe Mangel 2) */
void re15_r36_zombie_hoehe_enable(int an) { s_r36_hoehe_an = (an == 2) ? 2 : (an ? 1 : 0); }

/* BELEG-TABELLE (Nachbesserung 2, Abnahme 1 Mangel 2): NUR diese Zellen sind Moebel; jede andere Nibble-3-Zelle hat volle
 * Hoehe (Wand). Schluessel = Raum ohne Varianten-Nibble (Leon/Elza teilen die Zellen) + Zell-Rechteck (SCA-Satz).
 * grund 1 = (a) Obj_model_set RUHT auf der Zelle: Oberkante = sein y-Operand pc[12..13] (Datei-Byte-Offset genannt);
 *           das MD1-Modell liegt ganz UEBER seinem Ursprung (Vertex-y nur <= 0, Unterkante = Ursprung = Standflaeche).
 * grund 2 = (b) PORT-WAHL (Messwert): Projektion mit den Engine-Matrizen (probe_r34n_e_dokumente kamera 1010, Kamera-Record
 *           Cut 0 @ROOM1010.RDT 0x60) auf das Hintergrundbild Cut 0, geeicht an der Tischplatte (Ecken bei y -1600 liegen auf
 *           den Tischkanten); die Marke y -2000 der Stuhlsaeule liegt auf der Wand UEBER der Lehne -> Oberkante tiefer als
 *           -2000, als OBERGRENZE eingetragen (Bild F_belege/nb1_r1010_c0_hoehen.png, Skript F_belege/nb1_hoehe_1010.py). */
static const struct { uint16_t raum; int16_t x, z; uint16_t w, d; int16_t oben; uint8_t grund; } k_r36_moebel[] = {
    /* ROOM1010/1011 Verhoerraum: Tisch #5 (SCA @0x004f8). Obj_model_set id0 (200,-1600,5500) @ROOM1010.RDT 0x00930, y-Bytes
     * @0x0093c = c0 f9 (ROOM1011 @0x008ee / @0x008fa); id1 (1800,-1600,5750) @0x00952, y @0x0095e (ROOM1011 @0x00910 / @0x0091c);
     * MD1 id0 y -489..0, id1 y -72..0 (Gegenstaende auf der Platte). */
    { 0x1010,   -50,   1550, 2500, 4350, -1600, 1 },
    /* ROOM1010/1011 Stuhlkreise (t3) #12/#13/#14 (SCA @0x0054c/@0x00558/@0x00564): (b) Messwert, Obergrenze. */
    { 0x1010,  1850,   4100, 1200, 1200, -2000, 2 },
    { 0x1010,  1850,   2200, 1200, 1200, -2000, 2 },
    { 0x1010,  -700,   2800, 1200, 1200, -2000, 2 },
    /* ROOM1220/1221 Zellentrakt: Betten (je Zelle Bett 1950/2125 x 3650..4150, Kreis 1200 = WC, 1375 x 1000 = Becken; Zell-
     * waende 200 dick). Auf drei Betten ruht ein Obj_model_set (MD1 nv40 x -136..113 y -176..-1 z -255..256) bei y -1000:
     * #18 (SCA @0x00620) id0 @ROOM1220.RDT 0x00e72, y-Bytes @0x00e7e = 18 fc; #21 (@0x00644) id2 @0x00eb6, y @0x00ec2;
     * #34 (@0x006e0) id1 @0x00e94, y @0x00ea0 (ROOM1221 bytegleich an denselben Offsets). */
    { 0x1220, -27600, -11700, 1950, 3650, -1000, 1 },
    { 0x1220, -27600, -19750, 1950, 4150, -1000, 1 },
    { 0x1220, -13475, -23600, 2125, 3800, -1000, 1 },
};

/* NB1-Abbildung (nur noch Gegenprobe der Sonden, s_r36_hoehe_an == 2): jedes Weltraum-Obj_model_set im Rechteck. */
static int32_t r36_oberkante_nb1(const re15_sca_entry_t *c, int *g)
{
    int32_t top = 0;
    for (int i = 0; i < (int)g_scd.prop_count && i < RE15_SCD_MAX_PROPS; i++) {
        const int32_t px = g_scd.props[i].x, py = g_scd.props[i].y, pz = g_scd.props[i].z;
        if (g_scd.props[i].parent_obj >= 0 || py >= 0) continue;
        if ((uint32_t)(px - (int32_t)c->x) >= (uint32_t)c->width || (uint32_t)(pz - (int32_t)c->z) >= (uint32_t)c->density) continue;
        if (*g == 0 || py > top) { top = py; *g = 1; }
    }
    return top;
}

int32_t re15_r36_zelle_oberkante(const re15_sca_entry_t *c, int *grund)
{
    int32_t top = 0; int g = 0;
    if (s_r36_hoehe_an == 2) top = r36_oberkante_nb1(c, &g);
    else
        for (unsigned k = 0; k < sizeof k_r36_moebel / sizeof k_r36_moebel[0]; k++)
            if ((g_current_room_id & 0xFFF0u) == k_r36_moebel[k].raum && c->x == k_r36_moebel[k].x && c->z == k_r36_moebel[k].z &&
                c->width == k_r36_moebel[k].w && c->density == k_r36_moebel[k].d) { top = k_r36_moebel[k].oben; g = k_r36_moebel[k].grund; break; }
    if (grund) *grund = g;
    return top;
}

/* ---- NB2 Mangel 1: der Wurf endet nicht in einer unaufloesbaren Lage (Belege + PORT-WAHL: re15_zombieki_r36.h) ---- */
static int s_r36_wurf_an = 1;
void re15_r36_wurf_klemme_enable(int an) { s_r36_wurf_an = an ? 1 : 0; }
void re15_r36_wurf_klemme(re15_actor_t *e, int32_t ox, int32_t oz)
{
    if (!s_r36_wurf_an || !e || !g_room_rdt_ok || e->hit_radius_min == 0) return;
    if (!re15_ai_re2_for_type(e->type) || !re15_re2z_owns_type(e->type)) return;     /* nur das RE2-Zombie-Hirn */
    if (!(e->state == 1 && e->sub_state_1 == 3 && (e->sub_state_2 == 7 || e->sub_state_2 == 8))) return;
                                                                                     /* EXEC[3] P7 @0x80102C30 / P8 @0x80102C60 */
    int32_t nx = e->x, nz = e->z; uint8_t ct = e->ai_contact; uint16_t at = 0;
    re15_collision_constrain_contact(&g_room_rdt, e->x, e->z, &nx, &nz, e->hit_radius_min, e->y,
                                     e->sca_mask ? e->sca_mask : 4u, &ct, &at);     /* dieselbe Klemme wie eben (Maske +0x1d7) */
    if (nx == e->x && nz == e->z) return;                                            /* stabile Lage: nichts */
    e->x = ox; e->z = oz;                                                            /* Rueckfall auf die Vorlage (Form
                                                                                      * `lh v0,0(s4)` / `sw v0,0(t2)` / `lh v1,4(s4)` /
                                                                                      * `sw v1,8(t2)` @0x8003ced4-ceec, LAB_8003cb9c) */
}

/* RE2 FUN_80050858 mit a3 = 1 (@0x80050b34-c30): blockt nur, wo der Strahl A->B im Satz-Rechteck tiefer liegt als die
 * Oberkante (y nach oben negativ). Abgetastet im 50er-Schritt ueber die Strecke (Ganzzahl). 1 = Strahl geht darueber. */
static int r36_strahl_darueber(const re15_sca_entry_t *c, const int32_t a[3], const int32_t b[3], int32_t top)
{
    int64_t dx = (int64_t)b[0] - a[0], dz = (int64_t)b[2] - a[2];
    int64_t len2 = dx * dx + dz * dz; int n = 1;
    while ((int64_t)n * n * 2500 < len2 && n < 4096) n++;
    for (int i = 0; i <= n; i++) {
        int32_t x = a[0] + (int32_t)(dx * i / n), z = a[2] + (int32_t)(dz * i / n);
        if ((uint32_t)(x - (int32_t)c->x) >= (uint32_t)c->width || (uint32_t)(z - (int32_t)c->z) >= (uint32_t)c->density) continue;
        int32_t y = a[1] + (int32_t)(((int64_t)b[1] - a[1]) * i / n);
        if (y >= top) return 0;                                                       /* Strahl im Grundriss nicht ueber der Kante */
    }
    return 1;
}

int re15_r36_zombie_sicht_frei(re15_actor_t *e, re15_actor_t *pl)
{
    if (!s_r36_zombie_sicht_an) return re15_re2_los_clear(e, pl) ? 1 : 0;
    if (!g_room_rdt_ok) return 1;                                   /* keine Zellen -> frei (wie bisher) */
    if (!s_r36_hoehe_an) return re15_r36_los_ray_nib3(e, pl->x, pl->z) ? 0 : 1;   /* Sonden: Abbildung vor NB1 */
    /* FUN_8003dcc4(0xf00, 0x300) @0x8001b8d4/8ec ueber die 4 Regionen wie re15_los_ray_blocked — je Zelle: */
    unsigned maske = e->sca_mask ? e->sca_mask : 4u;
    int kopf = 0; int32_t ka[3] = { 0, 0, 0 }, kb[3] = { 0, 0, 0 };
    int start = 0;
    for (int k = 0; k < 4; k++) {
        int cnt = g_room_rdt.sca_rgn[k];
        if (start + cnt > g_room_rdt.sca_count) break;
        for (int i = 0; i < cnt; i++) {
            const re15_sca_entry_t *c = &g_room_rdt.sca[start + i];
            if ((c->floor >> 4) != e->floor) continue;              /* w5>>12 == +0x82 @0x8003de60-6c */
            if ((c->floor & 0x0f) != 3) continue;                   /* (w5&0xf00) == 0x300 @0x8003de74-94 */
            if (!(maske & c->u0)) continue;                         /* Aktor-Maske (Runde 35 Spur H) */
            if (!re15_r36_los_zelle_kreuzt(e->x, e->z, pl->x, pl->z, c)) continue;
            int32_t top = re15_r36_zelle_oberkante(c, NULL);
            if (top == 0) return 0;                                 /* ohne belegte Hoehe: blockt (RE1.5-Klasse, Wand) */
            if (!pl->head_world_ok) return 0;                       /* NB2 Mangel 5: ohne Leons Kopf kein Hoehenvergleich */
            if (!kopf) {                                            /* Kopfteil 8 (RE2 +0x1c1 = 8 @0x8010082c-30) */
                kopf = 1;
                re15_enemy_bone_world_pos(e, 8, ka);
                kb[0] = pl->x; kb[2] = pl->z;
                kb[1] = pl->head_world[1];                          /* Plc_neck-Ursprung (skeleton_common.c), je Bild */
            }
            if (!r36_strahl_darueber(c, ka, kb, top)) return 0;
        }
        start += cnt;
    }
    return 1;
}

/* NUR FUER SONDEN (Gegenprobe "vorher" im selben Lauf): 0 = die Leichen-Haken schreiben nichts. Das Spiel ruft das nie. */
static int s_r36_affen_lache_an = 1;
void re15_r36_affen_lache_enable(int an) { s_r36_affen_lache_an = an ? 1 : 0; }

static void r36_padsuche_reset(const re15_actor_t *e);   /* Punkt 2, unten: Pad-Suche des Slots beim INIT loeschen */
void re15_r36_affen_lache_init(re15_actor_t *e)
{
    if (!e) return;
    r36_padsuche_reset(e);
    e->crow_shadow_w = (uint16_t)RE15_R36_AFFEN_SCHATTEN_X;   /* +0xBC = 1600 + 100 @0x801171e8 -> sh @0x8001b030 */
    e->crow_shadow_h = (uint16_t)RE15_R36_AFFEN_SCHATTEN_Z;   /* +0xBE = 1600 + 200 @0x801171f0 -> sh @0x8001b038 */
    e->crow_pool     = 0;                                     /* Farbe des Records: 0x80808080 @0x801171bc (grau) */
}

void re15_r36_affen_lache_tick(re15_actor_t *e)
{
    if (!e || !s_r36_affen_lache_an) return;
    e->crow_pool     = 1;                                                              /* +0xC4/+0xEC = (alt & 0xff000000) | 0x00ffff38 @0x8011bed0-f00 */
    e->crow_shadow_w = (uint16_t)(e->crow_shadow_w + RE15_R36_AFFEN_LACHE_SCHRITT);   /* +0xBC += 8 @0x8011bf08/bf10 */
    e->crow_shadow_h = (uint16_t)(e->crow_shadow_h + RE15_R36_AFFEN_LACHE_SCHRITT);   /* +0xBE += 8 @0x8011bf0c/bf14 */
}

int re15_r36_affen_lache_render(const re15_actor_t *npc, int32_t *nhx, int32_t *nhz, int *pool,
                                uint8_t *sh_r, uint8_t *sh_g, uint8_t *sh_b)
{
    if (!npc || npc->type != 0x27u || npc->crow_shadow_w == 0) return 0;
    *nhx = npc->crow_shadow_w; *nhz = npc->crow_shadow_h;
    if (npc->crow_pool) {
        *pool = 1;
        *sh_r = 0x38; *sh_g = 0xff; *sh_b = 0xff;   /* 0x00ffff38 (B,G,R-Bytes) @0x8011bed0 `ori a2,a2,0xff38` + `lui a2,0xff` @0x8011be6c */
    }
    return 1;
}

/* ----- Punkt 2: Umweg des Gorillas (PORT-WAHL, Nachbesserung 1; Belege: re15_zombieki_r36.h) ----- */
static int     s_r36_padsuche_an = 1;                  /* NUR Sonden: 0 = Original-Verhalten (Gegenprobe) */
static uint8_t s_r36_padsuche_aktiv[RE15_ACTOR_MAX];   /* 0 = aus, 1 = Umweg zu Fuss, 2 = Anlauf auf ein Pad (Sprung = Original) */
static int32_t s_r36_ziel_x[RE15_ACTOR_MAX], s_r36_ziel_z[RE15_ACTOR_MAX];
static int16_t s_r36_pad_idx[RE15_ACTOR_MAX];
static int16_t s_r36_g14_pad1[RE15_ACTOR_MAX];   /* NB6g: Pad + 1, in dem der Sprung aus der Mitte (Grund 14) schon einmal versucht wurde (0 = keins) */
/* NB3 (Abnahme 2, Maengel 1/2): das KLEMMEN-HAENGEN, das der Umweg aufloesen soll, zaehlt das Original selbst — der
 * Gehirn-Schwanz des Gorillas (Zustand 1, nach dem A/B-Wechsel) fuehrt +0x1de: `lhu v0,470(v1)` (+0x1d6 = Rueckgabe der
 * Klemme FUN_8003b0a4) / `beq v0,zero` @0x80117448-50, `lhu v0,478(v1)` / `addiu v0,v0,1` / `sh v0,478(v1)` @0x80117458-68
 * (+1 je Bild an der Klemme), sonst `sh zero,478(v1)` @0x8011746c; INIT `sh zero,478(v0)` @0x8011706c. Voll-Scan des
 * Gorilla-Bereichs 0x80116db8..0x8011c600 (STAGE1.BIN ohne Kopf): KEIN weiterer Zugriff auf 478 — der Zaehler hat in der
 * Beta keinen Verbraucher. Der Umweg ist dieser fehlende Verbraucher (PORT-WAHL): er startet erst, wenn der Gorilla FERN
 * (Leon >= 0x1771 @0x80117bc8) in Sub 3/4 seit R36_FEST_START Bildern weniger als R36_FEST_ANKER (Manhattan) vorankommt und
 * im Startbild an der Klemme steht (+0x1de > 0) — die Mess-Definition der Abnahmen ("blockiert" = >= 90 Bilder im Anker
 * 400) mit HALBER Dauer, damit ein aufgeloestes Haengen nicht mehr als blockiert zaehlt (+0x1de selbst setzt bei jedem
 * kontaktfreien Bild zurueck; gemessen in der Ecke #20/#21: Kontakt flackert 1/0). Bis NB2 startete er bei JEDEM
 * Klemmen-Kontakt und lenkte auch Gorillas um, die an der Wand entlang zu Leon rutschten (gemessen: Sonde Leon
 * (6000,-9000) Gorilla 1 Angriff Bild 196 -> 284). Schwelle gemessen (52 Seeds x 6000 Bilder, Radius/4): 1 / 15 / 30 / 45 ->
 * 24353 / 22328 / 25112 / 20084 blockierte Bilder. */
#define R36_FEST_START 45
#define R36_FEST_ANKER 400
static uint16_t s_r36_1de[RE15_ACTOR_MAX], s_r36_fest[RE15_ACTOR_MAX];
static int32_t  s_r36_anker_x[RE15_ACTOR_MAX], s_r36_anker_z[RE15_ACTOR_MAX];
static uint16_t s_r36_nah[RE15_ACTOR_MAX];                                     /* NB5: Bilder NAH im Anker (Jagd, ausser Bissweite) */
static int32_t  s_r36_nanker_x[RE15_ACTOR_MAX], s_r36_nanker_z[RE15_ACTOR_MAX];
static uint8_t  s_r36_nahumweg[RE15_ACTOR_MAX];                                /* NB5: Nah-Umweg zum freien Bissplatz aktiv */
/* NB5 (Abnahme 4, Mangel 1) — NAH-UMWEG (PORT-WAHL, Nutzer "irgendwie verbessern"). GEMESSEN: an den Stoss-Zielen (exe Stelle 3
 * Ostgassen-Eingang, O3b #21-Ecke, O5 Westkante #2) steht EIN naher Gorilla 900..1875 Bilder an der Klemme 41..160 Einheiten
 * ausserhalb des Bissfensters — B[3] steuert gerade auf playerX (`addiu a0,a0,-13688` @0x80117d4c, `jal 0x8001a8f8` @0x80117d50),
 * A[3] hat NAH nur den Biss (`ori a0,zero,0xbb8` / `ori a1,zero,0x180` / `jal 0x8001a804` / `bgez v0,0x80117b40` @0x80117a60-74),
 * kein anderer Ausgang (@0x80117b40-c74). Ein freier Bissplatz ist zu Fuss erreichbar (Sonde `platz`). Dieselbe Lage hat der
 * KANONISCHE Ankunftskampf (Slot 2 an der V-Spitze d 3041..3099 bis zum Biss +521, Spur-J-Riegel `szene` = GDB jnb1/g_orig):
 * NAH-Anker dort hoechstens 353 Bilder (J-Harness `szene` und exe-Tuerweg 11B0->11C0 je 353, exe-Sprung 346). Messreihe Beginn N
 * (Sonde `kanon`): jedes N < 354 kann den kanonischen Kampf veraendern (1/45/90/.../343/345 veraendert, 344/350/353 zufaellig
 * gleich), ab 354 bleibt er bitgleich. R36_NAH_START = 353 + 1 — kein Original-Wert (das Original hat NAH keinen Ausgang), sondern
 * der kleinste Beginn, der den gepinnten Original-Kampf unberuehrt laesst. Kein freier Bissplatz (z. B. Nordwest-Ecke: alle
 * Bissplaetze in < 3168 Durchmesser, der andere Gorilla greift dort an) -> kein Umweg, der Gorilla steht an (Original). */
#define R36_NAH_START 354
static int      s_r36_nah_k = R36_NAH_START;                                   /* NUR Sonden: Schwelle (Messreihe), 0 = aus */
static int      s_r36_nah_belegt_an = 1;                                       /* NUR Sonden: 0 = Bissplaetze ohne Belegt-Test (Gegenprobe) */
void re15_r36_affen_nah_enable(int an)                                          /* NUR Sonden: 0 aus (Stand NB4), 1 an, 2 an ohne Belegt-Test,
                                                                                 * >= 10: Schwelle an - 10 */
{ s_r36_nah_k = (an >= 10) ? an - 10 : (an ? R36_NAH_START : 0); s_r36_nah_belegt_an = (an != 2);
  memset(s_r36_nahumweg, 0, sizeof s_r36_nahumweg); }
/* NB6 (Abnahme 5) — zwei Erweiterungen des Nah-Umwegs (PORT-WAHL, Nutzer "irgendwie verbessern"), Belege an den Stellen:
 *   Bit 0 (Mangel 1): kein freier Bissplatz zu Fuss -> der naechste freie Bissplatz hinter einem 0x10-Pad (r36_nah_route),
 *                     Weg = A* mit den Pad-Kanten wie der FERN-Umweg, Sprung = der Original-Pad-Sprung A[4] Path A;
 *   Bit 1 (Mangel 2): NACHRUECKEN — stirbt ein Gorilla, der in Bissweite stand, gilt die Wartezeit der Wartenden als
 *                     abgelaufen (re15_r36_affen_fest_takt). */
static int      s_r36_nb6 = 3;                    /* NUR Sonden: 0 = Stand NB5 (Gegenprobe), Bits wie oben */
static uint8_t  s_r36_tot[RE15_ACTOR_MAX];        /* NB6 Bit 1: Tod des Slots schon gesehen (Zustand 3/7) */
static uint8_t  s_r36_nah_grund[RE15_ACTOR_MAX];  /* NUR Sonden: letzte Antwort von r36_nah_route (s. dort) */
void re15_r36_affen_nb6_enable(int bits) { s_r36_nb6 = bits; memset(s_r36_tot, 0, sizeof s_r36_tot); }
static uint8_t  s_r36_vortritt[RE15_ACTOR_MAX];   /* NB4: NAHER Gorilla, der einem FERN haengenden den einzigen Weg versperrt */
#define R36_VORTRITT_START (R36_FEST_START / 2)   /* NB4, s. re15_r36_affen_fest_takt */
static int      s_r36_vortritt_an = 1;            /* NUR Sonden: 0 = Stand NB3 (Gegenprobe Mangel 1 der Abnahme 3) */
static int      s_r36_vortritt_k = R36_VORTRITT_START;
void re15_r36_affen_vortritt_enable(int an)      /* NUR Sonden: 0 aus, 1 an, >= 10: an mit Schwelle an - 10 (Messreihe im Dossier) */
{ s_r36_vortritt_an = an ? 1 : 0; s_r36_vortritt_k = (an >= 10) ? an - 10 : R36_VORTRITT_START; }
void re15_r36_affen_padsuche_enable(int an) { s_r36_padsuche_an = an ? 1 : 0; memset(s_r36_padsuche_aktiv, 0, sizeof s_r36_padsuche_aktiv);
                                              memset(s_r36_vortritt, 0, sizeof s_r36_vortritt); }
int  re15_r36_affen_padsuche_aktiv(int slot) { return (slot >= 0 && slot < RE15_ACTOR_MAX) ? s_r36_padsuche_aktiv[slot] : 0; }
static void r36_padsuche_reset(const re15_actor_t *e)
{
    long slot = e - g_actors;
    if (slot >= 0 && slot < RE15_ACTOR_MAX) { s_r36_padsuche_aktiv[slot] = 0; s_r36_pad_idx[slot] = -1; s_r36_g14_pad1[slot] = 0;
                                              s_r36_1de[slot] = 0; s_r36_fest[slot] = 0; s_r36_vortritt[slot] = 0; s_r36_nah[slot] = 0;
                                              s_r36_nahumweg[slot] = 0; s_r36_tot[slot] = 0; s_r36_nah_grund[slot] = 0; }   /* +0x1de INIT @0x8011706c */
}
/* NB4 (Abnahme 3, Mangel 1) — STAU ZWEIER GORILLAS. GEMESSEN (exe Leon (-24452,11938)/(-22288,11938), Sonde `lage`, und im
 * ORIGINAL per GDB ab derselben Lage, F_nb4/g_o3_stau.txt): Gorilla 1 steht NAH (d 3298) in der Jagd an der #21-Ecke
 * (-23302,14489), Gorilla 2 FERN (d 6122..6228) in der Auswahl dahinter am Nordstreifen (-20579,16118), Koerperkontakt
 * 3165..3209 (2 x 1600 @0x80121356); 900/900 Bilder beide ohne Bewegung, im ORIGINAL wie im Port (e2 +0x1de 179 -> 1079).
 * Der einzige Weg in Leons Ecke ist der Streifen z 15728..16118 ueber #21 — Gorilla 1 versperrt ihn mit seinem Koerper,
 * Gorilla 2 kommt an ihm nicht vorbei (dz <= 1629 < 3200). Der Umweg von Gorilla 2 sah nur SCA-Zellen ("gerader Weg frei"
 * -> Original), die NAHE Original-Jagd von Gorilla 1 (B[3] auf playerX @0x80117d4c) drueckt in die Ecke.
 * PORT-WAHL (Erweiterung des Umwegs, NUR fuer diesen Stau): steht ein Gorilla FERN (Sub 3/4, +0x1d4 >= 0x1771 @0x80117bc8)
 * seit R36_VORTRITT_START Bildern im Anker R36_FEST_ANKER und im KOERPERKONTAKT (Radien @0x80121356 + ein Rasterschritt, wie
 * r36_pad_belegt) mit einem Gorilla, der NAH (< 0x1771) und naeher an Leon in Sub 3/4 steht, bekommt der NAHE den Vortritt: er
 * geht den Umweg ZU FUSS (r36_uw_route Modus 1, kuerzester Weg), bis sein gerader Weg zu Leon frei ist (dann Original), er
 * selbst FERN wird oder Sub 3/4 verlaesst (Angriff). R36_VORTRITT_START = halbe Umweg-Startzeit, Messreihe (Zufallsgang 52
 * Seeds x 6000 Bilder, Anker 400 / 40; Stand NB3 17975 / 221): Schwelle 1 / 8 / 15 / 22 / 30 / 37 / 45 / 60 -> 22057 / 21150 /
 * 17356 / 16352 / 16964 / 18881 / 17806 / 17975 bzw. 1012 / 257 / 228 / 125 / 221 / 327 / 221 / 221; mit 45 stand Gorilla 2 an
 * der exe-Stelle (-24452,11938) noch 103 Bilder FERN (F1245-F1325). Ueber ein Pad (Modus 2) fuehrte der Vortritt in die
 * Original-Blindsprung-Schleife vor #19 bzw. an den Ost-Pad-Anlauf (gemessen, s. decide) — deshalb nur zu Fuss. Im
 * Spur-J-Original (griff: e1/e2 ~3925/~4990, beide NAH, +0x1de bis 497, jnb1/g_griff.txt) gilt die Bedingung nie — zwei NAHE
 * Gorillas bleiben Original. */
void re15_r36_affen_fest_takt(const re15_actor_t *e)
{
    long slot = e - g_actors;
    if (!e || slot < 0 || slot >= RE15_ACTOR_MAX) return;
    s_r36_1de[slot] = e->sca_wall_hit ? (uint16_t)(s_r36_1de[slot] + 1) : 0;          /* +0x1de @0x80117448-6c */
    int sub = e->sub_state_1 & 31;
    if (sub != 3 && sub != 4) s_r36_vortritt[slot] = 0;                               /* NB4: Vortritt endet mit Sub 3/4 (Angriff, Sprung) */
    if (sub != 3 || e->state != 1) s_r36_nahumweg[slot] = 0;                          /* NB5: Nah-Umweg endet mit der Jagd (Biss, Griff, Treffer) */
    if ((sub != 3 && sub != 4)                                                        /* Anker-Messung: +0x1de setzt bei JEDEM Bild ohne
                                                                                       * Kontakt zurueck — gemessen: Gorilla in der Ecke #20/#21
                                                                                       * (-23302,13100) flackert klemme 1/0, +0x1de kommt nie hoch */
        || (uint16_t)e->dog_dist < 0x1771u                                            /* NAH (`sltiu 0x1771` @0x80117bc8 / @0x8011806c): die
                                                                                       * Original-Nahlogik bleibt (s. decide) */
        || labs((long)e->x - s_r36_anker_x[slot]) + labs((long)e->z - s_r36_anker_z[slot]) >= R36_FEST_ANKER) {
        s_r36_anker_x[slot] = e->x; s_r36_anker_z[slot] = e->z; s_r36_fest[slot] = 0;
    } else if (s_r36_fest[slot] < 0xffffu) s_r36_fest[slot]++;
    if ((s_r36_nb6 & 2) && s_r36_nah_k > 0)                                          /* NB6 Bit 1: NACHRUECKEN (Abnahme 5, Mangel 2) */
        for (int s = 0; s < RE15_ACTOR_MAX; s++) {
            const re15_actor_t *q = &g_actors[s];
            if (s == slot || !q->active || q->type != 0x27u || (q->state != 3 && q->state != 7) || s_r36_tot[s]) continue;
            s_r36_tot[s] = 1;                                                         /* Zustand 3 = DEATH 0x8011b6fc / 7 = CORPSE 0x8011be54
                                                                                       * (Zustandstabelle @0x801213c8, Wurzel `lbu v0,4(v1)` /
                                                                                       * `jalr` @0x80116e00-20) — einmal je Tod */
            if ((uint16_t)q->dog_dist >= 0xbb8u) continue;                            /* stand er in Bissweite (+0x1d4 @0x80117314 < a804-Radius
                                                                                       * 0xbb8 @0x80117a60)? Sonst wird kein Bissplatz frei */
            for (int w = 0; w < RE15_ACTOR_MAX; w++) {                                /* jeder, der gerade NAH in der Jagd wartet: Wartezeit
                                                                                       * abgelaufen; Anker = jetzige Lage (sonst setzt ihn das
                                                                                       * naechste Gleiten an der Kante wieder auf 0 — gemessen
                                                                                       * Nachruecken F480/F502/F527/F579, exe nw12 F1470..F1830) */
                const re15_actor_t *a = &g_actors[w];
                if (!a->active || a->type != 0x27u || a->state != 1 || (a->sub_state_1 & 31) != 3
                    || (uint16_t)a->dog_dist >= 0x1771u || (uint16_t)a->dog_dist < 0xbb8u) continue;
                if (s_r36_nah[w] < (uint16_t)s_r36_nah_k) s_r36_nah[w] = (uint16_t)s_r36_nah_k;
                s_r36_nanker_x[w] = a->x; s_r36_nanker_z[w] = a->z;
            }
        }
    if (e->state != 1 || sub != 3|| (uint16_t)e->dog_dist >= 0x1771u || (uint16_t)e->dog_dist < 0xbb8u   /* NB5: NAH-Anker (Jagd, ausser Bissweite) */
        || labs((long)e->x - s_r36_nanker_x[slot]) + labs((long)e->z - s_r36_nanker_z[slot]) >= R36_FEST_ANKER) {
        s_r36_nanker_x[slot] = e->x; s_r36_nanker_z[slot] = e->z; s_r36_nah[slot] = 0;
    } else if (s_r36_nah[slot] < 0xffffu) s_r36_nah[slot]++;
    if (!s_r36_padsuche_an || !s_r36_vortritt_an || (sub != 3 && sub != 4) || (uint16_t)e->dog_dist < 0x1771u   /* NB4: FERN ... */
        || s_r36_fest[slot] < (uint16_t)s_r36_vortritt_k) return;                   /* ... und seit R36_VORTRITT_START Bildern im Anker */
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *q = &g_actors[s];
        int qs = q->sub_state_1 & 31;
        if (s == slot || !q->active || q->type != 0x27u || q->state != 1 || (qs != 3 && qs != 4)) continue;
        if ((uint16_t)q->dog_dist >= 0x1771u || (uint16_t)q->dog_dist >= (uint16_t)e->dog_dist) continue;   /* NAH und vor ihm */
        int64_t kx = (int64_t)q->x - e->x, kz = (int64_t)q->z - e->z;
        int64_t rr = (int64_t)q->hit_radius_min + e->hit_radius_min + e->hit_radius_min / 2;   /* Kontakt + Rasterschritt (Radius/2) */
        if (kx * kx + kz * kz < rr * rr) s_r36_vortritt[s] = 1;
    }
}

/* ---- Raster: Begehbarkeit aus der ORIGINAL-Klemme (FUN_8003b0a4-Zwilling re15_collision_constrain_contact_band
 * mit Radius, Band und Maske des Aktors).
 * NB3 (Abnahme 2, Mangel 2) — FREI-TEST: ein Knoten ist frei, wenn die Klemme einen Aktor, der von NEBENAN (Vorlage =
 * Knoten + 1 Einheit) dort ankommt, NICHT verschiebt. Bis NB2 war die Vorlage der Knoten selbst; dann setzt der
 * Rechteck-Schieber FUN_8003bca8 im Code-0-Zweig ("bewegt, frontal in eine Flaeche") die Lage auf die VORLAGE zurueck
 * (`lh v0,0(a2)` / `sw v0,0(a1)` / `lh v1,4(a2)` / `addu v0,zero,zero` / `sw v1,8(a1)` @0x8003be8c-a0, Vergleich mit der
 * Vorlage @0x8003be38-54) — der Knoten blieb unveraendert und galt als frei, obwohl er IN Zellen liegt. Gemessen ROOM11C0
 * (Schritt 800): 286 von 708 "freien" Knoten waren solche Zurueckschnapp-Lagen, darunter die ganze "Nordwest-Tasche"
 * (in #13/#20/#21, ROOM11C0.RDT 0x014c4/0x01518/0x01524): ein dort abgesetzter Gorilla bewegt sich nie (Sonde `fest`).
 * Schritt = Radius/2 (wie NB1/NB2; NB3 gemessen: Radius/4 fuehrt zwar den 390 breiten Nordstreifen ueber #21, der aber nur
 * in der NAHJAGD gebraucht wuerde, die Original bleibt — Zufallsgang 52 Seeds Radius/4 20084, Radius/2 17975 blockierte
 * Bilder): zwei freie Nachbarknoten liegen je >= Radius von jeder Zelle, eine Zelle passt nicht zwischen sie. Komponenten =
 * 8er-Nachbarschaft ueber begehbare Kanten (s. r36_uw_bauen). Laufzeit: einmal je Raum ~5000 Knoten x (1 + 8) Klemmen-
 * Aufrufe (ROOM11C0); vor einem PSX-Port als Tabelle vorzuberechnen (Nutzer-Regel 2026-09-03, OFFEN im Dossier). */
#define R36_UW_MAX    12288
#define R36_UW_KEINER 0xffffu
#define R36_UW_ZU     0xfffeu
#define R36_KOMP_MAX  1024
typedef struct {
    const void *rdt_id; int band; int32_t r; uint32_t mask; int ok;
    int32_t x0, z0, st; int nx, nz;
    int nkomp;                 /* Zahl der Komponenten (Ids 1..nkomp) */
    uint8_t  frei[R36_UW_MAX];
    uint8_t  kante[R36_UW_MAX];   /* NB3: Bit d = Schritt zum Nachbarn d begehbar (beide Richtungen, s. r36_uw_bauen) */
    uint16_t komp[R36_UW_MAX];
} r36_uw_t;
static const int8_t k_r36_dx[8] = { -1, 0, 1, -1, 1, -1, 0, 1 }, k_r36_dz[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };   /* Gegenrichtung = 7 - d */
static r36_uw_t s_uw;
static uint16_t s_uw_dist[R36_UW_MAX], s_uw_pred[R36_UW_MAX], s_uw_heap[R36_UW_MAX], s_uw_hpos[R36_UW_MAX];

static int r36_frei_bei(int32_t x, int32_t z, int32_t r, int band, uint32_t mask)
{
    int32_t nx = x, nz = z; uint8_t ct = 0; uint16_t at = 0;
    re15_collision_constrain_contact_band(&g_room_rdt, x + 1, z, &nx, &nz, r, band, mask, &ct, &at);   /* Vorlage = Nachbarpunkt (NB3) */
    return nx == x && nz == z;
}
static int32_t r36_kx(int k) { return s_uw.x0 + (k % s_uw.nx) * s_uw.st; }
static int32_t r36_kz(int k) { return s_uw.z0 + (k / s_uw.nx) * s_uw.st; }

static int r36_uw_bauen(int band, int32_t r, uint32_t mask)
{
    if (!g_room_rdt_ok || !g_room_rdt.sca || g_room_rdt.sca_count <= 0 || r <= 0) return 0;
    if (s_uw.ok && s_uw.rdt_id == (const void *)g_room_rdt.sca && s_uw.band == band && s_uw.r == r && s_uw.mask == mask) return 1;
    int32_t xa = 0x7fffffff, xb = -0x7fffffff, za = 0x7fffffff, zb = -0x7fffffff;
    for (int i = 0; i < g_room_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[i];
        if ((int32_t)c->x < xa) xa = c->x;
        if ((int32_t)c->z < za) za = c->z;
        if ((int32_t)c->x + (int32_t)c->width > xb) xb = (int32_t)c->x + (int32_t)c->width;
        if ((int32_t)c->z + (int32_t)c->density > zb) zb = (int32_t)c->z + (int32_t)c->density;
    }
    int32_t st = r / 2;
    int nx = (st > 0) ? (xb - xa) / st + 1 : 0, nz = (st > 0) ? (zb - za) / st + 1 : 0;
    s_uw.ok = 0;
    if (st <= 0 || nx <= 2 || nz <= 2 || nx * nz > R36_UW_MAX) return 0;
    s_uw.rdt_id = (const void *)g_room_rdt.sca; s_uw.band = band; s_uw.r = r; s_uw.mask = mask;
    s_uw.x0 = xa; s_uw.z0 = za; s_uw.st = st; s_uw.nx = nx; s_uw.nz = nz;
    for (int k = 0; k < nx * nz; k++) s_uw.frei[k] = (uint8_t)r36_frei_bei(r36_kx(k), r36_kz(k), r, band, mask);
    /* NB3: KANTEN = ein Schritt Knoten -> Nachbar, den die Original-Klemme nicht aufhebt (Vorlage = Ausgangsknoten), in BEIDE
     * Richtungen; diagonal nur mit beiden Kantenknoten frei. Die Schieber der Schraegzellen haengen an der Bewegungsrichtung
     * (Rueckfall auf die Vorlage @0x8003ced4-ceec, LAB_8003cb9c): gemessen ROOM11C0 Ost-Pad-Suedkante — Knoten (7000,-5900) ist
     * aus jeder Richtung stabil, der Gorilla kam von (7790,-7700) trotzdem 1200 Bilder nicht hin (Umweg-Ziel ueber eine
     * Schraege, die nur die Knoten-Probe fuer frei hielt). */
    memset(s_uw.kante, 0, sizeof s_uw.kante);
    for (int k = 0; k < nx * nz; k++) {
        if (!s_uw.frei[k]) continue;
        int ix = k % nx, iz = k / nx;
        for (int d = 0; d < 8; d++) {
            int jx = ix + k_r36_dx[d], jz = iz + k_r36_dz[d];
            if (jx < 0 || jz < 0 || jx >= nx || jz >= nz) continue;
            int m = jz * nx + jx;
            if (!s_uw.frei[m]) continue;
            if (k_r36_dx[d] && k_r36_dz[d] && (!s_uw.frei[iz * nx + jx] || !s_uw.frei[jz * nx + ix])) continue;
            int32_t bx = r36_kx(m), bz = r36_kz(m), qx = bx, qz = bz; uint8_t ct = 0; uint16_t at = 0;
            re15_collision_constrain_contact_band(&g_room_rdt, r36_kx(k), r36_kz(k), &qx, &qz, r, band, mask, &ct, &at);
            if (qx == bx && qz == bz) s_uw.kante[k] |= (uint8_t)(1u << d);
        }
    }
    for (int k = 0; k < nx * nz; k++)                                  /* nur Kanten, die in beide Richtungen gehen */
        for (int d = 0; d < 8; d++) {
            if (!(s_uw.kante[k] & (1u << d))) continue;
            int m = (k / nx + k_r36_dz[d]) * nx + (k % nx + k_r36_dx[d]);
            if (!(s_uw.kante[m] & (1u << (7 - d)))) s_uw.kante[k] &= (uint8_t)~(1u << d);
        }
    memset(s_uw.komp, 0, sizeof s_uw.komp);
    uint16_t id = 0;
    for (int k = 0; k < nx * nz; k++) {
        if (!s_uw.frei[k] || s_uw.komp[k]) continue;
        id++; int h = 0, t = 0; s_uw_heap[t++] = (uint16_t)k; s_uw.komp[k] = id;   /* s_uw_heap hier als Warteschlange */
        while (h < t) {
            int n = s_uw_heap[h++], ix = n % nx, iz = n / nx;
            for (int d = 0; d < 8; d++) {
                if (!(s_uw.kante[n] & (1u << d))) continue;
                int m = (iz + k_r36_dz[d]) * nx + (ix + k_r36_dx[d]);
                if (s_uw.komp[m]) continue;
                s_uw.komp[m] = id; s_uw_heap[t++] = (uint16_t)m;
            }
        }
    }
    s_uw.nkomp = id;
    s_uw.ok = 1;
    return 1;
}

/* gerade Strecke (ax,az) -> (bx,bz) fuer Radius rs / Maske ms frei (Abtastung je halber Schritt, Endpunkt eingeschlossen,
 * die eigene Lage (ax,az) ausgenommen) */
static int r36_strecke_frei_pt(int32_t ax, int32_t az, int32_t bx, int32_t bz, int32_t rs, uint32_t ms)
{
    int64_t dx = (int64_t)bx - ax, dz = (int64_t)bz - az, len2 = dx * dx + dz * dz, h = s_uw.st / 2;
    int n = 1; while ((int64_t)n * n * h * h < len2) n++;
    for (int i = 1; i <= n; i++)
        if (!r36_frei_bei(ax + (int32_t)(dx * i / n), az + (int32_t)(dz * i / n), rs, s_uw.band, ms)) return 0;
    return 1;
}

/* NB3 (Abnahme 2, Mangel 2): naechster freier Knoten innerhalb `weite`, den der Aktor von (x,z) GERADE erreicht (Strecke
 * frei fuer Radius rs / Maske ms) — bis NB2 der naechste freie Knoten ohne Pruefung: am Taschenrand (-23302,13088) lag er
 * hinter der Wand. rs = 0: ohne Streckenpruefung (Pad-Seiten). -1 = keiner. */
static int r36_uw_knoten(int32_t x, int32_t z, int32_t weite, int32_t rs, uint32_t ms)
{
    int ring = (int)(weite / s_uw.st); if (ring > 8) ring = 8;
    int ix = (int)((x - s_uw.x0 + s_uw.st / 2) / s_uw.st), iz = (int)((z - s_uw.z0 + s_uw.st / 2) / s_uw.st);
    int kand[289]; int64_t kd[289]; int n = 0;
    for (int dz = -ring; dz <= ring; dz++) for (int dx = -ring; dx <= ring; dx++) {
        int jx = ix + dx, jz = iz + dz;
        if (jx < 0 || jz < 0 || jx >= s_uw.nx || jz >= s_uw.nz) continue;
        int m = jz * s_uw.nx + jx;
        if (!s_uw.frei[m]) continue;
        int64_t ex = (int64_t)r36_kx(m) - x, ez = (int64_t)r36_kz(m) - z, d = ex * ex + ez * ez;
        int p = n++;
        while (p > 0 && kd[p - 1] > d) { kand[p] = kand[p - 1]; kd[p] = kd[p - 1]; p--; }
        kand[p] = m; kd[p] = d;
    }
    for (int i = 0; i < n; i++)
        if (rs <= 0 || r36_strecke_frei_pt(x, z, r36_kx(kand[i]), r36_kz(kand[i]), rs, ms)) return kand[i];
    return -1;
}

static int r36_uw_strecke_frei(int a, int b)   /* gerade Strecke Knoten a -> b: jeder Halbschritt in DIESER Richtung bleibt unter
                                                * der Original-Klemme stehen (NB3; bis NB2 nur "Stuetzpunkte frei" — die Schraeg-
                                                * schieber haengen an der Bewegungsrichtung, @0x8003ced4-ceec) */
{
    int32_t ax = r36_kx(a), az = r36_kz(a), bx = r36_kx(b), bz = r36_kz(b);
    int32_t dx = bx - ax, dz = bz - az; int64_t len = (int64_t)dx * dx + (int64_t)dz * dz;
    int n = 1; while ((int64_t)n * n * (s_uw.st / 2) * (s_uw.st / 2) < len) n++;
    int32_t vx = ax, vz = az;
    for (int i = 1; i <= n; i++) {
        int32_t px = ax + (int32_t)((int64_t)dx * i / n), pz = az + (int32_t)((int64_t)dz * i / n), qx = px, qz = pz;
        uint8_t ct = 0; uint16_t at = 0;
        re15_collision_constrain_contact_band(&g_room_rdt, vx, vz, &qx, &qz, s_uw.r, s_uw.band, s_uw.mask, &ct, &at);
        if (qx != px || qz != pz) return 0;
        vx = px; vz = pz;
    }
    return 1;
}

/* Pad-Kanten zwischen Komponenten: SCA-Zelle u1 == 0x10 auf dem Band (die Zellen, die FUN_8003b93c(attr 0x10) in A[4]
 * @0x80117ee0-f00 abfragt); von einer Seite s (freier Knoten) UEBER das Pad auf die GEGENUEBERLIEGENDE Seite. Der Sprung
 * des Originals geht von der Kontaktseite ueber das Pad (coll_contact_dir @0x8003ba8c: westlich 0x000, suedlich 0xc00,
 * noerdlich 0x400, oestlich 0x800). NB3: Kosten = Weglaenge Start -> Landung (Achtel-Metrik wie das Raster). */
typedef struct { uint16_t ka, kb; int16_t pad; uint16_t start, lande, kosten; } r36_kante_t;
static int r36_okt(int a, int b)   /* Achtel-Abstand in Rasterschritten, gerade 2 / diagonal 3 */
{
    int dx = abs(a % s_uw.nx - b % s_uw.nx), dz = abs(a / s_uw.nx - b / s_uw.nx);
    return (dx > dz) ? 2 * dx + dz : 2 * dz + dx;
}
static int r36_uw_kanten(r36_kante_t *kt, int max)
{
    int n = 0;
    for (int i = 0; i < g_room_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[i];
        if ((int)(c->floor >> 4) != s_uw.band || c->u1 != 0x10u) continue;
        int32_t mx = (int32_t)c->x + (int32_t)c->width / 2, mz = (int32_t)c->z + (int32_t)c->density / 2;
        int32_t a = s_uw.r + s_uw.st;                 /* knapp ausserhalb der Pad-Kante */
        int32_t sx[4] = { mx, mx, (int32_t)c->x - a, (int32_t)c->x + (int32_t)c->width + a };
        int32_t sz[4] = { (int32_t)c->z - a, (int32_t)c->z + (int32_t)c->density + a, mz, mz };
        int ks[4];
        for (int s = 0; s < 4; s++) ks[s] = r36_uw_knoten(sx[s], sz[s], 3 * s_uw.r / 2, 0, 0u);
        for (int s = 0; s < 4 && n < max; s++) {
            int o = s ^ 1;                             /* Gegenseite: 0<->1 (Sued/Nord), 2<->3 (West/Ost) */
            if (ks[s] < 0 || ks[o] < 0 || s_uw.komp[ks[s]] == s_uw.komp[ks[o]]) continue;
            kt[n].ka = s_uw.komp[ks[s]]; kt[n].kb = s_uw.komp[ks[o]]; kt[n].pad = (int16_t)i;
            kt[n].start = (uint16_t)ks[s]; kt[n].lande = (uint16_t)ks[o]; kt[n].kosten = (uint16_t)r36_okt(ks[s], ks[o]); n++;
        }
    }
    return n;
}

/* Ist Pad p von einem ANDEREN Gorilla (lebt, Umweg aktiv) belegt? NB2: zwei Gorillas auf demselben Anlauf schieben sich
 * gegenseitig fest (gemessen: Szene Leon (-7000,12500), beide auf Pad #36, 1200 Bilder kein Angriff). */
static int r36_pad_belegt(int p, int slot)
{
    const re15_actor_t *e = &g_actors[slot];
    const re15_sca_entry_t *c = &g_room_rdt.sca[p];
    int64_t mx = (int64_t)c->x + c->width / 2, mz = (int64_t)c->z + c->density / 2;
    int64_t ich = (e->x - mx) * (e->x - mx) + (e->z - mz) * (e->z - mz);
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *o = &g_actors[s];
        if (s == slot || !o->active || o->type != 0x27u || o->state != 1 || !s_r36_padsuche_aktiv[s] || s_r36_pad_idx[s] != p) continue;
        int64_t kx = (int64_t)o->x - e->x, kz = (int64_t)o->z - e->z, rr = (int64_t)o->hit_radius_min + e->hit_radius_min + s_uw.st;
        if (kx * kx + kz * kz >= rr * rr) continue;                 /* nur im KOERPERKONTAKT (Radien @0x80121356 + ein Rasterschritt) */
        int64_t er = (o->x - mx) * (o->x - mx) + (o->z - mz) * (o->z - mz);
        if (er < ich) return 1;                                      /* der Naehere behaelt das Pad, der andere weicht aus */
    }
    return 0;
}

/* Komponenten, die von kg aus zu Fuss + ueber die erlaubten 0x10-Pads erreichbar sind */
static uint8_t s_kerr[R36_KOMP_MAX + 1];
static void r36_komp_erreichbar(int kg, const r36_kante_t *kt, int nk, const uint8_t *erlaubt)
{
    memset(s_kerr, 0, sizeof s_kerr);
    if (kg <= 0 || kg > R36_KOMP_MAX) return;
    s_kerr[kg] = 1;
    for (int weiter = 1; weiter; ) {
        weiter = 0;
        for (int i = 0; i < nk; i++)
            if (erlaubt[i] && kt[i].ka <= R36_KOMP_MAX && kt[i].kb <= R36_KOMP_MAX && s_kerr[kt[i].ka] && !s_kerr[kt[i].kb]) { s_kerr[kt[i].kb] = 1; weiter = 1; }
    }
}
static int r36_erreichbar(int k) { return k >= 0 && s_uw.komp[k] <= R36_KOMP_MAX && s_kerr[s_uw.komp[k]]; }

/* NB3 (Abnahme 2, Mangel 1): KUERZESTER Weg g -> t (A*, Achtel-Metrik gerade 2 / diagonal 3, 0x10-Pads als Kanten mit
 * ihrer Weglaenge Start -> Landung). Bis NB2 eine UNGEWICHTETE Breitensuche ueber Komponenten: die erste Pad-Kante in
 * SCA-Reihenfolge gewann (#36 Ost vor #47 West) — gemessen: Gorilla-Start (-9013,-15461), Leon im Nordwest-Gang -> Umweg
 * ueber das Ost-Pad, erster Angriff 850..920 Bilder spaeter als v0.8.23. Rueckgabe 1 = t erreicht (Vorgaenger s_uw_pred). */
static int r36_f(int k, int t) { return (int)s_uw_dist[k] + r36_okt(k, t); }
static void r36_heap_hoch(int p, int t)
{
    uint16_t k = s_uw_heap[p]; int fk = r36_f(k, t);
    while (p > 0) {
        int q = (p - 1) / 2; uint16_t kq = s_uw_heap[q];
        if (r36_f(kq, t) <= fk) break;
        s_uw_heap[p] = kq; s_uw_hpos[kq] = (uint16_t)p; p = q;
    }
    s_uw_heap[p] = k; s_uw_hpos[k] = (uint16_t)p;
}
static void r36_entspannen(int a, int m, int w, int t, int *hn)
{
    if (s_uw_hpos[m] == R36_UW_ZU) return;
    int nd = (int)s_uw_dist[a] + w;
    if (nd >= (int)s_uw_dist[m]) return;
    s_uw_dist[m] = (uint16_t)nd; s_uw_pred[m] = (uint16_t)a;
    if (s_uw_hpos[m] == R36_UW_KEINER) { s_uw_heap[*hn] = (uint16_t)m; s_uw_hpos[m] = (uint16_t)*hn; (*hn)++; }
    r36_heap_hoch(s_uw_hpos[m], t);
}
static int r36_uw_astern(int g, int t, const r36_kante_t *kt, int nk, const uint8_t *erlaubt)
{
    int n = s_uw.nx * s_uw.nz, hn = 0;
    for (int k = 0; k < n; k++) { s_uw_dist[k] = R36_UW_KEINER; s_uw_hpos[k] = R36_UW_KEINER; }
    s_uw_dist[g] = 0; s_uw_pred[g] = R36_UW_KEINER; s_uw_heap[hn] = (uint16_t)g; s_uw_hpos[g] = 0; hn++;
    while (hn > 0) {
        int a = s_uw_heap[0];
        s_uw_hpos[a] = R36_UW_ZU;                                       /* geschlossen */
        hn--;
        if (hn > 0) {                                                   /* letztes Element an die Wurzel, absenken */
            uint16_t k = s_uw_heap[hn]; int p = 0, fk = r36_f(k, t);
            for (;;) {
                int c1 = 2 * p + 1, c2 = c1 + 1, m = p, fm = fk;
                if (c1 < hn && r36_f(s_uw_heap[c1], t) < fm) { m = c1; fm = r36_f(s_uw_heap[c1], t); }
                if (c2 < hn && r36_f(s_uw_heap[c2], t) < fm) { m = c2; }
                if (m == p) break;
                s_uw_heap[p] = s_uw_heap[m]; s_uw_hpos[s_uw_heap[p]] = (uint16_t)p; p = m;
            }
            s_uw_heap[p] = k; s_uw_hpos[k] = (uint16_t)p;
        }
        if (a == t) return 1;
        int ix = a % s_uw.nx, iz = a / s_uw.nx;
        for (int d = 0; d < 8; d++) {                                   /* begehbare Rasterkanten (NB3) */
            if (!(s_uw.kante[a] & (1u << d))) continue;
            r36_entspannen(a, (iz + k_r36_dz[d]) * s_uw.nx + (ix + k_r36_dx[d]), (k_r36_dx[d] && k_r36_dz[d]) ? 3 : 2, t, &hn);
        }
        for (int j = 0; j < nk; j++)                                    /* 0x10-Pad: Start -> Landung */
            if (erlaubt[j] && kt[j].start == a) r36_entspannen(a, kt[j].lande, kt[j].kosten, t, &hn);
    }
    return 0;
}

/* Dem gefundenen Weg g -> l folgen (nach r36_uw_astern == 1; Vorgaenger in s_uw_pred). Rueckgabe 1 = zu Fuss (Zielpunkt =
 * fernster gerade erreichbarer Wegknoten bis 4 Radien voraus, vor dem ersten Sprung), 2 = am Anlaufpunkt des ersten Pads
 * (Ziel = Pad-Mitte), 0 = Sprung ohne Kante. NB6: aus r36_uw_route herausgeloest (unveraendert), damit der Nah-Umweg
 * denselben Weg nimmt. */
static int s_uw_sprung_start = -1;   /* NB6: Startknoten des ersten Pad-Sprungs auf dem zuletzt gefolgten Weg (-1 = keiner) */
/* Gorilla innerhalb des Pad-Rechtecks p, erweitert um r (die Box-Abfrage des Original-Pad-Sprungs: FUN_8003b93c(+0x34, box[6]+100,
 * +0x82, 0x10) @0x80117ee0-f00 findet das Pad genau dann). */
static int r36_am_pad(const re15_actor_t *e, int p, int32_t r)
{
    const re15_sca_entry_t *c = &g_room_rdt.sca[p];
    return (uint32_t)(e->x - ((int32_t)c->x - r)) < (uint32_t)((int32_t)c->width + 2 * r) &&
           (uint32_t)(e->z - ((int32_t)c->z - r)) < (uint32_t)((int32_t)c->density + 2 * r);
}
static uint8_t r36_seiten_code(int32_t px, int32_t pz, const re15_sca_entry_t *c);   /* unten (Kontaktcode wie @0x8003ba8c) */
static int r36_weg_folgen(const re15_actor_t *e, int slot, int g, int l, const r36_kante_t *kt, int nk, int nah)
{
    s_uw_sprung_start = -1;
    int len = 0;                                                                 /* Weg l -> g, dann umdrehen (s_uw_heap als Puffer) */
    for (int k = l; k != (int)R36_UW_KEINER && len < R36_UW_MAX; k = s_uw_pred[k]) s_uw_heap[len++] = (uint16_t)k;
    for (int i = 0; i < len / 2; i++) { uint16_t t = s_uw_heap[i]; s_uw_heap[i] = s_uw_heap[len - 1 - i]; s_uw_heap[len - 1 - i] = t; }
    int sprung = -1;                                                               /* erster Sprung = Wegnachbarn, die keine Rasternachbarn sind */
    for (int i = 0; i + 1 < len && sprung < 0; i++) {
        int a = s_uw_heap[i], b = s_uw_heap[i + 1];
        if (abs(a % s_uw.nx - b % s_uw.nx) > 1 || abs(a / s_uw.nx - b / s_uw.nx) > 1) sprung = i;
    }
    int ende = (sprung >= 0) ? sprung : len - 1;                                    /* zu Fuss bis hierher */
    if (sprung >= 0) {
        int p = -1;
        for (int j = 0; j < nk; j++) if (kt[j].start == s_uw_heap[sprung] && kt[j].lande == s_uw_heap[sprung + 1]) { p = kt[j].pad; break; }
        if (p < 0) return 0;
        s_r36_pad_idx[slot] = (int16_t)p; s_uw_sprung_start = s_uw_heap[sprung];
        int64_t ex = (int64_t)r36_kx(s_uw_heap[sprung]) - e->x, ez = (int64_t)r36_kz(s_uw_heap[sprung]) - e->z;
        const re15_sca_entry_t *c = &g_room_rdt.sca[p];
        int am = ex * ex + ez * ez <= (int64_t)s_uw.r * s_uw.r;                     /* am Anlaufpunkt (NB2: zwei Schritte a 800 = ein
                                                                                     * Radius): gerader Anlauf auf die Pad-Mitte — die
                                                                                     * Kontaktseite zeigt auf Leons Seite */
        if (!am && nah && r36_am_pad(e, p, (int32_t)e->hit_radius_min + 100)) {
            /* NB6b/g (nur Nah-Umweg): oder schon AM PAD im Sinne des Original-Sprungs (Box FUN_8003b93c(+0x34, box[6]+100, +0x82, 0x10)
             * @0x80117ee0-f00), AUSSERHALB und auf der Startseite des Wegs (Kontaktcode FUN_8001bf04 @0x8001bfa4/ac/e8/ec wie @0x8003ba8c
             * gleich dem des Startknotens; sonst spraenge der Original-Sprung mit dem Code der falschen Seite quer ueber das Pad).
             * GEMESSEN (exe Stelle 3, Messschiene RE15_AFFEN_LOG, F_belege/nb6/nb6_stelle3_affenlog_vorher.txt): Slot 2 steht in der
             * Gassenmuendung an der Suedkante von #36 (6418,-6820), der Startknoten der Pad-Kante liegt 1970 entfernt ausserhalb der
             * Gasse (1550 breit < 2 x 1600) -> "Weg zum Anlauf" fuehrte ihn von Leon WEG, +0x1d4 kippte ueber 0x1771 (FERN -> Sub 4,
             * Umweg geloescht), an der Kante zaehlte der NAH-Anker neu: Zyklus ~150 Bilder ohne Sprung (S2 "nie").
             * VERWORFEN (gemessen, Dossier NB6d-f): dieselbe Regel fuer den FERN-Umweg (Zufallsgang Anker 400 17242 -> 20975) und ein
             * Gedaechtnis "einmal Anlauf, immer Anlauf" (-> 21168, Stillstaende in Sub 4 am Pad, Anker 40 0 -> 537). */
            uint8_t sg = r36_seiten_code(e->x, e->z, c), ss = r36_seiten_code(r36_kx(s_uw_heap[sprung]), r36_kz(s_uw_heap[sprung]), c);
            am = (sg == ss) && (sg & 0xf0u) != 0xf0u;
        }
        if (am) {
            /* NB6 (Nah-Umweg): oder schon AM PAD im Sinne des Original-Sprungs (Box + box[6]+100 @0x80117ee0-f00). GEMESSEN
             * (exe Stelle 3, Messschiene RE15_AFFEN_LOG, F_belege/nb6/nb6_stelle3_affenlog_vorher.txt): Slot 2 steht in der Gassen-
             * muendung an der Suedkante von #36 (6418,-6820), der Startknoten der Pad-Kante liegt 1970 entfernt bei (7000,-8700)
             * (die Gasse ist schmaler als 2 x 1600 -> dort kein freier Rasterknoten) -> "Weg zum Anlauf" fuehrte ihn von Leon WEG,
             * +0x1d4 kippte ueber 0x1771 (FERN -> Sub 4, Umweg geloescht), an der Kante zaehlte der NAH-Anker neu: Zyklus von
             * ~150 Bildern ohne Sprung bis zum Laufende (S2 "nie"). */
            s_r36_ziel_x[slot] = (int32_t)c->x + (int32_t)c->width / 2;
            s_r36_ziel_z[slot] = (int32_t)c->z + (int32_t)c->density / 2;
            return 2;
        }
    }
    int wahl = -1, bis = ende;                                                     /* Zielpunkt: der fernste Wegknoten bis 4 Radien voraus */
    while (bis > 1 && (int64_t)bis * s_uw.st > 4 * (int64_t)s_uw.r) bis--;         /* (NB2: 8 Knoten a 800), der gerade erreichbar ist —   */
    for (int i = bis; i >= 1 && wahl < 0; i--)                                     /* vom fernsten her, der erste Treffer gilt            */
        if (r36_uw_strecke_frei(g, s_uw_heap[i])) wahl = s_uw_heap[i];
    if (wahl < 0) wahl = s_uw_heap[(ende >= 1) ? 1 : 0];
    s_r36_ziel_x[slot] = r36_kx(wahl); s_r36_ziel_z[slot] = r36_kz(wahl);
    return 1;
}

/* Zielwahl fuer Gorilla e mit Leon pl. Rueckgabe 0 = kein Umweg (Original), 1 = zu Fuss, 2 = Pad-Anlauf.
 * ZIEL = Leons Knoten, wenn seine Komponente zu Fuss + ueber 0x10-Pads erreichbar ist; sonst der erreichbare Knoten, der
 * Leon am naechsten liegt (NB2). NB3: Weg = der kuerzeste (A*); der erste Sprung auf DIESEM Weg ist der Original-Sprung. */
static int r36_uw_route(const re15_actor_t *e, const re15_actor_t *pl, int slot)
{
    s_r36_pad_idx[slot] = -1;
    if (!r36_uw_bauen((int)e->floor, (int32_t)e->hit_radius_min, 4u)) return 0;   /* Maske 4 = re15_enemy_sca_clamp_band(..., 4u) des 0x27-Zweigs */
    int g = r36_uw_knoten(e->x, e->z, 3 * s_uw.r / 2, s_uw.r, 4u);                 /* NB3: vom Gorilla GERADE erreichbar (eigener Radius) */
    if (g < 0) return 0;
    r36_kante_t kt[64]; uint8_t erlaubt[64]; int nk = r36_uw_kanten(kt, 64);
    int kg = s_uw.komp[g];
    int l = r36_uw_knoten(pl->x, pl->z, 2 * s_uw.r, 450, 1u);                      /* NB3: auf LEONS Seite der Wand: Strecke frei fuer die
                                                                                     * Spieler-Klemme (Radius 450 = PR, Maske 1; re15_collision_constrain) */
    for (int i = 0; i < nk; i++) erlaubt[i] = (uint8_t)!r36_pad_belegt(kt[i].pad, slot);   /* zuerst ohne die Pads der anderen */
    r36_komp_erreichbar(kg, kt, nk, erlaubt);
    if (!r36_erreichbar(l)) {                                                      /* sonst alle Pads */
        for (int i = 0; i < nk; i++) erlaubt[i] = 1;
        r36_komp_erreichbar(kg, kt, nk, erlaubt);
    }
    if (r36_erreichbar(l) && s_uw.komp[l] == kg && r36_uw_strecke_frei(g, l)) return 0;   /* NB3: gerader Weg zu Leon frei: Original
                                                                                     * (B[3]/B[4] auf playerX @0x80117d4c/@0x801181e0). Gemessen:
                                                                                     * auf Leons KNOTEN gelenkt, verfehlten zwei Gorillas 2100 vor
                                                                                     * Leon das Biss-Fenster a804(0xbb8,0x180) @0x80117a60-74 */
    if (!r36_erreichbar(l)) {                                                      /* Leon unerreichbar: naechster erreichbarer Knoten */
        l = -1; int64_t bd = 0;
        for (int k = 0; k < s_uw.nx * s_uw.nz; k++) {
            if (!s_uw.frei[k] || !r36_erreichbar(k)) continue;
            int64_t ex = (int64_t)r36_kx(k) - pl->x, ez = (int64_t)r36_kz(k) - pl->z;
            if (l < 0 || ex * ex + ez * ez < bd) { l = k; bd = ex * ex + ez * ez; }
        }
    }
    if (l < 0 || l == g || !r36_uw_astern(g, l, kt, nk, erlaubt)) return 0;        /* schon am naechsten Punkt: Original */
    return r36_weg_folgen(e, slot, g, l, kt, nk, 0);
}

/* NB5 (Abnahme 4, Mangel 1): NAH-UMWEG zum naechsten FREIEN BISSPLATZ. Ziel = der Rasterknoten der eigenen Komponente
 * (zu Fuss), der Leon am naechsten liegt, in Bissweite (< 0xbb8 = a804-Radius @0x80117a60) und von keinem anderen lebenden
 * Gorilla belegt (Abstand >= Summe der Radien box[6] @0x80121356 = Koerperkontakt). Kein solcher Knoten -> 0 (Original:
 * der Gorilla wartet). Weg = A* zu Fuss (keine Pads, s. Vortritt), Zielpunkt wie r36_uw_route Modus 1. */
static int r36_belegt_koerper(int k, int slot)
{
    const re15_actor_t *e = &g_actors[slot];
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *o = &g_actors[s];
        if (s == slot || !o->active || o->type != 0x27u || o->state != 1) continue;
        int64_t kx = (int64_t)r36_kx(k) - o->x, kz = (int64_t)r36_kz(k) - o->z, rr = (int64_t)o->hit_radius_min + e->hit_radius_min;
        if (kx * kx + kz * kz < rr * rr) return 1;
    }
    return 0;
}
/* NB6 (Abnahme 5, Mangel 1): hat die EIGENE Fuss-Komponente keinen freien Bissplatz, sucht der Nah-Umweg den naechsten
 * freien Bissplatz in einer Komponente, die ueber 0x10-Pads erreichbar ist (dieselben Kanten wie der FERN-Umweg
 * r36_uw_route: r36_uw_kanten / r36_pad_belegt / A*), und folgt dem Weg bis zum Anlauf; der Sprung selbst ist der
 * Original-Pad-Sprung (r36_pad_sprung). GEMESSEN (Abnahme 5, 2.4/2.5): ein naher Gorilla, den nur das 0x10-Pad #36
 * (ROOM11C0.RDT 0x015d8, x 4900..9500, z -6080..-2180) von Leon trennt, stand im Stand NB5 bis ~1530 Bilder ohne Angriff
 * (r36_nah_route sah nur die eigene Fuss-Komponente).
 * s_r36_nah_grund (NUR Sonden): 1 kein Startknoten, 2 kein freier Bissplatz (Anstehen), 3 schon am Bissplatz,
 * 4 kein Weg, 5 Pad vom anderen Gorilla belegt (Anstehen), 10 zu Fuss, 11 ueber ein Pad (Weg zum Anlauf),
 * 12 ueber ein Pad (Anlauf auf die Pad-Mitte), 13 verkeilt in diesem Pad (Sprung mit dem Seitencode, Rueckgabe 3);
 * 14 (NB6b/g) mit der Mitte in diesem Pad, nicht verkeilt — Sprung mit dem Seitencode, genau EINMAL je Pad-Aufenthalt (Rueckgabe 3). */
static int r36_bissplatz(const re15_actor_t *pl, int slot, int kg)   /* naechster freier Bissplatz in Komponente kg (kg = 0: in
                                                                      * jeder ueber s_kerr erreichbaren); -1 = keiner */
{
    int t = -1; int64_t bd = 3000LL * 3000LL;                                       /* Bissweite a804(0xbb8,..) @0x80117a60 */
    for (int k = 0; k < s_uw.nx * s_uw.nz; k++) {
        if (!s_uw.frei[k] || (kg ? s_uw.komp[k] != kg : !r36_erreichbar(k))) continue;
        int64_t ex = (int64_t)r36_kx(k) - pl->x, ez = (int64_t)r36_kz(k) - pl->z, dk = ex * ex + ez * ez;
        if (dk < bd && !(s_r36_nah_belegt_an && r36_belegt_koerper(k, slot))) { bd = dk; t = k; }
    }
    return t;
}
/* Kontaktcode eines Punkts gegen eine Zelle wie FUN_8003b93c ihn nach entity+0x90 schreibt: (Richtung >> 4) + 8 + (u1 & 3)
 * (`sb v0,144(a0)` @0x8003ba8c), Richtung = FUN_8001bf04: links der Zelle 0x000 (@0x8001bfa4), rechts 0x800 (@0x8001bfac),
 * X innerhalb und davor 0xc00 (@0x8001bfe8) / dahinter 0x400 (@0x8001bfec), sonst 0xf00 (@0x8001bf74). */
static uint8_t r36_seiten_code(int32_t px, int32_t pz, const re15_sca_entry_t *c)
{
    int32_t dir;
    if (px < (int32_t)c->x) dir = 0x000;
    else if (px >= (int32_t)c->x + (int32_t)c->width) dir = 0x800;
    else if (pz < (int32_t)c->z) dir = 0xc00;
    else if (pz >= (int32_t)c->z + (int32_t)c->density) dir = 0x400;
    else dir = 0xf00;
    return (uint8_t)((dir >> 4) + 8 + (c->u1 & 3));
}
static uint8_t s_r36_nah_code[RE15_ACTOR_MAX];   /* NB6: Sprungcode des verkeilten Nah-Umwegs (Modus 3) */
static int r36_im_pad10(const re15_actor_t *e);
static int r36_nah_route(const re15_actor_t *e, const re15_actor_t *pl, int slot)
{
    s_r36_pad_idx[slot] = -1; s_r36_nah_grund[slot] = 1;
    if (!r36_uw_bauen((int)e->floor, (int32_t)e->hit_radius_min, 4u)) return 0;
    int g = r36_uw_knoten(e->x, e->z, 3 * s_uw.r / 2, s_uw.r, 4u), verk = 0;
    if (g < 0 && (s_r36_nb6 & 1) && r36_im_pad10(e)) {                              /* NB6: MITTE IM PAD und kein Knoten gerade erreichbar
                                                                                     * (exe-Stelle 8: (6418,-3617) in #36, Sonde lage: die Klemme
                                                                                     * setzt jeden Schritt auf die Vorlage zurueck, 900/900 Bilder
                                                                                     * ohne Bewegung, auch in Richtung Ausgang) = VERKEILT */
        g = r36_uw_knoten(e->x, e->z, 3 * s_uw.r / 2, 0, 0u);
        verk = 1;
    }
    if (g < 0) return 0;
    int kg = s_uw.komp[g];
    int t = r36_bissplatz(pl, slot, kg);
    if (t >= 0) {                                                                   /* NB5: zu Fuss in der eigenen Komponente */
        s_r36_nah_grund[slot] = (t == g) ? 3 : 4;
        if (t == g || !r36_uw_astern(g, t, NULL, 0, NULL)) return 0;
        s_r36_nah_grund[slot] = 10;
        return r36_weg_folgen(e, slot, g, t, NULL, 0, 1);
    }
    s_r36_nah_grund[slot] = 2;
    if (!(s_r36_nb6 & 1) || kg <= 0 || kg > R36_KOMP_MAX) return 0;
    int l = r36_uw_knoten(pl->x, pl->z, 2 * s_uw.r, 450, 1u);                      /* Leons Seite der Wand (wie r36_uw_route: Strecke frei
                                                                                     * fuer die Spieler-Klemme, Radius 450 = PR, Maske 1) */
    if (l < 0 || s_uw.komp[l] == kg) return 0;
    r36_kante_t kt[64]; uint8_t erlaubt[64]; int nk = r36_uw_kanten(kt, 64);       /* NB6: ueber 0x10-Pads */
    for (int i = 0; i < nk; i++) erlaubt[i] = 1;
    r36_komp_erreichbar(kg, kt, nk, erlaubt);
    if (!r36_erreichbar(l)) return 0;
    t = r36_bissplatz(pl, slot, s_uw.komp[l]);                                      /* nur Bissplaetze auf LEONS Seite (ein Knoten < 0xbb8
                                                                                     * hinter einer Wand waere ein Biss durch die Wand) */
    if (t < 0) return 0;                                                           /* Anstehen: kein freier Bissplatz */
    s_r36_nah_grund[slot] = 4;
    if (!r36_uw_astern(g, t, kt, nk, erlaubt)) return 0;
    int m = r36_weg_folgen(e, slot, g, t, kt, nk, 1);
    if (m && s_r36_pad_idx[slot] >= 0 && r36_pad_belegt(s_r36_pad_idx[slot], slot)) {   /* der andere nimmt dasselbe Pad und steht
                                                                                     * naeher an der Mitte (NB2-Regel r36_pad_belegt):
                                                                                     * anstehen statt Umweg ueber ein fernes Pad */
        s_r36_pad_idx[slot] = -1; s_r36_nah_grund[slot] = 5; return 0;
    }
    if (m && s_r36_pad_idx[slot] >= 0 && s_uw_sprung_start >= 0) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[s_r36_pad_idx[slot]];
        int drin = (uint32_t)(e->x - (int32_t)c->x) < (uint32_t)c->width && (uint32_t)(e->z - (int32_t)c->z) < (uint32_t)c->density;
        if (!drin) s_r36_g14_pad1[slot] = 0;                                        /* ausserhalb: Merker des Mitten-Sprungs loeschen */
        if (drin && (verk || s_r36_g14_pad1[slot] != (int16_t)(s_r36_pad_idx[slot] + 1))) {
            /* MIT DER MITTE in DIESEM Pad (der Weg springt ueber genau dieses Pad): verkeilt (Grund 13, kein Knoten gerade erreichbar;
             * zu Fuss kommt er nicht hinaus) oder nicht verkeilt (Grund 14, NB6g: genau EINMAL je Pad-Aufenthalt — gemessen an
             * exe-Stelle 7, Pad #47 bei x -16346, nb6_stelle7_affenlog_nb6c.txt: der Sprung wurde vom Landeplatz auf die Startlage
             * zurueckgeschnappt und wiederholte sich alle ~85 Bilder; beim zweiten Mal laeuft er wie NB6a zu Fuss zum Startknoten).
             * Frei kommt er nur per Sprung (im Flug Band +0x82 = 1 @0x80118ab4-af0: die Band-0-Autozellen klemmen nicht). Die Mitte hat keinen
             * Seitencode (Platzhalter 0xf00 @0x8001bf74) — PORT-WAHL: der Code der Seite, von der der Weg das Pad ueberquert
             * (Startknoten der Pad-Kante, r36_seiten_code wie @0x8003ba8c), also der Sprung, den der Gorilla von dort aus
             * gemacht haette; Ausfuehrung = Original B[7] (Windup-Drehung, Start, Flug, Landung). */
            s_r36_nah_code[slot] = r36_seiten_code(r36_kx(s_uw_sprung_start), r36_kz(s_uw_sprung_start), c);
            int d = (((int)(s_r36_nah_code[slot] & 0xf0u)) << 4) & 0xfff;
            s_r36_ziel_x[slot] = e->x + (int32_t)(((int64_t)re15_cos_q12(d) * (int64_t)e->hit_radius_min) >> 12);   /* Vorwaerts = (cos, -sin) */
            s_r36_ziel_z[slot] = e->z - (int32_t)(((int64_t)re15_sin_q12(d) * (int64_t)e->hit_radius_min) >> 12);   /* (FUN_800152C8) */
            s_r36_nah_grund[slot] = (uint8_t)(verk ? 13 : 14);
            if (!verk) s_r36_g14_pad1[slot] = (int16_t)(s_r36_pad_idx[slot] + 1);   /* NB6g: Grund 14 genau EINMAL je Pad-Aufenthalt */
            return 3;
        }
    }
    s_r36_nah_grund[slot] = (uint8_t)(m == 2 ? 12 : (m == 1 ? 11 : 4));
    return m;
}

/* Der 0x10-Sprung des Originals aus der Jagd (Sub 3): am Pad (FUN_8003b93c(+0x34, box[6]+100, +0x82, 0x10) wie A[4]
 * @0x80117ee0-f00) im Yaw-Fenster @0x80117f18-38 -> +0x9f = +0x90 @0x80117f40, +0x5 = 7 / +0x6 = 0 / +0x7 = 1
 * @0x80117f50-74. NB6: nur_seite = 1 (Nah-Umweg) springt nur mit einer SEITE als Kontaktcode (coll_contact_dir @0x8003ba8c
 * West 0x000 / Nord 0x400 / Ost 0x800 / Sued 0xc00): 0xf00 ist der Platzhalter "Mitte im Pad" ohne Richtung. */
static int r36_pad_sprung_code(re15_actor_t *e, int slot, uint8_t zc)
{
    int win = ((((int)(zc & 0xf0u)) << 4) - (int)e->rot_y + 512) & 0xfff;       /* Yaw-Fenster des Originals @0x80117f18-38 */
    if (win >= 1024) return 0;                                                    /* `slti v0,v0,1024` @0x80117f34 */
    s_r36_padsuche_aktiv[slot] = 0;                                               /* der 0x10-Sprung des Originals (A[4] @0x80117f40-74) */
    e->dog_aux9f = (int8_t)zc;                                                    /* +0x9f = +0x90 @0x80117f40 */
    e->sub_state_1 = 7; e->sub_state_2 = 0; e->sub_state_3 = 1;                  /* +0x5 = 7, +0x6 = 0, +0x7 = 1 @0x80117f50-74 */
    return 1;
}
static int r36_pad_sprung(re15_actor_t *e, int slot, int nur_seite)
{
    uint8_t zc = 0;
    if (!re15_collision_zone_query(&g_room_rdt, e->x, e->z, (int32_t)e->hit_radius_min + 100, (int)e->floor, 0x10u, &zc)) return 0;
    if (nur_seite && (zc & 0xf0u) == 0xf0u) return 0;
    return r36_pad_sprung_code(e, slot, zc);
}

/* Mitte des Gorillas in einem 0x10-Pad-Rechteck (Band des Gorillas)? */
static int r36_im_pad10(const re15_actor_t *e)
{
    if (!g_room_rdt_ok || !g_room_rdt.sca) return 0;
    for (int i = 0; i < g_room_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[i];
        if ((int)(c->floor >> 4) != (int)e->floor || c->u1 != 0x10u) continue;
        if ((uint32_t)(e->x - (int32_t)c->x) < (uint32_t)c->width && (uint32_t)(e->z - (int32_t)c->z) < (uint32_t)c->density) return 1;
    }
    return 0;
}

/* Gehakt aus A[3] (sub 3, nach dem Biss-Test) und vor B[4] (sub 4). Rueckgabe 1 = der 0x10-Sprung des Originals wurde
 * ausgeloest (nur sub 3; in sub 4 fuehrt A[4] Path A @0x80117ecc-8011802c ihn selbst aus). */
int re15_r36_affen_padsuche_decide(re15_actor_t *e, int slot, int sub)
{
    if (!s_r36_padsuche_an || !e || slot < 0 || slot >= RE15_ACTOR_MAX || (sub != 3 && sub != 4)) return 0;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (s_r36_vortritt[slot]) {                                                       /* NB4: Vortritt (s. re15_r36_affen_fest_takt) — auch NAH
                                                                                       * und mit Sicht der Umweg zu Fuss, bis der gerade Weg frei ist */
        uint8_t m = ((uint16_t)e->dog_dist < 0x1771u) ? (uint8_t)r36_uw_route(e, pl, slot) : 0u;
        if (m == 1) { s_r36_padsuche_aktiv[slot] = 1; return 0; }
        s_r36_vortritt[slot] = 0; s_r36_padsuche_aktiv[slot] = 0;                     /* NUR zu Fuss und NUR solange NAH: gemessen (Zufallsgang 52
                                                                                       * Seeds) fuehrte ein Vortritt ueber ein Pad (Modus 2) in die
                                                                                       * Original-Blindsprung-Schleife vor #19 (Seed 0x9075ea6c, 1789
                                                                                       * Bilder Sub 7) bzw. an den Ost-Pad-Anlauf (Seed 0x95e8eee7,
                                                                                       * 2346 Bilder Sub 4) — FERN gilt der Umweg selbst */
    }
    if (sub == 3 && s_r36_nah_k > 0 && (s_r36_nahumweg[slot] || s_r36_nah[slot] >= (uint16_t)s_r36_nah_k               /* NB5: Nah-Umweg */
                                        || ((s_r36_nb6 & 1) && s_r36_nah[slot] >= R36_FEST_START))) {
        uint8_t m = ((uint16_t)e->dog_dist < 0x1771u && (uint16_t)e->dog_dist >= 0xbb8u) ? (uint8_t)r36_nah_route(e, pl, slot) : 0u;
        if (m == 1 && s_r36_nah_grund[slot] == 10 && !s_r36_nahumweg[slot] && s_r36_nah[slot] < (uint16_t)s_r36_nah_k) m = 0;
            /* NB6: ZU FUSS erst ab R36_NAH_START (J-Fenster: der kanonische Kampf hat einen Fuss-Weg, Slot 2 wartet 353 Bilder);
             * ohne Fuss-Weg (nur ueber ein 0x10-Pad) ab R36_FEST_START wie der FERN-Umweg (NB3) — der Gorilla haengt dann
             * genauso wie ein ferner (Pad-Mitte verkeilt, Klemme hebt jeden Schritt auf). */
        if (m == 1) { s_r36_nahumweg[slot] = 1; s_r36_padsuche_aktiv[slot] = 1; return 0; }
        if (m == 2) { s_r36_nahumweg[slot] = 1; s_r36_padsuche_aktiv[slot] = 2; return r36_pad_sprung(e, slot, 1); }   /* NB6: Anlauf + Original-Sprung */
        if (m == 3) { s_r36_nahumweg[slot] = 1; s_r36_padsuche_aktiv[slot] = 2;                                      /* NB6: verkeilt im Pad: drehen +
                                                                                                                        * Original-Sprung mit dem Seitencode */
                      return r36_pad_sprung_code(e, slot, s_r36_nah_code[slot]); }
        if (s_r36_nahumweg[slot]) { s_r36_nahumweg[slot] = 0; s_r36_padsuche_aktiv[slot] = 0; }
    }
    if (sub == 3 && (e->dog_flags & 1)) { s_r36_padsuche_aktiv[slot] = 0; return 0; }   /* Jagd mit Sicht: Original (fern+Sicht -> Sub 4
                                                                                       * @0x80117c48-54) */
    if ((uint16_t)e->dog_dist < 0x1771u && !r36_im_pad10(e)) { s_r36_padsuche_aktiv[slot] = 0; return 0; }
                                                                                      /* NAH (< 6001, `sltiu 0x1771` @0x80117bc8 / @0x8011806c):
                                                                                       * Original — die Spur-J-Riegel griff/szene pinnen die
                                                                                       * Original-Nahjagd bildgenau (GDB): nach der Freigabe warten
                                                                                       * e1/e2 ~3925/~4990 vor Leon an der Klemme, kein Biss bis T495.
                                                                                       * NB3 gemessen: ein Haenge-Beginn auch NAH (Leon > 3000) liess
                                                                                       * e1 T474 beissen -> griff/szene rot; deshalb nur FERN. */
    uint8_t zk = 0;
    int am_pad = re15_collision_zone_query(&g_room_rdt, e->x, e->z, (int32_t)e->hit_radius_min + 100, (int)e->floor, 0x10u, &zk);
    int verkeilt = am_pad && r36_im_pad10(e) && (uint16_t)e->dog_dist >= 0xbb8u;   /* NB3: Pad-Mitte-Drehen nur, wenn Leon ausserhalb der
                                                                                       * Bissweite a804(0xbb8,0x180) @0x80117a60-74 steht —
                                                                                       * gemessen: Gorilla 2 sprang sonst 2053 vor Leon (Nordkante
                                                                                       * #47, Biss-Sperre +0x1dc) in Richtung 0xf00 davon */
    if (!s_r36_padsuche_aktiv[slot] && !verkeilt && (s_r36_fest[slot] < R36_FEST_START || s_r36_1de[slot] == 0)) {
        /* NB3: Beginn erst, wenn der Gorilla FERN haengt (+0x1de @0x80117448-6c, s. oben) — bis NB2 bei jedem Klemmen-Kontakt.
         * Verkeilt MIT DER MITTE in einem 0x10-Pad (Kontaktcode 0xf00 @0x8003ba8c): sofort (NB1). Ausserdem sofort in der JAGD (Sub 3) an der Kante eines
         * 0x10-Pads (dieselbe Abfrage wie A[4] FUN_8003b93c(+0x34, box[6]+100, +0x82, 0x10) @0x80117ee0-f00), wenn der Weg zu
         * Leon UEBER dieses Pad fuehrt: A[3] hat keinen Sprung (@0x80117b40-c74), der Gorilla laeuft in das Pad, verkeilt mit
         * der Mitte (Code 0xf00) und kann nur noch in Richtung 0xf00 springen — gemessen ROOM11C0 West-Pad #47: Landung bei
         * (-13518,-356) vor dem 0x20-Pad #19, dort die Original-Blindsprung-Schleife (+0x7 = 3 @0x80117fe4-80118024,
         * Spur-J-Riegel zonensprung) ohne Ende. */
        if (sub != 3 || !am_pad) return 0;
        uint8_t m = (uint8_t)r36_uw_route(e, pl, slot);
        if (m == 0 || s_r36_pad_idx[slot] < 0) return 0;
        const re15_sca_entry_t *c = &g_room_rdt.sca[s_r36_pad_idx[slot]];
        int32_t r = (int32_t)e->hit_radius_min + 100;
        if (!((uint32_t)(e->x - ((int32_t)c->x - r)) < (uint32_t)((int32_t)c->width + 2 * r) &&
              (uint32_t)(e->z - ((int32_t)c->z - r)) < (uint32_t)((int32_t)c->density + 2 * r))) return 0;   /* Weg ueber ein ANDERES Pad */
        s_r36_padsuche_aktiv[slot] = m;
    }
    if (verkeilt) {
        /* MITTE IM PAD (Engstelle, verkeilt): der Kontaktcode ist der Platzhalter 0xf00 (coll_contact_dir @0x8003ba8c);
         * zu Fuss kommt der Gorilla nicht heraus (Klemme hebt jeden Schritt auf). Ausweg = der Sprung des Originals:
         * in die Code-Richtung (zc & 0xf0) << 4 drehen, dann passt das Yaw-Fenster @0x80117f18-38. */
        int d = (((int)(zk & 0xf0u)) << 4) & 0xfff;
        s_r36_ziel_x[slot] = e->x + (int32_t)(((int64_t)re15_cos_q12(d) * (int64_t)e->hit_radius_min) >> 12);   /* Vorwaerts = (cos, -sin) */
        s_r36_ziel_z[slot] = e->z - (int32_t)(((int64_t)re15_sin_q12(d) * (int64_t)e->hit_radius_min) >> 12);   /* (FUN_800152C8); Laenge egal */
        s_r36_padsuche_aktiv[slot] = 2;
    } else {
        s_r36_padsuche_aktiv[slot] = (uint8_t)r36_uw_route(e, pl, slot);
    }
    if (sub != 3 || s_r36_padsuche_aktiv[slot] != 2) return 0;
    return r36_pad_sprung(e, slot, 0);                                            /* am Pad? (wie A[4] @0x80117ee0-f00) */
}

/* Haken in B[3] / B[4]: Steuerziel = Spieler (Original, playerX @0x80117d4c / @0x801181e0) oder das Umweg-Ziel. */
int re15_r36_affen_padsuche_ziel(const re15_actor_t *e, int slot, int sub, int32_t plx, int32_t plz, int32_t *tx, int32_t *tz)
{
    *tx = plx; *tz = plz;
    if (!s_r36_padsuche_an || !e || slot < 0 || slot >= RE15_ACTOR_MAX || (sub != 3 && sub != 4) || !s_r36_padsuche_aktiv[slot]) return 0;
    *tx = s_r36_ziel_x[slot]; *tz = s_r36_ziel_z[slot];
    return 1;
}

/* ----- Nachbesserung 1, Mangel 2: zonenloser RE2-Zombie -> Ziel = Spielerlage (Belege: re15_zombieki_r36.h) ----- */
#include "re15_enemy_ai.h"   /* re15_nav_zone_count / re15_nav_zone_from_pos */
static int s_r36_nav_an = 1;
void re15_r36_nav_zonenlos_enable(int an) { s_r36_nav_an = an ? 1 : 0; }
void re15_r36_nav_zonenlos(re15_actor_t *e, const re15_actor_t *pl)
{
    if (!s_r36_nav_an || !e || !pl) return;
    if (!re15_ai_re2_for_type(e->type) || !re15_re2z_owns_type(e->type)) return;   /* nur das RE2-Zombie-Hirn */
    if (re15_nav_zone_count() <= 0) return;                                      /* ohne Graph steuert der Navigator schon direkt */
    if (re15_nav_zone_from_pos((int16_t)e->x, (int16_t)e->z) != 0xffu) return;   /* in einer Zone: Original (FUN_8004aab8 != 0xff) */
    e->steer_x = (int16_t)pl->x;                                                 /* +0x1c4 := Ziel.x @0x8004a90c-914 (Direkt-Form) */
    e->steer_z = (int16_t)pl->z;                                                 /* +0x1c6 := Ziel.z @0x8004a918-924 */
}

/* NUR Sonden (NB4): +0x1de-Zwilling, Anker-Zaehler und Vortritt des Slots lesen. */
void re15_r36_affen_fest_lesen(int slot, int *h1de, int *fest, int *vortritt)
{
    *h1de = 0; *fest = 0; *vortritt = 0;
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    *h1de = s_r36_1de[slot]; *fest = s_r36_fest[slot]; *vortritt = s_r36_vortritt[slot];
}

/* NUR Sonden (NB5): NAH-Anker-Zaehler des Slots lesen. */
int re15_r36_affen_nah_lesen(int slot)
{
    return (slot >= 0 && slot < RE15_ACTOR_MAX) ? (int)s_r36_nah[slot] : 0;
}

/* NUR Sonden (NB6): letzte Antwort von r36_nah_route (Codes dort). */
int re15_r36_affen_nah_grund(int slot)
{
    return (slot >= 0 && slot < RE15_ACTOR_MAX) ? (int)s_r36_nah_grund[slot] : 0;
}

/* NUR Sonden (NB6): freie Bissplatz-Knoten des Umweg-Rasters fuer den Gorilla im Slot (alle Komponenten; Bissweite < 0xbb8
 * @0x80117a60, kein anderer lebender Gorilla in Koerperkontakt, r36_belegt_koerper) — "steht nachweislich an" = 0. */
int re15_r36_affen_bissplaetze_frei(int slot)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return -1;
    const re15_actor_t *e = &g_actors[slot], *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!r36_uw_bauen((int)e->floor, (int32_t)e->hit_radius_min, 4u)) return -1;
    int n = 0;
    for (int k = 0; k < s_uw.nx * s_uw.nz; k++) {
        if (!s_uw.frei[k]) continue;
        int64_t ex = (int64_t)r36_kx(k) - pl->x, ez = (int64_t)r36_kz(k) - pl->z;
        if (ex * ex + ez * ez < 3000LL * 3000LL && !r36_belegt_koerper(k, slot)) n++;
    }
    return n;
}

/* NUR Sonden: Umweg-Ziel und Pad des Slots lesen. */
int re15_r36_affen_umweg_lesen(int slot, int32_t *x, int32_t *z, int *pad)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return 0;
    *x = s_r36_ziel_x[slot]; *z = s_r36_ziel_z[slot]; *pad = s_r36_pad_idx[slot];
    return s_r36_padsuche_aktiv[slot];
}

/* NUR Messschiene (NB6): RE15_AFFEN_LOG=<datei> (env, PC) schreibt je Bild fuer jeden Gorilla (Typ 0x27) den Port-Umweg-Zustand,
 * den gy.log nicht zeigt: ua = s_r36_padsuche_aktiv, up = Pad, nu = Nah-Umweg aktiv, nah = NAH-Anker-Zaehler, ng = letzte Antwort von
 * r36_nah_route, h = +0x1de-Zwilling, fest = Anker-Zaehler, vt = Vortritt, dd = +0x1d4. Kein Verhalten (gemessen: gy.log bitgleich). */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>
void re15_r36_affen_log_tick(void)
{
    static int s_init = 0; static FILE *s_lg = NULL; static unsigned s_f = 0;
    if (!s_init) { s_init = 1; const char *p = getenv("RE15_AFFEN_LOG"); if (p && *p) s_lg = fopen(p, "w"); }
    s_f++;
    if (!s_lg) return;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != 0x27u) continue;
        if (!n++) fprintf(s_lg, "F%u PL(%d,%d)", s_f, (int)pl->x, (int)pl->z);
        fprintf(s_lg, " [%d st=%d/%d/%d/%d @(%d,%d) rot=%d dd=%u L%d k%d ua=%d up=%d z(%d,%d) nu=%d nah=%u ng=%d h=%u fest=%u vt=%d 9f=%02x]",
                s, (int)e->state, (int)e->sub_state_1, (int)e->sub_state_2, (int)e->sub_state_3, (int)e->x, (int)e->z, (int)e->rot_y,
                (unsigned)(uint16_t)e->dog_dist, (int)(e->dog_flags & 1), (int)(e->sca_wall_hit != 0), (int)s_r36_padsuche_aktiv[s],
                (int)s_r36_pad_idx[s], (int)s_r36_ziel_x[s], (int)s_r36_ziel_z[s], (int)s_r36_nahumweg[s], (unsigned)s_r36_nah[s],
                (int)s_r36_nah_grund[s], (unsigned)s_r36_1de[s], (unsigned)s_r36_fest[s], (int)s_r36_vortritt[s], (unsigned)(uint8_t)e->dog_aux9f);
    }
    if (n) { fputc('\n', s_lg); fflush(s_lg); }
}
#else
void re15_r36_affen_log_tick(void) { }
#endif
