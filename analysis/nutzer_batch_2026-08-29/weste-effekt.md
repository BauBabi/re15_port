# R.P.D.-Weste: „Ein Zombie-Biss mehr" — Recon-Bericht (2026-08-29)

Nutzer-WUNSCH (bewusste Erweiterung, KEIN byte-true Fix): *„Die POLICE-Weste sollte Leons
Energie insoweit erhoehen, dass er einen Zombie-Biss mehr aushaelt."*

Alle Konstanten unten tragen ihre Original-Adresse. Nur-lesende Recon; keine Code-Edits.

---

## 1. Hat das Original eine Ruestungs-/Defense-Mechanik? — **NEIN** (belegt)

### 1.1 RE1.5: FUN_80012d60 (der vereinheitlichte Hit/Damage-Resolver) — vollstaendig gelesen

Quelle: `RE_15_Quellcode_V2/FUN_80012d60.c` (komplett) + `ghidra1_V2.txt` Disasm/Xrefs.

Der SPIELER-Zweig ist eine gerade Linie ohne jeden Equipment-/Item-/Multiplikator-Check:

```
80012e24-e30   if (DAT_800acae7 & 1) -> skip        ; Ein-Treffer-Gate pro Attack-Fenster
80012e44-e64   DAT_800acaee -= *(s16*)(0x8006f418 + (attack_type&0xff)*2)
               ; Decompile Z.38: DAT_800acaee = DAT_800acaee - *(short*)(&DAT_8006f418 + (param_3&0xff)*2)
               ; Xrefs bestaetigt: DAT_8006f418 @80012e54(*), DAT_800acaee @80012e44(R)/80012e64(W)
80012e70       jal FUN_800453d0 (SE #10)             ; nur type<2
80012e78/e80   2x jal FUN_8001af20 -> beide ungerade -> DAT_800acaec |= 2 (Bleed/Poison)
80012ebc       DAT_800aca58 = 2 (HURT)
80012ec8-ed4   DAT_800aca59 = FUN_8001a7a8(...) + 2  ; Front/Back-Anim
80012eec       DAT_800acae7 |= 1
80012ee8-efc   if (DAT_800acaee < 0) state=3 (DEATH) ; signed, `bgez` — HP==0 lebt
```

Der EINZIGE Daten-Eingang in die Schadenshoehe ist `attack_type` → Tabelle `@0x8006f418`.
Kein Lesen von Inventar, Flags, work_vars, Modell-Byte in diesem Zweig.

**Caller-Zensus (EXE-weit, ghidra1_V2.txt:77605-77606, XREF[2]):**
- `@0x80018008` `jal FUN_80012d60` in **FUN_80017fa4** — Zombie-**Lunge** des (schlafenden)
  EXE-Zombie-Hirns, `FUN_80012d60(500, &pos, 0)` → **attack_type 0 = 10 Schaden**.
- `@0x800185b8` `jal FUN_80012d60` in **LAB_8001854c** (erreicht wenn `lhu v1,0x1e(a1)` == 7,
  `@0x80018560/70`), `ori a2,zero,0x2` `@0x800185b4` → **attack_type 2 = 1000 = Instakill-Klasse**
  (Devour-artiger Handler; Label = HYPOTHESE, Instruktionen zitiert).
  Beide Handler stehen in derselben Zeigertabelle `@0x80071da4-…` (`80071da4 addr FUN_80017fa4`,
  `80071dbc addr LAB_8001854c` — ghidra1_V2.txt:233502-233508).

**Die LIVE-Zombie-Schaeden laufen gar nicht ueber den Resolver**, sondern als direkte
HP-Schreiber im STAGE1-Overlay (siehe §2.2) — auch dort keinerlei Equipment-Check
(die Grab-Maschine FUN_80102548 dispatcht nur entity+0x6 ueber `@0x80100024`).

