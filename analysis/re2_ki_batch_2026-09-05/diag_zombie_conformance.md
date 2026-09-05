# Diagnose SYMPTOM 6 — RE2-Zombie-Konformitäts-Sweep („nicht richtig perfekt RE2-mäßig")

Datum: 2026-09-05. Quellen: eigener Voll-Disasm `EMOVL10_S0.BIN`/`EMZ0.BIN` (13267 Instr.,
`scratchpad/emz0_full.asm`, `re2_disasm.py --bin EMZ0.BIN`), RE2-EXE-Disasm (`re2_disasm.py` ohne
--bin), `RE2_Quellcode_V2/FUN_8004a808.c` / `FUN_8004aab8.c` / `FUN_8004ab60.c`, Port
`re15_port/engine/src/enemy_ai_re2_zombie.c` (7512 Z.), `nav_zone_common.c`, `enemy_ai_common.c`.
Keine Datei geändert, kein Build.

## 0. Kurzbefund

Der Port ist WEIT über den Stand von `RE15_RE2_AI.md` hinaus (HURT-Zeilen-Dispatch, DEATH-Ketten,
CORPSE-12-Sub-Maschine, Kriecher, Gore/Dismember, Leichen-Tint sind inzwischen portiert — die
OFFEN-Listen des Dokuments sind zu großen Teilen veraltet). Die W2-Leiter, die HURT-Schwellen, das
Grab-Timing und das Walk-Turn-Gate habe ich per eigenem Disasm nachgeprüft: **byte-konform**.

Was dem Spieler trotzdem als „nicht RE2-mäßig" auffällt, sitzt fast vollständig in **zwei
Executor-Rümpfen, die der Port nur teilweise nachgebaut hat**:

1. **IDLE (EXEC[0] @0x801013F4) ist im Original eine 4-Phasen-Maschine mit WANDER-Modus** — der
   RE2-Zombie schlurft periodisch zu einer ZUFALLS-NAV-ZONE und weckt sich nach Ablauf eines
   Schutz-Timers bei Distanz < 7500 **mit Sichtlinie, ohne Blick-Kegel** selbst. Der Port spielt
   nur Idle-Clip + Moan → **Port-Zombies sind Statuen**, bis der Spieler von vorn auf < 5000 kommt.
2. **WALK (EXEC[1] @0x80101A40)**: dem Port fehlen vier Details des Gang-Rumpfs (Zufalls-Startbild,
   ±16-Yaw-Jitter je Gait-Zeile, der periodische Rate-16-Steuerpuls über +0x16A, die daran
   gekoppelte Moan-Taktung) plus — bekannt und bewusst — die drei Produzenten des
   Arme-hoch-Gangs EXEC[2].

Dazu zwei aufgelöste „OPEN"s: das Sicht-Bit `+0x154 & 0x800` hat einen **gefundenen Produzenten**
(der RE2-EXE-Navigator FUN_8004A808 schießt JEDEN Tick einen LOS-Ray), und der Kriecher-Umbau
`+0x21A & 0x10 → FUN_80107A78` ist im Port **erreichbar** (der Port setzt das Bit selbst in
Knockdown-P6), wird aber übersprungen — der Kommentar „ohne Produzenten unerreichbar"
(enemy_ai_re2_zombie.c:6115-6117) ist veraltet.

---

## 1. Klassifikation der OFFEN-Punkte aus RE15_RE2_AI.md (Stand Code HEUTE)

