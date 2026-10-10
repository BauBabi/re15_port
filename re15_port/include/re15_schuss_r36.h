/*
 * re15_schuss_r36.h — Runde 36 Spur C: der allgemeine Schusspfad (alle 22 Waffen des
 * Spieler-Resolvers FUN_80011f50).
 *
 * Dossier: analysis/befunde_runde36/C_schuss.md.
 *
 * (1) SICHTLINIE (RE1.5, Auslieferungsstand): FUN_80011f50 prueft NACH der Kandidatenschleife
 *     und VOR der Typ-Pruefung/dem +0x93-Latch die Sichtlinie vom Spieler zum GEWINNER:
 *         80012160: beq s1,zero,0x8001253c      ; kein Gewinner -> 0
 *         80012164: addiu s0,s1,52              ; s0 = Gewinner + 0x34 (Lage)
 *         80012168: jal 0x8001b9b4              ; FUN_8001b9b4(&Lage)
 *         8001216c: addu a0,s0,zero
 *         80012170: bne v0,zero,0x80012540      ; blockiert -> return 0 (kein 2. Kandidat)
 *         80012174: addu v0,zero,zero
 *     FUN_8001b9b4: delta = Ziel - DAT_800ac784(+0x34/+0x38/+0x3c) @0x8001b9d8-ba10, dann fuer die
 *     Regionen 3..0 `ori a2,zero,0xf00` / `jal 0x8003dcc4` / `ori a3,zero,0x300` @0x8001ba1c-24,
 *     ODER-verknuepft @0x8001ba28. DAT_800ac784 ist beim Spielerschuss der Spieler (FUN_80031c44).
 *     FUN_8003dcc4 filtert Zellen auf Band == Spieler+0x82 (`lbu v1,130(cur)` @0x8003de60, `sra
 *     v0,v0,28` @0x8003de68) und Klasse 3 (`and 0xf00` / `bne 0x300` @0x8003de7c-94) und testet den
 *     Strahl (alle Koordinaten /0x12) nur in x/z gegen beide Zelldiagonalen — keine Hoehe. Der
 *     Konferenztisch ROOM1140 (SCA-Zelle 6, `01 ff 00 03` @RDT 0x5d0) sperrt deshalb jede Waffe,
 *     auch das Messer (die Tabelle @0x8006e548 schickt w1/w2 durch dieselbe Funktion).
 *     Port: re15_los_ray_blocked (enemy_ai_common.c, byte-true FUN_8003dcc4) mit dem SPIELER als
 *     Strahl-Ursprung (sein `floor` = +0x82) und dem Gewinner als Ziel, Regionen 3..0.
 *
 * (2) FRESSER-SPERRE (RE2 Retail, weil RE1.5 die Fresser-Zustandsmaschine nicht fertig hat):
 *     FUN_800410CC (alle Schusswaffen, `jal 0x800410CC` @0x80043AFC) wirft in der Kandidaten-
 *     schleife `lbu v0,467(s2)` / `bne v0,zero,0x80041300` @0x80041270-78 (+0x1D3 != 0) und
 *     `lhu v0,270(s2)` / `andi v0,v0,0xc000` / `bne` @0x80041290-9c (+0x10E) heraus; EXEC[8] P0
 *     setzt +0x1D3 |= 0x80 @0x80103c04-14 (EMZ0.BIN), solange der Zombie frisst. Im Port fuehrt
 *     re15_re2z_hit_filter_apply (enemy_ai_re2_zombie.c) diese Gates ueber +0x93 Bit 0; die
 *     v0.3.5-Ausnahme fuer die Spawn-Pose gilt seit Runde 36 nicht mehr fuer EXEC[8] (Nutzer:
 *     "Im Original geht beides nicht -> angehen").
 */
#ifndef RE15_SCHUSS_R36_H
#define RE15_SCHUSS_R36_H

#include <stdint.h>
#include "re15_actor.h"

