# Pruefung: pickup-bestaetigungssound.md

Rolle: Skeptiker. Keine Engine-Aenderung, kein Bau. ~30 Adressen selbst nachgelesen
(re15_disasm.py / re2_disasm.py / Roh-Python auf die Binaerdateien).

**Urteil: NICHT HALTBAR als Ganzes.** Der Kern (Id 6 = Bestaetigung; RE1.5 ist beim
Aufnahme-Ja stumm) HAELT und ist sogar besser belegbar als im Dossier gezeigt. Drei
Zensus-Saetze sind aber falsch — darunter eine falsche Ladebasis fuer DEBUG.BIN —, und
die Rechtfertigung "RE2 hat die Regel" behauptet mehr, als die RE2-Bytes hergeben.

---

## A. Was ich bestaetigen kann (selbst gelesen)

| Behauptung | Ergebnis |
|---|---|
| `@0x8004a51c: lui a0,0x406` (`06 04 04 3c`), `@0x8004a520: jal 0x80045024` | **stimmt** |
| `@0x8004a660: lui a0,0x405`, `@0x8004a664: jal 0x80045024` | **stimmt** |
| Se_on-Zerlegung: `srl v1,a0,24` @0x80045028 = Bank, `srl v0,a0,16` + `andi 0xff` @0x80045078-7c = Satz, `sltiu v0,v1,0x6` @0x80045094 | **stimmt** |
| 41 x `jal 0x80045024` (Wort `0c011409`) in PSX.EXE, alle 4-aligned im Textsegment (t_addr 0x80010000, t_size 0xaf000) | **stimmt, exakt 41** |
| MSG-FSM-Bestaetigungszweig @0x80028564-b4: `andi 0x4000` → `jal 0x8002877c` @0x800285a8, kein Se_on | **stimmt** |
| Aufloesung @0x8002877c: Auswahl-Byte 0x800b8520 `andi 0x1` @0x800287b0, `f8/f9`-Pruefung @0x800287e0-f4, Tabelle 0x80072f34 @0x80028818, `jalr v0` @0x80028828 | **stimmt** |
| Tabelle roh: `0x80072f34 = 80021f6c`, `+4 = 00000000`, `+8 = 000345e0` | **stimmt** |
| Aufnahme-Skript endet `1b 03 02 f9 00 01` — DEBUG.BIN Dateioffset 0x4fd6, Offsettabelle @0x4fc6 beginnt mit `10 00` | **stimmt** |
| FUN_80021f6c (0x80021f6c-0x80022000): einziger `jal` = 0x8004ef90 @0x80021fd8, `sh v0,0x800b0fd6` @0x80021fec, `jr ra` @0x80021ffc, **kein Se_on** | **stimmt** |
| RE2 @0x800308f8-0x8003095c byte-genau wie zitiert (`3c040405` @0x8003093c, `3c040406` @0x80030944 und @0x80030950, `jal 0x8005ba28` @0x80030954) | **stimmt** |
| RE2-Aufrufer @0x80030734 `lbu a2,1(s0)`, Deskriptor-Basis 0x8009df10 @0x80030758, `jal 0x80030844` @0x80030760 | **stimmt** |

**Die negative Kernaussage ist sogar STAERKER als das Dossier sie belegt.** Ich habe den
Aufnahme-Teilbaum ab FUN_8001db28 **transitiv** abgesucht (58 Funktionen, Funktionsgrenzen
ueber gefilterte Prologe): **null** `jal 0x80045024`. Die beiden Ziele ausserhalb der EXE,
die das Dossier NICHT verfolgt hat — `0x800c004c` (@0x80027fb4) und `0x800c69bc`
(@0x800284f0) —, liegen in DEBUG.BIN, sind 0xb8 bzw. 0x34 Byte lang und enthalten
**ueberhaupt keinen** `jal`/`jalr`. Der Ja-Pfad ist damit wirklich stumm.

---

## B. Falsch

### B1 ⛔ Falsche Ladebasis: DEBUG.BIN liegt @0x800c0000, nicht @0x80100000

