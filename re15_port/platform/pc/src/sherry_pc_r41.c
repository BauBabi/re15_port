/*
 * RE1.5 Rebuilt - Runde 41 Spur K "stage3szenen_b" (PC): Spielfigur-Wechsel mit SKELETT, ANIMATION und WAFFEN-BANK.
 *
 * Original (selbst disassembliert, Belege include/re15_sherry_r41.h + Dossier K1.5): der Raumlader FUN_800396fc ruft
 * bei neuem Modell-Nibble @0x80039788 FUN_800314b0. Der bindet ALLE VIER Teile der PLD neu - EDD (+0x16c) @0x8003154c,
 * EMR (+0x84) @0x80031578, MD1 @0x800315a0, TIM @0x800315b0 - baut das Skelett aus dem neuen EMR auf
 * (jal 0x8001e56c @0x80031614, jal 0x8001e5b0 @0x80031624), setzt die Waffe (@0x80031690; Modell 0x0F -> 0
 * @0x800316c4), loescht die Wunden (FUN_80037c1c @0x800316c8), laedt die CORE-Klangbank des Modells
 * (FUN_800440c4 @0x800316e8), die Waffen-Bank FUN_80036b68 @0x800316f0 (Datei @0x800741e8[Modell] + Waffe;
 * W-EDD +0x174 @0x80036be4, W-EMR +0x170 @0x80036c04; Waffe 0 -> kein Hand-Mesh @0x80036c08) und die ARMS-Klangbank
 * der Waffe (jal 0x80043d8c @0x80031708).
 *
 * Port vorher: pc_sync_player_model (main.c) tauschte NUR MD1 + TIM.
 *
 * KOERPER bei JEDEM Neuladen (Nachbesserung 1, M3 - vorher nur bei neuer Waffen-Familie, mit der falschen Behauptung
 * "gleiche Familie = bytegleiche Teile"): EDD/EMR der neuen PLD werden gebunden, sobald sie sich vom gebundenen Koerper
 * unterscheiden (Bytevergleich gegen die PLD des gebundenen Modells). Bytegleich sind nur PL00/01/02 und PL04..07
 * (Ruestungs-Varianten); PL03, PL08..PL0E und PL0F weichen ab - im Spiel genutzt: Marvin 0x0D (ROOM3020 @0x00D7A
 * `24 10 0d 00`, PL0D EDD 3152 B / EMR 56976 B gegen PL00 3160 / 57136 B) und Sherry 0x0F. Die W-Banken sind ein
 * Verbund aus der Bindelage des Koerpers und den Keyframes der PLW (main.c [wpn-bank]); ein neuer Koerper bekommt
 * deshalb neue Verbund-Bindelagen, die Keyframes bleiben. Neue Waffen-FAMILIE (@0x800741e8[Modell]: 76/97/118): W-Banken
 * der Familie neu (Sherry: nur PL0FW00), die Boot-Familie wird beim Verlassen gesichert und beim Zurueckkehren
 * (3091 Ende -> Elza) wiederhergestellt.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_sherry_r41.h"
#include "sherry_pc_r41.h"
#include "asset_root_pc.h"

#define K_WPN_MAX 21

/* Waffen-Datei-Basis je Modell, u16 @0x800741e8 (Bytes 4c 00 x4, 61 00 x4, 4c 00 x7, 76 00): 76 = PL00W00,
 * 97 = PL04W00, 118 = PL0FW00 (CD-Dateitabelle @0x8006f440 Eintrag 75/96/117: 22960/22756/14696 B). */
static const uint16_t k_waffen_basis[16] = { 76,76,76,76, 97,97,97,97, 76,76,76,76, 76,76,76, 118 };

static re15_sherry_pc_r41_baenke_t s_b;
static int s_angemeldet = 0;
static int s_familie = -1, s_boot_familie = -1;
static int s_koerper_modell = -1;   /* Modell, dessen PLD-EDD/EMR gerade gebunden sind (Nachbesserung 1, M3) */

