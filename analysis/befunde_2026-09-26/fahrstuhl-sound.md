# Fahrstuhl-Sound — RE1.5 hat KEINEN; RE2 hat GENAU ZWEI, und sie sitzen an derselben Skript-Stelle

## 1. Was der Nutzer meldet / was ich gemessen habe

Nutzer: "Der Fahrstuhl hat noch nicht den Fahrstuhl Sound, den musst du noch aus
Resident Evil 2 extrahieren."

Der Fahrstuhl ist **ROOM1080** (STAGE1). Beleg: `re15_port/include/debug_jump_table.h:22`
(aus `DEBUG.BIN` erzeugt), Stage-0-Slot **8** = `"ELEVATOR"` → Raum-Id 0x1080.

## 2. Das Original — mit Adressen und Bytes

### 2.1 ROOM1080.RDT enthaelt KEIN einziges Se_on (0x36)

Voller opcode-exakter SCD-Walk mit `re15_port/tools/scd_dump_room.py`
(Laengen aus `scd_vm.c s_opcode_sizes`, Abbruch bei unbekanntem Opcode):

```
ROOM1080.RDT  177696 B
header: nSprite=0 nCut=5 nOmodel=1 nItem=0 nDoor=0 nRoom_at=0 reverb=0
main_scd=0x480 sub_scd=0x4E4     (main 1 Region, sub 11 Regionen)
```
`grep -c Se_on` ueber den gesamten Dump = **0**. Dasselbe fuer ROOM1081.RDT
(= dieselbe Datei-Groesse 177696 B, dieselben Sektions-Offsets).

Die Fahrt selbst steht in `sub07..sub10`. sub07, Datei-Offsets aus ROOM1080.RDT:

```
0x006EE (+0000) Sleep       09 0a 01 00
0x006F6 (+0008) Ck          21 03 36 00      ; bank3 bit54
0x00726 (+0038) Cut_chg     29 02
0x00728 (+003A) Sleep       09 0a 0f 00      ; 15 Frames
0x00734 (+0046) Cut_chg     29 00
0x0073A (+004C) Member_set  34 00 ae ca      ; x = -13650
0x0073E (+0050) Member_set  34 02 cc f7      ; y = -2100
0x00742 (+0054) Member_set  34 04 00 0c      ; z = 3072
0x00746 (+0058) Set         22 01 1c 01      ; bank1 bit28 = 1
0x0074A (+005C) Sleep       09 0a 08 00      ; 8 Frames
0x0074E (+0060) Set         22 01 1c 00      ; bank1 bit28 = 0
0x00752 (+0064) Sleep       09 0a 5a 00      ; 90 Frames = die FAHRT (3 s @30 Hz)
0x00756 (+0068) Set         22 01 1c 01
0x0075A (+006C) Sleep       09 0a 08 00
0x0075E (+0070) Set         22 01 1c 00
0x00762 (+0074) Sleep       09 0a 14 00
0x00766 (+0078) Plc_dest    40 00 04 20 ae ca a8 fd
0x0077C (+008E) Aot_on      47 00
0x0077E (+0090) Evt_end     01 00
```

Zwischen `Cut_chg 29 02` und `Aot_on 47 00` steht **kein** Opcode 0x36 (Se_on),
kein 0x37, kein 0x38 (Xa_on). Die Fahrt ist im ausgelieferten RE1.5 **skript-stumm**.

### 2.2 Der Raum HAT zwei VAB-Baenke — sie werden nur nie per Se_on angetippt

RDT-Kopf (Datei-Offsets 0x08..0x1F, 6 x u32 LE; Feldbedeutung RE15_KNOWLEDGE.md §1.1):

