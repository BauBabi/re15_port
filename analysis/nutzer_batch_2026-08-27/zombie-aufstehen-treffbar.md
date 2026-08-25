# FINDING D — Zombie waehrend der Aufsteh-Animation unverwundbar

STATUS: **TEILWEISE** — fuer den **RE2-Flavor** ist die Divergenz gefunden und exakt lokalisiert.
Fuer den **RE1.5-Flavor** ist das Symptom auf zwei der vier Aufsteh-Pfade **byte-true zum
Original** (das Original sperrt dort ebenfalls, Belege unten); die beiden anderen
RE1.5-Aufsteher sind im Port bereits treffbar.

---

## 1. Symptom (Nutzer-Wortlaut)

> "Bei der Aufsteh-Animation vom Zombie ist er unverwundbar. Im Original kann man da bereits
> anschiessen. Auch bei RE2."

---

## 2. Der ORIGINAL-Trefferfilter — beide Engines, Instruktion fuer Instruktion

### 2.1 RE1.5: FUN_80011f50 @0x80011f50 (PSX.EXE) — der Schuss-Aufloeser

Kandidatenschleife @0x800120d0-0x8001215c (selbst disassembliert):

    800120d0: lw   v0,0(s0)          ; enemy.word0
    800120d8: lw   v1,-13740(v1)     ; player.word0  (0x800ACA54)
    800120e0: and  v0,v0,v1
    800120e4: lui  v1,0xe000
    800120e8: and  v0,v0,v1          ; & 0xE0000000   <-- (A) ELEVATIONS-BAND
    800120ec: beq  v0,zero,0x80012128    ;   kein gemeinsames Band -> VERWORFEN
    800120f4: lw   v0,144(s0)        ; enemy+0x90
    800120fc: and  v0,v0,s4          ; s4 = 0x03000000  (= +0x93 Bit0|Bit1)
    80012100: beq  v0,s4,0x80012124  ;   BEIDE Bits gesetzt -> VERWORFEN        <-- (B)
    80012108: lw   v0,0(s5)          ; s5 = 0x8006E548 + weapon*4 = Waffen-Praedikat
    80012110: jalr v0                ;   Streifen-/Kegel-Test
    80012118: beq  v0,zero,0x80012128    ;   nicht im Streifen -> VERWORFEN     <-- (C)
    80012120: addu s1,s0,zero        ;   -> aktuell bester Kandidat

Nach der Auswahl (LAB_80012370, selbst disassembliert):

    80012370: lbu  v0,147(s1)        ; +0x93
    80012378: andi v0,v0,0x1         ;   Bit1 wird geloescht, Bit0 bleibt
    8001237c: sb   v0,147(s1)
    ...
    800123fc: lbu  v1,147(s1)
    80012404: andi v0,v1,0x1
    80012408: beq  v0,zero,0x80012428   ; Bit0 CLEAR -> SCHADEN                 <-- (D)
    8001240c: ori  v0,v1,0x2            ; Bit0 SET   -> +0x93 |= 2 ...
    80012410: sb   v0,147(s1)
    80012418: jal  0x80011f50           ; ... und REKURSION auf das naechste Ziel

**+0x93 Bit0 == 1 heisst in RE1.5 "dieser Gegner nimmt keinen Schaden"** — und der Rueckweg
setzt Bit1, wodurch Gate (B) ihn beim naechsten Schuss schon in der Schleife verwirft.
Zusaetzlich (A): der Gegner braucht ein Band-Bit, das mit dem Spieler-Band uebereinstimmt.
Das Spieler-Band ist exklusiv EIN Bit (`andi 0x1fff` + `ori 0x4000` @0x80033dcc-d0 = LEVEL;
alle Schreiber von DAT_800ACAEC folgen diesem `andi 0x1fff`/`ori <ein Bit>`-Muster).

Weitere Ablehnungsgruende nach der Auswahl (FUN_80011f50, disassembliert):

    80012168: jal 0x8001b9b4   ; Sicht-/Kollisions-Raycast auf target+0x34 -> !=0 => return 0
    80012178: lbu v0,8(s1)     ; Typ
    80012180: sltiu v0,v0,0x40 ; Typ >= 0x40 (NPCs) => return 0
    80012194..80012370         ; Distanz-/Hoehenkegel (nur wenn dist >= 501)

Es gibt in der gesamten Funktion **kein** Gate auf +0x4/+0x5/+0x6 (State/Substate) und keines
auf eine Animation.

### 2.2 RE2: FUN_800470C0 @0x800470c0 (info/re2leon/PSX.EXE) — der Projektil-Applier

Selbst disassembliert @0x80047124-0x800471a4:

    80047124: lw   v0,0(s0)        ; word0
    8004712c: andi v0,v0,0x1
    80047130: beq  v0,zero,0x8004740c     ; (1) nicht aktiv     -> naechster Kandidat
    80047138: lbu  v0,467(s0)      ; +0x1D3   (GANZES Byte!)
    80047140: bne  v0,zero,0x8004740c     ; (2) +0x1D3 != 0     -> naechster Kandidat
    80047148: lh   v0,342(s0)      ; HP (+0x156)
    80047150: bltz v0,0x8004740c          ; (3) HP < 0          -> naechster Kandidat
    80047158: lhu  v0,270(s0)      ; +0x10E
    80047160: andi v0,v0,0xc000
    80047164: bne  v0,zero,0x8004740c     ; (4) +0x10E & 0xC000 -> naechster Kandidat
    ; --- danach das VERTIKALE FENSTER (kein Band!) ---
    80047170: lh   a0,152(s0)      ; +0x98  Box-Oberkante-Offset
    80047174: lw   v0,60(s0)       ; +0x3C  enemy Y
    8004717c: addu v0,v0,a0
    80047180: addiu v0,v0,100
    80047188: lhu  v1,158(s0)      ; +0x9E  Halbhoehe
    80047190: addu v0,v0,v1
    8004718c: lw   a0,4(s4)        ; Schuss-Y
    80047194: subu v0,v0,a0
    80047198: addiu v1,v1,100
    8004719c: sll  v1,v1,1         ; (Halbhoehe + 100) * 2
    800471a0: sltu v0,v0,v1
    800471a4: beq  v0,zero,0x8004740c

**RE2 kennt KEIN Elevations-Band.** Die vertikale Diskriminierung ist ein *stetiges Fenster*
um die pro Pose umgeschriebene Trefferbox. Auch hier: kein State-/Animations-Gate.

---

## 3. Was das ORIGINAL waehrend der Aufsteh-Animation tut

### 3.1 RE1.5 — vier verschiedene Aufsteher

| # | Pfad | Original +0x93 Bit0 | im Original waehrend des Aufstehens treffbar? |
|---|------|---------------------|---------------------------------------------|
| A | Liege-Spawn-Wecker FUN_80103a58 (@0x8011F9D4[0]; Nibbles 5/6, 7/8, 9/10) | Phase 0 `\|=1` **jeden Tick** @0x80103aac-b8; Clear erst Phase 3 @0x80103b5c-68 | **NEIN** |
| B | Knockdown-Erholung FUN_8010512c (+0x5 = 0x11) | Phase[2] `andi 0xfe` @0x801052e8-f4 (liegend = treffbar), Phase[4] **`ori 0x1`** @0x80105378-84, Clear Phase[6] @0x80105438 | **NEIN** |
| C | Schlaefer FUN_801054f4 (+0x5 = 0x12) -> Standup FUN_80104a50 | Phase[4] `andi 0xfe` @0x801055fc-08 **VOR** dem Standup; FUN_80104a50 schreibt +0x93 nie | **JA** |
| D | Fresser-Wecker FUN_801048e8 -> Standup FUN_80104a50 (Clip 0x29) | beide schreiben +0x93 nie | **JA** |