| Doc-OFFEN-Punkt | Stand im Code | sichtbar? |
|---|---|---|
| Navigator 0x8004A808 „Steuerziel=Spieler" | **VERALTET** — Port steuert seit dem Nav-Port über `re15_nav_update_steer` (nav_zone_common.c:204, RE1.5-Zwilling FUN_80039e7c; Aufruf enemy_ai_common.c:5267 VOR dem RE2-Brain). Zombies laufen um den 1140-Tisch herum. Rest-Lücke: der LOS-Nebeneffekt, s. Befund B2 | (b) bis auf B2 |
| `+0x16A`-Konsument „nicht identifiziert" | **VON MIR AUFGELÖST**: Walk-Puls-Block @0x80101C08-40 (s. B4c) — nicht portiert | (a) |
| `+0x14D`-Konsument „nicht identifiziert" | **AUFGELÖST**: (1) Zufalls-STARTBILD des Gang-Clips @0x80101B04-0C (s. B4a); (2) der %3-Zusatz-Advance-Block @0x80101D00-5C für +0x10E&0x80/+0x21A&0x8000 (s. B5) | (a) |
| EXEC[2]-Anrempeln-Entry OFFEN | **AUFGELÖST, bewusst NICHT scharf** — drei Produzenten im Gang selbst disassembliert (Code-Kommentar enemy_ai_re2_zombie.c:1210-1252), Scharfschalten kollabierte gemessen den Nahkampf (3523→0). S. B3 | (a) |
| EXEC[11]-Entry OFFEN | geschlossen (W5, Kampf-Eintritt @0x80104530 scharf) | erledigt |
| State-8-EINTRITT | weiter OPEN, Port-Stub löst nur auf (`re2z_state8`, Z. 6925-6929); kein Produzent im Port → toter Code | (b) |
| CORPSE-Beschuss-/Zell-Subs 1..11 | **portiert** (re2z_corpse Z. 6803-6919, 12 Zellen, Lachen-Grower, Zuck-Takt) | erledigt |
| Kriech-Konvertierung `+0x21A&0x10 → 0x80107A78` | OPEN, aber der Code-Kommentar „ohne Produzenten unerreichbar" ist **FALSCH geworden**: Knockdown-P6 setzt das Bit im Port selbst (Z. 1902). S. B6 | (a) |
| Leichen-Tint 0xBFBF10 | **portiert** (`crow_pool=1`, Z. 6814-6819) | erledigt |
| Wake-Produzent der Limpets (0xF01) | MAPPING auf RE1.5-Nähe-Gate, inkl. ROOM1140-Ausnahme Nibble 7/8 (Z. 2141-2216) — deklariert, verhaltensäquivalent zum RE1.5-Zwilling | (b) |
| Partner-Domino +0xD | MAPPING body-push, dokumentiert | (b) |
| Block A/C/K, B/J-Fenster, +0x1F4/+0x1D4, PL+0x8==15 | tote Zweige mit belegter Null (fill_gates Z. 521-533) — im Original in RE1.5-Raumdaten genauso stumm | (b) |
| Kriecher `+0x5 > 2` | Rückfall auf aufrechte Tabelle, im Original Datenwort-jalr = unmöglich; dokumentiert | (b) |
| SE 9 / HURT-Per-Sub-Bereich @0x80105168+ | Flinch-Pfad ist stumm wie das Original (beide Zweige `j 0x80105418`); Per-Sub-Gates (+0x1D0&0xC0) weiter OPEN | (b), leise |
| Rumble/Screenshake, Kill-Zähler 0x800D46C0 | kein Port-Kanal, dokumentiert | (b) |
| Walk-Kanten-Sturz 0xA03/0xB03 (@0x80101E64/6C) | keine Klippen-Geometrie in RE1.5-Räumen | (b) |

---

## 2. Eigen-Disasm-Stichproben (Auftrag Punkt 2) — Ergebnis je Kern-Verhalten

### (a) Walk-Gait + Turn-Gate @0x80101A40-0x80101DC0 — **4 Divergenzen** (Befund B4)
Turn-Gate selbst ist konform: `sltiu 0x1389` @0x80101bb4 → Rate 8 (@0x80101bec), sonst
`tbl[+0x16B]>>15` → ±8 (@0x80101bdc-e8), Nah-Zusatz `sltiu 0xbb8` @0x80101c94 → Rate 16
(@0x80101cac) == Port `re15_re2z_walk_turn` (Z. 394-410, Konstanten Z. 271-274). Der RUMPF
darum herum weicht ab — s. B4.

### (b) W2-Leiter @0x80101714 + RNG-Ziehungen — **konform**
Eigener Disasm @0x80101744-0x80101A1C deckungsgleich mit `re15_re2z_decide_walk` (Z. 564-643):
A-Frühausstieg `j 0x80101a1c` @0x801017e0; B `sltiu 0x7d0` @0x801017fc + Etagen-Test @0x80101814
+ ganzes `PL+0x1D3`-Byte @0x80101824; D `sltiu 0xdac` @0x8010185c, Kegel-1024-Ausschluss
@0x80101868, `cfbf6&0x15` @0x8010187c, **Draw @0x80101888**, `&3` @0x80101890; E `sltiu 0x9c4`
@0x801018a4, `&0x17` @0x801018c4, **Draw @0x801018d0**, `&1` @0x801018d8. D und E sequenziell
(@0x80101860 springt bei D-Miss auf 0x801018a4) — Port zieht exakt so (Z. 592-609).
Decision-dann-Executor im selben Tick: Port `re2z_active` (Z. 3140-3149) == `lbu +5` neu
@0x801011D0. Der verworfene 1300er-Arc (@0x80101788, v0 zerstört @0x80101790) ist im Port
korrekt weggelassen (Z. 450-452).

