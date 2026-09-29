# Gegenprüfung — re_schaden_resolver.md (Runde 34, Granaten)

Stand: 2026-09-29, abgeschlossen (Urteil §6).
Prüfer: Skeptiker-Agent. Methode: jede Zeile selbst disassembliert (`re15_disasm.py` / `re2_disasm.py`),
Bytes selbst gelesen, Sprungziele selbst disassembliert. Decompilate nur als Lesehilfe.
Werkzeuge: `analysis/befunde_runde34_granaten/re_schaden_gegen_werkzeug/`, Ausgaben `build/r34g_schaden_gegen/`.

## Gliederung

1. Konstanten-Tabelle — Zeile für Zeile
2. Mechanismus-Behauptungen (Aufruf, Resolver, Gates, Tabellen, Spieler-Zweig, Sperre, Tester, KI-Tabellen, NPC)
3. Port-Abgleich-Behauptungen (Datei:Zeile)
4. Vollständigkeit — was der Bau braucht und im Dossier fehlt
5. OFFEN-Liste nach Prüfung
6. Urteil

---

## 1. Konstanten-Tabelle

Alle Disasm-Mitschnitte der Prüfung: `build/r34g_schaden_gegen/dis_*.txt` (selbst erzeugt).

| # | Zeile (Dossier) | Urteil | Eigener Beleg |
|---|---|---|---|
| K1 | Explosions-Radius a0 = 500 @0x80018598 | **bestätigt** | `80018598: ori a0,zero,0x1f4` (dis_8001854c.txt) |
| K2 | Punkt Y = slot.y − 500 @0x800185a0-ac; X `lh 40(v1)` @0x80018594, Z `lh 44(v1)` @0x800185b0 | **bestätigt** | `800185a0: lh v0,42(v1)` / `800185a8: addiu v0,v0,-500` / `800185ac: sw v0,20(sp)`; Z-Store `800185bc: sw v0,24(sp)` liegt im **Delay-Slot** des `jal` @0x800185b8 → wird VOR dem Sprung ausgeführt, P ist vollständig. v1 = `lw 21188(0x800b)` @0x8001858c = laufender Slot. Alle drei `lh` = s16 vorzeichenrichtig |
| K3 | Angriffsart 2 @0x800185b4 | **bestätigt** | `800185b4: ori a2,zero,0x2` |
| K4 | Resolver-Bild Zünder == 7, genau einmal | **bestätigt** (+ Ergänzung) | `80018560: lhu v1,30(a1)` / `8001856c: ori v0,zero,0x7` / `80018570: bne v1,v0,0x800185f4` / `jal 0x80012d60` @0x800185b8. Ergänzend selbst belegt, dass die 7 sicher durchlaufen wird: Routine 30 (Start) setzt Zünder **0x2a = 42** (`80018474: ori v0,zero,0x2a` / `8001847c: sh v0,30(v1)`), A := 0 / B := 29 (`80018470: sh v0(=0x1d),2(v1)`, `80018478: sh zero,0(v1)`); Routine 29 (Flug, läuft als B über `jalr` @0x8001a2d4) fasst slot+0x1e nicht an und schaltet bei `slot+0x2a > 0` auf A := 31 (`80018378: ori v0,zero,0x1f` / `8001837c: sh v0,0(v1)`), B := 0; Routine 0 = `jr ra` (@0x80017248). Einziger Abzug: `8001867c: addiu v0,v0,-1` / `80018684: sh v0,30(v1)` (Delay-Slot des `j` @0x80018680). Routine 29 läuft als B im zweiten Durchlauf des Landebilds N; Routine 31 läuft ab Bild N+1 im ersten Durchlauf mit Zünder 42 → der Resolver fällt in den **36. Aufruf = Bild N+36**, genau einmal |
| K5 | Schaden Art 2 = 1000 @0x8006f41c | **bestätigt** | `bytes 0x8006f418`: `0a 00 14 00 e8 03 …` → [2] = `e8 03` |
| K6 | Reaktion Art 2 = 9 @0x8006f432 | **bestätigt** | `8006f430: 03 03 09 0a 0b 0e 0f 10 11 12 14` |
| K7 | Art 3/4: 1000/10, 1000/11 | **bestätigt** | wie K5/K6; Port `re15_damage.c:41-58` bytegleich (selbst gelesen) |

Zensus `jal/j/Datenwort/lui+addiu-Paar` auf 0x80012d60 über PSX.EXE + DEBUG.BIN + STAGE1..6 + TITLE
(eigenes Werkzeug `re_schaden_gegen_werkzeug/zensus.py`, Ausgabe `zensus_80012d60.txt`):
**genau zwei** `jal` (@0x80018008, @0x800185b8), kein `j`, kein Zeigerwort, kein Adressbau. **bestätigt.**
Schreiber von 0x800b52c4 (alle `sb/sh/sw` mit Imm 0x52c4 + Adressbau-Suche): nur @0x80019e58/eb8/ecc und
@0x8001a494, alle in FUN_80019e20. **bestätigt.** Routine 31 steht nur in `0x80071d40[31]` (@0x80071dbc),
die Tabelle wird nur in FUN_80019e20 adressiert (@0x80019e5c, @0x80019f1c, @0x8001a2c0).

### 1b. Resolver-Konstanten (FUN_80012d60, selbst disassembliert `dis_80012d60.txt`)

