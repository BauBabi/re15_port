# Audit der offenen Punkte — Stand 2026-08-21 (nach v0.3.9)

Anlass: „Schaue dir die offenen Punkte an und analysiere sie."
Methode: Marker-Zensus über den EIGENEN Code (`engine/`, `include/`, `platform/pc/src/`),
danach Stichproben-Verifikation jedes großen Clusters gegen den tatsächlichen Code-Pfad.

⚠️ **Leitbefund: die Marker sind KEIN Backlog.** Von 903 Treffern liegen 622 in Fremdcode
(SDL, Vulkan-Header). Von den verbleibenden **281 im eigenen Code** beschreibt ein großer Teil
Zustände, die längst überholt sind. Das deckt sich mit dem früheren Befund
(Memory `reai-v2-deferred-backlog`: von 219 Markern ~81 stale). **Wer diese Liste als
To-do liest, arbeitet an Dingen, die fertig sind.**

---

## 1. STALE — Marker lügt, Sache ist erledigt (verifiziert)

| Marker | Behauptung | Wirklichkeit |
|---|---|---|
| `re15_damage.h:84/88/92`, `game_step_common.c:351` | „game-over screen … DEFERRED presentation (no port fade/game-over)" | **Vollständig implementiert**: Herzschlag-Puls-FSM (`s_go_sub` 0–6), YOU-DIED-Einflug über 50 Frames, Schluss-Fade, SEQ-Stopp und Übergang zum Titel (`main.c:4843-4889`). |
| `re15_collision.c:366` | „DEFERRED (the player walked through diagonal walls in ROOM11C0)" | Der Satz steht im **Präteritum**: die Diagonal-/Slope-Zellen sind byte-true portiert (`wf_b5520814`, Handler ab :374). |
| `enemy_ai_common.c:1403` | — | Sagt es selbst: „IMPLEMENTED (was cited DEFERRED)". |

**Konsequenz:** Vor jedem Aufgreifen eines Markers erst prüfen, ob der beschriebene Pfad
überhaupt noch läuft. Drei von drei Stichproben waren stale.

## 2. TOTER CODE — 34 Marker, die kein Spielverhalten beschreiben

`re15_enemy_ai_tick()` / `re15_enemy_ai_step()` (`enemy_ai_common.c:167/200`) sind der
**Frühphasen-Pfad**. Ein Aufruf-Zensus zeigt: sie werden **nur noch von `test_enemy_ai.c`**
aufgerufen, nirgends aus dem Spiel. Der Live-Pfad ist `re15_enemy_ai_live_tick`.

Beispiel für die Fallhöhe — dieselbe Datei, zwei Zeilen:
```
:187  case RE15_AI_STATE_HURT:  /* FUN_8011db40 — body not decoded, deferred */ break;   <- TOT
:4419 case RE15_AI_STATE_HURT:  re15_enemy_ai_live_hurt(slot);  /* [2] FUN_80105a8c */    <- LIVE
```
Betroffen: **14** Marker in `enemy_ai_common.c` (<:500) + **20** in `re15_enemy_ai.h`.
Diese 34 sind ersatzlos zu ignorieren (besser: Header-Kopf entrümpeln, damit sie nicht
weiter Aufmerksamkeit binden).

---

## 3. ECHT OFFEN UND SPÜRBAR — priorisiert

### P1 — Einziges Softlock-Risiko: der Krähen-Wurf
`game_step_common.c:78` — „Kraehen-Wurf (cmd-4 Mode 6) bleibt OFFEN (**Exit unbelegt —
Softlock-Gefahr**)". Die Krähe wirft den Spieler um, indem sie Mode 6 **direkt als Attacke
schreibt** (`@0x80115d04/10/18`: cmd 4, dir 6, Phase 0); Mode 6 ist laut
`analysis/player_knockdown.md` „Hinlegen/Halten (Clip 1→Loop 2)". Der Ausstieg aus diesem
Loop ist nicht belegt. Das ist der **einzige gefundene Punkt, der das Spiel anhalten kann**,
und er ist in einem Raum erreichbar, den der Spieler betritt (ROOM10C0).
⚠️ Zusatzverdacht, der mitgeprüft gehört: der Port-Kommentar `game_step_common.c:87` nennt
Mode 6 „EVENT-REACH", das Dossier „Hinlegen/Halten" — eine der beiden Zuordnungen ist falsch.

### P2 — Positionale Lautstärke/Panning fehlt game-weit
`audio_pc.c:1109` / `:947` — `FUN_8005bec0-f08` übernimmt `entity+56/60/64` (die Emitter-
Position) in den Voice-Record; der Port nutzt statt dessen die tone-eigene vol/pan. Wirkung:
**Gegnergeräusche klingen unabhängig von Entfernung und Richtung gleich.** Betrifft jeden
Raum und jeden Gegner, also permanent hörbar.

### P3 — Zielhöhe ohne sichtbare Armneigung
`player_common.c:596-602` — die drei Zielstufen (hoch/mittel/tief) **sind** implementiert,
nur der `aim-elevation pitch` fehlt. Mechanisch korrekt, optisch nicht.

