# Waffen-FSM (Dauerfeuer): Entry 0x80034014 + Sub 0 (RAISE) 0x80034060 + FUN_800369f8

Quelle: ghidra1_V2.txt (RE1.5 PSX.EXE, t_addr=0x80010000, verifiziert aus PSX.EXE[0x18]).
Tabellen byte-gedumpt aus info/Re1.5/PSX.EXE (Dateioffset = 0x800 + (va - 0x80010000)).
RE2-Belege aus RE2_Quellcode_V2/ (klar gekennzeichnet).

## 0. Aufruf-Kontext (Caller-Kette, verifiziert)

Player-Top-Level-State "Aim/Fire" = LAB_80032e44 (Eintrag der Player-State-Tabelle, XREF 0x8007400c(*)):

```
80032e44 addiu sp,sp,-0x18
80032e48 lui   a1,0x800b
80032e4c addiu a1,a1,-0x35ac          ; a1 = &DAT_800aca54  (= Spieler-Entity-Basis, +0x00 Flags)
80032e50 lui   a0,0x4000
80032e58 lw    v0,0x0(a1)             ; Spieler-Flags
80032e60 lbu   v1,DAT_800aca5d        ; angelegte Waffen-Id
80032e64 or    v0,v0,a0               ; Flags |= 0x40000000   (Aim-Bit, jeden Frame)
80032e6c sw    v0,0x0(a1)
80032e70-7c   at = 0x80074030 + id*4  ; Waffen-Dispatch
80032e84 jalr  v0                     ; -> 0x80034014 fuer Id 12/14/19
```

**Waffen-Dispatch @0x80074030** (byte-gedumpt, 21 Eintraege):
[0..2]=0x80034E70 (Nahkampf), [12]/[14]/[19]=**0x80034014** (Dauerfeuer: Ingram M10 / Flammenwerfer / MC51),
[20]=**0x00000000 (NULL)**, Rest=0x80032E9C (Standard). D.h. Waffen-Id 20 dispatcht auf NULL —
im Auslieferungsstand ist Id 20 keine feuerbare Waffe (jalr 0 wuerde crashen; wird nie erreicht).

## 1. Entry 0x80034014 — Dauerfeuer-FSM-Rahmen (0x80034014-0x8003405C)

```
80034014 lui   v0,0x800b
80034018 lbu   v0,DAT_800aca5a        ; FSM-Substate (0..9)
8003401c addiu sp,sp,-0x18
80034020 sw    ra,0x10(sp)
80034024 sll   v0,v0,0x2
80034028 lui   at,0x8007
8003402c addiu at,at,0x4150           ; Sub-Tabelle @0x80074150
80034030 addu  at,at,v0
80034034 lw    v0,0x0(at)             ; PTR_LAB_80074150[substate]
8003403c jalr  v0                     ; Sub ausfuehren
80034044 clear a0                     ; a0 = 0
80034048 jal   FUN_800369f8           ; FUN_800369f8(0, 1)
8003404c _ori  a1,zero,0x1            ; a1 = 1 (Delay-Slot)
80034050 lw    ra,0x10(sp)
80034058 jr    ra
```

Pseudo-C:
```c
void weapon_full_auto_frame(void)   /* 0x80034014 */
{
    (*fsm_subs_80074150[DAT_800aca5a])();   /* lbu @0x80034018, Tabelle @0x8003402c */
    FUN_800369f8(0, 1);                     /* a0=0 @0x80034044, a1=1 @0x8003404c   */
}
```

**Sub-Tabelle @0x80074150** (byte-gedumpt, 10 Eintraege):
[0]=0x80034060 [1]=0x80034278 [2]=0x80034510 [3]=0x80034C74 [4]=0x80034D68
[5]=0x80034EE8 [6]=0x800350C4 [7]=0x80035314 [8]=0x80035424 [9]=0x80035538

## 2. Sub 0 = LAB_80034060 (0x80034060-0x80034274) — RAISE (Waffe anlegen)

