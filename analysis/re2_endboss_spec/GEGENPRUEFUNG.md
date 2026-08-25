# Gegenpruefungen

⛔ NUR ZWEI von drei Gegenpruefern haben geliefert. Der dritte (Modell, Boss-Sound,
Sound-Importliste, Dokumente) ist mit 'StructuredOutput retry cap exceeded' gescheitert.
Diese vier Straenge sind also NICHT adversarisch gegengeprueft.

## Pruefer 0

Das Geruest traegt: Zustandstabelle, alle Untertabellen, alle Datentabellen (TAB_COMP_CMD, TAB_PERM_4x4, TAB_S2_PHASE, STAGGER_WEIGHT_TABLE) und praktisch alle Konstanten sind byte-genau. Die Kernfrage ist eindeutig beantwortet — ich habe JEDES slti/sltiu in z0/z1/z2 maschinell mit rueckwaerts aufgeloestem Operanden ausgelesen: 6000/7000/10001/11001 sind Abstaende gegen +0x1F0 (lw 496), 4001/7000/8000/9001/10000/12000 sind Welt-X gegen +0x38 (lw 56, signed slti), und in z2 sind -20499/-26000/1499 Z-Koordinaten, 15 der Akkumulator +0x222, 11 ein Tabellenbyte. KEIN einziger davon ist ein Timer, und keine Schwelle ist in der Doku falsch zugeordnet. z0 und z2 sind sauber — fuer z2 ist der Store-Census sogar nachweislich vollstaendig. Gekippt wird 14-mal, davon vier port-relevant, alle in z1: (1) die Behauptung, drei SE-Aufrufe uebergaeben keinen Entity-Zeiger (widerlegt — alle 29 tun es, Delay-Slots @0x80100F1C/@0x80100F84/@0x8010109C); (2) drei FEHLENDE Stores — [Begleiter+0x198]+0x8C := 0 in s2 p1 (@0x80101244/@0x8010126C/@0x80101298), +0x15A += 1 in s3 (@0x80101A7C; ohne ihn feuert der Begleiter-Befehl jeden Frame statt alle 32), +0x16B := 0 in s0 p0 (@0x801009D0; ohne ihn bleibt der Befehls-Riegel haengen); (3) ein fehlender SE 10 beim Uebergang s2 p0 -> p1 (@0x8010120C j 0x801017E0 mit a0=10); (4) drei Feld-/Basis-Verwechslungen im Abschnitt "Zustand 7", die z1/z2 betreffen (0x800CFB74 statt +0x226, +0x222 statt Gewichtstabelle, 0x800D3C38 statt 0x800CFE20). Der Rest sind Zitat-Ungenauigkeiten (nop statt Ladebefehl, Sprungziel 4 Byte daneben, Delay-Slot-Setzer verwechselt, fehlendes andi 0x3FF im EXE-Applier). Vor der C-Umsetzung: die vier port-relevanten Punkte einarbeiten und den z7-Abschnitt gegen den z1/z2-Abschnitt korrigieren.

### GEKIPPT: z0-Prosa: 'kein sll 2 / lw TABELLE in der ganzen Funktion'
In 0x801003CC stehen ZWEI 'sll v0,v0,2' plus zwei 'lbu v0,8(s0)' (Typ) und zwei indizierte sw. Der Schluss (kein Unter-Dispatch auf +0x5/+0x6) traegt, die Aussage ueber die Instruktionen nicht.

**Gegenbeleg:** 801005c8: lbu v0,8(s0) / 801005d0: sll v0,v0,2 / 801005dc: sw v1,-7424(at)  ;  801005e0: lbu v0,8(s0) / 801005ec: sll v0,v0,2 / 801005f8: sw v1,-7168(at)

### GEKIPPT: z1: SE-Aufrufe 0x80100F18 / 0x80100F80 / 0x80101098 sollen a1 NICHT als Entity uebergeben
REFUTIERT. Alle drei setzen a1 = s1 = Entity im Delay-Slot. Die daraus abgeleitete Aussage '0x8005BD6C dereferenziert a1 (Restwert 0x00070B01)' ist Unsinn. Maschineller Census: ALLE 29 jal 0x8005BD6C in 0x80100784..0x80101D4C setzen a1 per Registermove auf die Entity, ohne Ausnahme.

