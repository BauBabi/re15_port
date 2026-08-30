# RE2-Leon — Item-Box SPEICHER-MODELL, PERSISTENZ, EINBETTUNG (RE-Bericht, in Arbeit)

Ziel: Speicher-Layout der Box, Initialisierung bei New Game, Savegame-Persistenz,
Verdichtung, Einbettung in den Spielfluss, Sounds — byte-true belegt.

Quelle: RE2-Leon SLUS-00748, `info/re2leon/SLUS_007.48` (t_addr 0x80010000),
Ghidra-Dump `ghidra_re2_Leon.txt`, Decompilate `RE2_Quellcode_V2/FUN_*.c`.

STATUS: IN ARBEIT — wird inkrementell ergaenzt. NICHTS wurde in den Port eingebaut.

## 0. Arbeitsprotokoll
- [start] Datei angelegt.

## 1. KAPAZITAET — 64 Box-Plaetze (Stand: 2 harte Belege)

### 1a) Symbol-Grenze im BSS (Ghidra-Datenlisting)
`ghidra_re2_Leon.txt` Zeile ~516731 ff.:

```
DAT_800d4a68   XREF[23]: FUN_800703b8:80070488(*), ...      <- box[0].id
DAT_800d4a69   XREF[17]: FUN_800703b8:8007047c(*), ...      <- box[0].qty
DAT_800d4a6a   XREF[12]: FUN_800703b8:8007045c(*), ...      <- box[0].size/typ
DAT_800d4a6c   XREF[1] : FUN_80072c6c:80072ce0(R)
DAT_800d4a6e   XREF[1] : FUN_80072c6c:80072cf4(R)
DAT_800d4a74   XREF[1] : 8006fa70(R)
DAT_800d4a78   XREF[1] : 8006fa70(R)
   ... (unbenannter, nur genullter Bereich) ...
DAT_800d4b68   XREF[14]: FUN_800692dc:800692ec(R), ...      <- NAECHSTES eigenstaendiges Symbol
```
0x800d4b68 - 0x800d4a68 = **0x100 = 256 Byte = 64 Eintraege x 4 Byte**.

### 1b) Maske 0x3f im Box-Render-Loop @0x8006fa5c
`re2_disasm.py dis 0x8006fa50 20` (info/re2leon/PSX.EXE):
```
8006fa50: lbu v0,36(s3)          ; s3=0x800d5bf0 -> +0x24 = DAT_800d5c14 (Box-Fensteroffset)
8006fa54: addiu s0,s0,-1
8006fa58: addu v0,v0,s0          ; v0 = fenster + i
8006fa5c: andi v0,v0,0x3f        ; <<< MODULO 64
8006fa60: sll v0,v0,2            ; *4  (4 Byte pro Slot)
8006fa64: addu v0,s1,v0          ; s1 = 0x800cc1e8
8006fa68: lui at,0x1
8006fa6c: addu at,v0,at
8006fa70: lbu a1,-30592(at)      ; = 0x800d4a68 + ((fenster+i)&0x3f)*4   -> box[slot].id
```
Rechnung: s1(0x800cc1e8) + slot*4 + 0x10000 - 0x7780 = **0x800d4a68 + slot*4**. Damit ist
`DAT_800d4a74` = Slot 3 und `DAT_800d4a78` = Slot 4 nur ein Ghidra-Artefakt derselben
Zugriffs-Instruktion 8006fa70, kein eigenes Feld.

### 1c) Box-Screen-INIT @0x8006f900 (Zustands-Handler, Zeiger in Tabelle @0x800a9378)
```
8006f900: addiu sp,sp,-0x38
8006f904: li a0,0xa9
8006f908/0c: a1 = 0x8019c000
8006f914/18: a3 = "ITEM_ALL"            (String @0x80011d34)
8006f930: jal FUN_80012fb8              ; Datei-Load "ITEM_ALL" nach 0x8019c000
8006f938/3c: s3 = 0x800d5bf0            ; <<< Box-Screen-Statusblock
8006f944: sb 1 -> DAT_800d5bf0          ; +0x00 = 1
8006f950: sb 3 -> DAT_800d5bf1          ; +0x01 = 3  (Substate-Index in PTR-Tabelle 0x800a9ba8)
8006f958: sb 0 -> DAT_800d5c15          ; +0x25 = 0
8006f960: sb 0 -> DAT_800d5c14          ; +0x24 = 0  (Box-Fensteroffset zurueckgesetzt)
8006f964: jal FUN_80070e58
```
=> Der Statusblock des Box-Screens liegt bei **0x800d5bf0**; Cursor `DAT_800d5bfc` = +0x0c,
Box-Fensteroffset `DAT_800d5c14` = +0x24.