Belege (roh, selbst disassembliert aus `info/Re1.5/PSX/BIN/STAGE1.BIN`, Load @0x80100000):

    ; A — Liege-Phase 0 (FUN_80103a58)
    80103aac: lbu v0,147(a1)
    80103ab4: ori v0,v0,0x1
    80103ab8: sb  v0,147(a1)          ; +0x93 |= 1  JEDEN Tick
    ; A — Phase 2 = der Aufsteh-Clip (einziger f314-Aufruf der Funktion)
    80103b08: addu a2,zero,zero
    80103b0c: lw   a0,368(a1)  / 80103b10: lw a1,372(a1)
    80103b14: jal  0x8001f314  / 80103b18: ori a3,zero,0x100
    80103b30: addu v1,v1,v0    / 80103b38: sb v1,6(a0)     ; +0x6 += ret
    ; A — Phase 3 (erst hier frei)
    80103b3c: sb  zero,9(a1)          ; +0x9 = 0 (Downed-Bit + Nibble weg)
    80103b5c: lbu v0,147(v1)
    80103b64: andi v0,v0,0xfe
    80103b68: sb  v0,147(v1)

    ; B — Knockdown-Phasentabelle @0x801000c4 (selbst gedumpt):
    ;   [0]=0x8010516c [1]=0x80105234 [2]=0x80105278 [3]=0x801052f8
    ;   [4]=0x8010532c [5]=0x801053c4 [6]=0x80105400
    ; B — Phase[0]: Sturz, Sperre + Downed-Bit
    80105178: lbu v0,147(v1) / 80105180: ori v0,v0,0x1 / 80105184: sb v0,147(v1)
    80105190: ori v0,zero,0xb / 80105194: sb v0,148(v1)      ; Clip 0x0b (Fall)
    80105224: lbu v0,9(v1) / 8010522c: ori v0,v0,0x80 / 80105230: sb v0,9(v1)
    ; B — Phase[2]: Liegen beginnt -> FREIGABE
    801052e8: lbu v0,147(v1)
    801052f0: andi v0,v0,0xfe
    801052f4: sb  v0,147(v1)
    ; B — Phase[4]: AUFSTEHEN beginnt (Clip 0x12) -> WIEDER GESPERRT
    80105334: ori v0,zero,0x12
    80105338: sb  v0,148(v1)          ; +0x94 = Clip 0x12 (Get-up)
    80105348: sb  zero,149(v0)        ; +0x95 = 0
    80105378: lbu v0,147(v1)
    80105380: ori v0,v0,0x1           ; <<<<<< +0x93 |= 1
    80105384: sb  v0,147(v1)
    ; B — Phase[6]: Ende
    8010540c: lbu v0,9(v1) / 80105414: andi v0,v0,0x7f / 80105418: sb v0,9(v1)
    80105424: ori v0,zero,0x201 / 80105428: sw v0,4(v1)
    80105438: lbu v0,147(v1)  ...      ; +0x93 &= 0xfe

    ; C — Schlaefer Phase[4]
    801055d8: ori v0,zero,0xd01
    801055dc: sw  v0,4(v1)            ; -> Standup-Substate 0x0d
    801055fc: lbu v0,147(v1)
    80105604: andi v0,v0,0xfe
    80105608: sb  v0,147(v1)          ; FREIGABE **vor** dem Aufsteh-Clip

    ; D — Standup FUN_80104a50 (@0x8011f890[0x0d]) komplett, KEIN Store nach 147:
    80104aa8: ori v0,zero,0x29 / 80104aac: sb v0,148(v1)   ; Clip 0x29
    80104abc: sb zero,149(v0)                              ; +0x95 = 0
    80104ac8: ori v0,zero,0x7 / 80104ad0: sb v0,143(v1)    ; +0x8f = 7
    80104ae0: jal 0x800453d0 / 80104ae4: ori a0,zero,0x5   ; 1/4 SE 5
    80104afc: jal 0x8001f314 / 80104b00: ori a3,zero,0x200
    80104b24: sw v0(=0x201),4(a0)

Zusaetzlich das BAND: der ACTIVE-Tail des Zombie-Roots stempelt es jeden Tick neu und nimmt
einem `+0x9 & 0x80`-Gegner das LEVEL-Bit (selbst disassembliert):

    80101614: lbu v0,9(a1)
    8010161c: andi v0,v0,0x80
    80101620: beq v0,zero,0x80101640      ; nicht downed -> Block ueberspringen
    80101624/28: v1 = 0xBFFFFFFF
    8010162c: lw v0,0(a1) / 80101634: and v0,v0,v1 / 8010163c: sw v0,0(a1)   ; LEVEL weg
    80101630: ori a0,zero,0x1388
    80101638: jal 0x80012974                                                  ; dist<5000 -> DOWN

Das Downed-Bit setzt der Knockdown in Phase[0] (@0x80105224-30) und loescht es erst Phase[6]
(@0x8010540c-18): **der RE1.5-Aufsteher ist im Original doppelt gesperrt (+0x93 UND Band).**

### 3.2 RE2 — zwei Aufsteher, unterschiedliche Antwort

Selbst disassembliert aus `info/re2leon/COMMON/BIN/EMOVL10_S0.BIN` (Roh-Load @0x80100000):

**EXEC[5] Sturz/Knockdown — der Aufsteher IST treffbar.**

    ; P0 @0x8010328C — Sturz beginnt: Anspruch setzen
    80103304 / 8010330C:  lbu +0x1D3 ; ori 0x80 ; sb 467(s2)
    ; P2 @0x80103404 — AUFSCHLAG: Trefferbox auf LIEGEND umschreiben + Anspruch FREIGEBEN
    80103454: addiu v1,zero,200
    80103458: sh v1,144(s2)          ; +0x90 = 200
    8010345c: sh v1,146(s2)          ; +0x92 = 200
    80103460: addiu v1,zero,-350
    80103464: sh v1,152(s2)          ; +0x98 = -350   (Box-Oberkante)
    80103468: addiu v1,zero,350
    8010346c: sh v1,158(s2)          ; +0x9E = 350    (Halbhoehe)
    80103470: lbu v1,467(s2)
    80103478: sh zero,154(s2)        ; +0x9A = 0
    8010347c: sh zero,156(s2)        ; +0x9C = 0
    80103484: andi v1,v1,0x7f
    80103490: sb  v1,467(s2)         ; <<<<<< +0x1D3 FREI -> der Liegende ist treffbar
    ; P6 @0x80103568 — DER BODEN-AUFSTEHER (Clip 8/9)
    80103568: lb  v0,362(s2)         ; side
    80103574: lbu a0,20(v0)          ; Clip = param[0x44+4+side] = {8,9} = Boden-Aufsteher
    80103580: sb  v1(=7),6(s2)       ; -> P7
    80103588: ori v0,v0,0x10
    8010358c: sh  v0,538(s2)         ; +0x21A |= 0x10  (Boden-Aufsteh-Marker)
    ;   0x80103568-0x80103624 enthaelt KEINEN Store nach 467(s2)
    ; P7 @0x80103628 — Clip laeuft aus (advance a3=256 @0x801036A0-A4)
    ;   0x80103628-0x801036F0 enthaelt KEINEN Store nach 467(s2)
    ; P8 @0x801036F4 — Ausgang
    801036F4/F8: addiu v0,zero,257 ; sw v0,4(s2)       ; -> 0x101 WALK
    80103718/24/28: lbu 467 ; andi 0x7f ; sb 467       ; (idempotent)

=> **Waehrend des RE2-Aufstehens gilt +0x1D3 == 0, HP >= 0, +0x10E & 0xC000 == 0. Alle vier
Gates von FUN_800470C0 sind offen — der Zombie IST treffbar.** Das vertikale Fenster ist dabei
die in P2 gesetzte LIEGE-Box: `(350 + 100) * 2 = 900` Einheiten (@0x8004718c-a0), also
grosszuegig — genau die Beobachtung des Nutzers.

