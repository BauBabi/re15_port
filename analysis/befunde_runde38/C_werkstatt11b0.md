# Runde 38 — Spur C "werkstatt11b0" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r38_werkstatt11b0` (Zweig `r38/werkstatt11b0`, Basis 1bcbd0ff).
Auftrag (AUFTRAG.md Z.16/Z.34/Z.35, woertlich):

1. "Ändere in ROOM 11B0 in der neuen Cutscene von Marvin seinen Text von "Hold on..." zu "But, hold on...""
2. "In ROOM 11B0 macht das Verschieben des COntainers mit Marvin zusammen in der Cutscene noch keinen Verschiebe Sound"
3. "In ROOM 11B0 macht Marvin während/nach dem verschieben des Containers so eine komische Fall Animation, die er im
   original nicht macht."

Vorgaenger: Runde 36 Spur J (`analysis/befunde_runde36/J_szenenleon.md`): Schub-Sub 0x13 @0x80052864,
Erweiterung (Ereignis 22/23, msg 20..22) in `tools/r36_j/szene11b0_bauen.py` -> `engine/src/gen/szl11b0_szene.inc`,
Laeufer `engine/src/szenenleon_r36.c`.

Werkzeug: `re15_port/tools/r38_c/messlauf.sh` (exe-Kopie `re15_pc_r38c.exe`, RE15_WINDOW_SCALE=1,
SDL_RENDER_DRIVER=direct3d11, Bilder per RE15_FRAMEDUMP). ⛔ Messfalle: die Bildnummern der Szene haengen an den
Schaltern — ein Lauf OHNE RE15_AUDIO_CAP_SYNC lag 154 Bilder frueher (Cut_chg(6) F1428 statt F1582). Mit identischen
Schaltern (CAP_SYNC) ist der Lauf bildgenau wiederholbar (Laeufe v1/v3: `[walk] F1620..F1770` Zeile fuer Zeile gleich).
Alle Bildnummern unten gelten fuer den Grundlauf
`RE15_DEBUG_JUMP=11b0@250 RE15_SET_FLAG=3:0x43,3:0x82,4:0xf3 RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_ANIM_TRACE=..
RE15_STATE_LOG=.. RE15_AUDIO_CAP_SYNC=..` (Stand 1bcbd0ff).

## Stand / Fortschritt

- [x] Messung vorher (alle drei Punkte) + RE-Belege — §1.1/§1.2, §2.1/§2.2, §3.1/§3.2
- [x] Punkt 1 Text "But, hold on..." — Umsetzung §1.3, nachher §1.4
- [x] Punkt 2 Schiebe-Ton — Umsetzung §2.3, nachher §2.4
- [x] Punkt 3 keine Fall-Animation — Umsetzung §3.3, nachher §3.4
- [ ] Riegel gruen (gezielt) + integration_r38_werkstatt11b0 — Abschnitt Tests

## §1 Punkt 1 — Text "But, hold on..."

### 1.1 Messung vorher
- debug.log (Grundlauf v1, Erweiterung zwischen F2100 und F2130): `[msgtext] room=11b0 id=20 "Marvin: Hold on."` — der heutige Wortlaut ist
  "Hold on." (Punkt), erzeugt von `tools/r36_j/szene11b0_bauen.py` Z.281 `MSG20 = marvin_zeile(["Hold on."])` ->
  `engine/src/gen/szl11b0_szene.inc` `k_szl_msg20`.
- Sprachdatei: `synchro/STAGE1/room11B0/` traegt main00..main14.wav, KEINE main20.wav (auch nicht unter
  `synchro/unused/`) — es gibt keine Aufnahme, die den alten Wortlaut spricht; Text und Stimme koennen nicht
  auseinanderlaufen.

### 1.2 Beleg der FORM (Wortlaut = NUTZER-VORGABE)
- Sprecherform: ROOM11B0 msg 12 @0x1BD0 `04 00 05 07` "Marvin" `16 05 00 00` ... `04 01 01 5f` (unveraendert, Generator
  prueft die Bytes).