/* FUN_8003dcc4-Zwilling (enemy_ai_common.c): Strahl von `e` (Ursprung, Band = e->floor) nach
 * `ziel` gegen die Klasse-3-Zellen der SCA-Region `step` (0..3); re2_maske 0 = ungefiltert.
 * Runde 36 Spur C: fuer den Spieler-Schuss exportiert (vorher static). */
int re15_los_ray_blocked(const re15_actor_t *e, const re15_actor_t *ziel, int step,
                         unsigned re2_maske);

/* FUN_8001b9b4-Zwilling fuer den Spieler-Schuss: 1 = Sichtlinie Spieler -> Ziel durch eine
 * Klasse-3-Zelle im Band des Spielers blockiert (Regionen 3..0, @0x8001ba1c-28), sonst 0.
 * Ohne geladene SCA (g_room_rdt_ok == 0) immer 0 — wie `sltu fp,v0` @0x8003dd94 bei leerer Region. */
int re15_schuss_r36_sichtlinie_blockiert(const re15_actor_t *pl, const re15_actor_t *ziel);

/* Mess-Zaehler (nur fuer Sonden): wie oft die Sichtlinie seit dem letzten Reset einen Gewinner
 * verworfen hat (`bne v0,zero,0x80012540` @0x80012170). */
