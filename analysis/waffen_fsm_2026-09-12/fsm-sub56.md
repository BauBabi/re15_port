# Waffen-FSM Subs 5+6 — Nahkampf RAISE & HOLD (0x80034EE8 / 0x800350C4)

Quelle: `ghidra1_V2.txt` Zeilen 130949–131315 (RE1.5 PSX.EXE, t_addr=0x80010000, verifiziert
gegen `info/Re1.5/PSX.EXE` Dateioffset `0x800 + (va − 0x80010000)`).
Decompilate-Belege: `RE_15_Quellcode_V2/FUN_8003703c.c`, `FUN_8001a8f8.c`, `FUN_8001f314.c`,
`FUN_8001f3bc.c`. Port-Abgleich: `re15_port/engine/src/player_common.c` (Aim-FSM, zitiert dieselben Adressen).

---

## 0. Einbettung: ZWEI Dispatcher teilen sich EINE 10er-Tabelle

Die Master-Tabelle @**0x80074150** hat 10 Einträge, wird aber von **zwei** Dispatchern gelesen:

1. **Dauerfeuer-Master 0x80034014** (Waffen-Ids 12/14/19): `lbu DAT_800aca5a → sll 2 → 0x80074150[idx]`.
   Dieser Ast schreibt **nie** einen Substate ≥ 5 — alle `sh …→DAT_800aca5a`-Stores im Bereich
   0x80033EBC–0x80034E40 wurden geprüft, gespeicherte Werte:

   | Store-Adresse | Wert | Kontext |
   |---|---|---|
   | @0x800341F4 | 1 | (v1=1 gesetzt @0x800341E4) |
   | @0x80034244 | 1 | (v1=1 gesetzt @0x80034234) |
   | @0x80034264 | 1 | (v0=1 aus Delay-Slot @0x800340A0, Sprung @0x8003409C) |
   | @0x8003430C | 3 | (@0x80034304 `ori v0,zero,0x3`) |
   | @0x80034418 | 2 | (@0x80034410) |
   | @0x80034474 | 4 | (@0x8003446C) |
   | @0x800347E4 | 1 | (@0x800347DC) |
   | @0x80034D40 | 0 | (sh zero = FSM-Exit) |
   | @0x80034E40 | 1 | (@0x80034E38) |

2. **Nahkampf-Dispatch LAB_80034E70** (XREF: 0x80074030/34/38 = Waffen-Ids **0..2** der
   Waffen-Dispatch-Tabelle @0x80074030). Er indexiert mit Basis **0x80074164** = &Master-Tabelle[5]:

   ```
   80034e70 addiu sp,sp,-0x18          ; Funktionsanfang (Vorgänger endet jr ra @0x80034e68,
   80034e74 lui   v1,0x800b            ;  Delay-nop @0x80034e6c — Grenze sauber)
   80034e78 addiu v1,v1,-0x35a6        ; v1 = &DAT_800aca5a (FSM-Substate)
   80034e7c sw    ra,0x10(sp)
   80034e80 lhu   v0,0x0(v1)           ; v0 = Substate (HALFWORD: aca5a + Phase aca5b!)
   80034e88 bne   v0,zero,LAB_80034eac ; Substate/Phase != 0 → direkt dispatchen
   80034e90 lui   v0,0x800b
   80034e94 lw    v0,=>DAT_800aca54    ; Spieler-Statuswort
   80034e9c andi  v0,v0,0x4000         ; Bit 0x4000 = Messer bereits in der Hand
   80034ea0 bne   v0,zero,LAB_80034eac ;  in der Hand → Substate 0 = RAISE
   80034ea4 _ori  v0,zero,0x4
   80034ea8 sb    v0,=>DAT_800aca5a    ; NICHT in der Hand → Substate := 4 (DRAW @0x80035538)
   LAB_80034eac:
   80034eb0 lbu   v0,=>DAT_800aca5a
   80034eb8 sll   v0,v0,0x2
   80034ebc lui   at,0x8007
   80034ec0 addiu at,at,0x4164         ; STOP: Basis 0x80074164 = Master-Tabelle[5]!
   80034ec8 lw    v0,0x0(at)
   80034ed0 jalr  v0                   ; Nahkampf-Substate-Handler
   80034ed8..e4  ra-Restore, jr ra     ; (KEIN FUN_800369f8 hier — die Subs rufen es selbst)
   ```

   **Nahkampf-Substate → Handler** (Pointer aus PSX.EXE @0x80074164, Ghidra-XREFs `(*)`):

   | Substate | Tabellenzelle | Handler | Rolle |
   |---|---|---|---|
   | 0 | 0x80074164 | **0x80034EE8** | RAISE (dieses Dossier, "Sub 5") |
   | 1 | 0x80074168 | **0x800350C4** | HOLD (dieses Dossier, "Sub 6") |
   | 2 | 0x8007416C | 0x80035314 | SLASH/Angriff |
   | 3 | 0x80074170 | 0x80035424 | LOWER |
   | 4 | 0x80074174 | 0x80035538 | DRAW (Messer ziehen) |

   ⇒ **"Sub 5/6" der Master-Tabelle sind in Wahrheit die Nahkampf-Substates 0/1.** Der
   Dauerfeuer-Ast erreicht sie nie (siehe Store-Liste oben). Die Substate-Nummern, die
   Sub5/Sub6 selbst schreiben (1/2/3), meinen also die NAHKAMPF-Tabelle (Basis 0x80074164).

