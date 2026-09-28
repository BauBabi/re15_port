# T2 — RE2-Tuersemantik + Port-Anschluss-Plan (Runde 31)

Auftrag: analysis/befunde_runde31/AUFTRAG.md (Nutzer woertlich). Teil T2: zu einer RE1.5-Tuer mit
zugeordnetem RE2-Archiv die RICHTIGE Variante, den RICHTIGEN Griff, den RICHTIGEN Ton — auch fuer
Tueren zwischen zwei Raeumen. Kein Spielcode; Werkzeuge unter re15_port/tools/tueren/re2_*.py.

Stand: IN ARBEIT (laufend nachgefuehrt, nach jedem Abschnitt committet).

## 0. Quellen und Werkzeuge

Herkunftsmarken: `@0x8...` = selbst disassemblierte Instruktion (RE2 = `info/re2leon/PSX.EXE`, t_addr 0x80010000,
`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py`); `Datei 0x...` = Byte-Offset; `[SIM]` = Ergebnis des
Skript-Simulators `re15_port/tools/tor/tuerkatalog.py` (VM, Handler je Opcode mit PC-Vorschub-Adresse, siehe
analysis/tor_1170/03/04); `[BILD]` = selbst angesehenes Bild.

| Werkzeug (neu, nur lesend) | Zweck | Ausgabe |
|---|---|---|
| `re15_port/tools/tueren/re2_tuer_zensus.py` | alle 572 RE2-`Door_aot_set`/`_4p` lesen, Seiten bilden, zu physischen Tueren paaren | `build/r31_tueren/t2/re2_tueren.json` |
| `re15_port/tools/tueren/re2_tuer_varianten.py` | je Archiv und Variante: Rollen der Objekte, Angel/Griff im Bild, Richtung, Bilder Anfang/Mitte | `build/r31_tueren/t2/varianten.json`, `bilder/DOORxx_vN.png`, `bogen_varianten_NN.png` |

Satzformat (selbst gelesen):
- `0x3B Door_aot_set`: Handler `0x80054be4` (Tabelle `0x800a74c8[59]`), Satzzeiger = pc+2 (`@0x80054c30 addiu v0,v0,2`),
  Laenge 32 (`@0x80054c40 addiu v0,v0,32`). Rechteck pc+6..13, Nutzlast pc+14.
- `0x68 Door_aot_set_4p`: Handler `0x80054c50` (Tabelle `[104]`), Satzzeiger pc+2 (`@0x80054c9c`), sat |= 0x80
  (`@0x80054cb4 ori v0,v0,0x80`), Laenge 40 (`@0x80054cc4 addiu v0,v0,40`). Vier Punkte pc+6..21, Nutzlast pc+22.
- Nutzlast +12 = Archiv (`@0x80015088 lbu v1,12(v0)`), +13 = Variante & 0x7f / Bit 7 (`@0x80013e5c`, `@0x80013e6c andi 0xff7f`,
  `@0x80013e84 andi 0x80`), +15/+16 Schloss (`@0x800515a8`, `@0x800515d0`); Rest 03_tuersequenz 4.
- Tuerkamera: Auge (10000,0,0), Ziel 0, H 290 -> Bild-x = 160 + 290*z/(10000-x) (03 3.2; Bildmitte (160,120) aus den
  Spiel-Aufrufern `@0x80049cac/b0`, `@0x80068e80/88`).


## 1. Variante <-> Griff je Archiv (Angel/Griff links/rechts, weg/hin, Meshes)
(folgt)

## 2. Paar-Regel in RE2 (572 Door_aot_set -> physische Tueren, Variante je Seite, Griff im Hintergrund)
(folgt)

## 3. Griff-Formen je Archiv (Knauf/Druecker/Ring/Stange/keiner, Mesh, Anhaengepunkt, Austauschbarkeit)
(folgt)

## 4. Ton je Archiv (Tonteil, Se_on-Bilder, Door_exit-Ton, Tonfamilien, Treppen/Leitern)
(folgt)

## 5. Treppen/Leitern/Aufzug/Schott (Varianten 4/5, Archive 0E/0F/12/16, 1E/33/35, 25/29, 2B, 2D)
(folgt)

## 6. Port-Anschluss-Plan (Datei:Zeile, RE2-Adressen, Schrittfolge fuer den Bau-Agenten)
(folgt)

## 7. Offen / nicht belegt
(folgt)