Innerer Phasenzaehler: DAT_800aca5b (Spieler +0x07). Switch @0x80034070-a8:
Phase 0 -> Init @0x800340ac (faellt danach in die Tick-Logik durch!),
Phase 1 -> Tick @0x80034114, Phase 2 -> Abschluss @0x80034260, sonst -> Exit.

### Roh-Listing mit Kommentar

```
80034060 addiu sp,sp,-0x18
80034064 lui   a1,0x800b
80034068 addiu a1,a1,-0x35a5          ; a1 = &DAT_800aca5b (Sub-Phase)
8003406c sw    ra,0x10(sp)
80034070 lbu   v1,0x0(a1)             ; Phase
80034074 ori   v0,zero,0x1
80034078 beq   v1,v0,LAB_80034114     ; Phase 1 -> Tick
8003407c _slti v0,v1,0x2
80034080 beq   v0,zero,LAB_80034098   ; Phase >=2
80034088 beq   v1,zero,LAB_800340ac   ; Phase 0 -> Init
8003408c _ori  v0,zero,0x1            ; (Delay: v0=1, wird in Init als Phasenwert benutzt)
80034090 j     LAB_80034268           ; (unerreichbar fuer lbu-Werte)
80034098 ori   v0,zero,0x2
8003409c beq   v1,v0,LAB_80034260     ; Phase 2 -> Abschluss
800340a0 _ori  v0,zero,0x1            ; (Delay: v0=1 = neuer FSM-Substate)
800340a4 j     LAB_80034268           ; Phase >2 -> Exit

; ---------- Phase 0: Init ----------
800340ac sb    v0,0x0(a1)             ; DAT_800aca5b := 1
800340b0 ori   v0,zero,0x6
800340b8 sb    v0,DAT_800acae8        ; Clip := 6  (RAISE-Clip der Waffenbank)
800340c0 lhu   v0,DAT_800acaec        ; Aim-Wort
800340c4 ori   v1,zero,0x7
800340cc sb    zero,DAT_800acae9      ; In-Clip-Bildzaehler := 0
800340d4 sb    v1,DAT_800acae3        ; Crossfade-Frames := 7 (Spieler +0x8f)
800340dc sh    zero,DAT_800acae0      ; Speed (+0x8c) := 0
800340e0 andi  v0,v0,0x1fff           ; Aim-Wort: Bits 15/14/13 loeschen
800340e4 ori   v0,v0,0x4000           ;   Bit 14 = MITTE setzen
800340ec sh    v0,DAT_800acaec
800340f0 jal   FUN_8003703c           ; Auto-Aim-Zielsuche
800340f4 _ori  a0,zero,0x7530         ;   Radius 0x7530 = 30000
800340f8 lui   v1,0x800b
800340fc lw    v1,DAT_800aca3c
80034104 sb    v0,DAT_800acaf3        ; Ziel-Klasse cachen (0/1/2)
80034108 ori   v1,v1,0xc0
80034110 sw    v1,DAT_800aca3c        ; DAT_800aca3c |= 0xC0 (Aim-aktiv-Marker;
                                      ;   geloescht @0x80035f14 beim Aim-Exit)
; ---- faellt OHNE Sprung in Phase 1 durch ----

; ---------- Phase 1: Tick ----------
80034114 lui   v0,0x800b
80034118 lbu   v0,DAT_800acaf3        ; gecachte Ziel-Klasse
80034120 andi  v0,v0,0x1              ; nur Klasse 1 (lebendes A/B-Ziel) ...
80034124 beq   v0,zero,LAB_8003413c
80034128 _ori  a1,zero,0xc0           ;   Drehrate 0xC0/Frame (Delay-Slot)
8003412c lui   a0,0x800b
80034130 lw    a0,DAT_800acbfc        ; Ziel-Slot-Zeiger
80034134 jal   FUN_8001a8f8           ; Yaw-Slew zum Ziel
80034138 _addiu a0,a0,0x34            ;   a0 = &ziel->X (+0x34; Z bei +0x3C)
8003413c lui   v1,0x800b
80034140 lw    v1,DAT_800ac768        ; virtuelles Pad (GEHALTEN)
80034148 andi  v0,v1,0x8              ; Bit 3 = LINKS
8003414c beq   v0,zero,LAB_80034170
80034150 _andi v0,v1,0x2
80034154-68    lhu/addiu/sh           ; DAT_800acabe -= 0x18   (Yaw, 24/4096 U./Frame)
8003416c andi  v0,v1,0x2              ; Bit 1 = RECHTS
80034170 beq   v0,zero,LAB_80034190
80034174 _clear a2                    ; (a2=0 fuer den Anim-Call)
80034178-8c   lhu/addiu/sh            ; DAT_800acabe += 0x18
80034190 lw    a0,DAT_800acbc4        ; PLW-Animationsbank Teil A
8003419c lw    a1,DAT_800acbc8        ; PLW-Animationsbank Teil B (Clip-Tabelle)
800341a0 jal   FUN_8001f314           ; Keyframe-Advance: (bankA, bankB, 0, 0x200)
800341a4 _ori  a3,zero,0x200          ;   0x200 = Crossfade-Schritt (7*0x200 -> 0 in 7 Frames)
800341a8 lui   v1,0x800b
800341ac lbu   v1,DAT_800aca5b
800341b4 lbu   a0,DAT_800acae9        ; In-Clip-Frame (von FUN_8001f3bc/f8b4 inkrementiert)
800341b8 addu  v1,v1,v0               ; Phase += Rueckgabe (1 am Clip-Ende)
800341bc sltiu a0,a0,0x8
800341c4 sb    v1,DAT_800aca5b        ;   -> Phase 2, wenn RAISE-Clip fertig
800341c8 bne   a0,zero,LAB_80034208   ; Frame < 8 -> kein Tief-Ausstieg
800341d0 lw    v0,DAT_800ac768
800341dc andi  v0,v0,0x20             ; Bit 5 = UNTEN gehalten
800341e0 beq   v0,zero,LAB_80034208
800341e4 _ori  v1,zero,0x1
800341e8 lhu   v0,DAT_800acaec
800341f4 sh    v1,DAT_800aca5a        ; !! sh 1: Substate:=1 UND aca5b:=0 (LE-Halbwort!)
800341f8 andi  v0,v0,0xbfff           ; Bit 14 (MITTE) weg
800341fc ori   v0,v0,0x2000           ; Bit 13 = TIEF
80034204 sh    v0,DAT_800acaec
80034208 lbu   v0,DAT_800acae9
80034214 sltiu v0,v0,0x9
80034218 bne   v0,zero,LAB_80034268   ; Frame < 9 -> kein Hoch-Ausstieg
80034220 lw    v0,DAT_800ac768
8003422c andi  v0,v0,0x10             ; Bit 4 = OBEN gehalten
80034230 beq   v0,zero,LAB_80034268
80034234 _ori  v1,zero,0x1
80034238 lhu   v0,DAT_800acaec
80034244 sh    v1,DAT_800aca5a        ; !! Substate:=1, aca5b:=0
80034248 andi  v0,v0,0xbfff
8003424c ori   v0,v0,0x8000           ; Bit 15 = HOCH
80034254 sh    v0,DAT_800acaec
80034258 j     LAB_80034268

; ---------- Phase 2: Abschluss ----------
80034260 lui   at,0x800b
80034264 sh    v0,DAT_800aca5a        ; v0=1 (Delay @0x800340a0): Substate:=1, aca5b:=0

; ---------- Exit ----------
80034268 lw    ra,0x10(sp) / addiu sp / jr ra   (bis 80034274)
```