| # | Zeile (Dossier) | Urteil | Eigener Beleg |
|---|---|---|---|
| K8 | Gegner-Trefferspalte +0x6 := 1 @0x80012fd0-d8 | **bestätigt** | `80012fd0: ori v0,zero,0x1` / `80012fd8: sb v0,6(s1)`; erreichbar nur über `80012fc0: beq v0,zero,0x80012fd0` (Bit 0 frei). Der Delay-Slot `80012fc4: ori v0,v1,0x2` läuft auch hier, v0 wird aber @0x80012fd0 überschrieben — kein `\|= 2` auf dem Schadenspfad |
| K9 | Gegner +0x7 := 0 @0x80012fd4 | **bestätigt** | `80012fd4: sb zero,7(s1)` |
| K10 | Art-<2-Tor @0x80012e58 / @0x80012f68 | **bestätigt** | Spieler: `80012e58: sltiu a0,a0,0x2` → `80012e68: beq a0,zero,0x80012ebc` (Delay `ori v0,zero,0x2` = der spätere +0x04-Wert). Gegner: `80012f68: sltiu v0,s0,0x2` / `80012f6c: beq v0,zero,0x80012f7c` (Delay `nop`) überspringt `jal 0x800453d0` + `ori a0,zero,0xa`. Art 2 → kein SE 10, keine zwei `jal 0x8001af20` |
| K11 | Abstand waagrecht < r_ziel + 500 (streng), vorher \|dx\|,\|dz\| ≤ R | **bestätigt** | `8002b6fc: andi v0,a3,0xffff` / `8002b700: andi s1,s7,0xffff` / `8002b704: addu s0,v0,s1` (R = r_ziel + a0) / `8002b708: sll a2,s0,1` / `8002b720: addu v0,a1,s0` / `8002b724: sltu v0,a2,v0` (verwirft bei (u32)(dx+R) > 2R) / dz ebenso @0x8002b744-4c / `8002b764: jal 0x80065f60` mit `addu a0,v0,a0` im Delay-Slot / `8002b76c: subu s0,s0,v0` / `8002b770: blez s0` (verwirft bei R − dist ≤ 0) |
| K12 | Höhenband \|P.y − (e.y + O.y)\| < 500 + h (streng) | **bestätigt** | `8002b778: lhu v0,8(s2)` / `8002b780: addu a2,s1,v0` / `8002b784: lh v0,2(s5)` + `8002b77c: lw v1,56(s3)` / `8002b790: subu a0,a0,v0` / `8002b794: subu v0,zero,a2` / `8002b798: slt v0,v0,a0` / `8002b79c: beq v0,zero,0x8002b7b0` (→ Rückgabe fp = 0) / `8002b7a0: slt v0,a0,a2` / `8002b7a4: beq` / `8002b7ac: ori fp,zero,0x1`. Beide Grenzen streng |
| K13 | Gate A: e+0x188+0x40 == slot+0x74 | **bestätigt** (Bytes) | `80012f38: lui v1,0x800b` / `80012f3c: lw v1,21188(v1)` / `80012f40: lw v0,392(s1)` / `80012f44: lw v1,116(v1)` / `80012f48: addiu v0,v0,64` / `80012f4c: beq v0,v1,0x8001302c`. Sprungziel 0x8001302c liegt HINTER `80013024: addiu s4,s4,1` → übersprungene Ziele zählen nicht. slot+0x74 = a2 des Spawns: `8001980c: sw a2,116(t0)` (Delay-Slot des `j` @0x80019808, läuft also auch im Versatz-Pfad) bzw. `8001981c`. Granaten-Spawn a2 = `lw a2,-13348(a2)` (= [0x800acbdc] = [Spieler+0x188]) + `addiu a2,a2,1956` (0x7a4) @0x800336d4/f0, @0x80033734/4c, @0x80033790 ff. Weitere `sw …,116(…)`-Schreiber im ESP-Bereich: @0x80019ae0/af0 (Schwester-Spawner 0x800199d4, gleiche Semantik) und @0x80017004/0x8001701c (anderes Objekt, nicht Routinen 29/30/31). Für die Granate also keine Gleichheit mit einer Gegner-Matrix möglich → **attacker_slot = −1 richtig** |
| K14 | Gate B: (e+0x93 & 3) == 3 → überspringen | **bestätigt** | `80012f54: lw v0,144(s1)` / `80012f58: lui v1,0x300` / `80012f5c: and v0,v0,v1` / `80012f60: beq v0,v1,0x8001302c`. Wort +0x90 little-endian → Bits 24/25 = Byte +0x93 Bits 0/1. Laufzeit-Gegenprobe (eigenes `ss_ents.py`): +0x90-Wort der Spinnen/NPCs = 0x00000100/0x200/… (Byte +0x91 = Platzindex), oberstes Byte = +0x93 |
| K15 | Seiten-Test ((w − yaw + 0x400) & 0xfff) < 0x800 = Punkt HINTEN | **bestätigt** | FUN_8001a7a8: `8001a7c4: lh a2,52(s0)` / `8001a7c8: lh a3,60(s0)` / `8001a7cc: jal 0x8001a6d4` (a0 = P.x, a1 = P.z, s16) / `8001a7d8: lh v1,106(s0)` / `8001a7e0: subu v0,v0,v1` / `8001a7e4: addiu v0,v0,1024` / `8001a7e8: andi v0,v0,0xfff` / `8001a7ec: slti v0,v0,2048`. FUN_8001a6d4(sx,sz,dx,dz) = Winkel von (dx−sx, dz−sz) (`8001a6f0: subu s0,a2,a0`, `8001a704: subu a0,a3,a1`); FUN_8001aac4 dreht +0x6a auf genau diesen Winkel Selbst→Ziel (`8001aaf8: jal 0x8001a6d4` mit a0/a1 = eigene Lage, `8001ab54: sh a0,106(v1)`). Also 1 ⇔ Vektor P→Figur ∥ Blickrichtung ⇔ P hinter der Figur. Gegenprobe an den Handlern: Zustand 2 schiebt mit `ori a0,zero,0x800` (@0x80035f18/1c, rückwärts), Zustand 3 mit `addu a0,zero,zero` (@0x8003609c/a0, vorwärts) |
| K16 | Spieler-Hitbox r 450 / h 1530 / Versatz (0,−1530,0) | **bestätigt** | `bytes 0x80073e94`: `00 00 06 fa 00 00 c2 01 fa 05 c2 01`; Zeiger `0x80073ea0: 94 3e 07 80`; `80031644: lw v1,16032(v1)` + `80031664: sw v1,-13620(at)` (+0x78) / `8003166c: addiu v1,v1,9044` + `80031674: sw v1,-13616(at)` (+0x7c = 0x800b2354, statisch 0, zur Laufzeit gefüllt — Schreiber siehe §2/O1). Laufzeit (eigenes `ss_ents.py`): (0,−1530,0) |

