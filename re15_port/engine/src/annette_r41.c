/*
 * RE1.5 Rebuilt — Runde 41 Spur AN: Annette-Szene im hinteren Zugwagen ROOM5090 (Leon) und Zugstart im Fuehrerstand
 * ROOM6030 (Nachbesserung 1).
 *
 * Kopf, Herleitung und Abgrenzung: include/re15_annette_r41.h, Dossier analysis/befunde_runde41/AN_annette.md.
 *
 * ⛔ PORT-ERGAENZUNG nach Nutzerentscheid, KEIN Auslieferungscode. Vorbild der 5090-Szene ist die echte RE2-Szene
 * (Claire-Disc SLES-10972, PL1/RDT/ROOM6141.RDT MAIN sub02 @0x01024..@0x0118E, Emulator-Aufnahme mit Ton und 97
 * Savestates: analysis/befunde_runde41/AN_belege/re2_claire/zeitachse.md). Reihenfolge, Sleeps und Toene sind von dort
 * uebernommen (jede Zahl mit RE2-Adresse); Kameras, Lagen und Clips muessen auf RE1.5 5090 abgebildet werden und sind
 * PORT-WAHL (Begruendung je Konstante). Kein Asset-Patch: die RDTs bleiben byte-true, die Szenen sind Bytecode aus
 * ORIGINAL-Opcodes ueber die vorhandene VM.
 */
#include "re15_annette_r41.h"

#include <string.h>
#include <stdlib.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_damage.h"        /* re15_enemy_bone_world_pos */
#include "re15_esp.h"           /* re15_esp_fx_spawn_rows, re15_esp_room_bank, re15_esp_fx_count (Blut, RE2 FUN_80104C24) */
#include "re15_msg.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_engine.h"
#include "re15_math.h"         /* re15_squareroot0 */

#include "gen/annette_r41_texte.inc"   /* k_an_5090_msg40.. / k_an_6030_msg27.. (tools/r41_annette/texte_bauen.py) */

int re15_r38_birkin_anim(re15_actor_t *e);        /* enemy_ai_common.c: Clip-Vorschub mit der Bank des Typs */
int re15_r38_birkin_fc(const re15_actor_t *e);    /* enemy_ai_common.c: Clip-Laenge */
int re15_bb_bank_da(const re15_actor_t *e);       /* birkin_bahnsteig.c: 0x30 traegt die Riesenform */

/* ---- 5090: Lagen (PORT-WAHL, Dossier Nachbesserung 1 Abschnitt N1.2) -----------------------------------------------
 * RE2 ROOM6141: Annette geht auf die Stelle zu, an der William herabfaellt (@0x010A2/@0x010B8/@0x010DE), und bleibt
 * 4497 vor ihm stehen (Savestate e07: Annette (-11097,-15001), Birkin (-6600,-14800)); der Hieb kommt mit einem Vorstoss
 * von 1397 (e20..e22: -6600 -> -7997). Die RE1.5-Riesenform trifft mit dem Hieb links (Clip 4) nur im Trefferfenster
 * lokal x 3685..3864 (gemessen aus BAHNSTEIG_FORM.EMD, Dossier 8.2) und hat keinen Vorstoss; deshalb Abstand 3760.
 * Kameras (Projektion mit den RID-Daten 5090 @0x60+32*Cut, tools/form_kamera.py-Kette, Dossier N1.2):
 *   Cut 12 (Auge (18526,-2656,-24660), weit, Leon vorn)      = RE2 Cut 7 (beide im Bild, Krach, "William.")
 *   Cut 15 (Auge (46,-1236,-24302), Blick nach Osten)          = RE2 Cut 2 und 0x0B (von Williams Seite auf Annette)
 *   Cut 11 (Auge (7626,-3256,-25360), Blick nach Westen)       = RE2 Cut 0x0A (von Annettes Seite auf William)
 *   Cut 13 (Auge (11050,-3110,-24812), Blick nach Osten, Leon) = RE2 Cut 6 (weg von der Tat, Schrei aus dem Off)
 * Lagen per Projektion (Werkzeuge analysis/befunde_runde41/AN_belege/werkzeug/n1/proj5090.py, lage2.py, lagesuche.py; Dossier N1.2), gemessen
 * im ersten Fensterlauf korrigiert: mit L (1000,-22800) fuellte die Form in Cut 15 fast das ganze Bild und Annette war beim
 * Satz nur zwischen ihren Beinen zu sehen; mit A0 (9500,-22900) stand Annette in Cut 12 genau hinter Leon.
 *   L  Landestelle: in Cut 15 liegt die Form (Kasten +-1300, Hoehe 4000) links bis Bild-x 155, Annette (Hoehe 1750) rechts
 *      bei x 259..302 - keine Ueberdeckung; in Cut 11 steht Annette vorn (x 170..194), die Form dahinter (x 126..195).
 *   S  Haltepunkt 3759 vor L (Trefferfenster s. oben).
 *   A0 Start in Cut 12 bei Bild-x 117 neben Leon (x 156, Ausloeser-Lage (13775,-23550), debug.log Runde 41), mindestens 1600
 *      von den Bodenfeuern main00 @0x0110E..@0x0120E (naechstes (9900,-24500)), der Weg A0->S bleibt >= 1035 von ihnen.
 * Wegpunkte W1/W2 teilen A0->S wie RE2 seine drei Plc_dest-Strecken (8040/5016/4000 = 47 % / 76 %). */
#define AN_A0_X            8000
#define AN_A0_Z          (-24250)
#define AN_W1_X            6182
#define AN_W1_Z          (-23932)
#define AN_W2_X            5060
#define AN_W2_Z          (-23735)
#define AN_S_X             4131
#define AN_S_Z           (-23573)
#define AN_L_X              500
#define AN_L_Z           (-22600)
/* Sturz der Riesenform vom Dach = ROOM5080 sub03 (Original): @0x00822 Pos_set y -5600 `20 ea`, @0x0082A Sleep 1,
 * @0x0082E Speed_set(1,400) `2f 01 90 01`, @0x00832 For 14 `0d 00 06 00 0e 00` { Add_speed `30`, Evt_next } -> y 0.
 * Im Takt statt im Programm: das Programm muss wie RE2 (gosub sub10 @0x010E6) waehrend des Sturzes auf Annettes Ankunft
 * warten, und deren Bit (5,0x21) ist ein Ein-Bild-Puls (Bank 5 Wort 1 wird jedes Bild gewischt, scd_vm.c FUN_8003ebf4);
 * eine blockierende For-Schleife verpasste ihn (gemessen: Puls F175, Schleife bis F176). */
#define AN_STURZ_Y       (-5600)
#define AN_STURZ_HALT        1
#define AN_STURZ_V          400
#define AN_STURZ_N           14
/* Liegen: RE2 @0x01162 setzt Annette von ihrem Standort (-11097,-15001) auf (-12300,-15300) = 1203 von William weg,
 * 299 seitlich (Savestate e23). Port: dieselbe Strecke von der Form weg; Pose wie bisher (Clip 6 Bild 12, gekippt). */
