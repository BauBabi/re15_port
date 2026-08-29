# Ada+Marvin-„Vor-Szene" vor ROOM11C0 — existiert sie? (RECON)

Stand 2026-08-30. ⛔ Reiner Recon — kein Code geändert, kein Build. Nutzer-Logik: In 11C0 fragt
Leon „Ada! Where's Marvin? Wasn't he with you?" — die gemeinsame Ada+Marvin-Szene (11B0 sub06)
kommt aber erst DANACH. Fehlt also eine Szene DAVOR, in der Marvin und Ada zusammen sind?

**Antwort: NEIN. Eine solche Vor-Szene existiert nirgends — weder aktiv noch schlafend.
Leons Zeile ist Off-Screen-Story, und das Skript liefert die Erklärung selbst nach, in derselben
Szene:** 11C0 msg01 @0x1CD0 „Ada: He turned around all of a sudden to go look for you." +
msg02 @0x1D13 „Ada: There was this really loud noise from upstairs earlier."

Alle Zitate sind Datei-Byte-Offsets in `re15_port/shared_assets/PSX/STAGE{1..6}/ROOM####.RDT`
(Roh-Bytes bei den Kernfunden), Dekodierung = Port-Zeichensatz `re15_port/engine/src/msg_common.c:177-201`.
Werkzeug: `scdtool.py` + `dormant2_stage1_2_3_4_5_6.json` (Scratchpad; erweiterte Kopien des
100%-Coverage-Walkers `re15_port/tools/aot_sce_census.py`, 0 Desyncs über 240 RDTs).
Vorarbeit (nicht wiederholt): `analysis/nutzer_batch_2026-08-29/cutscenes-11c0-11b0-1050.md`
(11C0/11B0/1050-Vollkarten), `…/schlafender-content.md` (Waisen-Korpus).

---

## 1. Story-Reihenfolge ist hart verdrahtet: 11C0-Boss-Szene ZWINGEND vor 11B0 sub06

**(a) 11C0 ist eine Sackgasse hinter 11B0.** Game-weiter Tür-Sweep (alle `Door_aot_set` in 240
RDTs): Türen mit Ziel 0x11C0 existieren **nur** in 11B0 main00 @0x0FAA (slot1, sce=2,
dest_room=0x1C) und 11B1 @0x0F9C (Elza). 11C0s einzige Tür führt zurück: 11C0 main00 @0x1712
→ 0x11B0. Der Spieler ist also IMMER erst in 11B0, dann in 11C0 — aber beim ersten Durchqueren
von 11B0 kann sub06 nicht laufen (b/c unten).