§2.6 schreibt woertlich: *"Roh-Zensus ueber alle Overlays (`info/Re1.5/PSX/BIN/*.BIN`,
Code @0x80100000, kein Header-Offset)"* und listet in der Tabelle darueber
*"| DEBUG.BIN | 0x80106484 | Item-Raster …"*.

Gemessen:
- Histogramm der internen `jal`-Ziele je Datei (obere Halbwoerter):
  `DEBUG.BIN: 800c x43, 8004 x25, 8006 x13, 8002 x9` — **kein einziges 0x8010**.
  Zum Vergleich `STAGE1.BIN: 8001 x707, 8004 x148, 8002 x123, 8011 x108` und
  `TITLE.BIN: 8002 x65, 8010 x38`. Die STAGE/TITLE-Basis 0x80100000 stimmt, DEBUG.BIN nicht.
- DEBUG.BIN ist 0x40000 Byte lang; 0x800c0000 + 0x40000 = **0x80100000**, also genau bis
  an die Overlay-Basis heran.
- Die EXE ruft aus der MSG-Kette `jal 0x800c004c` (@0x80027fb4) und `jal 0x800c69bc`
  (@0x800284f0). Beide existieren nur, wenn DEBUG.BIN @0x800c0000 liegt.
- Das Dossier selbst benutzt in §2.5 fuer dieselbe Datei die Basis 0x800c0000
  (*"Tabelle @0x800c4fc6, Skript-Offset 0x10"*) — nachgeprueft: DEBUG.BIN Dateioffset
  0x4fc6 = `10 00 2f 00 50 00 …`. **§2.5 und §2.6 widersprechen sich.**

Richtige Adresse: **0x800c6484** (`3c 04 04 06 / 0c 01 14 09` @Dateioffset 0x6484).

### B2 ⛔ "CORE-Satz 6 wird in RE1.5 an GENAU DREI Stellen gespielt" — es sind mindestens FUENF

§2.6 woertlich: *"CORE-Satz 6 wird in RE1.5 an GENAU DREI Stellen gespielt"* mit der
Tabelle PSX.EXE / DEBUG.BIN / TITLE.BIN, je eine Zeile.

Roh gescannt (`0c011409` + statisches `lui a0` davor):

| Datei | Adresse | Bytes |
|---|---|---|
| PSX.EXE | 0x8004a51c | `3c040406` |
| DEBUG.BIN | **0x800c6484** | `3c040406` |
| DEBUG.BIN | **0x800c6ea0** | `3c040406` (unmittelbar vor `0c011409`, `34050000`) |
| DEBUG.BIN | **0x800c704c** | `3c040406` (Zweig hinter `andi v1,v0,0x4000` @0x800c7030) |
| TITLE.BIN | 0x80102ac4 | `3c040406` |

DEBUG.BIN hat 23 Se_on-Aufrufe, davon 22 Bank 4, Saetze
`4 x10, 5 x6, 6 x3, 8 x2, 10 x1` — das Dossier nennt *"0x404 x4, 0x406 @80106484, 0x40a, 0x405 …"*.
Zwei Satz-6-Stellen fehlen komplett.

### B3 ⛔ "Davon benutzen genau acht die Bank 4" ist keine vollstaendige Aussage

§2.2 woertlich: *"41 Aufrufstellen. Davon benutzen **genau acht** die Bank 4"*.

**14 der 41 Aufrufe setzen `a0` nicht mit einem statischen `lui`.** Beispiele, selbst gelesen:
- `@0x80041718: sll v1,a3,24` → `@0x80041728: or v1,v1,v0` → `@0x80041734: or a0,v1,a0`:
  **die Bank kommt als Argument `a3` vom Aufrufer.** Diese Funktion beginnt @0x80041624,
  hat in EXE **und** allen Overlays **null** `jal`-Aufrufer und steht als Datenwort in der
  Zeiger-Tabelle @0x80074580 (Nachbarn 0x80041048/0x80041080/…/0x80041864) — sie wird
  also indirekt dispatcht. Bank 4/Satz 6 ueber diesen Weg ist nicht ausgeschlossen.
