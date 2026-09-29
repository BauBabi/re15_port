/* re15_door_seq.h — die RE2-TUERSEQUENZ ("Tuer auf, Tuer zu") als plattformfreie Maschine.
 *
 * ⛔ RE2-ERGAENZUNG (Beta -> Retail). RE1.5 hat die Tuermaschine vollstaendig (Door_main
 * 0x80016188, gerufen aus FUN_8001d600 @0x8001d838/48 bei JEDEM Tuerwechsel), aber keine
 * Sequenz: das einzige Skript in DOOR00.DO2 ist `Evt_end` (@Datei 0x9A6 `01 00`), und die
 * Raumdaten waehlen kein Archiv (649 von 653 Door_aot_set: Payload+12/+13 = 0). Vorbild ist
 * deshalb RE2 Retail (info/re2leon/PSX.EXE); Belege je Stelle im Quelltext und in
 * analysis/tor_1170/03_tuersequenz.md, 04_tuerkatalog.md, 08_re_*.md.
 *
 * RE2-Funktionskarte: Door_main LAB_80013bc4 -> Door_init FUN_80013c1c, Door_move
 * FUN_80013eb4 (ein Durchlauf = ein Bild), Door_exit 0x8001417c; Scheduler FUN_80014058
 * (Plaetze 10..13); Objekte FUN_80014234 (10 Stueck); Opcode-Tabelle 0x800a74c8.
 *
 * Diese Datei kennt nur Daten und Rechnung (Skripte, Objekte, Matrizen, Blende, Toene).
 * Zeichnen, Takt und Ton gibt die Plattform (platform/pc/src/door_scene_pc.c). */
#ifndef RE15_DOOR_SEQ_H
#define RE15_DOOR_SEQ_H

#include <stdint.h>
#include "re15_md1.h"
#include "re15_tim.h"

#define RE15_DOOR_OBJEKTE   10   /* FUN_80014234 @0x80014664 slti v0,s2,10                   */
#define RE15_DOOR_PLAETZE    4   /* Plaetze 10..13: @0x80014068 addiu s2,zero,10 / @0x80014150 sltiu v0,s2,0xe */
#define RE15_DOOR_ERSTER_PLATZ 10
#define RE15_DOOR_SKRIPTE_MAX 32
#define RE15_DOOR_EBENEN     4   /* Gosub-Tiefe / Schleifenebenen je Ereignis */

/* Kameramatrix der Tuerszene: Auge (10000,0,0), Ziel (0,0,0) (Daten @0x80010830/@0x8001083c,
 * FUN_80013c1c @0x80013e38..3c jal 0x80076cb0) -> Zeilen (0,0,1),(0,1,0),(-1,0,0), t=(0,0,10000). */
#define RE15_DOOR_KAMERA_X  10000
#define RE15_DOOR_H           290   /* @0x80013e34 addiu a0,zero,290 / @0x80013e30 jal 0x8008de24 (ctc2 H) */

typedef struct {
    int16_t m[9];      /* Q12, Zeile fuer Zeile (MATRIX.m[3][3]) */
    int32_t t[3];
} re15_door_mat_t;

typedef struct {
    uint16_t on;       /* obj+0   (Door_model_set pc[4])                */
    uint8_t  b8;       /* obj+8   (pc[2])                               */
    uint16_t bild;     /* obj+0x10e (pc[3]) Bildnummer fuer Flag 0x400    */
    int16_t  w10;      /* obj+0x10 (s16 @8)                             */
    uint16_t mesh;     /* obj+0x146 (pc[5])                             */
    uint16_t flags;    /* obj+0x144 (u16 @6)                            */
    int32_t  pos[3];   /* obj+0x38/0x3c/0x40                            */
    uint16_t rot[3];   /* obj+0x74/0x76/0x78 (4096 = 360 Grad)          */
    uint16_t rot0[3];  /* Drehung aus Door_model_set pc+16..21 (nur Port: Grund-Drehung fuer den
                        * Griff-Tausch, door_scene_pc.c; die Maschine liest es nie)          */
    int8_t   eltern;   /* Flag 0x10 -> flags & 0xF, sonst -1 = Kamera   */
    re15_door_mat_t welt;       /* obj+0x54: Objekt -> Sicht, dieses Bild   */
    re15_door_mat_t welt_vor;   /* dieselbe Matrix des VORIGEN Bildes (Licht) */
} re15_door_obj_t;

