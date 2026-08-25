# 1210-ausloeser — die Arme fahren zu frueh aus

## URSACHE (belegt)

Alles hier von mir selbst disassembliert bzw. selbst aus der RDT gelesen.

### A. Der Original-Ausloeser ist ein raumfestes Rechteck, nicht ein Radius — und er ist 1700 TIEF INSGESAMT

`FUN_80042b64` @0x80042b64 testet Ecke + Ausdehnung, **unsigned**:

```
80042b68: lh  v0,0(a1)     ; rect.x   (signed)
80042b6c: lw  v1,0(a2)     ; punkt.x  (entity+0x34)
80042b70: lhu a0,4(a1)     ; rect.w   (UNSIGNED)
80042b74: subu v1,v1,v0
80042b78: sltu a0,a0,v1    ; unsigned -> px < rect.x faellt durch den Wrap heraus
80042b7c: bne  a0,zero,0x80042ba4
```
z-Zwilling: `lh v0,2(a1)` / `lw v1,8(a2)` / `lhu a0,6(a1)`.

Rechteck `ROOM1210.RDT` @Datei `0x1EAE`, von mir byte-genau nachgelesen:
```
2c 06 03 41 00 00 ac a9 68 c5 50 14 a4 06 ff 00 18 02 00 00
```
= Aot_set, aot=6, sce=3, sat=0x41, nFloor=0, x=-22100, z=-15000, w=5200, d=1700,
Nutzlast `ff 00 18 02 00 00`.
=> Bereich x -22100..-16900, z **-15000..-13300**, Mitte (-19500,-14150). **BELEGT.**

### B. Der Ausloeser schaltet ALLE ZEHN Arme in EINEM Bild, genau einmal im Spiel

Sub-Tabelle @0x1ea0 = `06 00 26 00 28 00` ⇒ sub00=0x1ea6, sub01=0x1ec6, **sub02=0x1ec8**.
sub02 selbst gelesen:
```
0x1ec8: 22 03 2c 01                      ; Flag(Bank 3, Bit 0x2c) := 1
0x1ecc: 46 06 00 00 00 00 00 00 00 00    ; Aot_reset Slot 6 -> sce=0, sat=0  (Ausloeser tot)
0x1ed6: 10x ( 2e 02 0N 00  +  34 0c 01 00 )   ; Work_set(Gegner N) + Member_set(12, 1)
0x1f26: 01 00
```
Member 12 -> `sb a2,9(a0)` @0x800411f8, also `entity+0x9 = 1` fuer alle zehn.
sub00 installiert das Rechteck nur, solange Flag(3,0x2c) == 0 (`21 03 2c 00`). **BELEGT.**

⛔ **Das ist genau das Verhalten, das der Nutzer verworfen hat.** Es ist der byte-true Stand.

### C. Das Original hat KEIN Abstands-Tor am Arm

Root @0x8010c234-88 rechnet den Spielerabstand (`jal 0x80065f60` = SquareRoot0 @0x8010c27c) und
**verwirft ihn** — kein `sw v0,464(...)` wie bei Zombie (@0x801004c0), ZGirl (@0x8010a964) oder
Hund (@0x8010dc8c). Der naechste Befehl ueberschreibt a0. **BELEGT** (zwei Pruefer haben es
unabhaengig nachvollzogen; mein eigener jal-Zensus ueber 0x8010c1ec..0x8010d800 findet genau
**einen** `jal 0x80065f60`).

⇒ **Jedes Naeherungs-Tor pro Arm ist per Definition eine Nachruestung.**

### D. Die RVD-Kanten (Kamera) — Feldlage von mir selbst bestimmt

RVD @RDT+0x28 = 0x188, Stride 20. Roh-Bytes:
```
@0x228: 01 ff 03 00  ...   -> byte[2]=from=3, byte[3]=to=0   Quad z -15001 .. 802
@0x250: 01 ff 03 04  ...   -> from=3, to=4                    Quad z -15001 .. -10599
```
Der Befund zitierte @0x228 als "3->4"; **das ist falsch, @0x228 ist 3->0**. Der echte 3->4-Schalter
ist @0x250. **Aber beide Vierecke haben dieselbe z-Untergrenze -15001**, also 1 Einheit neben der
Nahkante des Ausloese-Rechtecks (-15000). Die Kamera-Lesart ueberlebt also die Korrektur — sie
stuetzt sich nur auf einen anderen Eintrag. Und sie enthaelt **kein Arm-Mass**.