**EXEC[7] Liege-Spawn — der Aufsteher ist NICHT treffbar** (das ist der Zwilling von RE1.5-A):

    ; P0 @0x801037CC
    80103804: lbu v0,467(s0)
    8010380c: ori v0,v0,0x80
    80103814: sb  v0,467(s0)          ; Anspruch gesetzt
    ; P2 @0x80103838 spielt den Boden-Aufsteher (Clip 8/9, `lui v1,0xf; ori v1,v1,0x8`
    ;   @0x80103840-44 -> Wort 0x000F0008) — KEIN Store nach 467
    ; P4 @0x80103900
    80103900: addiu v1,zero,257 ; 8010390c: sw v1,4(s0)      ; -> 0x101
    80103904: lbu v0,467(s0) ; 80103914: andi v0,v0,0x7f ; 80103918: sb v0,467(s0)

**EXEC[8] Fresser-Aufsteher (Clip 0x15) — treffbar**: P3 @0x80103CD8 loescht +0x1D3
(`andi 0x7f` @0x80103CE4-FC) und setzt gleichzeitig `+0x21A |= 0x10` (@0x80103D00-10).

---

## 4. Was der PORT heute tut

### 4.1 Der Kandidatenfilter (fuer BEIDE Flavors derselbe Code)

`re15_port/engine/src/re15_damage.c:1206-1290` (`re15_player_weapon_fire`):

    if ((e->hit_react & 0x3) == 0x3) continue;              /* == Gate (B) @0x800120f4-100 */
    ...
    if (re15_band_is_zombie_family(e->type)) {
        eband = re15_band_stamp_aa4(pl, e, bdist, 0xbb8);   /* :1256 */
        {   int lying = (e->grid_id & 0x80) ||              /* :1267 */
                        (re15_ai_re2_for_type(e->type) &&
                         re15_re2z_owns_type(e->type) &&
                         (e->re2z_flags21a & 0x2u));        /* :1269-70 */
            if (lying) {
                eband &= ~0x40000000u;                      /* :1272  LEVEL weg */
                if (bdist < 0x1388u) eband |= 0x20000000u;  /* :1273  DOWN nur < 5000 */
            }
        }
    }
    ...
    if ((pband & eband) == 0) continue;                     /* :1290 */

`pband` ist genau EIN Bit (`player_common.c:191` `re15_player_aim_elevation`), beim normalen
Zielen `0x40000000` = LEVEL. Ist der Gegner als `lying` klassifiziert, hat sein Band **kein**
LEVEL -> `continue` -> **er ist kein Kandidat**, unabhaengig von allem anderen.

### 4.2 RE1.5-Flavor — deckungsgleich mit dem Original

* `enemy_ai_common.c:79`   (Liege-Phase 0)        `e->hit_react |= 1;`      == @0x80103aac-b8
* `enemy_ai_common.c:104`  (Liege-Phase 3)        `hit_react &= ~1`         == @0x80103b5c-68
* `enemy_ai_common.c:2550` (Knockdown Phase[2])   `hit_react &= ~1`         == @0x801052e8-f4
* `enemy_ai_common.c:2561` (Knockdown Phase[4] = **AUFSTEHEN**, Fn @:2516) `hit_react |= 1;` == @0x80105378-84
* `enemy_ai_common.c:2655` (Schlaefer[4], Fn @:2639) `hit_react &= ~1`      == @0x801055fc-08
* `enemy_ai_common.c:1756-1789` (`re15_enemy_ai_standup_animate`, Clip 0x29) schreibt
  `hit_react` **nicht** == FUN_80104a50 (kein Store nach 147)

=> Auf allen vier RE1.5-Aufsteh-Pfaden ist der Port byte-true. **Keine RE1.5-Divergenz.**

### 4.3 RE2-Flavor — hier sitzt die Divergenz

* `enemy_ai_re2_zombie.c:7443` `re15_re2z_hit_filter_apply()` bildet die vier Original-Gates
  (@0x80047124-64) 1:1 ab und uebersetzt sie in `hit_react` Bit0. Waehrend EXEC[5] P6/P7 sagt
  dieser Filter korrekt **TREFFBAR** (`re2z_self1d3` wurde in P2 geloescht,
  `enemy_ai_re2_zombie.c:1824`, Zitat @0x80103484-90); er setzt zugleich `hit_react &= ~2`,
  Gate (B) greift also nie.
* ABER `enemy_ai_re2_zombie.c:1760` setzt in EXEC[5] **P0** zusaetzlich `e->grid_id |= 0x80u;`
  — im Kommentar ausdruecklich als **"PORT-MAPPING (Review #18)"** deklariert. Dazu gibt es
  **keinen** RE2-Original-Store; RE2 kennt kein +0x9-Downed-Bit. Geloescht wird es erst in
  **P8** (`enemy_ai_re2_zombie.c:1931`).
* Ausserdem traegt der Zombie ab P0 `re2z_flags21a & 0x2` (`|0x202`), geloescht erst am
  P7-Ende (`enemy_ai_re2_zombie.c:1902`, @0x801036C8-CC).

=> Waehrend des RE2-Aufstehers (EXEC[5] P6 und P7) ist im Port `lying == 1`, das LEVEL-Bit
faellt weg, `pband & eband == 0`, und der Zombie wird in `re15_damage.c:1290` **verworfen**.
Der bereits korrekt berechnete RE2-Trefferfilter kommt nie zum Tragen.

---

## 5. Die Divergenz (praezise)

**RE2-Flavor (BESTAETIGT):** Der Port sperrt den aufstehenden RE2-Zombie ueber ein
**port-erfundenes** Downed-Band (`grid_id |= 0x80` @ `enemy_ai_re2_zombie.c:1760`, Lebensdauer
bis P8 :1931) plus das RE2-Liegebit `+0x21A & 0x2`, obwohl der **einzige** Trefferfilter des
Originals (FUN_800470C0 @0x80047124-64) waehrend genau dieser Phasen offen ist und RE2 gar kein
Elevations-Band besitzt (stattdessen das 900-Einheiten-Fenster @0x8004718c-a0 um die in P2
gesetzte Liege-Box @0x80103454-6C).

**RE1.5-Flavor (NICHT GEFUNDEN):** Fuer den Knockdown-Aufsteher (Clip 0x12) und den
Liege-Spawn-Wecker sperrt das Original selbst — `ori v0,v0,0x1` @0x80105380 bzw. @0x80103ab4.
Die Nutzer-Aussage "im Original kann man da bereits anschiessen" trifft im RE1.5-Code **nur**
auf den Schlaefer- und den Fresser-Aufsteher (beide Clip 0x29) zu, und die sind im Port bereits
treffbar.

---

## 6. Fix-Rezept fuer den Implementierer

### FIX 1 (RE2-Flavor) — `re15_port/engine/src/re15_damage.c:1267-1270`

Die `lying`-Klassifikation darf fuer RE2-eigene Zombies waehrend des **Boden-Aufstehers** nicht
mehr greifen. RE2 markiert diesen Zustand selbst eindeutig mit `+0x21A & 0x10`: gesetzt exakt
am Beginn der beiden Boden-Aufsteher — EXEC[5] P6 `ori v0,v0,0x10 / sh v0,538(s2)`
@0x80103588-8C und EXEC[8] P3 `ori 0x10` @0x80103D00-10 — und geloescht an deren Ende
(@0x801036B8-BC bzw. @0x80103D84-8C). EXEC[7] (Liege-Spawn) setzt es NIE.

    /* RE2-Boden-Aufsteher: der Original-Filter FUN_800470C0 (@0x80047124-64) hat KEIN
     * Band-Gate, und sein einziges Anspruchs-Byte +0x1D3 ist seit EXEC[5] P2
     * (`andi 0x7f; sb 467` @0x80103484-90) frei und wird von P6/P7
     * (0x80103568-0x801036F0: kein Store nach 467) NICHT neu belegt. Das hier abgefragte
     * grid_id-Bit ist ein PORT-MAPPING (enemy_ai_re2_zombie.c:1760, "Review #18"), kein
     * Original-Store — es darf den Aufsteher nicht sperren. Marker "Boden-Aufsteher laeuft"
     * = +0x21A & 0x10 (@0x80103588-8C / @0x80103D00-10). */
    int re2_owned  = re15_ai_re2_for_type(e->type) && re15_re2z_owns_type(e->type);
    int re2_rising = re2_owned && (e->re2z_flags21a & 0x10u);
    int lying = !re2_rising &&
                ((e->grid_id & 0x80) ||
                 (re2_owned && (e->re2z_flags21a & 0x2u)));

