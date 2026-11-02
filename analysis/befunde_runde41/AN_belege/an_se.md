# Spur AN, Nachbesserung 1: die vier RE2-Toene der Annette-Szene (ANNETTE_SE.VBS)

Runde 41, 2026-11-02. Schnittstelle (vom Aufrufer): `include/re15_annette_r41.h` `RE15_AN_SE_KRACH=0, _STURZ=1, _BRUELL=2,
_ZUG_AN=3`, `re15_annette_r41_se_hook(void (*)(int))`. Diese Spur liefert die Bank, den Spieler und die Belege, keine Szene.

⛔ RE2-Ergaenzung, kein RE1.5-Original (Beta -> Retail, Sound ist RE2). Vorbild ist RE2 Claire `PL1/RDT/ROOM6141.RDT`
MAIN sub02 (Zeitachse und Aufnahme: `re2_claire/zeitachse.md`).

## 0. Ergebnis

| Satz | Quelle (Datei, Offset, Bytes) | RE2-Satz -> Ton -> Welle | Laenge | Gegenprobe Aufnahme (Lauf 1) |
|---|---|---|---|---|
| 0 KRACH | ROOM6141 sub02 @0x01040 `36 03 0b 00 00 00 00 00 00 00 00 00` Se_on(3,0x0B); Bank 3 = **ENEMSE-Bank 64** | `info/re2clair/COMMON/SOUND/ENEMSE.VBS` EDT @0x5A382C `00 00 d2 04` -> Prog 0 Ton 13 @0x5A4240 -> VAG 6 @0x5AEC20, 13104 B, sha1 e6b624dd2e64 | 22904 Abtastwerte @16397,5 Hz (pitch 0x5F3) = **1,397 s** | Einsatz **49,431 s**, r = **0,988** (zweitbeste Stelle 0,098) |
| 1 STURZ | ROOM6141 sub02 @0x010CE `36 02 0a 00 ..` Se_on(2,0x0A) | ROOM6141 Raumbank EDT @0xA858 `00 01 23 20` -> Prog 1 Ton 2 @0xB350 **und** Ton 3 @0xB370 (Byte 3 >> 5 = 1 Zusatzlage) -> beide VAG 10 @0x11B60, 9568 B, sha1 0348b34ff963 | 16716 @11025 Hz (0x400) = **1,516 s** | Einsatz **58,698 s**, r = **0,882** (zweitbeste 0,120) |
| 2 BRUELL | ROOM6141 sub02 @0x01104 `36 02 0c 00 ..` Se_on(2,0x0C) | EDT @0xA860 `00 00 c3 01` -> Prog 0 Ton 12 @0xB290 -> VAG 11 @0x140C0, 9072 B, sha1 dba3cffcb83c | 15848 @4866,5 Hz (0x1C4) = **3,257 s** | Einsatz **60,829 s**, r = **0,998** (zweitbeste 0,116) |
| 3 ZUG_AN | RE2 Leon ROOM7020 sub04 @0x014AA `36 02 0c 00 ..` Se_on(2,0x0C), Hochfahren nach Set(4,0x88)=1 @0x01466 | `info/re2leon/PL0/RDT/ROOM7020.RDT` EDT @0x5CC0 `00 00 93 00` -> Prog 0 Ton 9 @0x6690 -> VAG 15 @0x167F0, 12736 B, sha1 0bab6e45719e | 22260 @6137,0 Hz (0x23A) = **3,627 s** | keine Aufnahme; Claire ROOM7021 @0x165B4 bitgleich; steigender Ton 1066 -> 1528 Hz (Abschnitt 4) |

Alle vier Wellen sind Einmal-Toene (ADPCM-Flags: Endblock `1`, danach der stille Block `7`); der Port-Decoder hoert am
Endblock auf (Probe `lagen`: dekodiert = Endblock-Laenge).

## 1. KRACH: welche Bank ist "Bank 3" in ROOM6141?

ROOM6141-Kopf +0x08..+0x18 = `0xA830 0xA8F0 0xB710 0 0`: es gibt keine zweite RDT-Bank. Bank 3 wird zur Laufzeit belegt.

