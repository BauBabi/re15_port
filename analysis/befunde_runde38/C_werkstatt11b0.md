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
- [ ] Punkt 1 Text "But, hold on..." — Umsetzung/nachher
- [ ] Punkt 2 Schiebe-Ton — Umsetzung/nachher
- [ ] Punkt 3 keine Fall-Animation — Umsetzung/nachher

## §1 Punkt 1 — Text "But, hold on..."

### 1.1 Messung vorher
- debug.log (Grundlauf, Erweiterung ab F2135): `[msgtext] room=11b0 id=20 "Marvin: Hold on."` — der heutige Wortlaut ist
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
  `lbu v0,2(s1)`) = ROOM11B0 Ton 0 vol **0**. Die Original-Szene ist an dieser Stelle STUMM.
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

## Tests

## OFFEN

## Fuer den Nutzer
