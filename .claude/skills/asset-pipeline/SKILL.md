---
name: asset-pipeline
description: Dokumentiert die Extraktions- und Parse-Pipeline pro RE1.5-Asset-Typ (RDT, BSS, EMD, TIM, VAB) — Extraktionsschritte, Java-Parser-Klassen, C-Runtime und Ausgabeformat. Verwenden bei Fragen zur Asset-Extraktion, zu Dateiformaten oder welche Parser-Klasse für einen Asset-Typ zuständig ist.
---

# Asset-Pipeline Skill

Dokumentiert die Extraktions- und Parse-Pipeline für alle RE 1.5 Asset-Typen. Pro Asset-Typ werden die Extraktionsschritte, benötigten Parser-Klassen und das Ausgabeformat als geordnete Liste beschrieben.

## Kontext laden

Beim Aktivieren dieses Skills lade folgende Quellen als Kontext:

1. **RE15_KNOWLEDGE.md §1.1** — RDT Format (Adresstabelle, Header, 21 Sektionen)
2. **RE15_KNOWLEDGE.md §1.2** — SCD Bytecode (Pointer-Tabelle, Opcodes)
3. **RE15_KNOWLEDGE.md §1.3** — BSS Format (MDEC VLC, 64KB-Chunks)
4. **RE15_KNOWLEDGE.md §1.4** — EMD/EDD/EMR/MD1 (Skelett, Mesh, Animation)
5. **RE15_KNOWLEDGE.md §1.5** — PLD/PLW/EMS (Container-Formate)
6. **RE15_KNOWLEDGE.md §1.6** — TIM/PIX/ITP (Textur-Formate)
7. **RE15_KNOWLEDGE.md §1.7** — ESP (Effekt-Sprites)
8. **RE15_KNOWLEDGE.md §1.8** — VAB (VH+VB Audio-Bänke)

Zusätzlich:
- `src/main/java/de/re15/extractors/RE15MasterExtractor.java` — 7-Phasen-Gesamtpipeline
- `re15_port/engine/src/rdt_common.c` — C-Runtime-Parser (In-Place-Parsing)
- `re15_port/engine/src/room_load.c` — Raum-Lade-System (Runtime-Nutzung)
- `re15_port/platform/pc/src/audio_pc.c` — ADPCM-Dekodierung + VAB-Loading (PC)

## Unterstützte Asset-Typen

| Typ | Beschreibung | Quelldateien | Ausgabe |
|-----|-------------|--------------|---------|
| RDT | Room Definition Table | `STAGE{N}/ROOM*.RDT` | Sektions-Dateien (SCA, RID, RVD, SCD, MSG, etc.) |
| BSS | Background Still Screen | `STAGE{N}/ROOM*.BSS` | BMP/PNG pro Kamerawinkel (320×240) |
| EMD | Enemy/Entity Model Data | `EMD/*.EMD`, `EMS/*.EMS` | EDD + EMR + MD1 + TIM, optional glTF |
| TIM | Texture Image | `DATA/*.TIM`, eingebettet in RDT/EMD | BMP/PNG (4/8/16/24-bit) |
| VAB | Virtual Audio Bank | `SOUND/*.VH` + `SOUND/*.VB` | WAV-Dateien pro Sample |

---

## RDT — Room Definition Table

### Extraktionsschritte

1. **RDT-Datei identifizieren**: Pfad `STAGE{N}/ROOM{Stage}{RoomHex}{Player}.RDT`
   - Stage: 1–6 (dezimal), RoomHex: 00–27 (hex, 2-stellig), Player: 0 (Leon) / 1 (Elza)

2. **Header parsen (8 Bytes, Offset 0x00–0x07)**:
   - nSprite, nCut, nOmodel, nItem, nDoor, nRoom_at, Reverb_lv, Reserved

3. **Adresstabelle lesen (21 × u32 LE, Offset 0x08–0x5C)**:
   - Jeder Eintrag = absoluter Offset einer Sektion innerhalb der Datei
   - Offset 0x00000000 = Sektion nicht vorhanden

4. **Sektionen extrahieren** — pro Sektion:
   - Daten ab Offset bis zum nächsten nicht-Null-Offset (oder Dateiende)
   - Sub-Parser je nach Sektionstyp aufrufen