**Gegenbeleg:** 80100f14: addiu a0,zero,9 / 80100f18: jal 0x8005bd6c / 80100f1c: addu a1,s1,zero || 80100f7c: addiu a0,zero,11 / 80100f80: jal 0x8005bd6c / 80100f84: addu a1,s1,zero || 80101094: addiu a0,zero,9 / 80101098: jal 0x8005bd6c / 8010109c: addu a1,s1,zero

### GEKIPPT: z1 s0: a1-Quelle des SE-9-Aufrufs @0x80100AB8 mit '@0x80100a8c' zitiert
Argument nicht rueckwaerts aufgeloest: @0x80100A8C setzt a1 = s1 (EDD-Zeiger fuer den davor liegenden Anim-Aufruf). Der wirksame Setzer ist der Delay-Slot @0x80100ABC (a1 = s2 = Entity).

**Gegenbeleg:** 80100a8c: addu a1,s1,zero (fuer jal 0x8002959c @0x80100a94) / 80100ab4: addiu a0,zero,9 / 80100ab8: jal 0x8005bd6c / 80100abc: addu a1,s2,zero

### GEKIPPT: z1 s0 p0: FEHLENDER Store +0x16B := 0
Ausgelassen. Der Store steht im Delay-Slot von 'bne v1,zero' und laeuft daher IMMER, wenn p0 betreten wird. Ohne ihn bleibt der Befehls-Riegel +0x16B|1 aus dem vorigen Zyklus stehen und die Begleiter-Befehle in p1 werden dauerhaft gesperrt.

**Gegenbeleg:** 801009c8: slti v1,v1,8000 / 801009cc: bne v1,zero,0x801009e8 / 801009d0: sb zero,363(s2)

### GEKIPPT: z1 s2 p0 -> p1: FEHLENDER SE 10
Ausgelassen. p0 endet nicht mit 'return', sondern springt in den gemeinsamen SE-Schwanz mit a0=10 und a1=Entity. Der Uebergang spielt also SE 10, genau wie p4->p5.

**Gegenbeleg:** 801011c0: addiu a0,zero,10 (Delay-Slot von beq) / 801011f8: addu a1,s0,zero / 8010120c: j 0x801017e0 / 801017e0: jal 0x8005bd6c

### GEKIPPT: z1 s2 p1: FEHLENDER Store [Begleiter+0x198]+0x8C := 0 (3 Fundstellen)
Ausgelassen. Bei den Timer-Marken 10/30/40 wird nicht nur Begleiter+0x06 := 1 gesetzt, sondern zusaetzlich die Auszugslaenge des jeweiligen EM37-Tentakels (model+0x8C, laut EM37-Doku 0..4096) auf 0 zurueckgesetzt. Ohne den Store startet die Beschwoerungssequenz mit voll ausgefahrenen Tentakeln.

**Gegenbeleg:** 80101234: lw a2,15416(s3) / 8010123c: lw a3,408(a2) / 80101240: sb v0,6(a2) / 80101244: sh zero,140(a3) || 80101260: lw a3,408(a2) / 8010126c: sh zero,140(a3) || 80101288: lw a3,408(a2) / 80101298: sh zero,140(a3)

### GEKIPPT: z1 s3: FEHLENDER Store +0x15A += 1 pro Frame
Ausgelassen, obwohl die Doku 'S3_CMD_PERIOD alle 32 Frames' behauptet. +0x15A wird in s3 p0 auf 0 gesetzt (@0x80101888) und danach NUR hier hochgezaehlt. Ohne den Inkrement bleibt (+0x15A & 0x1F) permanent 0 und der Begleiter-Befehl feuert in JEDEM Frame statt alle 32.