#define AN_LIEGEN_WEG      1203
#define AN_LIEGEN_SEIT    (-299)
/* Clips der Riesenform (Stufe 3, BB_C_* in birkin_bahnsteig.c) */
#define AN_FC_DROHEN         10     /* Drohen; Bild 45-52 = Pose Bild 2 = 80 % #145 (STUFE3.md Abschnitt 3) */
#define AN_FC_STEHEN          2
#define AN_FC_HIEB            4     /* Hieb links, Teil 13 */
#define AN_HIEB_TREFFER      14     /* erstes Trefferbild, EDD-Bit 16 Bild 14-17 (G4-Clip 4, STUFE3.md Abschnitt 3) */
#define AN_HAND_LINKS        13
#define AN_TON_BIT   0x08000000u    /* Bild-Ton: Bit 27, Id = Wort >> 28 (FUN_80016028, birkin_bahnsteig.c bb_bild_ton) */
#define AN_TON_BANK          24     /* G4-Bank, wie bb_se */
/* Clips der Frau (0x4D-Bank, 24 Clips, Kontaktblatt AN_belege). RE2 laesst Annette waehrend des Hiebs Clip 0x02 ihrer
 * Bank spielen (`3f 02 02 44` @0x01138, Savestates e20..e22 M02 Bild 6/26/46 synchron zu Birkins M07). Naechster
 * 0x4D-Clip: 4 = Zurueckweichen mit erhobenen Armen (PORT-WAHL, an Ort und Stelle). */
#define AN_FA_REAKTION        4
#define AN_FA_STURZ           6     /* faellt zu Boden (Bild 12 = am Boden) */
#define AN_FA_STURZ_BODEN    12
#define AN_KIPP_WINKEL     (-683)   /* 60 Grad nach hinten (Kontaktblatt liegen.png) */
#define AN_KIPP_HEBEN      (-480)   /* Clip 6 Bild 12 um -60 Grad gekippt reicht bis y +482 unter den Ursprung (Gerippe gemessen) */
/* Blut (Nachbesserung 2): der Emitter der RE2-Szene selbst. Claire-Disc SLES-10972, Birkin-Overlay (Typ 0x33, im RAM
 * @0x80100000, Savestate e22 aus analysis/befunde_runde41/AN_belege/re2_claire/), Routinen-Tabelle @0x80106584: [2] =
 * FUN_80104C24 (Member_set 0x17=2 @0x01142 -> Routine 2 @0x80104BE4-F8). Dort, NACH dem Bildvorschub @0x80104C9C-A8:
 *   @0x80104CD4-E8  Bildwort & 0x10000 (EDD-Trefferbit): `lw v0,0x178(s0)` `lw v0,0(v0)` `lui v1,1` `and v0,v0,v1`
 *   @0x80104CF0-F8  nur einmal (+7): `lbu v0,7(s0)` / `bnez`; @0x80104D04-0C `sb v0,7(s0)`, `jal 0x80057690` (a0 = 3)
 *   @0x80104D10-14  `addiu v0,zero,0xf` / `sb v0,0x16b(s0)`             -> 15 Bilder Blut, ab diesem Bild
 *   @0x80104D18-60  solange +0x16B: `jal 0x801058bc` (e, [0x80106330]+1, 1) @0x80104D38 und (e, [0x80106330]+3, 1)
 *                   @0x80104D4C, dann +0x16B - 1; [0x80106330] = 13 (RAM e22) -> Teile 14 und 16
 *   FUN_801058BC    Beschreiber 0x801065B8 + 6 * 1 = `00 01 00 00 00 20` (@0x801058D0-E4, Packung @0x80105994-B4) =
 *                   Effekt 0, sub 1, Skala 0x2000; Bit 0x80 = 0 -> Anker Einheitsmatrix 0x80095F68 (T = 0), a3 = Weltlage
 *                   des Teils (+0x5C/+0x60/+0x64 @0x80105950-78) = Blut bleibt, wo die Klaue in DIESEM Bild war;
 *                   Winkel a1 = FUN_80014E4C(e.x, e.z, Spieler.x, Spieler.z) @0x801058F0-908 (= Leon FUN_800154AC)
 *   FUN_8001B910    (= Leon FUN_8001BF10, Pool 96 @0x8001B910 `addiu t2,zero,0x60`): 3 Zeilen je Aufruf.
 * Gemessen (RAM e22, 71,54 s): 60 Plaetze 0/1/0x2000 in 10 Gruppen zu 6 (2 Teile x 3 Zeilen), Gruppen eines Bildes
 * wandern mit der Klaue; e23 (6 Spielbilder spaeter) noch 4 Gruppen. RAM e22: Teile 14/15/16 in einer Reihe (je ~195
 * auseinander, je ~600 von Teil 13) = die drei Klauen der Hand 13, 14 und 16 die aeusseren.
 * Port: dieselbe Kette auf dem vorhandenen RE2->RE1.5-Dekoder (enemy_ai_re2_zombie.c re2z_gore_fx_core: RE2-Effekt 0 ->
 * RE1.5-Blut 0, sub und Skala durchgereicht, re15_esp_fx_spawn_rows). Teile -> Knochen der Riesenform: die linke Hand 13
 * (Hieb links, AN_HAND_LINKS) traegt in BAHNSTEIG_FORM.EMD die Klauen 14 (729,30,-228), 15 (782,30,83), 28 (684,30,258)
 * und den Daumen 30 -> die aeusseren 14 und 28 (PORT-WAHL der Zuordnung, Form ist kein RE2-Modell). Die 8 Spritzer
 * vorher (4 an Annette im Trefferbild, je 2 nach 6/12 Bildern) hatten fuer Lage und Zeitpunkt keinen Beleg: entfallen. */
#define AN_BLUT_BILDER       15     /* @0x80104D10 `addiu v0,zero,0xf` */
#define AN_BLUT_KLAUE_A      14     /* RE2 Teil [0x80106330]+1 = 14 (@0x80104D2C `addiu a1,s1,1`) */
#define AN_BLUT_KLAUE_B      28     /* RE2 Teil [0x80106330]+3 = 16 (@0x80104D44 `addiu a1,s1,3`) */
#define AN_BLUT_EFFEKT        0     /* 0x801065BE `00` (Bit 0x80 = 0: Einheitsanker) */
#define AN_BLUT_SUB           1     /* 0x801065BF `01` + u16 0x801065C0 `00 00` */
#define AN_BLUT_SKALA    0x2000     /* u16 0x801065C2 `00 20` */

/* ---- 5090: Programm --------------------------------------------------------------------------------------------------
 * Opcode-Formen mit Vorbild (Laengen = s_opcode_sizes, scd_vm.c):
 *   Set `22 bb ii vv`               ROOM5090 sub04 @0x012F2/@0x012F6 (Sperre (2,7), Balken (1,27))
 *   Cut_chg `29 cc`                 ROOM5090 sub04 @0x0130E
 *   Work_set `2e kk ii` + Nop       ROOM5090 sub04 @0x0131C
 *   Plc_dest `40 00 09 20 x z`      ROOM5090 sub04 @0x01320 (Modus 9 = drehen, Ankunftsbit 0x20)
 *   Plc_dest `40 00 09 21 x z`      ROOM6030 sub08 @0x01160 (Annette dreht sich, Ankunftsbit 0x21)
 *   Plc_dest `40 00 04 21 x z`      ROOM6000 sub02 @0x00FB0 (Annette geht, Modus 4) mit dem Bit 0x21 von oben
 *   Ankunfts-Poll `11 00 08 00 02 00 12 04 21 05 bb 00`   ROOM5090 sub04 @0x01328..@0x01330 / ROOM6030 @0x0128A
 *   Sleep `09 0a nn nn`             ROOM5090 sub04 @0x0133E
 *   Message_on `2b id 00 00`        ROOM6030 sub02 @0x00DC2 (Maske 0 = Untertitel)
 *   Evt_exec `04 ff 18 ss`          ROOM5090 sub01 @0x01296
 * Bildzahlen = RE2-Skript (ROOM6141 sub02), XA-Laengen aus der Claire-STAGE6.BIN-Tabelle @0x4D4 (Laenge in
 * Kanalsektoren * 4032 / 37800 s, je 30 Bilder/s): Xa 0x30 11 Sektoren = 1,173 s = 35 Bilder; Xa 0x31 37 = 3,947 s =
 * 118 Bilder; Xa 0x32 13 = 1,387 s = 42 Bilder. RE2 wartet nach jedem Xa mit Wsleep bis zum Ende der Stimme. */