```
ROOM1080.RDT  EDT0=0x2338  VH0=0x23B8  VB0=0x3E58
              EDT1=0x2FD8  VH1=0x3038  VB1=0x5398
```
VH0 @0x23B8: magic `pBAV`, ver=7, nprog=1, ntone=4, **nvag=3**, fsize=8544.
VAG-Groessentabelle @0x2DD8 → 48 B / 1552 B / 3840 B (Summe 5440 B = 0x3E58..0x5398 ✓).
Die drei Wellenformen sind 0,008 s / 0,25 s / 0,61 s (bei 11025 Hz) — **keine**
Motor-Dauerschleife.
VH1 @0x3038: nprog=2, ntone=19, nvag=15, fsize=113104 — das ist die gemeinsame
snd1-Bank (dieselbe Groessen-Signatur wie ROOM1130 snd1: nprog=2/ntone=22/nvag=15).

### 2.3 Gegenprobe: welche RE1.5-Raeume ueberhaupt Se_on haben

Census ueber ALLE 6 Stages (scd_dump_room.py, `grep -c Se_on`), Treffer:

```
1030:1 1040:13 1050:3 10D0:3 1130:3 1150:6 1171:8 1190:8 11A0:5
2060:12 20A0:19 3061:2 3080:3 3091:5 4000:8/4001:20 4040:3 4080:3
5040:16 5060:14 5070:6 50A0:2 5100:2      (+ jeweils die 1-Variante)
```
**ROOM1080/1081 (ELEVATOR) und ROOM4020/4021 (A-2 ELEVATOR) sind NICHT dabei.**
Der einzige "Lift"-Raum mit Se_on ist ROOM3080 ("WAREHOUSE LIFT",
`debug_jump_table.h:56` Stage-2-Slot 8), und dessen drei Se_on sind nachweislich
**nicht** das Fahrgeraeusch:

```
ROOM3080.RDT sub?  0x00958 Plc_motion  3f 00 07 00
                   0x0095C Se_on       36 02 0a 00 02 00 00 00 00 00 00 00
                   0x00968 Sleep       09 0a 14 00
                   0x0096C Plc_motion  3f 00 07 00
                   0x00970 Se_on       36 02 0a 00 02 00 00 00 00 00 00 00
                   0x00980 Plc_motion  3f 00 07 00
                   0x00984 Se_on       36 02 0a 00 02 00 00 00 00 00 00 00
```
Dreimal dieselbe Id (bank=2 → snd0, id=0x0a), jeweils direkt nach derselben
Spieler-Animation `Plc_motion(0,7)` und 20 Frames Abstand = ein **Schlag/Kurbel**-SE,
kein Dauerlauf.

### 2.4 RE2 hat den Fahrstuhl — und sein Fahrskript ist BYTE-GLEICH mit dem von RE1.5

Die RE1.5-Fahrt (§2.1) besteht aus der Folge

```
22 01 1c 01   Set   bank1 bit28 = 1     ; Tuer-/Kabinen-Flagge AN
09 0a 08 00   Sleep 8
22 01 1c 00   Set   bank1 bit28 = 0
09 0a 5a 00   Sleep 90                  ; DIE FAHRT
```

Diese 16 Bytes `22 01 1c 01 09 0a 08 00 22 01 1c 00 09 0a 5a 00` habe ich roh ueber
**alle** RDTs beider Spiele gesucht. Vollstaendige Trefferliste:

| Spiel | Datei | Datei-Offsets |
|---|---|---|
| RE1.5 | `shared_assets/PSX/STAGE1/ROOM1080.RDT` (ELEVATOR) | 0x746, 0x7D8, 0x86A |
| RE1.5 | `ROOM1081.RDT` | 0x746, 0x7D8, 0x86A |
| RE1.5 | `shared_assets/PSX/STAGE4/ROOM4020.RDT` (A-2 ELEVATOR) | 0x95A, 0xA00, 0xA96 |
| RE1.5 | `ROOM4021.RDT` | 0x95A, 0xA00, 0xA96 |
| RE2   | `info/re2leon/PL0/RDT/ROOM21B0.RDT` | 0x2762 |
| RE2   | `info/re2leon/PL0/RDT/ROOMB1B0.RDT` | 0x28B8 |