Strukturvergleich: Dauerfeuer-Sub0 @0x80034060 ist der Zwilling von Sub 5 mit Radius
0x7530=30000 (@0x800340F4) statt 0x1388=5000 — deckt sich mit dem Port-Kommentar
"Latch radii: gun 30000, melee draw 2000, melee re-raise 5000" (player_common.c:717).

---

## 1. Sub 5 = Nahkampf-Substate 0: RAISE (0x80034EE8–0x800350C3)

Grenze: `jr ra` @0x800350BC, Delay-nop @0x800350C0 → letztes Byte 0x800350C3. Kein
Einsprung von außen außer der Tabellenzelle 0x80074164 (Ghidra-XREF), alle Labels intern.

### 1.1 Roh-Listing mit Kommentar

```
80034ee8 e8 ff bd 27   addiu sp,sp,-0x18
80034eec 0b 80 05 3c   lui   a1,0x800b
80034ef0 5b ca a5 24   addiu a1,a1,-0x35a5      ; a1 = &DAT_800aca5b (Phase im Substate)
80034ef4 10 00 bf af   sw    ra,0x10(sp)
80034ef8 00 00 a3 90   lbu   v1,0x0(a1)         ; v1 = Phase
80034efc 01 00 02 34   ori   v0,zero,0x1
80034f00 26 00 62 10   beq   v1,v0,LAB_80034f9c ; Phase 1 → Tick
80034f04 02 00 62 28   _slti v0,v1,0x2
80034f08 05 00 40 10   beq   v0,zero,LAB_80034f20 ; Phase >= 2 → Vergleich mit 2
80034f10 08 00 60 10   beq   v1,zero,LAB_80034f34 ; Phase 0 → Init
80034f14 01 00 02 34   _ori  v0,zero,0x1
80034f18 2b d4 00 08   j     LAB_800350ac       ; (Sammelfall, a0=0)
80034f1c 21 20 00 00   _clear a0

LAB_80034f20:                                    ; Phase >= 2
80034f20 02 00 02 34   ori   v0,zero,0x2
80034f24 5e 00 62 10   beq   v1,v0,LAB_800350a0 ; Phase 2 → Übergang zu Substate 1
80034f28 01 00 02 34   _ori  v0,zero,0x1        ;   (v0=1 wird DRÜBEN als Substate-Wert benutzt!)
80034f2c 2b d4 00 08   j     LAB_800350ac       ; Phase >= 3: nur Anim-Bank-Refresh
80034f30 21 20 00 00   _clear a0

LAB_80034f34:                                    ; ---- Phase 0: INIT ----
80034f34 00 00 a2 a0   sb    v0,0x0(a1)         ; Phase := 1 (v0=1 aus 80034f14)
80034f38 06 00 02 34   ori   v0,zero,0x6
80034f40 e8 ca 22 a0   sb    v0,=>DAT_800acae8  ; STOP: Clip := 6 (Waffe-anheben)
80034f48 ec ca 42 94   lhu   v0,=>DAT_800acaec  ; Aim-Wort
80034f4c 07 00 03 34   ori   v1,zero,0x7
80034f54 e9 ca 20 a0   sb    zero,=>DAT_800acae9 ; In-Clip-Frame-Cursor := 0
80034f5c e3 ca 23 a0   sb    v1,=>DAT_800acae3  ; Anim-Untertakt := 7 (Port: anim_frac)
80034f64 e0 ca 20 a4   sh    zero,=>DAT_800acae0 ; Vorschub := 0 (Spieler verwurzelt)
80034f68 ff 1f 42 30   andi  v0,v0,0x1fff       ; Elevationsbits 13/14/15 löschen
80034f6c 00 40 42 34   ori   v0,v0,0x4000       ; Bit 14 = NEUTRAL setzen
80034f74 ec ca 22 a4   sh    v0,=>DAT_800acaec
80034f78 0f dc 00 0c   jal   FUN_8003703c       ; Auto-Aim-Ziel latchen
80034f7c 88 13 04 34   _ori  a0,zero,0x1388     ; STOP: Radius 0x1388 = 5000 (Nahkampf-Re-Raise)
80034f84 3c ca 63 8c   lw    v1,=>DAT_800aca3c  ; Spieler-Slot-Flagwort (+0x04 im 0x1F4-Mirror)
80034f8c f3 ca 22 a0   sb    v0,=>DAT_800acaf3  ; Latch-Ergebnis merken (0/1/2)
80034f90 c0 00 63 34   ori   v1,v1,0xc0
80034f98 3c ca 23 ac   sw    v1,=>DAT_800aca3c  ; Flags |= 0xC0 ("Aim aktiv"; Gegenstück:
                                                 ;  Clear @0x80034D44-4C, Masken ~0x80,~0x40)

LAB_80034f9c:                                    ; ---- Phase 1: TICK (auch Fallthrough) ----
80034fa0 f3 ca 42 90   lbu   v0,=>DAT_800acaf3
80034fa8 01 00 42 30   andi  v0,v0,0x1
80034fac 05 00 40 10   beq   v0,zero,LAB_80034fc4 ; nur wenn Latch-Rückgabe==1 (Bit0)
80034fb0 c0 00 05 34   _ori  a1,zero,0xc0       ; STOP: Slew-Rate 0xC0 = 192/Frame
80034fb8 fc cb 84 8c   lw    a0,=>DAT_800acbfc  ; gelatchter Ziel-Mirror (aus FUN_8003703c)
80034fbc 3e 6a 00 0c   jal   FUN_8001a8f8       ; Yaw-Slew des Spielers auf Ziel
80034fc0 34 00 84 24   _addiu a0,a0,0x34        ;   a0 = &ziel[+0x34]=x (z liegt bei +0x3C)

LAB_80034fc4:
80034fc8 68 c7 84 8c   lw    a0,=>DAT_800ac768  ; Pad-/Action-Wort
80034fd0 08 00 82 30   andi  v0,a0,0x8
80034fd4 12 00 40 10   beq   v0,zero,LAB_80035020 ; Bit 0x8 = manuell drehen (Richtung A)
80034fd8 02 00 82 30   _andi v0,a0,0x2
80034fe0 5d ca 63 90   lbu   v1,=>DAT_800aca5d  ; angelegte Waffen-Id
80034fe8 ff ff 63 24   addiu v1,v1,-0x1
80034fec 80 10 03 00   sll   v0,v1,0x2
80034ff0 21 10 43 00   addu  v0,v0,v1           ; idx = (id-1)*5
80034ff8 90 40 21 24   addiu at,at,0x4090
80035000 00 00 23 90   lbu   v1,[0x80074090+idx] ; STOP: byte0 des 5-Byte-Records = 0x18 = 24
80035008 be ca 42 94   lhu   v0,=>DAT_800acabe  ; Spieler-Yaw (Q12)
80035010 23 10 43 00   subu  v0,v0,v1
80035018 be ca 22 a4   sh    v0,=>DAT_800acabe  ; Yaw -= 24
8003501c 02 00 82 30   andi  v0,a0,0x2

LAB_80035020:
80035020 11 00 40 10   beq   v0,zero,LAB_80035068 ; Bit 0x2 = manuell drehen (Richtung B)
80035024 21 30 00 00   _clear a2                 ;   (a2=0 für FUN_8001f314 — läuft IMMER)
8003502c 5d ca 63 90   lbu   v1,=>DAT_800aca5d
80035034 ff ff 63 24   addiu v1,v1,-0x1
80035038 80 10 03 00   sll   v0,v1,0x2
8003503c 21 10 43 00   addu  v0,v0,v1
80035044 90 40 21 24   addiu at,at,0x4090
8003504c 00 00 22 90   lbu   v0,[0x80074090+idx] ; byte0 = 24
80035054 be ca 63 94   lhu   v1,=>DAT_800acabe
8003505c 21 10 43 00   addu  v0,v0,v1
80035064 be ca 22 a4   sh    v0,=>DAT_800acabe  ; Yaw += 24

LAB_80035068:
8003506c c4 cb 84 8c   lw    a0,=>DAT_800acbc4  ; Anim-Bank-Zeiger 1 (an Interp-Pfad gereicht)
80035074 c8 cb a5 8c   lw    a1,=>DAT_800acbc8  ; Anim-Bank-Zeiger 2 = EDD-Clip-Tabelle
80035078 c5 7c 00 0c   jal   FUN_8001f314       ; Keyframe-Anim-Advance (a2=0 vorwärts)
8003507c 00 02 07 34   _ori  a3,zero,0x200      ; param_4 = 0x200 (nur Interp-Pfad 8001f8b4)
80035084 5b ca 63 90   lbu   v1,=>DAT_800aca5b
8003508c 21 18 62 00   addu  v1,v1,v0           ; STOP: Phase += Rückgabe (1 exakt am Clip-Ende)
80035094 5b ca 23 a0   sb    v1,=>DAT_800aca5b  ;   → Phase 1→2, Folgeframe nimmt Übergang
80035098 2b d4 00 08   j     LAB_800350ac
8003509c 21 20 00 00   _clear a0

LAB_800350a0:                                    ; ---- Phase 2: RAISE fertig ----
800350a4 5a ca 22 a4   sh    v0,=>DAT_800aca5a  ; STOP: HALFWORD-Store v0=1: Substate := 1
                                                 ;  (HOLD) UND Phase (aca5b, High-Byte) := 0
800350a8 21 20 00 00   clear a0

LAB_800350ac:                                    ; ---- gemeinsamer Ausgang ----
800350ac 7e da 00 0c   jal   FUN_800369f8       ; Anim-Bank-Setter
800350b0 21 28 00 00   _clear a1                ;   Aufruf (0,0)
800350b4 10 00 bf 8f   lw    ra,0x10(sp)
800350b8 18 00 bd 27   addiu sp,sp,0x18
800350bc 08 00 e0 03   jr    ra
800350c0 00 00 00 00   _nop
```