### Pseudo-C

```c
void sub0_raise(void)   /* LAB_80034060 */
{
    switch (DAT_800aca5b) {                       /* Sub-Phase, Spieler +0x07 */
    case 0:                                       /* Init @0x800340ac */
        DAT_800aca5b = 1;
        DAT_800acae8 = 6;                         /* RAISE-Clip 6        @0x800340b0 */
        DAT_800acae9 = 0;                         /* Frame-Ctr           @0x800340cc */
        DAT_800acae3 = 7;                         /* Crossfade 7 Frames  @0x800340d4 */
        DAT_800acae0 = 0;                         /* Speed 0             @0x800340dc */
        DAT_800acaec = (DAT_800acaec & 0x1fff) | 0x4000;  /* Elevation=MITTE @0x800340e0-ec */
        DAT_800acaf3 = FUN_8003703c(0x7530);      /* Ziel-Scan r=30000   @0x800340f0-f4 */
        DAT_800aca3c |= 0xC0;                     /* Aim-Marker          @0x80034108 */
        /* FALL-THROUGH in case 1 (gleicher Frame) */
    case 1:                                       /* Tick @0x80034114 */
        if (DAT_800acaf3 & 1)                     /* lebendes Ziel gefunden */
            FUN_8001a8f8(DAT_800acbfc + 0x34, 0xC0);  /* Auto-Drehung 0xC0/Frame */
        if (DAT_800ac768 & 0x8)  DAT_800acabe -= 0x18;   /* LINKS:  Yaw -24 @0x80034160 */
        if (DAT_800ac768 & 0x2)  DAT_800acabe += 0x18;   /* RECHTS: Yaw +24 @0x80034184 */
        DAT_800aca5b += FUN_8001f314(DAT_800acbc4, DAT_800acbc8, 0, 0x200); /* Clip-Ende -> Phase 2 */
        if (DAT_800acae9 >= 8 && (DAT_800ac768 & 0x20)) {     /* UNTEN ab Frame 8 */
            *(u16*)&DAT_800aca5a = 1;             /* Substate 1 + aca5b=0 @0x800341f4 */
            DAT_800acaec = (DAT_800acaec & 0xbfff) | 0x2000;  /* TIEF */
        }
        if (DAT_800acae9 >= 9 && (DAT_800ac768 & 0x10)) {     /* OBEN ab Frame 9 (gewinnt) */
            *(u16*)&DAT_800aca5a = 1;             /* @0x80034244 */
            DAT_800acaec = (DAT_800acaec & 0xbfff) | 0x8000;  /* HOCH */
        }
        break;
    case 2:                                       /* RAISE-Clip war fertig */
        *(u16*)&DAT_800aca5a = 1;                 /* -> Sub 1 (Idle/Halten) @0x80034264 */
        break;
    }
}
```

