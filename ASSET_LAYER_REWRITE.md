# RE2-Style Streaming Asset Layer — Design & Roadmap

Status: **DESIGN** (2026-06-14). Driven by the user directive: *"eine saubere Architektur die
weitestgehend auf Streaming basiert (von CD), alle Charaktere, Hintergründe, Musik, Voiceover,
Gegner etc. abbilden kann, originalgetreu wie RE1.5, architektonisch nach RE2 (der Release-Version)."*

The reverse-engineered RE2/RE1.5 memory architecture this is based on: see the memory
`re_enemy_memory_architecture_re2_2026_06_14` (RE2 = one file directory + one CD reader
`FUN_80012fb8` + one shared staging buffer @0x80198000 + fixed resident work areas; textures
DMA to VRAM; lazy on-demand loads).

---

## 1. Why a rewrite (the problem with today's asset layer)

- **Fat incbin baseline (~1.66 MB `_end`).** Leon (`test.*` ~150 KB), Elliot (`elliot.*` ~80 KB),
  `room1170_*` (rbj 55 KB, light, msgs, obj), `room1100_bg*`, BGM/SFX/voice (~160 KB) are all
  `.incbin`'d into the EXE as PERMANENT BSS/.data — exactly what RE2 STREAMS from CD. This leaves
  only ~350 KB under the stack top, so a single enemy model barely fits (the 2026-06-14 crisis).
- **Scattered, duplicated loaders.** `asset_psx.c` (incbin + CD), `re15_cdfs` (CD by name),
  `re15_room.c` (RDT), `audio_psx.c`, `bg_psx.c`, the enemy loader — each with its own buffer.
  Several big transient buffers (`s_vlc_buf` 256 KB, `re15_cd_staging` 100 KB, the old enemy pool)
  that RE2 would funnel through ONE shared staging region.
- **No explicit RAM map.** Nobody accounts for the 2 MB budget in one place → overflows are found
  only by `_end` inspection after the fact (the boot-hang).

## 2. Target architecture (RE2 model)

### 2.1 `re15_res` — the unified resource manager (NEW, shared PSX+PC)
- **One load entry point:**
  - `int re15_res_load(const char *name, void *dst, int cap)` — load a named asset into `dst`
    (a resident work area). PSX = `re15_cdfs` CD read; PC = file read. Returns byte size or -1.
  - `unsigned char *re15_res_stage(const char *name, int *out_size)` — load into the SHARED staging
    buffer and return the pointer (for parse-then-discard or parse-then-copy-to-VRAM flows).