### 1.2 Pseudo-C

```c
/* Nahkampf-Substate 0: RAISE — Messer in der Hand (aca54&0x4000), Waffe hochnehmen. */
void melee_sub0_raise(void)                      /* 0x80034EE8 */
{
    u8 phase = DAT_800aca5b;
    if (phase == 0) {                            /* INIT @0x80034F34 */
        DAT_800aca5b = 1;
        DAT_800acae8 = 6;                        /* Clip 6 = Anheben        @0x80034F38/F40 */
        DAT_800acae9 = 0;                        /* Frame-Cursor            @0x80034F54 */
        DAT_800acae3 = 7;                        /* Anim-Untertakt          @0x80034F4C/F5C */
        DAT_800acae0 = 0;                        /* Vorschub 0 = verwurzelt @0x80034F64 */
        DAT_800acaec = (DAT_800acaec & 0x1FFF) | 0x4000;   /* Elevation NEUTRAL @0x80034F68-74 */
        DAT_800acaf3 = FUN_8003703c(5000);       /* Ziel-Latch, Radius 0x1388 @0x80034F78/7C */
        DAT_800aca3c |= 0xC0;                    /* "Aim aktiv"-Flags       @0x80034F90-98 */
        /* fällt in den Tick */
    } else if (phase == 2) {                     /* @0x80034F24 */
        *(u16 *)&DAT_800aca5a = 1;               /* → Substate 1 (HOLD), Phase 0 @0x800350A4 */
        goto bank;
    } else if (phase != 1) {
        goto bank;                               /* Phase >= 3: nur Bank-Refresh */
    }
    /* TICK (Phase 1) @0x80034F9C */
    if (DAT_800acaf3 & 1)                        /* nur Latch-Klasse 1     @0x80034FA8 */
        FUN_8001a8f8((short *)(*(u8 **)&DAT_800acbfc + 0x34), 0xC0);  /* @0x80034FBC */
    u32 pad = DAT_800ac768;
    if (pad & 0x8)                               /* manuell drehen A       @0x80034FD0 */
        DAT_800acabe -= TBL_74090[(DAT_800aca5d - 1) * 5 + 0];   /* -24  @0x80035000-18 */
    if (pad & 0x2)                               /* manuell drehen B       @0x80035020 */
        DAT_800acabe += TBL_74090[(DAT_800aca5d - 1) * 5 + 0];   /* +24  @0x8003504C-64 */
    DAT_800aca5b += FUN_8001f314(DAT_800acbc4, DAT_800acbc8, 0, 0x200);  /* @0x80035068-94 */
bank:
    FUN_800369f8(0, 0);                          /* @0x800350AC-B0 */
}
```

