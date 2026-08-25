# ROOM1090 — der Feuer-Sound: belegt / bestritten / offen

Adjudikation 2026-08-28. **[eigen]** = in dieser Sitzung selbst geparst / selbst
disassembliert / selbst per `xxd` gelesen.

**Alle drei Gegenpruefer haben `haelt_stand = true` gemeldet. Ich schliesse mich an:
die Kausalkette ist lueckenlos.** Alle Einwaende betreffen Neben-Zahlen und
Feld-Etiketten, keiner traegt den Befund.

---

## A. BELEGT

### A1 — Die Datenlage **[eigen, eigener Parser]**

`MAIN15.BGM` (211240 B), VH-Offset aus dem Trailer `u32 @ size-8` = **0x0F14**, Magic `pBAV`:

    ProgAtr[0] @0xF34 = 03 00 01 42 40 00 00 00
      tones = 3,  mvol = 0 (Byte @0xF35),  prior = 1,  mode = 0x42,  mpan = 64 (@0xF38)
    prog 1..7: mvol = 127 durchgehend

**Programm 0 ist das EINZIGE ab Werk stumme Programm.**

Tone-Tabelle (VH+0x820) **[eigen]**:

    @0x1734  vagid 15  center 72  min = max = 60
    @0x1754  vagid 16  center 73  min = max = 61
    @0x1774  vagid 17  center 74  min = max = 62

`SUB_03.BGM` (29548 B), SEQ#2-Offset aus dem Trailer `u32 @ size-8` = **0x88**, Magic `pQES`:

    Kopf @0x88: ppqn = 0x0030 = 48,  tempo = 0x0AC55A = 705882 us/Viertel  -> 14.706 ms/Tick
    Erstes Ereignis @0x97: 00 b0 00 05   = delta 0, CC0 (Bank Select) = 5
    danach: 01 c0 00 (ProgramChange ch0 = 0), CC7=127, CC10=64, CC99=0x14
    58 Note-Ons, ausschliesslich die Noten 60/61/62

Eigene Ereignis-Zaehlung **[eigen]**: 58 Note-Ons, mittlerer Abstand **1.018 s**,
Median 1.029 s, min 0.353 s, max 1.059 s, letzter Ton bei Tick 3950 = 58.1 s.

`MAIN15`s EIGENE Sequenz waehlt Programm 0 **nie** an (ihre ProgramChanges sind
(ch0,1)…(ch6,7)) — die Knister-Tones brauchen zwingend einen Fremdspieler.

### A2 — Das Raumskript **[eigen, xxd]**

    000022e0: 04ff 1803 0800 0600 1c00 2103 8100 5400
    000022f0: 0001 7833 0600 0c00 2103 8001 5400 0100
    00002300: 0000 ...

    0x22E6  06 00 1c 00           IF, Blocklaenge 0x1c
    0x22EA  21 03 81 00           Ck bank 3, bit 0x81 == 0     (Feuer brennt noch)
    0x22EE  54 00 00 01 78 33     Sce_bgm_control              <-- EINSCHALTER
    0x22F4  06 00 0c 00           IF                            (der Befund las hier 21 03 80 01)
    0x22F8  21 03 80 01
    0x22FC  54 00 01 00 00 00     SsSeqPlay Slot 0

Ausschalter in sub03: `0x24DA  54 00 00 01 01 41` (vol 1 -> mvol 0) und
`0x24E0  54 00 02 00 00 00` (SsSeqStop).

`Flag(3, 0x81)` ist das Feuerloescher-Flag — unabhaengig belegt in
`re15_port/tests/unit/test_1090_flame_out_pin.c` (ROOM1090 sub06 @0x271E).

