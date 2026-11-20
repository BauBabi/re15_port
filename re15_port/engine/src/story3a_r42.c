/*
 * RE1.5 Rebuilt — Runde 42 "story" (v0.8.31), Stufe STAGE3a: Marvin. Belege, Formen und Konstanten: include/re15_story3a_r42.h.
 *
 * PORT-STORY nach dem freigegebenen Konzept (Abschnitt 5 T-01/T-02, 3.2 L4-L7, Zeitleiste L-05..L-10; F-04, F-05, F-06). Gebaut
 * nur aus Original-Formen: Figurwechsel nach RE2 FUN_80068f9c (Kopf der .h), Szenen als SCD-Bytecode aus Original-Opcodes, aus-
 * gefuehrt von der vorhandenen VM (Form story2_r42.c / story1_r42.c), Ereignis-/Evt_exec-Umleitung wie dort, Tuer-Umwidmung per
 * Aot_reset-Form (re15_aot_retype, LAB_80040738). Jede Zeile hat Text UND Dauer (re15_msg_install_text +
 * re15_msg_install_durations): fehlt die Aufnahme synchro/STAGE3/room30x0/mainNN.wav, laeuft die Zeile mit der Textdauer wie jede
 * RDT-Zeile ohne Ton (VERTRAG 2.1).
 * NUR Leon: Raum exakt 3020/3060/3010 und Figur Leon (re15_char_variant() == 0); Elzas 3021/3061/3011 bleiben unberuehrt.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "re15_story3a_r42.h"
#include "re15_story_r42.h"
#include "re15_aot.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_rdt.h"
#include "re15_msg.h"
#include "re15_actor.h"
#include "re15_engine.h"
#include "re15_inventory.h"
#include "re15_itembox.h"
#include "re15_item_prompt.h"
#include "re15_gameflow.h"
#include "re15_fade.h"
#include "re15_zustand_r39.h"       /* NB2 N5: RE2-Gift-Takt-Zaehler (cfe12) sichern */
#include "re15_story1_r42.h"        /* RE15_S1_BLENDE_* (Bausteine der Abblende, belegt dort) */
#include "font_width.h"             /* Vorschub DEBUG.BIN[0x4416+code] fuer msg 20 */

#include "gen/story3a_r42_texte.inc"

extern unsigned g_current_room_id;
extern int g_re15_voice_laeuft, g_re15_voice_restbilder;
extern int  re15_player_equipped_weapon(void);   /* re15_damage.c (DAT_800aca5d) */
extern void re15_player_waffe_roh(int id);       /* re15_damage.c: nur aca5d (wie @0x800316c4) */
extern void re15_audio_weapon_se(int se_id);     /* audio_pc.c (Test-Stub tests/test_support.c) */
extern uint8_t re15_em_status_zone(void);        /* enemy_ai_common.c */

#define S3A_LOG(...) do { fprintf(stderr, "[story3a] F%u ", (unsigned)g_engine.frame_count); fprintf(stderr, __VA_ARGS__); \
                          fputc('\n', stderr); } while (0)

unsigned g_re15_s3a_zaehler[12];

static re15_s3a_figur_t s_f;      /* Spielstand-Teil (export/import) */

static int leon(void) { return re15_char_variant() == 0; }
static int bit9(int b) { return re15_game_flag_get(RE15_R42_BANK, (uint8_t)b) ? 1 : 0; }
static void bit9_setzen(int b) { re15_game_flag_set(RE15_R42_BANK, (uint8_t)b, 1); }
static int ist_leon_modell(int m) { return m >= 0 && m <= RE15_S3A_LEON_MAX; }

int re15_story3a_r42_marvin_spielt(void) { return s_f.aktiv ? 1 : 0; }
const re15_s3a_figur_t *re15_story3a_r42_figur(void) { return &s_f; }

/* ==== T-01/T-02: Figurwechsel ============================================================================================ */
static void seite_loggen(const char *was, const re15_inv_slot_t *s)
{
    char b[160]; int n = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS && n < (int)sizeof b - 12; i++)
        if (s[i].id) n += snprintf(b + n, sizeof b - (size_t)n, " %02x x%u", (unsigned)s[i].id, (unsigned)s[i].qty);
    if (!n) snprintf(b, sizeof b, " (leer)");
    S3A_LOG("%s:%s", was, b);
}

static int s_waffe_vor = -1;   /* DAT_800aca5d VOR dem Schwanz von FUN_800314b0 (der setzt sie auf @0x800c00d4[Modell]) */
static int s_hp_vor = -1;      /* +0x9a VOR dem Schwanz (HP := 100 @0x80031710/18)                                      */
static unsigned s_status_vor;  /* +0x98 VOR dem Schwanz (Status := 0 @0x80031720)                                         */
static int s_rueck_frisch = 0; /* NB1 B6: Rueckwechsel gerade gelaufen -> "Gunshots...?" nur beim DIREKT folgenden 3020   */

int re15_story3a_r42_figur_vor(int alt, int neu)
{
    (void)alt;
    s_waffe_vor = re15_player_equipped_weapon();
    s_hp_vor = (int)g_actors[RE15_ACTOR_SLOT_PLAYER].hp;
    s_status_vor = (unsigned)g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags;
    /* Neuladen im laufenden Tausch (Spielstand als Marvin geladen: der Lader zieht das Modell 0x0D nach, main.c CONTINUE
     * @0x80039760-8c): kein neuer Figurwechsel - Seite, Waffe, HP und Status kommen aus dem Spielstand. */
    if ((neu & 0x0F) == RE15_S3A_MODELL_MARVIN && s_f.aktiv) {
        g_re15_s3a_zaehler[2]++;
        S3A_LOG("Neuladen als Marvin im laufenden Tausch: Seite/Waffe/HP aus dem Spielstand (kein Schwanz FUN_800314b0)");
        return 1;
    }
    return 0;
}