- Komma + Auslassungspunkte in einer Original-Marvin-Zeile: ROOM11B0 msg 4 @0x19CD "Marvin: Oh, I see... Damn it!" =
  `... 2b 44 18 00 25 00 4f 41 41 57 57 57 00 ...` -> "Oh," = `2b 44 18` + Leerzeichen `00`, "..." = DREI Punkte
  `57 57 57` (kein eigenes Auslassungszeichen).
- Damit "But, hold on..." = `1e 51 50 18 00 44 4b 48 40 00 4b 4a 57 57 57` (B u t , _ h o l d _ o n . . .).

### 1.3 Umsetzung
- `re15_port/tools/r36_j/szene11b0_bauen.py`: `MSG20 = marvin_zeile(["But, hold on..."])` (+ Kommentare/Listing mit
  Formbeleg msg 4 @0x19CD), neu erzeugt -> `engine/src/gen/szl11b0_szene.inc`: NUR `k_szl_msg20` aendert sich
  (26 -> 33 B: `04 00 05 07 29 3d 4e 52 45 4a 16 05 00 00 1e 51 50 18 00 44 4b 48 40 00 4b 4a 57 57 57 04 01 01 5f`),
  das Programm (294 B, Message_on 20 @+0x0008) bleibt Byte fuer Byte. Ende `04 01 01 5f` (Wartezeit 95 Bilder) wie
  bisher aus msg 12 (Generator prueft @0x1BFA).

### 1.4 Messung nachher
- exe (Grundlauf n1, Stand mit Fix): `[msgtext] room=11b0 id=20 "Marvin: But, hold on..."` (debug.log, zwischen
  F2100 und F2130). Bildablauf der Szene unveraendert (`[walk] F1620..F1770` identisch zu v1/v3).
- Riegel unit_r38_werkstatt11b0_text: Bytes, Form gegen ROOM11B0 @0x1BD0/@0x1BFA/@0x19CD, Programm 294 B,
  `re15_msg_get_text(20)` = "Marvin: But, hold on..." (gruen).

## §2 Punkt 2 — Schiebe-Ton beim gemeinsamen Schieben

### 2.1 Messung vorher
- Der Ton WIRD angefordert: `engine/src/szenenleon_r36.c` ruft `re15_audio_room_se_snd0(7)` in Phase 5 bei Bild 1 jedes
  Clip-0x12-Umlaufs und in Phase 7 (Spieler UND Marvin); Riegel unit_r36_szenenleon_schub zaehlt 8 Aufrufe.
- Er KLINGT nicht: debug.log des Grundlaufs zeigt waehrend des Schubs (Sub-0x13-Start bis "Schub-Sub zu Ende",
  F1634..F1730) KEINE `[se] Stimme: se=7`-Zeile (RE15_SE_DEBUG an). audio_pc.c se_play_layers kehrt bei einem leeren
  EDT-Satz vor jeder Ausgabe zurueck (`rec.empty`).
- Ursache = die Raumdaten: ROOM11B0.RDT snd0 (Kopfworte +0x08/+0x0C/+0x10 = EDT @0xE484 / VH @0xE504 / VB @0xFBA4):
  **Satz 7 @0xE4A0 = `00 00 00 00`** (leer), die VH hat nur 3 VAGs (ps=1 ts=4 vs=3) — der Raum besitzt keinen
  Schiebe-Ton.

### 2.2 RE-Belege (selbst disassembliert, info/Re1.5/PSX.EXE)
- Schub-Sub 0x13 Phase 5: @0x80052ac0 `lbu v1,149(v0)` (+0x95) / @0x80052ac4 `ori v0,zero,0x1` / @0x80052ac8 `bne`
  -> @0x80052acc `lui a0,0x207` / @0x80052ad8 `jal 0x80045024` (a1 = Aktor+0x34) = Bank 2 Satz 7 bei Bild 1 jedes
  Umlaufs; Phase 7 @0x80052bc8 `lui a0,0x207` / @0x80052bcc `jal 0x80045024`. Kistenschieben des Spielers:
  @0x80035a08 `lbu v1,0(a1)` (a1 = 0x800acae9 = +0x95) / @0x80035a10 `bne` / @0x80035a18 `lui a0,0x207` /
  @0x80035a1c `jal 0x80045024` — DERSELBE Satz im selben Takt.