5. **Runtime-Laden (C-Engine)**: In-Place-Parsing, kein Kopieren — Pointer zeigen direkt in den geladenen Puffer

### Parser-Klassen

| Sektion | Java-Parser | C-Runtime |
|---------|-------------|-----------|
| Gesamt-RDT | `RDTExtractor.java:37-120` | `rdt_common.c` → `re15_rdt_parse()` |
| Kollision (SCA) | `CollisionParser.java` | `collision.c` |
| Kamera (RID) | `CameraParser.java`, `RIDParser.java` | `camera_common.c` |
| Zonen (RVD) | `ZoneRvdParser.java` | `aot_common.c` |
| Licht | `LightParser.java` | (direkt aus RDT-Puffer) |
| Boden | `FloorParser.java` | (direkt aus RDT-Puffer) |
| Nachrichten (MSG) | `MSGParser.java` | (direkt aus RDT-Puffer) |
| SCD (Main/Sub) | `SCDScriptDisassembler.java` | `scd_vm.c` |
| Effekte (ESP) | `EffectEspParser.java` | (direkt aus RDT-Puffer) |
| Sprites (ESP-TIM) | `Step5ESPExtractor.java` | (direkt aus RDT-Puffer) |

### Ausgabeformat

- **Extraktion**: Einzeldateien pro Sektion (`.sca`, `.rid`, `.rvd`, `.scd`, `.msg`, `.tim`, `.md1`)
- **Runtime**: `re15_rdt_t` Struct mit 21 Pointern in den residenten Puffer
- **Analyse**: Tabellarische Ausgabe (siehe RDT-Analyse-Skill)

---

## BSS — Background Still Screen

### Extraktionsschritte

1. **BSS-Datei identifizieren**: Pfad `STAGE{N}/ROOM{Stage}{RoomHex}{Player}.BSS`
   - Gleiche Namenskonvention wie RDT, aber Endung `.BSS`

2. **Chunk-Aufteilung**: Datei besteht aus N × 64KB Chunks (N = nCut aus RDT-Header)
   - Chunk für Kamera-ID `c`: Offset `c * 65536` bis `(c+1) * 65536 - 1`

3. **VLC-Dekodierung pro Chunk**:
   - Prüfe VLC-Marker: Bytes 2–3 = `0x3800`
   - VLC (Variable Length Coding) → Quantisierte DCT-Koeffizienten

4. **MDEC-Dekodierung**:
   - Inverse Quantisierung (IQTAB)
   - Inverse DCT (8×8 Blöcke)
   - YCbCr → RGB Farbraumkonvertierung
   - Ausgabe: 320×240 RGB-Bild

5. **Runtime-Laden (C-Engine)**: BSS-Chunk direkt in 64KB-Puffer laden → MDEC/VLC-Hardware (PSX) oder Software-Decoder (PC) → Framebuffer/Textur

### Parser-Klassen

| Schritt | Java-Klasse |
|---------|-------------|
| Chunk-Split + Steuerung | `BssExtractor.java` |
| VLC-Dekodierung | `VlcDecoder.java` |
| MDEC + IDCT + Farbraum | `MdecDecoder.java` |

### Ausgabeformat

- **Extraktion**: Pro Chunk eine `.bmp` / `.png` Datei (320×240, 24-bit RGB)
- **Runtime (PSX)**: MDEC-Hardware dekodiert VLC → YCbCr → VRAM-Transfer via DMA
- **Runtime (PC)**: Software-Decoder → OpenGL-Textur (320×240)

---

## EMD — Enemy/Entity Model Data

### Extraktionsschritte

1. **Quelldateien identifizieren**:
   - Einzelne EMDs: `EMD/EM{XX}.EMD`
   - PLD-Container (Spieler): `PLD/PL{XX}.PLD`, `PLD/PL{XX}.PLW`
   - EMS-Archive (Feinde): `EMS/EM{StageRoom}.EMS`

2. **Container entpacken** (bei PLD/EMS):
   - PLD: Header → EDD-Offset, EMR-Offset, MD1-Offset, TIM-Offset
   - EMS: Mehrere EMD-Einträge, 2048-Byte-Alignment