### 1c. Restliche Tabellenzeilen

| # | Zeile (Dossier) | Urteil | Eigener Beleg |
|---|---|---|---|
| K17 | Hunde-Hitbox Sektor 900/720/450, Versatz (0,−720,0), Box @0x80120f64, INIT @0x8010da70 | **bestätigt** (Versatz jetzt statisch belegt, s. O1) | `bytes 0x80120f60 --bin STAGE1.BIN`: `… 00 00 30 fd 00 00 84 03 d0 02 c2 01` ab 0x80120f64 → {0,−720,0, r1 900, h 720, r2 450}; Zeiger `0x80120f70: 64 0f 12 80`; `8010da68: lw v0,3952(v0)` / `8010da70: sw v0,120(v1)`. Laufzeit-Dispatch (4 STAGE1-Saves, eigenes Skript) 0x80072bac[0x20] = **0x8010d7f8**; diese Wurzel ruft nach dem Zustands-`jalr` (@0x8010d860, Tabelle 0x80120f74, [0] = INIT 0x8010d93c) **jeden Tick `jal 0x8002b498`** (@0x8010d870). Box-Bytes auch in STAGE3 @0x8011ea7c, sonst in keiner Stage. RAM == BIN für 0x80120f64..+0x14 in 4 Saves |
| K18 | Zombie-Hitbox 400/1440, Versatz (0,−1440,0), Box @0x8011f778, INIT @0x80100778 | **bestätigt** | `8011f778: 00 00 60 fa 00 00 90 01 a0 05 90 01`; Zeiger `0x8011f790 -> 0x8011f778`; `80100770: lw v0,-2160(v0)` / `80100778: sw v0,120(v1)` (Delay-Slot von `jal 0x8001af20`). RAM (room1140_entry, mzd_stage1_engage_live): Box 0x8011f778, Versatz (0,−1440,0) |
| K19 | Spieler-HP Start 100 @0x80031710/18 | **bestätigt** | `80031710: ori v0,zero,0x64` / `80031718: sh v0,-13586(at)` (0x800acaee) |
| K20 | Spieler-Tod Clip 7 @0x80036778/80 | **bestätigt** | `80036778: ori v1,zero,0x7` / `80036780: sb v1,-13592(at)` (+0x94); dazu `80036790: sb v1,-13597(at)` (+0x8f := 7), `80036788: sb zero,-13591(at)` (+0x95), `80036798: sh zero,-13600(at)` (+0x8c/+0x8d) |
| K21 | Spieler-Tod SE 0x04030001 | **bestätigt** | `80036744: lui a0,0x403` steht im **Delay-Slot** von `80036740: beq v1,zero,0x80036764` (läuft also auf dem Phase-0-Pfad), `80036764: ori a0,a0,0x1`, `800367a8: jal 0x80045024` mit `800367ac: addiu a1,a2,46` (= 0x800aca88, Spielerlage) im Delay-Slot. Phase 2: `80036804: jal 0x80045630` (a0 = 2 aus dem Delay-Slot @0x80036758, a1 = 0 @0x80036808), `80036814: sh v0(=7),-13736(at)` = **Halbwort** → +0x04 := 7 UND +0x05 := 0 |
| K22 | Reihenfolge Gegner → Spieler → ESP | **bestätigt** | `8001ce04: jal 0x8001a50c` / `8001ce0c: jal 0x80031c44` / `8001ce14: jal 0x8002bd44` / `8001ce1c: jal 0x800436a8` / `8001ce24: jal 0x8004f0b0` / `8001ce2c: jal 0x80019e20`. Innerhalb FUN_80019e20 läuft Routine A (31) im ersten Durchlauf (`80019e9c: jalr v0`), Routine B (29) erst im zweiten (`8001a2d4: jalr v0`) — der Resolver liest also die Slot-Lage, die der zweite Durchlauf im VORBILD geschrieben hat |
| K23 | Entlade 9/10/11 = nur Munition | **bestätigt** | `table 0x80074100`: [9] 0x80033b38, [10] 0x80033b58, [11] 0x80033b78; je `addiu sp,sp,-24 / sw ra / jal 0x8004eae4 / nop / lw ra / addiu sp / jr ra / nop` (@0x80033b40/60/80). Waffen-Dispatch `table 0x80074030` [9..11] = 0x80032e9c. Port-Zeile ist `game_step_common.c:1775` (Dossier: 1776 — 1 Zeile daneben, belanglos) |
| K24 | Zombie-Reaktionszeile 9 NULL (Hurt Sp0..7, Tod Sp0..3) | **bestätigt** | `table 0x8011fcb0 8 --bin STAGE1.BIN`: 8 × 0; `table 0x8011ffcc 8`: [0..3] 0, [4] 0x80107634, [5..7] 0. Adressrechnung aus den Wurzeln selbst: HURT `80105ae8: lui a0,0x8012` / `80105aec: addiu a0,a0,-1136` (0x8011fb90) / `80105af8: sll v1,v1,5` / `80105b00: sll v0,v0,2`; DEATH `80106bdc: addiu a0,a0,-340` (0x8011feac), `80106c00: jalr v0`. Zeile 9 = +0x120. RAM == BIN (0x8011fb90+0x280, 0x8011feac+0x2a0) in 4 STAGE1-Saves. Liegend-Weiche `80106bbc: andi v0,v0,0x80` (+9) → `jal 0x80107cb0` bestätigt |

