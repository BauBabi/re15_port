/*
 * RE1.5 Rebuilt — RE2-Asset-Infrastruktur (WELLE A des RE2-Modus-Vollausbaus).
 *
 * PORT-OPTION, kein RE1.5-Original-Verhalten: dieser Loader existiert nur fuer den
 * OPTIONS→AI→RE2-Flavor (re15_ai_flavor.h) und wird ausschliesslich PC-seitig
 * benutzt (PSX-BSS ist hart an der Decke; re15_enemy.h Kopfkommentar). Der
 * RE1.5-Default-Pfad bleibt byte-identisch unangetastet.
 *
 * Datenquellen (alle Byte-Belege im Generator tools/gen_re2_ems_toc.py + hier):
 *   - shared_assets/RE2/CDEMD0.EMS  — RE2-Gegner-Archiv (Overlay/TIM/EMD-Records,
 *     tabellengetrieben ueber die EXE-TOC @0x8009adf4 — anders als RE1.5s
 *     self-describing EMS-Kette, darum passt re15_ems.h hier NICHT).
 *   - shared_assets/RE2/ENEMSE.VBS  — RE2-Gegner-SE-Container (EDT+VBD-Records,
 *     Bank-TOC @0x800a7b1c, Loader FUN_8005a09c, Trigger FUN_8005bd6c).
 *
 * RE2-EMD-Record-Layout (byte-verifiziert an EM010 = kind 0x10 @EMS+0x2A800):
 *   +0 u32 dir_offset (EM010: 0x23DAC, Directory am DATEI-ENDE)
 *   +4 u32 dir_count == 8   (RE1.5: 9 — RE2-EMD traegt KEIN TIM!)
 *   Directory-Bindung (Binder FUN_8001aaa8, Zuweisungen @0x8001aba0..0x8001ac14):
 *     dir[0]  = 4 Zero-Bytes (Reserve, Entity+0x1BC)
 *     dir[1]/[2] = EDD/EMR Paar 1 (EMR traegt die Bone-STRUKTUR, EM010 {0x64,0xB0,15,0x50})
 *     dir[3]/[4] = EDD/EMR Paar 2 (EMR = Pool-only, kf_ofs=8 — wie RE1.5-Bank-Konvention)
 *     dir[5]/[6] = EDD/EMR Paar 3 (Pool-only)
 *     dir[7]  = MD1 (EM010: nObj=34)
 *   Sektionsformate == RE1.5 (EDD/EMR/MD1 byte-kompatibel, Lane-I §1) — der Splitter
 *   fuellt re15_enemy_bank_t ueber die UNVERAENDERTEN RE1.5-Parser.
 */
#ifndef RE2_EMS_H
#define RE2_EMS_H

#include <stddef.h>
#include <stdint.h>
#include "re15_enemy.h"
#include "re15_tim.h"

/* ---- CDEMD0.EMS Sektor-TOC (EXE @0x8009adf4, vendored gen/re2_ems_toc.inc) ---- */

#define RE2_EMS_KIND_MIN    0x10
#define RE2_EMS_KIND_COUNT  75        /* kinds 0x10..0x5A = 300/4 Eintraege        */

#define RE2_EMS_REC_OVL_SLOT1  0      /* AI-Overlay-Kopie, vorgelinkt @0x8010D000  */
#define RE2_EMS_REC_OVL_SLOT0  1      /* AI-Overlay-Kopie, vorgelinkt @0x80100000  */
#define RE2_EMS_REC_TIM        2      /* Standard-TIM (Binder-Index |2 @0x8001ab4c) */
#define RE2_EMS_REC_EMD        3      /* EMD-Record   (Binder-Index |3 @0x8001ab7c) */

/* TOC-Eintrag fuer (kind, rec): *out_off = Byte-Offset im EMS (Sektor*0x800),
 * *out_size = Byte-Groesse. Rueckgabe 0 ok; -1 ausserhalb der Tabelle oder
 * leerer Eintrag (Groesse 0 — z.B. kinds 0x11-0x13 haben keine eigenen
 * Overlay-Kopien, FUN_8001b710 klemmt 0x10..0x1F auf das 0x10-Overlay). */
int re2_ems_toc_entry(int kind, int rec, uint32_t *out_off, uint32_t *out_size);

