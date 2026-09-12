# Dauerfeuer-FSM Sub 1 — AIM-HOLD/READY (0x80034278–0x8003450C)

Gruppe: fsm-sub1. Quelle: `ghidra1_V2.txt` Zeilen 129731–129911 (voller Bereich extrahiert,
bis `jr ra` + Delay-Slot @0x8003450c). Tabellen aus `info/Re1.5/PSX.EXE` gedumpt
(t_addr=0x80010000 @Dateioffset 0x18; Dateioffset(va)=0x800+(va−0x80010000)).

## Einordnung

- Erreicht NUR über die Sub-Tabelle @0x80074150[1] (einziger Xref: `80074154(*)`,
  ghidra1_V2.txt:129731/242249) des Dauerfeuer-Handlers 0x80034014.
- 0x80034014 wird vom Waffen-Dispatch @0x80074030 ausschließlich für die Waffen-Ids
  **0x0C (Ingram M10), 0x0E (Flammenwerfer), 0x13 (MC51)** angesprungen
  (PSX.EXE-Dump dieser Session: [12]=0x80034014, [14]=0x80034014, [19]=0x80034014,
  alle übrigen 3..18 = 0x80032e9c, 0..2 = 0x80034e70, 20 = NULL).
- Sub-Index = `lbu DAT_800aca5a` (im Dispatcher 0x80034014). Sub 1 = dieser Zustand:
  Waffe ist angelegt, wartet auf Abzug/Elevationswechsel/R1-Release.
- Spieler-Entity liegt statisch @0x800aca54 (DAT_800ac784 zeigt darauf):
  +0x06=DAT_800aca5a Substate, +0x07=DAT_800aca5b Phase, +0x09=DAT_800aca5d Waffen-Id,
  +0x34=DAT_800aca88 Welt-X, +0x6a=DAT_800acabe Yaw, +0x94=DAT_800acae8 Clip,
  +0x95=DAT_800acae9 Clip-Cursor, +0x8f=DAT_800acae3 Crossfade-Frac,
  +0x98=DAT_800acaec Aim-Wort. (Feld-Zuordnungen: climb_common.c:45/47,
  player_common.c:73, menu_common.c:1745, RE15_FUN_CATALOG.md Zeile 91.)
- Pad-Wörter: DAT_800ac768 = VIRTUELLES Held-Wort, DAT_800ac76c = VIRTUELLES
  Edge-Wort (Builder FUN_80030444 @0x800304b8-e4, Preset-0-Tabelle @0x80073dbc).
  Bit-Bedeutung in dieser Funktion (Preset 0): 0x02=RIGHT, 0x08=LEFT, 0x10=UP,
  0x20=DOWN, 0x40=SQUARE, 0x100=R1 (pad_common.c:10-44).

## Roh-Listing mit Kommentaren