- **One shared staging buffer** (`re15_psx_staging_buf`, 0x42000 = 270 KB; already created
  2026-06-14b) for ALL transient loads: MDEC-BG VLC decode, EMD model load, TIM, VAB. They never
  run in the same instant (RE2's exact trick). `re15_cd_staging` collapses into this.
- **Textures always → VRAM** via the structured `re15_vram` allocator; the RAM staging copy is
  transient (RE2: only TPage/CLUT coords kept).

### 2.2 `re15_resmap.h` — the explicit RAM map (NEW)
One header that declares every resident work area + its byte budget, so the 2 MB is accounted in
ONE place (mirrors RE2's fixed `0x801xxxxx` work-area layout). Rough PSX budget (2 MB, NO heap —
all headroom under the stack top is the stack's):

| Work area            | ~Size  | Contents                                  | Lifetime          |
|----------------------|--------|-------------------------------------------|-------------------|
| code + libs (.text)  | ~145 KB| engine + PSn00bSDK                        | resident          |
| shared staging       | 270 KB | BG VLC / EMD / TIM / VAB transient loads  | reused per load   |
| room RDT             | ~370 KB| current room RDT (biggest = 369 KB)       | per room          |
| player model (Leon)  | ~150 KB| PL00 MD1+EDD+EMR (TIM→VRAM)               | resident (stream) |
| NPC model (Elliot…)  | ~80 KB | PL05 (TIM→VRAM)                            | per room (stream) |
| enemy model bank     | N×196KB| EM## model-only (TIM→VRAM)                | per room (stream) |
| fonts/shadow/sprite  | small  | resident                                  | resident          |
| sound (SPU)          | —      | VAB → 512 KB SPU RAM (separate from main) | per room (stream) |

The enemy slot count `N` is a function of how much main RAM the migration frees: today `N=1`;
after the incbin globals stream, `N=2` (RE2's count) becomes affordable.

### 2.3 Lazy, event-driven loads (RE2)
- RDT on room enter (`re15_room_load`) — already done.
- Enemy `EM##` on the `Sce_em_set` spawn event (move the trigger OUT of the render loop into
  `op_sce_em_set` via a per-port hook — RE2 loads at spawn, not at draw).
- Background `BSS` on cut-change — already done.
- Voice `XA`/`VAG` on `Message_on` — RE2 facade.
- BGM on room/stage change via the SS_BGMTBL table.

## 3. Staged migration (each stage: build all 3 green, measure `_end`, USER verifies behavior)

Keep the working engine runnable throughout; the proven subsystems (room1170 cinematic, stairs,
crows, multi-room→1140) are the regression baseline, NOT to be rewritten blindly.

- **Stage 1 — Unify staging. ❌ FIRST ATTEMPT REVERTED (2026-06-14).** Aliasing `re15_cd_staging`
  into `re15_psx_staging_buf` corrupted everything: the BG path loads the compressed BSS chunk into
  `re15_cd_staging` and DECODES it into `re15_psx_staging_buf` (src→dst) — merging them made the VLC
  decoder overwrite its own source + overflow into camera/actor globals ("camera stuck, models
  misplaced"). The two buffers stay SEPARATE (RE2 itself uses src 0x80190000 + dst 0x80198000). A
  future staging consolidation must respect **src ≠ dst for any single decode**; only loads that
  don't overlap in time may share a buffer. The −100 KB is deferred to a safe consolidation.
- **Stage 2 — `re15_res` skeleton.** Introduce the unified loader API wrapping `re15_cdfs`; point
  existing CD callers at it. No incbin removed yet. Pure refactor, behavior-identical.
- **Stage 3 — Stream the player model.** `test.* (PL00)` → CD-stream `\PLD\PL00.{MD1,EDD,EMR}` into
  a resident `re15_res` work area at boot; drop the `test_*` incbin (**−~150 KB**). Leon must look
  identical. (TIM already streams.)
- **Stage 4 — Stream Elliot.** `elliot.*` → CD; drop incbin (**−~80 KB**).
- **Stage 5 — Stream room1170 cinematic data.** rbj/light/msgs sliced from the RDT (Phase-0 RDT
  slicing already proves this works) or CD; drop the `room1170_*` incbin.
- **Stage 6 — Raise the enemy bank to RE2's 2 slots** with the freed RAM, and FIX the "still-Leon"
  load failure (the 2026-06-14c TTY diagnostics will show the cause — prime suspect: VRAM-tex
  exhaustion in non-1170 rooms; the fix is per-room VRAM budgeting in `re15_vram`).
- **Stage 7 — Voice/BGM streaming facade** (RE2 CD-XA + SsSeq) finalized for all rooms.
- **Stage 8 — Generalize for ALL rooms** (the globalization goal): every room's
  RDT/EMD/TIM/BSS/VAB flows through `re15_res`, no per-room special cases.

## 4. Invariants / rules
- Every stage builds **all 3** (PSX ISO + PC Debug + PC Release) and reports `_end` via
  `mipsel-none-elf-size build/re15_reborn.elf`; `_end` < `0x80200000` with ≥64 KB stack margin.
- PSX has **no heap** (no `InitHeap`/`malloc`) → never `malloc` on PSX; resident data lives in the
  named work areas; transient data in the shared staging.
- Textures go to VRAM, never kept in main-RAM resident.
- The author cannot run the PSX build here → each stage is USER-verified on DuckStation; TTY
  `printf` diagnostics (`[enemy] …`, etc.) are the eyes. Don't claim a stage works without it.
- PC is the parity reference (file-backed `re15_res`); PSX is CD-backed. Same interface.
- **src ≠ dst for any single decode/transform.** A buffer may be shared across loads that don't
  overlap IN TIME, but the read-source and write-dest of one decode (VLC→MDEC, unpack, copy) must be
  distinct memory. RE2 keeps MDEC source (0x80190000) and dest (0x80198000) separate for this. (The
  Stage-1 revert was caused by violating this.)
- **Diagnosis loop:** DuckStation settings.ini has `[BIOS] TTYLogging=true` + `[Logging]
  LogToFile=true` (+ `LogToWindow=true`); the agent reads `…/AppData/Local/DuckStation/duckstation.log`
  to see TTY `printf` + exceptions. Reproduce a bug in the right room before claiming a cause.