### 1.1 Statisch (Claire PAL `SLES_109.72`, mit Capstone disassembliert; Instruktionsworte wie disassembliert)

- Se_on-Handler = SCD-Tabelle 0x8009f4f8[0x36] (@0x8009f5d0 `8005278c`):
  @0x800527ac `92070001` lbu a3,1(s0) (Bank), @0x800527b0 `86040002` lh a0,2(s0) (Satz), @0x800527a8 `86050004` lh a1,4(s0)
  (Bezug: Bytes 4..11 = 0 -> Lage (0,0,0)), @0x8005287c-90 a0 = Bank<<24 | Satz<<16, @0x80052894 `0c015cd3` jal 0x8005734c.
- SE-Verteiler 0x8005734c: @0x80057354 `00044e02` srl t1,a0,0x18 (Bank); @0x80057388 `8022c730` lb v0,-0x38d0(at) =
  Stimmen-Bank-Handle 0x800cc730[Bank] (-1 = still); @0x800573b0 `8c253658` lw a1,0x3658(at) = **EDT-Zeiger 0x800d3658[Bank]**,
  @0x800573b8 + Satz*4; @0x800573ec `8c26f088` lw a2,-0xf78(at) = **VH-Zeiger 0x800cf088[Bank]**; Ton = VH + Prog*0x200 +
  Ton*0x20 + 0x820 (@0x8005740c-1c); Zusatzlagen s4 = Byte3>>5 (@0x80057404 `0003a142`), Schleife @0x80057590-654.
  Ton-Bytes +6/+5 werden als Note/Feinstimmung in den Stimmen-Satz geschrieben (@0x80057510/@0x8005751c) - wie RE1.5.
- ENEMSE-Lader @0x80055b90: @0x80055bd0 `92b5bddf` lbu s5,-0x4221(s5) = Bank-Nummer **0x800cbddf**;
  @0x80055bf8 `36318610` ori s1,s1,0x8610 / @0x80055c00 `ac313664` sw s1,0x3664(at) -> **EDT-Zeiger[3] = 0x801f8610**;
  @0x80055c2c `8c22fb28` lw v0,-0x4d8(at) = **TOC 0x8009fb28** + Bank*0x10 (Leon: 0x800a7b1c). Datei-Id 0x143 (@0x80055c18).
- Bank-Nummer = Zeile der Raum-Paar-Tabelle **0x8009f464** (@0x8004f1d4/d8 `3c06800a`/`24c6f464`), Suche @0x8004f1b8
  (Leon FUN_80052b38 @0x80052b38, maskierter Code-Vergleich `exe_map.py find 0x80052b38 24` -> 1 Treffer), RAUM_A/B
  @0x800d07b0/1, Ergebnis `sb` @0x8004f298 nach 0x800cbddf. Regel und Fuellung von RAUM_A/B: `tools/gen_re2_ems_toc.py`
  TABELLE 3. RAUM_A = Ton-Byte +7 des ersten Spawns mit Wert != 0: Birkin @0x00C82 `44 00 01 33 02 60 00 31` -> **0x31**
  (Annette @0x00C6C `44 00 00 49 00 08 00 00` traegt 0x00).
- Paar-Tabelle Claire hat 72 Zeilen (Leon 73), Zeile **64 = `31 28`** (einzige Zeile mit 0x31, in beiden EXEs).
  TOC[64] @0x8009ff28 = `a8 0e 00 00 47 0b 00 00 20 1e 01 00 49 0b 00 58`: EDT 0xEA8 B ab Sektor 2887 = ENEMSE.VBS
  **@0x5A3800**, VBD 0x11E20 B ab Sektor 2889 = **@0x5A4800**. Trailer @0x5A46A0 = 0x80 -> VH @0x5A3880.
  Leon-EXE (`info/re2leon/PSX.EXE`): Zeile 64 @Datei 0x97C80 = `31 28`, TOC[64] @Datei 0x9871C = dieselben 16 Bytes.

### 1.2 Dynamisch (DuckStation-Savestates der echten Claire-Disc, `capture/lauf2`, Zeiten = Videozeit Lauf 1)

