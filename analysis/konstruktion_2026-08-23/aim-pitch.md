# Aim-Elevation-Pitch

**RE-DOSSIER: AIM-ELEVATION-PITCH (P3)**

## 1) MECHANISMUS (vollständig, disasm-belegt)

**Kernbefund: Die Armneigung ist im Original zu 100 % CLIP-DATEN (Kandidat b) — pro Zielstufe ein eigener 1-Frame-Hold-Clip + eigener Recoil-Clip aus der Waffen-PLW-Bank, weich gemacht durch den 8-Frame-Crossfade (+0x8f=7) bei jedem HOLD-(Re-)Entry. Es gibt KEINE Bone-Rotation-Injektion, KEIN IK, KEIN vertikales Auto-Track.**

**Elevation-Zustand** = `DAT_800acaec` (u16, = Spieler-Entity `+0x98`; Basis `0x800aca54`): bit15 `0x8000`=HOCH, bit14 `0x4000`=MITTEL, bit13 `0x2000`=TIEF; low 13 bits = Status-Flags (bit1 Gift, `andi 0xfffd`-Clear im Heal-Pfad @0x8004af8c) und werden bei jedem Elevation-Write mit `& 0x1fff` erhalten.

**Gun-FSM** (Sub-Dispatch-Tabelle @0x800740f4: [0]=RAISE 0x80032f18, [1]=HOLD 0x80033180, [2]=DISCHARGE 0x80033460, [3]=LOWER 0x80033c74, [4]=RELOAD 0x80033d7c, [5]=L1-RETARGET 0x80033eec):

- **RAISE-Entry** @0x80032f64–0x80032fa4: Clip 6 (`ori v0,6` @0x80032f68 → `sb v0,0x800acae8` = +0x94), Frame 0 (`sb zero,0x800acae9` @0x80032f84), **Blend 7** (`ori v1,7` @0x80032f7c → `sb v1,0x800acae3` = +0x8f @0x80032f8c), Elevation-Reset `acaec = (acaec & 0x1fff) | 0x4000` (@0x80032f98–fa4). Autoaim-Latch `jal 0x8003703c` mit `a0=0x7530`=30000 (@0x80032fa8-ac); Auto-Track `jal 0x8001a8f8(target+0x34, 0xC8)` @0x80032fec — **FUN_8001a8f8 schreibt ausschließlich Yaw** (`entity+0x6a` = DAT_800acabe; Decompile RE_15_Quellcode_V2\FUN_8001a8f8.c), keine vertikale Komponente.
- **RAISE-Früh-Preempt (im Port fehlend):** @0x800330b0–0x8003316c: ab Raise-Frame ≥7 (`sltiu a0,+0x95,7` @0x800330c4) + Pad-TIEF (0x20, @0x800330e4) → sofort `aca5a=1` (HOLD, @0x800330fc) mit `acaec=(acaec&0xbfff)|0x2000` (@0x80033100–10c); ab Frame ≥8 (`sltiu +0x95,8` @0x8003311c) + Pad-HOCH (0x10, @0x80033134) → `aca5a=1` + `|0x8000` (@0x80033150–15c). Normales Raise-Ende: `aca5b==2` (@0x80032f54) → `aca5a=1` @0x80033168.
- **HOLD-Entry** (bei `aca5b==0`) @0x800331a0–dc: **Blend +0x8f=7** (@0x800331ac), Frame=0 (@0x800331bc), **Clip +0x94 = 8 + 2·(acaec>>15) + 4·((acaec>>11)&4)** (@0x800331c0–d4) = **8 (MITTEL) / 10 (HOCH) / 12 (TIEF)**.
- **Dpad-Umschaltung in HOLD** (Pad-held DAT_800ac768; R1=0x100-Gate @0x800331ec): HOCH (0x10): wenn `!(acaec&0x8000)` → `acaec=(acaec&0x1fff)|0x8000` + `aca5b=0` (@0x8003320c–3c) → nächster Tick läuft den Entry erneut = neuer Clip mit Blend 7. TIEF (0x20): `|0x2000` (@0x80033250–88). Kein Dpad: zurück auf `|0x4000` (@0x8003329c–d4).
- **DISCHARGE-Entry** @0x80033480–bc: `aca5b=1`, Frame=0 (@0x8003349c), **Blend 7** (@0x800334a4), **Recoil-Clip = 7 + 2·(acaec>>15) + 4·((acaec>>11)&4)** (@0x800334a8–bc) = **7/9/11**. Feuer-Gate: SQUARE 0x40 @0x80033308, Munition 0x8004ea6c, leer+Edge→Reload sub4 (@0x80033378) bzw. Dry-Fire-SE (@0x8003338c).
- **LOWER/RELOAD** setzen Elevation zurück (`&0x1fff` bzw. `|0x4000`, Reads/Writes @0x80033ca8/cc8, @0x80033db4/dd8 — im Port bereits zitiert und umgesetzt).