**Ergebnis Konstanten-Tabelle: 24/24 Zeilen bestätigt, 0 widerlegt.** Keine falsche Adresse, kein falsches Vorzeichen,
kein übersehener Delay-Slot, keine falsche Binärdatei gefunden.

---

## 2. Mechanismus-Behauptungen (was in Code wandern wird)

| # | Behauptung | Urteil | Eigener Beleg / Korrektur |
|---|---|---|---|
| M1 | DAT_800b52c4 = laufender ESP-Slot, Etikett „attack_workstruct" falsch | **bestätigt** | Schreiber nur in FUN_80019e20 (s. §1); Pool 0x800a73b8, Ende `80019e50: addiu s2,s0,12672` (+0x3180), Schritt `80019eb0: addiu v0,v0,132`. In JEDEM geprüften Save steht 0x800b52c4 = 0x800aa538 = Pool-Ende (Schleife fertig) — passt |
| M2 | „0x800b5358 := 1 (Lärm-Latch)" (§1, Z. 35) | **widerlegt (Etikett)** | Einziger Leser `8001ce60: lbu v0,21336(v0)` im Hauptlauf NACH dem ESP-Tick. Er sichert per `jal 0x8004ee38` (memcpy, `ori a2,zero,0x28` im Delay-Slot @0x8001ce6c) die Lichtzeile der aktiven Kamera (`lw 44([0x800ac778])` + 40·`lh 0x800b0fe4`), schreibt ein Licht (+3 := 0 @0x8001cef8; Farbe ≥ 0xd2/0x8c/0x50 @0x8001cf28/64/a0; Lage = Spieler + (1200 gedreht mit Spieler-Gier, `ori v0,zero,0x4b0` @0x8001cebc / `jal 0x8004f008` @0x8001cecc), Y = Spieler-Y − 800 @0x8001d018; +38 := 0x1770 @0x8001d080-84), zeichnet die Figuren (`jal 0x8001e8c8`), stellt die Zeile zurück (`8001d1ac: jal 0x8004ee38`) und **löscht den Latch** (`8001d16c: lui s0,0x800b` / `8001d170: addiu s0,s0,21336` / `8001d1b4: sb zero,0(s0)`). Also ein **Ein-Bild-Lichtblitz vor dem Spieler**, kein Lärm. (Deckt sich mit `re_wurf_flug_explosion.md` §6; die dort und in `port_inventar_baseline.md` O9 offene Löschstelle ist hiermit **@0x8001d1b4** belegt.) Für den Resolver ohne Belang |
| M3 | Nur Zweig Zünder==7 ruft den Resolver | **bestätigt, Listing unvollständig** | Derselbe Zweig spawnt danach `0x03195000` am Punkt P (`800185c0: lui a0,0x319` / `800185c4: ori a0,a0,0x5000` / `800185dc: jal 0x800199d4`, a3 = sp+16 = &P im Delay-Slot) und spielt SE `0x04080001` an P (`800185e4-ec`: `lui a0,0x408` / `ori a0,a0,0x1` / `jal 0x80045024`, a1 = &P). Das Dossier-Listing §1 bricht vor diesen Zeilen ab; für die Resolver-Anzahl egal, für den Bau (Reihenfolge Resolver → Kind → SE im selben Bild) wichtig |
| M4 | Schleife 1 zählt nur aktive Plätze, Stride 500, Spieler immer, Schleife 2 rückwärts | **bestätigt** | `80012dc0: beq v0,zero,0x80012df4` überspringt `80012dc8: addiu s1,s1,-1`; `80012dfc: addiu s0,s0,500` (Delay-Slot); Spieler @0x80012e00-10 unabhängig von s1; Schleife 2 `80012f24: addiu s2,s2,-1` … `8001302c: bne v0,zero,0x80012f28` / `80013030: addiu s2,s2,-1`. Rückgabe (`80013038: andi v0,s4,0xff`) wird von Routine 31 **nicht** gelesen (@0x800185c0 überschreibt a0, v0 unbenutzt) |
| M5 | Gegnerzweig: +0x93 &= 1, Seite → Bit 0x80, gesperrt → \|= 2 ohne Schaden, sonst +7/+6/+5/HP/+0x93\|=1/+4 = 2 bzw. 3 | **bestätigt** | @0x80012f7c-0x80013020 wie im Dossier; HP-Test signiert (`80013004: lh v1,154(s1)` / `80013014: bgez v1,…`), +0x04 := 2 steht im Delay-Slot `80013018` und läuft immer |
| M6 | Spielerzweig: HP −1000, +4 = 2, +5 = 2 + Seite, +6 = 0, +0x93 \|= 1 (immer), HP < 0 → 3/0/0 | **bestätigt** | `80012ebc: sb v0,4(s1)`, `80012ed0: addiu v0,v0,2` / `80012ed4: sb v0,5(s1)`, `80012ee0: sb zero,6(s1)`, `80012eec: sb v0,147(s1)` im Delay-Slot von `80012ee8: bgez v1`, `80012ef4/f8/fc`. HP = 0 zählt als lebend (bgez) |
| M7 | Sektor-Radius r1 längs Blickrichtung, r2 quer | **bestätigt** | `jal 0x800683e8` (rsin, Vorzeichenweiche `800683ec: bltz a0`) bei r2 ≥ r1, `jal 0x80068348` (rcos: `andi a0,a0,0xfff`, Tabelle `lh -9468(at)` = 0x8007db04 mit Index 0x400 − a) bei r2 < r1 — beide ergeben r1 bei w = 0 und r2 bei w = 0x400. FUN_80065de0 hat die Struktur von PsyQ `ratan2(y,x)` (Vorzeichen-Flags @0x80065de4-e00, beide 0 → 0 @0x80065e14, Quotient `(y<<10)/x` bzw. `(x<<10)/y`, Tabelle 0x80078cc0); Quadranten-Abbildung nicht nachgeprüft (O4 bleibt teilweise offen) |
| M8 | Sperre verhindert Schaden an Spielern im Taumel/Griff/Tod | **bestätigt + ERWEITERT** | Laufzeit (eigenes `ss_ents.py`): Modus 5 (3 Saves), 6, 7 → +0x93 = 0x01. **Zusätzlich** hält das Original Bit 0 im **Modus 1** während Treppe/Klettern: Schreiber liegen in den Modus-1-Unterzuständen (Dispatch `80031ee0`/`80031f08` über 0x80073fb0/0x80073ff0 auf +0x05): Treppe hoch [11] Phase 2 setzt `80038a58: sb v0,-13593(at)` (Phasentabelle 0x80010c0c: [2] = 0x80038a00), Phase 4 löscht `80038c24`; Treppe runter [12] setzt `80038d00`, löscht `80038ec0`; Hochklettern [9] setzt `80038180` (bei +0x95 == 0x11, `80038160: ori v0,zero,0x11`), löscht `80038258`; Herunterspringen [10] setzt über den Adressbau @0x800383dc, löscht `80038754`; [13] setzt `80038fcc`, löscht `80039078`. → Ein Spieler auf der Treppe wird von der Explosion **nicht** getroffen (O3 damit geschlossen) |
| M9 | FUN_80011f50 nie mit 9/10/11, Tester 0x800128A0 nur über 14 lebendig | **bestätigt** | 11 `jal`-Stellen (eigener Zensus identisch), a0 überall `lbu a0,-13731(a0)` (0x800aca5d): @0x8003354c, @0x80033878, @0x80033964, @0x80033a2c, @0x80033b08, @0x80033c48, @0x800349e4, @0x80034c1c, @0x800353ac, DEBUG.BIN @0x800c479c; @0x80012418 rekursiv mit `addu a0,s0,zero`. Gates: Schrot `8003350c-14` (== 8), Entlade-Tabelle 0x80074100 [9..11] = Munitions-Stubs, Waffen-Dispatch 0x80074030 [9..11] = 0x80032e9c. Tester-Tabelle 0x8006e548 [9..11],[14..18],[20] = 0x800128a0 |
| M10 | FUN_80012d60 hat keinen Typfilter, FUN_80011f50 schon | **bestätigt** | FUN_80011f50: `80012178: lbu v0,8(s1)` / `80012180: sltiu v0,v0,0x40` / `80012184: beq v0,zero,0x80012540`. FUN_80012d60 prüft nur Wort0 Bit 0 + FUN_8002b5d0 + Gate A/B (Volldisasm) |
| M11 | NPC (≥ 0x40) nach Explosion: Zustand 3, +5 = 9, +6 = 1 → 0x80050DDC | **bestätigt + präzisiert** | RAM: NPC-HP −1, +0x93 0 (0x40/0x42/0x47/0x4b). Alle sechs STAGE1-NPC-Wurzeln haben in Zustand [3] dieselbe +0x6-Tabelle {0x80050cb8, **0x80050ddc**, 0x80050f00, 0x80051024} (0x80121630/700/768/7d0/8a0/988, selbst gelesen). 0x80050ddc mit +6 = 1: `jal 0x8001f314` auf Bank (+0x84, +0x16c) @0x80050e70, Ende → `80050ec8: sb v0(=2),6(v1)` (bei +0x1c4 & 4 zurück auf 1 @0x80050eec). Mit +6 = 2 läuft 0x80050f00, liest +6 erneut (`80050f10: lbu v1,6(a0)`) und springt bei 2 sofort nach 0x80051014 (`jr ra`) → das NPC spielt seinen laufenden Clip EINMAL zu Ende und **steht dann still in Zustand 3**, bis ein Skript +4/+5/+6 neu schreibt. Das engt O2 auf die Skript-Frage ein |
| M12 | Zeile 9 in allen 26 2D-Tabellen NULL | **teilweise geprüft** | Selbst geprüft nur die Zombie-Tabellen STAGE1 (K24). Die Querschnittsbehauptung über 26 Tabellen stammt aus `tab2d_suche.py` und ist nicht wiederholt. Gegenbeispiel-Klasse gefunden: Typ **0x26** (STAGE1 Feuer-Emitter, Wurzel 0x80116288, Zustände 2/3/4 → 0x8011697c → `table 0x80121290`[+5]) hat für +5 = 9 einen **echten Handler 0x80116a04** (kein NULL). Das ist keine 2D-Tabelle und widerspricht der Aussage nicht, zeigt aber: „Zeile 9 fehlt" gilt nicht für alle Typen (Schwester-Dossier `re_gegner_bosse_sonstige.md` §2.10 deckt das) |
| M13 | Gegner-Löscher +0x93: Zombie @0x80105fa4, 0x80106b8c-90; Hund `+0x93 = 0` @0x80110b70 / @0x80110d90 | **Zombie bestätigt, Hund-Adressen ungenau** | Zombie: `80105fa4: andi v0,v0,0xfe` / `80105fac: sb v0,147(v1)` (Delay-Slot von `jal 0x8001af20`), `80106b8c: andi` / `80106b90: sb`. Hund: an @0x80110b70/@0x80110d90 steht `lw v0,-14460(v0)` (Blockanfang); die Stores sind **`80110b88: sb zero,147(v0)`** und **`80110dac: sb zero,147(v0)`**. Inhalt stimmt, Adresse nicht |
| M14 | Spieler-Löscher @0x80031964 = Modus-0-Einmal-Init | **bestätigt** | `table 0x80073f90`[0] = 0x800318f8; darin `8003192c: sw v0(=1),-13736(at)` (Wort +0x04..07 := 1 → Modus 1) und `80031964: sb zero,-13593(at)`. Modus 0 läuft also genau ein Bild |
| M15 | „Schaden 1000 gegen Zombie-HP 81..105 → **immer** Zustand 3" (§8.1) | **widerlegt (zu pauschal)** | Gilt nur bei freiem +0x93 Bit 0. Der liegende Fress-Zombie Typ 0x16 in ROOM1140 (+9 = 0x88) steht in **16 von 16** STAGE1-Saves mit ihm (eigenes Skript, u.a. room1140_entry, mzd_stage1_briefing_live, mzd_stage1_engage_live) auf **+0x93 = 0x01**, HP 97. Im Resolver: `80012fbc: andi v0,v1,0x1` / `80012fc0: beq` nicht genommen → `80012fcc: sb v0(\|2),147(s1)` → **kein Schaden**, nur Bit 1 (Gore-Auslöser); eine zweite Explosion überspringt ihn über Gate B. Ebenso parity_turn_R2: liegender 0x10 (+9 = 0x80, +5 = 17) mit +0x93 = 0x01. Die Aussage „nur ein liegender Zombie stirbt sauber über FUN_80107cb0" gilt entsprechend nur für liegende Zombies mit freiem Bit 0 |
| M16 | „+0x90 = 0x01000400" beim 0x16 (§2.4) | **bestätigt (Wert je Save)** | room1140_entry / engage_live: +0x90 = **0x01000300** (Byte +0x91 = Platzindex); oberstes Byte 0x01 = +0x93 wie behauptet |