### Mechanik-Notizen

- **Halbwort-Trick**: Alle drei Uebergaenge schreiben `sh 1` auf DAT_800aca5a. Little-Endian
  liegt DAT_800aca5b direkt dahinter -> EIN Store setzt Substate=1 UND nullt die Sub-Phase
  (@0x800341f4, @0x80034244, @0x80034264). Ein Port, der beide Bytes getrennt haelt, MUSS
  beide schreiben.
- **Elevation-Encoding im Aim-Wort DAT_800acaec**: Bit15=HOCH, Bit14=MITTE, Bit13=TIEF.
  Init erzwingt MITTE (@0x800340e0-ec). Gegenprobe: die Standard-Feuerpfad-Clip-Formel
  @0x800334a8-bc rechnet Clip = 7 + 2*Bit15 + 4*Bit13 (7=mitte, 9=hoch, 11=tief) und liest
  nur Bits 15/13.
- **Fruehausstieg**: TIEF ab In-Clip-Frame >= 8 (@0x800341bc sltiu 8), HOCH ab >= 9
  (@0x80034214 sltiu 9). Kein Sprung nach dem TIEF-Block: haelt man BEIDE Richtungen,
  ueberschreibt HOCH im selben Frame (ab Frame 9) den TIEF-Eintrag.