**Die Weste selbst:** Skript-Kette ROOM1190 sub15 setzt `Flag(3,0x75)` + `work_vars[0x10]`
(SCD-Bytes `21 03 75 00 / 22 03 75 01 / 24 10 01 00`, ROOM1190.RDT @0x2DA6). Die EXE liest
davon GENAU EINS: `work_vars[0x10]` = `DAT_800b0ff0` als **Modell-Index** im Raumlader
FUN_800396fc (`@0x80039768 lh v1,DAT_800b0ff0`, `@0x80039770 beq` → bei Abweichung
`@0x80039788 jal FUN_800314b0` = Spielermodell PL0n.PLD neu laden, Tabelle `@0x80073f70`).
Das Flag-Wort (Bank 3, Bit 0x75 → 0x800b1004) hat **keine einzige Code-Referenz im
Ghidra-Dump** (Zensus in Commit 2ef34524: 14 Fundstellen, alle im Skript). Der Bit-0x4-Test
im Waffen-Schadenspfad `@0x80012088 andi v1,v1,0x4` trifft die Modell-Nibbles 4..7
(zweite Figur), nicht die Ruestung (Modell 1).

→ **Die Weste hat im Original NULL Wirkung auf den Schaden.** Sie ist rein kosmetisch/Skript.

### 1.2 ABER: ein origineller Nebeneffekt — Anlegen = Vollheilung

FUN_800314b0 (der komplette Spieler-Load, den der Modellwechsel ausloest) endet mit:

```
80031710  ori  v0,zero,0x64          ; 100
80031718  sh   v0,DAT_800acaee       ; HP := 100  (ghidra1_V2.txt:126394-126396)
```

Da der Raumlader FUN_800314b0 NUR bei geaenderter Modell-Nibble ruft (`@0x80039770 beq`),
ist der einzige reguläre In-Game-Ausloeser genau der Ruestungs-Toggle: **im Original setzt
An- oder Ablegen der Weste die HP auf 100 zurueck (Vollheilung).** Der Port tut das derzeit
NICHT (`pc_sync_player_model`, main.c:891-937, tauscht nur Mesh+Textur) → offene
Nutzer-Entscheidung, ob dieser Original-Nebeneffekt uebernommen werden soll (er wuerde den
+B-Bonus beim Anlegen ueberschreiben bzw. mit ihm kombiniert werden muessen).

### 1.3 RE2-Retail-Vergleich: Skalierer JA, Equipment NEIN

`RE2_Quellcode_V2/FUN_800401d4.c` (der RE2-Spieler-Schadensapplier, komplett gelesen):

```c
if ((((DAT_800cfb74 & 0x40) != 0) || (0x78 < (short)DAT_800cfd4e)) && (param_1 < 0x29))
    param_1 = param_1 + (param_1 >> 1);          // +50% wenn Flag 0x40 ODER HP > 120, dmg < 41
if (DAT_800d482a == 3) {
    if (param_1 < 0x1e)      param_1 = param_1 * 5;
    else if (param_1 < 0x3c) param_1 = param_1 << 1;
}
DAT_800cfd4e -= param_1;                          // HP (RE2: max 200)
```

RE2 skaliert also nach **Zustand** (Condition/HP>120 → ×1,5; DAT_800d482a==3 → ×5/×2;
Semantik von DAT_800d482a = HYPOTHESE: Modus/Szenario) — aber auch dort **kein
Equipment-/Item-Check**. Negativ-Befund beidseitig belegt.

---

## 2. Original-Konstanten

### 2.1 Max-HP / Init / Heilung / Schwellen