typedef struct {
    uint8_t  aktiv;
    uint16_t pc;
    uint8_t  sub;                                     /* Gosub-Ebene */
    int8_t   ifn[RE15_DOOR_EBENEN];
    int8_t   lvl[RE15_DOOR_EBENEN];
    uint16_t ifstk[RE15_DOOR_EBENEN][8];
    uint8_t  ifsp, ifsp_sub;
    uint16_t cnt[RE15_DOOR_EBENEN][RE15_DOOR_EBENEN];
    uint16_t lstart[RE15_DOOR_EBENEN][RE15_DOOR_EBENEN];
    uint16_t lend[RE15_DOOR_EBENEN][RE15_DOOR_EBENEN];
    int8_t   lifn[RE15_DOOR_EBENEN][RE15_DOOR_EBENEN];
    uint16_t ret[RE15_DOOR_EBENEN];
    int8_t   work_typ, work_id;                       /* Work_set */
    int16_t  speed[12];                               /* Ereignis+344 (@0x80055aac sh a1,344(v1)) */
} re15_door_evt_t;

/* Toene, die ein Bild ausgeloest hat (Se_on im Skript; Door_exit spielt Eintrag 1). */
typedef struct {
    uint8_t vab;       /* pc[1]  */
    int16_t se;        /* s16@2  */
    int16_t bezug;     /* s16@4  */
    int16_t pos[3];    /* s16@6..10 */
} re15_door_ton_t;

typedef struct {
    /* Archiv (Modellteil im RE2-Aufbau: +0 MD1-Versatz, +4 TIM-Versatz, ab +8 SCD) */
    const uint8_t *teil;
    int            teil_groesse;
    uint8_t       *scd;              /* EIGENE Kopie: Work_copy schreibt in die Skriptbytes */
    int            scd_groesse;
    uint16_t       skript_off[RE15_DOOR_SKRIPTE_MAX];
    int            n_skripte;
    re15_md1_t     md1;
    re15_tim_t     tim;
    int            md1_ok, tim_ok;

    int16_t        var[256];         /* Skriptvariablen (s16-Feld ab 0x800d47ec)         */
    re15_door_obj_t obj[RE15_DOOR_OBJEKTE];
    re15_door_evt_t ev[RE15_DOOR_PLAETZE];

    int            bild;             /* Bildzaehler (Arbeitsbereich +0x22e)               */
    uint8_t        schliesston;      /* Merker Arbeitsbereich +0x248 (Flag 0x800)          */
    int            ende;             /* Platz 10 aus -> Door_move-Schleife verlassen       */

    re15_door_ton_t ton[8];          /* in diesem Bild ausgeloeste Toene                   */
    int            n_ton;

    /* Blende Kanal 0 (RE2-Satz ab 0x800dfc1c): Belege in door_seq_common.c */
    uint16_t       blende_pegel;
    int16_t        blende_schritt;
    uint8_t        blende_art;
    uint8_t        blende_maske;

    unsigned       notizen;          /* Bitmaske: unbekannter Opcode, Endlosschleife ...   */
} re15_door_seq_t;

#define RE15_DOOR_NOTIZ_UNBEKANNT   0x01u
#define RE15_DOOR_NOTIZ_SCHLEIFE    0x02u
#define RE15_DOOR_NOTIZ_OHNE_WORK   0x04u
#define RE15_DOOR_NOTIZ_FORMAT      0x08u

/* Door_init FUN_80013c1c. teil = Modellteil, variante = var 12 (Payload+13 & 0x7f),
 * var0e = Payload+13 & 0x80, tuer_nr = var 15 (Payload+12). 0 = gut, <0 = Formatfehler. */
int  re15_door_seq_start(re15_door_seq_t *s, const uint8_t *teil, int teil_groesse,
                         int variante, int var0e, int tuer_nr);