- **Pad-Bits sind VIRTUELL** (DAT_800ac768, gebaut von FUN_80030444 aus Preset-Tabelle
  @0x80073dbc, Datei-Offset 0x645BC, byte-gedumpt): virt[1]<-raw 0x2000 (RECHTS),
  virt[3]<-raw 0x8000 (LINKS), virt[4]<-raw 0x1000 (OBEN), virt[5]<-raw 0x4000 (UNTEN).
- **Auto-Aim nur bei Rueckgabe 1**: `andi 1` @0x80034120 — Klasse-2-Ziel (nur +0x9A<0
  gefunden) wird gelockt (DAT_800acbfc), aber NICHT angedreht.
- Sub 0 spawnt nichts und zieht keine Munition ab — reiner RAISE/Einstieg. Feuer beginnt
  erst in Sub 2 (0x80034510), das die Entlade-Tabelle @0x80074100 aufruft (@0x800345e4).

## 3. Das "Waffe-20-Raetsel" — GELOEST: Tabellen-Ueberlappung

Byte-Dump @0x80074100 (Entlade-/Schussausfuehrungs-Tabelle, Konsumenten @0x800334d8
(Standard-Feuer) und @0x800345e4 (Dauerfeuer-Sub 2), Index = DAT_800aca5d * 4):

```
[ 0]=80033C74 [ 1]=80033D7C [ 2]=80033EEC [ 3]=800337BC [ 4]=800337BC
[ 5]=800338A8 [ 6]=800338A8 [ 7]=800339A4 [ 8]=80033A58 [ 9]=80033B38
[10]=80033B58 [11]=80033B78 [12]=800347F8 [13]=80033B98 [14]=800C45A8
[15]=00000000 [16]=00000000 [17]=00000000 [18]=00000000 [19]=80034A30
```

Das Wort an der Stelle des vermeintlichen Eintrags "[20]" liegt bei
0x80074100 + 20*4 = **0x80074150** — das ist die ERSTE Zelle der Dauerfeuer-Sub-Tabelle.
Die Entlade-Tabelle hat nur 20 echte Eintraege [0..19]; "[20]=0x80034060" ist ein
Ueberlappungs-Artefakt der direkt anschliessenden Tabelle. Beweis:
1. Ghidra-XREF: `LAB_80034060 XREF[1]: 80074150(*)` — genau EIN Daten-Xref, aus der
   Sub-Tabelle; kein Xref aus einer Entlade-Tabellen-Position.
2. Waffen-Id 20 kann die Entlade-Tabelle nie erreichen: die Leser sitzen NUR in den
   Feuer-Handlern, und der Dispatch @0x80074030[20] = NULL — Id 20 feuert nie.

## 4. FUN_800369f8 (0x800369f8-0x80036B64) — Fuss-Anker / Root-Motion-Kompensation

WICHTIGE Korrektur zum Auftragskontext: FUN_800369f8 ist KEIN "Anim-Bank-Setter". Sie setzt
keinerlei Bank/Clip; sie kompensiert die Entity-Position um die Bone-Drift der laufenden
Animation (Beleg unten, Instruktionen + RE2-Strukturzwilling).

### Ablauf (Roh-Listing gekuerzt auf die Traeger; vollstaendig im Ghidra-Dump Z.132939ff)

