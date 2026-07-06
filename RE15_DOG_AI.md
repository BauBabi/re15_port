# RE1.5 Hund (DOG / Cerberus, Typ 0x20) — byte-true RE (ENUMERATE)

Der Hund = **Enemy-Typ 0x20** (Modell **EM020.EMD**, BioModels-bestätigt `{ "EM020.EMD", DOG }`). Ein
klassischer boden-basierter Combat-Gegner (Patrol → Chase → Sprung-Lunge → Biss). Diese Datei = die
ENUMERATE-Grundlage (Skill `re15-enemy-ai-re`); der Port ist ein Mehr-Wellen-Aufwand wie Zombie/Crow.

## Dispatch (byte-verifiziert, STAGE1.BIN)

- **EXE-Dispatch @0x80072bac[0x20] = 0x8010d7f8** (STAGE1-Overlay-Registrierung @0x8011e8d4:
  `lui/addiu v0=0x8010d7f8; sw v0, 0x80072c2c`). **Root-Handler = 0x8010d7f8**.
- Der Root dispatcht den STATE (+0x4) via Tabelle **@0x80120f74**, DANN läuft der geteilte
  Body/Kollisions-Tail (0x8002b498 contact-clear, 0x8002aec4/0x8002b544 body-push, 0x8003b0a4
  SCA-Wall-Clamp) — **dieselbe geteilte Infra wie Zombie/Crow** (~70%-Shared-These bestätigt;
  boden-basiert, teilt Steering/Kollision mit dem Zombie).
- **State-Tabelle @0x80120f74 (12 Einträge, byte-verifiziert):**

| +0x4 | Handler | Rolle (klassifiziert — via Workflow) |
|---|---|---|
| 0 | 0x8010d93c | INIT |
| 1 | 0x8010dbcc | ACTIVE (Haupt-Brain: idle/patrol/chase/lunge-decide) |
| 2 | 0x801108f0 | (Angriff? Lunge/Biss) |
| 3 | 0x80110dc0 | (Angriff?) |
| 4,5,6 | 0x80111350 | (shared — HURT/Stagger?) |
| 7 | 0x80111774 | (?) |
| 8 | 0x8010ddb8 | (?) |
| 9 | 0x8010df94 | (?) |
| 10 | 0x8010e0c4 | (DEATH?) |
| 11 | 0x8010e568 | (CORPSE?) |

## Modell (verifiziert)

- **EM020.EMD** liegt in **CDEMD0.EMS @Index 7** (Offset 0x152800, 166912 B), valides EMD, **28 Clips**.
- **Clip-Frame-Counts (verbatim, für's byte-true Einbetten wie beim Crow):**
  `{49,18,30,14,14,14,18,15,30,35,35,14,42,14,44,14,27,14,50,29,17,13,20,29,18,40,30,99}`.
- Lädt generisch via `pc_enemy_load(0x20)` (Typ 0x20 in `s_ems_order` Index 7).

## Port-Infra (Wave-1-bereit, analog Crow)

- Spawn via `op_sce_em_set` (Typ 0x20 aus der RDT-Enemy-Liste); Render generisch (NPC-Loop);
  AI-Tick-Einfügepunkt: `run_all`-Gate [enemy_ai_common.c:2457] (`else if (t==0x20)`, eigener Branch;
  der Hund ist boden-basiert → nutzt anders als der Crow die geteilte Wall-Clamp/Body-Push-Reihenfolge).

## Nächste Schritte (Wellen — Skill §8)

1. **CLUSTER** (Workflow wf_ccc60f69, läuft): je 1 Agent auf INIT+ACTIVE / Angriff (2/3/7) / HURT (4/5/6/8/9)
   / DEATH (10/11), byte-true Disasm-Zitate + adversariale Verifikation.
2. **Port Wave 1**: INIT + ACTIVE (idle/patrol/chase) — sichtbar: Hund spawnt + jagt den Spieler.
3. **Wave 2**: Lunge-Sprung + Biss (Damage).
4. **Wave 3**: Hurt + Death (Sturz/Corpse).
5. **Dynamik-Verify**: Savestate aus einem Hunde-Raum + `re15_enemy_state.py` mit NEUER Dog-Label-Map
   (@0x80120f74; die Zombie-Map @0x8011f7b4 passt NICHT).