void re15_story3a_r42_figur_nach(int alt, int neu)
{
    alt &= 0x0F; neu &= 0x0F;
    if (neu == RE15_S3A_MODELL_MARVIN && s_f.aktiv) {
        /* Neuladen (s. o.): Waffe := Gegenstand des Ausruest-Platzes (RE2 @0x80069180..@0x800691a0) */
        const int p = re15_inv_equipped_slot();
        if (p >= 0 && p < RE15_INV_MAX_SLOTS && g_inv.slots[p].id && re15_item_is_weapon(g_inv.slots[p].id))
            re15_player_waffe_roh(g_inv.slots[p].id);
        return;
    }
    if (!leon()) return;   /* Elza <-> Sherry: unveraendert (Runde 41 Spur K) */
    if (ist_leon_modell(alt) && neu == RE15_S3A_MODELL_MARVIN && !s_f.aktiv) {
        /* Eintritt (RE2 @0x80068fc8..@0x800690f4): Ausruest-Platz + Waffe sichern, 11 Plaetze sichern, Marvins Seite laden. */
        s_f.leon_platz = (uint8_t)re15_inv_equipped_slot();
        s_f.leon_waffe = (uint8_t)(s_waffe_vor >= 0 ? s_waffe_vor : re15_player_equipped_weapon());
        /* NB1 B8: Vor-Platz (Port) und - wie RE2 FUN_80049e48 beim Eintritt (`lhu v0,0x3b66(s0)` Leben @0x8004a000, Gift
         * @0x8004a004/08 -> `sh v0,..d46be` @0x8004a020, `sb v1,..d46b3` @0x8004a02c, `sh a0,..d46b4` @0x8004a038) - Leben und
         * Gift VOR dem RE1.5-Schwanz (der setzt HP 100 @0x80031710 / Status 0 @0x80031720; Marvin beginnt damit gesund). */
        s_f.leon_vorplatz = (uint8_t)re15_inv_prev_equip_slot();
        {
            const int hp = s_hp_vor >= 0 ? s_hp_vor : 100;
            s_f.leon_hp_lo = (uint8_t)(hp & 0xff); s_f.leon_hp_hi = (uint8_t)((hp >> 8) & 0xff);
            /* Gift-Wort cfe10 (@0x8004a008 -> @0x8004a038) und - NB2 N5 - Takt-Zaehler cfe12 (`lbu v1,0x3c2a(s0)` @0x8004a004 ->
             * `sb v1,..d46b3` @0x8004a02c), der fuer Marvin genullt wird (`sb zero,0x3c2a(s0)` @0x8004a010) */
            const unsigned takt = (unsigned)re15_zustand_r39_gift_zaehler() & RE15_S3A_TAKT_MASKE;
            s_f.leon_gift = (uint8_t)(((s_status_vor & 0x2u) ? RE15_S3A_GIFT_BIT : 0u) | (takt << RE15_S3A_TAKT_SHIFT));
            re15_zustand_r39_gift_takt_setzen(0);
        }
        memcpy(s_f.leon, g_inv.slots, sizeof s_f.leon);
        memset(s_f.marvin, 0, sizeof s_f.marvin);
        s_f.marvin_offen = 0;
        s_f.aktiv = 1;
        memset(g_inv.slots, 0, sizeof g_inv.slots);
        g_inv.slots[0].id = RE15_S3A_PISTOLE;  g_inv.slots[0].qty = RE15_S3A_PISTOLE_MENGE;
        g_inv.slots[1].id = RE15_S3A_MUNITION; g_inv.slots[1].qty = RE15_S3A_MUNITION_MENGE;
        re15_inv_set_equipped_slot(0);                       /* RE2 Ada: Platz 0 @0x80069024 */
        re15_inv_set_prev_equip_slot(RE15_S3A_LEER_PLATZ);
        re15_player_waffe_roh(RE15_S3A_PISTOLE);             /* RE2 Ada: Waffe 3 @0x8006901c */
        g_re15_s3a_zaehler[0]++;
        seite_loggen("Tausch Leon -> Marvin, Leons Seite gesichert", s_f.leon);
        S3A_LOG("Leon: Ausruest-Platz 0x%02x Waffe %d Vor-Platz 0x%02x HP %d Gift %d Takt %u; Marvin: Platz 0 Waffe %d (Browning x%d, "
                "H. Gun Bullets x%d)", (unsigned)s_f.leon_platz, (int)s_f.leon_waffe, (unsigned)s_f.leon_vorplatz,
                (int)s_f.leon_hp_lo | ((int)s_f.leon_hp_hi << 8), (int)(s_f.leon_gift & RE15_S3A_GIFT_BIT),
                (unsigned)((s_f.leon_gift >> RE15_S3A_TAKT_SHIFT) & RE15_S3A_TAKT_MASKE),
                re15_player_equipped_weapon(),
                RE15_S3A_PISTOLE_MENGE, RE15_S3A_MUNITION_MENGE);
        return;
    }
    if (alt == RE15_S3A_MODELL_MARVIN && ist_leon_modell(neu) && s_f.aktiv) {
        /* Rueckweg (RE2 @0x80069108..@0x800691e8): Marvins Seite fuer die Uebergabe festhalten (Konzept L6: die Funde verfallen
         * nicht, Marvin uebergibt sie), Leons Plaetze/Ausruest-Platz/Waffe zurueck. */
        memcpy(s_f.marvin, g_inv.slots, sizeof s_f.marvin);
        s_f.marvin_offen = 1;
        memcpy(g_inv.slots, s_f.leon, sizeof g_inv.slots);
        re15_inv_set_equipped_slot(s_f.leon_platz);
        re15_inv_set_prev_equip_slot(s_f.leon_vorplatz);                 /* NB1 B8: Vor-Platz zurueck (nicht 0x80) */
        re15_player_waffe_roh(s_f.leon_waffe);
        /* NB1 B8: Leben und Gift zurueck wie RE2 FUN_80049e48 im Rueckweg-Zweig (alte Figur > 0xb: `sltiu v0,s1,0xc` @0x8004a044,
         * `lhu v0,..d46be` @0x8004a058 -> `sh v0,0x3b66(s0)` @0x8004a074, Gift @0x8004a064/70 -> @0x8004a078/7c). Ohne das
         * heilte der RE1.5-Schwanz (HP 100 @0x80031710) Leon bei jedem Rueckwechsel. */
        {
            const int hp = (int)s_f.leon_hp_lo | ((int)s_f.leon_hp_hi << 8);
            if (hp > 0) g_actors[RE15_ACTOR_SLOT_PLAYER].hp = (int16_t)hp;
            if (s_f.leon_gift & RE15_S3A_GIFT_BIT) g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags |= 0x2u;   /* cfe10 @0x8004a070/7c */
            re15_zustand_r39_gift_takt_setzen((uint8_t)((s_f.leon_gift >> RE15_S3A_TAKT_SHIFT) & RE15_S3A_TAKT_MASKE));   /* cfe12 @0x8004a064/78 */
        }
        s_f.aktiv = 0;
        s_rueck_frisch = 1;
        g_re15_s3a_zaehler[1]++;
        seite_loggen("Tausch Marvin -> Leon, Marvins Seite (Uebergabe offen)", s_f.marvin);
        seite_loggen("Leons Seite zurueck", g_inv.slots);
        S3A_LOG("Leon: Ausruest-Platz 0x%02x Waffe %d Vor-Platz 0x%02x HP %d Gift %d Takt %u", (unsigned)re15_inv_equipped_slot(),
                re15_player_equipped_weapon(), (unsigned)re15_inv_prev_equip_slot(), (int)g_actors[RE15_ACTOR_SLOT_PLAYER].hp,
                (g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags & 0x2u) ? 1 : 0, (unsigned)re15_zustand_r39_gift_zaehler());
    }
}

void re15_story3a_r42_neues_spiel(void) { memset(&s_f, 0, sizeof s_f); s_rueck_frisch = 0; }

void re15_story3a_r42_export(uint8_t out[RE15_S3A_SPEICHER_BYTES])
{
    memset(out, 0, RE15_S3A_SPEICHER_BYTES);
    memcpy(out, &s_f, sizeof s_f < RE15_S3A_SPEICHER_BYTES ? sizeof s_f : RE15_S3A_SPEICHER_BYTES);
}

