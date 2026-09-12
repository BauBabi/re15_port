# Liegende RE2-Zombies halb im Boden (ROOM1030, Marke 3 F1012) — Y-Kette komplett vermessen

Nutzer-Marker `r7_befund_1030_F1012_marke3.png` (v0.7.89, inkl. Runde-6-Klemmen-Fix 74dcf218):
zwei RE2-Zombies liegen halb in den Fliesen (einer kopfueber, Beine in der Luft; einer auf dem
Ruecken), ein dritter kriecht korrekt. Kontext: "Zombies fallen mit Shotgun-Schuss teilweise
unter den Boden." NUR statische Analyse; alle Zahlen unten sind SELBST aus den Bytes gedumpt
(Skripte: scratchpad `dump_em010.py` / `dump_multi.py`, FK-Nachbau = exakt die Port-Kette
`mat3_from_euler`/Q12, skeleton_common.c:135-155).

Quellen: `re15_port/shared_assets/RE2/CDEMD0.EMS` (EM010-EMD @0x2A800; TOC-Beleg
`re15_port/engine/src/gen/re2_ems_toc.inc`: kind 0x10 EMD = Sektor 0x55, Groesse 0x23DCC;
EM016 = Sektor 0x284 = @0x142000), `build/extracted/re2_ems/CDEMD0_EM10_ai1.BIN`
(Zombie-Overlay @0x80100000), `RE2_Quellcode_V2/FUN_80027160.c`, Port
`enemy_ai_re2_zombie.c` / `re2_ems.c` / `skeleton_common.c` / `platform/pc/main.c`.
Vorbefund: `analysis/befunde_runde6_2026-09-12/port-y-audit.md`.

---

## 1. Q1 — Die Liege-/Kriech-/Knockdown-Clips der EM010-Bank, selbst gedumpt

EMD @EMS+0x2A800: dir @+0x23DAC, D = {0x8, 0xC, 0x69C, **0x506C** (EDD Paar 2), **0x64A8**
(EMR-Pool Paar 2, Header {0x64,8,15,0x50}), 0x17300, 0x17B80, 0x1EE38 (MD1, 34 Objekte =
17 Meshes)}. Struktur-EMR D[2]=0x69C: 15 Bones, kf_size 0x50; EDD: 31 Clips, 1263 Frames.
rootY = s16 @Keyframe+2 des Pools @D[4]+8.

| Clip | Inhalt | Frames | rootY-Verlauf | Ende-Pose (RE2-Binds+MD1), Y-Spanne |
|---|---|---|---|---|
| 1 | Knockdown ruecklings | 60 (kf59-88) | -1976 -> Flug -1118 -> **-222** | -454..+146 |
| 2 | Knockdown vorwaerts | 60 (kf89-118) | -1976 -> Kopfstand -99 -> **-227** | -571..+98 |
| 5 | Kriechen (Loop) | 50 (kf211-260) | konstant **-175** | -1059..+284 |
| 6 | Kriech-HURT | 20 (kf261-280) | -175 -> -461 -> -175 | -972..+293 |
| 8 | Aufstehen (Ruecken) | 80 | -222 -> **-1957** | Ende steht: -3107..+85 |
| 9 | Aufstehen (Bauch) | 80 | -227 -> **-1974** | Ende steht: -3151..+13 |
| 12/13 | Steh-Stagger | 38/58 | -1929..-2054 / -1929..-1677 | steht durchweg |
| 22/23 | Leichen-Posen (CORPSE Sub 0) | je 10 | -227 / -222 | tiefster Vertex +99 / +146 |
| **3/4** | **RAGDOLL-Wurf (Schrot EBEN)** | 40/44 | **-1529..-1847 / -1593..-1917** | steht durchweg (-2986..+20) |

MD1-BBox Mesh 0 (Huefte/Rumpf, 41 Verts): x -267..241, **y -852..+132**, z -379..376.
Stehhoehe ~3200 Einheiten, Koerperdicke liegend ~500-750 Einheiten.

**Antwort Q1:** Bei rootY -175..-227 liegt das MESH AUF dem Boden (tiefster Vertex +98..+146,
Kriech-Arme bis +284/+319 — dieselbe autorisierte Boden-Durchdringung wie in RE2 Retail, denn
das ist die reine Asset-FK). Es braucht KEINEN zusaetzlichen Anker — SOLANGE Entity-Y (+0x3C)
auf dem Boden steht. "Halb im Boden" (>= 300 Einheiten) kann aus diesen Clips nicht kommen.
WICHTIG: die Schrot-EBEN-Ragdoll-Clips 3/4 sind KEINE Liege-Clips — ihr root bleibt auf
Stehhoehe; der sichtbare Sturz ist (a) die Lean-Kippung (Part-0-MulMatrix @0x80106A04-3C /
@0x80106CA0-D4; Port skeleton_common.c:633-648) plus (b) der **Entity-Y-Integrator**: e->y
sinkt bis zur Klemm-Schwelle gy+1354..+1647 UNTER den Boden (Schwelle `gy-200-rooty`
@0x80106EB4-C0 mit rooty -1554..-1847), waehrend gerendert `e->y + rooty` bei ~ -200..-300
bleibt. Die Marker-Pose "kopfueber, Beine raus" ist exakt Clip-2-kf104 (Pose -1298..+307)
mit versunkener Entity.