Die Byte-Folge kommt in **keinem** anderen der 495 RE2-Raeume und in **keinem** anderen
RE1.5-Raum vor. Es ist genau das Fahrstuhl-Fahrskript — in RE1.5 dreimal je Kabine
(drei Etagen), in RE2 einmal je Szenario.

ROOM21B0 ist als Fahrstuhl belegt: die englische Nachricht id30 @0x304A lautet
`"An elevator"` (RDT-Offset 0x40 = engl. Message-Sektion, Glyphentabelle wie
`re15_port/tools/rdt_msgdump.py`).

### 2.5 Die zwei Se_on, die RE2 dort hat und RE1.5 nicht

`ROOM21B0.RDT`, main05 (Region 0x26F8..0x27AC):

```
0x026F8 Set       22 02 07 01      ; bank2 bit7 = 1  (Szene AN) -- RE1.5 ROOM1080 @0x72C
0x026FC Set       22 01 1b 01      ; bank1 bit27 = 1            -- RE1.5 ROOM1080 @0x730
0x02700 Cut_chg   29 0f
        ...       (Kabinen-Positionierung)
0x02756 Se_on     36 02 11 01 01 00 00 00 00 00 00 00   <<< NUR RE2
0x02762 Set       22 01 1c 01      ; bit28 AN         -- RE1.5 ROOM1080 @0x746
0x02766 Sleep     09 0a 08 00      ; 8
0x0276A Set       22 01 1c 00
0x0276E Sleep     09 0a 5a 00      ; 90 = DIE FAHRT
        ...       (8A/8B = Sprach-Opcodes)
0x02784 Se_on     36 02 12 01 01 00 00 00 00 00 00 00   <<< NUR RE2
0x02790 Set       22 01 1c 01      ; bit28 AN         -- RE1.5 ROOM1080 @0x756
0x02794 Sleep     09 0a 08 00
0x02798 Set       22 01 1c 00
0x0279C Sleep     09 0a 14 00      ; 20
0x027A0 Set       22 02 07 00
0x027A4 Set       22 01 1b 00
0x027A8 Evt_end   01 00
```

Se_on-Operanden (Layout wie `re15_port/engine/src/scd_vm.c:1870-1908`, LAB_80041624):
`bank=2` (= snd0 der RDT), `id=0x11` bzw. `0x12`, `flags=1` (Positional-Pfad),
Ursprungs-Modus `1`, Offsets (0,0,0) — der Ton haengt also **am Spieler in der Kabine**.

ROOMB1B0 (Szenario-B-Spiegel) traegt dieselben zwei Se_on-Rohbytes an derselben
relativen Stelle (Kontext-Dump um 0x28B8 selbst gelesen:
`… 36 02 11 01 01 00 00 00 00 00 00 00 | 22 01 1c 01 09 0a 08 00 22 01 1c 00 09 0a 5a 00 |
8a 00 04 00 … 36 02 12 01 01 00 00 00 00 00 00 00 | 22 01 1c 01 09 0a 08 00 22 01 1c 00
09 0a 14 00 | 22 02 02 00 22 02 07 00 22 01 1b 00 01 00`).

### 2.6 Wo die zwei Wellenformen liegen — Datei-Offsets, byte-identisch in beiden Raeumen

RE2-RDT-Kopf (Offset-Tabelle 0x08..0x60; EDT/VH/VB wie RE1.5 an 0x08/0x0C/0x10):

```
ROOM21B0.RDT  EDT=0x5BC4  VH=0x5C84  VB=0x6AA4   (nprog=2 ntone=18 nvag=18)
ROOMB1B0.RDT  EDT=0x4308  VH=0x43C8  VB=0x4FE8   (nprog=1 ntone=9  nvag=7)
```

EDT-Records (4 B je se_id, Zerlegung byte-true wie `re15_edt_decode`,
`re15_port/engine/src/vab_common.c:252-265`):