### 1.3 Konstanten (Sub 5)

| Adresse | Wert | Bedeutung |
|---|---|---|
| @0x80034F38/F40 | 6 | Anheben-Clip (DAT_800acae8) |
| @0x80034F4C/F5C | 7 | Anim-Untertakt-Init (DAT_800acae3) |
| @0x80034F54 | 0 | Frame-Cursor-Reset (DAT_800acae9) |
| @0x80034F64 | 0 | Vorschub (DAT_800acae0) — Spieler verwurzelt |
| @0x80034F68/6C | &0x1FFF, \|0x4000 | Aim-Wort: Elevation NEUTRAL (Bit 14) |
| @0x80034F7C | 0x1388 = 5000 | Auto-Aim-Latch-Radius (Nahkampf-Re-Raise) |
| @0x80034F90 | 0xC0 | Spieler-Flagwort DAT_800aca3c \|= 0x40\|0x80 |
| @0x80034FB0 | 0xC0 = 192 | Yaw-Slew-Rate/Frame für FUN_8001a8f8 |
| @0x80035000 | byte0 @0x80074090 = 0x18 = 24 | manuelle Drehrate im RAISE |
| @0x8003507C | 0x200 | FUN_8001f314 param_4 (Interp-Pfad) |
| @0x800350A4 | sh 1 | Substate := 1 (HOLD) + Phase := 0 (Halfword) |

---

## 2. Sub 6 = Nahkampf-Substate 1: HOLD (0x800350C4–0x80035313)

Grenze: `jr ra` @0x8003530C, Delay-nop @0x80035310 → letztes Byte 0x80035313. XREF nur
Tabellenzelle 0x80074168.

### 2.1 Roh-Listing mit Kommentar