void re15_story3a_r42_import(const uint8_t in[RE15_S3A_SPEICHER_BYTES])
{
    memset(&s_f, 0, sizeof s_f);
    s_rueck_frisch = 0;
    memcpy(&s_f, in, sizeof s_f < RE15_S3A_SPEICHER_BYTES ? sizeof s_f : RE15_S3A_SPEICHER_BYTES);
    if (s_f.aktiv > 1 || s_f.marvin_offen > 1) memset(&s_f, 0, sizeof s_f);   /* fremde Bytes: kein Tausch */
    if (s_f.aktiv) {
        g_scd.work_vars[0x10] = RE15_S3A_MODELL_MARVIN;   /* DAT_800b0ff0 = Block+0x234 im Original-Save (@0x80026294-a0) */
        re15_vest_model_mark(RE15_S3A_MODELL_MARVIN);
        S3A_LOG("Spielstand: Marvin spielt (Leons Seite gesichert) -> work_vars[0x10] = 0x%02X", RE15_S3A_MODELL_MARVIN);
    }
    if (s_f.marvin_offen) seite_loggen("Spielstand: Marvins Funde noch nicht uebergeben", s_f.marvin);
}

_Static_assert(sizeof(re15_s3a_figur_t) <= RE15_S3A_SPEICHER_BYTES, "re15_s3a_figur_t passt nicht in story_r42[]");

/* ==== Texte ============================================================================================================= */
const uint8_t *re15_story3a_r42_text(uint16_t room_id, uint8_t id, int *out_len)
{
    for (int i = 0; i < S3A_TEXTE_N; i++)
        if (k_s3a_texte[i].raum == room_id && k_s3a_texte[i].id == id) {
            if (out_len) *out_len = k_s3a_texte[i].n;
            return k_s3a_texte[i].b;
        }
    if (out_len) *out_len = 0;
    return NULL;
}

static void text_ablegen(uint8_t id, const uint8_t *b, int n)
{
    re15_msg_install_text(id, b, (size_t)n);
    const int d = re15_msg_compute_duration(b, n, 0);   /* wie re15_msg_load_room_block */
    if (d > 0 && d < 65535) re15_msg_install_durations(id, d);
}

static void texte_einsetzen(uint16_t room_id)
{
    int n = 0;
    for (int i = 0; i < S3A_TEXTE_N; i++)
        if (k_s3a_texte[i].raum == room_id) { text_ablegen(k_s3a_texte[i].id, k_s3a_texte[i].b, k_s3a_texte[i].n); n++; }
    if (n) S3A_LOG("ROOM%04X: %d Nachrichten eingesetzt (nach den RDT-Texten)", (unsigned)room_id, n);
}

/* NB1 B1: Haken in re15_msg_load_room_block NACH den RDT-Texten (wie re15_story_r36_texte). Der Installer laeuft im Raumaufbau
 * VOR Schritt 11 (room_common.c re15_msg_load_room_block) - dort eingesetzte Texte mit Ids im RDT-Bereich (3020 msg 0..2,
 * 30E0 msg 3) ueberschrieb der Loader wieder (Pruefer pruef_s3a_r1 B1, Lauf: alter Wortlaut). */
void re15_story3a_r42_texte(uint16_t room_id)
{
    if (!leon()) return;
    if (room_id == RE15_S3A_RAUM_3020 || room_id == RE15_S3A_RAUM_3060 || room_id == RE15_S3A_RAUM_30E0) texte_einsetzen(room_id);
}

/* msg 20 "You've received the <Name>." fuer einen Fund (Name = Namensbank wie "Will you take the <Name>?"); Umbruch vor dem
 * Namen, wenn die Zeile breiter als der Textkasten waere (271 px, texte_stage3a.py MAX_PX). */
static void msg20_bauen(uint8_t item)
{
    static uint8_t b[160];
    uint8_t nm[64];
    int nn = re15_item_prompt_name(item, nm, (int)sizeof nm);
    int n = 0, w = S3A_MSG20_KOPF_PX;
    memcpy(b, k_s3a_msg20_kopf, sizeof k_s3a_msg20_kopf); n = (int)sizeof k_s3a_msg20_kopf;
    for (int i = 0; i < nn; i++) w += re15_font_width[nm[i]];
    for (int i = 0; i < (int)sizeof k_s3a_msg20_fuss - 2; i++) w += re15_font_width[k_s3a_msg20_fuss[i]];
    if (w > S3A_MAX_PX && n > 2 && b[n - 1] == 0x00) b[n - 1] = 0x08;   /* das Leerzeichen vor dem Namen wird zum Umbruch */
    memcpy(b + n, nm, (size_t)nn); n += nn;
    memcpy(b + n, k_s3a_msg20_fuss, sizeof k_s3a_msg20_fuss); n += (int)sizeof k_s3a_msg20_fuss;
    text_ablegen(RE15_R42_MSG_3060_ERSTE + 9, b, n);   /* 20 */
}

/* ==== Opcode-Formen (wie story2_r42.c; Satzbreiten = s_opcode_sizes, scd_vm.c) ============================================= */
#define LE16(v)            (uint8_t)((uint16_t)(v) & 0xff), (uint8_t)(((uint16_t)(v) >> 8) & 0xff)
#define OP_SET(b,i,v)      0x22, (b), (i), (v)
#define OP_CUT(c)          0x29, (c)
#define OP_CUT_AUTO        0x3c, 0x01
#define OP_WORK(k,i)       0x2e, (k), (i), 0x00
#define OP_PLC_RET         0x42, 0x00
#define OP_DEST(m,bit,x,z) 0x40, 0x00, (m), (bit), LE16(x), LE16(z)
#define OP_WARTE(bit)      0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, (bit), 0x00
#define OP_SLEEP(n)        0x09, 0x0a, LE16(n)
#define OP_END             0x01, 0x00
#define OP_POS(x,y,z)      0x32, 0x00, LE16(x), LE16(y), LE16(z)            /* Pos_set ROOM3060 sub03 @0x01064 */
#define OP_EM(slot,typ,x,z,gier) 0x44, (slot), (typ), 0x00, 0x00, 0x00, 0x00, 0xff, LE16(x), 0x00, 0x00, LE16(z), 0x00, 0x00, \
                           LE16(gier), 0x00, 0x00                           /* ROOM3060 sub00 @0x00F58 `44 00 40 00 00 00 00 ff ..` */
#define OP_ZEILE(id)       OP_SET(5, RE15_S3A_SIG_FREI, 0), 0x2b, (id), 0x00, 0x00, OP_SLEEP(2), OP_WARTE(RE15_S3A_SIG_FREI)
#define OP_RIEGEL          OP_SET(2, 7, 1), OP_SET(1, 27, 1)                /* ROOM3060 sub03 @0x01056 / @0x0105A */
#define OP_RIEGEL_LOS      OP_SET(2, 7, 0), OP_SET(1, 27, 0)
#define OP_UEBERGABE       OP_SET(5, RE15_S3A_SIG_UEBERGABE, 1), OP_SLEEP(1)  /* Takt (vor den Faeden) gibt den naechsten Fund */
#define M_GEHEN  0x04      /* WALK: ROOM1021 sub02 @0x020B2 `40 00 04 22` */
#define M_DREH   0x09      /* drehen: ROOM2040 sub02 @0x01648 `40 00 09 20` */
#define S3A_N    RE15_S3A_NPC_X, RE15_S3A_NPC_Z
#define S3A_P    RE15_S3A_SPIELER_X, RE15_S3A_SPIELER_Z
#define S3A_T    RE15_S3A_TUER_X, RE15_S3A_TUER_Z
#define OP_DESTV(...)      OP_DEST(__VA_ARGS__)