```
ROOM21B0  EDT+0x11*4 = Datei 0x5C08 : 00 00 e3 00 -> prog=0 tone=14 prio=3 voice=-16 extra=0
ROOM21B0  EDT+0x12*4 = Datei 0x5C0C : 00 00 f3 00 -> prog=0 tone=15 prio=3 voice=-16 extra=0
ROOMB1B0  EDT+0x11*4 = Datei 0x434C : 00 00 e3 00 (identisch)
ROOMB1B0  EDT+0x12*4 = Datei 0x4350 : 00 00 f3 00 (identisch)
```

Tone-Attribute (32 B je Tone, Tone-Tabelle @VH+32+128*16):

```
ROOM21B0 prog0 tone14: vol=90  pan=64 center=103 shift=57 min=max=73 vag=16 adsr=80ff/1fc0
ROOM21B0 prog0 tone15: vol=110 pan=64 center=101 shift=0  min=max=74 vag=17 adsr=80ff/1fc0
```

Wellenform-Bloecke (VAG-Groessentabelle @VH+32+128*16+nprog*512, Einheit 8 B):

| SE | VAG | Groesse | Datei-Offset ROOM21B0 | Datei-Offset ROOMB1B0 | SHA-1 (beide) |
|---|---|---|---|---|---|
| 0x11 "Fahrt"   | vag16 / vag6 | **16400 B** | **0x1BA34..0x1FA44** | 0x7D68..0xBD78 | `ac0b9a13a8806802575e60f37e9691ef7b1ef846` |
| 0x12 "Ankunft" | vag17 / vag7 | **6336 B**  | **0x1FA44..0x21304** | 0xBD78..0xD638 | `5a9aca0d211de60a78871e7bff63371333ab4a3c` |

Die beiden Bloecke sind in ROOM21B0.RDT und ROOMB1B0.RDT **bitgleich** (SHA-1 oben,
selbst gerechnet) — zwei unabhaengige Container tragen dieselbe Welle. Das ist die
Gegenprobe, dass es sich nicht um zufaellig grosse Raum-Samples handelt.

### 2.7 Dauer-Gegenprobe: die Wellen passen genau auf das Fahrfenster

PSX-ADPCM: 16 B -> 28 Samples. Pitch aus `re15_vab_note2pitch2`
(`vab_common.c`, LUT DAT_80077520), Aufruf wie im Port `audio_pc.c:720`
= `note2pitch2(tone.min_note, tone.shift, tone.center, tone.shift)`:

* SE 0x11: 16400 B = 1025 Bloecke = **28700 Samples**; note=73 fine=57 center=103 shift=57
  -> s=(57+57)>>3=14, sem=73+60-103=30, oct=30/12-5=-3, LUT[6][14]=0x17CC=6092, >>3=761
  -> 44100*761/4096 = **8193 Hz** -> **3,50 s**.
  Fahrfenster im Skript: 8 + 90 = 98 Frames = 3,27 s @30 Hz. Passt.
* SE 0x12: 6336 B = 396 Bloecke = **11088 Samples**; note=74 fine=0 center=101 shift=0
  -> sem=74+60-101=33, oct=-3, LUT[9][0]=0x1AE8=6888, >>3=861
  -> 44100*861/4096 = **9269 Hz** -> **1,20 s**.
  Rest-Fenster: 8 + 20 = 28 Frames = 0,93 s. Passt.

### 2.8 Gegenprobe, dass RE1.5 die Welle nicht selbst fuehrt

Die snd0-Bank von ROOM1080.RDT hat nur 3 Wellenformen (48 / 1552 / 3840 B, §2.2) — die
groesste ist 3840 B. Eine Welle in der Groessenordnung 16400 B gibt es dort nicht; die
gemeinsame snd1-Bank (15 VAGs, groesste 15392 B) hat ebenfalls keine. Und ROOM1080.RDT
hat **null** Se_on (§2.1) — selbst wenn die Welle in der Bank laege, wuerde sie nie
angetippt.

### 2.9 Harte Gegenprobe: die zwei Wellen kommen im RE1.5-Auslieferungsstand NICHT vor