/* Bounds-gecheckter Record-Zeiger in einen geladenen EMS-Puffer. 0 ok / -1. */
int re2_ems_locate(const uint8_t *ems, size_t ems_size, int kind, int rec,
                   const uint8_t **out_ptr, size_t *out_len);

/* RE2-EMD-Splitter: parst einen RE2-EMD-Record (dir@+0, count==8) in eine
 * re15_enemy_bank_t — ueber die unveraenderten RE1.5-Parser. Feld-Belegung
 * (STRUKTURELL nach Directory-Position, keine Semantik-Behauptung):
 *   skel/anim           = das Paar mit den MEISTEN EDD-Clips (dieselbe Regel wie
 *                         re15_emd_parse_container; EM010: Paar 2, 31 Clips)
 *   skel_loco/anim_loco = Paar 1 (dir[1]/[2]; EM010: 8 Clips, clip0=65 Frames)
 *   skel_own/anim_own   = Paar 2 (dir[3]/[4])
 *   skel_victim/anim_victim = Paar 3 (dir[5]/[6]; EM010: 17 Clips)
 *   md1                 = dir[7]
 * Bone-Struktur kommt IMMER aus dir[2] (Paar-2/3-EMRs sind kf-Pools, kf_ofs=8);
 * Re-Point wie re15_emd_parse_own_bank. `emd` muss resident bleiben (die Bank
 * aliast hinein); buf/pc_tex_slot/type setzt der Aufrufer. 0 ok / negativ. */
int re2_emd_parse_bank(const uint8_t *emd, size_t emd_size, re15_enemy_bank_t *eb);

/* Komplett-Lader: EMD-Record fuer `kind` im EMS lokalisieren + splitten; optional
 * das TIM des kinds (separater TOC-Record — NICHT im EMD!) via re15_tim_parse.
 * Bank aliast in `ems` (kein Kopieren — Aufrufer haelt das EMS resident). */
int re2_ems_load_bank(const uint8_t *ems, size_t ems_size, int kind,
                      re15_enemy_bank_t *eb, re15_tim_t *out_tim);

