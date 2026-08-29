# Cutscenes ROOM11C0 / ROOM11B0 / ROOM1050 — drei Nutzer-Fragen (RECON)

Stand 2026-08-29. ⛔ Reiner Recon — kein Code geändert, kein Build. Alle Zitate sind
Datei-Byte-Offsets in `re15_port/shared_assets/PSX/STAGE{1,2}/ROOM####.RDT` (mit Roh-Bytes),
dekodierte Message-Bytes (Zeichensatz = `re15_port/engine/src/msg_common.c:177-201`,
derselbe wie im 10F0-Präzedenzfall) oder Port-Quellstellen mit ihren `@0x…`-Original-Zitaten.
Werkzeug: erweiterte Kopie des verifizierten Walkers `re15_port/tools/aot_sce_census.py`
(Scratchpad `scdtool.py`; identische Sektions-/Längenlogik inkl. beider `rdt_section_end`-Fixes;
alle hier disassemblierten Räume ohne Desync-Stopp). Sweeps liefen über **alle 240 RDTs**
(1229 dekodierte Messages, 844 Sce_em_set-Records).

---

## 0. Kurzfassung

| Frage | Antwort |
|---|---|
| **(A)** Cutscene mit Ada UND Marvin zusammen? | **JA — aktiv im Spiel, nicht deaktiviert.** Direkt nach der 11C0-Kette: **ROOM11B0 sub06** (Manhole-Szene, Datei 0x1478-0x17EC) positioniert Marvin (em-Slot 0, Typ 0x40) **und** Ada (em-Slot 1, Typ 0x42) gemeinsam neben Leon; Ada steigt am Ende zuerst in den Gully („you help her get down" = Ada). Danach **ROOM1260 sub02** (beide zusammen im Kanal) und **ROOM2000 sub02** (STAGE2, Drei-Personen-Dialog „Leon: Ada, Marvin, hold on.", msg00-11). Weitere gemeinsame Spawn-Räume: 3070, 3080, 30E0, 4000, 4010, 6020. |
| **(B)** Szene mit Tod/Fund des Chiefs? | **NEIN — existiert nirgends, auch nicht deaktiviert.** „Chief" = **Irons** (1150-Dialog; 10D0 msg20: „the chief is injured, one of those things bit him"). Irons-Entity (Typ 0x45) spawnt game-weit **nur** in ROOM1150/1151. Message-Sweep über alle 240 RDTs: kein einziger Text über Tod/Leiche des Chiefs. Leons „Kopfschütteln" in 11B0 ist eine reine `Plc_neck`-Gesten-Sequenz @0x154E/0x155C, keine abgeschnittene Szene. |
| **(C)** ROOM1050 „Give me a moment, I need a break"? | **Die beiden Zeilen sind WAISEN (deaktiviert), die Szene drumherum ist AKTIV.** msg06/msg07 (Datei 0x1007/0x1037) werden von **keinem** Opcode referenziert (kein `Message_on`, kein AOT). Die zugehörige Szene ROOM1050 **sub03** (Ada läuft davon, Leon: „Hey, wait!" = msg08) feuert beim Raum-Betreten, wenn **Flag(3,0x6E)==1** — gesetzt **ausschließlich** von ROOM1090 sub03 (Ada-Rettung aus dem brennenden Lagerraum). Erreichbar über die 1090-Kette (Feuerlöscher aus ROOM1000). ⚠️ Port-Risiko: die 1090-Kette hängt an einer per `Aot_on` gefeuerten **Selbst-Tür** (dest==selber Raum) — im Port teleportiert die nur (aot_common.c:518), sub00 läuft nicht neu → Szene feuert dort erst nach Verlassen+Wiederbetreten. |

---

## 1. ROOM11C0 (Parkhaus-Rampe, Leon) — vollständige Karte

Datei: `STAGE1/ROOM11C0.RDT` (155 552 B), main@0x1710, sub@0x1758, msg@0x1C84.
ROOM11C1 (Elza) hat **keine** Message-Sektion (Header 0x3C = 0) und **keinen** Ada/Boss-Inhalt —
sub00 spawnt dort 5 Hunde Typ 0x20 (@0x16F8-0x175C). Der ganze Komplex ist Leon-exklusiv.