Aus jeder der beiden RE2-Wellen eine 512-Byte-Nadel geschnitten
(ROOM21B0.RDT @0x1BA34+0x1000 -> `00 00 bd c2 f1 ed 0c ff …`,
@0x1FA44+0x800 -> `12 00 0f 2d 0c e2 4d 11 …`) und **jede Datei** unter
`re15_port/shared_assets/PSX/` danach durchsucht (= der vollstaendige RE1.5-CD-Baum,
alle 6 Stages + BIN/DATA/DOOR/EMD/ITEM/MOVIE/PLD/SOUND/VOICE):

```
durchsucht 3193 Dateien, Treffer = 0
Positivkontrolle: dieselben zwei Nadeln in ROOMB1B0.RDT @0x8D68 / @0xC578 gefunden
```

RE1.5 fuehrt diese Wellenformen also nirgends — sie muessen tatsaechlich aus RE2 kommen.

## 3. Was der Port tut — mit datei.c:zeile

* Die ROOM1080-Fahrt laeuft im Port ueber die generische SCD-VM
  (`re15_port/engine/src/scd_vm.c`). Da das Skript kein 0x36 enthaelt, entsteht **kein**
  Audio-Ereignis; `op_se_on` (`scd_vm.c:1882-1908`) ist der einzige Erzeuger von
  `SCD_AUDIO_SE_ON`.
* Die Wiedergabekette ist vollstaendig vorhanden:
  `scd_vm.c:1885` -> `enqueue_audio` -> `audio_pc.c:3151` `case SCD_AUDIO_SE_ON:` ->
  `re15_audio_se_bank_kind(evt.bank)` -> `bank 2 = SND0` ->
  `re15_audio_room_se_snd0(se_id)` (`audio_pc.c:827-831`) -> `se_play_layers`
  (`audio_pc.c:684`) -> EDT-Record -> Tone -> VAG -> Mixer-Stimme.
  PSX-Gegenstueck: `re15_port/platform/psx/src/audio_psx.c:805`.
* Ein **RE2-Asset-Weg ist bereits gebaut** und produktiv:
  - Datei-Zugriff: `re15_pc_read_re2(rel, &size)` —
    `re15_port/platform/pc/src/asset_root_pc.c:297`, deklariert `asset_root_pc.h:62`:
    env `RE15_RE2_ASSET_ROOT` hat Vorrang, sonst `<shared>/RE2/<rel>`.
  - Beispiel-Bank: `read_re2_enemse_vbs` (`audio_pc.c:1061-1065`) laedt
    `shared_assets/RE2/ENEMSE.VBS`; Bank-TOC generiert
    (`re15_port/tools/gen_re2_ems_toc.py` -> `engine/src/gen/re2_ems_toc.inc`,
    genutzt ueber `engine/src/re2_ems.c` / `include/re2_ems.h`).
  - Dekodiert wird mit **demselben** VAB-Code wie RE1.5: `re15_vab_parse`
    (`vab_common.c:52`), `re15_edt_decode` (`vab_common.c:252`),
    `re15_edt_resolve_layers_ex` (`re15_vab.h:103`), `re15_vab_note2pitch2`.
  - Weitere RE2-Assets liegen schon in `re15_port/shared_assets/RE2/` (224 Dateien, u.a.
    `ENEMSE.VBS`, `EM23.EMD`, `CDEMD0.EMS`, `FILES/*.TIM`); geschnitten mit
    `re15_port/tools/re2_ems_cut.py` / `re2_files_cut.py`.

## 4. Der Unterschied, in einem Satz

RE1.5 fuehrt in ROOM1080/ROOM4020 exakt dasselbe Fahrstuhl-Fahrskript aus wie RE2 in
ROOM21B0/ROOMB1B0 (16 identische Bytes: bit28-Puls 8 Frames, Fahrt 90 Frames), aber ohne
die zwei `Se_on(bank=2, id=0x11)` / `Se_on(bank=2, id=0x12)`, die RE2 unmittelbar vor die
beiden bit28-Pulse setzt — und deren Wellenformen (16400 B / 6336 B) liegen in der
RE1.5-Bank gar nicht erst vor.