int  re15_schuss_r36_sichtlinie_verworfen(void);
void re15_schuss_r36_sichtlinie_reset(void);
/* ---- Punkt 2: BLUT-ANKER des RE2-Zombie-Treffers (Dossier C_schuss.md §2) --------------------
 * Die drei Treffer-Emitter in EMZ0.BIN rufen FUN_8001BF10(id, Gier, a2, a3) mit a2 = Part-WELTMATRIX
 * (+0x198 + part*172 + 72) und a3 = VERSATZ im Part-Rahmen. FUN_8001BF10 legt a3 in Record +44/+48
 * ab (`lw v0,0(a3)` / `sw v0,44(t0)` / `lw v0,4(a3)` / `sw v0,48(t0)` @0x8001BFE8-FFC; a3 wird nur
 * GELESEN) und die Part-Matrix in +76..+107 (@0x8001C03C ff.); der RE2-Lagen-Tick 0x8001D894 rechnet
 * RT = Record+76 (`addiu v1,v0,76` @0x8001DB74, ctc2), V0 = Record+44 (`addiu v0,v0,44` @0x8001DBA0,
 * lwc2), MVMVA 0x4A486012 @0x8001DBB4, dazu +96/+100/+104 (`lw v1,96(a0)` ... `addu` @0x8001DBD0-DC1C)
 * => Welt = Anker.R * a3 + Anker.T. (Das fruehere Zitat @0x8001a248 ist die RE1.5-ESP-Maschine.)
 *   Wurzel-Flinch @0x801050B0-158 : Zone 0 -> Part 1 (+244), a3 = (0, 800, 0), id 6096
 *                                   sonst  -> Part 0 (+72), a3 = (P8.x, P8.y + 500, P8.z), id 4096
 *   Haupt-Handler P0 @0x80105650-708: Zone 0 -> Part 1, (0, 800, 0)
 *                                   sonst  -> Part 0, (P8.x - 100*+0x15A, P8.y + 300, P8.z), Gier - 2048 bei Ruecken
 *                                   (+0x15A = 1 @0x801055BC, -1 bei +0x1D0 & 0x20 @0x80105624-38)
 *   Stagger P0 @0x80105D14-DBC     : Zone 0 -> Part 1, (0, 800, 0)
 *                                   sonst  -> Part 0, (P8.x - 100*+0x16A, P8.y + 300, P8.z)  (`lb v1,362` @0x80105D2C)
 * P8 = lokale Translation von Part 8 = +0x198 + 8*172 + 0x2C (`lw 1420/1424/1428(a1)` @0x80105698-B8 /
 * @0x80105D44-64 / @0x80105124-40) = der Bind-Versatz des Teils (bone_relative_pos[8] der geladenen Bank);
 * Zone = +0x1D2 % 3 (0 Beine / 1 Rumpf / 2 Kopf, Maskentabelle @0x800A6DB4 `srl s7,v1,1` @0x8004149C).
 * Port vorher: Lage = Knochen-Weltposition OHNE a3 -> immer am Becken (gemessen probe_r36_blut: Rumpf-
 * Treffer 505 zu tief, Bein-Treffer 794 zu hoch).
 *
 * Lage = T + R * a3 mit (R, T) = re15_schuss_r36_gezeichnet. Rueckgabe 0 = ohne Bank (out = Aktor-Wurzel
 * + Ryaw * a3), 1 = nur T (a3 NULL oder 0), 2 = mit Versatz.
 *
 * NACHBESSERUNG 2 (Abnahme 1 M1, Dossier C_schuss.md §9): DER ANKER IST FUER JEDEN EMITTER DIE GEZEICHNETE MATRIX —
 * auch fuer a3 = 0 (Zensus-Gruppe B), die vorher die ROHE Hauptbank-Pose nahm (re15_enemy_bone_world_pos, QUERY).
 *   FUN_8001BF10: a3 == 0 -> nur `sw zero,44(t0)` / `sw zero,48(t0)` @0x8001C000-04; die Matrix kommt fuer alle aus a2
 *   (`lw` 0..28(a2) -> slot+76..+104 @0x8001C03C-78). a2 = Part-Record+72 schreibt NUR der Zeichner FUN_80027434
 *   (Part-Walk `addiu a3,s2,72` @0x80027390 / `addiu s0,s2,72` @0x800273B0, Stride 172): R = R_Eltern * R_lokal(Record+0x18)
 *   (MVMVA @0x80027504/48/8C, `sh` -> Record+72 @0x8002751C-A8), t = Eltern * t_lokal(Record+0x2C) + TR (`swc2` -> +92..+100
 *   @0x800275D4-DC); Flag 0x40 = eigene Matrix, keine Neukomposition (@0x80027498-9C), die Teile-Physik fuehrt sie
 *   (`jal 0x80028ad8`, a1 = Record+72 @0x800276A0-A4). Record+0x18 fuellt der Anim-Durchlauf aus dem (EMR,EDD)-Paar der
 *   Aufrufstelle (`sw a2,376(a0)` / `jal 0x80029614` @0x800295F8-FC) mit der Blende a3 * +0x14E (@0x800296A8-BC).
 *   Port-Zwilling (re15_schuss_r36_gezeichnet) = was der Port-Renderer zeichnet (platform/pc/main.c): Bank wie dort
 *   (9798-9925: Loco-Bank in den Paar-1-Phasen, Kanaele, Sitz-Import; clip_override bei +0x1C4 & 4), Crossfade
 *   (9960, auf einer Schatten-Kopie), Unterhaelfte Bones 1..6 aus re15_re2z_lower_kf (10195-10220), Ragdoll-Bodenanker
 *   (10315-10330) und fuer ein geseedetes 0x40-Teil dessen eigene Matrix (10339, re15_re2z_gore_part_matrix_peek).
 *   PORT-KONVENTION (unveraendert, §2.3/§8.2): die Pose des LAUFENDEN Bilds statt der des Vorbilds. */
int re15_schuss_r36_blut_anker(const re15_actor_t *e, int bone, const int32_t a3[3], int32_t out[3]);

/* Welt-Rotation R (Q12, zeilenweise; Ryaw * R_part bzw. die eigene Matrix des Teils) und Welt-Translation T des
 * Knochens `bone`, wie der Port-Renderer ihn in diesem Bild zeichnet (s. o.). Rueckgabe 0 = ohne Bank/Pose (T =
 * Wurzel, R = Ryaw), 1 = Skelett-Pose, 2 = eigene Matrix eines freifliegenden Teils. */
int re15_schuss_r36_gezeichnet(const re15_actor_t *e, int bone, int32_t R[9], int32_t T[3]);