/* Ein Durchlauf von Door_move FUN_80013eb4: Ladewarten, Scheduler, Objektmatrizen,
 * Bildzaehler. ton_geladen = 1, sobald der Tonteil geladen ist (var 13 faellt dann auf 0).
 * Rueckgabe 1 = Bild gezeigt, weiter; 0 = Platz 10 ist aus (Schleife verlassen). */
int  re15_door_seq_bild(re15_door_seq_t *s, int ton_geladen);
/* Blende: Takt eines Bildes. Liefert die Helligkeit (0..255), die abzuziehen ist,
 * oder -1, wenn der Kanal nichts zeichnet. */
int  re15_door_seq_blende_takt(re15_door_seq_t *s);
int  re15_door_seq_blende_fertig(const re15_door_seq_t *s);
void re15_door_seq_blende_schwarz(re15_door_seq_t *s);   /* Door_exit: Pegel 0x7fff, Schritt 0 */
void re15_door_seq_ende(re15_door_seq_t *s);   /* Speicher der Skriptkopie freigeben */

/* Mathematik (auch fuer Tests): RE2 libgte RotMatrix @0x8008e1f4 mit rcossin_tbl @0x800adeac. */
void re15_door_rotmatrix(const uint16_t rot[3], int16_t m[9]);
/* Objektmatrix = Eltern * RotMatrix(rot) + Lage (Kette FUN_80014234), fuer den Griff-Tausch. */
void re15_door_seq_objmatrix(const re15_door_mat_t *eltern, const uint16_t rot[3],
                             const int32_t pos[3], re15_door_mat_t *out);


/* ===========================================================================
 * Anbindung ans Spiel
 * ======================================================================== */
#define RE15_DOOR_ARCHIV_KEINS     0
#define RE15_DOOR_ARCHIV_TOR1170   1   /* engine/src/gen/tor_1170_door.inc */
#define RE15_DOOR_ARCHIV_RE2       2   /* RE2-Archiv DOORxx.DO2 aus shared_assets/RE2/DOOR (Runde 31) */

#define RE15_DOOR_KEIN_SPENDER  0xFFu

typedef struct {
    uint8_t  aktiv;
    uint8_t  archiv;     /* RE15_DOOR_ARCHIV_* */
    uint8_t  variante;   /* var 12 = Payload+13 & 0x7f (@0x80013e5c/6c)            */
    uint8_t  tuer_nr;    /* var 15 = Payload+12 (@0x80013e90/98) = RE2-Archivnummer */
    uint8_t  bit7;       /* var 14 = Payload+13 & 0x80 (@0x80013e84/8c)             */
    uint8_t  re2_nr;     /* RE2-Archiv 0..0x36 (nur RE15_DOOR_ARCHIV_RE2)           */
    uint8_t  spender;    /* Griff-Tausch: Spender-Archiv, RE15_DOOR_KEIN_SPENDER     */
    uint16_t seite;      /* Tuerseite Sxxx (Runde 31, Pruefhaken/Protokoll), 0 = keine */
    uint16_t tuer;       /* Tuer Txxx, 0 = keine                                     */
} re15_door_seq_anfrage_t;

/* Eine Zeile der Port-Tabelle RE1.5-Tuerseite -> RE2-Archiv (engine/src/gen/tuer_zuordnung.inc,
 * erzeugt von tools/tueren/tuer_zuordnung_gen.py). PORT-WAHL aus dem Bildvergleich. */
typedef struct {
    uint16_t raum;        /* volle Raum-Id inkl. Variante (xxx0 / xxx1)               */
    uint8_t  form;        /* 0 = Rechteck (Mitte/Halbmass), 1 = Viereck (Punkte)      */
    uint8_t  band;        /* Door_aot_set pc[4]                                      */
    int32_t  x, z, hw, hh;
    int16_t  qx[4], qz[4];
    uint8_t  re2_nr;
    uint8_t  variante;
    uint8_t  bit7;
    uint8_t  spender;
    uint16_t seite, tuer;
    uint32_t off;         /* RDT-Datei-Offset eines Door_aot_set dieser Seite (Herkunft, Riegel) */
} re15_tuer_zeile_t;

