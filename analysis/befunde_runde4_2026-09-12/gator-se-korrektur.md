# BEFUND — gator-se-korrektur: "Der Sound von den Aligator Bissen ist falsch"

Nutzer 2026-09-12 (v0.7.86+, ROOM2090-Boss spielt seit Runde 3 ENEMSE Bank 17 SE 4 als Biss).
Analyse 2026-09-12, Runde 4. Vorgänger: analysis/befunde_runde3_2026-09-12/gator-biss-sound.md.

## KURZFAZIT

**Bank 17 / flag2000=0 sind RICHTIG — die SE-Id 4 am ZUBEISSEN ist falsch.**
SE 4 ist der 2,40-s-ANGRIFFS-BRÜLLER mit **1,43 s Attack** (WAV-Render unten): sein Peak
kommt erst ~1,4 s nach Key-On. RE2 startet ihn am **Lunge-BEGINN** (Clip 3: Maul öffnet erst
ab ~F54, Peak F96/150) — der Brüller schwillt während des Anlaufs an und peakt MIT dem Biss.
Der Port ruft ihn dagegen an 6 ZUBEISS-Stellen (Clip 4: Maul auf F4, zu F24), d.h. der
Brüller fängt beim Zubeißen gerade erst an zu grummeln und läuft 2,4 s nach — bei FRESSEN
Schnapp 1+2 überlappen sich zwei Brüller auf Kanal 4. Das hört der Nutzer als "falschen
Biss-Sound". Das einzige kurz-perkussive Sample der Bank ist **SE 3** (Sofort-Attack <30 ms,
hell/rauschig — "Klatschen"); ein echtes Schnapp-Sample existiert für EM23 **nirgends**
(Zubeiß-Clip 4 ist in RE2 datenseitig stumm, Room-Bank ROOM40A0 ist leer).

## 1. PAAR-TABELLE @0x800A7400 — KOMPLETT-DUMP + Hunde-Kalibrierung

Quelle: info/re2leon/PSX.EXE, t_addr=0x80010000 (u32 @0x18) → Datei-Offset 0x97C00.
Lesart (Hunde-Lehre enemy_ai_re2_dog.c:98-131, bestätigt): Zeilenindex = Bank-Nummer,
Zeile = {sound_a, sound_b} — **SOUND-IDs aus Spawn-Record+7 → entity+0x1FA**
(`lbu v0,7(v0)` @0x80057274, Vergleich `lb v1,506(a0)` @0x80052C48), KEINE Gegner-kinds.
flag2000: die Entity, deren Sound-Id in der ZWEITEN Zeilenhälfte steht, bekommt
word0|=0x2000 (@LAB_80052c2c).

Voller Dump (74 Zeilen, Terminator 0xFF @Datei 0x97C92) — Auszug der relevanten Zeilen;
kompletter Dump im Scratch-Skript reproduzierbar (gator_census.py):

| Zeile=Bank | Paar | Datei | Bedeutung |
|---|---|---|---|
| 6 | {0x0C,0x00} | 0x97C0C | Hund solo (Referenz-Anker, Bank 6 = Hunde-Fix) |
| **17** | **{0x16,0x00}** | 0x97C22 | **Alligator solo** |
| 22 | {0x23,0x00} | 0x97C2C | (Sound-Id 0x23 — NICHT der kind 0x23!) |
| 67 | {0x16,0x1F} | 0x97C86 | Alligator + NPC-Sound 0x1F |
| 68 | {0x16,0x20} | 0x97C88 | Alligator + NPC-Sound 0x20 |

0x16 kommt in GENAU diesen drei Zeilen vor (17/67/68); in allen dreien steht 0x16 in der
ERSTEN Hälfte → **flag2000 des Alligators = 0 in jedem möglichen Raum-Paar**.

## 2. EM23-ZENSUS über die RDTs — welcher Raum, welche Bank

Eigener Zensus über alle **250** ROOM*.RDT in info/re2leon/PL0/RDT, Opcode 0x44
(Sce_em_set, 22 B; +3=kind, +7=sound), mit echtem SCD-Walk (RE2-Opcode-Längen aus
analysis/nutzer_batch_2026-08-27/tools/re2_scd_lens.py) über scd16(init)+scd17(main):

- kind **0x23** existiert in **GENAU EINEM** Record auf der ganzen Disc:
  **ROOM40A0.RDT scd16 sub00 @0x025A0** = `44 00 00 23 00 00 01 16 …` → **sound-id 0x16**.
- Gegenprobe Roh-Byte-Scan (`44 ?? ?? 23`, alle Bytes aller RDTs): 1008 Roh-Treffer,
  davon liegt **genau 1** in einer SCD-Sektion — derselbe @0x025A0. Kein EM23-Spawn kann
  sich in einem Walk-Abbruch verstecken.