Wirkung, jeweils gegen das Original geprueft:

* EXEC[5] P6/P7 (Knockdown-Aufsteher): jetzt treffbar — wie RE2 (@0x80103484-90, kein Re-Claim).
* EXEC[8] P3/P4 (Fresser-Aufsteher, Clip 0x15): jetzt treffbar — RE2 loescht +0x1D3 dort
  ebenfalls (`andi 0x7f` @0x80103CE4-FC).
* EXEC[7] P1/P2/P3 (Liege-Spawn): **unveraendert gesperrt** — 0x10 wird dort nie gesetzt, und
  Gate (2) des portierten Filters haelt weiter (+0x1D3 = 0x80 @0x80103804-14, Clear erst P4
  @0x80103914-18). Byte-true.
* Liegender RE2-Zombie (EXEC[5] P3/P4/P5): **unveraendert** (0x10 dort nicht gesetzt) — keine
  Regression an der bestehenden DOWN-Band-Semantik.
* RE1.5-Flavor: unberuehrt (`re2_owned` ist dort 0).

### FIX 2 (optional, engerer Blast-Radius) — `enemy_ai_re2_zombie.c:1869` (EXEC[5] case 6)

Das port-gemappte Downed-Bit dort freigeben, wo das Original den Aufsteher startet, statt erst
in P8:

    case 6:                                     /* P6 @0x80103568 */
        e->re2z_flags21a |= 0x10u;              /* RISE-MARKER @0x80103588-8C */
        e->grid_id &= (uint8_t)0x7Fu;           /* PORT-MAPPING-Freigabe: Zwilling des
                                                 * Original-Frees +0x1D3 in P2
                                                 * (@0x80103484-90); P6/P7 belegen 467
                                                 * nicht neu */

Diese Variante allein genuegt **nicht**, solange `re2z_flags21a & 0x2` (Clear erst P7-Ende
@0x801036C8-CC) in `re15_damage.c:1269` weiter als `lying` zaehlt. FIX 1 ist die vollstaendige
Loesung; FIX 2 nur zusaetzlich, wenn der Downed-Band-Zustand auch fuer andere Konsumenten
frueher fallen soll.

### KEIN FIX fuer RE1.5

`enemy_ai_common.c:2561` (`e->hit_react |= 1`) ist byte-true zu `ori v0,v0,0x1` @0x80105380 und
darf **nicht** entfernt werden; ebenso `enemy_ai_common.c:79` gegen @0x80103ab4.

---

## 7. Wie man es verifiziert (Sonde/Messung)

Pro Flavor getrennt, Trefferzahl waehrend der Aufsteh-Frames.

**Sonde `probe_re2z_getup_hit` (Skill `re15-room-probe`, deterministischer ctest):**

1. ROOM1140 laden, Zombie (Typ 0x10/0x16) spawnen, Flavor auf RE2 stellen
   (`re15_ai_re2_for_type` liefert 1).
2. `re15_ai_set_state_word(e, 0x501)` = EXEC[5]-Sturz erzwingen; Spieler auf Distanz < 1500;
   `re15_player_set_aim_elevation_for_test(0)` (LEVEL — der Normalfall).
3. Pro Tick protokollieren: `sub_state_1`, `sub_state_2`, `re2z_self1d3`, `re2z_flags21a`,
   `grid_id`, `hit_react` und den Rueckgabewert von `re15_player_weapon_fire(0)`.
4. **Erwartung nach dem Fix:** in allen Ticks mit `sub_state_1 == 5 && sub_state_2 in {6,7}`
   ist `re2z_flags21a & 0x10` gesetzt, `re2z_self1d3 == 0` und `weapon_fire != 0`.
   **Vor dem Fix:** dort 0 Treffer, obwohl `hit_react & 1 == 0` — das ist der Beweis, dass die
   Sperre aus dem Band-Gate `re15_damage.c:1290` kommt und nicht aus dem RE2-Filter.
5. Gegenprobe EXEC[7]: `0x701` erzwingen; in P1..P3 muss `weapon_fire` weiter 0 liefern
   (`re2z_self1d3 == 0x80`).
6. Gegenprobe RE1.5-Flavor: `0x1101` erzwingen; `sub_state_2 == 2/3` -> Treffer,
   `sub_state_2 == 4/5` -> **kein** Treffer (byte-true, darf sich nicht aendern).

**Live (Skill `re15-port-visual-verify`, ffmpeg-gdigrab):** ROOM1140, Zombie mit der Pistole
niederschlagen und waehrend der Aufsteh-Animation weiterschiessen — im RE2-Modus muessen jetzt
Treffer-Reaktion/Blut kommen, im RE1.5-Modus nicht.

---

## 8. Offene Punkte / NICHT GEFUNDEN

1. **RE2s vertikales Trefferfenster ist im Port ueberhaupt nicht modelliert.** Der Port nutzt
   fuer RE2-Gegner den RE1.5-Streifen/Kegel + das 3-Band; RE2 nutzt `(Halbhoehe+100)*2` um
   `enemyY + (+0x98) + 100 + (+0x9E)` (@0x8004716c-a4) mit pro-Pose umgeschriebener Box
   (liegend 200/200/-350/350 @0x80103454-6C). Ein byte-treuer Nachbau braeuchte die Felder
   +0x90/+0x92/+0x98/+0x9A/+0x9C/+0x9E im Aktor und einen Schuss-Y-Produzenten.
   **NICHT portiert, NICHT geraten.**
2. **Deskriptor-Zensus fuer den RE1.5-Fresser/Schlaefer.** Ob ein Fresser mit Deskriptor
   0x85/0x86 (statt 0x05/0x06) das Downed-Band traegt und dadurch im Original waehrend des
   Standups (Clip 0x29) doch band-gesperrt ist, habe ich **nicht** ausgezaehlt. Naechster Weg:
   Byte-Zensus des Spawn-Musters ueber `re15_port/shared_assets/PSX/STAGE1/ROOM*.RDT`.
3. **Vollstaendiger `sb ...,467`-Zensus ueber EMOVL10_S0.BIN** steht aus; geprueft wurden
   EXEC[5], EXEC[7], EXEC[8], EXEC[9]. Ein weiterer, bisher nicht gefundener +0x1D3-Setzer
   koennte einzelne Aufsteh-Frames doch sperren.
4. **Die Nutzer-Aussage zu RE1.5 ist durch die Bytes widerlegt** (@0x80105380 / @0x80103ab4).
   Falls der Nutzer eine konkrete RE1.5-Szene meint, in der er im Original waehrend des
   Aufstehens trifft, ist das entweder der Schlaefer-/Fresser-Aufsteher (Clip 0x29 — im Port
   bereits treffbar) oder eine RE2-Beobachtung. Naechster Weg zur Klaerung: DuckStation-Messung
   (`re15-room-capture` + Savestate-RAM-Lesung von entity+0x93 waehrend Clip 0x12).

---

## 8. Verifikation (unabhaengig nachdisassembliert)

Pruefer: adversariale Zweitmeinung, 2026-08-27. Jede unten genannte Adresse wurde SELBST
disassembliert (`re15_disasm.py` fuer RE1.5 `PSX.EXE` / `STAGE1.BIN`; fuer RE2 ein duenner
Treiber um denselben Disassembler, der `info/re2leon/PSX.EXE` (PS-X-EXE, t_addr 0x80010000,
Text @File 0x800) bzw. `info/re2leon/COMMON/BIN/EMOVL10_S0.BIN` (roh @0x80100000) laedt).

