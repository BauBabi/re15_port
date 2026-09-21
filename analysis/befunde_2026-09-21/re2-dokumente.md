# RE2-Dokumente vollständig extrahiert — Hintergründe, Texte, Modelle

**Auftrag.** Alle Dokumenten-Assets von Resident Evil 2 (Retail, Leon) unverändert
extrahieren, damit auswählbar ist, welches als Vorlage für die RE1.5-Dokumente dient.

**Ergebnis in einem Satz.** RE2 hat **genau 25 Dokumente** mit **191 Seitenbildern** und
**25 Hintergrundbildern**; alle 216 sind als Roh-TIM und als PNG extrahiert, die
Dokument-**Namen** liegen als echte Zeichenkette in der EXE, der Dokument-**Text** dagegen
ist auf der PSX kein Zeichenstrom sondern in die Bilder gerastert, und ein **3D-Modell je
Dokument existiert nicht** — es gibt nur einen Anzeige-Körper für den FILE-Bildschirm.

**Auswahlbogen:** `extracted_re2_dokumente/uebersicht.html` — im Browser öffnen
(Doppelklick genügt, keine Serverfreigabe nötig; alle 457 Verweise sind relativ und
wurden geprüft, 0 tote).

---

## 1. Wie die Daten gefunden wurden

Alle Adressen in RAM-Schreibweise. `info/re2leon/PSX.EXE` ist ein PS-X-EXE mit
`t_addr = 0x80010000` (u32 @Datei-0x18), also

```
Datei-Offset = RAM-Adresse - 0x8000F800      (Datei 0x800 <-> RAM 0x80010000)
```

Gegenprobe der Formel gegen Ghidras eigene Beschriftung: Datei-0x104C → 0x8001084C, und
`ghidra_re2_Leon.txt:73278` benennt genau dort `s_DOOR_SOUND_8001084c`. Datei-0x13CC →
0x80010BCC, dort `s_DOOR_LOCK_SERVICE]--------------_80010bcc`. Die Formel stimmt.

### 1.1 Der Container: `COMMON/DATA/FILES.TIM`, 5 257 216 Bytes

Eine Verkettung von PSX-TIMs. Der Einstieg war der Dateiname selbst plus die 216 bereits
im Datensatz liegenden `FILES_*.bmp` — genau so viele TIMs enthält der Container.

### 1.2 Die drei Tabellen in der EXE

Die Aufteilung ist **nicht geraten**, sie steht in drei Tabellen, die der Lader liest:

| Tabelle | Adresse | Format | gelesen bei |
|---|---|---|---|
| Dokument → erster Slot | `0x800A9AD0` | 25 × u8 | `lbu a0,-25904(at)` **@0x8006D480** |
| Dokument-Record | `0x800AA144` | 25 × 4 B `{u16 max_page; u8 y_off; u8 pad}` | `lhu s4,-24252(at)` **@0x80076224** (max_page), `lhu s3,-24250(at)` **@0x8007603C** (y_off) |
| Slot-Record | `0x800A94B4` | 191 × 8 B `{u32 größe; u16 sektor_lo; u8 sektor_hi; u8 pad}` | `lw v0,-27468(at)` **@0x8006D4D8** (größe), `lhu a0,-27464(at)` **@0x8006D518** / `lbu v0,-27462(at)` **@0x8006D50C** (Sektor) |

* Byte-Offset in FILES.TIM = `sektor * 0x800`.
* Seitenhöhe `H = 256 - y_off`: `addiu s1,zero,256` **@0x80076040**, `subu s5,s1,s3`
  **@0x80076044**. Gemessen über alle 25 Dokumente ist H immer eines von 112/128/144/176.
* Seite → Slot: Titelseite = `first[doc]`, Folgeseite p = `first[doc] + 1 + p`
  (`beq v1,zero` **@0x8006D484**, `addiu v0,a0,1` **@0x8006D488**).
* CD-Datei-Id von FILES.TIM = **166** (`sh` → `0x800D531C` **@0x8006D4B4**,
  `jal 0x80012FB8` **@0x8006D530**).

Diese drei Tabellen waren im Repo schon reverse-engineert
(`re15_port/tools/re2_files_cut.py`, `gen_re2_files_toc.py`) — ich habe sie nachgerechnet
statt sie zu übernehmen: die Tabelle @0x800A9AD0 endet nach dem 25. Eintrag mit `0x00`
(im Extraktor als `assert`), und die Summe der Slots je Dokument ergibt exakt 191.

### 1.3 Dokument-Index = Item-Id − 0x68 — das war die fehlende Brücke

Die Dokument-Tabellen sind über einen Index 0…24 adressiert, der Name eines Dokuments
hängt aber an einer **Item-Id**. Die Verbindung steht im Aufnahme-Pfad:

```
80071D00   jal    FUN_800692DC
80071D04   _addiu a0,a3,-0x68          <- Verzögerungsslot: doc = item_id - 0x68
80071D10   srl    v1,v0,0x3            <- Rückgabe = Listenplatz, /8 = Zeile
80071D14   andi   v0,v0,0x7            <-                      &7 = Spalte
80071D1C   sb     v0,offset DAT_800d5c02(at)
```

`FUN_800692DC` hängt den Wert an die FILE-Liste des Spielers `DAT_800D4B68` an
(`lbu v0,0x0(v1)` **@0x800692EC**, `sb a0,0x0(v1)` **@0x80069300**, max 24 Einträge). Der
FILE-Bildschirm liest den Dokument-Record damit zurück (`FUN_800724B4`,
`ghidra_source_re2_Leon.txt:36241`):

```c
uVar1 = *(ushort *)(&DAT_800aa144
        + (uint)(byte)(&DAT_800d4b68)[DAT_800d5c01 * 8 + (int)DAT_800d5c02] * 4);
```

→ Item-Ids **0x68…0x80** sind die 25 Dokumente. Gegenprobe: das sind genau 25 Ids, und
die Namen dort sind genau Dokumenttitel (siehe 1.4), während 0x64…0x67 „no item" heißen.

### 1.4 Die Namen — echte Zeichenketten

`FUN_80030B9C(item_id)` liefert den Namenszeiger. Zwei Bänke:

```
80030C24   lhu    v1,-0x1454(at)   => u16[0x8009EBAC + id*2]
80030C2C   addiu  v0,v0,-0x1AB0    => Basis 0x8009E550        (lateinische Bank)
80030C74   lhu    v1,-0x1BC8(at)   => u16[0x8009E438 + id*2]
80030C7C   addiu  v0,v0,-0x20C4    => Basis 0x8009DF3C        (zweite Bank)
```