| Groesse | Wert | Beleg |
|---|---|---|
| HP-Global | `DAT_800acaee` (s16) | Xref-Liste ghidra1_V2.txt:474973-474985 |
| Init/„Max" | **100** | `@0x80031710 ori v0,0x64` + `@0x80031718 sh v0,DAT_800acaee` (FUN_800314b0, Spieler-Load) |
| Todes-Schwelle | **HP < 0** (signed; HP==0 lebt) | `@0x80012ee8` bgez → state 3; Grab-Maschine identisch |
| Heilung additiv | **UNgeclampt** (HP > 100 moeglich!) | `@0x8004afa8-b0` addu+sh ohne Clamp; Tabelle `@0x80010fbc` (Green +25 = 100>>2 `@0x8004af78`) |
| Heilung absolut | 100 (`sh s0`, s0=100 `@0x8004ae14`); „NUT" 0x2f → 77 `@0x8004b028` | item_use_common.c:18-65 |
| Bleed/Poison-Drain | **−2**, Keep-Alive-Floor **1** | `@0x80031e9c lhu` / `@0x80031ea4 addiu -2` / `@0x80031eac sh`; `bgez @0x80031eb4` sonst `ori v0,1 @0x80031ec0` + `sh @0x80031ec8` — Gift toetet nie |
| Status-Screen-Condition | Fine ≥ **80**; Caution < **80** (0x50); Danger < **20** (0x14); Poison-Override (acaec&2 → 3) | FUN_8004ed6c `@0x8004ed6c` (Decompile Z.7-16); Port: re15_inv_screen.c:191 |
| Humpel-Idle | verletzt < **50** (slti 0x32), schlimmer < **30** (slti 0x1e) | switchD_8003206c case4/case9; HP-Reads `@0x80032190` / `@0x80032294`; Port: player_common.c:283-286 |

Merke: es gibt im Original **keinen HP-Ceiling-Clamp** — „Max-HP 100" ist nur der
Init-/Vollheilungs-Wert. Ein Bonus ueber 100 kollidiert mit keinem Original-Mechanismus.

### 2.2 Zombie-Biss-Schaden (alle Arten)

**Schadenstabelle des Resolvers `DAT_8006f418`** (11×s16, Bytes ghidra1_V2.txt:223455-223478,
`0a 00 | 14 00 | e8 03 | e8 03 | e8 03 | 32 00 | 64 00 | c8 00 | 2c 01 | e8 03 | 00 00`):
`{10, 20, 1000, 1000, 1000, 50, 100, 200, 300, 1000, 0}` — Typ 0 = Zombie-Lunge 10,
Typ 2 = Instakill 1000.

**Live-Pfad (STAGE1-Overlay, direkte HP-Schreiber; Port-Fundstellen enemy_ai_common.c):**

| Angriff | Schaden | Original-Adresse | Port-Zeile |
|---|---|---|---|
| **Zombie-Grab IMPACT** (Zupacken) | **−10** | `@0x8010277c` | :2070 |
| **Zombie-Grab BISS** (Loop im 0x6e-Escape-Fenster) | **−5 je Biss** | `@0x801027dc` | :2137 |
| Zombie-Girl-Grab-Biss | −5 | `@0x80103ddc-ec` | :3742 |
| Zombie-Girl Kau-Drain | −1/Tick | `@0x80103e38-4c` | :3750 |
| EXE-Lunge (schlafendes Hirn) | −10 (Typ 0) | Tabelle `@0x8006f418[0]`, Call `@0x80018008` | re15_damage.c:40 |
| EXE-„Devour"-Handler | −1000 (Typ 2) | Call `@0x800185b8` | — |
| Kraehe Sturzflug | −4 | `@0x80113b04` | :5444 |
| Hund | −5 / −10 | `@0x80111f60` / `@0x801122b4` | :10511/:10522 |
| Gorilla-Boss | −10 (z.T. Clamp auf 1: `@0x80117810/@0x80117c94`) | `@0x8010f2c4-d0`, `@0x801177f0`, `@0x80117c78` | :7388/:10767/:10807 |
| Maggot/Boss-Biss | −6 / −12 / −600 | `@0x80118460-6c` / `@0x801187b4-c0` / `@0x80119198-ac` | :8308/:8339/:8468 |
| Feuer-Overlap | −2/Tick | (Overlay, Port :7998) | :7998 |

**„Der Standard-Zombie-Biss" = −5** (`@0x801027dc`); ein kompletter minimaler Grab-Zyklus
= Impact −10 + n×(−5), Biss-Anzahl haengt am Mash/Escape-Fenster (+0x9c = 0x6e).

---

## 3. Port-Stand der Weste

**Repraesentation (v0.3.36, Commits 1913c2cc / 2ef34524 / 0151da23 / 5509860b):**
- Zustand = `g_scd.work_vars[0x10]` (∈{0,1} = Modell-Index PL00/PL01) **plus** `Flag(3,0x75)`
  (`re15_game_flag_get(3,0x75)`, re15_scd.h:401). Beides setzt ROOM1190 sub15.
