# Sicherungs-Raetsel ROOM1050 — Verdrahtung

Arbeitsbaum: `.claude/worktrees/wf_3a416b11-82e-1` · Start 2026-09-27

## 0. Der grosse Befund vorweg: RE1.5 HAT das Sicherungs-Raetsel — nur nicht in ROOM1050

Die Vorgabe sagte "Item 0x40 Fuse hat 0 Platzierungen". Das stimmt fuer den Opcode
`Item_aot_set` (0x50) — aber die Sicherung ist in RE1.5 **kein Inventar-Gegenstand**,
sondern ein **Flag-Gegenstand**. Sie ist im Auslieferungsstand vollstaendig verdrahtet:

### Fundstelle der Sicherung: ROOM2030 (STAGE2)
```
ROOM2030.RDT main00 @0x01BA6  Ck  bank=3 bit=108 val=0     ; noch nicht genommen?
ROOM2030.RDT main00 @0x01BAA  Aot_set slot=9 sce=3 flags=0x31 floor=1
                              rect=(-5900,6650,800,1000)  Nutzlast ff 00 18 06 -> sub06
ROOM2030.RDT sub06  @0x01FE4  Message_on 2   "Will you take the Fuse? "     (msg @0x208B)
ROOM2030.RDT sub06  @0x01FEE  Ck  bank=12 bit=31 val=0     ; Antwort JA
ROOM2030.RDT sub06  @0x01FF2  Set bank=3 bit=108 val=1     ; = "Sicherung genommen"
ROOM2030.RDT sub06  @0x01FF6  Aot_reset slot=9 sce=0       ; Aufnahmestelle verschwindet
ROOM2030.RDT sub06  @0x02000  Message_on 3   "You've taken the Fuse."       (msg @0x20AC)
```

### Benutzungsstelle: ROOM2060 (STAGE2) — und die ist das VORBILD fuer ROOM1050
```
ROOM2060.RDT sub00  @0x010A2  Ck  bank=3 bit=108 val=0  -> slot9 sce=1 msg 3
                              "One of the fuses is missing..."          (msg @0x1833)
ROOM2060.RDT sub00  @0x010C2  Ck  bank=3 bit=144 val=0  -> slot9 sce=3 -> sub18
ROOM2060.RDT sub00  @0x010DE  (sonst)                   -> slot9 sce=1 msg 6
                              "The fuse is in place."                    (msg @0x1892)
ROOM2060.RDT sub00  @0x010F2  Cut_replace 5,11
ROOM2060.RDT sub00  @0x010F5  Cut_replace 6,10          ; Zustand beim Wiedereintritt
ROOM2060.RDT sub18  @0x0167E  Message_on 4  "Will you use the Fuse? "     (msg @0x1855)
ROOM2060.RDT sub18  @0x01688  Ck bank=12 bit=31 val=0 -> Cut_chg 8 + Arbeitsbits
ROOM2060.RDT sub19  @0x0169E  Set bank=3 bit=144 val=1
ROOM2060.RDT sub19  @0x016C2  Cut_replace 5,11
ROOM2060.RDT sub19  @0x016C5  Cut_replace 6,10
ROOM2060.RDT sub19  @0x016C8  Message_on 5  "You've used the Fuse."       (msg @0x1875)
```

### ⛔ KORREKTUR AM AUFTRAG: der Mechanismus ist `Cut_replace`, NICHT `Cut_chg 8`
Gemessen an den Kameratabellen (@0x60, 32 B je Cut, letztes Dword = pri/bg-Offset):

| Raum | Cut-Paar | alle Felder gleich? | pri-Offset |
|---|---|---|---|
| ROOM2060 | 5 / 11 | ja | 0x65C / 0x674 |
| ROOM2060 | 6 / 10 | ja | 0x660 / 0x670 |
| ROOM2060 | 8 / 9  | ja | 0x668 / 0x66C |
| ROOM1050 | **7 / 8** | ja | **0x518 / 0x51C** |

ROOM2060 tauscht seine identischen Paare mit `Cut_replace src,dst` (Opcode 0x4B) —
einmal im Moment des Einsetzens (sub19) und einmal beim Raumladen (sub00), damit der
Zustand bestehen bleibt. `Cut_chg 8` waere nur ein einmaliger Blick; `Cut_replace 7,8`
macht Kamera-Slot 7 dauerhaft zum "Sicherung drin"-Bild. ROOM1050s Paar 7/8 hat genau
die Form, die ROOM2060 mit Cut_replace bedient.

Zensus `Cut_replace` ueber alle 240 RDT: 80 Vorkommen in 14 Raeumen
(1030/1031, 1130/1131, 11B0/11B1, 2040/2041, 2060/2061, 3090/3091, 30E1, 40A0/40A1).

## 1. Fundstelle fuer ROOM1050 — Wege, die ich gegangen bin
(f) TEXTSUCHE ueber ALLE 240 RDT-Nachrichtenbloecke nach "fuse": 20 Treffer in
    1050/1051 (msg 2, verwaist), 2030/2031, 2060/2061, 4030/4031 ("Main Fuse",
    eigenes spaeteres Raetsel mit MEHREREN Sicherungen).
(b) FLAG-VOLLSCAN bank=3 bit=108 / 144 / 121 ueber alle RDT: siehe oben, plus
    ROOM1050 sub00 @0x00C1E Ck(3,121,0) / sub02 @0x00CBA Set(3,121,1) = Shutter offen.
(d) ITEM-NACHBARN: 0x3F Pocket Watch = ROOM2090 main00 @0x00A12; 0x41 Spark Plug =
    ROOM4040 main00 @0x00F66. Keine raeumliche Nachbarschaft -> Indiz faellt aus.

(weiter in Arbeit)