**Gegenbeleg:** 80101a70: lhu v0,346(s2) / 80101a78: addiu v0,v0,1 / 80101a7c: sh v0,346(s2)   (einzige weitere 346-Stores im Chunk: 0x80101888=0, 0x80101cc4=150, 0x80101cf0=Dekrement)

### GEKIPPT: z2 p0: zweite Wund-Emission — der +500-Schritt fehlt und ist ein ORIGINAL-BUG
Unvollstaendig. Zwischen den beiden Wund-ESPs steht ein '+500'-Schritt, der aber sp+16 trifft, waehrend a3 = sp+24 = vecB unveraendert bleibt. Beide Sprites landen am IDENTISCHEN Punkt. Die z3-Doku dokumentiert genau diesen Bug als QUIRK_SP24; die z2-Doku verschweigt ihn.

**Gegenbeleg:** 801028ec: addiu s1,sp,24 (a3) / 801028f0: jal 0x8001bf10 / 801028f8: lhu v0,16(sp) / 80102900: addiu v0,v0,500 / 80102908: sh v0,16(sp) / 80102928: jal 0x8001bf10 / 8010292c: addu a3,s1,zero

### GEKIPPT: z1 s2: Sprungziel '@0x801017e0 (per j @0x801012e4)'
Adresse falsch: @0x801012E4 springt nach 0x801017DC, nicht nach 0x801017E0. Wirkung identisch (0x801017DC setzt a1 = s0 und faellt in den jal), aber ein Port, der die Zieladresse als Einstieg nachbaut, laesst den a1-Setzer weg.

**Gegenbeleg:** 801012e4: j 0x801017dc / 801012e8: addiu a0,zero,10 (Delay) ; 801017dc: addu a1,s0,zero / 801017e0: jal 0x8005bd6c. Exhaustiver Scan: 'j 0x801017e0' existiert nur @0x8010120c, @0x80101414, @0x80101528

### GEKIPPT: z1 s0: COMPANION_SCAN-Zitat '@0x801009fc'
@0x801009FC ist ein nop. Der Ladebefehl fuer die Begleitermaske +0x228 steht eine Instruktion frueher.

**Gegenbeleg:** 801009f8: lbu a1,552(s2) / 801009fc: nop / 80100a00: srav v0,a1,a0

### GEKIPPT: Querwiderspruch z7-Abschnitt: 'z1 -> z3 wenn 0x800CFB74 & 0x6 == 4'
Falsche Basis. Das Tor liest Entity+0x226, nicht die Globale 0x800CFB74. Der z1- und der z2/z3-Abschnitt sagen es richtig; der z7-Abschnitt wuerde den Tod-Uebergang an eine globale Flagge haengen.

**Gegenbeleg:** 801007e0: lhu v0,550(s0)  (550 = 0x226, s0 = Entity) / 801007e4: addiu v1,zero,4 / 801007e8: andi v0,v0,0x6 / 801007ec: bne v0,v1,0x80100800

### GEKIPPT: Querwiderspruch z7-Abschnitt: 'TIMER_222_LT11 = 11 (sltiu gegen +0x222) @0x801029C4'
Falsches Feld. Die sltiu 0xb vergleicht den Gewichts-Byte-Wert aus der Tabelle 0x801056B3[+0x5], NICHT den Akkumulator +0x222. Der z2-Abschnitt (STAGGER_FLASH_THRESHOLD) hat es richtig. Nur die sltiu 0xf @0x80102A58 liest +0x222.

**Gegenbeleg:** 801029b0: lbu v0,5(s3) / 801029b8: addu at,at,v0 / 801029bc: lbu v0,22195(at) [=0x801056b3] / 801029c4: sltiu v0,v0,0xb   vs.   80102a50: lbu v0,546(s3) / 80102a58: sltiu v0,v0,0xf

### GEKIPPT: Querwiderspruch z7-Abschnitt: Begleiter-Zeiger '0x800D3C38/3C3C/3C40/3C44'
Basisregister falsch aufgeloest (0x800D0000 statt 0x800CC1E8). Die vier EM37-Zeiger liegen bei 0x800CFE20/24/28/2C — so wie es der z1-Abschnitt und die EM37-Doku sagen.

