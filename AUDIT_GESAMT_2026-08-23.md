# AUDIT GESAMT — reAi_v2 / re15_port (Stand 2026-08-23, Commit 38217ac7 / v0.3.16)

Synthese aus fünf read-only erhobenen Rohberichten: A Beleg-Disziplin-Scan, B EXE-Lücken-Triage (96 Fn.), C STAGE1-Overlay-Triage (136 Fn., Voll-Census), D OPEN-Marker-Refresh, E Gesamtfortschritt. Alle Aussagen sind als BELEGT / VERDACHT / NICHT GEPRÜFT getrennt; Quellen als Datei:Zeile bzw. `@0x…`.

---

## 1) Kernaussage

Der Leon-STAGE1-Kritikpfad ist end-to-end spielbar und tief byte-verifiziert (Boot-FMV→Title→Kampagne mit Save/Load, ctest 224/224 grün, 11703 unique Adress-Zitate, EXE 75,1 % / STAGE1 79,1 % Coverage), und die Beleg-Disziplin ist insgesamt hoch — von 532 gescannten Verhaltens-Konstanten-Zeilen bleiben nach Triage nur ~20-40 echte unbelegte Konstanten. Die Coverage-Lücken sind überwiegend Artefakte (Helfer bereits RE'ter Ketten, tot/ersetzt, Zitat-Granularität), aber das Audit fand reale Substanz: 5 belegte EXE-Lücken (SHITAI.TIM-Upload, SCD-RNG, SCD-Opcodes 0x45/0x5C, Aim-Reacquire), 4 unportierte per-Typ-NPC-Overlay-States (t42/t45/t49/t4b) sowie zwei Werkzeug-Defekte (STAGE1-Decompiles um +0x800 verschoben; Coverage zählt RE2-Zitate als STAGE1). Der Restaufwand (~45-55 % der Gesamtarbeit, ehrliche Schätzung) ist Breite statt Tiefe: 0/80 STAGE2-6-Räume und 0/40 Elza-RDTs verifiziert, W2-Sweep 0/6, dazu die Querschnitts-Lücken Positional-Audio, Gore-Modelltausch und das PSX-Target (linkt, aber 262 KB über RAM) — und der größte Hebel, das automatische Parity-Oracle, ist weiterhin nicht gebaut.

---

## 2) Beleg-Disziplin (Bericht A)

**Zahlen (BELEGT, Scanner reproduzierbar im Scratchpad `scan_constants.py`/`scan_constants2.py`):** 61 Dateien `engine/src/*.c` + `render_pc.c` + `audio_pc.c` (~59,5k Zeilen) gescannt. 532 Verhaltens-Konstanten-Zeilen erkannt → 106 ohne Evidenz in ±10 Zeilen → 36 ohne jede Adress-Referenz in ±25 Zeilen. Nach manueller Lektüre der Top ~30: ca. die Hälfte False Positives (Dead Code, Debug-Env, Clamps, Eigencontent-Audio, kanonische PSX-Formate). **Ehrliche Schätzung: ~20-40 echte unbelegte Verhaltens-Konstanten** im gescannten Bereich.

**Top-Funde (Schwere absteigend):**

| # | Fundort | Konstante(n) | Befund | Status |
|---|---|---|---|---|
| 1 | engine/src/game_step_common.c:307/317/337 | Death-Glow `-=4`, Floor 56, Start 255 | wörtlich „ramps down over ~50 ticks … live ~1.1s" — live-kalibriert, kein @0x; der restliche Death-Chain-Header (Z.34-52) ist voll zitiert | BELEGT unbelegt |
| 2 | platform/psx/main.c:757 | Letterbox `24` | PC-Pfad hat den 24/0-Toggle als „superseded (#21)" durch den byte-true FUN_80021a0c-Zähler ersetzt (pc/main.c:4389-4391); PSX-Target trägt noch die alte 24 → Target-Divergenz | BELEGT stale |
| 3 | platform/pc/src/audio_pc.c:1247-1249 | `>>1` Headroom, Cap 0x4000 | SCD-Volume nach Gehör getunt („klopfen"), als „Phase 4.6.3 demo" markiert, kein SPU-Beleg | BELEGT getunt |
| 4 | platform/pc/src/audio_pc.c:1769 | Pan-Deadzone 56..72 | Pan-Law ohne Zitat; Nachbar-Refs decken vol/pbmin/pbmax, nicht Pan | BELEGT unbelegt |
| 5 | engine/src/enemy_ai_common.c:5108-5121 | Krähen-DIVE-Zahlenblock (80/20/200/5/9/30/+2/−7/<600/3599) | kein per-Konstante-@0x; Nachbar-Hit-Teil dicht zitiert (@0x80113ad4ff) schirmt den Block im Scan ab; Dossier existiert → vermutlich RE'd, im Code unbelegt | VERDACHT (Doku-Lücke) |
| 6 | engine/src/scd_vm.c:3168-3177 | Spawn-Pose-Tabelle (sel→clip) | Header zitiert Decoder @0x80100c20, aber nur 3 von ~10 Mappings als „Proven" | VERDACHT |
| 7 | platform/pc/src/render_pc.c:2520/2523 | Font-Metrik 13/6 | im als „byte-true glyph replay" deklarierten Pfad ohne Zitat | VERDACHT |
| 8 | engine/src/stair_common.c:61 | `STAIR_YBOB 0x96` | einziges unzitiertes Define im sonst voll belegten Block | VERDACHT |
| 9 | engine/src/enemy_common.c:176-177 | Raum→RBJ-Tabelle {0x1150, 0x45, rec 1} | Mechanismus RE'd (Memory), Beleg im Code fehlt | VERDACHT |
| 10 | render_pc.c:74/1695 | „~17px measured" | stale Kommentare, führen in die Irre (PC nutzt längst den Zähler) | BELEGT stale |

Kleinkram (niedrig): item_icon_common.c:97 (Transparenz-Indizes), msg_common.c:123 (deklarierte Approximation Opcode 0x06), re15_memcard.c:73-83 + audio_pc.c:1692-1694 (kanonische PSX-Formate ohne psx-spx-Ref), anim_select_common.c:340-343 (HP-Schwellen-Ursprung NICHT GEPRÜFT).

**Dunkelziffer (ehrlich):** Der Scan wertet EIN Zitat im Fenster als Deckung für alle Nachbarn — Cluster wie #5 rutschen durch; 106/36 sind Untergrenze der Fenster-Betrachtung, Obergrenze echter Verstöße. NICHT GESCANNT: `platform/pc/main.c` (7881 Zeilen — Funde #2/#10 nur per Ziel-Grep), übrige `platform/pc/src/*_pc.c`, `platform/psx/`, Tabellen-Dateien. main.c ist der wahrscheinlichste weitere Fundort.

---

## 3) Lücken-Triage EXE / STAGE1 (Berichte B + C)

### 3.1 EXE — 96 unzitierte Funktionen (Anhang A), alle 96 klassifiziert

| Kategorie | Anzahl |
|---|---:|
| (a) tot / ersetzt (Card-HW-I/O, Overlay-CD-Loader, Packet-Builder, Thread-Infra) | 20 |
| (b) PsyQ-/Lib-/HW-Glue (CD, Audio, GPU/GTE, Syscalls) | 32 |
| (c) portiert-aber-unzitiert (Game-Over, Kisten, MSG, STR, OPTIONS, MAP, Init) | 30 |
| (d) ECHTE LÜCKE | **14** |

Befund: fast alle 96 haben zitierte Caller — es sind Helfer bereits RE'ter Subsysteme, keine unentdeckten Subsysteme. Ehrlichkeits-Hinweis zu (c): „Subsystem verifiziert" beweist NICHT die Byte-Treue des einzelnen Helfers; Detail-Gegenprobe steht u. a. für 80021eb4 (`0x…c00000` nirgends zitiert), 8003edbc, 8003da78, 8004d4c4 und das Kisten-Trio aus.

**(d) BELEGT fehlend im Port (5):**
1. **0x80039c2c — SHITAI.TIM-Upload beim Raumladen** (Leichen-Textur, unconditional aus FUN_800396fc @0x800399a8; `grep -ri SHITAI re15_port` = 0 Treffer). Slot-Leser NICHT GEPRÜFT.
2. **0x8003ea3c — SCD-RNG** (LFSR → work_vars[13] @DAT_800b0fea; läuft in INIT-VM, jedem Gameplay-Frame UND als Opcode 0x28). Port: nicht verdrahtet; Skripte mit 0x23-0x27-Prädikaten auf var13 verlieren Zufälligkeit. Betroffene Räume NICHT GEPRÜFT.
3. **0x80044fec — Opcode-0x5C-Helfer** (schreibt in Sound/BGM-Tabelle UNK_80074828); Port-Stub `op_sce_shake_on` (scd_vm.c:4113) ist No-op, RE2-Name irreführend. Nutzung NICHT GEPRÜFT.
4. **0x800396a8 — Opcode-0x45-Helfer** (DAT_800b2584-Tabelle); 0x45 im Port nicht verdrahtet. Nutzung NICHT GEPRÜFT.
5. **0x80037250 — zyklisches Aim-Target-Reacquire** (Post-Reload, scannt AB aktuellem Ziel); Port nutzt stattdessen Nächster-Scan (player_common.c:666-679) → bei ≥2 Gegnern abweichende Zielwahl.

**(d) VERDACHT (9 Funktionen, Wirkung nicht zu Ende geprüft):** 0x8001f1e0/0x8001f220 Gib-/Teile-Physik (passt zum offenen Dismember-Rest); 0x80020610/674-Umfeld/20894/209b8/209ec visueller Screen-Shake (Bit-Setter nicht gefunden, evtl. dormant); 0x8002441c Mesh-Farb-Lerp (temporale Licht-Glättung — Port evtl. instantan); 0x8002a050 Movie-Text mit Schatten-Smear (Erreichbarkeit im Preview unklar); 0x8003bc2c Stehen-auf-Objekten-Fallback im Band-Walker (Port scannt nur SCA; climb_common-Abdeckung NICHT GEPRÜFT).

### 3.2 STAGE1 — 136 unzitierte Funktionen, Voll-Census (keine Stichprobe)

**Zwei WERKZEUG-BEFUNDE vorab:**
- **BELEGT:** `RE_15_Quellcode_Overlays/STAGE1/FUN_*.c` und `STAGE1_overlay.c` sind um **+0x800 verschoben** (Datei `FUN_A.c` = Code von A+0x800; Doppelbeweis via test_1090_fire_pin.c:521ff und enemy_ai_common.c:8690). Nur `STAGE1_full/` ist korrekt adressiert.
- **VERDACHT (Mechanismus belegt, Ausmaß ungezählt):** die STAGE1-„zitiert"-Spalte enthält RE2-Overlay-Zitate (z. B. enemy_ai_re2_spider.c:1907-1912) → **79,1 % ist eine obere Schranke**.

**Zuordnung (alle 136 liegen in den Entity-Blöcken der Registrierung @0x8011e864 — keine einzige ist Raum-/Tür-/Event-Logik, die liegt im SCD der RDTs):**

| Kategorie | Anzahl | Inhalt |
|---|---:|---|
| (a) Helfer/Sliver bereits portierter Ketten | ~63 | Zombie/Writher/Hund/Krähe/Feuer/Gorilla; Positiv-Beweis: Krähen-Move-Rows 9/11 (FUN_80113384/801137fc) zeilengenau in enemy_ai_common.c:5067-5117, nur ohne Intervall-Zitat; 26 von 136 sind ≤16-Byte-Sliver |
| (a′) belegt TOT im Shipped-Game | ~29 | ZGirl Mode 1 (SCD-Census: 0 Spawns, enemy_ai_common.c:8491-8496), Gorilla-Subs 9-14 (:7771), NPC-HURT/DEATH (HP=−1-invulnerabel) |
| NPC-Trampoline auf zitierte EXE-Leaves | ~39 | Ziel-Tabellen (0x8004f100…0x80051cf8, 0x80050be8) portiert |
| (b) fremde Stage-Logik | 0 | Pointer-Census direkt über STAGE1.BIN |
| (c) ECHTE LÜCKE | **4 + 1** | s. u. |

**(c) Die echte STAGE1-Lücke — per-Typ-NPC-Overlay-States (vom Port selbst als deferred markiert, enemy_ai_common.c:7955-7956):** Der Port routet alle NPC-Typen durch die 0x47-Familie. Uncited und **nachweislich abweichend**: t4b-ACTIVE **FUN_8011e518** (Zusatz-Probe 0x800509e4(15000,…,0x514,0), Sub-Wahl via 0x8005070c, Übergang bei `+0x1c2&1 && DAT_800acae0 ≥ 0x65` — im t47-Original NICHT vorhanden); gleiche Klasse: t42-INIT 0x8011ccac, t45-ACTIVE 0x8011d460, t49-ACTIVE 0x8011df40. In keinem Port-/Audit-Dokument erwähnt (grep-verifiziert). NICHT GEPRÜFT: ob ein Shipped-Szenario diese NPCs je in Main-State 0/1 laufen lässt (Nachweisweg: Sce_em_set-Deskriptor-Census + Savestate `+0x4`). Dazu **1 Unbekannte**: FUN_80109ef8 (0x1d0 Bytes, Zombie-Block, in keinem korrekten Decompile) — Kandidat für `re15-psx-disasm`.

**Fazit Triage:** Anhang A/B messen primär Zitat-Granularität, nicht fehlendes Verhalten — mit 5 belegten EXE-Lücken, 9 EXE-Verdachten und der NPC-State-Lücke als realem Kern.

---

## 4) Offene Punkte priorisiert (Bericht D)

**Zensus (BELEGT):** 469 Marker-Zeilen in 30 Dateien (2026-08-21: 281 — Vergleich nur als Tendenz, Muster evtl. abweichend). Der Zuwachs ist KEIN neues Backlog: überwiegend geschlossene Fix-Doku-Blöcke der RE2-Welle. Die im Audit 2026-08-21 empfohlene Marker-Hygiene ist NICHT passiert (u. a. re15_damage.h:84, re15_collision.c:368 stehen stale).

**Status Alt-Prioritäten:** P1 Krähen-Wurf OFFEN, P2 Positional-Audio OFFEN, P3 Aim-Pitch OFFEN, P4 NPC-VM OFFEN, P5 Gore-Tausch OFFEN, P6 teilweise zu (Kletter-Mechanik repariert, Sound/Staub offen).

**Priorisierte Liste ECHT OFFEN + SPÜRBAR:**
1. **Positional-Audio game-weit** — audio_pc.c:945-947/1109-1110 (FUN_8005bec0-f08 / FUN_80045a64 @0x800451c0-cc), auch scd_vm.c:1636/1653. Jede Spielminute hörbar.
2. **RE2-Zombie EXEC[2] „Arme-oben-Gang"** — enemy_ai_re2_zombie.c:1103-1144: drei Produzenten voll disassembliert (@0x80101D68-DBC u. a.), aber bewusst nicht scharf (scharf = Nahkampf-Positivkontrolle 3523→0, Bisektion dokumentiert). Spürbar, weil RE2-KI seit e11962ec Default ist.
3. **Krähen-Wurf cmd-4/Mode-6** — game_step_common.c:79: der Port hat KEINEN Writer → Wurf fehlt komplett (Divergenz ROOM10C0); Softlock kann mangels Writer nicht feuern; Mode-6-Widerspruch ist aufgelöst (:88-98 belegt @0x800517f0), nur der Wurf-Exit unbelegt. Einziges bekanntes Softlock-Risiko.
4. **RE2-FX-Familie 0x8001bf10** — Blut-Kadenz (enemy_ai_common.c:1147-1157 @0x8010B5CC ff.), Fress-Tropf, Gore-Modelltausch (:10813), stumme Kinds (enemy_ai_re2_dog.c:53 u. a.).
5. **NPC-Footstep-SE** — enemy_ai_common.c:8272-8279, Blocker benannt (audio_pc.c:2448 maskiert &0x7f).
6. **Aim-Elevation-Pitch** — player_common.c:114-115 (mechanisch da, optisch fehlt).
7. **Messer-Reichweite** vom Spieler-Zentrum statt Klingenspitze — game_step_common.c:1150-1154.
8. **P6-Reste**: Kletter-Sound/Staub (climb_common.c:379/535), Hund-Orbit ±256 (enemy_ai_re2_dog.c:452-459), Blutlachen-Basis 600/700 (pc/main.c:6604), texel-census ohne add_test.
9. **NPC-Dialog-VM Welle 2** (enemy_ai_common.c:7956) — überschneidet sich mit der STAGE1-NPC-State-Lücke aus §3.2.

**Neu offen (HANDOVER §2, im Alt-Audit fehlend):** ROOM1090 „Leon verschwindet" (wartet auf Nutzer-Log), ROOM10F0 Band-5-Y-Divergenz, Hund-Opfer-FSM Fress-Kollaps-Anker, EXEC[11]-`+0x148`, Thread-Vergabe FUN_8003ee3c (3 folgenlose Abweichungen), Mixed-Raum ENEMSE-Bank ungeprüft. — Toter Code (verifiziert): `re15_enemy_ai_tick/_step`-Frühpfad nur von Tests gerufen; bewusst tot: Spinnen-Modi 1-3, pc[18..19], Hund-Sub-16-C.

---

## 5) Gesamtfortschritt je Track + Restaufwand (Bericht E)

**Harte Zahlen (BELEGT):** ctest **224/224** registriert/grün (Handover nannte 223; live nachgezählt +1: `integration_save_counter_pin`). Coverage: 11703 Zitate; EXE 290/386 (75,1 %), STAGE1 514/650 (79,1 %, obere Schranke §3.2), STAGE2-6 81,9-98,6 % (Spalten NICHT disjunkt → obere Schranken). Raum-Bestand 240 RDTs (STAGE1 80 = 40 Leon + 40 Elza). Alle 39 realen Leon-RDTs laden crashfrei; Türgraph 202 Räume/640 Türen; ~22 Leon-Räume tief bearbeitet (38 Dossiers) — **aber W2-Sweep selbst für die 6 Ketten-Räume 0/6** (ROADMAP_ROOMCHAIN.md:96-97), **Elza 0/40** (spielt mit Leon-Modell, 02ed15eb), **STAGE2-6 0/80 Räume** verifiziert (nur KI aller 50 Typen vorportiert).

| Track | Stand | Kern-Beleg |
|---|---|---|
| Engine-Tiefe | **~90 %** | 224 ctests, 11703 Zitate, 81 KI-Divergenzen gefixt; Reste: Positional-Audio, Gore, ESP 12-19, 5 Infra-Blocker |
| STAGE1-Breite | **~55-60 %** | Kritikpfad tief; ~22/40 Leon tief, W2 0/6, Elza 0/40 |
| STAGE2-6-Breite | **~15 %** | KI-Vorleistung ja, 0/80 Räume, kein Türgraph, keine Baselines |
| PSX-Hardware | **~40 %** | linkt + ISO bootfähig (targets/psx/README §4/5, überholt Memory „baut nicht"); RAM **−262 KB** (bss 1,77 MB), CD-Layout/SPU/Streaming offen |
| Tooling/Infra | **~70 %** | Harness + 2-Plattform-Release; fehlt: Parity-Oracle, Java-Gradle (build.gradle/settings.gradle weiter fehlend), Toolchain-Pinning |

**Subsystem-Kurzstatus:** fertig: Front-End (bis auf Attract/FE-3.2), FMV-Boot, Save/Load v5, Inventar/HUD/Map/File (pixel-verifiziert). FEHLT: Positional-Audio, Gore-Modelltausch, Aim-Pitch, Parity-Oracle; Movie_on 0x6F nur Operandenlänge (scd_vm.c:207).

**Restaufwand (Klassen):** 1. STAGE2-6-Raum-Parität **XL** (abh. 2/3); 2. Parity-Oracle **M-L** (höchster Hebel); 3. STAGE1 fertig (W2 + ~18 Leon + 40 Elza + E2E) **L**; 4. PSX-RAM/CD/SPU/Streaming **L** (unabhängig); 5. Gore/Modell-Cluster **L**; 6. Positional-Audio **M**; 7. Krähen-Wurf + Handover-Reste **M** (teils Nutzer-Input); 8. Movie_on + Attract + Gradle + Aim-Pitch **je S**. **Gesamt: ~45-55 % der Arbeit offen.**

**VERDACHT/NICHT GEPRÜFT:** Wie viele der „~22 tiefen Räume" dem vollen W2-Kriterium standhalten, ist ungemessen (vermutlich deutlich weniger); Phase-0-Items ohne Marker; PSX-Target-Stand nach 2026-08-05 sowie Elza-Ladbarkeit nicht geprüft.

---

## 6) Empfohlene nächste Schritte (max. 8)

1. **Beleg-Nachtrag + Marker-Hygiene (S):** Death-Glow (game_step_common.c:307ff), Krähen-DIVE-Block (:5108ff), Spawn-Pose-Tabelle (scd_vm.c:3168ff), Pan-Law/SCD-Volume (audio_pc.c) per Disasm belegen oder ehrlich als OFFEN markieren; PSX-Letterbox-24 auf den FUN_80021a0c-Zähler nachziehen; die 3 belegten Stale-Marker + stale „~17px"-Kommentare bereinigen.
2. **Die 5 belegten EXE-Lücken schließen (S-M):** SHITAI.TIM-Upload, SCD-RNG 0x8003ea3c + Opcode 0x28, Opcodes 0x45/0x5C — jeweils mit vorgeschaltetem game-weitem SCD-Nutzungs-Census (Wirkung ist dreimal NICHT GEPRÜFT), plus Aim-Reacquire 0x80037250.
3. **Werkzeug-Defekte fixen (S, hohe Folgewirkung):** STAGE1/-Decompile-Verschiebung +0x800 dokumentieren/regenerieren (nur STAGE1_full nutzen) und die RE2-Zitat-Mehrdeutigkeit in der Coverage bereinigen — sonst bleiben alle STAGE1-Prozente obere Schranken; FUN_80109ef8 per Raw-Disasm klären.
4. **NPC-per-Typ-States klären (M):** Sce_em_set-Deskriptor-Census für 0x42/45/49/4b + Savestate-`+0x4`-Probe; falls erreichbar → t4b/t42/t45/t49 portieren (verbindet sich mit NPC-VM Welle 2).
5. **Parity-Oracle bauen (M-L):** höchster Hebel laut Roadmap — ohne ihn bleibt jede der noch ausstehenden ~200 Raum-Verifikationen Handarbeit.
6. **Positional-Audio (M):** FUN_80045a64 / FUN_8005bec0-f08 — größter hörbarer Dauereffekt, blockiert nichts anderes.
7. **Krähen-Wurf-Exit RE'en (M):** einziges bekanntes Softlock-Risiko (ROOM10C0), Wurf fehlt derzeit komplett; zusammen mit den Handover-Punkten 10F0-Y-Band und Hund-Fress-Anker.
8. **W2-Sweep der 6 Ketten-Räume (M):** als Methodik-Pilot abschließen, bevor STAGE2-6-Breite (XL) startet — validiert zugleich, wie belastbar die „~22 tiefen Räume" wirklich sind.
