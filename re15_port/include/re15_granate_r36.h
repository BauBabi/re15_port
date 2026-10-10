/* re15_granate_r36.h — Runde 36 Spur D: die Handgranate — (1) KEIN Feuer an Zombies bei der
 * Explosiv-Granate (NUTZER-VORGABE), (2) Wurf-Aussetzer ("Granate wird manchmal nicht geworfen").
 *
 * Dossier: analysis/befunde_runde36/D_granate.md. Adressen = RE2 Leon (info/re2leon/PSX.EXE bzw.
 * COMMON/BIN/EMOVL10_S0.BIN = Zombie-Overlay @0x80100000) und RE1.5 PSX.EXE, selbst disassembliert.
 *
 * (1) FEUER NACH DER HE-EXPLOSION. Der Zerreiss-Tod der RE2-Zombie-Familie (FUN_80108BEC, DEATH[9][1]
 *     @0x8010CD68+4) ruft fuer die Zeilen 9 (HE/GL-Explosiv) und 17 (Rakete) den Spreng-Russ FUN_8010640C
 *     (`beq v1,9` @0x80108cc8, `bne v1,17` @0x80108cd0, `lhu 270 / andi 0x80 / bne` @0x80108cd8-e4,
 *     `jal 0x8010640c` @0x80108cec; dieselbe Leiter @0x801051E8-208 und @0x801084A0-B8). FUN_8010640C
 *     spawnt ZWEI Flammen der RE2-Klasse 5 (= RE2-CORE00-Id 5 FEUER, Sub 3 = Anim 0..9 mit Status 0x800
 *     "folgt dem Anker"): 0x05032710 am Part 0 mit a3 = (0,200,0) (`lui a0,0x503 / ori a0,a0,0x2710`
 *     @0x80106418-1c, `addiu v0,zero,200` @0x80106424, `jal 0x8001bf10` @0x80106440, Anker +72 @0x80106444)
 *     und 0x050313E8 am Part 3 (`lw v0,516(a2) / andi 0x1 / beq` @0x80106450-5c, `lui a0,0x503 / ori
 *     a0,a0,0x13e8` @0x80106460-64, `addiu a2,a2,588` @0x8010646c, `jal 0x8001bf10` @0x80106470); danach
 *     die Russ-Tinten (@0x80106478-f8). Der Port-Dekoder bildet Klasse 5 auf RE1.5-CORE00-Id 8 (FEUER)
 *     ab — gemessen (Dossier §2.1): je zerrissenem Zombie 2 Flammen, 211+ Bilder, erloeschen nicht.
 *     RE2 Retail laesst die HE-Leiche also brennen. NUTZER-VORGABE (AUFTRAG.md Punkt 1): "Bei normalen
 *     Granaten keinen Feuer-Effekt an den Zombies, nur das Zerplatzen der Koerperteile; Feuer nur bei
 *     den incendiary Granaten." FORM der Abweichung: die beiden Flammen-Spawns @0x80106418-44 und
 *     @0x80106450-74 entfallen (fuer alle Aufrufer von FUN_8010640C = Zeilen 9/17, beide explosiv);
 *     die Russ-Tinten @0x80106478-f8, das Zerreissen (0x80108BEC) und das Wegschleudern (0x80109610)
 *     bleiben; die Brand-Verkohlung FUN_80106128 (Zeile 10 = Brandgranate 0x0B / Art 4, Zeile 16
 *     Flammenwerfer) und die Saeure FUN_80106310 (Zeile 11) bleiben unveraendert.
 *
 * (2) WURF-AUSSETZER. Gemessen (Dossier §3.1, r1_sweep): 8 Abzuege, R1 nach 0,0..0,7 s losgelassen -> 2 Wuerfe,
 *     8 Granaten verbraucht. Ursache = RE1.5 Standard-Feuer-Handler Substate 2 (LAB_80033460), jedes Bild:
 *     `andi v0,a0,0x100 / bne` @0x80033600-04 (R1 gehalten -> weiter), sonst `lbu 0x800aca5d / lbu 0x800acae9`
 *     (Waffe, Cursor) und `lbu v0,0(at)` @0x80033634 = Byte 2 des Waffensatzes @0x80074092+(id-1)*5 (Waffe 9:
 *     @0x800740b8 `18 30 0a 01 00` -> 10), `sltu v0,v0,a0 / beq` @0x8003363c-40, `ori v0,zero,0x3 / sh
 *     v0,0x800aca5a` @0x80033644-4c (Substate 3 LOWER) und `j 0x800337ac` @0x80033650 = RETURN vor dem
 *     Granaten-Spawn @0x80033680-7a8 (Cursor 19 HOCH `ori v0,zero,0x13` @0x80033690, 22 MITTE `0x16`
 *     @0x800336a4, 24 TIEF `0x18` @0x80033758). Der Munitionsabzug faellt aber schon im Rueckstossbild 0
 *     (`jalr v0` @0x800334e0 -> 0x80033B38 `jal 0x8004eae4` @0x80033b40). R1 loslassen zwischen Cursor 11 und
 *     18/21/23 = Granate weg, kein Wurf — im Original wie im Port (byte-true).
 *     Beta -> Retail: die Abbruch-Regel ist fuer Schusswaffen gebaut (Schuss im Bild 0, Abbruch ab 11 kostet
 *     nichts); die Handgranate ist eine RE1.5-eigene Waffe ohne RE2-Gegenstueck, ihr Wurfkoerper haengt am
 *     Loslass-Bild der Animation. Das Retail-Vorbild (RE2-Granatwerfer) gibt die Runde im Rueckstossbild 1
 *     ab (werfer_r35.c, Tabelle @0x800A6FDC, Aufruf @0x800431ac-cc) — ein Abzug ist dort immer ein Schuss.
 *     NUTZER-VORGABE/PORT-WAHL: fuer die Granate (Ids 9/10/11) liegt die Abbruch-Schwelle auf dem Spawnbild
 *     der aktuellen Zielhoehe (19/22/24) statt auf 10; die Form der Regel (`Cursor > Schwelle` -> LOWER)
 *     bleibt, nur die Schwelle folgt dem "Schuss-Bild" wie bei den Schusswaffen.
 *
 * (2b) VOLLER POOL IM SPAWNBILD. Der Spawner FUN_80019700 sucht die Plaetze 0..95 (`sltiu` @0x8001978c)
 *     und liefert bei vollem Pool 0xFF (`ori v0,zero,0xff` @0x800197c8-cc) — der Wurf unterbleibt, die
 *     Granate ist weg. Gemessen (Dossier §3.1): nach der HE-Explosion an zwei Zombies 95/96 Plaetze
 *     (F120..F130, RE2-Gore-Schub Blut 0/0 + Brocken 5/7 auf dem RE1.5-Pool = Port-Konstruktion), ein
 *     Spawnbild in diesem Fenster (Abzug 22 Bilder vorher) verliert die Granate. PORT-WAHL: das Blut-
 *     Teilchen mit dem kleinsten Index raeumt den Platz (re15_granate_r36_platz_freimachen).
 */