### (c) HURT-Flinch/Knockdown-Schwellen @0x8010503C-0x801050C8 — **konform**
`v1 = (+0x10E&0x40) ? ((+0x5!=1)?0x17:0) : 0` (@0x8010503c-58, xori/sltu/subu/andi-Trick);
Flinch nur `+0x6==0` (@0x80105064) und `+0x223 <= v1` (`lb`+`slt` @0x8010506c-78); Eligibility
`+0x222==1 || +0x5==1` (@0x80105080-98) → `0x501` @0x801050a4-ac + Re-Seed `16+(rand&0xf)`
@0x801050c0-c8, sonst nur Marke `sb 1,546` @0x80105164. == Port re2z_hurt Z. 6119-6137.
Zeilen-Semantik (+0x5 = Waffen-/Attacken-Id) ist im Port über `re2z_row_from_weapon`
(Z. 3418-3423) als begründetes Klassen-MAPPING gelöst; Dispatch-Tabelle 19×9 (Z. 3463-3487)
gegen den eigenen Dump `table 0x8010c940`.

### (d) Grab-Phasen-Timing P2/P3 @0x80102814-0x80102900 — **konform**
P2: Biss-Clip = grab+1 PLAIN (`addiu v1,1; sw` @0x80102830-34, Rate-Halbwort 0), Budget 148
(@0x80102828-2c), Phase 3 (@0x80102820-24). P3: Pose 0x80015cb8 (@0x80102844) + Advance
0x8002959c(256) (@0x80102858) + Mash 0x8001598c (@0x80102860) → Budget −= 2+5·mash
(@0x80102868-7c); `bgez` @0x80102884 überspringt NUR die Phase-4-Writes (beidseitig
`sb 4,6(s1)`/`sb 4,6(s3)` @0x8010288c-94 — auch der SPIELER bekommt +0x6=4); Biss-Gate
`+0x14D == pair[0]` @0x801028a0-ac → SE 3 @0x801028e8 + `FUN_800401d4(pair[1], s5&1)`
@0x801028f4-fc (a1 = Kriecher-Bit als Mode). == Port re2z_exec_grab P2/P3 (Z. 1452-1494).

### (e) Idle→Walk-Entscheid DECISION[0] @0x80101294 — **eine gemappte Lücke** (Befund B2)
Struktur/Schwellen/Draws konform (Wake `sltiu 0x1388`+Kegel-1024 @0x801012f0-1c → 257; Lunge 1
`0xbb8`/Kegel-800-außen/`cfbf6&0x15`/`rand&1` @0x80101320-78; Lunge 2 `0x7d0`/Kegel-1024-außen/
`&0x17`/`rand&1` @0x8010137c-d0 — Port Z. 1134-1146). ABER: **alle drei Blöcke tragen zusätzlich
`lhu v0,340(s0); andi 0x800`** (@0x80101308-14, @0x8010134c-58, @0x80101390-9c) = das
Sichtlinien-Bit, das der Port konstant auf 1 mappt. Produzent jetzt gefunden → B2.

---

## 3. Die Befunde (sichtbar für den Spieler)

### B1 — IDLE ist im Original eine WANDER-Maschine; der Port spielt eine Statue  **[NEU, HOCH]**

Eigener Disasm EXEC[0] @0x801013F4-0x801016C4 (voll in `scratchpad/emz0_full.asm`):

* P0 @0x80101458: Clip 0, Zufallsframe `(rand&0x1f)<<8`, `+0x158 = rand+150` (@0x80101480-88),
  `+0x15A = rand+300` (@0x8010148c-90). *(Port hat genau das, Z. 1172-1178.)*
* **P1 @0x80101494 — der Port bricht hier ab, das Original macht 4 Dinge:**
  1. `jal 0x8002959c(a3=256)` @0x801014a0 — Idle-Advance (Port: globaler Advancer, ok).
  2. `+0x15A`-Ablauf → Moan-Block (@0x801014bc-ec: `+0x239`-Gate, `rand&1` → SE 11, cd 150) +
     Re-Seed `rand+300` (@0x801014f0-fc). *(Port hat das, Z. 1180-1184 — aber er `return`t nach
     dem Dekrement und ÜBERSPRINGT damit alles Folgende.)*
  3. **WANDER-Wurf**: liest `+0x15A` NACH dem Dekrement (`lh v0,346` @0x8010151c); ist er GENAU 0
     (der Tick VOR dem Moan-Tick) → `rand&1 == 0` → **`+0x6 = 2`** (@0x8010152c-40) = Phase 2.
  4. **SELBST-WECKER**: `lh v0,344` @0x80101544; wenn `+0x158` (der 150-405-Tick-Schutz-Timer)
     abgelaufen ist: `dist < 0x1d4c` (**7500**) @0x8010155c UND `+0x154 & 0x800` (**Sichtlinie**)
     @0x80101568-74 → **`+0x4 = 0x101`** (@0x80101578-7c) — OHNE Blick-Kegel. Sonst Dekrement
     @0x80101590-a0.