- Modellwechsel: Engine-Rueckruf `re15_scd_set_player_model_sync` (re15_scd.h:571-572),
  gerufen am Ende von `scd_room_reenter` (scd_room_setup.c:261) = der Port-Analogpunkt zu
  FUN_800396fc `@0x80039760-88`; PC-Implementierung `pc_sync_player_model` (main.c:891-937,
  Registrierung main.c:2836). Getauscht werden nur Mesh + Textur-Slot 0; **HP wird — anders
  als im Original (`@0x80031718`) — nicht angefasst.**

**Persistenz-Matrix:**

| Traeger | Raumwechsel | Save/Load |
|---|---|---|
| `work_vars[0x10]` | JA — wird ueber den `memset` gerettet (scd_room_setup.c:149-164; Original wischt 0x800b0ff0 nicht, FUN_8003ecec `@0x8003ed60-94`) | **NEIN** — `re15_savedata_capture` (re15_savedata.c:67-98) serialisiert keine work_vars. Original schon: Save-memcpy a1=0x800b0fbc a2=0x1230 `@0x800261c4-d8` deckt 0x800b0ff0 (GSB+0x34) ab. → nach Load: Flag=1, Modell=PL00 = **bestehende Desync-Luecke** |
| `Flag(3,0x75)` | JA | **JA** — `g_game.flags` ↔ `savedata.flags[]` (re15_savedata.c:92/136) |
| `hp` | JA | JA — `savedata.player_hp` (re15_savedata.c:82/114) |

**Wo ein Schadens-Modifikator NICHT hingehoert:** `re15_player_take_damage`
(re15_damage.c:197) wird vom Live-Grab-Biss gar nicht durchlaufen — die Gegner-Module
schreiben `player->hp` direkt (explizit dokumentiert game_step_common.c:929; ~25
Schreibstellen allein in enemy_ai_common.c). Ein Modifikator dort wuerde den
Standard-Biss verfehlen; alle Schreibstellen einzeln zu gaten waere fehlertraechtig.

**→ Der saubere Mechanismus ist der vom Nutzer gewuenschte Max-HP-Bonus** (und das Original
hat keinen Ceiling-Clamp, der dem im Weg stuende).

### Vorgeschlagene Hook-Stellen (konkret)

1. **Praedikat**: `re15_player_vest_on()` = `re15_game_flag_get(3, 0x75)` — das Flag ist der
   einzige save-persistente Traeger (Tabelle oben).
2. **Anlege-/Ablege-Flanke**: in `scd_room_reenter` (scd_room_setup.c, direkt beim
   work_vars[0x10]-Rettungsblock :149-164 bzw. vor dem Sync-Aufruf :261): statisches
   `prev`-Byte vergleichen; Flanke 0→1: `hp += B`; 1→0: `hp -= B`, Floor 0 (nicht toeten).
   Das ist derselbe Ladeweg, auf dem das Original den Modellwechsel (und seine HP=100-
   Zuweisung `@0x80031718`) ausfuehrt — alle drei Port-Ladewege (Selbst-Tuer, Raumwechsel,
   Boot) laufen dort durch (0151da23).
3. **Vollheilungs-Ziel**: item_use_common.c:61 `pl->hp = 100` → `100 + (vest ? B : 0)`
   — sonst loescht ein First-Aid-Spray den Bonus still. (Additive Heals sind original
   ungeclampt und brauchen nichts.)
4. **Save/Load-Reparatur** (unabhaengig noetig): in `re15_savedata_restore`
   (re15_savedata.c:~136, nach dem flags-memcpy) `g_scd.work_vars[0x10] =
   re15_game_flag_get(3,0x75) ? 1 : 0` — behebt die bestehende Modell-Desync nach Load
   UND haelt Praedikat/Modell konsistent. (Alternative mit Original-Parity: work_vars
   komplett in die savedata aufnehmen, wie der Original-Blob es tut.)
5. **New Game**: actor_common.c:39 (`hp = 100`) bleibt — Start ohne Weste.