`werkzeug/an_se_gegenprobe.py ram`:

| Stand | RAUM_A/B | Bank 0x800cbddf | EDT*[3] / VH*[3] | RAM-EDT[0x0B] | RAM EDT+VH-Kopf == ENEMSE.VBS @0x5A3800 |
|---|---|---|---|---|---|
| s00 (Startraum, vor der Tuer) | `02 01` | 32 | 0x801f8610 / 0x801f8690 | `00 00 c9 05` | nein (andere Bank) |
| s10 (11,20 s, ROOM6141 geladen) | `31 00` | **64** | 0x801f8610 / 0x801f8690 | `00 00 d2 04` | **ja** |
| s49 (49,33 s, kurz vor dem Krach) | `31 00` | **64** | dieselben | `00 00 d2 04` | **ja** |
| s50 (50,25 s) | `31 00` | **64** | dieselben | `00 00 d2 04` | **ja** |

Stimmen-Handle 0x800cc730..35 = `00 01 02 03 04 05`. Der Slot 0x801f8610 wechselt mit dem Raum (32 -> 64): es ist der
ENEMSE-Slot. Ueber die ganze EDT+VH (0xEA0 B, s49) weichen im RAM genau 536 Bytes ab, alle in den Bytes 8..15 der
Programm-Zeilen (VH +0x20 + 16*p + 8..15, Reservefelder, von libsnd zur Laufzeit beschrieben); SE-Map, VH-Kopf und
Ton-Bytes sind gleich.

### 1.3 Ergebnis KRACH

Bank 3 in ROOM6141 = ENEMSE-Bank 64 (`ENEMY64.SAP - Birkin Train Fight ...` der Nutzerliste, gleiche Nummer). Satz 0x0B =
`00 00 d2 04` -> Prog 0 Ton 13 -> VAG 6. Kein Ersatz, kein naechstbelegter Ton noetig. Leon-ENEMSE.VBS traegt die Welle
@0x5AEC20 bitgleich (Werkzeug-Gegenprobe).

## 2. STURZ und BRUELL (Raumbank ROOM6141, Bank 2)

Bank 2 = RDT-Kopf +0x08/+0x0C/+0x10 (EDT 0xA830, VH 0xA8F0, VB 0xB710); RAM-Raumbank im Savestate @0x80124e6c traegt
[0x0A] = `00 01 23 20`, [0x0C] = `00 00 c3 01` (= Datei). STURZ ist **zweilagig**: Byte 3 = 0x20 -> eine Zusatzlage, Ton 2
und Ton 3 haben dieselben 32 Bytes (`00 00 7f 40 55 00 3d 3d .. ff 80 c0 1f 01 00 0a 00 ..`) und dieselbe Welle VAG 10,
d. h. RE2 spielt die Welle doppelt. Die Mini-Bank behaelt beide Lagen (Ton 1 und 2 auf Welle 2).
Gegenprobe: Leon `PL0/RDT/ROOM6140.RDT` traegt VAG 10 @0x0910C und VAG 11 @0x0B66C bitgleich.

## 3. Gegenprobe gegen die echte Aufnahme

`werkzeug/an_se_gegenprobe.py kreuz`: Wellen **aus der installierten** `shared_assets/RE2/ANNETTE_SE.VBS`, mit der
Abspielrate aus den Ton-Bytes (note2pitch2), linear auf 22050 Hz gebracht, normierte Kreuzkorrelation mit der Tonspur von
`capture/lauf1/re2c_lauf1_voll.mp4` (80,898 s; Verfahren wie `xa_einsatz.py`, Energieboden 5 %).

| Satz | global beste Stelle | zweitbeste (> 1 s weg) | Fenster | Nullmodell: andere Wellen derselben RE2-Bank (bestes / Median) | eigene Welle -1 / +1 Halbton |
|---|---|---|---|---|---|
| KRACH | 49,431 s r = 0,988 | 19,561 s r = 0,098 | 49,40 +- 3 s: 49,431 s | 0,114 (VAG 9) / 0,067 | 0,090 / 0,091 |
| STURZ | 58,698 s r = 0,882 | 63,076 s r = 0,120 | 58,70 +- 3 s: 58,698 s | 0,345 (VAG 6) / 0,133 | 0,104 / 0,099 |
| BRUELL | 60,829 s r = 0,998 | 62,584 s r = 0,116 | 60,85 +- 3 s: 60,829 s | 0,291 (VAG 3) / 0,167 | 0,207 / 0,207 |