**Melee-FSM strukturell identisch:** HOLD-Entry @0x800350e4–120 (**Blend 7** @0x800350f0, Frame=0 @0x80035100, gleiche Clip-Formel @0x80035104–120), HOCH @0x80035164–7c, TIEF @0x800351ac–c8; Slash 7/9/11 @0x8003535c–70.

**Asset-Beweis (Datei-Byte-Offsets, `re15_port/shared_assets/PSX/PLD/`):**
- `PL00W03.EDD` (1208 B): Clip-Table-Einträge (u16 cnt, u16 off) — Clip 8 @+0x20: cnt=1, Frame-u32 @+0x368 = key **165**; Clip 10 @+0x28: cnt=1, @+0x3cc = key **190**; Clip 12 @+0x30: cnt=1, @+0x430 = key **215**. Recoils 7/9/11: 23/24/24 Frames, Keys 142/166/191. `PL00W01.EDD` analog (Holds Keys 166/186/206, Recoils 20f).
- `PL00W03.EMR` (Header: frames@+0x8, 15 Bones, Keyframe 80 B, 12-bit-Winkel gepackt ab kf+12): Die drei Hold-Keyframes unterscheiden sich in **praktisch allen 15 Bones** — Ganzkörper-Neigung, z. B. Bone 0 (Root) x-Winkel: MITTEL 4095, HOCH 132, TIEF 3984 (≈ ±10–17°); Armkette Bones 9–14 mit großen Deltas (z. B. Bone 12 z: 3395/3164/3584). Gleiches Bild in W01.
- Nebenkonsument: die Muzzle-Flash-ESP-Routine @0x80018480–d4 liest `acaec` bit15/bit14 und positioniert sich je Stufe (0x17c/−0x6e/0x15 vs. 0x118/−0x32/0x18 vs. Default).
- Damage-Band: FUN_80011f50 (Decompile Z. 38): `aca54-Wort = acaec<<16 | …` = Trefferband-Gate (im Port vorhanden).