/* Griff-Tausch je Paar Archiv <- Spender (tools/tueren/tuer_zuordnung_gen.py, [SIM]-Werte). */
typedef struct {
    uint8_t  archiv, spender;
    uint8_t  mesh_archiv, mesh_spender;
    uint16_t rot_vorn[3], rot_hinten[3];   /* Grund-Drehung des Spendergriffs            */
    int16_t  aus_archiv, aus_spender;      /* Ausschlag rot x beim Oeffnen               */
} re15_griff_tausch_t;

extern re15_door_seq_anfrage_t g_door_seq_anfrage;

/* Welche Tuer bekommt welche Sequenz? RE1.5-Raumdaten tragen keine (Payload+12/+13 = 0 in
 * 649 von 653 Door_aot_set), die Zuordnung ist deshalb eine Port-Tabelle. Schluessel =
 * Raum + Rechteck (Mitte/Halbmass) + Band - NICHT der Slot (ROOM1170 Slot 6 = ROOM1171 Slot 5,
 * und ROOM1170 Slot 3, die Intro-Uebergabe, traegt dieselbe Nutzlast wie das Tor zurueck,
 * aber Rechteck 0 und Band 0; analysis/tor_1170/05_port_anschluss.md 1.4).
 * Rueckgabe RE15_DOOR_ARCHIV_*, *variante = var 12. */
int  re15_door_seq_zuordnen(unsigned room_id, int32_t x, int32_t z, int32_t half_w, int32_t half_h,
                            int band, int *variante);
/* Dasselbe fuer jede Flaechenform (Runde 31): Rechteck (viereck = 0: Mitte/Halbmass) oder
 * Viereck (viereck = 1: die vier Punkte pc+6..21). Erst das Tor, dann die RE2-Tabelle.
 * Fuellt *out (aktiv bleibt 0) und liefert RE15_DOOR_ARCHIV_*. */
int  re15_door_seq_zuordnen_flaeche(unsigned room_id, int viereck, int32_t x, int32_t z,
                                    int32_t half_w, int32_t half_h,
                                    const int16_t qx[4], const int16_t qz[4], int band,
                                    re15_door_seq_anfrage_t *out);
/* Nur die RE2-Tabelle (ohne Tor), door_seq_zuordnung.c. */
int  re15_door_seq_zuordnen_re2(unsigned room_id, int viereck, int32_t x, int32_t z,
                                int32_t half_w, int32_t half_h,
                                const int16_t qx[4], const int16_t qz[4], int band,
                                re15_door_seq_anfrage_t *out);
/* Tabellenzugriff (Pruefhaken, Tests): Anzahl Zeilen, Zeile i. Auf der PSX ist die Tabelle leer. */
int  re15_door_seq_zeilen(void);
const re15_tuer_zeile_t *re15_door_seq_zeile(int i);
/* Anfrage einer Tuerseite (Sxxx) aus der Tabelle - erste Zeile dieser Seite. 0 = unbekannt. */
int  re15_door_seq_anfrage_fuer_seite(int seite, re15_door_seq_anfrage_t *out, unsigned *raum);
/* Griff-Tausch-Satz fuer Archiv <- Spender, NULL = keiner. */
const re15_griff_tausch_t *re15_door_seq_griff_tausch(int archiv, int spender);
/* RE2-Archivtabelle @0x8009a520 (engine/src/gen/re2_tuer_tabelle.inc): Tonteil- und
 * Modellteil-Groesse, Sektor. 0 = gut. */
int  re15_door_seq_re2_archiv(int nr, int *ton, int *modell, int *sektor, int *datei);
/* Modellteil eines Archivs (eingebacken). */
const uint8_t *re15_door_seq_archiv(int archiv, int *groesse);

/* Die Plattform spielt die Sequenz (eigene Bildschleife, Zeichnen, Ton). Ohne Laeufer
 * (Tests, headless, PSX) verfaellt die Anfrage und der Tuerwechsel laeuft wie bisher. */
typedef void (*re15_door_seq_laeufer_t)(const re15_door_seq_anfrage_t *a);
void re15_door_seq_setze_laeufer(re15_door_seq_laeufer_t f);
/* Anstehende Anfrage abarbeiten (blockiert, solange die Sequenz laeuft). 1 = gespielt. */
int  re15_door_seq_ausfuehren(void);

#endif /* RE15_DOOR_SEQ_H */