- **room1100 / room2070 / room60B0 (Frage der Aufgabe): KEIN EM23** — der Alligator-Raum
  der RE2-Leon-Disc ist ausschließlich ROOM40A0.
- ROOM40A0, alle 0x44-Records: scd16 = 0x41/snd 0x00, 0x45/snd 0x00, **0x23/snd 0x16**;
  scd17 = 2× 0x41/snd **0x1F**. Raum-Paar also {0x16} (nur Init) bzw. {0x16,0x1F}
  (wenn die scd17-NPCs spawnen) → **Bank 17 oder Bank 67** — nie eine andere.

**Und das ist klanglich egal — die drei Bänke tragen byte-identische Gator-Hälften**
(TOC @0x800A7B1C, Datei 0x9831C; ENEMSE.VBS aus re15_port/shared_assets/RE2/):

| Bank | EDT @ | VBD @ | VBD-md5 | Map SE0-5 |
|---|---|---|---|---|
| 17 | 0x128000 | 0x129000 (62512 B) | `37b9088e7909` | 04120000 02230000 03330000 02430000 04520000 04620000 |
| 67 | 0x5D9000 | 0x5DA000 (62512 B) | `37b9088e7909` | identisch |
| 68 | 0x5E9800 | 0x5EA800 (62512 B) | `37b9088e7909` | identisch |

(67/68 haben zusätzlich NPC-Einträge SE16-31, teils mit VAB-Override b0=0x82 auf eine
fremde Laufzeit-VAB — für den Gator irrelevant.) → **GATOR_ENEMSE_BANK 17 bleibt richtig.**

Die Room-SE-Bank von ROOM40A0 ist leer (snd0.edt 192 B ohne pBAV, snd0.snd 12 B) — es
gibt in RE2 also KEINEN raumseitigen Zusatz-Laut (kein verstecktes Chomp-Sample).

## 3. EDT/VH DER BANK 17 + WAV-RENDER (analysis/befunde_runde4_2026-09-12/gator_se_wavs/)

VH: progs=1, tones=7, vags=7. Pitch je SE = note2pitch2(min, shift, center, shift)
(vab_common.c:320; gegengerechnet: SE4 → Pitch 0x4C1 = 13102 Hz ≈ Render-Rate 13111 Hz,
SE3 → 0x800 = 22050 Hz — die WAVs klingen wie im Port). Decoder: SPU-ADPCM (eigenes
Python-Render, tools/gator_banks.py).

| SE | Kanal/Prio | VAG (B) | Rate | Dauer | Hüllkurve (30 ms/Zeichen) | Charakter |
|---|---|---|---|---|---|---|
| 0 | ch4 p2 | 5 (9328) | 12375 | 1,32 s | `+#*+++*++********#####%##**++++==--::..` | Sofort-Einsatz, ~1 s getragen, ZCR~1000 Hz — **großes Jaulen/Bellen** (Hurt-Großreaktion) |
| 1 | ch2 p3 | 4 (7728) | 15592 | 0,87 s | ` -**@@#**++==--:...::...` | 0,15 s Anschwellen, vokal — **Knurr-Grunzer** (Clip 5 f2) |
| 2 | ch3 p3 | 2 (8224) | 14717 | 0,98 s | `    .-+++=-=+#==:::....:..` | aus Stille anschwellend, tief (ZCR~600) — **Wasser-Schub/Schwimmen** (Clip 0 f76/f162) |
| **3** | ch2 p3 | 3 (11888) | **22050** | 0,94 s | `+%@%%@#***+==-:....:.....:.` | **Attack <30 ms auf Vollpegel**, hell/breitbandig (ZCR~1600), ~0,4 s Abklang — **Klatsch/Crash** (Clip 5 f80, Wasser-Einschlag) — das einzige kurz-perkussive Sample |
| 4 | ch4 p2 | 6 (18032) | 13111 | **2,40 s** | `.-+#######**##*#*###***###%##**%++++%##%##%##*@@%-=++==-===++=--...` | tief (ZCR~700), **Attack 1,43 s**, mehrere Schwell-Peaks, Maximum bei ~1,4-2,0 s — **der Angriffs-BRÜLLER**, definitiv KEIN Schnappen |
| 5 | ch4 p2 | 7 (7264) | 15592 | 0,81 s | `.+%##**+===--:::..` | Attack 0,05 s, 0,5 s Abklang, vokal — **kurzer Bell-Grunzer = Standard-Hurt** (@0x801020C4) |

## 4. RE2-TIMING DES SE 4 — warum er am Port-Zubeissen falsch klingt

Rufstellen re-verifiziert (jal-Scan EM23_OVL_0000.BIN): SE4 @0x80100D84 (li a0,4
@0x80100D64; ACTIVE sub3 **Phase 0 = Clip-3-START**), SE5 @0x801020C4, SE4 @0x801021EC,
SE0 @0x80102C10. Frame-Flags: Clip 3 f50/f77 = SE4 (0x48000178/0x48000193).