3. **Komponenten extrahieren** (4 Teile pro Entity):

   | Komponente | Inhalt | Größe |
   |-----------|--------|-------|
   | **EDD** | Animation-Timing, Clip-Metadaten (Start-Frame, Länge, Speed) | variabel |
   | **EMR** | Skelett: Bone-Hierarchie + 12-bit gepackte Rotationen (ZYX Euler) | variabel |
   | **MD1** | Mesh: Triangles + Quads, 14 × u32 Header | variabel |
   | **TIM** | Textur: CLUT + Pixeldaten (4/8-bit) | variabel |

4. **Skelett-Rotationen dekodieren (EMR)**:
   - 12-bit pro Achse (4096 = 360°)
   - Lesereihenfolge: Z → Y → X (Euler-Konvention)
   - BitReader für Bit-Packed-Werte (nicht byte-aligned)

5. **Mesh-Parsing (MD1)**:
   - 14 × u32 Header (Triangle/Quad-Counts, Offsets)
   - Vertices: s16 × 3 (X, Y, Z)
   - Faces: Tri (3 Vertex-Indices + UV + Farbe) / Quad (4 Vertex-Indices + UV + Farbe)

6. **Optional: glTF-Export** — Skelett + Mesh + Animation + Textur zu `.glb`

### Parser-Klassen

| Komponente | Java-Klasse |
|-----------|-------------|
| PLD-Container | `extractors/pld/PldExtractor.java:18-92` |
| EDD (Animation) | `extractors/emd/AnimationParser.java` |
| EMR (Skelett) | `extractors/emd/SkeletonParser.java`, `BitReader.java` |
| MD1 (Mesh) | `extractors/md1/MD1File.java:163-253`, `extractors/MD1Extractor.java` |
| TIM (Textur) | `extractors/emd/TimParser.java`, `extractors/md1/TimFile.java` |
| glTF-Export | `extractors/emd/EmdGltfExtractor.java` |

### Ausgabeformat

- **Extraktion**: `.edd`, `.emr`, `.md1`, `.tim` (Rohdaten) + optional `.obj` (Mesh) + `.glb` (glTF)
- **Runtime**: Skelett-Struct + Vertex-Puffer → GTE-Transformation (PSX) / Software-Transform (PC)

---

## TIM — Texture Image

### Extraktionsschritte

1. **TIM-Datei identifizieren**:
   - Standalone: `DATA/*.TIM`
   - Eingebettet in RDT (Sektionen `esp_tim`, `model_tim`)
   - Eingebettet in EMD/PLD (TIM-Komponente)
   - PIX-Container: `DATA/*.PIX` (Sammlung von TIM-ähnlichen Sprites)

2. **Header parsen**:
   - Magic: `0x10` (4 Bytes: `10 00 00 00`)
   - Flags (4 Bytes): Bit 0–2 = BPP-Modus (0=4bit, 1=8bit, 2=16bit, 3=24bit), Bit 3 = CLUT vorhanden

3. **CLUT lesen** (wenn vorhanden):
   - CLUT-Header: Länge (u32) + Org-X/Y (u16×2) + Breite/Höhe (u16×2)
   - CLUT-Daten: Palette-Einträge (16-bit ABGR1555)
   - 4-bit: 16 Farben, 8-bit: 256 Farben

4. **Pixel-Daten lesen**:
   - Pixel-Header: Länge (u32) + Org-X/Y (u16×2) + Breite/Höhe (u16×2)
   - Breite in VRAM-Einheiten (bei 4-bit: Pixel/4, bei 8-bit: Pixel/2)
   - Pixel-Daten: Rohe Pixelwerte laut BPP-Modus

5. **Konvertierung zu Bitmap**:
   - 4-bit/8-bit: CLUT-Lookup → RGB
   - 16-bit: ABGR1555 → RGB (A=0 → transparent)
   - 24-bit: Direkt RGB

### Parser-Klassen

| Funktion | Java-Klasse |
|----------|-------------|
| TIM-Parsing (standalone) | `Step1TIMExtractor.java` |
| TIM-Parsing (eingebettet) | `extractors/md1/TimFile.java` |
| TIM→BMP Konvertierung | `extractors/TimToBmpConverter.java` |
| PIX-Container | `Step2PIXToTIMConverter.java`, `Step2bItemPIXToTIMConverter.java` |
| ITP-Container (Icons) | `extractors/ItpParser.java` |

### Ausgabeformat