### A3 — Der Opcode-0x54-Pfad (von zwei Pruefern instruktions-woertlich bestaetigt)

    80042998 ff   Handler: pc[1]<<28 | pc[2]<<24 | pc[3]<<16 | pc[4]<<8 | pc[5]; pc += 6
    80044dac      srl s1,a0,28 (Slot) / srl+andi 0xf (ctl) / sltiu 6
    80010e58      Sprungtabelle = [0x80044f28, 0x80044e00, 0x80044e50, 0x80044e88,
                                   0x80044ee8, 0x80044f20]
    80044f28      ctl 0 = reine Programm-Lautstaerke
    80044f3c/48   Slot 0 -> lw 0x800b3f88 (MAIN-VAB) ; Slot 1/2 -> 0x800b3f8c (SUB-VAB)
    80044f6c/70   part != 0 -> addiu v1,s2,255 ; sb v1,-15(v0)  = ProgAtr[part-1].mvol = vol-1
    80044fbc      ... sb v1,-12(v0)                             = ProgAtr[part-1]+4 (mpan) = pan-1

Fuer `54 00 00 01 78 33`: Slot 0 (MAIN), ctl 0, part 1 → `ProgAtr[0].mvol = 0x78-1 = 119`,
`ProgAtr[0]+4 = 0x33-1 = 50`.

Bank-Wahl: `0x80074828[9] = 0x0355`, Stage-Offsets `@0x800748fc = [0,38,50,65,77,98]`
→ MAIN-Byte 0x55 (&0x3f = 0x15 = MAIN15, Flag 1), SUB-Byte 0x03 (SUB_03).

### A4 — Der Rebind-Mechanismus (von zwei Pruefern woertlich bestaetigt)

    8005dbc0  Fall 0 der CC-Tabelle @0x80011480
    8005db98  lbu s6,0(v1)      ; Controller-WERT
    8005dbd4  sh  s6,76(s5)     ; Seq-Objekt +0x4c = VAB-ID
    8005da50  lh  a1,76(s0)     ; _SsNoteOn liest genau dieses Feld
    8005da54  lbu a2,44(a2)     ; Programm aus seq + curchan + 0x2c
    8005da60  jal 0x80058410    ; SpuVmKeyOn

`CC0 = 5` haengt SEQ#2 also auf VAB 5 = MAIN15 um.

### A5 — Die Live-Gegenprobe im ORIGINAL (von allen drei Pruefern gelesen)

`stage_saves/room1090_orig.sav`: Raum `0x800b0fe2 = 0x0009`;
MAIN-VAB-Zeiger `0x800b3f88 = 0x801f6414` mit Kopf `pBAV ps=8 ts=25 vs=17 mvol=95` (= MAIN15);
**`ProgAtr[0] = 03 77 01 42 32 00 00 00` → mvol 119, +4 = 50** — exakt `0x78-1` / `0x33-1`.
Seq-Objekte (Zeigerfeld 0x800bb500):

| Objekt | +0x4c (VAB) | +0x90 (laeuft) | Sequenz |
|---|---|---|---|
| `[0] 0x800b8774` | 5 | **0** | MAIN15 eigene SEQ — laeuft NICHT |
| `[1] 0x800b8820` | 6 | 1 | SUB_03 SEQ#1 |
| `[2] 0x800b88cc` | **5** | 1 | SUB_03 SEQ#2 — umgehaengt |

Beide Sub-Sequenzen wurden mit derselben Id (6, aus `0x800b21f2`) geoeffnet; nur `[2]`
traegt das CC0 und nur `[2]` liest 5. Das ist staerker, als der Befund argumentiert.

### A6 — Der Port-Defekt (statisch zwingend)

`ss_advance` hat **genau einen** Aufrufer: die Zeile direkt hinter dem Guard in `ss_mix`.
`vab_ok = 1` wird nur in `re15_ss_load` gesetzt, und deren einziger Aufrufer laeuft
ausschliesslich mit `s_ss_main` / `s_ss_sub` — nie mit `s_ss_sub2`
(`re15_bgm_load_sub` setzt dort explizit `vab_ok = 0` + `tone_src = &s_ss_sub`).
Mit dem alten `if (!s->vab_ok) return;` war `ss_mix(&s_ss_sub2, …)` also ein
**dauerhafter No-Op**: SEQ#2 wurde nie getickt, nie gekeyt, nie gemischt.