* **P2 @0x801015a4 (WANDER-SETUP)**: `+0x6=3`, Clip = `+0x218`-GANG-Clip `0xF0000+walkclip`
  (@0x801015ac-bc), `+0x158 = rand+150` (@0x801015c0-c8), **`jal 0x8004aa50`** = ZUFALLS-NAV-ZONE
  (@0x801015c4; EXE-Disasm: `rand % blk_count`, RNG 0x80015fe8 — derselbe Zombie-RNG),
  Ergebnis → `+0x235` (@0x801015d0), **`+0x21A |= 0x400`** (@0x801015d4-d8) = der
  WP-Modus-Schalter des Navigators (Root übergibt `lbu a2,565` / `andi a3,0x21A,0x400`
  @0x80100350-58 an FUN_8004A808). Fällt nach P3 durch.
* **P3 @0x801015e0 (WANDER-GANG)**: Steer auf `+0x1C4/6` Rate **16** (@0x801015e8-ec),
  volles Bewegungs-Trio `0x80015e7c` + `0x8002959c(256)` + `0x800152c8` (@0x801015fc-20) —
  der Zombie LÄUFT wirklich. Ausgänge: `+0x158`-Ablauf → `+0x6=0` + `&= ~0x400`
  (@0x80101624-4c, zurück ins Idle); `dist < 0xbb8` → **0x101** + Clear (@0x80101650-70);
  `dist < 0x1d4c && +0x154&0x800` → **0x101** + Clear (@0x80101674-a8).

**Port (re2z_exec_stand, Z. 1170-1185): nur P0 + Moan.** Kein Wander, kein 7500er-LOS-Wecker,
kein `+0x158`-Dekrement. Sub 0 ist im Port hochgradig erreichbar (Grab-P9 committet Wort `0x1`,
Schnapp-Biss endet `+0x4=1`, Kriecher-HURT-P2 ebenso) — nach jedem Griff/Biss steht der Zombie
also da und tut NICHTS, bis der Spieler frontal auf <5000 kommt. In RE2 schlurft er nach
150-405 Ticks von selbst los (50 % Wander alle ~300-555 Ticks, Wake bei 7500 mit Sicht).
**Genau das „lebendige" RE2-Gefühl, das dem Nutzer fehlt.**

Repro: headless ROOM1140 `RE15_AI_FLAVOR=re2`, einen Zombie in Sub 0 bringen (Griff auslaufen
lassen), Spieler bei Distanz 5500-7000 HINTER dem Zombie parken → Port: ewige Statue; Original
(DuckStation RE2 bzw. Soll aus dem Disasm): Wake nach ≤ 405 Ticks.

Fix-Skizze: P1-Schwanz + P2/P3 in `re2z_exec_stand` nachziehen (~60 Zeilen). Zonen-Wurf über
den RE2-Draw-Strom (`re2z_rand() % nav_count`, NICHT `re15_nav_rand_zone` — die zieht aus dem
RE1.5-RNG); WP-Modus: `re15_nav_update_steer(e, …, e->re2z_wp235, flags21a&0x400)` — der
Port-Navigator hat den WP-Pfad schon (nav_zone_common.c:220-228). LOS s. B2.

### B2 — Das Sicht-Bit `+0x154 & 0x800` hat einen PRODUZENTEN: der Navigator schießt jeden Tick einen Ray  **[NEU, MITTEL-HOCH]**

`RE15_RE2_AI.md` §„Was RE2-KI NICHT ist" stellt fest, das OVERLAY rufe nie `0x80050858` — stimmt,
aber die EXE tut es FÜR den Zombie: `RE2_Quellcode_V2/FUN_8004a808.c:19-26`:

```c
if ((bVar1 & 0x80) == 0) {                     /* Repath-Timer +0x150 aktiv */
    *(ushort *)(param_1 + 0x154) &= 0xf7ff;    /* Bit 0x800 löschen */
    iVar3 = FUN_80050858(self_part_matrix, player_part_matrix, 0x2000, 1);
    if (iVar3 == 0) *(ushort *)(param_1 + 0x154) |= 0x800;   /* Sicht FREI */
}
```

