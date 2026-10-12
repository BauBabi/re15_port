# Runde 37 — Spur I feuerloescher (Dossier, fortlaufend)

Baum `.claude/worktrees/r37_feuerloescher`, Zweig `r37/feuerloescher`, Basis master 64ba31e0.
Wortlaut (AUFTRAG.md Nachtrag 4, massgeblich):

> Bitte erstelle mir zusätzlich auch ein Modell des Feuerlöschers. Ich finde deine Idee, bei elza einen Leeren
> Feuerlöscher am Boden zu haben gut. Der müsste dann bei Leon nach der Nutzung erstmalig auftauchen.
>
> Außerdem finde ich den FEuerlöscher im Spind bei Leon noch unglücklich und würde ihn gerne woanders platzieren....

Punkte:
1. Modell des Feuerloeschers bauen (es gibt keines, weder RE1.5 noch RE2 — Vorarbeit
   `analysis/befunde_2026-09-21/re2-feuerloescher-modell.md`).
2. Elza ROOM1091: leerer Loescher liegt von Anfang an am Boden.
3. Leon ROOM1090: derselbe leere Loescher liegt erst NACH dem Loeschen, danach dauerhaft.
4. Leons Loescher aus dem Spind ROOM1000 an einen anderen Ort verlegen (sichtbar, mit dem neuen Modell).