**GESAMTURTEIL: TEILWEISE.** Der Kern des Dossiers — die RE2-Divergenz und die
RE1.5-Nicht-Divergenz — traegt: die behaupteten Adress-Bloecke sind Instruktion fuer Instruktion
bestaetigt, alle Port-Zitate (Datei:Zeile) stimmen woertlich. WIDERLEGT bzw. korrigiert werden
drei Punkte, die weiter unten mit dem tatsaechlichen Disasm stehen:
(a) "RE2-Trefferfilter = **nur VIER** Gates" unterschlaegt einen fuenften, echten
Ablehnungspfad (`jal 0x80041ef8` @0x800471e8 -> Reject nach 0x800473dc);
(b) die Etikettierung "Aufsteh-Clip" fuer FUN_80103a58 Phase 2 ist NICHT belegt (kein
+0x94-Store in der ganzen Funktion);
(c) das Dossier nennt +0x21A Bit 0x10 "Boden-Aufsteh-Marker", der Port-Kommentar an genau der
zitierten Zeile (`enemy_ai_re2_zombie.c:1869`) nennt dasselbe Bit "CRAWL MARKER" — ich habe die
Frage per vollstaendigem Bit-Zensus ueber EMOVL10_S0.BIN zugunsten des Dossiers entschieden
(§8.11), aber der Widerspruch muss beim Einbau mit aufgeloest werden, sonst wandert er
unkommentiert in den Port.

### 8.1 BESTAETIGT — RE1.5 Schuss-Aufloeser FUN_80011f50, Kandidatenschleife

Eigene Disasm (PSX.EXE), 0x800120c0-0x8001215c:

    800120c0: lui  s4,0x300            ; s4 = 0x03000000 (Dossier korrekt)
    800120c8: addiu v0,v0,-6840        ; 0x8006e548 Waffen-Praedikat-Tabelle
    800120cc: addu s5,v1,v0            ; v1 = (weapon&0xff)<<2  (@0x800120a8-ac)
    800120d0: lw   v0,0(s0)
    800120d8: lw   v1,-13740(v1)       ; 0x800aca54 player.word0
    800120e0: and  v0,v0,v1
    800120e4: lui  v1,0xe000
    800120e8: and  v0,v0,v1
    800120ec: beq  v0,zero,0x80012128  ; (A) Band-Reject
    800120f4: lw   v0,144(s0)
    800120fc: and  v0,v0,s4
    80012100: beq  v0,s4,0x80012124    ; (B) +0x93 Bit0+Bit1 -> verworfen
    80012108: lw   v0,0(s5)
    80012110: jalr v0
    80012118: beq  v0,zero,0x80012128  ; (C) Streifen-Reject
    80012120: addu s1,s0,zero          ; bester Kandidat
    80012158: bne  a0,zero,0x800120d0  ; Schleifenrueckkante

Beide Reject-Ziele (0x80012124 / 0x80012128) ueberspringen `addu s1,s0,zero`. Ebenfalls
bestaetigt: `jal 0x8001b9b4` @0x80012168 (bei !=0 -> 0x80012540), `lbu v0,8(s1)` +
`sltiu v0,v0,0x40` @0x80012178-80, und der Kegel erst ab |dy| >= 501 (`slti v0,v0,501`
@0x800121a8/b4, `bne v0,zero,0x80012370` @0x800121b8 = Kegel UEBERSPRINGEN).
**In 0x80011f50..0x80012540 gibt es keinen Load von +0x4/+0x5/+0x6.** Das "kein State-Gate"
haelt.

### 8.2 BESTAETIGT — +0x93 Bit0 = "kein Schaden" (LAB_80012370)

    80012370: lbu  v0,147(s1)
    80012378: andi v0,v0,0x1
    8001237c: sb   v0,147(s1)
    ...
    800123fc: lbu  v1,147(s1)
    80012404: andi v0,v1,0x1
    80012408: beq  v0,zero,0x80012428   ; Bit0 clear -> Schadenspfad
    8001240c: ori  v0,v1,0x2
    80012410: sb   v0,147(s1)
    80012418: jal  0x80011f50           ; Rekursion auf das naechste Ziel
    80012420: j    0x80012540

### 8.3 BESTAETIGT — RE1.5 Knockdown FUN_8010512c (STAGE1.BIN)

Phasentabelle @0x801000c4 selbst gedumpt (`sltiu v0,v1,0x7` @0x80105144 gattert 7 Eintraege):

    801000c4: 6c 51 10 80  34 52 10 80  78 52 10 80  f8 52 10 80
    801000d4: 2c 53 10 80  c4 53 10 80  00 54 10 80  00 00 00 00
    -> [0]0x8010516c [1]0x80105234 [2]0x80105278 [3]0x801052f8
       [4]0x8010532c [5]0x801053c4 [6]0x80105400     (Dossier korrekt)

Phase[4] @0x8010532c — **verzweigungsfreier Block, kein toter Code, nichts ueberschreibt es**:

    8010532c: lui  v1,0x800b
    80105330: lw   v1,-14460(v1)
    80105334: ori  v0,zero,0x12
    80105338: sb   v0,148(v1)          ; +0x94 = Clip 0x12
    80105348: sb   zero,149(v0)        ; +0x95 = 0
    80105354: ori  v0,zero,0x5
    80105358: sb   v0,6(v1)            ; -> Phase 5
    80105364: ori  v0,zero,0xf
    80105368: sb   v0,143(v1)          ; +0x8f = 0xF
    80105378: lbu  v0,147(v1)
    80105380: ori  v0,v0,0x1           ; <<< +0x93 |= 1  UNBEDINGT
    80105384: sb   v0,147(v1)

Ebenfalls bestaetigt: Phase[0]-Sperre @0x80105178-84 (`ori 0x1`) + Downed-Bit @0x80105224-30
(`ori 0x80` auf +0x9); Phase[2]-Freigabe @0x801052e8-f4 (`andi 0xfe`); Phase[6]
@0x8010540c-18 (`andi 0x7f` auf +0x9), `ori v0,zero,0x201 / sw v0,4(v1)` @0x80105424-28,
+0x93-Freigabe @0x80105438-48 (der `sb` liegt im Delay-Slot von `jal 0x8001af20` und wird
ausgefuehrt).

=> Die Dossier-Aussage "RE1.5 sperrt den Knockdown-Aufsteher selbst" ist belegt; die
Nutzer-Annahme fuer diesen Pfad ist durch die Bytes widerlegt.

### 8.4 BESTAETIGT — RE1.5 Schlaefer FUN_801054f4 / Standup FUN_80104a50

Tabelle @0x801000e4 (`sltiu v0,v1,0x5` @0x8010550c), selbst gedumpt:

    801000e4: 34 55 10 80  0c 56 10 80  54 55 10 80  94 55 10 80
    801000f4: d0 55 10 80
    -> [0]0x80105534 [1]0x8010560c [2]0x80105554 [3]0x80105594 [4]0x801055d0

Phase[4] = 0x801055d0, verzweigungsfrei:

    801055d8: ori  v0,zero,0xd01
    801055dc: sw   v0,4(v1)            ; Substate 0x0d
    801055ec: sb   zero,440(v0)
    801055fc: lbu  v0,147(v1)
    80105604: andi v0,v0,0xfe
    80105608: sb   v0,147(v1)          ; FREIGABE vor dem Standup

Substate-Tabelle @0x8011f890 selbst gedumpt: `[0x0c]=0x801048e8`, `[0x0d]=0x80104a50` —
Dossier-Mapping stimmt. FUN_80104a50 komplett gelesen (0x80104a50..0x80104b34):
**kein einziger Store nach 147**; Clip 0x29 @0x80104aa8-ac, +0x95=0 @0x80104abc, +0x8f=7
@0x80104ac8-d0, 1/4-SE-5 (`andi v0,v0,0x3 / bne` @0x80104ad4-d8, `jal 0x800453d0 / ori a0,5`
@0x80104ae0-e4), Advance `jal 0x8001f314 / ori a3,0x200` @0x80104afc-b00, `sw 0x201,4(a0)`
@0x80104b24, `jr ra` @0x80104b30. Bestaetigt.