```
; ---- Prolog -------------------------------------------------------------------
80034278  addiu  sp,sp,-0x18
8003427c  lui    a1,0x800b
80034280  addiu  a1,a1,-0x35a5          ; a1 = &DAT_800aca5b (Phase-Byte, Entity+0x07)
80034284  sw     ra,0x14(sp)
80034288  sw     s0,0x10(sp)

; ---- Phase-Init (nur wenn DAT_800aca5b == 0) ------------------------------------
8003428c  lbu    v0,0x0(a1)             ; Phase
80034294  bne    v0,zero,LAB_800342ec   ; schon initialisiert -> Eingabe-Teil
80034298  _ori   v0,zero,0x7            ; (Delay) v0 = 7
8003429c  lui    a0,0x800b
800342a0  lhu    a0,DAT_800acaec        ; Aim-Wort (Bits 15/14/13 = hoch/mitte/tief)
800342a8  sb     v0,DAT_800acae3        ; Crossfade-Frac (+0x8f) = 7  <<< 7-Frame-Blend
800342ac  ori    v0,zero,0x1
800342b0  sb     v0,0x0(a1)             ; Phase = 1 (initialisiert)
800342b8  sb     zero,DAT_800acae9      ; Clip-Cursor (+0x95) = 0
800342bc  srl    v0,a0,0xf              ; b15 = hoch (0/1)
800342c0  sll    v1,v0,0x1
800342c4  addu   v1,v1,v0               ; 3*b15
800342c8  addiu  v1,v1,0x9              ; 9 + 3*b15
800342cc  andi   a0,a0,0x2000
800342d0  sltu   a0,zero,a0             ; b13 = tief (0/1)
800342d4  sll    v0,a0,0x1
800342d8  addu   v0,v0,a0               ; 3*b13
800342dc  sll    v0,v0,0x1              ; 6*b13
800342e0  addu   v1,v1,v0               ; 9 + 3*b15 + 6*b13
800342e8  sb     v1,DAT_800acae8        ; HOLD-CLIP: 9=Mitte, 12=hoch, 15=tief

; ---- R1 losgelassen? -> Sub 3 (Waffe senken) ------------------------------------
LAB_800342ec:
800342ec  lui    v1,0x800b
800342f0  lw     v1,DAT_800ac768        ; virtuelles HELD-Wort
800342f8  andi   v0,v1,0x100            ; 0x100 = R1 (Preset @0x80073dbc[8])
800342fc  bne    v0,zero,LAB_80034318   ; R1 noch gehalten -> weiter
80034300  _andi  v0,v1,0x10             ; (Delay) v0 = held & UP
80034304  ori    v0,zero,0x3
8003430c  sh     v0,DAT_800aca5a        ; Substate = 3 (0x80074150[3] = 0x80034c74)
                                        ; sh = HALBWORT: löscht zugleich Phase aca5b!
80034310  j      LAB_800344fc           ; -> Epilog (kein Anim-Advance dieses Bild)

; ---- UP gehalten -> Aim HOCH (Bit 15) -------------------------------------------
LAB_80034318:
80034318  beq    v0,zero,LAB_80034350   ; UP nicht gehalten -> DOWN-Test
80034320  lui    v1,0x800b
80034324  lhu    v1,DAT_800acaec
8003432c  andi   v0,v1,0x8000
80034330  bne    v0,zero,LAB_80034350   ; schon hoch -> weiter (kein Re-Init)
80034334  _andi  v0,v1,0x1fff           ; (Delay) untere 13 Bits erhalten
80034338  ori    v0,v0,0x8000
8003433c  sb     zero,0x0(a1)           ; Phase = 0 -> Re-Init nächstes Bild (Clip 12)
80034344  sh     v0,DAT_800acaec        ; Aim-Wort = (x&0x1fff)|0x8000
80034348  j      LAB_800344fc           ; EXIT (kein Feuer/Drehen/Advance dieses Bild)

; ---- DOWN gehalten -> Aim TIEF (Bit 13) -----------------------------------------
LAB_80034350:
80034350  lui    v0,0x800b
80034354  lw     v0,DAT_800ac768
8003435c  andi   v0,v0,0x20             ; 0x20 = DOWN
80034360  beq    v0,zero,LAB_8003439c
80034368  lui    v1,0x800b
8003436c  lhu    v1,DAT_800acaec
80034374  andi   v0,v1,0x2000
80034378  bne    v0,zero,LAB_8003439c   ; schon tief -> weiter
8003437c  _andi  v0,v1,0x1fff
80034380  ori    v0,v0,0x2000
80034388  sb     zero,DAT_800aca5b      ; Phase = 0 -> Re-Init (Clip 15)
80034390  sh     v0,DAT_800acaec        ; Aim-Wort = (x&0x1fff)|0x2000
80034394  j      LAB_800344fc           ; EXIT

; ---- weder UP noch DOWN -> Aim MITTE (Bit 14) -----------------------------------
LAB_8003439c:
8003439c  lui    v0,0x800b
800343a0  lw     v0,DAT_800ac768
800343a8  andi   v0,v0,0x30             ; UP|DOWN
800343ac  bne    v0,zero,LAB_800343e8   ; eins von beiden gehalten -> Feuer-Test
800343b4  lui    v1,0x800b
800343b8  lhu    v1,DAT_800acaec
800343c0  andi   v0,v1,0x4000
800343c4  bne    v0,zero,LAB_800343e8   ; schon Mitte -> weiter
800343c8  _andi  v0,v1,0x1fff
800343cc  ori    v0,v0,0x4000
800343d4  sh     v0,DAT_800acaec        ; Aim-Wort = (x&0x1fff)|0x4000
800343dc  sb     zero,DAT_800aca5b      ; Phase = 0 -> Re-Init (Clip 9)
800343e0  j      LAB_800344fc           ; EXIT

; ---- SQUARE gehalten -> Abzug ---------------------------------------------------
LAB_800343e8:
800343e8  lui    s0,0x800b
800343ec  addiu  s0,s0,-0x3898          ; s0 = &DAT_800ac768
800343f0  lw     v0,0x0(s0)
800343f8  andi   v0,v0,0x40             ; 0x40 = SQUARE (Feuer), HELD — kein Edge
800343fc  beq    v0,zero,LAB_80034490   ; nicht gedrückt -> Dreh-Teil
80034404  jal    FUN_8004ea6c           ; Magazin-Check (Slot-Byte DAT_800b10ad+slot*4
                                        ;  != 0; Slot 0x80 = keine Waffe -> false)
8003440c  beq    v0,zero,LAB_80034434   ; leer -> Leer-Zweig
80034410  _ori   v0,zero,0x2            ; (Delay) v0 = 2
80034418  sh     v0,DAT_800aca5a        ; Substate = 2 = FEUER (0x80074150[2]=0x80034510)
                                        ; sh löscht zugleich Phase aca5b -> Sub 2 startet Phase 0
80034420  sb     zero,DAT_800acae9      ; Clip-Cursor = 0
80034428  sb     zero,DAT_800acaf2      ; Feuertakt-Zähler = 0
8003442c  j      LAB_800344fc           ; EXIT

; ---- Magazin leer: nur SQUARE-EDGE und NUR Waffen-Id 0x12 (toter Zweig, s.u.) ----
LAB_80034434:
80034434  lui    v0,0x800b
80034438  lw     v0,DAT_800ac76c        ; virtuelles EDGE-Wort
80034440  andi   v0,v0,0x40             ; SQUARE neu gedrückt?
80034444  beq    v0,zero,LAB_80034490   ; nein -> nichts (gehalten allein tut NICHTS)
80034448  _ori   v0,zero,0x12           ; (Delay) v0 = 0x12
8003444c  lui    v1,0x800b
80034450  lbu    v1,DAT_800aca5d        ; angelegte Waffen-Id
80034458  bne    v1,v0,LAB_80034490     ; != 0x12 -> nichts (für 0x0C/0x0E/0x13 IMMER)
80034460  jal    FUN_8004eb70           ; Reserve-Munition vorhanden? (Item-Tabelle
                                        ;  PTR_DAT_80074dac[id*3] -> FUN_8004dfec Zählung)
80034468  beq    v0,zero,LAB_80034480   ; keine Reserve -> Leerklick
8003446c  _ori   v0,zero,0x4            ; (Delay) v0 = 4
80034470  sh     v0,DAT_800aca5a        ; Substate = 4 = RELOAD (0x80074150[4]=0x80034d68)
80034478  j      LAB_80034490
LAB_80034480:
80034480  lui    a0,0x101
80034484  ori    a0,a0,0x1              ; a0 = 0x01010001 = Leerklick-SE
                                        ;  (Pack (bank<<24)|(vol<<16)|id, Se_on-Format
                                        ;   LAB_80041624, RE15_FUN_CATALOG.md:185)
80034488  jal    FUN_80045024           ; SE abspielen
8003448c  _addiu a1,s0,0x320            ; a1 = 0x800aca88 = &Spieler-Position (X/Y/Z)
                                        ;  für die Distanz-Lautstärke (FUN_80045a64)

; ---- Drehen beim Zielen: LEFT/RIGHT ±0x30 auf den Yaw-Akku ----------------------
LAB_80034490:
80034490  lui    v1,0x800b
80034494  lw     v1,DAT_800ac768        ; HELD-Wort
8003449c  andi   v0,v1,0x8              ; 0x08 = LEFT
800344a0  beq    v0,zero,LAB_800344c4
800344a4  _andi  v0,v1,0x2              ; (Delay) v0 = held & RIGHT
800344a8  lui    v0,0x800b
800344ac  lhu    v0,DAT_800acabe        ; Yaw-Akku (0..4095 = 360°)
800344b4  addiu  v0,v0,-0x30            ; -48/4096-Umdrehung pro 30-Hz-Bild
800344bc  sh     v0,DAT_800acabe
800344c0  andi   v0,v1,0x2              ; RIGHT neu berechnen
LAB_800344c4:
800344c4  beq    v0,zero,LAB_800344e4
800344c8  _clear a2                     ; (Delay, läuft IMMER) a2 = 0 für f314-Call
800344cc  lui    v0,0x800b
800344d0  lhu    v0,DAT_800acabe
800344d8  addiu  v0,v0,0x30             ; +48
800344dc  lui    at,0x800b
800344e0  sh     v0,DAT_800acabe

; ---- Hold-Pose weiterschalten (Waffenbank-Clip) ---------------------------------
LAB_800344e4:
800344e4  lui    a0,0x800b
800344e8  lw     a0,DAT_800acbc4        ; PLW-Bank EMR (Keyframes; Schreiber
                                        ;  FUN_80036b68 @0x80036c04, dir[1])
800344ec  lui    a1,0x800b
800344f0  lw     a1,DAT_800acbc8        ; PLW-Bank EDD (Clip-Tabelle; @0x80036be4, dir[0])
800344f4  jal    FUN_8001f314           ; Anim-Advance: Clip aus Entity+0x94(=acae8),
                                        ;  Cursor +0x95(=acae9)+1, Crossfade +0x8f-Decay
800344f8  _ori   a3,zero,0x200          ; a2=0 (vorwärts), a3=0x200 (Flags an Interp-Pfad)

; ---- Epilog ---------------------------------------------------------------------
LAB_800344fc:
800344fc  lw     ra,0x14(sp)
80034500  lw     s0,0x10(sp)
80034504  addiu  sp,sp,0x18
80034508  jr     ra
8003450c  _nop
```