/* ---- WELLE G: HYBRID-RIG ("RE2 AI" + RE1.5-Modelle) --------------------------------------
 *
 * Nutzer-Auftrag 2026-08-19: der RE2-Modus soll wahlweise die RE1.5-GEOMETRIE (MD1-Mesh +
 * TIM-Textur) mit RE2-Gehirn UND RE2-Animation fahren. Animation MUSS RE2 bleiben — das RE2-
 * Gehirn adressiert Clip-Indizes, die es in den RE1.5-Baenken nicht gibt (RE2-EM010 Paar 1 =
 * 8 Clips gegen RE1.5-EM10 Paar 1 = 6; Frame-Laengen komplett verschieden).
 *
 * Damit RE2-Keyframes RE1.5-Meshes posieren koennen, muss die BONE-ORDNUNG uebersetzt werden.
 * Selbst gemessen (2026-08-19, Parser 1:1 wie emd_common.c/md1_common.c, Quellen
 * shared_assets/PSX/EMD/CDEMD0.EMS bzw. shared_assets/RE2/CDEMD0.EMS):
 *
 *   RE2 EM010/11/12/13/16/18  parent[] = [-1,0,1,2,3,1,5,6,0,0,9,10,0,12,13]   (Wurzel = BRUST)
 *      Slot 0 Brust | 1 Huefte | 2-4 R-Bein | 5-7 L-Bein | 8 Kopf | 9-11 R-Arm | 12-14 L-Arm
 *   RE1.5 EM10/11/12/16/18    parent[] = [-1,0,1,2,0,4,5,0,7,8,9,7,11,12,7]    (Wurzel = HUEFTE)
 *      Slot 0 Huefte | 1-3 R-Bein | 4-6 L-Bein | 7 Brust | 8-10 R-Arm | 11-13 L-Arm | 14 Kopf
 *   RE1.5 EM13 (Zombie Girl)  parent[] = [-1,0,1,2,0,4,5,0,7,7,9,10,7,12,13]
 *      wie oben, aber 8 Kopf | 9-11 R-Arm | 12-14 L-Arm (= RE2-Reihenfolge ab Slot 8)
 *   RE1.5 PL00.PLD (Leon) parent[] == RE2 EM010 — nur die RE1.5-ZOMBIE-Familie weicht ab.
 *
 * Zuordnung unabhaengig ueber die MD1-Geometrie belegt (Mesh-Index == Bone-Index,
 * emd_common.c:190; Zahlen = Tri-Vertices / Bounding-Box in PSX-Einheiten):
 *   Brust  RE2 Mesh 0 (41 v, y -852..132)  == RE1.5 Mesh 7  (29 v, y -735..120)
 *   Huefte RE2 Mesh 1 (15 v, y   -2..463)  == RE1.5 Mesh 0  (18 v, y   -2..433)
 *   Kopf   RE2 Mesh 8 (68 v, y -520.. 44)  == RE1.5 Mesh 14 (42 v, y -448.. 64)
 *   Fuss   RE2 Mesh 4 (16 v, x -122..374)  == RE1.5 Mesh 3  (16 v, x -132..361)
 *
 * Das Um-Wurzeln Huefte<->Brust ist GEOMETRISCH KOSTENLOS: der Versatz zwischen beiden ist in
 * BEIDEN Rigs exakt (0,0,0) (RE1.5 EM10 Bone 7, RE2 EM010 Bone 1) — die Kante dreht nur ihre
 * Richtung um, ihre Laenge ist null.
 *
 * HUND 0x20: RE1.5 15 Bones / kf 80 gegen RE2 17 / kf 92 — RE2 gibt jedem VORDERBEIN ein
 * drittes Segment (Slots 7 und 10, Versatz (-21,520,0)). Die uebrigen 15 Slots decken sich
 * (Bone-Offsets bis auf +-2 Einheiten identisch). Die zwei Extra-Slots haben KEIN RE1.5-
 * Gegenstueck (-1): RE1.5 traegt Unterarm+Pfote in EINEM Mesh (Slot 6/8, 26 Vertices, gegen
 * RE2 18+12). => im Hybrid bleibt die Hundepfote STARR; das RE2-Pfotengelenk wird nicht
 * gezeichnet. Bewusst dokumentierte Abweichung, NICHT byte-true.
 * KRAEHE 0x21 (13/72), SPINNE 0x25 (20/104), BABY 0x26 (1/20): parent[] identisch,
 * Bone-Offsets identisch (Spinne byte-gleich) => Identitaets-Permutation.
 */

/* RE2-Bone-Slot -> RE1.5-Bone-/Mesh-Index fuer `kind`. Rueckgabe = Slot-Zahl (== RE2-
 * bone_count) oder -1, wenn fuer den kind kein Hybrid definiert ist. */
int re2_hybrid_perm(int kind, const int8_t **out_perm);

/* Baut die Bank `eb` (frisch aus re2_ems_load_bank) zur HYBRID-Bank um:
 *   - eb->md1 := *md15                        (RE1.5-Geometrie)
 *   - eb->mesh_remap := Permutation, remap_ok := 1
 *   - die Bind-Positionen ALLER vier Skelett-Kopien (skel/skel_loco/skel_own/skel_victim —
 *     re2_emd_parse_bank parst dir[2] je Paar erneut) werden auf die RE1.5-Werte umgesetzt,
 *     wobei die RE2-HIERARCHIE erhalten bleibt (die RE2-Keyframes indizieren RE2-Slots).
 *     Regel pro Kante (i, parent p) in RE2-Slots, m = perm:
 *       parent15[m[i]] == m[p]  -> pos := pos15[m[i]]           (gleiche Kante)
 *       parent15[m[p]] == m[i]  -> pos := -pos15[m[p]]          (umgekehrte Kante = Um-Wurzelung)
 *       sonst / m < 0           -> RE2-Wert bleibt stehen       (gezaehlt in *out_unmapped)
 * Rueckgabe 0 ok, negativ = kein Hybrid fuer den kind / Bone-Zahl passt nicht.
 * `out_unmapped` (optional) = Zahl der Kanten ohne RE1.5-Entsprechung — PIN-Wert der Tests
 * (Soll: 0 fuer alle Typen; die beiden Hunde-Extra-Slots sind KEINE Kanten mit m>=0). */
