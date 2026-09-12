# Entlade-Handler Waffe 19 (H&K MC51) @0x80034A30 und Waffe 14 (Flammenwerfer) @0x800C45A8

Gruppe: entlade-mc51-flamme / 2026-09-12 / Quellen: `ghidra1_V2.txt` (Region liegt dort nur als
roh-Byte -> Weg C `re15_disasm.py`), `info/Re1.5/PSX.EXE`, `info/Re1.5/PSX/BIN/DEBUG.BIN`,
Savestates `stage_saves/{boot_48,doorA_square,equip_test}.sav`, `RE_15_Quellcode_V2/*.c`.

## Kernbefund vorab

1. **0x80034A30 (MC51)**: Bereich **0x80034A30-0x80034C70** (jr ra @0x80034C6C, Delay @0x80034C70;
   naechste Funktion = FSM-Sub 3 @0x80034C74, beginnt mit `lbu [0x800ACA5B]`). Grenze geprueft.
2. **0x800C45A8 (Flammenwerfer) liegt NICHT in der EXE** (t_addr=0x80010000, t_size=0xAF000 ->
   EXE-Ende 0x800BF000, aus dem PS-X-EXE-Header @0x18). Er liegt in **DEBUG.BIN**, das beim Start
   resident nach **0x800C0000** geladen wird:
   - Loader: `FUN_8001311c` ruft `FUN_80013b60(7, &DAT_800c0000, 0)` (Datei-Id 7 = DEBUG.BIN;
     `RE_15_Quellcode_V2/FUN_8001311c.c`), Aufrufer `FUN_800116f4` (jal @0x80011704).
   - RAM-Beweis: Savestates boot_48/doorA_square/equip_test: RAM `0x800C4000..0x800C5000` ==
     `DEBUG.BIN[0x4000:0x5000]` **4096/4096 Bytes identisch**. Datei-Offset des Handlers = 0x45A8.
   - DEBUG.BIN traegt neben dem Debug-Menue auch residenten Engine-Code: Masken-Trailer-Decoder
     `FUN_800c47e8` (jal aus dem BSS-Lader @0x80021DB8), Font-Breiten @0x800C4416,
     Message-Tabelle @0x800C4FC6.
3. **Entlade-Tabelle @0x80074100 ist zur Laufzeit UNGEPATCHT**: Live-RAM (doorA_square.sav)
   `[12]=0x800347F8 [14]=0x800C45A8 [19]=0x80034A30 [20]=0x80034060 [15..18]=0` - identisch zur EXE.
4. **Aufrufstellen der Entlade-Tabelle** (jalr `[0x80074100 + waffe*4]`):
   - @0x800334E0 (im Standard-Feuer-Pfad `FUN_80032e9c`; davor `sb -> 0x800ACAE8` @0x800334C8),
   - @0x800345EC (im Feuer-Substate `FUN_80034510` der Dauerfeuer-FSM 0x80034014).
   D.h. der Entlade-Handler laeuft im Dauerfeuer JEDEN Feuer-Frame.
5. **Der Flammenwerfer-Handler ist eine handgepatchte VARIANTE des Ingram-Handlers 0x800347F8**
   (gleiches Skelett Instruktion fuer Instruktion, 6 gezielte Aenderungen; die toten Reste der
   Originalrechnungen stehen noch drin - Details unten). Er ruft sehr wohl
   `FUN_80011f50` (Schadens-Resolver) - die Erwartung "kein Hitscan-Pfad" trifft auf den
   ENTLADE-Handler nicht zu; was der Resolver fuer Waffe 14 daraus macht, liegt in FUN_80011F50
   (andere Gruppe).

## Beteiligte Globals/Helfer (Belege)