#define LE16(v)          (uint8_t)((uint16_t)(v) & 0xff), (uint8_t)(((uint16_t)(v) >> 8) & 0xff)
#define OP_SET(b,i,v)    0x22, (b), (i), (v)
#define OP_CUT(c)        0x29, (c)
#define OP_WORK(k,i)     0x2e, (k), (i), 0x00
#define OP_DREHEN(x,z)   0x40, 0x00, 0x09, 0x20, LE16(x), LE16(z)
#define OP_DREHEN_AN(x,z) 0x40, 0x00, 0x09, 0x21, LE16(x), LE16(z)
#define OP_GEHEN_AN(x,z) 0x40, 0x00, 0x04, 0x21, LE16(x), LE16(z)
#define OP_WARTE(bit)    0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, (bit), 0x00
#define OP_SLEEP(n)      0x09, 0x0a, LE16(n)
#define OP_MSG(id)       0x2b, (id), 0x00, 0x00
#define OP_KAMPF         0x04, 0xff, 0x18, RE15_AN_SUB_KAMPF
#define OP_END           0x01, 0x00

static const uint8_t k_prog[] = {
    OP_SET(2, 7, 1), OP_SET(1, 27, 1),          /* Sperre + Balken (5090 sub04 @0x012F2/@0x012F6 = RE2 @0x00E32/@0x00E36) */
                                                /* Cut 12 bleibt (RE2 Cut 7 @0x01024); Krach im Haken (RE2 @0x01040) */
    OP_SLEEP(25),                               /* RE2 @0x0104C `0a 19 00` */
    OP_WORK(1, 0), OP_DREHEN(AN_L_X, AN_L_Z), OP_WARTE(0x20),        /* beide sehen hin (RE2 Plc_neck @0x0105A/@0x01068) */
    OP_WORK(2, 0), OP_DREHEN_AN(AN_L_X, AN_L_Z), OP_WARTE(0x21),
    OP_SLEEP(36), OP_SLEEP(1),                  /* RE2 @0x01072 `0a 24 00`, @0x01076 `0a 01 00` */
    OP_MSG(RE15_AN_MSG_WILLIAM),                /* "William." RE2 Xa 0x30 @0x0107A */
    OP_SLEEP(35),                               /* RE2 Wsleep @0x01084 bis Xa-Ende (11 Sektoren) */
    OP_WORK(2, 0), OP_GEHEN_AN(AN_W1_X, AN_W1_Z), OP_WARTE(0x21),   /* RE2 Strecke 1 @0x010A2 + gosub sub10 @0x010AE */
    OP_CUT(15),                                 /* RE2 Cut 2 @0x010B2 */
    OP_GEHEN_AN(AN_W2_X, AN_W2_Z), OP_WARTE(0x21),                  /* RE2 Strecke 2 @0x010B8 + gosub @0x010C4 */
    OP_SET(5, RE15_AN_SIG_STURZ, 1),            /* RE2 @0x010CA Member_set 0x17=1 (Birkin faellt), @0x010CE Se_on(2,0x0A) */
    OP_GEHEN_AN(AN_S_X, AN_S_Z),                /* RE2 Strecke 3 @0x010DE; der Sturz (Takt, 5080 sub03) laeuft dabei (RE2 e06) */
    OP_WARTE(0x21),                             /* RE2 gosub sub10 @0x010E6 */
    OP_SLEEP(30),                               /* RE2 @0x010FE `0a 1e 00` */
    OP_CUT(11), OP_SET(5, RE15_AN_SIG_BRUELL, 1),   /* RE2 Cut 0x0A @0x01102 + Se_on(2,0x0C) @0x01104 */
    OP_SLEEP(60),                               /* RE2 @0x01110 `0a 3c 00` */
    OP_CUT(15), OP_SLEEP(1),                    /* RE2 Cut 0x0B @0x01114, @0x01116 `0a 01 00` */
    OP_MSG(RE15_AN_MSG_SATZ),                   /* "W-William! You're alive!" RE2 Xa 0x31 @0x0111A */
    OP_SLEEP(118),                              /* RE2 Sleep 30 @0x01132 + Wsleep @0x01136 bis Xa-Ende (37 Sektoren) */
    OP_SET(5, RE15_AN_SIG_SCHLAG, 1),           /* RE2 @0x01138 Annette Clip 2, @0x01142 Member_set 0x17=2 (Hieb) */
    OP_CUT(11),                                 /* RE2 Cut 0x0A @0x0113C */
    OP_SLEEP(45),                               /* RE2 @0x01146 `0a 2d 00` */
    OP_CUT(13), OP_SLEEP(1),                    /* RE2 Cut 6 @0x0114A (weg von der Tat), @0x0114C `0a 01 00` */
    OP_SET(5, RE15_AN_SIG_LIEGT, 1),            /* RE2 Xa 0x32 @0x01150 (Schrei, nur Ton) + Pos_set liegend @0x01162 */
    OP_SLEEP(42),                               /* RE2 Wsleep @0x01184 bis Xa-Ende (13 Sektoren) */
    OP_MSG(RE15_AN_MSG_LEON),                   /* "Leon: Annette!" (PORT-WAHL, RE2 gibt hier die Steuerung zurueck) */
    OP_SLEEP(60),
    OP_KAMPF,                                   /* Kampfstart wie bisher (sub04) */
    OP_END
};

/* ---- 6030: Zugstart nach dem Tor (Nutzer-Idee, Wortlaut PORT-WAHL) -----------------------------------------------
 * Das Programm besteht aus ausgelieferten Byte-Bereichen von ROOM6030 (rdt != 0, der Riegel unit_r41_annette vergleicht
 * jeden gegen die RDT) und wenigen Port-Befehlen in Original-Form (rdt == 0, Vorbild im Kommentar). Gesten der neuen Zeilen
 * = Gestenbloecke desselben Sprechers aus sub08 (wie Spur ST80-T, analysis/story80/TEXTE_UMSETZUNG.md Abschnitt 4). */
/* Abgang durch die Wagentuer Slot 0 (main00 @0x00C32, rect x -5100..-4200, z -24200..-22500). Die Tuer liegt in der Wand
 * SCA #13 (Rechteck x -5941..-4341, z -27900..-19610, ROOM6030 SCA @0x708): erreichbar ist die Wandflaeche x -3873 = Ziel
 * der Tuer Slot 1 (@0x00C52 `df f0`, Leons Eintritt). z -22550 = Nordteil des Tuerrechtecks, 937 von Leons Eintritt
 * (-3873,-23487), also ausserhalb der zwei Koerperradien (Fensterlauf: Stopp bei Abstand 902). PORT-WAHL. */
#define Z_TUER_X          (-3873)
#define Z_TUER_Z          (-22550)

typedef struct { const uint8_t *b; uint16_t n; uint16_t rdt; } an_seg_t;

static const uint8_t z_bit[]      = { 0x22, RE15_AN_ZUG_BANK, RE15_AN_ZUG_BIT, 0x01 };       /* Set-Form sub08 @0x0113A */
static const uint8_t z_sperre[]   = { 0x22, 0x02, 0x07, 0x01, 0x22, 0x01, 0x1b, 0x01 };     /* @0x0113E */
static const uint8_t z_emset[]    = { 0x44, 0x00, 0x4d, 0x40, 0x00, 0x00, 0x00, 0xff, 0x02, 0xf9,
                                      0x00, 0x00, 0x00, 0xa9, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00 }; /* sub07 @0x010F8 */