## 5. Umsetzungsplan (konkrete Dateien/Zeilen, jede Konstante mit @0x)

**Schritt 1 — Extraktion (Zwischenstand nach `build/extracted/`, NICHT nach shared_assets).**
Neues Werkzeug `re15_port/tools/re2_elevator_cut.py` nach dem Muster von
`re15_port/tools/re2_ems_cut.py`. Es liest `info/re2leon/PL0/RDT/ROOM21B0.RDT` (nur lesend)
und schneidet:
* EDT-Records 0x11 / 0x12 @Datei 0x5C08 / 0x5C0C (je 4 B, `00 00 e3 00` / `00 00 f3 00`),
* Tone-Attribute prog0 tone14 / tone15 @VH+32+2048+tone*32 (Tone-Basis 0x64A4)
  = Datei **0x6664** / **0x6684** (je 32 B; roh nachgelesen:
  `00 00 5a 40 67 39 49 49 … ff 80 c0 1f 00 00 10 00 …` = vol 0x5a/90, pan 0x40, center 0x67/103,
  shift 0x39/57, min=max 0x49/73, adsr 80ff/1fc0, vag 0x10/16, und
  `00 00 6e 40 65 00 4a 4a … 11 00` = vol 110, center 101, shift 0, min=max 74, vag 17),
* VAG 16 @Datei **0x1BA34**, 16400 B (SHA-1 `ac0b9a13…`),
* VAG 17 @Datei **0x1FA44**, 6336 B (SHA-1 `5a9aca0d…`).
Selbsttest: dieselben Bloecke aus ROOMB1B0.RDT (@0x7D68 / @0xBD78) muessen bitgleich sein.

**Schritt 2 — Auslieferungsformat.** Die zwei Wellen + ihre EDT-/Tone-Saetze als EINE
Mini-VAB-Bank `re15_port/shared_assets/RE2/ELEVSE.VBS` schreiben, im Satzformat, das
`load_re2_enemy_se_pc` schon liest: `[EDT-Records @0 .. vh_off)[VH "pBAV" @vh_off][Trailer]`
mit `vh_off = u32 @edt[edt_size-8]` (Format-Beleg: `audio_pc.c:1000-1007`, FUN_8005a09c).
Dann braucht der Port **keinen** neuen Parser.

**Schritt 3 — Laden.** In `re15_port/platform/pc/src/audio_pc.c` neben
`read_re2_enemse_vbs` (Zeile 1061) ein `read_re2_elev_vbs()` mit
`re15_pc_read_re2("ELEVSE.VBS", &sz)`; Dekodierung 1:1 wie `load_re2_enemy_se_pc`
(`audio_pc.c:1090`ff.), aber als eigener Ein-Bank-Slot (2 Samples, keine LRU noetig).
PSX-Seite analog in `platform/psx/src/audio_psx.c`.

**Schritt 4 — Ausloeser (die einzige echte Design-Entscheidung).**
Der Ton ist eine bewusste RE2-Ergaenzung; RE1.5 hat ihn nicht (§2.1/§2.8). Er gehoert
deshalb genau dorthin, wo RE2 ihn hat, und nirgends sonst: unmittelbar **vor** das
`Set bank1 bit28 = 1`. Anker-Offsets (roh nachgezaehlt, §2.4):

| RDT | erster Puls (SE 0x11 davor) | zweiter Puls (SE 0x12 davor) |
|---|---|---|
| ROOM1080.RDT / ROOM1081.RDT | 0x746 / 0x7D8 / 0x86A | 0x756 / 0x7E8 / 0x87A |
| ROOM4020.RDT / ROOM4021.RDT | 0x95A / 0xA00 / 0xA96 | 0x96A / 0xA10 / 0xAA6 |

(zweiter Puls = erster + 0x10, weil dazwischen genau
`09 0a 08 00 / 22 01 1c 00 / 09 0a 5a 00` liegt.)