| Adresse | Bedeutung | Beleg |
|---|---|---|
| 0x800ACAE9 | In-Clip-Bildzaehler (Takt der Effekte + Munitions-Gate) | lbu @0x80034A5C u.a. |
| 0x800ACAF2 | Feuertakt-Zaehler, wird HIER inkrementiert | sb @0x80034BFC bzw. @0x800C477C |
| 0x800ACABE | Aim-/Gun-Winkel s16 (a1 an Effekt-Spawn) | `FUN_8001d600.c:39` (Quelle DAT_800ac9a8[3]), `FUN_8002d474.c:26` (sin/cos-Argument), `FUN_80035538.c:28-31` (per-Waffe-Schrittweite aus Tabelle @0x80074090) |
| 0x800ACA5C | Charakter-/Kostuem-Index; **Bit 2** waehlt alternative Muendungs-Offsets UND die zweite Schadenszeile (+0x58) der Tabelle @0x8006E5A0 | `FUN_80011f50.c:54`; PLD-Lade-Index `FUN_800314b0.c:15` (Tabelle @0x80073F70), Grafik-Index `FUN_80036b68.c:9` (Tabelle @0x800741E8) |
| 0x800ACA5D | angelegte Waffen-Id (a0 fuer Schadens-Resolver) | lbu @0x80034C1C |
| 0x800ACA5B | Sub-Phase-Byte der Feuer-FSM; ==2 ("Clip leer") laesst FSM-Sub 9 `FUN_80035538` das Substate-Wort 0x800ACA5A auf 1 setzen | `FUN_80035538.c:6-9`; Schreiber hier @0x80034C38 / @0x800C47B8; Ruecksetzer `FUN_8002d474.c:85,95` |
| 0x800ACBDC | Posebuffer-Basis; +0x7A4 (=+1956) = Gun-Bone-Matrix | lw @0x80034AA8 + addiu 1956 @0x80034AC0 |
| 0x800ACA88 | Welt-Position des Waffen-Effekts (x; Block bis +0x08) - SE-Pan-Quelle | `FUN_8002d474.c:27` (`local_38 + DAT_800aca88` nach sin/cos); a1 @0x80034C48 = 0x800ACAF2-0x6A |
| FUN_80019700 | Effekt-Spawn. a0 = (effekt_id<<24)\|(sub_byte<<16)\|scale16 - Slot +0x70=id, +0x71=sub_byte, Sheet-Wahl nutzt sub_byte&7, +0x72=scale16; a1 -> Slot +0x2E (Winkel/Param); a2=Matrix; a3=Offset-Triple (sp+0x10/14/18) | `RE_15_Quellcode_V2/FUN_80019700.c` Z.23-47 |
| FUN_8004EAE4 | Munition -1, ret 0 = leer | Kontext (verifiziert) |
| FUN_80011F50 | Schadens-Resolver(waffen_id, [entity+0x7C]) | Kontext (verifiziert) |
| FUN_80045024 | SE-Play: a0=(bank<<24)\|(sample<<16)\|(mode); mode&0xFF!=0 -> Distanz-Pan via FUN_80045a64(a1=Positionszeiger) | `RE_15_Quellcode_V2/FUN_80045024.c` (switch a0>>24; `(param_1&0xff)==0` statisch, sonst FUN_80045a64(param_2)); RE15_FUN_CATALOG.md Z.185 |

---

## Funktion 1: Entlade-Handler H&K MC51 (Waffe 19) - 0x80034A30..0x80034C70

Im Ghidra-Dump nur roh-Byte (ghidra1_V2.txt Z.130262ff) -> direkt disassembliert (re15_disasm.py).

### Roh-Listing (annotiert)