**Gegenbeleg:** 80101134: lui s3,0x800d / 80101138: addiu s3,s3,-15896 ; 0x800CC1E8 + 15416 = 0x800CFE20. Gegenprobe 80104e60: lw v0,-480(v0)=0x800CFE20 / 80104e70: 0x800CFE24 / 80104e80: 0x800CFE28 / 80104e90: 0x800CFE2C

### GEKIPPT: EXE-Schadensapplier: fehlende Schadensmaske und +0x1D3-Maskierung
Zwei Instruktionen ausgelassen: der Schaden wird vor dem Abzug auf 10 Bit maskiert, und nach dem Treffer wird +0x1D3 auf sein Bit 7 reduziert (alle Timer-Bits geloescht).

**Gegenbeleg:** 8004725c: andi v1,v1,0x3ff / 80047260: subu v0,v0,v1  ||  8004731c: lbu a0,467(s1) / 8004732c: andi a0,a0,0x80 / 80047334: sb a0,467(s1)

## Pruefer 1

Das Material zu z3/z4/z7 und EM37 ist ueberwiegend byte-genau und tragfaehig. Die beiden explizit angefragten HP-Stores stimmen exakt: 0x80103004 `sh zero,342(s3)` nur im Zweig (+0x226 & 2) != 0, 0x80103060 `sh -1,342(s3)` nur im Zweig == 0, und +0x225 := 240 laeuft als Delay-Slot in BEIDEN Zweigen. Zustand 5/6 ist als unerreichbar bestaetigt: die Tabelleneintraege @0x801055E0/@0x801055E4 sind literal 0, der Root-Dispatch @0x80100164-84 hat keinen Bereichstest, und mein eigener Store-Scan ueber EM36_ai1 UND die komplette RE2-PSX.EXE findet kein einziges Literal 5, 6 oder 7 auf Offset +0x04. Alle groesseren Tabellen (0x80100054, 0x801055CC, 0x801056C8, 0x801056E8, EM37 0x80105688 mit 42 Eintraegen, 0x80105668, 0x80105730-0x801057F0, 0x8010582C) habe ich roh aus den Dateien nachgelesen, die Rechenketten (2950*t-Shiftkette, Sink-Stufen 16/12/8/4, Farbziele 48/48/64, 12 Explosions-Ruettler, 7er-Blutschleife, EM37-Doppeldispatch mit 16 leeren PRE-Stubs, 18 SE-Aufrufe, 4 Schadensaufrufe) instruktionsweise nachvollzogen — deckungsgleich. Sieben Punkte kippen. Zwei sind hart port-relevant: (1) das Instruktionsfenster @0x80103a68 ist um ein Wort verschoben — die -2048-Ruecknahme auf 0x800CFC6E ist KORREKT und wird nicht vom Rueckgabewert ueberschrieben (Rohbytes `00 f8 42 24 | 67 a5 00 0c | 00 00 02 a6`); (2) die Begleiter-Zeiger sind 0x800CFE20/24/28/2C, nicht 0x800D3C38ff — 15416 wurde gegen 0x800D0000 statt gegen s3 = 0x800CC1E8 gerechnet. Dazu zwei Etikettierungsfehler im Abschnitt 'Zustand 7' (0x800CFB74 statt Entity+0x226 als Gate-Quelle; 0x80102B74/88 als 'z3' statt Taumel-Executor 0x80102AD0) und zwei Vollstaendigkeitsluecken (EM37-Relokationsliste uebersieht 0x8010558C/0x801055D8 mit Delta 0x3400; die Se_on-Liste uebersieht 0x80104724 und 0x8010496C in den voellig undokumentierten Hook-Substates 0x801046AC/0x801048F4). Auffaellig: die Abschnitte 'Zustand 4' und 'Zustand 7' widersprechen sich bei der Delay-Slot-Frage — der Zustand-7-Abschnitt hat dort recht, der Zustand-4-Abschnitt nicht. Der siebte Punkt ist keine Falsifikation, sondern eine offene RE-Frage, die VOR dem Port geklaert werden muss: die Unverwundbarkeit von 0x37 ist jetzt belegt (Gates @0x80047150 und @0x80041288 auf HP<0), aber derselbe Block prueft auch (+0x10E & 0xC000) != 0, und EM36 spawnt mit +0x10E = 0x8000 bei NULL Schreibzugriffen auf Offset 270 im gesamten Chunk — damit waere der Boss nie treffbar und die halbe z2/z3-Maschinerie toter Code. Ausserdem gibt es lebenden EM36-Code jenseits von 0x80103A9C, den kein Abschnitt abdeckt (u.a. `sw 1025,4(...)` @0x80103EA4 sowie die Tabellenworte 0x8010006C/0x80100070 -> 0x80103F44/0x80103F58, die nur das `sltiu 0x6` @0x80102C0C unerreichbar haelt).