**(b) Vor Strom-Ein ist die 11C0-Tür gar keine Tür.** 11B0 sub01 @0x11F2: `If Ck(4,0xF3)==0`
→ `Aot_reset slot=1 sce=1 pay[0f 00 …]` @0x1216 = die 11C0-Tür (slot1) wird zu msg15
(„It's too dark to see anything…") umgetypt. Flag 4:0xF3 hat game-weit genau zwei Schreiber:
11F0 sub18 @0x16F6 und 11F1 sub18 @0x16F6 (Generator-Szene). Reihenfolge also:
Strom an (11F0) → 11B0 → 11C0.

**(c) 11B0 sub06 hat genau EINEN Trigger, und der hängt an 11C0.** Bytes 11B0 sub01 @0x11C2:

```
06 00 14 00  21 03 83 00   If Ck Flag(3,0x83)==0      ; Szene noch nicht gelaufen
06 00 0a 00  21 03 43 01   If Ck Flag(3,0x43)==1      ; Zünder
04 ff 18 06                Evt_exec sub06
```

sub_refs von 11B0 sind erschöpfend kartiert (Evt_exec/Evt_chain/Gosub/AOT-sce3): sub06 wird
**nur** hier @0x11D2 gerufen; 11B0 enthält keinen AOT-sce3-Record auf sub 6.

**(d) Flag 3:0x43 hat game-weit genau EINEN Schreiber: 11C0 sub03.** Eigener Re-Sweep aller
`Set`/`Ck`-Ops über 240 RDTs:

```
11B0 sub01 @0x11CE: READ  Ck  3:0x43==1
11C0 sub00 @0x17D0: READ  Ck  3:0x43==0
11C0 sub01 @0x182E: READ  Ck  3:0x43==0
11C0 sub03 @0x1B02: WRITE Set 3:0x43=1     (Bytes: 22 03 43 01)
```

Zusatz-Sweep über alle AOT-sce4-FLAG-Records (`Aot_set`/`Aot_reset` sce=4, der zweite
Flag-Schreib-Kanal): **0 Treffer** für 3:0x43. Und 11C0 sub03 feuert erst, wenn beide
0x27-Bosse tot sind (sub01 @0x182A-0x1842: `Ck(3,0x43)==0 && Ck(7,0x60)==1 && Ck(7,0x61)==1`
→ `Evt_exec sub03`; Bank 7 = em-Kill-Bank, Indizes = die `persist`-Bytes 0x60/0x61 der beiden
Boss-Records @0x1784/0x1798 — Vorbericht §1.2).

**Fazit (1): 11B0 sub06 kann unter keinen Umständen vor der 11C0-Boss-Szene laufen.**
Die Chronologie Leon: 11B0 (dunkel/Transit) → 11F0 Strom → 11B0 → 11C0 (Ada-Intro sub02,
Boss-Kampf, sub03 setzt 3:0x43) → zurück nach 11B0 → sub06 (Marvin+Ada+Manhole).

---

## 2. Suche nach einer Marvin+Ada-Szene VOR 11C0 — alle Kanäle negativ

### 2.1 (a) Spawns: kein Raum vor 11B0 spawnt beide Typen

Frischer `Sce_em_set`-Sweep (alle 240 RDTs, main+sub), STAGE1-Treffer für 0x40/0x42 komplett:

| Raum | Record | Typ | Gate | Was passiert dort |
|---|---|---|---|---|
| 10D0 sub00 @0x1256 | `44 00 40 40 00 00 00 ff 8f 16 00 00 49 5c …` | **Marvin 0x40** @ (5775,23625) | `Ck(4,0xF7)==0` @0x123E | „Freeze!"-Erstbegegnung: Berühr-AOT slot18 sce3 ev=0x15 @0x1242 (gleicher Gate-Zweig, Rect (4700,19400,800,800) direkt vor Marvin) → **sub21** = msg12-27 (msg14 „Leon... You're still alive!" bis msg26 „Okay, I'll meet with the chief first…"). sub21 setzt 4:0xF7=1 @0x19EC (Einmaligkeit); Marvin geht ab: `Plc_dest mode=5 (7675,10700)` @0x1D6A, dann `Pos_set (22300,-4300)` @0x1D9E. Else-Zweig danach: Zombie 0x10 statt Marvin (@0x12A4). **Keine Ada** — kein 0x42-Spawn, kein Ada-Wort in allen 28 Messages. |
| 11A0 sub00 @0x118C | `06 00 1a 00 21 04 c6 00 44 00 40 40 01 00 00 ff c0 f9 f8 f8 9a 5b` | **Marvin 0x40** @ (-1600,-1800,23450) | `Ck(4,0xC6)==0` | Stummer Warteposten neben der 3000-Tür. 4:0xC6-Schreiber game-weit **nur** 3000 sub03 @0x166E / 3001 @0x16A6 (die STAGE3-Szene „Marvin! Aren't these... RPD officers!?" nimmt ihn mit). **11A0 ist vor 11C0 unerreichbar**: Türen hinein game-weit nur aus 2070, 20A0, 3000 (+2 11A0-interne Etagen-Türen @0x0F9C/0x0FC6) — alles Post-Manhole-Gebiet. Nur 3 Messages (Shutter/locked), kein Dialog. |
| 1090 sub00 @0x22CC | type=0x42, geparkt (-30000,-30000) | **Ada 0x42** | `Ck(3,0x84)==1` (Rettung armiert) | Rettungs-Szene sub03 (msg03-06, Vorbericht §5.3). Kein Marvin-Wort in allen 10 Messages (msg-Liste geprüft). |
| 1050 sub00 @0x0C8E | type=0x42 @ (18550,-22300) | **Ada 0x42** | `Ck(3,0x6E)==1` | Break-Szene sub03 (Ada läuft weg, „Hey, wait!"; Waisen msg06/07 = Ada+**Leon**-Dialog, Vorbericht §5). Kein Marvin-Wort in allen 9 Messages. |
| 11B0 main00 @0x1080/0x1094 | 0x40 slot0 + 0x42 slot1 geparkt (-30000,-30000) | **beide** | bedingungslos | **Der erste gemeinsame Raum.** Genutzt erst von sub06 (Gate §1). |
| 11C0 sub00 @0x1770/0x17D4 | type=0x42 | **Ada** | 4:0x40/3:0x43-Zweige | Die Gorilla-Szene selbst. Kein Marvin-Spawn. |

(11B1/Elza: die geparkten 0x42/0x40-Slots @0x1058/0x106C dienen den Sherry-Szenen sub02/sub03
— msg00-08 Sherry-Intro, msg10/11 „Sherry!? Where has she gone!?" — kein Ada/Marvin-Dialog.)

Ohne `Sce_em_set` existiert kein NPC-Actor im Raum (`Work_set kind=2` referenziert nur
raumeigene em-Slots) → der Spawn-Sweep ist der erschöpfende Kanal für „wer kann in einer
Szene auftreten".

### 2.2 (b) Waisen-Texte und ungetriggerte subs: nichts deutet eine gemeinsame Szene an

- **Game-weiter Waisen-Korpus** (dormant2-JSON, alle 240 RDTs; Waise = Message ohne
  `Message_on`/AOT-sce1-Referenz): Waisen mit „Marvin" im Text: **0**. Mit „Ada": **genau 1** —
  1050 msg06 @0x1007 „Ada: Give me a moment, I need a break." (der bekannte Ada+**Leon**-Rest,
  kein Marvin). STAGE1-Waisen-Vorkommen gesamt: 61 (== Zählung des Vorberichts, Sanity ✓).
- **Alle Messages der Kandidaten-Räume referenziert** (eigener Referenz-Check
  Message_on/Aot_set-sce1/Aot_reset-sce1): 10D0 28/28, 10D1 12/12, 11A0+11A1 3/3, 1090 10/10,
  11C0 10/10, 11B0 16/16, 11B1 15/15. (Einzige Waisen im Umfeld: 1050 msg02/05/06/07 und
  1150 msg11 — beide Fälle bereits dokumentiert, keiner enthält Marvin+Ada.)
- **Namens-Sweep über ALLE 1229 dekodierten Messages** (nicht nur Waisen): „Marvin" kommt in
  STAGE1 nur in 10D0 (msg14-26), 11B0 (msg01-12) und 11C0 (msg00) vor; „Ada" nur in 1050
  (msg06), 11C0 (msg00-08). Kein STAGE1-Text kombiniert beide außer 11C0 msg00 selbst.
- **Ungetriggerte subs** in 10D0/11A0/1090/1050/11C0: **keine** (sub_refs-Abgleich, alle subs
  haben Trigger). In 11B0 einzig sub02 @0x1238 — ein 14-Byte-Warte-Helfer ohne Inhalt
  (Bytes: `11 00 08 00 02 00 12 04 21 0c 18 01 01 00` = `Do{Evt_next}Edwhile Ck(12,0x18)`).
- **XA-Voice:** STAGE1 enthält game-weit genau 2 Xa-Opcodes, beide 1030/1031 @0x2822/0x290E
  (`59 05 04 01`, Vorplatz-Ambient). In den Ada/Marvin-Räumen liegt keinerlei Voice-Content —
  alle Dialoge sind Text (`Message_on`).

### 2.3 (c) 11C0/11B0 selbst: keine stillgelegte frühere Begegnung

- 11C0: subs 0-7 sämtlich referenziert (sub02←sub01@0x1824, sub03←sub01@0x1842,
  sub04←sub03@0x1BE6, sub05/06=Gosub-Helfer, sub07←sub02@0x1ABA); alle 10 Messages verdrahtet
  (Vorbericht §1.1 + eigener Re-Check).
- 11B0: alle 16 Messages verdrahtet; einziger triggerloser sub ist der leere Helfer sub02 (§2.2).

---

## 3. Antwort auf die Nutzer-Logik

**Leons Zeile ist Off-Screen-Story — und zwar absichtsvoll verdrahtete:** Die Antwort auf
„Wasn't he with you?" steht direkt in derselben Szene (11C0 sub02, msg00-03 @0x18A0-0x1904):
Ada erklärt rückblickend, Marvin **war** bei ihr und drehte um, um Leon zu suchen
(msg01 @0x1CD0), ausgelöst durch den Lärm von oben (msg02 @0x1D13). Die erzählte Lücke
schließt sich skriptseitig sauber:

1. **10D0 sub21**: Marvin trifft Leon, geht ab mit „I'll meet with the chief first…"
   (msg26 @0x24CE).
2. **1090→1050**: Leon rettet Ada, sie läuft ihm davon (Richtung Westen, `Member_set`
   @0x0E2C/0x0E30).
3. **Off-screen** (nie als Szene gebaut): Marvin trifft Ada, begleitet sie Richtung Parkhaus,
   kehrt wegen des Lärms um.
4. **11C0 sub02**: Leon findet Ada allein → die Frage; Ada liefert Punkt 3 als Dialog nach.
5. **11B0 sub06**: Marvin ist zurück in B1 („What was the commotion up there?" msg01) —
   erster tatsächlicher gemeinsamer Auftritt.

Bemerkenswert: Das 11C0-Intro hängt **nur** an `Ck(4,0x40)==0` (sub01 @0x1820) — Leon stellt
die Frage sogar dann, wenn die 1090-Rettung nie gespielt wurde. Auch das zeigt: die
Ada↔Marvin-Verbindung existiert ausschließlich als erzählte, nie als inszenierte.

**Es gibt daher nichts zu (re)aktivieren.** Der nächstliegende gemeinsame Content ist und
bleibt 11B0 sub06 — im Port sofort abspielbar (`RE15_DEBUG_JUMP=11b0…` + `RE15_FORCE_EVENT=6`;
Marvin/Ada/Sherry spawnen bedingungslos in main00 @0x1080-0x10A8, die Szene positioniert sie
selbst — Vorbericht §6).

---

## 4. Durchsuchte Orte (Negativ-Beleg-Umfang)

1. **Tür-Graph**: alle `Door_aot_set` aller 240 RDTs (Ziele 11C0/11B0/10D0/11A0 vollständig
   gelistet, §1a/§2.1).
2. **Spawns**: alle `Sce_em_set` aller 240 RDTs (Typen 0x40/0x42; deckungsgleich mit dem
   844-Record-Census des Vorberichts).
3. **Flags**: game-weite `Set`/`Ck`-Sweeps für 3:0x43, 4:0xF3, 4:0xF7, 4:0xC6 + AOT-sce4-Sweep
   für 3:0x43 (0 Treffer).
4. **Messages**: alle 1229 dekodierten Messages aller 240 RDTs (Namens-Suche „Marvin"/„Ada",
   case-sensitiv); Referenz-Status aller Messages in 10D0/10D1/11A0/11A1/1090/1091/1050/1051/
   11C0/11C1/11B0/11B1/1150/1151 einzeln geprüft.
5. **subs**: Trigger-Vollreferenz (Evt_exec/Evt_chain/Gosub/AOT-sce3) für dieselben Räume;
   game-weiter Untriggered-Korpus aus dem dormant2-Sweep.
6. **Voice**: Xa-Opcode-Sweep (0x59/0x5E) über ganz STAGE1.

Grenze: Die BSS-/Kamera-Bilddaten (Hintergrund-Videoframes) und die `voice/`-WAV-Rohablage des
Repos wurden nicht durchsucht — die fraglichen Szenen sind im Skript vollständig textbasiert
(kein einziger Xa-Aufruf in den beteiligten Räumen), eine „Szene" ohne Skript-Träger kann in
diesem Engine-Modell nicht existieren.

Reproduktion: Scratchpad `scdtool.py` (`disasm`/`msgs`/`sweepem`/`sweepflag`), Disasm-Dumps
`dis2_10D0.txt`, `dis2_11A0.txt`, `dis2_11C0.txt`, `dis2_11B0.txt`, `dis2_11B1.txt`,
`dormant2_stage1_2_3_4_5_6.json`.