```
80034a30  addiu sp,sp,-64            ; Prolog, s0-s5+ra gesichert
80034a34  sw    s1,36(sp)
80034a38  lui   s1,0xaaaa            ; s1 = 0xAAAAAAAB = Magic fuer unsigned /3 bzw. /6
80034a3c  sw    s4,48(sp)
80034a40  lui   s4,0x800b
80034a44  addiu s4,s4,-13591         ; s4 = &0x800ACAE9  (In-Clip-Bildzaehler)
80034a48  sw    ra,56(sp)
80034a4c  sw    s5,52(sp)
80034a50  sw    s3,44(sp)
80034a54  sw    s2,40(sp)
80034a58  sw    s0,32(sp)
80034a5c  lbu   a0,0(s4)             ; a0 = clip_frames
80034a60  ori   s1,s1,0xaaab
80034a64  multu a0,s1                ; hi = a0*0xAAAAAAAB
80034a68  mfhi  v1
80034a6c  srl   v1,v1,1              ; v1 = a0/3
80034a70  sll   v0,v1,1
80034a74  addu  v0,v0,v1             ; v0 = (a0/3)*3
80034a78  subu  a0,a0,v0             ; a0 = a0 % 3
80034a7c  andi  a0,a0,0xff
80034a80  bne   a0,zero,0x80034b64   ; clip_frames % 3 != 0 -> Effekte ueberspringen
80034a84  lui   v0,0x9249            ; (Delay: obere Haelfte /7-Magic)
; ---- Muendungsfeuer: alle 3 In-Clip-Frames -------------------------------------
80034a88  lui   a0,0x201
80034a8c  ori   a0,a0,0x800          ; a0 = 0x02010800 = Effekt 2, Sub 1, Scale 0x800
80034a90  addiu a3,sp,16             ; a3 = Offset-Triple
80034a94  ori   s5,zero,0x41         ; x-Offset = 0x41 (65)
80034a98  ori   s3,zero,0x514        ; y-Offset = 0x514 (1300)
80034a9c  lui   a1,0x800b
80034aa0  lh    a1,-13634(a1)        ; a1 = s16[0x800ACABE] = Aim-Winkel
80034aa4  lui   v0,0x800b
80034aa8  lw    v0,-13348(v0)        ; v0 = [0x800ACBDC] Posebuffer
80034aac  lui   v1,0x800b
80034ab0  lbu   v1,-13732(v1)        ; v1 = [0x800ACA5C] Charakter-Index
80034ab4  ori   s2,zero,0x5a         ; z-Basis = 0x5A (90)
80034ab8  sw    s5,16(sp)            ; sp[16] = 65
80034abc  sw    s3,20(sp)            ; sp[20] = 1300
80034ac0  addiu s0,v0,1956           ; s0 = Posebuffer+0x7A4 = Gun-Bone-Matrix
80034ac4  addu  a2,s0,zero           ; a2 = Gun-Matrix
80034ac8  andi  v1,v1,0x4            ; Charakter-Bit 2?
80034acc  sltu  v1,zero,v1           ; v1 = (char&4)?1:0
80034ad0  sll   v0,v1,2
80034ad4  addu  v0,v0,v1             ; v0 = v1*5
80034ad8  sll   v0,v0,5              ; v0 = v1*160
80034adc  subu  v0,s2,v0             ; z = 90 - (char&4?160:0)  -> +90 bzw. -70
80034ae0  jal   0x80019700           ; Effekt-Spawn(0x02010800, winkel, gun_mtx, {65,1300,z})
80034ae4  sw    v0,24(sp)            ; (Delay) sp[24] = z
; ---- Rauch zusaetzlich: alle 6 In-Clip-Frames ----------------------------------
80034ae8  lbu   a0,0(s4)             ; a0 = clip_frames (neu)
80034af0  multu a0,s1
80034af4  mfhi  v1
80034af8  srl   v1,v1,2              ; v1 = a0/6
80034afc  sll   v0,v1,1
80034b00  addu  v0,v0,v1
80034b04  sll   v0,v0,1              ; v0 = (a0/6)*6
80034b08  subu  a0,a0,v0             ; a0 = a0 % 6
80034b0c  andi  a0,a0,0xff
80034b10  bne   a0,zero,0x80034b64   ; % 6 != 0 -> skip
80034b14  lui   v0,0x9249            ; (Delay)
80034b18  lui   a0,0x300
80034b1c  ori   a0,a0,0xb00          ; a0 = 0x03000B00 = Effekt 3, Sub 0, Scale 0xB00
80034b20  addu  a2,s0,zero           ; a2 = Gun-Matrix
80034b24  lui   a1,0x800b
80034b28  lh    a1,-13634(a1)        ; a1 = Aim-Winkel
80034b2c  lui   v1,0x800b
80034b30  lbu   v1,-13732(v1)        ; Charakter-Index
80034b34  addiu a3,sp,16
80034b38  sw    s5,16(sp)            ; x = 65
80034b3c  sw    s3,20(sp)            ; y = 1300
80034b40  andi  v1,v1,0x4
80034b44  sltu  v1,zero,v1
80034b48  sll   v0,v1,2
80034b4c  addu  v0,v0,v1
80034b50  sll   v0,v0,5
80034b54  subu  v0,s2,v0             ; z = 90/-70 (wie oben)
80034b58  jal   0x80019700           ; Effekt-Spawn(0x03000B00, ...)
80034b5c  sw    v0,24(sp)            ; (Delay) sp[24] = z
80034b60  lui   v0,0x9249
; ---- Huelsenauswurf: alle 7 Takte des ZWEITEN Zaehlers -------------------------
80034b64  lui   s0,0x800b
80034b68  addiu s0,s0,-13582         ; s0 = &0x800ACAF2 (Feuertakt-Zaehler)
80034b6c  lbu   v1,0(s0)
80034b70  ori   v0,v0,0x2493         ; v0 = 0x92492493 = signed /7-Magic
80034b74  sll   v1,v1,24
80034b78  sra   a0,v1,24             ; a0 = (s8)takt
80034b7c  mult  a0,v0
80034b80  sra   v1,v1,31
80034b84  mfhi  v0
80034b88  addu  v0,v0,a0
80034b8c  sra   v0,v0,2
80034b90  subu  v0,v0,v1             ; v0 = a0/7 (signed)
80034b94  sll   v1,v0,3
80034b98  subu  v1,v1,v0             ; v1 = v0*7
80034b9c  subu  a0,a0,v1             ; a0 = a0 % 7
80034ba0  sll   a0,a0,24
80034ba4  bne   a0,zero,0x80034be4   ; % 7 != 0 -> skip
80034ba8  lui   a0,0x400             ; (Delay)
80034bac  ori   a0,a0,0x800          ; a0 = 0x04000800 = Effekt 4, Sub 0, Scale 0x800
80034bb0  addiu a3,sp,16
80034bb4  lui   a1,0x800b
80034bb8  lh    a1,-13634(a1)        ; a1 = Aim-Winkel
80034bbc  lui   a2,0x800b
80034bc0  lw    a2,-13348(a2)        ; Posebuffer
80034bc4  ori   v0,zero,0x91
80034bc8  sw    v0,16(sp)            ; x = 0x91 (145)
80034bcc  ori   v0,zero,0x212
80034bd0  sw    v0,20(sp)            ; y = 0x212 (530)
80034bd4  ori   v0,zero,0x19
80034bd8  sw    v0,24(sp)            ; z = 0x19 (25)   (charakter-UNabhaengig)
80034bdc  jal   0x80019700           ; Effekt-Spawn(0x04000800 = Huelse)
80034be0  addiu a2,a2,1956           ; (Delay) a2 = Gun-Matrix
; ---- Takt++, Munition/Schaden im 4-an/4-aus-Fenster ----------------------------
80034be4  lbu   v0,0(s0)             ; v0 = takt
80034be8  lui   v1,0x800b
80034bec  lbu   v1,-13591(v1)        ; v1 = clip_frames [0x800ACAE9]
80034bf0  addiu v0,v0,1
80034bf4  andi  v1,v1,0x4
80034bf8  bne   v1,zero,0x80034c4c   ; (clip_frames & 4) != 0 -> KEIN Schuss-Resolve
80034bfc  sb    v0,0(s0)             ; (Delay, laeuft immer) [0x800ACAF2]++
80034c00  jal   0x8004eae4           ; Munition -1
80034c04  nop
80034c08  beq   v0,zero,0x80034c34   ; leer? -> Trockenklick
80034c0c  ori   v0,zero,0x2          ; (Delay) v0 = 2
80034c10  lui   v0,0x800b
80034c14  lw    v0,-14460(v0)        ; v0 = [0x800AC784] Spieler-Entity
80034c18  lui   a0,0x800b
80034c1c  lbu   a0,-13731(a0)        ; a0 = [0x800ACA5D] Waffen-Id (=19)
80034c20  lw    a1,124(v0)           ; a1 = entity[0x7C]
80034c24  jal   0x80011f50           ; Schadens-Resolver
80034c28  nop
80034c2c  j     0x80034c4c
80034c30  nop
80034c34  lui   at,0x800b            ; ---- Clip leer:
80034c38  sb    v0,-13733(at)        ; [0x800ACA5B] = 2 (Sub-Phase "leer" -> FSM-Sub 9 setzt Substate=1)
80034c3c  lui   a0,0x101
80034c40  ori   a0,a0,0x1            ; a0 = 0x01010001 = SE Bank1/Sample1, positions-gepannt
80034c44  jal   0x80045024           ; SE-Play (Trockenklick)
80034c48  addiu a1,s0,-106           ; (Delay) a1 = 0x800ACAF2-0x6A = 0x800ACA88 (FX-Weltposition)
80034c4c  lw    ra,56(sp)            ; Epilog (s0-s5, ra)
80034c50  lw    s5,52(sp)
80034c54  lw    s4,48(sp)
80034c58  lw    s3,44(sp)
80034c5c  lw    s2,40(sp)
80034c60  lw    s1,36(sp)
80034c64  lw    s0,32(sp)
80034c68  addiu sp,sp,64
80034c6c  jr    ra
80034c70  nop
```

