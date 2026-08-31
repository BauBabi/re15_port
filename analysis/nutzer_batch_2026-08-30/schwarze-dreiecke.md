# schwarze-dreiecke — Schiedsspruch

Alle strittigen Punkte selbst nachdisassembliert/nachgeparst (nicht nach Mehrheit entschieden).

## URSACHE (belegt)

Dem Port fehlt das **Per-Part-Zeichenbit** (`part.flags` Bit 0, Part-Array `entity+0x188`, Stride 0xAC).
Der Typ-0x26-INIT loescht es; der Port kennt es nicht und zeichnet das Mesh.

Selbst verifiziert, woertlich:

```
STAGE1.BIN  801165d0: lw    a0,392(v0)      ; entity+0x188 = Part-Array
            801165d4: nop
            801165d8: lw    v0,0(a0)
            801165dc: addiu v1,zero,-2      ; ~1  -> Bit 0
            801165e0: and   v0,v0,v1
            801165e4: sw    v0,0(a0)        ; part[0].flags &= ~1
```

```
PSX.EXE     8001eb14: andi  v0,v0,0x20      ; Part-Flag Bit 5
            8001eb18: beq   v0,zero,0x8001eb2c
            8001eb20: addu  a0,s2,zero      ; s2 = Part
            8001eb24: jal   0x8001f024
PSX.EXE     8001e74c: ori   v0,zero,0x1     ; Binder FUN_8001e5b0: JEDER Part flags = 1
            8001e758: sw    v0,0(s2)
```

Der Zeichner FUN_8001e9ec springt ohne Bit 0 in den Epilog (`andi v0,v1,0x1` @0x8001ecc4 /
`beq v0,zero,0x8001ee48` @0x8001ecc8); die Emitter @0x8001ed04 (Tris) / @0x8001ed34 (Quads)
laufen dann nie. Von beiden Pruefern und von mir uebereinstimmend bestaetigt.

**EM26 ist ein Modell aus einem Dreieck — jetzt vollstaendig belegt.** Die im Befund unbelegte
Praemisse "1 Bone" habe ich nachgemessen:

```
CDEMD0.EMS @0x1B5820 (EMR):  10 00 14 00 01 00 14 00
   -> relpos_ofs 0x10, len 20, count = 1, size 20   => 1 Bone => 1 Part
CDEMD0.EMS @0x1B586C (MD1, 124 B): hdr (65,0,2); tv=3 tf=1 qf=0
   Verts  84 03 00 00 / 3e fe f5 fc / 3e fe 0b 03  = (900,0,0)(-450,-779,0)(-450,779,0)
   Normale 00 f0 00 00 = (0,0,-4096)
   Tri-UV  1f 37 00 78 | 7f 00 80 00 | 7f 6f       = (31,55)(127,0)(127,111) clut 0x7800 tpage 0x0080
```

Sieben Emitter in ROOM1090, selbst geparst (`ROOM1090.RDT` @Datei 0x2214, 7x 20 Byte, op 0x44,
Typ 0x26): (2052,-1800,-1334) (2375,-1800,-2333) (3104,-1800,-3498) (2974,-1800,-860)
(2874,-1800,440) (2774,-1800,2040) (1374,-1800,1340).

Portseitig bestaetigt: `platform/pc/main.c:6580-6612` gated nur auf `npc->active` und den
Cut-Region-Quad-Cull. Die einzige Part-Maske im ganzen Port ist `re2z_part_flags`
(enemy_ai_re2_zombie.c:3761 Seed, :4090-4092 Bit-0-Clear) — RE2-Zombie-lokal.

## BESTRITTEN

1. **"Scan ueber ALLE sechs STAGE*.BIN ... Umfang exakt zwei Typen" — WIDERLEGT.**
   Es gibt einen dritten, *generischen* Clear in der EXE, und er sitzt im Zeichner selbst:
   ```
   8001f024: addiu sp,sp,-64
   8001f028: addu  a2,a0,zero        ; a2 = a0 = der PART
   ...
   8001f0f4: addiu v1,zero,-17       ; ~0x10 -> Bit 4 clear
   8001f100: sw    v0,0(a2)
   8001f12c: addiu v1,zero,-2        ; ~1   -> Bit 0 clear
   8001f130: and   v0,v0,v1
   8001f134: sw    v0,0(a2)
   ```
   Aufgerufen aus FUN_8001e9ec heraus, gegated durch Part-Flag 0x20 (@0x8001eb14-24).
   `part.flags` Bit 0 ist damit **kein statisches Spawn-Attribut zweier Typen**, sondern ein
   lebendes Per-Frame-Flag mit mindestens drei Schreibern (Truemmer-/Gib-Physik).