#ifndef RE15_GRANATE_R36_H
#define RE15_GRANATE_R36_H

#include <stdint.h>

/** Punkt 1 — Gate der beiden Flammen-Spawns des Spreng-Russ FUN_8010640C (@0x80106418-44 Part 0,
 *  @0x80106450-74 Part 3). Rueckgabe 1 = Flammen spawnen (RE2 Retail), 0 = KEINE Flammen (NUTZER-
 *  VORGABE Runde 36: Feuer nur bei der Brandgranate). `zeile` = RE2-Treffer-Zeile +0x5 des Zombies
 *  (9 = HE/GL-Explosiv, 17 = Rakete — die einzigen Aufrufer der Russ-Funktion). Kein Umgebungs-
 *  schalter: die Vorgabe gilt immer; die Funktion existiert, damit die Sonde die Zuordnung (welche
 *  Zeile = welche Granatenart) pruefen kann und der Haken in enemy_ai_re2_zombie.c eine Zeile bleibt. */
int re15_granate_r36_russ_flammen(unsigned zeile);

/** Zaehlung fuer Sonden: wie oft der Spreng-Russ seine Flammen UNTERDRUECKT hat (seit Reset). */
unsigned re15_granate_r36_russ_unterdrueckt(void);
void     re15_granate_r36_zaehler_reset(void);

/** Punkt 2 — Rueckstoss-Abbruch-Schwelle der Granate (Ids 9/10/11; Spawn-Gate `id == 9` @0x8003368c, die
 *  Port-Zuordnung E1 fuehrt 10/11 ueber denselben Wurf): das Spawnbild der Zielhoehe `elev` (+1 HOCH -> 19
 *  @0x80033690, 0 MITTE -> 22 @0x800336a4, -1 TIEF -> 24 @0x80033758). Andere Ids: -1 = kein Eingriff (der
 *  Aufrufer nimmt Byte 2 @0x80074092+(id-1)*5). Der Aufrufer prueft `Cursor > Schwelle` (sltu @0x8003363c). */