### Pseudo-C

```c
void entlade_mc51(void)   /* @0x80034A30, Tabelle @0x80074100[19] */
{
    u8   cf  = DAT_800acae9;                 /* In-Clip-Bildzaehler */
    void *gun = *(u8**)&DAT_800acbdc + 0x7a4; /* Gun-Bone-Matrix */
    int  alt = (DAT_800aca5c & 4) != 0;      /* Charakter-Bit 2 */
    s32  ofs[3];

    if (cf % 3 == 0) {                       /* @0x80034a80 */
        ofs[0]=0x41; ofs[1]=0x514;           /* @0x80034a94/98 */
        ofs[2]=0x5a - (alt?160:0);           /* @0x80034ab4..adc: +90 / -70 */
        FUN_80019700(0x02010800, DAT_800acabe, gun, ofs);    /* Muendungsfeuer @0x80034ae0 */
        if (cf % 6 == 0) {                   /* @0x80034b10 */
            ofs[2]=0x5a - (alt?160:0);
            FUN_80019700(0x03000b00, DAT_800acabe, gun, ofs); /* Rauch @0x80034b58 */
        }
    }
    if ((s8)DAT_800acaf2 % 7 == 0) {         /* @0x80034ba4 */
        ofs[0]=0x91; ofs[1]=0x212; ofs[2]=0x19;   /* @0x80034bc4/bcc/bd4 */
        FUN_80019700(0x04000800, DAT_800acabe, gun, ofs);    /* Huelse @0x80034bdc */
    }
    DAT_800acaf2++;                          /* @0x80034bfc (immer) */
    if ((DAT_800acae9 & 4) == 0) {           /* @0x80034bf4: 4 Frames an / 4 aus */
        if (FUN_8004eae4()) {                /* Munition -1 @0x80034c00 */
            FUN_80011f50(DAT_800aca5d, *(u32*)(g_entity+0x7c));  /* @0x80034c24 */
        } else {
            DAT_800aca5b = 2;                /* @0x80034c38: FSM-Sub-Phase "leer" */
            FUN_80045024(0x01010001, &DAT_800aca88);  /* Trockenklick @0x80034c44 */
        }
    }
}
```