- Bank 2 = Raum-snd0: FUN_80045024 @0x80045028 `srl v1,a0,24`, Sprungtabelle @0x80010e70[2] = 0x800450e4 ->
  @0x800450e4 `sltiu v0,s4,0x21` -> @0x80045130 `lw v0,DAT_800ac778` (RDT) / @0x8004513c `lw a0,8(v0)` (RDT+0x08 =
  snd0-EDT) / @0x80045140 `sll v0,s4,2` / @0x80045144 `addu a0,a0,v0` (Satz*4).
- Leerer Satz `00 00 00 00` ist im Original ebenfalls stumm: byte3&0x1f = 0 < 0x10 -> Direktzweig @0x800451a8..
  @0x8004522c SsUtKeyOnV(Prog 0, Ton 0); a0&0xff = 0 -> nicht positional, voll = volr = Ton-vol (@0x800451dc
  `lbu v0,2(s1)`) = ROOM11B0 Ton 0 @0xED24 `00 00 00 40 52 43 ...` vol **0** (VAG 1, 48 B). Die Original-Szene ist an
  dieser Stelle STUMM.
- Zensus Bank-2-Satz-7-Aufrufer in PSX.EXE (jal 0x80045024 mit `lui a0,0x0207`): NUR @0x80035a18 (Kiste),
  @0x80052acc und @0x80052bc8 (Schub-Sub) — Satz 7 ist im Original ausschliesslich der Schiebe-Ton.
- Zensus Plc_dest Modus 0x13 ueber alle 206 RDTs (`scd_dump_room.py`): NUR ROOM11B0 (@0x0170E/@0x0171A) und ROOM1171
  (@0x01566/@0x01572, Elza + Partner, danach Evt_exec 3/4 = Objekt 3 wird gefahren). ROOM1171 snd0 Satz 7
  @0x6EF8 = `00 00 45 10` (Prog 0, Ton 4, Prio 5, Stimme 0) -> Ton 4 @0x77FC (32 B `00006e4057003f3f...`, vol 110,
  pan 64, center 87, min/max 63) -> VAG 4 @0xA8BC (VB @0x87FC + 0x20C0), 6688 B, sha1 f343933e.
- **Derselbe Ton wie Leons Kistenschieben**: ROOM1090 (Leon schiebt die Kiste allein, der Port spielt dort
  re15_audio_room_se_snd0(7)) Satz 7 @0x118EC `00 00 45 10`, Ton 4 @0x121F0 bytegleich, VAG 4 @0x152B0 6688 B sha1
  f343933e; ebenso ROOM1091/1170/3080 (dieselbe Groessenfolge der VAGs 48/4496/3840/6688/13920/7872). RE1.5 hat
  also EINEN Schiebe-Ton, bytegleich in allen Raeumen mit Kiste bzw. Schub-Szene — nur ROOM11B0s Raumbank fehlt er.