Der Zombie-Root ruft FUN_8004A808 JEDEN Tick (`jal 0x8004a808` @0x80100354, a2=`+0x235`,
a3=`+0x21A&0x400`). Konsumenten im Overlay (eigener Scan `lhu …,340(s0)` + `andi 0x800`):
DECISION[0] alle drei Blöcke (@0x80101308/@0x8010134c/@0x80101390), Idle-Selbst-Wecker
(@0x80101568), Wander-Wecker (@0x80101688), Kriecher-Warte-Entscheid (@0x80103ae8).

**Port**: überall „gemappt 1" (enemy_ai_re2_zombie.c:1140-1145 ohne das Gate; Kommentar
Z. 3005-3007). Sichtbare Folge: Port-Zombies wachen auf und werfen Bewegungs-Lunges
(DECISION[0]-0xC01), obwohl zwischen ihnen und dem Spieler Kollisionsgeometrie liegt; das
RE2-Original bleibt dort stumm stehen. Zusammen mit B1 steuert dieses Bit das komplette
Idle-Erwachen.

Fix-Skizze: pro Tick (oder auf dem Repath-0-Tick) `re15_ray`-MAPPING wie bei der Krähe
(RE1.5 FUN_8003dcc4, dokumentiert als MAPPING in enemy_ai_re2_crow) in ein neues Feld
`re2z_los154` schreiben; die sechs Leser konsumieren es. Mode-0x8400/0x2000-Semantik des
RE2-Rays bleibt OPEN → deklariertes MAPPING, kein Byte-Beweis möglich ohne RE von 0x80050858.

### B3 — EXEC[2]-Produzenten (Arme-hoch-Gang) bewusst unscharf  **[BEKANNT, HOCH]**

