# Integration Runde 41 - gestreckte Sherry (ROOM6031) und Annette (ROOM6030)

Nutzer-Befund (Story-Liste 2026-11-08): "In Room 6030, wenn Anette den Zug startet und in ROOM 6031 wenn Sherry den Zug
startet stimmt irgendwas mit dem Modell nicht. Das wird irgendwie auseinander gezogen, wenn sie mit dem Spieler sprechen."
Marke F9: `re15_port/build/platform/pc/befund_6031_F164_marke1.bmp` (Hauptbaum-exe b27d96891), befund.log MARKE 1:
F164 R6031 C1, Gegner 1 typ=0x4B clip=1 bild=29 anim=0020 - Sherry spielt den Sub-0-Kanal (RBJ rec1 Clip 1).
Fix: Commit 2c1c0eb1a (`engine/src/enemy_common.c` rbj_resolve_slot), Riegel `unit_r41_npcglieder_sherry6031` /
`unit_r41_npcglieder_annette6030` (`tests/unit/test_r41_npcglieder.c`).

## 1. Nachstellen (eigene exe-Kopien, Software-Renderer)

Der Weg des Nutzers (befund.log vor der Marke): ROOM6001 -> Tuer -> ROOM6031 Wagen (C7/C8) -> Treppe -> Tuer Slot 1 ->
Fuehrerstand C0/C1, Bildzaehler laeuft DURCH (F75 -> F90): die Tuer Slot 1 (main00 @0x00C52, Ziel (-3873,0,-23487)) fuehrt
in denselben Raum, es gibt kein Raumladen.

| Lauf | Aufbau | Ergebnis |
|---|---|---|
| e6031 | RE15_DEBUG_JUMP=6031 (Eintritt Cut 0) | Sherry heil - der Debug-Sprung laeuft nur sub07, nie sub06 |
| fa (vorher, b27d96891) | Karte `vor6031 wagen` (probe_r36_story_karte), Tuer 6001 -> 6031 Cut 6, `RE15_FIRE_AOT=1@200#6031` | Sherry wie die Marke: Kopf oben, graue Oberarme abgeloest (F240..F330) |
| fa_nach | derselbe Lauf mit dem Fix | Sherry heil |
| an2 (vorher) | Karte `vor6030 wagen tor_bereit`, Flags (3,89)/(4,23), Tuer Slot 1 -> Zugstart (Spur AN, msg 27..30) | Annette heil: Aktor 1 erst im Fuehrerstand gebunden |
| an1 (vorher) | Karte `vor6030`, Wagen-Szene (sub02, story80) -> `RE15_FIRE_AOT=1@3200#6030` -> Fuehrerstand msg 11,12,25,26 | Annettes Oberarme stehen seitlich ab (F3300..F3650) |
| an1_nach | derselbe Lauf mit dem Fix | Annette heil |

Bilder: `vorher_nachher.png` hier (Marke | vorher F260 | nachher F260) und `../annette6030/vorher_nachher.png`
(kein Nutzer-Marker | vorher F3300 | nachher F3300).