Umschalter `(DAT_800CFB74 & 0x840) == 0x40` @0x80030BC8-D0. Kodierung: `0x00` =
Leerzeichen, `0x0C…0x5F` = `chr(byte + 0x24)`, `0xF7` = Ende. Beide Bänke haben **129**
Einträge (Abstand zur jeweils folgenden Ersatztabelle `0x8009E53A` bzw. `0x8009ECAE` ist
in beiden Fällen 0x102 = 129 × 2 — ein struktureller Quercheck). Die lateinische Bank
dekodiert durchgehend zu lesbarem Englisch („Knife", „Hand Gun", „Magnum", „Shotgun",
„Ink Ribbon" …), die zweite benutzt Bytes 0xA0…0xEF und damit einen anderen Zeichensatz;
sie wird nur als Hex mitgegeben, **nicht** interpretiert.

Die Ersatztabelle `0x8009ECAE` (6 Einträge) ist die Umbenennung der vier Precinct-Keys in
`Spade Key / Diamond Key / Desk Key / Heart Key / Club Key` — für Dokumente ohne Belang,
hier nur zur Vollständigkeit notiert.

### 1.5 ⛔ Der Dokument-TEXT ist ein Bild, kein Zeichenstrom

Das ist der wichtigste Befund für die Übernahme nach RE1.5.

* Gemessen: `FILE00_p01_page.TIM` ist ein 4bpp-TIM 256×144 mit 16-Farben-CLUT. Eintrag 0
  ist `0x0000`, Eintrag 9 ist `0x7FFF` (weiß), Einträge 1–6 ein Graublau-Verlauf
  (Rohbytes ab TIM-Offset 0x14: `7b67 395f b54e 3142 ce35 4a29 0080 6310 ff7f 0080 …`).
* Auf der PSX ist ein Texel `0x0000` vollständig durchsichtig (psx-spx, „Texture Color
  0000h"). Es sind also **weiße Glyphen auf durchsichtigem Grund** — von 36 864 Pixeln
  der Seite sind nur 8 071 opak.
* Gerendert zeigt dieses Bild wörtlich „August 8th / I talked to the chief today once
  again, but he refused to listen to me. …" — der Text steckt in den Pixeln.

Als echte Zeichenkette existiert daher **nur der Name**. Die Texte in
`texte/transkription/` habe ich von Hand aus den Seitenbildern abgelesen; sie sind
ausdrücklich **keine Originalbytes** und stehen im Bogen hinter einem Aufklapper, der das
auch so benennt. Das Original bleibt immer das Bild.

### 1.6 ⛔ Der Hintergrund unter der Textseite ist das 8bpp-Bild aus demselben Slot

Das war zunächst offen und ist jetzt belegt. Meine beiden Vermutungen (die türkise Tafel
aus `ST_FILE.TIM`; eine ungetexturierte Farbfläche) waren **beide falsch**.

Der Titel-Slot eines Dokuments enthält **zwei** aneinandergehängte TIMs, und der
Öffnen-Zustand lädt beide in **denselben** VRAM-Ausschnitt:

```
8006CF2C  li   v0,0x917                     ; Slot 0x17, CLUT-Cursor 9  -> CLUT-Y 489
8006CF38  jal  FUN_80076A40 / _lui a0,0x801a ; 0x801A0000 = das 8bpp-Bild
8006CF44  li   v0,0xa17                     ; DERSELBE Slot, Cursor 10  -> CLUT-Y 490
8006CF50  jal  FUN_80076A40
8006CF54  _ori a0=>DAT_801a8220,a0,0x8220   ; +33312 = die 4bpp-Textseite
```

`FUN_80076A40` rechnet die VRAM-Lage aus dem Slot: `sll v0,v1,0x6` @0x80076A6C (x =
slot·64), `sll v0,v0,0x8` @0x80076AA4 (y = 0 oder 256), `addiu v0,v0,0x1e0` @0x80076B08
(CLUT-Y = 480 + Cursor). Slot 0x17 = VRAM (448, 256). Die Textseite überschreibt dort die
oberen H Zeilen des Bildes; es überleben genau die unteren 256−H.

Gezeichnet wird mit **zwei SPRT** (`li v0,0x66` @0x80076050 und @0x800760B0), CLUT (0,490)
für den Text (`li a1,0x1ea` @0x8007604C) und (0,489) für das Bild (`li a1,0x1e9`
@0x800760AC). `FUN_800761B8` reiht sie ein: Textseite an Bildschirm-(25,30) aus
`DAT_800D5C4C/4E` (`lhu` @0x8007623C, `sh v0,0x8(s0)` @0x80076244), Bild an (100,60)
(`li v0,0x64` @0x80076288, `li v0,0x3c` @0x80076290). `AddPrim` hängt vorn an, also wird
**zuerst das Bild, dann darüber der Text** gezeichnet — und weil die Textseite
durchsichtigen Grund hat, scheint das Bild hindurch. Rechnerisch liegt das Bildrechteck
(100,60)–(228,60+256−H) vollständig im Textrechteck (25,30)–(281,30+H).

**Damit ist das 8bpp-Bild der „Dokumenten-Hintergrund" im wörtlichen Sinn.** Ich hatte es
im ersten Durchgang „Anschauungsbild" genannt; das war zu vorsichtig und ist überall
korrigiert.

`ST_FILE.TIM` ist es ausdrücklich **nicht**: die EXE lädt die CD-Ids 221/222
(`ST_FILE.TIM`/`.TM2`) nie. Der Speicherblock mit dem irreführenden Namen
`"FILE TIM TM2"` (String @0x80011C20) lädt Id `0xdf` = 223 = **`ST_FILE.TS`**
(`li a0,0xdf` @0x8006C75C zusammen mit `addiu a3,a3,0x1c20` = genau dieser String) — und
das ist die Grafik der FILE-**Liste** (die 3D-Karteikarten), nicht der Seitenhintergrund.

### 1.7 Nebenprodukt: die CD-Datei-Tabelle `0x800988A4`

Für die Frage „welche Datei lädt der FILE-Bildschirm eigentlich" musste die
Id→Datei-Zuordnung her. Sie steht in `FUN_80012FB8`:

```c
iVar2 = param_1 * 8;
DAT_800d531e = (&DAT_800988ab)[iVar2];                       // +7
DAT_800d5308 = *(int *)(&DAT_800988a4 + iVar2);              // +0  Größe
DAT_800d5314 = (uint)(byte)(&DAT_800988aa)[iVar2] * 0x10000  // +6  LBA hi
             + (uint)*(ushort *)(&DAT_800988a8 + iVar2);     // +4  LBA lo
```

Record = `{u32 size; u16 lba_lo; u8 lba_hi; u8 chk}`. Gegenprobe zum bekannten Anker:
`0x800988A4 + 166·8 = 0x80098DD4`, LBA also `@0x80098DD8/DA` — genau die Adressen, die
`re2_files_cut.py` schon nannte. Byte +7 ist **kein Pad, sondern eine XOR-Prüfsumme** über
die Bytes an 0, 0x200, 0x400 … der auf 2048 gepolsterten Datei (`lw v0,0x0(s0)`
@0x80013878, `addiu s0,s0,0x200` @0x8001387C, `xor v1,v1,v0` @0x8001388C).

Relevante Ids: **166** = `FILES.TIM`, 169 = `ITEMALL.PIX`, 170 = `ITPS.ITP`,
171 = `MAPS.PIX`, 221 = `ST_FILE.TIM`, 222 = `ST_FILE.TM2`, **223 = `ST_FILE.TS`**.
Nur 223 wird geladen — das ist der Beleg dafür, dass `ST_FILE.TIM`/`.TM2` totes Gewicht
auf der Disc sind (Abschnitt 5).

### 1.8 Das Layout je Dokument — durch Zensus belegt

Ein Dokument belegt im VRAM einen **64 VRAM-Worte breiten Block von 256 Zeilen**:

| Zeilen | Inhalt | Breite |
|---|---|---|
| `0 … H-1` | die 4bpp-Textseite | 256 px = 64 Worte |
| `H … 255` | das 8bpp-Hintergrundbild | 128 px = 64 Worte |

Der Leser zeichnet dazu zwei Sprites — Textseite `u=0, v=0, w=256, h=H`
(`sb zero,-2(s0)` **@0x80076068**, `sb zero,-1(s0)` **@0x8007606C**, `sh s1,2(s0)`
**@0x80076070**, `sh s5,4(s0)` **@0x80076078**) und Hintergrundbild bei `v=H`
(`subu v0,zero,s3` **@0x800760B8**, `sb v0,-1(s0)` **@0x800760D0**).

**Gegenprobe an den Daten (Bandzensus, läuft bei jeder Extraktion mit):** oberhalb des
Bandes, also in den Zeilen `0…H-1` des 8bpp-Bildes, steht bei **keinem** der 25 Dokumente
Kunst. 21 Dokumente sind dort vollständig durchsichtig, die vier Dokumente 12/15/16/17
sind mit opakem Schwarz gefüllt — hellster Kanalwert über alle 25 Dokumente **8 von 255**.
Genau diese Zeilen überschreibt die Textseite beim Laden. Der Extraktor bricht ab, wenn
dieser Zensus jemals verletzt wird.

> Ehrlichkeitsvermerk: mein erster Zensus prüfte nur die Alpha-Bounding-Box und meldete
> deshalb „4 von 25 Dokumenten ragen über das Band". Das war ein Messfehler meiner Sonde,
> nicht ein Befund: die vier Bilder haben dort opakes **Schwarz**, keine Kunst. Erst die
> Messung des hellsten Kanalwerts hat das geklärt.

---

## 2. Vollständigkeit: Soll gegen Ist

| Größe | Soll (woher) | Ist | |
|---|---|---|---|
| Dokumente | **25** — Tabelle `0x800A9AD0` endet nach 25 Einträgen mit `0x00` | **25** | ✅ |
| Seitenbild-Slots | **191** — Slot-Tabelle `0x800A94B4`; Summe der Slots je Dokument | **191** | ✅ |
| TIMs im Container | **216** = 191 × 4bpp + 25 × 8bpp | **216** | ✅ |
| Dokument-Namen | **25** — Item-Ids 0x68…0x80 | **25** | ✅ |
| Byte-Abdeckung `ST_FILE.TM2` | 372 Bytes | 372 lückenlos erklärt | ✅ |
| Byte-Abdeckung `ST_FILE.TS` | 21 752 Bytes | 0x4E20 TIM + 0x64C Block A + 0x8C Block B | ✅ |
| Byte-Abdeckung `FILES.TIM` | 5 257 216 Bytes | 4 891 872 in TIMs + 365 344 Nullfüllung | ✅ |

Die 216 deckt sich mit den 216 `FILES_*.bmp`, die im Datensatz schon neben `FILES.TIM`
lagen — eine unabhängige zweite Zählung.

### 2.1 ⛔ Der stärkste Beleg: im Container bleibt nichts übrig

Drei Messungen, alle als `assert` im Extraktor, alle grün:

1. **Jedes der 216 geschnittenen TIMs ist byte-identisch** mit genau der Scheibe von
   `FILES.TIM`, die `toc.csv` für es nennt — 216 geprüft, **0 Abweichungen**.
2. **Kein Byte liegt in einem Slot, aber in keinem TIM.** Die von den 216 TIMs belegten
   4 891 872 Bytes deckungsgleich mit den von der Slot-Tabelle `0x800A94B4` benannten
   4 891 872 Bytes — Differenz **0**.
3. Die **365 344 Bytes (6,9 %)**, die kein Slot benennt, sind **ausnahmslos `0x00`** —
   reine Füllung bis zur nächsten 0x800-Sektorgrenze. Bytes ≠ 0 außerhalb aller Slots:
   **0**.

Damit ist nicht nur „alles extrahiert, was ich gefunden habe", sondern **in `FILES.TIM`
existiert nichts weiter als diese 216 Bilder**.

**Datengetriebene Nebenbefunde** (md5 über alle 216 geschnittenen TIMs):

* **166 von 216** Bildern sind verschieden, 38 Hashes haben Dubletten.
* Bei **allen 25** Dokumenten ist das 4bpp-Bild im Titel-Slot **byte-identisch** mit dem
  der Seite 0. Das Titelbild liegt zweimal vor: einmal für den Titel-Slot, einmal als
  erste Seite.
* **Dokument 5 und 6** („Mail to the chief") sind in **allen 10 Slots** byte-identisch —
  dasselbe Dokument zweimal im Container.
* **5 von 191** Seitenbildern sind vollständig leer (nur Index 0, also nichts Sichtbares):
  `FILE08_p04`, `FILE11_p01`, `FILE12_p01`, `FILE13_p01`, `FILE19_p01`. Bei den vier
  Film-Dokumenten ist das die Seite, auf der allein das Foto zu sehen ist; die drei
  Film-Leerseiten sind untereinander byte-identisch. Auch dieser Zensus läuft bei jeder
  Extraktion mit und wird gemeldet.
* `FILE23_p12` == `FILE24_p10`.
* Von den 25 Hintergrundbildern sind nur **14 verschieden**; acht Dokumente
  (1/5/18/20/21/22/23/24) teilen dasselbe Bild „loser Papierstapel".

**Drei Dokumente haben mehr Bilddaten, als ihr `max_page`-Startwert meldet** —
kein Extraktionsfehler, sondern Absicht des Spiels:

| Dokument | Slots | `max_page` Startwert | Überhang |
|---|---|---|---|
| 9 „Operation report 2" | 8 | 4 | +2 |
| 23 „Rookie files" | 17 | 2 | +13 |
| 24 „Rookie files" | 17 | 2 | +13 |

`max_page` ist ein RAM-Wert und wächst zur Laufzeit; für Record 23 patcht ihn
`sh 1,-24160(at)` **@0x80068368** nach `0x800AA1A0`. Bei Dokument 9 enthält der Überhang
eine **zweite Fassung des Schlusses**: Seiten S03/S04 enden mit „Recorder: Elliot Edward",
S05/S06 mit „Recorder: David Ford" — sichtbar in den Bildern. Ich extrahiere **alle**
Slots, unabhängig von `max_page`, es fehlt also nichts.

---

## 3. Was extrahiert ist

```
extracted_re2_dokumente/
├── uebersicht.html              ← DER AUSWAHLBOGEN
├── dokumente.csv / .json        25 Zeilen: Name, Item-Id, Slots, Seiten, Höhe
├── toc.csv                      216 Zeilen: je TIM Byte-Offset in FILES.TIM, bpp, Maß, CLUT
├── hintergruende/
│   ├── seiten_roh/     191 TIM  4bpp 256×H   — die Textseiten, byte-true geschnitten
│   ├── seiten_png/     191 PNG  RGBA, Index 0 durchsichtig wie im Spiel
│   ├── seiten_lesbar/  191 PNG  dieselben auf neutralem Grund (18,26,32) — Ansichtshilfe
│   ├── bilder_roh/      25 TIM  8bpp 128×256 — das Hintergrundbild je Dokument
│   ├── bilder_png/      25 PNG  volle 128×256-Leinwand
│   └── bilder_band/     25 PNG  auf das gezeichnete Band 128×(256−H) beschnitten
├── texte/
│   ├── FILE00_name.txt … FILE24_name.txt   Name dekodiert + Rohbytes, beide Bänke, mit @0x
│   ├── transkription/FILE00.txt … FILE24.txt   abgelesene Seitentexte (KEINE Originalbytes)
│   └── exe_terminaltexte.txt   Zugabe: 18 Klartext-Strings des Computer-Terminals
└── modelle/
    ├── ST_FILE.TM2 / .TIM / .TS   Rohdateien des FILE-Bildschirms
    ├── TM2.obj                    ST_FILE.TM2 (8 Vertices, 6 Vierecke) — vom Spiel UNGENUTZT
    ├── TS_BlockA.obj              ST_FILE.TS @0x4E20 (36 Vertices, 3 Tri + 29 Quad) — das Regal
    └── TS_BlockB.obj              ST_FILE.TS @0x546C (4 Vertices, 1 Quad) — die Karteikarte
```

Gesamt **8,2 MB** — liegt im Arbeitsbaum und ist mitcommittet.
`info/re2leon/` wurde nur gelesen, nichts dort verändert.

Der Ansichts-Hintergrund (18,26,32) in `seiten_lesbar/` ist die **einzige** Zutat; sie ist
nötig, weil die Originale durchsichtig sind und sonst unsichtbar wären. Byte-true liegen
dieselben Seiten in `seiten_roh/` (TIM) und `seiten_png/` (RGBA mit echtem Alpha). Keine
Retusche, keine Umfärbung, keine Skalierung.

---

## 4. Die 25 Dokumente

| # | Item-Id | Name | Slots | Höhe | Hintergrundbild |
|---|---|---|---|---|---|
| 0 | 0x68 | CHRIS's diary | 7 | 144 | türkises Tagebuch mit Schloss |
| 1 | 0x69 | Mail to Chris | 7 | 176 | loser Papierstapel |
| 2 | 0x6A | Memo to LEON | 3 | 176 | gefalteter Zettel |
| 3 | 0x6B | Police memorandum | 4 | 176 | gelber Notizblock |
| 4 | 0x6C | Operation report 1 | 11 | 176 | gelber Notizblock |
| 5 | 0x6D | Mail to the chief | 11 | 176 | loser Papierstapel |
| 6 | 0x6E | Mail to the chief *(= 5, byte-identisch)* | 11 | 176 | Papierstapel, khaki |
| 7 | 0x6F | Secretary's diary A | 8 | 144 | braunes Tagebuch |
| 8 | 0x70 | Secretary's diary B | 6 | 144 | olivgrünes Tagebuch |
| 9 | 0x71 | Operation report 2 | 8 | 176 | gelber Notizblock |
| 10 | 0x72 | User registration | 3 | 128 | handbeschriebener Zettel |
| 11 | 0x73 | Film A | 4 | 112 | **Foto**: Präparate-Tank, rot |
| 12 | 0x74 | Film B | 5 | 112 | **Foto**: Zombie-Gesicht |
| 13 | 0x75 | Film C | 4 | 112 | **Foto**: Präparate-Kammer, grün |
| 14 | 0x76 | Patrol report | 5 | 144 | gerolltes Papier |
| 15 | 0x77 | Watchman's diary | 10 | 176 | aufgeschlagenes Notizbuch |
| 16 | 0x78 | Chief's diary | 8 | 176 | aufgeschlagenes Tagebuch mit Stift |
| 17 | 0x79 | Sewer manager diary | 11 | 176 | aufgeschlagenes Notizbuch |
| 18 | 0x7A | Sewer manager fax | 7 | 176 | loser Papierstapel |
| 19 | 0x7B | Film D | 3 | 112 | **Foto**: Rebecca Chambers im RPD-Shirt |
| 20 | 0x7C | Vaccine synthesis | 8 | 176 | loser Papierstapel |
| 21 | 0x7D | Lab security manual | 7 | 176 | loser Papierstapel |
| 22 | 0x7E | P_epsilon report | 6 | 176 | loser Papierstapel |
| 23 | 0x7F | Rookie files *(Szenario A)* | 17 | 176 | loser Papierstapel |
| 24 | 0x80 | Rookie files *(Szenario B)* | 17 | 176 | loser Papierstapel |

Die Titelseite von Dokument 19 zeigt übrigens nicht „Film D", sondern „RECRUIT"; die
Titelseiten von 23/24 lauten „HINT FILES FOR THE ROOKIE MODE".

**Für die Übernahme nach RE1.5 sind gestalterisch die 14 verschiedenen
Hintergrundbilder und die vier Seitenhöhen (112/128/144/176) die eigentliche Auswahl** —
die Textseiten selbst sind ein einheitliches Raster: 256 px breit, weiße Glyphen,
feste Zeilenumbrüche im Bild.

---

## 5. „Modelle" — meine Auslegung, mit Beleg

**Kein Gegenstand in RE2 hat ein 3D-Mesh — Dokumente schon gar nicht.**

* Die Nahansicht eines Gegenstands ist **ein einziges texturiertes Rechteck 112×72**:
  `FUN_80075A00` setzt `li v0,0x70` @0x80075A5C (w = 112) und `li v0,0x48` @0x80075A64
  (h = 72), CLUT (0,488) via `li a1,0x1e8` @0x80075A50, und erzeugt genau zwei Quads
  (Doppelpuffer) — kein Mesh, und nirgends ein Modell-Lookup je Item-Id.
* Das Inventar-Icon ist eine rohe 40×30-Kachel aus `ITEMALL.PIX`, direkt über die Item-Id
  indiziert: `&DAT_8019C000 + item_id * 0x4b0` (0x4B0 = 1200 B = 20 Worte × 30 Zeilen
  @ 8bpp). 127 200 B = 106 × 1200 → 86 Haupt-Kacheln (Ids 0…85) + 20 zweite Hälften
  für die Zweislot-Waffen.
* **Die FILE-Items (Ids 0x68…0x80) sind explizit ausgeschlossen:**
  ```
  80071BBC  sltiu v0,a3,0x68          ; a3 = Item-Id
  80071BC0  beq   v0,zero,LAB_80071D00 ; Id >= 0x68 -> Dokument-Pfad
  80071BC8  li    a0,0xaa             ; nur fuer Id < 0x68: ITPS.ITP laden
  ```
  Dokumente sind in RE2 also gar keine Inventar-Gegenstände; sie gehen direkt in die
  24-Byte-FILE-Liste. Passend dazu die Datenlage: `ITEMALL.PIX` hat 86 Haupt-Kacheln
  (Ids 0…85), `ITPS.ITP` 100 Einträge — für Id 104 aufwärts existiert in keinem von
  beiden ein Eintrag.
* Am Boden liegende Gegenstände tragen nur einen **Effekt-Marker** aus dem *raumeigenen*
  Modellsatz (`FUN_80053394` → `FUN_8001BF10`, Modell-Id 0x0F/0x17/0x1F, y − 0x5A über
  dem AOT-Eintrag) — der bekannte Glitzer, **kein** item-spezifisches Mesh.

Was es an Geometrie überhaupt gibt, ist das **Möbelstück des FILE-Bildschirms**. Ich habe
das Format lückenlos nachgerechnet (`re15_port/tools/re2_tm2_obj.py`) — zuerst an
`ST_FILE.TM2` (372 B):

| Datei-Offset | Inhalt |
|---|---|
| `0x00` | u32 `0x00000114` = Offset des UV-Blocks |
| `0x04` | u32 `0` = flags; 0 heißt: Zeiger relativ zum Ende des 3-Wort-Kopfes, also zu `0x0C` (TMD-Regel) |
| `0x08` | u32 `2` = Anzahl Objekte |
| `0x0C` | Objekt 0, 7 Worte: `{vert=0x38, n_vert=8, norm=0x78, n_norm=6, prim=0xA8, n_prim=0, uv=0x108}` |
| `0x28` | Objekt 1, 7 Worte: dasselbe, aber `n_prim=6` |
| `0x44` | 8 Vertices, je 4× s16 |
| `0x84` | 6 Normalen, je 4× s16 (±4096 = ±1,0 in 1.12-Fixkomma) |
| `0xB4` | 6 Primitive, je 4 Paare u16 `(normal_index, vertex_index)` |
| `0x114` | 6 × 16 B Texturdaten `(u0,v0,clut)(u1,v1,tpage)(u2,v2,-)(u3,v3,-)` |

Die Aufteilung ist durch drei unabhängige Quercheks erzwungen und **lückenlos**:
`0x0C + 2×28 = 0x44` = genau der Beginn der Vertices; `6×4×4 = 96` B Primitive reichen
lückenlos von `0xB4` bis `0x113`; `6×16 = 96` B UV reichen von `0x114` bis `0x173` =
Dateiende. Kein unerklärtes Byte.

Die Vertices sind
`(0,270,-204) (0,-270,-204) (-72,270,-204) (-72,-270,-204) (0,270,204) (0,-270,204)
(-72,270,204) (-72,-270,204)` — ein **Kasten**, 72 dick × 540 hoch × 408 tief.

Zwei Dinge, die ich erst danach belegt habe und die das Bild korrigieren:

1. Das 7. Objektwort ist **nicht** PsyQs `scale`, sondern der Zeiger auf die
   GPU-Paket-Vorlagen. Maßstab ist die EXE selbst: `get_tmd_addr` (die Funktion mit den
   PsyQ-Debug-Strings `"analizing TMD..."` @0x800126E8 und
   `"\tid=%08X, flags=%d, nobj=%d, objid=%d"` @0x800126FC) rechnet
   `piVar2 = param_1 + objid*7 + 3` und liest Vertices/Normalen/Index als
   `base + piVar2[0|2|4] + 0xc`. RE2 hält Index und Paket in zwei **parallelen** Arrays
   statt in einem verschränkten TMD-Record.
2. **Gerade Objektnummer = Dreiecke, ungerade = Vierecke**, festgeschrieben in
   `FUN_80076B60`: `iVar2 = (uVar8 & 1) + 3;`. Genau deshalb hat Objekt 0 `n_prim = 0`
   und Objekt 1 `n_prim = 6` — der Kasten hat null Dreiecke und sechs Vierecke.

**⛔ Und: `ST_FILE.TM2` wird von RE2 nie geladen.** Es ist ein auf der Disc
liegengebliebenes Build-Zwischenprodukt. Die ausgelieferte Geometrie steckt in
**`ST_FILE.TS`** (CD-Id 223 = `0xdf`, `li a0,0xdf` @0x8006C75C), und zwar als
`[TIM 8bpp 128×144, 0x4E20 B][Block A 0x64C][Block B 0x8C]`. Selbst nachgerechnet:

| Block | Vertices | Normalen | Primitive | Paketbytes | Gegenprobe |
|---|---|---|---|---|---|
| `ST_FILE.TM2` | 8 | 6 | 0 Tri + 6 Quad | 96 | endet exakt bei 0x174 = 372 B |
| `ST_FILE.TS` A | 36 | 32 | 3 Tri + 29 Quad | 3·12+29·16 = 500 | = 0x64C − 0x458 ✓ |
| `ST_FILE.TS` B | 4 | 1 | 0 Tri + 1 Quad | 16 | = 0x8C − 0x7C ✓ |

und `0x4E20 + 0x64C + 0x8C = 0x54F8 = 21 752` = Dateilänge von `ST_FILE.TS`, kein
unerklärtes Byte. Die **8 Vertices von `ST_FILE.TM2` sind byte-identisch die ersten 8 der
36 von Block A** (im Konverter per `assert` geprüft) — TM2 ist die ältere, reine
Quader-Fassung desselben Modells. Block A ist das Regal/die drei Reihen der FILE-Liste,
Block B die einzelne Karteikarte, die 24-mal im Karussell steht.

Exportiert liegen alle drei als OBJ in `modelle/` (`TM2.obj`, `TS_BlockA.obj`,
`TS_BlockB.obj`), die Rohdateien daneben.

**Meine Auslegung des Auftrags-Worts „Modelle" — ausdrücklich, damit widersprochen werden
kann:** ich habe (a) die Geometrie des FILE-Bildschirms als OBJ exportiert und (b) die 25
8bpp-Bilder als das mitgegeben, was in RE2 die Rolle eines „Dokument-Modells" tatsächlich
spielt. Ein item-spezifisches Mesh gibt es nicht und kann es nicht geben — siehe die
Belege oben.

---

## 6. Zugabe: die Klartext-Texte des Terminals

Nicht direkt verlangt, aber für RE1.5-Dokumente wahrscheinlich das Nützlichste, weil es
**echter Text mit Auszeichnung** ist: `texte/exe_terminaltexte.txt`, 18 Zeichenketten aus
`PSX.EXE`, Block `0x80010BCC`…, referenziert von `addiu a1,a1,0xbcc` **@0x8003AB98**.
Auszeichnung wie in den Bytes, nichts ersetzt: `]` = Zeilenumbruch, `|<n>` = Farbwechsel
(`|0` = zurück), `[0nn.` = Pause um nn Einheiten und dann ein Punkt. Beispiel:

```
@0x80010C48  ]The doors can be unlocked]by a |2CARD KEY|0.]
@0x80010C7C  ]Checking up ID CARD[025.[025.[025.[025.
```

---

## 7. Was offen blieb

*Die beiden großen offenen Punkte des ersten Durchgangs — der Hintergrund unter der
Textseite und die Frage nach Item-Modellen — sind inzwischen belegt (Abschnitt 1.6 bzw.
5). Es bleibt:*

1. **Der Inhalt der beiden `DR_MODE`-Primitive** `0x800D6BA8` und `0x800D6C08`, die
   `FUN_800761B8` vor den beiden Sprites einreiht (`addiu v0,s3,0x1018` @0x80076274,
   `addiu v0,s3,0xfb8` @0x800762B0), ist **nicht belegt**. Gesucht wurde nach dem
   Schreiber: alle 7 Treffer auf diese Offsets sind `AddPrim`-Aufrufe, und keiner der 15
   `jal SetDrawMode` im ganzen Image trifft sie. Aus der VRAM-Ablage *muss* die Textseite
   tpage 0x17 (4bpp @448,256) und das Bild tpage 0x97 (8bpp @448,256) tragen — am
   Initialisierungscode zeigen lässt es sich nicht. Für die Extraktion ohne Belang.
2. **Dass das sichtbare Boden-Objekt selbst im vorgerenderten Hintergrund steckt**, ist
   ein Schluss aus Abwesenheit, nicht aus Code: belegt ist nur der Effekt-Marker.
   Durchsucht wurden die SCD-Opcode-Tabelle (`0x800A74C8`, 0x76 Einträge), alle
   `FUN_8001BF10`-Aufrufer, die AOT-Handler 0x38/0x4E und alle `DAT_800D4CD8`-Nutzer;
   kein Pfad schlägt pro Item-Id ein Modell nach.
3. **Die zweite Namensbank** (`0x8009DF3C` / `0x8009E438`) ist nur als Hex mitgegeben. Ihr
   Zeichensatz (Bytes 0xA0…0xEF) ist nicht aufgelöst; dafür müsste der zugehörige Font
   identifiziert werden. Für lateinische Dokumentnamen ist sie ohne Belang.
4. **Die Transkriptionen sind abgelesen**, nicht dekodiert — Lesefehler sind möglich.
   Maßgeblich ist immer das Bild daneben.

---

## 8. Werkzeuge (alle im Repo, reproduzierbar)

| Werkzeug | Zweck |
|---|---|
| `re15_port/tools/re2_dokumente_extrakt.py` | die vollständige Extraktion + der Bogen; ein Aufruf, keine Argumente. Enthält alle `@0x`-Belege im Kopf und bricht bei verletztem Zensus ab. |
| `re15_port/tools/re2_dokumente_transkription.py` | schreibt die 25 Transkriptionen (muss vor dem Extraktor laufen, damit der Bogen sie einbettet) |
| `re15_port/tools/re2_tm2_obj.py` | die RE2-TM2-Geometrie → OBJ: `ST_FILE.TM2` **und** die beiden Blöcke in `ST_FILE.TS`, mit dem lückenlosen Format-Nachweis im Kopf und drei Gegenproben als `assert` |

Vorhanden waren schon `re2_files_cut.py` (Schnitt) und `re2_files_png.py` (PNG) — ich habe
sie zur Gegenprobe laufen lassen (gleiche 216 TIMs, gleiche Maße) und ihre Tabellen
nachgerechnet, den Rest aber neu gebaut, weil sie weder Namen, noch Hintergrundbilder als
solche, noch einen Auswahlbogen liefern.

Aufruf:

```bash
python re15_port/tools/re2_dokumente_transkription.py
python re15_port/tools/re2_dokumente_extrakt.py
python re15_port/tools/re2_tm2_obj.py
# dann extracted_re2_dokumente/uebersicht.html im Browser öffnen
```