## Pseudo-C

```c
/* Dauerfeuer-FSM Sub 1 — AIM-HOLD (Tabelle @0x80074150[1]).
   Läuft jedes 30-Hz-Bild solange DAT_800aca5a == 1. */
void gun_auto_sub1_aim_hold(void)                          /* 0x80034278 */
{
    if (DAT_800aca5b == 0) {                               /* @0x8003428c-94: Phase-Init */
        u16 aim = DAT_800acaec;                            /* @0x800342a0 */
        DAT_800acae3 = 7;                                  /* @0x800342a8  Crossfade 7 Bilder */
        DAT_800aca5b = 1;                                  /* @0x800342b0 */
        DAT_800acae9 = 0;                                  /* @0x800342b8  Clip-Cursor */
        DAT_800acae8 = 9 + 3*((aim >> 15) & 1)             /* @0x800342bc-e8 */
                         + 6*((aim & 0x2000) ? 1 : 0);     /* 9=Mitte, 12=hoch, 15=tief */
    }

    u32 held = DAT_800ac768;                               /* virtuelles Held-Wort */

    if (!(held & 0x100)) {                                 /* @0x800342f8  R1 losgelassen */
        *(u16*)&DAT_800aca5a = 3;                          /* @0x8003430c  -> Sub 3 SENKEN
                                                              (sh nullt auch Phase aca5b) */
        return;                                            /* @0x80034310 */
    }
    if ((held & 0x10) && !(DAT_800acaec & 0x8000)) {       /* @0x80034318/2c  UP, noch nicht hoch */
        DAT_800aca5b = 0;                                  /* @0x8003433c  Re-Init -> Clip 12 */
        DAT_800acaec = (DAT_800acaec & 0x1fff) | 0x8000;   /* @0x80034334-44 */
        return;                                            /* @0x80034348  (Bild ohne Advance) */
    }
    if ((held & 0x20) && !(DAT_800acaec & 0x2000)) {       /* @0x8003435c/74  DOWN, noch nicht tief */
        DAT_800acaec = (DAT_800acaec & 0x1fff) | 0x2000;   /* @0x8003437c-90 */
        DAT_800aca5b = 0;                                  /* @0x80034388  Re-Init -> Clip 15 */
        return;                                            /* @0x80034394 */
    }
    if (!(held & 0x30) && !(DAT_800acaec & 0x4000)) {      /* @0x800343a8/c0  weder/noch, nicht Mitte */
        DAT_800acaec = (DAT_800acaec & 0x1fff) | 0x4000;   /* @0x800343c8-d4 */
        DAT_800aca5b = 0;                                  /* @0x800343dc  Re-Init -> Clip 9 */
        return;                                            /* @0x800343e0 */
    }

    if (held & 0x40) {                                     /* @0x800343f8  SQUARE GEHALTEN */
        if (FUN_8004ea6c()) {                              /* @0x80034404  Magazin > 0 */
            *(u16*)&DAT_800aca5a = 2;                      /* @0x80034418  -> Sub 2 FEUER */
            DAT_800acae9 = 0;                              /* @0x80034420 */
            DAT_800acaf2 = 0;                              /* @0x80034428  Feuertakt-Zähler */
            return;                                        /* @0x8003442c */
        }
        /* Magazin LEER */
        if ((DAT_800ac76c & 0x40)                          /* @0x80034438-44  nur PRESS-EDGE */
            && DAT_800aca5d == 0x12) {                     /* @0x80034448-58  NUR Waffe 0x12
                                                              — hier NIE wahr, s. Befund */
            if (FUN_8004eb70())                            /* @0x80034460  Reserve da? */
                *(u16*)&DAT_800aca5a = 4;                  /* @0x80034470  -> Sub 4 RELOAD */
            else
                FUN_80045024(0x01010001, &player_pos);     /* @0x80034480-8c  Leerklick-SE,
                                                              a1 = 0x800aca88 */
        }
    }

    if (held & 0x8) DAT_800acabe -= 0x30;                  /* @0x8003449c/b4  LEFT:  Yaw −48 */
    if (held & 0x2) DAT_800acabe += 0x30;                  /* @0x800344a4/d8  RIGHT: Yaw +48 */

    FUN_8001f314(DAT_800acbc4 /*EMR*/, DAT_800acbc8 /*EDD*/,
                 0 /*vorwärts*/, 0x200);                   /* @0x800344e4-f8  Hold-Pose-Tick */
}
```