```
800369f8 addiu sp,sp,-0x48
80036a00 move  s2,a0                 ; param_1 = Modus
80036a08 move  s1,a1                 ; param_2 = Ketten-Auswahl (0/1)
80036a20 lw    a1,DAT_800ac784       ; Spieler-Entity (= 0x800aca54)
80036a30 lw    s3,0x188(v0)          ; s3 = Posebuffer-Basis (*(player+0x188))
80036a34 jal   RotMatrix             ; RotMatrix(player+0x68 Winkel, player+0x20 Matrix)
80036a48 jal   FUN_80022da0          ; M = playerMat x Bone0-Matrix (s3+0x18)
80036a58 jal   FUN_80022da0          ; M x= Bone1-Matrix (s3+0xC4)
80036a60-70    s3 += param_2*0x204 + 0x2B0   ; Bone-Block = 0xAC gross, Matrix @Block+0x18:
                                     ;   param_2=0 -> Bones 2,3,4 ; param_2=1 -> Bones 5,6,7
80036a7c jal   FUN_80022da0          ; M x= Matrix(s3'-0x140)
80036a8c jal   FUN_80022da0          ; M x= Matrix(s3'-0x94)
80036a9c jal   FUN_80022da0          ; M x= Matrix(s3'+0x18)   ; M liegt auf sp+0x10
80036aa4 bne   s2,zero,LAB_80036af0
; ---- Modus 0 (param_1==0): Position kompensieren ----
80036ab4-ac8   player->X (+0x34) -= (M.t.x [sp+0x24] - *(s3'+0x54))
80036ad4-aec   player->Z (+0x3c) -= (M.t.z [sp+0x2c] - *(s3'+0x5c))
; ---- Modus !=0: Drift als Distanz messen ----
80036af0-b34   dx=(u16)M.t.x-(u16)*(s3'+0x54); dz=...(+0x5c); SquareRoot0(dx*dx+dz*dz)
80036b44 sh    v0,0x8c(v1)           ; -> player+0x8c (DAT_800acae0, Speed-Feld)
```

### Semantik

- Der Posebuffer besteht aus 0xAC-Byte-Bone-Bloecken: +0x18 = lokale MATRIX (FUN_8001f3bc
  schreibt dorthin), **+0x40 = Welt-MATRIX** -> deren Translation X/Z bei +0x54/+0x5C.
  Gegenprobe: bekannter Gun-Bone-Matrix-Offset +0x7A4 = 11*0xAC + 0x40 (Bone 11, Welt-Matrix).
- Die Funktion multipliziert die Kette Wurzel -> Bone0 -> Bone1 -> (2,3,4 | 5,6,7) frisch
  durch (FUN_80022da0 = GTE-MulMatrix mit Ausgabe, s. Decompile) und vergleicht die frische
  Welt-Translation des Endbones mit der beim letzten Compose gespeicherten (+0x54/+0x5C):
  - **Modus 0**: Entity-X/Z um die Differenz zurueckschieben -> der Endbone (Fuss) bleibt
    weltfest ("Foot-Lock"); Waffen-Anims schieben den Spieler nicht.
  - **Modus !=0**: Distanz der Drift -> player+0x8c (Speed) — misst den Schritt der Anim.
- Entry-Aufruf `FUN_800369f8(0, 1)` @0x80034044-4c: Modus 0 (kompensieren), Kette 1
  (Bones 5/6/7). Nach Standard-RE-Skelett eine BEIN-Kette mit Fuss als Endglied;
  links/rechts ist hier nicht belegt (siehe "Unklar").
- **RE2-Beleg (Strukturzwilling)**: RE2_Quellcode_V2/FUN_80016200.c ist dieselbe Funktion
  mit Entity als Parameter — identische Konstanten `param_3 * 0x204 + 0x2b0`, identische
  Kette (+0x18, +0xC4, -0x140, -0x94, +0x18), Modus 0 subtrahiert von Entity-X/Z
  (+0x38/+0x40 in RE2), sonst SquareRoot0(dx^2+dz^2) in ein Entity-Short (+0x144).
- Weitere RE1.5-Callsites (XREF[22], u.a. 80051f90/80052020/8005210c/800521f8/80052288/
  80052374/80052404/800524f0) liegen in den Geh-/Lauf-Handlern — konsistent mit
  Schrittmessung/Foot-Lock, nicht mit Bank-Setzen.

## 5. FUN_8003703c (0x8003703C-0x8003724C) — Auto-Aim-Zielscan

Callsites: 0x80031c2c, 0x80031e20, 0x80032fa8 (Standard-Feuer), 0x800340f0 (Sub 0),
0x80034f78 (Nahkampf), 0x800355cc (Sub 9). a0 = Suchradius.