- **Extraktion**: `.bmp` / `.png` (konvertiert), `.tim` (Rohdaten)
- **Runtime (PSX)**: Direkt in VRAM hochladen (TPage + CLUT-Position)
- **Runtime (PC)**: Konvertiert zu OpenGL-Textur (RGBA8)

---

## VAB — Virtual Audio Bank

### Extraktionsschritte

1. **VAB-Dateien identifizieren**:
   - Standalone: `SOUND/CORE{XX}.VH` + `SOUND/CORE{XX}.VB` (oder `ARMS`, `BGM`, etc.)
   - In RDT eingebettet: Sektionen `snd0_vh`/`snd0_vb` und `snd1_vh`/`snd1_vb`
   - EDH-Variante (Capcom-Wrapper): 64B oder 16B Prefix vor Sony-VabHdr

2. **VH-Header parsen (32 Bytes)**:
   - Magic: `pBAV` (`0x56414270`)
   - Felder: version, vab_id, total_size, ps (Program-Anzahl), ts (Tone-Anzahl), vs (VAG-Anzahl), mvol, mpan

3. **Program-Attribute lesen (128 × 16 Bytes = 2048 Bytes)**:
   - Pro Programm: Tone-Count, Volume, Pan, Priority, ADSR-Werte

4. **Tone-Attribute lesen (16 Tones pro Programm × 32 Bytes)**:
   - Pro Tone: Center-Note, Shift, Min/Max-Key, Min/Max-Vel, VAG-Index, ADSR1/2

5. **VAG-Offset-Tabelle lesen (256 × u16)**:
   - Werte in 8-Byte-Einheiten (Offset × 8 = tatsächlicher Byte-Offset in VB)

6. **VB-Body verarbeiten**:
   - Rohe ADPCM-Daten: 16-Byte-Blöcke pro Sample-Segment
   - Pro Block: `[shift_filter | flags | 14 Bytes Nibbles]`
   - Shift: Bits 0–3 des Header-Bytes (effektiver Shift = 12 - Wert)
   - Filter: Bits 4–7 (0–4, wählt Koeffizienten-Paar)
   - 28 Nibbles → 28 PCM-Samples

7. **ADPCM-Dekodierung (pro Block)**:
   ```
   Für jedes Nibble n (signed, -8..+7):
     sample = (n << shift) + (K0 * prev1 + K1 * prev2) >> 6
     sample = clamp(sample, -32768, 32767)
   ```
   Filter-Koeffizienten (K0, K1):
   | Filter | K0 | K1 |
   |--------|----|----|
   | 0 | 0 | 0 |
   | 1 | 60 | 0 |
   | 2 | 115 | -52 |
   | 3 | 98 | -55 |
   | 4 | 122 | -60 |

8. **EDH-Variante (Capcom-Wrapper)**: Erkenne Prefix-Größe über Trailer-Byte am Ende. Überspringe Prefix (64B oder 16B), dann reguläres VH-Parsing ab Sony-Header.

### Parser-Klassen

| Funktion | Java-Klasse |
|----------|-------------|
| VH-Parsing + Programm/Tone-Tabellen | `extractors/audio/VabFile.java` |
| ADPCM → PCM Dekodierung | `extractors/audio/PsxAdpcmDecoder.java` |
| VAB → WAV Export | `extractors/audio/VabToWavConverter.java` |
| Sample-Fenster-Extraktion | `extractors/audio/SampleWindowExtractor.java` |
| EDH-Wrapper (Format) | `RE15_KNOWLEDGE.md` §1.9 (EDH/VB-Wrapper) |
| VAB-Extension | (kein Skript im Repo — manuell über `VabFile.java`) |

### Ausgabeformat

- **Extraktion**: `.wav` pro Sample (16-bit PCM, Mono, Samplerate aus VH)
- **Runtime (PSX)**: VB direkt in SPU-RAM hochladen (kein Dekodieren — SPU decodiert ADPCM nativ)
- **Runtime (PC)**: ADPCM → 16-bit PCM in Software → SDL2 Audio-Mixer (44100 Hz)

---

## Gesamtpipeline (RE15MasterExtractor)

Die Java-Extraktion läuft in 7 Phasen (gesteuert durch `RE15MasterExtractor.extractAll()`):