## Konstanten-Tabelle

| Adresse | Wert | Bedeutung |
|---|---|---|
| 0x80034298 | 7 | Crossfade-Frac-Startwert (+0x8f); Decay saturierend in FUN_8001f3bc @0x8001f5a8-b4 |
| 0x800342c8 | 9 | Hold-Clip-Basis (Aim MITTE) der 16-Clip-Dauerfeuer-Bänke W0C/W0E/W13 |
| 0x800342bc-e0 | +3/+6 | Clip-Formel 9+3·b15+6·b13 → 12 = hoch (Bit 15), 15 = tief (Bit 13) |
| 0x800342f8 | 0x100 | Pad-Held R1 (virtuell; Preset 0 @0x80073dbc[8]) — losgelassen ⇒ Sub 3 |
| 0x80034304 | 3 | Ziel-Substate SENKEN = 0x80074150[3] = 0x80034c74 |
| 0x80034300/18 | 0x10 | Pad-Held UP ⇒ Aim-Bit 15 setzen |
| 0x8003435c | 0x20 | Pad-Held DOWN ⇒ Aim-Bit 13 setzen |
| 0x800343a8 | 0x30 | UP\|DOWN-Maske; beide frei ⇒ Aim-Bit 14 (Mitte) |
| 0x80034334/7c/c8 | 0x1fff | Aim-Wort: untere 13 Bits bleiben erhalten, Bits 15/14/13 exklusiv |
| 0x80034338/80/cc | 0x8000/0x2000/0x4000 | Aim hoch/tief/Mitte (RE2-Beleg unten) |
| 0x800343f8 | 0x40 | Pad-Held SQUARE = Abzug (HELD, kein Edge — Dauerfeuer) |
| 0x80034410 | 2 | Ziel-Substate FEUER = 0x80074150[2] = 0x80034510 |
| 0x80034420 | 0 | DAT_800acae9 Clip-Cursor-Reset beim Feuer-Übergang |
| 0x80034428 | 0 | DAT_800acaf2 Feuertakt-Zähler-Reset beim Feuer-Übergang |
| 0x80034440 | 0x40 | SQUARE im EDGE-Wort DAT_800ac76c (Leer-Zweig nur bei Neudruck) |
| 0x80034448 | 0x12 | Waffen-Id-Gate des Leer-Zweigs — TOTER CODE (s. Befund) |
| 0x8003446c | 4 | Ziel-Substate RELOAD = 0x80074150[4] = 0x80034d68 |
| 0x80034480-84 | 0x01010001 | Leerklick-SE (Se_on-Pack; identisch Standard-FSM @0x80033384-90) |
| 0x8003448c | +0x320 | 0x800ac768+0x320 = 0x800aca88 = Spieler-Position für SE-Distanz |
| 0x8003449c | 0x8 | Pad-Held LEFT ⇒ Yaw −0x30 |
| 0x800344a4 | 0x2 | Pad-Held RIGHT ⇒ Yaw +0x30 |
| 0x800344b4/d8 | ±0x30 | Zieldreh-Rate 48/4096-Umdrehung (≈4,2°) pro 30-Hz-Bild |
| 0x800344f8 | 0x200 | Flag-Wort an FUN_8001f314 (durchgereicht an den Interp-Pfad FUN_8001f8b4) |