### 1.1 Messages (alle 10, dekodiert aus der MSG-Sektion)

| # | Datei | Text |
|---|---|---|
| 0 | 0x1C98 | Leon: Ada! Where's Marvin? Wasn't he with you? |
| 1 | 0x1CD0 | Ada: He turned around all of a sudden to go look for you. |
| 2 | 0x1D13 | Ada: There was this really loud noise from upstairs earlier. |
| 3 | 0x1D59 | Ada: You didn't run into him? |
| 4 | 0x1D80 | Leon: What!? |
| 5 | 0x1D96 | Leon: Ada, you hide inside that patrol car. |
| 6 | 0x1DCB | Ada: O-okay, I'll do that... |
| 7 | 0x1DF1 | Ada: Okay, it's over now. |
| 8 | 0x1E14 | Ada: ...There are some outrageous monsters out there. |
| 9 | 0x1E53 | Leon: Yeah... Anyway, let's get out of here. |

**Alle 10 Messages sind referenziert** (sub02: msg00-06 @0x18A0/0x18D4/0x18EC/0x1904/0x1928/0x1A5E/0x1A80;
sub03: msg07-09 @0x1B68/0x1B7A/0x1B86). In 11C0 liegt **kein** deaktivierter Text.

### 1.2 Spawns (sub00 @0x1768) und Trigger (sub01 @0x181C)

```
0x1768  06 00 60 00        If
0x176C  21 04 40 00        Ck  Flag(4,0x40)==0        ; Intro-Szene noch nicht gelaufen
0x1770  44 00 42 40 …      Sce_em_set slot0 ADA(0x42) x=-8965 z=-14347   (sichtbar)
0x1784  44 01 27 30 02 00 00 60 3c fb e0 b1 c0 ab 00 00 60 0a 00 00
                           Sce_em_set slot1 BOSS(0x27) y=-20000 (geparkt) persist=0x60
0x1798  44 02 27 30 …      Sce_em_set slot2 BOSS(0x27) y=-20000 persist=0x61
        Else (Flag(4,0x40)==1 && Flag(3,0x43)==0):
0x17D4  Ada geparkt y=-20000 an der Streifenwagen-Position (-18214,-7229)
0x17E8/0x17FC  beide Bosse AKTIV (beh 0x10, y=0)      ; die Boss-Phase
        (beide Flags gesetzt -> Raum leer)
```

Typ 0x27 = der ROOM11C0-Parkhaus-Boss (Memory `reai-v2-maggot-ai`); `persist` =
Kill-Flag-Index (Sce_em_set pc[7], byte-true FUN_800420a0 @0x80042120-38, scd_vm.c:3270ff).

```
sub01 (läuft jeden Frame):
0x1820  21 04 40 00              Ck Flag(4,0x40)==0  -> 0x1824 Evt_exec (slot=0x0a) sub02
0x182A  06 00 1e 00 21 03 43 00  If Ck Flag(3,0x43)==0
0x1832  06 00 14 00 21 07 60 01  && Ck Flag(7,0x60)==1     ; Boss 1 tot
0x183A  06 00 0a 00 21 07 61 01  && Ck Flag(7,0x61)==1     ; Boss 2 tot
0x1842  04 ff 18 03              -> Evt_exec sub03
```

Bank 7 = die em-Status-Kill-Bank für STAGE1-3 (0x800b1038; Diskriminator DAT_800b0fe0<3
@0x800420ec — enemy_ai_common.c:12777 `re15_em_status_zone`). Die Indizes 0x60/0x61 sind exakt
die `persist`-Bytes der beiden Boss-Records → **sub03 feuert automatisch, sobald beide Bosse tot sind.**

- **sub02** (0x184E, Intro): setzt Flag(4,0x40)=1 (Einmaligkeit), spielt msg00-06
  („Ada! Where's Marvin?…"), Ada versteckt sich im Streifenwagen (sub07 @0x1C62: läuft zum Wagen,
  dann `Member_set y=20000` @0x1C74 = unter die Welt geparkt), Bosse fallen ein (Member_set y=-2500
  @0x198E/0x1996, Kamera-Shake-Schleifen 0x19A6-0x1A28), Evt_exec sub07 @0x1ABA.