---

## 3. Port-Abgleich — Datei:Zeile selbst nachgelesen

| # | Dossier | Urteil | Befund |
|---|---|---|---|
| P1 (A1) | `game_step_common.c:1776` `ENT[9] = {1,1,1,0}` | **bestätigt** (Zeile **1775**) | `[9] = {1,1,1,0, {{0}}},` steht auf 1775; Kommentar darüber nennt es selbst PORT-BRÜCKE; Aufruf `re15_player_weapon_fire(eq_item)` :1809 |
| P2 (A2) | R31 fehlt in `re15_esp.c` | **bestätigt** | `esp_fx_dispatch` (:523) kennt die Fälle 0/3/4/5/8/9/10/11/15/16/17/18/38, kein 29/30/31 |
| P3 (A3) | Gate A = Slot-Gleichheit (`re15_damage.c:3332`) | **bestätigt** | `if (slot == attacker_slot) continue;` |
| P4 (A4) | Gate B fehlt, Kommentar falsch (`re15_damage.c:3334-3339`) | **bestätigt** | Kommentar „terminal flags … inert … OMITTED"; danach `e->hit_react &= 1;` ohne Probe |
| P5 (A5) | `re2z_row_from_atktype[2] = 17` (:3847), Klammer 0, Zone aus Zielhöhe (`re15_damage.c:2934-2940`) | **bestätigt** | `enemy_ai_re2_zombie.c:3847` `{ 1, 1, 17, 17, 17, 9, 9, 10, 11, 17, 1 }`; `re2z_row_from_weapon[9] = 9` (:3831); `re15_damage.c:2932-2940` Trefferbox-Pfad `hits1d2 = (liegt \|\| elev < 0) ? 0 : 1` (ohne `+3·Klammer`) |
| P6 (A6) | Peilquelle Spieler (`re15_damage.c:2986`) | **bestätigt** | `re15_re15_re2z_gore_hit(e, &g_actors[RE15_ACTOR_SLOT_PLAYER], 1, (unsigned)type);` |
| P7 (A7) | Hund `r = 500, h = 600` (`re15_damage.c:3508`) | **bestätigt** | `case 0x20: r = 500;  h = 600;` mit „faithful-line"; Soll 900/720/450 + Versatz −720 (K17). ⚠ Risiko für den Bau: der Sektor-Radius hängt an `target->rot_y` (`re15_hitbox_test` → `re15_ellipse_radius(…, rot_y, dz, dx)`); das Original rechnet mit dem ROHEN +0x6a (`8002b650: lhu v1,106(s3)`). Läuft der Hund im Port über die RE2-KI, muss vor dem Einsetzen der Sektor-Box geprüft werden, dass dessen `rot_y` dieselbe Konvention trägt, sonst tauschen 900/450 die Achsen |
| P8 (A8) | RE1.5-Tod ignoriert +0x5 (`enemy_ai_common.c:5542-5580`) | **bestätigt** | `re15_enemy_ai_live_death` wählt nur nach +9 & 0x80 und +0x93 & 0x80 (Clip 0x0b/0x0d/0x1f); +0x6 wirkt nur auf die Blutspritzer-Wahl |
| P9 (A9) | ESP-Tick `main.c:5413` vor `re15_game_step` `main.c:7358` | **bestätigt** (Pfad `platform/pc/main.c`) | 5413 `re15_esp_fx_tick(re15_esp_room_bank());`, 7358 `re15_game_step(&gctx);` |
| P10 (A10) | `pl->hit_react = 0` jedes Normalbild (`game_step_common.c:1520`) | **bestätigt, Folge unterschätzt** | Siehe §4 V1: das Original hält Bit 0 in Modus 1 auf der Treppe; der Port-Treppenzweig (:1383-1391, `re15_stair_tick`) setzt es nie (`grep hit_react stair_common.c` = leer). Klettern/Springen sind im Port dagegen richtig (eigener Zweig :1373-1381, Setzer/Löscher `climb_common.c:406/421/471/534`) |
| P11 (A11) | Treppen-Gate beim Tod (`game_step_common.c:1296-1298`) | **bestätigt, Deutung unvollständig** | Im Original erreicht der Resolver den Spieler auf der Treppe gar nicht (Bit 0, §2 M8). Der Port zieht dort 1000 HP ab und setzt `state = 3` (`re15_player_take_damage`, `re15_damage.c:203-237`), der Tod-Auslöser ist aber durch `!re15_stair_active()` gesperrt, und `s_prev_hp` wird im selben Bild auf den negativen Wert gesetzt (:1300) → der cmd-3-Auslöser (`s_prev_hp >= 0`) feuert auch nach der Treppe **nie mehr**. Folge: der Spieler steigt mit HP < 0 die Treppe zu Ende, danach greift der Tot-Zweig (:1394, `re15_player_is_dead()` = HP < 0) mit der Game-Over-Kette, aber **ohne** Todes-Clip 7 und ohne SE 0x04030001 (die startet nur `re15_player_death_cmd3`). Original: kein Schaden, weiterleben. Das ist KEINE „kleinere Abweichung ohne Folge für die Granate" (Dossier §PORT-ABGLEICH Punkt 10) |
| P12 (A12) | Flinch-Richtung über FUN_8001a780 (:1270-1284) | **bestätigt** | Zeilen 1267-1285 |
| P13 (A13) | NPC `default:` hält Idle (`enemy_ai_common.c:10550`) | **bestätigt** | `default:` ab 10549: `re15_npc_clip(e, 2)` falls der Clip zu kurz ist + `re15_npc_anim(e)`. Original (M11): laufenden Clip einmal zu Ende, dann Stillstand |
| P14 | „`re15_hitbox_overlap` … bytegleich" | **bestätigt** | `re15_damage.c:3243-3264`: `R = (radius&0xffff) + (atk_radius&0xffff)`, `(uint32_t)(dx + R) > (uint32_t)(R * 2)`, `R - dist <= 0`, `h = atk_radius + height`, `-h < dy && dy < h` |
| P15 | „`hit_from_front` … Formel gleich" | **unklar** | Gleichheit gilt nur, wenn `rot_y` des Ziels = rohes +0x6a ist (Port: `rel = atan2_port − (rot_y + 1024) + 0x400`, `atan2_port = fa6d4 + 0x400` → `fa6d4 − rot_y + 0x400`). Der Port führt je Typ verschiedene Gier-Konventionen (`enemy_ai_common.c:4519`: Zombie-Bogen `ang − (rot_y + 1024)`). Für die Granate folgenlos (tödlich), für Art 0 nicht geprüft |