Folgerung fuer 6030: der Fehler tritt auf, wenn im SELBEN Besuch vorher die Wagen-Szene John (0x49) auf Aktor 1 gebunden
hat (erster Besuch "Wagen zuerst", Fuehrerstand msg 11/12/25/26 "Certainly." - in der Story-Liste "Annette startet den
Zug"). Der Zugstart beim Besuch nach dem Tor bindet Aktor 1 frisch und war schon heil (Lauf an2).

## 2. Ursache

Debug-Zeilen (Lauf fa): `[rbj-binder] ROOM6031 Aktor 1 Typ 0x40 Marker-Slot 1 -> rec1 gebunden` (Wagen, sub06 @0x00E42
Sce_em_set em0 = 0x40, sub03 @0x00E0E Work_set(2,0) Plc_motion(0,13)); nach der Tuer `Sce_em_set type=0x4B slot=0`
(sub07 @0x00EFA) und KEINE neue Bindung - der Port-Cache (enemy_common.c `s_rbj_cache`) hielt den Eintrag je SLOT und
lieferte das beim ersten Binden kopierte Skelett (`*out_skel = *base_skel` in re15_emd_parse_rbj_record, relpos der Bank
0x40) weiter. Modell (Netz) = 0x4B Sherry, Knochenlagen = 0x40, Keyframes = rec1: Skelett und Netz aus verschiedenen
Quellen, gleiche Knochenzahl (15) und gleiche Elterntabelle (-1 0 1 2 3 1 5 6 0 0 9 10 0 12 13), andere relpos.

Ruhelagen (Kind relativ zum Eltern, `tools`: Scratchpad relpos_vgl.py, Werte aus CDEMD1/CDEMD0 TOC @0x80073178/@0x80072f38):

| Knochen | Sherry 0x4B (gezeichnet) | 0x40 (alter Cache) | Annette RE2/ANNETTE_4D (gezeichnet) | John 0x49 (alter Cache) |
|---|---|---|---|---|
| 0 Becken | (0,-1268,0) | (0,-1810,0) | (0,-1709,0) | (0,-1810,0) |
| 8 Kopf | (-2,-465,0), Ruhe-Welt y -1733 | (-128,-709,0), y -2519 | (-21,-623,0), y -2332 | (-98,-704,0), y -2514 |
| 9 Oberarm L | (-37,-412,-219) | (-84,-717,-369) | (-40,-571,-285) | (-59,-692,-369) |
| 12 Oberarm R | (-37,-412,221) | (-84,-722,369) | (-40,-571,285) | (-59,-692,369) |
| max. Abweichung | | 542 (Becken), 311 (Glieder) | | 129 |

Das passt zum Bild: Kopf um ~244 hoeher (465 -> 709), Oberarme 150 weiter aussen und ~300 hoeher als die Schultern des
Netzes.

## 3. Original-Beleg

- `FUN_80022300` (einziger Aufrufer Sce_em_set `jal 0x80022300` @0x80042328) setzt je Spawn die Zeiger des NEUEN Modells:
  `80022498 sw v0,0x16c(s2)` (dir[1]), **`800224a8 sw v0,0x84(s2)` (dir[2] = Hierarchie + relpos)**, `800224b8 sw
  v0,0x174(s2)`, `800224c8 sw v0,0x170(s2)`. +0x180/+0x184 schreibt nur der RBJ-Binder FUN_8001b3f8 (Dossier
  J_stage3szenen_a.md §0.1).
- Sub 0 des Executors: `80050d40 lw a0,0x180(v0)` / `80050d48 lw a1,0x184(v0)` / `jal FUN_8001f314` - nur EDD und
  Keyframe-Pool kommen aus dem Kanal.
- Die RBJ-Records tragen keine Hierarchie: EMR-Kopf `00 00 08 00 0f 00 50 00` (o_arm 0, o_frm 8, 15 Knochen, 80 B je Bild)
  - ROOM6031.RDT RBJ @0x13C8, rec1 @+200; ROOM6030.RDT RBJ @0x17BC, rec1 @+200 (alle drei Records beider Raeume ebenso).
Also nimmt das Original die Knochenlagen immer vom Modell, das gerade im Slot steht.

## 4. Fix

`rbj_resolve_slot`: der Cache-Eintrag merkt `type` und `bank` (re15_enemy_find(type)); passt einer nicht mehr zum Aktor
im Slot, wird der Eintrag verworfen und mit der Bank des neuen Typs neu gebunden (Messzeile `[rbj-binder] ROOM6031 Aktor 1
Typ 0x4B Marker-Slot 1 -> rec1 gebunden`). Record, EDD und Keyframes bleiben unveraendert. Gleicher Weg fuer beide Szenen.

## 5. Riegel

`unit_r41_npcglieder_sherry6031` / `_annette6030`: echte RDT-RBJ, echte Modelle (CDEMD1 0x40/0x4B; CDEMD0 0x49 und
RE2/ANNETTE_4D.EMD), Aktor 1 erst Wagen-Typ, dann Fuehrerstand-Typ ohne Raumwechsel; Abstand jedes Kindknochens zum
Elternknochen (|relpos|, Drehungen aendern ihn nicht) gegen das gezeichnete Modell, Toleranz 0 (das Original zeigt die
relpos des Modells unveraendert, @0x800224a8); dazu: Keyframes zeigen weiter in den RBJ-Block.
Gegenprobe (enemy_common.c aus b27d96891, gebaut und gelaufen): beide rot - Sherry Abweichung 311 (Kopf |relpos| 720 statt
465, Oberarm 811 statt 468), Annette 129 (Oberarm 786 statt 639). Mit dem Fix: 0 Fehler.