- `@0x80018420: sll a0,a0,8` + `@0x80018428: or a0,a0,v1` (Bank aus `lhu 38(t0)`).
- `@0x8003338c`, `@0x8003537c`, `@0x80035c00` mit `ori a0,a0,0x1` auf ein anderswo
  gebildetes `a0`.

Das Wort "genau" behauptet Vollstaendigkeit, die die Suchmethode (Rueckwaerts-Scan nach
`lui a0`) nicht liefern kann. Dasselbe gilt fuer B2.

*Nicht* zu beanstanden: die STAGE-Zeilen in §2.6. Mein erster, loserer Scan fand dort mehr
Bank-4-Stellen; bei Einzelpruefung (z.B. STAGE1 @0x8010a3e4, @0x80111a50, @0x80115c04,
@0x8011c1e4 = 4 dynamische Aufrufe) sind die Zahlen des Dossiers (11/4, 4/2, 16/7, 12/7,
19/9, 0/0, 4/4) korrekt. Das ziehe ich ausdruecklich zurueck.

---

## C. Ueberzogen — mehr behauptet, als die Belege tragen

1. **§2.7-Ueberschrift: "RE2 hat die Regel, die der Nutzer will — FUN_80030844"** und §4:
   *"was der Nutzer will, ist die RE2-Regel aus FUN_80030844 @0x80030924-54 —
   Auswahl-Index 0 → Se_on(0x04060000), Index 1 → Se_on(0x04050000)"*.
   RE2s Ton ist **doppelt gegattert**, selbst gelesen:
   - `@0x80030910: andi v0,v0,0x2000` / `@0x80030914: bne → 0x80030924`, sonst
     `@0x80030918: andi v0,s1,0x60` / `@0x8003091c: beq v0,zero,0x80030b68` → **komplett stumm**.
   - Fuer Index 0: `@0x80030948: andi v0,s1,0x20` / `@0x8003094c: bne v0,zero,0x80030b68`
     → **das JA ist stumm**, wenn das Bit gesetzt ist.
   `s1 = a2 = lbu 1(s0)` (@0x80030734) ist ein **Pro-Nachricht-Flagbyte**. Solange dieses
   Byte fuer RE2s Aufnahme-/Untersuchungs-Abfrage nicht gelesen ist, ist nicht belegt, dass
   RE2 dort ueberhaupt einen Ton spielt. §6.1 gibt das zu (*"es kann also sein, dass RE2
   ausgerechnet dort ebenfalls stumm ist"*) — die Ueberschrift, §4 und der Umsetzungsplan
   tun es nicht. Das Dossier traegt seinen eigenen Widerruf im Kleingedruckten.
2. **§2.4: "Die komplette Aufnahme-Praesentation (Zoom, Flip, Prompt, Insert) loest keinen
   SE aus."** Belegt war nur die **erste** Aufrufebene. Die Aussage stimmt (ich habe sie
   transitiv nachgewiesen), aber das Dossier hat sie nicht bewiesen — insbesondere blieben
   0x800c004c und 0x800c69bc unverfolgt, und die lagen ausserhalb der EXE, also genau in
   der Datei, deren Basis §2.6 falsch angibt.
3. **§5/P1, Zusatzregel fuer den can't-carry-Zweig**: *"RE2 spielt dort mit Index 0 den
   Satz 6 (@0x80030950). Gehoert zu P1: `if (!(pad_edge & 0xc000)) return;
   re15_audio_core_se(6);`"*. `0xc000` enthaelt CROSS (0x8000). Damit spielt ein
   **Abbruch**-Tastendruck den **Bestaetigungs**-Ton. Aus @0x80030950 folgt das nicht:
   dort gibt es keinen Abbruchknopf, nur den Auswahl-Index.
4. **§2.8** schliesst von `pbav=0x40` in den .EDH auf Spielbarkeit — plausibel, aber
   "`re15_audio_core_se(6)` ist also zu jedem Zeitpunkt spielbar" ist eine Aussage ueber
   die Laufzeit, die ohne einen Lauf nicht belegt ist. §6.4 gibt das zu.

---

## D. Risiko des Umsetzungsplans