**Auflösung des stalen Port-Kommentars:** `player_common.c:114-115` („deferred aim-elevation pitch, states 0/1, +0x6a & 0x3e0") beschreibt nachweislich die **ACTION-8-KISTEN-SCHIEBEN-FSM**, nicht das Zielen: LAB_80035810 dispatcht `aca5a<8` über Tabelle @0x80010b68 (@0x80035828–40), und `andi v0,v0,0x3e0` @0x80035910 / @0x80035988 maskiert dort das **Yaw** `DAT_800acabe` (+0x6a) — Yaw-Snap fürs Schieben. Der „fehlende Pitch" aus dem Kommentar existiert im Aim-Pfad nicht als eigenes Subsystem.

## 2) PORT-STAND

- **Elevation-Clip-Umschaltung EXISTIERT:** `re15_port/engine/src/player_common.c:727-735` (dpad → `s_aim_elev`, Clip 8/10/12), Fire-Recoil 7/9/11 `:255-262` (setzt frac=7 ✓), Reload `:231-240` (frac=7 ✓), LOWER `:641-648` (frac=7 ✓), Recoil-Ende→HOLD `:753-758`.
- **Render-Pfad korrekt verdrahtet:** `re15_port/platform/pc/main.c:5747-5760` (Aim-Override posiert W-Bank + `re15_player_aim_clip()`), Kf-Auflösung `engine/src/anim_select_common.c:64ff` (fc=1-Hold → slot 0 → Keys 165/190/215), Pose `engine/src/skeleton_common.c:179ff`.
- **Byte-true LÜCKEN gefunden:**
  - (a) Fehlender Crossfade-Seed `anim_frac=7` an vier HOLD-Entry-Übergängen: RAISE→HOLD `:724-725`, **Elevation-Switch `:731-735`**, Recoil-Ende→HOLD `:753-758`, Reload-Ende→HOLD `:776-784`. Original seedet +0x8f=7 bei JEDEM Sub1-Entry (@0x800331ac gun / @0x800350f0 melee) — der Port-Seed `:869-874` feuert nur beim Motion-WECHSEL auf AIM_W (einmal je Aim), nicht bei Clip-Wechseln innerhalb AIM_W. Folge: Neigung snappt in 1 Frame statt 8-Frame-Blend.
  - (b) Fehlender RAISE-Früh-Preempt (TIEF ab Frame ≥7 / HOCH ab Frame ≥8, @0x800330d0–16c).
  - (c) Stale Kommentare `:114-115` und `:614-616` behaupten „Pitch deferred" — Quelle des Audit-Items P3 (OPEN_POINTS_AUDIT.md:61-63 zitiert genau diese).

## 3) IMPLEMENTIERUNGS-PLAN

1. `player_common.c`: `p->anim_frac = 7;` an den vier Stellen ergänzen, jeweils mit Zitat `/* HOLD-Entry Blend @0x800331ac (gun) / @0x800350f0 (melee) */`. Kleinste, sicherste Änderung; Blend-Rate bleibt Default 0x200 (FUN_8001f3bc-Pfad, bereits byte-true).
2. RAISE-Preempt (nur Gun-FSM): im RAISE-Zweig (`:719ff`) vor dem Terminal-Check: `if (anim_frame>=7 && (pad&DOWN))` → READY, `s_aim_elev=-1`, Clip 12, frame=0, frac=7 (@0x800330d0–10c); `if (anim_frame>=8 && (pad&UP))` → elev=+1, Clip 10 (@0x8003311c–5c).
3. Kommentare `:114-115`/`:614-616` korrigieren (Verweis Box-Push-FSM, Belege oben).
4. Verifikation: (a) neuer ctest-Probe (Muster `re15-room-probe`): R1+UP/DOWN treiben, `s_aim_cur_clip`∈{10,12} UND gerenderten Kf via `RE15_ANIM_TRACE` prüfen (clip_idx 10/12, slot 0 → Keys 190/215) — Pose messen, nicht State (Lehre NPC-Crossfade); (b) visuell per Skill `re15-port-visual-verify` (gdigrab, echtes Fenster): R1 halten + UP/DOWN → Ganzkörper-Neigung mit ~8-Frame-Blend; (c) PSX-Gegenprobe per `re15-parity-verify` (gleiche Eingabesequenz, DuckStation).

## 4) OFFEN (ehrlich)

- **Ob „optisch keine Neigung" ein echter Live-Defekt ist, konnte hier nicht gemessen werden** (READ-ONLY, kein Build/Lauf). Statisch ist der Clip-Wechsel vollständig verdrahtet; das Audit-Item P3 ist wörtlich aus den stalen Code-Kommentaren abgeleitet. Nächster RE-Schritt: gdigrab-Lauf; falls wirklich keine Neigung → `RE15_ANIM_TRACE` prüfen, ob clip_idx 10/12 den Render erreicht (sonst Input-Pfad `input_pc.c:308-318` E+Pfeiltasten/Keyboard-Ghosting live messen — W/S als Alternative vorhanden).
- Melee-RAISE-Früh-Preempt nicht belegt (kein `0xbfff`-Treffer in 0x80034e70–0x80035130); die Treffer @0x800341f8/@0x80034248 liegen im Sub5-L1-Retarget-Bereich und sind nicht zu Ende disassembliert — vor einem Melee-/L1-Preempt-Port erst diese Pfade disassemblieren.
- Feld `0x800acae0` (+0x8c, `sh zero` bei jedem Entry @0x80032f94) nicht verfolgt (vermutlich Frac-/Step-Akkumulator); Port-Äquivalent ungeprüft.
- Muzzle-Flash-Elevations-Offsets (@0x80018498–a8/0x800184c0–d0): Port-Stand ungeprüft — separater Punkt, falls der Mündungsblitz bei HOCH/TIEF auf Mittel-Höhe bleibt.

Scratch-Parser (EDD/EMR-Zensus): `C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\e15d7177-43f5-413e-8f99-d5e5f369c973\scratchpad\edd_census.py`