### 8.5 BESTAETIGT mit KORREKTUR — RE1.5 Liege-Spawn FUN_80103a58

Bestaetigt: Phase 0 (`beq v1,zero,0x80103aac` @0x80103a80):

    80103aac: lbu v0,147(a1)
    80103ab4: ori v0,v0,0x1
    80103ab8: sb  v0,147(a1)
    80103ac8: j   0x80103b7c            ; Funktionsende — +0x6 wird NICHT weitergeschaltet
    80103acc: sb  v0(=1),440(v1)

Die Sperre wird in dieser Phase also tatsaechlich in JEDEM Tick neu gesetzt. Clear erst
Phase 3: `sb zero,9(a1)` @0x80103b3c, `andi 0xfe / sb 147` @0x80103b64-68. Phase 1
(0x80103ad0) und Phase 2 (0x80103b08) enthalten **keinen** 147-Store. Tabelle
@0x8011f9d4[0] = 0x80103a58 selbst gedumpt. Bestaetigt.

**WIDERLEGT (nicht tragend):** Das Dossier nennt Phase 2 "der Aufsteh-Clip". In
0x80103a58..0x80103b84 gibt es **keinen Store nach 148 (+0x94)** — Phase 2 ruft nur

    80103b08: addu a2,zero,zero
    80103b0c: lw   a0,368(a1)   / 80103b10: lw a1,372(a1)
    80103b14: jal  0x8001f314   / 80103b18: ori a3,zero,0x100

auf, also einen reinen Advance mit Rate 0x100 auf den bereits geladenen Clip. WELCHER Clip dort
laeuft, ist aus dieser Funktion NICHT belegt und darf nicht als "Aufsteh-Clip" verkauft werden.
Fuer die Schlussfolgerung ist es folgenlos (+0x93 Bit0 steht ueber Phase 0..2 durchgehend).

### 8.6 BESTAETIGT — der RE1.5-Band-Stempel im ACTIVE-Tail

    801015d0: lui v1,0x1fff / 801015d8: ori v1,v1,0xffff
    801015dc: and v0,v0,v1              ; Bands 0xE0000000 weg
    801015f4: lui v1,0x4000
    801015f8: or  v0,v0,v1              ; LEVEL 0x40000000
    80101600: jal 0x80012aa4 / 80101604: ori a0,zero,0xbb8
    80101614: lbu v0,9(a1)
    8010161c: andi v0,v0,0x80
    80101620: beq v0,zero,0x80101640
    80101624: lui v1,0xbfff / 80101628: ori v1,v1,0xffff
    8010162c: lw  v0,0(a1)
    80101630: ori a0,zero,0x1388
    80101634: and v0,v0,v1               ; LEVEL weg
    80101638: jal 0x80012974
    8010163c: sw  v0,0(a1)

Bestaetigt inklusive der Konstanten 0xbb8 (@0x80101604) und 0x1388 (@0x80101630).

### 8.7 TEILWEISE WIDERLEGT — "der RE2-Trefferfilter hat NUR VIER Gates"

Die vier Gates sind exakt so bestaetigt (eigene Disasm `info/re2leon/PSX.EXE`):

    80047124: lw   v0,0(s0)     / 8004712c: andi v0,v0,0x1  / 80047130: beq -> 0x8004740c
    80047138: lbu  v0,467(s0)   / 80047140: bne  v0,zero    -> 0x8004740c
    80047148: lh   v0,342(s0)   / 80047150: bltz v0         -> 0x8004740c
    80047158: lhu  v0,270(s0)   / 80047160: andi 0xc000 / 80047164: bne -> 0x8004740c

Ebenso das vertikale Fenster; das Dossier laesst allerdings drei Instruktionen aus, die
mitten drin stehen und ein VIERTES Feld anfassen:

    8004716c: lhu  v1,464(s0)         ; +0x1D0
    80047178: andi v1,v1,0xff00
    80047184: sh   v1,464(s0)         ; low byte von +0x1D0 wird GELOESCHT (Nebenwirkung!)

**WIDERLEGT ist "nur vier Gates":** direkt nach dem Fenster steht ein FUENFTER echter
Ablehnungspfad, den das Dossier gar nicht erwaehnt:

    800471ac: sll  a1,fp,16 / 800471b0: sra a1,a1,16
    800471b4: addiu a2,s0,132        ; a2 = entity + 0x84  (Trefferbox-/Part-Array)
    800471b8: addu a3,s3,zero        ; a3 = Schuss-Deskriptor
    800471e8: jal  0x80041ef8
    800471f0: beq  v0,zero,0x800473dc ; <-- 0 => Kandidat VERWORFEN
    ...
    800473dc..80047408: Recoil-Offsets wieder abziehen
    8004740c: addiu s2,s2,4          ; Schleifen-Tail
    80047418: bne s2,v0,0x8004711c   ; naechster Kandidat

FUN_80041ef8 @0x80041ef8 ist ein reiner GEOMETRIE-Test (Matrix-Aufbau `jal 0x8008e1f4`
@0x80041f74, `jal 0x8002ce94` @0x80041fa0, Arbeitspuffer auf dem Stack) auf dem Part-Array
+0x84 — **er liest keinen State und kein Band**. Die tragende Aussage des Dossiers
("KEIN Elevations-Band, kein State-/Animations-Gate") bleibt damit richtig; die Zaehlung
"nur VIER Gates" ist es nicht, und wer das Dossier als Rezept fuer einen byte-treuen Nachbau
des RE2-Filters nimmt, wuerde diesen Test unterschlagen.

### 8.8 TEILWEISE WIDERLEGT — "das Spieler-Band ist exklusiv EIN Bit; ALLE Schreiber folgen
dem `andi 0x1fff`/`ori <ein Bit>`-Muster"

Der zitierte Schreiber stimmt:

    80033dcc: andi v0,v0,0x1fff
    80033dd0: ori  v0,v0,0x4000
    80033dd8: sh   v0,-13588(at)      ; 0x800acaec

Ich habe aber ALLE 26 `sh *,-13588(at)`-Schreiber in `PSX.EXE` gezaehlt und je vier
Instruktionen davor decodiert. Das Muster gilt NICHT durchgaengig:

    8003310c: ... 80033100 andi v0,v0,0xbfff / 80033104 ori v0,v0,0x2000   <-- nur 0x4000 geloescht
    8003315c: ... 80033150 andi v0,v0,0xbfff / 80033154 ori v0,v0,0x8000   <-- dito
    80034204: ... 800341f8 andi v0,v0,0xbfff / 800341fc ori v0,v0,0x2000
    80034254: ... 80034248 andi v0,v0,0xbfff / 8003424c ori v0,v0,0x8000
    80031720: sh zero,-13588(at)                                           <-- Band = 0
    80033cc8: ... 80033cc0 andi v0,v0,0x1fff  (KEIN ori)                   <-- Band = 0
    80012eb4: ... 80012eac ori v0,v0,0x2   (kein andi, kein Band-Bit)

`andi 0xbfff` loescht nur Bit 14 (0x4000/LEVEL) — 0x8000 (UP) und 0x2000 (DOWN) koennen danach
**gleichzeitig** stehen. Die Behauptung "exklusiv EIN Bit" ist damit als Allgemeinaussage
widerlegt. Fuer die Beweiskette des Dossiers folgenlos: fuer den Normalfall "geradeaus zielen"
liefern die `andi 0x1fff / ori 0x4000`-Schreiber (u.a. @0x80033dcc-d0, @0x80032f98-9c,
@0x800340e0-e4, @0x80034db8-bc, @0x80034f68-6c, @0x800355bc-c0) tatsaechlich genau LEVEL, und
genau das braucht §4.1.

### 8.9 BESTAETIGT — RE2 EXEC[5], Phasentabelle und die drei Schluesselphasen

Phasentabelle selbst aufgeloest (Wortsuche nach den Handler-Adressen in EMOVL10_S0.BIN):

    0x8010006c: [0]0x8010328c [1]0x80103370 [2]0x80103404 [3]0x801034dc [4]0x80103510
                [5]0x8010352c [6]0x80103568 [7]0x80103628 [8]0x801036f4