```
800350c4 e8 ff bd 27   addiu sp,sp,-0x18
800350c8 0b 80 04 3c   lui   a0,0x800b
800350cc 5b ca 84 24   addiu a0,a0,-0x35a5      ; a0 = &DAT_800aca5b (Phase) — bleibt in a0!
800350d0 10 00 bf af   sw    ra,0x10(sp)
800350d4 00 00 82 90   lbu   v0,0x0(a0)
800350dc 11 00 40 14   bne   v0,zero,LAB_80035124 ; Phase != 0 → Eingabe-Auswertung
800350e0 07 00 02 34   _ori  v0,zero,0x7

; ---- Phase 0: INIT / Clip-Wahl ----
800350e8 ec ca 63 94   lhu   v1,=>DAT_800acaec  ; Aim-Wort
800350f0 e3 ca 22 a0   sb    v0,=>DAT_800acae3  ; Anim-Untertakt := 7
800350f4 01 00 02 34   ori   v0,zero,0x1
800350f8 00 00 82 a0   sb    v0,0x0(a0)         ; Phase := 1
80035100 e9 ca 20 a0   sb    zero,=>DAT_800acae9 ; Frame-Cursor := 0
80035104 c2 13 03 00   srl   v0,v1,0xf          ; Bit 15 (hoch)
80035108 40 10 02 00   sll   v0,v0,0x1          ;   *2
8003510c 08 00 42 24   addiu v0,v0,0x8          ; Basis 8
80035110 c2 1a 03 00   srl   v1,v1,0xb          ; Bit 13 (tief)
80035114 04 00 63 30   andi  v1,v1,0x4          ;   → 4
80035118 21 10 43 00   addu  v0,v0,v1
80035120 e8 ca 22 a0   sb    v0,=>DAT_800acae8  ; STOP: Halteclip = 8+2*hoch+4*tief = 8/10/12

LAB_80035124:                                    ; ---- Eingabe-Auswertung (Prioritätskette) ----
80035128 68 c7 63 8c   lw    v1,=>DAT_800ac768  ; Pad-/Action-Wort
80035130 00 01 62 30   andi  v0,v1,0x100
80035134 06 00 40 14   bne   v0,zero,LAB_80035150 ; Bit 0x100 = R1/Aim gehalten
80035138 10 00 62 30   _andi v0,v1,0x10         ;   (Delay: Bit 0x10 vormaskiert)
8003513c 03 00 02 34   ori   v0,zero,0x3
80035144 5a ca 22 a4   sh    v0,=>DAT_800aca5a  ; STOP: R1 LOS → Substate := 3 (LOWER), Phase 0
80035148 c1 d4 00 08   j     LAB_80035304       ;   Früh-Exit: KEIN f314/369f8 in dem Frame

LAB_80035150:
80035150 0d 00 40 10   beq   v0,zero,LAB_80035188 ; Bit 0x10 (hoch) nicht gedrückt → weiter
8003515c ec ca 63 94   lhu   v1,=>DAT_800acaec
80035164 00 80 62 30   andi  v0,v1,0x8000
80035168 07 00 40 14   bne   v0,zero,LAB_80035188 ; schon "hoch"? → kein Wechsel
8003516c ff 1f 62 30   _andi v0,v1,0x1fff
80035170 00 80 42 34   ori   v0,v0,0x8000       ; Elevation := HOCH (Bit 15)
80035174 00 00 80 a0   sb    zero,0x0(a0)       ; STOP: Phase := 0 → Folgeframe wählt Clip 10
8003517c ec ca 22 a4   sh    v0,=>DAT_800acaec
80035180 c1 d4 00 08   j     LAB_80035304       ;   Früh-Exit (ohne Anim-Tick)

LAB_80035188:
8003518c 68 c7 42 8c   lw    v0,=>DAT_800ac768
80035194 20 00 42 30   andi  v0,v0,0x20
80035198 0e 00 40 10   beq   v0,zero,LAB_800351d4 ; Bit 0x20 (tief) nicht gedrückt → weiter
800351a4 ec ca 63 94   lhu   v1,=>DAT_800acaec
800351ac 00 20 62 30   andi  v0,v1,0x2000
800351b0 08 00 40 14   bne   v0,zero,LAB_800351d4 ; schon "tief"? → kein Wechsel
800351b4 ff 1f 62 30   _andi v0,v1,0x1fff
800351b8 00 20 42 34   ori   v0,v0,0x2000       ; Elevation := TIEF (Bit 13)
800351c0 5b ca 20 a0   sb    zero,=>DAT_800aca5b ; Phase := 0 → Clip 12
800351c8 ec ca 22 a4   sh    v0,=>DAT_800acaec
800351cc c1 d4 00 08   j     LAB_80035304

LAB_800351d4:
800351d8 68 c7 42 8c   lw    v0,=>DAT_800ac768
800351e0 30 00 42 30   andi  v0,v0,0x30
800351e4 0e 00 40 14   bne   v0,zero,LAB_80035220 ; hoch ODER tief gehalten → kein Neutral-Reset
800351ec ec ca 63 94   lhu   v1,=>DAT_800acaec
800351f8 00 40 62 30   andi  v0,v1,0x4000
800351fc 08 00 40 14   bne   v0,zero,LAB_80035220 ; schon neutral? → weiter
80035200 ff 1f 62 30   _andi v0,v1,0x1fff
80035204 00 40 42 34   ori   v0,v0,0x4000       ; Elevation := NEUTRAL (Bit 14)
8003520c ec ca 22 a4   sh    v0,=>DAT_800acaec
80035214 5b ca 20 a0   sb    zero,=>DAT_800aca5b ; Phase := 0 → Clip 8
80035218 c1 d4 00 08   j     LAB_80035304

LAB_80035220:
80035224 68 c7 84 8c   lw    a0,=>DAT_800ac768
8003522c 40 00 82 30   andi  v0,a0,0x40
80035230 05 00 40 10   beq   v0,zero,LAB_80035248 ; Bit 0x40 = Feuer/Slash
80035234 02 00 02 34   _ori  v0,zero,0x2
8003523c 5a ca 22 a4   sh    v0,=>DAT_800aca5a  ; STOP: FEUER → Substate := 2 (SLASH), Phase 0
80035240 c1 d4 00 08   j     LAB_80035304       ;   Früh-Exit

LAB_80035248:                                    ; ---- manuelles Drehen + Anim-Tick ----
80035248 08 00 82 30   andi  v0,a0,0x8
8003524c 12 00 40 10   beq   v0,zero,LAB_80035298
80035250 02 00 82 30   _andi v0,a0,0x2
80035258 5d ca 63 90   lbu   v1,=>DAT_800aca5d  ; Waffen-Id
80035260 ff ff 63 24   addiu v1,v1,-0x1
80035264 80 10 03 00   sll   v0,v1,0x2
80035268 21 10 43 00   addu  v0,v0,v1           ; idx = (id-1)*5
80035270 91 40 21 24   addiu at,at,0x4091       ; STOP: Basis 0x80074091 = byte1 des Records!
80035278 00 00 23 90   lbu   v1,[0x80074091+idx] ; byte1 = 0x30 = 48
80035280 be ca 42 94   lhu   v0,=>DAT_800acabe
80035288 23 10 43 00   subu  v0,v0,v1
80035290 be ca 22 a4   sh    v0,=>DAT_800acabe  ; Yaw -= 48
80035294 02 00 82 30   andi  v0,a0,0x2

LAB_80035298:
80035298 11 00 40 10   beq   v0,zero,LAB_800352e0
8003529c 21 30 00 00   _clear a2                ;   (a2=0 für FUN_8001f314)
800352a4 5d ca 63 90   lbu   v1,=>DAT_800aca5d
800352ac ff ff 63 24   addiu v1,v1,-0x1
800352b0 80 10 03 00   sll   v0,v1,0x2
800352b4 21 10 43 00   addu  v0,v0,v1
800352bc 91 40 21 24   addiu at,at,0x4091
800352c4 00 00 22 90   lbu   v0,[0x80074091+idx] ; byte1 = 48
800352cc be ca 63 94   lhu   v1,=>DAT_800acabe
800352d4 21 10 43 00   addu  v0,v0,v1
800352dc be ca 22 a4   sh    v0,=>DAT_800acabe  ; Yaw += 48

LAB_800352e0:
800352e4 c4 cb 84 8c   lw    a0,=>DAT_800acbc4
800352ec c8 cb a5 8c   lw    a1,=>DAT_800acbc8
800352f0 c5 7c 00 0c   jal   FUN_8001f314       ; Anim-Tick (a2=0, a3=0x200)
800352f4 00 02 07 34   _ori  a3,zero,0x200
800352f8 21 20 00 00   clear a0                 ; Rückgabe VERWORFEN → Halteclip loopt
800352fc 7e da 00 0c   jal   FUN_800369f8       ;   (f3bc wrappt den Frame-Cursor selbst auf 0)
80035300 21 28 00 00   _clear a1                ; Aufruf (0,0)

LAB_80035304:                                    ; ---- Ausgang ----
80035304 10 00 bf 8f   lw    ra,0x10(sp)
80035308 18 00 bd 27   addiu sp,sp,0x18
8003530c 08 00 e0 03   jr    ra
80035310 00 00 00 00   _nop
```