## 2. Q2 — Wie ankert das RE2-ORIGINAL liegende Zombies? VOLLZENSUS +0x3C

Eigener Scan ALLER sw/sh mit imm 0x3C ueber `CDEMD0_EM10_ai1.BIN` (20 Treffer): 13 sind
sp-Register-Saves; **@0x8010538C / @0x8010C048 / @0x8010C5A0 schreiben +0x3C eines
ABGERISSENEN-TEIL-Records** (Basis nach `addiu s0,s0,172`, word0 `ori 0x1062` @0x8010537C /
@0x8010C584 — Projektil-Felder, NICHT Entity-Y). Echte Entity-+0x3C-Schreiber im GANZEN
Zombie-Overlay sind exakt VIER Stellen:

1. `sw v0,60(s2)` @0x80106E88 — Ragdoll-Fall (`+0x3C += +0x15A`),
2. `sw v1,60(a1)` @0x80106F10 — Ragdoll-Bounce-Klemme (`gy232 - 300 - rooty`),
3. `sw v1,60(s2)` @0x80106AC8 — Ragdoll-P2-Gegenbuchung (`+0x3C = +0x232`),
4. der CORPSE-Servo @0x8010A890-8E0 — **WASSER-Schwimmer, kein Boden-Snap**: Gate
   `lh v0,268(s0); beq v0,zero -> Epilog` @0x8010A86C-74 (**+0x10C == 0 -> toter Block**;
   +0x10C = EXE-seitige Wasserhoehe, im Port ohne Produzent). Phase 1: `y -= 5` bis
   `y < +0x10C+300`, dann `y = +0x10C+300`, Zaehler +0x238 = 128 (@0x8010A890-B4); danach
   Endlos-Bob +-2/Frame ueber +0x238 128..191 (@0x8010A8B8-F0). Selbst disassembliert.

In den Liege-/Kriech-/Aufsteh-Executoren (EXEC[7] @0x80103780, EXEC[8] @0x80103B74,
Kriech-EXEC[0] @0x80103024, WARTEN @0x80103B48, EXEC[5]/EXEC[9]) gibt es **KEINEN**
+0x3C-Schreiber — auch nicht fuer grid&0x80-/f10e&1-Zombies. Der RENDERER traegt ebenfalls
keinen Fix: FUN_80027160 kopiert die Entity-Translation woertlich
(`MStack_38.t[1] = *(long*)(param_1+0x3c)`, RE2_Quellcode_V2/FUN_80027160.c:41).

**Antwort Q2:** Das Original ankert liegende Zombies GAR NICHT aktiv. Der Anker ist eine
INVARIANTE: +0x3C verlaesst den Boden nur innerhalb der Ragdoll, und P2 bucht auf **+0x232**
zurueck — und +0x232 wird in P0 aus **+0x1C2** kopiert, dem Boden-Y des EXE-Boden-Queries,
der +0x3C und +0x1C2 aus DEMSELBEN Query schreibt (`sw v0,60(s0)` / `sh v0,450(s0)`
@0x8003EE04-18) und beide auf Treppen gemeinsam nachzieht (@0x8003EAA0-AC). Der Original-
Rueckbucher kann daher NIE einen versunkenen Wert festschreiben, egal wann/wie P0 laeuft.

## 3. Q3 — Port-Vergleich: Hybrid vs. "pure RE2-Bank"

