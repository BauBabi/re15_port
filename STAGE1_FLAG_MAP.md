# STAGE1 Progression-Flag Map (RE1.5, byte-true)

Roadmap **S1-4 / PROG-3..5** foundation. This is the *actual* RE1.5 STAGE1 flag layout,
censused from the byte-true VM — **not** the RE2-retail flag ids the roadmap task text
originally guessed (see "RE2-retail premise" below).

## How this was built

`integration_flag_census` (`re15_port/tests/integration/test_flag_census.c`) loads every
STAGE1 Leon RDT, runs `main00`+`sub00` (installs the room's Door/event AOTs) **and** fires
each `sub_scd` event, so both sides execute:

- **READ** = `Ck` (0x21, `LAB_8003fcf4`) / `0x58` — the predicate a door/event **gates on**.
- **WRITE** = `Set` (0x22, `LAB_8003fdd0`) / `0x59` (`LAB_8003fe90`) — where a flag is **set/cleared**.

Regenerate the full dump with:

```bash
RE15_FLAG_CENSUS=1 ./re15_port/build/tests/integration/test_flag_census.exe 2>&1 | grep flagcensus
```

The store the ops read/write is `g_game.flags[zone][8]` (256 bits/zone, MSB-first), byte-identical
to the original `FUN_8004efe4` (check) / `FUN_8004ef90` (set) — verified + pinned by `unit_flag_gate`
(PROG-2). The `Ck`→door/event gate itself is pinned by `integration_door_lock` (PROG-1).

## Zone model (observed READ histogram: z5 1250, z3 111, z4 88, z12 18, z9 9, z7 6)

| Zone | Role | Evidence |
|------|------|----------|
| **z5** | Per-room **event / working** flags (idx 0..~35). Each room reuses the low bits for its own local script state (e.g. ROOM1080/10D0/1100 `Ck(z5,0..N)` sequences; ROOM11A0 alternating `Ck(z5,even) exp0 / Ck(z5,odd) exp1`). Often set **and** cleared within one event sweep → transient, not persistent progression. | 1250 reads, most rooms |
| **z3** | **Cross-room story progression** (idx ~94..210). One room `Set`s it, another `Ck`s it — e.g. ROOM1240 intro `Set(z3,139)` → ROOM11E0 `Ck(z3,139)`. | 111 reads |
| **z4** | **Enemy status / defeated latches** (high idx 195..247). `z4/idx243` is read in ~10 rooms (1040/10A0/10C0/1120/1160/1180/1190/11B0…) = a shared enemy/game-state gate; ROOM11C0 `Set(z4,64)` = the maggot room arming its own status. Matches `re15_em_status_zone()`. | 88 reads |
| **z12** | **Scenario / config gate.** `Ck(z12,31) exp0` in ~9 rooms (1040/1050/1090/1100/1110/1130/1140/1190/11A0) = a global "default path" branch; ROOM11B0 `Ck(z12,24) exp1`. | 18 reads |
| z9 / z7 | Misc (ROOM1000 `Ck(z9,134)`, ROOM11D0/1200 `Ck(z7,…)`). | 15 reads |

## Global entry / visited latches (PROG-5)

Set by **many** rooms' `main00` on entry (op=1 SET):

- **`Set(z1,27)`** — set by ROOM1240/11A0/1170/11C0/11B0/1090… on entry.
- **`Set(z2,7)`**  — set by the same rooms alongside z1/27.

These are the STAGE1 "you have progressed past here" / area-visited latches (the RE1.5 equivalent
of PROG-5's "visited-room" flags). ROOM1240 (the intro) additionally sets the `z3/139` story latch.
`integration_flag_census` pins the live write: after ROOM1240 init, `flag(3,139) == 1`.

## Notable per-room gates (readers)

| Room | Gate | Note |
|------|------|------|
| ROOM1240 | `Set(z1,27)`,`Set(z2,7)`,`Set(z3,139)` | intro progression latches (writers) |
| ROOM1030 | `Ck(z5,20/32/33/34) exp1`, `Ck(z3,116) exp1`, `Ck(z4,15)` | **the "Brad" room** — real gate is **z5 idx20/32/33/34**, *not* the roadmap's `0x11/0x12` |
| ROOM11C0 | `Ck(z5,0) exp0` (×31), `Set(z4,64)` | maggot room state machine |
| ROOM1130 | `Ck(z3,107)` | the roller-door switch flag (sub02 `Set(z3,107)`) |
| ROOM1170 | `Ck(z3,125/193)`, `Ck(z4,195/242)`, `Ck(z5,32/33)` | helipad intro gates |
| ROOM1190 | `Ck(z4,0..3) exp1`, `Ck(z4,234/243)`, `Ck(z3,111/117)` | dog room |

## The RE2-retail premise (why PROG-3/4 changed)

The roadmap's PROG-1/3/4 task text encodes **RE2-retail** progression, which does **not** match RE1.5:

- **PROG-1** "Door_aot_set lock/key operands": all 262 STAGE1 `Door_aot_set` records have `pc[25..31]==0`;
  the RE1.5 door handler `@0x800430bc` transitions unconditionally. The real lock is a **flag-gated
  sce1↔sce2 AOT swap** (see PROG-1 / `integration_door_lock`).
- **PROG-4** "Brad ROOM1030 (0x11/0x12)": the real ROOM1030 gate is `Ck(z5,20/32/33/34)`, not `0x11/0x12`.
  The named puzzles (red jewels, ladder button, Kendo) are **retail RE2 content** — RE1.5 STAGE1 is the
  RPD 1st-floor intro with a *different* flag layout (this table).

So PROG-3/4 must be derived from **this censused map**, not the RE2-retail ids. The plumbing that drives
every gate here (flag store + `Ck`/`Set`/`0x59`) is byte-true + verified (PROG-2).

## Status

- ✅ **Foundation**: flag store + `Ck`/`Set`/`0x58`/`0x59` byte-true (PROG-2); door lock = flag-gated
  sce swap (PROG-1); this censused map.
- ✅ **PROG-5** (visited/one-way latches): characterized — `z1/27` + `z2/7` global entry latches, `z3/139`
  cross-room intro latch (live-pinned by `integration_flag_census`).
- ⬜ **PROG-3** (key→door): needs the menu key-use → flag link (id classifier `@0x80049124`) tied to the
  gated doors above — the room SCD only reads the flag; the *key that sets it* lives in the inventory FSM.
- ⬜ **PROG-4** (per-room puzzle flags, live per-gate verification): the reader/writer map is here; each
  named gate still needs a live "set the writer → the gated door opens" check on real room data.