```
8003708c lbu s3,DAT_800aca4e         ; Slot-Anzahl (Kandidaten)
80037098 addiu s2,v0,0x1de           ; s2 = 0x800acc2c = Slot-1-Basis (Flags-Wort +0x00)
8003709c addiu a2,v0,0x3a            ; a2 = &DAT_800aca88 = Spieler-X (player+0x34)
800370a0 addiu s0,v0,0x278           ; s0 = 0x800accc6 = Slot1+0x9A
  Schleife (Stride 0x1F4 @0x800370bc/c4/78/7c):
800370ac andi v0,v0,0x1              ; Slot-Flags Bit0 = aktiv, sonst skip
800370c8 lw -0x66(s0)  ; Slot+0x34 = X   \  Distanz zu Spieler-X (DAT_800aca88)
800370dc lw -0x5e(s0)  ; Slot+0x3C = Z   /  und Spieler-Z (DAT_800aca90), SquareRoot0
80037104 lbu -0x91(s0) ; Slot+0x09 Status-Byte
8003710c andi 0x60                   ; (Status & 0x60) != 0 -> NICHT anvisierbar, skip
80037118 lh 0x0(s0)    ; Slot+0x9A (signed short)
80037120 bltz -> Klasse C            ; +0x9A < 0 -> niedrigste Prioritaet
80037124/28 andi 0x80 -> Klasse B    ; Status-Bit 0x80
  sonst -> Klasse A
  je Klasse: naechstgelegener Kandidat im Radius (s5/s6/s4 = Bestdistanz, s7/s8/local = Index)
Ergebnis:
  Klasse A gefunden (s7) ODER sonst Klasse B (s8):
    DAT_800acbfc = 0x800aca38 + idx*0x1F4   (Slot-Zeiger; Index 1 -> 0x800acc2c)
    return 1                                (@0x80037218)
  nur Klasse C: DAT_800acbfc = Slot(local_38); return 2   (@0x800371ec)
  nichts:       DAT_800acbfc = 0x800acc2c (Default);  return 0   (@0x800371d8-e4)
```

Sub 0 cached die Rueckgabe in DAT_800acaf3 (@0x80034104) und testet spaeter NUR Bit 0
(@0x80034120): Auto-Drehung gibt es also nur fuer Klasse-A/B-Ziele; ein Klasse-2-Ergebnis
lockt das Ziel, dreht aber nicht.

## 6. Kurzbelege der uebrigen Helfer

- **FUN_8001f314** (Katalog ghidra1:98294, "Keyframe anim advance"): liest Clip-Index
  player+0x94 (=DAT_800acae8) und Frame player+0x95 (=DAT_800acae9) — die Spiegel-Globals
  sind dieselben Bytes, da DAT_800ac784 = 0x800aca54. Frame-Record-Bit 0x8000 waehlt
  FUN_8001f8b4 (interpoliert), sonst FUN_8001f3bc. BEIDE inkrementieren player+0x95 und
  geben 1 zurueck, wenn der Clip durch ist (Frame wrappt auf 0) — daher
  `DAT_800aca5b += ret` @0x800341b8. param_4=0x200 ist der Crossfade-Schritt: Blendfaktor
  = 0x200 * Restframes(+0x8f); mit Seed 7 (@0x800340d4) ein 7-Frame-Einblenden des
  RAISE-Clips. FUN_8001f3bc dekrementiert +0x8f pro Frame.
- **DAT_800acbc4 / DAT_800acbc8** = PLW-Waffen-Animationsbank-Paar (geladen von
  FUN_80036b68 bei Waffe != 0; Port-Doku anim_select_common.c:393/447 mit
  Original-Ladestellen @0x80030bec/f4; identischer anim_set-Aufruf @0x8003809c).
- **FUN_8001a8f8** (Katalog ghidra1:88250, "Yaw-slew"): dreht player+0x6A (=DAT_800acabe)
  per ratan2(FUN_8001a6d4) auf das Ziel zu; |diff| <= rate -> snap, sonst +-rate/Frame;
  negatives rate (Bit15) dreht vom Ziel WEG. Hier rate=0xC0 (@0x80034128).