- Einordnung (Beta -> Retail): das Schiebe-SYSTEM ist in RE1.5 vollstaendig (Kiste + Schub-Sub, Ton in 1090/1171);
  unfertig ist allein die Raumbank von 11B0. Massgeblich bleibt RE1.5 (Memory reai-v2-beta-zu-retail: "wo RE1.5 ein
  System VOLLSTAENDIG hat, bleibt RE1.5 massgeblich"). RE2 geprueft und verworfen: RE2s Spieler-Schub spielt
  @0x8003f2f4 `lbu v1,333(s0)` == 1 -> @0x8003f304 `lui a0,0x204` / `ori a0,a0,0x1` / @0x8003f30c `jal 0x8005ba28`
  (Raumbank Satz 4, positional) — der Satz ist JE RAUM ein anderes Sample (RE2 ROOM10C0/1120/2030/3060/4030/5010/
  6030/6040 Satz 4: acht verschiedene VAGs, 1248..8896 B, keines = f343933e); RE2 hat keine Container-/Gemeinsam-
  Schub-Szene, aus der ein Satz fuer 11B0 folgen wuerde. Leon klingt im Port beim Kistenschieben nach RE1.5 —
  derselbe Ton fuer das gemeinsame Schieben ist die einzige belegte Wahl.

### 2.3 Umsetzung
- `engine/src/werkstatt11b0_r38.c` / `include/re15_werkstatt11b0_r38.h`: `re15_w11b0_ergaenzung_noetig(raum, edt, n,
  satz)` = 1 nur fuer ROOM11B0, Satz 7, Raumsatz leer (Byte 2/3 == 0 wie re15_edt_decode); Haken-Zeiger
  `re15_w11b0_snd0_hook` (NULL auf PSX/in Unit-Tests = Original).
- `platform/pc/src/werkstatt11b0_pc.c`: schneidet beim ersten Ton die snd0-Bank aus
  `shared_assets/PSX/STAGE1/ROOM1171.RDT` (re15_rdt_parse, Kopfworte +0x08/+0x0C/+0x10), prueft Offsets 0x6EDC/0x6F5C/
  0x87FC, Satz 7 `00 00 45 10` und VAG 4 = 6688 B, dekodiert die VAGs und spielt Satz 7 ueber re15_audio_se_play_bank
  (= se_play_layers, Stimme 0 / Prio 5 / nicht positional wie FUN_80045024 @0x800451dc). KEINE neue Datei, kein Patch
  an shared_assets/PSX. Mess-Haken `RE15_W11B0_SCHUB_STUMM` (nur Riegel: Ergaenzung aus = Stand vorher).
- Haken (je 1 Zeile): `platform/pc/src/audio_pc.c` re15_audio_room_se_snd0 fragt zuerst `re15_w11b0_snd0_hook`
  (+ 1 Include-Zeile); `platform/pc/main.c` Init-Block `re15_pc_w11b0_init()` neben den Runde-36/37-Ton-Haken.
- Takt unveraendert (szenenleon_r36.c: Bild 1 jedes Clip-0x12-Umlaufs @0x80052ac0-d8, Phase 7 @0x80052bc8), je Schieber.

### 2.4 Messung nachher
- exe (Grundlauf n1): `[schub-r38] Ergaenzungsbank ROOM1171 snd0 geladen: 6 VAGs, Satz 7 @0x6EF8 00 00 45 10 -> Ton 4
  -> VAG 4 (6688 B)`, dann 8 Toene `[schub-r38] F1646` x2, `F1671` x2, `F1696` x2, `F1729` (Leon Phase 7), `F1730`
  (Marvin Phase 7), jeder mit `[se] Stimme: se=7 layer=0 vag=3 note=63 fine=0 center=87 vol=110 pitch=0x400 (11025 Hz)
  -> SE-Stimme 0 (SPU 16)`. F1646 = Clip 0x12 Bild 1 (anim.txt: Clip 18 ab F1645), Abstand 25 = Clip-0x12-Laenge.
- Ton-Mitschnitt (RE15_AUDIO_CAP_SYNC, v3 vorher gegen n1 nachher, gleiche Schalter, Mitschnittblock = Spielbild +
  254): bis Block 1900 (= F1646) bitgleich; Unterschied genau in F1647..F1728 (drei Umlaeufe, Summe |Diff| 279 395 136)
  und F1730..F1762 (Phase-7-Toene, 109 329 132) — mehr Energie als die ganze uebrige Mischung im selben Fenster
  (171 615 336): der Schiebe-Ton ist deutlich zu hoeren, vorher fehlte er ganz.
- Mischer-Probe (unit_r38_werkstatt11b0_ton, echtes audio_pc.c): 11B0 ohne Ergaenzung Energie 0, mit Ergaenzung
  108 200 792, ROOM1090 (Leon allein, eigene Bank) 108 200 792 — **147 000 von 147 000 Samples gleich**: es ist exakt
  Leons Kisten-Ton; Tonlaenge 32 Bilder; 11B1 und 11B0-Satz-0 unberuehrt.

## §3 Punkt 3 — Marvins "Fall-Animation"

### 3.1 Messung vorher (Grundlauf, RE15_ANIM_TRACE + state.log + Framedump JEDES Bildes F1626..F1800)
- Clipfolge Marvin (Aktor 1, Typ 0x40), anim.txt-Spalten Bild/Clip/Bildzahl/cur/slot/loco/rueckwaerts:
  F1635-1644 Clip 17 (0x11 Ansatz, Loco) | F1645-1718 Clip 18 (0x12 Schub, Loco, 3 Umlaeufe) | F1719-1729 Clip 17
  rueckwaerts (Phase 6, slot 9 -> 0) | **F1730 Clip 17, cur 9, slot 9, loco 0, rueckwaerts 0** | F1731-1746 Clip 1
  (eigene Bank, 16 B.) | F1747 Clip 2 (Idle) | F1749-1759 Clip 5 (Drehung, Sub 9) | F1760 Clip 1 | F1776 Clip 2.
- state.log F1730: `[1 t=40 st=4 ss1=6 ss2=0 ... mo=17 af=9]` — Marvin ist schon in Sub 6 (Event-Reach), Clip noch
  0x11, Richtungsbit geloescht. Zensus ueber den ganzen Lauf: dies ist das EINZIGE Bild, in dem Marvin eine Bank-1-
  Regel (Sub 6) mit einem Clip >= 14 (EM040 Bank 1 hat 14 Clips) trifft.
- Bildfolge (Framedump, Ausschnitt um Marvin): F1719-F1729 richtet er sich aus der Schiebehaltung auf (aufrecht ab
  F1726), **F1730 schnappt er in die tiefe Schiebehaltung zurueck** (vorgebeugt, Beine gegraetscht), F1731-F1733
  bleibt er vorgebeugt und richtet sich bis F1734 wieder auf = der "komische Fall" nach vorn und zurueck.
- Mechanik im Port: main.c Renderer -> re15_actor_uses_own_bank (Sub 6 -> Bank 1) -> `motion 17 < anim_own.clip_count
  (14)` scheitert -> Rueckfall auf die Container-Bank (groesste Bank = dir[1], 24 Clips) Clip 17 VORWAERTS Bild 9 =
  das Ende des Ansatzes = die tiefste Schiebehaltung. Der 7-Bilder-FRAC-Uebergang des Sub 6 (re15_npc_seed_clip
  anim_frac 7, skeleton_common.c Blend aus der zuletzt GEZEICHNETEN Pose) zieht diese Fehlpose noch 3-4 Bilder mit.
- Nebenbefund derselben Art am Schub-Anfang: F1634 Clip 5 aus der LOCO-Bank (20 B.) statt aus Bank 1 (Phase 1 ->
  2 im selben Takt, re15_szl_schub_npc_kanal schaltet nach +0x6 statt nach dem Clip um) — 1 Bild, kaum sichtbar.
- EM040 (CDEMD0.EMS Blob 18 @0x2CD800): Bank 0 (dir[1]) 24 Clips `34,32,50,26,20,20,50,1,1,1,1,25,1,1,1,1,1,10,25,1,1,1,
  30,30` (Clip 0x11 = 10, 0x12 = 25 Bilder), Bank 1 (dir[3]) 14 Clips `22,16,52,1,50,30,10,16,1,17,1,17,1,1`.

### 3.2 RE-Belege (selbst disassembliert, info/Re1.5/PSX.EXE)
- Phase 6 @0x80052b84..@0x80052bbc: anim_set(+0x84,+0x16c,**1**,0x200) @0x80052b98 = Clip 0x11 rueckwaerts; FUN_8001f314
  waehlt rueckwaerts den Index `(Bildzahl - +0x95) - 1` (RE_15_Quellcode_V2/FUN_8001f314.c), +0x6 += Rueckgabe ->
  das LETZTE posierte Bild ist Index 0 des Clips 0x11; FUN_8001f3bc setzt danach +0x95 = 0 (@0x8001f63c `sb
  zero,149(v1)`).
- Phase 7 @0x80052bc0..@0x80052bf4: NUR Se_on (@0x80052bcc), +0x5 = 6 (@0x80052be0 `sb v0,5(v1)`), +0x6 = 0
  (@0x80052bf0 `sb zero,6(v0)`) — **kein `jal 0x8001f314`**: in diesem Bild wird nicht posiert.
- Die Pose lebt im Arbeitsblock Aktor+0x188, den NUR anim_set schreibt (FUN_8001f3bc: `puVar6 = *(entity+0x188)`,
  Wurzel puVar6[0xb..0xf], Winkel FUN_8001f664, Matrizen); der +0x8f-Blend liest die dort gespeicherte Vor-Pose
  (puVar6+0xe/0x3a/0xf). Ohne anim_set bleibt also die Pose von Phase 6 stehen (Clip 0x11 Bild 0) — und der Sub-6-
  Blend beginnt dort.
- Sub 6 (NPC-Tabelle 0x80076ca0[6] = 0x800517f0): [0] @0x80051844..@0x80051874 +0x6=1, +0x94=1, +0x95=0, +0x8f=7 ->
  [1] @0x80051878..@0x8005188c anim_set(**+0x170,+0x174**,0,0x200) = Bank 1 Clip 1, ab dem FOLGEBILD.
- Phase 1 @0x800528f4: anim_set(+0x170,+0x174) @0x8005294c VOR der Kurs-Rastung und VOR `+0x6 = 2` (@0x800529a0-b0)
  -> im Uebergangsbild 1 -> 2 ist Bank 1 Clip 5 posiert; erst Phase 2 (Folgebild) setzt +0x94 = 0x11 und posiert
  +0x84/+0x16c.
- Folgerung: das Original zeigt im Bild von Phase 7 die gehaltene Pose Clip 0x11 Bild 0 (aufrecht) und blendet von
  dort in Bank 1 Clip 1 — KEIN Rueckfall in die Schiebehaltung. Der Port-Fehler ist die Bank-Wahl fuer genau dieses
  Uebergabebild.
- Regression? Nein — der Fehler steckt seit Runde 36 Spur J (3d394d71, Einfuehrung des NPC-Sub 0x13) in
  re15_szl_schub_npc_kanal (Bank nach +0x6 statt nach dem posierten Clip, kein Halten in Phase 7); das Riegel
  unit_r36_szenenleon_schub prueft nur die Phasen 3/5, nie das Uebergabebild. Einordnung je Release (git merge-base
  --is-ancestor 3d394d71 <tag>):

  | Stand | enthaelt 3d394d71 | Szene |
  |---|---|---|
  | v0.8.23 3ec5b1f8 | nein | kein Schub ueberhaupt (Modus 0x13 unbekannt, Marvin auf re15_npc_sub_idle, J §1.1) |
  | v0.8.24 e8b3958f | ja | Schub + Fall im Uebergabebild |
  | v0.8.25 2077bb8e | ja | unveraendert (letzter Eingriff in szenenleon_r36.c: 35c02fab, Integration Runde 36, schon in v0.8.24) |

  Der "Fall" kam also mit dem Schub selbst in v0.8.24, er ist kein Rueckfall eines vorher richtigen Stands.

### 3.3 Umsetzung
- `engine/src/werkstatt11b0_r38.c`: `re15_w11b0_schub_kanal(e)` — Sub 0x13: Bank folgt dem posierten Clip (Clip 5 ->
  2 = +0x170 @0x8005294c, sonst 1 = +0x84 @0x80052a08/@0x80052af4/@0x80052b98); Uebergabebild (Sub 6, +0x6 = 0, Clip
  0x11, Merker aus Phase 7) -> 1 (Loco); der Merker verfaellt, sobald Sub 6 posiert hat. `re15_w11b0_schub_uebergabe(e)`
  in Phase 7: +0x95 = 0 (@0x8001f63c) + Merker + Log `[w11b0-r38] F<n> Aktor <i> ... Phase 7 ... gehalten`.
- Haken in `engine/src/szenenleon_r36.c` (3 Zeilen): Include, erste Zeile von re15_szl_schub_npc_kanal, Aufruf im
  default-Zweig (Phase 7). Alle Verbraucher (main.c Renderer, re15_actor_uses_own/loco_bank, Takt) lesen dieselbe
  Kanal-Funktion — eine Regel.
- Leon (Spieler) ist NICHT betroffen: sein Phase 7 startet Sub 6 im selben Bild (re15_player_event_reach_begin), die
  Spieler-Bank haengt nicht an dieser Regel (anim.txt F1729 Clip 1).

### 3.4 Messung nachher (Grundlauf n1, Framedump jedes Bildes F1626..F1800)
- anim.txt Marvin: `1634 1 64 5 5 30 0 0 30 0 0` (Bank 1 Clip 5, vorher Loco 20 B.), `1729 1 64 17 17 10 9 0 10 1 1`
  (Phase 6 letztes Bild, slot 0), **`1730 1 64 17 17 10 0 0 10 1 0`** (Uebergabebild: Loco Clip 0x11 slot 0 = dieselbe
  Pose), `1731 1 64 1 1 16 0 0 16 0 0` (Sub 6 Clip 1). state.log-Zensus ueber 2760 Bilder: 0 Rueckfall-Bilder; anim.txt:
  0 Zeilen Clip 0x11/0x12 ohne Loco-Bank.
- Bild (`C_belege/p3_marvin_vorher_nachher_F1726_F1737.png`, Ausschnitt um Marvin, oben vorher/unten nachher): vorher
  F1730-F1733 Rueckfall in die vorgebeugte Schiebehaltung, nachher bleibt er aufrecht. Mittlere Pixelabweichung zu F1729
  im Ausschnitt: F1730 vorher 3.92 / nachher 0.00, F1731 4.02 / 0.02, F1732 3.50 / 0.08, F1733 2.75 / 0.39 (danach
  gleiches Ausklingen in Clip 1, F1737 1.36 / 1.36).
- Riegel unit_r38_werkstatt11b0_uebergabe: Phase 1->2 B1457 Bank 1 Clip 5; Phase 7 B1553 Loco Clip 0x11 Bild 0,
  Keyframe 266 == Keyframe von Phase 6 (266); alte Regel haette Containerbank Keyframe 275 (tiefe Schiebehaltung)
  gezeigt; Folgebild Bank 1 Clip 1 Bild 0; 0 Rueckfall-Bilder; Merker verbraucht.

## Tests
Neue Riegel (`re15_port/tests/unit/probes/r38_werkstatt11b0.cmake`, registriert NUR dort):

| Test | Punkt | misst | Ergebnis |
|---|---|---|---|
| unit_r38_werkstatt11b0_text | 1 | msg 20 Bytes (33 B), Kopf/Ende == ROOM11B0 @0x1BD0/@0x1BFA, Formbeleg @0x19CD (`2b 44 18 00`, `57 57 57`), Programm 294 B unveraendert, `re15_msg_get_text(20)` == "Marvin: But, hold on..." | gruen |
| unit_r38_werkstatt11b0_takt | 2 | 11B0 Satz 7 @0xE4A0 leer; ROOM1171 EDT/VH/VB @0x6EDC/0x6F5C/0x87FC, Satz 7 @0x6EF8 `00 00 45 10`; Ton 4 @0x77FC + VAG 4 (6688 B) == ROOM1090; Zustaendigkeit (11B0/7 ja; 11B0/0, 11B1/7, 1090/7 nein); Szene: 8 Satz-7-Anforderungen B1469 x2, B1494 x2, B1519 x2 (Marvin Clip 0x12 Bild 1), Phase 7 B1552/B1553 | gruen |
| unit_r38_werkstatt11b0_uebergabe | 3 | Bankwahl wie main.c je Bild: Phase 1->2 Bank 1 Clip 5; Phase 7 Loco Clip 0x11 Bild 0, Keyframe 266 == Phase 6; alte Regel Containerbank kf 275; Folgebild Bank 1 Clip 1 Bild 0; 0 Rueckfall-Bilder; Merker verbraucht | gruen |
| unit_r38_werkstatt11b0_ton | 2 | echtes audio_pc.c + werkstatt11b0_pc.c, CAP_SYNC: 11B0 ohne Ergaenzung 0, mit 108 200 792, ROOM1090 108 200 792, 147 000/147 000 Samples gleich, Tonlaenge 32 Bilder, 11B1/11B0-Satz-0 unberuehrt | gruen |
| integration_r38_werkstatt11b0 | 1+2+3 | echte exe: A `[msgtext] ... id=20 "Marvin: But, hold on..."`; 8 gekeyte Toene F1646 x2/F1671 x2/F1696 x2/F1729/F1730 (Stimme 0, vag 3), F1646 Marvin Clip 0x12 Bild 1; Uebergabebild F1730 `17 17 10 0 0 10 1 0`, F1731 `1 1 16 0 0 16 0 0`, kein Rueckfall; B Mess-Haken STUMM: Mitschnitt A/B bis F1646 gleich (Versatz 254 Bloecke), danach verschieden | gruen (168.29 s) |

## OFFEN
1. **Leons Uebergabebild Phase 1 -> 2** (nicht gemeldet, nicht Teil des Auftrags): der Spieler-Sub 0x13 waehlt die Bank
   ueber `re15_szl_schub_spieler_pl00/_wclip` nach +0x6 (szenenleon_r36.c), im Bild F1634 also PL00 Clip 5 statt des
   W-Bank-Clips 5, den Phase 1 posiert (@0x8005294c). Ein Bild, im Framedump nicht als Sprung sichtbar: mittlere
   Bilddifferenz im Leon-Ausschnitt F1633->F1634 1.47 (inkl. Kurs-Rastung), Nachbarn 0.80 / 1.14 / 2.32 / 3.26.
   Messweg bei Bedarf: RE15_ANIM_TRACE Aktor 0 F1633..F1635 + Framedump, Regel wie §3.3 (Bank des posierten Clips).
2. **ROOM1171 (Elza)**: dieselbe Sub-0x13-Kanalregel gilt dort fuer den Partner (Sce_em_set sub07 @0x016C8 Typ 0x49);
   die Logik (Phasen, Takt, Yield-Tor `in_motion_pose`) ist unveraendert, nur die Bank im Uebergabebild. Gemessen ist
   1171 hier nicht (Spur I "heliport1171" besitzt den Raum; ihr Befund "Elza haengt in der Szene" haengt nicht an der
   Bankwahl). Der Schiebe-Ton klingt in 1171 schon immer (eigener Satz 7 @0x6EF8).

## Fuer den Nutzer
- **Keine neuen Asset-Dateien** (Paket-Gate unveraendert): der Schiebe-Ton wird zur Laufzeit aus der schon
  ausgelieferten `shared_assets/PSX/STAGE1/ROOM1171.RDT` gelesen (Satz 7 der Raum-Tonbank). Fehlt die Datei, bleibt
  nur der Schiebe-Ton stumm (`[schub-r38] ... fehlt`).
- **Sprachdatei**: zu msg 20 gibt es keine Aufnahme (`synchro/STAGE1/room11B0/` endet bei main14.wav) — nichts spricht
  den alten Wortlaut. Wer die Zeile vertonen will: `synchro/STAGE1/room11B0/main20.wav` mit "But, hold on..."
  (wird automatisch eingebunden, synchro/README.md).
- **Ansehen**: Leon-Spielstand vor der Container-Szene (Strom an, ROOM11C0 erledigt) -> ROOM11B0. Beim gemeinsamen
  Schieben kratzt der Container jetzt dreimal im Schritt-Takt (Bild 1 jedes Schub-Zyklus) und einmal beim Loslassen —
  es ist exakt der Ton, den Leon beim Kistenschieben in ROOM1090 macht. Danach richtet sich Marvin auf und bleibt
  aufrecht stehen (vorher kippte er fuer ~4 Bilder zurueck in die Schiebehaltung = der "Fall"). Spaeter sagt er
  "But, hold on..." und laeuft zur Werkstatt-Tuer.
- Beta -> Retail: RE1.5 hat das Schieben samt Ton fertig (ROOM1090/1170/1171); nur die Raumbank von 11B0 war
  unfertig — deshalb RE1.5-Ton, kein RE2-Ton (RE2 hat je Raum andere Schiebe-Samples und keine Container-Szene).
- Keine Entscheidungsfragen.