static const uint8_t k_evt_end[2] = { OP_END };

static int16_t le_s16(const uint8_t *p) { return (int16_t)(uint16_t)(p[0] | (p[1] << 8)); }

static const uint8_t *rdt_bytes(int off, const uint8_t *soll, int n)
{
    const re15_rdt_t *r = &g_room_rdt;
    if (!r->raw || off < 0 || off + n > r->raw_size) return NULL;
    if (soll && memcmp(r->raw + off, soll, (size_t)n) != 0) return NULL;
    return r->raw + off;
}

/* ---- 3020 sub02 mit Marvins Zeile 7 und Leons Zeile 8 vor dem Wechsel (L5 Text-Tabelle) ---------------------------------- */
static uint8_t s_p3020[(RE15_S3A_3020_SUB02_ENDE - RE15_S3A_3020_SUB02_OFF) + 64];
static int     s_p3020_n;
static int p3020_bauen(void)
{
    static const uint8_t kopf[] = { 0x22, 0x04, 0xc7, 0x00, 0x46, 0x03 };                          /* @0x0C10 Set(4,0xC7,0), Aot_reset 3 */
    static const uint8_t fuss[] = { 0x09, 0x0a, 0x0a, 0x00, 0x24, 0x10, 0x0d, 0x00, 0x47, 0x02, 0x01, 0x00 };   /* @0x0D76..@0x0D81 */
    static const uint8_t zeilen[] = { OP_ZEILE(RE15_R42_MSG_3020_ERSTE), OP_ZEILE(RE15_R42_MSG_3020_ERSTE + 1) };   /* 7, 8 */
    const uint8_t *a = rdt_bytes(RE15_S3A_3020_SUB02_OFF, kopf, (int)sizeof kopf);
    const uint8_t *e = rdt_bytes(RE15_S3A_3020_WECHSEL - 4, fuss, (int)sizeof fuss);
    if (!a || !e) return 0;
    const int n0 = RE15_S3A_3020_WECHSEL - RE15_S3A_3020_SUB02_OFF;
    memcpy(s_p3020, a, (size_t)n0);
    memcpy(s_p3020 + n0, zeilen, sizeof zeilen);
    memcpy(s_p3020 + n0 + sizeof zeilen, a + n0, (size_t)(RE15_S3A_3020_SUB02_ENDE - RE15_S3A_3020_WECHSEL));
    s_p3020_n = n0 + (int)sizeof zeilen + (RE15_S3A_3020_SUB02_ENDE - RE15_S3A_3020_WECHSEL);
    return 1;
}

/* ---- 3060 sub03 mit Marvins Ausruf (11), Schuessen und Abblende vor dem Rueckwechsel (L5 Punkt 1) ------------------------- */
static uint8_t s_p3060[(RE15_S3A_3060_SUB03_ENDE - RE15_S3A_3060_SUB03_OFF) + 64];
static int     s_p3060_n;
static int p3060_bauen(void)
{
    static const uint8_t kopf[] = { 0x22, 0x04, 0x08, 0x01, 0x22, 0x04, 0x09, 0x01 };                          /* @0x0104A */
    static const uint8_t mitte[] = { 0x3f, 0x02, 0x06, 0x00, 0x09, 0x0a, 0x28, 0x00, 0x06, 0x00, 0x10, 0x00 };  /* @0x010BE..@0x010C9 */
    static const uint8_t fuss[] = { 0x47, 0x03, 0x01, 0x00 };                                                  /* @0x010E4 */
    static const uint8_t neu[] = { OP_ZEILE(RE15_R42_MSG_3060_ERSTE), OP_SET(5, RE15_S3A_SIG_ABBLENDE, 1),
                                   OP_WARTE(RE15_S3A_SIG_SCHWARZ) };
    const uint8_t *a = rdt_bytes(RE15_S3A_3060_SUB03_OFF, kopf, (int)sizeof kopf);
    if (!a || !rdt_bytes(0x010BE, mitte, (int)sizeof mitte) || !rdt_bytes(0x010E4, fuss, (int)sizeof fuss)) return 0;
    const int n0 = RE15_S3A_3060_NACH_SLEEP - RE15_S3A_3060_SUB03_OFF;
    memcpy(s_p3060, a, (size_t)n0);
    memcpy(s_p3060 + n0, neu, sizeof neu);
    memcpy(s_p3060 + n0 + sizeof neu, a + n0, (size_t)(RE15_S3A_3060_SUB03_ENDE - RE15_S3A_3060_NACH_SLEEP));
    s_p3060_n = n0 + (int)sizeof neu + (RE15_S3A_3060_SUB03_ENDE - RE15_S3A_3060_NACH_SLEEP);
    return 1;
}

/* ---- 3020 nach dem Rueckwechsel: "Gunshots...? Marvin!" (L-08). Nur die Pad-Sperre (Set(2,7) ROOM2080 sub02 @0x0CB6), kein
 *      Letterbox: eine Reaktionszeile, keine Zwischensequenz. ------------------------------------------------------------- */
static const uint8_t k_p_schuesse[] = {
    OP_SET(RE15_R42_BANK, RE15_R42_BIT_3020_SCHUESSE, 1), OP_SET(2, 7, 1),
    OP_ZEILE(RE15_R42_MSG_3020_ERSTE + 2),                                   /* 9 */
    OP_SET(2, 7, 0), OP_END
};

/* ---- 3060 Wiedersehen (L5 Punkte 3-6, L6, L7). Zur Laufzeit gebaut: welche Uebergaben es gibt, haengt von Marvins Seite ab. --- */
static uint8_t s_pw[640];
static int     s_pw_n;
static uint8_t s_ueb[RE15_INV_MAX_SLOTS + 2];     /* Uebergabe-Folge: Gegenstand je Schritt, 0xFE = Fabrikplan */
static int     s_ueb_n, s_ueb_i;
#define S3A_PLAN 0xFE

static void pw(const uint8_t *b, int n) { if (s_pw_n + n <= (int)sizeof s_pw) { memcpy(s_pw + s_pw_n, b, (size_t)n); s_pw_n += n; } }
#define PW(...) do { static const uint8_t _b[] = { __VA_ARGS__ }; pw(_b, (int)sizeof _b); } while (0)

static int seite_hat(const re15_inv_slot_t *s, uint8_t id)
{
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (s[i].id == id) return i;
    return -1;
}

/* H. Gun Bullets in Marvins Seite ueber seiner Startmenge (alle Stapel zusammen). */
static int munition_ueber(void)
{
    int n = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (s_f.marvin[i].id == RE15_S3A_MUNITION) n += s_f.marvin[i].qty;
    return n > RE15_S3A_MUNITION_MENGE ? n - RE15_S3A_MUNITION_MENGE : 0;
}