## 2. Nachbarschaft von Inventar/Box im BSS (Zwischenstand)

- 0x800d4a3c = Inventar-Slot 0 {id@+0, qty@+1, size@+2}, Stride 4.
- Eigene Symbole bei 0x800d4a5c/5e/60/61/62/64/65/66 (= Slot 8/9/10 Region) -> Inventar-
  Nutzbereich endet vor 0x800d4a68.
- 0x800d4a68 = Box-Slot 0, 64 Slots, endet 0x800d4b67 (naechstes Symbol 0x800d4b68).
- 0x800d5bf0 = Statusblock des Box-Screens (+0x0c Cursor, +0x24 Box-Fensteroffset).
- 0x800d46ac = Inventar-Kapazitaet (byte).
- 0x8003ae44 ist KEIN Save-Code: dort loescht ein Skript-Pfad Inventarslot von Item 53
  (jal 0x800696cc mit a0=53 -> Slot-Suche, dann id/qty/size=0, dann jal 0x80069714).

## 3. SPEICHERSTAND — Suche (Zwischenstand)

WICHTIG: RE2s Karten-I/O liegt NICHT in der PSX.EXE, sondern im Overlay
`info/re2leon/COMMON/BIN/MEM_CARD.BIN` (Ladeadresse 0x801c0000, 0x5d7c Byte;
"BASLUS" @0x801c0008, "bu00" @0x801c02f4 — bereits in analysis/save_injured_state.md §4
festgehalten). Ein EXE-weiter Scan nach Adressbildung (lui 0x800d + addiu) auf
0x800d4a68 / 0x800d4a3c findet NUR Menue-Code (0x80068xxx-0x80072xxx), KEINEN Save-Kopierer.
=> Der Save-Blob wird im Overlay gebaut. Weiter dort.

### 3a) SAVE: das Inventar wird kopiert — 44 Byte @0x800d4a3c (Overlay MEM_CARD.BIN)

```
801c116c: lui  fp,0x800d
801c1170: addiu fp,fp,0x4a3c        ; fp = 0x800d4a3c  = INVENTAR-Basis
801c1174: addu a1,fp,zero           ; a1 = Quelle
801c1178: sw   t1,624(sp)           ; t1 = sp+496 = Ziel im Save-Puffer
801c117c: lw   a0,624(sp)           ; a0 = Ziel
801c1180: jal  0x80076a00           ; memcpy(dst,src,len)
801c1184: addiu a2,zero,44          ; <<< LAENGE = 44 = 0x2c Byte = 11 Slots x 4
```
Danach direkt der Farbband-Verbrauch:
```
801c1188: lbu  t1,-895(fp)          ; 0x800cfc81
801c118c: addiu a0,zero,30          ; Item-Id 30 = Farbband
801c1190: jal  0x800696cc           ; Inventar-Slot-Suche(id)
801c1198: sll  v1,v0,2
801c11a4: lbu  v0,0x4a3d(at+v1)     ; qty
801c11ac: addiu v0,v0,-1            ; qty-1
801c11b8: sb   v0,0x4a3d(at+v1)
801c11c0: bne  v0,zero,0x801c11e8
801c11d0: sb   zero,0x4a3c(at+v1)   ; id = 0
801c11dc: sb   zero,0x4a3e(at+v1)   ; size = 0
```
**Konsequenz:** 0x800d4a3c + 0x2c = 0x800d4a68 — die Kopie endet GENAU an der Box-Basis.
Die Box ist in DIESEM memcpy NICHT enthalten. (Weiter: gibt es einen zweiten Kopierer?)