/* Sicherung der Boot-Familie (Elza bzw. Leon) */
static int s_gesichert = 0, s_sv_koerper_modell = -1;
static re15_emd_skeleton_t  s_sv_pl00_skel, s_sv_w01_skel;
static re15_emd_animation_t s_sv_pl00_anim, s_sv_w01_anim;
static re15_emd_animation_t s_sv_wpn_anim[K_WPN_MAX];
static re15_emd_skeleton_t  s_sv_wpn_skel[K_WPN_MAX];
static int s_sv_bank_ok[K_WPN_MAX], s_sv_md1_ok[K_WPN_MAX];
static uint8_t *s_sv_koerper_buf = NULL;

/* eigene Kopien (EDD/EMR aliasen sie) */
static uint8_t *s_koerper_buf = NULL;
static uint8_t *s_w00_buf = NULL;
static re15_emd_skeleton_t  s_fr_skel, s_w00_raw;
static re15_emd_animation_t s_fr_anim, s_w00_anim, s_an_tmp;

static const char *familie_praefix(int basis)
{
    return (basis == 97) ? "PL04" : (basis == 118) ? "PL0F" : "PL00";
}

void re15_sherry_pc_r41_anmelden(const re15_sherry_pc_r41_baenke_t *b, int modell)
{
    if (!b) return;
    s_b = *b;
    s_angemeldet = 1;
    if (s_b.wpn_max > K_WPN_MAX) s_b.wpn_max = K_WPN_MAX;
    s_familie = s_boot_familie = k_waffen_basis[modell & 15];
    s_koerper_modell = modell & 15;   /* main.c bootet den Koerper aus PL<Figur>.PLD (bytegleich PL00.EDD/.EMR) */
    s_gesichert = 0;
    if (s_sv_koerper_buf != s_koerper_buf) free(s_sv_koerper_buf);
    s_sv_koerper_buf = NULL;
    free(s_koerper_buf); s_koerper_buf = NULL;
    free(s_w00_buf);     s_w00_buf = NULL;
    re15_sherry_r41_modell_init(modell);
}

static void sichern(void)
{
    s_sv_pl00_skel = *s_b.pl00_skel; s_sv_pl00_anim = *s_b.pl00_anim;
    s_sv_w01_skel  = *s_b.w01_skel;  s_sv_w01_anim  = *s_b.w01_anim;
    for (int i = 0; i < s_b.wpn_max; i++) {
        s_sv_wpn_anim[i] = s_b.wpn_anim[i]; s_sv_wpn_skel[i] = s_b.wpn_skel[i];
        s_sv_bank_ok[i] = s_b.wpn_bank_ok[i]; s_sv_md1_ok[i] = s_b.wpn_md1_ok[i];
    }
    s_sv_koerper_modell = s_koerper_modell;
    s_sv_koerper_buf = s_koerper_buf;   /* ein frueher gebundener Fremd-Koerper derselben Familie bleibt am Leben */
    s_koerper_buf = NULL;
    s_gesichert = 1;
}

static void zurueck(void)
{
    *s_b.pl00_skel = s_sv_pl00_skel; *s_b.pl00_anim = s_sv_pl00_anim;
    *s_b.w01_skel  = s_sv_w01_skel;  *s_b.w01_anim  = s_sv_w01_anim;
    for (int i = 0; i < s_b.wpn_max; i++) {
        s_b.wpn_anim[i] = s_sv_wpn_anim[i]; s_b.wpn_skel[i] = s_sv_wpn_skel[i];
        s_b.wpn_bank_ok[i] = s_sv_bank_ok[i]; s_b.wpn_md1_ok[i] = s_sv_md1_ok[i];
    }
    free(s_koerper_buf); s_koerper_buf = s_sv_koerper_buf; s_sv_koerper_buf = NULL;
    free(s_w00_buf);     s_w00_buf = NULL;
    s_koerper_modell = s_sv_koerper_modell;
    s_gesichert = 0;
}

/* 1 = EDD und EMR der PLD `pld` sind bytegleich mit denen der PLD des gebundenen Modells. */
static int koerper_gleich(const uint8_t *pld, int pld_size)
{
    if (s_koerper_modell < 0) return 0;
    char nm[32]; snprintf(nm, sizeof nm, "PLD/PL%02X.PLD", (unsigned)s_koerper_modell);
    int asz = 0; uint8_t *alt = re15_pc_read_any(nm, &asz);
    if (!alt) return 0;
    int gleich = 1;
    for (int t = 0; t < 2 && gleich; t++) {
        unsigned long ao = 0, al = 0, no = 0, nlen = 0;
        int idx = (t == 0) ? RE15_PLD_EDD : RE15_PLD_EMR;
        if (!re15_pld_part(alt, (long)asz, idx, &ao, &al) || !re15_pld_part(pld, (long)pld_size, idx, &no, &nlen) ||
            al != nlen || memcmp(alt + ao, pld + no, (size_t)al) != 0) gleich = 0;
    }
    free(alt);
    return gleich;
}

