/*
 * RE1.5 Rebuilt — Runde 41 Spur E "kleinkram_b", Nachbesserung 2 (master-AUFTRAG E-N1): das Anforderungsbild eines
 * Kamera-Applys wird NICHT gezeichnet, das vorige Bild bleibt stehen.
 *
 * GEMESSEN (Dossier "Nachbesserung 2"): ROOM2020, Spieler genau auf der gemeinsamen Kante x = 9000 der RVD-Zonen
 *   z1 @0x016C (Cut 0 -> 2, x 8000..9000) und z5 @0x01BC (Cut 2 -> 0, x 9000..10000). Port (Abnahme 1 §6.1): `cam=`
 *   wechselt F1..F300 299-mal 2,0,2,0 — sichtbar flackert das Bild zwischen den Hintergruenden. Original (DuckStation-
 *   GDB, o_cam9000*.txt): der angeforderte Cut DAT_800afbb5 wechselt GENAUSO jeden Durchlauf (FUN_800142f4 aus
 *   @0x800142b4), aber 39 von 40 Durchlaeufen laufen durch den Apply-Zweig (@0x80021558) und kein einziges DrawOTag:
 *   das Bild steht.
 * ORIGINAL (selbst disassembliert): Present FUN_8002137c @0x800214f4 `lh DAT_800b0fe4` / @0x80021500 `lbu
 *   DAT_800afbb5` / @0x80021508 `beq` / @0x80021514 `sb 1,DAT_800b5457`; @0x80021530 `lbu DAT_800b5457` / @0x80021538
 *   `beq` -> dirty: @0x80021558 `jal 0x80021bbc` (Apply) und @0x80021560 `j 0x800215fc` — uebersprungen werden die
 *   DrawOTag @0x800215bc/@0x800215d0/@0x800215e4 und der Pufferwechsel @0x800215f4 `xori`/@0x800215f8 `sb DAT_800aca34`.
 *   Cut_chg setzt DAT_800b5457 ohne Vergleich (@0x800402f4) -> auch ein Cut_chg auf denselben Cut ist ein Apply-Bild.
 * PORT: der Apply des Ports laeuft am Anfang des FOLGEBILDS (pc_cam_present_apply, re15_cam_present_tick), sobald
 *   angezeigt (work_vars[0x0A]) != angefordert (cam_id) oder cam_change_pending — genau die Bedingung, mit der das
 *   Original am Ende dieses Durchlaufs den Apply-Zweig nimmt. In diesem Bild praesentiert der Port darum nichts
 *   (re15_render_pc_bild_halten, platform/pc/src/bildhalt_r41_pc.c): das Fenster zeigt das vorige Bild weiter. Der
 *   Blenden-Takt FUN_80021880 laeuft dabei weiter (@0x80020f44 vor dem Present @0x80020f4c; Nachbesserung 3, Abnahme 2
 *   Mangel 4). Die Logik laeuft genau EIN Spielbild lang (ein Durchlauf).
 * NACHBESSERUNG 4 (Nachtrag 14, "halb fliegender" Sprung an die Kurbel): die DAUER des Apply-Durchlaufs. Das Original
 *   liest im Apply den Cut-Hintergrund BLOCKIEREND von CD (FUN_80021bbc @0x80021d2c `jal 0x80013c50`, a1 = 0 @0x80021d30;
 *   Bytes @0x80021cb8, LBA = Datei-LBA + Cut*0x20 @0x80021cd8-@0x80021d28) und dekodiert ihn (@0x80021e34 `jal 0x80053a8c`);
 *   der Bildschirm steht so lange. Gemessen auf dem Weg des Nutzers (o_kurbel_cut3.txt): 14 VSyncs statt 2 (Lesen 8 + MDEC 4).
 *   Port: das gehaltene Bild + re15_kamerahalt_r41_standbilder() weitere Bilddauern Wanduhr ohne Spieltakt
 *   (platform/pc/src/bildhalt_r41_pc.c re15_render_pc_standbild). Bildzaehler, Skripte und Logik bleiben unberuehrt — wie
 *   im Original zaehlt der Apply als EIN Durchlauf.
 * Ausnahmen: erstes Bild eines Raums (Raum gewechselt: kein Bild dieses Raums zum Halten), die ROOM1240-Montage
 *   (RE2-Praesentation des Ports), RE15_FORCE_CUT (Messhaken: setzt jedes Bild pending).
 */
#ifndef RE15_KAMERAHALT_R41_H
#define RE15_KAMERAHALT_R41_H

/* 1 = dieses Bild ist ein Apply-Bild des Originals: vorige Darstellung stehen lassen. `raum` = g_current_room_id. */
int re15_kamerahalt_r41_haelt(unsigned raum);

/* Pruefhaken: Zahl der gehaltenen Bilder seit Start. */
unsigned re15_kamerahalt_r41_zahl(void);

/* Nachbesserung 4: weitere Bilddauern (je 2 VSyncs), die das zuletzt entschiedene Apply-Bild stehen bleibt (0 = kein Apply).
 * 6 = gemessene 14 VSyncs des Apply-Durchlaufs (o_kurbel_cut3.txt n104 -> n105) minus das gehaltene Bild selbst. */
int re15_kamerahalt_r41_standbilder(void);

#endif /* RE15_KAMERAHALT_R41_H */