Gemessen (Vor-Agent, von Pruefer [messung] nachgerechnet): zwei Aufnahmen, einmal mit und
einmal ohne den Einschalter (`RE15_SET_FLAG=3:0x81`), ueber alle 1.911.000 Stereo-Bilder
**bit-identisch**. Kontrolle nachgeliefert: `logA.txt` enthaelt
`[bgm] Sce_bgm_control slot=0 op=0 (part=1 vol=120 pan=51) capTick=702`, `logB.txt` nicht,
dafuer `[setflag] flag(3,129/0x81) = 1`. Die Messung war also **nicht vakuant**.

### A7 — ⛔ STATUS: DER FIX IST BEREITS ANGEWENDET (uncommitted) **[eigen]**

`git status`: `M re15_port/platform/pc/src/audio_pc.c`.
Die Zeile lautet jetzt:

    if ((!s->vab_ok && !s->tone_src) || !s->seq) return;   /* == ss_start-Wache */

darueber ein Kommentarblock mit dem Nutzer-Report vom 2026-08-28 und der vollen
Beleglage. Der Build ist aktuell (`local_build.sh build` → `ninja: no work to do`).
**Der Fix ist gebaut UND gemessen — siehe A8.**

### A8 — ✅ DER FIX WIRKT — nachgemessen **[eigen]**

Lauf: `re15_port/build/platform/pc/re15_pc.exe`, `RE15_NO_INTRO=1 RE15_TITLE_SHOT=t.bmp
RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP="1090@700" RE15_AUDIO_CAP_SYNC=capA2.raw`,
deterministische Aufnahme (1470 Stereo-Frames je Spielbild, kein SDL-Geraet).
Auswertung: Pro-Bild-RMS im Fenster **absolute Bilder 800..1300** — identisch definiert
zur Messung des Vor-Agenten, gleiche Auswertung, gleiche Datei-Semantik.

| Aufnahme | mean | **min** | <RMS 5 | **laengste Stille** |
|---|---|---|---|---|
| `capA.raw`  VOR dem Fix, Feuer AN | 507.6 | **0.69** | 8.4 % | **34 Bilder = 1.13 s** |
| `capB.raw`  VOR dem Fix, Feuer AUS | 507.6 | 0.69 | 8.4 % | 34 Bilder |
| **`capA2.raw` NACH dem Fix, Feuer AN** | **1365.0** | **579.44** | **0.0 %** | **0 Bilder** |
| `capB3.raw` NACH dem Fix, Feuer AUS | 507.6 | 0.69 | 8.4 % | 34 Bilder |

Die vierte Zeile ist der Regressions-Beleg: mit geloeschtem Feuer liefert der Port nach
dem Fix **exakt** die alten Zahlen — die Aenderung fuegt nichts hinzu, wo das Skript
nichts aufdreht.

`sha256(capA.raw) == sha256(capB.raw)` = `62f9c90c737ff893…` **[eigen]** — die
Bit-Identitaet des Vor-Fix-A/B reproduziert exakt.

**Und der A/B-Lauf DIVERGIERT jetzt** — genau der Nachweis, den der Befund als
Regressions-Pin gefordert hat **[eigen]**:

    capA2.raw (Feuer an) gegen capB3.raw (RE15_SET_FLAG=3:0x81, Feuer aus)
      gemeinsame Laenge      5.218.304 Samples
      abweichende Samples    2.976.815      (vorher: 0)
      max |diff|             6574
      erste Abweichung       Stereo-Frame 1.120.200 = Spielbild 762
                             (= 62 Bilder nach dem Sprung nach ROOM1090 bei Bild 700)

Nach dem Fix: der Stille-Boden steigt von **0.69 auf 579**, die Near-Silence-Quote faellt
von 8.4 % auf **0 %**, die laengste Stille von 34 Bildern auf **0**, der Mittelwert
steigt um Faktor 2.7. Das ist genau die Signatur einer **dichten, lueckenlosen
Ton-Ebene** (58 Noten a ~1.0 s, A1) — die Knister-Ebene spielt.

⚠ Einschraenkung: mein Lauf und der des Vor-Agenten sind nicht byte-identisch
parametriert (ich brauchte `RE15_TITLE_SHOT`, um am Titel vorbeizukommen). Der Vergleich
`capA2` gegen `capB3` ist davon nicht beruehrt — beide stammen aus derselben Harness und
unterscheiden sich nur in `RE15_SET_FLAG`; und dass `capB3` die Vor-Fix-Zahlen auf die
zweite Nachkommastelle reproduziert, zeigt zusaetzlich, dass die Fenster-Indizierung
zwischen beiden Harnesses uebereinstimmt.

