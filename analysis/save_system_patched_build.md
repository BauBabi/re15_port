# Architektur-Dossier — Das Speichersystem des gepatchten Builds `re15_save_final`

**Datum:** 2026-08-06
**Auftrag:** Der Auslieferungsstand von RE1.5 hat kein funktionierendes Speichersystem. Das Vorprojekt
`c:/workspace/git/reAi` hat eines per EXE-Patch hineingebaut (Disc `re15_save_final`), dort funktioniert
Speichern/Laden inkl. **zweier verschiedener Ortsnamen**. Vollstaendig analysieren, WIE — als Architektur-Wissen
fuer den Port.
**Regel:** CLAUDE.md STOP-GATE. Jede Behauptung mit `@0x…`-Adresse oder Datei-Byte-Offset.
**Verfahren:** 5 unabhaengige Untersuchungs-Lanes mit adversarialer Gegenpruefung (24+23+14+17 Befunde),
danach **eigene Nachmessung aller tragenden Zitate**: eigene De-Sektorisierung der Disc, eigener
MIPS-R3000-Decoder, eigener ISO9660-Walker, eigener Registerverfolgungs-Scan, eigenes Parsen der echten
Memory Card. Sechs Lane-Befunde wurden dabei widerlegt; die korrigierten Fakten stehen unten (§7).
**In `reAi_v2` wurde KEIN Code geaendert** — nur dieses Dossier.

## Legende

| Marker | Bedeutung |
|---|---|
| **[BELEGT]** | Ich habe die Bytes/Instruktionen in dieser Sitzung selbst gelesen und dekodiert. Adresse + Datei-Offset im Text. |
| **[WAHRSCHEINLICH]** | Aus belegten Bytes erschlossen, oder von einer Lane belegt und von mir nur plausibilisiert, nicht selbst nachgemessen. |
| **[OFFEN]** | Nicht belegt. Vermutung, Sekundaerquelle oder ungeprueft. Darf nicht als Grundlage fuer Code dienen. |

---

## 0. Arbeitsgrundlage und Provenienz **[BELEGT]**

### 0.1 Die drei Binaries

| Rolle | Pfad | Groesse | md5 | `t_addr` | `t_size` |
|---|---|---|---|---|---|
| **ORIGINAL** (Auslieferungsstand) | `c:/workspace/git/reAi_v2/info/Re1.5/PSX.EXE` | 718848 | `b55fdaa5b9ec0f84bdd8a6b1107ea0b0` | `0x80010000` | `0x000af000` |
| **GEPATCHT** (aus dem Image) | `info/Re1.5/re15_save_final.bin`, Sektor 73020 | 727040 | `17d19f4896de70a01381e11f53e05490` | `0x80010000` | `0x000b1000` |
| **GEPATCHT** (Vorprojekt-Extrakt) | `c:/workspace/git/reAi/info/Re1.5_PATCHED/re15/PSX.EXE` | 727040 | `17d19f4896de70a01381e11f53e05490` | — | — |

De-Sektorisierung selbst durchgefuehrt: Rohsektor 2352 B, Nutzdaten bei `+24`, 2048 B/Sektor, Startsektor
73020 (= Datei-Offset `0xa3c9758` im Image). Ergebnis ist **bit-identisch** mit dem bereits entpackten
Vorprojekt-Extrakt — man muss also nicht jedes Mal de-sektorisieren.

**Offsetformel (gilt fuer beide EXEs):** `Datei-Offset = RAM-Adresse − 0x80010000 + 0x800`.

**Diff-Umfang:** 1653 **literal** differierende Bytes im gemeinsamen Bereich + 8192 angehaengte Bytes,
die **komplett `0x00`** sind (selbst geprueft). Der einzige Header-Patch ist `t_size` bei Datei-Offset
`0x1c`–`0x1d` (`0xaf000` → `0xb1000`); `pc0` und `t_addr` sind unveraendert.

> **Zahlen-Falle:** Lane-Berichte nennen „48 / 49 Diff-Laeufe, 2146 / 2164 Bytes". Das sind
> **merge-abhaengige** Zahlen (zusammenhaengende Laeufe: 263; bei Luecken-Schwelle 8 → 52, bei 16 → 48).
> Belastbar ist nur: **1653 literal differierende Bytes**. **[BELEGT]**

### 0.2 Die Disc als Ganzes

Eigener ISO9660-Walk ueber `re15_save_final.bin`, 693 Dateien gegen `info/Re1.5/PSX` verglichen:

| Datei | LBA | Groesse | Diff |
|---|---|---|---|
| `/PSX.EXE` | 73020 | 727040 | 1653 B + 8192 B Tail |
| `/PSX/STAGE1/ROOM1070.RDT` | 21587 | 153328 | **1 Byte** |
| `/PSX/STAGE1/ROOM1150.RDT` | 28604 | 194080 | **5 Bytes** |
| `/PSX/STAGE1/ROOM1151.RDT` | 28699 | 202392 | **5 Bytes** |
| `/PSX/DATA/TITLEU.TIM` | 1101 | 153620 | 2118 B — **kosmetisch (Titelgrafik), nicht Save** |
| `/PSX/MOVIE/CAPCOM.STR` | 6497 | 5390336 | **Mess-Artefakt, kein echter Diff** |

Zu `CAPCOM.STR`: die lokale Kopie ist 6148352 B, das ISO-Verzeichnis nennt 5390336 B.
`5390336/2048 = 2632` Sektoren, `2632 × 2336 = 6148352` — die lokale Kopie wurde mit **2336 B/Sektor**
extrahiert, mein Reader liest 2048 B/Sektor. Reiner Formatunterschied. **[BELEGT]**

Zusaetzlich neu auf der Disc: `/PATCH.BIN` (LBA 73388, 20480 B, Magic `PTCH` @0x0000, zweite Nutzlast
`PTH2` @0x1000) und `/PATCH2.BIN` (LBA 73398, 16384 B; byte-identisch zu `PATCH.BIN[0x1000:0x5000]`,
also redundant). 3282 Nichtnull-Bytes. **[BELEGT]**

### 0.3 ⚠ Einordnungs-Warnung: die „ORIGINAL"-EXE ist bereits ein Mod

`info/Re1.5/README.TXT:3` weist die Basis als **„Resident Evil 1.5 (Magic Zombie Door) Update
25-01-2025, Team IGAS build modification by MartinBiohazard"** aus. Alle Adressen in diesem Dossier
sind **MZD-Adressen**. Fuer `reAi_v2` ist das konsistent (der Port arbeitet auf derselben Basis), aber
Adressgleichheit mit einem anderen RE1.5-Build ist **nicht** garantiert. **[BELEGT]**

---

## 1. Frage 1 — Was kann der AUSLIEFERUNGSSTAND, und was fehlt ihm?

### Kurzantwort

Keine der drei angebotenen Kategorien trifft allein zu. Praezise:

> **Der Karten-Layer ist FERTIG und wird beim Boot initialisiert; die LADEN-Haelfte ist im Spiel sogar
> erreichbar. Die SPEICHERN-Haelfte ist im Dev-Zustand stecken geblieben — an fuenf unabhaengigen
> Stellen unfertig, von denen jede EINZELNE allein genuegt, um Speichern unmoeglich zu machen —
> und zusaetzlich per Klartext-Meldung im Spiel bewusst stillgelegt.**

### 1.1 Der Karten-Layer ist vollstaendig einkompiliert **[BELEGT]**

Zusammenhaengender Block `0x80025c00`–`0x800279c8` (Datei `0x016400`–`0x0181c8`), 22 Funktionen, alle mit
echtem Stack-Frame (kein Stub ausser einem, s. 1.3-L5). Die wichtigsten:

| Adresse | Rolle | Groesse |
|---|---|---|
| `0x80025c00` | Karten-FSM, **14 Zustaende** | 2280 B |
| `0x80026450`/`0x80026658` | UI-Renderer der Slot-Liste | 1424 B |
| `0x80026ca8` | Slot-Verzeichnis lesen + validieren | 300 B |
| `0x80026e4c` | **Ortsnamen-Resolver — STUB** | 8 B |
| `0x80026e54` | Kartentitel komponieren | 244 B |
| `0x80026f48` | Pre-Save-Snapshot (Charakter + X/Y/Z) | 72 B |
| `0x80026f90` | Memory-Card-Event-Init (7× `OpenEvent` + `EnableEvent`) | 376 B |
| `0x800271a8` | **WRITE** (30× open / 30× write / 3× Verify-Read à 0x200) | 448 B |
| `0x80027368` | **READ** (open → lseek → read; a3 = Seek-Offset) | 288 B |
| `0x80027488` | Verzeichnisliste (`"BISLPS-00222*"`) | 360 B |
| `0x80027724` | Format / Auto-Format bei neuer Karte | 92 B |

Zustands-Sprungtabelle der FSM **@`0x80010738`** (Datei `0xf38`), Bound-Check `sltiu v0,v1,14`
@`0x80025dd4`, `jr v0` @`0x80025df8`. Inhalt (ORIGINAL == GEPATCHT):

```
[0]=0x80025e00 [1]=0x80025e3c [2]=0x80025e7c [3]=0x80025ecc [4]=0x80025f00
[5]=0x80025f38 [6]=0x8002601c [7]=0x800260d4 [8]=0x800261b4 [9]=0x80026240
[10]=0x800262ac [11]=0x80026300 [12]=0x8002636c [13]=0x800263ec
[14] = 0x4f4d454d = ASCII "MEMO"  -> Fremddaten, die Tabelle hat exakt 14 Eintraege
```

**Zustand 8 = SCHREIBEN, Zustand 9 = LESEN.** Die Modus-Weiche steht in Zustand 7:
@`0x800260d4 lw t0,496(sp)` (das ist `param1`, abgelegt @`0x80025c04 sw a0,496(sp)`) + `bne t0,zero,0x80026198`.
`param1 == 0` faellt in den Zweig, der den Dateinamen `"BISLPS-002220"+Ziffer` baut und nach Zustand 13
(Ueberschreiben-Abfrage) bzw. 8 geht; `param1 != 0` geht direkt auf Zustand 9.
**Also: `modus 0 = SPEICHERN`.**