- RE2 Clip 3 (Lunge-Biss, 150 F): Maul öffnet ~F54, Peak F96 → SE4-Start bei F0 gibt dem
  Brüller **~1,8 s Vorlauf**; sein 1,43-s-Peak fällt MIT der Maul-Aktion zusammen.
  Die f50/f77-Flags re-triggern ihn mitten im Clip (Kanal 4 ersetzt sich selbst).
- Port Clip 4 (Schnapp, 45 F): Maul auf F4, Peak F12, zu F24 (~0,4-0,8 s) — der an
  6 Stellen (enemy_ai_boss_gator.c:351,1269,1333,1667,1843,1874) am **Schnapp-Start**
  gerufene SE4 ist beim Zubeißen noch im Anschwellen und brüllt 1,6+ s in die Zeit NACH
  dem Biss; FRESSEN Schnapp 1→2 (174 F ≈ 2,9 s) kettet zwei 2,4-s-Brüller.
- RE2 kennt datenseitig KEINEN Laut auf dem Zubeißen selbst: Clip 4 trägt keine
  Frame-SEs (Runde-3-EDD-Zensus), die Room-Bank ist leer (§2).

Rand-Divergenz (klein, dokumentieren): der 55-F-Brüll-Cooldown wird in RE2 NUR von
SE-4-**Frame-Flags** gearmt (Wort 0x48xxxxxx @0x80100424-34), nicht vom direkten Call
@0x80100D84; der Port armt in gb_se() bei jedem id==4.

## 5. FAZIT — WERTLISTE für enemy_ai_boss_gator.c

**Bank/Hälfte (unverändert): GATOR_ENEMSE_BANK = 17, flag2000 = 0** — Beleg §1/§2
(Paar-Zeile 17 {0x16,0x00} @Datei 0x97C22; einziger EM23-Raum ROOM40A0 mit Sound-Id 0x16;
Bänke 17/67/68 byte-identisch in der Gator-Hälfte, 0x16 überall erste Hälfte).

| Ereignis | SE | Beleg / Status |
|---|---|---|
| **ZUBEISSEN/Schnapp (alle 6 Stellen)** | **SE 3 statt SE 4** | MAPPING (als solches kommentieren): RE2 hat aufs Zubeißen keinen Laut; SE 3 ist das einzige kurz-perkussive Sample der Bank (Attack <30 ms, 22 kHz "Klatsch"; RE2-Kontext: Wasser-Einschlag Clip 5 f80). SE 4 dort ist die Fehlerursache. |
| **Angriffs-BRÜLLER** | SE 4 — nur mit ≥1,4 s Vorlauf vor Maulkontakt | RE2 @0x80100D64/84 (Lunge-Clip-START, Maul erst ab F54). Im Port passt das an einen Angriffs-COMMIT mit Anlauf (z.B. FRESSEN-Beginn), NICHT an den 45-F-Schnapp. Ersatzlos streichen ist ebenfalls RE2-näher als die heutige Position. |
| HURT Standard | SE 5, Gate brull_cd==0 | bereits korrekt (:599; RE2 @0x801020C4, Gate @0x801020AC-B8) |
| HURT Großreaktion (optional) | SE 0 | RE2 @0x80102C10 (rng&3==0, Cooldown 120+(rng&0x3f) @0x80101FBC-D4) |
| Frame-Flags (Spieler :823-836 vorhanden) | Wort>>28 bei Bit 0x08000000 | bereits korrekt; liefert SE 2 = Schwimmen (Clip 0 f76/f162), Todesrolle Clip 7 (SE4 f4/f45/f115 + SE2 f25/f36/f44), Wende Clip 6 (SE4 f6, SE2 f30/f34) |
| Brüll-Cooldown 55 F | nur von SE-4-FRAME-Flags armen | RE2 @0x80100424-34; heutiges Armen im direkten gb_se(4) ist eine kleine Divergenz (§4) |

**WAVs zum Anhören:** analysis/befunde_runde4_2026-09-12/gator_se_wavs/bank17_se0{0..5}_*.wav
(Pitch == Port, §3).

## Reproduktion

Skripte unter analysis/befunde_runde4_2026-09-12/tools/: gator_census.py
(Paar-Tabellen-Dump + SCD-Walk-Zensus + Roh-Scan), gator_banks.py (TOC/EDT/VH-Dump,
Bank-Vergleich, ADPCM→WAV), gator_wav_fine.py (10-ms-Hüllkurve/ZCR). Eingaben:
info/re2leon/PSX.EXE, info/re2leon/PL0/RDT/*.RDT, re15_port/shared_assets/RE2/ENEMSE.VBS,
info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN.