---

## 4. Vollständigkeit — was der Bau braucht und im Dossier fehlt oder nur behauptet ist

| # | Mechanismus | Stand im Dossier | Beleg (selbst) | Bedeutung für den Bau |
|---|---|---|---|---|
| V1 | **Treppen-Unverwundbarkeit des Spielers** (Modus 1, Unterzustände 9–13 halten +0x93 Bit 0) | als O3 „nicht zugeordnet", A10/A11 „ohne Folge für die Granate" | Setzer/Löscher §2 M8: Treppe hoch Phase 2 `80038a58` / Phase 4 `80038c24` (Phasentabelle 0x80010c0c), Treppe runter `80038d00` / `80038ec0`, Klettern `80038180` / `80038258`, Springen `8003841c` (Basis `800383dc-e0`) / `80038754`, [13] `80038fcc` / `80039078`; Dispatch `80031ee0` (0x80073fb0) / `80031f08` (0x80073ff0) auf +0x05 | Port-Treppenzweig (`game_step_common.c:1383-1391`, `stair_common.c`) setzt `hit_react` nie → Explosion trifft den Spieler auf der Treppe, zieht 1000 ab, der cmd-3-Auslöser ist gesperrt und feuert danach nie (s. P11). Fix-Richtung: Bit 0 in `re15_stair_tick` an den Original-Stellen setzen/löschen, NICHT den Todes-Auslöser gaten |
| V2 | **Schreiber des Hitbox-Versatzes +0x7c** = FUN_8002b498 | O1 offen | `8002b4bc: lw s0,120(s1)` / `8002b4c0: lw s2,124(s1)` / Drehmatrix aus (0, +0x6a, 0) `jal 0x80068098` @0x8002b4d4 / `jal 0x800661c0` @0x8002b4f4 auf (Box.x, ·, Box.z) / `8002b504: sh v0,0(s2)` (gedrehtes x) / `8002b508-10: lhu v0,2(s0)` → `sh v0,2(s2)` (Box.y **ungedreht**) / `8002b51c: sh v0,4(s2)` (gedrehtes z) / `8002b520: sb zero,450(s1)`. Aufgerufen JEDES Bild aus den Wurzeln (eigener Zensus: Spieler @0x80031d58, STAGE1 Zombie @0x801005f4, Hund @0x8010d870, 0x26 @0x8011630c, dazu alle anderen Stages). Einziger `lw …,124(…)`→Store-Pfad in allen Binärdateien (eigenes `ptr_store_scan.py`) | O1 ist damit **geschlossen**: Hund (0,−720,0) folgt statisch. Port setzt den Versatz fest auf (0, −h, 0) (`re15_damage.c:3552-3561`) → falsch für **Typ 0x26** (Box {0,0,0,600,720,600}, RAM-Versatz (0,0,0) in mzd_stage1_spider, Port −720: Höhenband um 720 verschoben) und für jede Box mit x/z ≠ 0 (Alligator x = 1000, im Schwester-Dossier `re_gegner_bosse_sonstige.md` Nachtrag). Beides fehlt im Resolver-Dossier |
| V3 | Liegende/gesperrte Gegner (+0x93 Bit 0 in Ruhe) | fehlt | M15 | Explosion tötet den ROOM1140-Fresser im Original NICHT; im Port gibt die RE2-Brücke (`enemy_ai_re2_zombie.c:8809-8815`, Spawn-Pose ausgenommen) Bit 0 frei → Port tötet ihn. Muss im Bau als bewusste Abweichung oder als Korrektur stehen |
| V4 | RE2-Ziel „Zeile 9, Klammer 1, Zone aus P" | nur aus Schwester-Dossier übernommen | Stichprobe RE2 selbst: `80020d54: lui a3,0x1002` / `80020d58: ori a3,a3,0x9` (Hitcode 0x10020009), `80047114: srl s6,s5,28` (Klammer), `80047324: sb s5,5(s1)` (Zeile), `80047330: sb v0,466(s1)` (Zone) — stimmt | Offene Bau-Fragen, die das Dossier nicht stellt: (a) In RE2 ist die Klammer an den Schaden gekoppelt (`(w0 >> 10·K) & 0x3FF`, Zombie K1 = 50 → meist HURT); mit dem RE1.5-Schaden 1000 landet jeder Gegner in DEATH, die RE2-HURT-Zellen der Zeile 9 sind unerreichbar — das ist eine Mischform und gehört als Entscheidung ins Bau-Dossier. (b) „Zone aus P": RE2 prüft an y und y+900 (Schwester §KONSTANTEN GL_EXPL_Y), RE1.5 hat nur P.y = slot.y − 500 — welcher Punkt die Zone speist, ist nicht festgelegt. (c) RE2-Explosion trifft nur den ERSTEN ungesperrten Gegner (Bit 0x10000 fehlt im Hitcode), RE1.5 alle im Radius — nach Beta→Retail bleibt RE1.5 (fertig), sollte aber ausdrücklich dastehen |
| V5 | NPC nach Explosion | O2 offen | M11: Clip einmal, dann Stillstand in Zustand 3 | Port-Default hält eine Idle-Schleife (`re15_npc_clip(e, 2)`); Original friert im letzten Bild des laufenden Clips ein. Skript-Wiedereinfang bleibt offen |
| V6 | Aufruf-Reihenfolge im Explosionsbild | teilweise | M3 | Resolver → Kind 0x03195000 an P → SE 0x04080001 an P, alles im selben Routine-31-Aufruf; Lichtblitz (0x800b5358) erst im Hauptlauf NACH dem ESP-Tick (@0x8001ce60) |