> ⚠ Das Decompilat `c:/workspace/git/reAi/RE_15_modified_V2/sys_memcard_menu_handler.c:10` behauptet
> das **Gegenteil** („0 = Laden, 1 = Speichern"); das Schwester-Decompilat
> `reAi/RE_15_modified/memcard_menu_handler.c` hat die Modi richtig, invertiert dafuer den
> **Rueckgabewert** im Kommentar. Beide Decompilate sind an dieser Stelle falsch — die Rohbytes
> sind massgeblich. **[BELEGT]**

Auch die BIOS-Ebene ist real verdrahtet (`_card_info` / `_card_write` / `_card_clear` / `_new_card` /
`InitCARD` / `StartCARD`, Decompilate unter echten Namen in `reAi/RE_15_modified_V2/`), und
`FUN_80026f90` hat genau einen Aufrufer: die Boot-Init @`0x80020fe4 jal 0x80026f90`. Strings vorhanden:
`"BISLPS-00222*"` @Datei `0x000f14`, `"bu00:"` @`0x001024`, `"MEMORY CARD BG"` @`0x000f70`. **[BELEGT]**

### 1.2 Die LADEN-Haelfte ist im Auslieferungsstand ERREICHBAR **[BELEGT]**

`TITLE.BIN` @`0x801025a8 jal 0x80025c00` mit `a0=1` (LADEN, @`0x8010259c ori a0,zero,0x1`) und
`a1=0x80180000` (@`0x801025a0 lui a1,0x8018` — innerhalb 2 MB). Verdrahtung: Hauptzustands-Tabelle
@`0x8010269c`[7] = `0x80102594`; Sub-Tabelle @`0x801026ac`[3] = derselbe Handler; der Menue-Cursor
`DAT_801026ca` ist auf `<2` begrenzt (`sltiu v0,v0,2` @`0x801021d8` und @`0x80102b38`) und wird per
`substate = cursor + 2` (@`0x80102c3c`) uebersetzt: **Cursor 0 → NEW GAME, Cursor 1 → Karten-Menue im
LADE-Modus.** Rueckkehr @`0x801025b0`: `v0==0` → Spielstart, `v0==1` → zurueck zum Titel.

Das heisst: **„RE1.5 kann gar nicht speichern" ist richtig — „RE1.5 hat gar kein Speichersystem" ist falsch.**
Ein LOAD-Menuepunkt existiert im ausgelieferten Titelbildschirm.

### 1.3 Die fuenf Luecken der SPEICHERN-Haelfte

#### L1 — Das Anforderungs-Bit wird getestet, aber nie gesetzt **[BELEGT]**

Der einzige Aufruf von `FUN_80025c00` im SAVE-Modus ist @`0x8001cba4`, gegated durch **Bit 18
(`0x00040000`) von `DAT_800ACA38`**:

```
8001cb78  3c020004  lui  v0,0x4
8001cb7c  00621024  and  v0,v1,v0          ; v1 = *0x800ACA38 (geladen @0x8001cadc)
8001cb80  1040000a  beq  v0,zero,0x8001cbac ; nicht gesetzt -> Menue ueberspringen
8001cb84  3c02fffb  lui  v0,0xfffb
8001cb88  3442ffff  ori  v0,v0,0xffff
8001cb8c  00621024  and  v0,v1,v0           ; Bit 18 loeschen
8001cb90  3c01800b  lui  at,0x800b
8001cb94  ac22ca38  sw   v0,-13768(at)
8001cb98  00002021  addu a0,zero,zero       ; modus = 0 = SPEICHERN
8001cb9c  3c058020  lui  a1,0x8020          ; Puffer 0x80200000  <-- siehe L2
8001cba4  0c009700  jal  0x80025c00
```

**Beweis, dass es niemand setzt** (eigener Scan, ORIGINAL `.text`, `t_size 0xaf000`):

* `lui rX,0x0004` kommt **exakt 7×** vor. Zwei davon sind die Bit-18-**Tests** (`@0x8001ca00` mit
  `and`+`bne`, `@0x8001cb78` mit `and`+`beq`), fuenf sind unbeteiligte Konstanten:
  `0x800446ac`/`0x800448d0` (`ori 0x2fc0` → Groesse `0x42fc0`), `0x80055ff0`/`0x800596d0`
  (`ori 0x0011` → Multiplikator `0x40011`, gefolgt von `multu`), `0x8005b7a0`
  (`and v0,a1,v0` auf ein **Registerargument**).
* Registerverfolgungs-Scan aller Stores nach `0x800ACA38..3b`: 14 Treffer im ORIGINAL, **kein einziger**
  mit einem Bit-18-Produzenten. Gesetzt werden nur `0x4000`, `0x8000`, `0x00010000`, `0x00020000`,
  `0x00100000`, `0x00400000`, `0x00800000`, `0x02000000`, `0x04000000`, `0x08000000`, `0x20000000`,
  `0x40000000`, `0x80000000`.
* Im **GEPATCHTEN** Build gibt es genau drei Stores mit Bit-18-Produzent — `@0x8006ea50` (OR, SCD-Hook),
  `@0x8007086c` (OR, AOT-Hook), `@0x8006f014` (CLEAR, `HP_SAVE`). Alle drei in **neuem** Code.
* Die STAGE1-6-Overlays referenzieren `0x800ACA38` **ueberhaupt nicht**, `DEBUG.BIN` nur lesend
  (`@0x801062ac`).

> **Methodischer Vorbehalt [WAHRSCHEINLICH]:** Statische Scans dieser Art haben Blindstellen
> (Delay-Slot-Stores, `lui/addiu`-Basis + Offset in einem anderen Register). Eine breitere Lane-Messung
> (20 Stores statt meiner 14, inkl. Delay-Slots) kam zum selben Ergebnis. Zusaetzlich wurde der
> **datengetriebene** Weg geprueft: Flag-Bank-Tabelle `@0x80074664[0] = 0x800ACA38`, Opcode-0x22-Handler
> `@0x8003fdd0` bildet die Maske als `0x80000000 >>> (bit & 0x1F)`, also `0x40000 == SCD-Bit-Index 13`;
> ein SCD-Walk ueber alle 240 RDTs fand **kein** `Flg_set(bank 0, bit 13, set)`. Diesen SCD-Walk habe ich
> **nicht** selbst wiederholt (34 der 240 Dateien scheiterten dort am Header-Parse) — daher
> [WAHRSCHEINLICH], nicht [BELEGT].

#### L2 — Der Save-Puffer liegt ausserhalb des Retail-RAM **[BELEGT]**

@`0x8001cb9c lui a1,0x8020` → `savePtr = 0x80200000`. PSX-Hauptspeicher ist **2048K** ab `0x80000000`
(`info/Resident_Evil_und_Playstation_Information/psx-spx.github.io-master/docs/memorymap.md:5`), und
„2MB RAM can be mirrored to the first 8MB (strangely, enabled by default)" (ebd.:76). `lui rX,0x8020`
kommt in der ganzen ORIGINAL-EXE nur zweimal vor: hier und @`0x80017120`, wo es das
`lui + negatives Offset`-Idiom ist (`lw v0,-8968(at)` = `0x801fdcf8`, regulaer). **`0x80200000` ist der
einzige echte Zeiger jenseits 2 MB in der EXE** — die Signatur eines auf einem 8-MB-Devkit gebauten
Puffers. Der Patch aendert genau 1 Byte: `lui a1,0x8016` (`0x80160000`).

#### L3 — Die I/O-Groesse passt nicht zum Zustandsblock **[BELEGT]**

Schreiben @`0x80026220 ori v0,zero,0x0800` (5. Argument, `sw v0,16(sp)`); Lesen @`0x80026240
ori a2,zero,0x0800`. Der Zustandsblock ist aber **0x1430** gross (`ori a2,zero,0x1430` @`0x800262a0`,
memcpy nach `0x800B0DBC`). **0x1430 − 0x800 = 0xC30 Bytes** kamen beim Laden aus uninitialisiertem
Puffer. Der Patch setzt beide Groessen auf `0x1680`.

#### L4 — Kein Speicherpunkt im Spiel feuert eine Anforderung **[BELEGT]**

Exhaustiver Scan aller `ROOM*.RDT` (Message-Kodierung `Zeichen = Byte + 0x24`) findet in **16 Raeumen**
denselben Klartext:

> „It's a phone/computer. You can save your progress with this. **Save is not available in this preview.**"

Fundstellen (Datei-Byte-Offset, Beginn von „progress"): ROOM1070 `0x1cc4`, ROOM1071 `0x1cc4`,
ROOM1120 `0x0f14`, ROOM1121 `0x0f14`, ROOM1150 `0x13a8`, ROOM1151 `0x1186`, ROOM2010 `0x099a`,
ROOM2011 `0x09ba`, ROOM30A0 `0x08f3`, ROOM30A1 `0x08f3`, ROOM30B0 `0x09f7`, ROOM30B1 `0x09f7`,
ROOM4010 `0x1959`, ROOM4011 `0x082a`, ROOM5010 `0x06e4`, ROOM5011 `0x179d`. Dieselben Bloecke tragen
„Itembox is not available in this preview" — **Speichern und Item-Box wurden gemeinsam stillgelegt.**

Und das Ingame-Menue hat keinen SAVE-Reiter: `jal 0x80025c00` existiert EXE-weit **genau einmal**
(`0x8001cba4`) plus einmal in `TITLE.BIN` (`0x801025a8`); keiner davon liegt im Menue-Code.

> ⚠ **Falle:** Die verbreitete Zusatzbegruendung „der Reiter-Cursor `DAT_800b25bc` nimmt nur 0..3 an"
> ist **falsch** und darf nicht zitiert werden. ORIGINAL @`0x8004cea8 ori v0,zero,0x4` +
> @`0x8004ceb0 sb v0,0(v1)` mit `v1 = 0x800b25bc` (@`0x8004ce90-94`) schreibt sehr wohl **4**. Die
> Bytes sind in beiden Builds identisch. Der Blocker haelt trotzdem — auf dem `jal`-Argument. **[BELEGT]**

#### L5 — Der Ortsnamen-Resolver ist ein Stub **[BELEGT]**

ORIGINAL @`0x80026e4c` (Datei `0x1764c`) = `08 00 e0 03 21 10 00 00` = `jr ra` / `addu v0,zero,zero`.
Selbst wenn man L1–L4 fixt, traegt **jeder** Speicherstand den Ortsnamen-Index 0. Details in §3.

### 1.4 Gegenprobe: das Subsystem ist lebendig, nur unerreichbar **[BELEGT]**

Der Pfad, auf dem das Gate liegt, laeuft in jedem Raum-/Fade-Uebergang: `DAT_800B5359` wird von der
Hauptschleife @`0x8001c994 lbu v0,21337(v0)` gelesen, `addiu v1,v0,-1`, `sltiu v0,v1,5`, dann
`jr v0` ueber die Tabelle **@`0x8001069c`**:

```
[0]=0x8001c9c8  [1]=0x8001ca98  [2]=0x8001cbb8  [3]=0x8001cc34  [4]=0x8001cc70
(Modus N -> Eintrag N-1; [5] = 0x00005342 = Fremddaten)
```

> ⚠ **Falle:** Eine Lane nennt fuer diese Tabelle `[0x8001CA08, 0x8001CAD8, 0x8001CBF8, 0x8001CC74,
> 0x8001CCB0]` — **jeder Wert exakt +0x40 daneben**. Wer das uebernimmt, portiert die falschen fuenf
> Handler. Die oben stehenden Werte sind aus den Rohbytes (Datei `0xe9c`, in beiden EXEs identisch). **[BELEGT]**

Der Bit-18-Test @`0x8001cb78` liegt **innerhalb** des Modus-2-Handlers `[0x8001ca98, 0x8001cbb8)`, der
zweite Test @`0x8001ca00` im Modus-1-Handler. Beide werden ausgefuehrt — sie sind nur immer falsch.

### 1.5 Verdikt Frage 1

| Behauptung | Urteil |
|---|---|
| „vollstaendig aber abgeschaltet" | **Halb richtig.** Gilt fuer den Karten-Layer und fuer L4, nicht fuer L1–L3, L5. |
| „unvollstaendig" | **Richtig fuer die Spielseite.** L1/L2/L3/L5 sind Dev-Zustaende, keine Schalter. |
| „nie gebaut" | **Falsch.** 22 Funktionen, Boot-Init, erreichbarer LOAD-Menuepunkt. |

**Formulierung fuer die Wissensbasis:**
> RE1.5 hat einen fertigen, beim Boot initialisierten Memory-Card-Layer und einen funktionierenden
> LOAD-Einstieg im Titelmenue. Die SPEICHERN-Seite ist Dev-Ware: Devkit-Puffer `0x80200000`,
> provisorische I/O-Groesse `0x800`, ein `return 0`-Stub fuer den Ortsnamen und ein Anforderungs-Bit,
> das nirgends gesetzt wird — flankiert von einer Klartext-Meldung in 16 Raeumen, die das Speichern
> ausdruecklich fuer „this preview" absagt.

---

## 2. Frage 2 — Wie der gepatchte Build es hineinbaut

### 2.1 Reproduzierbarkeit **[WAHRSCHEINLICH]**

`c:/workspace/git/reAi/scripts/patch_save_final.py::patch_psx_exe()` (Zeile 1879), auf eine frische Kopie
der ORIGINAL-EXE angewandt, erzeugt laut Lane-A-Messung **byte-identisch** die Image-EXE. Ich habe die
Ausgabe nicht selbst nachgefahren, aber alle unten stehenden Aenderungen einzeln an den Rohbytes beider
EXEs verifiziert. **Warnung:** die skript-eigene Selbstverifikation meldet „All checks: PASS", druckt
aber drei veraltete Spot-Check-Abweichungen — sie taugt **nicht** als Beleg; der Byte-Vergleich schon.

### 2.2 Patch-Tabelle A — Aenderungen im Stock-Code **[BELEGT]**

| # | Datei-Offset | RAM | ORIGINAL | GEPATCHT | Zweck |
|---|---|---|---|---|---|
| H0 | `0x00001c` | Header | `t_size 0x000af000` | `0x000b1000` | +0x2000 Null-Scratch bei `0x800BF000..0x800C1000` |
| **[H]** | `0x00d39c` | `0x8001cb9c` | `lui a1,0x8020` | `lui a1,0x8016` | Save-Puffer in den 2-MB-RAM (L2) |
| **[C]** | `0x0167ec` | `0x80025fec` | `bne v0,zero,+3` | `beq zero,zero,+3` | Speichern auf **leerer** Karte erlauben |
| **[K]** | `0x016990` | `0x80026190` | `j 0x80026284` / `ori s1,zero,2` | `j 0x80026444` / `ori s0,zero,9` | „Karte voll"-Fehlerausgang → **Zustand 9 = LESEN** |
| **[M]** | `0x0169c0` | `0x800261c0` | `addiu a0,a0,0x1430` | `addiu a0,a0,0x0200` | Arbeitspuffer-Refresh spiegelt jetzt das Datei-Layout |
| **[P]** | `0x0169b4` | `0x800261b4` | `jal 0x80026f48` | `jal 0x8006eb50` | Erweiterter Pre-Save-Snapshot |
| **[E1]** | `0x016a20` | `0x80026220` | `ori v0,zero,0x0800` | `0x1680` | Schreibgroesse (L3) |
| **[E2]** | `0x016a40` | `0x80026240` | `ori a2,zero,0x0800` | `0x1680` | Lesegroesse (L3) |
| **[L]** | `0x016a90` | `0x80026290` | `memcpy(0x800B0DBC, buf, 0x1430)` (7 Worte) | `j 0x8006eaa8` + 6× `nop` | Selektiver Restore statt Block-Load |
| **[Cnt]** | `0x016c88` | `0x80026488`–`9c` | `DAT_800b0fbd++` (6 Instr.) | 6× `nop` | Zaehler-Inkrement verlegt (⚠ §2.7) |
| **[W2]** | `0x016ca8` | `0x800264a8` | `jal 0x80026594` | `jal 0x80070818` | Karten-Cleanup ueber Wrapper |
| **[I]** | `0x016e30` | `0x80026630` | `lui at,0x800b` | `ori v0,zero,0x0001` | `DAT_800B5457` = 1 statt 2 (BG-Reload) |
| **[Z]** | `0x01764c` | `0x80026e4c` | `jr ra` / `v0=0` | `j 0x80070890` / `nop` | **Ortsnamen-Resolver** (§3) |
| **[N]** | `0x0123f8` | `0x80021bf8` | `lui at,0x800b` / `sh v1,0x0FE4(at)` | `j 0x8006ebd0` / `nop` | Kamera-/Raum-Setup-Hook (Post-Load) |
| **[O]** | `0x012408` | `0x80021c08` | `lui at,0x800b` / `sw v0,0xC790(at)` | `j 0x8006eb3c` / `nop` | Zone-Pointer-Sync |
| **[U]** | `0x00dd70` | `0x8001d570` | `jal 0x8003e4f4` | `jal 0x8006ea84` | `room_flag_init` einmalig ueberspringen |
| **[Y]** | `0x00dda4` | `0x8001d5a4` | `jal 0x800314b0` | `jal 0x800707a4` | Waffen-Restore nach der Messer-Init |
| **[W]** | `0x02a174` | `0x80039974` | `jal 0x80043eac` | `jal 0x80070720` | CD-Guard (im Auslieferungsstand Pass-Through) |
| **[PB]** | `0x02f888` | `0x8003f088` | `jal 0x8003f0a0` | `jal 0x8007153c` | `PATCH.BIN`-Nachlader an den SCD-Frame gehaengt |
| **[B]** | `0x064d54` | `0x80074554` | `0x800404f4` | `0x8006ea20` | **SCD-Opcode-Tabelle[0x2B] → SCD_HOOK** |
| **[D2]** | `0x064ea0` | `0x800746a0` | `0x80043084` | `0x8007084c` | **AOT-sce-Tabelle[1] → AOT_TYPE1_HOOK** |

Dazu ~23 weitere Redirects, die **nicht** zum Speichern gehoeren (Item-Box, §2.8).

### 2.3 Patch-Tabelle B — eingefuegte Funktionen **[BELEGT]**

| RAM | Datei | Name | Groesse | Zweck |
|---|---|---|---|---|
| `0x8006ea20` | `0x05f220` | `SCD_HOOK` | 92 B | Sentinel `0xFE` auf SCD-Opcode 0x2B → Trigger, Index 0 |
| `0x8006ea7c` | — | *Daten:* Kamera-Stash u16 | 2 B | Post-Load |
| `0x8006ea7e` | — | *Daten:* Pending u8 | 1 B | Post-Load |
| `0x8006ea7f` | — | *Daten:* RFI-Skip u8 | 1 B | `room_flag_init` einmal aus |
| `0x8006ea80` | — | *Daten:* Flag0-Stash u32 | 4 B | Post-Load |
| `0x8006ea84` | `0x05f284` | `ROOM_FLAG_SKIP` | 36 B | `room_flag_init` genau einmal ueberspringen |
| `0x8006eaa8` | `0x05f2a8` | `MEMCARD_EXIT_HOOK` | 100 B | Selektiver Restore-Einstieg (Ladepfad) |
| `0x8006eb0c` | `0x05f30c` | `FLAGS_1_11_MERGE` | 48 B | Flag1/Flag11 OR-mergen, Inventar-Bit 15 loeschen |
| `0x8006eb3c` | `0x05f33c` | `ZONE_PTR_HOOK` | 20 B | `0x800AC790` **und** `0x800AC794` schreiben |
| `0x8006eb50` | `0x05f350` | `EXTENDED_SNAPSHOT` | 76 B | 4 Originalfelder + Flag 0/1/2/11 |
| `0x8006eb9c` | `0x05f39c` | `RESTORE_CODE` | 44 B | Flag0/Flag2 stashen (mit Bit-18-Clear) |
| `0x8006ebd0` | `0x05f3d0` | `CAMERA_HOOK` | 68 B | Post-Load-Weiche im Kamera-Setup |
| `0x8006eff0` | `0x05f7f0` | `HP_SAVE` | 48 B | HP + Status sichern, Bit 18 aus Flag0 loeschen |
| `0x8006f020` | `0x05f820` | `HP_POLL` | 84 B | Kamera-Store nachholen |
| `0x8006f074` | `0x05f874` | `INVENTORY_LOAD` | 84 B | 11 Worte + HP stashen |
| `0x8006f0c4`.. | — | *Daten:* Waffen-Slot/Typ, 44-B-Stash, HP-Stash, Flag2-Stash | 60 B | Post-Load |
| `0x8006f100` | `0x05f900` | `LOAD_RESTORE_FULL` | 116 B | Flags/HP/Per-Raum-Kopf zurueckschreiben |
| `0x80070340` | `0x060b40` | `BOX_LOAD` | 56 B | Item-Box (200 B) restaurieren |
| `0x80070564` | `0x060d64` | `RELOAD_CHECK` | 96 B | Zweiten Durchlauf der Phasen-Maschine ausloesen |
| `0x800705c4` | `0x060dc4` | `RELOAD_RFI` | 80 B | **TOT** (0 Xrefs in EXE **und** `PATCH.BIN`) |
| `0x800706e0` | `0x060ee0` | `STASH_PERROOM` | 40 B | 0xB0-Block nach `0x800B0FF8` |
| `0x80070720` | `0x060f20` | `CD_FIX` | 8 B | **Pass-Through** `j 0x80043eac` |
| `0x80070748` | `0x060f48` | `WEAPON_CAPTURE` | 52 B | Waffe + Sentinel `0xA5`; **Zaehler-Inkrement** |
| `0x8007077c` | `0x060f7c` | `WEAPON_STASH_CHECK` | 40 B | Sentinel `0xA5` pruefen |
| `0x800707a4` | `0x060fa4` | `FUN314B0_WRAPPER` | 116 B | Waffe + Modell nach dem Laden wiederherstellen |
| `0x80070818` | `0x061018` | `CARD_CLEANUP_CDFIX` | 52 B | **Pass-Through** `j 0x80026594` |
| `0x8007084c` | `0x06104c` | `AOT_TYPE1_HOOK` | 68 B | Sentinel `0xFE` auf AOT-sce-Typ 1 → Trigger, Index 1 |
| `0x80070890` | `0x061090` | **`SAVE_LOC_FUNC`** | 44 B | Ortsnamen-Resolver (§3) |
| `0x800708bc` | `0x0610bc` | `SCD_SAVE_RET` | 16 B | `game_state[3] = 0` |
| `0x800708cc` | `0x0610cc` | `ITEM_BOX_CHECK` | 32 B | Sentinel `0xFD` (Item-Box) |
| `0x800708f0` | `0x0610f0` | `ITEM_BOX_OPEN` | 80 B | Item-Box → `PATCH.BIN` |
| `0x8007153c` | `0x061d3c` | `PATCH_LOADER` | 160 B | `PATCH.BIN` (LBA 73388, 0x5000 B) → `0x801F0000` |
| `0x800715dc` | `0x061ddc` | `CD18_TRAMPOLINE` | 40 B | Item-Box |
| `0x80071920` | `0x062120` | `DEPOSIT_HOOK` | 172 B | Item-Box-Fallback (⚠ **nicht** tot, §7) |

> ⚠ **Falle: „alle Caves waren im Original 0x00-gefuellt" stimmt NICHT.** Selbst geprueft:
> `CAMERA_HOOK` @`0x8006ebd0` frisst **20 Nichtnull-Bytes** einer u32-Parametertabelle ab `0x8006ebd4`
> (`05 00 00 00 | 14 00 00 00 | 0a 00 00 00 | …`); `LOAD_RESTORE_FULL` @`0x8006f100` frisst 11 Bytes ab
> `0x8006f154`; `ITEM_BOX_OPEN` @`0x800708f0` frisst 62 Bytes ab `0x80070928`, darunter den
> ASCII-Record **`"SHIKICHI  A   "`** (die Nachbarn `B`/`C` @`0x8007094a`/`0x80070964` ueberleben).
> Praktisch folgenlos (keine statische Referenz in diese Bereiche gefunden), aber als Praemisse falsch. **[BELEGT]**

### 2.4 Die RDT-Seite: 3 Dateien, 11 Bytes **[BELEGT]**

Eigener ISO-Diff, jede Aenderung mit Kontext:

**`ROOM1070.RDT` @`0x1568`: `0x14 → 0xFE`** (AOT-sce-Record, `u16@+0`)
```
van: e0 15 a0 0f e8 03 |14 00| ff ff 00 00 50 0d
mod: e0 15 a0 0f e8 03 |fe 00| ff ff 00 00 50 0d
```

**`ROOM1150.RDT`** @`0x10ed`, `0x10f8`, `0x10f9`, `0x10fb`, `0x1100`
```
van: ff 01 00  29 06  2b |01| ff ff  02 00  2a 00  3c 01 01 00  |29 08|  2b |03| ff ff  02 00  |2a 00|
mod: ff 01 00  29 06  2b |fe| ff ff  02 00  2a 00  3c 01 01 00  |00 00|  2b |fd| ff ff  02 00  |00 00|
```
* `2B 01 FF FF` → `2B FE FF FF` — **Save-Sentinel** (SCD `Message_on`, msgIndex `0xFE`, Arg `0xFFFF`)
* `2B 03 FF FF` → `2B FD FF FF` — **Item-Box-Sentinel** (nicht Save)
* `29 08` → `00 00` (`Cut_chg` NOP) und `2A 00` → `00 00` (`Cut_old` NOP) — die Kamera-Umschaltung
  auf die Objekt-Nahaufnahme wird ausgebaut, damit der Kartenbildschirm nicht auf dem Close-up sitzt.

**`ROOM1151.RDT`** (Elza-Variante) identisch, Offsets `0x10cb`, `0x10d6`, `0x10d7`, `0x10d9`, `0x10de`.

`ROOM1071` (Elza-Variante von 1070) ist **unveraendert** — Elza hat im Mod nur EINEN Save-Punkt. **[BELEGT]**

**Kollisions-Scan ueber alle 240 `ROOM*.RDT`** (selbst gefahren):
* `2B FE FF FF` (Save-Sentinel): **0 Treffer** → kollisionsfrei.
* `2B FD FF FF` (Item-Box-Sentinel): **2 Treffer**, `ROOM2070.RDT` und `ROOM2071.RDT`, je @`0x0174`
  → **echter Defekt** des Mods, falls man die Item-Box mitportiert. Fuer Save irrelevant. **[BELEGT]**

**Warum ausgerechnet die Message-Id ueberschrieben wird:** `0x14` / `0x01` sind exakt die
Message-Ids der Speicherpunkte aus der Port-Registry (`re15_port/engine/src/re15_savepoint.c:17-28`:
`{0x1070,0x14}`, `{0x1150,0x01}`, `{0x1151,0x01}`). Der Mod opfert also die Original-Meldung des Raums
(„Save is not available in this preview") und ersetzt sie durch den Sentinel. **[BELEGT]**

### 2.5 Die beiden Trigger **[BELEGT]**

**Trigger 1 — SCD (Typ-0-AOT), ROOM1150/1151:** Tabelleneintrag `0x80074554`
(Basis `0x800744a8` + `0x2b*4`) `0x800404f4 → 0x8006ea20`.

```
8006ea20  lw   v0,28(a0)          ; SCD-PC
8006ea28  lbu  v1,1(v0)           ; msgIndex
8006ea30  bne  v1,t0,0x8006ea74   ; t0 = 0xFE
8006ea34  lhu  v1,2(v0)           ; (Delay) Argument
8006ea3c  bne  v1,t0,0x8006ea74   ; t0 = 0xFFFF
8006ea44  lw   v0,-13768(at)      ; *0x800ACA38
8006ea48  lui  v1,0x4
8006ea4c  or   v0,v0,v1           ; |= 0x00040000
8006ea50  sw   v0,-13768(at)
8006ea54  ori  v0,zero,0x1
8006ea58  sb   v0,21337(at)       ; *0x800B5359 = 1
8006ea5c-68 SCD-PC += 4           ; Instruktion konsumiert (wie das Original)
8006ea6c  j    0x800708bc         ; SCD_SAVE_RET -> game_state[3] = 0
8006ea70  ori  v0,zero,1          ; (Delay) return 1
8006ea74  j    0x800708cc         ; kein Match -> ITEM_BOX_CHECK (0xFD) -> j 0x800404f4
```

**Trigger 2 — AOT sce-Typ 1 (MESSAGE), ROOM1070:** Tabelleneintrag `0x800746a0`
(Basis `0x8007469c` + `1*4`) `0x80043084 → 0x8007084c`.

```
8007084c  lhu  v0,0(a0)           ; sce-Param 0
80070854  bne  v0,v1,0x80070888   ; v1 = 0xFE  -> sonst j 0x80043084
80070860-6c  *0x800ACA38 |= 0x00040000
80070870-74  *0x800B5359 = 1
80070878-7c  sb v0(=1),0x800B0FBF ; game_state[3] = 1
80070880  jr   ra / addiu v0,zero,0
```

**Wichtig:** Beide Sentinel sind **Operandenwerte bestehender Opcodes**, kein neuer Opcode. Beide
Original-Handler (`0x800404f4` und `0x80043084`) lesen exakt dieselben Felder und landen in derselben
`FUN_80027e68(0, 0x300, msg, arg<<16)` — der Sentinel ist also eine im Spiel nie vorkommende
Message-Id. **[BELEGT]**

> **Nebenbefund [BELEGT]:** Der AOT-sce-Typ **2** waere im Original bereits ein NATIVER Save-Trigger —
> `FUN_800430bc` setzt `*0x800B5359 = 1` (@`0x800430d4`) und `pause_flags |= 0xFF000000`
> (@`0x800430dc-e4`). Der Mod nutzt ihn nicht; ob irgendein ausgeliefertes RDT sce-Typ 2 verwendet,
> ist **[OFFEN]** (Zensus ueber 240 RDTs nicht gefahren). Falls ja, waere das der byte-true
> Save-Einstieg — hoechst relevant fuer den Port.

### 2.6 Ablauf SAVE **[BELEGT]**

1. **Ausloesung.** Spieler untersucht Telefon/Schreibmaschine.
   *(a)* ROOM1150/1151: SCD fuehrt `Message_on(0xFE, 0xFFFF)` aus → `SCD_HOOK` → `game_state[3] = 0`.
   *(b)* ROOM1070: AOT-Dispatch Typ 1 mit `act = 0xFE` → `AOT_TYPE1_HOOK` → `game_state[3] = 1`.
   Beide setzen `Flag0 |= 0x00040000` und `DAT_800B5359 = 1`.
2. **Naechster Frame — Phasen-Maschine** (Original-Code, unveraendert). Phase 1 @`0x8001c9c8`:
   Fade-out, Phase := 2. Phase 2 @`0x8001ca98`: `Flag0 |= 0x4000`, Inventar-Bit-15-Test, dann der
   Bit-18-Test @`0x8001cb78` → Bit loeschen → `FUN_80025c00(0, 0x80160000, 0, 0)`.
3. **Kartenmenue** (Original, 14 Zustaende). Zustand 0 Slots pruefen → 4 Kartendaten laden → 5
   Slot-Verzeichnis bauen (⚠ **hier entsteht der Ortsname der Slot-Liste**, §3.2) → 6 Liste → 7
   Auswahl. Leerer Slot → Zustand 8; belegter Slot → 13 (Ueberschreiben) → 8.
   *(`[C]` macht Zustand 5 auf leerer Karte ueberhaupt erst durchlaessig.)*
4. **Zustand 8 — Schreiben** (@`0x800261b4`):
   ```
   800261b4  jal 0x8006eb50            ; [P] EXTENDED_SNAPSHOT -> HP_SAVE -> WEAPON_CAPTURE
                                       ;     -> PATCH.BIN 0x801F08B4 (Item-Box) -> jr ra
   800261bc  memcpy(cardbuf + 0x200 + slot*0x80, 0x800B0FBC, 0x1230)   ; [M] Arbeitspuffer-Refresh
   800261ec  s0 = 0x800B0FBC - 0x200 = 0x800B0DBC
   800261f0  jal 0x80026e54            ; Titel bauen (a0 = sp+424, a1 = s0)
   80026220  ori v0,zero,0x1680        ; [E1] Groesse
   80026224  jal 0x800271a8            ; a0 = 0x800B0DBC  <-- QUELLE IST RAM
   ```
   `FUN_800271a8` legt vor dem Schreiben zwei Felder **in die Quelle**: Blockanzahl
   @`0x80027234 sb v1,3(s5)` und Titel @`0x8002724c addiu a0,s5,4` + `strcpy`. Geschrieben wird
   @`0x800272b4 jal 0x8006e1b0` mit `a1 = s5 = 0x800B0DBC`.
5. **Nachbereitung.** `sb zero,3(v0)` @`0x80026484` markiert den Slot als belegt; `card_cleanup`
   ueber `CARD_CLEANUP_CDFIX`; `[I]` setzt `DAT_800B5457 = 1` (BG-Reload); Phase 3–5 Fade-in.

> **Kern-Einsicht, die eine ganze Lane falsch hatte:** Der Kartenschreiber liest **aus dem RAM**
> (`0x800B0DBC`), **nicht** aus dem Kartenpuffer. Deshalb gilt **Karten-Datei-Offset == RAM-Adresse −
> `0x800B0DBC`** — und zwar **auch im ORIGINAL**. Der `[M]`-Patch (`+0x1430 → +0x200`) betrifft nur
> einen Arbeitspuffer-Refresh, nicht den Transport. **[BELEGT]**

### 2.7 Kartenlayout **[BELEGT]** (Offsets an der echten `.mcd` verifiziert)

| Datei-Offset | RAM | Inhalt |
|---|---|---|
| `+0x0000` | `0x800B0DBC` | Blockkopf |
| `+0x0003` | `0x800B0DBF` | Blockanzahl (=1) |
| `+0x0004` | `0x800B0DC0` | **Titel**, 0x3C sichtbare Bytes Shift-JIS |
| `+0x0200` | `0x800B0FBC` | `game_state[0]` (=0x0A, vom Item-Box-Hook) |
| `+0x0201` | `0x800B0FBD` | `save_count` |
| `+0x0202` | `0x800B0FBE` | Charakter-Bits (Bit 0: 0=Leon, 1=Elza) |
| **`+0x0203`** | **`0x800B0FBF`** | **ORTSNAMEN-INDEX (Patch-Erfindung, §3)** |
| `+0x0204`.. | `0x800B0FC0`.. | Spieler X/Y/Z |
| `+0x0226` | `0x800B0FE2` | Raum-ID |
| `+0x0228` | `0x800B0FE4` | Kamera-Cut |
| `+0x023C` | `0x800B0FF8` | 0xB0-Block (Per-Raum-Kopf) |
| `+0x02F0` | `0x800B10AC` | 0x1140 B Per-Raum-Flags + Inventar |
| `+0x1430/34/38/3C` | `0x800B21EC/F0/F4/F8` | Flag 0 / 1 / 2 / 11 |
| `+0x1440/44` | `0x800B21FC/2200` | HP / HP-Status |
| `+0x1448/49/4A` | `0x800B2204..06` | Waffen-Sentinel `0xA5` / Slot+1 / Typ |
| `+0x144C` | `0x800B2208` | 200 B Item-Box |
| **Summe** | | **`0x1680`** |

**⚠ Save-Zaehler: der Mod weicht hier bewusst vom Original ab. [BELEGT]**
ORIGINAL @`0x80026488`–`9c`: `lbu 0x800B0FBD / addiu 1 / sb` — das laeuft **NACH** dem Titelbau
(@`0x800261f0`) und **NACH** dem Kartenschreiben (@`0x80026224`), also **Post-Inkrement**: der erste
Save eines frischen Spiels heisst **`/00/`**.
GEPATCHT: diese 6 Instruktionen sind `nop`; das Inkrement steht in `WEAPON_CAPTURE` @`0x80070764`–`70`,
also **vor** dem Titelbau → **Pre-Inkrement**, erster Save = `/01/`. Genau das zeigt die echte Karte
(`/01/` und `/02/`).
→ **Der Port darf hier NICHT der Mod-Karte folgen.** `re15_port/platform/pc/main.c:2808/2825`
macht es bereits richtig (`scount = s_save_counter` schreiben, `s_save_counter = scount+1` nur bei
Erfolg) — das ist byte-true zum ORIGINAL. Nicht „fixen".

### 2.8 Ablauf LOAD **[BELEGT]**

Der LADEN-Weg wird ueber `[K]` erreicht: Zustand 7, „Slot leer + Karte voll"-Ausgang @`0x80026190`
`j 0x80026284 / ori s1,zero,2` (Fehler) wird zu `j 0x80026444 / ori s0,zero,9` (**Zustand 9 = LESEN**).
Der native LADEN-Zweig (`modus != 0`) bleibt bestehen, ist aber unerreichbar, weil `FUN_80025c00`
im Spiel nur mit `modus = 0` gerufen wird.

Zustand 9: `FUN_80027368(buf, name, 0x1680, seek=0)` — dadurch ist **Puffer-Offset == Datei-Offset**.
Danach statt des Block-`memcpy` die `[L]`-Kette:

```
MEMCARD_EXIT_HOOK   0x8006eaa8  card_cleanup; memcpy(0x800B0FBC, buf+0x200, 0x0A)      <-- inkl. Ortsindex
                                memcpy(0x800B10AC, buf+0x2F0, 0x1140)
                                Raum <- buf+0x226 ; Kamera -> Stash 0x8006EA7C ; Pending 0x8006EA7E
STASH_PERROOM       0x800706e0  memcpy(0x800B0FF8, buf+0x23C, 0xB0) ; RFI-Skip 0x8006EA7F = 1
INVENTORY_LOAD      0x8006f074  11 Worte 0x800B10AC -> 0x8006F0C8 ; HP -> 0x8006F0F4/F8
BOX_LOAD            0x80070340  200 B buf+0x144C -> 0x800BFB00 ; 0x800719CA = 1
WEAPON_STASH_CHECK  0x8007077c  buf+0x1448 == 0xA5 -> Slot/Typ nach 0x8006F0C4/C5
RESTORE_CODE        0x8006eb9c  Flag0 = buf+0x1430 & ~0x40000 -> Stash ; Flag2 = buf+0x1438 -> Stash
FLAGS_1_11_MERGE    0x8006eb0c  Flag1 |= buf+0x1434 ; Flag11 |= buf+0x143C ; Flag1 &= 0xFFFF7FFF
                                j 0x800264b4 (Epilog von FUN_80025c00)
```

**Post-Load, ueber `[N]`** (`CAMERA_HOOK` haengt im Kamera-/Raum-Setup und laeuft in JEDEM Raumwechsel):

```
8006ebd0  lhu t8,[0x8006EA7C]          ; Kamera-Stash
8006ebdc  lw  t7,[0x800ACA3C]
8006ebe0  bne t8,zero,0x8006ebf8       ; Stash == 0 -> normaler Betrieb
8006ebe8  sw  t7,[0x800ACA3C] ; j 0x8006f020   (HP_POLL, holt den Kamera-Store nach)
8006ebf8  lbu t4,[0x8006EA7E]          ; Pending
8006ec00  bne t4,zero,0x8006ebe8       ; PENDING != 0 -> ZURUECK auf HP_POLL
8006ec0c  j   0x8006f100               ; nur Stash!=0 UND Pending==0 -> LOAD_RESTORE_FULL
8006ec10  sh  v1,4068(at)              ; (Delay) der ersetzte Original-Store 0x800B0FE4
```

> ⚠ **Falle:** Eine Lane beschreibt diese Weiche **invertiert** („Pending != 0 → LOAD_RESTORE_FULL").
> Die Bytes @`0x8006ec00` sagen das Gegenteil. Die tatsaechliche Frame-Reihenfolge ist:
> **erster** Durchlauf (Stash gesetzt, Pending gesetzt) → `HP_POLL` → `RELOAD_CHECK` @`0x80070564`;
> das loescht Pending, loescht Bit 18 aus Flag0 **und** aus dem Stash, setzt `DAT_800B5359 = 1`
> erneut — diesmal **ohne** Bit 18, also faehrt die Phasen-Maschine einen **normalen Raumwechsel**
> in den gespeicherten Raum. **Erst ein spaeterer** Durchlauf (Stash gesetzt, Pending geloescht) faellt
> auf `LOAD_RESTORE_FULL` durch (Flag0/Flag2 OR, 44 B Per-Raum-Kopf, HP, Stashes leeren). Dabei
> unterdrueckt `ROOM_FLAG_SKIP` einmalig `room_flag_init`, und `FUN314B0_WRAPPER` stellt die
> ausgeruestete Waffe samt Modell wieder her. **[BELEGT]**

### 2.9 Was am Patch NICHT Save ist **[BELEGT]**

Rund die **Haelfte** des Diffs ist **Item-Box**, nicht Speichern: Browse-Sprungtabelle
`0x8001126c/70/78/8c` → `PATCH.BIN`; `nop`-Setzungen `0x80045ffc`, `0x8004661c`, `0x800466e8`,
`0x800466f0`, `0x8004c688`, `0x8004c6fc`, `0x8004c718`, `0x8004c720`, `0x8004c734`;
Highlight-Gate-Rewrite `0x8004c928`–`0x8004c93c` (`bne`→`beq` gegen `state[0x25C9] == 0xFF`);
Redirects `0x8004cd18`, `0x8004cd6c`, `0x8004ce04`, `0x8004d254`, `0x8004d27c`, `0x80049a3c`;
Tabelle `0x80074c88[3]` `0 → 0x801F0A14`; `0x800466b8/c4/c8` → `jal 0x801F0E00` (in diesem Build ein
reiner `jr ra`-Stub). Wer den Save-Mechanismus portiert, kann das **komplett ignorieren**.

**Toter Code im Auslieferungsstand:** `RELOAD_RFI` @`0x800705c4` und das originale `FUN_80026f48`
@`0x80026f48` haben ueber EXE **und** `PATCH.BIN` **null** Referenzen. **[BELEGT]**

---

## 3. Frage 3 — Wie der Ortsname entsteht, und warum es genau zwei sind

### 3.0 Der Kern in drei Saetzen

> Der Mod ersetzt den `return 0`-Stub `FUN_80026e4c` durch eine eigene Funktion, die ihren **Aufrufer
> am Ruecksprungwert `ra`** unterscheidet. Beide Aufrufer bekommen denselben Index — nur aus zwei
> verschiedenen Quellen: der **Save-Pfad** aus dem Live-Global `0x800B0FBF`, der **Anzeige-Pfad** aus
> dem geladenen Kartenblock. Und dieses Byte ist **keine** Raum-Aufloesung, sondern eine **Konstante
> pro Save-Punkt**: `SCD_SAVE_RET` schreibt 0, `AOT_TYPE1_HOOK` schreibt 1. Es sind genau zwei
> Ortsnamen, weil es genau zwei gepatchte Save-Punkte gibt.

### 3.1 Der Stub und der Hook **[BELEGT]**

```
ORIGINAL  Datei 0x1764c:  08 00 e0 03  21 10 00 00
  80026e4c  jr   ra
  80026e50  addu v0,zero,zero            -> RETURN 0, IMMER, a0 wird ignoriert

GEPATCHT  Datei 0x1764c:  24 c2 01 08  00 00 00 00
  80026e4c  j    0x80070890
  80026e50  nop
```

`SAVE_LOC_FUNC` @`0x80070890` (Datei `0x61090`; im ORIGINAL 64 B `0x00`):

```
80070890  lui  v1,0x8002
80070894  ori  v1,v1,0x6f04        ; v1 = 0x80026f04
80070898  beq  ra,v1,0x800708ac    ; <-- Aufrufer-Erkennung am Ruecksprungwert
8007089c  nop
800708a0  lbu  v0,3(a0)            ; PFAD A: Index aus dem Kartenblock
800708a4  jr   ra
800708ac  lui  v0,0x800b
800708b0  lbu  v0,4031(v0)         ; PFAD B: Index aus 0x800B0FBF (Live-RAM)
800708b4  jr   ra
```

**Warum der `ra`-Trick noetig ist:** Es gibt genau zwei Aufrufer (eigener Scan nach dem `jal`-Wort
`0x0c009b93` ueber das gesamte `.text`, plus Suche nach rohen Datenzeigern `4c 6e 02 80` an beliebiger,
auch unaligned Position, plus `lui/ori`-Adressbildung — **0 weitere Treffer in beiden Builds**):

| Aufrufer | `ra` | Kontext | `a0` beim Aufruf |
|---|---|---|---|
| `0x80026810` | `0x80026818` | Slot-Liste (`FUN_80026658`) | **Kartenblock-Kopf** (`cardbuf + 0x1430 + i*0x80`) |
| `0x80026efc` | `0x80026f04` | Kartentitel (`FUN_80026e54`) | `0x800B0DBC` (Header-Puffer — dort ist Byte 3 die *Blockanzahl*, **nicht** der Index) |

Beim Speichern **existiert noch kein Kartenblock**, beim Anzeigen **gibt es kein Live-Global**. Eine
Funktion, zwei Kontexte — daher die `ra`-Weiche. **[BELEGT]**

### 3.2 Anzeige 1 — der BIOS-Kartentitel (Pfad B) **[BELEGT]**

Das ist der Name, den ein PSX-Kartenmanager (und DuckStation) zeigt. Gebaut von `FUN_80026e54`
(in **beiden** Builds byte-identisch, der Patch aendert nur den Callee):

```
80026e58  lbu v0,[0x800B0FBE]            ; Charakter-Bits
80026e70  andi v0,v0,0x1
80026e74  beq v0,zero,0x80026e8c         ; bit0==0 -> Leon
80026e80  addiu a1,a1,1996               ; 0x800107CC = "ＢＩＯ　ＨＡＺＡＲＤ　「Ｅｌｚａ　／００／"
80026e94  addiu a1,a1,2040               ; 0x800107F8 = "ＢＩＯ　ＨＡＺＡＲＤ　「Ｌｅｏｎ　／００／"
80026e98  jal 0x8004ee38 / ori a2,zero,0x2a     ; memcpy 0x2A Bytes -> Titel[0..0x29]
80026ea0..f0  save_count /10 und %10 (Magic 0xCCCCCCCD, srl 3), addiert auf 0x4F (Vollbreiten-Null)
80026ec8  sb v1,37(s0)                   ; Titel+0x25 = Zehner
80026f00  sb v0,39(s0)                   ; Titel+0x27 = Einer   (Delay-Slot des Stub-Calls!)
80026efc  jal 0x80026e4c                 ; <-- ORTSNAMEN-INDEX
80026f04  addiu a0,s0,42                 ; Ziel = Titel+0x2A
80026f0c-18  a1 = idx * 0x13             ; sll/addu/sll/subu = *19
80026f20  addiu v0,v0,13864              ; = 0x80073628
80026f28  jal 0x8004ee38 / ori a2,zero,0x13
```

**Namenstabelle 1 — SJIS @`0x80073628`** (Datei `0x63e28`), Stride `0x13`, in ORIGINAL und GEPATCHT
**byte-identisch** (selbst dekodiert):

| idx | Datei | Shift-JIS | Bedeutung |
|---|---|---|---|
| 0 | `0x63e28` | 館　小部屋　１Ｆ」 | Herrenhaus, kleiner Raum 1F |
| 1 | `0x63e3b` | 館　ホール　１Ｆ」 | Herrenhaus, Halle 1F |
| 2 | `0x63e4e` | 中庭　部屋　Ｂ１」 | Innenhof, Raum B1 |
| 3 | `0x63e61` | 　寄宿舎　　１Ｆ」 | Wohnheim 1F |
| 4 | `0x63e74` | 　研究所　　Ｂ３」 | Labor B3 |
| 5 | `0x63e87` | 館　　物置　１Ｆ」 | Herrenhaus, Abstellraum 1F |
| 6 | `0x63e9a` | 中庭　通路　Ｂ１」 | Innenhof, Gang B1 |
| 7 | `0x63ead` | `00 00 00 'bu00:' 00…` | **Fremddaten** → die Tabelle hat **exakt 7 Eintraege** |

**Inhaltlich sind das RESIDENT-EVIL-1-Herrenhausnamen — Leftovers. Sie passen zu KEINEM
RE1.5-Polizeirevier-Raum.** **[BELEGT]**

### 3.3 Anzeige 2 — die In-Game-Slot-Liste (Pfad A) **[BELEGT]**

Das ist der Name, den der Spieler **im Spiel** in der Slot-Liste sieht. Gebaut von `FUN_80026658`
(ebenfalls in beiden Builds byte-identisch):

```
800267a8  ori  s2,zero,0x1430              ; Basis des Slot-Header-Caches
800267d4  addu s0,s2,v0                    ; s0 = cardbuf + 0x1430 + i*0x80
800267d8  lbu v0,2(s0) / andi 3            ; -> a0 = 0x18 (Leon) bzw. 0x19 (Elza)
800267f8  jal 0x800c00e4                   ; sysmes-Getter (DEBUG.BIN) ; memcpy 0xC
80026810  jal 0x80026e4c                   ; a0 = s0 = Kartenblock   <-- ORTSNAMEN-INDEX
80026818  ori  a0,zero,0x1a
8002681c  jal 0x800c00e4
80026820  addu a0,a0,v0                    ; (Delay) sysmes-Id = 0x1A + idx
8002682c  jal 0x8004ee38                   ; memcpy 0x10
800268ac  s2 += 0x80                       ; naechster Slot (5 Slots)
```

**Namenstabelle 2 — sysmes in `DEBUG.BIN`.** Getter `FUN_800c00e4` (RAW @`0x800c0000`): Tabelle bei
Datei-Offset `0x5f96`, `return tab + u16[tab + idx*2]`. Selbst dekodiert (Atlas: `0x00`=SP,
`0x0c`–`0x15`=`0`–`9`, `0x1d`–`0x36`=`A`–`Z`, `0x3d`–`0x56`=`a`–`z`):

| sysmes | Datei | Name |
|---|---|---|
| `0x1A` | `0x6197` | **Irons' Office** |
| `0x1B` | `0x61a6` | Medical Room |
| `0x1C` | `0x61b4` | West Staircase 1F |
| `0x1D` | `0x61c7` | Sewer Maintainance |
| `0x1E` | `0x61db` | Sewer Control Room |
| `0x1F` | `0x61ef` | Factory - Office |
| `0x20` | `0x6201` | Security Room |
| `0x21` | `0x6210` | Monitor Room |

**Harter Beweis fuer „genau 8":** der erste Eintrag der Offset-Tabelle steht auf `0x44`, also hat die
Tabelle **`0x44/2 = 34` Eintraege (`0x00`–`0x21`)** und endet hinter `0x21`. `0x22`+ liest bereits
String-Bytes als Offsets. `DEBUG.BIN` auf der Mod-Disc (LBA 339) ist **byte-identisch** mit
`re15_port/shared_assets/PSX/BIN/DEBUG.BIN` (md5 `c2c11aab8a0ae4c0c7fdb56fb33c3a74`) — **der Patch hat
keinen einzigen Namen angefasst.** **[BELEGT]**

**Ist Pfad A im Mod lebendig?** **JA** — das ist eine wichtige Korrektur (§7-K1). Beleg-Kette:
`FUN_80026ca8` (Zustand 5, in beiden Builds byte-identisch) baut den Cache bei `cardbuf+0x1430+i*0x80`,
indem es aus **jeder** Save-Datei `0x80` Bytes ab **Datei-Seek `0x200`** liest:

```
80026d04  addiu s4,s5,512                  ; Scratch = cardbuf + 0x200
80026d08  ori   s2,zero,0x1430
80026d3c  ori   a2,zero,0x80               ; Laenge 0x80
80026d40  jal   0x80027368
80026d44  ori   a3,zero,0x200              ; SEEK 0x200  <-- Datei-Offset des game_state
80026d4c  addu  a0,s5,s2   ; a1 = s4 ; memcpy 0x80 -> cardbuf + 0x1430 + i*0x80
```
und `FUN_80027368` ist nachweislich `open → lseek(a3) → read` (@`0x80027418 jal 0x8006e180`,
@`0x8002742c jal 0x8006e438`, Argument-Mapping `s4=a0 Ziel, s0=a1 Name, s2=a2 Laenge, s5=a3 Seek`
aus dem Prolog @`0x80027370`–`88`).

Also: **`cardbuf + 0x1430 + i*0x80` IST eine Kopie von Datei `[0x200, 0x280)` = `game_state[0..0x80)`**,
und `lbu v0,3(a0)` in Pfad A liefert `game_state[3]`. Auf der echten Karte steht dort `0x00` bzw. `0x01`
→ Pfad A zeigt korrekt **„Irons' Office"** bzw. **„Medical Room"**. **[BELEGT]**

> ⚠ Nicht verwechseln: `lbu v0,3(a0)` @`0x8002615c` (Zustand 7) liest ein **anderes** Array —
> `s4 = 0x801FF404`, Byte 3 = **Slot-Status** (`5` = leer). Reine Offset-Koinzidenz.

### 3.4 Der Traeger: `game_state[3]` = `0x800B0FBF` **[BELEGT]**

Eigener Konstanten-Propagations-Scan (lui/ori/addiu-Basisverfolgung ueber das gesamte `.text`,
Invalidierung bei jedem anderen Registerschreiber und bei `jal`):

* **ORIGINAL: 0 skalare Zugriffe** auf `0x800B0FBF`.
* Nachbarschaft `0x800B0FBC..0FC5` im ORIGINAL: 32 Zugriffe (u.a. `0x800B0FBD` Zaehler @`0x8002648c`/
  `0x8002649c`, `0x800B0FBE` Charakter @`0x80026e58`) — **kein einziger auf `+3`**.
* **GEPATCHT: genau 3** — `sb v0,…0FBF` @`0x8007087c` (schreibt 1), `lbu v0,…0FBF` @`0x800708b0`
  (Pfad B), `sb zero,…0FBF` @`0x800708c0` (schreibt 0).

Der Patch belegt also ein echtes Reservebyte, kollisionsfrei.

> **Praezisierung:** „nirgends benutzt" gilt nur fuer **skalare** Adressierung. Das Byte wird sehr wohl
> von Blockkopien **mitbewegt** (`memcpy 0x1230` ab `0x800B0FBC` @`0x800261c4`–`d8`; im ORIGINAL-Ladepfad
> `memcpy 0x1430` nach `0x800B0DBC` @`0x8002629c`) — es ist bereits Teil des Save-Blobs. Genau deshalb
> braucht der Index **keinen** eigenen Transport. **[BELEGT]**

### 3.5 Warum genau zwei **[BELEGT]**

Beide Schreiber setzen **Konstanten**, es gibt keinen datengetriebenen Pfad:

| Schreiber | Adresse | Wert | Save-Punkt | RDT |
|---|---|---|---|---|
| `SCD_SAVE_RET` | `0x800708bc`: `lui at,0x800b / sb zero,4031(at)` | **0** | Schreibmaschine | ROOM1150 / ROOM1151 |
| `AOT_TYPE1_HOOK` | `0x8007087c`: `sb v0,4031(at)` mit `v0 = 1` (@`0x80070870`) | **1** | Telefon | ROOM1070 |

**Es sind zwei Ortsnamen, weil es zwei gepatchte Save-Punkte gibt — nicht, weil irgendetwas den Raum
aufloest.** Die uebrigen 6 Speicherpunkt-Raeume (ROOM1120/1121, ROOM2010/2011, ROOM30A0/A1,
ROOM30B0/B1, ROOM4010/4011, ROOM5010/5011) hat der Mod nie angefasst.

### 3.6 End-to-End-Beweis an der echten Memory Card **[BELEGT]**

`C:/Users/mjoedicke/AppData/Local/DuckStation/memcards/re15_save_final_1.mcd` (131072 B), selbst geparst.
Directory-Frames 1/2 Status `0x51`, je 8192 B, `BISLPS-002220` / `BISLPS-002221`; Frame 3 leer (`0xA0`).

| | Block 1 (@`0x2000`) | Block 2 (@`0x4000`) |
|---|---|---|
| `+0x0000` | `00 00 00 01` | `00 00 00 01` |
| Titel (`+4`, SJIS) | `ＢＩＯ　ＨＡＺＡＲＤ　「Ｌｅｏｎ　／０１／館　小部屋　１Ｆ」` | `…「Ｌｅｏｎ　／０２／館　ホール　１Ｆ」` |
| Titel `[0x25]/[0x27]` | `0x4f` / `0x50` (= „01") | `0x4f` / `0x51` (= „02") |
| `game_state` (`+0x200`) | `0a 01 00 00 44 a7 00 00 72 b2 …` | `0a 02 00 01 20 3c 00 00 5c 18 …` |
| **`game_state[3]` (`+0x203`)** | **`0x00`** → SJIS `[0]` = 館 小部屋 1F ✔ | **`0x01`** → SJIS `[1]` = 館 ホール 1F ✔ |
| Raum (`+0x226`) | `0x0015` = **ROOM1150** (Schreibmaschine) ✔ | `0x0007` = **ROOM1070** (Telefon) ✔ |
| Kamera (`+0x228`) | `0x0002` | `0x0002` |
| Flag0 (`+0x1430`) | `00 40 c2 40` | `00 40 c2 40` |
| Waffe (`+0x1448`) | `a5 04 08` | `a5 04 08` |

Damit ist die Kette **RDT-Sentinel → Hook → `game_state[3]` → `SAVE_LOC_FUNC` → Titel** geschlossen.

### 3.7 ⚠ Der Mod-Ortsname ist semantisch falsch **[BELEGT/WAHRSCHEINLICH]**

Debug-Menue-Namen der Save-Raeume (`DEBUG.BIN` @`0x2642`, 26 B/Satz, via
`analysis/bug_save_room_name.md:50-56`) **[WAHRSCHEINLICH]**:
`1070 = LOBBY OFFICE`, `1120 = 3F WEST SIDE`, `1150 = CHIEF OFFICE`, `2010 = BREAK ROOM`,
`30A0 = NORTH OFFICE`, `30B0 = SOUTH OFFICE`, `4010 = SECURITY ROOM`, `5010 = SECURITY ROOM`.

Abgleich mit der Mod-Zuweisung:

| Index | Save-Punkt | Debug-Name | sysmes-Anzeige | SJIS-Kartentitel | Urteil |
|---|---|---|---|---|---|
| 0 | ROOM1150 | CHIEF OFFICE | „Irons' Office" | 館 小部屋 1F | **passt** (Irons *ist* der Chief) |
| 1 | ROOM1070 | LOBBY OFFICE | „Medical Room" | 館 ホール 1F | **falsch** |

Der Autor hat 0 und 1 **gegriffen, nicht abgeleitet**. Fuer die SJIS-Tabelle passt ohnehin beides nicht
(RE1-Herrenhausnamen). **Das ist keine referenzfaehige Zuordnung.**

---

## 4. Frage 4 — Was das Vorprojekt gelernt hat

Dieser Abschnitt ist ueberwiegend **[WAHRSCHEINLICH]** (Sekundaerquellen aus `c:/workspace/git/reAi`),
wo nicht anders markiert. Er ist trotzdem der wertvollste Teil des Dossiers, weil er teuer erkauft ist.

### 4.1 Was FUNKTIONIERT **[WAHRSCHEINLICH]**

Save + Load an Schreibmaschine (ROOM1150/1151) und Telefon (ROOM1070); Raum/Position/Kamera-Restore;
globale Flags 0/1/2/11; Per-Raum-Flags (176 B); Inventar; HP + Status; ausgeruestete Waffe inkl. Modell;
Item-Box-Inhalt (200 B); Save-Zaehler; per-Slot-Ortsname (2 Varianten); **kein** Freeze bei
Save→Tuer und Load→Tuer.

### 4.2 Was NICHT funktioniert — „mehr oder minder erfolgreich" konkret **[WAHRSCHEINLICH]**

1. **AUDIO nach dem Speichern.** Bug 2 „alle SFX weg nach Save", Bug 3 „Shotgun-Crash". Nach dem
   v1.24-att2-Fix kamen die Sounds zurueck, aber es entstand ein neues Fehlerbild „alle Waffen spielen
   den Messer-SE". Die als kanonisch identifizierte Loesung (`PB_WEAPON_SFX_REINSTALL` am
   Save-FSM-Exit `0x8001CC00`, ~40 B) wurde **nie gebaut**. Der letzte Stand (Phase-2.2-C-Stub
   @`0x801F0E00`) steht auf **„built, all 42 patch tests green; user test pending"** (2026-05-18)
   — und ist im **ausgelieferten** Image **nicht enthalten**: `PATCH.BIN` @Datei `0xE20` (RAM
   `0x801F0E00`) ist dort **komplett Null**. **[BELEGT]**
2. **Blut-/Schadens-Textur nach Load.** Root Cause vollstaendig analysiert (3 Bloecke: 792 B
   Partikel-Slots @`0x800A73B8`, 152 B Sprite-State @`0x800B25CC`, Entity-Felder `+0x9E`/`+0x178`/
   `+0x17C`), Fix aus Aufwandsgruenden **DEFERRED**.
3. **Ortsname ist eine 2-Wert-Konstante, kein Resolver** (§3.5) — und Index 1 ist semantisch falsch (§3.7).

### 4.3 Die vier teuer erkauften Wissensbloecke **[WAHRSCHEINLICH]**

**(a) GSB-Gefahrenzone** (`reAi/memory/save-system.md:3-52`). Beim LOAD darf **nicht** der ganze Block
zurueckgeschrieben werden:

| Bereich | Status |
|---|---|
| `+0x00`..`+0x09` (10 B: Word0, Char-Flag, X/Y/Z) | **SICHER** |
| `+0x14` `scd_var[0-5]` | korrumpiert AOT-Dispatch |
| `+0x20` Kamera-Index + Raum/Stage | Out-of-Bounds-Crash |
| `+0x24` Floor/Area + Transition | Crash |
| `+0x28` Prev-Floor + Rotation | Crash |
| `+0x3C`..`+0xEC` Per-Raum-Flags | wird von `room_flag_init` @`0x8003e4f4` ohnehin genullt |
| ab `+0xF0` = `0x800B10AC` (0x1140 B) | **SICHER** |

Empirisch belegt durch die Crash-Historie v67…v104: Teil1 = `0x1230` CRASH, `0x3C` CRASH, `0x20`
anfangs ok, `0x14` scd_var-Problem, **`0x0A` STABIL**. Genau das erklaert, warum der ausgelieferte
Ladepfad `memcpy(0x800B0FBC, buf+0x200, 0x0A)` macht (§2.8) — **10 Bytes, nicht mehr**. **[BELEGT
fuer die 10 Bytes, [WAHRSCHEINLICH] fuer die Feld-Semantik.]**

**(b) Waffen-Feldsemantik — inkl. eines jahrelangen Irrtums:**
* `0x800B25C8` = `equipped_slot_index` = **das** Steuerfeld (`0x00` Messer, `0x01` Handgun, `0x03`
  Shotgun, `0x80` nicht ausgeruestet). Liegt **ausserhalb** des Kartenschreibbereichs (endet
  `0x800B243C`) → braucht ein Post-GSB-Relay.
* `entity+0x09` (`0x800ACA5D`) = **abgeleiteter** Waffentyp; `0x00` = komplett funktionslos.
* `entity+0x82` (`0x800ACAD6`) = **FLOOR NUMBER, NICHT Waffe.** Die Versuche v91–v94 zielten alle
  auf dieses falsche Feld. **Byte-Beweis [BELEGT]:** @`0x8001d7ac lbu v1,-13610(v1)` = `0x800ACAD6`,
  dann `sll v1,3 / subu / sll,5 / addu / sll,3` = `*1800`, dann `subu v0,zero,v0` → `Y = -(floor*1800)`.
* `0x800B25C0` = `menu_tab_index` — eine 1 dort oeffnet das Riddle-Menue.
* `+1`-Kodierung im Stash (0 = „nichts gespeichert", 1 = Messer, 2 = Handgun).

**(c) Verworfene Ansaetze** (`save-system.md:36-52, 67-75, 117-148, 198-202`):

| Ansatz | Warum gescheitert |
|---|---|
| RDT-Byte **einfuegen** @`0x16D4` (v1) | Dateigroesse mod 4 ≠ 0 → Freeze |
| RDT `0x16B6` **ueberschreiben** (v2) | `sub02` ist **kein** toter Raum, sondern aktiver Code (`Set`/`Aot_reset`/`Work_set`/`Member_set`) → 4 gleichzeitige Bugs |
| AOT Typ 1→3 konvertieren (v2b) | Typ-Mismatch, Message-Mechanismus kaputt |
| Per-Raum-Flags im **Kartenpuffer** halten | `0x80160000` wird vom CD-Raumladen ueberschrieben |
| v85/v86 RAM-Stash | Pending-Flag zu frueh konsumiert, `RELOAD_RFI` lief nie |
| v100 `SKIP_INV_INIT_HOOK` | Inventar leer; v101 (erst rufen, dann Slot restaurieren) war richtig |
| v102d Equip-Nachbau mit `a1=0x80198000` | Raumwechsel-Freeze; v103 korrigiert auf `a1 = *(0x800AC77C)` |
| Z11 „eigener Save-Dialog komplett in `PATCH.BIN`" | durchgeplant (12 States), als **strukturell sauberer** bewertet — **nie gebaut** (4–6 Sessions geschaetzt) |

→ **Loesung war der Dispatch-Level-Hook mit 1-Byte-RDT-Aenderung** (§2.4/2.5).

**(d) Die wichtigste Architektur-Lehre (Agent BB9).** Alle 1453 RE2-Decompilate durchsucht: **JEDE**
Aufrufstelle von `SsSeqPause` / `SsSetMVol` / `SsVabClose` / `InitCARD2` / `StartCARD2` / `_bu_init`
liegt bei RE2 **ausserhalb** des Save-Pfads. Das Kartensubsystem wird **genau einmal beim Boot**
initialisiert (`FUN_8002b48c → FUN_80095378 → InitCARD2 → _bu_init`) und bleibt live. **RE2s Save ist
ein reiner Dialog-Overlay ueber der weiterlaufenden Hauptschleife:** BGM laeuft weiter, kein VAB-Slot
wird geschlossen, kein Raum neu geladen, kein Inventar restauriert — **weil nie etwas abgebaut wurde.**
FSM-Zustand = **ein** Byte (`DAT_800d4235`), geteilt mit Tuerschloss-/Fingerabdruck-Prompts; Eingabe
flankengetrieben (`DAT_800ce310`); Karten-I/O laeuft auf dem **Karten-Bus**, kann also nie mit CD-Reads
kollidieren.

Demgegenueber hat das Vorprojekt einen **synthetischen Teardown/Restore-Layer** gebaut
(`saved_cursor`/`saved_phase`/`saved_equipped`/`mode_flag`/`HP_STASH`/`SAVE_AUDIOSAFE_SNAPSHOT` …) —
und **genau daraus stammen alle Save-Zeit-Bugs**. Wortlaut:
> „RE1.5 port must STOP doing teardown work — the patches' job is the trigger pin only."

**Unabhaengige Stuetze [BELEGT]:** das Vorprojekt ist dieser Lehre am Ende selbst gefolgt — im
**ausgelieferten** Image sind **beide** Teardown-Hooks auf reine Pass-Throughs zurueckgebaut:
`CD_FIX` @`0x80070720` = `j 0x80043eac ; nop`, `CARD_CLEANUP_CDFIX` @`0x80070818` = `j 0x80026594 ; nop`.

### 4.4 Der entschiedene Doku-Widerspruch: `0x800B5334..43` **[BELEGT]**

`reAi/memory/save-system.md:54-65` nennt die 16 Bytes „CD-Kanal-Status-Array" und das Nullen einen Fix
gegen den Save→Tuer-Freeze; spaetere Audits (U12/R3/S3 via `bug_re2_port_agent_Z8.md`) nennen sie
„libsnd-VAB-Zustandsarray" und das Nullen die **Ursache** fuer tote SFX. **Entschieden:**
Ein Scan aller Load/Store mit Immediate `0x5334..0x5343` liefert in ORIGINAL **und** Image **identische
17 Zugriffe** — der Patch fuegt **null** hinzu. Alle 9 Stores liegen in den libsnd-Containern
`FUN_800577D0` / `FUN_8005BFE0` (`SsVabOpenHead`) / `FUN_8005CE94` (`SsVabClose`) / `FUN_80060C34`.
→ **libsnd `vab_used[16]`, nicht CD-Kanal.** Das Patch-Skript widerruft die alte Doku selbst
(`patch_save_final.py:352-360`: „CRITICAL CORRECTION … NOT a CD channel status array as the old docs
claimed"). **Die alte `save-system.md`-Aussage nicht uebernehmen.**

### 4.5 Welche Version tatsaechlich ausgeliefert wurde **[BELEGT]**

Der Skript-Banner sagt „v1.18" (`patch_save_final.py:3`) — **das Image ist neuer.** `CD_FIX`
Pass-Through = v1.19 (`patch_save_final.py:316`), `CARD_CLEANUP_CDFIX` Pass-Through =
**v1.24-att2, committed `c8cfc187`, 2026-05-10** (`:352-353`). Beide Bytes im Image selbst
disassembliert. Wer Versionsangaben aus dem Skript zitiert, zitiert falsch.

### 4.6 Zwei Fehler im wichtigsten Vorprojekt-Dokument **[BELEGT]**

`reAi/memory/re2_save_system_re_2026_05_29.md`:
* Zeile 53/176: „icon meta | 0x13 | **icon gfx** from icon_table `0x80073628`" — **FALSCH.**
  `0x80073628` ist die **Shift-JIS-Ortsnamen-Tabelle** (§3.2), und das memcpy-Ziel ist **Titel+0x2A**.
* Zeile 176: „Leon `0x800107F8` / **Claire** `0x800107CC`" — **FALSCH.** RE1.5 hat **Leon/Elza**;
  Zeile 275 desselben Dokuments sagt es selbst richtig (interner Widerspruch).
* Zusatz-Fehler, den niemand markiert hat: Zeile 54 „checksum byte written at header +0x27 by
  `FUN_80026E4C`" — `0x80026E4C` ist der `return 0`-Stub; das `+0x27`-Byte schreibt `0x80026F00`
  in `FUN_80026E54` (es ist die **Einer-Ziffer** des Save-Zaehlers, keine Checksumme).

---

## 5. Frage 5 — Was das fuer `reAi_v2` heisst

### 5.1 „byte-true" ist hier kein sinnvoller Massstab — was stattdessen?

Das 100 %-Mandat lautet: jedes Feature byte-verifiziert. Fuer den Speicher-Screen laeuft das ins Leere,
weil **es kein Original-Verhalten gibt, gegen das man messen koennte** — der SAVE-Pfad ist im
Auslieferungsstand nie ausgefuehrt worden (L1–L4). „byte-true" ist hier keine Eigenschaft, die man
erreichen oder verfehlen kann; die Frage ist falsch gestellt.

**Der Massstab, der stattdessen gilt — vier Stufen, jede einzeln nachweisbar:**

| Stufe | Massstab | Pruefbar durch |
|---|---|---|
| **S1 — byte-true (hart)** | Alles, was im ORIGINAL **ausgefuehrten** Code steht, MUSS byte-true sein: Titel-Komposition `FUN_80026e54`, Dateiname `BISLPS-00222x`, Karten-Format, Slot-Semantik (`5` = leer), 5 Slots, Retry-Politik, Zaehler-**Post**-Inkrement, Namenstabellen. | Disasm-Zitat wie bisher. |
| **S2 — dormant-true** | Fuer Code, der im Original **existiert, aber nie laeuft** (Karten-FSM, UI-Renderer): der Port folgt der Struktur des Originals **einschliesslich seiner Konstanten**, aber „byte-true" ist eine Struktur-, keine Verhaltensaussage. Divergenzen sind erlaubt, **wenn** sie dokumentiert und begruendet sind. | Disasm-Zitat + explizite Divergenz-Notiz im Code. |
| **S3 — belegte Wahl** | Wo das Original eine **Luecke** hat (Trigger, I/O-Groesse, Ortsindex), waehlt der Port bewusst — und die Wahl zitiert **vorhandene Daten** (`sysmes 0x1A..0x21`, die 16 Save-Raum-RDTs, `re15_savepoint.c`), nicht Intuition. | Datenzitat + Kennzeichnung als PORT-ENTSCHEIDUNG. |
| **S4 — verboten** | Erfundene Konstanten ohne Datenbeleg, und „weil der Mod es so macht". Der Mod ist **kein** Referenzimplementierung — er hat den Zaehler falsch (§2.7), den Ortsindex semantisch falsch (§3.7) und Sentinel-Kollisionen (§2.4). | — |

**Die entscheidende Regel:** *Das gepatchte Image ist eine QUELLE FUER MECHANISMEN, nie eine Quelle
fuer Werte.* Es zeigt, **wo** die Engine Haken hat und **wie** man sie sauber bedient — es entscheidet
**nicht**, welchen Wert der Port schreibt.

### 5.2 Was der Port bereits richtig macht — nicht anfassen **[BELEGT]**

* **Save-Zaehler:** `main.c:2808` schreibt `s_save_counter`, `main.c:2825` inkrementiert nur bei Erfolg
  → **Post-Inkrement, byte-true zum ORIGINAL** (@`0x80026488`–`9c`). Der Mod macht Pre-Inkrement
  (§2.7). Die echte Mod-Karte (`/01/`, `/02/`) ist **kein** Zielbild.
* **Kein Teardown:** der Port implementiert Save als Screen ueber der laufenden Schleife — das ist
  genau die BB9-Lehre (§4.3d).
* **Ortsname konstant:** `main.c:519-539` verdrahtet „Irons' Office" als Atlas-Codes mit ausfuehrlicher
  Begruendung. Das ist die byte-true Wiedergabe des `return 0`-Stubs.
* **Kartentitel:** `pc_do_save` baut `"BIO HAZARD <Leon|Elza> /NN/"` nach den Templates
  `0x800107cc`/`0x800107f8`.

### 5.3 Ortsnamen im Port — die drei Optionen

| | **A — Status quo** | **B — Traeger-Byte + Registry-Index (empfohlen)** | **C — Mod 1:1 kopieren** |
|---|---|---|---|
| Verhalten | Immer „Irons' Office" | Pro Save-Punkt der passende der 8 vorhandenen Namen | Zwei Namen, Index 1 semantisch falsch |
| Massstab | **S1** (byte-true zum Stub) | **S3** (belegte Wahl) | **S4** (Wert vom Mod uebernommen) |
| Aufwand | 0 | **~2–3 h** | ~2 h |
| Risiko | keins | **sehr niedrig** (s.u.) | mittel (erbt Mod-Fehler) |
| Nutzer-Erwartung | verfehlt sie | trifft sie | trifft sie halb |

**Empfehlung: B**, aber **explizit als PORT-ERWEITERUNG markiert und abschaltbar**, nicht als Fix
verkauft.

#### Warum B mit den vorhandenen Daten belegbar ist

* Die **8 Namen existieren** und sind byte-verifiziert: `sysmes 0x1A..0x21` in `DEBUG.BIN`
  (§3.3, `analysis/bug_save_room_name.md` M1). Nichts wird erfunden.
* Es gibt **genau 8 Speicherpunkt-Orte** (`re15_savepoint.c:17-28`, 16 Eintraege = 8 Orte × Leon/Elza)
  und **genau 8 sysmes-Namen** (`0x1A`–`0x21`, Tabellenende hart belegt). **8 = 8.** Diese Uebereinstimmung
  ist der staerkste vorhandene Beleg dafuer, dass die Tabelle fuer genau diese Raeume gedacht war.
* Der **Mechanismus** ist byte-true aus dem Original ablesbar: `sysmes_id = 0x1A + idx`
  (@`0x80026818`/`0x80026820`) und `SJIS = 0x80073628 + idx*0x13` (@`0x80026f0c`–`0x80026f28`).
  Nur der **Wert von `idx`** ist die Port-Entscheidung.
* Der **Traegerweg** ist byte-true aus dem Mod ablesbar und im Port sogar sauberer: der Index gehoert
  in den Save-Blob, weil **beide** Anzeigen ihn brauchen — die Slot-Liste zum **Anzeige**-Zeitpunkt
  aus dem geladenen Block (§3.3), der Kartentitel zum **Save**-Zeitpunkt aus dem Live-Zustand (§3.2).
  Der `ra`-Trick des Mods ist nur ein Workaround dafuer, dass er den Stub nicht aufteilen konnte;
  **der Port braucht ihn nicht — er braucht ein Feld und zwei Lesestellen.**

#### Konkrete Umsetzung

1. **Traeger.** `re15_savedata_t` hat bereits `uint8_t reserved[2]` (`re15_savedata.h:70`). Eines davon
   wird `loc_idx`. **Kein Version-Bump, keine Checksummen-Layoutaenderung** — `reserved` ist bereits von
   der Checksumme erfasst, und alte Saves haben dort `0` = Index 0 = „Irons' Office" = das heutige
   Verhalten. **Null Migrationsrisiko.**
   *(Das ist genau die Rolle von `game_state[3]` = `0x800B0FBF` im Mod: ein Reservebyte im Blob, §3.4.)*
2. **Registry.** `s_savepoints[]` in `re15_savepoint.c:17-28` um ein drittes Feld `loc_idx` erweitern.
   Vorschlag, jeweils mit Begruendung im Kommentar:

   | Raum (Leon/Elza) | Debug-Name | `loc_idx` | sysmes-Name | Begruendung |
   |---|---|---|---|---|
   | `0x1070`/`0x1071` | LOBBY OFFICE | 0 | Irons' Office | **[OFFEN]** — s.u. |
   | `0x1120`/`0x1121` | 3F WEST SIDE | 2 | West Staircase 1F | Etage + Treppe passen |
   | `0x1150`/`0x1151` | CHIEF OFFICE | 0 | Irons' Office | Irons = Chief |
   | `0x2010`/`0x2011` | BREAK ROOM | 1 | Medical Room | **[OFFEN]** |
   | `0x30A0`/`0x30A1` | NORTH OFFICE | 3 | Sewer Maintainance | **[OFFEN]** |
   | `0x30B0`/`0x30B1` | SOUTH OFFICE | 4 | Sewer Control Room | **[OFFEN]** |
   | `0x4010`/`0x4011` | SECURITY ROOM | 7 | Monitor Room | Monitor ⊂ Security |
   | `0x5010`/`0x5011` | SECURITY ROOM | 6 | Security Room | woertlich identisch |

   **⚠ Ehrlich markiert: nur 3 der 8 Zuordnungen sind aus vorhandenen Daten begruendbar**
   (`1150→0`, `5010→6`, `4010→7`, plus schwaecher `1120→2`). Die uebrigen sind **[OFFEN]** — sie
   waeren geraten. **Deshalb ist die einzige regelkonforme Auslieferung von B eine, die genau diese
   Trennung im Code sichtbar macht:** belegte Zuordnungen gesetzt, unbelegte auf `0` (= heutiges
   Verhalten) mit `/* UNBELEGT — bleibt Index 0 bis ein Beleg existiert */`. Damit ist B **kein**
   Rate-Defekt: es verbessert genau dort, wo ein Beleg existiert, und aendert sonst nichts.
3. **Beide Lesestellen.**
   * Slot-Liste `pc_slot_title_codes` (`main.c:519`): `sysmes 0x1A + sd->loc_idx` statt der
     hartkodierten „Irons' Office"-Codes. Der Port muss die Codes ohnehin aus `DEBUG.BIN` ziehen —
     das behebt nebenbei die dokumentierte Rest-Divergenz (die Zeile ist heute ~28 px zu breit,
     `analysis/bug_save_room_name.md` F2/F4).
   * Kartentitel `pc_do_save`: **NICHT** anfassen. Die SJIS-Tabelle enthaelt RE1-Herrenhausnamen
     (§3.2), die zu keinem RE1.5-Raum passen — ein Ortsname im BIOS-Titel waere hier **schlechter**
     als keiner. Byte-true bleibt hier: Template + Zaehler, Index-Suffix 0.
4. **Schalter.** `RE15_LOC_NAMES=0` (oder eine `#define`-Konstante) stellt das Stub-Verhalten wieder her.
   Damit bleibt die byte-true Referenz jederzeit messbar.
5. **Verifikation.** `re15_port/tests/unit/probe_save_room_name.c` existiert bereits und baut die
   Slot-Zeile byte-true nach `FUN_80026658` nach — sie um einen `loc_idx`-Parameter erweitern und
   fuer alle 8 Indizes gegen `DEBUG.BIN` pruefen.

**Aufwand:** ~2–3 h (1 Feld, 1 Registry-Spalte, 1 Renderer-Aenderung, 1 Probe-Erweiterung).
**Risiken:** (a) alte Saves — **entschaerft** durch `reserved`-Byte = 0; (b) Zeilenbreite in der
Slot-Liste aendert sich mit der Namenslaenge („West Staircase 1F" ist deutlich laenger als „Irons'
Office") → die Breitentabelle @`0x800c4416` muss korrekt angewandt werden, sonst laeuft die Zeile ueber;
(c) die 5 unbelegten Zuordnungen — **entschaerft** dadurch, dass sie auf 0 bleiben.

### 5.4 Was der Port aus dem Original noch uebernehmen kann **[BELEGT]**

Falls der Speicher-Screen weiter ausgebaut wird, ist all das byte-true belegbar:
* Dateiname `"BISLPS-00222"` + Ziffer 0..4 (Template @`0x80010724`, Ziffer-Addition @`0x8002614c`–`58`),
  Pfad `"bu00:"` @`0x80010824`.
* Titel-Komposition: 0x2A-B-Template (Auswahl ueber `game_state[2] & 1`), Ziffern `+0x25`/`+0x27` via
  `/10` und `%10` auf `0x4F`, 0x13 B Ortsname ab `+0x2A`.
* 5 Slots, Slot-Status `5` = leer (`FUN_80026c34`), Slot-Ziffer = `Dateiname[12] - 0x30`.
* Retry-Politik: **30× open / 30× write / 3× Verify-Read à 0x200**
  (@`0x80027274`/`0x800272ac`/`0x80027334`, `sltiu v0,s4,3` @`0x8002732c`).
* Auto-Format bei neuer Karte (`FUN_800275f0` → `FUN_80027724`).
* Selektiver Restore: **10 Bytes Kopf + Rest ueber Raum-Reload** (§4.3a) — nie ein flacher Block-Copy.

### 5.5 Was der Port NICHT tun soll

1. **Keinen Teardown-Layer** bauen (§4.3d). Save = Overlay ueber der laufenden Schleife.
2. **Keine Mod-Werte** uebernehmen: nicht das Pre-Inkrement (§2.7), nicht `loc_idx = 1` fuer ROOM1070
   (§3.7), nicht den Sentinel `0xFD` (kollidiert mit ROOM2070/2071, §2.4).
3. **Nicht den `ra`-Trick nachbauen.** Er ist ein Artefakt der In-Place-Patcherei; der Port hat
   den Blob und den Live-Zustand ohnehin getrennt zur Hand.
4. **Nicht die SJIS-Tabelle** fuer den BIOS-Titel bespielen (RE1-Leftover-Namen, §3.2).
5. **`0x800B5334..43` nicht nullen** — das ist libsnd `vab_used[16]` (§4.4).

---

## 6. Belegstatus-Matrix

| Aussage | Status |
|---|---|
| Provenienz der drei Binaries, Diff-Umfang, ISO-Inhalt | **BELEGT** |
| Karten-Layer vollstaendig, 14-Zustands-FSM @`0x80010738`, `modus 0 = SPEICHERN` | **BELEGT** |
| LOAD-Menuepunkt in `TITLE.BIN` erreichbar | **BELEGT** |
| L1 Gate-Bit nie gesetzt (Instruktions-Ebene) | **BELEGT** |
| L1 auch datengetrieben nie gesetzt (SCD `Flg_set`) | **WAHRSCHEINLICH** |
| L2 Puffer `0x80200000`, L3 I/O `0x800` vs `0x1430`, L4 Flavor-Text 16 Raeume, L5 Stub | **BELEGT** |
| Vollstaendige Patch-Tabellen A/B, RDT-Diff 3 Dateien / 11 Bytes | **BELEGT** |
| Ablauf SAVE / LOAD inkl. `CAMERA_HOOK`-Weiche | **BELEGT** |
| Kartenlayout (alle Offsets an der echten `.mcd` verifiziert) | **BELEGT** |
| Zaehler: Original Post-, Mod Pre-Inkrement | **BELEGT** |
| Ortsname: Stub → `SAVE_LOC_FUNC`, `ra`-Weiche, `game_state[3]`, 2 Konstanten | **BELEGT** |
| Beide Namenstabellen (7 SJIS / 8 sysmes) + Tabellenenden | **BELEGT** |
| End-to-End-Beweis auf der echten Memory Card | **BELEGT** |
| Pfad A (In-Game-Slot-Liste) ist im Mod LEBENDIG | **BELEGT** |
| Sentinel-Kollision `0xFD` in ROOM2070/2071 | **BELEGT** |
| `0x800B5334..43` = libsnd `vab_used[16]`, nicht CD-Kanal | **BELEGT** |
| Ausgeliefertes Image ist v1.24-att2, nicht v1.18 | **BELEGT** |
| Skript-Reproduzierbarkeit (byte-identische Ausgabe) | **WAHRSCHEINLICH** |
| GSB-Gefahrenzone: Feld-Semantik `+0x14`/`+0x20`/`+0x24`/`+0x28` | **WAHRSCHEINLICH** |
| Waffen-Feldsemantik `0x800B25C8`/`0x800B25C0` | **WAHRSCHEINLICH** |
| `entity+0x82` = Floor Number (`Y = -(floor*1800)`) | **BELEGT** |
| Verworfene Ansaetze, Audio-Bug-Historie, BB9-Lehre | **WAHRSCHEINLICH** |
| Debug-Menue-Raumnamen (`DEBUG.BIN` @`0x2642`) | **WAHRSCHEINLICH** |
| Vorgeschlagene `loc_idx`-Zuordnung fuer 1150/4010/5010 (+1120) | **WAHRSCHEINLICH** |
| Vorgeschlagene `loc_idx`-Zuordnung fuer 1070/2010/30A0/30B0 | **OFFEN — nicht setzen** |

---

## 7. Korrekturen: sechs Lane-Befunde, die widerlegt wurden

Diese Punkte sind wichtig, weil sie in Zwischenberichten als „belegt" auftraten. Alle sechs habe ich
selbst nachgemessen.

**K1 — „Pfad A (In-Game-Slot-Liste) ist im gepatchten Build faktisch tot."** **FALSCH.**
`FUN_80026ca8` liest aus jeder Save-Datei `0x80` B ab **Seek `0x200`** und legt sie nach
`cardbuf+0x1430+i*0x80` (@`0x80026d3c`–`58`). Das ist **unabhaengig** vom `[M]`-memcpy. Pfad A liefert
korrekt `game_state[3]`. Der Gegenbeweis der Lane verwechselte einen **RAM**-Puffer-Offset mit einem
**Datei**-Offset. **→ Der Mod zeigt den Ortsnamen an BEIDEN Stellen.**

**K2 — „Der `[M]`-Patch `+0x1430 → +0x200` ist der Transport-Fix."** **FALSCH.**
`FUN_800271a8` schreibt aus **RAM `0x800B0DBC`** (@`0x800272b0`–`b8`), nicht aus dem Kartenpuffer;
Blockanzahl `sb v1,3(s5)` @`0x80027234` und Titel `s5+4` @`0x8002724c` beweisen, dass `s5` der
Blockkopf ist. `game_state[3]` liegt bei Datei `+0x203`, weil `0x800B0FBF = 0x800B0DBC + 0x203` **im
RAM** — auch im ORIGINAL. Der echte Auslieferungs-Defekt ist die **Laenge** `0x800` (L3).

**K3 — „`0x00040000` wird nur an einer Stelle getestet."** **FALSCH:** zwei Stellen,
`0x8001ca00` (Modus 1) und `0x8001cb78` (Modus 2).

**K4 — Modus-Sprungtabelle `@0x8001069c`.** Eine Lane nennt Werte, die **alle um +0x40 daneben** liegen.
Korrekt: `0x8001c9c8 / 0x8001ca98 / 0x8001cbb8 / 0x8001cc34 / 0x8001cc70`.

**K5 — „Nur 4 RDT-Bytes geaendert."** **FALSCH: 11 Bytes in 3 Dateien** (§2.4) — ROOM1150 hat
ebenfalls den Item-Box-Sentinel, und beide Raeume haben je zwei genoppte Cut-Opcodes. Zusaetzlich ist
`DATA/TITLEU.TIM` geaendert (kosmetisch).

**K6 — Drei Aussagen zu „totem Code" und „leeren Caves".**
* `DEPOSIT_HOOK` @`0x80071920` ist **nicht tot**: `PATCH.BIN` @`0x801f02e4` = `j 0x80071920`
  (selbst gefunden). Der Xref-Scan der Lane deckte nur die EXE ab, nicht das bei `0x801F0000`
  nachgeladene Overlay. (`RELOAD_RFI` und `FUN_80026f48` sind ueber EXE **und** `PATCH.BIN` wirklich
  referenzlos.)
* „Alle Caves waren `0x00`-gefuellt" ist **falsch** — drei Funktionen ueberschreiben echte
  Original-Daten, u.a. den ASCII-Record `"SHIKICHI  A   "` (§2.3).
* `CAMERA_HOOK`-Verzweigung war **invertiert** beschrieben (§2.8).

**K7 (Nebenbefund) — „`DAT_800b25bc` nimmt nur 0..3 an."** **FALSCH** (§1.3-L4). Der Blocker haelt
auf anderer Grundlage.

---

## 8. Offene Punkte

1. **[OFFEN — hoechste Prioritaet fuer den Port]** Nutzt irgendein ausgeliefertes RDT den **nativen**
   Save-Trigger **AOT-sce-Typ 2** (`FUN_800430bc`, §2.5)? Ein Zensus ueber alle 240 RDTs steht aus.
   Falls ja, waere das der byte-true Save-Einstieg und der ganze Hook-Aufwand des Vorprojekts unnoetig
   gewesen.
2. **[OFFEN]** Der `Flg_set(bank 0, bit 13)`-SCD-Walk (L1) wurde von mir nicht wiederholt; 34 der 240
   RDTs scheiterten dort am Header-Parse.
3. **[OFFEN]** Semantik von `game_state[0]` (`0x800B0FBC`, in beiden echten Saves `0x0A`). Im Mod
   setzt es der Item-Box-Hook @`0x80070900`; der Leser @`0x8001df54` ist bekannt, die Bedeutung nicht.
   *(Der Port deutet es als `max_inventory_slots = 10` — plausibel, unbelegt.)*
4. **[OFFEN]** Die zweite Namensquelle: der Rueckgabewert von `FUN_80026e4c` geht an Aufrufstelle A als
   `0x1A + idx` an das Overlay `jal 0x800c00e4`. Dass `0x1A`–`0x21` genau die 8 Save-Raeume meinen, ist
   die **plausibelste**, aber nicht bewiesene Lesart — ein Beleg (z.B. eine Raum→sysmes-Zuordnung
   irgendwo im Binary) fehlt. Genau deshalb bleiben 5 der 8 `loc_idx`-Werte in §5.3 auf 0.
5. **[OFFEN]** `PATCH.BIN` (3282 Nichtnull-Bytes, ~30 Routinen ab `0x801F0000`) ist nur an den drei
   save-relevanten Einsprungpunkten `0x801F08B4`, `0x801F08E4`, `0x801F0E00` zerlegt. Der Rest ist
   Item-Box/Text und koennte weitere `game_state`-Bytes belegen.
6. **[OFFEN]** Warum `t_size` um genau `0x2000` waechst: Verdacht „BSS-artigen Bereich beim Boot nullen
   lassen". Nebenwirkung, die niemand geprueft hat: **die EXE reicht jetzt bis `0x800C1000`, also nullt
   der Loader beim Boot auch `0x800C0000`** — dort liegt zur Laufzeit `DEBUG.BIN` (die sysmes-Quelle!).
   Da `DEBUG.BIN` erst **nach** dem EXE-Load von CD kommt, ist es vermutlich unschaedlich —
   **verifiziert ist das nicht.**
7. **[OFFEN]** Ob `0x800B0FE2` (das `PATCH_LOADER`-Gate) in **jedem** Nicht-Titel-Zustand ungleich null
   ist, also ob `PATCH.BIN` garantiert vor der ersten Item-Box-Nutzung geladen ist.
8. **[OFFEN]** Das Audio-Problem nach dem Speichern wurde im Vorprojekt nie abschliessend geloest
   (§4.2). Ob der Nutzer den letzten Stand je getestet hat, geht aus den Dokumenten nicht hervor —
   und der Stand ist im ausgelieferten Image ohnehin nicht enthalten.

---

## 9. Reproduktions-Anhang

```bash
# Gepatchte EXE aus dem Image holen (Rohsektor 2352, Nutzdaten +24, Startsektor 73020)
#   -> 727040 B, md5 17d19f4896de70a01381e11f53e05490
# ODER: die bereits entpackte, bit-identische Kopie verwenden:
#   c:/workspace/git/reAi/info/Re1.5_PATCHED/re15/PSX.EXE

# Offsetformel beider EXEs:
#   Datei-Offset = RAM-Adresse - 0x80010000 + 0x800

# Disassemblieren:
python c:/workspace/git/reAi_v2/.claude/skills/re15-psx-disasm/scripts/re15_disasm.py \
       dis <exe> 0x80026e4c 4
```

Arbeitsartefakte dieser Sitzung (eigener MIPS-Decoder, ISO-Walker, De-Sektorisierung, Scans):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/55862acd-3b0c-44af-b898-3db9f551f311/scratchpad/dossier/`

**Verwandte Dossiers:** `analysis/bug_save_room_name.md`, `analysis/bug_save_room_name_recheck.md`,
`analysis/save_counter.md`, `analysis/save_injured_state.md`.
**Memory-Notizen, die praezisiert werden sollten:** `reai-v2-save-room-name-stub` (die Namens-TABELLEN
existieren — sogar zwei; nur der Resolver fehlt), `reai-v2-save-load`.