---

## BESTRITTEN — und von mir entschieden

| Behauptung | Entscheidung | Beleg (eigene Disasm/Messung) |
|---|---|---|
| "Es gibt keinen zweiten Eingang" zu `+0x5 = 1` | **WIDERLEGT** | A[0] @0x8010c608 hat zwei Zweige. Eigene Disasm: `8010c614: lbu v0,9(a0)` / `8010c618: ori v1,zero,0x1` / `8010c61c: andi v0,v0,0x1f` / `8010c620: bne v0,v1,0x8010c644` / `8010c628: sb v0,5(a0)` **und** `8010c648: lbu v0,9(a0)` / `8010c64c: ori v1,zero,0x2` / `8010c654: bne v0,v1,0x8010c670` / `8010c65c: sb v0,5(a0)`. Also `+0x9==2 -> +0x5=2`. |
| "PORT HEUTE: REACH_Z 1700, REACH_X 11000" | **WIDERLEGT** | Arbeitsbaum `enemy_ai_common.c:11126` hat bereits `RE15_WRITHER_REACH_Z = 850`. HEAD (`32f2e9e4`) hat 1700. Die im Befund berichteten Oeffnungen bei dz 1630..1697 sind unter `dz < 850` unmoeglich — die Zahlen stammen aus HEAD. |
| "WIRKUNG: gleichzeitig ausgefahrene Arme von 3 auf 2" | **WIDERLEGT, gemessen** | Voller ctest heute gegen den 850-Baum: `unit_1210_gitterhaende` gibt aus `Durchlauf: 10 von 10 Armen haben reagiert, ... hoechstens **3** gleichzeitig ausgefahren`. Die "2" stammt aus einer Tor-Offen-Zaehlung, nicht aus ausgefahrenen Armen — zwei Metriken, als eine praesentiert. |
| "Damit sind BEIDE Haelften des Tors byte-belegt" | **WIDERLEGT** | Die x-Haelfte entscheidet nach der eigenen Messung des Befundes nie (Flur -20622..-18164 vollstaendig in -22100..-16900). Eine tote Bedingung belegt kein Tor. Die z-Haelfte ist von einem Raum-Rechteck auf jeden Arm umgehaengt (aus 1x1700 werden 10x1700) — Port-Zutat. |
| "Der Stolperdraht sitzt am Kamera-Schnitt 3->4 (@0x228)" | **teilweise widerlegt** | @0x228 ist 3->0 (eigene Byte-Lesung). @0x250 ist 3->4 und teilt die Kante -15001. Aussage im Kern haltbar, Zitat falsch. |
| "Der Stolperdraht feuert GENAU EINMAL im ganzen Spiel" | **eingeschraenkt** | Gilt fuer ROOM1210. ROOM1211 traegt @0x1e72 ein byte-identisches Rechteck; dessen sub03 beginnt mit `22 03 2c 00`, also **Loeschen** derselben Flagge. Von einem Pruefer belegt, von mir nicht nachgelesen. |
| "das Original hat einen eigenen gestaffelten Weck-Pfad ueber +0x1D0" | **WIDERLEGT, gemessen** | Siehe ZUSAMMENFASSUNG §1: der `+0x9==2`-Eingang wird in keinem Writher-Raum betreten (nur `34 0c 01 00` in ROOM1210/1211/3071), der Writher-Baum schreibt `+0x9` nie selbst (Store-Scan: 0 Treffer), und `+0x5=2` wird stattdessen **nach** der Lunge erreicht (`8010c8e0: addiu v0,v0,2` / `8010c8e4: sb v0,5(v1)`). `+0x1D0` ist der Zuck-Timer, kein Weck-Staffler. |

---

## OFFEN

1. **850 hat keine `@0x…`.** Belegt ist ausschliesslich `d = 1700` **eines raumfesten
   Rechtecks**, das alle zehn Arme gleichzeitig schaltet. Halbieren + Umverankern pro Arm ist
   Rechnung und Port-Zutat. Die Stuetz-Koinzidenz ("Mitte 47 Einheiten neben Arm 8") ist schwach:
   dasselbe Rechteck liegt in x auf **keiner** der beiden Arm-Spalten (-25000 / -14000), sondern
   deckungsgleich mit dem Flur; bei Arm-Abstaenden 1207..1400 trifft eine beliebige Bandmitte
   ohnehin mit ~7% einen der zehn Arme auf 47 genau, und der Arm wurde nachtraeglich gewaehlt.