| Phase | Schritt | Ausgabe |
|-------|---------|---------|
| 1 | TIM-Extraktion aus DATA/ | `.bmp` pro TIM |
| 2 | PIX→TIM Konvertierung | `.bmp` pro Sprite |
| 3 | RDT-Vollextraktion (alle 6 Stages) | Sektionsdateien pro Raum |
| 4 | BSS-Dekodierung (MDEC→BMP) | `.bmp` pro Kamerawinkel |
| 5 | ESP-Extraktion (Effekt-Sprites) | `.bmp` pro Effekt |
| 6 | EMD/PLD-Extraktion | `.edd`, `.emr`, `.md1`, `.tim`, `.obj` |
| 7 | Sound-Extraktion (VAB→WAV) | `.wav` pro Sample |

Einstiegspunkt: `src/main/java/de/re15/extractors/RE15MasterExtractor.java:100-200`

---

## Fehlerbehandlung

| Situation | Verhalten |
|-----------|-----------|
| Asset-Datei nicht gefunden | Warnung mit vollständigem Pfad, Extraktion für diese Datei überspringen |
| Ungültiger TIM-Magic (≠ 0x10) | Datei als "kein TIM" markieren, nächste Datei verarbeiten |
| BSS-Chunk kleiner als 64KB | Warnung: unvollständiger Chunk, teilweise Dekodierung versuchen |
| VH-Magic ≠ `pBAV` | Prüfe EDH-Variante (Capcom-Prefix). Falls auch ungültig: Fehler melden, Bank überspringen |
| EMD ohne gültigen MD1-Header | Warnung, Skelett/Animation trotzdem extrahieren falls EMR/EDD valide |
| ADPCM-Block mit Shift > 12 | Auf 12 clampen + Warnung (korrupter Block) |
| RDT-Sektion mit Null-Offset | Als nicht vorhanden behandeln, Rest normal weiterverarbeiten |

## Anwendungsbeispiele

### Beispiel 1: "Wie extrahiere ich die Hintergründe von Raum 117?"

1. Finde die BSS-Datei: `STAGE1/ROOM1170.BSS`
2. Lese nCut aus RDT-Header (z.B. nCut=16 → 16 Kamerawinkel)
3. Extrahiere 16 × 64KB Chunks
4. Dekodiere jeden Chunk: VLC → MDEC → BMP
5. Ergebnis: `room117_cam00.bmp` bis `room117_cam15.bmp`

### Beispiel 2: "Welche Sounds hat Stage 1?"

1. Finde VAB-Dateien: `SOUND/CORE00.VH` + `SOUND/CORE00.VB`
2. Parse VH-Header: ps=12 Programme, vs=48 Samples
3. Dekodiere jeden VAG aus VB via ADPCM-Decoder
4. Ergebnis: `core00_sample_000.wav` bis `core00_sample_047.wav`

### Beispiel 3: "Extrahiere das Zombie-Modell"

1. Finde EMS-Archiv: `EMS/EM10.EMS` (Stage 1, Entity 0)
2. Entpacke Container → EMD-Einträge
3. Pro EMD: Extrahiere EDD (Animation), EMR (Skelett), MD1 (Mesh), TIM (Textur)
4. Optional: glTF-Export via `EmdGltfExtractor` → `zombie.glb`

### Beispiel 4: "Welches Format hat eine bestimmte Textur?"

1. Prüfe TIM-Header: Magic `0x10`, Flags-Byte
2. BPP = Flags & 0x07 (0=4bit, 1=8bit, 2=16bit, 3=24bit)
3. CLUT vorhanden = (Flags >> 3) & 1
4. Pixel-Dimensionen aus Pixel-Header (Breite × BPP-Faktor)

## Weiterführende Referenzen

- Gesamtpipeline: `src/main/java/de/re15/extractors/RE15MasterExtractor.java`
- Wissensbasis: `RE15_KNOWLEDGE.md` §1.1–§1.8
- C-Engine (Runtime): `re15_port/engine/src/rdt_common.c`, `re15_port/engine/src/room_load.c`, `re15_port/platform/pc/src/audio_pc.c`
- ADPCM-Referenz: `extractors/audio/PsxAdpcmDecoder.java`, `RE15_KNOWLEDGE.md` §1.10
- EDH-Format: `RE15_KNOWLEDGE.md` §1.9