### Konstanten (MC51)

| Wert | Bedeutung | @0x |
|---|---|---|
| %3 | Muendungsfeuer-Takt auf 0x800ACAE9 (Magic 0xAAAAAAAB, srl 1) | 0x80034A38/60, 0x80034A6C, 0x80034A80 |
| 0x02010800 | Effekt Muendungsfeuer (Id 2, Sub 1, Scale 0x800) | 0x80034A88/8C |
| 65 / 1300 | Muendungs-Offset x/y | 0x80034A94 / 0x80034A98 |
| +90 / -70 | Muendungs-Offset z = 0x5A - ((char&4)?160:0) | 0x80034AB4, 0x80034AD0-ADC |
| %6 | Rauch-Takt (gleiches Magic, srl 2) | 0x80034AF8, 0x80034B10 |
| 0x03000B00 | Effekt Rauch (Id 3, Sub 0, Scale 0xB00) | 0x80034B18/1C |
| %7 | Huelsen-Takt auf 0x800ACAF2 (signed, Magic 0x92492493) | 0x80034A84+0x80034B70, 0x80034BA4 |
| 0x04000800 | Effekt Huelse (Id 4, Sub 0, Scale 0x800) | 0x80034BA8/AC |
| 145/530/25 | Huelsen-Offset x/y/z (charakter-unabhaengig) | 0x80034BC4/BCC/BD4 |
| &4 | Munition/Schaden nur wenn (0x800ACAE9 & 4)==0 (8er-Kadenz 4an/4aus) | 0x80034BEC-BF8 |
| 2 | 0x800ACA5B=2 bei leerem Clip | 0x80034C0C + 0x80034C38 |
| 0x01010001 | Trockenklick-SE (Bank 1, Sample 1, Pan an Position 0x800ACA88) | 0x80034C3C/40, Pos @0x80034C48 |

---

## Funktion 2: Entlade-Handler Flammenwerfer (Waffe 14) - 0x800C45A8..0x800C47E4 (DEBUG.BIN @0x45A8)

**Herkunft:** DEBUG.BIN, resident @0x800C0000 (Beweise oben). Der Handler ist das
Instruktions-Skelett des **Ingram-Handlers FUN_800347F8** (EXE) mit genau diesen Patches -
die toten Reste der Originalrechnungen stehen noch im Code:

| # | Ingram-Original (EXE) | Flammenwerfer-Variante (DEBUG.BIN) |
|---|---|---|
| 1 | Effekt 0x02010800 @0x80034844/48 | **0x031D1200** (Id 3, Sub-Byte 0x1D -> Sheet 0x1D&7=5, Scale 0x1200) @0x800C45F8/45FC |
| 2 | a1 = Aim-Winkel [0x800ACABE] @0x8003485C | Winkel wird geladen (@0x800C4610), dann **ueberschrieben: a1 = 0x0BB8 (3000)** @0x800C466C |
| 3 | Offsets x=0x46-D, y=0x41A-D', z=-60-D' (D=(char&4)?10:0, D'=(char&4)?30:0) @0x80034884-B4 | **konstant x=0x96 (150) @0x800C464C, y=0x4B0 (1200) @0x800C465C, z=0 (`sw zero`) @0x800C4674**; die D-Rechnungen @0x800C4624-4648/4654-4658/4664-4668 laufen noch, ihre Ergebnisse sind TOT |
| 4 | Rauch %6: jal 0x80019700 @0x800348FC | Argumente werden aufgebaut (0x03000B00 @0x800C46A8/46AC, Winkel @0x800C46B4, Matrix, a3) - **der jal FEHLT** (zwischen 0x800C46BC und 0x800C46C4 steht nur das /7-Magic-lui) -> **kein zweiter Effekt** |
| 5 | Huelse %7: jal 0x80019700 @0x800349A4 | Argumente werden aufgebaut (0x04000800 @0x800C4708/470C, y=0x64 @0x800C4724, x=0xC8-((char&4)?40:0) @0x800C474C/4750, z=-20-((char&4)?40:0) @0x800C4758/475C) - **der jal FEHLT** -> **kein Huelsenauswurf** |
| 6 | Trockenklick-SE 0x01010001 @0x80034A04/08 | **0x01000001** (Bank 1, Sample 0) @0x800C47BC/47C0 |

Identisch geblieben (gleiche Semantik wie MC51/Ingram): %3-Takt auf 0x800ACAE9 (@0x800C45F0),
%6-/%7-Checks (@0x800C46A0/@0x800C4704, nur noch als tote Argument-Setups), 0x800ACAF2++
(@0x800C477C), Munitions-/Schadens-Gate `(0x800ACAE9 & 4)==0` (@0x800C476C-4778),
FUN_8004EAE4 (@0x800C4780), FUN_80011F50(waffen_id=14, entity[0x7C]) (@0x800C47A4),
bei leer: 0x800ACA5B=2 (@0x800C478C+47B8) + SE (@0x800C47C4, a1=&0x800ACA88 @0x800C47C8).

### Roh-Listing (annotiert)

```
800c45a8  addiu sp,sp,-48            ; Prolog (wie Ingram 0x800347F8)
800c45ac  sw    s1,36(sp)
800c45b0  lui   s1,0xaaaa
800c45b4  sw    s2,40(sp)
800c45b8  lui   s2,0x800b
800c45bc  addiu s2,s2,-13591         ; s2 = &0x800ACAE9
800c45c0  sw    ra,44(sp)
800c45c4  sw    s0,32(sp)
800c45c8  lbu   a0,0(s2)             ; a0 = clip_frames
800c45cc  lui   s1,0xaaab            ; (Encoding-Variante, ergibt ebenfalls
800c45d0  addiu s1,s1,-21845         ;  s1 = 0xAAAAAAAB)
800c45d4  multu a0,s1
800c45d8  mfhi  v1
800c45dc  srl   v1,v1,1              ; /3
800c45e0  sll   v0,v1,1
800c45e4  addu  v0,v0,v1
800c45e8  subu  a0,a0,v0             ; a0 = clip_frames % 3
800c45ec  andi  a0,a0,0xff
800c45f0  bne   a0,zero,0x800c46c4   ; %3 != 0 -> skip
800c45f4  lui   v0,0x9249            ; (Delay)
; ---- FLAMMENSTRAHL: alle 3 In-Clip-Frames --------------------------------------
800c45f8  lui   a0,0x31d
800c45fc  addiu a0,a0,4608           ; a0 = 0x031D1200  [PATCH 1: Effekt-Id 3, Sub 0x1D, Scale 0x1200]
800c4600  addiu a3,sp,16
800c4604  lui   v0,0x800b
800c4608  lw    v0,-13348(v0)        ; Posebuffer [0x800ACBDC]
800c460c  lui   a1,0x800b
800c4610  lh    a1,-13634(a1)        ; a1 = Aim-Winkel (WIRD GLEICH UEBERSCHRIEBEN)
800c4614  lui   t0,0x800b
800c4618  lbu   t0,-13732(t0)        ; Charakter [0x800ACA5C]
800c461c  addiu s0,v0,1956           ; s0 = Gun-Bone-Matrix (+0x7A4)
800c4620  addu  a2,s0,zero
800c4624  andi  t0,t0,0x4            ; \
800c4628  sltu  t0,zero,t0           ;  | Ingram-D-Rechnung (x=0x46-D):
800c462c  sll   v1,t0,2              ;  | Ergebnis TOT (siehe 0x800c464c)
800c4630  addu  v1,v1,t0             ;  |
800c4634  sll   v1,v1,1              ; /
800c4638  ori   v0,zero,0x46         ; TOT (Ingram-x)
800c463c  subu  v0,v0,v1             ; TOT
800c4640  sll   v1,t0,4              ; \ Ingram-D' ((char&4)?30:0), Ergebnis TOT
800c4644  subu  v1,v1,t0             ;  |
800c4648  sll   v1,v1,1              ; /
800c464c  ori   v0,zero,0x96         ; [PATCH 3a] x = 0x96 (150), fest
800c4650  sw    v0,16(sp)
800c4654  ori   v0,zero,0x41a        ; TOT (Ingram-y)
800c4658  subu  v0,v0,v1             ; TOT
800c465c  ori   v0,zero,0x4b0        ; [PATCH 3b] y = 0x4B0 (1200), fest
800c4660  sw    v0,20(sp)
800c4664  addiu v0,zero,-60          ; TOT (Ingram-z)
800c4668  subu  v0,v0,v1             ; TOT
800c466c  ori   a1,zero,0xbb8        ; [PATCH 2] a1 = 3000 statt Aim-Winkel
800c4670  jal   0x80019700           ; Effekt-Spawn(0x031D1200, 3000, gun_mtx, {150,1200,0})
800c4674  sw    zero,24(sp)          ; [PATCH 3c] (Delay) z = 0
; ---- %6-Block: Argument-Setup OHNE Aufruf (Rauch entfernt) ---------------------
800c4678  lbu   a0,0(s2)
800c467c  nop
800c4680  multu a0,s1
800c4684  mfhi  v1
800c4688  srl   v1,v1,2              ; /6
800c468c  sll   v0,v1,1
800c4690  addu  v0,v0,v1
800c4694  sll   v0,v0,1
800c4698  subu  a0,a0,v0             ; % 6
800c469c  andi  a0,a0,0xff
800c46a0  bne   a0,zero,0x800c46c4
800c46a4  lui   v0,0x9249            ; (Delay)
800c46a8  lui   a0,0x300
800c46ac  addiu a0,a0,2816           ; a0 = 0x03000B00 (Rauch) - WIRD NIE GERUFEN
800c46b0  lui   a1,0x800b
800c46b4  lh    a1,-13634(a1)        ; a1 = Winkel - TOT
800c46b8  addu  a2,s0,zero           ; TOT
800c46bc  addiu a3,sp,16             ; TOT   [PATCH 4: jal 0x80019700 ENTFERNT]
800c46c0  lui   v0,0x9249
; ---- %7-Block: Argument-Setup OHNE Aufruf (Huelse entfernt) --------------------
800c46c4  lui   s0,0x800b
800c46c8  addiu s0,s0,-13582         ; s0 = &0x800ACAF2
800c46cc  lbu   v1,0(s0)
800c46d0  ori   v0,v0,0x2493         ; /7-Magic 0x92492493
800c46d4  sll   v1,v1,24             ; \
800c46d8  sra   a0,v1,24             ;  |
800c46dc  mult  a0,v0                ;  | (s8)takt % 7
800c46e0  sra   v1,v1,31             ;  | (identisch zu MC51
800c46e4  mfhi  v0                   ;  |  @0x80034b74-9c)
800c46e8  addu  v0,v0,a0             ;  |
800c46ec  sra   v0,v0,2              ;  |
800c46f0  subu  v0,v0,v1             ;  |
800c46f4  sll   v1,v0,3              ;  |
800c46f8  subu  v1,v1,v0             ;  |
800c46fc  subu  a0,a0,v1             ; /
800c4700  sll   a0,a0,24
800c4704  bne   a0,zero,0x800c4764   ; %7 != 0 -> skip
800c4708  lui   a0,0x400             ; (Delay)
800c470c  ori   a0,a0,0x800          ; a0 = 0x04000800 (Huelse) - WIRD NIE GERUFEN
800c4710  addiu a3,sp,16
800c4714  lui   a1,0x800b
800c4718  lh    a1,-13634(a1)        ; Winkel - TOT
800c471c  lui   a2,0x800b
800c4720  lw    a2,-13348(a2)
800c4724  ori   v0,zero,0x64
800c4728  sw    v0,20(sp)            ; y = 100 (ueberschreibt sp[20]; folgenlos, kein Konsument)
800c472c  lui   v0,0x800b
800c4730  lbu   v0,-13732(v0)        ; Charakter
800c4734  addiu a2,a2,1956
800c4738  andi  v0,v0,0x4
800c473c  sltu  v0,zero,v0
800c4740  sll   v1,v0,2
800c4744  addu  v1,v1,v0
800c4748  sll   v1,v1,3              ; D = (char&4)?40:0
800c474c  ori   v0,zero,0xc8
800c4750  subu  v0,v0,v1
800c4754  sw    v0,16(sp)            ; x = 200-D
800c4758  addiu v0,zero,-20
800c475c  subu  v0,v0,v1
800c4760  sw    v0,24(sp)            ; z = -20-D   [PATCH 5: jal 0x80019700 ENTFERNT]
; ---- Takt++, Munition/Schaden (identisch zu MC51) ------------------------------
800c4764  lbu   v0,0(s0)
800c4768  lui   v1,0x800b
800c476c  lbu   v1,-13591(v1)        ; clip_frames
800c4770  addiu v0,v0,1
800c4774  andi  v1,v1,0x4
800c4778  bne   v1,zero,0x800c47cc   ; (clip_frames & 4)!=0 -> skip
800c477c  sb    v0,0(s0)             ; (Delay) [0x800ACAF2]++
800c4780  jal   0x8004eae4           ; Munition -1
800c4784  nop
800c4788  beq   v0,zero,0x800c47b4   ; leer?
800c478c  ori   v0,zero,0x2          ; (Delay)
800c4790  lui   v0,0x800b
800c4794  lw    v0,-14460(v0)        ; Spieler-Entity [0x800AC784]
800c4798  lui   a0,0x800b
800c479c  lbu   a0,-13731(a0)        ; Waffen-Id [0x800ACA5D] (=14)
800c47a0  lw    a1,124(v0)           ; entity[0x7C]
800c47a4  jal   0x80011f50           ; Schadens-Resolver - der Flammenwerfer NUTZT den Standard-Pfad
800c47a8  nop
800c47ac  j     0x800c47cc
800c47b0  nop
800c47b4  lui   at,0x800b
800c47b8  sb    v0,-13733(at)        ; [0x800ACA5B] = 2
800c47bc  lui   a0,0x100
800c47c0  addiu a0,a0,1              ; a0 = 0x01000001  [PATCH 6: SE Bank1/Sample0]
800c47c4  jal   0x80045024           ; Trockenklick-SE
800c47c8  addiu a1,s0,-106           ; (Delay) a1 = 0x800ACA88 (FX-Position)
800c47cc  lw    ra,44(sp)            ; Epilog
800c47d0  lw    s2,40(sp)
800c47d4  lw    s1,36(sp)
800c47d8  lw    s0,32(sp)
800c47dc  addiu sp,sp,48
800c47e0  jr    ra
800c47e4  nop                        ; Ende; @0x800c47e8 beginnt der Masken-Trailer-Decoder
```

### Pseudo-C (effektives Verhalten, tote Pfade weggelassen)

```c
void entlade_flamme(void)   /* @0x800C45A8 (DEBUG.BIN@0x45A8), Tabelle @0x80074100[14] */
{
    if (DAT_800acae9 % 3 == 0) {            /* @0x800c45f0 */
        s32 ofs[3] = { 0x96, 0x4b0, 0 };    /* @0x800c464c/465c/4674 - charakter-UNabhaengig */
        FUN_80019700(0x031d1200, /* param statt Winkel: */ 3000,
                     *(u8**)&DAT_800acbdc + 0x7a4, ofs);  /* Flammenstrahl @0x800c4670; a1 @0x800c466c */
    }
    /* %6-Rauch und %7-Huelse: nur noch tote Argument-Setups, jals entfernt */
    DAT_800acaf2++;                         /* @0x800c477c */
    if ((DAT_800acae9 & 4) == 0) {          /* @0x800c4774 */
        if (FUN_8004eae4())                 /* @0x800c4780 */
            FUN_80011f50(DAT_800aca5d /*=14*/, *(u32*)(g_entity+0x7c));  /* @0x800c47a4 */
        else {
            DAT_800aca5b = 2;               /* @0x800c47b8 */
            FUN_80045024(0x01000001, &DAT_800aca88);  /* @0x800c47c4 */
        }
    }
}
```

### Konstanten (Flammenwerfer)

| Wert | Bedeutung | @0x |
|---|---|---|
| %3 | Flammenstrahl-Takt auf 0x800ACAE9 | 0x800C45CC-45F0 |
| 0x031D1200 | Flammen-Effekt: Id 3, Sub-Byte 0x1D (Sheet 0x1D&7=5), Scale 0x1200 | 0x800C45F8/45FC |
| 3000 (0xBB8) | a1/Param an den Effekt (ERSETZT den Aim-Winkel; landet in Slot+0x2E) | 0x800C466C |
| 150/1200/0 | Muendungs-Offset x/y/z, fest (Charakter-D tot) | 0x800C464C / 0x800C465C / 0x800C4674 |
| - | %6-Rauch 0x03000B00: Setup vorhanden, **jal entfernt** | 0x800C46A8-46BC |
| - | %7-Huelse 0x04000800: Setup vorhanden, **jal entfernt** | 0x800C4708-4760 |
| &4 | Munition/Schaden nur wenn (0x800ACAE9 & 4)==0 | 0x800C476C-4778 |
| 2 | 0x800ACA5B=2 bei leerem Tank | 0x800C478C + 0x800C47B8 |
| 0x01000001 | Trockenklick-SE Bank 1 Sample 0 (MC51/Ingram: Sample 1!) | 0x800C47BC/47C0 |

## Offen / nicht Teil dieser Gruppe

- Was FUN_80011F50 fuer Waffe 14 konkret aufloest (Reichweite/Kegel/Durchschlag) - Resolver-Gruppe.
  Fest steht HIER nur: der Entlade-Handler ruft ihn im selben (acae9&4)-Takt wie MC51/Ingram.
- Wie die ESP-Row-VM den Param 3000 (Slot+0x2E) fuer Effekt-Id 3 / Sub-Byte 0x1D interpretiert
  (Winkel? Reichweite? Seed?) - dafuer muesste die Row-Definition des Effekts 3 (CORE00.ESP bzw.
  geladene Waffen-Bank) durchgezogen werden; die oberen Bits des Sub-Bytes (0x1D & ~7 = 0x18,
  gespeichert in Slot+0x71) haben einen hier nicht verfolgten Konsumenten.
- Warum die Variante in DEBUG.BIN liegt (Build-Geschichte) - Fakt ist nur: die Tabelle zeigt ab
  Werk dorthin und DEBUG.BIN ist ab Boot resident (Loader FUN_8001311c).

RE2-Fallback wurde NICHT benoetigt - alle Mechanismen sind aus RE1.5-Bytes (EXE + DEBUG.BIN)
plus Savestate-RAM belegt. (In RE2_Quellcode_V2 existiert keine flame-benannte Funktion;
RE2 lagert Waffen-Effektcode in die PLW aus - fuer diese Gruppe ohne Belang.)