### 2.2 Pseudo-C

```c
/* Nahkampf-Substate 1: HOLD — Halteschleife mit 3-stufiger Elevation. */
void melee_sub1_hold(void)                       /* 0x800350C4 */
{
    if (DAT_800aca5b == 0) {                     /* INIT @0x800350D4-DC */
        u16 aim = DAT_800acaec;
        DAT_800acae3 = 7;                        /* @0x800350E0/F0 */
        DAT_800aca5b = 1;                        /* @0x800350F4/F8 */
        DAT_800acae9 = 0;                        /* @0x80035100 */
        DAT_800acae8 = 8 + ((aim >> 15) & 1) * 2 + ((aim >> 11) & 4);  /* 8/10/12 @0x80035104-20 */
    }
    u32 pad = DAT_800ac768;
    if (!(pad & 0x100)) {                        /* R1 losgelassen @0x80035130-34 */
        *(u16 *)&DAT_800aca5a = 3;               /* → Substate 3 (LOWER) @0x8003513C-44 */
        return;                                  /* ohne Anim-Tick @0x80035148 */
    }
    if ((pad & 0x10) && !(DAT_800acaec & 0x8000)) {          /* hoch @0x80035150/64 */
        DAT_800aca5b = 0;                                    /* @0x80035174 */
        DAT_800acaec = (DAT_800acaec & 0x1FFF) | 0x8000;     /* @0x8003516C/70/7C */
        return;
    }
    if ((pad & 0x20) && !(DAT_800acaec & 0x2000)) {          /* tief @0x80035194/AC */
        DAT_800aca5b = 0;                                    /* @0x800351C0 */
        DAT_800acaec = (DAT_800acaec & 0x1FFF) | 0x2000;     /* @0x800351B4/B8/C8 */
        return;
    }
    if (!(pad & 0x30) && !(DAT_800acaec & 0x4000)) {         /* neutral @0x800351E0/F8 */
        DAT_800acaec = (DAT_800acaec & 0x1FFF) | 0x4000;     /* @0x80035200/04/0C */
        DAT_800aca5b = 0;                                    /* @0x80035214 */
        return;
    }
    if (pad & 0x40) {                            /* Feuer @0x8003522C-30 */
        *(u16 *)&DAT_800aca5a = 2;               /* → Substate 2 (SLASH) @0x80035234-3C */
        return;
    }
    if (pad & 0x8)                               /* @0x80035248-4C */
        DAT_800acabe -= TBL_74090[(DAT_800aca5d - 1) * 5 + 1];   /* -48 @0x80035278-90 */
    if (pad & 0x2)                               /* @0x80035294-98 */
        DAT_800acabe += TBL_74090[(DAT_800aca5d - 1) * 5 + 1];   /* +48 @0x800352C4-DC */
    (void)FUN_8001f314(DAT_800acbc4, DAT_800acbc8, 0, 0x200);    /* @0x800352E0-F4 */
    FUN_800369f8(0, 0);                          /* @0x800352F8-300 */
}
```

