# Runde 41 — Spur H "kollision" (Dossier, fortlaufend)

Baum `.claude/worktrees/r41_kollision`, Zweig `r41/kollision`, Basis master-HEAD 4c667c97 (2026-10-24).

## Punkte (Wortlaut des Nutzers, AUFTRAG.md)
1. "Die NPCs z.B. Ada und Marvin koennen ineinander rein laufen, das ist falsch, die sind beide fest."
2. "In ROOM 2050 lassen sich die Kisten in einander schieben, das ist falsch"

## Stand
- 2026-10-24: Dossier angelegt, Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE gelesen.
- Punkt 2: Messung vorher + RE-Belege (FUN_8002bd44 @0x8002c03c-c0a8, FUN_8002cdf4) — Umsetzung folgt.

## Punkt 1 — NPC <-> NPC
### Messung vorher
(offen)
### RE-Belege
(offen)
### Umsetzung
(offen)
### Messung nachher
(offen)

## Punkt 2 — Kisten 2050
### Messung vorher (exe dieses Baums, Stand 4c667c97, Kopie re15_pc_h41_mess.exe)
Werkzeug: Wegplan-Treiber RE15_WEGPLAN (dachkiste_r36.c, geschlossen geregelt) + RE15_PUSH_LOG/RE15_OBJFALL_LOG.
Lauf `H_belege/kiste_vorher/` (wegplan.txt, wegplan_log.txt, push_schub.txt, objfall_ende.txt):
`RE15_DEBUG_JUMP=2050@250 RE15_PLAYER_POS=-2700,-64,0,0` (Band 0, westlich von obj 2, Blick Ost), `OBJ 2`, `GEGNERWEG`
(die zwei Adult Spider 0x25 frei), `SCHIEB 0 X >= 4500 1200`.

Raum: ROOM2050.RDT sub00 installiert die drei Raetsel-Kisten (Typ 4, Band 0, Box c(0,-900,0) h(900,900,900), pc[6..7] =
00 01 -> Pool-Wort 0x0101): obj 1 @0x01DB4 (2052,0,-5552), obj 2 @0x01DD6 (-1256,0,-64), obj 3 @0x01DF8 (4248,0,1400)
(Raetsel: die drei Kisten in die Kerben Aot slot 3/4/5 @0x01CAA/@0x01CBE/@0x01CD2, sce 5, x -7600..-5550).

Ergebnis: 289 Zeilen `SCHUB-OK`, keine einzige `SCHUB-GESPERRT`; obj 2 laeuft von x -1256 bis `F360 ... lage=(4497,-64)
ziel=(4506,-64)`, Bedingung `obj.x = 4506` erfuellt. Endlage `F1500 ... p3 (4506,0,-64) b0 | p4 (4248,0,1400) b0`:
obj 2 steckt in obj 3 (|dx| = 258 <= 1800 = hx+hx, |dz| = 1464 <= 1800, beide y 0) = **Kisten ineinander geschoben,
Nutzer-Befund reproduziert.**