P0 @0x8010328c (eigene Disasm) — Anspruch + Liege-Bit:

    801032e0: ori v0,v0,0x202
    801032e4: sh  v0,538(s2)          ; +0x21A |= 0x202
    801032f0: lbu v0,467(s2)
    80103304: ori v0,v0,0x80
    8010330c: sb  v0,467(s2)          ; +0x1D3 |= 0x80

P2 @0x80103404 — Liege-Box + FREIGABE, verzweigungsfrei zwischen 0x80103404 und 0x80103490:

    80103454: addiu v1,zero,200  / 80103458: sh v1,144(s2) / 8010345c: sh v1,146(s2)
    80103460: addiu v1,zero,-350 / 80103464: sh v1,152(s2)
    80103468: addiu v1,zero,350  / 8010346c: sh v1,158(s2)
    80103470: lbu v1,467(s2)     / 80103478: sh zero,154(s2) / 8010347c: sh zero,156(s2)
    80103484: andi v1,v1,0x7f
    8010348c: jal 0x80015fe8
    80103490: sb  v1,467(s2)          ; Delay-Slot — wird ausgefuehrt

P6 @0x80103568 / P7 @0x80103628 — Rise-Marker gesetzt, Box waechst, KEIN 467-Store:

    80103574: lbu a0,20(v0)           ; Clip = sp[20 + side] (side = lb 362(s2))
    80103580: sb  v1(=7),6(s2)
    80103588: ori v0,v0,0x10
    8010358c: sh  v0,538(s2)          ; +0x21A |= 0x10
    80103620: andi v0,v0,0xfdff / 80103624: sh v0,538(s2)   ; 0x200 weg (P6)
    80103628-98: +0x9A/+0x9C/+0x90/+0x92 je +10 (Deckel 0x1F4), +0x98 -10 (Deckel -1499),
                 +0x9E +10   ; die Liege-Box waechst zur Stehbox
    801036a0: jal 0x8002959c / 801036a4: addiu a3,zero,256   ; Advance
    801036b8: andi v0,v0,0xffef / 801036bc: sh v0,538(s2)    ; Rise-Marker weg
    801036c4: sb  v1(=8),6(s2)
    801036c8: andi v0,v0,0xfffd / 801036cc: sh v0,538(s2)    ; Liege-Bit weg

P8 @0x801036f4 — hier steht der naechste 467-Store:

    801036f4: addiu v0,zero,257 / 801036f8: sw v0,4(s2)
    80103718: lbu v0,467(s2) / 80103724: andi v0,v0,0x7f / 80103728: sb v0,467(s2)
    8010373c: lhu v0,270(s2) / 80103744: andi v0,v0,0xdfff / 8010374c: sh v0,270(s2)

### 8.10 BESTAETIGT + OFFENER PUNKT 3 DES DOSSIERS GESCHLOSSEN — `sb *,467`-Vollzensus

Ich habe **alle** `sb rt,467(rs)` in `EMOVL10_S0.BIN` (und zur Kontrolle in `EMOVL10_S1.BIN` —
in dieser Region byte-identisch) maschinell gezaehlt. Treffer im relevanten Fenster:

    80103490: sb v1,467(s2)   | andi 0x7f @0x80103484    (EXEC[5] P2  — FREIGABE)
    80103728: sb v0,467(s2)   | andi 0x7f @0x80103724    (EXEC[5] P8  — FREIGABE, idempotent)