**Prioritätskette** (byte-true Reihenfolge im Code): R1-Release → Elevation hoch →
Elevation tief → Elevation neutral → Feuer → manuelles Drehen + Anim-Tick. Jeder
Zustandswechsel ist ein **Früh-Exit ohne Anim-Tick in diesem Frame** (alle springen auf
LAB_80035304 HINTER die f314/369f8-Aufrufe). Ein Feuerdruck im selben Frame wie ein
Elevationswechsel verliert also: erst der Wechsel (Phase-Reinit), Feuer greift frühestens
im Folgeframe.

### 2.3 Konstanten (Sub 6)

| Adresse | Wert | Bedeutung |
|---|---|---|
| @0x800350E0/F0 | 7 | Anim-Untertakt-Init (DAT_800acae3) |
| @0x80035100 | 0 | Frame-Cursor-Reset (DAT_800acae9) |
| @0x80035104-20 | 8 + 2·Bit15 + 4·Bit13 | Halteclip 8 (neutral) / 10 (hoch) / 12 (tief) |
| @0x80035130 | Bit 0x100 | DAT_800ac768: R1/Aim gehalten |
| @0x8003513C/44 | sh 3 | Substate := 3 (LOWER) + Phase := 0 |
| @0x80035138/50 | Bit 0x10 | Elevation-hoch-Taste |
| @0x80035194 | Bit 0x20 | Elevation-tief-Taste |
| @0x8003516C/70 | &0x1FFF, \|0x8000 | Aim-Wort := HOCH |
| @0x800351B4/B8 | &0x1FFF, \|0x2000 | Aim-Wort := TIEF |
| @0x80035200/04 | &0x1FFF, \|0x4000 | Aim-Wort := NEUTRAL |
| @0x8003522C | Bit 0x40 | Feuer/Slash-Taste |
| @0x80035234/3C | sh 2 | Substate := 2 (SLASH) + Phase := 0 |
| @0x80035270/78 | byte1 @0x80074091 = 0x30 = 48 | manuelle Drehrate im HOLD |
| @0x800352F4 | 0x200 | FUN_8001f314 param_4 |

---

## 3. Datentabelle @0x80074090 (aus PSX.EXE gedumpt, Dateioffset 0x64890)

5-Byte-Record je Waffen-Id, Index = `DAT_800aca5d − 1`, gültig für Ids 1..20
(0x80074090–0x800740F3; ab 0x800740F4 folgen Code-Pointer 0x80032F18/0x80033180/0x80033460,
ab 0x80074100 die Entlade-Tabelle):

```
id= 1: 18 30 00 01 01      id= 8: 18 30 0a 00 01      id=15: 00 00 00 00 00
id= 2: 00 00 00 00 00      id= 9: 18 30 0a 01 00      id=16: 00 00 00 00 00
id= 3: 18 30 07 01 01      id=10: 18 30 0a 01 00      id=17: 00 00 00 00 00
id= 4: 18 30 07 01 01      id=11: 18 30 0a 01 00      id=18: 00 00 00 00 00
id= 5: 18 30 0a 01 01      id=12: 18 30 0a 01 01      id=19: 18 30 0a 01 01
id= 6: 18 30 0a 01 01      id=13: 18 30 0a 01 01      id=20: 00 00 00 00 00
id= 7: 18 30 0a 01 01      id=14: 00 00 00 00 00
```

- byte0 (@0x80074090+…): Drehrate RAISE (Sub5 @0x80035000) = 24.
- byte1 (@0x80074091+…): Drehrate HOLD (Sub6 @0x80035278) = 48.
- byte2: von anderen Subs konsumiert (Recoil-Break-Schwelle, vgl. @0x8003364C; Port
  player_common.c "recoil_break" — NICHT Teil von Sub5/6).
- Achtung: Waffen-Id 0 läse Index −1 = Bytes @0x8007408B–8F (außerhalb der Tabelle) —
  nur relevant, falls Bits 0x8/0x2 bei Id 0 im Aim gedrückt würden.

---

## 4. Hilfsfunktionen (Belege)

- **FUN_8003703c(radius)** (`RE_15_Quellcode_V2/FUN_8003703c.c`): iteriert die
  Entity-Mirror-Slots ab &DAT_800acc2c (Stride 0x1F4, Anzahl DAT_800aca4e, Aktiv-Bit
  `slot[0]&1`), 2D-Distanz `SquareRoot0((ex−DAT_800aca88)² + (ez−DAT_800aca90)²)` mit
  Entity-x @+0x34 / z @+0x3C, klassifiziert per Statuskurzwort @slot+0x9A und latcht den
  nächstliegenden Slot in **DAT_800acbfc**. Rückgabe 1 = Klasse A gefunden, 2 = nur Klasse B,
  0 = nichts (dann DAT_800acbfc = &DAT_800acc2c). Sub5 konsumiert nur **Bit 0** (@0x80034FA8).
- **FUN_8001a8f8(pos, rate)** (`FUN_8001a8f8.c`; RE15_FUN_CATALOG.md Z.46, high):
  Peilung Spieler→pos via FUN_8001a6d4(spieler.x @ent+0x34, spieler.z @ent+0x3C, pos[0],
  pos[4] = +8 Bytes = z), dann Spieler-Yaw @DAT_800ac784+0x6A pro Frame um ±rate nachführen
  (Snap, wenn Restwinkel < 2·rate). Sub5-Aufruf: pos = Ziel-Mirror+0x34, rate = 0xC0.