2. **"Dispatch 0x80072bac[0x26]=0x80116288" — der Beleg traegt die Aussage nicht.**
   `read 0x80072c40` liefert in `info/Re1.5/PSX.EXE` acht Nullen. Der Wert wird zur Laufzeit
   geschrieben: STAGE1 `lui v0,0x8011` / `addiu v0,v0,25224` (=0x80116288) @0x8011e8f4 /
   `sw v0,11332(at)` -> 0x80072c44 @0x8011e8fc. Aussage richtig, Zitat falsch — im Code die
   Installer-Adresse zitieren, nicht die Tabelle.

3. **STAGE2-Zitat war nicht woertlich.** Real steht dort ein `nop` und ein Cache-Store dazwischen:
   ```
   8010efc8: lw    a0,392(v0)
   8010efcc: nop
   8010efd0: lw    v0,0(a0)
   8010efd4: addiu v1,zero,-2
   8010efd8: lui   at,0x8012
   8010efdc: sw    a0,-27880(at)     ; = 0x80119318  PART-ZEIGER GECACHT
   8010efe0: and   v0,v0,v1
   8010efe4: sw    v0,0(a0)
   ```
   Der Pruefer hat recht: ueber diesen Cache kann STAGE2 das Flag spaeter ohne das
   `lw ?,0x188(?)`-Idiom anfassen — das Suchmuster des Umfangs-Beweises ist also blind fuer
   genau die Klasse Schreiber, die es beweisen soll.

4. **Dir-Index-Etiketten ("dir[7]=+0x6C, dir[8]=+0xE8") sind falsch — und die Gegenrede
   ("count=12, Index 5/6") ebenfalls.** Selbst geparst:
   ```
   Blob-Header @0x1B5800: 08 83 00 00 09 00 00 00      -> dir_offset 0x8308
   @0x1B5800+0x8308:      08 00 00 00 | 0c 20 5c 60 64 68 6c e8 (je u32)
                          -> count = 8, Offsets [0xc,0x20,0x5c,0x60,0x64,0x68,0x6c,0xe8]
   ```
   0x6C ist Index **6**, 0xE8 ist Index **7**. Drei Parses, drei Ergebnisse => im Code
   **Datei-Byte-Offsets** zitieren (blob+0x6C / blob+0xE8), keine Index-Etiketten.

5. **Der Yaw-Einwand des Pruefers ist FALSCH, der Befund hatte recht.** Record-Bytes 16..17 sind
   `00 04`, little-endian = **0x0400 = 1024**. Alle sieben Records tragen 1024. (Selbst geparst.)

6. **"Der Fehler ist NICHT flavor-abhaengig, weil A/B leer ist" — Schluss ungueltig.**
   Der Messbuild enthaelt 32f2e9e4 (v0.3.29) bereits; Flavor-Gleichheit ist die *erwartete*
   Signatur eines WIRKSAMEN v0.3.29. Der Widerruf der v0.3.29-**Begruendung** bleibt trotzdem
   richtig, aber er steht auf dem Modell-Argument: EM26 hat 1 Bone (EMR-count=1, s.o.) und die
   Directory-Eintraege 0x60/0x64/0x68 sind 4-Byte-Stubs — es gibt keine zweite Bank, "falsch
   posiertes Skelett" ist fuer 0x26 physisch unmoeglich.

7. **Nebenbefund bestaetigt:** `enemy_ai_common.c:11699` etikettiert `*+0x188 &= ~1` bei Typ 0x24
   als "DISABLES its own collision". Falsch — es ist das Zeichen-Bit. Kommentar korrigieren.

## OFFEN

- **Der Direktbeweis fehlt weiterhin.** Kein Kill-Switch-/Magenta-A/B des EM26-Draws. Die
  Zuordnung "die vom Nutzer gesehenen Dreiecke SIND EM26" ist indiziengestuetzt (Position, Form,
  Umkreisradius 900, Textur-Mittel, BG13-Ausschluss). Der Kernsatz muss bis dahin als
  **Hypothese** formuliert werden.