**Die Praemisse "der RE2-Modellpfad faehrt die pure Bank" ist ueberholt:** der Hybrid laeuft
im RE2-Flavor BEDINGUNGSLOS (platform/pc/main.c:848-866, "der reine RE2-Modell-Zweig ist
damit tot und weg"; Ausnahmen nur Ladefehler-Fallback und Baby-Spinne 0x26 :865). Der
root_y_fix wird fuer ALLE drei Skelette ueber den GANZEN Keyframe-Pool gebaut
(re2_ems.c:299-322, :363-365) und an beiden Tween-Seiten des Renderers
(skeleton_common.c:256/:264) UND seit 74dcf218 auch in der Klemmen-Ablesung
(enemy_ai_re2_zombie.c:5772) konsumiert.

Selbst nachgemessen (identischer K-Algorithmus wie `re2_lowest_bone_y`, RE1.5-Binds via
`k_perm_zombie` re2_ems.c:161, RE1.5-EM10/EM16 aus `shared_assets/PSX/EMD/CDEMD0.EMS`
Blob 0/4):

| Pose | rootY | K (Rootfix) | RE2-nativ tiefster Vertex | Hybrid+K tiefster Vertex |
|---|---|---|---|---|
| EM010 Stand kf0 | -1995 | +162 | +13 | -7 |
| EM010 Liege-Ende kf88 / kf118 | -222 / -227 | **+19 / +12** | +146 / +98 | +107 / +111 |
| EM010 Kriechen kf211 / kf236 | -175 | +17 / +21 | +231 / +179 | +230 / +216 |
| EM010 Leiche kf722 / kf732 | -227 / -222 | +12 / +19 | +99 / +146 | +111 / +107 |
| EM016 Liege-Ende | -222 / -227 | +19 / +12 | +146 / +98 | +99 / +89 |
| EM011/12 Liege-Ende (schlechtester Fall) | -222 | +19 | +146 | **+213** |

K ist fuer LIEGENDE Keyframes klein (+7..+36 — die Bindlaengen-Differenzen liegen dann
horizontal), die Hybrid-Bodenlage weicht maximal +67 von RE2-nativ ab — **eine
Groessenordnung unter "halb im Boden" (>= 300)**. Auch der pure Fallback-Pfad ist nativ
geerdet (RE2-Spalte). **Beide Modellpfade sind fuer dieses Symptom ENTLASTET; eine
Lade-Zeit-Tabelle, ein Renderer-Fix oder ein clip-seitiger y-Anker ist der FALSCHE Ort.**

## 4. Wo der Defekt allein noch wohnen kann — der fehlende +0x1C2-Zwilling

Ausschlussverfahren (alles oben byte-/messbelegt): die gerenderte Pose relativ zu e->y ist in
jedem Liege-Zustand geerdet (Leiche 22/23; WARTEN setzt Clip 0x17 @0x80103B5C-60 = Port
:3256; Kriechen Clip 5; Knockdown-Clips 1/2 tragen den root selbst und beruehren e->y nie —
Knockdown-Handler :6187-6246 ohne y-Schreiber). Also steht bei den versunkenen Liegern
**e->y selbst unter dem Boden** — und der einzige Mechanismus, der e->y je unter den Boden
bewegt, ist der Ragdoll-Integrator (planmaessig bis ~1650 Einheiten tief; Runde 6 mass
"e->y bis 1345"). Der Port-Rueckbucher haengt aber an einem Latch, das der Original-Semantik
NICHT entspricht:

**enemy_ai_re2_zombie.c:5869: `e->re2z_gy232 = (int16_t)e->y`** — der Port latcht die
AKTUELLE Entity-Y beim P0-Eintritt; das Original latcht den BODEN (+0x1C2, §2). Laeuft P0
irgendwann mit e->y != Boden (Re-Entry der Todeszellen RE2ZD_66FC/RE2ZD_7438 waehrend die
Entity noch abgesenkt ist, ein SCD-/Skript-State-Write mitten in der Ragdoll, jeder
kuenftige Pfad), schreiben P2 (:5890) und die Klemme (:5969/:5973) den versunkenen Wert als
"Boden" FEST — der Zombie liegt dauerhaft z.B. 500-900 Einheiten zu tief, mit exakt den
Marker-Posen (Clip-2-Kopfstand kf104: Pose -1298..+307 -> Beine ragen raus, Kopf im Boden).
Der 1d3-Filter (re15_re2z_hit_filter_apply :8121-8131, `hittable = spawn_pose || 1d3==0`)
blockt zwar den WAFFEN-Re-Stempel — er ist aber die EINZIGE Sicherung, und die Kette haengt
damit an einem Latch statt an der Original-Invariante.

**OFFEN (ehrlich benannt):** den konkreten P0-mit-versunkenem-y-Ausloeser habe ich statisch
NICHT reproduziert. Die Entscheidung liefert das befund.log des Nutzers: der Gegner-Block
der F9-Marke (platform/pc/main.c:5111-5127) druckt `pos=(x,y,z)` je Gegner; y der beiden
Lieger in Marke 3 (F1012) zwischen ~+300 und ~+1600 => Entity-Senke (dieser Patch),
y == Bodenwert => neue Fehlerklasse (dann zurueck an §1/§3-Messungen).

## 5. PATCH-PLAN (Datei:Zeile, nur belegte Zahlen)

1. **+0x1C2-Zwilling anlegen** — `re15_port/include/re15_actor.h` (bei `re2_lean`, :325):
   `int32_t re2z_ground_y;` Kommentar-Anker: der Original-Produzent schreibt +0x3C UND
   +0x1C2 aus demselben Boden-Query (`sw v0,60(s0)` / `sh v0,450(s0)` @0x8003EE04-18,
   Treppen-Nachzug @0x8003EAA0-AC).
2. **Pflegen wie das Original (per Tick, ausserhalb der Reaktion)** —
   `re15_port/engine/src/enemy_ai_re2_zombie.c` re15_re2z_tick (:7656ff, direkt nach
   `e->re2_lean_on = 0` :7671): `if (e->state == 0 || e->state == 1)
   e->re2z_ground_y = e->y;` plus Spawn-Init in re2z_init (:7435). In Zustand 0/1 IST +0x3C
   das Query-Ergebnis (Invariante §2); damit ist der Zwilling auf flachen Raeumen wertgleich
   zu +0x1C2 und folgt Etagenwechseln genauso.
3. **Latch umhaengen** — `enemy_ai_re2_zombie.c:5869`:
   `e->re2z_gy232 = (int16_t)e->re2z_ground_y;` (byte-true zu `+0x232 = +0x1C2`
   @0x80106994-A0). Klemme :5969/:5973 und P2 :5890 bleiben unveraendert — sie rechnen dann
   automatisch gegen den Boden statt gegen den letzten e->y-Zufallswert.
4. **Messschiene** — [z-ragdoll]-Trace (:5956) um eine P0-Zeile (`gy232-Latch, e->y,
   ground_y`) und eine P2-Zeile ergaenzen; Abnahme am Nutzer-Log: kein Lieger mehr mit
   `pos.y > Boden` in befund.log-Marken (Gegner-Block main.c:5111-5127).
5. **NICHT anfassen:** re2_ems.c-Rootfix (:299-322), skeleton_common.c (:256/:264),
   Renderer, Liege-Clips — alle gemessen sauber (§1/§3).

## FAZIT

1. Die EM010-Liege-Clips sind selbst-erdend: Knockdown 1/2 fahren den root -1976 -> -222/-227,
   Kriechen haelt -175, Leichen 22/23 liegen bei -227/-222; tiefster Vertex am Ende +98..+146.
2. Ein clip-seitiger Anker ist unnoetig — das MESH liegt auf dem Boden, sobald e->y der
   Boden ist (MD1-Mesh-0-BBox y -852..+132, Koerperdicke ~500-750).
3. Das RE2-Original hat KEINEN Boden-Snap fuer Lieger: Vollzensus aller +0x3C-Stores im
   Overlay = 3 Ragdoll-Stellen + Wasser-Bob (@0x8010A890-8E0, Gate +0x10C != 0); der
   Renderer (FUN_80027160.c:41) liest +0x3C woertlich.
4. Der Original-Anker ist die Invariante +0x3C == Boden plus die P2-Gegenbuchung aus +0x232,
   das aus dem BODEN-Register +0x1C2 gespeist wird (@0x8003EE04-18) — nie aus e->y.
5. Der Port latcht stattdessen e->y (enemy_ai_re2_zombie.c:5869) — der einzige noch offene,
   adressbelegte Bruch der Y-Kette nach dem Runde-6-Klemmen-Fix (74dcf218, :5772).
6. Der RE1.5-Flavor-Hybrid erdet die Liege-Clips korrekt: K(liegend) = +7..+36, Bodenlage
   maximal +67 neben RE2-nativ (gemessen EM010/011/012/016); der pure RE2-Pfad ist toter
   Fallback (main.c:848-866) und nativ geerdet — Renderer/Ladezeit-Tabelle sind der falsche
   Fix-Ort.
7. "Mit Shotgun unter den Boden fallen" erklaert sich NICHT ueber die Knockdown-Clips 1/2
   (die tragen den Fall im root und beruehren e->y nie), sondern ueber die Ragdoll-Familie
   (Clips 3/4, root bleibt auf Stehhoehe): dort sinkt die ENTITY planmaessig bis ~1650 unter
   den Boden, und alles haengt an der korrekten Gegenbuchung.
8. Patch: +0x1C2-Zwilling `re2z_ground_y` (Plan §5, drei Zeilen Kern) — danach ist die Kette
   strukturell so robust wie das Original; dazu P0/P2-Trace als Abnahme.
9. Naechste Messung ohne Code: befund.log der Nutzer-Marke F1012 lesen — der Gegner-Block
   druckt pos.y der beiden Lieger und entscheidet Entity-Senke vs. neue Fehlerklasse.