2. **Meint der Nutzer-Report 1700 (HEAD) oder 850 (Baum)?** Nicht geklaert. Entscheidend dafuer,
   ob der Befund das lebende Symptom ueberhaupt trifft.
3. **Der Einstieg in den Flur ist nicht der gemessene.** Fuenf der sechs Tueren muenden mitten in
   den Flur; ein Pruefer hat aus ROOM1220/1221 die Ankunfts-Koordinaten gelesen und mit echter AI
   gemessen, dass **4 von 5** Ankuenften bereits IM Tor liegen (|dz| 663/737, 347, 497, 170) —
   dort ist der Vorlauf mit 1700 **und** mit 850 exakt 0. Eine Ankunft (-20800,-13700) liegt sogar
   **innerhalb** des Original-Rechtecks. Von mir nicht reproduziert, aber es entwertet die
   Kernwirkung "22-23 -> 11-12 Bilder" auf dem haeufigen Weg.
4. **Fortbewegungsmodus.** Vorlauf nur bei WALK 75 (@0x80073ea4) gemessen. Bei RUN 200
   (@0x80073f24) schrumpft er laut Gegenmessung auf 8-9 (1700) bzw. 4-5 (850) Bilder. Welcher
   Modus dem Nutzer-Report zugrunde liegt, ist unbekannt.
5. **Keine Hardware-Gegenprobe** (DuckStation-RAM auf Flag(3,0x2c) und entity+0x9).
6. **Keine visuelle Verifikation** (gdigrab) — bei einem visuellen Report Pflicht.
7. **Die Bahn-Gueltigkeit ist unbelegt.** "0 von 287 blockierten Schritten" hat null Trennschaerfe:
   derselbe Zaehler ist auch auf einer Bahn bei x=-35000 (ausserhalb jeder SCA-Zelle) 0 von 287,
   weil der Klemmer nur aus *soliden* Zellen herausschiebt.

---

## NAECHSTER SCHRITT

**Implementierungsreif: NICHTS.** Die Empfehlung (REACH_Z 850) liegt bereits im Baum und hat
`unit_writher_ai` rot gemacht. Zu tun:

1. **`unit_writher_ai` reparieren.** Die Fixture setzt `pl->z = 900` (`test_writher_ai.c:29`) und
   zitiert in Zeile 107 noch "REACH_Z = 1700". Das ist eine Fixture-Verschiebung durch den Fix,
   kein Sachfehler des Tests. Standpunkt aus dem gemessenen Freiband waehlen — **nicht** die
   Schranke senken (Memory `reai-v2-pin-fixture-verschiebung`).
2. **Etikett korrigieren.** Im Kommentarblock `enemy_ai_common.c:11085-11090` faellt der Satz
   "der Raum sagt selbst, wie nah der Spieler sein muss" ersatzlos. Ersetzen durch: *850 = halbe
   Tiefe des raumfesten Ausloese-Rechtecks (@Datei 0x1EAE, d=1700), pro Arm neu verankert —
   dokumentierte Abweichung, das Original schaltet alle zehn Arme aus einem Rechteck.*
3. **`REACH_X = 11000` streichen** (keine Herkunft) — und **nicht** durch den absoluten
   Original-x-Test ersetzen (No-op, raumfeste Koordinate in einer per-Typ-globalen Funktion).
   Wenn eine x-Schranke bleiben muss: aus der gemessenen Flurbreite ableiten und **als Port-Zutat
   benennen**, nicht als Zitat.
4. **Zu RE'en, bevor die Dosis 850 ueberhaupt sinnvoll diskutiert werden kann:** der reale
   Eintritts- und Laufweg. Weg: `RE15_DEBUG_JUMP="1210@<frame>"` + `RE15_INPUT_SCRIPT` ueber die
   fuenf Tuer-Ankunftspunkte, Vorlauf pro Tuer und pro Modus (WALK 75 / RUN 200) protokollieren.
   Solange 4 von 5 Eintritte schon im Tor liegen, ist die Dosis-Diskussion gegenstandslos.
5. **gdigrab-Mitschnitt** des Flur-Durchlaufs — was der Nutzer sieht, ist die Abnahme-Groesse.