/* Strang fabrikplan: Marvin traegt den Fabrikplan = er hat ihn in 30C0 genommen (kartenitems_r36 Zeile 30C0, begleit_bit) und Leon
 * besitzt ihn noch nicht (Bit 35: Uebergabe schon gelaufen oder Leon nahm ihn selbst in 30C0). */
static int marvin_traegt_plan(void)
{
    return bit9(RE15_R42_BIT_PLAN_MARVIN) && !bit9(RE15_R42_BIT_PLAN_FABRIK);
}

static void wiedersehen_bauen(void)
{
    s_pw_n = 0; s_ueb_n = 0; s_ueb_i = 0;
    /* Funde (F-06: alle Funde, die eigene Pistole und seine Munition behaelt Marvin, "weil er sich in der Lobby verteidigen
     * muss" L6). Reihenfolge (NB2 N3): die Green Keycard zuerst (L6), und Leons "A keycard... This should open the door in the
     * loading area." (21) folgt DIREKT auf "You've received the Green Keycard." (16) - die Zeile meint diese Karte. Hat Marvin die
     * Red Master Keycard behalten (Wegwerf-Frage am Rechner K8 ROOM3050 abgelehnt; sonst ist sie verbraucht: sub15 msg 5 @0x02608,
     * Konzept 6b), kommt ihr Empfang VOR der Green Keycard: 15 "But I found this." -> Red (20) -> Green (16) -> 21. Dann der
     * Fabrikplan (17/18), dann alle weiteren Funde (19, 20 je Fund).
     * Strang fabrikplan (NUTZERWUNSCH 20.11.2026 "Das macht natürlich nur sinn wenn er ihn auch wirklich irgendwo aufnimmt..."):
     * 17/18 und der Plan-Schritt NUR, wenn Marvin den Wandplan in 30C0 genommen hat (Bit RE15_R42_BIT_PLAN_MARVIN, kartenitems_r36
     * Zeile 30C0) und Leon ihn nicht schon besitzt (Bit 35). Sonst entfallen 17/18; Ids, Texte und Reihenfolge der uebrigen
     * Zeilen bleiben (die Aufnahmen synchro/STAGE3/room3060/mainNN.wav haengen an den Ids). */
    int karte = 0;
    if (s_f.marvin_offen && seite_hat(s_f.marvin, RE15_S3A_KEYCARD) >= 0) karte = 1;
    /* Altstand v0.8.30 (gemeinsames Inventar, kein Tausch): Marvin hat die Karte genommen (Taken-Bit 85 @0x014B2), Leon traegt sie
     * aber nicht -> sie wird trotzdem uebergeben; traegt Leon sie schon, entfaellt die Kartenuebergabe. */
    if (!karte && !s_f.marvin_offen && re15_game_flag_get(9, 85) && re15_inv_find_item(RE15_S3A_KEYCARD) < 0 &&
        !re15_game_flag_get(RE15_S3A_KARTE_BANK, RE15_S3A_KARTE_BENUTZT)) karte = 1;
    const int rot = s_f.marvin_offen && seite_hat(s_f.marvin, RE15_S3A_ROT) >= 0;
    if (karte && rot) s_ueb[s_ueb_n++] = RE15_S3A_ROT;
    if (karte) s_ueb[s_ueb_n++] = RE15_S3A_KEYCARD;
    const int plan = marvin_traegt_plan();
    if (plan) s_ueb[s_ueb_n++] = S3A_PLAN;
    const int erste_weitere = s_ueb_n;
    if (s_f.marvin_offen) {
        if (!karte && rot) s_ueb[s_ueb_n++] = RE15_S3A_ROT;
        if (seite_hat(s_f.marvin, RE15_S3A_REDHAWK) >= 0) s_ueb[s_ueb_n++] = RE15_S3A_REDHAWK;
        for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) {
            const uint8_t id = s_f.marvin[i].id;
            if (!id || id == RE15_S3A_KEYCARD || id == RE15_S3A_ROT || id == RE15_S3A_REDHAWK || id == RE15_S3A_PISTOLE ||
                id == RE15_S3A_MUNITION) continue;
            s_ueb[s_ueb_n++] = id;
        }
        /* NB1 B8: Munition. F-06 "alle Funde, die eigene Pistole behaelt Marvin"; L6 "weil er sich in der Lobby verteidigen muss".
         * Seine eigene Munition (Startmenge RE15_S3A_MUNITION_MENGE @0x80074bd0) behaelt er mit der Pistole; was er an H. Gun
         * Bullets darueber hinaus aufgehoben hat, ist ein Fund und geht an Leon. */
        if (munition_ueber() > 0) s_ueb[s_ueb_n++] = RE15_S3A_MUNITION;
    }

    PW(OP_RIEGEL, OP_CUT(RE15_S3A_CUT_TUER),
       OP_EM(RE15_S3A_NPC_SLOT, RE15_S3A_TYP_MARVIN, RE15_S3A_TUER_X, RE15_S3A_TUER_Z, RE15_S3A_GIER_TUER), OP_SLEEP(1),
       OP_WORK(2, RE15_S3A_NPC_SLOT), OP_DESTV(M_GEHEN, RE15_S3A_AN_NPC, S3A_N), OP_WARTE(RE15_S3A_AN_NPC),
       OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 1),                                                   /* 12 "Leon... Over here." */
       OP_SET(5, RE15_S3A_SIG_ABBLENDE, 1), OP_WARTE(RE15_S3A_SIG_SCHWARZ),                     /* Zeitsprung: Leon laeuft hin */
       OP_WORK(1, 0), OP_POS(RE15_S3A_SPIELER_X, 0, RE15_S3A_SPIELER_Z),
       OP_DESTV(M_DREH, RE15_S3A_AN_SPIELER, S3A_N), OP_WARTE(RE15_S3A_AN_SPIELER),
       OP_WORK(2, RE15_S3A_NPC_SLOT), OP_DESTV(M_DREH, RE15_S3A_AN_NPC, S3A_P), OP_WARTE(RE15_S3A_AN_NPC),
       OP_SET(5, RE15_S3A_SIG_EIN, 1), OP_WARTE(RE15_S3A_SIG_HELL),
       OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 2), OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 3));           /* 13, 14 */
    if (karte) {
        PW(OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 4));                                               /* 15 */
        if (rot) PW(OP_UEBERGABE, OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 9));                        /* 20 "...Red Master Keycard." */
        PW(OP_UEBERGABE, OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 5), OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 10));   /* 16, 21 */
    }
    if (plan) PW(OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 6), OP_UEBERGABE, OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 7));   /* 17, 18 */
    if (s_ueb_n > erste_weitere) {
        PW(OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 8));                                                               /* 19 */
        for (int k = erste_weitere; k < s_ueb_n; k++) PW(OP_UEBERGABE, OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 9));   /* 20 je Fund */
    }
    PW(OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 11), OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 12), OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 13),
       OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 14), OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 15), OP_ZEILE(RE15_R42_MSG_3060_ERSTE + 16),
       /* 22..27; Marvin geht durch die orange Tuer zurueck (L5 Punkt 6 "Marvin geht in die Lobby 30C0" - ueber 3040).
        * KONZEPT L5 Punkt 4 "Marvin humpelt" NICHT gebaut (NB1 B4): die NPC-Gangarten des Originals sind nur Sub 4/5/7/8
        * (@0x80076ca0: Gehen Clip 5, Rennen, zwei Rueckwaerts-Subs); Marvins eigene Bank (CDEMD0.EMS Blob 18, 24 Clips) hat keinen
        * belegten Hinke-Zyklus. Gebaut ist M_GEHEN (ROOM1021 sub02 @0x020B2). Offen beim Nutzer (NB2: N4 nicht bauen). */
       OP_WORK(2, RE15_S3A_NPC_SLOT), OP_DESTV(M_GEHEN, RE15_S3A_AN_NPC, S3A_T), OP_WARTE(RE15_S3A_AN_NPC),
       OP_SET(5, RE15_S3A_SIG_MARVIN_WEG, 1), OP_SLEEP(1),
       OP_SET(RE15_R42_BANK, RE15_R42_BIT_3060_UEBERGABE, 1),
       OP_CUT_AUTO, OP_RIEGEL_LOS, OP_WORK(1, 0), OP_PLC_RET, OP_END);
}