---

## B. BESTRITTEN (alles kosmetisch, nichts traegt den Befund)

| Behauptung | Nachpruefung |
|---|---|
| „@0x22F4 `21 03 80 01`" | Bei 0x22F4 stehen `06 00 0c 00`; `21 03 80 01` liegt bei **0x22F8** **[eigen]** |
| „ProgAtr[0]: tones=3, mvol=0, mpan=1, attr=0x42" | Byte +2 = 1 ist `prior`, Byte +3 = 0x42 ist `mode`, **mpan liegt auf +4 und ist 64** **[eigen]**. Widerspricht der eigenen Disasm-Herleitung des Befunds (`sb v1,-12(v0)` = ProgAtr+4). |
| „im ~0,73-s-Raster" | Gemessen **1.018 s** Mittel / 1.029 s Median (ppqn 48, tempo 705882 us) **[eigen]**. Das Stille-Argument wird dadurch staerker, die Zahl ist trotzdem falsch. |
| „~1,2 s pro Note bei 22050 Hz" | Gilt nur fuer das ROHE Sample. center_note 72/73/74 gegen gespielte 60/61/62 = eine Oktave tiefer → effektiv ~2,4-2,8 s. |
| „+0x08 = 0x801eed98 (= 0x801eed00 + 0x88)" | 0x801eed00 + 0x88 = 0x801eed88. 0x801eed98 ist der Lesecursor 0x10 dahinter. Identifikation als SEQ#2 stimmt trotzdem. |
| „die MAIN-VAB ist byte-identisch zur Datei" | Falsch an genau den zwei Bytes, die den Befund ausmachen (mvol 0x77 vs 0x00, mpan 0x32 vs 0x40). |
| Lautstaerke „78 → q15 ≈ 20130" | Integer-Kette ergibt 77 → 19866. Irrelevant fuer „deutlich hoerbar". |
| „DER entscheidende Test" (A/B-Bit-Identitaet) | Nicht diskriminierend: ein Null-Ergebnis entsteht identisch bei (a) SEQ#2 wird nie getickt, (b) CC0-Rebind greift nicht, (c) `prog_mvol`-Write landet falsch, (d) Tone-Suche schlaegt fehl. Die **Ursache** steht allein durch die Code-Lesung fest (A6). Der Test belegt nur „die Ebene ist tot". |
| „CC0-Rebind ist korrekt modelliert / kein Offset-Fehler" | Der Port prueft hart `s->bank[chan] == 5`. Im Original ist 5 eine LAUFZEIT-VAB-Id aus `DAT_800b21f1` (`lb a1,8689(a1)` @0x8004470c; SUB aus 0x800b21f2, `lb a1,0(s1)` @0x80044948; Savestate 0x800b21f0..f2 = `04 05 06`). In sich stimmig, aber **kein byte-true Modell** — gehoert als Einschraenkung gemeldet. |

**Zusatzfund eines Pruefers, der den Befund stuetzt und einen Repo-Kommentar widerlegt:**
`audio_pc.c` schrieb den Nutzer-Report „Helikopter-Rotor unhoerbar in ROOM1170" dem
SEQ#2-mvol zu. `SUB_15`s SEQ#2 hat 24 B und **null** Note-Ons, und
`test_rotor_bgm_pin.c` belegt, dass der Rotor SEQ0 ist. Jener „Fix" kann nie geklungen
haben — schon wegen `vab_ok = 0`.

---

## C. OFFEN

1. *(geschlossen — siehe A8: der Post-Fix-A/B divergiert ab Spielbild 762, und der
   Feuer-aus-Lauf reproduziert exakt die Vor-Fix-Zahlen.)*