static const uint8_t z_kopf[]     = { 0x09, 0x0a, 0x01, 0x00, 0x29, 0x01, 0x2e, 0x01, 0x00, 0x00, 0x40, 0x00, 0x09, 0x20,
                                      0x02, 0xf9, 0x00, 0xa9, 0x09, 0x0a, 0x0a, 0x00 };   /* @0x01146: Sleep 1, Cut 1, Leon dreht sich */
static const uint8_t z_work_an[]  = { 0x2e, 0x02, 0x00, 0x00 };                             /* @0x011CC Work_set(2,0) */
static const uint8_t z_pult[]     = { 0x29, 0x03, 0x3f, 0x00, 0x12, 0x00, 0x09, 0x0a, 0x50, 0x00 };   /* @0x011F6 Cut 3, Pult, Sleep 80 */
static const uint8_t z_sig_an[]   = { 0x22, 0x05, RE15_AN_SIG_ZUG_AN, 0x01 };              /* Port: Ton Zug an (Set-Form) */
/* Pult: sub08 @0x01210..@0x01223 wechselt Cut 4 (Pult dunkel) und Cut 5 (rotes WARNING) = der gescheiterte Start "Not enough
 * power!". Fuer den gelungenen Start nur Cut 4 (PORT-WAHL, Fensterlauf: Cut 5 zeigt WARNING): Set(1,28,1) + Cut 4 + Sleep 15
 * @0x01210, Sleep 15 @0x01216, Set(1,28,0) @0x01220 - Original-Bytes ohne `29 05`. */
static const uint8_t z_licht[]    = { 0x22, 0x01, 0x1c, 0x01, 0x29, 0x04, 0x09, 0x0a, 0x0f, 0x00 };   /* @0x01210 */
static const uint8_t z_schlaf15[] = { 0x09, 0x0a, 0x0f, 0x00 };                                       /* @0x01216 */
static const uint8_t z_licht_aus[]= { 0x22, 0x01, 0x1c, 0x00 };                                       /* @0x01220 */
static const uint8_t z_cut1[]     = { 0x29, 0x01, 0x09, 0x0a, 0x3c, 0x00 };                 /* @0x01254 Cut 1, Sleep 60 */
static const uint8_t z_fahrt[]    = { 0x04, 0xff, 0x18, 0x05 };                             /* sub07 @0x01132 Licht-Schleife */
static const uint8_t z_m27[]      = { 0x2b, RE15_AN_MSG_FAEHRT, 0x00, 0x00 };              /* Form @0x01296 */
static const uint8_t z_geste16[]  = { 0x3f, 0x00, 0x09, 0x00, 0x09, 0x0a, 0x19, 0x00, 0x3f, 0x00, 0x09, 0x00, 0x43, 0x00,
                                      0x80, 0x00, 0x09, 0x0a, 0x19, 0x00, 0x3f, 0x00, 0x0b, 0x00, 0x09, 0x0a, 0x32, 0x00,
                                      0x3f, 0x00, 0x10, 0x00, 0x09, 0x0a, 0x14, 0x00 };   /* @0x0129A Annette nach msg 16 */
static const uint8_t z_sig_krach[]= { 0x22, 0x05, RE15_AN_SIG_KRACH, 0x01 };               /* Port: Krach (RE2 @0x01040) */
static const uint8_t z_schlaf25[] = { 0x09, 0x0a, 0x19, 0x00 };                             /* RE2 @0x0104C `0a 19 00` */
static const uint8_t z_work_leon[]= { 0x2e, 0x01, 0x00, 0x00 };                             /* @0x0125A Work_set(1,0) */
static const uint8_t z_dreh_leon[]= { 0x40, 0x00, 0x09, 0x20, LE16(Z_TUER_X), LE16(Z_TUER_Z) };   /* Form @0x01150 */
static const uint8_t z_warte20[]  = { 0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, 0x20, 0x00 }; /* @0x01358 */
static const uint8_t z_dreh_an[]  = { 0x40, 0x00, 0x09, 0x21, LE16(Z_TUER_X), LE16(Z_TUER_Z) };   /* Form @0x01160 */
static const uint8_t z_warte21[]  = { 0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, 0x21, 0x00 }; /* @0x0128A */
static const uint8_t z_schlaf36[] = { 0x09, 0x0a, 0x24, 0x00 };                             /* RE2 @0x01072 `0a 24 00` */
static const uint8_t z_m28[]      = { 0x2b, RE15_AN_MSG_WAS, 0x00, 0x00 };
static const uint8_t z_geste11[]  = { 0x3f, 0x00, 0x0d, 0x00, 0x09, 0x0a, 0x32, 0x00, 0x3f, 0x00, 0x0d, 0x00, 0x43, 0x00,
                                      0x80, 0x00, 0x09, 0x0a, 0x32, 0x00, 0x3f, 0x00, 0x10, 0x00, 0x09, 0x0a, 0x14, 0x00 }; /* @0x01178 */
static const uint8_t z_m29[]      = { 0x2b, RE15_AN_MSG_PRUEFE, 0x00, 0x00 };
static const uint8_t z_geste15[]  = { 0x3f, 0x00, 0x04, 0x00, 0x09, 0x0a, 0x32, 0x00, 0x3f, 0x00, 0x04, 0x00, 0x43, 0x00,
                                      0x80, 0x00, 0x09, 0x0a, 0x32, 0x00, 0x3f, 0x00, 0x07, 0x00, 0x09, 0x0a, 0x14, 0x00 }; /* @0x01262 Leon */
static const uint8_t z_work_an2[] = { 0x2e, 0x02, 0x00, 0x00 };                             /* @0x012E2 Work_set(2,0) */
static const uint8_t z_m30[]      = { 0x2b, RE15_AN_MSG_BLEIB, 0x00, 0x00 };
static const uint8_t z_geste18[]  = { 0x3f, 0x00, 0x0e, 0x00, 0x09, 0x0a, 0x32, 0x00, 0x3f, 0x00, 0x0e, 0x00, 0x43, 0x00,
                                      0x80, 0x00, 0x09, 0x0a, 0x32, 0x00, 0x3f, 0x00, 0x10, 0x00, 0x09, 0x0a, 0x14, 0x00 }; /* @0x012EA */
static const uint8_t z_gehen[]    = { 0x40, 0x00, 0x04, 0x21, LE16(Z_TUER_X), LE16(Z_TUER_Z) };   /* Form 6000 sub02 @0x00FFA */
static const uint8_t z_weg[]      = { 0x32, 0x00, 0xf5, 0xd6, 0xf0, 0xf1, 0xc6, 0x21 };     /* 6000 sub02 @0x0100A (versteckt) */
static const uint8_t z_schluss[]  = { 0x09, 0x0a, 0x1e, 0x00, 0x22, 0x02, 0x07, 0x00, 0x22, 0x01, 0x1b, 0x00, 0x3c, 0x01,
                                      0x42, 0x00 };                                       /* @0x01364 Sleep 30, Sperren auf, Cut_auto, Plc_ret */
static const uint8_t z_ende[]     = { 0x01, 0x00 };                                         /* @0x01374 Evt_end */