## Mechanismus-Befunde

1. **`sh` auf DAT_800aca5a nullt die Phase mit.** Alle drei Substate-Übergänge
   (3 @0x8003430c, 2 @0x80034418, 4 @0x80034470) schreiben ein HALBWORT auf
   0x800aca5a — Little-Endian liegt das High-Byte auf 0x800aca5b, dem Phase-Byte.
   Jeder Sub-Wechsel startet den Zielzustand also automatisch in Phase 0 (Init).
   Elevationswechsel INNERHALB von Sub 1 nullen die Phase dagegen explizit per
   `sb zero` (@0x8003433c/@0x80034388/@0x800343dc) und bleiben in Sub 1.

2. **Elevationswechsel kostet ein Bild.** Jeder der drei Aim-Wechsel-Pfade springt
   direkt zum Epilog (@0x80034348/@0x80034394/@0x800343e0) — in diesem Bild läuft
   weder der Feuer-Test noch das Drehen noch der Anim-Advance. Erst das Folgebild
   re-initialisiert (Clip 9/12/15, Cursor 0, Crossfade 7) und tickt wieder.

3. **Feuer ist HELD-getriggert** (DAT_800ac768 & 0x40 @0x800343f8), nicht
   Edge-getriggert — SQUARE gedrückt halten genügt; die Feuerkadenz regelt Sub 2
   über DAT_800acaf2 (hier auf 0 gesetzt @0x80034428).