int re15_granate_r36_recoil_break(int id, int elev);

/** Spawnbild der Granate je Zielhoehe (19/22/24, @0x80033690/@0x800336a4/@0x80033758) — fuer Sonden. */
int re15_granate_r36_spawnbild(int elev);

/** Punkt 2b — voller Pool im Spawnbild (FUN_80019700 `ori v0,zero,0xff` @0x800197c8-cc = Wurf verloren,
 *  Granate weg). PORT-WAHL (Dossier §3.4): das Blut-Teilchen (Effekt-Id 0) mit dem kleinsten Index raeumt
 *  den Platz; Rueckgabe = freigegebener Platz oder -1 (kein Blut im Pool = Original-Verhalten). Aufrufer:
 *  re15_esp_granate_spawn, nur wenn re15_esp_fx_count() == RE15_ESP_FX_MAX. */
int      re15_granate_r36_platz_freimachen(void);
unsigned re15_granate_r36_platz_geraeumt(void);   /* Zaehlung fuer Sonden */

/* (2c) NACHBESSERUNG 1 (Abnahme 0, M1/M2) — VERBRAUCH = ABGANG. Gemessen (Dossier §6.3, Lauf c2 ROOM1140):
 *     zweiter Abzug F105 (Menge 4 -> 3), Zombie 0x11 packt Leon F113, Spawnbild waere F129 -> 2 Abzuege, 1 Wurf,
 *     Menge 5 -> 3. Original: Mengen-Abzug im Rueckstossbild 0 (`jalr v0` @0x800334e0 -> Entlade-Handler
 *     0x80033B38/58/78 = nur `jal 0x8004eae4` @0x80033b40/60/80), Abgang erst im Bild 19/22/24 (`jal 0x80019700`
 *     @0x800336ec/@0x80033748/@0x800337a4); der Griff ersetzt das Kommando ohne Ziel-Gate (STAGE1.BIN
 *     `sb v1(=2),0x800aca58` @0x80113b00, `sh zero,0x800aca5a` @0x80113b30) -> Granate weg, kein Wurf.
 *     Retail-Vorbild RE2-Granatwerfer: Munition beim Eintritt (`jalr` @0x80042d3c -> 0x80043fcc `jal 0x8006a0cc`
 *     @0x80043fdc), Geschoss im Bild 1 (`lbu v1,333(s0) / addiu v0,zero,1 / bne` @0x80044b98-a0) = 1 Bild Fenster.
 *     ORCHESTRATOR-ENTSCHEIDUNG gemaess Nutzer-Wortlaut ("wird nicht geworfen"): eine Granate wird NUR verbraucht,
 *     wenn sie wirklich geworfen wird. FORM: fuer die Wurf-Ids 9/10/11 entfaellt der Abzug des Entlade-Handlers im
 *     Bild 0; derselbe FUN_8004eae4 laeuft im Spawnbild hinter der erfolgreichen Platzvergabe des Wurfkoerpers
 *     (Fenster 0 Bilder). Griff/Treffer/Statusschirm-Waffenwechsel vor dem Spawnbild oder kein Platz im Pool ->
 *     kein Wurf und kein Verbrauch. Abzug-Gate (Magazin > 0, `jal 0x8004ea6c` @0x80033314) unveraendert. */

/** 1 = fuer Waffe `id` entfaellt der Mengen-Abzug FUN_8004eae4 des Entlade-Handlers im Rueckstossbild 0
 *  (Wurf-Ids 9/10/11: Handler 0x80033B38/58/78, `jal 0x8004eae4` @0x80033b40/60/80), sonst 0. */
int      re15_granate_r36_verbrauch_im_spawnbild(int id);
/** Der Wurfkoerper hat seinen Platz bekommen (Spawnbild): jetzt FUN_8004eae4 (eine Granate, Rueckgabe wie
 *  dort: 1 = abgezogen, 0 = Menge war schon 0). Aufrufer: game_step_common.c hinter re15_esp_granate_spawn. */
int      re15_granate_r36_geworfen(void);
unsigned re15_granate_r36_verbraucht(void);       /* Zaehlung fuer Sonden (Abzuege im Spawnbild) */

#endif /* RE15_GRANATE_R36_H */