---

## 5. OFFEN-Liste des Ermittlers — Stand nach Prüfung

| # | Dossier | Nach Prüfung |
|---|---|---|
| O1 | Schreiber +0x7c | **geschlossen**: FUN_8002b498 (V2); Hund (0,−720,0) statisch belegt |
| O2 | NPC nach Zustand 3 | **eingeengt**: NPC-Seite statisch (M11); offen nur, ob das Skript das NPC neu setzt |
| O3 | Spieler +0x93 in 0x80038xxx | **geschlossen**: Modus-1-Unterzustände 9–13 (M8/V1) |
| O4 | FUN_80065de0 = ratan2 | **teilweise**: Struktur PsyQ-ratan2, rsin/rcos belegt (M7); Quadranten-Abbildung und Port-`re15_ratan2` nicht gegengeprüft |
| O5 | `jalr 0` auf der PSX | offen (für den Port ohne Belang, bestätigt) |
| O6 | Nicht-2D-Familien auf +5 = 9 | offen im Resolver-Dossier; 0x26 selbst gesehen (Handler 0x80116a04), Rest im Schwester-Dossier |
| O7 | Laufzeit-Zeitpunkt | statisch: Resolver im 36. Aufruf von Routine 31 nach dem Landebild (Zünder 42 → 7), Gegner lesen den Zustand im Folgebild (M1/K22); Messung fehlt weiter |