### GEKIPPT: EM36 Zustand 4 Phase 2 — GLOBALWINKEL_RUECKNAHME (0x800CFC6E bekommt angeblich den anim_advance-Rueckgabewert)
Der Bericht hat das Instruktionsfenster um genau ein Wort verschoben und behauptet, @0x80103a68 sei der `jal 0x8002959c`, @0x80103a6c das `addiu v0,v0,-2048` im Delay-Slot (verworfen) und @0x80103a70 ein `sh v0,0(s0)` NACH dem Call, der deshalb 0 oder 1 in 0x800CFC6E ablegt. Tatsaechlich ist es umgekehrt: 0x80103a68 = addiu, 0x80103a6c = jal, und der sh @0x80103a70 IST der Delay-Slot, laeuft also VOR dem Call und schreibt v0 = alterWert-2048. Die Ruecknahme ist korrekt und symmetrisch zum +2048. Ein Port nach dem Bericht wuerde den globalen Winkel 0x800CFC6E mit 0/1 zerstoeren.

**Gegenbeleg:** Rohbytes CDEMD0_EM36_ai1.BIN, File-Offset 0x3a68: `00 f8 42 24 | 67 a5 00 0c | 00 00 02 a6 | 02 00 40 10` = 80103a68 addiu v0,v0,-2048 / 80103a6c jal 0x8002959c / 80103a70 sh v0,0(s0) / 80103a74 beq v0,zero,0x80103a80. Vergleichsmuster der ersten Haelfte: 80103a40 lhu v0,0(s0) / 80103a48 addiu v0,v0,2048 / 80103a4c jal 0x80015cb8 / 80103a50 sh v0,0(s0) — beide Male steht der sh im Delay-Slot. Der Abschnitt 'Zustand 7' desselben Materials gibt die Reihenfolge richtig wieder; die beiden Abschnitte widersprechen sich.

### GEKIPPT: EM36 Abschnitt 'Zustand 7' — Begleiter-Zeiger angeblich 0x800D3C38 / 0x800D3C3C / 0x800D3C40 / 0x800D3C44
Der Offset 15416 wurde gegen die falsche Basis gerechnet (0x800D0000 statt des tatsaechlich geladenen s3). s3 wird in derselben Funktion mit 0x800CC1E8 geladen; 0x800CC1E8 + 15416 (0x3C38) = 0x800CFE20. Die Zeiger liegen bei 0x800CFE20/24/28/2C — genau dort, wo 0x80104E9C und der Zustand-1-Abschnitt sie verorten. Der Bericht widerspricht sich intern; die 0x800D3C38-Adressen liegen 0x3E18 daneben und wuerden im Port in fremden Speicher schreiben.