#define SEG(a, off) { a, (uint16_t)sizeof a, (uint16_t)(off) }
static const an_seg_t k_zug_seg[] = {
    SEG(z_bit, 0),          SEG(z_sperre, 0x0113E), SEG(z_emset, 0x010F8),  SEG(z_kopf, 0x01146),
    SEG(z_work_an, 0x011CC), SEG(z_pult, 0x011F6),   SEG(z_sig_an, 0),       SEG(z_licht, 0x01210),
    SEG(z_schlaf15, 0x01216), SEG(z_licht_aus, 0x01220),
    SEG(z_cut1, 0x01254),    SEG(z_fahrt, 0x01132),  SEG(z_work_an2, 0x012E2), SEG(z_m27, 0),
    SEG(z_geste16, 0x0129A), SEG(z_sig_krach, 0),    SEG(z_schlaf25, 0),
    SEG(z_work_leon, 0x0125A), SEG(z_dreh_leon, 0),  SEG(z_warte20, 0x01358),
    SEG(z_work_an2, 0x012E2), SEG(z_dreh_an, 0),     SEG(z_warte21, 0x0128A), SEG(z_schlaf36, 0),
    SEG(z_m28, 0),           SEG(z_geste11, 0x01178),
    SEG(z_work_leon, 0x0125A), SEG(z_m29, 0),        SEG(z_geste15, 0x01262),
    SEG(z_work_an2, 0x012E2), SEG(z_m30, 0),         SEG(z_geste18, 0x012EA),
    SEG(z_gehen, 0),         SEG(z_warte21, 0x0128A), SEG(z_weg, 0),
    SEG(z_work_leon, 0x0125A), SEG(z_schluss, 0x01364), SEG(z_ende, 0x01374),
};
#define AN_N_ZUG_SEG ((int)(sizeof k_zug_seg / sizeof k_zug_seg[0]))

/* Wagen-Eintritt (sub06 @0x010C4 Evt_exec sub05) solange der Zugstart nicht lief = der Zug steht noch. Beide Wagentueren
 * wie im Original vor dem Start, also wie im (3,89)=0-Zweig von sub06:
 *   Slot 2 (Ausgang zur Bahnsteighalle ROOM6000) ist wieder die Tuer aus main00 @0x00C72 (Door_aot_set, 32 Byte). Der
 *     (3,89)=1-Zweig hat sie davor mit @0x01098 `46 02 01 31 14 00` auf msg 20 "You can't get off the train right now."
 *     gestellt; der (3,89)=0-Zweig (@0x0105C..@0x01088) fasst Slot 2 nicht an. Nachbesserung 2 (Orchestrator-Entscheid):
 *     msg 20 erst, wenn der Zug faehrt - nach dem Zugstart laeuft sub06 unveraendert und @0x01098 gilt.
 *   Slot 3 (nach 5090) zeigt den Original-Text msg 10 "I have to get this thing moving first!" = sub06 @0x0106A.
 * Die Licht-Schleife sub05 ("Fahrt") startet erst nach dem Zugstart. */
static const uint8_t k_tuer[] = { 0x3b, 0x02, 0x02, 0x31, 0x00, 0x00, 0xf4, 0xb6, 0xc0, 0xae, 0xdc, 0x05, 0xb0, 0x04,
                                  0x9d, 0x06, 0xf0, 0xf1, 0xba, 0xd8, 0x00, 0x0c, 0x05, 0x00, 0x03, 0x02, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00,                                      /* main00 @0x00C72 */
                                  0x46, 0x03, 0x01, 0x31, 0x0a, 0x00, 0xff, 0xff, 0x00, 0x00,   /* sub06 @0x0106A */
                                  0x01, 0x00 };                                                /* sub06 @0x010CA */

static uint8_t s_zug_prog[512];
static int     s_zug_len = 0;

static void zug_bauen(void)
{
    if (s_zug_len) return;
    int n = 0;
    for (int i = 0; i < AN_N_ZUG_SEG; i++) {
        if (n + k_zug_seg[i].n > (int)sizeof s_zug_prog) { n = 0; break; }
        memcpy(s_zug_prog + n, k_zug_seg[i].b, k_zug_seg[i].n);
        n += k_zug_seg[i].n;
    }
    s_zug_len = n;
}

/* ---- Nachrichten -------------------------------------------------------------------------------------------------- */
static const struct { uint16_t raum; uint8_t id; const uint8_t *b; uint8_t n; } k_meldungen[] = {
    { 0x5090, RE15_AN_MSG_SATZ,    k_an_5090_msg40, (uint8_t)sizeof k_an_5090_msg40 },
    { 0x5090, RE15_AN_MSG_LEON,    k_an_5090_msg42, (uint8_t)sizeof k_an_5090_msg42 },
    { 0x5090, RE15_AN_MSG_WILLIAM, k_an_5090_msg43, (uint8_t)sizeof k_an_5090_msg43 },
    { 0x6030, RE15_AN_MSG_FAEHRT,  k_an_6030_msg27, (uint8_t)sizeof k_an_6030_msg27 },
    { 0x6030, RE15_AN_MSG_WAS,     k_an_6030_msg28, (uint8_t)sizeof k_an_6030_msg28 },
    { 0x6030, RE15_AN_MSG_PRUEFE,  k_an_6030_msg29, (uint8_t)sizeof k_an_6030_msg29 },
    { 0x6030, RE15_AN_MSG_BLEIB,   k_an_6030_msg30, (uint8_t)sizeof k_an_6030_msg30 },
};
#define AN_N_MELDUNGEN ((int)(sizeof k_meldungen / sizeof k_meldungen[0]))

/* ---- Zustand ------------------------------------------------------------------------------------------------------ */
static struct {
    int phase, t;
    int form, frau;               /* Slots, -1 = keiner */
    int luft;                     /* Add_speed-Schritte des Sturzes (5080 sub03 For 14) */
    int blut;                     /* Blut-Aufrufe FUN_801058BC (Testzaehler) */
    int blut_plaetze;             /* davon belegte ESP-Plaetze (Zeilen) */
    int blut_rest;                /* +0x16B: Blut-Bilder uebrig (@0x80104D10..60) */
    int blut_textur;              /* Textur mit Blut geladen */
    int treffer;                  /* Hieb hat getroffen */
    int zug;                      /* Zugstart-Programme seit Raumeintritt */
    int toene[RE15_AN_SE_N];
    int schrei;
    unsigned raum;
} s_an = { RE15_AN_AUS, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, { 0 }, 0, 0 };

static void (*s_modell_fn)(uint8_t) = 0;
static void (*s_se_fn)(int, int) = 0;
static void (*s_bank_fn)(int) = 0;
static void (*s_satz_fn)(int) = 0;
static void (*s_stimme_fn)(uint16_t, int) = 0;
void re15_annette_r41_modell_hook(void (*fn)(uint8_t type)) { s_modell_fn = fn; }
void re15_annette_r41_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int)) { s_se_fn = se_fn; s_bank_fn = bank_fn; }
void re15_annette_r41_se_hook(void (*fn)(int satz)) { s_satz_fn = fn; }
void re15_annette_r41_stimme_hook(void (*fn)(uint16_t room, int voice_id)) { s_stimme_fn = fn; }

static void ton(int satz)
{
    if (satz < 0 || satz >= RE15_AN_SE_N) return;
    s_an.toene[satz]++;
    if (s_satz_fn) s_satz_fn(satz);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[annette] F%u Ton %d (ANNETTE_SE.VBS)\n", (unsigned)g_engine.frame_count, satz);
#endif
}

void re15_annette_r41_reset(void)
{
    s_an.phase = RE15_AN_AUS; s_an.t = 0; s_an.form = s_an.frau = -1;
    s_an.luft = 0; s_an.blut = 0; s_an.blut_plaetze = 0; s_an.blut_rest = 0; s_an.blut_textur = 0; s_an.treffer = 0;
    s_an.zug = 0; memset(s_an.toene, 0, sizeof s_an.toene); s_an.schrei = 0;
}

int re15_annette_r41_phase(void) { return s_an.phase; }
int re15_annette_r41_slot(int form) { return form ? s_an.form : s_an.frau; }
int re15_annette_r41_blut(void) { return s_an.blut; }
int re15_annette_r41_blut_plaetze(void) { return s_an.blut_plaetze; }
int re15_annette_r41_zug_szene(void) { return s_an.zug; }
int re15_annette_r41_ton(int satz) { return (satz >= 0 && satz < RE15_AN_SE_N) ? s_an.toene[satz] : 0; }
int re15_annette_r41_schrei(void) { return s_an.schrei; }

