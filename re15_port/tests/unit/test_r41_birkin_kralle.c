/* test_r41_birkin_kralle.c — Riegel: Birkins Kralle ACT[3] (Typ 0x30, STAGE3 FUN_801174fc, STAGE5 bytegleich +0x814).
 *
 * Nutzerbefund 2026-11-15: im regulaeren Birkin-Kampf (3070/3071) und mit der Schwanz-Gestalt (Labor 50E0/50F1) macht Birkin nach
 * dem Hieb wiederholt eine Rueckzieh-Animation, die er im Original nicht macht. Gemessen (master 0b86e36c6): JEDER fertig
 * gespielte Hieb lief in Clip 0xa, weil der Port das Clip-Ende als "Sonde" nahm (`if (done) e->sub_state_2 = 3`).
 *
 * Original (Version_40% STAGE3.BIN, selbst disassembliert):
 *   Phase 1 @0x8011762c: anim_set @0x80117640, +0x6 += Rueckgabe UNBEDINGT vor dem Tor (@0x8011765c-66c) -> am Clip-Ende
 *     Phase 2 @0x80117888 (HUB, +0x1df = 0x1e @0x801178bc/@0x801179a8). Im Fenster (+0x95 - 0x24) < 8 (@0x801176ec-f0) jedes Bild
 *     Angriffsbox @0x80117710 und danach die Sonde FUN_8001c2dc(Klauenpunkt, 0x190) @0x80117818-834; Flag (@0x8011783c) -> +0x6 = 3
 *     (@0x80117848-858) = Erholung: Clip 0xa @0x801178dc, Se(6) @0x801178fc. Phase 4 @0x80117904: Form 2 (grid & 0x10) endet
 *     bei +0x95 == 0x1e (@0x80117948-970), sonst am Clip-Ende; Phase 5 @0x80117974: +0x1df = 0x3c (@0x801179a4-a8).
 *   Drehung FUN_8001aac4(Spieler, 0x50) nur in Phase 0 @0x80117624, beim Abbruch @0x801176c8 und in Phase 2/5 (@0x801178a8 /
 *     @0x80117994) - Phase 1 beginnt bei 0x8011762c, NACH dem Aufruf aus Phase 0.
 *
 * Faelle (echte EM30-Bank aus CDEMD0.EMS, echtes ROOM3070.RDT in g_room_rdt, nur die Gegner-KI tickt):
 *   frei   Birkin und Spieler mitten im Raum (keine Zelle im Umkreis 1500): kein Clip 0xa, Rueckkehr in den HUB mit +0x1df 0x1e,
 *          und der Clip 3 lief bis zum letzten Bild.
 *   drehen wie frei, der Spieler geht in Phase 1 seitlich weg: die Gier bleibt von Phase-1-Beginn bis zu ihrem Ende stehen.
 *   wand   Birkin schlaegt gegen Zelle #8 (x >= -20100): Sonde im Fenster -> Clip 0xa; Form 2 endet bei +0x95 == 0x1e, +0x1df 0x3c.
 *   wand1  wie wand mit Form 1 (grid 0x00): Clip 0xa bis zum letzten Bild, +0x1df 0x3c.
 * Gegenprobe: die master-exe (0b86e36c6) faellt in frei (Clip 0xa), drehen (Gier dreht), wand (Clip 0xa bis Bild 46).
 *
 * Runde 2 (Pruefbericht r1: im Labor 50E0/50F1 zog die Schwanz-Gestalt weiter bei 8 von 11 Hieben zurueck): der Port zeichnet dort
 * das Ersatzmodell EM30_HANGAR.EMD (laengerer Arm, geduckte Keyframes); der Klauenpunkt lag bis 590 weiter vom Birkin weg. Die
 * Kralle rechnet jetzt mit dem Capcom-EM30 als Logik-Gerippe (enemy_ai_common.c re15_bk_kralle_logik_setzen, PORT-WAHL dort).
 *   hangar       gezeichnete Bank = EM30_HANGAR.EMD, Logik-Gerippe = EM30, Birkin so weit vor Zelle #8, dass nur der Hangar-Arm
 *                sie erreicht: KEIN Clip 0xa, +0x1df 0x1e. Gegenprobe (Stand r1 45dbd9e10, ohne Logik-Gerippe): Clip 0xa.
 *   hangar_ohne  dieselbe Lage ohne Logik-Gerippe: Clip 0xa (die Lage trennt die beiden Arme wirklich).
 *   hangar_wand  Logik-Gerippe, Birkin an Zelle #8 wie `wand`: Clip 0xa bis 0x1e, +0x1df 0x3c. Seit Runde 1 "synchron" loest das
 *                die GEZEICHNETE Klaue aus (ROOM3070 steht nicht in der Tabelle sichtbarer Waende -> alle Zellen Wand).
 *
 * Runde 1 "synchron" (NUTZERENTSCHEID 16.11.2026: Rueckzug nur, wenn die Klaue sichtbar gegen eine Wand schlaegt, synchron dazu;
 * nur Schwanz-Gestalt = Ersatzmodell gezeichnet, re15_birkin_klaue_r41.h). Echte Raeume ROOM50A0 / ROOM50F1:
 *   schwanz_wand       Hangar 50A0, Birkin vor der Ostwand #4 (x >= -2700): in keinem Takt der Phase 1 steckt die gezeichnete
 *                      Klinge in der Wand, ohne dass im SELBEN Takt +0x6 = 3 gesetzt wird; der naechste Takt laeuft Clip 0xa mit
 *                      +0x8f = RE15_BKK_UEBERBLENDUNG - 1. Gegenprobe Merge-Commit e7858c897: die Klinge steckt Takte lang in der
 *                      Wand, bevor die Capcom-Sonde (erst ab +0x95 0x24) anschlaegt.
 *   schwanz_gelaender  Labor 50F1, Birkin vor dem Gelaender #12 (Oberkante 1900 gemessen): die gezeichnete Klinge kommt nicht
 *                      unter die Oberkante -> kein Clip 0xa. Gegenprobe Merge-Commit: die Capcom-Sonde schlaegt bei af 40 an.
 *
 * Runde 2 "synchron" (Pruefbericht r1: 5 Kontakte ausserhalb des Kamerabilds; Gelaender 50E0 #16 galt als Wand bis zur Decke).
 * Die Kralle prueft jetzt mit der Kamera des laufenden Cuts (g_scd.cam_id); jeder Fall setzt den Cut, den das Spiel dort zeigt.
 *   schwanz_gelaender16  50E0, Birkin bei (-13500,-10500), Blick nach Westen auf #16 (Pruefer-Probe r1, Cut 4). Stand r1: Kontakt
 *                        bei af 36 in 2773 Hoehe ueber der Oberkante 1900 -> Clip 0xa. Neu: kein Kontakt ueber der Oberkante;
 *                        der erste Kontakt liegt unter 1900 (die Klinge senkt sich bei af 38 ins Gelaender, 1871 hoch).
 *   schwanz_ausser_bild  50E0 vor der Suedwand #3, Birkin (-10496,-25129), Spieler (-7544,-26375) (Lage des Hiebs F1715, Pruefer
 *                        r1). Laufender Cut 5: die Klinge trifft #3 im Bild -> Rueckzug im Kontaktbild. Dieselbe Lage mit Cut 0
 *                        (Birkin und Wand ausserhalb des Bilds): die Klinge steckt genauso in #3, aber nicht sichtbar -> kein
 *                        Rueckzug. Gegenprobe Stand r1: Rueckzug auch mit Cut 0.
 *   schwanz_verdeckt     Hangar 50A0 wie schwanz_wand, aber laufender Cut 9 (Kamera schraeg hinter Birkin): beim Schlag (af 35)
 *                        liegt die Einschlagstelle an der Ostwand #4 im Bild, aber hinter Birkins eigenem gezeichneten Koerper
 *                        -> kein Rueckzug dort; der erste sichtbare Treffer kommt beim Absenken (af 67), Rueckzug im Kontaktbild.
 *                        Gegenprobe Stand 46ead4341 (ohne Verdeckung): Rueckzug schon bei af 36.
 *   schwanz_spieler      50E0 vor der Suedwand #3, Birkin (-8000,-25200), Leon (-6800,-26500), Cut 5. Ohne Spielerkoerper trifft
 *                        die Klinge #3 sichtbar (af 37) -> Rueckzug; mit dem Trefferzylinder der Figur (EXE @0x80073e94) liegt die
 *                        Einschlagstelle hinter Leon -> kein Rueckzug. Gegenprobe Stand 2d3bcef24 (ohne Spielerkoerper): Rueckzug
 *                        auch mit Zylinder.
 *   schwanz_abdeckung    jede SCA-Zelle (Band 0, solide) von ROOM50E0/50F1/50A0/50A1 steht in der Wand-Tabelle
 *                        (re15_bkk_zelle_art != 0) - keine Zelle gilt mehr ungeprueft als Wand.
 *
 * Runde 3 Kralle (Orchestrator-Auslegung der Nutzerregel: massgeblich ist der PHYSISCHE Wandschlag der gezeichneten Klaue in
 * Weltgeometrie, nicht ob die Kamera ihn zeigt; keine Bild-, Verdeckungs- oder Masken-Rechnung). Jeder Hieb zaehlt mit einer
 * Welt-Pruefung OHNE Kamera (re15_bkk_pruefen auf der Lage des Takts):
 *   durch = Takte, in denen die Klinge in einer Wand steckt und +0x6 nicht 3 wird (Soll 0);
 *   ohne  = Rueckzuege (+0x6 1 -> 3) ohne Weltkontakt (Soll 0);
 *   Verzug = erster 0xa-Takt - Kontakttakt (Soll 1).
 *   schwanz_ausserhalb  (vorher schwanz_ausser_bild) 50E0 vor #3, Cut 5 UND Cut 0: in beiden Faellen Rueckzug im Kontakttakt.
 *                       Gegenprobe Stand debcc485e: mit Cut 0 kein Rueckzug (Klinge steckt in #3).
 *   schwanz_verdeckt    50A0 Cut 9 (Kamera hinter Birkin): Rueckzug beim ersten Weltkontakt (gleiches Bild wie Cut 4).
 *                       Gegenprobe debcc485e: erst bei af 67.
 *   schwanz_spieler     50E0 vor #3 mit Trefferzylinder der Figur: Rueckzug wie ohne Figur. Gegenprobe debcc485e: kein Rueckzug.
 *   schwanz_abbruch     Ansprung nach dem Aufrichten (50E0), vom Tor bei af 21 abgebrochen; die Klinge gleitet im Ausstiegsbild ins
 *                       Gelaender #17 -> Rueckzug im Folgebild (gemessen N_50E0steh F810 vor dem Nachtrag: 148 tief, danach Gang).
 *   folge = Bilder mit Clip 3 + Klinge in Wand (Welt), denen kein Clip-0xa-Bild folgt - in jedem schwanz-Fall 0.
 *   Erholung: erster 0xa-Takt +0x8f = 3 (@0x80117900) mit a3 0x400 (@0x8011791c) = 3072 vor dem Abzug (Runde 1/2: PORT-WAHL 8).
 *   schwanz_treffer     wie schwanz_wand, im Kontakttakt trifft ein Schuss (HURT-Soak): der Schnappschuss +0x1d8 (@0x80116698-a0)
 *                       traegt +0x6 = 3 wie im Original (Sonde @0x80117858 vor dem Schnappschuss); PORT-WAHL: die Erholung beginnt
 *                       schon im Soak-Bild (Original `j 0x8011a268` @0x8011a20c ohne ACT), Verzug 1.
 *                       Gegenprobe Stand vor dem Nachtrag: der Soak stellt +0x6 = 1 her, die Klinge bleibt in der Wand (gemessen
 *                       50F1 Elza F1276/F1277).
 *   schwanz_zucken      Runde 3 r2 (Pruefung r1 rot, 50A1 Elza F1496 -> F1497): wie schwanz_treffer, aber ohne Soak (+0x1dc = 0
 *                       -> Zucken @0x8011a1f4, Clip 7 @0x8011a2e0); der Resolver schreibt +0x6 = 1 / +0x5 = Waffe (@0x80012fd8 /
 *                       @0x80012ff0). PORT-WAHL: ein anstehender Rueckzug (Schnappschuss +0x1d8 = Kralle +0x6 = 3) geht vor dem
 *                       Zucken - Clip 0xa im Trefferbild, kein Zuckbild, Verzug 1, +0x1dc wie beim Zucken (3..6, @0x8011a31c).
 *                       Gegenprobe 3a51d7aae: 39 Zuckbilder, kein Clip 0xa (folge 1), Rueckzug verloren.
 *
 * Ausholen (NUTZERENTSCHEID 17.11.2026 auf "Soll der Rueckzug auch zaehlen, wenn die Klaue schon beim Ausholen die Wand
 * beruehrt?": "Nein"). Ausholen = Clip 3 vor dem ersten Schlagbild af 0x20 (RE15_BKK_SCHLAG_AB, Herleitung im Header: EM30_HANGAR.EMD
 * @Datei 0x1538, Clip 3 Bild 32 = Keyframe 149, erstes Bild nach vorn/unten). durch/folge zaehlen nur noch Schlagbilder (af >= 0x20);
 *   aus_kontakt = Clip-3-Bilder af < 0x20 mit Klinge in Wand, aus_rueck = Rueckzuege, die in einem solchen Bild ausgeloest wurden (Soll 0).
 *   schwanz_ausholen        50A0, Birkin (-10310,-11990), Spieler (-9462,-12838) (Lagesuche ausholen/tools/suche.c): die Klinge
 *                           beruehrt die Wand nur beim Ausholen (af 21/22), der Schlag ist frei -> KEIN Rueckzug, Clip 3 bis zum
 *                           Ende, HUB mit +0x1df 0x1e. Gegenprobe 32b4575e6: Rueckzug bei af 21.
 *   schwanz_ausholen_schlag 50A0, Birkin (-13050,-10600), Spieler (-13898,-9752): Kontakt beim Ausholen (af 21) UND im Schlag (af 36)
 *                           -> der Ausholkontakt zaehlt nicht, der Schlagkontakt loest den Rueckzug im naechsten Bild aus.
 *                           Gegenprobe 32b4575e6: Rueckzug schon bei af 21.
 *   schwanz_abbruch         (umgekehrt) Kontakt im Abbruch-/Ausstiegsbild bei af 21 -> kein Rueckzug, danach Gang.
 *
 * Runde 3, Auftrag B - Kampf-Clipenden (Capcom-EM30, ROOM3070; Bericht birkin_aufrichten offen 1/2): anim_set FUN_8001f314 posiert
 * +0x95 VOR dem Vorschub (`lbu v0,149(t0)` @0x8001f35c, `sw a2,360(t0)` @0x8001f36c, `addiu v0,v0,1` @0x8001f618, Umlauf
 * `sb zero,149(v1)` @0x8001f63c) -> im Wechselbild steht das LETZTE Bild; Gewicht +0x8f (vor dem Abzug, @0x8001f408) * a3
 * (`mult v0,a3` @0x8001f474). Je Fall wird jedes KI-Bild mitgeschrieben:
 *   Ruecksprung = derselbe (nicht kreisende) Clip springt von Bild > 0 auf Bild 0 (Soll 0);
 *   Stillstand  = Bilder mit Gewicht frac * Rate >= 4096 (100 % alte Pose, Rate 0 = Zeichenpfad-Rueckfall 0x200) (Soll 0);
 *   Wechselbild-falsch = ein nicht kreisender Clip zeigt im Wechselbild Bild 0 (Umlauf) statt seines letzten Bilds (Soll 0).
 *   kampf_kralle   Clip 3 bis zum Ende -> Gang/Biss (a3 Kralle 0x100 @0x80117644)
 *   kampf_erholung Wand, Form 1: 3 -> 0xa (a3 0x400 @0x8011791c) -> Clip-Ende -> Gang (a3 0x100 @0x80117288)
 *   kampf_biss     Clip 4 bis zum Ende (a3 0x100 @0x80117b98) -> Gang
 *   kampf_wurf     Clip 0xb bis zum Ende (a3 0x100 @0x80117e54) -> Gang
 *   kampf_rempler  Clip 5 bis zum Ende (a3 0x100 @0x80118a20) -> Gang
 *   kampf_ansprung Auftritt Sub 9 (0x10 -> 0xd) -> Ansprung Clip 3 mit a3 0x100 (@0x80117640/@0x80117644) statt Rueckfall 0x200
 *                  (vorher 9 Bilder Stillstand).
 * Gegenprobe Merge-Stand 23db6c369 (debcc485e + birkin_aufrichten f7057a708): alle kampf_* rot (Ruecksprung, Stillstand).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_damage.h"
#include "re15_collision.h"
#include "re15_birkin_klaue_r41.h"
#include "re15_scd.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int16_t re15_atan2_q12(int32_t, int32_t);

static int fails = 0;
#ifndef BK_WAND_X
#define BK_WAND_X (-4000)
#endif
#ifndef BK_GEL_X
#define BK_GEL_X (-12200)
#endif
#ifndef BK_GEL_DX
#define BK_GEL_DX 1200
#endif
#ifndef BK_GEL_DZ
#define BK_GEL_DZ 0
#endif
#ifndef BK_GEL_Z
#define BK_GEL_Z (-16000)
#endif
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b && n) *n = (size_t)sz; return b;
}

static uint8_t s_blob[0x80000];
static size_t s_blob_len = 0;
static uint8_t *s_hangar = NULL;
void re15_bk_kralle_logik_setzen(const uint8_t *em30, size_t len);   /* enemy_ai_common.c */
void re15_bk_kontakt_takt_ende(void);                                 /* enemy_ai_common.c, Runde 2 */
int re15_bk_kralle_logik_aktiv(void);
/* gezeichnete 0x30-Bank = EM30_HANGAR.EMD (Ersatzmodell Labor/Hangar); s_blob behaelt das EM30 fuer das Logik-Gerippe */
static int hangar_bank(void)
{
    size_t n = 0;
    s_hangar = slurp(RE15_ASSET_PSX_DIR "/EMD/EM30_HANGAR.EMD", &n);
    re15_enemy_bank_t *eb = re15_enemy_find(0x30);
    if (!eb) eb = re15_enemy_alloc(0x30);
    if (!s_hangar || !eb || re15_emd_parse_container(s_hangar, n, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    return eb->skel.bone_count > 16;
}
static int em30_bank(void)
{
    size_t esz = 0, off = 0, len = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
    if (!ems || re15_ems_get_entry(ems, esz, re15_ems_index_for_type(0x30), &off, &len) != 0 || len > sizeof s_blob) { free(ems); return 0; }
    memcpy(s_blob, ems + off, len); free(ems); s_blob_len = len;
    re15_enemy_bank_t *eb = re15_enemy_find(0x30);
    if (!eb) eb = re15_enemy_alloc(0x30);
    if (!eb || re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    return eb->anim.clip_count > 0x0a;
}

static int raum_3070(void)
{
    size_t n = 0;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE3/ROOM3070.RDT", &n);
    if (!raw || re15_rdt_parse(raw, n, &g_room_rdt) != 0) return 0;   /* raw bleibt fuer die Zeiger des RDT am Leben */
    g_room_rdt_ok = 1;
    g_current_room_id = 0x3070u;
    re15_collision_reset_band();
    re15_collision_set_band(0);
    return 1;
}

static int16_t blick(int32_t ex, int32_t ez, int32_t tx, int32_t tz)
{
    return (int16_t)((((int)re15_atan2_q12(tz - ez, tx - ex) - 0x400)) & 0xfff);
}

static uint8_t *s_raum_raw = NULL;
static int raum_laden(const char *datei, uint16_t id)
{
    size_t n = 0;
    free(s_raum_raw);
    s_raum_raw = slurp(datei, &n);
    if (!s_raum_raw || re15_rdt_parse(s_raum_raw, n, &g_room_rdt) != 0) return 0;
    g_room_rdt_ok = 1;
    g_current_room_id = id;
    re15_collision_reset_band();
    re15_collision_set_band(0);
    return 1;
}

typedef struct {
    int ticks, clip_a, clip_a_max, clip3_max, aus_sub, cd, gier_dreht, sonde_af;
    int kontakt_af, durch, frac_a;   /* Runde 1 synchron: erstes Kontaktbild, Takte Klinge-in-Wand ohne +0x6 = 3, +0x8f im 1. 0xa-Takt */
    int kontakt_zelle; int32_t kontakt_y;  /* Runde 2: Zelle und Hoehe (Welt-y) des tiefsten Pruefpunkts im Kontaktbild */
    int kontakt_t, a_t, ohne, rate_a;      /* Runde 3: Takt des ersten Weltkontakts, erster 0xa-Takt, Rueckzuege ohne Weltkontakt, a3 */
    int soak_ph2;                          /* Runde 3 schwanz_treffer: +0x6 nach dem HURT-Soak-Takt, VOR dem Taktende-Haken */
    int folge;                             /* Runde 3: Bilder mit Clip 3 + Klinge in Wand (Welt), nach denen KEIN Clip 0xa folgt (Soll 0) */
    int zuck, zuck_stun;                   /* Runde 3 r2 schwanz_zucken: Zuckbilder (Clip 7) nach dem Kontakt (Soll 0), +0x1dc danach */
    int aus3_nach;                         /* Clip nach dem ERSTEN Verlassen von Clip 3 (-1 = nie) */
    int aus_kontakt, aus_af, aus_rueck;    /* Ausholen (17.11.): Clip-3-Bilder af < RE15_BKK_SCHLAG_AB mit Klinge in Wand, erstes davon,
                                            * Rueckzuege, die in einem solchen Bild ausgeloest wurden (Soll 0) */
    int ruecksprung, stillstand, wechsel, wechsel_falsch, wechsel_ab[8];   /* Runde 3 Auftrag B (Kampf-Clipenden) */
    char spur[512];
} ergebnis_t;

/* Ein Hieb ab Phase 0. seitlich != 0: der Spieler geht ab Phase 1 je Bild um `seitlich` in Z. */
#ifndef BK_WAND_CUT
#define BK_WAND_CUT 4
#endif
static int s_cut = 0;   /* Kamera des laufenden Cuts (g_scd.cam_id) fuer den Hieb */
#ifndef BK_SP_BX
#define BK_SP_BX (-8000)    /* Lage aus einer Suche ueber 32 Lagen vor der Suedwand #3 (r2/probe_vd), Cut 5 */
#define BK_SP_BZ (-25200)
#define BK_SP_PX (-6800)
#define BK_SP_PZ (-26500)
#endif
static int s_spieler_koerper = 0;   /* 1 = Trefferzylinder der Spielfigur setzen (re15_player_apply_hitbox, @0x80073e94) */
void re15_player_apply_hitbox(re15_actor_t *p);
/* Runde 3: Weltkontakt der Klinge in der Lage dieses Takts, OHNE Kamera (Stand debcc485e: dieselbe Rechnung mit sicht = NULL). */
static int welt_kontakt(const re15_actor_t *e, re15_bkk_befund_t *b)
{
    re15_bkk_lage_t l; re15_bkk_befund_t bb;
    if (!b) b = &bb;
    if (!re15_bk_kralle_logik_aktiv() || !re15_bkk_lage_logik(e, &l)) return 0;
#ifdef RE15_BKK_UEBERBLENDUNG   /* Stand debcc485e / 23db6c369 (Gegenprobe): Schnittstelle mit Kamera */
    return re15_bkk_pruefen(re15_bkk_klinge(), &l, &g_room_rdt, (uint16_t)g_current_room_id, NULL, b);
#else
    return re15_bkk_pruefen(re15_bkk_klinge(), &l, &g_room_rdt, (uint16_t)g_current_room_id, b);
#endif
}
/* Runde 3 Auftrag B: Kampf-Schreiber je KI-Bild (Zeichenstand nach dem Takt: anim_frame = gezeichnetes Bild, anim_frac = Gewicht). */
typedef struct { int ok, mo, af, fr, ra; } bild_t;
static void kampf_bild(ergebnis_t *r, bild_t *alt, const re15_actor_t *e)
{
    const int wp = (int)e->anim_frac * (e->anim_blend_rate ? (int)e->anim_blend_rate : 0x200);
    if (wp >= 4096) r->stillstand++;
    if (alt->ok && alt->mo == e->motion && e->motion != 1 && e->motion != 0 && e->anim_frame == 0 && alt->af > 0) r->ruecksprung++;
    if (alt->ok && alt->mo != e->motion) {
        if (alt->mo == 3 && r->aus3_nach < 0) r->aus3_nach = e->motion;
        if (r->wechsel < 8) r->wechsel_ab[r->wechsel] = alt->mo;
        r->wechsel++;
        /* Wechselbild eines nicht kreisenden Clips: Bild 0 kann dort nur der Umlauf sein (der Port zeichnet einen Clip ab Bild 1) */
        if (alt->mo != 1 && alt->mo != 0 && alt->af == 0) r->wechsel_falsch++;
        const size_t n = strlen(r->spur);
        if (n < sizeof r->spur - 48)
            snprintf(r->spur + n, sizeof r->spur - n, " %#x/af%d->%#x/af%d wp%d", alt->mo, alt->af, e->motion, e->anim_frame, wp);
    }
    alt->ok = 1; alt->mo = e->motion; alt->af = e->anim_frame; alt->fr = e->anim_frac; alt->ra = e->anim_blend_rate;
}
static int s_takte = 400;
static int s_treffer = 0;
static int s_gier = -1;     /* >= 0: Birkins Gier statt Blick zum Spieler (Fall schwanz_abbruch: Lage aus dem Messlauf) */   /* 1 = im Kontakttakt trifft ein Schuss (HURT-Soak, +0x1dc > 0) - Fall schwanz_treffer */
static int s_weiter = 0;    /* 1 = nach dem Verlassen des Unterzustands weiterlaufen (Kampf-Faelle: Wechsel in den Gang) */
static uint8_t s_sub = 3;   /* Unterzustand, mit dem der Lauf beginnt (Kampf-Faelle Runde 3) */
static ergebnis_t hieb(uint8_t grid, int32_t bx, int32_t bz, int32_t px, int32_t pz, int32_t seitlich)
{
    g_scd.cam_id = (uint8_t)s_cut;
    ergebnis_t r; memset(&r, 0, sizeof r); r.sonde_af = -1; r.kontakt_af = -1; r.frac_a = -1; r.kontakt_zelle = -1;
    r.kontakt_t = -1; r.a_t = -1; r.rate_a = -1; r.soak_ph2 = -1; r.aus_af = -1; r.aus3_nach = -1;
    bild_t alt; memset(&alt, 0, sizeof alt);
    int vor_kontakt = 0;
    memset(g_actors, 0, sizeof g_actors);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = px; pl->y = 0; pl->z = pz; pl->hp = 100; pl->hit_react = 1;
    if (s_spieler_koerper) re15_player_apply_hitbox(pl);
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = 0x30; e->state = 0; e->grid_id = grid; e->x = bx; e->y = 0; e->z = bz;
    re15_enemy_apply_hitbox(e, 0x30);
    re15_enemy_spawn_root(1);                                   /* INIT -> BRAIN, Unterzustand 0 (Nibble 0) */
    e->rot_y = s_gier >= 0 ? (int16_t)s_gier : blick(bx, bz, px, pz);
    e->state = 1; e->sub_state_1 = s_sub; e->sub_state_2 = 0; e->sub_state_3 = 0; e->birkin_atk_cd = 0;
    int16_t gier1 = 0; int gier1_ok = 0;
    for (int f = 0; f < s_takte; f++) {
        pl->hit_react = 1;                                     /* Spieler+0x93 gesetzt: kein Treffer-Block (@0x8011772c), nur Box + Sonde */
        const uint8_t ph_vor = e->sub_state_2, ss1_vor = e->sub_state_1;
        re15_enemy_ai_run_all(0);
        if (s_treffer == 1 && r.kontakt_t >= 0 && f == r.kontakt_t + 1) r.soak_ph2 = e->sub_state_2;   /* vor dem Taktende-Haken */
        if (s_treffer == 2 && r.kontakt_t >= 0 && f == r.kontakt_t + 1) { r.soak_ph2 = e->sub_state_2; r.zuck_stun = e->hit_stun; }
        re15_bk_kontakt_takt_ende();                           /* Taktende wie re15_game_step (Runde 2) */
        r.ticks++;
        kampf_bild(&r, &alt, e);
        if (e->sub_state_1 != s_sub && !r.aus_sub) { r.aus_sub = e->sub_state_1; r.cd = e->birkin_atk_cd; if (!s_weiter) break; }
        if (e->motion == 0x0a) {
            if (!r.clip_a) { r.frac_a = e->anim_frac; r.rate_a = e->anim_blend_rate; r.a_t = f; }
            r.clip_a = 1; if (e->anim_frame > r.clip_a_max) r.clip_a_max = e->anim_frame;
        }
        /* Runde 3: JEDES Bild mit Clip 3 und Klinge in einer Wand braucht im Folgebild Clip 0xa (auch Ausstiegsbild am Clipende).
         * Ausholen (NUTZERENTSCHEID 17.11.2026 "Nein"): nur Bilder ab dem ersten Schlagbild af 0x20 (RE15_BKK_SCHLAG_AB, Herleitung
         * re15_birkin_klaue_r41.h) - ein Kontakt beim Ausholen loest keinen Rueckzug aus. */
        const int schlag = e->anim_frame >= RE15_BKK_SCHLAG_AB;
        const int wk_bild = (e->motion == 3) ? welt_kontakt(e, NULL) : 0;
        if (vor_kontakt && e->motion != 0x0a) r.folge++;
        vor_kontakt = wk_bild && schlag;
        if (wk_bild && !schlag) { r.aus_kontakt++; if (r.aus_af < 0) r.aus_af = e->anim_frame; }
        /* Rueckzug ausgeloest (+0x4/+0x6 -> 3/3 in diesem Takt, aus Phase 1, Abbruch- oder Ausstiegsbild) in einem Ausholbild? */
        if (e->motion == 3 && e->sub_state_1 == 3 && e->sub_state_2 == 3 && !(ss1_vor == 3 && ph_vor == 3) && !schlag) r.aus_rueck++;
        /* Runde 3: Weltkontakt (ohne Kamera) im gezeichneten Bild der Phase 1 - zaehlt ab dem ersten Schlagbild */
        re15_bkk_befund_t wb;
        const int wk_roh = (e->motion == 3 && (e->sub_state_2 == 1 || e->sub_state_2 == 3)) ? welt_kontakt(e, &wb) : 0;
        const int wk = wk_roh && schlag;
        if (wk) {
            if (r.kontakt_af < 0) { r.kontakt_af = e->anim_frame; r.kontakt_t = f; r.kontakt_zelle = wb.zelle; r.kontakt_y = wb.punkt[1]; }
            if (e->sub_state_2 == 1) r.durch++;          /* Klinge steckt (Pose dieses Takts), Rueckzug nicht ausgeloest */
        }
        if (s_treffer == 1 && r.kontakt_t == f && e->sub_state_2 == 3) {   /* Schuss nach dem Takt: Schadensweg setzt +0x4 = 2 (HURT) */
            e->state = 2; e->sub_state_3 = 0; if (e->hit_stun <= 0) e->hit_stun = 9;
        }
        /* Runde 3 r2 (Pruefung r1 rot, 50A1 Elza F1496 -> F1497): Schuss im Folgebild OHNE Soak (+0x1dc = 0 -> Zucken @0x8011a1f4).
         * Der Resolver schreibt wie re15_damage.c: +0x7 = 0 (@0x80012fd4), +0x6 = 1 (@0x80012fd8), +0x5 = Waffe (@0x80012ff0; 3 wie
         * gemessen F1497 ss1=3), +0x4 = 2 (@0x80013018). */
        if (s_treffer == 2 && r.kontakt_t == f && e->sub_state_2 == 3) {
            e->state = 2; e->sub_state_3 = 0; e->sub_state_2 = 1; e->sub_state_1 = 3; e->hit_stun = 0; e->hp = (int16_t)(e->hp - 15);
        }
        if (s_treffer == 2 && r.kontakt_t >= 0 && f > r.kontakt_t) {
            if (e->state == 2 && e->motion == 7) r.zuck++;                                  /* Zuckbild statt Rueckzug */
        }
        if (ph_vor == 1 && e->sub_state_2 == 3 && re15_bk_kralle_logik_aktiv() && !wk_roh) r.ohne++;   /* Rueckzug ohne Weltkontakt */
        if (e->motion == 3 && e->anim_frame > r.clip3_max) r.clip3_max = e->anim_frame;
        if (ph_vor == 1 && e->sub_state_2 == 3 && r.sonde_af < 0) r.sonde_af = e->anim_frame;
        if (e->sub_state_2 == 1) {
            if (!gier1_ok) { gier1 = e->rot_y; gier1_ok = 1; }
            else if (e->rot_y != gier1) r.gier_dreht = 1;
            if (seitlich) pl->z += seitlich;
        }
    }
    return r;
}

int main(int argc, char **argv)
{
    const char *fall = argc > 1 ? argv[1] : "alle";
    const int alle = !strcmp(fall, "alle");
    if (!em30_bank()) { fprintf(stderr, "FAIL: EM30-Bank (CDEMD0.EMS) nicht ladbar\n"); return 1; }
    if (!raum_3070()) { fprintf(stderr, "FAIL: ROOM3070.RDT nicht ladbar\n"); return 1; }
    const re15_enemy_bank_t *eb = re15_enemy_find(0x30);
    const int fc3 = eb->anim.clips[3].frame_count, fca = eb->anim.clips[0x0a].frame_count;
    printf("EM30: Clip 3 = %d Bilder, Clip 0xa = %d Bilder\n", fc3, fca);

    if (alle || !strcmp(fall, "frei")) {
        ergebnis_t r = hieb(0x10, -24000, -16500, -22500, -15000, 0);
        printf("frei : ticks=%d clip0xa=%d clip3_max=%d aus_sub=%d cd=0x%02x\n", r.ticks, r.clip_a, r.clip3_max, r.aus_sub, r.cd);
        CHECK(r.clip3_max == fc3 - 1, "frei: Clip 3 muss bis zum letzten Bild laufen (%d), erreichte %d (Abbruch statt Clip-Ende?)", fc3 - 1, r.clip3_max);
        CHECK(!r.clip_a, "frei: ohne Klaue in einer Zelle KEIN Clip 0xa (Erholung nur ueber die Sonde @0x80117834-858)");
        CHECK(r.aus_sub == 1 && r.cd == 0x1e, "frei: Clip-Ende -> Phase 2 -> HUB mit +0x1df 0x1e (@0x8011765c-66c, @0x801178bc), aus_sub=%d cd=0x%02x", r.aus_sub, r.cd);
    }
    if (alle || !strcmp(fall, "drehen")) {
        ergebnis_t r = hieb(0x10, -24000, -16500, -22500, -15000, 6);
        printf("drehen: ticks=%d gier_dreht=%d aus_sub=%d\n", r.ticks, r.gier_dreht, r.aus_sub);
        CHECK(!r.gier_dreht, "drehen: in Phase 1 keine Nachdrehung (FUN_8001aac4 nur Phase 0 @0x80117624, Abbruch @0x801176c8, Phase 2/5)");
    }
    if (alle || !strcmp(fall, "wand")) {
        ergebnis_t r = hieb(0x10, -21300, -15000, -20400, -15000, 0);
        printf("wand : ticks=%d clip0xa=%d bis Bild %d sonde_af=%d aus_sub=%d cd=0x%02x\n", r.ticks, r.clip_a, r.clip_a_max, r.sonde_af, r.aus_sub, r.cd);
        CHECK(r.clip_a, "wand: Klaue in Zelle #8 -> Sonde -> Clip 0xa (@0x80117848-858, @0x801178dc)");
        CHECK(r.sonde_af >= 0x24 && r.sonde_af <= 0x2b, "wand: Sonde nur im Fenster +0x95 0x24..0x2b (@0x801176ec-f0), war %d", r.sonde_af);
        CHECK(r.clip_a_max == 0x1e, "wand: Form 2 endet die Erholung bei +0x95 == 0x1e (@0x8011795c-970), lief bis %d", r.clip_a_max);
        CHECK(r.aus_sub == 1 && r.cd == 0x3c, "wand: Phase 5 -> HUB mit +0x1df 0x3c (@0x801179a4-a8), aus_sub=%d cd=0x%02x", r.aus_sub, r.cd);
    }
    if (alle || !strcmp(fall, "wand1")) {
        ergebnis_t r = hieb(0x00, -21300, -15000, -20400, -15000, 0);
        printf("wand1: ticks=%d clip0xa=%d bis Bild %d aus_sub=%d cd=0x%02x\n", r.ticks, r.clip_a, r.clip_a_max, r.aus_sub, r.cd);
        CHECK(r.clip_a && r.clip_a_max == fca - 1, "wand1: Form 1 spielt Clip 0xa bis zum letzten Bild %d (+0x6 += Rueckgabe @0x8011792c-938), lief bis %d", fca - 1, r.clip_a_max);
        CHECK(r.aus_sub == 1 && r.cd == 0x3c, "wand1: Phase 5 -> HUB mit +0x1df 0x3c (@0x801179a4-a8), aus_sub=%d cd=0x%02x", r.aus_sub, r.cd);
    }
    if (!strcmp(fall, "hangar") || !strcmp(fall, "hangar_ohne") || !strcmp(fall, "hangar_wand")) {
        if (!hangar_bank()) { fprintf(stderr, "FAIL: EM30_HANGAR.EMD nicht ladbar\n"); return 1; }
        const int mit = strcmp(fall, "hangar_ohne") != 0, an_wand = !strcmp(fall, "hangar_wand");
        re15_bk_kralle_logik_setzen(mit ? s_blob : NULL, mit ? s_blob_len : 0);
        /* Blick +x auf Zelle #8 (x >= -20100). Groesster x-Vorlauf der Klaue im Fenster: EM30 2092, Hangar 2478 (af 40,
         * r2/tools/klauenpunkt.py); mit Sondenradius 400 erreicht von -22800 aus nur der Hangar-Arm die Zelle. */
        const int32_t bx = an_wand ? -21300 : -22800;
        s_cut = 5;                                              /* 3070: Kampfschnitt (Messlaeufe RE15_FORCE_CUT=5) */
        ergebnis_t r = hieb(0x10, bx, -15000, -20400, -15000, 0);
        printf("%s: ticks=%d clip0xa=%d bis Bild %d sonde_af=%d clip3_max=%d aus_sub=%d cd=0x%02x\n", fall, r.ticks, r.clip_a,
               r.clip_a_max, r.sonde_af, r.clip3_max, r.aus_sub, r.cd);
        if (!strcmp(fall, "hangar")) {
            CHECK(!r.clip_a, "hangar: mit dem EM30-Logik-Gerippe erreicht die Klaue Zelle #8 nicht -> kein Clip 0xa (gezeichnet: Hangar-Arm)");
            CHECK(r.aus_sub == 1 && r.cd == 0x1e, "hangar: Clip-Ende -> HUB mit +0x1df 0x1e (@0x801178bc), aus_sub=%d cd=0x%02x", r.aus_sub, r.cd);
        } else if (!mit) {
            CHECK(r.clip_a, "hangar_ohne: ohne Logik-Gerippe reicht der Hangar-Arm in Zelle #8 -> Clip 0xa (die Lage trennt die Arme)");
        } else {
            CHECK(r.clip_a && r.clip_a_max == 0x1e, "hangar_wand: die Sonde wirkt mit dem Logik-Gerippe weiter, Clip 0xa bis 0x1e (@0x8011795c-970), war %d/%d", r.clip_a, r.clip_a_max);
            CHECK(r.aus_sub == 1 && r.cd == 0x3c, "hangar_wand: +0x1df 0x3c (@0x801179a4-a8), aus_sub=%d cd=0x%02x", r.aus_sub, r.cd);
        }
        re15_bk_kralle_logik_setzen(NULL, 0);
    }
    if (!strcmp(fall, "schwanz_wand") || !strcmp(fall, "schwanz_gelaender") || !strcmp(fall, "schwanz_treffer") ||
        !strcmp(fall, "schwanz_zucken")) {
        if (!hangar_bank()) { fprintf(stderr, "FAIL: EM30_HANGAR.EMD nicht ladbar\n"); return 1; }
        re15_bk_kralle_logik_setzen(s_blob, s_blob_len);
        const int zucken = !strcmp(fall, "schwanz_zucken");
        const int treffer = !strcmp(fall, "schwanz_treffer") || zucken;
        const int wand = !strcmp(fall, "schwanz_wand") || treffer;
        s_treffer = zucken ? 2 : treffer;
        if (wand ? !raum_laden(RE15_ASSET_PSX_DIR "/STAGE5/ROOM50A0.RDT", 0x50A0u)
                 : !raum_laden(RE15_ASSET_PSX_DIR "/STAGE5/ROOM50F1.RDT", 0x50F1u)) {
            fprintf(stderr, "FAIL: Raum nicht ladbar\n"); return 1;
        }
        /* wand: Ostwand #4 (x -2700..-700, z -28700..-8200), Birkin 1300 davor (x -4000), Blick +x, Spieler 1200 vor ihm.
         *   Gegenprobe Merge-Commit (Lagesuche suche/): Klinge 3 Takte in der Wand (af 35..37), Sonde erst bei af 38, +0x8f 2.
         * gelaender: Laufsteg zwischen #2 (x <= -14245) und #12 (x >= -9725), Birkin bei (-12200, -16000), Blick +x auf #12.
         *   Neu: die gezeichnete Klinge kommt in keinem Bild unter die Oberkante 1900 von #12 -> kein Clip 0xa. Gegenprobe
         *   Merge-Commit: die Capcom-Sonde (EM30-Klauenpunkt, Radius 0x190 @0x80117818) schlaegt bei af 40 an -> Clip 0xa. */
        s_cut = wand ? BK_WAND_CUT : 5;                                   /* 50A0 Gang-Cut 4, 50F1 Kampf-Cut 5 (state.log cam= der Messlaeufe) */
        ergebnis_t r = wand ? hieb(0x10, BK_WAND_X, -15000, BK_WAND_X + 1200, -15000, 0) : hieb(0x10, BK_GEL_X, BK_GEL_Z, BK_GEL_X + BK_GEL_DX, BK_GEL_Z + BK_GEL_DZ, 0);
        printf("%s: ticks=%d clip0xa=%d bis Bild %d kontakt_af=%d durch=%d ohne=%d frac_a=%d rate_a=%#x verzug=%d sonde_af=%d aus_sub=%d cd=0x%02x\n",
               fall, r.ticks, r.clip_a, r.clip_a_max, r.kontakt_af, r.durch, r.ohne, r.frac_a, r.rate_a, r.a_t - r.kontakt_t, r.sonde_af,
               r.aus_sub, r.cd);
        s_treffer = 0;
        if (zucken) {
            /* Runde 3 r2: Schuss im Folgebild ohne Soak. Soll: Erholung (Clip 0xa, +0x6 = 4) im Trefferbild statt Clip 7, kein
             * Zuckbild, Verzug 1, +0x1dc wie beim Zucken neu geladen ((rng&3)+3 = 3..6, @0x8011a31c). Gegenprobe 3a51d7aae: Clip 7
             * (@0x8011a2e0), die Klinge bleibt in der Wand, kein 0xa (folge 1). */
            printf("schwanz_zucken: zuck=%d ph2_nach_treffer=%d stun=%d folge=%d\n", r.zuck, r.soak_ph2, r.zuck_stun, r.folge);
            CHECK(r.zuck == 0 && r.soak_ph2 == 4, "schwanz_zucken: anstehender Rueckzug muss vor dem Zucken gehen (Zuckbilder %d, +0x6 im "
                  "Trefferbild %d statt 4)", r.zuck, r.soak_ph2);
            CHECK(r.kontakt_af >= 0 && r.clip_a && r.a_t == r.kontakt_t + 1 && r.durch == 0 && r.ohne == 0 && r.folge == 0,
                  "schwanz_zucken: Schuss im Folgebild (+0x1dc = 0) - Rueckzug trotzdem im Folgebild (Kontakt Takt %d, 0xa ab Takt %d, "
                  "durch %d, folge %d)", r.kontakt_t, r.a_t, r.durch, r.folge);
            CHECK(r.zuck_stun >= 3 && r.zuck_stun <= 6, "schwanz_zucken: +0x1dc wie beim Zucken neu geladen (3..6, @0x8011a31c), war %d", r.zuck_stun);
            CHECK(r.aus_sub == 1 && r.cd == 0x3c, "schwanz_zucken: Phase 5 -> HUB mit +0x1df 0x3c (@0x801179a4-a8), aus_sub=%d cd=0x%02x", r.aus_sub, r.cd);
        } else if (treffer) {
            /* Runde 3: der Schnappschuss +0x1d8 (@0x80116698-a0) liegt im Original hinter dem +0x6 = 3 (@0x80117858); der HURT-Soak
             * (`+0x4 = *(+0x1d8)` @0x8011a208) setzt die Erholung fort. Der Soak-Takt selbst ruft kein anim_set (Verzug 2). */
            CHECK(r.soak_ph2 == 4, "schwanz_treffer: im Soak-Takt muss die Erholung beginnen (+0x6 = 4; Schnappschuss +0x1d8 traegt das "
                  "+0x6 = 3, PORT-WAHL Soak), war %d", r.soak_ph2);
            CHECK(r.kontakt_af >= 0 && r.clip_a && r.a_t == r.kontakt_t + 1 && r.durch == 0 && r.ohne == 0 && r.folge == 0,
                  "schwanz_treffer: Schuss im Kontakttakt - Rueckzug trotzdem im Folgebild (Kontakt Takt %d, 0xa ab Takt %d, durch %d, folge %d)",
                  r.kontakt_t, r.a_t, r.durch, r.folge);
        } else if (wand) {
            CHECK(r.kontakt_af >= 0, "schwanz_wand: die gezeichnete Klinge muss die Ostwand #4 erreichen (Lage pruefen)");
            CHECK(r.aus_rueck == 0 && r.kontakt_af >= RE15_BKK_SCHLAG_AB, "schwanz_wand: Rueckzug nur aus einem Schlagbild (aus_rueck %d, "
                  "kontakt_af %d)", r.aus_rueck, r.kontakt_af);
            CHECK(r.durch == 0 && r.folge == 0, "schwanz_wand: %d Takt(e) steckt die gezeichnete Klinge in der Wand, ohne dass +0x6 = 3 gesetzt wird "
                  "(ohne Clip 0xa im Folgebild: %d)", r.durch, r.folge);
            CHECK(r.clip_a && r.sonde_af == r.kontakt_af, "schwanz_wand: Rueckzug im Kontaktbild (af %d), ausgeloest bei af %d", r.kontakt_af, r.sonde_af);
            CHECK(r.frac_a == 3 && r.rate_a == 0x400, "schwanz_wand: erster 0xa-Takt +0x8f 3 (@0x80117900) mit a3 0x400 (@0x8011791c) vor dem "
                  "Abzug erwartet, war frac %d Rate %#x", r.frac_a, r.rate_a);
            CHECK(r.a_t == r.kontakt_t + 1 && r.ohne == 0, "schwanz_wand: Verzug 1 Bild (Kontakt Takt %d, 0xa ab Takt %d), Rueckzug ohne Weltkontakt %d",
                  r.kontakt_t, r.a_t, r.ohne);
            CHECK(r.aus_sub == 1 && r.cd == 0x3c, "schwanz_wand: Phase 5 -> HUB mit +0x1df 0x3c (@0x801179a4-a8), aus_sub=%d cd=0x%02x", r.aus_sub, r.cd);
        } else {
            CHECK(r.kontakt_af < 0 && !r.clip_a && r.ohne == 0, "schwanz_gelaender: Klaue ueber dem Gelaender -> kein Kontakt, kein Clip 0xa (kontakt_af %d)", r.kontakt_af);
            CHECK(r.aus_sub == 1 && r.cd == 0x1e, "schwanz_gelaender: Clip-Ende -> HUB mit +0x1df 0x1e (@0x801178bc), aus_sub=%d cd=0x%02x", r.aus_sub, r.cd);
        }
        re15_bk_kralle_logik_setzen(NULL, 0);
    }
    if (!strcmp(fall, "schwanz_gelaender16") || !strcmp(fall, "schwanz_ausserhalb")) {
        if (!hangar_bank()) { fprintf(stderr, "FAIL: EM30_HANGAR.EMD nicht ladbar\n"); return 1; }
        re15_bk_kralle_logik_setzen(s_blob, s_blob_len);
        if (!raum_laden(RE15_ASSET_PSX_DIR "/STAGE5/ROOM50E0.RDT", 0x50E0u)) { fprintf(stderr, "FAIL: ROOM50E0 nicht ladbar\n"); return 1; }
        if (!strcmp(fall, "schwanz_gelaender16")) {
            /* Pruefer-Probe r1 (probe16/t16.c): Birkin (-13500,-10500), Spieler 1200 westlich auf dem Steg, Cut 4 (Messbild e0_16_c4) */
            s_cut = 4;
            ergebnis_t r = hieb(0x10, -13500, -10500, -14700, -10500, 0);
            printf("%s: ticks=%d clip0xa=%d kontakt_af=%d zelle=%d y=%d durch=%d sonde_af=%d aus_sub=%d cd=0x%02x\n", fall, r.ticks,
                   r.clip_a, r.kontakt_af, r.kontakt_zelle, (int)r.kontakt_y, r.durch, r.sonde_af, r.aus_sub, r.cd);
            /* Gelaender-Oberkante -1900 (BKK_GELAENDER_OBEN, birkin_klaue_r41.c): ein Kontakt an #16 nur UNTER der Oberkante */
            CHECK(r.kontakt_af != 36, "schwanz_gelaender16: bei af 36 geht die Klinge in 2773 Hoehe ueber das Gelaender -> kein Kontakt");
            CHECK(r.kontakt_af < 0 || r.kontakt_y >= -1900, "schwanz_gelaender16: Kontakt ueber der Oberkante 1900 (y %d, Zelle %d)", (int)r.kontakt_y, r.kontakt_zelle);
            CHECK(r.durch == 0 && r.ohne == 0 && (!r.clip_a || r.sonde_af == r.kontakt_af), "schwanz_gelaender16: Rueckzug nur im Kontaktbild (kontakt_af %d, sonde_af %d)", r.kontakt_af, r.sonde_af);
        } else {
            /* Hieb F1715 (Pruefer r1, Lauf N_50E0schiess): Birkin (-10496,-25129), Spieler (-7544,-26375); Kontakt an #3 (Suedwand).
             * Runde 3: die Wand ausserhalb des Bilds (Cut 0) ist dieselbe Wand -> Rueckzug in beiden Cuts im selben Kontakttakt. */
            s_cut = 5;
            ergebnis_t r5 = hieb(0x10, -10496, -25129, -7544, -26375, 0);
            s_cut = 0;
            ergebnis_t r0 = hieb(0x10, -10496, -25129, -7544, -26375, 0);
            printf("%s: Cut 5 clip0xa=%d kontakt_af=%d zelle=%d sonde_af=%d durch=%d ohne=%d | Cut 0 clip0xa=%d kontakt_af=%d zelle=%d sonde_af=%d "
                   "durch=%d ohne=%d\n", fall, r5.clip_a, r5.kontakt_af, r5.kontakt_zelle, r5.sonde_af, r5.durch, r5.ohne, r0.clip_a,
                   r0.kontakt_af, r0.kontakt_zelle, r0.sonde_af, r0.durch, r0.ohne);
            CHECK(r5.clip_a && r5.kontakt_zelle == 3 && r5.sonde_af == r5.kontakt_af && r5.durch == 0 && r5.ohne == 0 && r5.a_t == r5.kontakt_t + 1,
                  "schwanz_ausserhalb: Cut 5 - Klinge trifft #3 -> Rueckzug im Kontaktbild (kontakt_af %d Zelle %d, sonde_af %d)",
                  r5.kontakt_af, r5.kontakt_zelle, r5.sonde_af);
            CHECK(r0.clip_a && r0.kontakt_zelle == 3 && r0.sonde_af == r0.kontakt_af && r0.durch == 0 && r0.ohne == 0 && r0.a_t == r0.kontakt_t + 1,
                  "schwanz_ausserhalb: Cut 0 (Wand ausserhalb des Bilds) - derselbe Weltkontakt an #3 -> Rueckzug im Kontaktbild (kontakt_af %d "
                  "Zelle %d, sonde_af %d, durch %d)", r0.kontakt_af, r0.kontakt_zelle, r0.sonde_af, r0.durch);
            CHECK(r0.kontakt_af == r5.kontakt_af, "schwanz_ausserhalb: der Kontakt haengt nicht an der Kamera (Cut 5 af %d, Cut 0 af %d)",
                  r5.kontakt_af, r0.kontakt_af);
        }
        re15_bk_kralle_logik_setzen(NULL, 0);
    }
    if (!strcmp(fall, "schwanz_verdeckt")) {
        if (!hangar_bank()) { fprintf(stderr, "FAIL: EM30_HANGAR.EMD nicht ladbar\n"); return 1; }
        re15_bk_kralle_logik_setzen(s_blob, s_blob_len);
        if (!raum_laden(RE15_ASSET_PSX_DIR "/STAGE5/ROOM50A0.RDT", 0x50A0u)) { fprintf(stderr, "FAIL: ROOM50A0 nicht ladbar\n"); return 1; }
        /* Runde 3: Kamera schraeg hinter Birkin (Cut 9) - der Wandschlag an #4 zaehlt wie mit Cut 4 (Weltkontakt). */
        s_cut = 4;
        ergebnis_t r4 = hieb(0x10, BK_WAND_X, -15000, BK_WAND_X + 1200, -15000, 0);
        s_cut = 9;
        ergebnis_t r = hieb(0x10, BK_WAND_X, -15000, BK_WAND_X + 1200, -15000, 0);
        printf("%s: Cut 9 ticks=%d clip0xa=%d kontakt_af=%d zelle=%d durch=%d ohne=%d sonde_af=%d | Cut 4 kontakt_af=%d sonde_af=%d\n", fall,
               r.ticks, r.clip_a, r.kontakt_af, r.kontakt_zelle, r.durch, r.ohne, r.sonde_af, r4.kontakt_af, r4.sonde_af);
        CHECK(r.kontakt_af >= 0 && r.kontakt_af < 67 && r.kontakt_zelle == 4, "schwanz_verdeckt: erster Weltkontakt an #4 beim Schlag (nicht erst "
              "beim Absenken af 67), war af %d Zelle %d", r.kontakt_af, r.kontakt_zelle);
        CHECK(r.clip_a && r.sonde_af == r.kontakt_af && r.durch == 0 && r.ohne == 0 && r.a_t == r.kontakt_t + 1 && r.folge == 0,
              "schwanz_verdeckt: Rueckzug im Kontaktbild auch hinter Birkins Koerper (kontakt_af %d, sonde_af %d, durch %d)", r.kontakt_af,
              r.sonde_af, r.durch);
        CHECK(r.sonde_af == r4.sonde_af, "schwanz_verdeckt: Rueckzug haengt nicht an der Kamera (Cut 4 af %d, Cut 9 af %d)", r4.sonde_af, r.sonde_af);
        re15_bk_kralle_logik_setzen(NULL, 0);
    }
    if (!strcmp(fall, "schwanz_spieler")) {
        if (!hangar_bank()) { fprintf(stderr, "FAIL: EM30_HANGAR.EMD nicht ladbar\n"); return 1; }
        re15_bk_kralle_logik_setzen(s_blob, s_blob_len);
        if (!raum_laden(RE15_ASSET_PSX_DIR "/STAGE5/ROOM50E0.RDT", 0x50E0u)) { fprintf(stderr, "FAIL: ROOM50E0 nicht ladbar\n"); return 1; }
        s_cut = 5;
        s_spieler_koerper = 0;
        ergebnis_t r0 = hieb(0x10, BK_SP_BX, BK_SP_BZ, BK_SP_PX, BK_SP_PZ, 0);
        s_spieler_koerper = 1;
        ergebnis_t r1 = hieb(0x10, BK_SP_BX, BK_SP_BZ, BK_SP_PX, BK_SP_PZ, 0);
        s_spieler_koerper = 0;
        printf("%s: ohne Koerper clip0xa=%d kontakt_af=%d zelle=%d | mit Koerper clip0xa=%d kontakt_af=%d sonde_af=%d durch=%d\n", fall,
               r0.clip_a, r0.kontakt_af, r0.kontakt_zelle, r1.clip_a, r1.kontakt_af, r1.sonde_af, r1.durch);
        CHECK(r0.clip_a && r0.kontakt_zelle == 3, "schwanz_spieler: ohne Spielerkoerper trifft die Klinge #3 sichtbar (Lage pruefen), kontakt_af %d Zelle %d", r0.kontakt_af, r0.kontakt_zelle);
        /* Runde 3: der Wandschlag hinter Leon ist ein Wandschlag -> Rueckzug wie ohne Spielerkoerper */
        CHECK(r1.clip_a && r1.kontakt_zelle == 3 && r1.sonde_af == r1.kontakt_af && r1.durch == 0 && r1.ohne == 0 && r1.sonde_af == r0.sonde_af,
              "schwanz_spieler: Einschlag hinter Leon -> Rueckzug im Kontaktbild wie ohne Koerper (mit: kontakt_af %d sonde_af %d durch %d, "
              "ohne Koerper sonde_af %d)", r1.kontakt_af, r1.sonde_af, r1.durch, r0.sonde_af);
        re15_bk_kralle_logik_setzen(NULL, 0);
    }
    if (!strcmp(fall, "schwanz_abbruch")) {
        /* Runde 3: Ansprung nach dem Aufrichten, vom Tor @0x80117668 bei af 21 abgebrochen; im Ausstiegsbild (Phase 2 @0x80117888,
         * kein anim_set) gleitet die Klinge mit der weiterlaufenden Ueberblendung ins Gelaender #17. Lage aus dem Messlauf
         * N_50E0steh F806-F810: Birkin (-15000,-16300) Gier 1024 (Auftritt Sub 9), Leon (-13301,-22184); gemessen F810 148 tief.
         * NUTZERENTSCHEID 17.11.2026 "Nein" (Kontakt beim Ausholen zaehlt nicht): das Tor bricht nur bei +0x95 < 0x1e ab
         * (`sltiu v0,v0,0x1e` @0x801176ac), Abbruch- und Ausstiegsbild liegen also immer im Ausholen (af < RE15_BKK_SCHLAG_AB 0x20).
         * Soll jetzt: die Klinge beruehrt das Gelaender beim Ausholen (aus_kontakt > 0), KEIN Rueckzug daraus (aus_rueck == 0, kein
         * Clip 0xa im Folgebild), danach Gang. Gegenprobe 32b4575e6: Rueckzug im Folgebild (aus_rueck 1). */
        if (!hangar_bank()) { fprintf(stderr, "FAIL: EM30_HANGAR.EMD nicht ladbar\n"); return 1; }
        re15_bk_kralle_logik_setzen(s_blob, s_blob_len);
        if (!raum_laden(RE15_ASSET_PSX_DIR "/STAGE5/ROOM50E0.RDT", 0x50E0u)) { fprintf(stderr, "FAIL: ROOM50E0 nicht ladbar\n"); return 1; }
        s_cut = 5; s_sub = 9; s_weiter = 1; s_takte = 260; s_gier = 1024;
        ergebnis_t r = hieb(0x13, -15000, -16300, -13301, -22184, 0);
        s_sub = 3; s_weiter = 0; s_takte = 400; s_gier = -1;
        printf("%s: ticks=%d clip0xa=%d kontakt_af=%d folge=%d ohne=%d Wechsel=%d%s\n", fall, r.ticks, r.clip_a, r.kontakt_af, r.folge, r.ohne,
               r.wechsel, r.spur);
        printf("%s: aus_kontakt=%d aus_af=%d aus_rueck=%d\n", fall, r.aus_kontakt, r.aus_af, r.aus_rueck);
        CHECK(r.aus_kontakt > 0 && r.aus_af < RE15_BKK_SCHLAG_AB, "schwanz_abbruch: die Klinge muss beim Ausholen (Abbruch-/Ausstiegsbild) das "
              "Gelaender #17 beruehren (Lage pruefen), aus_kontakt %d aus_af %d", r.aus_kontakt, r.aus_af);
        CHECK(r.aus_rueck == 0 && r.aus3_nach == 1, "schwanz_abbruch: Kontakt nur beim Ausholen -> kein Rueckzug, nach dem Ausstiegsbild "
              "Gang Clip 1 (aus_rueck %d, Clip nach dem Ansprung %#x)", r.aus_rueck, r.aus3_nach);
        CHECK(r.folge == 0 && r.ohne == 0, "schwanz_abbruch: %d Schlagbild(er) mit Klinge in der Wand ohne Clip 0xa im Folgebild, ohne %d",
              r.folge, r.ohne);
        re15_bk_kralle_logik_setzen(NULL, 0);
    }
    if (!strcmp(fall, "schwanz_ausholen") || !strcmp(fall, "schwanz_ausholen_schlag")) {
        /* NUTZERENTSCHEID 17.11.2026 "Nein": Kontakt beim Ausholen (af < RE15_BKK_SCHLAG_AB) loest keinen Rueckzug aus; der Schlag
         * (ab af 0x20 bis zum Ende des Absenkens) zaehlt weiter mit Rueckzug im naechsten Bild. Lagen aus ausholen/tools/suche.c. */
        if (!hangar_bank()) { fprintf(stderr, "FAIL: EM30_HANGAR.EMD nicht ladbar\n"); return 1; }
        re15_bk_kralle_logik_setzen(s_blob, s_blob_len);
        if (!raum_laden(RE15_ASSET_PSX_DIR "/STAGE5/ROOM50A0.RDT", 0x50A0u)) { fprintf(stderr, "FAIL: ROOM50A0 nicht ladbar\n"); return 1; }
        s_cut = 5;
        const int nur = !strcmp(fall, "schwanz_ausholen");
        ergebnis_t r = nur ? hieb(0x10, -10310, -11990, -9462, -12838, 0) : hieb(0x10, -13050, -10600, -13898, -9752, 0);
        printf("%s: ticks=%d clip0xa=%d aus_kontakt=%d aus_af=%d aus_rueck=%d kontakt_af=%d sonde_af=%d verzug=%d durch=%d folge=%d ohne=%d "
               "clip3_max=%d aus_sub=%d cd=0x%02x\n", fall, r.ticks, r.clip_a, r.aus_kontakt, r.aus_af, r.aus_rueck, r.kontakt_af, r.sonde_af,
               r.a_t - r.kontakt_t, r.durch, r.folge, r.ohne, r.clip3_max, r.aus_sub, r.cd);
        CHECK(r.aus_kontakt > 0 && r.aus_af >= 0 && r.aus_af < RE15_BKK_SCHLAG_AB, "%s: die Klinge muss beim Ausholen die Wand beruehren "
              "(Lage pruefen), aus_kontakt %d aus_af %d", fall, r.aus_kontakt, r.aus_af);
        CHECK(r.aus_rueck == 0, "%s: Kontakt beim Ausholen darf keinen Rueckzug ausloesen (aus_rueck %d, Rueckzug bei af %d)", fall,
              r.aus_rueck, r.sonde_af);
        if (nur) {
            CHECK(r.kontakt_af < 0 && !r.clip_a && r.clip3_max == fc3 - 1, "schwanz_ausholen: Schlag frei -> kein Clip 0xa, Clip 3 bis zum "
                  "letzten Bild %d (kontakt_af %d, clip0xa %d, clip3_max %d)", fc3 - 1, r.kontakt_af, r.clip_a, r.clip3_max);
            CHECK(r.aus_sub == 1 && r.cd == 0x1e, "schwanz_ausholen: Clip-Ende -> HUB mit +0x1df 0x1e (@0x801178bc), aus_sub=%d cd=0x%02x",
                  r.aus_sub, r.cd);
        } else {
            CHECK(r.kontakt_af >= RE15_BKK_SCHLAG_AB && r.clip_a && r.sonde_af == r.kontakt_af && r.a_t == r.kontakt_t + 1,
                  "schwanz_ausholen_schlag: Kontakt im Schlag -> Rueckzug im naechsten Bild (kontakt_af %d, sonde_af %d, Verzug %d)",
                  r.kontakt_af, r.sonde_af, r.a_t - r.kontakt_t);
            CHECK(r.durch == 0 && r.folge == 0 && r.ohne == 0, "schwanz_ausholen_schlag: durch %d folge %d ohne %d", r.durch, r.folge, r.ohne);
            CHECK(r.aus_sub == 1 && r.cd == 0x3c, "schwanz_ausholen_schlag: Phase 5 -> HUB mit +0x1df 0x3c (@0x801179a4-a8), aus_sub=%d "
                  "cd=0x%02x", r.aus_sub, r.cd);
        }
        re15_bk_kralle_logik_setzen(NULL, 0);
    }
    if (!strcmp(fall, "schwanz_abdeckung")) {
        static const struct { const char *datei; uint16_t id; } R[] = {
            { RE15_ASSET_PSX_DIR "/STAGE5/ROOM50E0.RDT", 0x50E0u }, { RE15_ASSET_PSX_DIR "/STAGE5/ROOM50F1.RDT", 0x50F1u },
            { RE15_ASSET_PSX_DIR "/STAGE5/ROOM50A0.RDT", 0x50A0u }, { RE15_ASSET_PSX_DIR "/STAGE5/ROOM50A1.RDT", 0x50A1u } };
        for (size_t k = 0; k < sizeof R / sizeof R[0]; k++) {
            if (!raum_laden(R[k].datei, R[k].id)) { fprintf(stderr, "FAIL: %s nicht ladbar\n", R[k].datei); return 1; }
            int n = 0, wand = 0, gel = 0;
            for (int i = 0; i < g_room_rdt.sca_count; i++) {
                const re15_sca_entry_t *c = &g_room_rdt.sca[i];
                const int typ = c->type & 0x0f;
                if ((typ != 1 && typ != 3) || !(c->u0 & 1) || (c->floor >> 4) != 0 || !c->width || !c->density) continue;
                const int art = re15_bkk_zelle_art(R[k].id, c);
                n++; wand += art == 1; gel += art == 2;
                CHECK(art != 0, "schwanz_abdeckung: ROOM%04X Zelle #%d (x %d z %d w %u d %u) fehlt in der Wand-Tabelle",
                      (unsigned)R[k].id, i, (int)c->x, (int)c->z, (unsigned)c->width, (unsigned)c->density);
            }
            printf("ROOM%04X: %d Zellen, Wand %d, Gelaender %d\n", (unsigned)R[k].id, n, wand, gel);
            CHECK(n > 0, "schwanz_abdeckung: ROOM%04X ohne Zellen", (unsigned)R[k].id);
        }
    }
    if (!strncmp(fall, "kampf_", 6)) {
        /* Runde 3 Auftrag B: Capcom-EM30 (s_bk_logik_ok = 0), ROOM3070, jedes KI-Bild mitgeschrieben (kampf_bild). Spieler 3300 vor
         * Birkin: Biss-/Kralle-Tor halten (dist < 0xdad / Kralle `+0x95 >= 0x1e`), danach waehlt der HUB den Gang (dist >= 0xc80,
         * >= 0x9c4). Spieler+0x93 gesetzt: kein Treffer, kein Griff. */
        re15_bk_kralle_logik_setzen(NULL, 0);
        s_cut = 5; s_weiter = 1; s_takte = 330;
        const int32_t bx = -24000, bz = -16500, px = bx + 3300, pz = bz;
        uint8_t grid = 0x10; int soll_ab = -1; uint16_t soll_rate = 0; int sx = px;
        if (!strcmp(fall, "kampf_kralle"))        { s_sub = 3; soll_ab = 3; soll_rate = 0x100; sx = bx + 1500; }   /* wie "frei" */
        else if (!strcmp(fall, "kampf_erholung")) { s_sub = 3; soll_ab = 0x0a; soll_rate = 0x400; grid = 0x00; }
        else if (!strcmp(fall, "kampf_biss"))     { s_sub = 4; soll_ab = 4; soll_rate = 0x100; }
        else if (!strcmp(fall, "kampf_wurf"))     { s_sub = 5; soll_ab = 0x0b; soll_rate = 0x100; }
        else if (!strcmp(fall, "kampf_rempler"))  { s_sub = 7; soll_ab = 5; soll_rate = 0x100; }
        else if (!strcmp(fall, "kampf_ansprung")) { s_sub = 9; soll_ab = 0x0d; soll_rate = 0x100; grid = 0x13; sx = bx + 1200; }
        /* kampf_ansprung: grid 0x13 = Nibble 3 (Auftritt Sub 9) nach dem Member_set(0x0C,0x13), das den Parkzustand 0x33 aufhebt
         * (0x33 & 0x20 laesst die Wurzel aussteigen @0x80116268-274) */
        else { fprintf(stderr, "FAIL: unbekannter Fall %s\n", fall); return 1; }
        ergebnis_t r;
        if (!strcmp(fall, "kampf_erholung")) r = hieb(grid, -21300, -15000, -20400, -15000, 0);   /* wie "wand1": Klaue in Zelle #8 */
        else if (!strcmp(fall, "kampf_kralle")) r = hieb(grid, bx, bz, sx, bz + 1500, 0);
        else r = hieb(grid, bx, bz, sx, pz, 0);
        int ab_da = 0;
        for (int k = 0; k < r.wechsel && k < 8; k++) if (r.wechsel_ab[k] == soll_ab) ab_da = 1;
        printf("%s: ticks=%d Wechsel=%d%s | Ruecksprung=%d Stillstand=%d Wechselbild-falsch=%d\n", fall, r.ticks, r.wechsel, r.spur,
               r.ruecksprung, r.stillstand, r.wechsel_falsch);
        CHECK(ab_da, "%s: der Wechsel aus Clip %#x kam nicht vor (Lage pruefen)", fall, soll_ab);
        CHECK(r.ruecksprung == 0, "%s: %d Ruecksprung(e) auf Bild 0 im Wechselbild (anim_set posiert +0x95 vor dem Vorschub @0x8001f35c/"
              "@0x8001f618, Umlauf @0x8001f63c)", fall, r.ruecksprung);
        CHECK(r.wechsel_falsch == 0, "%s: %d Wechsel ohne das letzte Bild des fertigen Clips im Wechselbild", fall, r.wechsel_falsch);
        CHECK(r.stillstand == 0, "%s: %d Bild(er) mit 100 %% alter Pose (Gewicht +0x8f * a3 `mult v0,a3` @0x8001f474, a3 je Aufruf; "
              "Rueckfall 0x200 kappt)", fall, r.stillstand);
        (void)soll_rate;
        s_weiter = 0; s_takte = 400; s_sub = 3;
    }
    if (fails) { fprintf(stderr, "test_r41_birkin_kralle %s: %d Fehler\n", fall, fails); return 1; }
    printf("test_r41_birkin_kralle %s: OK\n", fall);
    return 0;
}