**Gegenbeleg:** @0x80101134 lui s3,0x800d / @0x80101138 addiu s3,s3,-15896 -> s3 = 0x800CC1E8 (das Werkzeug annotiert die Zeile selbst mit 0x800cc1e8). Verbraucher: @0x80101448 lw a2,15416(s3) / @0x8010144c addiu v0,zero,2305 / @0x80101454 sw v0,4(a2). 0x800CC1E8 + 15416 = 0x800CFE20. Gegenprobe im Setter: 0x80104e9c sll a0,a0,2 / 0x80104ea0 lui at,0x800d / 0x80104ea4 addu at,at,a0 / 0x80104ea8 lw v0,-480(at) = 0x800CFE20 / 0x80104eb0 sw a1,4(v0).

### GEKIPPT: EM36 Abschnitt 'Zustand 7', UEBERGAENGE — 'z1 -> z3 wenn 0x800CFB74 & 0x6 == 4' (und dasselbe beim Finisher-Gate & 0x4)
Beide Gates lesen NICHT das globale Wort 0x800CFB74, sondern das Entity-Halbwort +0x226. Der Bericht zitiert die richtigen Instruktionsadressen, etikettiert die Quelle aber falsch. Wer daraus einen globalen Flagtest baut, macht aus einem pro-Gegner-Zustand einen weltweiten Schalter — der Boss ginge nie oder immer in Zustand 3. Der Zustand-1-Abschnitt hat es richtig (Z3_GATE_226 / FINISH_BLOCK_226).

**Gegenbeleg:** @0x801007e0 lhu v0,550(s0)  (550 = 0x226, s0 = Entity) / @0x801007e4 addiu v1,zero,4 / @0x801007e8 andi v0,v0,0x6 / @0x801007ec bne v0,v1,0x80100800 / @0x801007f4 addiu v0,zero,3 / @0x801007fc sw v0,4(s0). Finisher: @0x801008e8 lhu v0,550(s0) / @0x801008f0 andi v0,v0,0x4 / @0x801008f4 bne v0,zero,0x80100948 / @0x801008f8 addiu a0,zero,500 / @0x80100904 jal 0x800401d4. Basis ist beide Male s0 = Entity, kein lui/lw auf 0x800CFB74.

### GEKIPPT: EM36 Abschnitt 'Zustand 7', UEBERGAENGE — 'z3 -> z1 wenn jal 0x8002959c (a3=128) liefert != 0 || 80102b74 ... 80102b88 sw v1,4(s0)'
Der zitierte Code liegt nicht in Zustand 3. 0x80102B74/0x80102B88 gehoeren zum Taumel-Executor 0x80102AD0 (Rumpf bis jr ra @0x80102BB4); der Zustand-3-Rumpf beginnt erst bei 0x80102BBC/0x80102BDC. Der Uebergang ist 'Zustand 2 / Taumel -> Zustand 1', nicht 'Zustand 3 -> Zustand 1'. Zustand 3 kehrt NIE per anim_advance nach Zustand 1 zurueck — sein einziger Rueckweg ist der aufgeschobene Tod ueber +0x1FC.

**Gegenbeleg:** Funktionsgrenze selbst gelesen: @0x80102bb4 jr ra / @0x80102bb8 nop, danach @0x80102bbc addiu sp,sp,-24 / @0x80102bc0 sw ra,16(sp) / @0x80102bc4 jal 0x80102bdc — das ist der z3-Wrapper, Tabelleneintrag 0x801055CC[3] = bc 2b 10 80. Der einzige z3-Ausgang nach z1 ist @0x80103008 sw v1,4(s3) mit v1 = lw 508(s3).

### GEKIPPT: EM37 — 'AI_TABELLEN_RELOKATION = nur 0x80105688..0x8010572f, 0x8010582c..0x8010583f, 0x801058ec, 0x80105974, 0x801059ec'
Das 'nur' ist als vollstaendige Liste falsch: der ai0/ai1-Diff enthaelt im rodata-Bereich zwei weitere abweichende Worte, 0x8010558C und 0x801055D8. Sie verschieben sich nicht um 0xD000, sondern um 0x3400 — also relokierte Zeiger einer ZWEITEN Basis. Die Schlussfolgerung (0x801057E4/0x80105808/0x80105840 sind keine Zeigertabellen) bleibt korrekt; die Liste als 'alle relozierbaren Worte' ist es nicht.