/* enemy_ai_re2_zombie.c (Nachbesserung 2): eigene Matrix des Teils am Bone-Slot `bone`, wenn es Flag 0x40 + Bit 0 traegt
 * UND der Renderer sie schon geseedet hat (re15_re2z_gore_part_matrix) — reiner Lesezugriff, KEIN Physikschritt. 1 = rot/
 * trans geschrieben (Welt), 0 = keine eigene Matrix (Skelett-Pose gilt; vor dem ersten Zeichnen ist das genau die Pose,
 * aus der der Renderer seedet). */
int re15_re2z_gore_part_matrix_peek(const re15_actor_t *e, int bone, int32_t rot[9], int32_t trans[3]);

/* Mess-Ring (nur Sonden; die Engine liest ihn nicht): JEDER Spawn der RE2-Zombie-Blut-/Gore-Emitter
 * (re2z_blood_fx_scaled, re2z_gore_fx_*) mit Anker-Part, Weltlage und dem a3-Versatz (NULL = ohne,
 * dann a3 = 0 und mit_a3 = 0). `re15_schuss_r36_fx_zahl` zaehlt monoton; `_holen(nr, ...)` liefert den
 * Spawn Nummer `nr`, solange er noch im Ring liegt (1 = ok, 0 = ueberschrieben/unbekannt). */
#define RE15_SCHUSS_R36_FX_RING 64
void     re15_schuss_r36_fx_merke(int part, const int32_t pos[3], const int32_t *a3);
unsigned re15_schuss_r36_fx_zahl(void);
int      re15_schuss_r36_fx_holen(unsigned nr, int *part, int32_t pos[3], int32_t a3[3], int *mit_a3);

/* ---- Nachbesserung 3 (Abnahme 2 M1, Dossier §10.5): Anker-ZEITPUNKT der Spawns eines Gegner-Ticks --------------
 * Original: Anker a2 = Record+72, vom Zeichner im VORIGEN Bild geschrieben — also NACH dem Schwanz jenes Ticks
 * (RE2-Zombie-Root EMZ0: Zustands-jalr @0x801004E8, dann `jal 0x80035530` @0x8010062C und der SCA-Resolver
 * `jal 0x8003567c` mit a1 = 0x400 @0x80100638; dessen Sperren word0 & 8 @0x80035694-98 / +0x1E4 @0x800356A0-A8 setzt der
 * Kriecher-Griff nicht). Der Port rechnet den Anker im LAUFENDEN Bild schon im Handler (PORT-KONVENTION §2.3/§8.2);
 * Schub und SCA-Klemme im Schwanz von run_all verschieben den Aktor danach noch. Der Kriecher-Griff setzt die Lage
 * ABSOLUT (FUN_80015cb8: +0x38/+0x3C/+0x40 = +0x164/+0x166/+0x168 + R*Wurzelversatz @0x80015D58-90); liegt dieses Ziel
 * hinter einer Wand, holt die Klemme ihn im selben Tick zurueck (gemessen bis 665) und der Spawn sass an der verworfenen
 * Lage. Nachfuehrung: die Spawns dieses Aktors aus diesem Bild werden nach der Klemme um die Lage-Differenz des
 * SCHWANZES (Handler-Ende -> Klemme) verschoben — nicht bei Ankern mit eigener Matrix (freifliegende Teile haengen nicht
 * an der Wurzel). Bewegung IM Handler nach dem Spawn (z.B. MG-Schub @0x80108110-48) bleibt wie im Original vor dem
 * Spawn unberuehrt. Mess-Ring (Sonden) wird mitgefuehrt. */
void re15_schuss_r36_fx_nach(const re15_actor_t *e);          /* direkt nach dem ESP-Spawn eines re2z-Emitters */
void re15_schuss_r36_nachfuehr_marke(const re15_actor_t *e);  /* run_all: nach dem Handler, vor dem Schwanz */
void re15_schuss_r36_nachfuehren(const re15_actor_t *e);      /* run_all: nach der SCA-Klemme */