const uint8_t *re15_story3a_r42_programm(int welches, int *out_len)
{
    if (welches == 0 && p3020_bauen()) { if (out_len) *out_len = s_p3020_n; return s_p3020; }
    if (welches == 1 && p3060_bauen()) { if (out_len) *out_len = s_p3060_n; return s_p3060; }
    if (welches == 2) { wiedersehen_bauen(); if (out_len) *out_len = s_pw_n; return s_pw; }
    if (welches == 3) { if (out_len) *out_len = (int)sizeof k_p_schuesse; return k_p_schuesse; }
    if (out_len) *out_len = 0;
    return NULL;
}

static int starten(const uint8_t *p)
{
    for (int slot = SCD_EVENT_SLOT_FIRST; slot <= SCD_EVENT_SLOT_LAST; slot++)
        if (!g_scd.threads[slot].active && scd_thread_start(slot, p) == 0) return slot;
    return -1;
}

/* ==== Zustand der Szenen ================================================================================================== */
enum { SZ_KEINE = 0, SZ_3020, SZ_3060_MARVIN, SZ_SCHUESSE, SZ_WIEDERSEHEN };
static int s_szene = SZ_KEINE;
static int s_schuesse_faellig = 0;   /* 3020: nach dem Rueckwechsel "Gunshots...?" */
static int s_blende = 0;             /* 0 aus, 1 Rampe aus, 2 schwarz, 3 Rampe ein */
static int s_halten = 0;
static int s_knall = 0, s_knall_takt = 0;
static int s_schwarz_bis_raumwechsel = 0;
static uint32_t s_start_bild = 0;
static int s_frisch = 0;            /* Szene im Raumaufbau gestartet (sub00 laeuft VOR dem Installer): install laesst sie stehen */
int re15_story3a_r42_szene_laeuft(void) { return s_szene; }

/* ==== 3010: Kartentuer erst nach der Uebergabe (L-10) ========================================================================
 * Kein eigener Riegel, GEMESSEN (unit_r42_story3a_karte3010, Lauf r1): die Kistenregel des Ports (kistenregel_r37.c Fall K7, RE2-Regel
 * "Benutzung nur mit dem Gegenstand im INVENTAR") laesst Ck(3,0x3B)==0 @0x020AE nur mit Bit UND Green Keycard 0x36 im Inventar
 * durch. Seit T-01 liegt die Karte bis zur Uebergabe in Marvins Seite - die Tuer bleibt "locked" (msg 0 @0x020B2), auch wenn
 * (3,0x3B) schon gesetzt ist; die Uebergabe gibt Leon die Karte und setzt (3,0x3B) wie ROOM3040 sub01 @0x014DA (geben()), dann
 * fuehrt derselbe Satz in sub02 @0x020D2. Die 30E0-Seite der Kartentuer (Slot 0 @0x009F2) erreicht Marvin nicht: West-Ring von 30E0,
 * Marvin kommt in den Ost-Ring (Messung re15_story3a_r42.h "L-10 gegen Marvins Weg", NB1 B2). */

/* ==== Haken =============================================================================================================== */
void re15_story3a_r42_install(uint16_t room_id)
{
    if (s_schwarz_bis_raumwechsel && room_id != RE15_S3A_RAUM_3060) {
        /* Rueckwechsel 3060 -> 3020: das Schwarz der Abblende geht in die Einblende des Raumwechsels ueber (room_common.c
         * @0x8001cc00/18 setzt Kanal 0 neu auf -0x1800 ab 0x7fff) - hier nur den Takt beenden. */
        s_schwarz_bis_raumwechsel = 0; s_blende = 0;
    }
    if (s_szene != SZ_KEINE && !s_frisch) s_szene = SZ_KEINE;
    s_frisch = 0;
    if (!leon()) return;
    const int frisch = s_rueck_frisch;
    s_rueck_frisch = 0;   /* NB1 B6: gilt nur fuer den Raum direkt nach dem Rueckwechsel (3060 sub03 Aot_on 3 @0x010E4 -> 3020) */
    if (room_id == RE15_S3A_RAUM_3020) {
        s_schuesse_faellig = (frisch && !s_f.aktiv && re15_game_flag_get(RE15_S3A_A0_BANK, RE15_S3A_A0_BIT) &&
                              !bit9(RE15_R42_BIT_3020_SCHUESSE)) ? 1 : 0;
        if (s_schuesse_faellig) S3A_LOG("ROOM3020: Leon nach dem Rueckwechsel -> \"Gunshots...?\" faellig (Bit %d = 0, L-08)",
                                        RE15_R42_BIT_3020_SCHUESSE);
    }
}

const uint8_t *re15_story3a_r42_ereignis(uint16_t room_id, uint8_t event_id)
{
    if (room_id != RE15_S3A_RAUM_3020 || event_id != RE15_S3A_3020_EREIGNIS || !leon()) return NULL;
    if (!p3020_bauen()) { S3A_LOG("ROOM3020: sub02-Bytes anders - Original-sub02"); return NULL; }
    s_szene = SZ_3020; s_start_bild = g_engine.frame_count; s_frisch = 1;
    g_re15_s3a_zaehler[3]++;
    S3A_LOG("ROOM3020: Fensterszene = sub02 mit Zeilen %d/%d vor dem Wechsel @0x%04X", RE15_R42_MSG_3020_ERSTE,
            RE15_R42_MSG_3020_ERSTE + 1, RE15_S3A_3020_WECHSEL);
    return s_p3020;
}

const uint8_t *re15_story3a_r42_evt_exec(uint16_t room_id, uint8_t sub, const uint8_t *pc)
{
    if (room_id != RE15_S3A_RAUM_3060 || sub != RE15_S3A_3060_SUB || !leon() || !s_f.aktiv) return pc;
    if (!p3060_bauen()) { S3A_LOG("ROOM3060: sub03-Bytes anders - Original-sub03"); return pc; }
    s_szene = SZ_3060_MARVIN; s_start_bild = g_engine.frame_count; s_knall = 0; s_frisch = 1;
    g_re15_s3a_zaehler[4]++;
    S3A_LOG("ROOM3060: Marvin + Hunde = sub03 mit Zeile %d, %d Schuessen und Abblende vor dem Rueckwechsel @0x%04X",
            RE15_R42_MSG_3060_ERSTE, RE15_S3A_SCHUESSE, RE15_S3A_3060_NACH_SLEEP);
    return s_p3060;
}