const uint8_t *re15_annette_r41_programm(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_prog;
    return k_prog;
}

const uint8_t *re15_annette_r41_programm_zug(int *out_len)
{
    zug_bauen();
    if (out_len) *out_len = s_zug_len;
    return s_zug_len ? s_zug_prog : NULL;
}

const uint8_t *re15_annette_r41_programm_tuer(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_tuer;
    return k_tuer;
}

/* Fuer den Riegel: Segment i des Zugstart-Programms (rdt = Datei-Offset in ROOM6030.RDT, 0 = Port-Befehl). */
int re15_annette_r41_zug_segment(int i, const uint8_t **b, int *n, int *rdt);
int re15_annette_r41_zug_segment(int i, const uint8_t **b, int *n, int *rdt)
{
    if (i < 0 || i >= AN_N_ZUG_SEG) return 0;
    if (b) *b = k_zug_seg[i].b;
    if (n) *n = k_zug_seg[i].n;
    if (rdt) *rdt = k_zug_seg[i].rdt;
    return 1;
}

const uint8_t *re15_annette_r41_meldung_raum(uint16_t room_id, int msg_id, int *out_len)
{
    for (int i = 0; i < AN_N_MELDUNGEN; i++)
        if (k_meldungen[i].raum == room_id && (int)k_meldungen[i].id == msg_id) {
            if (out_len) *out_len = k_meldungen[i].n;
            return k_meldungen[i].b;
        }
    if (out_len) *out_len = 0;
    return NULL;
}

const uint8_t *re15_annette_r41_meldung(int msg_id, int *out_len)
{
    return re15_annette_r41_meldung_raum(RE15_AN_RAUM, msg_id, out_len);
}

static void protokoll(const char *was)
{
#ifdef RE15_PLATFORM_PC
    const re15_actor_t *f = s_an.form >= 0 ? &g_actors[s_an.form] : NULL;
    const re15_actor_t *a = s_an.frau >= 0 ? &g_actors[s_an.frau] : NULL;
    fprintf(stderr, "[annette] F%u %s: Phase %d, Form %s(%d,%d,%d) Gier %d Clip %d/%d, Annette %s(%d,%d,%d) Clip %d/%d, Cut %d\n",
            (unsigned)g_engine.frame_count, was, s_an.phase,
            f ? "" : "-", f ? (int)f->x : 0, f ? (int)f->y : 0, f ? (int)f->z : 0, f ? (int)f->rot_y : 0,
            f ? (int)f->motion : -1, f ? (int)f->anim_frame : -1,
            a ? "" : "-", a ? (int)a->x : 0, a ? (int)a->y : 0, a ? (int)a->z : 0, a ? (int)a->motion : -1,
            a ? (int)a->anim_frame : -1, (int)g_scd.work_vars[0x0A]);
#else
    (void)was;
#endif
}

/* ---- Bedingungen ---------------------------------------------------------------------------------------------------- */
static int raum_ist(uint16_t room_id) { return room_id == RE15_AN_RAUM; }

static int zug_gesehen(void) { return re15_game_flag_get(RE15_AN_ZUG_BANK, RE15_AN_ZUG_BIT) != 0; }

static int annette_steht_da(void)
{
    /* PORT-WAHL nach Nutzerentscheid: im Wagen erst nach dem Tor (3,89) UND nach der Zugstart-Szene im Fuehrerstand
     * (9,133); nach dem Kampfstart (3,42) nicht mehr (sie ist dann tot). */
    return re15_game_flag_get(RE15_AN_TOR_BANK, RE15_AN_TOR_BIT) && zug_gesehen() &&
           !re15_game_flag_get(RE15_AN_KAMPF_BANK, RE15_AN_KAMPF_BIT);
}

int re15_annette_r41_spawn_weg(uint16_t room_id, uint8_t type)
{
    if (!raum_ist(room_id) || type != RE15_AN_TYP) return 0;
    const int weg = !annette_steht_da();
#ifdef RE15_PLATFORM_PC
    if (weg) fprintf(stderr, "[annette] ROOM%04X: Frau im Kittel (0x4D, @0x0122A) verworfen: (3,89)=%d (9,133)=%d (3,42)=%d\n",
                     room_id, re15_game_flag_get(3, 89), re15_game_flag_get(RE15_AN_ZUG_BANK, RE15_AN_ZUG_BIT),
                     re15_game_flag_get(3, 42));
#endif
    return weg;
}

static int slot_vom_typ(uint8_t type)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++)
        if (g_actors[i].active && g_actors[i].type == type) return i;
    return -1;
}

static int16_t blick(const re15_actor_t *e, int32_t tx, int32_t tz)
{
    /* dieselbe Peilung wie re15_enemy_steer_point / birkin_formen.c bf_blick */
    return (int16_t)(((int)re15_atan2_q12(tz - e->z, tx - e->x) - 0x400) & 0x0fff);
}

static void clip(re15_actor_t *e, int c)
{
    e->motion = (int16_t)c; e->anim_frame = 0; e->anim_frac = 7;
    e->anim_flags &= (uint16_t)~0x80u;
}

/* Bild vorruecken, am Ende das letzte Bild halten; Bildton der Form wie bb_bild_ton. Rueckgabe 1 am Clip-Ende. */
static int vor(re15_actor_t *e, int halten)
{
    if (e->type == RE15_AN_FORM_TYP) {
        const re15_enemy_bank_t *b = re15_enemy_find(e->type);
        if (b && b->ok) {
            const uint32_t w = re15_emd_get_frame_entry(&b->anim, (int)e->motion, (int)e->anim_frame);
            if ((w & AN_TON_BIT) && s_se_fn) { if (s_bank_fn) s_bank_fn(AN_TON_BANK); s_se_fn((int)(w >> 28), 0); }
        }
    }
    if (halten && e->anim_frame + 1 >= re15_r38_birkin_fc(e)) return 1;
    return re15_r38_birkin_anim(e);
}

/* Ein Bild Blut von FUN_80104C24 (@0x80104D18-60): je ein FUN_801058BC-Aufruf an den beiden aeusseren Klauen, Lage = Welt-
 * lage des Knochens in diesem Bild (Einheitsanker 0x80095F68 + a3 = Teil-T @0x80105950-78), Winkel = Peilung Form -> Spieler
 * (FUN_80014E4C @0x80105908 = Leon FUN_800154AC; Port-Zwilling wie re2z_bearing_to), Boden = Aktor-y wie
 * re2z_gore_fx_core. Rueckgabe der Zeilen-Spawns = belegte Plaetze (Pool voll: das Original verliert den Aufruf,
 * @0x8001B98C-98 `j 0x8001bc1c` / `addiu v0,zero,0xff`). */
static void blut_bild(const re15_actor_t *f)
{
    static const int k_klaue[2] = { AN_BLUT_KLAUE_A, AN_BLUT_KLAUE_B };   /* Reihenfolge @0x80104D38, @0x80104D4C */
    if (s_an.blut_rest <= 0) return;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int16_t w = blick(f, pl->x, pl->z);
    for (int i = 0; i < 2; i++) {
        int32_t p[3];
        re15_enemy_bone_world_pos(f, k_klaue[i], p);
        s_an.blut++;
        s_an.blut_plaetze += re15_esp_fx_spawn_rows(re15_esp_room_bank(), AN_BLUT_EFFEKT, AN_BLUT_SUB, AN_BLUT_SKALA,
                                                    p[0], p[1], p[2], f->y, w);
    }
    s_an.blut_rest--;                                    /* @0x80104D54-60 `addiu v0,v0,-1` / `sb v0,0x16b(s0)` */
#ifdef RE15_PLATFORM_PC
    if (s_an.blut_rest == AN_BLUT_BILDER - 1 || s_an.blut_rest == 0)
        fprintf(stderr, "[annette] F%u Blut %d/%d: %d Aufrufe, %d Plaetze, Pool %d/%d\n", (unsigned)g_engine.frame_count,
                AN_BLUT_BILDER - s_an.blut_rest, AN_BLUT_BILDER, s_an.blut, s_an.blut_plaetze, re15_esp_fx_count(),
                RE15_ESP_FX_MAX);
#endif
}