- **FUN_8001f314(bank1, edd, rev, p4)** (`FUN_8001f314.c`; Katalog Z.37, high): Keyframe-
  Advance des SPIELERS — Clip = ent[DAT_800ac784+0x94], Frame = ent+0x95, Keyframe-Zeiger →
  ent+0x168; Keyframe-Flag 0x8000 → Interp-Pfad FUN_8001f8b4(p4), sonst FUN_8001f3bc.
  **Rückgabe = bool von FUN_8001f3bc** (`FUN_8001f3bc.c:89-95`): `ent+0x95 += 1`, TRUE
  exakt beim Erreichen der Clip-Framezahl, Cursor wrappt dabei auf 0. Sub5 addiert die
  Rückgabe auf die Phase (@0x8003508C) = "Clip fertig → Phase 2"; Sub6 verwirft sie
  (Halteclip loopt durch den Wrap).
- **FUN_800369f8(0,0)**: Anim-Bank-Setter (bekannter Kontext; hier stets (0,0) statt der
  (0,1) des Dauerfeuer-Masters 0x80034014).

## 5. Globals in Sub 5/6

| Global | Rolle (Beleg) |
|---|---|
| DAT_800aca5a | FSM-Substate (Dispatch @0x80034EB0; Halfword-Stores @0x800350A4/0x8003513C/0x80035234 setzen Substate UND löschen…) |
| DAT_800aca5b | …die Phase im Substate (High-Byte desselben Halfwords, little-endian) |
| DAT_800aca5d | angelegte Waffen-Id (Tabellenindex −1, @0x80034FE0/0x80035258) |
| DAT_800aca3c | Spieler-Slot-Flagwort: \|=0xC0 beim Raise (@0x80034F90), &=~0xC0 beim LOWER-Ende (@0x80034D28/48: li −0x81/−0x41) |
| DAT_800aca54 | Statuswort, Bit 0x4000 = Messer in der Hand (Dispatch @0x80034E9C; Port s_knife_in_hand) |
| DAT_800ac768 | Action-/Pad-Wort: 0x100=R1, 0x10=hoch, 0x20=tief, 0x40=Feuer, 0x8/0x2=drehen (∓Yaw) |
| DAT_800acabe | Spieler-Yaw Q12 (Port rot_y; Nachbarblock zu aca88=x/aca90=z, s. FUN_8003703c) |
| DAT_800acae8/ae9 | Clip / In-Clip-Frame-Cursor (an ent+0x94/+0x95 gespiegelt, s. FUN_8001f314) |
| DAT_800acae3 | Anim-Untertakt (Init 7; Port anim_frac) |
| DAT_800acae0 | Vorwärtsgeschwindigkeit (0 = verwurzelt; Treppen-Handler setzt 0xA, Katalog LAB_80038c60) |
| DAT_800acaec | Aim-Wort: Bit15=hoch, Bit13=tief, Bit14=neutral, Bits 0..12 unangetastet (&0x1FFF) |
| DAT_800acaf3 | Latch-Ergebnis von FUN_8003703c (Bit0 ⇒ Auto-Slew aktiv) |
| DAT_800acbfc | gelatchter Ziel-Mirror-Zeiger (FUN_8003703c → FUN_8001a8f8) |
| DAT_800acbc4/bc8 | Zeiger der aktiven Waffen-Animbank (bc8 = EDD-Clip-Tabelle, s. FUN_8001f314-Indexrechnung) |

## 6. Port-Abgleich (re15_port/engine/src/player_common.c)

Bereits byte-true umgesetzt und mit denselben Adressen kommentiert: RAISE-Clip 6 (Z.755),
Halteclips 8/10/12 (@0x80035164/1B8/204, Z.766-780), Drehraten 24/48 aus 0x80074090/91
(Z.695-710), Latch 5000 + Slew 0xC0 (Z.713-718), LOWER via !R1 @0x8003513C (Z.658-663).
NICHT im Port abgebildet (Kandidaten für Feinschliff): die Früh-Exits OHNE Anim-Tick im
Wechselframe und die Prioritätsreihenfolge Elevationswechsel-vor-Feuer aus §2.2.

## 7. Offen (auch mit RE1.5-Mitteln hier nicht abschließend geklärt)

- Zuordnung der Drehbits 0x8/0x2 in DAT_800ac768 zu links/rechts am Controller: aus Sub5/6
  ist nur das Vorzeichen am Yaw belegt (0x8 → acabe−=step @0x80035010, 0x2 → += @0x8003505C);
  die Tastenzuordnung hängt an der Pad-Remap-Stelle, die hier nicht verfolgt wurde.
- Klassen-Bits des Statuskurzworts @Mirror+0x9A in FUN_8003703c (Ghidra-Decompile zeigt
  eine kaputte 64-Bit-Maske `0x6000000000000000`): welche Klasse Rückgabe 1 vs. 2 genau
  bedeutet (lebendig/tot?), ist nicht verifiziert — Sub5 konsumiert ohnehin nur Bit 0.
- Semantik von param_4=0x200 an FUN_8001f314 (nur im Interp-Pfad FUN_8001f8b4 konsumiert).
- Wer außer Sub5/Dauerfeuer-Sub0 DAT_800acaf3-Bit1 (Rückgabe 2 des Latch) liest, wurde
  nicht verfolgt.