/* Fabrik-Blatt: Leons Besitz (Bit 35 - Uebergabe 3060 oder selbst genommen) ODER, solange Marvin spielt, Marvins Plan (Strang
 * fabrikplan: seine Karte zeigt Blatt 7 ganz; nach dem Rueckwechsel hat Leon es erst mit der Uebergabe). */
int re15_story3a_r42_blatt(unsigned page)
{
    if (page != RE15_S3A_FABRIK_BLATT) return 0;
    return bit9(RE15_R42_BIT_PLAN_FABRIK) || (s_f.aktiv && bit9(RE15_R42_BIT_PLAN_MARVIN));
}

/* ---- Uebergabe eines Fundes: in Leons Inventar; ist es voll, in die Kiste (RE2-Ring, re15_itembox.h), damit nichts verloren
 *      geht (die Keycard ist Pflicht fuer 3010). ------------------------------------------------------------------------- */
static void geben(uint8_t id)
{
    if (id == S3A_PLAN) {
        bit9_setzen(RE15_R42_BIT_PLAN_FABRIK);
        /* Strang fabrikplan: Marvin traegt ihn nicht mehr (der Plan ist jetzt Leons Besitz, haengt weder in 30C0 noch in 30C1) */
        re15_game_flag_set(RE15_R42_BANK, RE15_R42_BIT_PLAN_MARVIN, 0);
        g_re15_s3a_zaehler[10]++;
        S3A_LOG("ROOM3060: Fabrikplan uebergeben -> Bit %d = 1 (Kartenblatt %d), Marvins Bit %d = 0", RE15_R42_BIT_PLAN_FABRIK,
                RE15_S3A_FABRIK_BLATT, RE15_R42_BIT_PLAN_MARVIN);
        return;
    }
    uint8_t menge = 1;
    if (id == RE15_S3A_MUNITION) {
        /* nur der Teil ueber Marvins eigener Munition (munition_ueber), abgezogen vom letzten Stapel nach vorn */
        int rest = munition_ueber();
        menge = (uint8_t)(rest > 255 ? 255 : rest);
        for (int k = RE15_INV_MAX_SLOTS - 1; k >= 0 && rest > 0; k--)
            if (s_f.marvin[k].id == RE15_S3A_MUNITION) {
                const int ab = rest < s_f.marvin[k].qty ? rest : s_f.marvin[k].qty;
                s_f.marvin[k].qty = (uint8_t)(s_f.marvin[k].qty - ab); rest -= ab;
                if (!s_f.marvin[k].qty) s_f.marvin[k].id = 0;
            }
    } else {
        const int i = seite_hat(s_f.marvin, id);
        if (i >= 0) { menge = s_f.marvin[i].qty; s_f.marvin[i].id = 0; s_f.marvin[i].qty = 0; }
    }
    if (id == RE15_S3A_KEYCARD && !re15_game_flag_get(RE15_S3A_KARTE_BANK, RE15_S3A_KARTE_BIT)) {
        /* "Karte genommen" wie ROOM3040 sub01: Ck(9,0x55)=1 @0x014CE -> Set(3,0x3B) @0x014DA. sub01 laeuft nur beim Betreten von 3040
         * - nimmt Marvin die Karte und geht, steht das Bit nie, und die Kistenregel K7 (@0x020AE, Bit UND Karte im Inventar) liesse
         * die Kartentuer 3010 fuer immer zu (gemessen Lauf r1: msg 0 trotz Karte im Inventar). */
        re15_game_flag_set(RE15_S3A_KARTE_BANK, RE15_S3A_KARTE_BIT, 1);
        S3A_LOG("ROOM3060: Keycard uebergeben -> (3,0x%02X) = 1 wie ROOM3040 sub01 @0x014DA", RE15_S3A_KARTE_BIT);
    }
    if (id != RE15_S3A_KEYCARD) msg20_bauen(id);
    int ok = 0;
    for (int k = 0; k < RE15_INV_MAX_SLOTS && !ok; k++)
        if (g_inv.slots[k].id == 0) { g_inv.slots[k].id = id; g_inv.slots[k].qty = menge; g_inv.slots[k].flags = 0; ok = 1; }
    if (!ok) {
        re15_inv_slot_t box[RE15_BOX_SLOTS];
        re15_itembox_export(box);
        for (int k = 0; k < RE15_BOX_SLOTS && !ok; k++)
            if (box[k].id == 0) { box[k].id = id; box[k].qty = menge; box[k].flags = 0; ok = 2; }
        if (ok) re15_itembox_import(box);
        g_re15_s3a_zaehler[11]++;
    }
    g_re15_s3a_zaehler[8]++;
    S3A_LOG("ROOM3060: Marvin uebergibt Item 0x%02X x%u -> %s", (unsigned)id, (unsigned)menge,
            ok == 1 ? "Leons Inventar" : ok == 2 ? "Kiste (Inventar voll)" : "VERLOREN (Inventar und Kiste voll)");
}

/* ---- Abblende (Bausteine wie story1_r42.c blende_takt; RE15_S1_BLENDE_*) -------------------------------------------------- */
static void abblende_starten(void)
{
    re15_fade_config(RE15_S1_BLENDE_KANAL, RE15_S1_BLENDE_ABR, RE15_S1_BLENDE_MASKE, (int16_t)RE15_S1_BLENDE_AUS, 0);   /* @0x8001ca64 */
    re15_fade_kick(RE15_S1_BLENDE_KANAL, 0);                                                                          /* @0x8001ca88 */
    s_blende = 1;
}

static void knall(uint16_t room)
{
    re15_audio_weapon_se(RE15_S3A_KNALL_SATZ);
    s_knall++; s_knall_takt = RE15_S3A_SCHUSS_TAKT;
    g_re15_s3a_zaehler[5]++;
    S3A_LOG("ROOM%04X: Schuss %d/%d (ARMS-Satz %d der Waffe %d)", (unsigned)room, s_knall, RE15_S3A_SCHUESSE, RE15_S3A_KNALL_SATZ,
            re15_player_equipped_weapon());
}