static void meldungen_einsetzen(uint16_t room_id)
{
    for (int i = 0; i < AN_N_MELDUNGEN; i++) {
        if (k_meldungen[i].raum != room_id) continue;
        re15_msg_install_text(k_meldungen[i].id, k_meldungen[i].b, k_meldungen[i].n);
        int d = re15_msg_compute_duration(k_meldungen[i].b, k_meldungen[i].n, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(k_meldungen[i].id, d);
    }
}

void re15_annette_r41_install(uint16_t room_id)
{
    if (s_an.raum != room_id) { re15_annette_r41_reset(); s_an.raum = room_id; }
    if (raum_ist(room_id) || room_id == RE15_AN_ZUG_RAUM) meldungen_einsetzen(room_id);
}

/* ---- 6030 ----------------------------------------------------------------------------------------------------------- */
static const uint8_t *zug_evt_exec(uint8_t sub_id, const uint8_t *target)
{
    if (sub_id != RE15_AN_ZUG_SUB) return target;
    if (!re15_game_flag_get(RE15_AN_TOR_BANK, RE15_AN_TOR_BIT) || zug_gesehen()) return target;
    /* sub00 @0x00D28 Switch(Cut) Case 0 -> sub07 (Fuehrerstand), Case 6/8/12 -> sub06 (Wagen). */
    if (g_scd.work_vars[0x0A] != 0) {
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[annette] ROOM6030 Wagen (Cut %d): Zugstart nicht gelaufen -> Ausgang = Tuer (main00 @0x00C72), "
                "Tuer nach 5090 = msg 10 (sub06 @0x0106A)\n",
                (int)g_scd.work_vars[0x0A]);
#endif
        return k_tuer;
    }
    /* Erster Besuch erst nach dem Tor: sub07 @0x010D4 hat sie schon gesetzt und sub08 laeuft -> Zugstart beim naechsten
     * Eintritt (sub07 spawnt sie ab (4,23)=1 und (3,89)=1 nicht mehr, @0x010F4). */
    if (slot_vom_typ(RE15_AN_TYP) >= 0) return target;
    zug_bauen();
    if (!s_zug_len) return target;
    s_an.zug++;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[annette] F%u ROOM6030 Zugstart-Szene statt sub05 (sub07 @0x01132), %d Byte\n",
            (unsigned)g_engine.frame_count, s_zug_len);
#endif
    return s_zug_prog;
}

static void zug_tick(void)
{
    if (re15_game_flag_get(5, RE15_AN_SIG_ZUG_AN)) { re15_game_flag_set(5, RE15_AN_SIG_ZUG_AN, 0); ton(RE15_AN_SE_ZUG_AN); }
    if (re15_game_flag_get(5, RE15_AN_SIG_KRACH))  { re15_game_flag_set(5, RE15_AN_SIG_KRACH, 0);  ton(RE15_AN_SE_KRACH); }
}

/* ---- 5090 ----------------------------------------------------------------------------------------------------------- */
const uint8_t *re15_annette_r41_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *target)
{
    if (room_id == RE15_AN_ZUG_RAUM) return zug_evt_exec(sub_id, target);
    if (!raum_ist(room_id) || sub_id != RE15_AN_SUB_KAMPF) return target;
    if (s_an.phase == RE15_AN_LEICHE) {                  /* das eigene Programm ruft den Kampfstart: Form frei */
        s_an.phase = RE15_AN_FREI;
        protokoll("Kampfstart (sub04), Form frei");
        return target;
    }
    if (s_an.phase != RE15_AN_AUS || !annette_steht_da()) return target;
    s_an.form = slot_vom_typ(RE15_AN_FORM_TYP);
    s_an.frau = slot_vom_typ(RE15_AN_TYP);
    if (s_an.form < 0 || s_an.frau < 0 || !re15_bb_bank_da(&g_actors[s_an.form])) {
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[annette] keine Szene: Form-Slot %d (Riesenform %d), Annette-Slot %d\n", s_an.form,
                s_an.form >= 0 ? re15_bb_bank_da(&g_actors[s_an.form]) : 0, s_an.frau);
#endif
        s_an.form = s_an.frau = -1;
        return target;
    }
    for (unsigned b = RE15_AN_SIG_SCHLAG; b <= RE15_AN_SIG_BRUELL; b++) re15_game_flag_set(5, b, 0);
    s_an.phase = RE15_AN_WARTEN; s_an.t = 0; s_an.blut = 0; s_an.blut_plaetze = 0; s_an.blut_rest = 0; s_an.luft = 0;
    s_an.treffer = 0;
    re15_actor_t *f = &g_actors[s_an.form], *a = &g_actors[s_an.frau];
    f->no_draw = 1;                                      /* bis zum Sturz unsichtbar (RE2: geparkt auf y -9000, @0x00C82) */
    a->x = AN_A0_X; a->y = 0; a->z = AN_A0_Z; a->rot_x = a->rot_z = 0;   /* PORT-WAHL A0 (s. oben); vorher in Cut 13 nicht im Bild */
    a->rot_y = blick(a, g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
    ton(RE15_AN_SE_KRACH);                               /* RE2 @0x01040 Se_on(3,0x0B) */
    protokoll("Szenenstart statt sub04 (Krach)");
    return k_prog;
}

int re15_annette_r41_besitzt(int slot)
{
    if (s_an.phase == RE15_AN_AUS || slot < 0) return 0;
    if (slot == s_an.frau) return s_an.phase >= RE15_AN_HIEB;   /* vorher geht sie ueber Plc_dest (Sub-VM) */
    return slot == s_an.form && s_an.phase < RE15_AN_FREI;
}

int re15_annette_r41_volle_drehung(const void *actor)
{
    return s_an.frau >= 0 && actor == (const void *)&g_actors[s_an.frau] && s_an.phase >= RE15_AN_LEICHE;
}

const char *re15_annette_r41_emd_datei(uint8_t type)
{
    if (type != RE15_AN_TYP) return NULL;                /* 0x4D steht nur in 5090/6000/6030 (Leon) */
    return s_an.blut_textur ? "ANNETTE_4D_BLUT.EMD" : "ANNETTE_4D.EMD";
}

/* Hieb ausrichten: die Gier waehlen, bei der die linke Hand im Trefferbild ihr am naechsten kommt (gemessen am Gerippe). */
static void hieb_ausrichten(re15_actor_t *f, const re15_actor_t *a)
{
    const int16_t basis = blick(f, a->x, a->z);
    const int16_t m0 = f->motion; const int32_t b0 = f->anim_frame;
    int16_t beste = basis; int64_t best = -1;
    f->motion = AN_FC_HIEB; f->anim_frame = AN_HIEB_TREFFER + 1;
    for (int d = -0x100; d <= 0x100; d += 0x10) {
        int32_t h[3];
        f->rot_y = (int16_t)((basis + d) & 0x0fff);
        re15_enemy_bone_world_pos(f, AN_HAND_LINKS, h);
        const int64_t dx = h[0] - a->x, dz = h[2] - a->z, q = dx * dx + dz * dz;
        if (best < 0 || q < best) { best = q; beste = f->rot_y; }
    }
    f->rot_y = beste; f->motion = m0; f->anim_frame = b0;
}