- **sub03** (0x1B02, Nach-Kampf): setzt **Flag(3,0x43)=1** (`22 03 43 01` @0x1B02), msg07-09,
  Ada steigt aus dem Wagen und geht durch die Tür Richtung 11B0 (sub04-Wegpunkte bis
  (-25180,16380) = Tür-Rect der 11B0-Tür @main 0x0FAA-Gegenstück), `Aot_on 0` @0x1C08.

Flag(3,0x43) game-weit: **geschrieben nur hier** (11C0 sub03 @0x1B02), gelesen von 11C0 sub00/sub01
und **ROOM11B0 sub01 @0x11CE** → das ist der Zünder der Marvin+Ada-Szene (§2).

---

## 2. ROOM11B0 (Parkhaus B1, Leon) — vollständige Karte

Datei: `STAGE1/ROOM11B0.RDT` (232 492 B), main@0x0F4C, sub@0x1154 (12 subs), msg@0x18D8.

### 2.1 Messages (alle 16)

| # | Datei | Text | Referenz |
|---|---|---|---|
| 0 | 0x18F8 | Leon: H-hey! | sub04 @0x1338 (Sherry-Szene) |
| 1 | 0x190E | Marvin: Leon, you're okay! What was the commotion up there? | sub06 @0x14EE |
| 2 | 0x1953 | Leon: Those things broke through the shutters and started getting inside. | sub06 @0x150E |
| 3 | 0x19A6 | **Marvin: What about the chief?** | sub06 @0x152E |
| 4 | 0x19CD | Marvin: Oh, I see... Damn it! | sub06 @0x1574 |
| 5 | 0x19F4 | Marvin: Well you saw it, right? We can't get out using the patrol cars anymore. What should we do? | sub06 @0x1590 |
| 6 | 0x1A60 | Leon: Marvin, please come here! | sub06 @0x15B8 |
| 7 | 0x1A89 | Leon: There's a manhole here. How about an underground escape route? | sub06 @0x1622 |
| 8 | 0x1AD7 | Marvin: We don't really have any other option than to try. Okay, let's see if we can move this! | sub06 @0x166E |
| 9 | 0x1B40 | Marvin: Leon! give me a hand! | sub06 @0x168A |
| 10 | 0x1B67 | Marvin: Okay, let's hurry! | sub06 @0x176E |
| 11 | 0x1B8B | Leon: Marvin, I'll climb down first, you help **her** get down! | sub06 @0x178E |
| 12 | 0x1BD0 | Marvin: Okay! Be careful down there! | sub06 @0x17AE |
| 13 | 0x1BFE | It doesn't seem like you could move this on your own. | sub03 @0x126E/@0x1298 |
| 14 | 0x1C37 | The shutter is tightly sealed in place. | main00 Aot_set slot7 @0x106C |
| 15 | 0x1C62 | It's too dark to see anything...I must find way to activate the energy. | sub01-Retypes @0x1216/0x1220/0x122A |

**Kein deaktivierter Text in 11B0** — alle 16 referenziert. „her" in msg11 = **Ada** (die einzige
weibliche Figur der Szene; Sherry ist seit sub04 off-map, Beleg §3 in `sherry-despawn.md`).

### 2.2 Spawns und Gates

main00 spawnt die drei Begleiter **bedingungslos geparkt** bei (-30000,0,-30000):
slot0 = **Marvin 0x40** @0x1080, slot1 = **Ada 0x42** @0x1094, slot2 = Sherry 0x4B @0x10A8
(Etiketten-Beleg: `analysis/nutzer_batch_2026-08-29/sherry-despawn.md` §1).
Türen: slot0 → 1180 (dunkel) bzw. 1230 (Strom an, Flag 4:0xF3), slot1 → 11C0, slot5 → 1260
(als sce=0 INERT installiert @0x1010, scharf ab Flag 4:0xCD @0x1034).