Zwischen 0x80103490 und 0x80103728 liegt **kein einziger** weiterer 467-Store. Der
Dossier-Offenpunkt 3 ("Vollzensus steht aus — ein weiterer +0x1D3-Setzer koennte einzelne
Aufsteh-Frames doch sperren") ist damit **erledigt: es gibt keinen.**
(Vollstaendige Setzer-Liste zur Kontrolle: 0x80100c10, 0x80101968, 0x801019b0, 0x80101e88,
0x80102188, 0x801021d0, 0x80102514, 0x80102760, 0x80102c2c, 0x80102da4, 0x80103004,
0x8010330c, 0x80103814, 0x80103a04, 0x80103c14, 0x801040bc, 0x8010445c, 0x801046e8,
0x8010472c, 0x801069b0, 0x80107504, 0x80108608, 0x80108ab8, 0x80108dd0, 0x8010937c,
0x801096ac, 0x80109b2c, 0x80109efc, 0x8010a110, 0x8010a2e0 — alle ausserhalb 0x80103568..
0x801036F0.)

### 8.11 BESTAETIGT (gegen den Port-Kommentar) — +0x21A Bit 0x10 ist der GROUND-RISE-Marker

Das Dossier nennt 0x10 den "Boden-Aufsteh-Marker", der Port nennt dasselbe Bit an der vom
Dossier zitierten Zeile `enemy_ai_re2_zombie.c:1869` **"CRAWL MARKER"** und an
`:1901` "Kriech-Marker". Da FIX 1 genau auf dieses Bit schluesselt, habe ich einen
Vollzensus aller +0x21A-Zugriffe in EMOVL10_S0.BIN gefahren (Suche nach
`lhu/sh *,538(*)` plus Kontext-`ori`/`andi`). Ergebnis:

    SETZER von 0x10 (genau ZWEI, beide der Boden-Aufsteher):
      80103588: ori v0,v0,0x10 / 8010358c: sh v0,538(s2)     EXEC[5] P6 (Knockdown-Aufsteher)
      80103d08: ori v0,v0,0x10 / 80103d10: sh v0,538(s1)     EXEC[8] P3 (Fresser-Aufsteher)
    LOESCHER von 0x10:
      801036b8 (andi 0xffef, EXEC[5] P7-Ende)   80103748 (andi 0xffed, EXEC[5] P8)
      80103d84 (andi 0xffef, EXEC[8] P4-Ende)   80103da4 (andi 0xffef, EXEC[8] P5)
      80103f7c (andi 0xffef, Kriecher-EINTRITT — dort wird 0x10 GELOESCHT und 0x8 gesetzt)
      80107ea0 (andi 0xffed)                    801049dc/8010087c (sh zero = alles weg)
    LESER von 0x10:
      8010501c (Treffer-Router: `andi 0x10 / beq` -> eigener Handler `jal 0x80107a78`)
      801083ec (Todes-Router: `andi v0,v1,0x10 / beq` -> `jal 0x801099e4`)

=> 0x10 wird **ausschliesslich** von den beiden Boden-Aufstehern gesetzt und beim
Kriecher-Eintritt (@0x80103f7c, `(x & ~0x10) | 0x8`) sogar ausdruecklich GELOESCHT. Die
Dossier-Bezeichnung ist korrekt, der Port-Kommentar "CRAWL MARKER" ist ein **Fehl-Label**.
Zusatzbefund, der das Dossier stuetzt: der Treffer-Router @0x8010501c ROUTET einen Treffer
bei gesetztem 0x10 in einen eigenen Handler — er verwirft ihn nicht. Waere der Aufsteher im
Original unbeschiessbar, waere dieser Zweig toter Code.

### 8.12 BESTAETIGT — alle Port-Zitate (Datei:Zeile) woertlich geprueft

    re15_damage.c:1256   eband = re15_band_stamp_aa4(pl, e, bdist, 0xbb8);        OK
    re15_damage.c:1267-70 int lying = (e->grid_id & 0x80) || (re15_ai_re2_for_type(...)
                          && re15_re2z_owns_type(...) && (e->re2z_flags21a & 0x2u));  OK
    re15_damage.c:1272-73 eband &= ~0x40000000u; if (bdist < 0x1388u) eband |= 0x20000000u;  OK
    re15_damage.c:1290   if ((pband & eband) == 0) continue;                       OK
                         (das Band-Gate ist NICHT flavor-gattert — es laeuft fuer RE1.5 und RE2)
    enemy_ai_re2_zombie.c:1760  e->grid_id |= 0x80u;   /* PORT-MAPPING (Review #18) */   OK
    enemy_ai_re2_zombie.c:1869  e->re2z_flags21a |= 0x10u;                          OK
    enemy_ai_re2_zombie.c:1901-02  &= ~0x10u; &= ~0x2u;                             OK
    enemy_ai_re2_zombie.c:1931  e->grid_id &= (uint8_t)0x7Fu;                       OK
    enemy_ai_re2_zombie.c:2291  e->re2z_flags21a |= 0x10u;  (EXEC[8] P3)            OK
    enemy_ai_common.c:79 / :104 / :2550 / :2561 / :2655                             OK
    enemy_ai_common.c:1756-1789 re15_enemy_ai_standup_animate — kein hit_react-Store OK

Die drei einzigen `grid_id |= 0x80`-Stellen des Ports sind :1638, :1760, :2444 — alle drei
tragen den Kommentar "PORT-MAPPING (Review #18)", keine hat einen RE2-Original-Store. Die
Ursachen-Behauptung des Dossiers ist damit belegt.

### 8.13 WIDERLEGT — die Regressions-/Gegenprobe-Aussage zu EXEC[7]

Dossier §6: *"EXEC[7] (Liege-Spawn): unveraendert gesperrt — 0x10 wird dort nie gesetzt, und
**Gate (2) des portierten Filters haelt weiter** (+0x1D3 = 0x80 @0x80103804-14 ...). Byte-true."*
Dossier §7 Schritt 5: *"Gegenprobe EXEC[7]: 0x701 erzwingen; in P1..P3 muss weapon_fire weiter 0
liefern (re2z_self1d3 == 0x80)."*

Das ist gegen den echten Port-Code falsch. `re15_re2z_hit_filter_apply`
(`enemy_ai_re2_zombie.c:7443-7481`) hat eine ausdrueckliche Ausnahme:

    int spawn_pose = (e->state == 1)
                  && (e->sub_state_1 == 7 || e->sub_state_1 == 8);  /* EXEC[7]/EXEC[8] */
    ...
    int hittable = (e->active != 0)
                && (spawn_pose || e->re2z_self1d3 == 0u)       /* (2) +0x1D3 */
                && (e->hp >= 0)
                && (spawn_pose || !(e->re2z_f10e & 0xC000u));  /* (4) +0x10E */

`re15_ai_set_state_word` (`enemy_ai_common.c:61-68`) legt 0x701 als state=1 / sub_state_1=7 ab,
also ist `spawn_pose` in EXEC[7] und EXEC[8] WAHR — **Gate (2) und Gate (4) sind dort im Port
bewusst kurzgeschlossen** ("DER FILTER GEHOERT IN DEN AUFLOESER…", Kommentarblock ab :7420).
Der Port setzt in EXEC[7] folglich `hit_react &= ~1` = treffbar. Damit ist auch die
Dossier-Aussage §4.3 "der Port bildet die vier Original-Gates 1:1 ab" **nicht** zutreffend.

Was den EXEC[7]-Liegenden im Port tatsaechlich noch sperrt, ist die `lying`-Klassifikation
ueber **`grid_id & 0x80`**: der Liege-Spawn-Deskriptor traegt das Bit von Anfang an (der Port
protokolliert an `enemy_ai_re2_zombie.c:2121` die eigene Messung "ROOM1140 slot 1 Typ 0x16,
Deskriptor **0x88**"), und EXEC[7] P4 loescht es erst am Ende (`e->grid_id = 0`). Dass das
0x80-Bit im Original mit einem Low-Nibble koexistiert, ist belegt — RE1.5 schreibt selbst
`+0x9 = 0x81`:

    801050cc: beq v0,zero,0x80105100
    801050d0: ori v0,zero,0x81
    801050d4: sb  v0,9(v1)

**Konsequenz fuer den Implementierer:** Die Schlussfolgerung des Dossiers ("EXEC[7] bleibt von
FIX 1 unberuehrt und bleibt gesperrt") stimmt im ERGEBNIS — FIX 1 schluesselt auf 0x10, das
EXEC[7] nie setzt, und `grid_id & 0x80` bleibt dort stehen. Die BEGRUENDUNG ist falsch, und
**Verifikations-Schritt 5 in §7 ist als Erwartung unbrauchbar**: er behauptet, die Sperre komme
aus `re2z_self1d3 == 0x80`; sie kommt aus dem Band. Wer nach einem fehlgeschlagenen Schritt 5
den Filter "repariert", zerstoert die Ausnahme, die den ROOM1200-Zombie (`+0x9 & 0x20`,
tickt nie) treffbar haelt.

Analog fuer EXEC[8]: `spawn_pose` schliesst dort ebenfalls kurz, und EXEC[8] setzt weder
`grid_id |= 0x80` noch `flags21a |= 0x2` im Port. Ob der Fresser-Aufsteher heute schon treffbar
ist, haengt damit **allein** am Spawn-Deskriptor (Low-Nibble 5/6 -> Fresser; traegt er 0x8X, ist
er band-gesperrt, sonst nicht). Die Dossier-Zeile "EXEC[8] P3/P4: **jetzt** treffbar" ueberzeichnet
also die Wirkung von FIX 1 — genau das ist der offene Punkt 2 des Dossiers, und er ist weiterhin
**NICHT GESCHLOSSEN**.

### 8.14 Restliche NICHT-GEFUNDEN-Punkte (unveraendert offen)

* Dossier §8 Punkt 1 (RE2s vertikales Trefferfenster im Port nicht modelliert): bestaetigt
  offen. Ich habe zusaetzlich gesehen, dass das Fenster waehrend des Aufstehens NICHT konstant
  ist — P6/P7 wachsen +0x9E in 10er-Schritten (@0x80103684-94) und +0x98 in -10er-Schritten
  (@0x80103678-8c) bis zu den P8-Endwerten 1500/-1500 (@0x80103710-20). Ein byte-treuer Nachbau
  muss diese Rampe mitfuehren, nicht nur die Liege- und die Stehbox.
* Dossier §8 Punkt 2 (Deskriptor-Zensus Fresser/Schlaefer): weiterhin offen, und nach §8.13
  wichtiger als dort dargestellt — er entscheidet, ob FIX 1 fuer EXEC[8] ueberhaupt etwas
  aendert. Naechster Weg: `+0x9`-Werte der Zombie-Spawns aus allen STAGE1-`ROOM*.RDT` ziehen
  (SCD-Member-Setter, `actor_common.c:153` `case 12: a->grid_id = value`) und nach `& 0x80`
  auszaehlen.
* Dossier §8 Punkt 4 (RE1.5-Nutzerbeobachtung): unveraendert. Meine Nachdisassemblierung
  bestaetigt @0x80105380 und @0x80103ab4 — die RE1.5-Sperre ist real, es bleibt eine
  dynamische Messung noetig.

### 8.15 Empfehlung an den Implementierer

FIX 1 (`re15_damage.c:1267-1270`, Ausnahme ueber `re2z_flags21a & 0x10`) ist durch §8.9-§8.11
gedeckt und darf so eingebaut werden — mit zwei Auflagen:

1. Im Kommentar **nicht** "CRAWL MARKER" schreiben (Port-Fehl-Label, s. §8.11), sondern
   GROUND-RISE-Marker, gesetzt @0x80103588 (EXEC[5] P6) und @0x80103D00-10 (EXEC[8] P3),
   geloescht @0x801036B8 / @0x80103D84, beim Kriecher-Eintritt aktiv geloescht @0x80103F7C.
   Am besten gleich `enemy_ai_re2_zombie.c:1869` und `:1901` mit umbenennen.
2. Verifikations-Schritt 5 in §7 vor dem Lauf korrigieren (s. §8.13), sonst produziert die
   Gegenprobe einen Fehlalarm auf dem Filter.

FIX 2 bleibt optional und aendert nichts an der Trefferbarkeit, solange `flags21a & 0x2` in
`:1269` weiter als `lying` zaehlt — das steht im Dossier korrekt.