**Offene Nutzer-Entscheidung:** Original-Nebeneffekt „Anlegen = HP:=100" (`@0x80031718`)
uebernehmen? Wenn ja, waere die Flanke: `hp = 100 + (neu ? B : 0)` statt `hp ± B`.

---

## 4. Rechnung: „genau ein Biss mehr"

Todes-Regel: tot erst bei **HP < 0** (`@0x80012ee8` bgez; Grab-Maschine identisch).
Ueberlebbare Ereignisse der Groesse D bei Kapazitaet M+B: **k_max = floor((M+B)/D)**.
„Genau eins mehr": floor((M+B)/D) = floor(M/D) + 1.

Mit M = 100 (`@0x80031710/18`):

| D | Ereignis | ohne Bonus | Bedingung „+1" | mit **B = 5** |
|---|---|---|---|---|
| **5** | **Standard-Zombie-Biss** `@0x801027dc` | 20 (stirbt am 21.) | **B ∈ [5, 9]** | **21 (+1)** ✔ |
| 15 | minimaler Grab-Zyklus (Impact 10 + 1 Biss) | 6 | B ∈ [5, 19] | 7 (+1) ✔ |
| 10 | Grab-Impact `@0x8010277c` / EXE-Lunge Typ 0 | 10 | B ∈ [10, 19] | 10 (±0) |
| 4 | Kraehen-Sturzflug `@0x80113b04` | 25 | B ∈ [4, 7] | 26 (+1) |
| 1000 | Instakill-Klasse (Typ 2) | 0 | — | 0 (Weste hilft nie) |

**Empfehlung: B = +5 → HP 105 bei angelegter Weste.**
- Genau **ein Standard-Zombie-Biss mehr** (21 statt 20) UND genau ein minimaler
  Grab-Zyklus mehr (7 statt 6) — beide Lesarten des Wunsches erfuellt.
- Impact-/Lunge-Toleranz bleibt exakt unveraendert (kein verstecktes Doppel-Buff).
- Alle Anzeige-/Anim-Schwellen (80/50/30/20) liegen weit darunter → keine Interaktion;
  105 > 100 kollidiert mit nichts (Original heilt selbst ungeclampt ueber 100).
- Eine Schadens-REDUKTION kann das Ziel nicht treffen: 5→4 ergaebe 25 Bisse (+5statt+1),
  und sie muesste ~25 verstreute HP-Schreibstellen gaten.

Groessere B waeren kein „ein Biss mehr": B=10 → +2 Bisse und +1 Impact; B=25 → +5 Bisse.

---

## 5. Gelesene Quellen (Nachvollzug)

- `RE_15_Quellcode_V2/FUN_80012d60.c`, `FUN_80017fa4.c`, `FUN_800314b0.c`, `FUN_80031c44.c`,
  `FUN_8004ed6c.c`; `RE2_Quellcode_V2/FUN_800401d4.c` (alle vollstaendig).
- `ghidra1_V2.txt`: FUN_80012d60-Disasm+Xrefs (:77582-…, :84674, :85020-85064),
  DAT_8006f418-Bytes (:223453-223491), DAT_800acaee-Xrefs (:474973-474985),
  HP-Init (:126394-126396), Bleed-Drain (:127217-127238), Idle-Reads (:127426-127504),
  Handler-Tabelle @0x80071da4 (:233502-233512).
- Port: `re15_port/engine/src/re15_damage.c` (Tabelle re15_damage_table @0x8006F418 dort
  bereits korrekt zitiert, :39-52), `enemy_ai_common.c` (HP-Schreiber), `item_use_common.c`,
  `player_common.c` (:281-286), `re15_savedata.c`, `scd_room_setup.c` (:111-164, :261),
  `platform/pc/main.c` (:830-937, :2836), `re15_inv_screen.c` (:191).
- Commits 1913c2cc, 2ef34524, 0151da23, 5509860b (Weste-Kette v0.3.36).
- Overlays: `RE_15_Quellcode_Overlays/` enthaelt keine FUN_80012d60-Referenz (Zensus-Grep) —
  die Live-Schaeden sind die direkten HP-Writes oben.