```
sub01 (jeden Frame), Datei-Bytes @0x11C2:
  06 00 14 00  21 03 83 00   If Ck Flag(3,0x83)==0     ; Szene noch nicht gelaufen
  06 00 0a 00  21 03 43 01   If Ck Flag(3,0x43)==1     ; <- von 11C0 sub03 gesetzt!
  04 ff 18 06                Evt_exec sub06
```

Weitere sub01-Zweige: Sherry-Szene sub04 (Gate 3:0x82==0 && 4:0x06==1; 4:0x06 armiert main00
@0x0F56 nur bei 4:0xF3==1 = Strom an, gesetzt in ROOM11F0/11F1 sub18 @0x16F6);
Dunkel-Retype (4:0xF3==0): Tür 11C0, Item 4 und Shutter-AOT 7 werden zu msg15 umgetypt.

### 2.3 sub06 = die Szene, in der Marvin UND Ada zusammen auftreten (Frage A)

```
0x1478  22 03 83 01   Set Flag(3,0x83)=1   ; Einmaligkeit
0x147C  22 04 cd 01   Set Flag(4,0xCD)=1   ; Manhole offen (persistiert; main00-Tür 1260 scharf)
0x1480  22 04 f4 01   Set Flag(4,0xF4)=1   ; Handoff -> 1260/2000-Folgeszenen
0x149C  Pos_set Spieler (-24053,0,-22639)
0x14B0  2e 02 01 00 / 32 00 ef 9d 00 00 ee a6  Work_set(2,1)=ADA,  Pos_set (-25105,0,-22802), dir 3072
0x14C8  2e 02 00 00 / … / 32 00 92 a7 00 00 c4 ad  Work_set(2,0)=MARVIN, Pos_set (-22638,0,-21052)
```
→ Leon, **Ada und Marvin stehen gleichzeitig sichtbar beieinander**; danach der Dialog msg01-05
(inkl. „What about the chief?"), gemeinsames Manhole-Schieben (Spieler+Marvin `Plc_dest mode 19`
@0x170E/0x171A, Deckel-Objekt `Member_set 0x06=513` @0x1706), `Aot_reset slot5 sce=2` @0x172E
(öffnet die 1260-Tür), Ada kommt zum Gully (@0x175C-0x1766) und steigt zuerst hinab
(msg11 „you help her get down"), Abschluss-Gesten, `Aot_on 5` @0x17EA = Tür sofort feuern.

### 2.4 Leons „Kopfschütteln" nach „What about the chief?" (Frage B, Mechanik)

Nach msg03 (@0x152E) folgt auf dem SPIELER (Work_set kind=1 @0x154A):

```
0x154E  41 02 00 00 00 00 2c 01 00 0a   Plc_neck Mode 2 (param 300/10)
0x1558  09 0a 1e 00                     Sleep 30
0x155C  41 04 03 00 00 00 00 00 64 00   Plc_neck Mode 4 param 3
0x1566  09 0a 3c 00                     Sleep 60
0x156A  41 01 92 a7 …                   Plc_neck zurück auf Marvin
0x1574  2b 04 00 00                     Message_on msg04 „Oh, I see... Damn it!"
```

Die Antwort auf die Chief-Frage ist im Skript **nur diese Kopf-Geste + Marvins Resignation** —
es gibt keinen abgezweigten, stillgelegten Szenen-Pfad (sub06 ist linear, kein If zwischen
msg03 und msg04).

---

## 3. Frage (A): Ada und Marvin zusammen — game-weiter Befund

Sce_em_set-Sweep (844 Records / 240 RDTs): Räume mit **Marvin (0x40) UND Ada (0x42)**:
**11B0, 11B1, 1260, 2000, 3070, 3080, 30E0, 4000, 4010, 6020.**
(0x40-only: 10D0, 11A0, 3000, 3010, 3020, 3060, 30C1, 6030/6031; 0x42-only: 1050, 1090, 3091, 50B0.)

Aktive gemeinsame Szenen mit Beleg:

1. **ROOM11B0 sub06** (§2.3) — beide gleichzeitig positioniert und handelnd. Gate: 3:0x43 aus 11C0.
2. **ROOM1260 sub02 @0x1242**: `If Ck(4,0xF4)==1` (Bytes `06 … 21 04 f4 01` @0x1246) → Ada slot0
   (26750,-1800,11800) + Marvin slot1 (25400,-1800,9850) direkt unter dem Gully, mit Lauf-Subs 04/05;
   sonst-Zweig @0x128C: beide bei (-7500/-8900) nahe der 2000-Tür. 4:0xF4 wird in 11B0 sub06 @0x1480
   gesetzt → beim Abstieg stehen beide unten und laufen mit.
3. **ROOM2000 sub00 @0x1794** (Bytes `06 00 34 00 21 04 f4 01 04 ff 18 02 44 00 42 40 … 44 01 40 40 …`):
   `If Ck(4,0xF4)==1` → Evt_exec sub02 + Ada (550,-1800,-20800) + Marvin (2450,-1800,-20400).
   sub02 (@0x17FA, löscht 4:0xF4) = Drei-Personen-Dialog msg00-11: „Leon: Ada, Marvin, hold on." /
   Sherry-Suche / „Leon: Marvin sees Ada up front." (msg-Offsets 0x1BD8-0x1E66).
   Flag(4,0xF4) game-weit: Setzer 11B0 sub06 (Leon) und 3001 sub03 (Elza-Route, eigene Nutzung),
   Leser 1260/2000 bzw. 3061 — saubere Zwei-Raum-Handoffs.
4. Spätere Story-Räume mit beiden Spawns: 3070 (Lift), 3080, 30E0, 4000 („Take care of Marvin!"),
   4010 (verwundeter Marvin), 6020 — nicht einzeln disassembliert (nicht Auftragskern).

**Fazit (A): Es gibt nichts zu reaktivieren — die gemeinsamen Auftritte sind ausgeliefert und
im normalen Spielfluss erreichbar.** 11C0-msg00 („Wasn't he with you?") ist konsistent: Marvin war
mit Ada unterwegs, drehte um (msg01), und trifft in 11B0 wieder auf Leon+Ada.

---

## 4. Frage (B): Tod/Fund des Chiefs

1. **„Chief" = Irons.** ROOM1150 (Leon, Briefing) msg02-14: Irons ist verwundet
   („I'll be fine…", „don't make my mistake…" @0x13E5-0x175A). ROOM10D0 msg20 @0x2298:
   „the chief is **injured**, one of those things **bit him**"; msg26 @0x24CE: Marvin: „Okay,
   I'll meet with the chief first…"; ROOM1170 msg16 @0x1C31: „I have to report this to the chief."
2. **Message-Sweep über alle 240 RDTs** (1229 dekodierte Messages): Treffer für „chief"/„Irons"
   nur 10D0 (3×), 1120/1121 (Poster-Gag „Chief for a Day"), 1150/1151 (Dialog), 1170, 11B0 (msg03).
   **Kein** Text über Tod, Leiche oder Verschwinden des Chiefs.
3. **Entity-Sweep:** Typ 0x45 (Irons — Port lädt EM045 für den 1150-NPC,
   `re15_port/tests/unit/probe_irons_neck_1150.c:109`) spawnt game-weit **nur** in ROOM1150/1151
   (844-Record-Sweep). Es existiert kein Raum mit einer Irons-Leiche/-Szene.
4. 11B0 sub06 ist linear (§2.4) — kein deaktivierter Zweig hinter der Chief-Frage.

**Fazit (B): Eine Chief-Todes-/Fund-Szene existiert im Auslieferungsstand nicht — weder aktiv
noch deaktiviert.** Die Erzählung endet bei „verwundet/gebissen" (1150/10D0) + Geste (11B0).
Grenze des Sweeps: nur RDT-Message-Sektionen und RDT-Skripte; XA-Voice-Audio (`VOICE/`,
`synchro/`) wurde nicht auf Chief-Inhalte durchsucht (die fraglichen Szenen sind aber
Text-Dialoge, keine Voice-Cutscenes).

---

## 5. Frage (C): ROOM1050 „Give me a moment, I need a break"

Datei: `STAGE1/ROOM1050.RDT` (297 324 B), main@0x0AD8, sub@0x0C10 (5 subs), msg@0x0E44.

### 5.1 Messages und Referenzlage

| # | Datei | Text | Referenz |
|---|---|---|---|
| 0 | 0x0E56 | It's a shutter switch. Will you push it? \<YES/NO\> | sub02 @0x0CAC |
| 1 | 0x0E84 | Various posters of missing persons… | main Aot slot8 @0x0BD2 |
| 2 | 0x0ED2 | I need a fuse to run the shutter. | **UNREFERENZIERT** (auch in 1051) |
| 3 | 0x0EF7 | Just scattered papers and books… | main Aot slot9 @0x0BE6 |
| 4 | 0x0F2E | A blackboard full of reports of riots… | main Aot slot10 @0x0BFA |
| 5 | 0x0F7E | He's got a big bite on his neck… The weapon can be useful… | **in 1050 UNREFERENZIERT**; in **1051** aktiv (sub03 @0x0DB6 via sce-3-AOT slot11 @0x0C0E) |
| 6 | 0x1007 | **Ada: Give me a moment, I need a break.** | **UNREFERENZIERT (Waise)** |
| 7 | 0x1037 | **Leon: Relax, I'll take you to the medical room.** | **UNREFERENZIERT (Waise)** |
| 8 | 0x1070 | Leon: Hey, wait! | sub03 @0x0DD6 (`2b 08 00 00`) |

ROOM1051 (Elza) hat nur msg00-05 und **zusätzliche** AOTs (Leichen-Examine slot11 sce-3→sub03,
Item slot12 Typ 0x04 n=15 bit=0xA5; sub01 räumt das Examine nach Item-Nahme ab @0x0CBA) —
in Leons 1050 fehlen diese AOTs, dafür trägt nur Leons Variante die Ada-Szene und die
Waisen-Zeilen msg06/07. Die Message_on-Suche ist erschöpfend: 1050 enthält genau zwei
`Message_on` (id 0 @0x0CAC, id 8 @0x0DD6) und drei sce-1-AOTs (msg 1/3/4).

### 5.2 Die aktive Szene: sub00-Gate → sub03/sub04

```
sub00 @0x0C86 (Bytes: 06 00 1e 00 21 03 6e 01 44 00 42 40 00 00 00 ff 76 48 00 00 e4 a8 00 00 00 0c 00 00 04 ff 18 03 08 00):
  If Ck Flag(3,0x6E)==1
     Sce_em_set slot0 ADA(0x42) x=18550 y=0 z=-22300 dir=0x0C00 persist=0xFF
     Evt_exec sub03

sub03 @0x0D88:  22 03 6e 00  Set Flag(3,0x6E)=0        ; verbraucht das Gate
  Cutscene-Klammern (2:0x07 / 1:0x1B), Cut 6,
  Spieler auf (19850,-22300)/dir 3072   ; == exakt der Ankunfts-Spawn der 1090-Tür slot0!
  Ada (Work 2,0) läuft mode=9 nach (16250,-16650), Evt_exec sub04,
  Spieler folgt mode=9, Warte auf Flag(5,0x20),
  0x0DD6 Message_on msg08 „Leon: Hey, wait!" + Gesten, Plc_ret.
sub04 @0x0E00: Ada läuft weiter nach (16250,-14200) und wird auf (850,-14200)
  versetzt (Member_set @0x0E2C/0x0E30) = aus dem Sichtbereich nach Westen.
```

**Getriggert wird die Szene also automatisch beim Betreten von 1050 mit Flag(3,0x6E)==1.**
Die zwei zitierten Dialogzeilen msg06/07 waren offensichtlich für genau diese Stelle
geschrieben (Ada erschöpft → Leon will sie ins Behandlungszimmer bringen → sie läuft
trotzdem weg → „Hey, wait!"), sind aber im ausgelieferten Skript **nicht verdrahtet**.

### 5.3 Der Armierungs-Weg: ROOM1090 (Frau im brennenden Lagerraum)

Flag(3,0x6E) game-weit: **Setzer nur ROOM1090 sub03 @0x24D6** (`22 03 6e 01`), Leser nur 1050
(sub00 @0x0C8A, Clear sub03 @0x0D88). Die 1090-Kette (alle Offsets ROOM1090.RDT):

1. **Hilferufe**: sub01 @0x23F8 (Latch 3:0x80) → sub02 msg00-02
   („Woman: Anyone! Can someone please get me out of here!?").
2. **Feuerlöscher**: Flag(3,0x85) wird beim Feuerlöscher-Pickup in **ROOM1000 sub01** gesetzt
   (Sweep: Setzer @1000 sub01 0x0D0C; Item „fire extinguisher" in ROOM1000, vgl.
   `analysis/nutzer_batch_2026-08-27/item-name-feuerloescher.md`). Ohne ihn zeigt AOT slot2
   nur msg07 (sce-1 @0x230E: „I must hurry up and get something to put out this fire to save
   that woman!"); mit ihm (3:0x85==1 && 3:0x81==0) wird slot2 zum sce-3-Event @0x2336 → **sub06**.
3. **sub06** @0x2702 (Bytes `2b 07 ff ff 02 00 2b 08 ff ff … 21 0c 1f 00 … 22 03 81 01 22 03 84 01 47 03`):
   msg08 „Will you use the Fire Extinguisher? \<YES/NO\>"; Antwort-Flag (12,0x1F)==0 = YES
   (byte-true: msg_common.c:518 schreibt die Wahl nach Flag(12,31)) → Set 3:0x81 (Feuer aus),
   Set **3:0x84** (Rettungs-Szene armiert), `Aot_on 3` = die in sub00 @0x2352 installierte
   **Selbst-Tür** (dest 0x1090, next (1252,-1800,-2529), cut 6) wird SOFORT gefeuert
   (Aot_on = FIRE-NOW, byte-true LAB_800407bc — scd_vm.c:3749ff, dort ist genau dieser
   1090-slot3 als Sonderfall dokumentiert).
4. Beim (Neu-)Laden mit 3:0x84==1: sub00 @0x22A2 (Bytes `06 00 40 00 21 03 84 01 … 44 00 42 40 … 04 ff 18 03`)
   → **Ada-Spawn (0x42) + Evt_exec sub03 = Rettungs-Szene** (msg03-06: „There! Are you okay?" /
   „You saved me…" / „We can talk later…" / „Besides, you need to get that **wound treated**,
   right? Let's go." — die Verletzung, auf die 1050-msg07 „medical room" anspielt; die Frau IST
   Ada: Typ 0x42, und 30C1 msg06 nennt sie „Someone we rescued from the RPD").
   sub03 setzt 3:0x84=0, 3:0xBB=1 (Zombie-Spawns beim späteren Besuch) und **3:0x6E=1**.
5. **1090-Tür slot0 führt direkt nach 1050** (@0x211A, next (19850,0,-22300) = die
   Spieler-Position von 1050-sub03) → beim nächsten 1050-Betreten läuft die Break-Szene.

### 5.4 Erreichbarkeit / Port-Risiko

- **Original (statisch belegt):** Kette vollständig verdrahtet und erreichbar
  (Feuerlöscher ROOM1000 → 1090 löschen → Rettungs-Szene → 1050-Szene). HYPOTHESE (nur der
  letzte Schritt der Selbst-Tür): dass der Original-Warp (FUN_8001d600, Door-Zweig @0x8001d82c)
  bei dest==aktueller Raum den Raum **neu lädt** (und damit sub00 mit 3:0x84 sofort ausführt),
  ist nicht dynamisch gemessen. Falls nicht, braucht es im Original ein Verlassen+Wiederbetreten —
  die Szene bleibt in beiden Lesarten erreichbar.
- **Port:** `aot_common.c:518` behandelt dest_id==g_current_room_id als reinen Teleport
  (kein Reload, sub00 läuft nicht) — dieselbe Klasse wie die ROOM1190-Lektion
  (Memory `reai-v2-pin-prueft-zustand-nicht-weg`). Im Port feuert die 1090-Rettung deshalb
  über die Selbst-Tür NICHT sofort, sondern erst beim nächsten echten Raumwechsel
  (z.B. 1090→1100→1090 oder 1090→1050→1090). Danach ist 3:0x6E gesetzt und die
  1050-Szene läuft normal. **Port-Bug-Kandidat, separat zu verifizieren** (nicht Teil
  dieses Recons).

---

## 6. Aktivierbarkeit im Port (RE15_FORCE_EVENT / Flags)

`RE15_FORCE_EVENT=N[@F]` feuert `sub_scd[N]` einmalig nach dem Raum-Setup
(`re15_port/platform/pc/main.c:5321-5346`, `scd_event_fire`). Pro Szene:

| Szene | Reicht FORCE_EVENT? | Empfehlung |
|---|---|---|
| 11B0 sub06 (Marvin+Ada) | **JA** — Marvin/Ada/Sherry spawnen bedingungslos in main00 (@0x1080-0x10A8); sub06 positioniert sie selbst per Pos_set. | `RE15_DEBUG_JUMP=11b0…` + `RE15_FORCE_EVENT=6`. Natürlicher Weg: Strom an (11F0 sub18) → 11C0-Bosse töten → 11B0 betreten. |
| 11C0 sub02 (Ada-Intro) | Feuert ohnehin automatisch bei Erstbetreten (sub01 @0x1824). FORCE_EVENT=2 doppelt nur. | Einfach Raum betreten. |
| 11C0 sub03 (Nach-Kampf) | **Teilweise** — Ada existiert in jedem sub00-Zweig, steht aber nur im Zweig „4:0x40==1 && 3:0x43==0" an der Streifenwagen-Position, von der die Szene sie hochholt (Member_set y=0 @0x1B52). Bei frischem Raum (4:0x40==0) startet sie versetzt. | Byte-true: beide Bosse töten (setzt Flags 7:0x60/7:0x61 automatisch beim Death-Commit) — sub03 feuert dann von selbst. |
| 2000 sub02 (Drei-Personen-Dialog) | **NEIN** allein — Ada/Marvin-Spawns hängen am selben `Ck(4,0xF4)`-Zweig wie der Evt_exec (@0x1794). | 11B0 sub06 vorher laufen lassen (setzt 4:0xF4), dann 1260→2000; oder 4:0xF4 vor Betreten setzen. |
| 1050 sub03 (Ada-Break) | **NEIN** allein — der Ada-Spawn hängt am `Ck(3,0x6E)`-Zweig in sub00 (@0x0C86); ohne Flag existiert kein Ada-Actor. | Flag(3,0x6E) vor dem Betreten setzen (1090-Kette spielen; im Port mit Zwischen-Raumwechsel wegen §5.4), dann 1050 betreten — die Szene feuert ohne FORCE_EVENT. |
| 1050 msg06/07 (die Waisen-Zeilen) | Nicht über FORCE_EVENT erreichbar — **kein Opcode zeigt sie**. Reaktivierung wäre ein Content-Patch (z.B. zwei `Message_on`-Ops in sub03 vor dem Losgehen), also eine Design-Entscheidung des Nutzers, kein byte-true-Fix. | — |

---

## 7. Offenes

1. **Selbst-Tür-Semantik im Original** (§5.4): dynamisch messen (DuckStation, 1090-Kette),
   ob FUN_8001d600 bei dest==current den vollen Raum-Reload macht. Entscheidet zugleich über
   den Port-Bug-Kandidaten (aot_common.c:518-Teleport überspringt sub00).
2. `Plc_neck`-Modi 2/4 (das „Kopfschütteln" @0x154E/0x155C) sind nicht bis auf die
   Handler-Semantik disassembliert — für die Fragen nicht nötig (Nutzer hat die Geste gesehen;
   das Skript enthält keinerlei alternativen Pfad).
3. msg02 („I need a fuse…") ist in 1050 UND 1051 unreferenziert — Alt-Content einer
   früheren Shutter-Logik (der Switch @sub02 öffnet ohne Item-Check). Nur Randnotiz.
4. XA-/Voice-Bestand (`VOICE/`, `synchro/`) wurde nicht auf Chief-/Ada-Marvin-Inhalte
   durchsucht (Frage-B-Grenze, §4).
5. 11B1/Elza nutzt die 0x40/0x42-Spawns eigenwillig (Sherry-Intro sub02 mit Gate 9:0x0A,
   sub03 mit Gate 3:0x46) — nicht vertieft, da alle drei Fragen Leon-Szenario betreffen.