/* Koerper (EDD -> +0x16c @0x8003154c, EMR -> +0x84 @0x80031578) aus der PLD in eine eigene Kopie parsen. */
static uint8_t *koerper_parsen(const uint8_t *pld, int pld_size, re15_emd_animation_t *an, re15_emd_skeleton_t *sk)
{
    unsigned long eo = 0, el = 0, mo = 0, ml = 0;
    if (!re15_pld_part(pld, (long)pld_size, RE15_PLD_EDD, &eo, &el) ||
        !re15_pld_part(pld, (long)pld_size, RE15_PLD_EMR, &mo, &ml)) return NULL;
    uint8_t *kb = (uint8_t *)malloc((size_t)pld_size);
    if (!kb) return NULL;
    memcpy(kb, pld, (size_t)pld_size);
    memset(sk, 0, sizeof *sk);
    if (re15_emd_parse_animation(kb + eo, (size_t)el, an) != 0 ||
        re15_emd_parse_skeleton (kb + mo, (size_t)ml, sk) != 0) { free(kb); return NULL; }
    return kb;
}

/* Verbund-Bindelage: Skelett des neuen Koerpers, Keyframe-Vorrat der Bank bleibt (main.c [wpn-bank]). */
static void verbund(re15_emd_skeleton_t *w, const re15_emd_skeleton_t *koerper)
{
    re15_emd_skeleton_t v = *koerper;
    v.keyframe_data       = w->keyframe_data;
    v.keyframe_data_size  = w->keyframe_data_size;
    v.keyframe_count      = w->keyframe_count;
    v.keyframe_size_bytes = w->keyframe_size_bytes;
    *w = v;
}

/* Neuer Koerper in DERSELBEN Waffen-Familie (z.B. Leon <-> Marvin 0x0D): EDD/EMR binden, W-Verbunde neu verankern. */
static int koerper_binden(int modell, const uint8_t *pld, int pld_size)
{
    re15_emd_animation_t an; re15_emd_skeleton_t sk;
    uint8_t *kb = koerper_parsen(pld, pld_size, &an, &sk);
    if (!kb) return 0;
    s_fr_anim = an; s_fr_skel = sk;
    *s_b.pl00_anim = s_fr_anim; *s_b.pl00_skel = s_fr_skel;
    for (int i = 0; i < s_b.wpn_max; i++) if (s_b.wpn_bank_ok[i]) verbund(&s_b.wpn_skel[i], &s_fr_skel);
    verbund(s_b.w01_skel, &s_fr_skel);
    free(s_koerper_buf); s_koerper_buf = kb;
    s_koerper_modell = modell & 15;
    return 1;
}