2. **Regressionsflaeche derselben Zeile.** Der Guard aktiviert JEDE `tone_src`-Instanz,
   also auch `SUB_0A` (16 Noten), `SUB_11` (107) und `SUB_3F` (107). Ob deren SEQ#2 im
   ORIGINAL in ihren Raeumen hoerbar sind, ist ungemessen (kein Savestate geprueft).
   Anmerkung: diese drei haben **kein** CC0, laufen also auf der SUB-VAB.
3. **Kein Hoertest, keine spektrale Korrelation, keine PSX-Aufnahme.** Die Etikettierung
   „Feuer-Sound" ist eine Daten-Inferenz (drei Ein-Noten-Tones, vom Feuerloescher-Flag
   gegated). Ein Pruefer hat die drei VAGs ADPCM-dekodiert (Nulldurchgangsrate 0.022-0.040,
   spektrale Flachheit 0.39-0.43 = rauschartig, tieffrequent) — mit Feuer **vereinbar**,
   nicht beweisend. Lautstaerke-/Pan-/ADSR-Byte-Treue nach dem Fix unverifiziert.
4. **PSX-Target** (`re15_port/platform/psx/src/audio_psx.c:814`, `SCD_AUDIO_SEQ_CTL`)
   nicht untersucht — ob dort dieselbe SEQ#2-Luecke besteht, ist offen.
5. **Zeitpunkt des CC0-Konsums** im Original nicht dynamisch nachgemessen. Der Savestate
   zeigt nur das Ergebnis (+0x4c = 5).
6. **Nicht-SCD-Ambiente** nicht erhoben: der SCD-Weg ist sauber ausgeschlossen
   (vollstaendiger Raum-Walk, kein `0x36 Se_on`, kein `0x3A Sce_espr_on` —
   `analysis/nutzer_batch_2026-08-27/flammen-weg.md:502`). Ob die Engine-Seite
   (raumeigene SE-/EDT-Baenke) in ROOM1090 eine zweite Tonquelle keyt, hat niemand geprueft.

---

## D. NAECHSTER SCHRITT

**Implementierungsreif — bereits im Working Tree angewendet UND gemessen (A8):**

    re15_port/platform/pc/src/audio_pc.c, ss_mix()
    -   if (!s->vab_ok) return;
    +   if ((!s->vab_ok && !s->tone_src) || !s->seq) return;   /* == ss_start-Wache */

Begruendung mit Beleg: eine Sequenz ohne eigene VAB ist im Original normal —
`FUN_80044774` oeffnet BEIDE Sub-Sequenzen mit derselben Bank-Id
(`lb a1,0(s1)` @0x80044948, `SsSeqOpen` @0x8004494c und @0x8004498c) und die Sequenz
haengt sich per CC0 selbst um (`sh s6,0x4c(s5)` @0x8005dbd4; `_SsNoteOn` liest genau
dieses Feld, `lh a1,76(s0)` @0x8005da50). `vab_ok` beschreibt nur, ob die Instanz eine
EIGENE Bank geparst hat, und ist als Abspiel-Gate falsch.

**Was jetzt noch getan werden MUSS:**

1. ✅ *erledigt* — der A/B-Gegenlauf ist gefahren und divergiert (A8).
2. **Regressionsflaeche pruefen**: in welchen Raeumen `SUB_0A`/`SUB_11`/`SUB_3F` geladen
   werden und ob deren SEQ#2 dort im Original laeuft (Savestate + `+0x90` des dritten
   Seq-Objekts).
3. **PSX-Target** dieselbe Stelle nachziehen.
4. **Commit + Paket** — die Aenderung liegt uncommitted im Working Tree
   (`M re15_port/platform/pc/src/audio_pc.c`).

**Gestrichen (unbelegt):** nichts am Fix. Aber diese Formulierungen duerfen so nicht in
eine Commit-Message:
* „im ~0,73-s-Raster" (gemessen 1.018 s),
* „ProgAtr[0] … mpan = 1" (mpan liegt auf +4 und ist 64),
* „die MAIN-VAB ist byte-identisch zur Datei" (die zwei entscheidenden Bytes weichen ab),
* „@0x22F4 `21 03 80 01`" (das liegt bei 0x22F8),
* „der entscheidende Test" fuer die A/B-Bit-Identitaet (sie ist nicht diskriminierend).