Damit sind Welle UND Tonhoehe belegt (ein Halbton daneben faellt r auf 0,09-0,21). Zeitlich zur Zeitachse: KRACH 0,66 s
vor dem Bild von Schnitt 7 (50,09 s), BRUELL 0,34 s vor dem Bild von Schnitt 0x0A (61,17 s) - Se_on direkt nach Cut_chg
klingt sofort, das neue Bild kommt erst nach dem Hintergrund-Laden (`zeitachse.md` Abschnitt 2). STURZ 58,698 s = Se_on
@0x010CE direkt nach Member_set 0x17=1 (Birkin RAM e06 58,81 s schon y -5250).

## 4. ZUG_AN: welcher Satz von ROOM702.SAP

Nutzerliste `RE2_SOUND_LISTE.md` Zeile 223 "ROOM702.SAP - Train Powering Up" = Raumbank von RE2 ROOM7020 (Leon) / ROOM7021
(Claire). Der Raum ist der Generatorraum (msg 0 "A high yield generator that supplies power for the inside facilities and
the train. The high capacity plugs have been disconnected ..."). Der Zug wird hier mit Strom versorgt:

- MAIN sub01 @0x01300..@0x01338: (4,0x88)=0, Ck auf Gegenstand 0x54/0x55, beide Stecker (Keep_item_ck 0x54/0x55) ->
  Evt_exec sub04, nur einer -> sub05 (Satz 0x12 Knistern + msg 4 "Two plugs must be set simultaneously ...").
- sub04: @0x01458 Cut_chg 4, @0x0145A Se_on(2,0x0A) (Stecker), **@0x01466 `22 04 88 01` Set(4,0x88)=1**,
  @0x01476/@0x01478 Sce_item_lost 0x54/0x55, @0x014A6 Sleep 10, **@0x014AA `36 02 0c 00 ..` Se_on(2,0x0C)**, Sleep 20,
  @0x014BA Cut_chg 3, Espr3d 0x15/0x16, @0x014EC Se_on(2,0x13) + msg 1 "The emergency mode has been activated. The power
  supply will be cut temporarily. The emergency train will be activated upon restoration of the power.", Cut_replace
  @0x01524-2D (Notlicht-Schnitte), @0x01530 Cut_chg 0x0F, @0x01532 Se_on(2,0x0B).
- (4,0x88) heisst "Strom fuer den Zug": sub03 @0x01448 `2b 00 03 00 ff ff` msg 3 "The power already has been supplied to
  the train!" bei (4,0x88)=1; ROOM7030 (der Zug, "There's an activation lever for the train motor") sub12 @0x026B6
  `21 04 88 00` Ck(4,0x88)=0 -> @0x026BA msg 0 "... The motor cannot be activated due to the insufficient power supply."

Wahl **Satz 0x0C @0x014AA**, weil (`an_se_gegenprobe.py steigen`, staerkste Spitze 100..4000 Hz je 0,15 s):

| Satz (Stelle) | Laenge | Spektralspitze ueber die Zeit |
|---|---|---|
| 0x0A @0x0145A (Stecker, vor dem Flag) | 0,66 s | 3128, 3432, 1592, 224 Hz - kurzer Schlag |
| **0x0C @0x014AA (10 Bilder nach Set(4,0x88))** | **3,63 s** | 1678, 1492, 1702, **1066, 1078, 1174, 1258, 1264, 1312, 1462** (0,45-1,35 s), dann 113-341 Hz Grollen |
| 0x13 @0x014EC (msg 1) | 1,45 s | 1576-1592 Hz konstant = Piepton |
| 0x0B @0x01532 (nach Cut_replace) | 0,94 s | 2792-3664 Hz Rauschstoss |

0x0C ist der erste Ton nach dem Strom-Flag und der einzige mit steigendem Ton (Hochlaufen, 1066 -> 1528 Hz zwischen
0,45 und 1,50 s, Nachlauf als tiefes Grollen). Gegenprobe: Claire ROOM7021 (EDT 0x5A54, VH 0x5B14, VB 0x6934) traegt
Satz 0x0C `00 00 93 00`, Ton-Bytes `00 00 64 40 67 3b 44 44 ..` und VAG 15 @0x165B4 bitgleich.

**Offen (Entscheidung beim Aufrufer):** Das wirkliche Anfahren des RE2-Zuges liegt nicht in ROOM7020, sondern in ROOM7030
sub12 (Hebel des Zugmotors, msg 2 "Will you move the lever?"): @0x0275C `36 02 0d 01 00 00 d1 fa 28 f1 55 a0` Se_on(2,0x0D)
(Satz `00 00 e3 00` -> VAG 12, 1,98 s @16398 Hz) mit Ruettel-Schleife, @0x027F8 Se_on(2,0x0E) (Satz `00 00 f3 01` -> VAG 13,
2,35 s @22050 Hz), danach Movie_on 0x0D @0x0284D. Das ist ROOM703.SAP ("Another Train Area"), nicht die vom Nutzer
genannte Bank; beide Toene sind Hebel-/Schlaggeraeusche ohne Hochlaufen. Gebaut ist ROOM7020 0x0C wie beauftragt; der
Kommentar in `re15_annette_r41.h` ("ROOM702.SAP ... Satz im Werkzeug belegt") stimmt damit.

## 5. Mini-Bank `shared_assets/RE2/ANNETTE_SE.VBS`

Werkzeug `re15_port/tools/r41_annette/an_se_cut.py` (`--install`; `--claire-exe <SLES_109.72>` prueft zusaetzlich 14
Claire-Anker; Quellen unter `info/`, `info/re2clair` ist unversioniert und wird im Haupt-Checkout gesucht, `RE15_INFO`
geht vor). Selbsttest: 16 Skript-Anker, 2 Leon-EXE-Anker, alle EDT-/Ton-/VAG-Offsets, sha1, Gegenprobe-Wellen bitgleich,
Tonhoehen. Zweimal gebaut -> gleiche sha1.

- Format wie ELZASE/LIFT6030: `[SE-Map 0x10 B][VH pBAV @0x10, 0xC20 B][Trailer u32 0x10, 0][VBD]`;
  47608 B, FNV-1a 0xDDFD2496, sha1 b094d13363df445788cfd20addde76f594be1f1e; edt_size 3128, VBD 44480 B.
- SE-Map: `00 00 02 04` / `00 00 13 20` / `00 00 33 01` / `00 00 43 00`. Geaendert gegen die Quelle NUR Byte 1 (Programm -> 0)
  und das obere Nibble von Byte 2 (Ton-Lage 0/1/3/4); Byte 0, Byte 3 (Stimme, Zusatzlagen) und Prioritaets-Nibble gleich.
- Toene 0..4 in Programm 0: je 32 Quell-Bytes, geaendert NUR +0x14 (Programm -> 0) und +0x16 (VAG-Index 1..4).
  vol/pan/center/shift/min/max/ADSR/Modus bytegleich. Wellen bytegleich (STURZ einmal, zwei Toene darauf).
- VH-Kopf und Programm-Zeile 0 aus ENEMSE-Bank 64 (Quelle von Satz 0, Prog 0); neu gesetzt: Groesse, nprog 1,
  Tonsegment 16, nvag 4, Programm-Tonzahl 5 (wie `toene39_se_cut.py`).
- `engine/src/gen/re2_annette_se_bank.inc`: Groessen, FNV, Satznummern (= RE15_AN_SE_*), Tonhoehen
  0x5F3/0x400/0x1C4/0x23A, Lagen 1/2/1/1, Abtastwerte bis zum Endblock, Quell-Ton-/EDT-Bytes (nur mit
  `RE2_ANSE_MIT_QUELLEN`, fuer die Probe).

Spieler `re15_port/platform/pc/src/annette_se_pc.c` `re15_pc_annette_se(int satz)`: Laden wie `lift2030_se_pc.c`
(re15_pc_read_re2, Groessenpruefung, Trailer-VH, re15_vab_parse, re15_vag_adpcm_decode), Wiedergabe
`re15_audio_se_play_bank` -> `se_play_layers` (Lagen, Tonhoehe note2pitch2). Nicht positional (RE2: Bezug 0, Lage 0,0,0).
Fehlt/abweichende Datei: eine Zeile `[annettese] ...` auf stderr, danach still. `_Static_assert`: .inc-Nummern ==
RE15_AN_SE_*. Registrierung `platform/pc/main.c` direkt nach `re15_pc_lift2030_se_init()`:
`{ extern void re15_pc_annette_se(int); re15_annette_r41_se_hook(re15_pc_annette_se); }`.

Paket-Gate `release/make_package.sh`: ANNETTE_SE.VBS in der Runde-41-Liste neben ANNETTE_4D*.EMD (vorhanden, nicht leer,
bytegleich zum Quellbaum).

## 6. Tests

`tests/unit/probe_r41_annette_se.c`, registriert in `tests/unit/probes/r41_annette_se.cmake`:

| Test | prueft | Ergebnis (build_se, 2026-11-02) |
|---|---|---|
| unit_r41_annette_se_bank | Groesse/FNV, Trailer 0x10, pBAV, VH parsebar, 4 Wellen lueckenlos = VBD, Satznummern = RE15_AN_SE_* | gruen |
| unit_r41_annette_se_toene | EDT Byte 0/3/Prio = Quelle, Programm 0, Lage 0/1/3/4; Ton-Bytes jeder Lage = Quelle ausser +0x14..+0x17; pitch 0x5F3/0x400/0x400/0x1C4/0x23A | gruen |
| unit_r41_annette_se_lagen | Lagen 1/2/1/1 je auf der eigenen Welle, Dekodierung = Endblock 22904/16716/15848/22260, nicht stumm | gruen |

Gesamtbau `RE15_BUILD_DIR=re15_port/build_se bash re15_port/tools/local_build.sh build`: ohne die main.c-Zeile
`LOCAL-BUILD-OK`; mit ihr uebersetzt main.c, der Link von `re15_pc.exe` meldet genau 1 fehlendes Symbol
`re15_annette_r41_se_hook` (Definition beim Aufrufer, `engine/src/annette_r41.c`).

## 7. Dateien

Neu: `re15_port/tools/r41_annette/an_se_cut.py`, `re15_port/shared_assets/RE2/ANNETTE_SE.VBS`,
`re15_port/engine/src/gen/re2_annette_se_bank.inc`, `re15_port/platform/pc/src/annette_se_pc.c`,
`re15_port/tests/unit/probe_r41_annette_se.c`, `re15_port/tests/unit/probes/r41_annette_se.cmake`,
`analysis/befunde_runde41/AN_belege/werkzeug/an_se_gegenprobe.py`, dieses Dossier.
Geaendert: `re15_port/platform/pc/main.c` (eine Zeile), `release/make_package.sh` (Gate + Kommentar).

## 8. Offene Punkte

- `re15_annette_r41_se_hook` wird in `engine/src/annette_r41.c` vom Aufrufer definiert; bis dahin linkt `re15_pc` nicht.
- ZUG_AN-Alternative ROOM7030 sub12 (Abschnitt 4) - Entscheidung beim Aufrufer/Nutzer.
- Lautstaerke/Richtung: RE2 rechnet Se_on positional (Bezug 0 = Lage (0,0,0)); der Port spielt mit Ton-vol/pan wie die
  anderen Mini-Banken. Keine Klangabweichung, nur Pegel/Richtung.
- Die Gegenprobe-Rohdaten (Video, Savestates, Claire-EXE) liegen im Scratchpad der Sitzung
  `.../scratchpad/an/capture/` bzw. `.../scratchpad/an/cdisc/`, nicht im Repo.