## 7. Konstanten-Tabelle (alle mit Adresse)

| Adresse     | Wert | Bedeutung |
|-------------|------|-----------|
| 0x80034018  | DAT_800aca5a | FSM-Substate-Load des Entry |
| 0x8003402c  | 0x80074150 | Sub-Tabelle (10 Eintraege) |
| 0x80034044/4c | (0,1) | Argumente von FUN_800369f8 |
| 0x800340ac  | 1    | Sub-Phase := 1 |
| 0x800340b0/b8 | 6  | RAISE-Clip-Index (DAT_800acae8) |
| 0x800340cc  | 0    | In-Clip-Frame := 0 (DAT_800acae9) |
| 0x800340c4/d4 | 7  | Crossfade-Frames (DAT_800acae3, player+0x8f) |
| 0x800340dc  | 0    | Speed := 0 (DAT_800acae0, sh) |
| 0x800340e0/e4 | &0x1FFF, dann 0x4000 | Aim-Wort: Elevation := MITTE (Bit14) |
| 0x800340f4  | 0x7530 | Auto-Aim-Suchradius 30000 |
| 0x80034108  | 0xC0 | DAT_800aca3c OR= 0xC0 (Aim-Marker; Clear @0x80035f14) |
| 0x80034120  | &1   | nur Ziel-Klasse 1 dreht automatisch |
| 0x80034128  | 0xC0 | Auto-Aim-Drehrate/Frame (~16,9 Grad) |
| 0x80034148  | 0x8  | virt. LINKS (raw 0x8000 via @0x80073dbc[3]) |
| 0x80034160  | -0x18 | Yaw-Feinjustage links (24/4096 U.) |
| 0x8003416c  | 0x2  | virt. RECHTS (raw 0x2000 via @0x80073dbc[1]) |
| 0x80034184  | +0x18 | Yaw-Feinjustage rechts |
| 0x800341a4  | 0x200 | Crossfade-Schritt fuer FUN_8001f314 |
| 0x800341bc  | 8    | TIEF-Ausstieg erst ab In-Clip-Frame 8 |
| 0x800341dc  | 0x20 | virt. UNTEN (raw 0x4000 via @0x80073dbc[5]) |
| 0x800341f4  | sh 1 | Substate:=1 UND Sub-Phase:=0 (Halbwort!) |
| 0x800341fc  | 0x2000 | Aim-Wort Bit13 = TIEF |
| 0x80034214  | 9    | HOCH-Ausstieg erst ab In-Clip-Frame 9 |
| 0x8003422c  | 0x10 | virt. OBEN (raw 0x1000 via @0x80073dbc[4]) |
| 0x8003424c  | 0x8000 | Aim-Wort Bit15 = HOCH |
| 0x80034244/64 | sh 1 | Substate:=1 UND Sub-Phase:=0 |
| 0x80032e50/64 | 0x40000000 | Spieler-Flags-Bit im Aim-Mode-Wrapper LAB_80032e44 |
| 0x80074080  | NULL | Dispatch-Eintrag Waffe 20 (nie feuerbar) |

## 8. Unklar / nicht abschliessend belegbar

- Bedeutung des signed Short Slot+0x9A in FUN_8003703c (Klasse-C-Kriterium `< 0`;
  Kandidat: HP/Boden-Status). Mechanik und Prioritaet sind belegt, die Feld-Semantik nicht.
- Links/Rechts-Zuordnung der Bein-Ketten (2,3,4) vs (5,6,7) in FUN_800369f8 — braeuchte
  die EMR-Bone-Hierarchie des PL00-Skeletts; die Foot-Lock-Mechanik selbst ist belegt.
- Leser der DAT_800aca3c-Bits 0x40/0x80 (Setter @0x80034108, Clear @0x80035f14; im Port
  als KD-11 offen gefuehrt).