### RE-Belege (selbst disassembliert, re15_disasm.py auf info/Re1.5/PSX.EXE)
Schubkette FUN_8002bd44, Zweig nach angenommenem Zellentest:
```
8002bfa4: jal 0x8002cabc ; ori a2,zero,0x1   FUN_8002cabc(Spieler,obj,1) setzt die Kiste (Spieler +- (hx+450))
8002bfb0: jal 0x8003b558 ; ori a1,zero,0x2   FUN_8003b558(obj,2) Zellentest
8002bfb8: bne v0,zero,0x8002c0b0             Zelle -> gesperrt
8002bfc0: sb v0(=8),140(s2)                  obj+0x8C = 8
8002bfd4: ori v0,v0,0x2000 / sw 0x800aca3c   Kontaktbit
8002bfe4: bne v1(=player+5),8,0x8002c0b8     nicht Schub-Substate -> Ruecksetzung
8002bfec..c038: Entity-Durchlauf (g_active_count, 0x800acc2c, Stride 500): FUN_8002cabc(entity,obj,1) != 0 -> sb s7,140(s2)
8002c03c: lw v0,0x800ac778 / lbu v0,2(v0)    Objektzahl = RDT-Kopf +2 (nOmodel)
8002c050: beq v0,zero,0x8002c0d8             keine Objekte -> Schub bleibt
8002c058: addu s0,fp,zero                    s0 = Pool-Anfang (0x800b3f98)
8002c05c: beq s2,s0,0x8002c088               das geschobene Objekt selbst ueberspringen
8002c064: lw v0,0(s0) / andi v0,v0,0x1 / beq -> 0x8002c088   nur aktive Objekte
8002c078: jal 0x8002cdf4 ; addu a1,s2,zero   FUN_8002cdf4(anderes, geschobenes)
8002c080: bne v0,zero,0x8002c0b0             UEBERLAPPUNG -> GESPERRT
8002c09c: sltu v0,s1,v0 / bne -> 0x8002c05c ; addiu s0,s0,148   naechstes Objekt (Stride 0x94)
8002c0a8: j 0x8002c0d8                       kein Treffer -> Schub bleibt
8002c0b0: sb s7(=10),140(s2)                 obj+0x8C = 10
8002c0bc..c0d4: obj+0x34/+0x3C = gesicherte Lage (s4/s5 @0x8002beec), jal 0x8003b558(obj,2) (Ruhe-Zweig)
8002c0ec..c10c: Spiegel obj+0x42 = y, +0x40 = x, +0x44 = z (lhu 56/52/60 -> sh 66/64/68)
```
FUN_8002cdf4(a0 = anderes Objekt t2, a1 = geschobenes Objekt a3) — AABB mit Halbmass-Summen, beide Boxen aus +0x78:
```
8002ce0c: lw v0,0(t2) / lw v1,0(a3) / or / andi 0x40 / bne -> return 0     Bit 0x40 in einem der Woerter: kein Test
8002ce30..ce68: dX = (a3.box.cx + a3.x) - (t2.box.cx + t2.x), dZ dito (box +0 / +4, Lage +0x34 / +0x3C)
8002ce78: a1 = t2.hx + a3.hx (box +6)      8002ce7c: t3 = t2.hz + a3.hz (box +10)
8002ce80..ce8c: sltu (2*sumX) < (dX+sumX) -> return 0   (= |dX| <= sumX, inklusive)
8002ce94..cea0: dito Z
8002cea8..cee0: dY = (a3.box.cy + a3.y) - (t2.box.cy + t2.y) (+2 / +0x38), sumY = hy+hy (box +8); keine Y-Ueberlappung -> 0
8002cee8..cf10: Vor-Y aus dem Spiegel +0x42 beider Objekte: lag die Y-Ueberlappung im Vorbild NICHT vor (vertikaler
                Eintritt), setzt @0x8002cf18-3c a3.y = t2-Mitte +- (sumY+1) - a3.box.cy und gibt 1 zurueck
8002cf14..cf68: sonst Achse: |dX*sumZ| (mult t7,t3) gegen |dZ*sumX| (mult t5,a1); X wenn |dZ*sumX| < |dX*sumZ|
8002cf70..cf94: a3.x = t2.x +- (sumX+1) (+ bei dX >= 0)   8002cf98..cfb4: a3.z = t2.z +- (sumZ+1)
8002cfb8: ori v0,zero,0x1 -> return 1
```
Der Aufrufer setzt danach x/z auf die gesicherte Lage zurueck (@0x8002c0bc-d4) — die Ausschiebung von FUN_8002cdf4 in
x/z bleibt also ohne Wirkung, nur die Rueckgabe zaehlt. Port: `re15_prop_push_tick` (aot_common.c) fuehrte den
Durchlauf @0x8002c03c-c0a8 nicht (Kommentar dort: "Objekt-gegen-Objekt-Pass FUN_8002cdf4 ... Nicht portiert", mit der
1090-Begruendung zweier Kisten in verschiedenen Baendern — fuer 2050 mit drei Kisten in Band 0 falsch).
### Umsetzung
(offen)
### Messung nachher
(offen)

## Tests
(offen)

## OFFEN
(offen)

## Fuer den Nutzer
(offen)