/* Koerper + W00 einer fremden Familie aus der PLD (und PL..W00.PLW) binden. 1 = gebunden. */
static int fremd_binden(int modell, int basis, const uint8_t *pld, int pld_size)
{
    re15_emd_skeleton_t sk;
    uint8_t *kb = koerper_parsen(pld, pld_size, &s_an_tmp, &sk);
    if (!kb) return 0;
    /* W00 der Familie (Waffe 0: Basis + 0, @0x80036b84-c4) */
    char nm[32]; snprintf(nm, sizeof nm, "PLD/%sW00.PLW", familie_praefix(basis));
    int wsz = 0; uint8_t *wb = re15_pc_read_any(nm, &wsz);
    unsigned long weo = 0, wel = 0, wmo = 0, wml = 0;
    re15_emd_skeleton_t wraw; memset(&wraw, 0, sizeof wraw);
    static re15_emd_animation_t s_wan_tmp;
    int w_ok = wb && re15_pld_part(wb, (long)wsz, RE15_PLD_EDD, &weo, &wel) &&
                     re15_pld_part(wb, (long)wsz, RE15_PLD_EMR, &wmo, &wml) &&
                     re15_emd_parse_animation(wb + weo, (size_t)wel, &s_wan_tmp) == 0 &&
                     re15_emd_parse_skeleton (wb + wmo, (size_t)wml, &wraw) == 0;
    if (!w_ok) { free(kb); free(wb); return 0; }
    free(s_koerper_buf); s_koerper_buf = kb;
    free(s_w00_buf);     s_w00_buf = wb;
    s_fr_anim = s_an_tmp; s_fr_skel = sk; s_w00_anim = s_wan_tmp; s_w00_raw = wraw;
    s_koerper_modell = modell & 15;
    /* Koerper: EDD -> +0x16c, EMR -> +0x84 (@0x8003154c/@0x80031578) */
    *s_b.pl00_anim = s_fr_anim; *s_b.pl00_skel = s_fr_skel;
    /* W-Bank 0: Bindelage des NEUEN Koerpers + Keyframes der W00 (Verbund wie main.c [wpn-bank]: die W-EMR traegt
     * eine andere Bindepose, nur ihr Keyframe-Vorrat wird uebernommen); EDD der W00 (+0x174 @0x80036be4). */
    re15_emd_skeleton_t w = s_w00_raw;
    verbund(&w, &s_fr_skel);
    for (int i = 0; i < s_b.wpn_max; i++) { s_b.wpn_bank_ok[i] = 0; s_b.wpn_md1_ok[i] = 0; }   /* @0x80036c08 */
    s_b.wpn_anim[0] = s_w00_anim; s_b.wpn_skel[0] = w; s_b.wpn_bank_ok[0] = 1;
    *s_b.w01_anim = s_w00_anim; *s_b.w01_skel = w;   /* Geh-Quelle (Fusstritt-Bits) + Rueckfall */
    return 1;
}

int re15_sherry_pc_r41_koerper(int modell, const uint8_t *pld, int pld_size)
{
    if (!s_angemeldet) return 0;
    int alt = re15_sherry_r41_modell();
    int figur = (alt >= 0) && !re15_sherry_r41_ruestungswechsel(alt, modell);
    int basis = k_waffen_basis[modell & 15];
    re15_sherry_r41_modell_setzen(modell);              /* Waffe/Wunden/HP/Status @0x80031680-@0x80031720 (Figurwechsel) */
    if (figur) {                                        /* CORE (FUN_800440c4 @0x800316e8), ARMS (FUN_80043d8c @0x80031708) */
        extern void re15_audio_prime_core(int idx);
        extern void re15_audio_prime_weapon(int weapon_id);
        extern int  re15_player_equipped_weapon(void);
        re15_audio_prime_core(modell & 15);
        re15_audio_prime_weapon(re15_player_equipped_weapon());
    }
    int ok;
    if (basis == s_familie) {
        /* gleiche W-Familie: Koerper nur, wenn EDD/EMR sich unterscheiden (Ruestungs-Varianten sind bytegleich) */
        if (koerper_gleich(pld, pld_size)) return 0;
        ok = koerper_binden(modell, pld, pld_size);
    } else {
        if (s_familie == s_boot_familie && !s_gesichert) sichern();
        if (basis == s_boot_familie && s_gesichert) {
            zurueck();
            ok = koerper_gleich(pld, pld_size) ? 1 : koerper_binden(modell, pld, pld_size);
        } else ok = fremd_binden(modell, basis, pld, pld_size);
    }
    if (!ok) {
        fprintf(stderr, "[sherry-r41] Modell %d: Koerper/W00 nicht bindbar - Skelett bleibt\n", modell);
        return 0;
    }
    *s_b.skel = *s_b.pl00_skel; *s_b.anim = *s_b.pl00_anim;   /* Raum-RBJ legt sich danach darueber (@0x80039a08) */
    s_familie = basis;
    fprintf(stderr, "[sherry-r41] Koerper -> Familie %d (%s): %d Knochen, %d Clips, W00 %d Clips, Wurzel-Bind y %d (Modell %d)\n",
            basis, familie_praefix(basis), s_b.pl00_skel->bone_count, s_b.pl00_anim->clip_count,
            s_b.wpn_anim[0].clip_count, (int)s_b.pl00_skel->bone_relative_pos[0][1], modell & 15);
    return 1;
}