### P4 — NPC-Dialogverhalten (Welle 2)
`enemy_ai_common.c:7733` / `:10857` — „walk/look/dialogue VM stays Wave-2 deferred".
Die sieben STAGE1-NPC-Typen laufen inzwischen alle über den geteilten Executor
(Routing byte-true); offen ist die Verhaltens-VM darüber.

### P5 — Gore-Modelltausch (RE2-Modus)
`enemy_ai_common.c:10589` — „solange der Port keinen Gore-Modell-Tausch hat, setzt niemand
das Bit"; dazu `enemy_ai_re2_zombie.c:2913` (Part-Benennung im Renderpfad). Abgetrennte
Gliedmaßen erzeugen keinen Stumpf-Mesh; Kinder abgetrennter Teile folgen nicht.

### P6 — Reste aus v0.3.9 (klein, benannt, mit Adressen)
- Blutlachen-**Basisgröße**: der Grower ist byte-true (+5/Tick, Budget 120, Farbe
  `0x00BFBF10`), die **Startausdehnung** setzt der RE2-EXE-Spawnpfad, den der Port nicht hat
  (Overlay-Vollscan: kein Schreiber). Port nimmt die zitierte Zombie-Schattenbasis 600/700.
- Klettern: Ansetz-Sound `FUN_80045630(0,7|4)` @0x80038080 · Landestaub `FUN_80019700`
  @0x80038794/@0x800387bc · `FUN_8002cfd4` (braucht die Spieler-Kollisionsbox +0x7c/+0x78).
- RE2-Hund: umkreist einen **völlig reglosen** Spieler endlos, weil der Sektor-Test ±256
  (@0x80104D74) enger ist als die Peilungsänderung beim Orbit. Original-Verhalten oder
  Nav-Lücke — ungeklärt, im Spiel bewegt sich der Spieler.
- RE2-Sabber (Kind 6) und Glas-Kinds (9/11) sind stumm; die Blut-Kinds spawnen ersatzweise
  RE1.5-Room-Bank-Blut, sind also **nicht** unsichtbar.
- `probe_texel_key_census` ist ohne `add_test` registriert (läuft nie mit).

---

## 4. INFRASTRUKTUR-BLOCKER — hier hängen jeweils mehrere Punkte dran

Diese fünf sind der Grund, warum viele Einzelmarker nicht schließbar sind. Wer einen davon
baut, schließt eine ganze Gruppe auf einmal:

1. **RE2-FX-System `0x8001BF10`** (Tabelle @0x801056AC) — Blut-/Effekt-Kinds im RE2-Modus.
2. **Model-Pool / Part-Scatter / Bone-Bend-Layer** — Gore, Part-Streuung, Modell-Lean.
3. **Fehlende Entity-Felder**: `+0x10C` (Leichen-Absacken, auch Ragdoll-P2 @0x80107784),
   `+0x9A`, der Treffer-Datensatz `+0x200` (Part + Winkel).
4. **Spieler-Kollisionsbox `+0x7c/+0x78`** — Aktor-gegen-Objekt-Auflösung (Klettern, Schieben).
5. **Pad-Rumble** — kein Subsystem; die Latch-Kette @0x80101E18-EB4 ist reines Rumble.

---

## 5. BEWUSST TOT — kein Handlungsbedarf (nachgeprüft, nicht vermutet)

Diese Punkte sind mit **keinem ausgelieferten Skript erreichbar**. Sie zu bauen hieße,
Verhalten zu erfinden, das im Original nie auftritt:

- **Spinne, Modi 1/2/3** — `enemy_ai_re2_spider.c:1979`: „⛔ WARUM NUR MODUS 0 —
  NACHGEPRUEFT 2026-08-19, KEIN OFFENER PUNKT."
- **Hund, Sub-16-Sprungvariante C** (@0x801030FC) — Eintritt offen, kein Produzent bekannt.
- **Krähe, Skript-Perch** (0x801034DC) — kein RE1.5-Spawn erreicht ihn.
- **Zombie-Spawnvariante `pc[18..19]!=0`** (`scd_vm.c:3216`) — in STAGE1 latent, kein Autor.
- **AOT-Kanten-Konstanten `DAT_80010c34/c38`** — inerte Nullen, keine Geometrie.

---

## 6. Empfohlene Reihenfolge

1. **P1 Krähen-Wurf** — Softlock schlägt alles andere, und die Mode-6-Zuordnung muss ohnehin
   geklärt werden (zwei widersprüchliche Angaben im eigenen Baum).
2. **P2 Positionales Audio** — größter Dauer-Effekt pro Aufwand, betrifft jede Spielminute.
3. **Infrastruktur-Blocker 1 (RE2-FX)** — schaltet mehrere RE2-Marker gleichzeitig frei.
4. **P3 Aim-Pitch**, dann **P6-Reste**.
5. **P4/P5** sind eigene Wellen (Verhaltens-VM bzw. Modell-Ebene), keine Nebenbei-Fixes.

**Hygiene, unabhängig davon:** die 34 Marker am toten Frühpfad entfernen und die drei als
stale belegten korrigieren. Sonst kostet dieselbe Analyse beim nächsten Mal wieder den
gleichen Aufwand.