static void treffer(re15_actor_t *f, re15_actor_t *a)
{
    (void)f;
    s_an.blut_rest = AN_BLUT_BILDER;                     /* @0x80104D10-14 (Blut ab diesem Bild, blut_bild im Takt) */
    a->hp = -1;                                          /* NPC-Familie: unverwundbar, kein Treffer-Kandidat (re15_damage.c) */
    s_an.blut_textur = 1;
    if (s_modell_fn) s_modell_fn(RE15_AN_TYP);           /* Kittel mit Blut (ANNETTE_4D_BLUT.EMD = RE2 EM049-Textur) */
    s_an.treffer = 1;
    protokoll("Treffer");
}

/* RE2 @0x01162: Pos_set liegend, waehrend die Kamera weg ist. */
static void liegen(re15_actor_t *f, re15_actor_t *a)
{
    int32_t ux = a->x - f->x, uz = a->z - f->z;
    int32_t l = (int32_t)re15_squareroot0((uint32_t)(ux * ux + uz * uz)); if (l < 1) l = 1;   /* BIOS-Wurzel (re15_math.h) */
    const int64_t ex = (int64_t)ux * 4096 / l, ez = (int64_t)uz * 4096 / l;      /* weg von der Form, Q12 */
    a->x += (int32_t)((ex * AN_LIEGEN_WEG - ez * AN_LIEGEN_SEIT) / 4096);
    a->z += (int32_t)((ez * AN_LIEGEN_WEG + ex * AN_LIEGEN_SEIT) / 4096);
    a->rot_y = (int16_t)((blick(a, f->x, f->z) + 0x800) & 0x0fff);
    clip(a, AN_FA_STURZ);
    a->anim_frame = AN_FA_STURZ_BODEN;
    a->rot_z = AN_KIPP_WINKEL;
    a->y = AN_KIPP_HEBEN;
}

void re15_annette_r41_tick(void)
{
    if ((uint16_t)g_current_room_id == RE15_AN_ZUG_RAUM) { zug_tick(); return; }
    if (s_an.phase == RE15_AN_AUS) return;
    if (!raum_ist((uint16_t)g_current_room_id)) { re15_annette_r41_reset(); return; }
    if (s_an.form < 0 || s_an.frau < 0) return;
    re15_actor_t *f = &g_actors[s_an.form], *a = &g_actors[s_an.frau];
    if (!f->active || !a->active) { s_an.phase = RE15_AN_FREI; return; }
    s_an.t++;
    if (s_an.phase < RE15_AN_FREI && re15_game_flag_get(5, RE15_AN_SIG_BRUELL)) {
        re15_game_flag_set(5, RE15_AN_SIG_BRUELL, 0); ton(RE15_AN_SE_BRUELL);
    }
    switch (s_an.phase) {
    case RE15_AN_WARTEN:
        f->no_draw = 1;
        if (re15_game_flag_get(5, RE15_AN_SIG_STURZ)) {
            re15_game_flag_set(5, RE15_AN_SIG_STURZ, 0);
            f->no_draw = 0; f->rot_x = f->rot_z = 0;
            f->x = AN_L_X; f->y = AN_STURZ_Y; f->z = AN_L_Z;          /* 5080 sub03 @0x00822 Pos_set */
            f->rot_y = (int16_t)(((int)re15_atan2_q12(AN_S_Z - AN_L_Z, AN_S_X - AN_L_X) - 0x400) & 0x0fff);
            clip(f, AN_FC_STEHEN);                       /* 5080: faellt im Stehen-Clip vom Dach (re15_bb_wartet) */
            ton(RE15_AN_SE_STURZ);                       /* RE2 @0x010CE Se_on(2,0x0A) */
            s_an.phase = RE15_AN_STURZ; s_an.t = 0; s_an.luft = 0;
            protokoll("Sturz");
        }
        break;
    case RE15_AN_STURZ:
        vor(f, 0);
        if (s_an.t > AN_STURZ_HALT && s_an.luft < AN_STURZ_N) {   /* @0x0082A Sleep 1, dann je Bild Add_speed (+400) */
            f->y += AN_STURZ_V; s_an.luft++;
        }
        if (s_an.luft >= AN_STURZ_N) {                   /* 14 x 400 = gelandet (y -5600 -> 0) */
            f->y = 0;
            clip(f, AN_FC_DROHEN);
            s_an.phase = RE15_AN_DROHEN; s_an.t = 0;
            protokoll("gelandet, Drohen");
        }
        break;
    case RE15_AN_DROHEN:
        if (f->motion == AN_FC_DROHEN) { if (vor(f, 1)) clip(f, AN_FC_STEHEN); }
        else vor(f, 0);
        if (re15_game_flag_get(5, RE15_AN_SIG_SCHLAG)) {
            re15_game_flag_set(5, RE15_AN_SIG_SCHLAG, 0);
            a->state = 0; a->sub_state_1 = 0; a->sub_state_2 = 0; a->walk_active = 0;   /* Takt uebernimmt (Plc_dest fertig) */
            a->rot_y = blick(a, f->x, f->z);
            clip(a, AN_FA_REAKTION);
            hieb_ausrichten(f, a);
            clip(f, AN_FC_HIEB);
            s_an.phase = RE15_AN_HIEB; s_an.t = 0;
            protokoll("Hieb (RE2 @0x01142)");
        }
        break;
    case RE15_AN_HIEB:
        vor(a, 1);
        if (!s_an.treffer && (int)f->anim_frame == AN_HIEB_TREFFER) { treffer(f, a); s_an.phase = RE15_AN_REAKTION; s_an.t = 0; }
        vor(f, 1);
        if (re15_game_flag_get(5, RE15_AN_SIG_LIEGT)) {   /* Schnitt weg vor dem Trefferbild: Treffer nachholen */
            if (!s_an.treffer) treffer(f, a);
            s_an.phase = RE15_AN_REAKTION; s_an.t = 0;
        }
        break;
    case RE15_AN_REAKTION:
        vor(a, 1);
        if (f->motion == AN_FC_HIEB) { if (vor(f, 1)) clip(f, AN_FC_STEHEN); } else vor(f, 0);
        if (re15_game_flag_get(5, RE15_AN_SIG_LIEGT)) {
            re15_game_flag_set(5, RE15_AN_SIG_LIEGT, 0);
            liegen(f, a);
            s_an.schrei++;
            if (s_stimme_fn) s_stimme_fn(RE15_AN_RAUM, RE15_AN_MSG_SCHREI);   /* nur Ton, kein Untertitel (Nutzer F9) */
            s_an.phase = RE15_AN_LEICHE; s_an.t = 0;
            protokoll("Schnitt weg, liegt, Schrei (RE2 @0x01150/@0x01162)");
        }
        break;
    case RE15_AN_LEICHE:
        if (f->motion == AN_FC_HIEB) { if (vor(f, 1)) clip(f, AN_FC_STEHEN); } else vor(f, 0);
        f->rot_y = blick(f, a->x, a->z);
        /* fallthrough: Leiche halten */
    case RE15_AN_FREI:
    default:
        a->anim_frame = AN_FA_STURZ_BODEN;
        a->rot_z = AN_KIPP_WINKEL;
        a->y = AN_KIPP_HEBEN;
        break;
    }
    /* Blut wie FUN_80104C24: nach dem Bildvorschub (@0x80104C9C-A8) und dem Trefferbild (@0x80104CD4-14) je Bild an den
     * Klauen, 15 Bilder lang, unabhaengig vom Schnitt (Routine der Form, nicht des Skripts). */
    if (s_an.phase >= RE15_AN_HIEB) blut_bild(f);
}
