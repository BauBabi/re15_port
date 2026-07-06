# RE1.5 Krähe (CROW, Typ 0x21) — byte-true RE-Grundlage

Die Krähe = **Enemy-Typ 0x21** (Modell **EM021.EMD**, bestätigt via BioModels-master `{ "EM021.EMD",
CROW }`). Ein distanz-reaktiver FLIEGER (Flug/Kreisen → Sturzflug-Angriff bei Nähe). Vorkommen
(RDT-Sce_em_set-Zensus): **STAGE1 ROOM10C0/10C1/1120/1121**. Diese Datei = die ENUMERATE-Grundlage
(Skill `re15-enemy-ai-re` §0/§1); der Port ist ein Mehr-Wellen-Aufwand wie der Zombie.

## Dispatch (byte-verifiziert, STAGE1.BIN, Disasm)

- **EXE-Dispatch @0x80072bac[0x21] = 0x80072c30** → registriert beim STAGE1-Overlay-Load
  (@0x8011e8e0-ec: `lui/addiu v0=0x80112020; sw v0, 0x80072c30`). **Root-Handler = 0x80112020**
  (eigener Handler, verschieden vom Zombie-0x80100424; Nachbar-Typen: 0x20→0x8010d7f8,
  0x26→0x80116288, 0x27→0x80116db8).
- **Root nutzt DIESELBE Entity-Struct + geteilte Infra wie der Zombie** (bestätigt die Skill-70%-These):
  liest +0x4 (state) / +0x9 (grid); ruft room_coll (0x8001c6e8), b498/aec4/b544 (Body-Push/Contact),
  b0a4 (SCA-Wall-Clamp), b064 (Schatten), rng (0x8001af20). Player-State-Gate vorab (aca58/aca5a, sltiu<3).
- **Root-STATE-Tabelle @0x8012111c** (indexiert per +0x4: `lbu 4(e); sll 2; +0x8012111c; jalr` @0x80112194-b4):

| State | Handler | Rolle (klassifiziert) |
|---|---|---|
| 0 | 0x8011224c | INIT — Sub-Mode-Dispatch (`sltiu <8`), schreibt +0x4=1 / +0x4=4; ruft 0x80115d94 |
| 1 | 0x80112420 | ACTIVE — Bewegung/Steering (ruft 0x80116068, 0x80115f70) |
| 2,5,6 | 0x80114e4c | (shared — HURT/Reaktion?) |
| 3 | 0x801146d0 | DEATH |
| 4 | 0x80114e54 | (nahe 0x80114e4c — Variante) |
| 7 | 0x801157e8 | (Spezial — Corpse/Fade?) |
| 8,9 | 0x80112628 | **DISTANZ-REAKTIV** — `lhu +0x1dc` vs `sltiu <0x1388 (5000)` / `<0x2710 (10000)` = Sturzflug/Angriff-Trigger nach Spieler-Distanz; ruft 0x80115d74 (Fly-Move?) mehrfach |

- **Krähen-Helfer** (die typ-spezifische Logik, noch zu RE'en): 0x80115d74 (meist-genutzt, Fly-Move?),
  0x80115d94, 0x80115f70, 0x80116068, 0x80116288.

## Offen (die Wellen — Skill §8 CLUSTER)

1. **State-Handler byte-true RE'en** je Cluster: INIT/Sub-Mode (0x8011224c, die <8-Sub-Tabelle finden),
   ACTIVE-Flug (0x80112420 + Helfer 0x80116068/0x80115f70), DISTANZ-DIVE (0x80112628, die 5000/10000-
   Schwellen + der Angriff), HURT (0x80114e4c), DEATH (0x801146d0), State 7 (0x801157e8).
2. **Die Fly-Move-Kernhelfer** (0x80115d74 etc.) — Krähen fliegen 3D (Y-Bewegung!), anders als der
   Boden-Zombie; der Steering/Height-Mechanismus ist das Neue.
3. **Modell EM021.EMD** aus dem CDEMD-Container extrahieren (CDEMD0/1.EMS, je ~4.7 MB) + rendern.
4. **Port**: neuer Typ 0x21 in re15_enemy_ai (aktuell 0x10/0x11/0x16-gated) + Fly-State-FSM + Modell.
5. **Dynamik-Verifikation** (Skill §3): Savestate aus ROOM10C0/1120 ziehen (JUMP hex, DuckStation),
   `re15_enemy_state.py` mit einer NEUEN Krähen-Label-Map (die Zombie-Map @0x8011f7b4 passt NICHT).

## Nächster konkreter Schritt

CLUSTER-Workflow (ENUMERATE ist erledigt): je 1 Agent auf INIT / ACTIVE-Flug / DISTANZ-DIVE / HURT+DEATH
/ Fly-Move-Helfer, jeder mit Disasm-Zitaten (kein Decompile-only) + adversarialer Verify. Danach Modell-
Extraktion + Port-Wave 1 (Spawn + Modell + Idle/Kreis-Flug sichtbar), dann Dive-Angriff, dann Hurt/Death.