4. **⛔ Leer-Magazin-Zweig ist für alle drei Dauerfeuer-Waffen TOT.** Der Zweig
   verlangt DAT_800aca5d == 0x12 (@0x80034448-58). Der Dispatch @0x80074030 führt
   Waffe 0x12 (Id 18) aber auf den STANDARD-Handler 0x80032e9c (PSX.EXE-Dump), nie
   auf 0x80034014 — in dieser FSM ist die Id immer 0x0C/0x0E/0x13. Folge im
   Auslieferungsstand: Ingram/Flammenwerfer/MC51 lösen bei leerem Magazin aus
   Sub 1 WEDER Auto-Reload (Sub 4) noch den Leerklick 0x01010001 aus — SQUARE
   drücken tut nichts. Das RE1.5-interne Gegenstück im Standard-FSM prüft
   stattdessen `sltiu v0,id,0x9` (@0x80033368, Id<9 ⇒ Reload, sonst Klick
   @0x80033384-90) — gleiche Struktur (0x80033300-90: held 0x40 → FUN_8004ea6c;
   leer → Edge 0x40 → FUN_8004eb70 → Sub 4/Klick), nur das Id-Gate weicht ab.
   Zu Waffe 0x12 belegbar: Entlade-Handler @0x80074100[18] = NULL,
   Schadensspalte 18 = 400 in allen Typ-Zeilen (z.B. Zombie-Zeile @0x8006e650);
   die Projektgruppe 15..18 ist die Werfer-Klasse (Memory reai-v2-waffen-banken).
   Warum genau 0x12 hier steht (Build-Historie?), ist statisch nicht klärbar.