---

## 6. Urteil

**Die Tabelle „KONSTANTEN FUER DEN BAU" hält: 24 von 24 Zeilen selbst an den Bytes bestätigt** (Adresse,
Wert, Vorzeichen, Delay-Slots, Binärdatei). Der Kernmechanismus (einmaliger Aufruf aus Routine 31 bei
Zünder 7, Punkt 500 über der Liegestelle, Radius 500 additiv auf den Ziel-Radius, Gate A schließt bei der
Granate niemanden aus, Gate B = +0x93 Bits 0/1, Art 2 = 1000/9 ohne SE und Würfel, Spieler-Eigenschaden =
allgemeiner Tod Clip 7, kein Typfilter, Port-Brücke ENT[9].resolve muss weg) ist korrekt.

Widerlegt bzw. korrigiert:
1. Etikett **„Lärm-Latch" 0x800b5358** — es ist ein Ein-Bild-Lichtblitz vor dem Spieler (Leser @0x8001ce60,
   Löschung @0x8001d1b4). Für den Resolver folgenlos.
2. **„Zombie → immer Zustand 3"** — nicht bei gesetztem +0x93 Bit 0; der ROOM1140-Fresser (0x16) hat es in Ruhe
   (16/16 Saves) und bleibt im Original unverletzt.
3. **Port-Abweichungen A10/A11 „ohne Folge für die Granate"** — falsch: das Original macht den Spieler auf der
   Treppe (Modus-1-Unterzustände 11/12, ebenso 9/10/13) über +0x93 Bit 0 unverwundbar; der Port verliert dort
   1000 HP ohne Todes-Clip.
4. Hund-Löschadressen @0x80110b70/@0x80110d90 zeigen auf den Blockanfang; die Stores sind @0x80110b88/@0x80110dac.
5. Port-Zeile ENT[9] ist 1775, nicht 1776.

Geschlossen/eingeengt: O1 (FUN_8002b498 schreibt den Versatz, Hund belegt), O3 (Treppe/Klettern), O2 (NPC-Seite:
Clip einmal, dann Stillstand), O4 teilweise.

Nicht im Dossier, aber für „Schaden mit allen" nötig: V1 Treppen-i-Frames, V2 Versatz-Drehung + falscher
0x26-Versatz im Port, V3 gesperrte Liegende, V4 Mischform RE1.5-Schaden/RE2-Klammer + unbestimmter Zonenpunkt.

Stand: 2026-09-29, Prüfung abgeschlossen. Werkzeuge: `re_schaden_gegen_werkzeug/` (`zensus.py`,
`ptr_store_scan.py`, `ss_ents.py`, `ram_vs_bin.py`, `fn_start.py`); Mitschnitte `build/r34g_schaden_gegen/`.