Umsetzung ohne Aenderung der RDT-Bytes, bevorzugte Variante: ein generierter
Anker-Tabellen-Include `engine/src/gen/re15_elev_se.inc` (Generator sucht die 16-Byte-
Signatur in jeder RDT und schreibt Raum-Id + Offset + welche SE-Id), und im `Set`-Handler
(Opcode 0x22) in `scd_vm.c` ein Vergleich `t->pc - rdt_base` gegen diese Tabelle. Damit
steht keine geratene Raum-Id im Code, sondern der gemessene Datei-Offset.
Einfachere Variante (falls der rdt_base im VM-Thread nicht verfuegbar ist):
`bank==1 && bit==0x1c && val==1` mit Raum-Gate {0x1080,0x1081,0x4020,0x4021} und einem
Flankenzaehler, der beim `Set bank2 bit7 = 1` am Szenenanfang (ROOM1080 @0x72C)
zurueckgesetzt wird.

**Schritt 5 — Verifikation.** `RE15_SE_DEBUG=1` setzen und die Fahrt in ROOM1080 fahren:
erwartet sind genau zwei Zeilen `[se] Se_on bank=2 id=17` und `id=18` im Abstand von
98 Frames, und die Mixer-Zeile muss `pitch` 761 (8193 Hz) bzw. 861 (9269 Hz) melden —
die Zahlen aus §2.7. Andere Zahlen = Tone-Satz falsch uebernommen.

## 6. Offen / nicht belegt

1. ~~Byte-Vergleich der RE2-Wellen gegen den RE1.5-Baum~~ — **nachgeholt, siehe §2.9.**
   Kein offener Punkt mehr.
2. **Nicht belegt:** ob RE2 in ROOM21B0 zusaetzlich einen Dauer-Ambient fuer die Kabine
   fuehrt. Gewalkt habe ich nur die SCD-Sektionen `init` (RDT 0x48) und `main` (0x4C);
   die dritte Sektion @0x50 liess sich mit der RE1.5-Zeigertabellen-Heuristik nicht in
   Regionen zerlegen (204735 "Regionen" = Fehlparse) und wurde ausgelassen.
   Naechster Weg: das RE2-Format der 0x50-Sektion aus der RE2-EXE bestimmen.
3. **Teilweise verifiziert:** die RE2-SCD-Opcode-Laengentabelle. Sie ist NICHT aus der
   RE2-EXE gelesen, sondern empirisch geprueft: mit ihr walken 2334 von 2568
   init+main-Regionen aller 495 RE2-RDTs sauber durch (91 %); ROOM21B0 main05 und
   ROOMB1B0 sind fehlerfrei. Versucht und gescheitert: die Dispatch-Tabelle der RE2-SCD-VM
   zu finden — 0x8009D868 ist die Entity-State-Tabelle (ihre Handler schreiben Zustands-Ids
   wie 47/48/56 nach thread+1, keine Opcode-Laengen), 0x800A74C8 wird @0x800535a0 per
   1024-B-memcpy kopiert, ist also Daten. Naechster Weg: die VM ueber den Leser der
   RDT-Sektion 0x4C suchen statt ueber Sprungtabellen-Muster.
   **Fuer das Ergebnis dieses Dossiers ist die Tabelle unkritisch** — die Fahrstuhl-Stelle
   wurde zusaetzlich ueber die 16-Byte-ROHSUCHE gefunden, die ohne Opcode-Tabelle auskommt.
4. **Nicht entschieden:** ob der Ton auch in den drei RE1.5-ROOM4020-Fahrten
   (A-2 ELEVATOR, STAGE4) spielen soll. Die Skript-Signatur ist dort identisch
   (0x95A/0xA00/0xA96), das Original ist dort ebenfalls stumm.
5. **Regel (d) eingehalten:** `info/re2leon/` wurde in diesem Lauf nur gelesen; es wurde
   keine Datei extrahiert oder geschrieben. Ebenso wurde nichts unter `re15_port/engine/`,
   `platform/`, `include/` geaendert und nicht gebaut.
