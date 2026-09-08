# RE2-Alligator (Typ 0x23) — Beschaffung 2026-09-09

Auftrag (Nutzer): *"Hole mir Charaktermodell und KI des grossen Alligators aus
Resident Evil 2. Natuerlich brauche ich auch alle anderen Dinge - Sound
Animationen etc."*

## Quelle (byte-belegt)

Gegner-TOC der RE2-Leon-EXE `@0x8009ADF4` (Datei-Offset 0x8B5F4), 8 B je Record
(u32 Sektor, u32 Groesse), 4 Records je Typ, Index `(typ-0x10)*4+k` — Mechanismus
und k-Bedeutung aus `analysis/re2_ermittlung_2026-08-30/BERICHT.md` (dort gegen
Typ 0x20/0x36 byte-verifiziert). Container: `info/re2leon/PL0/PLD/CDEMD0.EMS`.

| k | Inhalt | Sektor | Bytes | abgelegt als |
|---|---|---|---|---|
| 0 | AI-Overlay (gelinkt @0x8010D000) | 1512 | 18664 | `EM23_OVL_D000.BIN` |
| 1 | AI-Overlay (gelinkt @0x80100000) | 1522 | 18664 | `EM23_OVL_0000.BIN` |
| 2 | TIM (Textur) | 1532 | 133152 | `EM23.TIM` |
| 3 | EMD (Modell+Animationen) | 1598 | 189208 | `EM23.EMD` |

⛔ NICHT aus `info/re2leon/PL0/PLD/CDEMD0/EM0xx.*` ziehen — der Vorab-Split ist
sektorverschoben (BERICHT.md, "DATENHYGIENE").

Kopien fuer die Laufzeit: `re15_port/shared_assets/RE2/EM23.{EMD,TIM}` und
`EM23_OVL_0000.BIN`.

## Modell und Animationen (EM23.EMD, dir @0x2e2f8, 8 Eintraege)

* **Anim-Paar 1** (dir[1]/dir[2]): **12 Clips**, Frames
  `[163, 26, 150, 150, 45, 120, 125, 165, 97, 37, 30, 59]`;
  EMR1: **22 Bones**, kf_size 112 — das Alligator-Rig (Koerper/Schwanz/Kiefer).
* **Anim-Paar 2** (dir[3]/dir[4]): leer (1 Clip, 0 Frames) — wie bei anderen Bossen.
* **Anim-Paar 3** (dir[5]/dir[6]): **2 Clips** `[30, 120]`; EMR3: **15 Bones**,
  kf_size 80 — das OPFER-Rig (15 Bones = Spieler-Skelett): die Verschling-Sequenz
  posiert Leon, dasselbe Muster wie beim 0x36-Endboss (BERICHT.md 1.4).
* **MD1** (dir[7] @0x23d3c): 28824 B, nObj 46 = **23 Meshes**.
* **TIM**: 256×256, **4 CLUTs**.

## KI-Overlay (EM23_OVL_0000.BIN, 18664 B, laeuft @0x80100000)

Kopf = Zustandstabelle, Zaehler **9**:
```
[0]=0x80101564  [1]=0x801013c4  [2]=0x801013d4  [3]=0x80101418
[4]=0x80101488  [5]=0x801014f8  [6]=0          [7]=0
[8]=0x80101b90   (danach 0x8010183c, 0x80101860, ...)
```
EXE-Bibliotheksprofil (jal-Scan, dieselben 14 Shared-Adressen wie bei den
anderen Overlays): RNG `0x80015FE8` ×39, Sound `0x8001BF10` ×24, Anim-Advance
`0x8002959C` ×11, Effekt `0x8003947C` ×5, ENEM-SE `0x8005BD6C` ×4,
Facing-Align `0x80015910` ×4, Spieler-Schaden `0x800401D4` ×1.
Meistgerufene interne Funktionen: `0x80101bac` ×10, `0x80101c74` ×8,
`0x801012fc` ×4.

Werkzeug fuer die Umsetzung:
`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr> <n> --bin EM23_OVL_0000.BIN`

## Sound

* Gegner-SE-Bank `ENEMSE.VBS` liegt bereits in `re15_port/shared_assets/RE2/`
  (die 4 ENEM-SE-Aufrufe des Overlays gehen ueber `0x8005BD6C` dort hinein).
* Die 24 `0x8001BF10`-Aufrufe tragen ihre Kennungen in Registern, die der grobe
  li-Scan nicht sicher liest — **OFFEN**, beim Disassemblieren der Rufstellen
  mitschreiben (kein Raten).
* Raum-VAB des Alligator-Raums (RE2-Kanalisation): OFFEN, aus dem Raum-RDT
  ziehen, wenn die Einbau-Raeume feststehen.

## Naechste Schritte (Implementierung, nicht heute Nacht "byte-true" behauptet)

1. Overlay disassemblieren: 9er-Zustandstabelle + ACTIVE-Dispatch wie beim
   Zombie (RE15_RE2_AI.md als Vorlage), jede Konstante mit @0x8010xxxx.
2. `enemy_ai_re2_alligator.c` nach dem Muster von `enemy_ai_re2_dog.c`.
3. Modell-Lader: EM23.EMD hat RE2-Rig (22 Bones) — eigener Slot, KEIN
   RE1.5-Retarget (Rig-Bruch-Lehre aus BERICHT.md).
4. Opfer-Rig (Paar 3) an die Victim-FSM anbinden (wie Kraehen/Zombie-Grab).
5. Sounds: Rufstellen-Ids + ENEMSE-Mapping messen.