- **Zweiter Setzer FUN_8001ee68 — Reichweite ungeklaert.** Er setzt per Entity ALLE Parts
  wieder auf 1 (`ori s3,zero,0x1` @0x8001ef04 / `sw s3,0(s1)` @0x8001ef14, Stride 172
  @0x8001ef20). Mein jal-Vollscan: aufgerufen aus JEDEM Stage-Overlay (STAGE1 @0x80100fe0,
  STAGE2 @0x80100e74, STAGE3 @0x801010cc + @0x801168fc, STAGE4 @0x80100f94, STAGE5 @0x80101114 +
  @0x80117110). In STAGE1 sitzt er im Tail von FUN_80100688; **diese Funktion hat KEINEN
  `jal`-Aufrufer und genau eine Datenreferenz (@0x8011f7b4)**, deren Tabellen-Index-Domaene ich
  nicht aufloesen konnte (0x8011f790 wird an anderer Stelle als Kollisions-Deskriptor-Zeiger
  nach entity+0x78 geladen, @0x80100770 / @0x8010ac2c — die Region ist gemischte Daten).
  Solange das offen ist, ist nicht bewiesen, dass das geloeschte Bit fuer 0x26 der Dauerzustand
  ist.
- Typ 0x24 in ROOM20B0/20B1 nicht geladen, nicht nachgemessen.
- Kein PSX-Gegenbeweis. "Im Original unsichtbar" ist rein statisch.
- ROOM1090 Slot 4/5 melden "0.0% transparent, 3.1% opak-schwarz" — ungeprueft, gleiches
  Stolperdraht-Muster wie vier fruehere Schwarzer-Klotz-Bugs.
- PSX-Renderer-Zweig (platform/psx) nicht analysiert.

## NAECHSTER SCHRITT

**Implementierungsreif (Konstanten alle mit Herkunft, keine erfundene dabei):**

1. Generische Per-Part-Zeichenmaske in `re15_actor_t`, beim Bank-Bind auf 1 geseedet
   — Gegenstueck zu `ori v0,zero,0x1` @0x8001e74c / `sw v0,0(s2)` @0x8001e758.
   `RE15_EMD_MAX_BONES` existiert (re15_emd.h:47). `re2z_part_flags` sollte sie speisen bzw.
   von ihr ersetzt werden, damit die Regel nur EINMAL existiert.
2. Schranke in der NPC-Render-Schleife (main.c:6580ff., pro Mesh-Slot) —
   `FUN_8001e9ec: andi v0,v1,0x1 @0x8001ecc4 / beq @0x8001ecc8`. Gleiche Schranke in
   `platform/psx`, sonst driften die Targets.
3. Clear im INIT von Typ 0x26 (`@0x801165d0-0x801165e4`) und Typ 0x24
   (`@0x8010efc8-0x8010efe4`, inkl. Notiz zum Cache-Store nach 0x80119318).
4. Schatten und Kollision der Emitter bleiben AN (`jal 0x8001b064` @0x80116740 mit a1=-1800,
   `jal 0x8002aec4` @0x80116368). "Aktor unsichtbar/inaktiv machen" waere falsch.
5. Kommentar-Widerrufe: enemy_ai_common.c:4747-4762 und main.c:6874ff. ("das ist die
   Geometrie, die als schwarze Dreiecke erscheint") — Kausalsatz streichen, der v0.3.29-CODE
   bleibt. enemy_ai_common.c:11699 Etikett korrigieren.

**Vor der Fertigmeldung noch zu RE'en:**

- **Direktbeweis**: Magenta-Tint des EM26-Draws, gdigrab am echten Fenster
  (Skill `re15-port-visual-verify`). Zwei Zeilen, schliesst die Zuordnung.
- **Lebensdauer des Bits**: Der dritte Schreiber (FUN_8001f024, Bit-0x20-gegatet) macht das
  Flag zu einem lebenden Zustand. Vor dem Gate die Lebensdauer messen
  (Memory `reai-v2-klebrige-bits`). Naechster Weg: PCSX-Redux-Watchpoint auf das Flag-Wort
  von part[0] einer 0x26-Entity (Skill `re15-pcsx-watchpoint`) — das schliesst in einem Zug
  auch die offene Frage nach FUN_8001ee68 und nach Lesern von Bit 0x20.
- Typ 0x24 in ROOM20B0/20B1 gegenmessen, bevor der Fix dort als fertig gemeldet wird.

**Kein Fix-Vorschlag hier enthaelt eine Konstante ohne Herkunft.** Zu streichen ist keine
Konstante, sondern eine **Umfangsbehauptung**: "Umfang exakt zwei Typen" ist widerlegt und darf
so nicht in den Code.