/* ---- Punkt 3: KRAEHE gegen Super Redhawk (RE2 Retail, EMOVL21_S0.BIN, Dossier C_schuss.md 3) ---
 * HURT-Zeilen-Dispatch @0x80104A18 auf +0x5 (RE2-Waffen-Id): [5]/[6] (Magnum/Custom Magnum), [9]/[10]/[11]
 * (GL-Runden) und [17] (Rakete) -> GIB 0x80102CA0; der Super Redhawk (w7) ist RE2-Zeile 5.
 * GIB 0x80102CA0: +0x146 = 0 (@0x80102CB8), Peilung 0x800154AC(PL, self) (@0x80102CD4), rand -> Maske s2
 * (@0x80102CDC), dann die 13 Part-Zeilen [+0x198] + i*172:
 *   Zeile 0 immer WURF (@0x80102CE8-F0); Bit 1: 1,2 Wurf sonst Wirbel; Bit 2: 3..5; Bit 4: 6..8;
 *   9/10 immer Wurf (@0x80102E20-34); Bit 8: 11; Bit 0x10: 12 (@0x80102CF4-E8C)
 *   WURF 0x80102EF4   : +0x70 = 0x00101040, +0x9C = 800, +0x9E = 10, +0x98 = Peilung, +0xA0 = 0, +0x9A = 0,
 *                       +0xA4 = -100, Flags |= 0x4A  -> Drift-Physik FUN_80028DAC (Bit 0x08): Tempo 800 - 100/Bild,
 *                       Fall +10/Bild, nach 29 Bildern Flags = 0 (`sw zero,0` @0x80028DDC) = das Teil VERSCHWINDET
 *   WIRBEL 0x80102F34 : Flags |= 0x1062, +0x38 = Peilung + rand<<2, +0x86 = 0, +0x3C = (rand&0x7f)+100, +0x3A = 0,
 *                       +0x79 = 20, +0x3E = 0, +0x40 = rand<<4, +0x42 = 0 -> Wurf-Physik FUN_80028AD8 (Bit 0x20):
 *                       Fall +20/Bild, Aufschlag v/4, beim ZWEITEN Bodenkontakt Bit 0 weg (`and v0,v0,-2`
 *                       @0x80028CA0) = das Teil VERSCHWINDET
 *   danach 3x Federburst 0x8010459C (@0x80102E90-A4), Federn (rand&3)|8 (@0x80102EA8-BC), 0x80018FB0,
 *   Zustand (7,1) (@0x80102EC8-D4) = GIB-Leiche 0x80103C0C: Schatten-Record frei, keine Lache.
 * => In RE2 zerplatzt die Kraehe beim Magnum-Treffer ebenfalls in ihre 13 Teile und ist danach komplett weg
 *    (kein Kadaver, keine Lache); die Teile fliegen ~0,5-1 s sichtbar auseinander. Der Port verbarg das
 *    Netz bisher SOFORT (crow_hide) und setzte 13 Blut-Splatter als Stand-in; jetzt laufen die 13 Teile
 *    ueber dieselbe Part-Bruecke wie der Zombie-Zerleger (re15_re2z_gore_part_matrix, Physik DAC/AD8). */
#define RE15_SCHUSS_R36_KRAEHE_PARTS 13          /* 13 Zeilen @0x80102CE8-E8C; RE2-EM021-Rig = 13 Bones */
/* Modellblock-Seed beim RE2-INIT der Kraehe (wie re15_re2z_part_reset: Bit 0, Tinte 0x808080, Mesh = i). */
void re15_schuss_r36_kraehe_parts_seed(re15_actor_t *e);
/* Der GIB: a1 = Peilung (0x800154AC(PL, self)), m = der Masken-Draw (rand @0x80102CDC); verbraucht die drei
 * Draws je Wirbel-Zeile in der Original-Reihenfolge. */
void re15_schuss_r36_kraehe_gib(re15_actor_t *e, int16_t a1, uint32_t m);

#endif /* RE15_SCHUSS_R36_H */