static void blende_takt(uint16_t room)
{
    if (s_szene == SZ_3060_MARVIN && s_knall > 0 && s_knall < RE15_S3A_SCHUESSE && --s_knall_takt <= 0) knall(room);
    switch (s_blende) {
    case 1:
        if (!re15_fade_done(RE15_S1_BLENDE_KANAL)) return;
        re15_fade_config(RE15_S1_BLENDE_KANAL, RE15_S1_BLENDE_ABR, RE15_S1_BLENDE_MASKE, 0, 0);       /* @0x80046620-50 */
        re15_fade_kick(RE15_S1_BLENDE_KANAL, RE15_S1_BLENDE_SCHWARZ);
        s_blende = 2; s_halten = RE15_S1_BLENDE_HALTEN;
        S3A_LOG("ROOM%04X: schwarz", (unsigned)room);
        return;
    case 2:
        if (s_szene == SZ_3060_MARVIN) {
            if (s_knall < RE15_S3A_SCHUESSE) return;                            /* erst alle Schuesse */
            if (s_halten > 0) { s_halten--; return; }
            if (!re15_game_flag_get(5, RE15_S3A_SIG_SCHWARZ)) {
                re15_game_flag_set(5, RE15_S3A_SIG_SCHWARZ, 1);
                s_schwarz_bis_raumwechsel = 1;
                S3A_LOG("ROOM%04X: Schwarz gehalten bis zum Raumwechsel - Rueckwechsel auf Leon", (unsigned)room);
            }
            return;
        }
        if (!re15_game_flag_get(5, RE15_S3A_SIG_SCHWARZ)) re15_game_flag_set(5, RE15_S3A_SIG_SCHWARZ, 1);
        if (s_halten > 0) { s_halten--; return; }
        if (!re15_game_flag_get(5, RE15_S3A_SIG_EIN)) return;
        re15_fade_config(RE15_S1_BLENDE_KANAL, RE15_S1_BLENDE_ABR, RE15_S1_BLENDE_MASKE, (int16_t)RE15_S1_BLENDE_EIN, 0); /* @0x8001cc00 */
        re15_fade_kick(RE15_S1_BLENDE_KANAL, 0);                                                                         /* @0x8001cc18 */
        s_blende = 3;
        return;
    case 3:
        if (!re15_fade_done(RE15_S1_BLENDE_KANAL)) return;
        s_blende = 0;
        re15_game_flag_set(5, RE15_S3A_SIG_HELL, 1);
        S3A_LOG("ROOM%04X: Einblende fertig", (unsigned)room);
        return;
    default:
        return;
    }
}

static int hunde_tot(void)
{
    const uint8_t z = re15_em_status_zone();
    for (int k = 0; k < RE15_S3A_HUNDE; k++)
        if (!re15_game_flag_get(z, (uint8_t)(RE15_S3A_HUND_KILL0 + k))) return 0;
    return 1;
}

static void marvin_weg(uint16_t room)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++)
        if (g_actors[i].active && g_actors[i].type == RE15_S3A_TYP_MARVIN) {
            S3A_LOG("ROOM%04X: Marvin (Aktor %d) bei (%d,%d) geht durch die orange Tuer", (unsigned)room, i, (int)g_actors[i].x,
                    (int)g_actors[i].z);
            re15_actor_free(i);
        }
}

void re15_story3a_r42_tick(void)
{
    const uint16_t room = (uint16_t)g_current_room_id;
    if (room != RE15_S3A_RAUM_3020 && room != RE15_S3A_RAUM_3060) return;
    if (s_szene != SZ_KEINE) {
        const int zeile  = g_scd.message_active != 0;
        const int stimme = g_re15_voice_laeuft && g_re15_voice_restbilder > 0;
        if (!zeile && !stimme) re15_game_flag_set(5, RE15_S3A_SIG_FREI, 1);
        blende_takt(room);   /* vor dem Signal: der erste Schuss faellt im Bild der Abblende, die naechsten im Takt danach */
        if (re15_game_flag_get(5, RE15_S3A_SIG_ABBLENDE) && s_blende == 0) {
            re15_game_flag_set(5, RE15_S3A_SIG_ABBLENDE, 0);
            abblende_starten();
            if (s_szene == SZ_3060_MARVIN) knall(room);
            S3A_LOG("ROOM%04X: Abblende (Rampe +0x%X @0x8001ca64)", (unsigned)room, RE15_S1_BLENDE_AUS);
        }
        if (re15_game_flag_get(5, RE15_S3A_SIG_UEBERGABE)) {
            re15_game_flag_set(5, RE15_S3A_SIG_UEBERGABE, 0);
            if (s_ueb_i < s_ueb_n) geben(s_ueb[s_ueb_i++]);
        }
        if (re15_game_flag_get(5, RE15_S3A_SIG_MARVIN_WEG)) { re15_game_flag_set(5, RE15_S3A_SIG_MARVIN_WEG, 0); marvin_weg(room); }
        if (s_szene == SZ_WIEDERSEHEN && bit9(RE15_R42_BIT_3060_UEBERGABE) && s_blende == 0 && !g_scd.message_active) {
            s_f.marvin_offen = 0;
            memset(s_f.marvin, 0, sizeof s_f.marvin);
            s_szene = SZ_KEINE;
            S3A_LOG("ROOM3060: Wiedersehen fertig nach %u Bildern, Bit %d = 1 (Kartentuer 3010 frei)",
                    (unsigned)(g_engine.frame_count - s_start_bild), RE15_R42_BIT_3060_UEBERGABE);
        }
        if (s_szene == SZ_SCHUESSE && bit9(RE15_R42_BIT_3020_SCHUESSE) && !g_scd.message_active && !re15_game_flag_get(2, 7))
            s_szene = SZ_KEINE;
        return;
    }
    if (s_schwarz_bis_raumwechsel) { blende_takt(room); return; }
    if (!leon() || s_f.aktiv || g_room_change.pending || g_scd.message_active || g_scd.player_mode != 0) return;
    if (room == RE15_S3A_RAUM_3020 && s_schuesse_faellig && re15_fade_done(0)) {
        s_schuesse_faellig = 0;
        if (starten(k_p_schuesse) >= 0) {
            s_szene = SZ_SCHUESSE; s_start_bild = g_engine.frame_count;
            g_re15_s3a_zaehler[6]++;
            S3A_LOG("ROOM3020: Leon hoert die Schuesse -> msg %d", RE15_R42_MSG_3020_ERSTE + 2);
        }
        return;
    }
    /* NB1 B6: ohne offene Uebergabe (Altstand v0.8.30, gemeinsames Inventar) nur, solange die Kartentuer 3010 nicht benutzt ist -
     * hat Leon sie schon durchschritten ((3,0x3C) = 1 @0x02302), ist er laengst weiter und die Szene faellt weg. */
    if (room == RE15_S3A_RAUM_3060 && !bit9(RE15_R42_BIT_3060_UEBERGABE) &&
        re15_game_flag_get(RE15_S3A_A0_BANK, RE15_S3A_A0_BIT) && hunde_tot() && re15_fade_done(0) &&
        (s_f.marvin_offen || !re15_game_flag_get(RE15_S3A_KARTE_BANK, RE15_S3A_KARTE_BENUTZT))) {
        wiedersehen_bauen();
        if (starten(s_pw) >= 0) {
            s_szene = SZ_WIEDERSEHEN; s_start_bild = g_engine.frame_count;
            g_re15_s3a_zaehler[7]++;
            S3A_LOG("ROOM3060: Hunde tot (Zone %u Bits 0x%02X..0x%02X) -> Wiedersehen, %d Uebergaben (Programm %d B), Fabrikplan %s",
                    (unsigned)re15_em_status_zone(), RE15_S3A_HUND_KILL0, RE15_S3A_HUND_KILL0 + RE15_S3A_HUNDE - 1, s_ueb_n, s_pw_n,
                    marvin_traegt_plan() ? "von Marvin (Zeilen 17/18)" : "nicht bei Marvin (ohne 17/18)");
        }
    }
}