**Gegenbeleg:** Eigener Wort-Diff CDEMD0_EM37_ai0.BIN gegen ai1.BIN (beide 23024 B): 362 abweichende Worte, davon 50 ab 0x80105500. Laufende Bloecke: 8010558c | 801055d8 | 80105688..80105698 | 801056a4..8010572c | 8010582c..8010583c | 801058ec | 80105974 | 801059ec. 48 der 50 haben ai0 = ai1 + 0xD000; die Ausreisser 0x8010558C und 0x801055D8 haben ai1 = 0x08040000, ai0 = 0x08043400 (Delta 0x3400). Nebenbefund: die Luecke bei 0x8010569C/0x801056A0 im Diff bestaetigt unabhaengig, dass STATE_TAB[5]/[6] in BEIDEN Bauten 0 sind.

### GEKIPPT: EM37 — Aufrufliste von 0x8005BA28 (Se_on) in den Ruecksto&-Routinen ist unvollstaendig
Der Bericht listet vier Se_on-Stellen (@0x80104358, @0x801044F8, @0x8010458C, @0x8010459C). Ein vollstaendiger jal-Scan ueber den Chunk findet SECHS: zusaetzlich @0x80104724 und @0x8010496C. Diese liegen in den Hook-Substates 3 (0x801046AC) und 4 (0x801048F4), die der Bericht ueberhaupt nicht beschreibt — er dokumentiert nur 0x801042C4 und nennt 0x80104454/0x801046AC/0x801048F4 bloss als Tabelleneintraege. Dem Port fehlten zwei Aufprall-Sounds und zwei komplette Zweige.

**Gegenbeleg:** Eigener jal-Scan ueber CDEMD0_EM37_ai1.BIN auf Ziel 0x8005BA28 liefert genau: 80104358, 801044f8, 8010458c, 8010459c, 80104724, 8010496c. Die zwei zusaetzlichen liegen in den selbst dekodierten Tabelleneintraegen 0x8010582C[3] = 0x801046AC und [4] = 0x801048F4.

### GEKIPPT: EM36 — 'startet schlafend, +0x10E = 0x8000' plus 'kein Schreibzugriff auf +0x10E im ganzen Overlay' macht den Boss nach Aktenlage dauerhaft untreffbar (offene Luecke, nicht geschlossen)
Die Unverwundbarkeit bei HP<0 wurde nur behauptet; ich habe den Beleg gefunden — und derselbe Gate-Block killt die Boss-Erzaehlung. Die EXE prueft vor dem Schaden nicht nur HP < 0, sondern auch (+0x10E & 0xC000) != 0 und ueberspringt dann den Treffer komplett. EM36 spawnt mit +0x10E = 0x8000, und ein eigener Store-Scan ueber den gesamten EM36-Chunk findet NULL Schreibzugriffe auf Offset 270/271. Damit waeren HP 600/400, Zustand 2, die Stagger-Tabelle und BEIDE HP-Stores in Zustand 3 unerreichbar. Entweder loescht ein externer Schreiber Bit 0x8000, oder der Startwert stimmt nicht. Vor dem Port zu klaeren, nicht zu raten.

**Gegenbeleg:** Applier-Gate: @0x80047148 lh v0,342(s0) / @0x8004714c nop / @0x80047150 bltz v0,0x8004740c / @0x80047158 lhu v0,270(s0) / @0x80047160 andi v0,v0,0xc000 / @0x80047164 bne v0,zero,0x8004740c. Identischer Zwilling im Schuss-Aufloeser: @0x80041280 lh v0,342(s2) / @0x80041288 bltz v0,0x80041300 / @0x80041290 lhu v0,270(s2) / @0x80041298 andi v0,v0,0xc000 / @0x8004129c bne v0,zero,0x80041300. Store-Scan auf Offset 270/271: EM36_ai1 = 0 Treffer; EM37_ai1 = 2 Treffer (@0x801008AC sh 8 -> Kind-Entity, @0x8010524C).