/* Wurzelhoehen-Korrektur des Hybrid-Rigs (siehe re15_emd.h `root_y_fix`).
 * reset() gibt den Tabellen-Pool frei (beim Bank-Reset aufrufen); stats() liefert Belegung
 * und Kapazitaet und gibt != 0 zurueck, wenn eine Tabelle wegen Platzmangel AUSGEFALLEN ist
 * (dann schwebt der betroffene Typ wieder — die Wache pinnt das). */
void re2_hybrid_rootfix_reset(void);
int  re2_hybrid_rootfix_stats(int *out_used, int *out_capacity);

int re2_hybrid_apply(re15_enemy_bank_t *eb, int kind,
                     const re15_md1_t *md15, const re15_emd_skeleton_t *skel15,
                     int *out_unmapped);

/* ---- ENEMSE.VBS Bank-TOC (EXE @0x800a7b1c) + SE-Map-Dekodierung -------------- */

#define RE2_ENEMSE_BANK_COUNT 73      /* == Paar-Tabelle @0x800a7400 (FUN_80052b38) */

typedef struct {
    uint32_t edt_off, edt_size;       /* EDT-Record: SE-Map @+0, VH @u32[size-8]    */
    uint32_t vbd_off, vbd_size;       /* VBD-Record: der SPU-/VB-Body               */
} re2_enemse_rec_t;

/* Bank-Eintrag (Byte-Offsets im VBS: 24-bit-Sektor*0x800; das CD-Flags-Byte +7
 * aus FUN_8005a09c/DAT_800d531e wird maskiert). 0 ok / -1. */
int re2_enemse_toc_entry(int bank, re2_enemse_rec_t *out);

/* Dekodierter SE-Map-Eintrag (byte-true FUN_8005bd6c; Map = EDT[se_id*4]):
 *   Eintrag 0xFFFFFFFF                  -> silent=1 (Original: return ohne KeyOn)
 *   b0 bit7 -> VAB-ID-Override b0&0x7F  (sonst aktuelle ENEM-VAB DAT_800d4c4b)
 *   b1&0x7F = Program, b2>>4 = Tone, b2&0x0F = Kanal-Prioritaet (FUN_8005c92c-Gate),
 *   b3&0x1F = SE-Kanal, b3>>5 = ZUSAETZLICHE konsekutive Tones/Kanaele
 *   (Schleife am FUN_8005bd6c-Ende: tone+1/chan+1 je Extra-Layer).
 * Der +0x10-Versatz bei Actor-Flag 0x2000 (zweite Map-Haelfte = zweiter kind des
 * Raum-Paars) passiert im AUFRUFER (re15_audio_re2_enemy_se), nicht hier. */
typedef struct {
    int silent;                       /* 1 = Eintrag 0xFFFFFFFF                     */
    int vab_override;                 /* -1 = keiner, sonst 0..0x7F                 */
    int prog, tone, chan, prio, extra;
} re2_enemse_se_t;

void re2_enemse_decode_entry(uint32_t entry, re2_enemse_se_t *out);

/* ---- ENEMSE-BANK-WAHL (byte-true FUN_80052b38, Belege in re2_ems.c) --------------------
 * Der Raum sammelt beim Spawn sein kind-PAAR (A/B, @0x8005728c-b8), FUN_80052b38 sucht damit
 * die Zeile in der Paar-Tabelle @0x800a7400; der ZEILENINDEX IST die ENEMSE-Bank
 * (DAT_800d424b -> Lader FUN_8005a09c). Rueckgabe 0xFF = kein Treffer = KEINE Bank.
 * *out_flag_kind = der kind, dessen Entities im Original word0 |= 0x2000 bekommen
 * (= die zweite Map-Haelfte / se_id+0x10 in FUN_8005bd6c); -1 = keiner. */
int  re2_enemse_select_bank(int kindA, int kindB, int *out_flag_kind);
int  re2_enemse_pair_row(int bank, int *out_k0, int *out_k1);
void re2_enemse_room_reset(void);          /* FUN_80052f3c @0x80053028/30 */
void re2_enemse_room_add_kind(int kind);   /* Spawn @0x8005728c-b8 */
void re2_enemse_room_kinds(int *out_a, int *out_b);

#endif /* RE2_EMS_H */