Hinweis zur Zuteilung: Der Auftrag, der diesen Workflow angestossen hat, ist Nachtrag 3b ("Es muss schon in der
richtigen Farbe leuchten das Panel ...") — der gehoert laut AUFTRAG.md/VERTRAG.md zur Spur E codepanel und wurde dem
Bau-Agenten E uebermittelt. Spur I fasst die Code-Panels NICHT an.

## Stand
(laufend)

## 1. Messung vorher (Stand master 64ba31e0, Baum gebaut 2026-10-12)

Raumstart mit allen Port-Installern (`probe_r34n_e_dokumente aots <Raum>`, echte RDT + scd_room_reenter):

* ROOM1000 (Leon): `AOT slot= 3 ITEM x[20500..21300] z[-800..0] item=0x31 x1 bit=134 prop=255` — die
  Aufnahme ist eine reine Zone ohne Modell (prop 255 = 0xFF), in Cut 0-2 (Umkleide, Spindreihen; Bild
  `I_belege/vorher_1000_umkleide.png`). Props: obj 0/1 Medizinkoffer, obj 2 Dokument (Runde 34).
* ROOM1090 (Leon, Feuer brennt): Slots 0-4 (Tueren, Text msg 7 Viereck, AUTO, Schrot r35), Props obj 0,1
  (Typ 4), obj 3 (Wagen-Teil), obj 4 (Schrot). Nach dem Loeschen legt der Raum KEINEN Loescher hin.
* ROOM1091 (Elza): Slots 0,1,2,4,5 (Schuppen-Text Runde 36 Spur P), Props obj 0,1,2,4 — kein Loescher.
* Modell: es gibt keins (Vorarbeit `analysis/befunde_2026-09-21/re2-feuerloescher-modell.md` §4.2/4.3:
  Item 0x31 kommt genau einmal vor, ROOM1000 @0x0C24 tk_prop 0xFF; 570/570 RE1.5-Props und 392 RE2-Netze
  angesehen).

## 2. RE-Belege (selbst disassembliert / aus den RDT-Bytes)

### 2.1 Die Aufnahme im Original — ROOM1000.RDT sub00 @0x00C24 (22 B, Kurzform)
```
0C24: 50 03 09 31 00 00 14 50 e0 fc 20 03 20 03 31 00 01 00 86 00 ff 00
      Item_aot_set Slot 3, sce 9, sat 0x31, Band 0, Rechteck (20500,-800) 800x800, Item 0x31 x1, Bit 0x86, tk_prop 0xFF
```
Handler Item_aot_set @0x80040644 (PSX.EXE, `re15_disasm.py dis 0x80040644`):
```
80040664  andi v0,v0,0x80        ; sat & 0x80 -> Lang/Kurzform
80040680  lhu  a1,18(a2)         ; +0x12 = Taken-Bit (Kurzform)
80040684  lbu  s1,20(a2)         ; +0x14 = tk_prop
80040688  addiu v0,a2,22         ; Kurzform 22 B
800406d8  lw   a0,18056(a0)      ; 0x80074688 = Flag-Tabelle Bank 9 (0x800b1078)
800406dc  jal  0x8004efe4        ; Bit lesen
800406e4  beq  v0,zero,0x8004071c ; nicht genommen -> fertig
800406f4  sb   zero,0(v0)        ; genommen -> Satz still (sce 0)
800406f8..80040714               ; Pool 0x800b3f98 + tk_prop*148
80040718  sw   v1,0(at)          ; = 0x80000000 -> Modell versteckt
```
Aufnahme bestaetigt (FUN_8001db28 Zustand 7 @0x8001e048): `sb zero,0(v1)` @0x8001e090 (Zone tot), Einfuegen
`jal 0x8004dc4c` @0x8001e0c4, Taken-Bit `jal 0x8004ef90` @0x8001e0d0 mit a0 = 0x800b0fd6+162 = 0x800b1078 (Bank 9),
a1 = Nutzlast +4. Prompt-Rueckruf LAB_80021f6c: Modell weg `sw zero,0(at)` @0x80021fc8 (Pool[Nutzlast+6 = tk_prop]),
Bit `jal 0x8004ef90` @0x80021fd8. ⇒ KEIN Pfad setzt dort (3,0x85).

### 2.2 Story-Bit (3,0x85) — ROOM1000.RDT sub01 @0x00CFC
```
0CFC: 06 00 14 00   Ifel_ck
0D00: 21 09 86 01   Ck(9,0x86)==1     (Loescher genommen)
0D04: 06 00 0a 00   Ifel_ck
0D08: 21 03 85 00   Ck(3,0x85)==0
0D0C: 22 03 85 01   Set(3,0x85)=1
```
sub01 wird in JEDEM Spielbild neu gestartet: FUN_8003f038 `ori a0,zero,0x1` @0x8003f064, `lw v0,68(v0)` @0x8003f070
(RDT+0x44), `jal 0x8003ee3c` @0x8003f080 mit a1=1. Liegt der Fundort NICHT mehr in ROOM1000, muss dieselbe Regel
dort greifen, wo genommen wird. Leser: ROOM1090 sub00 @0x02306 `06 00 24 00 21 03 85 00`: (3,0x85)=0 -> Slot 2
sce 1 msg 7 @0x0230E ("I must hurry up and get something to put out this fire to save that woman!"); sonst
@0x02336 Slot 2 sce 3 -> sub06 + Selbst-Tuer Slot 3 @0x02352. Zensus aller 258 RDTs (`scd_dump_room.py`):
(3,0x85) nur in ROOM1000 sub01 und ROOM1090 sub00.

### 2.3 Loeschen und das DAUERHAFTE Bit — ROOM1090.RDT
```
sub06 @0x02702: 2b 07 ff ff  Message_on 7 / 2b 08 ff ff Message_on 8 ("Will you use the Fire Extinguisher?")
      @0x0270E: 06 00 18 00  Ifel_ck ; @0x02712 21 0c 1f 00 Ck(12,0x1F)==0 (= Ja)
      @0x02716: 22 02 07 01  Set(2,7)=1 ; @0x0271A 09 0a 0a 00 Sleep 10
      @0x0271E: 22 03 81 01  Set(3,0x81)=1   <- Feuer aus (dauerhaft)
      @0x02722: 22 03 84 01  Set(3,0x84)=1   <- Einmal-Zeichen fuer die Rettungsszene
      @0x02726: 47 03        Aot_on 3 (Selbst-Tuer @0x02352 -> ROOM1090 neu laden)
sub00 @0x022A2: 06 00 40 00 / 21 03 84 01  Ck(3,0x84)==1 -> obj 2, Sce_em_set 0x42 (Ada), Evt_exec sub03 @0x022E0
sub03 @0x024CE: 22 03 84 00  Set(3,0x84)=0  <- ERSTER Befehl der Rettungsszene
```
⛔ Der Auftrag nennt (3,0x84) als Bedingung — das Bit ist nach der Rettungsszene wieder 0 (sub03 @0x024CE); ein daran
gehaengter Loescher laege nur waehrend des einen Neuladens da und waere beim Wiederbetreten weg. DAUERHAFT ist (3,0x81):
sub00 legt Feuer/brennende Kamerabilder nur bei Ck(3,0x81)==0 an (@0x021C8 Cut-Wahl 8/11, @0x021EE obj 3 + 7x Sce_em_set
Typ 0x26, @0x022EA Musik, @0x02332 Loesch-Platz). Zensus Set(3,0x81) in 258 RDTs: ROOM1090 sub06 (Leon) und ROOM10B1
@0x01832/@0x01954 (Elza-Variante, eigene Bedeutung — Leons Spiel betritt 10B1 nie). ⇒ Tor fuer Leon = (3,0x81)==1.

### 2.4 Elza — ROOM1091 / ROOM1001
ROOM1091 main00 @0x02202 `2d 02 00 00 02 00 08 00 10 00 6a 09 a3 f5 2e ff 40 00 40 07 ...` = obj 2 an Leons
Nach-Rettungs-Lage (wie ROOM1090 sub00 @0x0237E), kein Typ 0x26 — bei Elza ist der Platz schon geloescht (Leon war
zuerst da). ROOM1001 sub00 @0x00C24 `50 03 09 31 00 00 14 50 e0 fc 20 03 20 03 15 00 1e 00 86 00 ff 00` = derselbe
Spind traegt bei Elza Item 0x15 x30 mit demselben Bit 0x86 — bleibt unberuehrt (der Spind-Umbau gilt nur 0x1000).

### 2.5 Wie das Original Gegenstands-Modelle zeichnet (Licht)
Obj_model_set LAB_80040914: Flags `lhu v0,6(a2)` @0x80040990, `ori v0,v0,0x1` @0x80040998; Attribut `lh v0,8(a2)`
@0x800409a8 -> `sw v0,12(a1)` @0x800409b0 (Pool+0x0c). Zeichner FUN_8002c18c: `lw v0,12(s1)` @0x8002c2ec,
`andi v0,v0,0x10` @0x8002c2f4, `beq v0,zero,0x8002c360` @0x8002c2f8 -> ohne Bit NCCT (Raumlicht), mit Bit flach
`jal 0x80022f0c` @0x8002c320. Item-Props tragen Attribut 0: ROOM1010 @0x00930 `2d 00 00 00 01 00 0a 00 00 00`
(First Aid Spray), ROOM4050 @0x01E8A/@0x01F32/@0x01FC6 (Antidote Spray, Kraeuter), ROOM1000 @0x00C66/@0x00C88
(Medizinkoffer). ⇒ Loescher = Typ 0, Band 1, Flags 0x000A|1, Attribut 0 (NCCT) — wie jedes Gegenstands-Modell.
(Die Lageplaene der Runde 36 tragen 0x0010, weil RE2 seine WANDPLAENE so zeichnet — ein Zylinder ist kein Plan.)

### 2.6 Aktionspruefpunkt
FUN_80042bac `ori v0,zero,0x26c` @0x80042bd0 = 620 Einheiten vor dem Spieler; je Druck gewinnt der KLEINSTE treffende
Slot (aot_common.c, `j 0x80043028` @0x80042f94). Daraus: Aufhebe-Rechteck muss den 620-Punkt vor der Kollisionskante
enthalten (Abschnitt 4), Untersuchen-Platz in ROOM1091 braucht einen Slot < 5 (Schuppen-Text).

### 2.7 Nachtrag 3b (der Auftrag, der diesen Lauf ausgeloest hat)
"Es muss schon in der richtigen Farbe leuchten das Panel..." gehoert laut AUFTRAG.md (Nachtrag 3b) / VERTRAG.md zur
Spur E codepanel und wurde dem Bau-Agenten E uebermittelt. Spur I hat keine Panel-Datei angefasst.