**P3 (`re15_port/engine/src/msg_common.c:555-558`) ist der gefaehrliche Teil** — er
betrifft jede SCD-Ja/Nein-Abfrage im Spiel. Eigener Zensus ueber alle 240 RDTs unter
`re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT` (Message-Block @Header 0x3c, Auswahl-Marke
= Steuerbyte 0x03 nach derselben Code-Regel wie `re15_msg_is_choice`, msg_common.c:287):

**90 Ja/Nein-Abfragen in 68 RDTs** — 45 verschiedene Texte, jeweils doppelt durch die
`*0`/`*1`-Spielervarianten. Konkret stoerend:

- **Code-Panel-Kette**: `ROOM10D0/10D1 msg[7]`, `ROOM11E0/11E1 msg[7]`, `ROOM1230/1231 msg[7]`
  ("A card reader. You have to use the … Keycard and insert four digits"). Das Ja uebergibt
  direkt an das Code-Panel, das in v0.8.12 gerade erst seine eigene Bestaetigungs-/
  Sperrlogik bekommen hat — hier kaeme ein zweiter Ton unmittelbar vor dem Panel-Ton.
- **Ruestung an/aus**: `ROOM1190/1191 msg[4]` + `msg[5]`, `ROOM40A0/40A1 msg[12]` + `msg[13]`.
- **Registrieren/Speichern**: `ROOM5050/5051 msg[4]` "Will you register?" — im
  Auslieferungsstand endet das in "Save is not available in this preview".
- **Rollladen-Schalter**: derselbe Text in 8 Raeumen (`ROOM1040/1041/1050/1051/1130/1131/
  10D0/10D1`), dazu `11A0/11A1`, `20A0/20A1` — der Ton wird sehr repetitiv.
- **Jedes NEIN** spielt ab P3 den Abbruchton 0x405 — an allen 45 Stellen. Das ist ein
  hoerbarer Eingriff in das gesamte Spiel, nicht in "die Aufnahme".

Zweites Risiko — **Inkonsistenz zwischen den drei Stellen**:
- `msg_common.c:467` `act_edge = CONFIRM_BIT (0x4000)`, `:472` `dismiss_edge = 0x4000|0x8000`.
  P3 haengt an `act_edge`: CROSS in einer SELECT-Abfrage bleibt stumm.
- `item_modal_common.c:278` (can't-carry) soll nach P1 auf `0xc000` toenen: CROSS toent.
- `item_discard_common.c:316-317`: CROSS setzt `s_choice = 1` und faellt in dieselbe Zeile,
  toent also 0x405.
  Drei Stellen, drei verschiedene Regeln fuer dieselbe Taste.

Drittes Risiko: P1 und P3 sind getrennte Zustandsmaschinen (`item_modal_common.c` fuehrt
`s_state` selbst, `msg_common.c` `g_scd.message_fsm`), also **kein** Doppelton — das habe
ich geprueft und es ist in Ordnung.

---

## E. Die fehlende Messung

**RE2s Pro-Nachricht-Flagbyte.** Der gesamte "RE2 hat die Regel"-Teil steht und faellt mit
den Bits 0x40 und 0x20 in dem Byte, das `@0x80030734: lbu a2,1(s0)` liest und das als `s1`
die Gatter `@0x80030918 andi s1,0x60` und `@0x80030948 andi s1,0x20` bedient. Zu messen:
dieses Byte fuer RE2s Aufnahme-/Untersuchungs-Abfrage — ueber die Deskriptor-Tabellen
`0x8009dee4` / `0x8009df10` (6 Byte je Eintrag, Index `(a2 & 0xf)*6`, gebildet
@0x80030744-5c bzw. @0x80030770-88) und die vorhandenen RE2-RDTs unter
`info/re2leon/PL0/RDT/`. Ist Bit 0x20 fuer die Aufnahme-Nachricht gesetzt, ist **RE2 beim
Ja genauso stumm wie RE1.5** — dann ist der Plan eine reine Nutzer-Entscheidung ohne jedes
byte-true Vorbild, und die Ueberschrift von §2.7 muss weg.

Nachrangig, aber ebenfalls offen: der Ton wurde in dieser Runde nie gehoert (kein Bau,
kein Lauf) — §6.4 sagt das selbst.