Vollständig disassembliert im Code selbst (enemy_ai_re2_zombie.c:1210-1252): drei Produzenten
von `+0x4 = 0x201` IM GANG — (1) `+0x15A`-Timer (Seed `(rand>>3)+100` @0x80101B10-20; Ablauf
@0x80101D68-DBC: Re-Seed + 2. Wurf `&3` + `dist<0x1388` → 0x201), (2) Nahbereich
(@0x80101EC4-F10: `dist<0x7d0` + Kegel 512 + Spieler unbeansprucht), (3) Spieler rennt
(@0x80101F14-F60: `cfbf6&0x4` + `dist<0x9c4` + Kegel 1324). Der EXECUTOR selbst ist portiert
und läuft (re2z_exec_bump, Z. 1321-1382, inkl. ±16-Weave und Wurzelbewegung — der
„läuft auf der Stelle"-Bug ist gefixt). Das Scharfschalten kollabierte gemessen den
Nahkampf-Pin (3523→0 Treffer bei (1)+(2) zusammen; einzeln 3756/3589) — Mechanismus unklar,
deshalb byte-belegt, aber nicht drin. Sichtbar: **Port-Zombies wechseln beim Anmarsch nie in
den Arme-hoch-Gang** (nur nach Treffer-Erholungs-Leitern). Das ist eine der auffälligsten
RE2-Silhouetten.

Fix-Weg (steht schon im Code, Z. 1249-1251): den Kollaps aufklären — Verdacht: EXEC[2] steuert
nur ±16 ohne die Nah-Rate 8+16 des Gangs; DECISION[2] (Z. 3092-3134) hat als einzigen
Lunge-Block `dist<0xbb8 && arc512 && &0x17 && rand&1`, und der Grab-Block G verlangt Kegel
256 — wenn der Anmarsch in EXEC[2] endet, ohne dass der Zombie je wieder in EXEC[1]
zurückfällt (Rückweg `+0x158<120` @0x801025C0 erst nach 60+ Bildern), fehlen dem Test schlicht
die Grab-Fenster. Erst messen (Trace der +0x5-Verweildauern), dann alle DREI Blöcke zusammen
scharf.

### B4 — Vier fehlende Details im WALK-Rumpf  **[NEU, MITTEL]**

Eigener Disasm @0x80101A74-0x80101DC0 gegen Port re2z_exec_walk/re2z_gait_tick/re2z_walk_moan:

**(a) Zufalls-Startbild:** P0 setzt `+0x14D = rand & 0x1f` (@0x80101b04-0c) zwischen zwei
`0x80015e7c`-Re-Ankern (@0x80101ae4/@0x80101b08) — jeder Zombie beginnt seinen Gang-Zyklus an
einem zufälligen Bild. Port: `re2z_clip(e, walkclip, 0, …)` (Z. 1195) = Frame 0 für ALLE.
Sichtbar: **mehrere Zombies marschieren im Gleichschritt** (ROOM1140: bis zu 5). EXEC[2]
hat den Zufallsframe übrigens korrekt (Z. 1342) — nur der Gang nicht.

**(b) ±16-Yaw-Jitter je Gait-Zeile:** beim Zeilenwechsel zieht das Original einen ZWEITEN Wurf
und setzt `yaw = yaw − 16 + ((rand&1)<<5)` (@0x80101b8c-a8: `jal rand` / `andi 0x1` / `sll 5` /
`lhu 118` / `addiu −16` / `addu` / `sh 118`). Port re2z_gait_tick (Z. 384-390): nur
Timer-Reload, kein Jitter, ein Wurf weniger. Sichtbar: das feine Torkeln fehlt; RNG-Strom
divergiert an jeder Zeilengrenze.

**(c) Der `+0x16A`-Puls (Doc-OFFEN „Konsument unbekannt" — hier ist er):** P0 seedet
`+0x16A = (rand&0x1f)+30` (@0x80101ad4-e8). Pro Tick @0x80101c08-14: Dekrement; NUR am
Ablauf-Tick (alle 30-45 Ticks bzw. Reload `(rand&0xf)+30` @0x80101c2c-40): **ein zusätzlicher
`0x80015558`-Steer mit Rate 16** (@0x80101c28/`jal` @0x80101c3c) **und erst DANN der
Moan-Block** @0x80101c44-88. Port: kein `+0x16A` im Gang, und `re2z_walk_moan` läuft JEDEN
Tick (Z. 1199, Funktion Z. 916-924). Folgen: (i) der Gang dreht im Port minimal träger
(der periodische 16er-Puls fehlt); (ii) **hörbar zu viele Moans** — Port ≈ 1 Moan pro
~166 Ticks (cd 150 + E[Geom(2/32·(33/32))]≈16), Original ≈ 1 pro ~750 Ticks (Würfe nur am
Puls-Tick) → **~4,5-fach zu häufig**; (iii) 1-2 RNG-Würfe JEDEN Tick statt alle ~37.

**(d) RNG-Draw-Bilanz des Gangs:** P0 original 5 Würfe (Zeile, Timer, +0x16A, +0x14D, +0x15A) —
Port 2 (Zeile, Timer). „Die Zahl der RNG-Ziehungen ist selbst Verhalten" (RE15_RE2_AI.md).

Fix-Skizze: (a) `e->anim_frame = rand&0x1f; e->root_prev_kf = -1` in Walk-P0 (Zwilling der
EXEC[2]-Zeilen 1342-1343); (b) 2. Wurf + Jitter in `re2z_gait_tick`; (c) `re2z_dir16a` (im
Gang unbenutzt) als Puls-Timer + Moan dorthin verschieben; (d) +0x15A-Seed steht schon im
B3-Block bereit.

### B5 — Der Fettzombie-Block (`+0x21A & 0x8000`) ist im Port erreichbar, aber leer  **[NEU, MITTEL]**

INIT @0x801008bc-d4: Typ 0x11 (fetter Zombie) bekommt HP 250 **und `+0x21A |= 0x8000`** —
der Port führt BEIDES (Z. 7039: `if (e->type == 0x11) e->re2z_flags21a |= 0x8000u`).
Aber der Gang-Block, den dieses Bit schaltet (@0x80101cd8-0x80101d5c: bei
`+0x10E&0x80 || +0x21A&0x8000` und `+0x14D % 3 == 2` → `jal 0x801016c8` (Frame-Flag-SE-Probe)
+ Steer Rate 8 + **`jal 0x8002a9c8`**), ist im Port ausgelassen mit der Begründung „setzt in
RE1.5-Räumen kein Datum" (Z. 1317-1320 bzw. EXEC[2]-Kommentar) — **widerlegt durch die eigene
INIT-Zeile 7039**. `0x8002a9c8` ist per EXE-Disasm ein nackter Frame-Advance
(`lbu 333; addiu 1; sb 333` + Wrap + `sw 376` Frame-Wort — @0x8002a9cc-aa1c): der Fettzombie
schaltet auf ~jedem 3. Bild EIN ZUSATZBILD weiter ≈ **4/3-fache Gang-Kadenz** plus extra
Steer-8. Im Port watschelt Brad im Standard-Takt. Sichtbar in ROOM1140 (der fette Zombie ist
Nutzer-Referenzobjekt früherer Reports). Zweiter Produzent `0x800cfb74 & 0x40` → 1/3-Zufall
(@0x801008d8-940) hat keinen Port-Produzenten (totes Global) — nur Typ 0x11 zählt.
Derselbe Block existiert im EXEC[2]-Rumpf (@0x801023F8-468), dort ebenfalls ausgelassen.

Fix-Skizze: in `re15_re2z_move_root`-Umgebung des Gangs (und EXEC[2]): wenn
`(re2z_f10e&0x80)||(re2z_flags21a&0x8000)` und `anim_frame % 3 == 2` → ein Extra-Advance
(Port-Zwilling von 0x8002a9c8, OHNE Blend), `re2z_frame_flag_se`-Re-Probe, Steer 8.

### B6 — Kriecher-Umbau beim Beschuss am Boden: erreichbar, aber übersprungen  **[VERALTETES OPEN, MITTEL]**

HURT-Wurzel @0x80105014-30 (eigener Disasm): `lhu v0,538(s1); andi 0x10; beq zero → weiter;
jal 0x80107a78; j 0x80105418` — ein Treffer, während der Boden-Marker `+0x21A&0x10` steht,
läuft in die Umbau-Maschine FUN_80107A78 (Phasen 0-3; P0 @0x80107aec-b40: FX 6096 Scale 300 an
Part 8, Rate 15, `+0x1D0&0x20`-Sonderweg, danach Zerleger-/Umbau-Kette bis in den Kriecher).
Der Marker wird gesetzt in Knockdown-P6 (@0x8010358C) und Feeding-Wake (@0x80103D00-10),
gelöscht in P7-Exit (@0x801036B8-BC) — **alles drei ist im Port scharf** (Z. 1902/2324, 1934).
Der Port-HURT überspringt den Umbau aber (Z. 6115-6117, „ohne Produzenten … unerreichbar" —
stale) und fällt in die Liege-Route `+0x21A&2 → 0x60501` (Z. 6141-6156). Sichtbar: in RE2
verwandelt Beschuss eines am Boden liegenden (umgeworfenen) Zombies ihn in den KRIECHER; im
Port zuckt er nur (0x60501-Schleife, P6 stempelt +0x16B=1 → Blut) und steht später wieder auf.
Da W5 den kompletten Kriecher-Brain schon liefert, fehlt NUR FUN_80107A78 (~280 Instr.,
inkl. Bein-Gore-Zeilen).

Repro: RE2-Flavor, Zombie per Schrotflinte umwerfen (Zeile 7 → Knockdown), während der
Bodenphase (P6/P7) erneut treffen → Port: `re15_re2z_last_hit_handler()`-Trace zeigt die
0x60501-Route; Original-Soll: Umbau in `+0x10E|=1`-Kriecher.

### B7 — Navigator: RE2 FUN_8004A808 vollständig charakterisiert (Auftrag Punkt 3)

* **Zone-aus-Position** FUN_8004AAB8 (Decompilat): Knoten-Tabelle `*(room+0x38)`, Stride 12,
  Zählwort vorneweg, Scan LETZTE→ERSTE Zelle, u16-Wrap-Halboffen-Test — **instruktionsgleiches
  Muster wie RE1.5 FUN_8003A0FC** (= Port `re15_nav_zone_from_pos`, nav_zone_common.c:65-76).
* **Pfadsuche** FUN_8004AB60: DFS mit 16er-Stacks (`local_238[16]`-Trios, best=0x7fffffff) —
  Strukturzwilling von RE1.5 FUN_8003A524 (= Port `re15_nav_pathfind`).
* **Zufallszone** FUN_8004AA50 (eigener EXE-Disasm @0x8004aa50-aab4): `rand(0x80015fe8) %
  blk_count` — Zwilling von RE1.5 FUN_8003A07C, ABER auf dem RE2-RNG-Strom.
* **Der Treiber** FUN_8004A808: Repath-Timer `+0x150` (low7==0→|=7, immer −1, bit7 = Sperre) ==
  RE1.5 `+0x91`; `+0x10E&0x1000` → Beeline-Override (steer=roh); `+0x10E&0x4000` (Limpet) →
  kein Steer; WP-Modus (a3): Ziel = Boxmitte des Knotens `a2`; gleiche Zone → steer=Ziel jeden
  Tick; Fremdzone → DFS nur auf dem Timer-0-Tick. **Einziger substanzieller Zusatz ggü. RE1.5:
  der LOS-Ray in +0x154 (B2).**

**Bewertung:** die Doc-These „Steuerziel=Spieler → Zombies laufen in Ecken" trifft den heutigen
Port NICHT mehr — `re15_nav_update_steer` (RE1.5-Zwilling, byte-true auf RE1.5-BLK-Daten) läuft
VOR dem RE2-Brain (enemy_ai_common.c:5260-5270) und liefert Fremdzonen-Hops (ROOM1140:
um den Konferenztisch, RAM-verifiziert lt. nav_zone_common.c:24-27). Ein byte-true Port von
FUN_8004A808 wäre nur mit RE2-Zonen-Graphen möglich, die es für RE1.5-Räume nicht gibt — die
RE1.5-Substitution ist die korrekte Wahl und strukturell fast identisch. Verbleibende Deltas:
(i) der LOS-Nebeneffekt (B2); (ii) Wander-Zonen-Wurf muss aus dem RE2-RNG kommen, wenn B1
gebaut wird; (iii) RE2s Kostenfunktion/`>>3`-Skalierung nicht einzeln geprüft (gleiche Familie,
geringes Risiko, unsichtbar solange die Hop-Wahl gleich fällt).

---

## 4. Verifiziert KONFORM (Stichproben, eigene Zitate)

W2-Leiter komplett inkl. Draw-Zählung (§2b); DECISION[0]-Schwellen/Kegel/Draws (§2e);
HURT-Schwellen/Eligibility/Re-Seed (§2c); Grab P0-P9 inkl. Biss-Kadenz, Budget-Fallthrough,
beidseitigem +0x6=4, Kriecher-Kopf-Abriss-Frame 22 (§2d, Z. 1395-1600); Turn-Gate-Konstanten
5001/3000/8/±8/16; Gait-Tabelle/Zeilen-Wrap/Init-vs-Reload-Maske (voll/0x1f); Kriecher-Trio
(DECIDE/EXEC 0-2, Z. 2895-3068 gegen @0x80102EE4/@0x80103024/@0x80103A70/@0x80103B48);
CORPSE-12-Zellen-Maschine inkl. 2/3-Zweig und Grower-Raten (Z. 6803-6919); DEATH-Zweige und
19×9-Tabelle; Root-Prolog-Cooldowns (Z. 7153-7167 gegen @0x8010045C-98); Decision→Executor im
selben Tick; Ein-Angreifer-Riegel-Lebenszyklus.

## 5. Priorisierung (Top 5 nach Sichtbarkeit, mit Aufwand)

| # | Befund | Sichtbarkeit | Aufwand |
|---|---|---|---|
| 1 | **B1 Idle-Wander + Selbst-Wecker** — Zombies sind nach Griffen/Bissen Statuen statt herumzuschlurfen | sehr hoch (Dauerzustand in jedem Raum) | mittel: ~60 Z. in re2z_exec_stand, Nav-WP-Pfad existiert; plus B2-LOS als Gate (sonst Wake bei 7500 durch Wände) |
| 2 | **B3 EXEC[2]-Produzenten** — der Arme-hoch-Gang kommt beim Anmarsch nie | hoch (RE2-Silhouette) | mittel-hoch: erst den gemessenen Nahkampf-Kollaps aufklären (Verweildauer-Trace), dann 3 Blöcke scharf — Code liegt fertig zitiert im Kommentar |
| 3 | **B4 Gang-Details** (Gleichschritt-Start, ±16-Jitter, +0x16A-Puls, Moan-Takt ~4,5× zu oft) | mittel-hoch (hörbar + Gleichschritt bei 5 Zombies) | klein: 4 lokale Ergänzungen, alle Adressen liegen vor |
| 4 | **B6 Kriecher-Umbau FUN_80107A78** — Beschuss am Boden macht im Original einen Kriecher | mittel (klassisches RE2-Verhalten, aktiv provozierbar) | mittel: ~280 Instr. RE + Port, Kriecher-Brain existiert; Gate ist 5 Zeilen |
| 5 | **B5 Fettzombie-4/3-Kadenz** (+0x21A&0x8000-Block) | mittel (Typ 0x11 in ROOM1140, Referenzgegner des Nutzers) | klein: Extra-Advance-Zwilling von 0x8002a9c8 + Steer 8 + SE-Re-Probe an 2 Stellen |

(B2 als eigener Punkt ist Voraussetzung/Teil von #1 und macht zusätzlich DECISION[0] byte-näher.)

## 6. Doku-Hygiene

`RE15_RE2_AI.md` ist gegenüber dem Code deutlich veraltet (Navigator, HURT-Zeilen, CORPSE,
Leichen-Tint, EXEC[2]-Entry sind längst gelöst/portiert) — die OFFEN-Listen dort sollten beim
nächsten Doc-Pass gegen §1 dieser Diagnose abgeglichen werden, und der stale Kommentar
enemy_ai_re2_zombie.c:6115-6117 („+0x21A Bit 0x10 ohne Produzenten") gehört korrigiert
(Produzent: dieselbe Datei, Z. 1902/2324).