5. **Hold-Clips der Dauerfeuer-Bänke: 9/12/15** (Mitte/hoch/tief,
   @0x800342bc-e8) — gegenüber 8/10/12 der 14-Clip-Standard-Bänke
   (player_common.c/main.c, RE15_AIM_CLIP_MAX=16 wegen der 16-Clip-Bänke
   W0C/W0E/W13). Der Port-Verdacht „Clips 14/15 = Feuer-Ende/Halten-runter"
   ist damit für 15 WIDERLEGT: **Clip 15 ist der Aim-TIEF-Hold.**

6. **Anim-Advance:** FUN_8001f314(EMR=DAT_800acbc4, EDD=DAT_800acbc8, 0, 0x200)
   @0x800344e4-f8. Die beiden Zeiger werden EXE-weit nur von FUN_80036b68 gesetzt
   (dir[0]→0x800acbc8=EDD @0x80036be4, dir[1]→0x800acbc4=EMR @0x80036c04; Memory
   reai-v2-waffen-banken) — die Hold-Pose läuft also auf der Waffenbank der
   angelegten Waffe. FUN_8001f314 (Decompile RE_15_Quellcode_V2/FUN_8001f314.c)
   indiziert die EDD mit Entity+0x94 (=DAT_800acae8), sampelt am Cursor +0x95
   (=DAT_800acae9), und der Frame-Word-Bit 0x8000 wählt Interp (FUN_8001f8b4,
   bekommt das 0x200) vs. FUN_8001f3bc; f3bc dekrementiert dabei den Crossfade
   +0x8f saturierend (@0x8001f5a8-b4, climb_common.c:291-308).

## RE2-Belege

- **Aim-Wort-Kodierung bestätigt:** RE2-Leon nutzt dieselbe exklusive
  Bit-Trias auf einem Entity-Aim-Halbwort (+0x154):
  `(x & 0x1fff) | 0x4000` @0x80060f8c-90, `| 0x8000` @0x80060fa0-a8,
  `| 0x2000` @0x80060fc8-d0 (ghidra_re2_Leon.txt:189770-189830, dort als
  Auto-Aim-Höhenwahl nach Ziel-Y-Differenz mit Schwellen 0x1387/0x1b57/0xbb7).
- Ein strukturgleiches RE2-Gegenstück des Dauerfeuer-Hold-Subs (Waffen-Id-Gate im
  Leer-Zweig) wurde nicht gefunden — RE2 organisiert die Waffen-FSM anders
  (Kandidaten-Scan über 0x8000/0x4000/0x2000-Trios in RE2_Quellcode_V2: keine
  Gun-FSM darunter; SE-Konstante 0x01010001 in RE2-Decompiles ohne Treffer).
